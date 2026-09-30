# NLM Performance Guide

## Overview

This document provides comprehensive performance optimization guidelines for the NLM (熙然) artificial brain system. It covers build optimization, runtime performance, memory management, and simulation efficiency.

## Performance Optimization Strategies

### 1. Build Performance Optimization

#### Compiler Optimizations
```bash
# Optimized build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native -fomit-frame-pointer"
make -j$(nproc)
```

#### Parallel Build
```bash
# Multi-core build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)  # Use all available cores
```

#### Incremental Build
```bash
# Incremental builds for faster development
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# For faster compilation
make -j$(nproc)
```

### 2. Runtime Performance Optimization

#### Configuration-Based Optimization
```bash
# Optimized configuration for performance testing
./nlm --simulation_timestep 0.01 \
     --neuron_count 500 \
     --region_count 2 \
     --max_simulation_steps 10000 \
     --log_level WARNING
```

#### Memory Management
```cpp
// Configure memory management
auto config = std::make_shared<nlm::Config>();

// Set memory-efficient parameters
config->set("neuron_count", 1000);
config->set("working_memory_capacity", 100);
config->set("episodic_memory_max_episodes", 500);

// Enable memory pooling if available
config->set("enable_memory_pooling", true);
```

### 3. Simulation Performance Optimization

#### Timestep Optimization
```cpp
// Optimize timestep for performance
nlm::TimestepDuration optimal_timestep = 0.001f;  // 1ms
nlm::Timestamp start_time = 0.0f;

// Run simulation with optimized timestep
for (nlm::SimulationStep step = 0; step < 10000; ++step) {
    brain.step(start_time);
    start_time += optimal_timestep;
}
```

#### Neuron Optimization
```cpp
// Configure neurons for performance
nlm::Neuron neuron(nlm::NeuronId(1));

// Optimize neuron parameters
neuron.setRefractoryPeriod(2);  // Short refractory period for faster processing
neuron.setFiringState(nlm::FiringState::Resting);
neuron.setMembranePotential(-70.0f);

// Use optimized integration
neuron.step(current_time, optimal_timestep);
```

#### Synapse Optimization
```cpp
// Configure synapses for performance
nlm::Synapse synapse(nlm::SynapseId(1), src, dst);

// Optimize synaptic parameters
nlm::SynapseIntegrator integrator(synapse);
integrator.setFacilitationTimeConstant(100.0f);   // Faster facilitation
integrator.setDepressionTimeConstant(200.0f);    // Faster depression
integrator.setEligibilityDecayRate(0.001f);       // Fast eligibility decay

// Use optimized integration
integrator.step(current_time, config.get());
```

## Performance Benchmarking

### Basic Benchmarking
```bash
#!/bin/bash
# benchmark.sh - Performance benchmark script

BUILD_DIR="build"
RESULTS_FILE="benchmark_results.csv"

# Build project
mkdir -p $BUILD_DIR
cd $BUILD_DIR
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run benchmark
./nlm_benchmark \
    --iterations 100 \
    --neuron_count 1000 \
    --region_count 4 \
    --simulation_timestep 0.001 \
    --duration 10.0

# Save results
cd ..
./nlm_benchmark --output-results $RESULTS_FILE

# Analyze results
python3 analyze_benchmark.py $RESULTS_FILE
```

### Benchmarking Framework
```cpp
// benchmark_framework.h
#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>

class PerformanceBenchmark {
public:
    struct BenchmarkResult {
        std::string test_name;
        double elapsed_time;
        size_t neurons_per_second;
        double memory_usage;
        std::map<std::string, double> metrics;
    };
    
    PerformanceBenchmark() = default;
    
    // Run benchmark with specific configuration
    BenchmarkResult runBenchmark(const std::string& name,
                                 size_t neuron_count,
                                 size_t region_count,
                                 double timestep,
                                 size_t steps);
    
    // Run comparative benchmark
    std::vector<BenchmarkResult> comparativeBenchmark(
        const std::vector<std::map<std::string, double>>& configurations);
    
    // Generate performance report
    void generateReport(const std::vector<BenchmarkResult>& results,
                       const std::string& output_file);
    
    // Analyze performance bottlenecks
    std::map<std::string, double> analyzeBottlenecks(
        const std::vector<BenchmarkResult>& results);
};
```

