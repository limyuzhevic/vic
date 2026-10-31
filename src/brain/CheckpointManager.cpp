// CheckpointManager implementation
#include "BrainComponents.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

CheckpointManager::CheckpointManager(std::shared_ptr<Brain> brain, const BrainSystemConfig& config)
    : brain_(brain), config_(config), lastCheckpointStep_(0) {
    checkpointSystem_ = std::make_unique<CheckpointManager>();
}

bool CheckpointManager::initialize() {
    NLM_LOG_INFO("Initializing CheckpointManager...");
    
    // Configure checkpoint system
    checkpointSystem_->configure(config_.checkpointDir, 10000, 5, true);
    
    NLM_LOG_INFO("CheckpointManager initialized successfully");
    return true;
}

void CheckpointManager::reset() {
    NLM_LOG_INFO("Resetting CheckpointManager...");
    checkpointSystem_->reset();
    lastCheckpointStep_ = 0;
    NLM_LOG_INFO("CheckpointManager reset complete");
}

void CheckpointManager::update() {
    // Checkpoint updates are handled during simulation steps
}

void CheckpointManager::logStatus() const {
    NLM_LOG_INFO("=== CheckpointManager Status ===");
    NLM_LOG_INFO("Checkpoint directory: " + config_.checkpointDir);
    NLM_LOG_INFO("Checkpoint interval: " + std::to_string(config_.replayInterval));
    NLM_LOG_INFO("Consolidation interval: " + std::to_string(config_.consolidationInterval));
    NLM_LOG_INFO("Last checkpoint step: " + std::to_string(lastCheckpointStep_));
}

bool CheckpointManager::save(const std::string& filepath) const {
    return brain_->save(filepath);
}

bool CheckpointManager::load(const std::string& filepath) {
    return brain_->load(filepath);
}

void CheckpointManager::update(SimulationStep step, Timestamp time) {
    if (shouldCheckpoint(step)) {
        // Create checkpoint
        std::string checkpointFile = config_.checkpointDir + "/checkpoint_" + 
                                     std::to_string(step) + ".bin";
        if (save(checkpointFile)) {
            lastCheckpointStep_ = step;
            NLM_LOG_INFO("Checkpoint saved: " + checkpointFile);
        }
    }
}

bool CheckpointManager::shouldCheckpoint(SimulationStep step) const {
    return step % config_.replayInterval == 0 && step > lastCheckpointStep_;
}

} // namespace nlm