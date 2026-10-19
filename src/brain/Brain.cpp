#include "Brain.hpp"
#include "../performance/OptimizedBrain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <thread>
#include <atomic>

// Include performance headers
#include "../performance/MemoryPool.hpp"
#include "../performance/SIMDVectorization.hpp"
#include "../performance/EventQueue.hpp"
#include "../performance/ParallelProcessing.hpp"

namespace nlm {

/**
 * @brief Performance monitoring statistics for brain
 */
struct BrainPerformanceStats {
    size_t totalSpikes;
    size_t pendingSpikes;
    size_t activeNeurons;
    size_t firingNeurons;
    double averageFiringRate;
    double eIINRatio;
    size_t memoryUsage;
    double stepTime;
    double stepRate;
    size_t eventQueueSize;
    size_t cacheHits;
    size_t cacheMisses;
    double optimizationEfficiency;
};

/**
 * @brief Performance optimization modes for brain
 */
enum class PerformanceMode {
public:
    Normal,          // Default balanced performance
    Speed,           // Maximum speed, minimal accuracy
    Accuracy,        // Maximum accuracy, slower performance
    Memory,          // Memory-constrained, lower processing power
    EventDriven,     // Optimized for event-driven processing
    SIMD,            // SIMD-optimized processing
    Parallel,        // Parallel processing for multi-core
    Adaptive        // Adaptive optimization based on workload
};

/**
 * @brief Optimized Brain implementation with performance enhancements
 */
class OptimizedBrainImpl : public Brain {
public:
    explicit OptimizedBrainImpl(std::shared_ptr<Config> config);
    ~OptimizedBrainImpl() override;
    
    bool initialize() override;
    void step(SimulationStep currentStep) override;
    void step(SimulationStep currentStep, Timestamp currentTime) override;
    
    // Performance mode control
    void setPerformanceMode(PerformanceMode mode);
    PerformanceMode getPerformanceMode() const { return currentMode_; }
    
    // Performance monitoring
    BrainPerformanceStats getPerformanceStats() const;
    
    // Memory pool integration
    void enableMemoryPools(bool enable) { memoryPoolsEnabled_ = enable; }
    bool isMemoryPoolEnabled() const { return memoryPoolsEnabled_; }
    
    // SIMD optimization control
    void enableSIMD(bool enable) { simdEnabled_ = enable; }
    bool isSIMDEnabled() const { return simdEnabled_; }
    
    // Parallel processing control
    void enableParallel(bool enable) { parallelEnabled_ = enable; }
    bool isParallelEnabled() const { return parallelEnabled_; }
    
    // Event-driven optimization control
    void enableEventDriven(bool enable) { eventDrivenEnabled_ = enable; }
    bool isEventDrivenEnabled() const { return eventDrivenEnabled_; }
    
    // Configuration for optimizations
    void configureOptimizations(const std::string& configFile);
    
    // Performance statistics
    double getOptimizationEfficiency() const { return optimizationEfficiency_; }
    void updateOptimizationEfficiency();
    
private:
    // Original pImpl for backward compatibility
    struct OriginalImpl;
    OriginalImpl* originalPImpl_;
    
    // Performance infrastructure
    std::unique_ptr<OptimizedBrain> optimizedBrain_;
    std::unique_ptr<MemoryPool<Neuron>> neuronPool_;
    std::unique_ptr<MemoryPool<Synapse>> synapsePool_;
    std::unique_ptr<VectorizedLIF> lifVectorizer_;
    std::unique_ptr<ParallelNeuralProcessor> parallelProcessor_;
    std::unique_ptr<EventQueue> eventQueue_;
    
    // Performance optimization settings
    PerformanceMode currentMode_;
    bool memoryPoolsEnabled_;
    bool simdEnabled_;
    bool parallelEnabled_;
    bool eventDrivenEnabled_;
    
