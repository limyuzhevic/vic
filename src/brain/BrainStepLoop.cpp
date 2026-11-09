#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include "BrainUtils.cpp"

namespace nlm {

// BrainStepLoop implementation - Main 15-step Phase 6 integration loop
// Extracted from Brain.cpp lines 343-692

class BrainStepLoop {
public:
    // Phase 6 integration loop - simplified from original 1591 line Brain.cpp
    static void executeIntegrationLoop(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
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
         * 15. Homeostatic synaptic scaling (Phase 4)
         */
        
        impl->currentStep = currentStep;
        impl->currentTime = currentTime;
        impl->totalSpikesThisStep = 0;
        
        // STEP 1: Process pending delayed spikes (deliver synaptic input)
        processDelayedSpikes(impl, currentStep, currentTime);
        
        // STEP 2: Update all neurons (LIF dynamics)
        updateNeurons(impl, currentTime, impl->timestep);
        
        // STEP 3: Detect spikes and schedule spike events
        detectAndScheduleSpikes(impl, currentStep, currentTime);
        
        // STEP 4: Update working memory
        updateWorkingMemory(impl);
        
        // STEP 5: Apply neuromodulation effects
        applyNeuromodulationEffects(impl);
        
        // STEP 6: Apply plasticity rules
        applyPlasticity(impl, currentTime);
        
        // STEP 7: Update episodic memory
        updateEpisodicMemory(impl, currentStep, currentTime);
        
        // STEP 8: Update prediction system
        updatePredictionSystem(impl);
        
        // STEP 9: Update attention system
        updateAttentionSystem(impl);
        
        // STEP 10: Update concept formation
        updateConceptFormation(impl);
        
        // STEP 11: Apply structural plasticity
        applyStructuralPlasticity(impl, currentStep);
        
        // STEP 12: Replay important memories
        replayMemories(impl, currentStep);
        
        // STEP 13: Apply development effects
        applyDevelopmentEffects(impl, currentStep);
        
        // STEP 14: Collect statistics
        collectStatistics(impl);
        
        // STEP 15: Homeostatic synaptic scaling
        applyHomeostaticScaling(impl, currentStep, currentTime);
    }
    
private:
    static void processDelayedSpikes(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
        if (impl->spikeSystem) {
            impl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        }
    }
    
    static void updateNeurons(Brain::Impl* impl, Timestamp currentTime, Timestamp timestep) {
        for (auto& region : impl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, timestep);
                }
            }
        }
    }
    
    static void detectAndScheduleSpikes(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
        // Simplified spike detection - could be enhanced with actual spike logic
        for (auto& region : impl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Check if neuron just fired this step
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                    state.lastSpikeTime >= 0.0f &&
                                    std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < impl->timestep * 2.0f);
                    
                    if (justFired) {
                        // Count spikes for statistics
                        ++impl->totalSpikesThisStep;
                        ++impl->totalSpikesTotal;
                        
                        // Could be extended with actual spike event handling
                    }
                }
            }
        }
        
        if (impl->spikeSystem) {
            impl->spikeSystem->processSpikes(currentStep);
        }
    }
    
    static void updateWorkingMemory(Brain::Impl* impl) {
        if (impl->workingMemory) {
            impl->workingMemory->update(impl->timestep);
        }
    }
    
    static void applyNeuromodulationEffects(Brain::Impl* impl) {
        // Update novelty detection
        if (impl->novelty) {
            impl->novelty->update(impl->timestep);
        }
        
        // Update curiosity
        if (impl->curiosity) {
            float noveltyLevel = impl->novelty ? impl->novelty->getLevel() : 0.0f;
            float predictionError = impl->predictionError ? impl->predictionError->getError() : 0.0f;
            impl->curiosity->update(noveltyLevel, predictionError, impl->timestep);
        }
        
        // Update dopamine (reward prediction error)
        if (impl->dopamine) {
            impl->dopamine->update(impl->timestep);
            
            // Apply dopamine effects on neural excitability
            float dopamineLevel = impl->dopamine->getLevel();
            float excitabilityMod = dopamineLevel * 0.5f;
            if (excitabilityMod > 0.0f) {
                for (auto& region : impl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            neuron->injectCurrent(excitabilityMod);
                        }
                    }
                }
            }
        }
    }
    
    static void applyPlasticity(Brain::Impl* impl, Timestamp currentTime) {
        // Calculate neuromodulation factor for plasticity
        float plasticityMod = 1.0f;
        if (impl->dopamine) {
            plasticityMod = impl->dopamine->getPlasticityFactor();
        }
        
        for (auto& region : impl->regions) {
            for (auto& syn : region->getSynapses()) {
                // Apply STDP with neuromodulation
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        if (impl->stdp) {
                            impl->stdp->update(syn.get(), preSpikes, postSpikes, impl->timestep);
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
                        if (impl->hebbian) {
                            impl->hebbian->update(syn.get(), preSpikes, postSpikes, impl->timestep);
                        }
                    }
                }
                
                // Update synapse state
                syn->step(currentTime);
            }
        }
    }
    
    static void updateEpisodicMemory(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
        impl->stepsSinceLastEpisode++;
        if (impl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
            impl->stepsSinceLastEpisode = 0;
            
            if (impl->episodicMemory) {
                // Capture current brain state as an episode
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = impl->dopamine ? impl->dopamine->getLevel() : 0.0f;
                
                // Store active neurons
                for (auto& region : impl->regions) {
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
                episode.reward = impl->dopamine ? impl->dopamine->getLevel() : 0.0f;
                
                impl->episodicMemory->storeEpisode(episode);
            }
        }
    }
    
    static void updatePredictionSystem(Brain::Impl* impl) {
        if (impl->predictionSystem) {
            // The prediction system would be updated with sensory observations
            // For now, just track prediction error history
        }
    }
    
    static void updateAttentionSystem(Brain::Impl* impl) {
        if (impl->attention) {
            impl->attention->update(impl->timestep);
            
            // Apply attention to working memory winners
            if (impl->workingMemory && !impl->workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = impl->workingMemory->getMemoryNeurons();
                impl->attention->processCompetition(competitors);
            }
        }
    }
    
    static void updateConceptFormation(Brain::Impl* impl) {
        if (impl->conceptFormation) {
            // Would process current neural activity patterns to form concepts
            // This requires sensory state encoding
        }
    }
    
    static void applyStructuralPlasticity(Brain::Impl* impl, SimulationStep currentStep) {
        if (currentStep % 100 == 0) {
            if (impl->structuralPlasticity && impl->rng) {
                impl->structuralPlasticity->update(nullptr, *impl->rng);
            }
        }
    }
    
    static void replayMemories(Brain::Impl* impl, SimulationStep currentStep) {
        if (currentStep % impl->replayInterval == 0 && impl->episodicMemory) {
            // Get episodes for replay
            auto episodesToReplay = impl->episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                impl->episodicMemory->replayEpisode(episode);
            }
        }
    }
    
    static void applyDevelopmentEffects(Brain::Impl* impl, SimulationStep currentStep) {
        if (currentStep % 1000 == 0) {  // Update development every 1000 steps
            if (impl->developmentSystem && impl->rng) {
                impl->developmentSystem->update(nullptr, *impl->rng, impl->timestep * 1000);
                
                // Development affects plasticity rates
                auto* sp = impl->structuralPlasticity.get();
                if (sp) {
                    DevelopmentalStage stage = impl->developmentalStage;
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
    
    static void collectStatistics(Brain::Impl* impl) {
        // Statistics are collected throughout the step
        // This could be enhanced with more detailed metrics
    }
};

} // namespace nlm
