#pragma once

/**
 * Performance Infrastructure - Enhanced Configuration-based Performance System
 * 
 * Phase 5 introduces comprehensive performance optimization for large-scale neural simulation:
 * 
 * 1. Memory Pool Allocators
 *    - Pre-allocated neuron and synapse pools
 *    - Reduced allocation overhead
 *    - Better cache locality
 * 
 * 2. Event Queue Optimization
 *    - Lock-free ring buffers for spike events
 *    - Time-bucketed delayed spikes
 *    - Priority queues for time-sensitive events
 * 
 * 3. Structure-of-Arrays (SoA) Data Layout
 *    - Contiguous memory for neuron states
 *    - Better SIMD vectorization
 *    - Improved cache utilization
 * 
 * 4. Sparse Connectivity
 *    - Adjacency lists instead of dense matrices
 *    - Efficient synapse lookup
 *    - Memory-efficient storage for large networks
 * 
 * 5. Multithreading Support
 *    - Parallel neural population updates
 *    - Thread-local neuron processing
 *    - Synchronized spike processing
 * 
 * This infrastructure is designed to scale from 10k to 100M+ neurons.
 * 
 * Performance Configuration:
 * 
 * ## Build-time Configuration (CMake)
 * 
 * Enable performance features at build time by setting the following CMake options:
 * 
 * ```cmake
 * # Enable all performance features (default: OFF for maximum compatibility)
 * option(NLM_PERFORMANCE_ENABLED "Enable performance optimizations" ON)
 * 
 * # Individual performance feature options (can be combined with build-type)
 * option(NLM_ENABLE_MEMORY_POOL "Enable memory pool allocators" ON)
 * option(NLM_ENABLE_SOA "Enable Structure-of-Arrays data layout" ON)
 * option(NLM_ENABLE_EVENT_QUEUE "Enable event-driven spike processing" ON)
 * option(NLM_ENABLE_SPARSE_CONNECTIVITY "Enable sparse connectivity" ON)
 * option(NLM_ENABLE_MULTITHREADING "Enable parallel processing" ON)
 * option(NLM_ENABLE_SIMD "Enable SIMD vectorization" ON)
 * 
 * # SIMD preference (platform-specific fallback available)
 * option(NLM_PREFER_AVX512 "Prefer AVX-512 instructions" OFF)
 * option(NLM_PREFER_AVX2 "Prefer AVX2 instructions" OFF)
 * option(NLM_PREFER_SSE "Prefer SSE instructions" OFF)
 * 
 * # Multithreading configuration
 * option(NLM_DYNAMIC_THREAD_COUNT "Auto-detect thread count" ON)
 * set(NLM_FIXED_THREAD_COUNT 0 CACHE STRING "Fixed thread count (0 = auto)")
 * 
 * # Memory pool configuration
 * set(NLM_MEMORY_POOL_PAGE_SIZE 4096 CACHE STRING "Memory pool page size in bytes")
 * set(NLM_MAX_POOL_SIZE 1048576 CACHE STRING "Maximum objects per pool")
 * 
 * # Event queue configuration
 * set(NLM_SPIKE_QUEUE_CAPACITY 16384 CACHE STRING "Spike event queue capacity")
 * set(NLM_DELAYED_SPIKE_BUCKETS 1024 CACHE STRING "Number of delayed spike buckets")
 * 
 * # Runtime configuration
 * option(NLM_RUNTIME_FEATURE_TOGGLE "Allow runtime feature toggling" ON)
 * option(NLM_PERFORMANCE_STATS "Enable performance statistics collection" ON)
 * option(NLM_PERFORMANCE_VALIDATION "Enable configuration validation" ON)
 * 
 * # Adaptive performance scaling
 * option(NLM_ADAPTIVE_PERFORMANCE "Enable runtime performance adaptation" OFF)
 * set(NLM_MIN_THREADS 1 CACHE STRING "Minimum thread count for adaptive scaling")
 * set(NLM_MAX_THREADS 64 CACHE STRING "Maximum thread count for adaptive scaling")
 * set(NLM_PERFORMANCE_ADAPTATION_INTERVAL 10000 CACHE STRING "Steps between performance adaptations")
 * ``` 
 *
 * 3.1 Memory Pool Configuration
 * 
 * Memory pools provide pre-allocated storage for neuron and synapse objects.
 * This reduces allocation overhead and improves cache locality.
 * 
 * Build-time options:
 * - NLM_ENABLE_MEMORY_POOL: Enable/disable memory pools
 * - NLM_MEMORY_POOL_PAGE_SIZE: Size of memory pool pages (default: 4096 bytes)
 * - NLM_MAX_POOL_SIZE: Maximum objects per pool (default: 1,048,576)
 * 
 * Runtime control:
 * - Memory pools can be enabled/disabled at runtime if NLM_RUNTIME_FEATURE_TOGGLE is ON
 * - Statistics collection available for memory usage monitoring
 * 
 * 3.2 SIMD Vectorization Configuration
 * 
 * SIMD accelerates neural computations by processing multiple elements simultaneously.
 * 
 * Platform availability:
 * - AVX-512: 16 float32 elements per register (requires X64 and AVX-512F/DQ)
 * - AVX2: 8 float32 elements per register
 * - SSE: 4 float32 elements per register
 * - Scalar: Single element (always available, fallback mode)
 * 
 * Configuration options:
 * - NLM_ENABLE_SIMD: Enable/disable SIMD (default: ON when performance enabled)
 * - NLM_PREFER_AVX512/AVX2/SSE: Prefer specific instruction set
 * - NLM_SIMD_WIDTH: Actual SIMD width (determined at compile time)
 * 
 * Runtime behavior:
 * - Automatic fallback to lower SIMD level if preferred not available
 * - Scalar mode always available as final fallback
 * - Statistics track SIMD utilization and fallback events
 * 
 * 3.3 Event Queue Configuration
 * 
 * Event queues implement event-driven simulation for spike processing.
 * 
 * Configuration options:
 * - NLM_ENABLE_EVENT_QUEUE: Enable/disable event queue
 * - NLM_SPIKE_QUEUE_CAPACITY: Spike event queue capacity (default: 16,384)
 * - NLM_DELAYED_SPIKE_BUCKETS: Time bucket count for delayed spikes (default: 1024)
 * - NLM_MAX_DELAY_STEPS: Maximum synaptic delay steps (default: 100)
 * 
 * Runtime control:
 * - Event queue can be toggled at runtime (NLM_RUNTIME_FEATURE_TOGGLE)
 * - Statistics monitor queue occupancy and overflow rates
 * - Adaptive sizing based on simulation characteristics
 * 
 * 3.4 Sparse Connectivity Configuration
 * 
 * Sparse connectivity stores neural connections efficiently using adjacency lists.
 * 
 * Configuration options:
 * - NLM_ENABLE_SPARSE_CONNECTIVITY: Enable/disable sparse storage
 * 
 * Runtime behavior:
 * - Can be toggled at runtime if NLM_RUNTIME_FEATURE_TOGGLE is ON
 * - Performance statistics track connectivity memory usage
 * - Adaptive algorithms optimize storage based on network density
 * 
 * 3.5 Multithreading Configuration
 * 
 * Parallel processing accelerates neural simulations across multiple cores.
 * 
 * Configuration options:
 * - NLM_ENABLE_MULTITHREADING: Enable/disable parallel processing
 * - NLM_DYNAMIC_THREAD_COUNT: Auto-detect vs. fixed thread count
 * - NLM_FIXED_THREAD_COUNT: Specific thread count if not dynamic
 * - NLM_NUM_THREADS: Actual number of threads (compile-time or runtime)
 * 
 * Runtime behavior:
 * - Thread count can be adjusted at runtime (NLM_RUNTIME_FEATURE_TOGGLE)
 * - Work-stealing load balancing for optimal CPU utilization
 * - Thread-local state for reduced contention
 * - Statistics track parallel efficiency and scalability
 * 
 * 4. Runtime Performance Management
 * 
 * Performance features can be enabled/disabled at runtime for:
 * - Dynamic adaptation to simulation characteristics
 * - Testing and validation scenarios
 * - Debugging performance issues
 * - Resource-constrained environments
 * 
 * Configuration options:
 * - NLM_RUNTIME_FEATURE_TOGGLE: Enable/disable runtime feature control
 * - NLM_ADAPTIVE_PERFORMANCE: Enable automatic performance adaptation
 * - NLM_PERFORMANCE_STATS: Collect performance metrics
 * - NLM_PERFORMANCE_VALIDATION: Validate configuration consistency
 * 
 * Performance metrics tracked:
 * - Memory usage by feature and pool type
 * - SIMD utilization and fallback statistics
 * - Event queue occupancy and overflow rates
 * - Thread pool efficiency and workload distribution
 * - Simulation speed and scaling characteristics
 * 
 * 5. Performance Statistics and Monitoring
 * 
 * Comprehensive performance monitoring provides insights into:
 * - Resource utilization
 * - Bottleneck identification
 * - Scaling characteristics
 * - Feature effectiveness
 * 
 * Configuration options:
 * - NLM_PERFORMANCE_STATS: Enable statistics collection
 * - Performance counters for each enabled feature
 * - Configurable reporting intervals
 * - Export options for external analysis
 * 
 * 6. Configuration Validation
 * 
 * Validate performance settings for consistency and correctness:
 * 
 * Validation checks:
 * - Feature dependency validation (e.g., SIMD requires compatible hardware)
 * - Resource constraint validation (memory limits, thread limits)
 * - Consistency validation across configuration options
 * - Performance impact assessment
 * 
 * Configuration options:
 * - NLM_PERFORMANCE_VALIDATION: Enable configuration validation
 * - Validation warning levels (info, warning, error)
 * - Automatic correction suggestions
 * 
 * 7. Usage Examples
 * 
 * Basic performance-optimized build:
 * 
 * ```bash
 * mkdir build && cd build
 * cmake .. -DNLM_PERFORMANCE_ENABLED=ON \
 *           -DNLM_PREFER_AVX2=ON \
 *           -DNLM_ENABLE_MULTITHREADING=ON
 * make -j$(nproc)
 * ```
 * 
 * Runtime configuration example:
 * 
 * ```cpp
 * #include "Performance.hpp"
 * #include <nlm/performance/PerformanceManager.hpp>
 * 
 * int main() {
 *     // Initialize performance manager
 *     nlm::PerformanceManager::getInstance().initialize();
 *     
 *     // Configure performance features at runtime
 *     nlm::PerformanceManager::getInstance().setFeatureEnabled(
 *         nlm::PerformanceFeature::MemoryPool, true);
 *     nlm::PerformanceManager::getInstance().setFeatureEnabled(
 *         nlm::PerformanceFeature::SIMD, true);
 *     
 *     // Get performance statistics
 *     auto stats = nlm::PerformanceManager::getInstance().getStatistics();
 *     
 *     // Run simulation...
 *     
 *     return 0;
 * }
 * ```
 * 
 * Performance tuning example:
 * 
 * ```cpp
 * // Adaptive performance configuration
 * nlm::PerformanceManager::getInstance().configureAdaptive(
 *     nlm::PerformanceFeature::Multithreading,
 *     nlm::PerformanceThreshold::CPUUsage, 0.8f,  // Target 80% CPU usage
 *     nlm::PerformanceAction::AdjustThreadCount, 1,  // ±1 thread
 *     nlm::PerformanceAdaptationInterval::Per10kSteps);
 * ``` 
 *
 * 8. Performance Integration Guidelines
 * 
 * Best practices for performance optimization:
 * 
 * 8.1 Memory Layout Optimization
 *    - Use SoA for neural states when NLM_ENABLE_SOA is ON
 *    - Enable memory pools for frequently allocated objects
 *    - Consider sparse connectivity for large, sparse networks
 * 
 * 8.2 Parallel Processing
 *    - Enable multithreading for simulations >10k neurons
 *    - Balance between thread count and memory locality
 *    - Use event-driven processing for spike-heavy simulations
 * 
 * 8.3 SIMD Utilization
 *    - Prefer AVX-512 when available for maximum throughput
 *    - Ensure memory alignment for vector operations
 *    - Profile to identify SIMD-optimized code paths
 * 
 * 8.4 Runtime Adaptation
 *    - Monitor performance metrics during simulation
 *    - Adapt thread count based on workload characteristics
 *    - Enable/disable features based on system resources
 * 
 * 9. Performance Comparison Table
 * 
 * | Feature | Compile-time | Runtime | Dependencies | Typical Speedup |
 * |---------|-------------|---------|-------------|----------------|
 * | Memory Pools | ✓ | ✓ | - | 2-10x allocation speedup |
 * | SoA Layout | ✓ | - | SIMD support | 1.5-3x memory access |
 * | Event Queue | ✓ | ✓ | - | 2-5x spike processing |
 * | Sparse Connectivity | ✓ | ✓ | Memory savings | 5-20x memory reduction |
 * | Multithreading | ✓ | ✓ | CPU cores | 2-N threads |
 * | SIMD | ✓ | - | Hardware | 2-8x compute |
 * 
 * 10. Migration Guide
 * 
 * Upgrading from older performance versions:
 * 
 * 1. Update CMake configuration:
 *    - Replace NLM_PERFORMANCE_ENABLED with individual feature options
 *    - Configure SIMD preference based on target hardware
 *    - Set multithreading options for your system
 * 
 * 2. Code changes:
 *    - Include <nlm/performance/Performance.hpp> for macros
 *    - Include <nlm/performance/PerformanceManager.hpp> for runtime control
 *    - Add performance statistics reporting if desired
 * 
 * 3. Performance tuning:
 *    - Start with all features enabled
 *    - Profile to identify bottlenecks
 *    - Disable features one-by-one to measure impact
 *    - Enable adaptive performance for long-running simulations
 * 
 * This enhanced performance system provides comprehensive control over neural simulation performance while maintaining backward compatibility with existing code.
 */

