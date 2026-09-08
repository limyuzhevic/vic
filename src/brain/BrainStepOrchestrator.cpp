// BrainStepOrchestrator.cpp - Orchestrates the 15-step brain simulation loop
#include "BrainStepOrchestrator.hpp"
#include "Brain.hpp"
#include "../core/Types/Types.hpp"
#include "../neuron/Neuron.hpp"
#include <algorithm>

namespace nlm {

BrainStepOrchestrator::BrainStepOrchestrator(Brain* brain) : brain_(brain) {}

void BrainStepOrchestrator::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    brain_->getSpikeSystem()->processDelayedSpikes(currentStep, currentTime);
}

void BrainStepOrchestrator::updateNeurons(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, brain_->getSpikeSystem()->getTimestepDuration());
            }
        }
    }
}

void BrainStepOrchestrator::processSpikes(SimulationStep currentStep) {
    brain_->getSpikeSystem()->processSpikes(currentStep);
}

void BrainStepOrchestrator::updateWorkingMemory(TimestepDuration dt) {
    if (auto* workingMemory = brain_->getWorkingMemory()) {
        workingMemory->update(dt);
    }
}

void BrainStepOrchestrator::applyNeuromodulation() {
    // Update novelty detection
    if (auto* novelty = brain_->getNovelty()) {
        novelty->update(brain_->getSpikeSystem()->getTimestepDuration());
    }
    
    // Update curiosity
    if (auto* curiosity = brain_->getCuriosity()) {
        curiosity->update(brain_->getSpikeSystem()->getTimestepDuration());
    }
    
    // Update dopamine (reward prediction error)
    if (auto* dopamine = brain_->getDopamine()) {
        dopamine->update(brain_->getSpikeSystem()->getTimestepDuration());
        
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

void BrainStepOrchestrator::applyPlasticity(TimestepDuration dt) {
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
                    // Modify weight change based on dopamine
                    brain_->getSTDP()->update(syn, preSpikes, postSpikes, dt);
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
                    brain_->getHebbian()->update(syn, preSpikes, postSpikes, dt);
                }
            }
            
            // Update synapse state
            syn->step(dt);
        }
    }
}

void BrainStepOrchestrator::updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
    if (auto* episodicMemory = brain_->getEpisodicMemory()) {
        // Store episode every 10 steps
        if (currentStep % 10 == 0) {
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = (brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f);
            
            // Store active neurons
            for (auto& region : brain_->getRegions()) {
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
            episode.reward = (brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f);
            
            episodicMemory->storeEpisode(episode);
        }
    }
}

void BrainStepOrchestrator::updatePredictionSystem() {
    if (auto* predictionSystem = brain_->getPredictionSystem()) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void BrainStepOrchestrator::updateAttention(TimestepDuration dt) {
    if (auto* attention = brain_->getAttention()) {
        attention->update(dt);
        
        // Apply attention to working memory winners
        if (auto* workingMemory = brain_->getWorkingMemory()) {
            std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
            attention->processCompetition(competitors);
        }
    }
}

void BrainStepOrchestrator::updateConceptFormation() {
    if (auto* conceptFormation = brain_->getConceptFormation()) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void BrainStepOrchestrator::applyStructuralPlasticity(SimulationStep currentStep, RandomGenerator& rng) {
    if (currentStep % 100 == 0) {
        brain_->getStructuralPlasticity()->update(brain_, rng);
    }
}

void BrainStepOrchestrator::replayMemories(SimulationStep currentStep) {
    if (auto* episodicMemory = brain_->getEpisodicMemory()) {
        // Get episodes for replay
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainStepOrchestrator::applyDevelopment(SimulationStep currentStep, RandomGenerator& rng, TimestepDuration dt) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        brain_->getDevelopmentSystem()->update(brain_, rng, dt * 1000);
        
        // Development affects plasticity rates
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
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

void BrainStepOrchestrator::consolidateMemories(SimulationStep currentStep) {
    if (auto* episodicMemory = brain_->getEpisodicMemory()) {
        // Periodic memory consolidation
        if (currentStep % 1000 == 0) {  // consolidation_interval
            episodicMemory->consolidate(0.3f);
        }
    }
}

void BrainStepOrchestrator::checkpointManager(SimulationStep currentStep, Timestamp currentTime) {
    if (auto* checkpointManager = brain_->getSpikeSystem()->getCheckpointManager()) {
        checkpointManager->update(currentStep, currentTime);
    }
}

void BrainStepOrchestrator::executeStep(SimulationStep currentStep, Timestamp currentTime) {
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
     * 14. Periodic memory consolidation
     * 15. Checkpoint management
     */
    
    // Phase 1: Process pending delayed spikes (deliver synaptic input)
    processDelayedSpikes(currentStep, currentTime);
    
    // Phase 2: Update all neurons (LIF dynamics)
    updateNeurons(currentStep, currentTime);
    
    // Phase 3: Detect spikes and schedule spike events
    // Note: This is handled internally by the spike system during processDelayedSpikes
    // and processSpikes calls
    
    // Phase 4: Update working memory
    updateWorkingMemory(brain_->getSpikeSystem()->getTimestepDuration());
    
    // Phase 5: Apply neuromodulation effects
    applyNeuromodulation();
    
    // Phase 6: Apply plasticity rules
    applyPlasticity(brain_->getSpikeSystem()->getTimestepDuration());
    
    // Phase 7: Update episodic memory
    updateEpisodicMemory(currentStep, currentTime);
    
    // Phase 8: Update prediction system
    updatePredictionSystem();
    
    // Phase 9: Update attention system
    updateAttention(brain_->getSpikeSystem()->getTimestepDuration());
    
    // Phase 10: Update concept formation
    updateConceptFormation();
    
    // Phase 11: Apply structural plasticity
    applyStructuralPlasticity(currentStep, *brain_->getRandomGenerator());
    
    // Phase 12: Replay important memories
    replayMemories(currentStep);
    
    // Phase 13: Apply development effects
    applyDevelopment(currentStep, *brain_->getRandomGenerator(), brain_->getSpikeSystem()->getTimestepDuration());
    
    // Phase 14: Periodic memory consolidation
    consolidateMemories(currentStep);
    
    // Phase 15: Checkpoint management
    checkpointManager(currentStep, currentTime);
}

} // namespace nlm