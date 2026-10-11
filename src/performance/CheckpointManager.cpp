// CheckpointManager.cpp
#include "CheckpointManager.hpp"
#include "../brain/Brain.hpp"
#include "../memory/Memory.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <future>

namespace nlm {
    
    CheckpointManager::CheckpointManager()
        : compress_(true)
        , compressionAlgorithm_(CompressionAlgorithm::Zstd)
        , compressionLevel_(3)
        , parallelEnabled_(true)
        , maxConcurrent_(2)
        , incrementalEnabled_(true)
        , differentialEnabled_(true)
        , autoCleanup_(true)
        , lastIncrementalBaseStep_(0)
        , shutdownRequested_(false)
        , checkpointsCreated_(0)
        , checkpointsLoaded_(0)
        , checkpointsDeleted_(0)
        , totalBytesSaved_(0)
        , totalBytesLoaded_(0) {
        
        // Initialize cleanup thread
        cleanupThread_ = std::thread(&CheckpointManager::cleanupThreadFunc, this);
    }
    
    CheckpointManager::~CheckpointManager() {
        // Request shutdown
        shutdownRequested_ = true;
        cleanupCondition_.notify_all();
        
        // Wait for cleanup thread to finish
        if (cleanupThread_.joinable()) {
            cleanupThread_.join();
        }
        
        // Clean up pending operations
        for (auto& future : pendingOperations_) {
            future.wait();
        }
    }
    
    void CheckpointManager::configure(
        const std::string& checkpointDir,
        const CheckpointPolicy& policy,
        bool compress,
        size_t maxConcurrent) {
        
        checkpointDir_ = checkpointDir;
        policy_ = policy;
        compress_ = compress;
        maxConcurrent_ = maxConcurrent;
        
        // Create checkpoint directory if it doesn't exist
        if (!ensureDirectoryExists(checkpointDir_)) {
            throw std::runtime_error("Failed to create checkpoint directory: " + checkpointDir_);
        }
        
        // Rebuild index from existing checkpoints
        rebuildIndex();
    }
    
    void CheckpointManager::setDataProviders(
        std::function<bool(const NeuronCheckpointData&)> neuronProvider,
        std::function<bool(const SynapseCheckpointData&)> synapseProvider,
        std::function<bool(const std::vector<uint8_t>&)> plasticityProvider,
        std::function<bool(const std::vector<uint8_t>&)> memoryProvider,
        std::function<bool(const std::vector<uint8_t>&)> neuromodulationProvider) {
        
        std::lock_guard<std::mutex> lock(operationsMutex_);
        neuronProvider_ = std::move(neuronProvider);
        synapseProvider_ = std::move(synapseProvider);
        plasticityProvider_ = std::move(plasticityProvider);
        memoryProvider_ = std::move(memoryProvider);
        neuromodulationProvider_ = std::move(neuromodulationProvider);
    }
    
    void CheckpointManager::setSerializers(
        std::function<std::vector<uint8_t>(const Brain&)> brainSerializer,
        std::function<std::vector<uint8_t>(const Memory&)> memorySerializer) {
        
        std::lock_guard<std::mutex> lock(operationsMutex_);
        brainSerializer_ = std::move(brainSerializer);
        memorySerializer_ = std::move(memorySerializer);
    }
    
    bool CheckpointManager::update(uint64_t currentStep, double currentTime,
                                  uint64_t currentSize, const std::string& event) {
        std::lock_guard<std::mutex> lock(operationsMutex_);
        
        currentSize_ = currentSize;
        
        // Check if we should create a checkpoint based on policy
        if (policy_.shouldCreateCheckpoint(currentStep, currentTime, currentSize, event)) {
            return createCheckpoint();
        }
        
        return false;
    }
    
