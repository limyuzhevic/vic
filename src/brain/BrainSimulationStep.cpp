#include "BrainSimulationStep.hpp"
#include "../Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

BrainSimulationStep::BrainSimulationStep(std::shared_ptr<Config> config) 
    : config(config)
    , timestep(0.001)
    , developmentalStage(DevelopmentalStage::Initial)
{
    // Initialize random generator
    uint64_t seed = 42;  // Default seed
    if (auto seedOpt = config->get<uint64_t>("random_seed")) {
        seed = *seedOpt;
    }
    rng = std::make_unique<RandomGenerator>(seed);
}

void BrainSimulationStep::executeStep(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    // Thread safety is handled by BrainStepWrapper
    executePhase6Integration(brain, currentStep, currentTime);
}

void BrainSimulationStep::executePhase6Integration(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation:
     * 
     * 1. Process pending delayed spike events (deliver synaptic input)
     * 2. Update all neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing spike events
     * 4. Update working memory (maintenance and competition)
     * 5. Apply neuromodulation effects on neural excitability
     * 6. Apply plasticity rules (STDP, Hebbian)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Collect statistics
     */
    
    brain->pImpl->brainSimulationStep->setCurrentStep(currentStep);
    brain->pImpl->brainSimulationStep->setCurrentTime(currentTime);
    brain->pImpl->brainSimulationStep->resetStepStatistics();
    
    // Get component references for convenience
    auto& regions = brain->pImpl->brainInitializer->getRegions();
    auto* spikeSystem = brain->pImpl->brainInitializer->getSpikeSystem();
    auto* workingMemory = brain->pImpl->brainInitializer->getWorkingMemory();
    auto* novelropy = brain->pImpl->brainInitializer->getNovelty();
    auto* curiosity = brain->pImpl->brainInitializer->getCuriosity();
    auto* dopamine = brain->pImpl->brainInitializer->getDopamine();
    auto* episodicMemory = brain->pImpl->brainInitializer->getEpisodicMemory();
    auto* attention = brain->pImpl->brainInitializer->getAttention();
    auto* conceptFormation = brain->pImpl->brainInitializer->getConceptFormation();
    auto* structuralPlasticity = brain->pImpl->brainInitializer->getStructuralPlasticity();
    
    // STEP 1: Process pending delayed spikes (deliver synaptic input)
    processDelayedSpikes(brain, currentStep, currentTime);
    
    // STEP 2: Update all neurons (LIF dynamics)
    updateNeurons(brain, currentTime, timestep);
    
    // STEP 3: Detect spikes and schedule spike events
    detectAndQueueSpikes(brain, currentStep, currentTime);
    
    // Process immediate spikes
    processImmediateSpikes(brain, currentStep);
    
    // STEP 4: Update working memory
    updateWorkingMemory(brain, timestep);
    
    // STEP 5: Apply neuromodulation effects
    applyNeuromodulation(brain, timestep);
    
    // STEP 6: Apply plasticity rules (STDP and Hebbian)
    applyPlasticityRules(brain, currentTime);
    
    // STEP 7: Update episodic memory
    updateEpisodicMemory(brain, currentStep, currentTime);
    
    // STEP 8: Update prediction system
    updatePredictionSystem(brain);
    
    // STEP 9: Update attention system
    updateAttentionSystem(brain, timestep);
    
    // STEP 10: Update concept formation
    updateConceptFormation(brain);
    
    // STEP 11: Apply structural plasticity periodically
    applyStructuralPlasticity(brain, currentStep);
    
    // STEP 12: Replay important memories
    replayMemories(brain, currentStep);
    
    // STEP 13: Apply development effects
    applyDevelopmentEffects(brain, currentStep);
    
    // STEP 14: Periodic memory consolidation
    consolidateMemories(brain, currentStep);
    
    // STEP 15: Checkpoint management
    updateCheckpoint(brain, currentStep, currentTime);
}

void BrainSimulationStep::processDelayedSpikes(Brain* brain, SimulationStep step, Timestamp time) {
    auto* spikeSystem = brain->pImpl->brainInitializer->getSpikeSystem();
    if (spikeSystem) {
        spikeSystem->processDelayedSpikes(step, time);
    }
}

void BrainSimulationStep::updateNeurons(Brain* brain, Timestamp time, TimestepDuration dt) {
    auto& regions = brain->pImpl->brainInitializer->getRegions();
    
    // Update all neurons (LIF dynamics)
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(time, dt);
            }
        }
    }
}