### Performance Measurement
```cpp
// performance_monitor.h
#pragma once

#include <chrono>
#include <memory>
#include <string>

class PerformanceMonitor {
public:
    struct Metrics {
        double cpu_usage;
        double memory_usage;
        double simulation_speed;
        size_t spike_events_processed;
        double avg_firing_rate;
    };
    
    PerformanceMonitor() = default;
    
    // Start monitoring
    void startMonitoring();
    
    // Stop monitoring and get metrics
    Metrics stopMonitoring();
    
    // Record specific metric
    void recordMetric(const std::string& name, double value);
    
    // Get current metrics
    Metrics getCurrentMetrics() const;
    
    // Generate performance report
    std::string generateReport() const;
    
private:
    std::chrono::high_resolution_clock::time_point start_time_;
    Metrics current_metrics_;
    std::map<std::string, double> custom_metrics_;
};
```

## Memory Optimization

### Memory Pooling
```cpp
// memory_pool.h
#pragma once

#include <vector>
#include <memory>
#include <mutex>

template<typename T>
class MemoryPool {
public:
    explicit MemoryPool(size_t capacity = 1000);
    
    // Allocate object from pool
    T* allocate();
    
    // Deallocate object
    void deallocate(T* ptr);
    
    // Get pool statistics
    size_t getAllocatedCount() const;
    size_t getAvailableCount() const;
    
private:
    std::vector<std::unique_ptr<T>> pool_;
    std::vector<T*> available_;
    size_t allocated_count_;
    mutable std::mutex mutex_;
};
```

### Efficient Data Structures
```cpp
// efficient_spike_buffer.h
#pragma once

#include <vector>
#include <algorithm>
#include <chrono>

class EfficientSpikeBuffer {
public:
    struct SpikeEvent {
        nlm::NeuronId neuron_id;
        nlm::Timestamp timestamp;
        nlm::SimulationStep step;
    };
    
    explicit EfficientSpikeBuffer(size_t capacity = 10000);
    
    // Add spike event
    void addSpike(const SpikeEvent& spike);
    
    // Get spikes within time window
    std::vector<SpikeEvent> getSpikes(
        nlm::Timestamp start_time,
        nlm::Timestamp end_time) const;
    
    // Clear buffer
    void clear();
    
    // Get buffer statistics
    size_t getSize() const;
    double getMemoryUsage() const;
    
private:
    std::vector<SpikeEvent> spikes_;
    size_t max_capacity_;
    nlm::Timestamp oldest_timestamp_;
    nlm::Timestamp newest_timestamp_;
};
```

## Simulation Optimization

### Event-Driven Simulation
```cpp
// event_driven_simulator.h
#pragma once

#include <chrono>
#include <queue>
#include <functional>
#include <unordered_map>

class EventDrivenSimulator {
public:
    using EventCallback = std::function<void(const Event&)>;
    
    struct Event {
        enum Type {
            SPIKE,
            DELAYED_SPIKE,
            PLASTICITY_UPDATE,
            MEMORY_UPDATE,
            NEUROMODULATION_UPDATE
        };
        
        Type type;
        nlm::Timestamp timestamp;
        nlm::SimulationStep step;
        nlm::NeuronId neuron_id;
        std::any data;
    };
    
    EventDrivenSimulator() = default;
    
    // Schedule event
    void scheduleEvent(const Event& event);
    
    // Run simulation until time
    void runUntil(nlm::Timestamp target_time);
    
    // Register event handler
    void registerHandler(Event::Type type, const EventCallback& callback);
    
    // Get simulation time
    nlm::Timestamp getCurrentTime() const;
    
    // Get simulation step
    nlm::SimulationStep getCurrentStep() const;
    
private:
    nlm::Timestamp current_time_;
    nlm::SimulationStep current_step_;
    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> event_queue_;
    std::unordered_map<Event::Type, std::vector<EventCallback>> handlers_;
};
```

### Parallel Processing
```cpp
// parallel_processor.h
#pragma once

#include <vector>
#include <thread>
#include <future>
#include <functional>
#include <atomic>

class ParallelProcessor {
public:
    ParallelProcessor(size_t max_threads = std::thread::hardware_concurrency());
    
    // Process items in parallel
    template<typename T, typename F>
    std::vector<typename std::result_of<F(T)>::type> processParallel(
        const std::vector<T>& items,
        const F& processor,
        size_t batch_size = 100);
    
    // Process neuron spikes in parallel
    void processSpikesParallel(
        const std::vector<nlm::Neuron*>& neurons,
        const std::function<void(nlm::Neuron*)>& spike_handler);
    
    // Process synaptic connections in parallel
    void processConnectionsParallel(
        const std::vector<nlm::Synapse*>& synapses,
        const std::function<void(nlm::Synapse*)>& connection_handler);
    
    // Get processor statistics
    size_t getThreadCount() const;
    double getEfficiency() const;
    
private:
    size_t max_threads_;
    std::vector<std::thread> threads_;
    std::atomic<size_t> active_threads_;
    double efficiency_;
};
```

