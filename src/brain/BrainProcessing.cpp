// Brain Processing Implementation

#include "BrainProcessing.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulation.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <numeric>

namespace nlm {

void BrainProcessing::processStep(Brain::Impl& impl, SimulationStep currentStep, Timestamp currentTime) {
    // Reset step counters
    impl.totalSpikesThisStep = 0;
    impl.currentStep = currentStep;
    impl.currentTime = currentTime;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    impl.spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    updateNeurons(impl, currentTime);
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    detectAndProcessSpikes(impl, currentStep, currentTime);
    
    // Process immediate spikes
    impl.spikeSystem->processSpikes(currentStep);
    
    // ========== STEP 4: Update working memory ==========
    updateWorkingMemory(impl);
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    applyNeuromodulationEffects(impl);
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    applyPlasticityRules(impl);
    
    // ========== STEP 6b: Homeostatic synaptic scaling ==========
    applyHomeostaticScaling(impl, currentStep);
    
    // ========== STEP 7: Update episodic memory ==========
    updateEpisodicMemory(impl);
    
    // ========== STEP 8: Update prediction system ==========
    updatePredictionSystem(impl);
    
    // ========== STEP 9: Update attention system ==========
    updateAttentionSystem(impl);
    
    // ========== STEP 10: Update concept formation ==========
    updateConceptFormation(impl);
    
    // ========== STEP 11: Apply structural plasticity ==========
    applyStructuralPlasticity(impl, currentStep);
    
    // ========== STEP 12: Replay important memories ==========
    replayMemories(impl, currentStep);
    
    // ========== STEP 13: Apply development effects ==========
    applyDevelopmentEffects(impl, currentStep);
    
    // ========== STEP 14: Periodic memory consolidation ==========
    consolidateMemories(impl, currentStep);
    
    // ========== STEP 15: Checkpoint management ==========
    checkpointManagerUpdate(impl, currentStep, currentTime);
}

void BrainProcessing::updateNeurons(Brain::Impl& impl, Timestamp currentTime) {
    // Update all neurons using LIF dynamics
    for (auto& region : impl.regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, impl.timestep);
            }
        }
    }
}

void BrainProcessing::detectAndProcessSpikes(Brain::Impl& impl, SimulationStep currentStep, Timestamp currentTime) {
    // Detect spikes and schedule outgoing spike events
    for (auto& region : impl.regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < impl.timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    impl.spikeSystem->queueSpike(event);

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
                        Timestamp deliveryTime = currentTime + delay * impl.timestep;

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

                        impl.spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (impl.workingMemory) {
                        impl.workingMemory->storeToNeuron(neuron->getId(), 1.0f);
                    }
                }
            }
        }
    }
}

void BrainProcessing::updateWorkingMemory(Brain::Impl& impl) {
    if (impl.workingMemory) {
        impl.workingMemory->update(impl.timestep);
    }
}

void BrainProcessing::applyNeuromodulationEffects(Brain::Impl& impl) {
    // Update novelty detection
    if (impl.novelty) {
        impl.novelty->update(impl.timestep);
    }
    
    // Update curiosity
    if (impl.curiosity) {
        impl.curiosity->update(impl.novelty ? impl.novelty->getLevel() : 0.0f, 
                              impl.predictionError ? impl.predictionError->getError() : 0.0f, 
                              impl.timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (impl.dopamine) {
        impl.dopamine->update(impl.timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = impl.dopamine->getLevel();
        for (auto& region : impl.regions) {
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

void BrainProcessing::applyPlasticityRules(Brain::Impl& impl) {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (impl.dopamine) {
        plasticityMod = impl.dopamine->getPlasticityFactor();
    }
    
    for (auto& region : impl.regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    impl.stdp->update(syn.get(), preSpikes, postSpikes, impl.timestep);
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
                    impl.hebbian->update(syn.get(), preSpikes, postSpikes, impl.timestep);
                }
            }
            
            // Update synapse state
            syn->step(impl.currentTime);
        }
    }
}

void BrainProcessing::applyHomeostaticScaling(Brain::Impl& impl, SimulationStep currentStep) {
    // Homeostatic synaptic scaling every 100 steps
    if (currentStep > 0 && currentStep % 100 == 0) {
        constexpr float kIncomingExcitatoryCap = 3.0f;
        for (auto& region : impl.regions) {
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

void BrainProcessing::updateEpisodicMemory(Brain::Impl& impl) {
    impl.stepsSinceLastEpisode++;
    if (impl.stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        impl.stepsSinceLastEpisode = 0;
        
        if (impl.episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = impl.currentStep;
            episode.reward = impl.dopamine ? impl.dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : impl.regions) {
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
            episode.reward = impl.dopamine ? impl.dopamine->getLevel() : 0.0f;
            
            impl.episodicMemory->storeEpisode(episode);
        }
    }
}

void BrainProcessing::updatePredictionSystem(Brain::Impl& impl) {
    if (impl.predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void BrainProcessing::updateAttentionSystem(Brain::Impl& impl) {
    if (impl.attention) {
        impl.attention->update(impl.timestep);
        
        // Apply attention to working memory winners
        if (impl.workingMemory && !impl.workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = impl.workingMemory->getMemoryNeurons();
            impl.attention->processCompetition(competitors);
        }
    }
}

void BrainProcessing::updateConceptFormation(Brain::Impl& impl) {
    if (impl.conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void BrainProcessing::applyStructuralPlasticity(Brain::Impl& impl, SimulationStep currentStep) {
    // Apply structural plasticity periodically
    if (currentStep % 100 == 0) {
        impl.structuralPlasticity->update(nullptr, *impl.rng);
    }
}

void BrainProcessing::replayMemories(Brain::Impl& impl, SimulationStep currentStep) {
    // Replay important memories
    if (currentStep % impl.replayInterval == 0 && impl.episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = impl.episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            impl.episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainProcessing::applyDevelopmentEffects(Brain::Impl& impl, SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        impl.developmentSystem->update(nullptr, *impl.rng, impl.timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = impl.structuralPlasticity.get();
        if (sp) {
            DevelopmentalStage stage = impl.developmentalStage;
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

void BrainProcessing::consolidateMemories(Brain::Impl& impl, SimulationStep currentStep) {
    // Periodic memory consolidation
    if (currentStep % impl.consolidationInterval == 0 && impl.episodicMemory) {
        // Consolidate important memories, remove weak ones
        impl.episodicMemory->consolidate(0.3f);
    }
}

void BrainProcessing::checkpointManagerUpdate(Brain::Impl& impl, SimulationStep currentStep, Timestamp currentTime) {
    if (impl.checkpointManager) {
        impl.checkpointManager->update(currentStep, currentTime);
    }
}

} // namespace nlm
