// Step 5: Apply neuromodulation effects
// This module applies neuromodulatory signals to brain regions
// Implements dopamine, curiosity, novelty, and prediction error effects

#pragma once

#include <memory>
#include <vector>
#include "../brain/NeuralRegion.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

namespace Step5 {

// Helper function to apply dopamine effects on neural excitability
void ApplyDopamineEffects(Brain::Impl* impl, float dopamineLevel);

// Helper function to apply neuromodulation to neurons
void ApplyNeuromodulationToNeurons(const std::vector<Neuron*>& neurons, float modulationFactor);

// Apply neuromodulation effects on neural excitability and plasticity
void ApplyNeuromodulation(Brain::Impl* impl,
                          SimulationStep currentStep,
                          Timestamp currentTime);

// Update novelty detection and apply novelty effects
void UpdateNoveltyDetection(Brain::Impl* impl, TimestepDuration timestep);

// Update curiosity and apply curiosity effects
void UpdateCuriosity(Brain::Impl* impl, TimestepDuration timestep);

} // namespace Step5

} // namespace nlm