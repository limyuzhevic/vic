// Step 8: Update prediction system
// This module updates the prediction system with current sensory observations
// Computes prediction errors and updates internal models

#pragma once

#include <memory>
#include <vector>
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

namespace Step8 {

// Helper function to update prediction system with sensory input
void UpdatePredictionSystemWithInput(Brain::Impl* impl, const class SensoryInput& input);

// Helper function to process prediction errors
void ProcessPredictionErrors(Brain::Impl* impl, Timestamp currentTime);

// Update prediction system
void UpdatePredictionSystem(Brain::Impl* impl,
                           SimulationStep currentStep,
                           Timestamp currentTime);

} // namespace Step8

} // namespace nlm