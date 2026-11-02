#include "BrainHelper.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <unordered_map>
#include <cmath>

namespace nlm {

BrainHelper::BrainHelper(Brain::Impl* brainImpl) : brainImpl_(brainImpl) {}

void BrainHelper::processDelayedSpikeDelivery(SimulationStep currentStep, Timestamp currentTime) {
    brainImpl_->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void BrainHelper::updateAllNeurons(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : brainImpl_->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, brainImpl_->timestep);
            }
        }
    }
}

void BrainHelper::processSpikes(SimulationStep currentStep) {
    brainImpl_->spikeSystem->processSpikes(currentStep);
}

void BrainHelper::detectAndScheduleSpikes(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : brainImpl_->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < brainImpl_->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    brainImpl_->spikeSystem->queueSpike(event);

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
                        Timestamp deliveryTime = currentTime + delay * brainImpl_->timestep;

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

                        brainImpl_->spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (brainImpl_->workingMemory) {
                        brainImpl_->workingMemory->storeToNeuron(neuron->getId(), 1.0f);
                    }
                }
            }
        }
    }
}

void BrainHelper::updateWorkingMemory() {
    if (brainImpl_->workingMemory) {
        brainImpl_->workingMemory->update(brainImpl_->timestep);
    }
}

void BrainHelper::applyNeuromodulationEffects() {
    // Update novelty detection
    if (brainImpl_->novelty) {
        brainImpl_->novelty->update(brainImpl_->timestep);
    }
    
    // Update curiosity
    if (brainImpl_->curiosity) {
        brainImpl_->curiosity->update(
            brainImpl_->novelty ? brainImpl_->novelty->getLevel() : 0.0f,
            brainImpl_->predictionError ? brainImpl_->predictionError->getError() : 0.0f,
            brainImpl_->timestep
        );
    }
    
    // Update dopamine (reward prediction error)
    if (brainImpl_->dopamine) {
        brainImpl_->dopamine->update(brainImpl_->timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = brainImpl_->dopamine->getLevel();
        applyDopamineToNeurons(dopamineLevel);
    }
}

void BrainHelper::applyPlasticityRules(float plasticityMod) {
    for (auto& region : brainImpl_->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    applyPlasticityToSynapse(syn.get(), preSpikes, postSpikes, plasticityMod);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    brainImpl_->hebbian->update(syn.get(), preSpikes, postSpikes, brainImpl_->timestep);
                }
            }
            
            // Update synapse state
            syn->step(brainImpl_->currentTime);
        }
    }
}