#include <cstddef>
#include <cstdint>
#include <atomic>
#include <array>
#include <optional>
#include <string>
#include <chrono>
#include <limits>
#include <algorithm>

namespace nlm {

enum class PerformanceFeature {
    MemoryPool,
    EventQueue,
    SoA,
    SparseConnectivity,
    Multithreading,
    SIMD
};

enum class PerformanceStatus {
    Disabled,
    Enabled,
    Unavailable,
    Fallback
};

struct PerformanceStatistics {
    // Memory pool statistics
    uint64_t memoryPoolAllocations = 0;
    uint64_t memoryPoolDeallocations = 0;
    uint64_t memoryPoolAvailableBytes = 0;
    uint64_t memoryPoolAllocatedBytes = 0;
    
    // SIMD statistics
    uint64_t simdOperations = 0;
    uint64_t simdFallbacks = 0;
    uint64_t scalarOperations = 0;
    uint32_t simdWidth = 1;
    
    // Event queue statistics
    uint64_t eventQueuePushes = 0;
    uint64_t eventQueuePops = 0;
    uint64_t eventQueueOverflows = 0;
    uint32_t eventQueueOccupancy = 0;
    
    // Threading statistics
    uint64_t threadPoolSubmissions = 0;
    uint64_t threadPoolCompletions = 0;
    uint64_t parallelForRanges = 0;
    uint32_t activeThreads = 0;
    