## Profiling and Optimization

### Performance Profiling
```bash
#!/bin/bash
# profile_performance.sh - Performance profiling script

BUILD_DIR="build"
PROFILE_OUTPUT="profile_results"

# Build with profiling support
mkdir -p $BUILD_DIR
cd $BUILD_DIR
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_PROFILING=ON
make -j$(nproc)

# Run with profiling
./nlm --profile --output $PROFILE_OUTPUT

# Analyze profile
python3 analyze_profile.py $PROFILE_OUTPUT

# Generate optimization recommendations
cd ..
./generate_optimization_recommendations.py $PROFILE_OUTPUT
```

### Profiling Tools
```cpp
// profiler.h
#pragma once

#include <chrono>
#include <string>
#include <map>
#include <vector>
#include <fstream>

class Profiler {
public:
    struct ProfileData {
        std::string function_name;
        double total_time;
        double average_time;
        double min_time;
        double max_time;
        uint64_t call_count;
        double cpu_time_percentage;
        double memory_percentage;
    };
    
    Profiler() = default;
    
    // Start profiling section
    void startSection(const std::string& name);
    
    // End profiling section
    void endSection(const std::string& name);
    
    // Get profile data
    std::vector<ProfileData> getProfileData() const;
    
    // Generate profile report
    void generateReport(const std::string& output_file) const;
    
    // Save profile data to file
    void saveToFile(const std::string& output_file) const;
    
    // Load profile data from file
    void loadFromFile(const std::string& input_file);
    
    // Clear profile data
    void clear();
    
    // Get performance bottlenecks
    std::vector<std::string> getPerformanceBottlenecks() const;
    
    // Get optimization recommendations
    std::vector<std::string> getOptimizationRecommendations() const;
    
private:
    std::map<std::string, std::chrono::high_resolution_clock::time_point> start_times_;
    std::map<std::string, std::chrono::duration<double>> elapsed_times_;
    std::map<std::string, uint64_t> call_counts_;
    std::vector<ProfileData> profile_data_;
};
```

### Memory Profiling
```cpp
// memory_profiler.h
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>

class MemoryProfiler {
public:
    struct MemoryUsage {
        size_t total_allocated;
        size_t total_deallocated;
        size_t current_usage;
        size_t peak_usage;
        std::map<std::string, size_t> allocations_by_type;
    };
    
    MemoryProfiler() = default;
    
    // Start memory tracking
    void startMemoryTracking();
    
    // Stop memory tracking
    MemoryUsage stopMemoryTracking();
    
    // Track memory allocation
    void trackAllocation(const std::string& type, size_t size);
    
    // Track memory deallocation
    void trackDeallocation(const std::string& type, size_t size);
    
    // Get current memory usage
    MemoryUsage getCurrentMemoryUsage() const;
    
    // Generate memory report
    void generateMemoryReport(const std::string& output_file) const;
    
    // Detect memory leaks
    std::vector<std::string> detectMemoryLeaks() const;
    
    // Get memory optimization recommendations
    std::vector<std::string> getMemoryOptimizationRecommendations() const;
    
private:
    MemoryUsage current_usage_;
    std::map<std::string, size_t> allocation_history_;
    std::map<std::string, size_t> deallocation_history_;
};
```

## Configuration for Performance

