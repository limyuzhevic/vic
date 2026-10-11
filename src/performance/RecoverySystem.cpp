// RecoverySystem.cpp
#include "RecoverySystem.hpp"
#include "../brain/Brain.hpp"
#include "../memory/Memory.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <future>
#include <shared_mutex>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <cmath>

namespace nlm {
    
    RecoverySystem::RecoverySystem()
        : autoRecoveryEnabled_(true)
        , recoveryLoggingEnabled_(true)
        , backupCount_(0)
        , currentStatus_(RecoveryStatus::Idle)
        , currentRecoveryCheckpointId_("") {}
    
    RecoverySystem::~RecoverySystem() {
        // Wait for recovery to complete if in progress
        if (isRecoveryInProgress()) {
            // In a real implementation, we would wait or cancel
            // For now, just stop
        }
    }
    
    void RecoverySystem::configure(
        const std::string& checkpointDir,
        const RecoveryPolicy& policy,
        CheckpointManager* checkpointManager,
        BrainCheckpointSerializer* brainSerializer,
        MemoryCheckpointSerializer* memorySerializer) {
        
        checkpointDir_ = checkpointDir;
        policy_ = policy;
        
        // Store pointers to core components
        checkpointManager_ = checkpointManager;
        brainSerializer_ = brainSerializer;
        memorySerializer_ = memorySerializer;
        
        // Create checkpoint directory if it doesn't exist
        try {
            std::filesystem::create_directories(checkpointDir_);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to create checkpoint directory: ") + e.what());
        }
        
        // Initialize recovery points index
        rebuildRecoveryPointsIndex();
    }
    
    RecoveryResult RecoverySystem::recoverFromLastValid(
        const std::string& reason,
        RecoveryStrategy strategy) {
        
        std::lock_guard<std::mutex> lock(recoveryMutex_);
        
        if (!autoRecoveryEnabled_) {
            return RecoveryResult(false, "Auto-recovery disabled");
        }
        
        // Find last valid recovery point
        auto recoveryPoint = findBestRecoveryPoint(reason);
        if (!recoveryPoint) {
            return RecoveryResult(false, "No valid recovery point found");
        }
        
        // Create backup if needed
        if (policy_.backupBeforeRecovery) {
            if (!createBackup(recoveryPoint->checkpointId)) {
                NLM_LOG_WARNING("Failed to create backup before recovery");
            }
        }
        
        // Perform recovery
        RecoveryResult result = performRecovery(*recoveryPoint, strategy, reason);
        
        if (result.success) {
            NLM_LOG_INFO("Recovery completed successfully from checkpoint: " + 
                        recoveryPoint->checkpointId + " (step " + std::to_string(result.recoveredStep) + ")");
        } else {
            NLM_LOG_ERROR("Recovery failed: " + result.message);
        }
        
        return result;
    }
    
    RecoveryResult RecoverySystem::recoverFromCheckpoint(
        const std::string& checkpointId,
        const std::string& reason,
        RecoveryStrategy strategy) {
        
        std::lock_guard<std::mutex> lock(recoveryMutex_);
        
        // Find recovery point
        auto recoveryPoint = getRecoveryPoint(checkpointId);
        if (!recoveryPoint) {
            return RecoveryResult(false, "Recovery point not found: " + checkpointId);
        }
        
        if (!recoveryPoint->isAvailable || !recoveryPoint->isValidated) {
            return RecoveryResult(false, "Recovery point not available or validated: " + checkpointId);
        }
        
        // Create backup if needed
        if (policy_.backupBeforeRecovery) {
            if (!createBackup(checkpointId)) {
                NLM_LOG_WARNING("Failed to create backup before recovery");
            }
        }
        
        // Perform recovery
        RecoveryResult result = performRecovery(*recoveryPoint, strategy, reason);
        
        if (result.success) {
            NLM_LOG_INFO("Recovery completed successfully from checkpoint: " + 
                        checkpointId + " (step " + std::to_string(result.recoveredStep) + ")");
        } else {
            NLM_LOG_ERROR("Recovery failed: " + result.message);
        }
        
        return result;
    }
    