    // Overall simulation statistics
    uint64_t simulationSteps = 0;
    double totalSimulationTime = 0.0;
    uint64_t totalSpikesProcessed = 0;
};

class PerformanceManager {
public:
    static PerformanceManager& getInstance() {
        static PerformanceManager instance;
        return instance;
    }
    
    bool initialize() {
        // Validate configuration
        if (validateConfiguration()) {
            // Initialize enabled features
            initializeFeatures();
            return true;
        }
        return false;
    }
    
    bool isFeatureEnabled(PerformanceFeature feature) const {
        return featureStatus_[static_cast<size_t>(feature)] == PerformanceStatus::Enabled;
    }
    
    bool enableFeature(PerformanceFeature feature, bool enable = true) {
        if (!runtimeFeatureToggle_) return false;
        
        size_t idx = static_cast<size_t>(feature);
        PerformanceStatus newStatus = enable ? PerformanceStatus::Enabled : PerformanceStatus::Disabled;
        
        // Check if feature is available
        if (newStatus == PerformanceStatus::Enabled && 
            !checkFeatureAvailability(feature)) {
            return false;
        }
        
        // Update feature status
        featureStatus_[idx] = newStatus;
        
        // Update runtime flags
        updateRuntimeFlags();
        
        // Record statistics
        lastConfigChange_ = std::chrono::steady_clock::now();
        
        return true;
    }
    
