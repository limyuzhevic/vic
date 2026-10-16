#include "CheckpointManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <zlib.h>  // For compression
#include <thread>
#include <future>

namespace nlm {

struct CheckpointManager::Impl {
    DistributedConfig config;
    CheckpointMetadata metadata;
    
    // Node information
    int nodeId;
    std::string checkpointDir;
    CheckpointMode mode;
    CompressionLevel compressionLevel;
    uint64_t maxCheckpoints;
    uint64_t checkpointInterval;
    
    // Checkpoint tracking
    std::unordered_map<int, CheckpointInfo> checkpoints;  // stepNumber -> CheckpointInfo
    std::vector<int> activeNodes;
    mutable std::mutex checkpointsMutex;
    
    // Communication
    std::unique_ptr<Communicator> communicator;
    
    // Statistics
    std::vector<float> checkpointTimes;
    std::atomic<uint64_t> totalCheckpointsCreated{0};
    std::atomic<uint64_t> totalCheckpointsLoaded{0};
    std::atomic<uint64_t> totalBytesTransferred{0};
    
    // Callbacks
    std::function<void(const CheckpointInfo&)> checkpointCallback;
    
    // Monitoring
    std::atomic<bool> healthCheckRunning{false};
    
    Impl(const DistributedConfig& cfg) 
        : config(cfg), nodeId(0), mode(CheckpointMode::Distributed),
          compressionLevel(CompressionLevel::Balanced), maxCheckpoints(100),
          checkpointInterval(100), totalCheckpointsCreated(0), totalCheckpointsLoaded(0),
          totalBytesTransferred(0) {
        // Set up checkpoint directory
        checkpointDir = cfg.checkpointDir.empty() ? "./checkpoints" : cfg.checkpointDir;
    }
};

CheckpointManager::CheckpointManager(const DistributedConfig& config) : pImpl(new Impl(config)) {}

CheckpointManager::~CheckpointManager() = default;

CheckpointManager::CheckpointManager(CheckpointManager&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

CheckpointManager& CheckpointManager::operator=(CheckpointManager&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool CheckpointManager::initialize() {
    try {
        NLM_LOG_INFO("Initializing CheckpointManager (Node " + std::to_string(pImpl->nodeId) + ")");
        
        // Create checkpoint directory
        std::error_code ec;
        if (!std::filesystem::exists(pImpl->checkpointDir, ec)) {
            if (!std::filesystem::create_directories(pImpl->checkpointDir, ec)) {
                NLM_LOG_ERROR("Failed to create checkpoint directory: " + pImpl->checkpointDir);
                return false;
            }
        }
        
        // Initialize communicator if in distributed mode
        if (pImpl->mode == CheckpointMode::Distributed || pImpl->mode == CheckpointMode::MasterSlave ||
            pImpl->mode == CheckpointMode::Mirrored) {
            pImpl->communicator = std::make_unique<Communicator>(pImpl->config);
            if (!pImpl->communicator->initialize()) {
                NLM_LOG_ERROR("Failed to initialize communicator for checkpointing");
                return false;
            }
        }
        
        // List existing checkpoints
        auto existingCheckpoints = listCheckpoints();
        if (!existingCheckpoints.empty()) {
            NLM_LOG_INFO("Found " + std::to_string(existingCheckpoints.size()) + 
                        " existing checkpoints");
        }
        
        NLM_LOG_INFO("CheckpointManager initialization complete");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception initializing CheckpointManager: ") + e.what());
        return false;
    }
}

bool CheckpointManager::createCheckpoint(int stepNumber, const std::string& customPath) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Determine checkpoint path
        std::string checkpointPath;
        if (!customPath.empty()) {
            checkpointPath = customPath;
        } else {
            checkpointPath = getNodeCheckpointPath(pImpl->nodeId, stepNumber);
        }
        
        NLM_LOG_INFO("Creating checkpoint at step " + std::to_string(stepNumber) + 
                    " (Node " + std::to_string(pImpl->nodeId) + ")");
        
        // Create local checkpoint
        if (!createLocalCheckpoint(stepNumber, checkpointPath)) {
            return false;
        }
        
        // Update checkpoint info
        CheckpointInfo info;
        info.nodeId = pImpl->nodeId;
        info.checkpointPath = checkpointPath;
        info.stepNumber = stepNumber;
        info.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        info.checkpointTime = std::chrono::duration<float>(endTime - startTime).count();
        
        // Calculate checkpoint size
        info.memoryUsage = calculateCheckpointSize(checkpointPath);
        info.valid = true;
        
        // Add to checkpoints map
        {
            std::lock_guard<std::mutex> lock(pImpl->checkpointsMutex);
            pImpl->checkpoints[stepNumber] = info;
        }
        
        // Update statistics
        pImpl->totalCheckpointsCreated++;
        
        // Add to checkpoint times
        pImpl->checkpointTimes.push_back(info.checkpointTime);
        if (pImpl->checkpointTimes.size() > 100) {
            pImpl->checkpointTimes.erase(pImpl->checkpointTimes.begin());
        }
        
        // Call checkpoint callback if set
        if (pImpl->checkpointCallback) {
            pImpl->checkpointCallback(info);
        }
        
        // Distribute checkpoint to other nodes if in distributed mode
        if (pImpl->mode == CheckpointMode::Distributed || pImpl->mode == CheckpointMode::Mirrored) {
            distributeCheckpoint(info);
        }
        
        NLM_LOG_INFO("Checkpoint created successfully at step " + std::to_string(stepNumber) + 
                    " (Node " + std::to_string(pImpl->nodeId) + ") in " +
                    std::to_string(info.checkpointTime * 1000.0f) + "ms");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception creating checkpoint: ") + e.what());
        return false;
    }
}

bool CheckpointManager::loadCheckpoint(int stepNumber, const std::string& customPath) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Determine checkpoint path
        std::string checkpointPath;
        if (!customPath.empty()) {
            checkpointPath = customPath;
        } else {
            checkpointPath = getNodeCheckpointPath(pImpl->nodeId, stepNumber);
        }
        
