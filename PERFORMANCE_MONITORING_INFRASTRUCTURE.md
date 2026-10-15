# Performance Monitoring Infrastructure

This document describes the performance monitoring and profiling infrastructure implemented for the NLM (Neural Learning Machine) framework.

## Overview

The performance monitoring infrastructure provides comprehensive insights into simulation performance, memory usage, and computational efficiency. It includes:

- Real-time performance monitoring
- Memory usage tracking and leak detection
- Cache performance analysis
- Custom profiling hooks
- Automated performance reporting

## Components

### 1. Performance Profiler

The `PerformanceProfiler` class provides high-resolution timing and profiling capabilities:

```cpp
class PerformanceProfiler {
public:
    struct ProfilerData {
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point lastFrameTime;
        double totalTime;
        double frameTime;
        size_t frames;
        size_t spikeProcessTime;
        size_t neuronStepTime;
        size_t memoryAllocationTime;
        size_t plasticityUpdateTime;
    };
    
    static void startFrame();
    static void endFrame();
    static void recordSpikeProcessing(size_t timeNs);
    static void recordNeuronStepping(size_t timeNs);
    static void recordMemoryAllocation(size_t timeNs);
    static void recordPlasticityUpdate(size_t timeNs);
    static void reportPerformance();
};
```

### 2. Memory Management Tracker

The `MemoryTracker` class tracks memory allocations and detects leaks:

```cpp
class MemoryTracker {
public:
    struct AllocationRecord {
        void* ptr;
        size_t size;
        const char* file;
        int line;
        std::chrono::steady_clock::time_point timestamp;
        bool isPool;
        size_t poolId;
    };
    
    void trackAllocation(void* ptr, size_t size, const char* file, int line, 
                        bool isPool = false, size_t poolId = 0);
    
    void trackDeallocation(void* ptr, bool isPool = false, size_t poolId = 0);
    
    size_t getActiveAllocations() const;
    size_t getPeakAllocations() const;
    
    void reportMemoryLeaks() const;
    double getMemoryUsageMB() const;
    void reset();
};
```

### 3. Memory Monitor

The `MemoryMonitor` class provides real-time memory usage monitoring:

```cpp
class MemoryMonitor {
public:
    struct MemoryStats {
        size_t totalAllocations;
        size_t totalDeallocations;
        size_t activeAllocations;
        size_t allocatedBytes;
        double allocationRate;  // allocations per second
        double deallocationRate;
        float memoryUsagePercent;
        size_t objectsInUse;
    };
    
    void start(size_t sampleIntervalMs = 100);
    void stop();
    bool isRunning() const;
    MemoryStats getStats() const;
    void setAllocationCallback(std::function<void(const MemoryStats&)>) {}
    void reportMemoryUsage() const;
};
```

### 4. Advanced Memory Manager

The `AdvancedMemoryManager` provides sophisticated memory management with cache optimization:

```cpp
class AdvancedMemoryManager {
public:
    size_t allocationCount;
    size_t deallocationCount;
    size_t cacheHits;
    size_t cacheMisses;
    std::chrono::steady_clock::time_point lastCleanup;
    
    void trackAllocation() { allocationCount++; }
    void trackDeallocation() { deallocationCount++; }
    void trackCacheHit() { cacheHits++; }
    void trackCacheMiss() { cacheMisses++; }
    
    double getCacheHitRate() const;
    bool needsCleanup() const;
    void performCleanup();
};
```

### 5. Intellectual Optimizer

The `IntellectualOptimizer` class implements advanced optimization algorithms:

```cpp
class IntellectualOptimizer {
public:
    double temperature;  // For simulated annealing
    double explorationRate;
    double exploitationRate;
    size_t adaptationSteps;
    
    void adapt(double error);
    float getExplorationBias() const;
};
```

## Usage Examples

### Basic Performance Monitoring

```cpp
// Enable performance monitoring
PerformanceProfiler::startFrame();

// Run simulation
brain->step(step, currentTime);

// End frame and collect stats
PerformanceProfiler::endFrame();

// Record specific operations
PerformanceProfiler::recordSpikeProcessing(timeNs);
PerformanceProfiler::recordNeuronStepping(timeNs);
PerformanceProfiler::recordMemoryAllocation(timeNs);
PerformanceProfiler::recordPlasticityUpdate(timeNs);
```

### Memory Leak Detection

```cpp
// Initialize memory tracking
auto& memoryTracker = MemoryTracker::getInstance();

// Perform simulation
brain->initialize();
brain->step(step, currentTime);

// Check for memory leaks
memoryTracker.reportMemoryLeaks();

// Get memory usage
std::cout << "Memory usage: " << memoryTracker.getMemoryUsageMB() << " MB" << std::endl;
```

