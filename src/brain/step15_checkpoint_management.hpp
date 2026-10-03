// Step 15: Checkpoint management
// This module manages checkpoint saving and loading
// Provides persistence for brain state

#pragma once

#include <memory>
#include <vector>
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

namespace Step15 {

// Helper function to configure checkpoint manager
void ConfigureCheckpointManager(Brain::Impl* impl,
                                const std::string& checkpointDir,
                                size_t maxCheckpoints,
                                size_t maxCheckpointSize,
                                bool compressionEnabled);

// Helper function to update checkpoint manager
void UpdateCheckpointManager(Brain::Impl* impl,
                             SimulationStep currentStep,
                             Timestamp currentTime);

// Checkpoint management
void CheckpointManagement(Brain::Impl* impl,
                          SimulationStep currentStep,
                          Timestamp currentTime);

} // namespace Step15

} // namespace nlm