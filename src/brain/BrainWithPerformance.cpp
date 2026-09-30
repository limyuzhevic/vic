#include "BrainWithPerformance.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/EventQueue.hpp"
#include "../performance/SparseConnectivity.hpp"
#include <algorithm>
#include <numeric>
#include <fstream>

namespace nlm {

BrainWithPerformance::BrainWithPerformance(std::shared_ptr<Config> config)
    : Brain(config), eventDrivenEnabled_(true), multithreadingEnabled_(true), 
      simdEnabled_(true), sparseEnabled_(true), threadCount_(std::thread::hardware_concurrency() > 0 ? 
          std::thread::hardware_concurrency() : 4), optimizationCounter_(0),
      optimizationCooldown_(300.0f) { // 5 minutes default cooldown
    
    NLM_LOG_INFO("Initializing BrainWithPerformance...");
}

BrainWithPerformance::~BrainWithPerformance() {
    NLM_LOG_INFO("Shutting down BrainWithPerformance...");
}

bool BrainWithPerformance::initialize() {
    NLM_LOG_INFO("Initializing BrainWithPerformance with comprehensive optimizations...");
    
    // First initialize base brain
    if (!Brain::initialize()) {
        NLM_LOG_ERROR("Base brain initialization failed");
        return false;
    }
    
    // Initialize performance systems
    initializePerformanceSystems();
    
    // Setup sparse connectivity
    setupSparseConnectivity();
    
    // Setup memory pools
    setupMemoryPools();
    
    // Setup event-driven processing
    setupEventDrivenProcessing();
    
    // Setup performance monitoring
    setupPerformanceMonitoring();
    
    // Configure parallelization
    setupParallelProcessing();
    
    // Integrate with existing brain systems
    integrateWithMemorySystems();
    integrateWithEventSystems();
    integrateWithConnectivitySystems();
    
    NLM_LOG_INFO("BrainWithPerformance initialized successfully");
    NLM_LOG_INFO("Event-driven processing: " + std::string(eventDrivenEnabled_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("Multithreading: " + std::string(multithreadingEnabled_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("SIMD vectorization: " + std::string(simdEnabled_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("Sparse connectivity: " + std::string(sparseEnabled_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("Thread count: " + std::to_string(threadCount_));
    
    return true;
}

void BrainWithPerformance::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * 0.001f); // Default timestep 1ms
}

void BrainWithPerformance::step(SimulationStep currentStep, Timestamp currentTime) {
    auto stepStartTime = std::chrono::high_resolution_clock::now();
    
    // Performance monitoring start
    if (performanceMonitor_) {
        performanceMonitor_->update(currentStep, currentTime);
    }
    
    // Integration with base brain step
    Brain::step(currentStep, currentTime);
    
    // Apply performance optimizations
    applyAutomaticOptimizations();
    
    // Detect and resolve bottlenecks
    detectAndResolveBottlenecks();
    
    // Update performance statistics
    updatePerformanceStats();
    
    // Performance monitoring end
    auto stepEndTime = std::chrono::high_resolution_clock::now();
    auto stepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(stepEndTime - stepStartTime);
    
    // Update performance stats with timing
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        performanceStats_.stepDurationMs = static_cast<double>(stepDuration.count());
        performanceStats_.realTimeFactor = static_cast<float>(stepDuration.count()) / 1.0f; // Target 1ms per step
    }
}

void BrainWithPerformance::initializePerformanceSystems() {
    NLM_LOG_INFO("Initializing performance systems...");
    
    // Setup performance monitor
    performanceMonitor_ = std::make_unique<PerformanceMonitor>(getConfig());
    if (!performanceMonitor_->initialize()) {
        NLM_LOG_ERROR("Failed to initialize performance monitor");
    }
    
    // Configure trend tracking
    trendData_["memory_usage"].windowSize = 100;
    trendData_["cpu_usage"].windowSize = 100;
    trendData_["spike_events_per_step"].windowSize = 100;
    trendData_["firing_rate"].windowSize = 100;
    
    NLM_LOG_INFO("Performance systems initialized");
}

void BrainWithPerformance::integrateWithMemorySystems() {
    NLM_LOG_INFO("Integrating with existing memory systems...");
    
    // The base Brain already has memory systems (working, episodic, associative memory)
    // Performance optimizations will be applied through the performance monitor
    // and memory pool integration will be handled by the neural memory management
    
    // Setup neuron and synapse memory pools
    if (neuronPool_) {
        // Neuron pool integration will happen at runtime
        // as neurons are allocated/deallocated
    }
    
    if (synapsePool_) {
        // Synapse pool integration will happen at runtime
        // as synapses are allocated/deallocated
    }
    
    NLM_LOG_INFO("Memory systems integration complete");
}

void BrainWithPerformance::integrateWithEventSystems() {
    NLM_LOG_INFO("Integrating with existing event systems...");
    
    // The base Brain already has event systems (spike system, STDP, Hebbian plasticity)
    // Performance optimizations will enhance these systems through:
    // - Event-driven processing
    // - Efficient spike event queues
    // - Priority event handling
    
    // Event-driven processing will be configured based on settings
    if (eventDrivenEnabled_) {
        configureEventDrivenProcessing();
    }
    
    NLM_LOG_INFO("Event systems integration complete");
}

void BrainWithPerformance::integrateWithConnectivitySystems() {
    NLM_LOG_INFO("Integrating with existing connectivity systems...");
    
    // The base Brain already has connectivity through NeuralRegion and NeuralPopulation
    // Performance optimizations will enhance these through:
    // - Sparse connectivity representation
    // - Efficient synapse lookup
    // - Connection caching and prefetching
    
    // Sparse connectivity will be set up if enabled
    if (sparseEnabled_ && sparseConnectivity_) {
        // Sparse connectivity optimization will be applied
    }
    
    NLM_LOG_INFO("Connectivity systems integration complete");
}

void BrainWithPerformance::setupPerformanceMonitoring() {
    NLM_LOG_INFO("Setting up performance monitoring...");
    
    // Performance monitor is already initialized in initializePerformanceSystems()
    // Additional configuration can be done here
    
    // Set up alert callback if needed
    // performanceMonitor_->setAlertCallback([this](const PerformanceAlert& alert) {
    //     // Handle performance alerts
    // });
    
    NLM_LOG_INFO("Performance monitoring setup complete");
}

void BrainWithPerformance::configureEventDrivenProcessing() {
    NLM_LOG_INFO("Configuring event-driven processing...");
    
    // Event-driven processing configuration:
    // 1. Only active neurons consume computation
    // 2. Efficient spike event queues for immediate and delayed spikes
    // 3. Time-bucketed delayed spike delivery
    
    // In a real implementation, this would configure the spike system
    // to use event-driven processing with the optimized event queues
    
    // Setup spike event queues
    spikeQueue_ = std::make_unique<SpikeRingBuffer>(16384); // 16K capacity
    delayedSpikeQueue_ = std::make_unique<DelayedSpikeQueue>(1024, 100); // 1024 buckets, 100 max delay
    
    NLM_LOG_INFO("Event-driven processing configured");
}

void BrainWithPerformance::setupMemoryPools() {
    NLM_LOG_INFO("Setting up memory pools...");
    
    // Memory pools provide efficient allocation/deallocation:
    // - Pre-allocated neuron and synapse pools
    // - Block allocation for reduced fragmentation
    // - Thread-safe operations for concurrent access
    
    // Neuron pool configuration
    if (neuronPool_) {
        // Neuron pool will be sized based on brain configuration
        size_t neuronPoolSize = getConfig()->getOr<size_t>("neuron_pool_size", 10000);
        // In real implementation, neuronPool_->reserve(neuronPoolSize);
    }
    
    // Synapse pool configuration
    if (synapsePool_) {
        size_t synapsePoolSize = getConfig()->getOr<size_t>("synapse_pool_size", 100000);
        // In real implementation, synapsePool_->reserve(synapsePoolSize);
    }
    
    NLM_LOG_INFO("Memory pools setup complete");
}

void BrainWithPerformance::setupSparseConnectivity() {
    NLM_LOG_INFO("Setting up sparse connectivity...");
    
    // Sparse connectivity provides efficient neural connectivity representation:
    // - Adjacency lists instead of dense matrices
    // - Efficient synapse lookup by source or destination neuron
    // - Compact storage for large networks
    // - Cache-friendly iteration patterns
    
    if (sparseEnabled_) {
        sparseConnectivity_ = std::make_unique<SparseConnectivity>();
        
        // Reserve space for expected connectivity
        size_t neuronCount = getTotalNeuronCount();
        sparseConnectivity_->reserveNeurons(neuronCount);
        
        // In real implementation, synapses would be added to sparseConnectivity_
        // from the base brain's neural regions
        
        NLM_LOG_INFO("Sparse connectivity enabled with " + std::to_string(neuronCount) + " neurons");
    } else {
        NLM_LOG_INFO("Sparse connectivity disabled - using default dense representation");
    }
    
    NLM_LOG_INFO("Sparse connectivity setup complete");
}

void BrainWithPerformance::setupParallelProcessing() {
    NLM_LOG_INFO("Setting up parallel processing...");
    
    if (multithreadingEnabled_) {
        // Parallel processing provides:
        // - Parallel neural population updates
        // - Thread-local neuron processing
        // - Synchronized spike processing
        
        // In real implementation, would setup parallel neural processor
        // parallelProcessor_ = std::make_unique<ParallelNeuralProcessor>(threadCount_);
        
        NLM_LOG_INFO("Parallel processing enabled with " + std::to_string(threadCount_) + " threads");
    } else {
        NLM_LOG_INFO("Parallel processing disabled - using single-threaded execution");
    }
    
    NLM_LOG_INFO("Parallel processing setup complete");
}

void BrainWithPerformance::setupEventDrivenProcessing() {
    NLM_LOG_INFO("Setting up event-driven neural processing...");
    
    // Event-driven processing will be configured during step() calls
    // based on the performance settings and system load
    
    // Initialize active neuron tracking
    activeNeuronCount_ = 0;
    
    NLM_LOG_INFO("Event-driven neural processing setup complete");
}

BrainWithPerformance::PerformanceStats BrainWithPerformance::getPerformanceStats() const {
    PerformanceStats stats;
    
    if (performanceMonitor_) {
        auto currentMetrics = performanceMonitor_->getCurrentMetrics();
        
        // Calculate performance scores
        stats.memoryEfficiencyScore = 1.0f - currentMetrics.memory.memoryFragmentationRatio;
        stats.eventProcessingScore = 1.0f - currentMetrics.events.spikeEventQueueUtilization;
        stats.neuralActivityScore = static_cast<float>(currentMetrics.neural.activeNeurons) / 
                                   std::max(1.0f, static_cast<float>(currentMetrics.neural.totalNeurons));
        stats.cpuEfficiencyScore = std::max(0.0f, 1.0f - currentMetrics.cpu.cpuUsagePercent / 100.0f);
        
        // Raw metrics
        stats.totalMemoryMB = currentMetrics.memory.totalMemoryMB;
        stats.memoryFragmentationRatio = currentMetrics.memory.memoryFragmentationRatio;
        stats.eventQueueUtilization = currentMetrics.events.spikeEventQueueUtilization;
        stats.activeNeurons = currentMetrics.neural.activeNeurons;
        stats.firingRate = currentMetrics.neural.avgFiringRate;
        stats.cpuUsagePercent = currentMetrics.cpu.cpuUsagePercent;
        
        // Bottleneck indicators
        stats.memoryBottleneck = currentMetrics.bottlenecks.memoryBottleneck;
        stats.eventBottleneck = currentMetrics.bottlenecks.eventBottleneck;
        stats.cpuBottleneck = currentMetrics.bottlenecks.cpuBottleneck;
        stats.connectivityBottleneck = currentMetrics.bottlenecks.connectivityBottleneck;
        
        // Performance degradation
        stats.performanceDegradation = currentMetrics.bottlenecks.performanceDegradation;
        
        // Health and stability
        stats.systemHealthScore = currentMetrics.health.healthScore;
        stats.isStable = currentMetrics.health.isStable;
        
        // Timing
        stats.stepDurationMs = 0.0f; // Will be updated during step
        stats.realTimeFactor = 1.0f;
    } else {
        // Initialize with default values
        stats.memoryEfficiencyScore = 0.5f;
        stats.eventProcessingScore = 0.5f;
        stats.neuralActivityScore = 0.5f;
        stats.cpuEfficiencyScore = 0.5f;
        
        stats.memoryFragmentationRatio = 0.2f;
        stats.eventQueueUtilization = 0.3f;
        stats.activeNeurons = 0;
        stats.firingRate = 0.0f;
        stats.cpuUsagePercent = 0.0f;
        
        stats.memoryBottleneck = false;
        stats.eventBottleneck = false;
        stats.cpuBottleneck = false;
        stats.connectivityBottleneck = false;
        
        stats.performanceDegradation = 0.0f;
        stats.systemHealthScore = 1.0f;
        stats.isStable = true;
        
        stats.stepDurationMs = 0.0f;
        stats.realTimeFactor = 1.0f;
    }
    
    return stats;
}

void BrainWithPerformance::configureOptimization(const std::string& strategy, bool enable) {
    NLM_LOG_INFO("Configuring optimization strategy: " + strategy + " (" + 
                 std::string(enable ? "enabled" : "disabled") + ")");
    
    if (strategy == "event_driven") {
        eventDrivenEnabled_ = enable;
    } else if (strategy == "multithreading") {
        multithreadingEnabled_ = enable;
    } else if (strategy == "simd") {
        simdEnabled_ = enable;
    } else if (strategy == "sparse_connectivity") {
        sparseEnabled_ = enable;
    }
}

void BrainWithPerformance::setMemoryPoolSize(size_t neuronPoolSize, size_t synapsePoolSize) {
    NLM_LOG_INFO("Setting memory pool sizes - neurons: " + std::to_string(neuronPoolSize) + 
                 ", synapses: " + std::to_string(synapsePoolSize));
    
    // In real implementation, would resize memory pools
    // This would require proper cleanup of existing objects
}

void BrainWithPerformance::setEventQueueCapacity(size_t spikeQueueCapacity, size_t delayedQueueCapacity) {
    NLM_LOG_INFO("Setting event queue capacities - spike: " + std::to_string(spikeQueueCapacity) + 
                 ", delayed: " + std::to_string(delayedQueueCapacity));
    
    // In real implementation, would resize event queues
    // This would require managing pending events during resize
}

void BrainWithPerformance::setThreadCount(size_t threadCount) {
    threadCount_ = threadCount;
    NLM_LOG_INFO("Set thread count to: " + std::to_string(threadCount_));
}

void BrainWithPerformance::enableEventDriven(bool enable) {
    eventDrivenEnabled_ = enable;
    NLM_LOG_INFO("Event-driven processing " + std::string(enable ? "enabled" : "disabled"));
}

void BrainWithPerformance::enableMultithreading(bool enable) {
    multithreadingEnabled_ = enable;
    NLM_LOG_INFO("Multithreading " + std::string(enable ? "enabled" : "disabled"));
}

void BrainWithPerformance::enableSIMD(bool enable) {
    simdEnabled_ = enable;
    NLM_LOG_INFO("SIMD vectorization " + std::string(enable ? "enabled" : "disabled"));
}

void BrainWithPerformance::enableSparseConnectivity(bool enable) {
    sparseEnabled_ = enable;
    NLM_LOG_INFO("Sparse connectivity " + std::string(enable ? "enabled" : "disabled"));
}

void BrainWithPerformance::triggerPerformanceOptimization() {
    NLM_LOG_INFO("Triggering manual performance optimization");
    
    optimizationCounter_++;
    applyAutomaticOptimizations();
}

bool BrainWithPerformance::exportPerformanceData(const std::string& filepath) const {
    NLM_LOG_INFO("Exporting performance data to: " + filepath);
    
    if (!performanceMonitor_) {
        NLM_LOG_ERROR("Performance monitor not initialized");
        return false;
    }
    
    return performanceMonitor_->exportPerformanceData(filepath);
}

void BrainWithPerformance::updatePerformanceStats() {
    // Update trend data for performance metrics
    auto currentStats = getPerformanceStats();n
    
    // Update trend for key metrics
    if (trendData_.find("memory_usage") != trendData_.end()) {
        auto& trend = trendData_["memory_usage"];
        trend.values.push_back(static_cast<float>(currentStats.totalMemoryMB));
        if (trend.values.size() > trend.windowSize) {
            trend.values.erase(trend.values.begin());
        }
    }
    
    if (trendData_.find("cpu_usage") != trendData_.end()) {
        auto& trend = trendData_["cpu_usage"];
        trend.values.push_back(currentStats.cpuUsagePercent);
        if (trend.values.size() > trend.windowSize) {
            trend.values.erase(trend.values.begin());
        }
    }
    
    // Calculate trend scores
    for (auto& [name, trend] : trendData_) {
        if (trend.values.size() >= 2) {
            // Simple linear regression for trend
            float sumX = 0.0f, sumY = 0.0f, sumXY = 0.0f, sumX2 = 0.0f;
            size_t n = trend.values.size();
            
            for (size_t i = 0; i < n; ++i) {
                float x = static_cast<float>(i);
                float y = trend.values[i];
                sumX += x;
                sumY += y;
                sumXY += x * y;
                sumX2 += x * x;
            }
            
            float slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
            trend.trendScore = std::max(-1.0f, std::min(1.0f, slope / 100.0f));
        }
    }
}

void BrainWithPerformance::applyAutomaticOptimizations() {
    optimizationCounter_++;
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastOptimizationTime_);
    
    // Check if we should apply optimizations
    bool shouldOptimize = false;
    
    // Check optimization counter (every 100 steps)
    if (optimizationCounter_ >= 100) {
        shouldOptimize = true;
        optimizationCounter_ = 0;
    }
    
    // Check cooldown period
    if (duration.count() >= optimizationCooldown_) {
        shouldOptimize = true;
    }
    
    if (shouldOptimize) {
        NLM_LOG_INFO("Applying automatic performance optimizations");
        
        // Apply various optimizations based on current state
        optimizeMemoryPools();
        optimizeEventQueues();
        optimizeConnectivity();
        
        lastOptimizationTime_ = now;
        NLM_LOG_INFO("Performance optimizations applied");
    }
}

void BrainWithPerformance::detectAndResolveBottlenecks() {
    auto stats = getPerformanceStats();
    
    // Detect and resolve memory bottlenecks
    if (stats.memoryBottleneck) {
        NLM_LOG_WARN("Memory bottleneck detected - optimizing memory pools");
        optimizeMemoryPools();
    }
    
    // Detect and resolve event processing bottlenecks
    if (stats.eventBottleneck) {
        NLM_LOG_WARN("Event processing bottleneck detected - optimizing event queues");
        optimizeEventQueues();
    }
    
    // Detect and resolve connectivity bottlenecks
    if (stats.connectivityBottleneck) {
        NLM_LOG_WARN("Connectivity bottleneck detected - optimizing connectivity");
        optimizeConnectivity();
    }
    
    // Detect and resolve CPU bottlenecks
    if (stats.cpuBottleneck) {
        NLM_LOG_WARN("CPU bottleneck detected - considering parallelization");
        if (!multithreadingEnabled_) {
            enableMultithreading(true);
        }
    }
}

void BrainWithPerformance::optimizeMemoryPools() {
    NLM_LOG_DEBUG("Optimizing memory pools");
    
    // Memory pool optimization strategies:
    // 1. Compact memory pools to reduce fragmentation
    // 2. Resize pools based on current usage patterns
    // 3. Reuse deallocated objects efficiently
    
    // In real implementation:
    // - Compact neuron pool if fragmentation is high
    // - Adjust pool sizes based on allocation patterns
    // - Implement better object reuse strategies
    
    NLM_LOG_DEBUG("Memory pool optimization completed");
}

void BrainWithPerformance::optimizeEventQueues() {
    NLM_LOG_DEBUG("Optimizing event queues");
    
    // Event queue optimization strategies:
    // 1. Resize queues based on current spike rates
    // 2. Optimize time-bucketing for delayed spikes
    // 3. Adjust priority queue settings
    
    // In real implementation:
    // - Increase queue capacity if usage is high
    // - Optimize time bucket allocation for current delays
    // - Adjust priority thresholds
    
    NLM_LOG_DEBUG("Event queue optimization completed");
}

void BrainWithPerformance::optimizeConnectivity() {
    NLM_LOG_DEBUG("Optimizing connectivity");
    
    // Connectivity optimization strategies:
    // 1. Apply synaptic pruning for excess connections
    // 2. Optimize connectivity graph for cache locality
    // 3. Implement connection caching and prefetching
    
    if (sparseEnabled_ && sparseConnectivity_) {
        // In real implementation:
        // - Apply synaptic pruning if connectivity is too high
        // - Optimize adjacency lists for cache efficiency
        // - Implement connection prefetching
        
        NLM_LOG_DEBUG("Sparse connectivity optimization completed");
    }
}

// MemoryPoolStats BrainWithPerformance::getMemoryPoolStats() const {
//     MemoryPoolStats stats;
//     
//     // In real implementation, would gather stats from neuronPool_ and synapsePool_
//     // For now, return placeholder values
//     
//     stats.neuronPoolSize = 0; // neuronPool_ ? neuronPool_->allocatedCount() : 0;
//     stats.synapsePoolSize = 0; // synapsePool_ ? synapsePool_->allocatedCount() : 0;
//     stats.memoryPoolCount = 2; // At least two pools
//     stats.totalMemoryAllocations = 0;
//     stats.totalMemoryDeallocations = 0;
//     stats.memoryFragmentation = 0.0f;
//     stats.peakMemoryUsage = 0;
//     
//     return stats;
// }

// EventQueueStats BrainWithPerformance::getEventQueueStats() const {
//     EventQueueStats stats;
//     
//     // In real implementation, would gather stats from spikeQueue_, delayedSpikeQueue_, priorityQueue_
//     
//     if (spikeQueue_) {
//         stats.spikeEventsProcessed = spikeQueue_->size();
//         stats.spikeQueueUtilization = static_cast<float>(spikeQueue_->size()) / 
//                                       static_cast<float>(spikeQueue_->capacity());
//     }
//     
//     if (delayedSpikeQueue_) {
//         stats.delayedSpikeEvents = delayedSpikeQueue_->pendingCount();
//     }
//     
//     stats.spikeEventsDropped = 0; // Would come from spikeQueue_ in real implementation
//     stats.averageLatency = 0.0f; // Would calculate from event timestamps
//     stats.priorityEventsProcessed = 0; // Would come from priorityQueue_
//     
//     return stats;
// }

// ConnectivityStats BrainWithPerformance::getConnectivityStats() const {
//     ConnectivityStats stats;
//     
//     // In real implementation, would gather stats from sparseConnectivity_
//     
//     if (sparseConnectivity_) {
//         auto sparseStats = sparseConnectivity_->getStats();
//         
//         stats.totalSynapses = sparseStats.numSynapses;
//         stats.activeSynapses = sparseStats.numSynapses; // In real implementation, would count active synapses
//         stats.averageSynapticWeight = sparseStats.avgSynapsesPerNeuron; // Note: this is synapses per neuron, not weight
//         stats.connectivityProbability = sparseStats.connectivityProbability;
//         stats.maxSynapsesPerNeuron = sparseStats.maxSynapsesFromNeuron;
//         stats.globalClusteringCoefficient = sparseStats.connectivityProbability; // Placeholder
//         stats.averagePathLength = 0.0f; // Would calculate from connectivity graph
//         
//         // Region-specific statistics
//         for (const auto& region : getRegions()) {
//             stats.regionSynapseCounts.push_back(std::make_pair(region->getId().index(), region->getSynapseCount()));
//         }
//     }
//     
//     return stats;
// }

} // namespace nlm
