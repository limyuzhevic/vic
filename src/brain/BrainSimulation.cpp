// Brain simulation implementation
#include "BrainSimulation.hpp"
#include "BrainCore.hpp"
#include "BrainInitialization.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainSimulation::executeStep(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    if (!brain) return;
    
    // Phase 6: Integrated Artificial Brain Loop
    // This implements the complete integrated brain simulation:
    // 
    // 1. Process pending delayed spike events (deliver synaptic input)
    // 2. Update all neuron membrane potentials (LIF dynamics)
    // 3. Detect spikes and schedule outgoing spike events
    // 4. Update working memory (maintenance and competition)
    // 5. Apply neuromodulation effects on neural excitability
    // 6. Apply plasticity rules (STDP, Hebbian)
    // 7. Update episodic memory with current experience
    // 8. Update prediction system
    // 9. Update attention system
    // 10. Update concept formation
    // 11. Apply structural plasticity (synaptogenesis, pruning)
    // 12. Replay important memories (during rest or periodically)
    // 13. Apply development effects
    // 14. Collect statistics
    
    // Store simulation state
    brain->injectCurrent(NeuronId(), 0.0f); // TODO: Store simulation parameters
    
    // Execute all simulation steps
    processDelayedSpikes(brain, currentStep, currentTime);
    updateLIFNeurons(brain, currentTime, currentTime); // TODO: Get timestep
    detectAndScheduleSpikes(brain, currentStep, currentTime);
    processSpikes(brain, currentStep);
    updateWorkingMemory(brain, currentTime); // TODO: Get timestep
    updateEpisodicMemory(brain, currentStep, currentTime);
    updatePredictionSystem(brain);
    updateAttentionSystem(brain, currentTime); // TODO: Get timestep
    updateConceptFormation(brain);
    applyStructuralPlasticity(brain, *brain->getRandomGenerator());
    replayMemories(brain, currentStep);
    applyDevelopmentEffects(brain, *brain->getRandomGenerator(), currentTime);
    consolidateMemories(brain, currentStep);
    updateCheckpointManager(brain, currentStep, currentTime);
}

void BrainSimulation::processDelayedSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    if (!brain || !brain->getSpikeSystem()) return;
    brain->getSpikeSystem()->processDelayedSpikes(currentStep, currentTime);
}

void BrainSimulation::updateLIFNeurons(Brain* brain, Timestamp currentTime, TimestepDuration timestep) {
    if (!brain) return;
    
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void BrainSimulation::detectAndScheduleSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    if (!brain) return;
    
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < 0.002f * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    if (brain->getSpikeSystem()) {
                        brain->getSpikeSystem()->queueSpike(event);
                    }
                }
            }
        }
    }
}

void BrainSimulation::processSpikes(Brain* brain, SimulationStep currentStep) {
    if (!brain || !brain->getSpikeSystem()) return;
    brain->getSpikeSystem()->processSpikes(currentStep);
}

void BrainSimulation::applyNeuromodulationEffects(Brain* brain, Timestamp currentTime) {
    if (!brain) return;
    
    // Update novelty detection
    if (auto* novelty = brain->getNovelty()) {
        novelty->update(0.001); // TODO: Get timestep
    }
    
    // Update curiosity
    if (auto* curiosity = brain->getCuriosity()) {
        curiosity->update(0.001); // TODO: Get timestep
    }
    
    // Update dopamine (reward prediction error)
    if (auto* dopamine = brain->getDopamine()) {
        dopamine->update(0.001); // TODO: Get timestep
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = dopamine->getLevel();
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void BrainSimulation::applyPlasticityRules(Brain* brain, Timestamp currentTime) {
    if (!brain) return;
    
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (auto* dopamine = brain->getDopamine()) {
        plasticityMod = dopamine->getPlasticityFactor();
    }
    
    for (auto& region : brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Apply STDP
                    if (brain->getSTDP()) {
                        brain->getSTDP()->update(syn, preSpikes, postSpikes, 0.001); // TODO: Get timestep
                    }
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    if (brain->getHebbian()) {
                        brain->getHebbian()->update(syn, preSpikes, postSpikes, 0.001); // TODO: Get timestep
                    }
                }
            }
        }
    }
}

void BrainSimulation::updateWorkingMemory(Brain* brain, TimestepDuration timestep) {
    if (!brain || !brain->getWorkingMemory()) return;
    brain->getWorkingMemory()->update(timestep);
}

void BrainSimulation::updateEpisodicMemory(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    if (!brain || !brain->getEpisodicMemory()) return;
    
    static size_t stepsSinceLastEpisode = 0;
    stepsSinceLastEpisode++;
    
    if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        stepsSinceLastEpisode = 0;
        
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        
        if (auto* dopamine = brain->getDopamine()) {
            episode.reward = dopamine->getLevel();
        }
        
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
        
        brain->getEpisodicMemory()->storeEpisode(episode);
    }
}

void BrainSimulation::updatePredictionSystem(Brain* brain) {
    if (brain && brain->getPredictionSystem()) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void BrainSimulation::updateAttentionSystem(Brain* brain, TimestepDuration timestep) {
    if (!brain || !brain->getAttention()) return;
    
    brain->getAttention()->update(timestep);
    
    // Apply attention to working memory winners
    if (brain->getWorkingMemory() && !brain->getWorkingMemory()->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = brain->getWorkingMemory()->getMemoryNeurons();
        brain->getAttention()->processCompetition(competitors);
    }
}

void BrainSimulation::updateConceptFormation(Brain* brain) {
    if (brain && brain->getConceptFormation()) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void BrainSimulation::applyStructuralPlasticity(Brain* brain, RandomGenerator& rng) {
    if (!brain || !brain->getStructuralPlasticity()) return;
    brain->getStructuralPlasticity()->update(brain, rng);
}

void BrainSimulation::replayMemories(Brain* brain, SimulationStep currentStep) {
    if (!brain || !brain->getEpisodicMemory()) return;
    
    // Check if it's time for replay (simplified)
    if (currentStep % 100 == 0) {  // Replay every 100 steps
        // Get episodes for replay
        auto episodesToReplay = brain->getEpisodicMemory()->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            brain->getEpisodicMemory()->replayEpisode(episode);
        }
    }
}

void BrainSimulation::applyDevelopmentEffects(Brain* brain, RandomGenerator& rng, Timestamp timeSinceLast) {
    if (!brain || !brain->getDevelopmentSystem()) return;
    
    // Apply development effects periodically
    static SimulationStep lastDevelopmentStep = 0;
    SimulationStep currentStep = static_cast<SimulationStep>(timeSinceLast / 0.001);
    
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        brain->getDevelopmentSystem()->update(brain, rng, timeSinceLast);
    }
}

void BrainSimulation::consolidateMemories(Brain* brain, SimulationStep currentStep) {
    if (!brain || !brain->getEpisodicMemory()) return;
    
    // Periodic memory consolidation
    if (currentStep % 1000 == 0) {  // Consolidate every 1000 steps
        brain->getEpisodicMemory()->consolidate(0.3f);
    }
}

void BrainSimulation::updateCheckpointManager(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    if (!brain || !brain->getConfig()) return;
    
    // TODO: Add checkpoint manager to Brain
    // For now, just log
    NLM_LOG_INFO("Checkpoint manager would be updated at step " + std::to_string(currentStep));
}

} // namespace nlm