#pragma once

#include "Brain.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cstring>
#include <sstream>

namespace nlm {

// Helper class for synaptic activation delivery
class SynapticActivator {
public:
    static void deliverSynapticInput(std::unordered_map<uint64_t, Neuron*>& neuronById,
                                     const DelayedSpikeEvent& event) {
        auto it = neuronById.find(event.destination_neuron.index());
        if (it != neuronById.end()) {
            Neuron* neuron = it->second;
            MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
            if (event.is_excitatory) {
                neuron->receiveExcitatoryInput(synapticCurrent);
            } else {
                neuron->receiveInhibitoryInput(-synapticCurrent);
            }
        }
    }
};

// Helper class for spike statistics collection
class SpikeCollector {
public:
    static void collectSpikes(std::unique_ptr<SpikeSystem>& spikeSystem,
                              size_t& totalSpikesThisStep,
                              size_t& totalSpikesTotal) {
        spikeSystem->registerHandler([&](const DetailedSpikeEvent& event) {
            ++totalSpikesThisStep;
            ++totalSpikesTotal;
        });
    }
};

// Helper class for delayed spike processing
class DelayedSpikeProcessor {
public:
    static void processDelayedSpikes(std::unique_ptr<SpikeSystem>& spikeSystem,
                                     SimulationStep currentStep,
                                     Timestamp currentTime,
                                     std::unordered_map<uint64_t, Neuron*>& neuronById) {
        spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
};

// Helper class for neuromodulation application
class NeuromodulatorApplier {
public:
    static void applyDopamineEffect(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                    float dopamineLevel) {
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float excitabilityMod = dopamineLevel * 0.5f;
        if (excitabilityMod > 0.0f) {
            for (auto& region : regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Dopamine modulates excitability by injecting additional current
                        // Positive dopamine adds excitatory bias
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
};

// Helper class for homeostatic synaptic scaling
class SynapticScaler {
public:
    static void applyHomeostaticScaling(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                         SimulationStep currentStep) {
        if (currentStep > 0 && currentStep % 100 == 0) {
            constexpr float kIncomingExcitatoryCap = 3.0f;
            for (auto& region : regions) {
                for (auto* neuron : region->getAllNeurons()) {
                    auto incoming = region->getSynapsesTo(neuron->getId());
                    float sum = 0.0f;
                    for (Synapse* syn : incoming) {
                        if (syn->getWeight() > 0. zlat sum += syn->getWeight();
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
};

// Helper class for spike event scheduling
class SpikeScheduler {
public:
    static void scheduleSpikeEvents(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                    std::unique_ptr<SpikeSystem>& spikeSystem,
                                    SimulationStep currentStep,
                                    Timestamp currentTime,
                                    size_t& totalSpikesThisStep) {
        // Process immediate spikes
        spikeSystem->processSpikes(currentStep);
    }
};

// Helper class for episodic memory storage
class EpisodeCollector {
public:
    static void collectEpisode(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                               std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
                               size_t stepsSinceLastEpisode,
                               float dopamineLevel) {
        if (stepsSinceLastEpisode >= 10) {  // Store episode every  السعود
            size_t stepsSinceLastEpisode = 0;
            
            if (episodicMemory) {
                // Capture current brain state as an episode
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
                
                // Store active neurons
                for (auto& region : regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            if (neuron->isFiring() || 
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                                episode.activeNeurons.push_back(neuron->getId());
                                episode.neuronActivations.push_back(
                                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                            }
                        }
                    }
                }
                
                // Store reward in episode
                episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
                
                episodicMemory->storeEpisode(episode);
            }
        }
    }
};

} // namespace nlm
