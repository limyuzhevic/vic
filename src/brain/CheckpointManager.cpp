#include "CheckpointManager.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

CheckpointManager::CheckpointManager(Brain* brain)
    : brain_(brain)
    , checkpointManager(std::make_unique<CheckpointManager>()) {
}

CheckpointManager::~CheckpointManager() = default;

bool CheckpointManager::initialize() {
    NLM_LOG_INFO("Initializing Checkpoint Manager...");
    
    if (!brain_) return false;
    
    // Configure checkpoint manager
    std::string checkpointDir = brain_->pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    checkpointManager->configure(checkpointDir, 10000, 5, true);
    
    NLM_LOG_INFO("Checkpoint Manager initialized successfully");
    return true;
}

void CheckpointManager::reset() {
    // Checkpoint manager maintains state across steps
}

void CheckpointManager::step(SimulationStep currentStep, Timestamp currentTime) {
    if (checkpointManager) {
        checkpointManager->update(currentStep, currentTime);
    }
}

bool CheckpointManager::save(const std::string& filepath) const {
    if (!brain_) return false;
    return brain_->save(filepath);
}

bool CheckpointManager::load(const std::string& filepath) {
    if (!brain_) return false;
    return brain_->load(filepath);
}

} // namespace nlm
