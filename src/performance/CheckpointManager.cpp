#include "CheckpointManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

struct CheckpointManager::Impl {
    std::string checkpointDir_;
    uint64_t saveIntervalSteps_;
    size_t maxCheckpoints_;
    bool compress_;
    uint64_t lastSaveStep_;
    std::string lastCheckpointPath_;
    
    Impl() : saveIntervalSteps_(10000), maxCheckpoints_(10), compress_(true), lastSaveStep_(0) {}
};

CheckpointManager::CheckpointManager() : pImpl(new Impl) {
    NLM_LOG_INFO("CheckpointManager created");
}

CheckpointManager::~CheckpointManager() {
    NLM_LOG_INFO("CheckpointManager destroyed");
}

void CheckpointManager::configure(const std::string& checkpointDir, 
                                 uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints,
                                 bool compress) {
    pImpl->checkpointDir_ = checkpointDir;
    pImpl->saveIntervalSteps_ = saveIntervalSteps;
    pImpl->maxCheckpoints_ = maxCheckpoints;
    pImpl->compress_ = compress;
    
    // Create directory if it doesn't exist
    std::error_code ec;
    if (!std::filesystem::exists(pImpl->checkpointDir_, ec) && 
        !std::filesystem::create_directories(pImpl->checkpointDir_, ec)) {
        NLM_LOG_ERROR("Failed to create checkpoint directory: " + checkpointDir);
    } else {
        NLM_LOG_INFO("Checkpoint directory configured: " + checkpointDir);
    }
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (!shouldSave(currentStep)) {
        return false;
    }
    
    return saveImmediately();
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    // For now, just log the save request
    std::string checkpointName = name.empty() ? "checkpoint_" + std::to_string(pImpl->lastSaveStep_) : name;
    std::string filepath = pImpl->checkpointDir_ + "/" + checkpointName + ".nckpt";
    
    NLM_LOG_INFO("Checkpoint requested: " + filepath);
    // TODO: Implement actual checkpoint saving
    // This would require the CheckpointWriter to be integrated with the brain
    
    pImpl->lastSaveStep_ = pImpl->lastSaveStep_;  // Keep track
    pImpl->lastCheckpointPath_ = filepath;
    
    return true;  // Assume success for now
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = pImpl->checkpointDir_ + "/" + name + ".nckpt";
    
    NLM_LOG_INFO("Loading checkpoint: " + filepath);
    // TODO: Implement actual checkpoint loading
    
    return true;  // Assume success for now
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    // For now, return empty list - TODO: Implement directory scanning
    // This would actually scan the checkpointDir_ for .nckpt files
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    NLM_LOG_INFO("Pruning old checkpoints, keeping " + std::to_string(keepCount) + " most recent");
    // TODO: Implement actual file cleanup
    // This would delete old checkpoint files from the checkpointDir_
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (currentStep == 0) {
        return true;  // Always save initial state
    }
    
    if (pImpl->lastSaveStep_ == 0) {
        return true;  // First save
    }
    
    return (currentStep - pImpl->lastSaveStep_) >= pImpl->saveIntervalSteps_;
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return pImpl->lastCheckpointPath_;
}

} // namespace nlm
