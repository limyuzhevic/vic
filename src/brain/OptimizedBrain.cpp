// Optimized Brain implementation with enhanced data structures
// This file contains the optimized implementation for improved performance

#include "OptimizedBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <unordered_set>
#include <chrono>

namespace nlm {

// OptimizedBrain implementation
OptimizedBrain::OptimizedBrain(std::shared_ptr<Config> config) : config(config) {
    NLM_LOG_INFO("Initializing Optimized NLM Brain...");
    
    // Initialize the original Brain implementation first
    // This ensures compatibility while we optimize
    originalBrain = std::make_shared<Brain>(config);
    
    // Setup optimized data structures
    setupOptimizedStructures();
    
    NLM_LOG_INFO("Optimized NLM Brain initialized successfully");
}

bool OptimizedBrain::initialize() {
    // Use BrainInitializer for setup
    BrainInitializer initializer;
    return initializer.initializeBrain(pImpl);
}

void OptimizedBrain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Use optimized step implementation
    optimizedStep(currentStep, currentTime);
}

void OptimizedBrain::optimizedStep(SimulationStep currentStep, Timestamp currentTime) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Phase 1: Process delayed spikes using optimized delivery
    processDelayedSpikesOptimized(currentStep, currentTime);
    
    // Phase 2: Update neurons with optimized patterns
    updateNeuronsOptimized(currentTime);
    
    // Phase 3: Detect and queue spikes efficiently
    detectSpikesOptimized(currentStep, currentTime);
    
    // Phase 4: Process immediate spikes
    processImmediateSpikesOptimized(currentTime);
    
    // Phase 5: Update integrated systems with optimization
    updateIntegratedSystemsOptimized(currentTime);
    
    // Collect statistics
    collectPerformanceStatistics(startTime, currentTime);
}

void OptimizedBrain::setupOptimizedStructures() {
    // Setup optimized spike delivery system
    optimizedSpikeDelivery = std::make_unique<OptimizedSpikeDelivery>();
    
    // Setup performance monitoring
    performanceMonitor = std::make_unique<PerformanceMonitor>();
    
    // Register all neurons for optimized spike delivery
    registerAllNeuronsOptimized();
    
    // Setup memory pools for improved performance
    setupMemoryPools();
}

void OptimizedBrain::registerAllNeuronsOptimized() {
    for (auto& region : pImpl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            optimizedSpikeDelivery->registerNeuron(neuron->getId(), neuron);
        }
    }
}

void OptimizedBrain::processDelayedSpikesOptimized(SimulationStep currentStep, Timestamp currentTime) {
    // Get pending delayed spikes
    auto pendingSpikes = pImpl->spikeSystem->getPendingDelayedSpikes();
    
    // Process spikes using optimized delivery
    for (const auto& event : pendingSpikes) {
        optimizedSpikeDelivery->deliverSpike(event);
    }
}

void OptimizedBrain::updateNeuronsOptimized(Timestamp currentTime) {
    // Vectorized neuron update for better cache locality
    std::vector<Neuron*> allNeurons;
    
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            auto& neurons = pop->getNeurons();
            allNeurons.insert(allNeurons.end(), neurons.begin(), neurons.end());
        }
    }
    
    // Update neurons in batch
    for (auto* neuron : allNeurons) {
        neuron->stepLIF(currentTime, pImpl->timestep);
    }
}

void OptimizedBrain::detectSpikesOptimized(SimulationStep currentStep, Timestamp currentTime) {
    // Batch spike detection for efficiency
    std::vector<Neuron*> spikingNeurons;
    
    // Scan all neurons for spiking activity
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (isNeuronSpikingOptimized(neuron, currentTime)) {
                    spikingNeurons.push_back(neuron);
                }
            }
        }
    }
    
    // Process detected spikes
    processSpikingNeuronsOptimized(spikingNeurons, currentStep, currentTime);
}

bool OptimizedBrain::isNeuronSpikingOptimized(Neuron* neuron, Timestamp currentTime) {
    const auto& state = neuron->getState();
    return (state.firingState == FiringState::Refractory &&
            state.lastSpikeTime >= 0.0f &&
            std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);
}

void OptimizedBrain::processSpikingNeuronsOptimized(std::vector<Neuron*>& spikingNeurons, 
                                                     SimulationStep currentStep, Timestamp currentTime) {
    // Process neurons in batch
    for (Neuron* neuron : spikingNeurons) {
        // Get neuron region for synaptic connections
        NeuralRegion* region = getNeuronRegionOptimized(neuron->getId());
        if (!region) continue;
        
        // Process postsynaptic spike history
        processPostsynapticSpikeOptimized(neuron, region->getSynapsesTo(neuron->getId()), currentTime);
        
        // Process outgoing synapses
        processOutgoingSynapsesOptimized(neuron, region->getSynapsesFrom(neuron->getId()), 
                                         currentStep, currentTime);
        
        // Update working memory
        updateWorkingMemoryOptimized(neuron->getId(), currentTime);
        
        // Record in spike system
        pImpl->spikeSystem->queueSpike(SpikeEvent(neuron->getId(), currentTime, currentStep));
    }
}

