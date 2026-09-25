#include "BrainCheckpointManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../performance/CheckpointManager.hpp"
#include "../performance/CheckpointWriter.hpp"
#include "../performance/CheckpointReader.hpp"

namespace nlm {

void BrainCheckpointManager::initialize(Brain* brain) {
    auto* config = brain->getConfig().get();
    if (!config) return;
    
    std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    brain->pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

void BrainCheckpointManager::update(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    if (brain->pImpl->checkpointManager) {
        brain->pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

bool BrainCheckpointManager::save(Brain* brain, const std::string& filepath) const {
    if (!brain) return false;
    
    return brain->save(filepath);
}

bool BrainCheckpointManager::load(Brain* brain, const std::string& filepath) {
    if (!brain) return false;
    
    return brain->load(filepath);
}

void BrainCheckpointManager::collectCheckpointStats(Brain* brain) const {
    // Checkpoint statistics will be collected in logStatus()
}

} // namespace nlm
