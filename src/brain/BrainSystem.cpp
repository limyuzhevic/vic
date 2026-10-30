#pragma once

#include <memory>
#include <vector>
#include <string>

namespace nlm {

class Brain;
class BrainStepHandler {
public:
    BrainStepHandler() = default;
    ~BrainStepHandler() = default;
    
    void initialize(Brain* brain) {
        this->brain = brain;
    }
    
    void processStep(SimulationStep currentStep, Timestamp currentTime) {
        if (!brain) {
            return;
        }
        
        // Process delayed spikes
        brain->pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        
        // Update neurons
        updateNeurons(brain, currentStep, currentTime);
        
        // Detect and process spikes
        processSpikes(brain, currentStep, currentTime);
        
        // Process immediate spikes
        brain->pImpl->spikeSystem->processSpikes(currentStep);
    }
    
    void updateStatistics() {
        // Update statistics based on processed spikes
        if (brain) {
            brain->pImpl->totalSpikesThisStep = brain->pImpl->spikeSystem->getPendingSpikeCount();
        }
    }
    
private:
    void updateNeurons(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
        for (auto& region : brain->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, brain->pImpl->timestep);
                }
            }
        }
    }
    
    void processSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
        for (auto& region : brain->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < brain->pImpl->timestep * 2.0f);

                    if (justFired) {
                        // Queue the spike
                        SpikeEvent event(neuron->getId(), currentTime, currentStep);
                        brain->pImpl->spikeSystem->queueSpike(event);

                        // Record plasticity events
                        recordPlasticityEvents(brain, region.get(), neuron->getId(), currentTime);
                        
                        // Update working memory
                        updateWorkingMemory(brain, neuron->getId(), state);
                    }
                }
            }
        }
    }
    
    void recordPlasticityEvents(Brain* brain, NeuralRegion* region, NeuronId neuronId, Timestamp currentTime) {
        // Record post-synaptic spike for incoming synapses (plasticity)
        auto incomingSynapses = region->getSynapsesTo(neuronId);
        for (Synapse* syn : incomingSynapses) {
            syn->recordPostSpike(currentTime);
        }

        // Schedule delayed spike events and record pre-synaptic spikes
        auto outgoingSynapses = region->getSynapsesFrom(neuronId);
        for (Synapse* syn : outgoingSynapses) {
            Delay delay = syn->getDelay();
            SimulationStep deliveryStep = brain->pImpl->currentStep + delay;
            Timestamp deliveryTime = currentTime + delay * brain->pImpl->timestep;

            DelayedSpikeEvent delayedEvent(
                neuronId,
                syn->getDestinationNeuron(),
                syn->getId(),
                syn->getWeight(),
                syn->getType(),
                currentTime,
                deliveryTime,
                brain->pImpl->currentStep,
                deliveryStep
            );

            brain->pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
            syn->recordPreSpike(currentTime);
        }
    }
    
    void updateWorkingMemory(Brain* brain, NeuronId neuronId, const NeuronState& state) {
        if (brain->pImpl->workingMemory) {
            float activation = std::abs(state.membranePotential - state.restingPotential) / 10.0f;
            brain->pImpl->workingMemory->storeToNeuron(neuronId, activation);
        }
    }
    
    Brain* brain = nullptr;
};

class BrainIntegrationManager {
public:
    BrainIntegrationManager() = default;
    ~BrainIntegrationManager() = default;
    
    void initialize(Brain* brain) {
        this->brain = brain;
        initializeDefaultIntervals();
    }
    
    void update(SimulationStep currentStep, Timestamp currentTime) {
        updateNeuromodulation();
        updatePlasticity(currentStep, currentTime);
        updateMemory(currentStep);
        updateCognition();
        updateDevelopment(currentStep);
        updateReplay(currentStep);
        updateConsolidation(currentStep);
    }
    
    void collectStatistics() {
        updateNeuromodulationStats();
    }
    
private:
    void initializeDefaultIntervals() {
        if (brain) {
            brain->pImpl->replayInterval = 100;
            brain->pImpl->consolidationInterval = 1000;
        }
    }
    
    void updateNeuromodulation() {
        // Update novelty detection
        if (brain->pImpl->novelty) {
            brain->pImpl->novelty->update(brain->pImpl->timestep);
        }
        
        // Update curiosity
        if (brain->pImpl->curiosity) {
            brain->pImpl->curiosity->update(brain->pImpl->timestep);
        }
        
        // Update dopamine and apply effects
        if (brain->pImpl->dopamine) {
            brain->pImpl->dopamine->update(brain->pImpl->timestep);
            applyDopamineEffects();
        }
    }
    