    std::string CheckpointManager::saveImmediately(
        const std::string& name,
        CompressionAlgorithm compressionAlgorithm,
        int compressionLevel) {
        
        std::lock_guard<std::mutex> lock(operationsMutex_);
        
        // If there are pending operations, queue this one
        if (activeOperations_ >= maxConcurrent_) {
            operationQueue_.push_back(name);
            return ""; // Queued
        }
        
        ++activeOperations_;
        
        // Create checkpoint in background thread if parallel enabled
        if (parallelEnabled_) {
            auto future = std::async(std::launch::async, [this, name, compressionAlgorithm, compressionLevel]() {
                bool success = createCheckpoint(name);
                --activeOperations_;
                if (!operationQueue_.empty()) {
                    std::string nextName = operationQueue_.front();
                    operationQueue_.pop_front();
                    saveImmediately(nextName, compressionAlgorithm, compressionLevel);
                }
                return success;
            });
            
            pendingOperations_.push_back(std::move(future));
            return "queued"; // Indicates operation is queued
        } else {
            // Run synchronously
            bool success = createCheckpoint(name);
            --activeOperations_;
            if (success) {
                return currentCheckpointId_;
            } else {
                return "";
            }
        }
    }
    
    bool CheckpointManager::load(const std::string& checkpointId) {
        std::lock_guard<std::mutex> lock(operationsMutex_);
        
        auto metadataIt = checkpoints_.find(checkpointId);
        if (metadataIt == checkpoints_.end()) {
            return false;
        }
        
        const auto& metadata = metadataIt->second;
        
        try {
            std::vector<uint8_t> data;
            if (storageBackend_) {
                // Load from remote storage
                if (!storageBackend_->load(checkpointId, data)) {
                    return false;
                }
            } else {
                // Load from local storage
                std::string filepath = getCheckpointPath(checkpointId);
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
            }
            
            // Decompress if needed
            if (metadata.compressed && metadata.compressionAlgorithm > 0) {
                // Decompress data (simplified for now)
                // In real implementation, use CompressionEngine
            }
            
            // Validate checkpoint integrity
            if (!validateCheckpointData(data)) {
                NLM_LOG_ERROR("Checkpoint validation failed: " + checkpointId);
                return false;
            }
            
            // Load data into brain (this would be implemented in the Brain class)
            // For now, just track the load
            ++checkpointsLoaded_;
            totalBytesLoaded_ += data.size();
            
            currentCheckpointId_ = checkpointId;
            lastSaveStep_ = metadata.step;
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception loading checkpoint: ") + e.what());
            return false;
        }
    }
    
    bool CheckpointManager::loadLatest() {
        std::lock_guard<std::mutex> lock(operationsMutex_);
        
        auto checkpoints = listCheckpoints(1, true);
        if (checkpoints.empty()) {
            return false;
        }
        
        return load(checkpoints[0].getId());
    }
    
    bool CheckpointManager::loadByStep(uint64_t step) {
        std::lock_guard<std::mutex> lock(operationsMutex_);
        
        auto checkpoint = getCheckpointByStep(step);
        if (!checkpoint) {
            return false;
        }
        
        return load(checkpoint->getId());
    }
    
    std::vector<CheckpointMetadata> CheckpointManager::listCheckpoints(
        size_t limit,
        bool includeInvalid) const {
        
        std::lock_guard<std::mutex> lock(operationsMutex_);
        
        std::vector<CheckpointMetadata> result;
        result.reserve(std::min(checkpoints_.size(), limit));
        
        // Copy all checkpoints
        for (const auto& pair : checkpoints_) {
            const auto& metadata = pair.second;
            
            if (!includeInvalid && !metadata.isValid) {
                continue;
            }
            
            result.push_back(metadata);
            
            if (result.size() >= limit) {
                break;
            }
        }
        
        // Sort by timestamp (newest first)
        std::sort(result.begin(), result.end(),
            [](const CheckpointMetadata& a, const CheckpointMetadata& b) {
                return a.timestamp > b.timestamp;
            });
        
        return result;
    }
    
    std::optional<CheckpointMetadata> CheckpointManager::getCheckpoint(
        const std::string& checkpointId) const {
        
        std::lock_guard<std::mutex> lock(operationsMutex_);
        
        auto it = checkpoints_.find(checkpointId);
        if (it == checkpoints_.end()) {
            return std::nullopt;
        }
        
        return it->second;
    }
    
    std::optional<CheckpointMetadata> CheckpointManager::getCheckpointByStep(
        uint64_t step) const {
        
        std::lock_guard<std::mutex> lock(operationsMutex_);
        
        auto it = stepIndex_.find(step);
        if (it == stepIndex_.end()) {
            return std::nullopt;
        }
        
        auto checkpointIt = checkpoints_.find(it->second);
        if (checkpointIt == checkpoints_.end()) {
            return std::nullopt;
        }
        
        return checkpointIt->second;
    }
    