void BrainSimulationStep::detectAndQueueSpikes(Brain* brain, SimulationStep step, Timestamp time) {
    auto& regions = brain->pImpl->brainInitializer->getRegions();
    auto* spikeSystem = brain->pImpl->brainInitializer->getSpikeSystem();
    
    if (!spikeSystem) return;
    
    // Detect spikes and schedule spike events
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(time) - state.lastSpikeTime) < timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), time, step);
                    spikeSystem->queueSpike(event);
                    
                    // Count spikes
                    brain->pImpl->brainSimulationStep->incrementStepStatistics();
                    brain->pImpl->brainSimulationStep->incrementTotalSpikesThisStep();
                    
                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(time);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = step + delay;
                        Timestamp deliveryTime = time + delay * timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            time,
                            deliveryTime,
                            step,
                            deliveryStep
                        );

                        spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(time);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    auto* workingMemory = brain->pImpl->brainInitializer->getWorkingMemory();
                    if (workingMemory) {
                        workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void BrainSimulationStep::processImmediateSpikes(Brain* brain, SimulationStep step) {
    auto* spikeSystem = brain->pImpl->brainInitializer->getSpikeSystem();
    if (spikeSystem) {
        spikeSystem->processSpikes(step);
    }
}

void BrainSimulationStep::updateWorkingMemory(Brain* brain, TimestepDuration dt) {
    auto* workingMemory = brain->pImpl->brainInitializer->getWorkingMemory();
    if (workingMemory) {
        workingMemory->update(dt);
    }
}

void BrainSimulationStep::applyNeuromodulation(Brain* brain, TimestepDuration dt) {
    // Update novelty detection
    auto* novelty = brain->pImpl->brainInitializer->getNovelty();
    if (novelty) {
        novelty->update(dt);
    }
    
    // Update curiosity
    auto* curiosity = brain->pImpl->brainInitializer->getCuriosity();
    if (curiosity) {
        curiosity->update(dt);
    }
    
    // Update dopamine (reward prediction error)
    auto* dopamine = brain->pImpl->brainInitializer->getDopamine();
    if (dopamine) {
        dopamine->update(dt);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = dopamine->getLevel();
        for (auto& region : brain->pImpl->brainInitializer->getRegions()) {
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

void BrainSimulationStep::applyPlasticityRules(Brain* brain, Timestamp time) {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    auto* dopamine = brain->pImpl->brainInitializer->getDopamine();
    if (dopamine) {
        plasticityMod = dopamine->getPlasticityFactor();
    }
    
    auto& regions = brain->pImpl->brainInitializer->getRegions();
    auto* stdp = brain->pImpl->brainInitializer->getSTDP();
    auto* hebbian = brain->pImpl->brainInitializer->getHebbian();
    
    for (auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    stdp->update(syn, preSpikes, postSpikes, timestep);
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
                    hebbian->update(syn, preSpikes, postSpikes, timestep);
                }
            }
            
            // Update synapse state
            syn->step(time);
        }
    }
}

void BrainSimulationStep::updateEpisodicMemory(Brain* brain, SimulationStep step, Timestamp time) {
    auto* episodicMemory = brain->pImpl->brainInitializer->getEpisodicMemory();
    if (!episodicMemory) return;
    
    auto& stepsSinceLastEpisode = brain->pImpl->brainSimulationStep->getStepsSinceLastEpisode();
    auto& replayInterval = brain->pImpl->brainSimulationStep->getReplayInterval();
    
    stepsSinceLastEpisode++;
    if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        stepsSinceLastEpisode = 0;
        
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = step;
        episode.reward = brain->pImpl->brainInitializer->getDopamine() ? 
                        brain->pImpl->brainInitializer->getDopamine()->getLevel() : 0.0f;
        
        // Store active neurons
        auto& regions = brain->pImpl->brainInitializer->getRegions();
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
        episode.reward = brain->pImpl->brainInitializer->getDopamine() ? 
                        brain->pImpl->brainInitializer->getDopamine()->getLevel() : 0.0f;
        
        episodicMemory->storeEpisode(episode);
    }
}

