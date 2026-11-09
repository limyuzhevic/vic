#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <sstream>

namespace nlm {

// BrainUtils implementation - Common utilities and helper functions extracted from Brain.cpp

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
                      std::vector<Neuron*>& motorNeurons) {
    for (const auto& region : regions) {
        for (auto* neuron : region->getAllNeurons()) {
            neuronById[neuron->getId().index()] = neuron;
        }
    }
    
    // Collect sensory and motor neurons from first region for I/O
    for (const auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type == NeuronType::Sensory) {
                for (auto* neuron : pop->getNeurons()) {
                    sensoryNeurons.push_back(neuron);
                }
            } else if (type == NeuronType::Motor) {
                for (auto* neuron : pop->getNeurons()) {
                    motorNeurons.push_back(neuron);
                }
            }
        }
    }
}

// Helper function to initialize random connectivity
void initializeRegionConnectivity(NeuralRegion* region, RandomGenerator& rng,
                                 float connectionProbability, float excitatoryWeight,
                                 float inhibitoryWeight) {
    if (region) {
        region->initializeRandomConnectivity(rng, connectionProbability, excitatoryWeight, inhibitoryWeight);
    }
}

// Helper function to log brain statistics
void logBrainStats(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                  size_t totalSpikesTotal, size_t totalSpikesThisStep,
                  size_t regionsCount, float timestep) {
    NLM_LOG_INFO("Brain Statistics:");
    NLM_LOG_INFO("  Regions: " + std::to_string(regionsCount));
    NLM_LOG_INFO("  Total neurons: " + std::to_string(getTotalNeuronCountFromRegions(regions)));
    NLM_LOG_INFO("  Total synapses: " + std::to_string(getTotalSynapseCountFromRegions(regions)));
    NLM_LOG_INFO("  Total spikes (episode): " + std::to_string(totalSpikesTotal));
    NLM_LOG_INFO("  Spikes this step: " + std::to_string(totalSpikesThisStep));
    NLM_LOG_INFO("  Simulation timestep: " + std::to_string(timestep));
}

// Helper function to get total neuron count from regions
size_t getTotalNeuronCountFromRegions(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

// Helper function to get total synapse count from regions
size_t getTotalSynapseCountFromRegions(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getSynapseCount();
    }
    return total;
}

// Helper function to check if neuron just fired
bool neuronJustFired(const Neuron* neuron, Timestamp currentTime, Timestamp timestep) {
    const auto& state = neuron->getState();
    bool justFired = (state.firingState == FiringState::Refractory &&
                     state.lastSpikeTime >= 0.0f &&
                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);
    return justFired;
}

// Helper function to create delayed spike event
DelayedSpikeEvent createDelayedSpikeEvent(const Neuron* neuron, const Synapse* synapse,
                                         Timestamp currentTime, SimulationStep currentStep,
                                         Timestamp timestep) {
    Delay delay = synapse->getDelay();
    SimulationStep deliveryStep = currentStep + delay;
    Timestamp deliveryTime = currentTime + delay * timestep;
    
    return DelayedSpikeEvent(
        neuron->getId(),
        synapse->getDestinationNeuron(),
        synapse->getId(),
        synapse->getWeight(),
        synapse->getType(),
        currentTime,
        deliveryTime,
        currentStep,
        deliveryStep
    );
}

// Helper function to compute spike statistics
void computeSpikeStatistics(size_t& totalSpikesThisStep, size_t& totalSpikesTotal,
                          std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    size_t stepSpikes = 0;
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                if (state.firingState == FiringState::Refractory && state.lastSpikeTime >= 0.0f) {
                    // Simple check - could be enhanced
                    stepSpikes++;
                }
            }
        }
    }
    totalSpikesThisStep = stepSpikes;
    totalSpikesTotal += stepSpikes;
}

// Helper function for homeostatic synaptic scaling
void applyHomeostaticScaling(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                           SimulationStep currentStep, Timestamp timestep) {
    if (currentStep > 0 && currentStep % 100 == 0) {
        constexpr float kIncomingExcitatoryCap = 3.0f;
        for (auto& region : regions) {
            for (auto* neuron : region->getAllNeurons()) {
                auto incoming = region->getSynapsesTo(neuron->getId());
                float sum = 0.0f;
                for (Synapse* syn : incoming) {
                    if (syn->getWeight() > 0.0f) sum += syn->getWeight();
                }
                if (sum > kIncomingExcitatoryCap) {
                    float scale = kIncomingExcitatoryCap / sum;
                    for (Synapse* syn : incoming) {
                        if (syn->getWeight() > 0.0f) {
                            syn->setWeight(syn->getWeight() * scale);
                        }
                    }
                }
            }
        }
    }
}

// Helper function to log action production
std::unique_ptr<Action> produceAction(const std::vector<Neuron*>& motorNeurons) {
    if (motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    size_t firingMotor = 0;
    for (auto* neuron : motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    return std::make_unique<Action>(type);
}

// Helper function to log checkpoint status
void logCheckpointStatus(uint64_t currentStep, double currentTime,
                        const std::unique_ptr<CheckpointManager>& checkpointManager) {
    if (checkpointManager) {
        checkpointManager->update(currentStep, currentTime);
    }
}

} // namespace BrainUtils

} // namespace nlm