### Performance Configuration Options
```cpp
// performance_config.h
#pragma once

#include <string>

class PerformanceConfig {
public:
    struct Settings {
        // Simulation settings
        double simulation_timestep;
        size_t max_simulation_steps;
        
        // Memory settings
        size_t working_memory_capacity;
        size_t episodic_memory_capacity;
        bool enable_memory_pooling;
        
        // Processing settings
        size_t max_threads;
        size_t batch_size;
        
        // Logging settings
        std::string log_level;
        bool enable_profiling;
        std::string profile_output_file;
        
        // Optimization settings
        bool enable_simd;
        bool enable_parallel;
        bool enable_event_driven;
        
        // Hardware settings
        size_t neuron_count;
        size_t region_count;
        size_t synaptic_connections;
    };
    
    PerformanceConfig() = default;
    
    // Load configuration from file
    bool loadFromFile(const std::string& filename);
    
    // Get configuration settings
    const Settings& getSettings() const;
    
    // Update configuration settings
    void updateSetting(const std::string& key, const std::string& value);
    
    // Generate performance-optimized configuration
    static Settings generateOptimizedConfig(
        size_t neuron_count,
        size_t region_count,
        const std::string& target_hardware);
    
    // Get performance recommendations
    std::vector<std::string> getPerformanceRecommendations() const;
    
private:
    Settings settings_;
};
```

### Hardware-Specific Configuration
```bash
#!/bin/bash
# configure_for_hardware.sh - Hardware-specific configuration

HARDWARE_TYPE="$1"  # x86_64, ARM, GPU, etc.
NEURON_COUNT="$2"
MEMORY_LIMIT="$3"

# Generate hardware-specific configuration
cat > hardware_config.cfg << EOF
# Hardware-specific NLM configuration
# Target hardware: $HARDWARE_TYPE

# Simulation parameters
simulation_timestep = 0.001
max_simulation_steps = 100000
neuron_count = $NEURON_COUNT
region_count = 4

# Memory optimization
working_memory_capacity = 200
e episodic_memory_capacity = 1000
enable_memory_pooling = true
max_memory_usage = $MEMORY_LIMIT

# Processing optimization
max_threads = $(nproc)
batch_size = 100
enable_simd = true
enable_parallel = true

# Logging optimization
log_level = WARNING
enable_profiling = false
profile_output_file = null

# Hardware-specific settings
case $HARDWARE_TYPE in
    x86_64)
        enable_avx = true
        enable_sse = true
        enable_fma = true
        ;;
    ARM)
        enable_neon = true
        enable_sve = false
        ;;
    GPU)
        enable_cuda = true
        enable_opencl = true
        ;;
    *)
        enable_simd = false
        enable_parallel = false
        ;;
esac

EOF

echo "Generated hardware-specific configuration: hardware_config.cfg"
```

## Performance Testing

### Automated Performance Testing
```bash
#!/bin/bash
# run_performance_tests.sh - Automated performance testing

TEST_DIR="performance_tests"
RESULTS_DIR="performance_results"

mkdir -p $RESULTS_DIR

# Run performance tests for different configurations
cd $TEST_DIR

# Configuration 1: Basic performance
./run_benchmark.py \
    --neuron_count 100 \
    --region_count 2 \
    --simulation_timestep 0.001 \
    --duration 5.0 \
    --output $RESULTS_DIR/basic_results.csv

# Configuration 2: Memory performance
./run_benchmark.py \
    --neuron_count 1000 \
    --region_count 4 \
    --simulation_timestep 0.01 \
    --duration 10.0 \
    --output $RESULTS_DIR/memory_results.csv

# Configuration 3: Speed performance
./run_benchmark.py \
    --neuron_count 500 \
    --region_count 1 \
    --simulation_timestep 0.0005 \
    --duration 2.0 \
    --output $RESULTS_DIR/speed_results.csv

# Configuration 4: Large-scale performance
./run_benchmark.py \
    --neuron_count 10000 \
    --region_count 8 \
    --simulation_timestep 0.002 \
    --duration 30.0 \
    --output $RESULTS_DIR/large_results.csv

cd ..

# Analyze and compare results
python3 analyze_all_tests.py $RESULTS_DIR

# Generate performance report
./generate_performance_report.py $RESULTS_DIR
```

