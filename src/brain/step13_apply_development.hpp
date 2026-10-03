// Step 13: Apply development effects
// This module applies developmental changes over time
// Manages developmental stage progression and maturation

#pragma once

#include <memory>
#include <vector>
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

namespace Step13 {

// Helper function to apply development effects every 1000 steps
void ApplyDevelopmentEffects(Brain::Impl* impl,
                             SimulationStep currentStep,
                             Timestamp currentTime,
                             Timestamp cumulativeTime);

// Helper function to update developmental stage
void UpdateDevelopmentalStage(Brain::Impl* impl,
                              SimulationStep currentStep,
                              Timestamp cumulativeTime);

// Apply development effects
void ApplyDevelopment(Brain::Impl* impl,
                      SimulationStep currentStep,
                      Timestamp currentTime);

} // namespace Step13

} // namespace nlm