### Memory Usage Monitoring

```cpp
// Start memory monitoring
memoryMonitor.start(1000); // Sample every 1 second

// Run simulation
brain->initialize();
for (size_t step = 0; step < steps; ++step) {
    brain->step(step, currentTime);
}

// Stop monitoring
memoryMonitor.stop();

// Get memory statistics
auto stats = memoryMonitor.getStats();
std::cout << "Allocations: " << stats.totalAllocations << std::endl;
std::cout << "Active allocations: " << stats.activeAllocations << std::endl;
std::cout << "Memory usage: " << stats.allocatedBytes << " bytes" << std::endl;
```

### Cache Performance Analysis

```cpp
// Monitor cache performance
advancedMemoryManager.trackCacheHit();
advancedMemoryManager.trackCacheMiss();

// Get cache hit rate
double hitRate = advancedMemoryManager.getCacheHitRate();
std::cout << "Cache hit rate: " << hitRate * 100 << "%" << std::endl;

// Perform cleanup if needed
if (advancedMemoryManager.needsCleanup()) {
    advancedMemoryManager.performCleanup();
}
```

### Intellectual Optimization

```cpp
// Create intellectual optimizer
IntellectualOptimizer optimizer;

// Adapt to performance errors
optimizer.adapt(error);

// Get exploration bias
float explorationBias = optimizer.getExplorationBias();
std::cout << "Exploration bias: " << explorationBias << std::endl;
```

## Integration with NLM Brain

The performance monitoring infrastructure is integrated with the NLM Brain class:

```cpp
// Enable comprehensive performance monitoring
Brain::Impl::PerformanceProfiler::startFrame();

// Initialize brain with performance tracking
brain->initialize();

// Run simulation with monitoring
for (size_t step = 0; step < steps; ++step) {
    brain->step(step, currentTime);
    PerformanceProfiler::endFrame();
    
    // Record specific operations
    PerformanceProfiler::recordSpikeProcessing(timeNs);
    PerformanceProfiler::recordNeuronStepping(timeNs);
    PerformanceProfiler::recordMemoryAllocation(timeNs);
    PerformanceProfiler::recordPlasticityUpdate(timeNs);
}

// Generate performance report
PerformanceProfiler::reportPerformance();

// Check for memory leaks
memoryTracker.reportMemoryLeaks();

// Get memory usage
memoryMonitor.reportMemoryUsage();
```

## Performance Reports

### Memory Usage Report

The memory usage report provides detailed information about memory consumption:

```
[MEMORY] Total allocations: 1,234,567
[MEMORY] Active allocations: 12,345
[MEMORY] Memory usage: 123.45 MB
[MEMORY] Cache hit rate: 95.2%
[MEMORY] Cache misses: 612
[MEMORY] Cleanup performed: Yes
```

### Performance Report

The performance report provides detailed information about simulation performance:

```
Performance: 0.50ms frame, 2000 fps
  - Spike processing: 0.10ms
  - Neuron stepping: 0.25ms
  - Memory allocation: 0.05ms
  - Plasticity update: 0.10ms
```

## Configuration

Performance monitoring can be configured through the `NLM_PERFORMANCE_HPP` configuration:

```cpp
// Enable all performance features
#define NLM_PERFORMANCE_ENABLED 1

// SIMD width
#define NLM_SIMD_AVX2 1

// Thread count
#define NLM_THREAD_COUNT 8

// Memory pool size
#define NLM_MAX_POOL_SIZE (1024 * 1024)
```

## Benefits

1. **Real-time Performance Monitoring**: Continuous monitoring of simulation performance
2. **Memory Leak Detection**: Automatic detection of memory leaks
3. **Cache Performance Analysis**: Tracking of cache hit/miss ratios
4. **Custom Profiling**: Hooks for custom profiling needs
5. **Automated Reporting**: Automatic generation of performance reports
6. **Memory Optimization**: Advanced memory management with cleanup mechanisms
7. **Intellectua Optimization**: Dynamic adaptation based on performance errors
8. **Scalability**: Support for large-scale neural network simulations

## Limitations

1. **Overhead**: Performance monitoring adds some overhead to simulations
2. **Memory Usage**: Memory tracking consumes additional memory
3. **Thread Safety**: Some components require thread synchronization
4. **Configuration**: Requires proper configuration for optimal performance

## Future Enhancements

1. **Distributed Monitoring**: Support for distributed performance monitoring
2. **GPU Profiling**: Integration with GPU performance counters
3. **Network Monitoring**: Monitoring of distributed simulations
4. **AI-based Optimization**: Machine learning-based performance optimization
5. **Predictive Analytics**: Predictive analysis of performance trends

This infrastructure provides a comprehensive foundation for performance monitoring and optimization of NLM neural network simulations.