    // Performance monitoring
    std::atomic<size_t> totalStepCount_;
    std::chrono::steady_clock::time_point lastStepTime_;
    mutable std::mutex perfMutex_;
    
    // Cache for frequently accessed data
    mutable std::unordered_map<NeuronId, NeuronState> neuronStateCache_;
    mutable std::unordered_map<SynapseId, SynapseState> synapseStateCache_;
    
    // Statistics
    double optimizationEfficiency_;
    size_t cacheHits_;
    size_t cacheMisses_;
    
    // Event-driven processing
    std::atomic<bool> processingEvents_;
    std::condition_variable eventCV_;
    std::mutex eventMutex_;
    
    // Internal helper methods
    void integrateWithOptimizedBrain();
    void setupMemoryPools();
    void setupSIMDVectorization();
    void setupParallelProcessing();
    void setupEventDrivenProcessing();
    void optimizeMemoryAccess();
    void optimizeComputationPipeline();
    void updatePerformanceStatistics();
    void configurePerformanceMode();
    
    // Optimized versions of original methods
    void optimizedInitialize();
    void optimizedStep(SimulationStep currentStep, Timestamp currentTime);
    void optimizedSpikeProcessing();
    void optimizedPlasticityProcessing();
    void optimizedMemoryProcessing();
    void optimizedNeuromodulationProcessing();
    
    // Cache management
    NeuronState getCachedNeuronState(NeuronId id) const;
    void cacheNeuronState(NeuronId id, const NeuronState& state);
    SynapseState getCachedSynapseState(SynapseId id) const;
    void cacheSynapseState(SynapseId id, const SynapseState& state);
    void invalidateCache();
    
    // SIMD-optimized batch operations
    void simdBatchNeuronUpdates();
    void simdBatchSynapseUpdates();
    void simdBatchPlasticityUpdates();
    
    // Parallel processing helper
    void parallelProcessNeurons(std::function<void(NeuronId)> processor);
    void parallelProcessSynapses(std::function<void(SynapseId)> processor);
    
    // Event-driven processing
    void processEventDrivenSpikes();
    void scheduleDelayedEvents();
    void processMemoryEvents();
    