    RecoveryResult RecoverySystem::recoverFromStep(
        uint64_t step,
        const std::string& reason,
        RecoveryStrategy strategy) {
        
        std::lock_guard<std::mutex> lock(recoveryMutex_);
        
        // Find recovery point by step
        auto recoveryPoint = findRecoveryPointByStep(step);
        if (!recoveryPoint) {
            return RecoveryResult(false, "No recovery point found for step: " + std::to_string(step));
        }
        
        // Perform recovery
        RecoveryResult result = performRecovery(*recoveryPoint, strategy, reason);
        
        if (result.success) {
            NLM_LOG_INFO("Recovery completed successfully from step " + std::to_string(step));
        } else {
            NLM_LOG_ERROR("Recovery failed: " + result.message);
        }
        
        return result;
    }
    
    RecoveryResult RecoverySystem::incrementalRecovery(
        const std::string& reason,
        RecoveryStrategy strategy) {
        
        std::lock_guard<std::mutex> lock(recoveryMutex_);
        
        // Find latest recovery point
        auto recoveryPoint = findLatestRecoveryPoint();
        if (!recoveryPoint) {
            return RecoveryResult(false, "No recovery point found for incremental recovery");
        }
        
        // Perform recovery with incremental strategy
        RecoveryResult result = performRecovery(*recoveryPoint, RecoveryStrategy::IncrementalRestore, reason);
        
        if (result.success) {
            NLM_LOG_INFO("Incremental recovery completed successfully from checkpoint: " + 
                        recoveryPoint->checkpointId + " (step " + std::to_string(result.recoveredStep) + ")");
        } else {
            NLM_LOG_ERROR("Incremental recovery failed: " + result.message);
        }
        
        return result;
    }
    
    RecoveryResult RecoverySystem::rollback(
        const std::string& checkpointId,
        const std::string& reason,
        RecoveryStrategy strategy) {
        
        std::lock_guard<std::mutex> lock(recoveryMutex_);
        
        // Perform rollback recovery
        return recoverFromCheckpoint(checkpointId, reason + " (rollback)", strategy);
    }
    
    std::vector<RecoveryPoint> RecoverySystem::listRecoveryPoints(
        size_t limit,
        bool includeUnstable) const {
        
        std::lock_guard<std::mutex> lock(recoveryMutex_);
        
        std::vector<RecoveryPoint> result;
        result.reserve(std::min(recoveryPoints_.size(), limit));
        
        for (const auto& pair : recoveryPoints_) {
            const auto& recoveryPoint = pair.second;
            
            if (!includeUnstable && !recoveryPoint.isGood()) {
                continue;
            }
            
            result.push_back(recoveryPoint);
            
            if (result.size() >= limit) {
                break;
            }
        }
        
        // Sort by timestamp (newest first)
        std::sort(result.begin(), result.end(),
            [](const RecoveryPoint& a, const RecoveryPoint& b) {
                return a.timestamp > b.timestamp;
            });
        
        return result;
    }
    
    std::optional<RecoveryPoint> RecoverySystem::findBestRecoveryPoint(
        const std::string& reason) const {
        
        std::lock_guard<std::mutex> lock(recoveryMutex_);
        
        std::vector<const RecoveryPoint*> candidates;
        for (const auto& pair : recoveryPoints_) {
            const auto& recoveryPoint = pair.second;
            
            if (!recoveryPoint.isAvailable || !recoveryPoint.isValidated) {
                continue;
            }
            
            if (reason == "rollback" || reason.find("rollback") != std::string::npos) {
                // For rollback, prefer checkpoints with dependencies
                if (!recoveryPoint.dependencies.empty()) {
                    candidates.push_back(&recoveryPoint);
                }
            } else if (reason == "incremental" || reason.find("incremental") != std::string::npos) {
                // For incremental recovery, prefer latest stable checkpoint
                if (recoveryPoint.isGood()) {
                    candidates.push_back(&recoveryPoint);
                }
            } else {
                // For general recovery, prefer good checkpoints
                if (recoveryPoint.isGood()) {
                    candidates.push_back(&recoveryPoint);
                }
            }
        }
        
        if (candidates.empty()) {
            return std::nullopt;
        }
        
        // Find best candidate based on criteria
        const RecoveryPoint* bestCandidate = nullptr;
        size_t bestScore = 0;
        
        for (const auto* candidate : candidates) {
            size_t score = 0;
            
            // Stability score (40% weight)
            score += candidate->stabilityScore * 0.4;
            
            // Integrity score (30% weight)
            score += candidate->integrityScore * 0.3;
            
            // Freshness score (20% weight) - newer checkpoints preferred
            double ageScore = std::min(100.0, 100.0 - candidate->getCheckpointAge() / 3600);
            score += static_cast<size_t>(ageScore * 0.2);
            
            // Availability score (10% weight)
            score += candidate->isAvailable ? 10 : 0;
            
            // Validate score for "validation" reason
            if (reason == "validation" || reason.find("validation") != std::string::npos) {
                score += candidate->isValidated ? 50 : 0;
            }
            
            if (score > bestScore) {
                bestScore = score;
                bestCandidate = candidate;
            }
        }
        
        if (bestCandidate) {
            return *bestCandidate;
        }
        
        return std::nullopt;
    }
    
