// Brain system coordinators implementation
// This file contains system coordinators for better separation of concerns

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// ========== NEURAL COMPUTATION SYSTEM COORDINATOR ==========

class NeuralComputationSystem {
public:
    NeuralComputationSystem(Brain* brain) : brain(brain) {}
    
    void updateNeurons(TimestepDuration timestep) {
        if (!brain) return;
        
        for (auto& region : brain->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(brain->pImpl->currentTime, timestep);
                }
            }
        }
    }
    
    void processSpikes(SimulationStep currentStep) {
        if (!brain) return;
        
        // Process immediate spikes through spike system
        brain->pImpl->spikeSystem->processSpikes(currentStep);
    }
    
    void detectAndQueueSpikes(SimulationStep currentStep, Timestamp currentTime) {
        if (!brain) return;
        
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
                        
                        // Record post-synaptic spike for incoming synapses
                        auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                        for (Synapse* syn : incomingSynapses) {
                            syn->recordPostSpike(currentTime);
                        }
                        
                        // Get outgoing synapses and schedule delayed spike events
                        auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                        for (Synapse* syn : outgoingSynapses) {
                            Delay delay = syn->getDelay();
                            SimulationStep deliveryStep = currentStep + delay;
                            Timestamp deliveryTime = currentTime + delay * brain->pImpl->timestep;
                            
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
                            
                            brain->pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                            
                            // Record pre-synaptic spike for plasticity
                            syn->recordPreSpike(currentTime);
                        }
                    }
                }
            }
        }
    }
    
    void applyAllPlasticity(TimestepDuration timestep, float plasticityMod) {
        if (!brain) return;
        
        for (auto& region : brain->pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                // Apply STDP with neuromodulation
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        brain->pImpl->stdp->update(syn, preSpikes, postSpikes, timestep);
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
                        brain->pImpl->hebbian->update(syn, preSpikes, postSpikes, timestep);
                    }
                }
                
                // Update synapse state
                syn->step(brain->pImpl->currentTime);
            }
        }
    }
    
    void injectCurrentToNeuronsOfType(NeuronType type, MembranePotential current) {
        if (!brain) return;
        
        for (auto& region : brain->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == type) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent(current);
                    }
                }
            }
        }
    }
    
    void applyNeuromodulationEffects() {
        if (!brain) return;
        
        // Update novelty detection
        if (brain->pImpl->novelty) {
            brain->pImpl->novelty->update(brain->pImpl->timestep);
        }
        
        // Update curiosity
        if (brain->pImpl->curiosity) {
            brain->pImpl->curiosity->update(brain->pImpl->timestep);
        }
        
        // Update dopamine (reward prediction error)
        if (brain->pImpl->dopamine) {
            brain->pImpl->dopamine->update(brain->pImpl->timestep);
            
            // Apply dopamine effects on neural excitability
            float dopamineLevel = brain->pImpl->dopamine->getLevel();
            float excitabilityMod = dopamineLevel * 0.5f;
            if (excitabilityMod > 0.0f) {
                for (auto& region : brain->pImpl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            neuron->injectCurrent(excitabilityMod);
                        }
                    }
                }
            }
        }
    }
    
    void processDelayedSpikeInput(SimulationStep currentStep, Timestamp currentTime) {
        if (!brain) return;
        
        brain->pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
    
private:
    Brain* brain;
};

// ========== MEMORY SYSTEM COORDINATOR ==========

class MemorySystem {
public:
    MemorySystem(Brain* brain) : brain(brain) {}
    
    void updateWorkingMemory(TimestepDuration timestep) {
        if (brain && brain->pImpl->workingMemory) {
            brain->pImpl->workingMemory->update(timestep);
        }
    }
    
    void updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
        if (!brain || !brain->pImpl->episodicMemory) return;
        
        brain->pImpl->stepsSinceLastEpisode++;
        if (brain->pImpl->stepsSinceLastEpisode >= 10) {
            brain->pImpl->stepsSinceLastEpisode = 0;
            
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = brain->pImpl->dopamine ? brain->pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : brain->pImpl->regions) {
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
            episode.reward = brain->pImpl->dopamine ? brain->pImpl->dopamine->getLevel() : 0.0f;
            
            brain->pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    void updateAssociativeMemory(TimestepDuration timestep) {
        if (brain && brain->pImpl->associativeMemory) {
            // Associative memory updates would go here
        }
    }
    
    void replayMemories(SimulationStep currentStep) {
        if (!brain || !brain->pImpl->episodicMemory) return;
        
        if (currentStep % brain->pImpl->replayInterval == 0) {
            // Get episodes for replay
            auto episodesToReplay = brain->pImpl->episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                brain->pImpl->episodicMemory->replayEpisode(episode);
            }
        }
    }
    