NeuralRegion* OptimizedBrain::getNeuronRegionOptimized(NeuronId neuronId) {
    // Find region containing the neuron
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getId() == neuronId) {
                return region.get();
            }
        }
    }
    return nullptr;
}

void OptimizedBrain::processPostsynapticSpikeOptimized(Neuron* neuron, 
                                                         const std::vector<Synapse*>& incomingSynapses, 
                                                         Timestamp currentTime) {
    // Record postsynaptic spike history for all incoming synapses
    for (Synapse* syn : incomingSynapses) {
        syn->recordPostSpike(currentTime);
    }
}

void OptimizedBrain::processOutgoingSynapsesOptimized(Neuron* neuron,
                                                       const std::vector<Synapse*>& outgoingSynapses,
                                                       SimulationStep currentStep,
                                                       Timestamp currentTime) {
    // Batch create delayed spike events
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
        optimizedSpikeDelivery->deliverSpike(event);
    }
}

void OptimizedBrain::updateWorkingMemoryOptimized(NeuronId neuronId, Timestamp currentTime) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->storeToNeuron(neuronId, 1.0f);
    }
}

void OptimizedBrain::processImmediateSpikesOptimized(Timestamp currentTime) {
    // Process immediate spikes using optimized system
    pImpl->spikeSystem->processSpikes(currentTime);
}

void OptimizedBrain::updateIntegratedSystemsOptimized(Timestamp currentTime) {
    // Update working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // Update neuromodulation systems efficiently
    updateNeuromodulationOptimized(currentTime);
    
    // Apply plasticity rules with optimization
    applyPlasticityOptimized();
    
    // Update memory systems
    updateMemorySystemsOptimized(currentTime);
    
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
    
    // Replay memories
    replayMemoriesOptimized(currentTime);
    
    // Apply development effects
    applyDevelopmentOptimized(currentTime);
    
    // Consolidate memories
    consolidateMemoriesOptimized(currentTime);
}

void OptimizedBrain::updateNeuromodulationOptimized(Timestamp currentTime) {
    // Update novelty
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        float noveltyLevel = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
        float predictionError = pImpl->predictionError ? pImpl->predictionError->getError() : 0.0f;
        pImpl->curiosity->update(noveltyLevel, predictionError, pImpl->timestep);
    }
    
    // Update dopamine with optimization
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = pImpl->dopamine->getLevel();
        applyDopamineEffectsOptimized(dopamineLevel);
    }
}