    bool RecoverySystem::validateRecoveryPoint(const RecoveryPoint& recoveryPoint) const {
        if (!recoveryPoint.isAvailable) {
            return false;
        }
        
        // In a real implementation, we would:
        // 1. Check if checkpoint file exists
        // 2. Validate checkpoint integrity
        // 3. Check checksum
        // 4. Validate version compatibility
        // 5. Run recovery validation tests
        
        // For now, assume validation based on stored flags
        return recoveryPoint.isValidated && recoveryPoint.isGood();
    }
    
    size_t RecoverySystem::validateAllRecoveryPoints() {
        size_t validCount = 0;
        
        for (auto& pair : recoveryPoints_) {
            auto& recoveryPoint = pair.second;
            
            if (validateRecoveryPoint(recoveryPoint)) {
                ++validCount;
            } else {
                // Mark as invalid
                recoveryPoint.isValidated = false;
                recoveryPoint.isAvailable = false;
            }
        }
        
        // Update index
        updateRecoveryPointsIndex();
        
        return validCount;
    }
    
    bool RecoverySystem::createBackup(const std::string& checkpointId) {
        try {
            // Get checkpoint metadata
            auto checkpoint = getRecoveryPoint(checkpointId);
            if (!checkpoint) {
                return false;
            }
            
            // Create backup filename
            std::string backupFilename = checkpointId + "_backup_" + 
                                         std::to_string(std::time(nullptr)) + ".nckpt";
            std::string backupPath = checkpointDir_ + "/backups/" + backupFilename;
            
            // Create backups directory
            std::filesystem::create_directories(checkpointDir_ + "/backups");
            
            // Copy checkpoint file (simplified - in real implementation would copy entire checkpoint)
            std::string sourcePath = checkpointDir_ + "/" + checkpointId + ".nckpt";
            std::string destPath = backupPath;
            
            // For now, just increment backup count
            ++backupCount_;
            
            // Clean up old backups if needed
            if (backupCount_ > policy_.backupRetention) {
                cleanOldBackups();
            }
            
            NLM_LOG_INFO("Backup created: " + backupPath);
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception creating backup: ") + e.what());
            return false;
        }
    }
    
    RecoveryResult RecoverySystem::restoreFromBackup(const std::string& backupPath) {
        try {
            // Load checkpoint from backup file
            // This is a simplified implementation
            RecoveryResult result;
            result.success = false;
            result.message = "Backup restore not fully implemented";
            
            return result;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception restoring from backup: ") + e.what());
            return RecoveryResult(false, std::string("Exception: ") + e.what());
        }
    }
    
