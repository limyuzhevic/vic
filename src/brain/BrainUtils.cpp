#include "BrainUtils.hpp"

namespace nlm {

void SynapticActivator::deliverSynapticInput(std::unordered_map<uint64_t, Neuron*>& neuronById,
                                             const DelayedSpikeEvent& event) {
    // Implementation in header for performance
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

void SpikeCollector::collectSpikes(std::unique_ptr<SpikeSystem>& spikeSystem,
                                   size_t& totalSpikesThisStep,
                                   size_t& totalSpikesTotal) {
    spikeSystem->registerHandler([&](const DetailedSpikeEvent& event) {
        ++totalSpikesThisStep;
        ++totalSpikesTotal;
    });
}

void DelayedSpikeProcessor::processDelayedSpikes(std::unique_ptr<SpikeSystem>& spikeSystem,
                                                 SimulationStep currentStep,
                                                 Timestamp currentTime,
                                                 std::unordered_map<uint64_t, Neuron*>& neuronById) {
    spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void NeuromodulatorApplier::applyDopamineEffect(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                                  float dopamineLevel) {
    float excitabilityMod = dopamineLevel * 0.5f;
    if (excitabilityMod > 0.0f) {
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

void SynapticScaler::applyHomeostaticScaling(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                              SimulationStep currentStep) {
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

void SpikeScheduler::scheduleSpikeEvents(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                           std::unique_ptr<SpikeSystem>& spikeSystem,
                                           SimulationStep currentStep,
                                           Timestamp currentTime) {
    // Process immediate spikes
    spikeSystem->processSpikes(currentStep);
}

void EpisodeCollector::collectEpisode(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                      std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
                                      size_t stepsSinceLastEpisode,
                                      float dopamineLevel) {
    if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        if (episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = dopamineLevel;
            
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
            episode.reward = dopamineLevel;
            
            episodicMemory->storeEpisode(episode);
        }
    }
}

} // namespace nlm