        NLM_LOG_INFO("Loading checkpoint at step " + std::to_string(stepNumber) + 
                    " (Node " + std::to_string(pImpl->nodeId) + ")");
        
        // Validate checkpoint
        if (!validateCheckpoint(checkpointPath)) {
            NLM_LOG_ERROR("Checkpoint validation failed");
            return false;
        }
        
        // Load local checkpoint
        if (!loadLocalCheckpoint(stepNumber, checkpointPath)) {
            NLM_LOG_ERROR("Failed to load local checkpoint");
            return false;
        }
        
        // Update statistics
        pImpl->totalCheckpointsLoaded++;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        float loadTime = std::chrono::duration<float>(endTime - startTime).count();
        
        NLM_LOG_INFO("Checkpoint loaded successfully at step " + std::to_string(stepNumber) + 
                    " (Node " + std::to_string(pImpl->nodeId) + ") in " +
                    std::to_string(loadTime * 1000.0f) + "ms");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading checkpoint: ") + e.what());
        return false;
    }
}

bool CheckpointManager::distributeCheckpoint(const CheckpointInfo& checkpoint) {
    if (!pImpl->communicator) {
        return false;
    }
    
    try {
        // Create checkpoint request
        CheckpointRequest request;
        request.requestingNodeId = pImpl->nodeId;
        request.targetNodeId = -1;  // Distribute to all nodes
        request.stepNumber = checkpoint.stepNumber;
        request.checkpointPath = checkpoint.checkpointPath;
        request.timestamp = checkpoint.timestamp;
        
        // Broadcast to all nodes
        if (!pImpl->communicator->broadcastCheckpoint(0, request)) {
            NLM_LOG_ERROR("Failed to broadcast checkpoint to all nodes");
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception distributing checkpoint: ") + e.what());
        return false;
    }
}

bool CheckpointManager::retrieveCheckpoint(int sourceNode, int stepNumber, std::string& checkpointPath) {
    if (!pImpl->communicator) {
        return false;
    }
    
    try {
        // Create checkpoint request
        CheckpointRequest request;
        request.requestingNodeId = pImpl->nodeId;
        request.targetNodeId = sourceNode;
        request.stepNumber = stepNumber;
        request.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        // Exchange checkpoint with source node
        CheckpointResponse response;
        if (!pImpl->communicator->exchangeCheckpoint(sourceNode, request, response)) {
            NLM_LOG_ERROR("Failed to exchange checkpoint with node " + std::to_string(sourceNode));
            return false;
        }
        
        if (!response.accepted) {
            NLM_LOG_ERROR("Node " + std::to_string(sourceNode) + " rejected checkpoint request: " +
                         response.errorMessage);
            return false;
        }
        
        checkpointPath = response.checkpointInfo.checkpointPath;
        
        // Retrieve actual checkpoint data
        if (!retrieveCheckpointData(sourceNode, stepNumber, checkpointPath)) {
            NLM_LOG_ERROR("Failed to retrieve checkpoint data from node " + std::to_string(sourceNode));
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception retrieving checkpoint: ") + e.what());
        return false;
    }
}

bool CheckpointManager::deleteCheckpoint(int stepNumber) {
    try {
        // Delete checkpoint from all nodes
        if (pImpl->mode == CheckpointMode::Distributed || pImpl->mode == CheckpointMode::Mirrored) {
            // Create request to delete checkpoint
            CheckpointRequest request;
            request.requestingNodeId = pImpl->nodeId;
            request.targetNodeId = -1;
            request.stepNumber = stepNumber;
            request.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            
            // Broadcast deletion request
            if (!pImpl->communicator->broadcastCheckpoint(0, request)) {
                NLM_LOG_ERROR("Failed to broadcast delete checkpoint request");
                return false;
            }
        }
        
        // Delete local checkpoint
        std::string checkpointPath = getNodeCheckpointPath(pImpl->nodeId, stepNumber);
        if (std::filesystem::exists(checkpointPath)) {
            std::error_code ec;
            if (!std::filesystem::remove(checkpointPath, ec)) {
                NLM_LOG_ERROR("Failed to delete checkpoint file: " + checkpointPath);
                return false;
            }
        }
        
        // Remove from checkpoints map
        {
            std::lock_guard<std::mutex> lock(pImpl->checkpointsMutex);
            pImpl->checkpoints.erase(stepNumber);
        }
        
        NLM_LOG_INFO("Checkpoint deleted at step " + std::to_string(stepNumber));
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception deleting checkpoint: ") + e.what());
        return false;
    }
}

std::vector<CheckpointInfo> CheckpointManager::listCheckpoints(int nodeId) const {
    std::vector<CheckpointInfo> allCheckpoints;
    
    try {
        if (nodeId >= 0) {
            // Only list checkpoints for specific node
            std::lock_guard<std::mutex> lock(pImpl->checkpointsMutex);
            for (const auto& pair : pImpl->checkpoints) {
                if (pair.second.nodeId == nodeId) {
                    allCheckpoints.push_back(pair.second);
                }
            }
        } else {
            // List all checkpoints from all nodes
            std::lock_guard<std::mutex> lock(pImpl->checkpointsMutex);
            allCheckpoints.reserve(pImpl->checkpoints.size());
            for (const auto& pair : pImpl->checkpoints) {
                allCheckpoints.push_back(pair.second);
            }
            
            // If in distributed mode, also query other nodes
            if (pImpl->mode == CheckpointMode::Distributed || pImpl->mode == CheckpointMode::Mirrored) {
                // This would involve MPI communication to query other nodes
                // For simplicity, we just return local checkpoints here
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception listing checkpoints: ") + e.what());
    }
    
    // Sort by step number
    std::sort(allCheckpoints.begin(), allCheckpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.stepNumber < b.stepNumber;
              });
    
    return allCheckpoints;
}

CheckpointInfo CheckpointManager::getLatestCheckpoint(int nodeId) const {
    std::vector<CheckpointInfo> checkpoints = listCheckpoints(nodeId);
    
    if (checkpoints.empty()) {
        return CheckpointInfo();
    }
    
    return checkpoints.back();
}

void CheckpointManager::configure(int nodeId, const std::string& checkpointDir,
                                 CheckpointMode mode, CompressionLevel compression,
                                 uint64_t maxCheckpoints, uint64_t checkpointInterval) {
    std::lock_guard<std::mutex> lock(pImpl->checkpointsMutex);
    
    pImpl->nodeId = nodeId;
    pImpl->checkpointDir = checkpointDir;
    pImpl->mode = mode;
    pImpl->compressionLevel = compression;
    pImpl->maxCheckpoints = maxCheckpoints;
    pImpl->checkpointInterval = checkpointInterval;
}

void CheckpointManager::setCompressionLevel(CompressionLevel level) {
    pImpl->compressionLevel = level;
}

void CheckpointManager::setMaxCheckpoints(uint64_t maxCheckpoints) {
    pImpl->maxCheckpoints = maxCheckpoints;
    // Cleanup old checkpoints
    cleanupOldCheckpoints();
}

void CheckpointManager::setCheckpointInterval(uint64_t interval) {
    pImpl->checkpointInterval = interval;
}

float CheckpointManager::getAverageCheckpointTime() const {
    if (pImpl->checkpointTimes.empty()) return 0.0f;
    
    float sum = std::accumulate(pImpl->checkpointTimes.begin(), pImpl->checkpointTimes.end(), 0.0f);
    return sum / static_cast<float>(pImpl->checkpointTimes.size());
}

uint64_t CheckpointManager::getTotalCheckpointsCreated() const {
    return pImpl->totalCheckpointsCreated;
}

uint64_t CheckpointManager::getTotalCheckpointsLoaded() const {
    return pImpl->totalCheckpointsLoaded;
}

uint64_t CheckpointManager::getTotalBytesTransferred() const {
    return pImpl->totalBytesTransferred;
}

std::string CheckpointManager::getCheckpointDirectory() const {
    return pImpl->checkpointDir;
}

bool CheckpointManager::cleanupOldCheckpoints() {
    try {
        std::lock_guard<std::mutex> lock(pImpl->checkpointsMutex);
        
        // Get all checkpoint paths
        std::vector<std::pair<int, std::string>> checkpointPaths;
        for (const auto& pair : pImpl->checkpoints) {
            checkpointPaths.emplace_back(pair.first, pair.second.checkpointPath);
        }
        
        // Sort by step number
        std::sort(checkpointPaths.begin(), checkpointPaths.end(),
                  [](const auto& a, const auto& b) { return a.first < b.first; });
        
        // Delete excess checkpoints
        if (checkpointPaths.size() > pImpl->maxCheckpoints) {
            size_t numToDelete = checkpointPaths.size() - pImpl->maxCheckpoints;
            
            NLM_LOG_INFO("Cleaning up " + std::to_string(numToDelete) + 
                        " old checkpoints (keeping " + std::to_string(pImpl->maxCheckpoints) + ")");
            
            for (size_t i = 0; i < numToDelete; ++i) {
                int stepNumber = checkpointPaths[i].first;
                std::string path = checkpointPaths[i].second;
                
                // Delete checkpoint file
                if (std::filesystem::exists(path)) {
                    std::error_code ec;
                    if (std::filesystem::remove(path, ec)) {
                        // Remove from checkpoints map
                        pImpl->checkpoints.erase(stepNumber);
                    } else {
                        NLM_LOG_ERROR("Failed to delete checkpoint: " + path);
                    }
                }
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception cleaning up checkpoints: ") + e.what());
        return false;
    }
}

bool CheckpointManager::validateCheckpoint(const std::string& checkpointPath) const {
    if (!std::filesystem::exists(checkpointPath)) {
        NLM_LOG_ERROR("Checkpoint file does not exist: " + checkpointPath);
        return false;
    }
    
    // Check file size
    uint64_t fileSize = calculateCheckpointSize(checkpointPath);
    if (fileSize == 0) {
        NLM_LOG_ERROR("Checkpoint file is empty: " + checkpointPath);
        return false;
    }
    
    // Simple validation - check if file can be read
    std::ifstream file(checkpointPath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Cannot open checkpoint file: " + checkpointPath);
        return false;
    }
    
    // Try to read metadata (simple validation)
    try {
        // For now, just check that file is readable and has some content
        char buffer[100];
        file.read(buffer, sizeof(buffer));
        if (file.gcount() == 0) {
            NLM_LOG_ERROR("Checkpoint file is empty or corrupted: " + checkpointPath);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception validating checkpoint: ") + e.what());
        return false;
    }
    
    return true;
}

bool CheckpointManager::synchronizeCheckpoints(const std::vector<int>& activeNodes) {
    if (!pImpl->communicator) {
        return false;
    }
    
    try {
        // Update active nodes list
        pImpl->activeNodes = activeNodes;
        
        // Send checkpoint synchronization request to all nodes
        CheckpointRequest request;
        request.requestingNodeId = pImpl->nodeId;
        request.targetNodeId = -1;  // Broadcast to all
        request.stepNumber = -1;    // All checkpoints
        request.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        if (!pImpl->communicator->broadcastCheckpoint(0, request)) {
            NLM_LOG_ERROR("Failed to broadcast checkpoint synchronization request");
            return false;
        }
        
        NLM_LOG_INFO("Checkpoint synchronization completed for " + 
                    std::to_string(activeNodes.size()) + " nodes");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception synchronizing checkpoints: ") + e.what());
        return false;
    }
}

bool CheckpointManager::recoverFromFailedNode(int failedNodeId, const std::vector<int>& backupNodes) {
    if (!pImpl->communicator) {
        return false;
    }
    
    try {
        // Retrieve checkpoints from backup nodes
        NLM_LOG_INFO("Recovering from failed node " + std::to_string(failedNodeId) + 
                    " using backup nodes");
        
        for (int backupNode : backupNodes) {
            // Get latest checkpoint from backup node
            std::string checkpointPath;
            if (retrieveCheckpoint(backupNode, -1, checkpointPath)) {  // -1 means latest
                NLM_LOG_INFO("Retrieved checkpoint from backup node " + 
                            std::to_string(backupNode) + ": " + checkpointPath);
                
                // Load checkpoint
                if (loadCheckpointFromFile(checkpointPath)) {
                    NLM_LOG_INFO("Successfully recovered from backup node " + 
                                std::to_string(backupNode));
                    return true;
                }
            }
        }
        
        NLM_LOG_ERROR("Failed to recover from failed node " + std::to_string(failedNodeId));
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception recovering from failed node: ") + e.what());
        return false;
    }
}

bool CheckpointManager::preemptiveCheckpoint(const std::string& reason) {
    try {
        // Create emergency checkpoint
        std::string customPath = pImpl->checkpointDir + "/emergency_" + 
                                std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                    std::chrono::high_resolution_clock::now().time_since_epoch()).count()) + 
                                "_" + reason + ".bin";
        
        if (createCheckpoint(-1, customPath)) {  // -1 means current step
            NLM_LOG_INFO("Pre-emptive checkpoint created due to: " + reason);
            return true;
        }
        
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception creating pre-emptive checkpoint: ") + e.what());
        return false;
    }
}

bool CheckpointManager::isCheckpointAvailable(int nodeId, int stepNumber) const {
    std::lock_guard<std::mutex> lock(pImpl->checkpointsMutex);
    
    // Check local checkpoints
    for (const auto& pair : pImpl->checkpoints) {
        if (pair.second.nodeId == nodeId && pair.second.stepNumber == stepNumber) {
            return true;
        }
    }
    
    // If querying self, return false
    if (nodeId == pImpl->nodeId || nodeId < 0) {
        return false;
    }
    
    // In distributed mode, would check other nodes via MPI
    // For now, assume not available
    return false;
}

CheckpointMetadata CheckpointManager::getCheckpointMetadata(const std::string& checkpointPath) const {
    CheckpointMetadata metadata;
    
    try {
        std::ifstream file(checkpointPath, std::ios::binary);
        if (!file) {
            return metadata;
        }
        
        // Read metadata (simplified - would need proper serialization)
        file.read(reinterpret_cast<char*>(&metadata), sizeof(CheckpointMetadata));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception reading checkpoint metadata: ") + e.what());
    }
    
    return metadata;
}

bool CheckpointManager::saveCheckpointMetadata(const CheckpointMetadata& metadata) {
    try {
        std::string metadataPath = pImpl->checkpointDir + "/metadata.bin";
        std::ofstream file(metadataPath, std::ios::binary);
        if (!file) {
            return false;
        }
        
        file.write(reinterpret_cast<const char*>(&metadata), sizeof(CheckpointMetadata));
        
        NLM_LOG_INFO("Checkpoint metadata saved");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving checkpoint metadata: ") + e.what());
        return false;
    }
}

CheckpointMetadata CheckpointManager::loadCheckpointMetadata(const std::string& checkpointPath) const {
    CheckpointMetadata metadata;
    
    try {
        std::ifstream file(checkpointPath, std::ios::binary);
        if (!file) {
            return metadata;
        }
        
        file.read(reinterpret_cast<char*>(&metadata), sizeof(CheckpointMetadata));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading checkpoint metadata: ") + e.what());
    }
    
    return metadata;
}

bool CheckpointManager::monitorHealth(std::vector<std::string>& healthIssues) const {
    healthIssues.clear();
    
    // Check if checkpoint directory is accessible
    std::error_code ec;
    if (!std::filesystem::exists(pImpl->checkpointDir, ec) && 
        !std::filesystem::create_directories(pImpl->checkpointDir, ec)) {
        healthIssues.push_back("Checkpoint directory is not accessible: " + pImpl->checkpointDir);
    }
    
    // Check number of checkpoints
    std::lock_guard<std::mutex> lock(pImpl->checkpointsMutex);
    if (pImpl->checkpoints.size() > pImpl->maxCheckpoints) {
        healthIssues.push_back("Too many checkpoints: " + std::to_string(pImpl->checkpoints.size()) +
                             " (limit: " + std::to_string(pImpl->maxCheckpoints) + ")");
    }
    
    // Check if communicator is initialized
    if (pImpl->mode == CheckpointMode::Distributed && !pImpl->communicator->isInitialized()) {
        healthIssues.push_back("Communicator not initialized for distributed checkpointing");
    }
    
    return healthIssues.empty();
}

void CheckpointManager::setCheckpointCallback(std::function<void(const CheckpointInfo&)> callback) {
    pImpl->checkpointCallback = callback;
}

bool CheckpointManager::saveState(const std::string& filepath) const {
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            return false;
        }
        
        // Save configuration
        file.write(reinterpret_cast<const char*>(&pImpl->nodeId), sizeof(int));
        file.write(reinterpret_cast<const char*>(&pImpl->mode), sizeof(CheckpointMode));
        file.write(reinterpret_cast<const char*>(&pImpl->compressionLevel), sizeof(CompressionLevel));
        file.write(reinterpret_cast<const char*>(&pImpl->maxCheckpoints), sizeof(uint64_t));
        file.write(reinterpret_cast<const char*>(&pImpl->checkpointInterval), sizeof(uint64_t));
        
        // Save checkpoints count
        size_t checkpointCount = pImpl->checkpoints.size();
        file.write(reinterpret_cast<const char*>(&checkpointCount), sizeof(size_t));
        
        // Save each checkpoint info
        for (const auto& pair : pImpl->checkpoints) {
            file.write(reinterpret_cast<const char*>(&pair.first), sizeof(int));
            file.write(reinterpret_cast<const char*>(&pair.second), sizeof(CheckpointInfo));
        }
        
        NLM_LOG_INFO("Checkpoint manager state saved");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving checkpoint manager state: ") + e.what());
        return false;
    }
}

bool CheckpointManager::loadState(const std::string& filepath) {
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            return false;
        }
        
        // Load configuration
        file.read(reinterpret_cast<char*>(&pImpl->nodeId), sizeof(int));
        file.read(reinterpret_cast<char*>(&pImpl->mode), sizeof(CheckpointMode));
        file.read(reinterpret_cast<char*>(&pImpl->compressionLevel), sizeof(CompressionLevel));
        file.read(reinterpret_cast<char*>(&pImpl->maxCheckpoints), sizeof(uint64_t));
        file.read(reinterpret_cast<char*>(&pImpl->checkpointInterval), sizeof(uint64_t));
        
        // Load checkpoints
        size_t checkpointCount;
        file.read(reinterpret_cast<char*>(&checkpointCount), sizeof(size_t));
        
        pImpl->checkpoints.clear();
        for (size_t i = 0; i < checkpointCount; ++i) {
            int stepNumber;
            CheckpointInfo info;
            file.read(reinterpret_cast<char*>(&stepNumber), sizeof(int));
            file.read(reinterpret_cast<char*>(&info), sizeof(CheckpointInfo));
            pImpl->checkpoints[stepNumber] = info;
        }
        
        NLM_LOG_INFO("Checkpoint manager state loaded");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading checkpoint manager state: ") + e.what());
        return false;
    }
}