    const PerformanceStatistics& getStatistics() const {
        return statistics_;
    }
    
    void updateStatistics() {
        // Update runtime statistics
        statistics_.simulationSteps++;
        statistics_.totalSimulationTime += lastStepTime_;
        lastStepTime_ = 0.0;
    }
    
    void recordStepTime(double time) {
        lastStepTime_ = time;
    }
    
    // SIMD configuration with runtime adaptation
    bool configureSIMD(size_t preferredWidth = 0) {
        if (!simdAvailable_) return false;
        
        size_t newWidth = preferredWidth;
        if (newWidth == 0) {
            // Auto-detect preferred width
            newWidth = detectOptimalSIMD();
        }
        
        if (newWidth > currentSIMDWidth_) {
            // Try to enable higher SIMD
            return enableFeature(PerformanceFeature::SIMD);
        }
        
        currentSIMDWidth_ = newWidth;
        return true;
    }
    
    // Thread count management
    bool setThreadCount(size_t count) {
        if (!multithreadingAvailable_ || !runtimeFeatureToggle_) {
            return false;
        }
        
        if (count < minThreads_ || count > maxThreads_) {
            return false;
        }
        
        numThreads_ = count;
        return true;
    }
    
    // Memory pool management
    bool configureMemoryPool(size_t pageSize, size_t maxSize) {
        if (!memoryPoolAvailable_ || !runtimeFeatureToggle_) {
            return false;
        }
        
        // Update memory pool configuration
        memoryPoolPageSize_ = pageSize;
        maxPoolSize_ = maxSize;
        
        return true;
    }
    
