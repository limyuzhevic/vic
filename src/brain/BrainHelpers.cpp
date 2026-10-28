#include "BrainHelpers.hpp"
#include "core/Types/Types.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralPopulation.hpp"
#include "brain/NeuralRegion.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "performance/CheckpointManager.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

namespace nlm {

// Step helper methods
void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime, Brain::Impl* pImpl) {
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void updateNeuronDynamics(Brain::Impl* pImpl, Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
}

void detectAndScheduleSpikes(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    pImpl->spikeSystem->queueSpike(event);

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * pImpl->timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            currentTime,
                            deliveryTime,
                            currentStep,
                            deliveryStep
                        );

                        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void processImmediateSpikes(SimulationStep currentStep, Brain::Impl* pImpl) {
    pImpl->spikeSystem->processSpikes(currentStep);
}

void updateWorkingMemory(Brain::Impl* pImpl) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

void applyNeuromodulationEffects(Brain::Impl* pImpl) {
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }

    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }

    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = pImpl->dopamine->getLevel();
        applyDopamineEffects(pImpl, dopamineLevel);
    }
}

void applyDopamineEffects(Brain::Impl* pImpl, float dopamineLevel) {
    float excitabilityMod = dopamineLevel * 0.5f;
    if (excitabilityMod > 0.0f) {
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

void applyPlasticityRules(Brain::Impl* pImpl, float plasticityMod) {
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(pImpl->currentTime);
        }
    }
}

void updateEpisodicMemory(Brain::Impl* pImpl) {
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = pImpl->currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : pImpl->regions) {
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
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void updatePredictionSystem(Brain::Impl* pImpl) {
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void updateAttentionSystem(Brain::Impl* pImpl) {
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
}

void updateConceptFormation(Brain::Impl* pImpl) {
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void applyStructuralPlasticity(Brain::Impl* pImpl, SimulationStep currentStep) {
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(static_cast<Brain*>(nullptr), *pImpl->rng);
    }
}

void replayImportantMemories(Brain::Impl* pImpl, SimulationStep currentStep) {
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void applyDevelopmentEffects(Brain::Impl* pImpl, SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(static_cast<Brain*>(nullptr), *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = pImpl->developmentalStage;
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;  // High plasticity
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;  // Stable
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void consolidateMemories(Brain::Impl* pImpl, SimulationStep currentStep) {
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void checkpointManagement(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

// Neuron helper methods
void injectSensoryCurrent(Brain::Impl* pImpl, const class SensoryInput& input) {
    const auto& values = input.getData();
    if (values.empty()) return;

    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;

    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void injectCurrentToSpecificNeuron(Brain::Impl* pImpl, NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void injectCurrentToNeuronType(Brain::Impl* pImpl, NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

// Memory helper methods
void storeActiveNeuronsToEpisodicMemory(Brain::Impl* pImpl, SimulationStep currentStep, const EpisodicMemoryItem& episode) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->storeEpisode(episode);
    }
}

void processMemorySystems(Brain::Impl* pImpl) {
    // Process working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // Process episodic memory
    if (pImpl->episodicMemory) {
        // Periodic cleanup and consolidation could be done here
    }
}

// Spike helper methods
void registerSpikeHandlers(Brain::Impl* pImpl) {
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([pImpl](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
}

void registerDelayedSpikeHandler(Brain::Impl* pImpl) {
    // Register delayed spike handler to deliver synaptic input
    pImpl->spikeSystem->registerDelayedHandler([pImpl](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // Apply synaptic weight as current
                    MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                    if (event.is_excitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    return;
                }
            }
        }
    });
}

// Plasticity helper methods
void configureSTDP(Brain::Impl* pImpl) {
    float ltpWeight = pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = pImpl->config->getOr<float>("stdp_tau", 20.0f);
    pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
}

void configureStructuralPlasticity(Brain::Impl* pImpl) {
    float synaptogenesisRate = pImpl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = pImpl->config->getOr<float>("pruning_rate", 0.00001f);
    pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    pImpl->structuralPlasticity->setPruningRate(pruningRate);
}

// Statistics and logging helper methods
void logBrainStatus(Brain::Impl* pImpl) {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(pImpl->regions.size()));
    
    size_t totalNeurons = 0, totalSynapses = 0, activeNeurons = 0;
    for (const auto& region : pImpl->regions) {
        totalNeurons += region->getTotalNeuronCount();
        totalSynapses += region->getSynapseCount();
        activeNeurons += region->getActiveNeuronCount();
        printRegionStats(region.get(), region->getId().index());
    }
    
    NLM_LOG_INFO("Total neurons: " + std::to_string(totalNeurons));
    NLM_LOG_INFO("Total synapses: " + std::to_string(totalSynapses));
    NLM_LOG_INFO("Active neurons: " + std::to_string(activeNeurons));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(pImpl->totalSpikesThisStep));
    NLM_LOG_INFO("Total spikes: " + std::to_string(pImpl->totalSpikesTotal));
}

void printRegionStats(const NeuralRegion* region, int regionIndex) {
    NLM_LOG_INFO("  Region " + std::to_string(regionIndex) + 
                " (" + region->getName() + "): " +
                std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                std::to_string(region->getSynapseCount()) + " synapses, " +
                "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
}

// Development helper methods
DevelopmentStage calculateDevelopmentalStage(Brain::Impl* pImpl) {
    // Calculate developmental stage based on simulation progress
    if (pImpl->currentStep < 100) return DevelopmentalStage::Initial;
    if (pImpl->currentStep < 1000) return DevelopmentalStage::CriticalPeriod;
    if (pImpl->currentStep < 5000) return DevelopmentalStage::Maturation;
    return DevelopmentalStage::Adult;
}

} // namespace nlm
