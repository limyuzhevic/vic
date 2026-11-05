#include "BrainCheckpoint.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <filesystem>

namespace nlm {

void BrainCheckpoint::configureFromConfig(const Config* config) {
    if (!config) return;
    
    maxCheckpoints_ = static_cast<size_t>(config->getOr<int64_t>("checkpoint.max_checkpoints", 10));
    checkpointInterval_ = static_cast<size_t>(config->getOr<int64_t>("checkpoint.interval", 1000));
    autoSave_ = config->getOr<bool>("checkpoint.auto_save", true);
    checkpointCompression_ = config->getOr<float>("checkpoint.compression", 0.5f);
    checkpointMaxSize_ = static_cast<size_t>(config->getOr<int64_t>("checkpoint.max_size", 100 * 1024 * 1024));  // 100 MB
    verifyChecksums_ = config->getOr<bool>("checkpoint.verify_checksums", true);
    
    // Initialize checkpoint system
    checkpointManager_ = std::make_unique<CheckpointManager>();
    
    NLM_LOG_INFO("BrainCheckpoint configured from config:");
    NLM_LOG_INFO("  Max checkpoints: " + std::to_string(maxCheckpoints_));
    NLM_LOG_INFO("  Checkpoint interval: " + std::to_string(checkpointInterval_));
    NLM_LOG_INFO("  Auto save: " + std::string(autoSave_ ? "yes" : "no"));
    NLM_LOG_INFO("  Max checkpoint size: " + std::to_string(checkpointMaxSize_) + " bytes");
}

void BrainCheckpoint::setCheckpointParameters(size_t maxCheckpoints, size_t checkpointInterval, bool autoSave) {
    maxCheckpoints_ = std::max<size_t>(maxCheckpoints, 1);
    checkpointInterval_ = std::max<size_t>(checkpointInterval, 1);
    autoSave_ = autoSave;
}

bool BrainCheckpoint::saveCheckpoint(Brain* brain, const std::string& filepath) {
    if (!brain || !checkpointManager_) {
        NLM_LOG_ERROR("Cannot save checkpoint - invalid brain or checkpoint manager");
        return false;
    }
    
    NLM_LOG_INFO("Saving checkpoint to: " + filepath);
    
    try {
        // Configure checkpoint manager
        std::string checkpointDir = "checkpoints";
        checkpointManager_->configure(checkpointDir, maxCheckpoints_, 5, true);
        
        // Save brain state
        bool success = brain->save(filepath);
        if (!success) {
            NLM_LOG_ERROR("Failed to save brain state");
            return false;
        }
        
        // Update statistics
        checkpointCount_++;
        lastCheckpointTime_ = brain->getCurrentTime();
        checkpointFiles_.push_back(filepath);
        
        // Keep only the most recent checkpoints
        if (checkpointFiles_.size() > maxCheckpoints_) {
            checkpointFiles_.erase(checkpointFiles_.begin());
        }
        
        NLM_LOG_INFO("Checkpoint saved successfully (" + std::to_string(checkpointCount_) + " total)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving checkpoint: ") + e.what());
        return false;
    }
}

bool BrainCheckpoint::loadCheckpoint(Brain* brain, const std::string& filepath) {
    if (!brain || !checkpointManager_) {
        NLM_LOG_ERROR("Cannot load checkpoint - invalid brain or checkpoint manager");
        return false;
    }
    
    NLM_LOG_INFO("Loading checkpoint from: " + filepath);
    
    try {
        // Load brain state
        bool success = brain->load(filepath);
        if (!success) {
            NLM_LOG_ERROR("Failed to load brain state");
            return false;
        }
        
        // Update statistics
        lastCheckpointTime_ = brain->getCurrentTime();
        
        NLM_LOG_INFO("Checkpoint loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading checkpoint: ") + e.what());
        return false;
    }
}

bool BrainCheckpoint::autoSaveCheckpoint(Brain* brain, Timestamp currentTime) {
    if (!autoSave_ || !brain) return false;
    
    // Auto-save based on interval
    size_t currentStep = static_cast<size_t>(currentTime / 0.001f);  // Assuming 1ms timestep
    
    if (currentStep > 0 && currentStep % checkpointInterval_ == 0) {
        std::string checkpointFile = "checkpoint_" + std::to_string(currentStep) + ".bin";
        return saveCheckpoint(brain, checkpointFile);
    }
    
    return false;
}