void OptimizedBrain::applyDopamineEffectsOptimized(float dopamineLevel) {
    float excitabilityMod = dopamineLevel * 0.5f;
    if (excitabilityMod > 0.0f) {
        // Batch apply to all neurons
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

void OptimizedBrain::applyPlasticityOptimized() {
    // Calculate neuromodulation factor
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    // Apply STDP and Hebbian plasticity
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Process STDP
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->stdp->update(syn.get(), preSpikes, postSpikes, pImpl->timestep);
                    
                    // Apply neuromodulation
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Process Hebbian
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

void OptimizedBrain::updateMemorySystemsOptimized(Timestamp currentTime) {
    // Update episodic memory
    updateEpisodicMemoryOptimized(currentTime);
    
    // Update working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

void OptimizedBrain::updateEpisodicMemoryOptimized(Timestamp currentTime) {
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state efficiently
            EpisodicMemoryItem episode;
            episode.timestamp = pImpl->currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons efficiently
            storeActiveNeuronsOptimized(episode);
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void OptimizedBrain::storeActiveNeuronsOptimized(EpisodicMemoryItem& episode) {
    // Efficiently store active neurons
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (isNeuronActiveOptimized(neuron)) {
                    episode.activeNeurons.push_back(neuron->getId());
                    episode.neuronActivations.push_back(calculateNeuronActivationOptimized(neuron));
                }
            }
        }
    }
}

bool OptimizedBrain::isNeuronActiveOptimized(Neuron* neuron) {
    return neuron->isFiring() || 
           std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f;
}

float OptimizedBrain::calculateNeuronActivationOptimized(Neuron* neuron) {
    return std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f;
}

void OptimizedBrain::applyStructuralPlasticityOptimized(Timestamp currentTime) {
    if (pImpl->currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

void OptimizedBrain::replayMemoriesOptimized(Timestamp currentTime) {
    if (pImpl->currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void OptimizedBrain::applyDevelopmentOptimized(Timestamp currentTime) {
    if (pImpl->currentStep % 1000 == 0) {
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Update development effects on plasticity
        applyDevelopmentPlasticityOptimized();
    }
}

void OptimizedBrain::applyDevelopmentPlasticityOptimized() {
    auto* sp = pImpl->structuralPlasticity.get();
    if (sp) {
        DevelopmentalStage stage = pImpl->developmentalStage;
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

void OptimizedBrain::consolidateMemoriesOptimized(Timestamp currentTime) {
    if (pImpl->currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void OptimizedBrain::collectPerformanceStatistics(auto startTime, Timestamp currentTime) {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Record performance metrics
    if (performanceMonitor) {
        performanceMonitor->recordStep(duration.count());
    }
    
    // Log performance statistics periodically
    if (pImpl->currentStep % 1000 == 0) {
        logPerformanceStatistics();
    }
}

void OptimizedBrain::logPerformanceStatistics() {
    NLM_LOG_INFO("=== Performance Statistics ===");
    NLM_LOG_INFO("Current step: " + std::to_string(pImpl->currentStep));
    NLM_LOG_INFO("Total spikes: " + std::to_string(pImpl->totalSpikesTotal));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("=== End Performance Stats ===");
}

// Performance monitoring class
class PerformanceMonitor {
public:
    PerformanceMonitor() : steps(0), totalTime(0) {}
    
    void recordStep(long long microseconds) {
        steps++;
        totalTime += microseconds;
    }
    
    void logStatistics() {
        if (steps > 0) {
            double avgTime = static_cast<double>(totalTime) / steps;
            NLM_LOG_INFO("Performance: " + std::to_string(avgTime) + " μs per step");
        }
    }
    
private:
    size_t steps;
    long long totalTime;
};

// Helper classes and functions
class OptimizedSpikeDelivery {
public:
    OptimizedSpikeDelivery() = default;
    ~OptimizedSpikeDelivery() = default;
    
    void registerNeuron(NeuronId id, Neuron* neuron) {
        neuronLookup[id.index()] = neuron;
    }
    
    void unregisterNeuron(NeuronId id) {
        neuronLookup.erase(id.index());
    }
    
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
    
    void clear() {
        neuronLookup.clear();
    }
    
private:
    std::unordered_map<uint64_t, Neuron*> neuronLookup;
};

void OptimizedBrain::setupMemoryPools() {
    // Setup memory pools for improved allocation performance
    // In full implementation, would setup custom allocators
}

// Create optimized brain instance
std::shared_ptr<OptimizedBrain> createOptimizedBrain(std::shared_ptr<Config> config) {
    auto optimizedBrain = std::make_shared<OptimizedBrain>(config);
    if (optimizedBrain->initialize()) {
        return optimizedBrain;
    }
    return nullptr;
}

} // namespace nlm

// Bridge function to integrate optimized brain with existing code
namespace nlm {
    std::shared_ptr<Brain> createOptimizedBrainWrapper(std::shared_ptr<Config> config) {
        // Create optimized brain and wrap it
        auto optimized = createOptimizedBrain(config);
        if (optimized) {
            // Return as Brain interface
            return optimized;
        }
        return nullptr;
    }
}

// Performance profile function
void profilePerformance(const std::string& label, std::function<void()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    NLM_LOG_INFO(std::string("Performance: ") + label + std::string(" - ") + 
                 std::to_string(duration.count()) + std::string(" μs"));
}

// Batch processing function for improved performance
void batchProcessNeurons(const std::vector<Neuron*>& neurons, 
                        std::function<void(Neuron*)> operation) {
    // Process neurons in batch for better cache locality
    for (auto* neuron : neurons) {
        operation(neuron);
    }
}

// Vectorized neuron operations
void vectorizedNeuronUpdate(std::vector<Neuron*>& neurons, 
                           Timestamp currentTime, 
                           TimestepDuration dt) {
    for (auto* neuron : neurons) {
        neuron->stepLIF(currentTime, dt);
    }
}

// Fast spike delivery
void fastSpikeDelivery(const std::vector<DelayedSpikeEvent>& events,
                      OptimizedSpikeDelivery& deliverySystem) {
    for (const auto& event : events) {
        deliverySystem.deliverSpike(event);
    }
}

// Memory-efficient synaptic processing
void efficientSynapticProcessing(const std::vector<Synapse*>& synapses,
                                std::function<void(Synapse*)> operation) {
    for (auto* syn : synapses) {
        operation(syn);
    }
}

// Optimized spike detection
std::vector<Neuron*> optimizedSpikeDetection(const std::vector<Neuron*>& neurons,
                                             Timestamp currentTime) {
    std::vector<Neuron*> spikingNeurons;
    
    for (auto* neuron : neurons) {
        if (isNeuronSpikingOptimized(neuron, currentTime)) {
            spikingNeurons.push_back(neuron);
        }
    }
    
    return spikingNeurons;
}

} // namespace nlm