void BrainSimulationStep::updatePredictionSystem(Brain* brain) {
    auto* predictionSystem = brain->pImpl->brainInitializer->getPredictionSystem();
    if (predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void BrainSimulationStep::updateAttentionSystem(Brain* brain, TimestepDuration dt) {
    auto* attention = brain->pImpl->brainInitializer->getAttention();
    if (attention) {
        attention->update(dt);
        
        // Apply attention to working memory winners
        auto* workingMemory = brain->pImpl->brainInitializer->getWorkingMemory();
        if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
            attention->processCompetition(competitors);
        }
    }
}

void BrainSimulationStep::updateConceptFormation(Brain* brain) {
    auto* conceptFormation = brain->pImpl->brainInitializer->getConceptFormation();
    if (conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void BrainSimulationStep::applyStructuralPlasticity(Brain* brain, SimulationStep step) {
    auto* structuralPlasticity = brain->pImpl->brainInitializer->getStructuralPlasticity();
    if (structuralPlasticity) {
        structuralPlasticity->update(brain, *brain->pImpl->brainInitializer->getRandomGenerator());
    }
}

void BrainSimulationStep::replayMemories(Brain* brain, SimulationStep step) {
    auto* episodicMemory = brain->pImpl->brainInitializer->getEpisodicMemory();
    if (!episodicMemory) return;
    
    auto& replayInterval = brain->pImpl->brainSimulationStep->getReplayInterval();
    if (step % replayInterval == 0) {
        // Get episodes for replay
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainSimulationStep::applyDevelopmentEffects(Brain* brain, SimulationStep step) {
    auto* developmentSystem = brain->pImpl->brainInitializer->getDevelopmentSystem();
    auto* structuralPlasticity = brain->pImpl->brainInitializer->getStructuralPlasticity();
    
    if (!developmentSystem || !structuralPlasticity) return;
    
    // Update development every 1000 steps
    if (step % 1000 == 0) {
        developmentSystem->update(brain, *brain->pImpl->brainInitializer->getRandomGenerator(), timestep * 1000);
        
        // Development affects plasticity rates
        DevelopmentStage stage = brain->pImpl->brainInitializer->getDevelopmentalStage();
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
        
        structuralPlasticity->setSynaptogenesisRate(0.0001f * plasticityMod);
        structuralPlasticity->setPruningRate(0.00001f * (2.0f - plasticityMod));
    }
}

void BrainSimulationStep::consolidateMemories(Brain* brain, SimulationStep step) {
    auto* episodicMemory = brain->pImpl->brainInitializer->getEpisodicMemory();
    if (!episodicMemory) return;
    
    auto& consolidationInterval = brain->pImpl->brainSimulationStep->getConsolidationInterval();
    if (step % consolidationInterval == 0) {
        // Consolidate important memories, remove weak ones
        episodicMemory->consolidate(0.3f);
    }
}

void BrainSimulationStep::updateCheckpoint(Brain* brain, SimulationStep step, Timestamp time) {
    auto* checkpointManager = brain->pImpl->brainInitializer->getCheckpointManager();
    if (checkpointManager) {
        checkpointManager->update(step, time);
    }
}

void BrainSimulationStep::resetStepStatistics() {
    // Reset step-specific statistics
}

void BrainSimulationStep::incrementStepStatistics() {
    // Increment step statistics
}

void BrainSimulationStep::processSensoryInput(Brain* brain, const class SensoryInput& input) {
    // Process sensory input using brainInitializer's sensory neurons
    auto& sensoryNeurons = brain->pImpl->brainInitializer->getSensoryNeurons();
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        if (sensoryNeurons[i]) {
            sensoryNeurons[i]->injectCurrent(normalizedValue);
            
            // Also store in working memory
            auto* workingMemory = brain->pImpl->brainInitializer->getWorkingMemory();
            if (workingMemory && normalizedValue > 0.5f) {
                workingMemory->storeToNeuron(sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
            }
        }
    }
}

void BrainSimulationStep::storeSensoryToWorkingMemory(Brain* brain, const class SensoryInput& input) {
    processSensoryInput(brain, input);
}

std::unique_ptr<class Action> BrainSimulationStep::generateAction(Brain* brain) {
    // Simple action selection based on motor neuron activity
    auto& motorNeurons = brain->pImpl->brainInitializer->getMotorNeurons();
    
    if (motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    return action;
}

void BrainSimulationStep::injectCurrentToNeuron(Brain* brain, NeuronId neuron, MembranePotential current) {
    brain->injectCurrent(neuron, current);
}

void BrainSimulationStep::injectCurrentToNeurons(Brain* brain, NeuronType type, MembranePotential current) {
    brain->injectCurrentToNeurons(type, current);
}

bool BrainSimulationStep::saveBrainState(Brain* brain, const std::string& filepath) const {
    return brain->save(filepath);
}

bool BrainSimulationStep::loadBrainState(Brain* brain, const std::string& filepath) {
    return brain->load(filepath);
}

void BrainSimulationStep::updateDevelopment(Brain* brain, SimulationStep step) {
    brain->develop();
}

void BrainSimulationStep::develop(Brain* brain) {
    brain->develop();
}

void BrainSimulationStep::resetBrain(Brain* brain) {
    brain->reset();
}