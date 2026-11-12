// BrainStep.cpp - Implementation of Brain stepping handler
#include "BrainStep.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

BrainStep::BrainStep(Brain* brain) : brain_(brain) {
}

BrainStep::~BrainStep() = default;

void BrainStep::step(SimulationStep currentStep, Timestamp currentTime) {
    // Execute the main integrated brain loop
    executeMainLoop(currentStep, currentTime);
}

void BrainStep::executeMainLoop(SimulationStep currentStep, Timestamp currentTime) {
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
     * * 6b: Homeostatic synaptic scaling (Phase 4)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Collect statistics
     */
    
    brain_->setImplCurrentStep(currentStep);
    brain_->setImplCurrentTime(currentTime);
    brain_->setImplTotalSpikesThisStep(0);
    
    // STEP 1: Process pending delayed spikes (deliver synaptic input)
    processDelayedSpikes(currentStep, currentTime);
    
    // STEP 2: Update all neurons (LIF dynamics)
    updateNeurons(currentStep, currentTime);
    
    // STEP 3: Detect spikes and schedule spike events
    detectSpikes(currentStep, currentTime);
    
    // Process immediate spikes
    if (auto* spikeSystem = brain_->getSpikeSystem()) {
        spikeSystem->processSpikes(currentStep);
    }
    
    // STEP 4: Update working memory
    updateWorkingMemory(currentStep);
    
    // STEP 5: Apply neuromodulation effects
    applyNeuromodulationEffects();
    
    // STEP 6: Apply plasticity rules (STDP and Hebbian)
    applyPlasticityRules();
    
    // STEP 6b: Homeostatic synaptic scaling
    applyHomeostaticScaling(currentStep);
    
    // STEP 7: Update episodic memory
    updateEpisodicMemory(currentStep);
    
    // STEP 8: Update prediction system
    updatePredictionSystem();
    
    // STEP 9: Update attention system
    updateAttentionSystem();
    
    // STEP 10: Update concept formation
    updateConceptFormation();
    
    // STEP 11: Apply structural plasticity
    applyStructuralPlasticity(currentStep);
    
    // STEP 12: Replay important memories
    replayMemories(currentStep);
    
    // STEP 13: Apply development effects
    applyDevelopmentEffects(currentStep);
    
    // STEP 14: Periodic memory consolidation
    consolidateMemory(currentStep);
    
    // STEP 15: Checkpoint management
    updateCheckpointManager(currentStep, currentTime);
}

void BrainStep::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    if (auto* spikeSystem = brain_->getSpikeSystem()) {
        spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
}

void BrainStep::updateNeurons(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, brain_->getTimestep());
            }
        }
    }
}

void BrainStep::detectSpikes(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < brain_->getTimestep() * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    if (auto* spikeSystem = brain_->getSpikeSystem()) {
                        spikeSystem->queueSpike(event);
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
                        Timestamp deliveryTime = currentTime + delay * brain_->getTimestep();

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

                        if (auto* spikeSystem = brain_->getSpikeSystem()) {
                            spikeSystem->queueDelayedSpike(delayedEvent);
                        }

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                }
            }
        }
    }
}

void BrainStep::updateWorkingMemory(SimulationStep currentStep) {
    if (auto* workingMemory = brain_->getWorkingMemory()) {
        workingMemory->update(brain_->getTimestep());
    }
}

