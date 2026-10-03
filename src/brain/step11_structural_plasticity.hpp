// Step 11: Apply structural plasticity
// This module applies structural plasticity rules (synaptogenesis, pruning)
// Modifies neural connectivity based on activity patterns

#pragma once

#include <memory>
#include <vector>
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

namespace Step11 {

// Helper function to apply developmental effects on plasticity rates
void ApplyDevelopmentalPlasticityModulation(Brain::Impl* impl,
                                            DevelopmentalStage stage);

// Helper function to apply structural plasticity updates
void ApplyStructuralPlasticityUpdates(Brain::Impl* impl,
                                     SimulationStep currentStep,
                                     TimestepDuration timestep);

// Apply structural plasticity (synaptogenesis, pruning)
void ApplyStructuralPlasticity(Brain::Impl* impl,
                               SimulationStep currentStep,
                               Timestamp currentTime);

} // namespace Step11

} // namespace nlm