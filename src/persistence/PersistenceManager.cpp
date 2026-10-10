// Persistence manager implementation
#include "PersistenceManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

PersistenceManager::PersistenceManager() : dataDir_(), config_(nullptr), 
                                        backupPolicy_(BackupPolicy::None),
                                        compressionAlgorithm_(CompressionAlgorithm::Zlib),
                                        encryptionAlgorithm_(EncryptionAlgorithm::None),
                                        retentionPolicy_(), status_(PersistenceStatus::Inactive),
                                        startTime_(0), errorCount_(0), stats_(), checkpointEventCallback_(nullptr) {
}

PersistenceManager::~PersistenceManager() {
    shutdown();
}

bool PersistenceManager::configure(const std::string& dataDir, const std::shared_ptr<Config>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    setStatus(PersistenceStatus::Initializing);
    
    try {
        dataDir_ = dataDir;
        config_ = config;
        
        // Create data directory
        if (!createCheckpointDirectory()) {
            errorCount_++;
            setStatus(PersistenceStatus::Error);
            NLM_LOG_ERROR("Failed to create checkpoint directory: " + dataDir_);
            return false;
        }
        
        // Initialize components
        checkpointManager_ = std::make_unique<CheckpointManager>();
        checkpointValidator_ = std::make_unique<CheckpointValidator>();
        checkpointIndexer_ = std::make_unique<CheckpointIndexer>();
        checkpointCompressor_ = std::make_unique<CheckpointCompressor>();
        checkpointEncryptor_ = std::make_unique<CheckpointEncryptor>();
        resumeManager_ = std::make_unique<ResumeManager>();
        backupManager_ = std::make_unique<BackupManager>();
        memoryPersistence_ = std::make_unique<MemoryPersistence>();
        neuralStatePersistence_ = std::make_unique<NeuralStatePersistence>();
        configurationPersistence_ = std::make_unique<ConfigurationPersistence>();
        neuromodulationPersistence_ = std::make_unique<NeuromodulationPersistence>();
        developmentPersistence_ = std::make_unique<DevelopmentPersistence>();
        versionControl_ = std::make_unique<VersionControl>();
        diffTracker_ = std::make_unique<DiffTracker>();
        
        // Configure checkpoint manager
        uint64_t saveInterval = config_ ? config_->getOr<uint64_t>("checkpoint_save_interval", 10000) : 10000;
        size_t maxCheckpoints = config_ ? config_->getOr<size_t>("max_checkpoints", 10) : 10;
        bool compress = config_ ? config_->getOr<bool>("checkpoint_compress", true) : true;
        
        checkpointManager_->configure(dataDir_ + "/checkpoints", saveInterval, maxCheckpoints, compress);
        
        // Configure other components
        // (In real implementation would set up compression, encryption, indexing, etc.)
        
        setStatus(PersistenceStatus::Active);
        startTime_ = time(nullptr);
        
        NLM_LOG_INFO("Persistence manager initialized successfully in: " + dataDir_);
        NLM_LOG_INFO("Checkpoint interval: " + std::to_string(saveInterval) + " steps");
        NLM_LOG_INFO("Max checkpoints: " + std::to_string(maxCheckpoints));
        
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        setStatus(PersistenceStatus::Error);
        NLM_LOG_ERROR(std::string("Persistence manager configuration failed: ") + e.what());
        return false;
    }
}

void PersistenceManager::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (status_ == PersistenceStatus::Shutdown) return;
    
    NLM_LOG_INFO("Shutting down persistence manager...");
    
    // Cleanup components
    checkpointManager_.reset();
    checkpointValidator_.reset();
    checkpointIndexer_.reset();
    checkpointCompressor_.reset();
    checkpointEncryptor_.reset();
    resumeManager_.reset();
    backupManager_.reset();
    memoryPersistence_.reset();
    neuralStatePersistence_.reset();
    configurationPersistence_.reset();
    neuromodulationPersistence_.reset();
    developmentPersistence_.reset();
    versionControl_.reset();
    diffTracker_.reset();
    
    setStatus(PersistenceStatus::Shutdown);
    NLM_LOG_INFO("Persistence manager shutdown complete");
}