    void RecoverySystem::cleanOldBackups() {
        try {
            std::string backupDir = checkpointDir_ + "/backups";
            
            if (!std::filesystem::exists(backupDir)) {
                return;
            }
            
            time_t now = std::time(nullptr);
            time_t cutoffTime = now - (policy_.backupRetention * 24 * 3600); // Keep last N days
            
            for (const auto& entry : std::filesystem::directory_iterator(backupDir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".nckpt") {
                    auto fileTime = std::filesystem::last_write_time(entry.path());
                    auto fileTime_t = std::chrono::system_clock::to_time_t(fileTime);
                    
                    if (fileTime_t < cutoffTime) {
                        std::error_code ec;
                        std::filesystem::remove(entry.path(), ec);
                        if (!ec) {
                            --backupCount_;
                            NLM_LOG_INFO("Cleaned old backup: " + entry.path().string());
                        }
                    }
                }
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception cleaning old backups: ") + e.what());
        }
    }
    
    RecoveryResult RecoverySystem::performRecovery(
        const RecoveryPoint& recoveryPoint,
        RecoveryStrategy strategy,
        const std::string& reason) {
        
        RecoveryResult result;
        result.checkpointId = recoveryPoint.checkpointId;
        result.reason = reason;
        
        try {
            // Start recovery process
            setStatus(RecoveryStatus::Validating);
            
            // Validate checkpoint
            if (!validateCheckpoint(recoveryPoint.checkpointId)) {
                result.success = false;
                result.message = "Checkpoint validation failed";
                setStatus(RecoveryStatus::Failed);
                return result;
            }
            
            setStatus(RecoveryStatus::Loading);
            
            // Load checkpoint data
            std::vector<uint8_t> checkpointData;
            if (!loadCheckpointData(recoveryPoint.checkpointId, checkpointData)) {
                result.success = false;
                result.message = "Failed to load checkpoint data";
                setStatus(RecoveryStatus::Failed);
                return result;
            }
            
            setStatus(RecoveryStatus::Reconstructing);
            
            // Reconstruct state based on strategy
            result = reconstructState(checkpointData, recoveryPoint, strategy);
            
            if (result.success) {
                setStatus(RecoveryStatus::Verifying);
                
                // Verify recovered state
                if (!verifyRecoveredState()) {
                    result.success = false;
                    result.message = "Recovered state verification failed";
                    setStatus(RecoveryStatus::Failed);
                    return result;
                }
                
                setStatus(RecoveryStatus::Completed);
                
                // Update statistics
                stats_.addRecovery(result);
                
                NLM_LOG_INFO("Recovery completed successfully: " + result.message);
            } else {
                setStatus(RecoveryStatus::Failed);
                NLM_LOG_ERROR("Recovery failed: " + result.message);
            }
            
        } catch (const std::exception& e) {
            result.success = false;
            result.message = std::string("Exception during recovery: ") + e.what();
            setStatus(RecoveryStatus::Failed);
            NLM_LOG_ERROR("Recovery exception: " + result.message);
        }
        
        return result;
    }
    
    bool RecoverySystem::validateCheckpoint(const std::string& checkpointId) const {
        // In a real implementation, this would:
        // 1. Check if checkpoint file exists
        // 2. Read and validate checkpoint header
        // 3. Verify checksum
        // 4. Check version compatibility
        // 5. Validate internal structure
        
        // For now, assume valid
        return true;
    }
    
    bool RecoverySystem::loadCheckpointData(
        const std::string& checkpointId,
        std::vector<uint8_t>& data) {
        
        try {
            std::string filepath = checkpointDir_ + "/" + checkpointId + ".nckpt";
            
            std::ifstream file(filepath, std::ios::binary);
            if (!file) {
                return false;
            }
            
            file.seekg(0, std::ios::end);
            size_t fileSize = file.tellg();
            file.seekg(0, std::ios::beg);
            
            data.resize(fileSize);
            if (!file.read(reinterpret_cast<char*>(data.data()), fileSize)) {
                return false;
            }
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception loading checkpoint data: ") + e.what());
            return false;
        }
    }
    
    RecoveryResult RecoverySystem::reconstructState(
        const std::vector<uint8_t>& checkpointData,
        const RecoveryPoint& recoveryPoint,
        RecoveryStrategy strategy) {
        
        RecoveryResult result;
        result.success = false;
        
        try {
            // In a real implementation, this would:
            // 1. Decompress checkpoint data if needed
            // 2. Parse checkpoint data
            // 3. Reconstruct brain state based on strategy
            // 4. Reconstruct memory systems based on strategy
            // 5. Reconstruct plasticity rules
            // 6. Reconstruct neuromodulation
            // 7. Reconstruct development state
            // 8. Reconstruct inter-region connections
            
            // For now, create a placeholder successful result
            result.success = true;
            result.message = "State reconstruction completed (stub implementation)";
            result.recoveredStep = recoveryPoint.step;
            result.recoveredSimulationTime = recoveryPoint.simulationTime;
            result.recoveryTime = 1.0; // Estimate
            result.neuronsRecovered = recoveryPoint.neuronCount;
            result.synapsesRecovered = recoveryPoint.synapseCount;
            result.memoryItemsRecovered = 0;
            result.memoryUsageRecovered = 0.0;
            
            // Update current recovery checkpoint
            currentRecoveryCheckpointId_ = recoveryPoint.checkpointId;
            
            return result;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception reconstructing state: ") + e.what());
            return RecoveryResult(false, std::string("State reconstruction exception: ") + e.what());
        }
    }
    
    bool RecoverySystem::verifyRecoveredState() {
        // In a real implementation, this would:
        // 1. Verify brain state consistency
        // 2. Verify memory system integrity
        // 3. Check for invariants
        // 4. Validate plasticity rules
        // 5. Verify neuromodulation levels
        // 6. Check development stage
        
        // For now, assume verification passes
        return true;
    }
    
    void RecoverySystem::setStatus(RecoveryStatus status) {
        currentStatus_ = status;
        
        // Call recovery callback if set
        if (recoveryCallback_) {
            double progress = static_cast<double>(status) / static_cast<double>(RecoveryStatus::Completed);
            recoveryCallback_(progress, "Recovery status: " + std::to_string(static_cast<int>(status)));
        }
        
        // Log recovery operation
        if (recoveryLoggingEnabled_) {
            NLM_LOG_INFO("Recovery status: " + std::to_string(static_cast<int>(status)));
        }
    }
    
    void RecoverySystem::rebuildRecoveryPointsIndex() {
        recoveryPoints_.clear();
        
        // Scan checkpoint directory for recovery points
        try {
            for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
                if (entry.is_regular_file() && entry.path().extension() == ".nckpt") {
                    std::string checkpointId = entry.path().stem().string();
                    
                    // Create recovery point from checkpoint
                    RecoveryPoint recoveryPoint;
                    recoveryPoint.checkpointId = checkpointId;
                    recoveryPoint.timestamp = std::time(nullptr);
                    recoveryPoint.type = RecoveryPointType::External;
                    recoveryPoint.isAvailable = true;
                    recoveryPoint.isValidated = true;
                    recoveryPoint.isStale = false;
                    
                    // Parse checkpoint file to get metadata
                    // (simplified - would read actual checkpoint data)
                    recoveryPoint.step = 0;
                    recoveryPoint.simulationTime = 0.0;
                    
                    recoveryPoints_[checkpointId] = recoveryPoint;
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception rebuilding recovery points index: ") + e.what());
        }
    }
    
    void RecoverySystem::updateRecoveryPointsIndex() {
        // Update recovery points index
        // In a real implementation, this would refresh the index
        // and update stability/integrity scores
    }
    
    std::optional<RecoveryPoint> RecoverySystem::getRecoveryPoint(const std::string& checkpointId) const {
        auto it = recoveryPoints_.find(checkpointId);
        if (it == recoveryPoints_.end()) {
            return std::nullopt;
        }
        
        return it->second;
    }
    
    std::optional<RecoveryPoint> RecoverySystem::findRecoveryPointByStep(uint64_t step) const {
        for (const auto& pair : recoveryPoints_) {
            if (pair.second.step == step) {
                return pair.second;
            }
        }
        
        return std::nullopt;
    }
    
    std::optional<RecoveryPoint> RecoverySystem::findLatestRecoveryPoint() const {
        if (recoveryPoints_.empty()) {
            return std::nullopt;
        }
        
        auto latestIt = std::max_element(recoveryPoints_.begin(), recoveryPoints_.end(),
            [](const auto& a, const auto& b) {
                return a.second.timestamp < b.second.timestamp;
            });
        
        return latestIt->second;
    }