// Private helper methods
bool CheckpointManager::createLocalCheckpoint(int stepNumber, const std::string& customPath) {
    // For now, create a simple text-based checkpoint file
    std::ofstream file(customPath, std::ios::binary);
    if (!file) {
        return false;
    }
    
    // Write basic checkpoint information
    CheckpointMetadata metadata;
    metadata.version = 1;
    metadata.creationTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    metadata.stepNumber = stepNumber;
    metadata.neuronCount = 1000;  // Placeholder
    metadata.synapseCount = 10000;  // Placeholder
    metadata.checkpointSize = 0;  // Will be updated after writing
    
    // Write metadata
    file.write(reinterpret_cast<const char*>(&metadata), sizeof(CheckpointMetadata));
    
    // Write some placeholder data
    std::string placeholderData = "NLM Checkpoint Data (Placeholder)";
    file.write(placeholderData.c_str(), placeholderData.size());
    
    // Get actual file size
    std::streampos currentPos = file.tellp();
    metadata.checkpointSize = static_cast<float>(currentPos);
    
    // Rewrite metadata with correct size
    file.seekp(0, std::ios::beg);
    file.write(reinterpret_cast<const char*>(&metadata), sizeof(CheckpointMetadata));
    file.seekp(0, std::ios::end);
    
    return true;
}