    bool CheckpointManager::deleteCheckpoint(const std::string& checkpointId) {
        std::lock_guard<std::mutex> lock(operationsMutex_);
        
        auto it = checkpoints_.find(checkpointId);
        if (it == checkpoints_.end()) {
            return false;
        }
        
        const auto& metadata = it->second;
        
        // Delete from storage
        if (storageBackend_) {
            storageBackend_->deleteCheckpoint(checkpointId);
        } else {
            std::string filepath = getCheckpointPath(checkpointId);
            std::error_code ec;
            std::filesystem::remove(filepath, ec);
        }
        
        // Remove from index
        checkpoints_.erase(it);
        
        // Remove from step index
        stepIndex_.erase(metadata.step);
        
        // Update indexer
        for (auto& indexerPair : indexers_) {
            indexerPair.second->removeCheckpoint(checkpointId);
        }
        
        ++checkpointsDeleted_;
        
        return true;
    }
    
    size_t CheckpointManager::deleteCheckpoints(
        const std::vector<std::string>& checkpointIds) {
        
        size_t deleted = 0;
        for (const auto& checkpointId : checkpointIds) {
            if (deleteCheckpoint(checkpointId)) {
                ++deleted;
            }
        }
        
        return deleted;
    }
    
    void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
        std::lock_guard<std::mutex> lock(operationsMutex_);
        
        auto checkpoints = listCheckpoints();
        
        // Keep the most recent N checkpoints
        if (checkpoints.size() <= keepCount) {
            return;
        }
        
        // Sort by timestamp (newest first) and delete oldest ones
        std::sort(checkpoints.begin(), checkpoints.end(),
            [](const CheckpointMetadata& a, const CheckpointMetadata& b) {
                return a.timestamp > b.timestamp;
            });
        
