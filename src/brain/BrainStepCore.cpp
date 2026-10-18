#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

void Brain::stepCore(SimulationStep currentStep) {
    NLM_LOG_DEBUG("Core step: " + std::to_string(currentStep));
    
    // Reset statistics for this step
    totalSpikesThisStep = 0;
    
    // ========== STEP 1: Spike integration ==========
    
    // Process immediate spikes
    spikeSystem->processSpikes(currentStep);
    
    // ========== STEP 2: Update neuromodulation ==========
    
    // Apply neuromodulatory signals
    // (Neuromodulators can be applied automatically based on prediction errors, rewards, etc.)
    
    // ========== STEP 3: Update working memory ==========
    if (workingMemory) {
        workingMemory->update(timestep);
    }
    
    // ========== STEP 4: Apply neuromodulation effects ==========
    
    // Update novelty detection
    if (novelty) {
        novelty->update(timestep);
    }
    
    // Update curiosity
    if (curiosity) {
        curiosity->update(timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (dopamine) {
        dopamine->update(timestep);
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = dopamine->getLevel();
        for (auto& region : regions) {
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
    
    // ========== STEP 5: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (dopamine) {
        plasticityMod = dopamine->getPlasticityFactor();
    }
    
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
            syn->step(currentTime);
        }
    }
    
    // ========== STEP 6: Update episodic memory ==========
    stepsSinceLastEpisode++;
    if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        stepsSinceLastEpisode = 0;
        
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
            
            // Store in episodic memory
            episodicMemory->storeEpisode(episode);
            
            // Run consolidation periodically
            if (stepsSinceLastEpisode == 0 && episodicMemory) {
                episodicMemory->consolidate(0.5f);
            }
        }
    }
    
    // ========== STEP 7: Development ==========
    developmentSystem->update(timestep);
    
    // ========== STEP 8: Handle replay and consolidation ==========
    if (stepsSinceLastEpisode == 0 && replayInterval > 0 && currentStep % replayInterval == 0) {
        // Trigger replay of recent episodes (for consolidation)
        if (episodicMemory) {
            auto recentEpisodes = episodicMemory->getRecentEpisodes(10);
            for (const auto& episode : recentEpisodes) {
                // Replay the episode
                for (const auto& neuronId : episode.activeNeurons) {
                    // Find neuron and activate it
                    for (auto& region : regions) {
                        for (auto& pop : region->getPopulations()) {
                            for (auto* neuron : pop->getNeurons()) {
                                if (neuron->getId() == neuronId) {
                                    neuron->injectCurrent(0.5f);  // Replay activation
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Update development
    if (developmentSystem) {
        developmentSystem->develop();
    }
    
    // Check for consolidation
    if (currentStep % consolidationInterval == 0 && episodicMemory) {
        episodicMemory->consolidate(0.5f);
    }
}

void Brain::stepIntegration(SimulationStep currentStep, Timestamp currentTime) {
    this->currentStep = currentStep;
    this->currentTime = currentTime;
    
    // Store old state for comparison
    std::vector<NeuronState> previousStates;
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                previousStates.push_back(neuron->getState());
            }
        }
    }
    
    // Call core step
    stepCore(currentStep);
    
    // Compute prediction errors and update prediction system
    if (predictionSystem) {
        predictionSystem->update(this, currentStep, currentTime);
    }
    
    // Apply learned predictions to neural activity
    // (This allows the brain to simulate its own predictions)
}

} // namespace nlm