bool CheckpointManager::loadLocalCheckpoint(int stepNumber, const std::string& customPath) {
    std::ifstream file(customPath, std::ios::binary);
    if (!file) {
        return false;
    }
    
    // Read metadata
    CheckpointMetadata metadata;
    file.read(reinterpret_cast<char*>(&metadata), sizeof(CheckpointMetadata));
    
    // Check metadata
    if (metadata.version != 1) {
        NLM_LOG_ERROR("Unsupported checkpoint version: " + std::to_string(metadata.version));
        return false;
    }
    
    // Skip actual checkpoint data (in real implementation would load brain state)
    file.seekg(metadata.checkpointSize, std::ios::cur);
    
    return true;
}

std::string CheckpointManager::getNodeCheckpointPath(int nodeId, int stepNumber) const {
    std::stringstream ss;
    ss << pImpl->checkpointDir << "/step_" << stepNumber << "_node_" << nodeId << ".bin";
    return ss.str();
}

bool CheckpointManager::isPathAccessible(const std::string& path) const {
    std::error_code ec;
    return std::filesystem::exists(path, ec);
}

uint64_t CheckpointManager::calculateCheckpointSize(const std::string& checkpointPath) const {
    std::error_code ec;
    if (std::filesystem::exists(checkpointPath, ec)) {
        return std::filesystem::file_size(checkpointPath, ec);
    }
    return 0;
}

bool CheckpointManager::retrieveCheckpointData(int sourceNode, int stepNumber, std::string& checkpointPath) {
    // For now, just return the checkpoint path
    // In real implementation, would retrieve actual checkpoint data from source node
    return true;
}

bool CheckpointManager::loadCheckpointFromFile(const std::string& checkpointPath) {
    // Placeholder for loading checkpoint from file
    return true;
}

} // namespace nlm