        size_t toDelete = checkpoints.size() - keepCount;
        for (size_t i = keepCount; i < checkpoints.size(); ++i) {
            deleteCheckpoint(checkpoints[i].getId());
        }
    }
    
    bool CheckpointManager::validateCheckpoint(const std::string& checkpointId) {
        std::lock_guard<std::mutex> lock(operationsMutex_);
        
        auto it = checkpoints_.find(checkpointId);
        if (it == checkpoints_.end()) {
            return false;
        }
        
        bool isValid = validateCheckpointIntegrity(checkpointId);
        
        if (isValid != it->second.isValid) {
            it->second.isValid = isValid;
            ++it->second.validationCount;
            
            if (!isValid) {
                NLM_LOG_WARNING("Checkpoint marked as invalid: " + checkpointId);
            }
        }
        
        return isValid;
    }
    
    size_t CheckpointManager::validateAllCheckpoints() {
        size_t validCount = 0;
        
        for (const auto& pair : checkpoints_) {
            if (validateCheckpoint(pair.first)) {
                ++validCount;
            }
        }
        
        return validCount;
    }
    
    std::string CheckpointManager::getLastCheckpointPath() const {
        std::lock_guard<std::mutex> lock(operationsMutex_);
        
        if (checkpoints_.empty()) {
            return "";
        }
        
        // Find checkpoint with latest timestamp
        auto latestIt = std::max_element(checkpoints_.begin(), checkpoints_.end(),
            [](const auto& a, const auto& b) {
                return a.second.timestamp < b.second.timestamp;
            });
        
        return getCheckpointPath(latestIt->first);
    }
    
    bool CheckpointManager::shouldSave(uint64_t currentStep) const {
        return policy_.shouldCreateCheckpoint(currentStep, 0.0, 0, "");
    }
    
    bool CheckpointManager::createCheckpoint(const std::string& name) {
        try {
            // Generate checkpoint ID
            std::string checkpointId = "ckpt_" + std::to_string(std::time(nullptr)) + "_" + std::to_string(lastSaveStep_ + 1);
            
            // Collect checkpoint data
            std::vector<uint8_t> checkpointData;
            
            // Serialize brain state
            if (brainSerializer_) {
                // This would require access to a Brain instance
                // For now, create placeholder data
                checkpointData = serializeBrainDataPlaceholder();
            }
            
            // Serialize memory state
            if (memorySerializer_) {
                // This would require access to a Memory instance
                // For now, create placeholder data
                std::vector<uint8_t> memoryData = serializeMemoryDataPlaceholder();
                checkpointData.insert(checkpointData.end(), memoryData.begin(), memoryData.end());
            }
            
            // Create metadata
            CheckpointMetadata metadata;
            metadata.name = name.empty() ? ("checkpoint_" + std::to_string(lastSaveStep_ + 1)) : name;
            metadata.step = lastSaveStep_ + 1;
            metadata.simulationTime = lastSaveTime_;
            metadata.neuronCount = 0; // TODO: get from brain
            metadata.synapseCount = 0; // TODO: get from brain
            metadata.regionCount = 0; // TODO: get from brain
            metadata.fileSize = checkpointData.size();
            metadata.timestamp = std::time(nullptr);
            metadata.author = "nlm_system";
            metadata.version = "1.0.0";
            metadata.compressed = compress_;
            metadata.compressionAlgorithm = static_cast<uint32_t>(compressionAlgorithm_);
            metadata.checksum = calculateChecksum(checkpointData);
            
            // Write checkpoint
            if (!writeCheckpoint(metadata, checkpointData)) {
                return false;
            }
            
            // Update current checkpoint
            currentCheckpointId_ = checkpointId;
            lastSaveStep_++;
            
            // Update index
            updateCheckpointMetadata(checkpointId);
            
            // Update statistics
            incrementCheckpointsCreated();
            
            NLM_LOG_INFO("Checkpoint created: " + metadata.name + " (step " + 
                        std::to_string(metadata.step) + ", " + std::to_string(metadata.fileSize) + 
                        " bytes)");
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception creating checkpoint: ") + e.what());
            return false;
        }
    }
    
    bool CheckpointManager::writeCheckpoint(
        const CheckpointMetadata& metadata,
        const std::vector<uint8_t>& data) {
        
        // Compress if needed
        std::vector<uint8_t> compressedData = data;
        if (compress_) {
            compressedData = CompressionEngine::compress(
                data, compressionAlgorithm_, compressionLevel_);
        }
        
        // Write to storage
        if (storageBackend_) {
            return storageBackend_->write(metadata.getId(), compressedData);
        } else {
            // Write to local file
            std::string filepath = getCheckpointPath(metadata.getId());
            std::ofstream file(filepath, std::ios::binary);
            if (!file) {
                return false;
            }
            
            // Write metadata
            if (!writeMetadata(file, metadata)) {
                return false;
            }
            
            // Write data
            if (!file.write(reinterpret_cast<const char*>(compressedData.data()), 
                          compressedData.size())) {
                return false;
            }
            
            return true;
        }
    }
    
    bool CheckpointManager::readCheckpoint(
        const CheckpointMetadata& metadata,
        std::vector<uint8_t>& data) {
        
        if (storageBackend_) {
            return storageBackend_->load(metadata.getId(), data);
        } else {
            std::string filepath = getCheckpointPath(metadata.getId());
            std::ifstream file(filepath, std::ios::binary);
            if (!file) {
                return false;
            }
            
            // Read metadata (skip it)
            skipMetadata(file);
            
            // Read data
            file.seekg(0, std::ios::end);
            size_t fileSize = file.tellg();
            file.seekg(0, std::ios::beg);
            file.seekg(sizeof(CheckpointMetadata), std::ios::cur); // Skip metadata
            
            data.resize(fileSize - sizeof(CheckpointMetadata));
            if (!file.read(reinterpret_cast<char*>(data.data()), data.size())) {
                return false;
            }
            
            // Decompress if needed
            if (metadata.compressed && metadata.compressionAlgorithm > 0) {
                // Decompress data
                // In real implementation, use CompressionEngine
            }
            
            return true;
        }
    }
    
    std::vector<uint8_t> CheckpointManager::createIncrementalData(
        uint64_t baseStep,
        const std::vector<uint8_t>& baseData) {
        
        // Placeholder implementation for incremental checkpointing
        return baseData;
    }
    
    std::vector<uint8_t> CheckpointManager::createDifferentialData(
        uint64_t baseStep,
        const std::vector<uint8_t>& baseData) {
        
        // Placeholder implementation for differential checkpointing
        return CompressionEngine::compressDelta(baseData, baseData, compressionAlgorithm_);
    }
    
    void CheckpointManager::updateCheckpointMetadata(const std::string& checkpointId) {
        auto it = checkpoints_.find(checkpointId);
        if (it == checkpoints_.end()) {
            return;
        }
        
        CheckpointMetadata& metadata = it->second;
        
        // Update step index
        if (stepIndex_.count(metadata.step) > 0) {
            // Remove old entry
            const std::string& oldId = stepIndex_[metadata.step];
            if (oldId != checkpointId) {
                stepIndex_.erase(metadata.step);
                stepIndex_[metadata.step] = checkpointId;
            }
        } else {
            stepIndex_[metadata.step] = checkpointId;
        }
        
        // Update time index
        if (timeIndex_.count(metadata.timestamp) > 0) {
            const std::string& oldId = timeIndex_[metadata.timestamp];
            if (oldId != checkpointId) {
                timeIndex_.erase(metadata.timestamp);
                timeIndex_[metadata.timestamp] = checkpointId;
            }
        } else {
            timeIndex_[metadata.timestamp] = checkpointId;
        }
        
        // Update indexer
        for (auto& indexerPair : indexers_) {
            indexerPair.second->indexCheckpoint(metadata);
        }
    }
    
    void CheckpointManager::cleanupOldCheckpoints() {
        if (!autoCleanup_) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(operationsMutex_);
        
        auto checkpoints = listCheckpoints();
        time_t now = std::time(nullptr);
        
        for (const auto& metadata : checkpoints) {
            double age = metadata.getCheckpointAge();
            if (age > policy_.minCheckpointAge) {
                deleteCheckpoint(metadata.getId());
            }
        }
    }
    
    bool CheckpointManager::ensureDirectoryExists(const std::string& path) const {
        try {
            std::filesystem::create_directories(path);
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to create directory: ") + e.what());
            return false;
        }
    }
    
    std::string CheckpointManager::getCheckpointPath(const std::string& checkpointId) const {
        return checkpointDir_ + "/" + checkpointId + ".nckpt";
    }
    
    bool CheckpointManager::validateCheckpointData(const std::vector<uint8_t>& data) const {
        if (data.size() < sizeof(uint64_t)) {
            return false;
        }
        
        // Simple checksum validation
        uint64_t storedChecksum = *reinterpret_cast<const uint64_t*>(data.data());
        uint64_t calculatedChecksum = calculateChecksum(data);
        
        return storedChecksum == calculatedChecksum;
    }
    
    bool CheckpointManager::validateCheckpointIntegrity(const std::string& checkpointId) const {
        auto metadataIt = checkpoints_.find(checkpointId);
        if (metadataIt == checkpoints_.end()) {
            return false;
        }
        
        const auto& metadata = metadataIt->second;
        
        // Check if checksum matches
        if (metadata.checksum == 0) {
            return false; // Checksum not calculated
        }
        
        // In a real implementation, read the file and verify checksum
        return true;
    }
    
    std::vector<uint8_t> CheckpointManager::serializeBrainData(const Brain& brain) const {
        if (brainSerializer_) {
            return brainSerializer_(brain);
        }
        return std::vector<uint8_t>();
    }
    
    std::vector<uint8_t> CheckpointManager::serializeMemoryData(const Memory& memory) const {
        if (memorySerializer_) {
            return memorySerializer_(memory);
        }
        return std::vector<uint8_t>();
    }
    
    std::vector<uint8_t> CheckpointManager::serializeBrainDataPlaceholder() const {
        std::vector<uint8_t> data;
        
        // Add a magic number to identify this as a valid checkpoint
        uint64_t magic = 0x4E4C4D434B545430ULL; // "NLMCKT0"
        data.insert(data.end(), reinterpret_cast<const uint8_t*>(&magic),
                   reinterpret_cast<const uint8_t*>(&magic) + sizeof(uint64_t));
        
        // Add placeholder data
        data.resize(1024); // Minimal valid size
        
        return data;
    }
    
    std::vector<uint8_t> CheckpointManager::serializeMemoryDataPlaceholder() const {
        std::vector<uint8_t> data(512); // Minimal memory data
        return data;
    }
    
    uint64_t CheckpointManager::calculateChecksum(const std::vector<uint8_t>& data) const {
        return ChecksumCalculator::crc64(data.data(), data.size());
    }
    
    bool CheckpointManager::writeMetadata(std::ofstream& file, const CheckpointMetadata& metadata) {
        // Serialize metadata to file
        std::stringstream ss;
        ss << metadata.name << "|" << metadata.step << "|" << metadata.simulationTime
           << "|" << metadata.neuronCount << "|" << metadata.synapseCount
           << "|" << metadata.regionCount << "|" << metadata.fileSize
           << "|" << metadata.timestamp << "|" << metadata.author
           << "|" << metadata.version;
        
        std::string metadataStr = ss.str();
        
        if (!file.write(metadataStr.c_str(), metadataStr.size())) {
            return false;
        }
        
        return file.flush();
    }
    
    bool CheckpointManager::skipMetadata(std::ifstream& file) {
        // Read metadata (simplified implementation)
        std::string line;
        std::getline(file, line, '|');
        return true;
    }
    
    void CheckpointManager::cleanupThreadFunc() {
        while (!shutdownRequested_) {
            std::unique_lock<std::mutex> lock(operationsMutex_);
            
            // Wait for cleanup interval
            cleanupCondition_.wait_for(lock, std::chrono::seconds(60),
                [this]() { return shutdownRequested_; });
            
            if (shutdownRequested_) {
                break;
            }
            
            cleanupOldCheckpoints();
        }
    }
    
    void CheckpointManager::incrementCheckpointsCreated() {
        std::lock_guard<std::mutex> lock(statsMutex_);
        ++checkpointsCreated_;
    }
    
    void CheckpointManager::incrementCheckpointsLoaded() {
        std::lock_guard<std::mutex> lock(statsMutex_);
        ++checkpointsLoaded_;
    }
    
    void CheckpointManager::incrementCheckpointsDeleted(uint64_t bytes) {
        std::lock_guard<std::mutex> lock(statsMutex_);
        ++checkpointsDeleted_;
    }
    
    void CheckpointManager::addToTotalBytesSaved(uint64_t bytes) {
        std::lock_guard<std::mutex> lock(statsMutex_);
        totalBytesSaved_ += bytes;
    }
    
    void CheckpointManager::addToTotalBytesLoaded(uint64_t bytes) {
        std::lock_guard<std::mutex> lock(statsMutex_);
        totalBytesLoaded_ += bytes;
    }
    
    CheckpointManager::CheckpointStats CheckpointManager::getStats() const {
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        CheckpointStats stats;
        stats.totalCount = checkpointsCreated_ + checkpointsDeleted_;
        stats.validCount = stats.totalCount; // In real implementation, track valid/invalid
        stats.invalidCount = 0;
        stats.totalSize = totalBytesSaved_;
        stats.avgSize = stats.totalCount > 0 ? totalBytesSaved_ / stats.totalCount : 0;
        stats.maxSize = 0; // Track max size
        stats.totalCompressionRatio = 0.0; // Calculate from data
        stats.oldestTimestamp = 0;
        stats.newestTimestamp = 0;
        
        return stats;
    }
    
    void CheckpointManager::rebuildIndex() {
        std::lock_guard<std::mutex> lock(operationsMutex_);
        
        checkpoints_.clear();
        stepIndex_.clear();
        timeIndex_.clear();
        
        // Scan checkpoint directory for existing checkpoints
        try {
            for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
                if (entry.is_regular_file() && entry.path().extension() == ".nckpt") {
                    std::string checkpointId = entry.path().stem().string();
                    CheckpointMetadata metadata = readMetadataFromFile(entry.path());
                    if (metadata.step > 0) {
                        checkpoints_[checkpointId] = metadata;
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to rebuild checkpoint index: ") + e.what());
        }
        
        // Update indexer
        for (auto& indexerPair : indexers_) {
            indexerPair.second->rebuildIndex();
        }
    }
    
    CheckpointMetadata CheckpointManager::readMetadataFromFile(const std::filesystem::path& filepath) {
        CheckpointMetadata metadata;
        
        // Read and parse metadata from file
        // This is a simplified implementation
        
        return metadata;
    }
    
    std::vector<std::string> CheckpointManager::getActiveCheckpoints() const {
        std::lock_guard<std::mutex> lock(operationsMutex_);
        
        std::vector<std::string> active;
        
        // Check pending operations
        for (const auto& future : pendingOperations_) {
            if (future.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) {
                active.push_back("pending");
            }
        }
        
        // Check operation queue
        active.insert(active.end(), operationQueue_.begin(), operationQueue_.end());
        
        return active;
    }
    
} // namespace nlm