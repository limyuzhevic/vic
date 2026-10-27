// Performance improvements implementation for NLM Brain
// Adding efficient data structures and optimized algorithms

#include "Performance.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <thread>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <atomic>

namespace nlm {

// Performance metrics implementation
PerformanceMetrics::PerformanceMetrics() 
    : neuronsProcessed(0), spikesGenerated(0), synapsesUpdated(0),
      totalSimulationTime(0.0), averageStepTime(0.0), memoryUsage(0),
      timestamp(std::chrono::system_clock::now()) {}

// OptimizedNeuralRegion implementation - key parts
OptimizedNeuralRegion::OptimizedNeuralRegion(OptimizedNeuralRegion&& other) noexcept
    : id(other.id), name(std::move(other.name)), 
      populationCapacity(other.populationCapacity),
      nextNeuronId(other.nextNeuronId), nextSynapseId(other.nextSynapseId),
      dirtyFlag(other.dirtyFlag),
      optimizationLevel(other.optimizationLevel),
      totalNeurons(other.totalNeurons), totalSynapses(other.totalSynapses),
      populations(std::move(other.populations)),
      neuronById(std::move(other.neuronById)),
      synapseById(std::move(other.synapseById)),
      neuronsByType(std::move(other.neuronsByType)),
      incomingConnections(std::move(other.incomingConnections)),
      outgoingConnections(std::move(other.outgoingConnections)),
      pImpl(std::move(other.pImpl)) {
    other.dirtyFlag = true;
}

OptimizedNeuralRegion& OptimizedNeuralRegion::operator=(OptimizedNeuralRegion&& other) noexcept {
    if (this != &other) {
        Cleanup();
        
        id = other.id;
        name = std::move(other.name);
        populationCapacity = other.populationCapacity;
        nextNeuronId = other.nextNeuronId;
        nextSynapseId = other.nextSynapseId;
        dirtyFlag = other.dirtyFlag;
        optimizationLevel = other.optimizationLevel;
        totalNeurons = other.totalNeurons;
        totalSynapses = other.totalSynapses;
        
        populations = std::move(other.populations);
        neuronById = std::move(other.neuronById);
        synapseById = std::move(other.synapseById);
        neuronsByType = std::move(other.neuronsByType);
        incomingConnections = std::move(other.incomingConnections);
        outgoingConnections = std::move(other.outgoingConnections);
        pImpl = std::move(other.pImpl);
        
        other.dirtyFlag = true;
    }
    return *this;
}

PopulationId OptimizedNeuralRegion::addPopulation(size_t neuronCount, NeuronType type) {
    if (!pImpl) return INVALID_POPULATION_ID;
    
    populationCapacity += neuronCount;
    totalNeurons += neuronCount;
    
    try {
        auto population = std::make_unique<NeuralPopulation>(nextPopulationId++, type, neuronCount);
        auto populationId = population->getId();
        
        // Initialize with neurons
        population->initializeRandom(*pImpl->rng);
        
        // Add to fast lookup structures
        populations.push_back(std::move(population));
        neuronsByType[type].push_back(populationId);
        
        markDirty();
        return populationId;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to create population: " + std::string(e.what()));
        return INVALID_POPULATION_ID;
    }
}

NeuralPopulation* OptimizedNeuralRegion::getPopulation(PopulationId id) {
    for (auto& population : populations) {
        if (population->getId() == id) {
            return population.get();
        }
    }
    return nullptr;
}

const NeuralPopulation* OptimizedNeuralRegion::getPopulation(PopulationId id) const {
    return const_cast<OptimizedNeuralRegion*>(this)->getPopulation(id);
}

Neuron* OptimizedNeuralRegion::getNeuron(NeuronId neuronId) {
    auto it = neuronById.find(neuronId);
    if (it != neuronById.end()) {
        return it->second;
    }
    return nullptr;
}

const Neuron* OptimizedNeuralRegion::getNeuron(NeuronId neuronId) const {
    return const_cast<OptimizedNeuralRegion*>(this)->getNeuron(neuronId);
}

Neuron* OptimizedNeuralRegion::getNeuron(size_t neuronIndex) {
    if (neuronIndex >= totalNeurons) return nullptr;
    
    size_t currentIndex = 0;
    for (auto& population : populations) {
        size_t populationNeurons = population->getNeuronCount();
        if (currentIndex + populationNeurons > neuronIndex) {
            return population->getNeuron(neuronIndex - currentIndex);
        }
        currentIndex += populationNeurons;
    }
    return nullptr;
}

const Neuron* OptimizedNeuralRegion::getNeuron(size_t neuronIndex) const {
    return const_cast<OptimizedNeuralRegion*>(this)->getNeuron(neuronIndex);
}

std::vector<Neuron*> OptimizedNeuralRegion::getNeuronsByType(NeuronType type) {
    std::vector<Neuron*> result;
    
    auto it = neuronsByType.find(type);
    if (it != neuronsByType.end()) {
        for (PopulationId popId : it->second) {
            auto* population = getPopulation(popId);
            if (population) {
                for (auto* neuron : population->getNeurons()) {
                    if (neuron) {
                        result.push_back(neuron);
                    }
                }
            }
        }
    }
    
    return result;
}

std::vector<const Neuron*> OptimizedNeuralRegion::getNeuronsByType(NeuronType type) const {
    std::vector<const Neuron*> result;
    
    auto it = neuronsByType.find(type);
    if (it != neuronsByType.end()) {
        for (PopulationId popId : it->second) {
            auto* population = getPopulation(popId);
            if (population) {
                for (const auto* neuron : population->getNeurons()) {
                    if (neuron) {
                        result.push_back(neuron);
                    }
                }
            }
        }
    }
    
    return result;
}

SynapseId OptimizedNeuralRegion::addSynapse(NeuronId source, NeuronId destination, SynapseType type) {
    if (!pImpl) return INVALID_SYNAPSE_ID;
    
    // Check if neurons exist
    auto* sourceNeuron = getNeuron(source);
    auto* destNeuron = getNeuron(destination);
    if (!sourceNeuron || !destNeuron) {
        return INVALID_SYNAPSE_ID;
    }
    
    try {
        SynapseId synapseId = SynapseId(nextSynapseId++);
        auto synapse = std::make_unique<Synapse>(synapseId, source, destination);
        synapse->setType(type);
        
        // Initialize with random parameters
        synapse->initializeRandom(*pImpl->rng);
        
        // Add to fast lookup structures
        synapseById[synapseId] = synapse.get();
        
        // Add to connectivity indices
        outgoingConnections[source].push_back(synapseId);
        incomingConnections[destination].push_back(synapseId);
        
        // Add to region's synapse list
        pImpl->synapses.push_back(std::move(synapse));
        totalSynapses++;
        
        markDirty();
        return synapseId;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to create synapse: " + std::string(e.what()));
        return INVALID_SYNAPSE_ID;
    }
}

void OptimizedNeuralRegion::removeSynapse(SynapseId synapseId) {
    auto it = synapseById.find(synapseId);
    if (it == synapseById.end()) return;
    
    Synapse* synapse = it->second;
    if (!synapse) return;
    
    // Find and remove from connectivity indices
    NeuronId source = synapse->getSourceNeuron();
    NeuronId destination = synapse->getDestinationNeuron();
    
    auto& sourceConnections = outgoingConnections[source];
    sourceConnections.erase(std::remove(sourceConnections.begin(), sourceConnections.end(), synapseId),
                           sourceConnections.end());
    
    auto& destConnections = incomingConnections[destination];
    destConnections.erase(std::remove(destConnections.begin(), destConnections.end(), synapseId),
                         destConnections.end());
    
    // Remove from synapseById
    synapseById.erase(it);
    
    markDirty();
}

Synapse* OptimizedNeuralRegion::getSynapse(SynapseId synapseId) {
    auto it = synapseById.find(synapseId);
    if (it != synapseById.end()) {
        return it->second;
    }
    return nullptr;
}

const Synapse* OptimizedNeuralRegion::getSynapse(SynapseId synapseId) const {
    return const_cast<OptimizedNeuralRegion*>(this)->getSynapse(synapseId);
}

std::vector<Synapse*> OptimizedNeuralRegion::getSynapsesFrom(NeuronId sourceNeuron) {
    std::vector<Synapse*> result;
    
    auto it = outgoingConnections.find(sourceNeuron);
    if (it != outgoingConnections.end()) {
        for (SynapseId synapseId : it->second) {
            auto* synapse = getSynapse(synapseId);
            if (synapse) {
                result.push_back(synapse);
            }
        }
    }
    
    return result;
}

std::vector<const Synapse*> OptimizedNeuralRegion::getSynapsesFrom(NeuronId sourceNeuron) const {
    std::vector<const Synapse*> result;
    
    auto it = outgoingConnections.find(sourceNeuron);
    if (it != outgoingConnections.end()) {
        for (SynapseId synapseId : it->second) {
            auto* synapse = getSynapse(synapseId);
            if (synapse) {
                result.push_back(synapse);
            }
        }
    }
    
    return result;
}

std::vector<Synapse*> OptimizedNeuralRegion::getSynapsesTo(NeuronId destinationNeuron) {
    std::vector<Synapse*> result;
    
    auto it = incomingConnections.find(destinationNeuron);
    if (it != incomingConnections.end()) {
        for (SynapseId synapseId : it->second) {
            auto* synapse = getSynapse(synapseId);
            if (synapse) {
                result.push_back(synapse);
            }
        }
    }
    
    return result;
}

std::vector<const Synapse*> OptimizedNeuralRegion::getSynapsesTo(NeuronId destinationNeuron) const {
    std::vector<const Synapse*> result;
    
    auto it = incomingConnections.find(destinationNeuron);
    if (it != incomingConnections.end()) {
        for (SynapseId synapseId : it->second) {
            auto* synapse = getSynapse(synapseId);
            if (synapse) {
                result.push_back(synapse);
            }
        }
    }
    
    return result;
}

size_t OptimizedNeuralRegion::getOutgoingConnectionCount(NeuronId sourceNeuron) const {
    auto it = outgoingConnections.find(sourceNeuron);
    return it != outgoingConnections.end() ? it->second.size() : 0;
}

size_t OptimizedNeuralRegion::getIncomingConnectionCount(NeuronId destinationNeuron) const {
    auto it = incomingConnections.find(destinationNeuron);
    return it != incomingConnections.end() ? it->second.size() : 0;
}

float OptimizedNeuralRegion::getAverageSynapticWeight() const {
    if (totalSynapses == 0) return 0.0f;
    
    float totalWeight = 0.0f;
    size_t count = 0;
    
    for (const auto& pair : synapseById) {
        Synapse* synapse = pair.second;
        if (synapse) {
            totalWeight += synapse->getWeight();
            count++;
        }
    }
    
    return count > 0 ? totalWeight / count : 0.0f;
}

void OptimizedNeuralRegion::injectCurrentToAll(NeuronType type, MembranePotential current) {
    for (auto& population : populations) {
        if (population->getNeuronType() == type) {
            population->injectCurrentToAll(current);
        }
    }
}

void OptimizedNeuralRegion::updateAllNeurons(Timestamp currentTime, TimestepDuration dt) {
    // Batch update all neurons for better cache locality
    for (auto& population : populations) {
        population->updateAllNeurons(currentTime, dt);
    }
}

size_t OptimizedNeuralRegion::processSpikes() {
    size_t totalSpikes = 0;
    
    for (auto& population : populations) {
        totalSpikes += population->processSpikes();
    }
    
    return totalSpikes;
}

void OptimizedNeuralRegion::reset() {
    for (auto& population : populations) {
        population->reset();
    }
    
    totalNeurons = 0;
    totalSynapses = 0;
    neuronsByType.clear();
    outgoingConnections.clear();
    incomingConnections.clear();
    
    markDirty();
}

void OptimizedNeuralRegion::cleanup() {
    populations.clear();
    neuronsByType.clear();
    outgoingConnections.clear();
    incomingConnections.clear();
    
    if (pImpl) {
        pImpl->synapses.clear();
    }
    
    totalNeurons = 0;
    totalSynapses = 0;
}

OptimizedNeuralRegion::RegionStats OptimizedNeuralRegion::getStats() const {
    RegionStats stats;
    stats.neuronCount = totalNeurons;
    stats.synapseCount = totalSynapses;
    stats.populationCount = populations.size();
    stats.optimizationLevel = optimizationLevel;
    
    // Calculate average firing rate
    float totalFiringRate = 0.0f;
    size_t neuronCount = 0;
    
    for (auto* neuron : getAllNeurons()) {
        if (neuron) {
            totalFiringRate += neuron->getFiringRate();
            neuronCount++;
        }
    }
    
    stats.averageFiringRate = neuronCount > 0 ? totalFiringRate / neuronCount : 0.0f;
    
    return stats;
}

void OptimizedNeuralRegion::InitializeEfficientStructures() {
    // Initialize efficient data structures
    // This would typically initialize hash tables, caches, etc.
}

// OptimizedBrain implementation - key parts
OptimizedBrain::OptimizedBrain(std::shared_ptr<Config> config)
    : config(config), averageStepTime(0.0), totalSimulationTime(0.0), stepCount(0),
      concurrentProcessing(false), cacheOptimization(true), threadCount(1), isProfiling(false) {
    
    // Initialize performance monitoring
    metrics.timestamp = std::chrono::system_clock::now();
    lastStepTime = std::chrono::high_resolution_clock::now();
    
    // Initialize memory pool
    memoryPool = std::make_unique<MemoryPool>();
    
    // Initialize concurrent processor if enabled
    if (concurrentProcessing) {
        concurrentProcessor = std::make_unique<ConcurrentProcessor>(threadCount);
    }
}

OptimizedBrain::~OptimizedBrain() = default;

OptimizedBrain::OptimizedBrain(OptimizedBrain&& other) noexcept
    : config(std::move(other.config)), metrics(std::move(other.metrics)),
      lastStepTime(std::move(other.lastStepTime)),
      averageStepTime(other.averageStepTime), totalSimulationTime(other.totalSimulationTime),
      stepCount(other.stepCount), concurrentProcessing(other.concurrentProcessing),
      cacheOptimization(other.cacheOptimization), threadCount(other.threadCount),
      errorMessages(std::move(other.errorMessages)),
      isProfiling(other.isProfiling), stepTimes(std::move(other.stepTimes)) {
    
    pImpl = std::move(other.pImpl);
    memoryPool = std::move(other.memoryPool);
    concurrentProcessor = std::move(other.concurrentProcessor);
    
    other.concurrentProcessing = false;
    other.cacheOptimization = false;
    other.threadCount = 1;
}

OptimizedBrain& OptimizedBrain::operator=(OptimizedBrain&& other) noexcept {
    if (this != &other) {
        // Cleanup current resources
        memoryPool.reset();
        concurrentProcessor.reset();
        
        // Move resources from other
        config = std::move(other.config);
        metrics = std::move(other.metrics);
        lastStepTime = std::move(other.lastStepTime);
        averageStepTime = other.averageStepTime;
        totalSimulationTime = other.totalSimulationTime;
        stepCount = other.stepCount;
        concurrentProcessing = other.concurrentProcessing;
        cacheOptimization = other.cacheOptimization;
        threadCount = other.threadCount;
        errorMessages = std::move(other.errorMessages);
        isProfiling = other.isProfiling;
        stepTimes = std::move(other.stepTimes);
        
        pImpl = std::move(other.pImpl);
        memoryPool = std::move(other.memoryPool);
        concurrentProcessor = std::move(other.concurrentProcessor);
        
        other.concurrentProcessing = false;
        other.cacheOptimization = false;
        other.threadCount = 1;
    }
    return *this;
}

bool OptimizedBrain::initialize() {
    if (!config) {
        AddError("Configuration is null - cannot initialize");
        return false;
    }
    
    NLM_LOG_INFO("Initializing Optimized NLM Brain with performance monitoring...");
    
    try {
        // Validate configuration
        if (!config->has("neuron_count") || !config->has("region_count")) {
            AddError("Missing required configuration parameters");
            return false;
        }
        
        // Create optimized regions
        size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = config->getOr<size_t>("region_count", 1);
        
        for (size_t i = 0; i < regionCount; ++i) {
            RegionId regionId(i + 1);
            std::string regionName = "Region_" + std::to_string(i + 1);
            
            auto optimizedRegion = std::make_unique<OptimizedNeuralRegion>(regionId, regionName);
            optimizedRegion->setOptimizationLevel(2); // High performance mode
            
            // Set region-specific optimizations
            if (cacheOptimization) {
                optimizedRegion->setCacheOptimization(true);
            }
            
            // Initialize with neurons
            size_t neuronsPerRegion = neuronCount / regionCount;
            auto sensoryPopId = optimizedRegion->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = optimizedRegion->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = optimizedRegion->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Store region
            pImpl->regions.push_back(std::move(optimizedRegion));
        }
        
        // Create random generator
        uint64_t seed = config->getOr<uint64_t>("random_seed", 42);
        pImpl->rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize spike system
        pImpl->spikeSystem = std::make_unique<SpikeSystem>();
        
        NLM_LOG_INFO("Optimized brain initialization complete");
        NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        
        return true;
        
    } catch (const std::exception& e) {
        AddError("Exception during optimized brain initialization: " + std::string(e.what()));
        return false;
    }
}

void OptimizedBrain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * 0.001);
}