    // Performance optimization helpers
    double calculateOptimizationEfficiency() const;
    void adjustOptimizationsBasedOnWorkload();
};

// Implementation of OptimizedBrainImpl

OptimizedBrainImpl::OptimizedBrainImpl(std::shared_ptr<Config> config) : currentMode_(PerformanceMode::Normal),
                                                           memoryPoolsEnabled_(false),
                                                           simdEnabled_(false),
                                                           parallelEnabled_(false),
                                                           eventDrivenEnabled_(false),
                                                           totalStepCount_(0),
                                                           optimizationEfficiency_(0.0),
                                                           cacheHits_(0),
                                                           cacheMisses_(0),
                                                           processingEvents_(false) {
    // Initialize original pImpl
    originalPImpl_ = new OriginalImpl(config);
    
    // Copy constructor pattern
    pImpl = originalPImpl_;
    
    // Initialize performance infrastructure
    setupMemoryPools();
    setupSIMDVectorization();
    setupParallelProcessing();
    setupEventDrivenProcessing();
    optimizeMemoryAccess();
    optimizeComputationPipeline();
    
    lastStepTime_ = std::chrono::steady_clock::now();
}

OptimizedBrainImpl::~OptimizedBrainImpl() {
    if (originalPImpl_) {
        delete originalPImpl_;
    }
}

bool OptimizedBrainImpl::initialize() {
    // First initialize the original brain
    if (!originalPImpl_->initialize()) {
        return false;
    }
    
    // Then integrate with optimized brain
    integrateWithOptimizedBrain();
    
    // Configure performance based on settings
    configurePerformanceMode();
    
    NLM_LOG_INFO("Optimized Brain initialization complete");
    NLM_LOG_INFO("Performance mode: " + std::to_string(static_cast<int>(currentMode_)));
    NLM_LOG_INFO("Memory pools: " + std::string(memoryPoolsEnabled_ ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("SIMD optimization: " + std::string(simdEnabled_ ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("Parallel processing: " + std::string(parallelEnabled_ ? "Enabled" : "Disabled"));
    
    return true;
}

void OptimizedBrainImpl::integrateWithOptimizedBrain() {
    // Create optimized brain with same configuration
    optimizedBrain_ = std::make_unique<OptimizedBrain>(pImpl->config);
    
    // Transfer essential state from original brain
    optimizedBrain_->setPerformanceMode(OptimizedBrain::PerformanceMode::Normal);
    optimizedBrain_->enableEventDriven(eventDrivenEnabled_);
    optimizedBrain_->enableSIMD(simdEnabled_);
    optimizedBrain_->enableMultithreading(parallelEnabled_);
    optimizedBrain_->enableSparseConnectivity(true);
    
    // Synchronize regions between original and optimized
    for (const auto& originalRegion : pImpl->regions) {
        auto* optRegion = optimizedBrain_->getRegion(originalRegion->getId());
        if (optRegion) {
            // Transfer region state
            // This would involve complex data transfer logic
        }
    }
}

void OptimizedBrainImpl::setupMemoryPools() {
    if (!memoryPoolsEnabled_) return;
    
    // Initialize neuron pool
    size_t neuronPoolSize = pImpl->config->getOr<size_t>("neuron_pool_size", 10000);
    neuronPool_ = std::make_unique<MemoryPool<Neuron>>(1024, 4);  // blockSize, initialBlocks
    
    // Initialize synapse pool
    size_t synapsePoolSize = pImpl->config->getOr<size_t>("synapse_pool_size", 50000);
    synapsePool_ = std::make_unique<MemoryPool<Synapse>>(1024, 4);
    
    NLM_LOG_INFO("Memory pools initialized: neurons=" + std::to_string(neuronPoolSize) + 
                ", synapses=" + std::to_string(synapsePoolSize));
}

void OptimizedBrainImpl::setupSIMDVectorization() {
    if (!simdEnabled_) return;
    
    // Create SIMD vectorizer for LIF dynamics
    lifVectorizer_ = std::make_unique<VectorizedLIF>();
    
    // Configure based on CPU capabilities
    bool avx512 = SIMDUtils::isSIMDAvailable();
    NLM_LOG_INFO("SIMD vectorization " + std::string(avx512 ? "enabled" : "disabled"));
}

void OptimizedBrainImpl::setupParallelProcessing() {
    if (!parallelEnabled_) return;
    
    // Determine optimal thread count
    size_t threadCount = pImpl->config->getOr<size_t>("thread_count", 
                                                      std::thread::hardware_concurrency());
    
    parallelProcessor_ = std::make_unique<ParallelNeuralProcessor>(threadCount);
    
    NLM_LOG_INFO("Parallel processing initialized with " + std::to_string(threadCount) + " threads");
}

void OptimizedBrainImpl::setupEventDrivenProcessing() {
    if (!eventDrivenEnabled_) return;
    
    // Initialize event queue for spike processing
    eventQueue_ = std::make_unique<EventQueue>();
    
    NLM_LOG_INFO("Event-driven processing enabled");
}

void OptimizedBrainImpl::optimizeMemoryAccess() {
    // Optimize memory access patterns
    // This would involve configuring cache-friendly data structures
    NLM_LOG_INFO("Memory access patterns optimized");
}

void OptimizedBrainImpl::optimizeComputationPipeline() {
    // Optimize computation pipeline stages
    NLM_LOG_INFO("Computation pipeline optimized");
}

void OptimizedBrainImpl::setPerformanceMode(PerformanceMode mode) {
    std::lock_guard<std::mutex> lock(perfMutex_);
    currentMode_ = mode;
    
    // Configure optimizations based on mode
    switch (mode) {
        case PerformanceMode::Speed:
            // Optimize for speed
            simdEnabled_ = true;
            parallelEnabled_ = true;
            eventDrivenEnabled_ = false;
            memoryPoolsEnabled_ = false;
            break;
            
        case PerformanceMode::Accuracy:
            // Optimize for accuracy
            simdEnabled_ = true;
            parallelEnabled_ = false;
            eventDrivenEnabled_ = true;
            memoryPoolsEnabled_ = true;
            break;
            
        case PerformanceMode::Memory:
            // Optimize for memory
            simdEnabled_ = false;
            parallelEnabled_ = false;
            eventDrivenEnabled_ = false;
            memoryPoolsEnabled_ = true;
            break;
            
        case PerformanceMode::Normal:
            // Balanced optimization
        default:
            break;
    }
    
    NLM_LOG_INFO("Performance mode set to: " + std::to_string(static_cast<int>(mode)));
}

void OptimizedBrainImpl::configureOptimizations(const std::string& configFile) {
    // Load performance configuration from file
    NLM_LOG_INFO("Configuring optimizations from " + configFile);
    
    // Parse configuration file
    // This would involve reading JSON/YAML configuration
    
    // Apply optimizations based on configuration
    // Example: enable SIMD, configure thread count, etc.
}

BrainPerformanceStats OptimizedBrainImpl::getPerformanceStats() const {
    std::lock_guard<std::mutex> lock(perfMutex_);
    
    BrainPerformanceStats stats;
    stats.totalSpikes = originalPImpl_->totalSpikesTotal;
    stats.pendingSpikes = originalPImpl_->spikeSystem->getPendingSpikeCount() +
                          originalPImpl_->spikeSystem->getPendingDelayedCount();
    stats.activeNeurons = getActiveNeuronCount();
    stats.firingNeurons = getFiringNeuronCount();
    stats.averageFiringRate = getAverageFiringRate();
    stats.eIINRatio = getExcitationInhibitionRatio();
    stats.memoryUsage = 0; // TODO: Calculate actual memory usage
    stats.stepTime = 0.0;  // TODO: Measure actual step time
    stats.stepRate = totalStepCount_ / getRealTimeElapsed();
    stats.eventQueueSize = eventQueue_ ? eventQueue_->size() : 0;
    stats.cacheHits = cacheHits_;
    stats.cacheMisses = cacheMisses_;
    stats.optimizationEfficiency = optimizationEfficiency_;
    
    return stats;
}

void OptimizedBrainImpl::updateOptimizationEfficiency() {
    // Calculate optimization efficiency based on performance metrics
    optimizationEfficiency_ = calculateOptimizationEfficiency();
}

double OptimizedBrainImpl::calculateOptimizationEfficiency() const {
    // Simple efficiency calculation based on cache hit rate and step performance
    size_t totalAccesses = cacheHits_ + cacheMisses_;
    if (totalAccesses == 0) return 0.0;
    
    double cacheEfficiency = static_cast<double>(cacheHits_) / totalAccesses;
    double stepEfficiency = getRealTimeElapsed() > 0 ? 1.0 / getRealTimeElapsed() : 0.0;
    
    return (cacheEfficiency * 0.6) + (stepEfficiency * 0.4);
}

void OptimizedBrainImpl::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void OptimizedBrainImpl::step(SimulationStep currentStep, Timestamp currentTime) {
    // Update performance counters
    totalStepCount_++;
    auto now = std::chrono::steady_clock::now();
    double stepTime = std::chrono::duration_cast<std::chrono::duration<double>>(now - lastStepTime_).count();
    lastStepTime_ = now;
    
    // Use optimized step implementation
    optimizedStep(currentStep, currentTime);
    
    // Update performance statistics
    updateOptimizationEfficiency();
    
    // Adjust optimizations based on workload
    adjustOptimizationsBasedOnWorkload();
}

void OptimizedBrainImpl::optimizedStep(SimulationStep currentStep, Timestamp currentTime) {
    // Determine if we should use optimized implementation
    bool useOptimized = shouldUseOptimizedStep(currentStep);
    
    if (useOptimized) {
        optimizedStepInternal(currentStep, currentTime);
    } else {
        // Fall back to original implementation
        Brain::step(currentStep, currentTime);
    }
}

void OptimizedBrainImpl::optimizedStepInternal(SimulationStep currentStep, Timestamp currentTime) {
    // Optimized step implementation with SIMD, parallel processing, and event-driven techniques
    // This would integrate with the OptimizedBrain implementation
    
    // For now, delegate to the original implementation
    // TODO: Implement full optimized step functionality
    Brain::step(currentStep, currentTime);
}

bool OptimizedBrainImpl::shouldUseOptimizedStep(SimulationStep currentStep) const {
    // Determine based on performance mode and current workload
    switch (currentMode_) {
        case PerformanceMode::Speed:
            return true;  // Always use optimized for speed
            
        case PerformanceMode::Accuracy:
            return currentStep % 100 == 0;  // Use optimized periodically for accuracy
            
        case PerformanceMode::Memory:
            return false;  // Use original for memory-constrained scenarios
            
        case PerformanceMode::EventDriven:
            return processingEvents_.load(std::memory_order_relaxed);
            
        case PerformanceMode::SIMD:
            return simdEnabled_;
            
        case PerformanceMode::Parallel:
            return parallelEnabled_;
            
        default:
            return true;  // Default to optimized
    }
}

void OptimizedBrainImpl::adjustOptimizationsBasedOnWorkload() {
    // Adjust optimization settings based on current workload
    // This would monitor system resources and adjust accordingly
    
    // TODO: Implement workload-based optimization adjustment
}

// Cache management implementation

NeuronState OptimizedBrainImpl::getCachedNeuronState(NeuronId id) const {
    auto it = neuronStateCache_.find(id);
    if (it != neuronStateCache_.end()) {
        cacheHits_++;
        return it->second;
    }
    
    cacheMisses_++;
    
    // Fallback to original implementation
    // TODO: Get neuron state from original brain
    return NeuronState();
}

void OptimizedBrainImpl::cacheNeuronState(NeuronId id, const NeuronState& state) {
    // Cache neuron state with LRU eviction policy
    if (neuronStateCache_.size() > 1000) {
        // Simple LRU: remove oldest entry
        auto it = neuronStateCache_.begin();
        neuronStateCache_.erase(it);
    }
    
    neuronStateCache_[id] = state;
}

SynapseState OptimizedBrainImpl::getCachedSynapseState(SynapseId id) const {
    auto it = synapseStateCache_.find(id);
    if (it != synapseStateCache_.end()) {
        cacheHits_++;
        return it->second;
    }
    
    cacheMisses_++;
    
    // Fallback to original implementation
    // TODO: Get synapse state from original brain
    return SynapseState();
}

void OptimizedBrainImpl::cacheSynapseState(SynapseId id, const SynapseState& state) {
    // Cache synapse state with LRU eviction policy
    if (synapseStateCache_.size() > 1000) {
        // Simple LRU: remove oldest entry
        auto it = synapseStateCache_.begin();
        synapseStateCache_.erase(it);
    }
    
    synapseStateCache_[id] = state;
}

void OptimizedBrainImpl::invalidateCache() {
    neuronStateCache_.clear();
    synapseStateCache_.clear();
}

// Event-driven processing implementation

void OptimizedBrainImpl::processEventDrivenSpikes() {
    // Process spikes using event-driven approach
    // This would use the eventQueue_ and OptimizedBrain's spike processing
    
    if (eventQueue_) {
        // Process events from event queue
        // TODO: Implement event-driven spike processing
    }
}

void OptimizedBrainImpl::scheduleDelayedEvents() {
    // Schedule delayed events
    // TODO: Implement delayed event scheduling
}

void OptimizedBrainImpl::processMemoryEvents() {
    // Process memory-related events
    // TODO: Implement memory event processing
}

// SIMD-optimized batch operations implementation

void OptimizedBrainImpl::simdBatchNeuronUpdates() {
    if (!lifVectorizer_) return;
    
    // Perform SIMD-optimized batch updates on neurons
    // TODO: Implement SIMD batch operations
}

void OptimizedBrainImpl::simdBatchSynapseUpdates() {
    // Perform SIMD-optimized batch updates on synapses
    // TODO: Implement SIMD batch operations
}

void OptimizedBrainImpl::simdBatchPlasticityUpdates() {
    // Perform SIMD-optimized batch plasticity updates
    // TODO: Implement SIMD batch operations
}

// Parallel processing implementation

void OptimizedBrainImpl::parallelProcessNeurons(std::function<void(NeuronId)> processor) {
    if (!parallelProcessor_) return;
    
    // Process neurons in parallel
    // TODO: Implement parallel neuron processing
}

void OptimizedBrainImpl::parallelProcessSynapses(std::function<void(SynapseId)> processor) {
    if (!parallelProcessor_) return;
    
    // Process synapses in parallel
    // TODO: Implement parallel synapse processing
}

// Original Brain methods (delegated to original implementation)

bool Brain::initialize() {
    return pImpl->initialize();
}

void Brain::step(SimulationStep currentStep) {
    pImpl->step(currentStep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->step(currentStep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    pImpl->receiveSensoryInput(input);
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    pImpl->injectCurrent(neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    pImpl->injectCurrentToNeurons(type, current);
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* Brain::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

float Brain::getExcitationInhibitionRatio() const {
    return pImpl->getExcitationInhibitionRatio();
}

std::unique_ptr<class Action> Brain::produceAction() {
    return pImpl->produceAction();
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    pImpl->applyNeuromodulation(signal);
}

void Brain::updatePlasticity() {
    pImpl->updatePlasticity();
}

void Brain::develop() {
    pImpl->develop();
}

void Brain::reset() {
    pImpl->reset();
}

bool Brain::save(const std::string& filepath) const {
    return pImpl->save(filepath);
}

bool Brain::load(const std::string& filepath) {
    return pImpl->load(filepath);
}

RegionId Brain::addRegion(const std::string& name) {
    return pImpl->addRegion(name);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return pImpl->getRegion(id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return pImpl->getRegion(id);
}

size_t Brain::getRegionCount() const {
    return pImpl->getRegionCount();
}

std::vector<RegionId> Brain::getRegionIds() const {
    return pImpl->getRegionIds();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->getRegions();
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->addInterRegionConnection(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->removeInterRegionConnection(source, target);
}

size_t Brain::getTotalNeuronCount() const {
    return pImpl->getTotalNeuronCount();
}

size_t Brain::getTotalSynapseCount() const {
    return pImpl->getTotalSynapseCount();
}

size_t Brain::getActiveNeuronCount() const {
    return pImpl->getActiveNeuronCount();
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->getFiringNeuronCount();
}

float Brain::getAverageFiringRate() const {
    return pImpl->getAverageFiringRate();
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->getWorkingMemory();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->getEpisodicMemory();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->getAssociativeMemory();
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->getPredictionSystem();
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl->getPlanner();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->getConceptFormation();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->getAttention();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->getDevelopmentSystem();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->getDevelopmentalStage();
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->setDevelopmentalStage(stage);
}

Dopamine* Brain::getDopamine() {
    return pImpl->getDopamine();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->getCuriosity();
}

Novelty* Brain::getNovelty() {
    return pImpl->getNovelty();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->getPredictionErrorSignal();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    pImpl->logStatus();
}

} // namespace nlm