    void consolidateMemories(float consolidationThreshold) {
        if (!brain || !brain->pImpl->episodicMemory) return;
        
        if (brain->pImpl->currentStep % brain->pImpl->consolidationInterval == 0) {
            // Consolidate important memories, remove weak ones
            brain->pImpl->episodicMemory->consolidate(consolidationThreshold);
        }
    }
    
    void storeToWorkingMemory(NeuronId neuronId, float activation) {
        if (brain && brain->pImpl->workingMemory) {
            brain->pImpl->workingMemory->storeToNeuron(neuronId, activation);
        }
    }
    
    void updateAttentionAndPlanning() {
        if (!brain) return;
        
        // Update attention system
        if (brain->pImpl->attention) {
            brain->pImpl->attention->update(brain->pImpl->timestep);
            
            // Apply attention to working memory winners
            if (brain->pImpl->workingMemory && !brain->pImpl->workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = brain->pImpl->workingMemory->getMemoryNeurons();
                brain->pImpl->attention->processCompetition(competitors);
            }
        }
        
        // Update concept formation
        if (brain->pImpl->conceptFormation) {
            // Would process current neural activity patterns to form concepts
            // This requires sensory state encoding
        }
    }
    
    void updatePredictionSystem() {
        if (brain && brain->pImpl->predictionSystem) {
            // The prediction system would be updated with sensory observations
            // For now, just track prediction error history
        }
    }
    
    void storeActionOutcome(float reward, float predictionError) {
        // Store action outcomes in episodic memory
        if (brain && brain->pImpl->dopamine) {
            brain->pImpl->dopamine->setLevel(reward);
        }
        if (brain && brain->pImpl->predictionError) {
            brain->pImpl->predictionError->setPredictionError(predictionError);
        }
    }
    
    // Store neuron activity in working memory during step execution
    void storeActiveNeuronsToWorkingMemory(Timestamp currentTime) {
        if (!brain || !brain->pImpl->workingMemory) return;
        
        for (auto& region : brain->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                        brain->pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
private:
    Brain* brain;
};

// ========== DEVELOPMENT SYSTEM COORDINATOR ==========

class DevelopmentSystemCoordinator {
public:
    DevelopmentSystemCoordinator(Brain* brain) : brain(brain) {}
    
    void updateDevelopmentSystem(SimulationStep currentStep, TimestepDuration timestep) {
        if (!brain || !brain->pImpl->developmentSystem) return;
        
        if (currentStep % 1000 == 0) {  // Update development every 1000 steps
            brain->pImpl->developmentSystem->update(brain, *brain->pImpl->rng, timestep * 1000);
            
            // Development affects plasticity rates
            auto* sp = brain->pImpl->structuralPlasticity;
            if (sp) {
                DevelopmentalStage stage = brain->pImpl->developmentalStage;
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
                    case DevelopmentalStage::Aging:
                        plasticityMod = 0.1f;  // Reduced plasticity
                        break;
                }
                
                sp->setSynaptogenesisRate(0.0001f * plasticityMod);
                sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
            }
        }
    }
    
    void applyStructuralPlasticity(SimulationStep currentStep) {
        if (!brain || !brain->pImpl->structuralPlasticity) return;
        
        if (currentStep % 100 == 0) {
            brain->pImpl->structuralPlasticity->update(brain, *brain->pImpl->rng);
        }
    }
    
    void updateDevelopmentalStage() {
        if (!brain) return;
        
        // Automatic developmental stage progression based on age/stability
        if (brain->pImpl->currentStep > 5000) {
            if (brain->pImpl->developmentalStage < DevelopmentalStage::Adult) {
                brain->pImpl->developmentalStage = DevelopmentalStage::Adult;
            }
        }
    }
    
private:
    Brain* brain;
};

// ========== SYSTEM STATE TRACKER ==========

BrainSystemState::BrainSystemState() : neural(), memory(), neuromodulation(), development(), timing() {}

class SystemStateTracker {
public:
    SystemStateTracker(Brain* brain) : brain(brain), state(), lastUpdateStep(0) {}
    
