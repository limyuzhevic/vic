// Brain Step Handler implementation
#include "BrainStepHandler.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

BrainStepHandler::BrainStepHandler(Brain* brain) : brain(brain), totalSpikesThisStep(0), totalSpikesTotal(0) {}

void BrainStepHandler::executeStep(SimulationStep currentStep, Timestamp currentTime) {
    totalSpikesThisStep = 0;
    
    // STEP 1: Process pending delayed spikes (deliver synaptic input)
    processPendingDelayedSpikes(currentStep, currentTime);
    
    // STEP 2: Update all neurons (LIF dynamics)
    updateAllNeurons(currentTime);
    
    // STEP 3: Detect spikes and schedule spike events
    detectSpikesAndScheduleEvents(currentTime, currentStep);
    
    // STEP 4: Process immediate spikes
    processSpikes(currentStep);
    
    // STEP 5: Update working memory
    updateWorkingMemory();
    
    // STEP 6: Apply neuromodulation effects
    applyNeuromodulationEffects();
    
    // STEP 7: Apply plasticity rules (STDP and Hebbian)
    float plasticityMod = 1.0f;
    if (brain->getDopamine()) {
        plasticityMod = brain->getDopamine()->getPlasticityFactor();
    }
    applyPlasticityRules(plasticityMod);
    
    // STEP 8: Update episodic memory with current experience
    updateEpisodicMemory(currentStep);
    
    // STEP 9: Update prediction system
    updatePredictionSystem();
    
    // STEP 10: Update attention system
    updateAttentionSystem();
    
    // STEP 11: Update concept formation
    updateConceptFormation();
    
    // STEP 12: Apply structural plasticity (synaptogenesis, pruning)
    applyStructuralPlasticity(currentStep);
    
    // STEP 13: Replay important memories (during rest or periodically)
    replayImportantMemories(currentStep);
    
    // STEP 14: Apply development effects
    applyDevelopmentEffects(currentStep);
    
    // STEP 15: Collect statistics
    collectStatistics();
    
    // STEP 16: Checkpoint management
    checkpointManagement(currentStep, currentTime);
}

void BrainStepHandler::processPendingDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    if (brain->getSpikeSystem()) {
        brain->getSpikeSystem()->processDelayedSpikes(currentStep, currentTime);
    }
}

void BrainStepHandler::updateAllNeurons(Timestamp currentTime) {
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
            }
        }
    }
}

void BrainStepHandler::detectSpikesAndScheduleEvents(Timestamp currentTime, SimulationStep currentStep) {
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < 0.002f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    if (brain->getSpikeSystem()) {
                        brain->getSpikeSystem()->queueSpike(event);
                    }

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
                        Timestamp deliveryTime = currentTime + delay * 0.001;

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

                        if (brain->getSpikeSystem()) {
                            brain->getSpikeSystem()->queueDelayedSpike(delayedEvent);
                        }

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (brain->getWorkingMemory()) {
                        brain->getWorkingMemory()->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void BrainStepHandler::processSpikes(SimulationStep currentStep) {
    if (brain->getSpikeSystem()) {
        brain->getSpikeSystem()->processSpikes(currentStep);
    }
}

void BrainStepHandler::updateWorkingMemory() {
    if (brain->getWorkingMemory()) {
        brain->getWorkingMemory()->update(brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
    }
}

void BrainStepHandler::applyNeuromodulationEffects() {
    // Update novelty detection
    if (brain->getNovelty()) {
        brain->getNovelty()->update(brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
    }

    // Update curiosity
    if (brain->getCuriosity()) {
        brain->getCuriosity()->update(brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
    }

    // Update dopamine (reward prediction error)
    if (brain->getDopamine()) {
        brain->getDopamine()->update(brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = brain->getDopamine()->getLevel();
        for (auto& region : brain->getRegions()) {
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

void BrainStepHandler::applyPlasticityRules(float plasticityMod) {
    for (auto& region : brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    if (brain->getSTDP()) {
                        brain->getSTDP()->update(syn, preSpikes, postSpikes, brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
                    }
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
                    if (brain->getHebbian()) {
                        brain->getHebbian()->update(syn, preSpikes, postSpikes, brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
                    }
                }
            }
            
            // Update synapse state
            syn->step(0.0f);
        }
    }
}

void BrainStepHandler::updateEpisodicMemory(SimulationStep currentStep) {
    if (brain->getStepsSinceLastEpisode() >= 10) {  // Store episode every 10 steps
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = brain->getDopamine() ? brain->getDopamine()->getLevel() : 0.0f;
        
        // Store active neurons
        for (auto& region : brain->getRegions()) {
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
        episode.reward = brain->getDopamine() ? brain->getDopamine()->getLevel() : 0.0f;
        
        if (brain->getEpisodicMemory()) {
            brain->getEpisodicMemory()->storeEpisode(episode);
        }
        
        // Reset episode counter
        brain->resetStepsSinceLastEpisode();
    } else {
        brain->incrementStepsSinceLastEpisode();
    }
}

void BrainStepHandler::updatePredictionSystem() {
    if (brain->getPredictionSystem()) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void BrainStepHandler::updateAttentionSystem() {
    if (brain->getAttention()) {
        brain->getAttention()->update(brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
        
        // Apply attention to working memory winners
        if (brain->getWorkingMemory() && !brain->getWorkingMemory()->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = brain->getWorkingMemory()->getMemoryNeurons();
            brain->getAttention()->processCompetition(competitors);
        }
    }
}

void BrainStepHandler::updateConceptFormation() {
    if (brain->getConceptFormation()) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void BrainStepHandler::applyStructuralPlasticity(SimulationStep currentStep) {
    if (currentStep % 100 == 0 && brain->getStructuralPlasticity()) {
        brain->getStructuralPlasticity()->update(brain, *brain->getRandomGenerator());
    }
}

void BrainStepHandler::replayImportantMemories(SimulationStep currentStep) {
    if (currentStep % brain->getReplayInterval() == 0 && brain->getEpisodicMemory()) {
        // Get episodes for replay
        auto episodesToReplay = brain->getEpisodicMemory()->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            brain->getEpisodicMemory()->replayEpisode(episode);
        }
    }
}

void BrainStepHandler::applyDevelopmentEffects(SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        if (brain->getDevelopmentSystem()) {
            brain->getDevelopmentSystem()->update(brain, *brain->getRandomGenerator(), 1.0);
            
            // Development affects plasticity rates
            if (brain->getStructuralPlasticity()) {
                DevelopmentalStage stage = brain->getDevelopmentalStage();
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
                
                brain->getStructuralPlasticity()->setSynaptogenesisRate(0.0001f * plasticityMod);
                brain->getStructuralPlasticity()->setPruningRate(0.00001f * (2.0f - plasticityMod));
            }
        }
    }
}

void BrainStepHandler::collectStatistics() {
    if (brain->getNovelty()) {
        float novelty = brain->getNovelty()->getLevel();
        if (novelty > 0.8f) {
            NLM_LOG_INFO("High novelty detected: " + std::to_string(novelty));
        }
    }
}

void BrainStepHandler::checkpointManagement(SimulationStep currentStep, Timestamp currentTime) {
    if (brain->getCheckpointManager()) {
        brain->getCheckpointManager()->update(currentStep, currentTime);
    }
}

} // namespace nlm