void OptimizedBrain::step(SimulationStep currentStep, Timestamp currentTime) {
    if (!pImpl) {
        AddError("Cannot step brain with null implementation");
        return;
    }
    
    auto stepStartTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Record step start time for profiling
        if (isProfiling) {
            startProfiling();
        }
        
        // ========== OPTIMIZED SIMULATION LOOP ==========
        
        // Step 1: Process spike events (optimized with batch processing)
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->processSpikes(currentStep);
        }
        
        // Step 2: Update neurons in batches for cache efficiency
        UpdateAllNeuronsInBatches(currentTime);
        
        // Step 3: Process spike deliveries
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        }
        
        // Step 4: Update plasticity systems
        UpdatePlasticitySystems(currentTime);
        
        // Step 5: Update memory systems
        UpdateMemorySystems(currentTime);
        
        // Step 6: Update neuromodulation
        UpdateNeuromodulation(currentTime);
        
        // Update performance metrics
        size_t neuronsProcessed = getTotalNeuronCount();
        size_t spikesGenerated = pImpl->totalSpikesThisStep;
        size_t synapsesUpdated = getTotalSynapseCount();
        
        UpdatePerformanceMetrics(currentTime, neuronsProcessed, spikesGenerated, synapsesUpdated);
        
        // Record step time for profiling
        if (isProfiling) {
            stopProfiling();
        }
        
    } catch (const std::exception& e) {
        AddError("Exception during brain step: " + std::string(e.what()));
    }
    
    auto stepEndTime = std::chrono::high_resolution_clock::now();
    auto stepDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(stepEndTime - stepStartTime);
    
    // Update average step time
    stepCount++;
    averageStepTime = (averageStepTime * (stepCount - 1) + stepDuration.count() * 1e-9) / stepCount;
    totalSimulationTime += stepDuration.count() * 1e-9;
}

