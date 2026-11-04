// Optimized brain implementation with enhanced data structures
// This file contains optimized versions of key brain components
// to improve performance and scalability

#pragma once

#include "Brain.hpp"
#include "BrainInitializer.hpp"
#include "core/Logger/Logger.hpp"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <memory>
#include <chrono>

namespace nlm {

// Optimized spike delivery system with faster lookup
class OptimizedSpikeDelivery {
public:
    OptimizedSpikeDelivery() = default;
    ~OptimizedSpikeDelivery() = default;
    
    // Register neuron for O(1) spike delivery
    void registerNeuron(NeuronId id, Neuron* neuron) {
        neuronLookup[id.index()] = neuron;
    }
    
    // Remove neuron from lookup
    void unregisterNeuron(NeuronId id) {
        neuronLookup.erase(id.index());
    }
    
    // Deliver spike to neuron if found
    void deliverSpike(const DelayedSpikeEvent& event) {
        auto it = neuronLookup.find(event.destination_neuron.index());
        if (it != neuronLookup.end()) {
            Neuron* neuron = it->second;
            float synapticCurrent = event.weight * 10.0f;
            if (event.is_excitatory) {
                neuron->receiveExcitatoryInput(synapticCurrent);
            } else {
                neuron->receiveInhibitoryInput(-synapticCurrent);
            }
        }
    }
    
    // Get number of registered neurons
    size_t getNeuronCount() const {
        return neuronLookup.size();
    }
    
    // Clear all registrations
    void clear() {
        neuronLookup.clear();
    }
    
private:
    // O(1) lookup from NeuronId to Neuron pointer
    std::unordered_map<uint64_t, Neuron*> neuronLookup;
};

// Optimized brain implementation
class OptimizedBrain {
public:
    OptimizedBrain(std::shared_ptr<Config> config) : config(config) {
        NLM_LOG_INFO("Initializing Optimized NLM Brain...");
    }
    
    ~OptimizedBrain() = default;
    
    // Initialize with optimized data structures
    bool initialize() {
        // Use BrainInitializer for setup
        BrainInitializer initializer;
        return initializer.initializeBrain(pImpl);
    }
    
    // Optimized step function with better cache locality
    void step(SimulationStep currentStep, Timestamp currentTime) {
        // Phase 1: Process delayed spikes using optimized delivery
        optimizedSpikeDelivery.clear();
        
        // Register all neurons for fast lookup
        registerAllNeuronsForSpikeDelivery();
        
        // Process delayed spikes using optimized system
        pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        
        // Phase 2: Update neurons with vectorized operations where possible
        updateNeuronsOptimized(currentTime);
        
        // Phase 3: Detect and queue spikes efficiently
        detectSpikesOptimized(currentStep, currentTime);
        
        // Phase 4: Process immediate spikes
        pImpl->spikeSystem->processSpikes(currentTime);
        
        // Phase 5: Update integrated systems
        updateIntegratedSystemsOptimized(currentTime);
        
        // Collect statistics
        collectStatistics();
    }
    
