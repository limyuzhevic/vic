// BrainStep.cpp - Implementation of Brain step processing functions
// Provides implementations for split Brain.cpp functionality

#include "BrainStep.h"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// BrainStep implementation

void BrainStep::stepImpl(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    processDelayedSpikes(pImpl, currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    updateNeurons(pImpl, currentTime, pImpl->timestep);
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    detectAndScheduleSpikes(pImpl, currentStep, currentTime);
    
    // Process immediate spikes
    processSpikes(pImpl, currentStep);
    
    // ========== STEP 4: Update working memory ==========
    updateWorkingMemory(pImpl, pImpl->timestep);
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    applyNeuromodulation(pImpl, pImpl->timestep);
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    applyPlasticity(pImpl, pImpl->timestep, plasticityMod);
    
    // ========== STEP 7: Update episodic memory ==========
    updateEpisodicMemory(pImpl, currentStep);
    
    // ========== STEP 8: Update prediction system ==========
    updatePredictionSystem(pImpl);
    
    // ========== STEP 9: Update attention system ==========
    updateAttentionSystem(pImpl, pImpl->timestep);
    
    // ========== STEP 10: Update concept formation ==========
    updateConceptFormation(pImpl);
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    applyStructuralPlasticity(pImpl, currentStep);
    
    // ========== STEP 12: Replay important memories ==========
    replayMemories(pImpl, currentStep);
    
    // ========== STEP 13: Apply development effects ==========
    applyDevelopmentEffects(pImpl, currentStep);
    
    // ========== STEP 14: Periodic memory consolidation ==========
    consolidateMemory(pImpl, currentStep);
    
    // ========== STEP 15: Checkpoint management ==========
    manageCheckpoints(pImpl, currentStep, currentTime);
    
    // ========== STEP 16: Collect statistics ==========
    collectStatistics(pImpl);
}

void BrainStep::processDelayedSpikes(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime) {
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void BrainStep::updateNeurons(Brain::Impl* pImpl, Timestamp currentTime, TimestepDuration timestep) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void BrainStep::detectAndScheduleSpikes(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

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
                        Timestamp deliveryTime = currentTime + delay * timestep;

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
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void BrainStep::processSpikes(Brain::Impl* pImpl, SimulationStep currentStep) {
    pImpl->spikeSystem->processSpikes(currentStep);
}

void BrainStep::updateWorkingMemory(Brain::Impl* pImpl, TimestepDuration timestep) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(timestep);
    }
}

void BrainStep::applyNeuromodulation(Brain::Impl* pImpl, TimestepDuration timestep) {
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(timestep);
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

void BrainStep::applyPlasticity(Brain::Impl* pImpl, TimestepDuration timestep, float plasticityMod) {
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    pImpl->stdp->update(syn, preSpikes, postSpikes, timestep);
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
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, timestep);
                }
            }
            
            // Update synapse state
            syn->step(pImpl->currentTime);
        }
    }
}

