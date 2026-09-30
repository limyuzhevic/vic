#pragma once

/**
 * BrainWithPerformance - Enhanced Brain with comprehensive performance optimization systems
 * 
 * This class extends the base Brain functionality with integrated performance optimization:
 * 
 * 1. Memory Pool System
 *    - Neuron memory pools for efficient allocation/deallocation
 *    - Synapse memory pools with block allocation
 *    - Working memory pools with reuse strategies
 *    - Memory allocation tracking and statistics
 *    - Memory fragmentation tracking and optimization
 * 
 * 2. Event Queue System
 *    - Spike event queue with priority management
 *    - Connection event queue with delay handling
 *    - Memory access event queue for memory pool management
 *    - Event scheduling and execution
 *    - Event statistics and profiling
 * 
 * 3. Sparse Connectivity System
 *    - Sparse connectivity matrix for large brain simulations
 *    - Connection representation and access optimization
 *    - Synaptic pruning and optimization
 *    - Connection statistics and profiling
 * 
 * 4. Performance Monitoring System
 *    - Real-time performance metrics
 *    - Memory usage tracking
 *    - CPU usage profiling
 *    - Event queue statistics
 *    - Bottleneck identification
 * 
 * 5. Optimization Integration
 *    - Enhanced Brain class with performance systems
 *    - Performance-aware step() method
 *    - Memory pool integration with existing memory systems
 *    - Event-driven processing optimization
 *    - Automatic optimization based on load
 * 
 * Features:
 * - Scalable memory allocation for large brain simulations
 * - Event-driven architecture for efficient spike processing
 * - Sparse matrix operations for connection management
 * - Real-time performance monitoring and optimization
 * - Memory efficiency and fragmentation reduction
 * - Integration with existing NLM brain architecture
 * - Backward compatibility with existing API
 * - Configurable performance optimization strategies
 */

#include "Brain.hpp"
#include "../performance/PerformanceMonitor.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/EventQueue.hpp"
#include "../performance/SparseConnectivity.hpp"
#include <memory>
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <functional>

namespace nlm {

/**
 * Performance-Optimized Brain with integrated performance systems
 * 
 * This class provides comprehensive performance optimization for neural simulations,
 * integrating memory pools, event queues, sparse connectivity, and performance monitoring.
 * It maintains backward compatibility with the base Brain API while providing
 * advanced performance optimization capabilities.
 */
class BrainWithPerformance : public Brain {
public:
    /**
     * Constructor with configuration and performance settings
     */
    explicit BrainWithPerformance(std::shared_ptr<Config> config);
    
    ~BrainWithPerformance() override;
    
    /**
     * Initialize the performance-enhanced brain
     */
    bool initialize() override;
    
    /**
     * Enhanced step method with performance awareness
     * 
     * This method integrates all performance systems:
     * - Memory pool management
     * - Event-driven processing
     * - Sparse connectivity operations
     * - Performance monitoring and optimization
     */
    void step(SimulationStep currentStep) override;
    void step(SimulationStep currentStep, Timestamp currentTime) override;
    
    /**
     * Performance statistics and metrics
     */
    struct PerformanceStats {
        // Performance scores (0.0 to 1.0)
        float memoryEfficiencyScore;
        float eventProcessingScore;
        float neuralActivityScore;
        float cpuEfficiencyScore;
        
        // Raw metrics
        size_t totalMemoryMB;
        float memoryFragmentationRatio;
        float eventQueueUtilization;
        size_t activeNeurons;
        float firingRate;
        float cpuUsagePercent;
        
        // Performance trends
        float memoryTrend;
        float eventTrend;
        float neuralTrend;
        
        // Bottleneck indicators
        bool memoryBottleneck;
        bool eventBottleneck;
        bool cpuBottleneck;
        bool connectivityBottleneck;
        
        // Performance degradation
        float performanceDegradation;
        
        // Health and stability
        float systemHealthScore;
        bool isStable;
        
        // Step timing
        double stepDurationMs;
        float realTimeFactor;
    };
    
    /**
     * Get current performance statistics
     */
    PerformanceStats getPerformanceStats() const;
    
    /**
     * Get performance monitor instance
     */
    PerformanceMonitor* getPerformanceMonitor() { return performanceMonitor_.get(); }
    const PerformanceMonitor* getPerformanceMonitor() const { return performanceMonitor_.get(); }
    
    /**
     * Get memory pool statistics
     */
    struct MemoryPoolStats {
        size_t neuronPoolSize;
        size_t synapsePoolSize;
        size_t memoryPoolCount;
        size_t totalMemoryAllocations;
        size_t totalMemoryDeallocations;
        float memoryFragmentation;
        size_t peakMemoryUsage;
        
        // Pool-specific statistics
        std::vector<std::pair<std::string, size_t>> poolDetails;
    };
    
    MemoryPoolStats getMemoryPoolStats() const;
    
    /**
     * Get event queue statistics
     */
    struct EventQueueStats {
        size_t spikeEventsProcessed;
        size_t spikeEventsDropped;
        float spikeQueueUtilization;
        size_t delayedSpikeEvents;
        float averageLatency;
        size_t priorityEventsProcessed;
    };
    
    EventQueueStats getEventQueueStats() const;
    