    void applyDopamineEffects() {
        float dopamineLevel = brain->pImpl->dopamine->getLevel();
        float excitabilityMod = dopamineLevel * 0.5f;
        
        for (auto& region : brain->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    void updateNeuromodulationStats() {
        // Update neuromodulation-related statistics
    }
    
    void updatePlasticity(SimulationStep currentStep, Timestamp currentTime) {
        float plasticityMod = 1.0f;
        if (brain->pImpl->dopamine) {
            plasticityMod = brain->pImpl->dopamine->getPlasticityFactor();
        }
        
        for (auto& region : brain->pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                applyPlasticityRules(syn, currentStep, currentTime, plasticityMod);
            }
        }
    }
    
    void applyPlasticityRules(Synapse* syn, SimulationStep currentStep, Timestamp currentTime, float plasticityMod) {
        // Apply STDP
        if (syn->getPlasticityFlags().stdp) {
            const auto& preSpikes = syn->getPreSpikeHistory();
            const auto& postSpikes = syn->getPostSpikeHistory();
            
            if (!preSpikes.empty() && !postSpikes.empty()) {
                brain->pImpl->stdp->update(syn, preSpikes, postSpikes, brain->pImpl->timestep);
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
                brain->pImpl->hebbian->update(syn, preSpikes, postSpikes, brain->pImpl->timestep);
            }
        }
        
        // Update synapse state
        syn->step(currentTime);
    }
    
    void updateMemory(SimulationStep currentStep) {
        // Update working memory
        if (brain->pImpl->workingMemory) {
            brain->pImpl->workingMemory->update(brain->pImpl->timestep);
        }
        
        // Update episodic memory
        updateEpisodicMemory(currentStep);
    }
    
    void updateEpisodicMemory(SimulationStep currentStep) {
        brain->pImpl->stepsSinceLastEpisode++;
        if (brain->pImpl->stepsSinceLastEpisode >= 10) {
            brain->pImpl->stepsSinceLastEpisode = 0;
            
            if (brain->pImpl->episodicMemory) {
                storeEpisodicMemory(currentStep);
            }
        }
    }
    
    void storeEpisodicMemory(SimulationStep currentStep) {
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = brain->pImpl->dopamine ? brain->pImpl->dopamine->getLevel() : 0.0f;
        
        for (auto& region : brain->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    if (neuron->isFiring() || 
                        std::abs(state.membranePotential - state.restingPotential) > 5.0f) {
                        episode.activeNeurons.push_back(neuron->getId());
                        episode.neuronActivations.push_back(
                            std::abs(state.membranePotential - state.restingPotential) / 20.0f);
                    }
                }
            }
        }
        
        episode.reward = brain->pImpl->dopamine ? brain->pImpl->dopamine->getLevel() : 0.0f;
        brain->pImpl->episodicMemory->storeEpisode(episode);
    }
    
    void updateCognition() {
        if (brain->pImpl->attention) {
            brain->pImpl->attention->update(brain->pImpl->timestep);
            updateAttentionWithWorkingMemory();
        }
    }
    
    void updateAttentionWithWorkingMemory() {
        if (brain->pImpl->workingMemory && !brain->pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = brain->pImpl->workingMemory->getMemoryNeurons();
            brain->pImpl->attention->processCompetition(competitors);
        }
    }
    
    void updateDevelopment(SimulationStep currentStep) {
        if (currentStep % 1000 == 0) {
            brain->pImpl->developmentSystem->update(brain, *brain->pImpl->rng, brain->pImpl->timestep * 1000);
            updateDevelopmentPlasticity();
        }
    }
    
    void updateDevelopmentPlasticity() {
        auto* sp = brain->pImpl->structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = brain->pImpl->developmentalStage;
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
    
    void updateReplay(SimulationStep currentStep) {
        if (currentStep % brain->pImpl->replayInterval == 0 && brain->pImpl->episodicMemory) {
            auto episodesToReplay = brain->pImpl->episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                brain->pImpl->episodicMemory->replayEpisode(episode);
            }
        }
    }
    
    void updateConsolidation(SimulationStep currentStep) {
        if (currentStep % brain->pImpl->consolidationInterval == 0 && brain->pImpl->episodicMemory) {
            brain->pImpl->episodicMemory->consolidate(0.3f);
        }
    }
    
    Brain* brain = nullptr;
};

class BrainErrorHandler {
public:
    BrainErrorHandler() = default;
    ~BrainErrorHandler() = default;
    
    void handleError(const std::string& message) {
        NLM_LOG_ERROR(message);
        // Error recovery and logging
    }
};

}