bool PersistenceManager::createCheckpoint(const std::string& name, const CheckpointOptions& options) {
    return executeCheckpointOperation("create", name, options);
}

bool PersistenceManager::saveCheckpoint(const std::string& name, const CheckpointOptions& options) {
    return executeCheckpointOperation("save", name, options);
}

bool PersistenceManager::loadCheckpoint(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (status_ != PersistenceStatus::Active) {
        NLM_LOG_ERROR("Persistence manager is not active");
        return false;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    PersistenceResult result;
    result.success = false;
    
    try {
        setStatus(PersistenceStatus::Loading);
        
        // Validate checkpoint
        if (!validateCheckpoint(name)) {
            errorCount_++;
            result.message = "Checkpoint validation failed: " + 
                           (validationErrors_.empty() ? "unknown error" : validationErrors_.back());
            logEvent("load_failed", name, result);
            return false;
        }
        
        // Load checkpoint
        if (!resumeManager_->loadCheckpoint(name)) {
            errorCount_++;
            result.message = "Failed to load checkpoint: " + name;
            logEvent("load_failed", name, result);
            return false;
        }
        
        result.success = true;
        result.message = "Checkpoint loaded successfully";
        
        updateStats(result);
        logEvent("loaded", name, result);
        
        NLM_LOG_INFO("Checkpoint loaded successfully: " + name);
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        result.message = std::string("Exception loading checkpoint: ") + e.what();
        logEvent("load_exception", name, result);
        return false;
    }
}

bool PersistenceManager::deleteCheckpoint(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    PersistenceResult result;
    result.success = false;
    
    try {
        std::string filepath = getCheckpointPath(name);
        
        // Delete checkpoint file
        if (std::remove(filepath.c_str()) != 0) {
            errorCount_++;
            result.message = "Failed to delete checkpoint file: " + filepath;
            logEvent("delete_failed", name, result);
            return false;
        }
        
        // Update index
        if (checkpointIndexer_) {
            checkpointIndexer_->removeEntry(name);
        }
        
        result.success = true;
        result.message = "Checkpoint deleted successfully";
        
        stats_.checkpointsDeleted++;
        updateStats(result);
        logEvent("deleted", name, result);
        
        NLM_LOG_INFO("Checkpoint deleted successfully: " + name);
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        result.message = std::string("Exception deleting checkpoint: ") + e.what();
        logEvent("delete_exception", name, result);
        return false;
    }
}

std::vector<std::string> PersistenceManager::listCheckpoints() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> checkpoints;
    
    if (checkpointIndexer_) {
        checkpoints = checkpointIndexer_->getAllCheckpoints();
    }
    
    return checkpoints;
}

bool PersistenceManager::validateCheckpoint(const std::string& name) {
    validationErrors_.clear();
    
    // Clear previous validation errors
    if (checkpointValidator_) {
        return checkpointValidator_->validate(name, validationErrors_);
    }
    
    return true;
}

bool PersistenceManager::resumeFromCheckpoint(const std::string& name) {
    if (!resumeManager_) return false;
    return resumeManager_->resumeFromCheckpoint(name);
}

bool PersistenceManager::createIncrementalCheckpoint() {
    if (!resumeManager_) return false;
    return resumeManager_->createIncrementalCheckpoint();
}

std::string PersistenceManager::getLastCheckpoint() const {
    if (!resumeManager_) return "";
    return resumeManager_->getLastCheckpointPath();
}

std::vector<std::string> PersistenceManager::getCheckpointHistory(size_t limit) const {
    if (!versionControl_) return {};
    return versionControl_->getHistory(limit);
}

bool PersistenceManager::saveToFile(const std::string& path) {
    if (!configurationPersistence_) return false;
    return configurationPersistence_->saveToFile(path);
}