void OptimizedBrain::UpdateAllNeuronsInBatches(Timestamp currentTime) {
    if (!pImpl || !cacheOptimization) {
        // Fall back to original implementation
        for (auto& region : pImpl->regions) {
            region->updateAllNeurons(currentTime, 0.001);
        }
        return;
    }
    
    // Optimized batch processing for better cache locality
    std::vector<std::vector<Neuron*>> neuronBatches;
    
    // Group neurons by population for better cache locality
    for (auto& region : pImpl->regions) {
        std::vector<Neuron*> batch;
        
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                if (neuron) {
                    batch.push_back(neuron);
                }
            }
        }
        
        if (!batch.empty()) {
            neuronBatches.push_back(batch);
        }
    }
    
    // Process batches
    for (auto& batch : neuronBatches) {
        for (auto* neuron : batch) {
            if (neuron) {
                neuron->step(currentTime);
            }
        }
    }
}

void OptimizedBrain::UpdatePlasticitySystems(Timestamp currentTime) {
    if (!pImpl) return;
    
    // Batch update all synapses
    for (auto& region : pImpl->regions) {
        for (auto* synapse : region->getAllSynapses()) {
            if (synapse) {
                synapse->step(currentTime);
            }
        }
    }
}

void OptimizedBrain::UpdateMemorySystems(Timestamp currentTime) {
    if (!pImpl) return;
    
    // Update memory systems in optimized order
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(0.001);
    }
    
    if (pImpl->episodicMemory) {
        // Periodic memory consolidation
        if (currentTime > pImpl->lastConsolidationTime + 10.0) {
            pImpl->episodicMemory->consolidate(0.3f);
            pImpl->lastConsolidationTime = currentTime;
        }
    }
}