void BrainStep::applyNeuromodulationEffects() {
    // Update novelty detection
    if (auto* novelty = brain_->getNovelty()) {
        novelty->update(brain_->getTimestep());
    }
    
    // Update curiosity
    if (auto* curiosity = brain_->getCuriosity()) {
        float noveltyLevel = 0.0f;
        if (auto* novelty = brain_->getNovelty()) {
            noveltyLevel = novelty->getLevel();
        }
        float predictionError = 0.0f;
        if (auto* predictionErrorSignal = brain_->getPredictionErrorSignal()) {
            predictionError = predictionErrorSignal->getError();
        }
        curiosity->update(noveltyLevel, predictionError, brain_->getTimestep());
    }
    
    // Update dopamine (reward prediction error)
    if (auto* dopamine = brain_->getDopamine()) {
        dopamine->update(brain_->getTimestep());
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = dopamine->getLevel();
        for (auto& region : brain_->getRegions()) {
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

void BrainStep::applyPlasticityRules() {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (auto* dopamine = brain_->getDopamine()) {
        plasticityMod = dopamine->getPlasticityFactor();
    }
    
    for (auto& region : brain_->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    if (auto* stdp = brain_->getSTDP()) {
                        stdp->update(syn.get(), preSpikes, postSpikes, brain_->getTimestep());
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
                    if (auto* hebbian = brain_->getHebbian()) {
                        hebbian->update(syn.get(), preSpikes, postSpikes, brain_->getTimestep());
                    }
                }
            }
            
            // Update synapse state
            syn->step(brain_->getCurrentTime());
        }
    }
}

void BrainStep::applyHomeostaticScaling(SimulationStep currentStep) {
    if (currentStep > 0 && currentStep % 100 == 0) {
        constexpr float kIncomingExcitatoryCap = 3.0f;
        for (auto& region : brain_->getRegions()) {
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

void BrainStep::updateEpisodicMemory(SimulationStep currentStep) {
    brain_->incrementStepsSinceLastEpisode();
    if (brain_->getStepsSinceLastEpisode() >= 10) {
        brain_->resetStepsSinceLastEpisode();
        
        if (auto* episodicMemory = brain_->getEpisodicMemory()) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            
            // Store active neurons
            for (auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getMembranePotential() - neuron->getRestingPotential()) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getMembranePotential() - neuron->getRestingPotential()) / 20.0f);
                        }
                    }
                }
            }
            
            // Store reward in episode
            if (auto* dopamine = brain_->getDopamine()) {
                episode.reward = dopamine->getLevel();
            }
            
            episodicMemory->storeEpisode(episode);
        }
    }
}

void BrainStep::updatePredictionSystem() {
    if (auto* predictionSystem = brain_->getPredictionSystem()) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void BrainStep::updateAttentionSystem() {
    if (auto* attention = brain_->getAttention()) {
        attention->update(brain_->getTimestep());
        
        // Apply attention to working memory winners
        if (auto* workingMemory = brain_->getWorkingMemory() && 
            !workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
            attention->processCompetition(competitors);
        }
    }
}

void BrainStep::updateConceptFormation() {
    if (auto* conceptFormation = brain_->getConceptFormation()) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void BrainStep::applyStructuralPlasticity(SimulationStep currentStep) {
    if (currentStep % 100 == 0 && auto* structuralPlasticity = brain_->getStructuralPlasticity()) {
        structuralPlasticity->update(brain_, *brain_->getRandomGenerator());
    }
}

void BrainStep::replayMemories(SimulationStep currentStep) {
    if (auto* episodicMemory = brain_->getEpisodicMemory()) {
        // Get episodes for replay
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainStep::applyDevelopmentEffects(SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        if (auto* developmentSystem = brain_->getDevelopmentSystem()) {
            developmentSystem->update(brain_, *brain_->getRandomGenerator(), brain_->getTimestep() * 1000);
            
            // Development affects plasticity rates
            if (auto* sp = brain_->getStructuralPlasticity()) {
                DevelopmentalStage stage = brain_->getDevelopmentalStage();
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
}

void BrainStep::consolidateMemory(SimulationStep currentStep) {
    if (currentStep % 1000 == 0 && auto* episodicMemory = brain_->getEpisodicMemory()) {
        // Consolidate important memories, remove weak ones
        episodicMemory->consolidate(0.3f);
    }
}

void BrainStep::updateCheckpointManager(SimulationStep currentStep, Timestamp currentTime) {
    if (auto* checkpointManager = brain_->getCheckpointManager()) {
        checkpointManager->update(currentStep, currentTime);
    }
}

} // namespace nlm