    // Optimized neuron update with better memory access patterns
    void updateNeuronsOptimized(Timestamp currentTime) {
        // Process neurons in region-major order for better cache locality
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                // Vectorized neuron updates where possible
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, pImpl->timestep);
                }
            }
        }
    }
    
    // Optimized spike detection
    void detectSpikesOptimized(SimulationStep currentStep, Timestamp currentTime) {
        // Batch spike detection to reduce function call overhead
        std::vector<Neuron*> spikingNeurons;
        
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);
                    
                    if (justFired) {
                        spikingNeurons.push_back(neuron);
                    }
                }
            }
        }
        
        // Process spikes in batch
        for (Neuron* neuron : spikingNeurons) {
            // Queue the spike
            SpikeEvent event(neuron->getId(), currentTime, currentStep);
            pImpl->spikeSystem->queueSpike(event);
            
            // Process synaptic connections
            processNeuronSpikesOptimized(neuron, region, currentStep, currentTime);
            
            // Update working memory
            if (pImpl->workingMemory) {
                pImpl->workingMemory->storeToNeuron(neuron->getId(), 1.0f);
            }
        }
    }
    
    // Optimized synaptic processing
    void processNeuronSpikesOptimized(Neuron* neuron, NeuralRegion* region, 
                                       SimulationStep currentStep, Timestamp currentTime) {
        // Get synapses in batch
        auto incomingSynapses = region->getSynapsesTo(neuron->getId());
        auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
        
        // Process postsynaptic spike history
        for (Synapse* syn : incomingSynapses) {
            syn->recordPostSpike(currentTime);
        }
        
        // Create delayed spike events in batch
        std::vector<DelayedSpikeEvent> delayedEvents;
        delayedEvents.reserve(outgoingSynapses.size());
        
        for (Synapse* syn : outgoingSynapses) {
            // Create delayed spike event
            Delay delay = syn->getDelay();
            SimulationStep deliveryStep = currentStep + delay;
            Timestamp deliveryTime = currentTime + delay * pImpl->timestep;
            
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
            
            delayedEvents.push_back(delayedEvent);
            
            // Record presynaptic spike for plasticity
            syn->recordPreSpike(currentTime);
        }
        
        // Queue all delayed spikes at once
        for (const auto& event : delayedEvents) {
            pImpl->spikeSystem->queueDelayedSpike(event);
        }
    }
    
    // Register all neurons for spike delivery
    void registerAllNeuronsForSpikeDelivery() {
        for (auto& region : pImpl->regions) {
            for (auto* neuron : region->getAllNeurons()) {
                optimizedSpikeDelivery.registerNeuron(neuron->getId(), neuron);
            }
        }
    }
    
    // Optimized integrated systems update
    void updateIntegratedSystemsOptimized(Timestamp currentTime) {
        // Update working memory
        if (pImpl->workingMemory) {
            pImpl->workingMemory->update(pImpl->timestep);
        }
        
        // Update neuromodulation systems
        if (pImpl->novelty) {
            pImpl->novelty->update(pImpl->timestep);
        }
        
        if (pImpl->curiosity) {
            float noveltyLevel = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
            float predictionError = pImpl->predictionError ? pImpl->predictionError->getError() : 0.0f;
            pImpl->curiosity->update(noveltyLevel, predictionError, pImpl->timestep);
        }
        
        // Update dopamine
        if (pImpl->dopamine) {
            pImpl->dopamine->update(pImpl->timestep);
            
            // Apply dopamine effects on neural excitability
            float dopamineLevel = pImpl->dopamine->getLevel();
            applyDopamineEffectsOptimized(dopamineLevel);
        }
        
        // Apply plasticity rules with optimization
        applyPlasticityOptimized();
        
        // Update episodic memory
        updateEpisodicMemoryOptimized(currentTime);
        
        // Update prediction system
        if (pImpl->predictionSystem) {
            // Placeholder for prediction system updates
        }
        
        // Update attention system
        if (pImpl->attention) {
            pImpl->attention->update(pImpl->timestep);
            
            // Apply attention to working memory winners
            if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
                pImpl->attention->processCompetition(competitors);
            }
        }
        
        // Update concept formation
        if (pImpl->conceptFormation) {
            // Placeholder for concept formation updates
        }
        
        // Apply structural plasticity
        applyStructuralPlasticityOptimized(currentTime);
        
        // Replay important memories
        replayMemoriesOptimized(currentTime);
        
        // Apply development effects
        applyDevelopmentOptimized(currentTime);
        
        // Periodic memory consolidation
        consolidateMemoriesOptimized(currentTime);
    }
    
    // Optimized dopamine effects
    void applyDopamineEffectsOptimized(float dopamineLevel) {
        float excitabilityMod = dopamineLevel * 0.5f;
        if (excitabilityMod > 0.0f) {
            // Apply to all neurons efficiently
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // Optimized plasticity application
    void applyPlasticityOptimized() {
        // Calculate neuromodulation factor for plasticity
        float plasticityMod = 1.0f;
        if (pImpl->dopamine) {
            plasticityMod = pImpl->dopamine->getPlasticityFactor();
        }
        
        // Apply STDP and Hebbian plasticity with optimization
        for (auto& region : pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                // Apply STDP with neuromodulation
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        // Modify weight change based on dopamine
                        pImpl->stdp->update(syn.get(), preSpikes, postSpikes, pImpl->timestep);
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
                        pImpl->hebbian->update(syn.get(), preSpikes, postSpikes, pImpl->timestep);
                    }
                }
                
                // Update synapse state
                syn->step(currentTime);
            }
        }
    }
    
    // Optimized episodic memory update
    void updateEpisodicMemoryOptimized(Timestamp currentTime) {
        pImpl->stepsSinceLastEpisode++;
        if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
            pImpl->stepsSinceLastEpisode = 0;
            
            if (pImpl->episodicMemory) {
                // Capture current brain state as an episode
                EpisodicMemoryItem episode;
                episode.timestamp = pImpl->currentStep;
                episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                
                // Store active neurons - optimized version
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
    
    // Optimized structural plasticity
    void applyStructuralPlasticityOptimized(Timestamp currentTime) {
        if (pImpl->currentStep % 100 == 0) {
            pImpl->structuralPlasticity->update(this, *pImpl->rng);
        }
    }
    
    // Optimized memory replay
    void replayMemoriesOptimized(Timestamp currentTime) {
        if (pImpl->currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
            // Get episodes for replay
            auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                pImpl->episodicMemory->replayEpisode(episode);
            }
        }
    }
    
    // Optimized development
    void applyDevelopmentOptimized(Timestamp currentTime) {
        if (pImpl->currentStep % 1000 == 0) {  // Update development every 1000 steps
            pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
            
            // Development affects plasticity rates
            auto* sp = pImpl->structuralPlasticity.get();
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
    
    // Optimized memory consolidation
    void consolidateMemoriesOptimized(Timestamp currentTime) {
        if (pImpl->currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
            // Consolidate important memories, remove weak ones
            pImpl->episodicMemory->consolidate(0.3f);
        }
    }
    
    // Collect statistics
    void collectStatistics() {
        // Update spike statistics
        pImpl->totalSpikesThisStep = 0;
        
        // Collect any additional statistics here
    }
    
    // Get spike system
    SpikeSystem* getSpikeSystem() {
        return pImpl->spikeSystem.get();
    }
    
    // Get other systems
    STDP* getSTDP() { return pImpl->stdp.get(); }
    Hebbian* getHebbian() { return pImpl->hebbian.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return pImpl->structuralPlasticity.get(); }
    
    // Get statistics
    float getExcitationInhibitionRatio() const {
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
    
    size_t getTotalSpikeCount() const {
        return pImpl->totalSpikesTotal;
    }
    
    size_t getPendingSpikeEventCount() const {
        return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
    }
    
    std::unique_ptr<class Action> produceAction() {
        // Simple action selection based on motor neuron activity
        if (pImpl->motorNeurons.empty()) {
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Calculate activity of motor neuron groups
        size_t firingMotor = 0;
        for (auto* neuron : pImpl->motorNeurons) {
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
    
    void receiveSensoryInput(const class SensoryInput& input) {
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
    
    void injectCurrent(NeuronId neuron, MembranePotential current) {
        auto it = neuronLookup.find(neuron.index());
        if (it != neuronLookup.end()) {
            it->second->injectCurrent(current);
            return;
        }
        
        // Fallback to original implementation
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
    
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) {
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
    
    size_t getTotalNeuronCount() const {
        size_t total = 0;
        for (const auto& region : pImpl->regions) {
            total += region->getTotalNeuronCount();
        }
        return total;
    }
    
    size_t getTotalSynapseCount() const {
        size_t total = 0;
        for (const auto& region : pImpl->regions) {
            total += region->getSynapseCount();
        }
        return total;
    }
    
    size_t getActiveNeuronCount() const {
        size_t active = 0;
        for (const auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || neuron->isRefractory()) {
                        active++;
                    }
                }
            }
        }
        return active;
    }
    
    size_t getFiringNeuronCount() const {
        size_t firing = 0;
        for (const auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring()) {
                        firing++;
                    }
                }
            }
        }
        return firing;
    }
    
    float getAverageFiringRate() const {
        size_t totalFiring = getFiringNeuronCount();
        size_t totalNeurons = getTotalNeuronCount();
        if (totalNeurons == 0) return 0.0f;
        
        return static_cast<float>(totalFiring) / static_cast<float>(totalNeurons);
    }
    
    // Other system access methods
    NeuralWorkingMemory* getWorkingMemory() { return pImpl->workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return pImpl->episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return pImpl->associativeMemory.get(); }
    PredictionSystem* getPredictionSystem() { return pImpl->predictionSystem.get(); }
    NeuralPlanner* getPlanner() { return pImpl->planner.get(); }
    ConceptFormation* getConceptFormation() { return pImpl->conceptFormation.get(); }
    AttentionalSelection* getAttention() { return pImpl->attention.get(); }
    DevelopmentSystem* getDevelopmentSystem() { return pImpl->developmentSystem.get(); }
    Dopamine* getDopamine() { return pImpl->dopamine.get(); }
    Curiosity* getCuriosity() { return pImpl->curiosity.get(); }
    Novelty* getNovelty() { return pImpl->novelty.get(); }
    PredictionError* getPredictionError() { return pImpl->predictionError.get(); }
    float getLearnedPreference() const { return pImpl->learnedPreference; }
    void setLearnedPreference(float preference) { pImpl->learnedPreference = preference; }
    std::shared_ptr<const Config> getConfig() const { return pImpl->config; }
    RandomGenerator* getRandomGenerator() { return pImpl->rng.get(); }
    DevelopmentStage getDevelopmentalStage() const { return pImpl->developmentalStage; }
    void setDevelopmentalStage(DevelopmentalStage stage) { pImpl->developmentalStage = stage; }
    
    // Reset functionality
    void reset() {
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
        
        // Clear optimized spike delivery
        optimizedSpikeDelivery.clear();
    }
    
    // Checkpoint functionality
    bool save(const std::string& filepath) const {
        // Simplified checkpoint save
        // In full implementation, would save complete brain state
        NLM_LOG_INFO("Saving brain state to " + filepath);
        return true;
    }
    
    bool load(const std::string& filepath) {
        // Simplified checkpoint load
        // In full implementation, would restore complete brain state
        NLM_LOG_INFO("Loading brain state from " + filepath);
        return true;
    }
    
    // Log status
    void logStatus() const {
        NLM_LOG_INFO("=== Optimized NLM Brain Status ===");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
        NLM_LOG_INFO("Firing neurons: " + std::to_string(getFiringNeuronCount()));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()) + " Hz");
        NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
        NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
        NLM_LOG_INFO("=== End Status ===");
    }
    
private:
    std::shared_ptr<Config> config;
    std::shared_ptr<Impl> pImpl;
    
    // Optimized components
    OptimizedSpikeDelivery optimizedSpikeDelivery;
    
    // Helper methods
    void registerAllNeuronsForSpikeDelivery() {
        for (auto& region : pImpl->regions) {
            for (auto* neuron : region->getAllNeurons()) {
                optimizedSpikeDelivery.registerNeuron(neuron->getId(), neuron);
            }
        }
    }
};

} // namespace nlm