void BrainStep::updateEpisodicMemory(Brain::Impl* pImpl, SimulationStep currentStep) {
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

void BrainStep::updatePredictionSystem(Brain::Impl* pImpl) {
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void BrainStep::updateAttentionSystem(Brain::Impl* pImpl, TimestepDuration timestep) {
    if (pImpl->attention) {
        pImpl->attention->update(timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
}

void BrainStep::updateConceptFormation(Brain::Impl* pImpl) {
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void BrainStep::applyStructuralPlasticity(Brain::Impl* pImpl, SimulationStep currentStep) {
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(pImpl, *pImpl->rng);
    }
}

void BrainStep::replayMemories(Brain::Impl* pImpl, SimulationStep currentStep) {
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainStep::applyDevelopmentEffects(Brain::Impl* pImpl, SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(pImpl, *pImpl->rng, pImpl->timestep * 1000);
        
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

void BrainStep::consolidateMemory(Brain::Impl* pImpl, SimulationStep currentStep) {
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void BrainStep::manageCheckpoints(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void BrainStep::collectStatistics(Brain::Impl* pImpl) {
    // Update spike statistics
    pImpl->totalSpikesTotal += pImpl->totalSpikesThisStep;
}

void BrainStep::registerSpikeHandlers(Brain::Impl* pImpl) {
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([pImpl](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    pImpl->spikeSystem->registerDelayedHandler([pImpl](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // Apply synaptic weight as current
                    MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                    if (event.is_excitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    return;
                }
            }
        }
    });
}

void BrainStep::configureCheckpointManager(Brain::Impl* pImpl) {
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

// Memory system implementation

namespace BrainMemory {

void initializeMemorySystems(Brain::Impl* pImpl) {
    // Initialize working memory
    pImpl->workingMemory->initialize(nullptr);  // TODO: Pass proper reference
    pImpl->workingMemory->setCapacity(100);  // Default capacity
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(nullptr);  // TODO: Pass proper reference
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(nullptr);  // TODO: Pass proper reference
}

void clearMemorySystems(Brain::Impl* pImpl) {
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
}

void updateMemorySystems(Brain::Impl* pImpl, SimulationStep currentStep) {
    // Update working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // Update episodic memory consolidation
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

} // namespace BrainMemory

// Neuromodulation implementation

namespace BrainNeuromodulation {

void updateNeuromodulationSystems(Brain::Impl* pImpl, TimestepDuration timestep) {
    if (pImpl->novelty) {
        pImpl->novelty->update(timestep);
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->update(timestep);
    }
    
    if (pImpl->dopamine) {
        pImpl->dopamine->update(timestep);
    }
}

void applyDopamineEffects(Brain::Impl* pImpl, float dopamineLevel) {
    // Apply dopamine effects on neural excitability
    float excitabilityMod = dopamineLevel * 0.5f;
    if (excitabilityMod > 0.0f) {
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

} // namespace BrainNeuromodulation

// Cognition system implementation

namespace BrainCognition {

void initializeCognitionSystems(Brain::Impl* pImpl) {
    // Initialize cognition systems
    if (pImpl->planner) {
        pImpl->planner->initialize(nullptr);  // TODO: Pass proper reference
        pImpl->planner->setPlanningDepth(5);
    }
    
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->initialize(nullptr);  // TODO: Pass proper reference
    }
    
    if (pImpl->attention) {
        pImpl->attention->initialize(nullptr);  // TODO: Pass proper reference
        pImpl->attention->setInhibitionStrength(0.5f);
        pImpl->attention->setExcitationStrength(1.5f);
    }
}

void updateCognitionSystems(Brain::Impl* pImpl, TimestepDuration timestep) {
    if (pImpl->planner) {
        // Update planner if needed
    }
    
    if (pImpl->conceptFormation) {
        // Update concept formation if needed
    }
    
    if (pImpl->attention) {
        pImpl->attention->update(timestep);
    }
}

} // namespace BrainCognition

// Development system implementation

namespace BrainDevelopment {

void updateDevelopmentSystem(Brain::Impl* pImpl, SimulationStep currentStep, TimestepDuration timestep) {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(pImpl, *pImpl->rng, timestep * 1000);
    }
}

void applyDevelopmentalPlasticity(Brain::Impl* pImpl, DevelopmentalStage stage) {
    auto* sp = pImpl->structuralPlasticity;
    if (sp) {
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

} // namespace BrainDevelopment

// Input/output implementation

namespace BrainIO {

void receiveSensoryInputImpl(Brain::Impl* pImpl, const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void injectCurrentImpl(Brain::Impl* pImpl, NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void injectCurrentToNeuronsImpl(Brain::Impl* pImpl, NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

void produceActionImpl(Brain::Impl* pImpl) {
    // Simple action selection based on motor neuron activity
    if (pImpl->motorNeurons.empty()) {
        // Return default action
        return;
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Action selection logic here
}

void applyNeuromodulationImpl(Brain::Impl* pImpl, const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    if (pImpl->stdp) {
        pImpl->stdp->setLTPWeight(0.01f * modulation);
        pImpl->stdp->setLTDWeight(0.012f * modulation);
    }
}

void updatePlasticityImpl(Brain::Impl* pImpl) {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void developImpl(Brain::Impl* pImpl) {
    // Development updates structural plasticity
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(pImpl, *pImpl->rng);
    }
}

void resetImpl(Brain::Impl* pImpl) {
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
}

} // namespace BrainIO

// Statistics implementation

namespace BrainStats {

void logStatusImpl(const Brain::Impl* pImpl) {
    // Implementation will be added as needed
}

float BrainStats::getExcitationInhibitionRatioImpl(const Brain::Impl* pImpl) {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t BrainStats::getTotalSpikeCountImpl(const Brain::Impl* pImpl) {
    return pImpl->totalSpikesTotal;
}

size_t BrainStats::getPendingSpikeEventCountImpl(const Brain::Impl* pImpl) {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

size_t BrainStats::getTotalNeuronCountImpl(const Brain::Impl* pImpl) {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t BrainStats::getTotalSynapseCountImpl(const Brain::Impl* pImpl) {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t BrainStats::getActiveNeuronCountImpl(const Brain::Impl* pImpl) {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t BrainStats::getFiringNeuronCountImpl(const Brain::Impl* pImpl) {
    return pImpl->totalSpikesThisStep;
}

float BrainStats::getAverageFiringRateImpl(const Brain::Impl* pImpl) {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

} // namespace BrainStats

} // namespace nlm