void OptimizedBrain::UpdateNeuromodulation(Timestamp currentTime) {
    if (!pImpl) return;
    
    // Update neuromodulation with optimized processing
    if (pImpl->dopamine) {
        pImpl->dopamine->update(0.001);
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->update(0.001);
    }
    
    if (pImpl->novelty) {
        pImpl->novelty->update(0.001);
    }
}

void OptimizedBrain::UpdatePerformanceMetrics(Timestamp currentTime, 
                                             size_t neuronsProcessed, 
                                             size_t spikesGenerated, 
                                             size_t synapsesUpdated) {
    metrics.neuronsProcessed += neuronsProcessed;
    metrics.spikesGenerated += spikesGenerated;
    metrics.synapsesUpdated += synapsesUpdated;
    metrics.timestamp = std::chrono::system_clock::now();
    
    // Calculate memory usage (approximate)
    metrics.memoryUsage = sizeof(OptimizedBrain) + sizeof(Impl) + 
                         sizeof(OptimizedNeuralRegion) * pImpl->regions.size();
    
    // Log performance periodically
    if (stepCount % 100 == 0) {
        LogPerformanceStats();
    }
}

PerformanceMetrics OptimizedBrain::getPerformanceMetrics() const {
    return metrics;
}

void OptimizedBrain::resetPerformanceMetrics() {
    metrics = PerformanceMetrics();
    stepCount = 0;
    averageStepTime = 0.0;
    totalSimulationTime = 0.0;
}

