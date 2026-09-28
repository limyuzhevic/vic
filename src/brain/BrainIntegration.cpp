// BrainIntegration.cpp - Integration of brain systems
// Contains integration logic and helper functions for memory, prediction, cognition, and neuromodulation systems
// This file does NOT contain Brain:: method implementations - those are in BrainCore.cpp

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// ===== MEMORY SYSTEM INTEGRATION =====

void initializeMemorySystems(Brain* brain) {
    auto& pImpl = brain->pImpl;
    
    // Initialize working memory
    pImpl->workingMemory->initialize(brain);
    pImpl->workingMemory->setCapacity(brain->getTotalNeuronCount() / 10);
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(brain);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(brain);
}

void processMemorySystems(Brain* brain, Timestamp currentTime, SimulationStep currentStep) {
    auto& pImpl = brain->pImpl;
    
    // ========== STEP 4: Update working memory ==========
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // ========== STEP 5: Update episodic memory ==========
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
            
            // Store reward in episode
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void processMemoryReplay(Brain* brain, SimulationStep currentStep) {
    auto& pImpl = brain->pImpl;
    
    // ========== STEP 12: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

// ===== COGNITION SYSTEM INTEGRATION =====

void initializeCognitionSystems(Brain* brain) {
    auto& pImpl = brain->pImpl;
    
    // Initialize cognition systems
    pImpl->planner->initialize(brain);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(brain);
    
    pImpl->attention->initialize(brain);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
}

void processCognitionSystems(Brain* brain, Timestamp currentTime, SimulationStep currentStep) {
    auto& pImpl = brain->pImpl;
    
    // ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void processPredictionSystem(Brain* brain) {
    auto& pImpl = brain->pImpl;
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

// ===== NEUROMODULATION SYSTEM INTEGRATION =====

void initializeNeuromodulationSystems(Brain* brain) {
    auto& pImpl = brain->pImpl;
    
    // Initialize neuromodulation
    pImpl->novelty->initialize(brain);
    pImpl->curiosity->initialize(brain);
    
    // Note: Dopamine initialization would go here if it has an initialize method
}

void processNeuromodulationSystems(Brain* brain, Timestamp currentTime) {
    auto& pImpl = brain->pImpl;
    
    // ========== STEP 5 (continued): Apply neuromodulation effects ==========
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
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void processPlasticitySystems(Brain* brain, Timestamp currentTime, float plasticityMod) {
    auto& pImpl = brain->pImpl;
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
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
            syn->step(currentTime);
        }
    }
}

void processDevelopmentSystem(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    auto& pImpl = brain->pImpl;
    
    // ========== STEP 13: Apply development effects ==========
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(brain, *pImpl->rng, pImpl->timestep * 1000);
        
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

void processCheckpoints(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    auto& pImpl = brain->pImpl;
    
    // ========== STEP 15: Checkpoint management ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

// ===== SYSTEM ACCESSORS =====

void applyNeuromodulation(Brain* brain, const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    brain->pImpl->stdp->setLTPWeight(0.01f * modulation);
    brain->pImpl->stdp->setLTDWeight(0.012f * modulation);
}

void updatePlasticity(Brain* brain) {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void develop(Brain* brain) {
    // Development updates structural plasticity
    brain->pImpl->structuralPlasticity->update(brain, *brain->pImpl->rng);
}

} // namespace nlm