bool PersistenceManager::loadFromFile(const std::string& path) {
    if (!configurationPersistence_) return false;
    return configurationPersistence_->loadFromFile(path);
}

bool PersistenceManager::exportToJSON(const std::string& path) {
    if (!configurationPersistence_) return false;
    return configurationPersistence_->exportToJSON(path);
}

bool PersistenceManager::importFromJSON(const std::string& path) {
    if (!configurationPersistence_) return false;
    return configurationPersistence_->importFromJSON(path);
}

std::string PersistenceManager::getStatusString() const {
    switch (status_) {
        case PersistenceStatus::Inactive: return "Inactive";
        case PersistenceStatus::Initializing: return "Initializing";
        case PersistenceStatus::Active: return "Active";
        case PersistenceStatus::Saving: return "Saving";
        case PersistenceStatus::Loading: return "Loading";
        case PersistenceStatus::Error: return "Error";
        case PersistenceStatus::Shutdown: return "Shutdown";
        default: return "Unknown";
    }
}

uint64_t PersistenceManager::getTotalCheckpoints() const {
    if (!checkpointIndexer_) return 0;
    return checkpointIndexer_->getCheckpointCount();
}

uint64_t PersistenceManager::getTotalSize() const {
    if (!checkpointIndexer_) return 0;
    return checkpointIndexer_->getTotalSize();
}

double PersistenceManager::getUptime() const {
    if (startTime_ == 0) return 0.0;
    return difftime(time(nullptr), startTime_);
}

bool PersistenceManager::validateConfiguration() const {
    validationErrors_.clear();
    
    if (dataDir_.empty()) {
        validationErrors_.push_back("Data directory is not configured");
    }
    
    if (!std::filesystem::exists(dataDir_)) {
        validationErrors_.push_back("Data directory does not exist: " + dataDir_);
    }
    
    if (config_ && !config_->getOr<bool>("enable_checkpointing", true)) {
        validationErrors_.push_back("Checkpointing is disabled in configuration");
    }
    
    return validationErrors_.empty();
}

bool PersistenceManager::validateCheckpoints() const {
    validationErrors_.clear();
    
    if (!checkpointValidator_) return false;
    
    std::vector<std::string> checkpoints = listCheckpoints();
    bool allValid = true;
    
    for (const auto& checkpoint : checkpoints) {
        std::vector<std::string> errors;
        if (!checkpointValidator_->validate(checkpoint, errors)) {
            allValid = false;
            for (const auto& error : errors) {
                validationErrors_.push_back("Checkpoint '" + checkpoint + "': " + error);
            }
        }
    }
    
    return allValid;
}

void PersistenceManager::updateStats(const PersistenceResult& result) {
    stats_.totalTimeSpent += result.elapsedTime;
    if (!result.success) {
        stats_.errorCount++;
    }
}

void PersistenceManager::logEvent(const std::string& eventType, const std::string& checkpointName,
                                 const PersistenceResult& result) {
    std::stringstream logMsg;
    logMsg << "[Persistence] " << eventType << " - Checkpoint: " << checkpointName;
    
    if (!result.message.empty()) {
        logMsg << " - " << result.message;
    }
    
    if (result.success) {
        NLM_LOG_INFO(logMsg.str());
    } else {
        NLM_LOG_ERROR(logMsg.str());
    }
    
    // Call event callback if registered
    if (checkpointEventCallback_) {
        checkpointEventCallback_(checkpointName, result);
    }
}

