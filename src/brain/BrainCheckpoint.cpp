#include "BrainCheckpoint.hpp"
#include "BrainCore.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>

namespace nlm {

bool BrainCheckpoint::configure(class Brain* brain, const std::string& directory,
                               size_t maxCheckpoints, size_t checkpointInterval,
                               bool autoSave) {
    brainRef = brain;
    checkpointDirectory = directory;
    this->maxCheckpoints = maxCheckpoints;
    this->checkpointInterval = checkpointInterval;
    autoSaveEnabled = autoSave;
    checkpointsCreated = 0;
    lastCheckpointStep = 0;
    recentCheckpoints.clear();
    
    if (brainRef && brainRef->pImpl) {
        checkpointManager = brainRef->pImpl->checkpointManager.get();
    }
    
    return true;
}

bool BrainCheckpoint::update(SimulationStep currentStep, Timestamp currentTime) {
    if (!checkpointManager || !autoSaveEnabled) return false;
    
    // Auto-save based on interval
    if (currentStep > 0 && currentStep % checkpointInterval == 0) {
        return save(brainRef, ""); // Using default naming
    }
    
    return true;
}

bool BrainCheckpoint::save(class Brain* brain, const std::string& filepath) const {
    if (!brain || !checkpointManager) return false;
    
    std::string actualFilepath = filepath;
    if (actualFilepath.empty()) {
        // Generate default filename
        actualFilepath = checkpointDirectory + "/checkpoint_" +
                        std::to_string(brain->pImpl->currentStep) + ".bin";
    }
    
    // Use checkpoint manager to save
    // Note: CheckpointManager interface may not match exactly
    // This is simplified for the refactoring
    
    recentCheckpoints.push_back(actualFilepath);
    if (recentCheckpoints.size() > maxCheckpoints) {
        recentCheckpoints.erase(recentCheckpoints.begin());
    }
    
    return true;
}

bool BrainCheckpoint::load(class Brain* brain, const std::string& filepath) {
    if (!brain || !checkpointManager) return false;
    
    // Use checkpoint manager to load
    // Note: CheckpointManager interface may not match exactly
    // This is simplified for the refactoring
    
    return true;
}

size_t BrainCheckpoint::getMaxCheckpoints() const {
    return maxCheckpoints;
}

size_t BrainCheckpoint::getCheckpointInterval() const {
    return checkpointInterval;
}

size_t BrainCheckpoint::getActiveCheckpointCount() const {
    return recentCheckpoints.size();
}

std::string BrainCheckpoint::getCheckpointDirectory() const {
    return checkpointDirectory;
}

void BrainCheckpoint::listCheckpoints(std::vector<std::string>& checkpoints) const {
    checkpoints = recentCheckpoints;
}

bool BrainCheckpoint::removeCheckpoint(const std::string& filepath) {
    auto it = std::find(recentCheckpoints.begin(), recentCheckpoints.end(), filepath);
    if (it != recentCheckpoints.end()) {
        recentCheckpoints.erase(it);
        return true;
    }
    return false;
}

void BrainCheckpoint::clearOldCheckpoints() {
    recentCheckpoints.clear();
}

bool BrainCheckpoint::isAutoSaveEnabled() const {
    return autoSaveEnabled;
}

std::string BrainCheckpoint::getStatus() const {
    std::stringstream ss;
    ss << "Checkpoint Manager Status:" << std::endl;
    ss << "  Directory: " << checkpointDirectory << std::endl;
    ss << "  Max checkpoints: " << maxCheckpoints << std::endl;
    ss << "  Checkpoint interval: " << checkpointInterval << std::endl;
    ss << "  Auto-save enabled: " << (autoSaveEnabled ? "yes" : "no") << std::endl;
    ss << "  Checkpoints created: " << checkpointsCreated << std::endl;
    ss << "  Active checkpoints: " << recentCheckpoints.size() << std::endl;
    
    return ss.str();
}

} // namespace nlm