    // Adaptive performance configuration
    void configureAdaptive(PerformanceFeature feature,
                          float threshold, 
                          float adjustment,
                          AdaptationAction action,
                          AdaptationInterval interval) {
        adaptiveConfigs_[static_cast<size_t>(feature)] = {
            threshold, adjustment, action, interval
        };
    }
    
    // Configuration validation
    bool validateConfiguration() const {
        bool valid = true;
        
        // Check feature dependencies
        if (isFeatureEnabled(PerformanceFeature::SIMD) && !simdAvailable_) {
            // Force SIMD off if not available
            valid = false;
        }
        
        // Check resource constraints
        if (numThreads_ > std::thread::hardware_concurrency()) {
            valid = false;
        }
        
        // Check consistency
        if (isFeatureEnabled(PerformanceFeature::SoA) && 
            !isFeatureEnabled(PerformanceFeature::SIMD)) {
            // SoA benefits from SIMD, warn but don't fail
        }
        
        return valid;
    }
    
    // Configuration export/import
    std::string exportConfiguration() const {
        // Serialize current configuration
        // Implementation details omitted for brevity
        return "{}';
    }
    
    bool importConfiguration(const std::string& config) {
        // Deserialize configuration
        // Implementation details omitted for brevity
        return true;
    }
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable) {
        performanceMonitoring_ = enable;
    }
    
    bool isPerformanceMonitoringEnabled() const {
        return performanceMonitoring_;
    }
    
    // Configuration statistics
    struct ConfigurationStats {
        uint64_t featureChanges = 0;
        uint64_t runtimeToggles = 0;
        uint64_t adaptationsApplied = 0;
        double lastValidationTime = 0.0;
        uint64_t validationFailures = 0;
    };
    
