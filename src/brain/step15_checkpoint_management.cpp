// Step 15: Checkpoint management
// This module manages checkpoint saving and loading
// Provides persistence for brain state

#include "step15_checkpoint_management.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

namespace Step15 {

void ConfigureCheckpointManager(Brain::Impl* impl,
                                const std::string& checkpointDir,
                                size_t maxCheckpoints,
                                size_t maxCheckpointSize,
                                bool compressionEnabled) {
    if (!impl) {
        return;
    }
    
    if (impl->checkpointManager) {
        impl->checkpointManager->configure(checkpointDir, maxCheckpoints, maxCheckpointSize, compressionEnabled);
    }
}

void UpdateCheckpointManager(Brain::Impl* impl,
                             SimulationStep currentStep,
                             Timestamp currentTime) {
    if (!impl) {
        return;
    }
    
    if (impl->checkpointManager) {
        impl->checkpointManager->update(currentStep, currentTime);
    }
}

void CheckpointManagement(Brain::Impl* impl,
                          SimulationStep currentStep,
                          Timestamp currentTime) {
    if (!impl) {
        NLM_LOG_ERROR("CheckpointManagement called with null impl pointer");
        return;
    }
    
    NLM_LOG_DEBUG("Checkpoint management (Step " + std::to_string(currentStep) + ")");
    
    if (impl->checkpointManager) {
        UpdateCheckpointManager(impl, currentStep, currentTime);
    }
}

} // namespace Step15

} // namespace nlm