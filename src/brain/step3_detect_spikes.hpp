// Step 3: Detect spikes and schedule spike events
// This module detects spiking neurons and creates spike events for propagation
// Optimized spike detection with reduced search complexity

#pragma once

#include <memory>
#include <vector>
#include "../brain/NeuralRegion.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

namespace Step3 {

// Helper function to check if neuron fired in the current timestep
bool DetectNeuronSpike(Neuron* neuron,
                      Timestamp currentTime,
                      SimulationStep currentStep,
                      TimestepDuration timestep);

// Helper function to create spike event from neuron
void CreateSpikeEvent(Brain::Impl* impl,
                      Neuron* neuron,
                      Timestamp currentTime,
                      SimulationStep currentStep);

// Helper function to process synaptic plasticity on pre-synaptic neurons
void ProcessPreSynapticPlasticity(NeuralRegion* region,
                                  NeuronId neuronId,
                                  Timestamp currentTime);

// Helper function to create and queue delayed spike events
void ScheduleDelayedSpikes(Brain::Impl* impl,
                           Neuron* neuron,
                           Timestamp currentTime,
                           SimulationStep currentStep);

// Detect spikes and schedule outgoing spike events
void DetectAndScheduleSpikes(Brain::Impl* impl,
                             SimulationStep currentStep,
                             Timestamp currentTime);

// Optimized spike detection with batch processing
void OptimizedSpikeDetection(Brain::Impl* impl,
                             SimulationStep currentStep,
                             Timestamp currentTime);

} // namespace Step3

} // namespace nlm