bool PersistenceManager::createCheckpointDirectory() {
    try {
        if (!std::filesystem::exists(dataDir_)) {
            if (!std::filesystem::create_directories(dataDir_)) {
                return false;
            }
        }
        
        // Create subdirectories
        std::vector<std::string> subdirs = {"checkpoints", "backups", "archives"};
        for (const auto& subdir : subdirs) {
            std::string fullPath = dataDir_ + "/" + subdir;
            if (!std::filesystem::exists(fullPath)) {
                if (!std::filesystem::create_directories(fullPath)) {
                    return false;
                }
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception creating checkpoint directory: ") + e.what());
        return false;
    }
}

PersistenceResult PersistenceManager::executeCheckpointOperation(const std::string& operation,
                                                                 const std::string& name,
                                                                 const CheckpointOptions& options) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    PersistenceResult result;
    result.success = false;
    
    try {
        setStatus(PersistenceStatus::Saving);
        
        // Validate configuration
        if (!validateConfiguration()) {
            errorCount_++;
            result.message = "Invalid configuration";
            for (const auto& error : validationErrors_) {
                NLM_LOG_ERROR(error);
            }
            return result;
        }
        
        // Create checkpoint name if not provided
        std::string checkpointName = name.empty() ? generateCheckpointName() : name;
        
        // Backup before save if requested
        if (options.backupBeforeSave && backupPolicy_ != BackupPolicy::None) {
            if (!performBackup(checkpointName)) {
                errorCount_++;
                result.message = "Backup failed before checkpoint save";
                setStatus(PersistenceStatus::Active);
                return result;
            }
        }
        
        // Create checkpoint
        if (!createCheckpointImpl(checkpointName, options)) {
            errorCount_++;
            result.message = "Failed to create checkpoint: " + checkpointName;
            setStatus(PersistenceStatus::Active);
            return result;
        }
        
        // Validate checkpoint if requested
        if (options.validateAfterSave) {
            if (!validateCheckpoint(checkpointName)) {
                errorCount_++;
                result.message = "Checkpoint validation failed after save";
                setStatus(PersistenceStatus::Active);
                return result;
            }
        }
        
        // Apply retention policy
        if (retentionPolicy_.autoPrune) {
            applyRetentionPolicy();
        }
        
        result.success = true;
        result.message = "Checkpoint " + operation + "d successfully: " + checkpointName;
        
        updateStats(result);
        logEvent(operation, checkpointName, result);
        
        NLM_LOG_INFO("Checkpoint " + operation + "d successfully: " + checkpointName);
        
    } catch (const std::exception& e) {
        errorCount_++;
        result.message = std::string("Exception in checkpoint operation: ") + e.what();
        logEvent(operation + "_exception", name, result);
    }
    
    setStatus(PersistenceStatus::Active);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
    
    return result;
}

bool PersistenceManager::createCheckpointImpl(const std::string& name, const CheckpointOptions& options) {
    // Create checkpoint using checkpoint manager
    if (!checkpointManager_) return false;
    
    // In real implementation, would create checkpoint with all components:
    // 1. Neural state (via neuralStatePersistence_)
    // 2. Memory state (via memoryPersistence_)
    // 3. Neuromodulation state (via neuromodulationPersistence_)
    // 4. Development state (via developmentPersistence_)
    // 5. Configuration (via configurationPersistence_)
    // 6. Spike history, random state, simulation state
    
    // For demonstration, just call checkpoint manager's save
    return checkpointManager_->saveImmediately(name);
}

bool PersistenceManager::performBackup(const std::string& checkpointName) {
    if (!backupManager_) return false;
    return backupManager_->createBackup(checkpointName);
}

bool PersistenceManager::applyRetentionPolicy() {
    if (!backupManager_) return false;
    return backupManager_->applyRetentionPolicy(retentionPolicy_);
}

std::string PersistenceManager::generateCheckpointName(const std::string& baseName) const {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << baseName << "_" << timestamp << "_" << std::this_thread::get_id();
    return ss.str();
}

std::string PersistenceManager::getCheckpointPath(const std::string& name) const {
    return dataDir_ + "/checkpoints/" + name + ".bin";
}

std::string PersistenceManager::getBackupPath(const std::string& checkpointName) const {
    return dataDir_ + "/backups/" + checkpointName + ".bak";
}

} // namespace nlm