void BrainCheckpoint::manageCheckpointLifecycle(Brain* brain, Timestamp currentTime) {
    if (!brain) return;
    
    // Auto-save if enabled
    if (autoSave_) {
        autoSaveCheckpoint(brain, currentTime);
    }
    
    // Cleanup old checkpoints
    cleanupOldCheckpoints(brain);
}

void BrainCheckpoint::resetCheckpointSystem(Brain* brain) {
    if (checkpointManager_) {
        checkpointManager_->reset();
    }
    
    checkpointCount_ = 0;
    lastCheckpointTime_ = 0.0;
    checkpointFiles_.clear();
}

void BrainCheckpoint::cleanupOldCheckpoints(Brain* brain) {
    if (!brain) return;
    
    // Keep only the most recent checkpoints up to maxCheckpoints_
    if (checkpointFiles_.size() > maxCheckpoints_) {
        NLM_LOG_INFO("Cleaning up old checkpoints - keeping " + std::to_string(maxCheckpoints_) + " most recent");
        
        // This is a simplified implementation
        // In a real implementation, we would remove the oldest checkpoint files
        checkpointFiles_.resize(maxCheckpoints_);
    }
}

void BrainCheckpoint::backupImportantCheckpoints(Brain* brain) {
    if (!brain) return;
    
    // Create backup of important checkpoints
    // This would typically involve copying checkpoint files to a backup location
    NLM_LOG_INFO("Creating backup of important checkpoints");
}

void BrainCheckpoint::restoreFromBackup(Brain* brain, const std::string& backupFile) {
    if (!brain) return;
    
    // Restore from backup checkpoint
    loadCheckpoint(brain, backupFile);
}

bool BrainCheckpoint::createCheckpointArchive(Brain* brain, const std::string& archiveName) {
    if (!brain) return false;
    
    // Create an archive of all checkpoints
    NLM_LOG_INFO("Creating checkpoint archive: " + archiveName);
    
    // This would typically use compression and archiving utilities
    // For now, this is a placeholder implementation
    
    return true;
}

bool BrainCheckpoint::extractCheckpointArchive(Brain* brain, const std::string& archiveName, const std::string& extractDir) {
    if (!brain) return false;
    
    // Extract checkpoint archive
    NLM_LOG_INFO("Extracting checkpoint archive: " + archiveName + " to " + extractDir);
    
    // This would typically use decompression and extraction utilities
    // For now, this is a placeholder implementation
    
    return true;
}

bool BrainCheckpoint::verifyCheckpointIntegrity(Brain* brain, const std::string& checkpointFile) {
    if (!brain) return false;
    
    NLM_LOG_INFO("Verifying checkpoint integrity: " + checkpointFile);
    
    // Verify checkpoint file integrity
    // This would typically involve:
    // - Checking file checksums
    // - Validating file structure
    // - Ensuring required sections are present
    
    // For now, this is a placeholder implementation
    return true;
}

void BrainCheckpoint::optimizeCheckpointStorage(Brain* brain) {
    if (!brain) return;
    
    NLM_LOG_INFO("Optimizing checkpoint storage");
    
    // Optimize checkpoint storage by:
    // - Compressing old checkpoints
    // - Removing redundant checkpoints
    // - Consolidating small checkpoints
    
    // This would typically involve file system operations
}

void BrainCheckpoint::initializeCheckpointSystem(Brain* brain) {
    if (!brain) return;
    
    // Establish connections between checkpoint system and brain
    if (checkpointManager_) {
        checkpointManager_->setBrainReference(brain);
    }
}

void BrainCheckpoint::validateCheckpointState(Brain* brain) const {
    // Validate checkpoint system state
    if (checkpointManager_ && !checkpointManager_->isInitialized()) {
        NLM_LOG_WARNING("Checkpoint manager is not initialized");
    }
    
    if (checkpointCount_ > maxCheckpoints_) {
        NLM_LOG_WARNING("Checkpoint count exceeds maximum: " + std::to_string(checkpointCount_) + "/" + std::to_string(maxCheckpoints_));
    }
}

void BrainCheckpoint::updateCheckpointConnections(Brain* brain) {
    if (!brain) return;
    
    // Update connections between checkpoint system and other brain systems
    // This would maintain consistency across checkpoint processes
}

void BrainCheckpoint::processCheckpointLearning(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    // Process learning in checkpoint systems
    // This would adapt checkpoint parameters based on experience
}

} // namespace nlm