void BrainHelper::applyHomeostaticSynapticScaling(SimulationStep currentStep) {
    if (currentStep <= 0 || currentStep % 100 != 0) return;
    
    for (auto& region : brainImpl_->regions) {
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

void BrainHelper::updateEpisodicMemory(SimulationStep currentStep) {
    brainImpl_->stepsSinceLastEpisode++;
    if (brainImpl_->stepsSinceLastEpisode >= 10) {
        brainImpl_->stepsSinceLastEpisode = 0;
        
        if (brainImpl_->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = brainImpl_->dopamine ? brainImpl_->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : brainImpl_->regions) {
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
            episode.reward = brainImpl_->dopamine ? brainImpl_->dopamine->getLevel() : 0.0f;
            
            brainImpl_->episodicMemory->storeEpisode(episode);
        }
    }
}

void BrainHelper::updatePredictionSystem() {
    if (brainImpl_->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void BrainHelper::updateAttentionSystem() {
    if (brainImpl_->attention) {
        brainImpl_->attention->update(brainImpl_->timestep);
        
        // Apply attention to working memory winners
        if (brainImpl_->workingMemory && !brainImpl_->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = brainImpl_->workingMemory->getMemoryNeurons();
            brainImpl_->attention->processCompetition(competitors);
        }
    }
}

void BrainHelper::updateConceptFormation() {
    if (brainImpl_->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void BrainHelper::applyStructuralPlasticity(SimulationStep currentStep) {
    if (currentStep % 100 == 0) {
        brainImpl_->structuralPlasticity->update(nullptr, *brainImpl_->rng, brainImpl_->timestep);
    }
}

void BrainHelper::replayImportantMemories(SimulationStep currentStep) {
    if (currentStep % brainImpl_->replayInterval == 0 && brainImpl_->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = brainImpl_->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            brainImpl_->episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainHelper::applyDevelopmentEffects(SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        brainImpl_->developmentSystem->update(nullptr, *brainImpl_->rng, brainImpl_->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = brainImpl_->structuralPlasticity.get();
        if (sp) {
            DevelopmentalStage stage = brainImpl_->developmentalStage;
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

void BrainHelper::consolidateMemories(SimulationStep currentStep) {
    if (currentStep % brainImpl_->consolidationInterval == 0 && brainImpl_->episodicMemory) {
        // Consolidate important memories, remove weak ones
        brainImpl_->episodicMemory->consolidate(0.3f);
    }
}

void BrainHelper::manageCheckpoints(SimulationStep currentStep, Timestamp currentTime) {
    if (brainImpl_->checkpointManager) {
        brainImpl_->checkpointManager->update(currentStep, currentTime);
    }
}

BrainHelper::SpikeStats BrainHelper::getStepStatistics() const {
    SpikeStats stats;
    stats.totalSpikesThisStep = brainImpl_->totalSpikesThisStep;
    
    // Count firing neurons
    for (auto& region : brainImpl_->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring()) {
                    stats.firingNeuronCount++;
                }
            }
        }
    }
    
    // Count active memory neurons
    if (brainImpl_->workingMemory) {
        stats.activeMemoryNeurons = brainImpl_->workingMemory->getMemoryNeurons().size();
    }
    
    return stats;
}

Neuron* BrainHelper::getNeuronById(NeuronId id) {
    auto it = brainImpl_->neuronById.find(id.index());
    if (it != brainImpl_->neuronById.end()) {
        return it->second;
    }
    
    // Fallback: linear search
    for (auto& region : brainImpl_->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getId() == id) {
                return neuron;
            }
        }
    }
    return nullptr;
}

void BrainHelper::injectSensoryInput(const SensoryInput& input) {
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = brainImpl_->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        brainImpl_->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (brainImpl_->workingMemory && normalizedValue > 0.5f) {
            brainImpl_->workingMemory->storeToNeuron(brainImpl_->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void BrainHelper::injectCurrentToNeuron(NeuronId neuron, MembranePotential current) {
    auto it = brainImpl_->neuronById.find(neuron.index());
    if (it != brainImpl_->neuronById.end()) {
        it->second->injectCurrent(current);
        return;
    }
    
    // Fallback: linear search
    for (auto& region : brainImpl_->regions) {
        for (auto* n : region->getAllNeurons()) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void BrainHelper::injectCurrentToNeuronType(NeuronType type, MembranePotential current) {
    for (auto& region : brainImpl_->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

void BrainHelper::storeNeuronInWorkingMemory(Neuron* neuron) {
    if (brainImpl_->workingMemory && neuron) {
        brainImpl_->workingMemory->storeToNeuron(neuron->getId(), 1.0f);
    }
}

float BrainHelper::calculateEIRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : brainImpl_->regions) {
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

std::vector<Neuron*> BrainHelper::getAllNeurons() const {
    std::vector<Neuron*> neurons;
    for (auto& region : brainImpl_->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neurons.push_back(neuron);
            }
        }
    }
    return neurons;
}

std::vector<Synapse*> BrainHelper::getAllSynapses() const {
    std::vector<Synapse*> synapses;
    for (auto& region : brainImpl_->regions) {
        for (auto& syn : region->getSynapses()) {
            synapses.push_back(syn.get());
        }
    }
    return synapses;
}

void BrainHelper::applyDopamineToNeurons(float dopamineLevel) {
    // Dopamine modulates excitability by adjusting effective current injection
    // Higher dopamine increases excitability (lower effective threshold)
    float excitabilityMod = dopamineLevel * 0.5f;
    if (excitabilityMod > 0.0f) {
        for (auto& region : brainImpl_->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

std::unique_ptr<Action> BrainHelper::generateAction() const {
    if (brainImpl_->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : brainImpl_->motorNeurons) {
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

void BrainHelper::updateFiringRates(Timestamp currentTime, TimestepDuration dt) {
    for (auto& region : brainImpl_->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                auto& state = neuron->getState();
                
                // Update the firing-rate metric from counted spikes, in Hz.
                if (dt > 0.0) {
                    state.firingRate = 0.9f * state.firingRate;
                }
            }
        }
    }
}

void BrainHelper::detectSpikesInRegion(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                       SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < brainImpl_->timestep * 2.0f);

                if (justFired) {
                    // Queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    brainImpl_->spikeSystem->queueSpike(event);

                    // Record post-synaptic spike for incoming synapses
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * brainImpl_->timestep;

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

                        brainImpl_->spikeSystem->queueDelayedSpike(delayedEvent);
                        syn->recordPreSpike(currentTime);
                    }
                    
                    if (brainImpl_->workingMemory) {
                        brainImpl_->workingMemory->storeToNeuron(neuron->getId(), 1.0f);
                    }
                }
            }
        }
    }
}

void BrainHelper::scheduleSpikeEvents(Neuron* neuron, SimulationStep currentStep, Timestamp currentTime) {
    // This method is now integrated into detectSpikesInRegion
}

void BrainHelper::applyDopamineModulation(float dopamineLevel) {
    // This method is now integrated into applyNeuromodulationEffects
}

void BrainHelper::applyPlasticityToSynapse(Synapse* syn, const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes, float plasticityMod) {
    brainImpl_->stdp->update(syn, preSpikes, postSpikes, brainImpl_->timestep);
    float weight = syn->getWeight();
    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
    syn->setWeight(weight);
}

void BrainHelper::updateWorkingMemoryActivity() {
    // This method is now integrated into updateWorkingMemory
}

void BrainHelper::updateAttentionBasedOnWorkingMemory() {
    // This method is now integrated into updateAttentionSystem
}

void BrainHelper::processDevelopmentChanges(SimulationStep currentStep) {
    // This method is now integrated into applyDevelopmentEffects
}

} // namespace nlm
