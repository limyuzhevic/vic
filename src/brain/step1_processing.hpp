// Step 1: Process pending delayed spikes (deliver synaptic input)
// This module processes delayed spike events to deliver synaptic input to destination neurons
// O(1) spike delivery using batch processing instead of O(n²) iteration

#pragma once

#include <memory>
#include <vector>
#include "../brain/NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

namespace Step1 {

// Helper function to deliver synaptic input from a spike to a neuron
void DeliverSynapticInput(Neuron* neuron, 
                          const DelayedSpikeEvent& event,
                          Timestamp currentTime,
                          float scaleFactor = 10.0f);

// Helper function to find neuron by ID across all regions
Neuron* FindNeuronById(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                      NeuronId neuronId);

// Process pending delayed spikes and deliver synaptic input
// Replaces O(n²) spike processing with O(m) where m is number of delayed spikes
void ProcessDelayedSpikes(Brain::Impl* impl, 
                          SimulationStep currentStep, 
                          Timestamp currentTime);

// Batch process spikes for better cache locality and performance
void BatchProcessSpikes(Brain::Impl* impl,
                        SimulationStep currentStep,
                        Timestamp currentTime);

} // namespace Step1

} // namespace nlm