    void updateState() {
        if (!brain) return;
        
        // Update neural system state
        state.neural.totalNeurons = brain->getTotalNeuronCount();
        state.neural.totalSynapses = brain->getTotalSynapseCount();
        state.neural.activeNeurons = brain->getActiveNeuronCount();
        state.neural.firingNeurons = brain->getFiringNeuronCount();
        state.neural.totalSpikes = brain->getTotalSpikeCount();
        state.neural.averageFiringRate = brain->getAverageFiringRate();
        state.neural.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        
        // Update memory system state
        if (brain->pImpl->workingMemory) {
            state.memory.workingMemoryTraces = brain->pImpl->workingMemory->getActiveTraces();
        }
        if (brain->pImpl->episodicMemory) {
            state.memory.episodicMemoryEpisodes = brain->pImpl->episodicMemory->getEpisodeCount();
        }
        
        // Update neuromodulation state
        if (brain->pImpl->dopamine) {
            state.neuromodulation.dopamineLevel = brain->pImpl->dopamine->getLevel();
        }
        if (brain->pImpl->curiosity) {
            state.neuromodulation.curiosityLevel = brain->pImpl->curiosity->getLevel();
        }
        if (brain->pImpl->novelty) {
            state.neuromodulation.noveltyLevel = brain->pImpl->novelty->getLevel();
        }
        if (brain->pImpl->predictionError) {
            state.neuromodulation.predictionError = brain->pImpl->predictionError->getLevel();
        }
        
        // Update development state
        state.development.stage = brain->getDevelopmentalStage();
        
        // Update timing information
        state.timing.currentStep = brain->pImpl->currentStep;
        state.timing.currentTime = brain->pImpl->currentTime;
        // Wall clock time would be tracked separately
        
        lastUpdateStep = brain->pImpl->currentStep;
    }
    
    const BrainSystemState& getState() const { return state; }
    
    void reset() {
        state = BrainSystemState();
        lastUpdateStep = 0;
    }
    
    bool hasStateChanged() const {
        return lastUpdateStep != state.timing.currentStep;
    }
    
private:
    Brain* brain;
    BrainSystemState state;
    SimulationStep lastUpdateStep;
};

// ========== BRAIN INITIALIZATION COORDINATOR ==========

class BrainInitializer {
public:
    static void initializeMemorySystems(Brain* brain) {
        if (!brain) return;
        
        // Initialize working memory
        if (brain->pImpl->workingMemory) {
            brain->pImpl->workingMemory->initialize(brain);
            brain->pImpl->workingMemory->setCapacity(brain->getTotalNeuronCount() / 10);
        }
        
        // Initialize episodic memory
        if (brain->pImpl->episodicMemory) {
            brain->pImpl->episodicMemory->initialize(brain);
            brain->pImpl->episodicMemory->setMaxEpisodes(1000);
        }
        
        // Initialize associative memory
        if (brain->pImpl->associativeMemory) {
            brain->pImpl->associativeMemory->initialize(brain);
        }
    }
    
    static void initializeCognitionSystems(Brain* brain) {
        if (!brain) return;
        
        // Initialize cognition systems
        if (brain->pImpl->planner) {
            brain->pImpl->planner->initialize(brain);
            brain->pImpl->planner->setPlanningDepth(5);
        }
        
        if (brain->pImpl->conceptFormation) {
            brain->pImpl->conceptFormation->initialize(brain);
        }
        
        if (brain->pImpl->attention) {
            brain->pImpl->attention->initialize(brain);
            brain->pImpl->attention->setInhibitionStrength(0.5f);
            brain->pImpl->attention->setExcitationStrength(1.5f);
        }
    }
    
    static void initializeNeuromodulationSystems(Brain* brain) {
        if (!brain) return;
        
        // Initialize neuromodulation systems
        if (brain->pImpl->novelty) {
            brain->pImpl->novelty->initialize(brain);
        }
        if (brain->pImpl->curiosity) {
            brain->pImpl->curiosity->initialize(brain);
        }
        if (brain->pImpl->dopamine) {
            brain->pImpl->dopamine->initialize(brain);
        }
        if (brain->pImpl->predictionError) {
            brain->pImpl->predictionError->initialize(brain);
        }
    }
    
    static void configurePlasticitySystems(Brain* brain, const Config& config) {
        if (!brain) return;
        
        // Configure STDP parameters
        float ltpWeight = config.getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config.getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config.getOr<float>("stdp_tau", 20.0f);
        
        if (brain->pImpl->stdp) {
            brain->pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
        }
        
        // Configure structural plasticity
        float synaptogenesisRate = config.getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config.getOr<float>("pruning_rate", 0.00001f);
        
        if (brain->pImpl->structuralPlasticity) {
            brain->pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
            brain->pImpl->structuralPlasticity->setPruningRate(pruningRate);
        }
    }
    
    static void configureTimeParameters(TimestepDuration& timestep,
                                       size_t& replayInterval,
                                       size_t& consolidationInterval,
                                       const Config& config) {
        timestep = config.getOr<double>("simulation_timestep", 0.001);
        replayInterval = config.getOr<size_t>("replay_interval", 100);
        consolidationInterval = config.getOr<size_t>("consolidation_interval", 1000);
    }
};

// Move the brain_system_coordinators.cpp content here
// This is a placeholder - the actual implementation needs to be moved from the original file

} // namespace nlm