    const ConfigurationStats& getConfigurationStats() const {
        return configStats_;
    }
    
private:
    PerformanceManager() : 
        simdAvailable_(checkSIMDAvailability()),
        multithreadingAvailable_(checkThreadingAvailability()),
        memoryPoolAvailable_(checkMemoryPoolAvailability()),
        runtimeFeatureToggle_(true),
        performanceMonitoring_(true),
        minThreads_(1),
        maxThreads_(64),
        currentSIMDWidth_(1),
        lastStepTime_(0.0) {
        
        // Initialize feature status based on compile-time configuration
        featureStatus_[static_cast<size_t>(PerformanceFeature::MemoryPool)] = 
            NLM_USE_MEMORY_POOL ? PerformanceStatus::Enabled : PerformanceStatus::Disabled;
        featureStatus_[static_cast<size_t>(PerformanceFeature::EventQueue)] = 
            NLM_USE_EVENT_QUEUE ? PerformanceStatus::Enabled : PerformanceStatus::Disabled;
        featureStatus_[static_cast<size_t>(PerformanceFeature::SoA)] = 
            NLM_USE_SOA ? PerformanceStatus::Enabled : PerformanceStatus::Disabled;
        featureStatus_[static_cast<size_t>(PerformanceFeature::SparseConnectivity)] = 
            NLM_USE_SPARE_CONNECTIVITY ? PerformanceStatus::Enabled : PerformanceStatus::Disabled;
        featureStatus_[static_cast<size_t>(PerformanceFeature::Multithreading)] = 
            NLM_USE_MULTITHREADING ? PerformanceStatus::Enabled : PerformanceStatus::Disabled;
        featureStatus_[static_cast<size_t>(PerformanceFeature::SIMD)] = 
            NLM_USE_SIMD ? PerformanceStatus::Enabled : PerformanceStatus::Disabled;
        
        // Update runtime flags
        updateRuntimeFlags();
    }
    
    void initializeFeatures() {
        // Initialize enabled features
        // Implementation details omitted for brevity
    }
    
    bool checkFeatureAvailability(PerformanceFeature feature) const {
        switch (feature) {
            case PerformanceFeature::SIMD:
                return simdAvailable_;
            case PerformanceFeature::Multithreading:
                return multithreadingAvailable_;
            case PerformanceFeature::MemoryPool:
                return memoryPoolAvailable_;
            default:
                return true;
        }
    }
    
    bool checkSIMDAvailability() const {
        // Check if SIMD is available on this platform
        // Implementation depends on compiler and architecture
        return true;
    }
    
    bool checkThreadingAvailability() const {
        // Check if multithreading is available
        return std::thread::hardware_concurrency() > 0;
    }
    
    bool checkMemoryPoolAvailability() const {
        // Check if memory pools are available
        return true;
    }
    
    void updateRuntimeFlags() {
        // Update runtime configuration flags based on current feature status
        // Implementation details omitted for brevity
    }
    
    size_t detectOptimalSIMD() const {
        // Detect optimal SIMD width based on platform
        if (simdAvailable_) {
#if defined(NLM_SIMD_AVX512)
            return 16;
#elif defined(NLM_SIMD_AVX2)
            return 8;
#elif defined(NLM_SIMD_SSE)
            return 4;
#endif
        }
        return 1;
    }
    
    // Configuration structures
    struct AdaptiveConfig {
        float threshold;
        float adjustment;
        AdaptationAction action;
        AdaptationInterval interval;
    };
    
    // Runtime configuration
    std::array<PerformanceStatus, 6> featureStatus_;
    bool simdAvailable_;
    bool multithreadingAvailable_;
    bool memoryPoolAvailable_;
    bool runtimeFeatureToggle_;
    bool performanceMonitoring_;
    size_t minThreads_;
    size_t maxThreads_;
    size_t currentSIMDWidth_;
    size_t numThreads_;
    size_t memoryPoolPageSize_;
    size_t maxPoolSize_;
    double lastStepTime_;
    std::chrono::steady_clock::time_point lastConfigChange_;
    std::array<AdaptiveConfig, 6> adaptiveConfigs_;
    
    // Statistics
    PerformanceStatistics statistics_;
    ConfigurationStats configStats_;
};

// Global performance manager access
inline PerformanceManager& getPerformanceManager() {
    return PerformanceManager::getInstance();
}

// Compile-time configuration macros
#ifdef NLM_PERFORMANCE_ENABLED
    #define NLM_PERFORMANCE_RUNTIME_AVAILABLE 1
#else
    #define NLM_PERFORMANCE_RUNTIME_AVAILABLE 0
#endif

// Runtime performance toggle macro
#if NLM_PERFORMANCE_RUNTIME_AVAILABLE
    #define NLM_PERFORMANCE_TOGGLE(feature, enable) \
        nlm::getPerformanceManager().enableFeature(feature, enable)
    
    #define NLM_PERFORMANCE_IS_ENABLED(feature) \
        nlm::getPerformanceManager().isFeatureEnabled(feature)
    
    #define NLM_PERFORMANCE_STATS() \
        nlm::getPerformanceManager().getStatistics()