    /**
     * Get sparse connectivity statistics
     */
    struct ConnectivityStats {
        size_t totalSynapses;
        size_t activeSynapses;
        float averageSynapticWeight;
        float connectivityProbability;
        size_t maxSynapsesPerNeuron;
        float globalClusteringCoefficient;
        float averagePathLength;
        
        // Region-specific statistics
        std::vector<std::pair<size_t, size_t>> regionSynapseCounts;
    };
    
    ConnectivityStats getConnectivityStats() const;
    
    /**
     * Configure performance optimization strategies
     */
    void configureOptimization(const std::string& strategy, bool enable);
    void setMemoryPoolSize(size_t neuronPoolSize, size_t synapsePoolSize);
    void setEventQueueCapacity(size_t spikeQueueCapacity, size_t delayedQueueCapacity);
    void setThreadCount(size_t threadCount);
    void enableEventDriven(bool enable);
    void enableMultithreading(bool enable);
    void enableSIMD(bool enable);
    void enableSparseConnectivity(bool enable);
    
    /**
     * Get optimization status
     */
    bool isEventDrivenEnabled() const { return eventDrivenEnabled_; }
    bool isMultithreadingEnabled() const { return multithreadingEnabled_; }
    bool isSIMDEnabled() const { return simdEnabled_; }
    bool isSparseEnabled() const { return sparseEnabled_; }
    size_t getThreadCount() const { return threadCount_; }
    
    /**
     * Performance alert callback type
     */
    using PerformanceAlertCallback = std::function<void(const PerformanceAlert&)';
    
    /**
     * Set performance alert callback
     */
    void setPerformanceAlertCallback(PerformanceAlertCallback callback) {
        if (performanceMonitor_) {
            performanceMonitor_->setAlertCallback(callback);
        }
    }
    
    /**
     * Trigger manual performance optimization
     */
    void triggerPerformanceOptimization();
    
    /**
     * Export performance data for analysis
     */
    bool exportPerformanceData(const std::string& filepath) const;
    
    /**
     * Get total steps processed
     */
    size_t getTotalStepsProcessed() const {
        return performanceMonitor_ ? performanceMonitor_->getTotalStepsProcessed() : 0;
    }
    
    /**
     * Get system health score
     */
    float getSystemHealthScore() const {
        return performanceMonitor_ ? performanceMonitor_->getSystemHealthScore() : 1.0f;
    }
    
private:
    /**
     * Initialize performance systems
     */
    void initializePerformanceSystems();
    
    /**
     * Integrate with existing brain memory systems
     */
    void integrateWithMemorySystems();
    
    /**
     * Integrate with existing brain event systems
     */
    void integrateWithEventSystems();
    
    /**
     * Integrate with existing brain connectivity systems
     */
    void integrateWithConnectivitySystems();
    
    /**
     * Setup performance monitoring
     */
    void setupPerformanceMonitoring();
    
    /**
     * Configure event-driven processing
     */
    void configureEventDrivenProcessing();
    
    /**
     * Setup memory pools
     */
    void setupMemoryPools();
    
    /**
     * Setup sparse connectivity
     */
    void setupSparseConnectivity();
    
    /**
     * Update performance statistics
     */
    void updatePerformanceStats();
    
    /**
     * Apply automatic optimizations based on load
     */
    void applyAutomaticOptimizations();
    
    /**
     * Detect and resolve bottlenecks
     */
    void detectAndResolveBottlenecks();
    
    /**
     * Optimize memory pool allocation
     */
    void optimizeMemoryPools();
    
    /**
     * Optimize event queue processing
     */
    void optimizeEventQueues();
    
    /**
     * Optimize connectivity access patterns
     */
    void optimizeConnectivity();
    
    /**
     * Setup event-driven neural processing
     */
    void setupEventDrivenProcessing();
    
    /**
     * Setup parallel neural processing
     */
    void setupParallelProcessing();
    
    // Performance systems
    std::unique_ptr<PerformanceMonitor> performanceMonitor_;
    
    // Performance configuration
    bool eventDrivenEnabled_;
    bool multithreadingEnabled_;
    bool simdEnabled_;
    bool sparseEnabled_;
    size_t threadCount_;
    
    // Memory pool integration
    std::unique_ptr<MemoryPool<Neuron>> neuronPool_;
    std::unique_ptr<MemoryPool<Synapse>> synapsePool_;
    
    // Event queue integration
    std::unique_ptr<SpikeRingBuffer> spikeQueue_;
    std::unique_ptr<DelayedSpikeQueue> delayedSpikeQueue_;
    std::unique_ptr<PriorityEventQueue> priorityQueue_;
    
    // Connectivity integration
    std::unique_ptr<SparseConnectivity> sparseConnectivity_;
    
    // Performance statistics
    PerformanceStats performanceStats_;
    mutable std::mutex statsMutex_;
    
    // Performance optimization state
    size_t optimizationCounter_;
    std::chrono::high_resolution_clock::time_point lastOptimizationTime_;
    float optimizationCooldown_;
    
    // Event-driven processing state
    std::vector<uint64_t> activeNeurons_;
    std::atomic<size_t> activeNeuronCount_;
    
    // Performance trend tracking
    struct TrendData {
        std::vector<float> values;
        size_t windowSize;
        float trendScore;
        
        TrendData(size_t size) : windowSize(size), trendScore(0.0f) {
            values.reserve(size);
        }
    };
    
    std::unordered_map<std::string, TrendData> trendData_;
};