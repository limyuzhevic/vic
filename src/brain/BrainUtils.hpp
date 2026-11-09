#pragma once

#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include "../core/Types/Types.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {
namespace BrainUtils {

// Helper function to create unique neuron name
define uniqueNeuronName(size_t regionIndex, size_t neuronIndex) {
    return "Region_" + std::to_string(regionIndex) + 
           "_Neuron_" + std::to_string(neuronIndex);
}

// Helper function to calculate network statistics
define calculateExcitatoryInhibitionRatio(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

// Helper function to build neuron lookup map
void buildNeuronLookup(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                      std::unordered_map<uint64_t, Neuron*>& neuronById,
                      std::vector<Neuron*>& sensoryNeurons,
                      std::vector<Neuron*>& motorNeurons);

// Helper function to initialize random connectivity
void initializeRegionConnectivity(NeuralRegion* region, RandomGenerator& rng,
                                 float connectionProbability, float excitatoryWeight,
                                 float inhibitoryWeight);

// Helper function to log brain statistics
void logBrainStats(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                 size_t totalSpikesTotal, size_t totalSpikesThisStep,
                 size_t regionsCount, float timestep);

// Helper function to get total neuron count from regions
size_t getTotalNeuronCountFromRegions(const std::vector<std::unique_ptr<NeuralRegion>>& regions);

// Helper function to get total synapse count from regions
size_t getTotalSynapseCountFromRegions(const std::vector<std::unique_ptr<NeuralRegion>>& regions);

// Helper function to check if neuron just fired
bool neuronJustFired(const Neuron* neuron, Timestamp currentTime, Timestamp timestep);

// Helper function to create delayed spike event
DelayedSpikeEvent createDelayedSpikeEvent(const Neuron* neuron, const Synapse* synapse,
                                         Timestamp currentTime, SimulationStep currentStep,
                                         Timestamp timestep);

// Helper function to compute spike statistics
void computeSpikeStatistics(size_t& totalSpikesThisStep, size_t& totalSpikesTotal,
                          std::vector<std::unique_ptr<NeuralRegion>>& regions);

// Helper function for homeostatic synaptic scaling
void applyHomeostaticScaling(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                           SimulationStep currentStep, Timestamp timestep);

// Helper function to log action production
std::unique_ptr<Action> produceAction(const std::vector<Neuron*>& motorNeurons);

// Helper function to log checkpoint status
void logCheckpointStatus(uint64_t currentStep, double currentTime,
                        const std::unique_ptr<CheckpointManager>& checkpointManager);

} // namespace BrainUtils

} // namespace nlm
