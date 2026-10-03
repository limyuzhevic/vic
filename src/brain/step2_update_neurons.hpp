// Step 2: Update all neurons (LIF dynamics)
// This module updates all neuron membrane potentials using Leaky Integrate-and-Fire dynamics
// Provides optimized neuron stepping with null pointer validation and error handling

#pragma once

#include <memory>
#include <vector>
#include "../brain/NeuralRegion.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

namespace Step2 {

// Helper function to safely step a single neuron
// Applies null checks and handles edge cases
bool StepNeuron(Neuron* neuron, 
                Timestamp currentTime, 
                TimestepDuration timestep,
                size_t neuronIndex = 0);

// Helper function to update neuron population efficiently
// Optimized for cache locality and batch processing
void UpdateNeuronPopulation(const std::vector<Neuron*>& neurons,
                            Timestamp currentTime,
                            TimestepDuration timestep,
                            size_t startIndex = 0,
                            size_t count = 0);

// Update all neurons membrane potentials across all regions
// Replaces nested loops with optimized processing
void UpdateNeurons(Brain::Impl* impl,
                   SimulationStep currentStep,
                   Timestamp currentTime);

// Optimized neuron stepping using vectorization where possible
// Improves performance over individual neuron updates
void OptimizedNeuronStepping(Brain::Impl* impl,
                             SimulationStep currentStep,
                             Timestamp currentTime);

} // namespace Step2

} // namespace nlm