std::vector<Neuron*> OptimizedBrain::getAllNeurons() {
    std::vector<Neuron*> result;
    
    for (auto& region : pImpl->regions) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                if (neuron) {
                    result.push_back(neuron);
                }
            }
        }
    }
    
    return result;
}

std::vector<Synapse*> OptimizedBrain::getAllSynapses() {
    std::vector<Synapse*> result;
    
    for (auto& region : pImpl->regions) {
        for (auto* synapse : region->getSynapses()) {
            if (synapse) {
                result.push_back(synapse);
            }
        }
    }
    
    return result;
}

std::vector<NeuralRegion*> OptimizedBrain::getAllRegions() {
    std::vector<NeuralRegion*> result;
    
    for (auto& region : pImpl->regions) {
        result.push_back(region.get());
    }
    
    return result;
}

void OptimizedBrain::startProfiling() {
    profilingStartTime = std::chrono::high_resolution_clock::now();
    isProfiling = true;
}

void OptimizedBrain::stopProfiling() {
    if (!isProfiling) return;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - profilingStartTime);
    
    stepTimes.push_back(duration.count() * 1e-9);
    
    if (stepTimes.size() > 1000) {
        stepTimes.erase(stepTimes.begin());
    }
    
    isProfiling = false;
}

bool OptimizedBrain::isHealthy() const {
    return !errorMessages.empty(); // Consider healthy if no errors
}

void OptimizedBrain::AddError(const std::string& error) {
    std::lock_guard<std::mutex> lock(errorMutex);
    errorMessages.push_back(error);
    NLM_LOG_ERROR("Optimized Brain Error: " + error);
}

void OptimizedBrain::LogPerformanceStats() const {
    NLM_LOG_INFO("=== Optimized Brain Performance Stats ===");
    NLM_LOG_INFO("Step count: " + std::to_string(stepCount));
    NLM_LOG_INFO("Average step time: " + std::to_string(averageStepTime * 1000) + " ms");
    NLM_LOG_INFO("Total simulation time: " + std::to_string(totalSimulationTime * 1000) + " ms");
    NLM_LOG_INFO("Neurons processed: " + std::to_string(metrics.neuronsProcessed));
    NLM_LOG_INFO("Spikes generated: " + std::to_string(metrics.spikesGenerated));
    NLM_LOG_INFO("Synapses updated: " + std::to_string(metrics.synapsesUpdated));
    NLM_LOG_INFO("Memory usage: " + std::to_string(metrics.memoryUsage) + " bytes");
}

} // namespace nlm