### Performance Analysis
```python
# analyze_performance.py - Performance analysis script

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

def analyze_performance_results(results_file):
    """Analyze performance test results"""
    df = pd.read_csv(results_file)
    
    # Basic statistics
    print("Performance Test Results:")
    print(f"  Total tests: {len(df)}")
    print(f"  Average neurons per second: {df['neurons_per_second'].mean():.2f}")
    print(f"  Average memory usage: {df['memory_usage'].mean():.2f} MB")
    print(f"  Average simulation time: {df['simulation_time'].mean():.2f} seconds")
    
    # Performance by configuration
    print("\nPerformance by Configuration:")
    for config, group in df.groupby('configuration'):
        print(f"  {config}:")
        print(f"    Neurons/sec: {group['neurons_per_second'].mean():.2f}")
        print(f"    Memory usage: {group['memory_usage'].mean():.2f} MB")
        print(f"    Simulation time: {group['simulation_time'].mean():.2f} seconds")
    
    return df

def plot_performance_comparison(results_dir):
    """Generate performance comparison plots"""
    import glob
    
    # Load all results
    result_files = glob.glob(f"{results_dir}/*.csv")
    dataframes = [pd.read_csv(f) for f in result_files]
    
    # Create performance comparison plot
    plt.figure(figsize=(12, 8))
    
    # Neurons per second comparison
    plt.subplot(2, 2, 1)
    for df in dataframes:
        plt.plot(df['iteration'], df['neurons_per_second'], 
                label=df['configuration'].iloc[0])
    plt.xlabel('Iteration')
    plt.ylabel('Neurons per Second')
    plt.title('Performance Comparison')
    plt.legend()
    plt.grid(True)
    
    # Memory usage comparison
    plt.subplot(2, 2, 2)
    for df in dataframes:
        plt.plot(df['iteration'], df['memory_usage'],
                label=df['configuration'].iloc[0])
    plt.xlabel('Iteration')
    plt.ylabel('Memory Usage (MB)')
    plt.title('Memory Usage Comparison')
    plt.legend()
    plt.grid(True)
    
    # Simulation time comparison
    plt.subplot(2, 2, 3)
    for df in dataframes:
        plt.plot(df['iteration'], df['simulation_time'],
                label=df['configuration'].iloc[0])
    plt.xlabel('Iteration')
    plt.ylabel('Simulation Time (seconds)')
    plt.title('Simulation Time Comparison')
    plt.legend()
    plt.grid(True)
    
    # Efficiency comparison
    plt.subplot(2, 2, 4)
    for df in dataframes:
        efficiency = df['neurons_per_second'] / df['memory_usage']
        plt.plot(df['iteration'], efficiency,
                label=df['configuration'].iloc[0])
    plt.xlabel('Iteration')
    plt.ylabel('Efficiency (neurons/sec/MB)')
    plt.title('Performance Efficiency')
    plt.legend()
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig('performance_comparison.png')
    plt.close()
```

## Optimization Recommendations

### General Optimization Strategies

1. **Memory Optimization**
   - Use memory pooling for frequently allocated objects
   - Implement efficient data structures
   - Use smart pointers appropriately
   - Profile memory usage regularly

2. **CPU Optimization**
   - Use SIMD instructions for parallel processing
   - Optimize memory access patterns
   - Minimize function call overhead
   - Use appropriate data types

3. **I/O Optimization**
   - Buffer large I/O operations
   - Use asynchronous I/O where possible
   - Compress output data
   - Implement efficient file formats

4. **Simulation Optimization**
   - Use event-driven simulation for sparse activity
   - Optimize timestep selection
   - Implement efficient spike propagation
   - Use parallel processing for independent computations

### Code Optimization Techniques

1. **Algorithmic Optimization**
   - Use more efficient algorithms
   - Reduce computational complexity
   - Optimize data structures
   - Implement caching

2. **Compiler Optimization**
   - Use compiler optimizations
   - Profile with compiler-specific tools
   - Optimize compilation flags
   - Use link-time optimization

3. **Hardware Optimization**
   - Use hardware-specific optimizations
   - Leverage SIMD instructions
   - Optimize for cache locality
   - Use vector instructions

## Conclusion

Performance optimization is crucial for NLM efficiency and scalability. This guide provides comprehensive strategies for:

1. **Build optimization** with parallel compilation and compiler flags
2. **Runtime optimization** with configuration-based tuning
3. **Memory optimization** with memory pooling and efficient data structures
4. **Simulation optimization** with event-driven and parallel processing
5. **Profiling and analysis** with detailed performance metrics

By following these optimization strategies, you can achieve significant performance improvements in NLM simulations, enabling larger-scale and more complex research studies.

Key performance optimization areas:
- **Build performance** with parallel compilation
- **Memory efficiency** with pooling and smart allocation
- **CPU utilization** with SIMD and parallel processing
- **I/O efficiency** with buffering and compression
- **Algorithm optimization** with better data structures
- **Hardware exploitation** with platform-specific optimizations

Continuous performance optimization is essential for maintaining and improving NLM performance as the system scales and new features are added.