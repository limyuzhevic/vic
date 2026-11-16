// Brain Core Step Implementation
// Refactored from Brain.cpp step loop and related methods

#include "BrainCoreStep.hpp"
#include "../brain/Brain.hpp"
#include "../brain/BrainInitialization.hpp"
#include "../brain/BrainIntegration.hpp"
#include "../brain/BrainHelpers.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Step sub-implementation methods
void BrainCoreStep::stepSubstep1ProcessDelayedSpikes(Brain::Impl& pImpl, SimulationStep currentStep, Timestamp currentTime) {
    // STEP 1: Process pending delayed spikes (deliver synaptic input)
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void BrainCoreStep::stepSubstep2UpdateNeurons(Brain::Impl& pImpl, Timestamp currentTime, TimestepDuration timestep) {
    // STEP 2: Update all neurons (LIF dynamics)
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void BrainCoreStep::stepSubstep3DetectSpikes(Brain::Impl& pImpl, SimulationStep currentStep, Timestamp currentTime) {
    // STEP 3: Detect spikes and schedule outgoing spike events
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
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
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 1.0f);
                    }
                }
            }
        }
    }
}

void BrainCoreStep::stepSubstep4ProcessSpikes(Brain::Impl& pImpl, SimulationStep currentStep) {
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
}

void BrainCoreStep::stepSubstep5UpdateWorkingMemory(Brain::Impl& pImpl, TimestepDuration timestep) {
    // STEP 4: Update working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(timestep);
    }
}

void BrainCoreStep::stepSubstep6ApplyNeuromodulation(Brain::Impl& pImpl, TimestepDuration timestep) {
    // STEP 5: Apply neuromodulation effects
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(
            pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f,
            pImpl->predictionError ? pImpl->predictionError->getError() : 0.0f,
            timestep
        );
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void BrainCoreStep::stepSubstep7ApplyPlasticity(Brain::Impl& pImpl, 
                                                SimulationStep currentStep,
                                                Timestamp currentTime,
                                                TimestepDuration timestep) {
    // STEP 6: Apply plasticity rules (STDP and Hebbian)
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    pImpl->stdp->update(syn.get(), preSpikes, postSpikes, timestep);
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
                    pImpl->hebbian->update(syn.get(), preSpikes, postSpikes, timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void BrainCoreStep::stepSubstep8HomeostaticScaling(Brain::Impl& pImpl, SimulationStep currentStep) {
    // STEP 6b: Homeostatic synaptic scaling (Phase 4)
    if (currentStep > 0 && currentStep % 100 == 0) {
        constexpr float kIncomingExcitatoryCap = 3.0f;
        for (auto& region : pImpl->regions) {
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

void BrainCoreStep::stepSubstep9UpdateEpisodicMemory(Brain::Impl& pImpl, SimulationStep currentStep) {
    // STEP 7: Update episodic memory
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
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
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void BrainCoreStep::stepSubstep10UpdatePredictionSystem(Brain::Impl& pImpl) {
    // STEP 8: Update prediction system
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void BrainCoreStep::stepSubstep11UpdateAttentionSystem(Brain::Impl& pImpl, TimestepDuration timestep) {
    // STEP 9: Update attention system
    if (pImpl->attention) {
        pImpl->attention->update(timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
}

void BrainCoreStep::stepSubstep12UpdateConceptFormation(Brain::Impl& pImpl) {
    // STEP 10: Update concept formation
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void BrainCoreStep::stepSubstep13ApplyStructuralPlasticity(Brain::Impl& pImpl, SimulationStep currentStep) {
    // STEP 11: Apply structural plasticity periodically
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(nullptr, *pImpl->rng);
    }
}

void BrainCoreStep::stepSubstep14ReplayMemories(Brain::Impl& pImpl, SimulationStep currentStep) {
    // STEP 12: Replay important memories
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainCoreStep::stepSubstep15ApplyDevelopment(Brain::Impl& pImpl, SimulationStep currentStep) {
    // STEP 13: Apply development effects
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(nullptr, *pImpl->rng, pImpl->timestep *  혼란ًا);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity.get();
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

void BrainCoreStep::stepSubstep16Consolidation(Brain::Impl& pImpl, SimulationStep currentStep) {
    // STEP 14: Periodic memory consolidation
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void BrainCoreStep::stepSubstep17Checkpoint(Brain::Impl& pImpl, SimulationStep currentStep, Timestamp currentTime) {
    // STEP 15: Checkpoint management
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void BrainCoreStep::executeCompleteStep(Brain::Impl& pImpl, 
                                       SimulationStep currentStep,
                                       Timestamp currentTime,
                                       TimestepDuration timestep) {
    // Complete step execution with all subtasks
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Execute all step subtasks
    stepSubstep1ProcessDelayedSpikes(pImpl, currentStep, currentTime);
    stepSubstep2UpdateNeurons(pImpl, currentTime, timestep);
    stepSubstep3DetectSpikes(pImpl, currentStep, currentTime);
    stepSubstep4ProcessSpikes(pImpl, currentStep);
    stepSubstep5UpdateWorkingMemory(pImpl, timestep);
    stepSubstep6ApplyNeuromodulation(pImpl, timestep);
    stepSubstep7ApplyPlasticity(pImpl, currentStep, currentTime, timestep);
    stepSubstep8HomeostaticScaling(pImpl, currentStep);
    stepSubstep9UpdateEpisodicMemory(pImpl, currentStep);
    stepSubstep10UpdatePredictionSystem(pImpl);
    stepSubstep11UpdateAttentionSystem(pImpl, timestep);
    stepSubstep12UpdateConceptFormation(pImpl);
    stepSubstep13ApplyStructuralPlasticity(pImpl, currentStep);
    stepSubstep14ReplayMemories(pImpl, currentStep);
    stepSubstep15ApplyDevelopment(pImpl, currentStep);
    stepSubstep16Consolidation(pImpl, currentStep);
    stepSubstep17Checkpoint(pImpl, currentStep, currentTime);
}

} // namespace nlm