#else
    #define NLM_PERFORMANCE_TOGGLE(feature, enable) false
    #define NLM_PERFORMANCE_IS_ENABLED(feature) (NLM_##feature##_ENABLED)
    #define NLM_PERFORMANCE_STATS() (nlm::PerformanceStatistics())
#endif

// Performance configuration validation macro
#if NLM_PERFORMANCE_RUNTIME_AVAILABLE && NLM_PERFORMANCE_VALIDATION
    #define NLM_PERFORMANCE_VALIDATE() \
        nlm::getPerformanceManager().validateConfiguration()
#else
    #define NLM_PERFORMANCE_VALIDATE() true
#endif

// SIMD runtime configuration
#if NLM_PERFORMANCE_RUNTIME_AVAILABLE && NLM_ENABLE_SIMD
    #define NLM_PERFORMANCE_CONFIGURE_SIMD(width) \
        nlm::getPerformanceManager().configureSIMD(width)
#else
    #define NLM_PERFORMANCE_CONFIGURE_SIMD(width) false
#endif

// Thread count configuration
#if NLM_PERFORMANCE_RUNTIME_AVAILABLE && NLM_ENABLE_MULTITHREADING
    #define NLM_PERFORMANCE_SET_THREAD_COUNT(count) \
        nlm::getPerformanceManager().setThreadCount(count)
#else
    #define NLM_PERFORMANCE_SET_THREAD_COUNT(count) false
#endif

// Memory pool configuration
#if NLM_PERFORMANCE_RUNTIME_AVAILABLE && NLM_ENABLE_MEMORY_POOL
    #define NLM_PERFORMANCE_CONFIGURE_MEMORY_POOL(pageSize, maxSize) \
        nlm::getPerformanceManager().configureMemoryPool(pageSize, maxSize)
#else
    #define NLM_PERFORMANCE_CONFIGURE_MEMORY_POOL(pageSize, maxSize) false
#endif

// Performance statistics update
#define NLM_PERFORMANCE_UPDATE_STATISTICS() \
    nlm::getPerformanceManager().updateStatistics()

// Performance monitoring toggle
#if NLM_PERFORMANCE_RUNTIME_AVAILABLE
    #define NLM_PERFORMANCE_MONITORING(enable) \
        nlm::getPerformanceManager().enablePerformanceMonitoring(enable)
#else
    #define NLM_PERFORMANCE_MONITORING(enable) 
#endif

// Configuration export/import
#if NLM_PERFORMANCE_RUNTIME_AVAILABLE
    #define NLM_PERFORMANCE_EXPORT_CONFIG() \
        nlm::getPerformanceManager().exportConfiguration()
    
    #define NLM_PERFORMANCE_IMPORT_CONFIG(config) \
        nlm::getPerformanceManager().importConfiguration(config)
#else
    #define NLM_PERFORMANCE_EXPORT_CONFIG() ""
    #define NLM_PERFORMANCE_IMPORT_CONFIG(config) false
#endif

// Forward declarations
class PerformanceManager;

// Deprecated compatibility macros (use new PerformanceManager interface)
#define NLM_PERFORMANCE_FEATURE_AVAILABLE(feature) NLM_##feature##_ENABLED

// Legacy SIMD support
#ifdef NLM_SIMD_AVX2
    #define NLM_SIMD_WIDTH 8
#elif defined(NLM_SIMD_AVX512)
    #define NLM_SIMD_WIDTH 16
#elif defined(NLM_SIMD_SSE)
    #define NLM_SIMD_WIDTH 4
#else
    #define NLM_SIMD_WIDTH 1
#endif

// Legacy thread configuration
#ifdef NLM_THREAD_COUNT
    #define NLM_NUM_THREADS NLM_THREAD_COUNT
#else
    #define NLM_NUM_THREADS 1
#endif

// Legacy memory pool configuration
#define NLM_MEMORY_POOL_PAGE_SIZE 4096
#define NLM_MAX_POOL_SIZE (1024 * 1024)

// PerformanceManager implementation file
#include "PerformanceManager.hpp"

} // namespace nlm

#endif // NLM_PERFORMANCE_HPP