# NLM Performance Tuning Guide

## Overview

This document provides comprehensive guidance for optimizing NLM (熙然) simulation performance. It covers architectural considerations, configuration options, and specific techniques for achieving optimal performance across different hardware platforms and use cases.

## Performance Overview

### Performance Characteristics

The NLM performance profile is determined by several factors:

1. **Neural Network Size**: O(N²) complexity for synaptic connections
2. **Simulation Granularity**: Time step size affects update frequency
3. **Memory System Load**: Memory operations add overhead
4. **Plasticity Complexity**: STDP and other plasticity rules scale with activity
5. **Event-Driven Efficiency**: Spike propagation efficiency

### Performance Metrics

- **Steps per Second (SPS)**: Primary simulation throughput metric
- **Memory Usage**: RAM consumption during simulation
- **Initialization Time**: Time to set up simulation state
- **Plasticity Overhead**: Time spent on synaptic updates
- **Spike Processing**: Efficiency of event-driven computation

## Hardware-Specific Considerations

### CPU Performance

#### Multi-core Optimization
```cmake
# Enable parallel processing
cmake .. -DNLM_PARALLEL=ON
make -j$(nproc)
```

##### Parallel Configuration Options
```cpp
// Configure thread usage
struct ParallelConfig {
    bool enableSpikeProcessing = true;      // Parallel spike propagation
    bool enablePlasticityUpdates = true;    // Parallel plasticity computation
    bool enableMemoryAccess = true;         // Parallel memory operations
    int maxThreads = std::thread::hardware_concurrency();
    bool useThreadPool = true;
};
```

#### SIMD Optimization
```cmake
# Enable SIMD vectorization
cmake .. -DNLM_USE_SIMD=ON -DNLM_SIMD_LEVEL=AVX2
make
```

##### SIMD-accelerated Operations
```cpp
// SIMD-optimized neuron updates
void Neuron::updateSIMD(const float* inputs, const float* weights) {
    __m256 inputVec = _mm256_loadu_ps(inputs);
    __m256 weightVec = _mm256_loadu_ps(weights);
    __m256 result = _mm256_mul_ps(inputVec, weightVec);
    __m256 sum = _mm256_hadd_ps(result, result);
    float total = _mm256_extractf128_ps(sum, 0);
    // ... more SIMD operations
}
```

### Memory Optimization

#### Memory Pool Configuration
```cpp
// Configure memory pools for efficiency
struct MemoryPoolConfig {
    size_t neuronPoolSize = 10000;           // Pre-allocate neurons
    size_t synapsePoolSize = 50000;          // Pre-allocate synapses
    size_t spikeBufferSize = 1000000;       // Large spike buffer
    bool enableArenaAllocation = true;       // Arena for simulation lifetime
    bool enableSlabAllocation = false;       // Slab allocation for batches
};
```

#### Cache Optimization
```cpp
// Structure of arrays (SoA) for cache efficiency
struct NeuronState {
    float membranePotential;     // Cache-aligned (64 bytes)
    float synapticConductance;  // Coalesced memory access
    float firingRate;
    int neuronId;
    bool active;
};

// Use SoA for large arrays
std::vector<NeuronState> neuronStates;  // Better cache locality
```

### GPU Acceleration

#### CUDA Integration
```cpp
// GPU-accelerated spike propagation
__global__ void kernelPropagateSpikes(
    const NeuronState* neurons,
    const SynapseState* synapses,
    NeuronState* postSynaptic,
    size_t neuronCount,
    size_t synapseCount
) {
    int neuronId = blockIdx.x * blockDim.x + threadIdx.x;
    if (neuronId >= neuronCount) return;
    
    // Process spikes in parallel
    if (neurons[neuronId].active) {
        // Find and process outgoing synapses
        for (size_t synId = neuronId * maxOutDegree; 
             synId < neuronId * maxOutDegree + neurons[neuronId].outDegree; ++synId) {
            const SynapseState& synapse = synapses[synId];
            int postId = synapse.postSynapticId;
            
            // Inject current into postsynaptic neuron
            atomicAdd(&postSynaptic[postId].synapticConductance, synapse.weight);
        }
    }
}
```

#### Offloading Strategy
```cpp
cpp
// Configure offloading for GPU-accelerated systems
cmake .. -DNLM_GPU_ACCELERATION=ON
make
```

## Simulation Configuration Optimization

### Time Step Configuration
```cpp
// Optimal time step settings
struct TimeStepConfig {
    double simulationTimeStep = 0.001;      // 1ms steps (1000 Hz)
    double plasticityTimeStep = 0.1;       // 100ms for plasticity
    double developmentTimeStep = 10.0;     // 10s for development
    double neuromodulationTimeStep = 1.0;   // 1s for neuromodulation
};

// Adaptive time stepping
void AdaptiveTimeStepper::update() {
    double currentActivity = computeAverageActivity();
    
    if (currentActivity > highActivityThreshold) {
        // Reduce time step for stability
        timeStep_ = std::min(timeStep_ * 0.9, minTimeStep_);
    } else if (currentActivity < lowActivityThreshold) {
        // Increase time step for performance
        timeStep_ = std::max(timeStep_ * 1.1, optimalTimeStep_);
    }
}
```

### Memory System Configuration
```cpp
// Memory system optimization
struct MemoryConfig {
    // Working memory
    bool workingMemoryEnabled = true;
    size_t workingMemoryCapacity = 1000;
    double workingMemoryDecayRate = 0.01;
    
    // Episodic memory
    bool episodicMemoryEnabled = true;
    size_t maxEpisodicMemory = 10000;
    double replayWindow = 1000.0;
    bool enableConsolidation = true;
    
    // Associative memory
    bool associativeMemoryEnabled = true;
    double similarityThreshold = 0.8;
    double associationLearningRate = 0.001;
};
```

### Plasticity Optimization
```cpp
// Plasticity system optimization
struct PlasticityConfig {
    bool enableSTDP = true;
    double stdpLearningRate = 0.01;
    double stdpTimeWindow = 20.0;  // ms
    
    bool enableHebbian = true;
    double hebbianLearningRate = 0.001;
    
    bool enableStructural = false;  // Expensive, use selectively
    double structuralFormationRate = 0.001;
    double structuralEliminationRate = 0.0001;
};
```

## Performance Tuning Techniques

### 1. Spike Compression
```cpp
// Compress spike events for large networks
class SpikeCompressor {
public:
    void compressSpikes(const std::vector<NeuronId>& spikes, 
                       std::vector<CompressedSpike>& compressed) {
        if (spikes.empty()) return;
        
        std::sort(spikes.begin(), spikes.end());
        compressed.clear();
        
        CompressedSpike current;
        current.neuronId = spikes[0];
        current.count = 1;
        
        for (size_t i = 1; i < spikes.size(); ++i) {
            if (spikes[i] == spikes[i-1]) {
                current.count++;
            } else {
                compressed.push_back(current);
                current.neuronId = spikes[i];
                current.count = 1;
            }
        }
        compressed.push_back(current);
    }
};
```

### 2. Activity-Based Updates
```cpp
// Only update neurons that are active
class ActivityAwareUpdater {
public:
    void updateNeurons(const std::vector<NeuronState>& neurons,
                      const std::vector<SpikeEvent>& recentSpikes) {
        // Identify active neurons
        std::vector<bool> isActive(neurons.size(), false);
        for (const auto& spike : recentSpikes) {
            isActive[spike.neuronId] = true;
        }
        
        // Update only active neurons
        for (size_t i = 0; i < neurons.size(); ++i) {
            if (isActive[i]) {
                updateNeuron(neurons[i]);
            }
        }
    }
}; 
```

### 3. Hierarchical Processing
```cpp
// Process neurons in groups for better cache locality
class HierarchicalProcessor {
public:
    void processNeurons(const std::vector<NeuronState>& neurons) {
        // Divide neurons into groups
        size_t groupSize = neurons.size() / numGroups_;
        
        std::vector<std::thread> threads;
        for (int group = 0; group < numGroups_; ++group) {
            threads.emplace_back([this, &neurons, group, groupSize]() {
                size_t start = group * groupSize;
                size_t end = (group + 1) * groupSize;
                if (group == numGroups_ - 1) end = neurons.size();
                
                processNeuronRange(neurons, start, end);
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
    }
};
```

### 4. Predictive Loading
```cpp
// Pre-load memory based on access patterns
class PredictiveLoader {
public:
    void predictAndLoad(const std::vector<NeuronId>& predictedAccess,
                       const std::vector<NeuronState>& neurons) {
        for (NeuronId neuronId : predictedAccess) {
            loadNeuronIntoCache(neurons[neuronId]);
        }
    }
    
private:
    void loadNeuronIntoCache(const NeuronState& neuron) {
        // Ensure neuron is in cache
        volatile const char* ptr = reinterpret_cast<const char*>(&neuron);
        volatile char dummy = *ptr; // Prevent optimization
    }
};
```

## Profiling and Measurement

### Performance Profiling
```cpp
// Comprehensive performance profiler
class PerformanceProfiler {
public:
    struct ProfilingData {
        double totalTime;
        double initializationTime;
        double stepTime;
        double plasticityTime;
        double memoryTime;
        double spikeTime;
        uint64_t totalSpikes;
        uint64_t spikeEvents;
        size_t memoryUsage;
        double averageFiringRate;
        std::vector<double> stepTimes;
    };
    
    void startProfiling() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void recordStepTiming() {
        auto end = std::chrono::high_resolution_clock::now();
        double stepTime = std::chrono::duration<double>(end - startTime_).count();
        stepTimes_.push_back(stepTime);
        
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    ProfilingData generateReport(size_t neuronCount, size_t synapseCount) {
        ProfilingData data;
        // Calculate all metrics
        // ...
        return data;
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<double> stepTimes_;
    // ... other metrics
};
```

### Benchmark Suite
```bash
// Run performance benchmarks
cd build
./benchmark_suite --neurons 100 --neurons 500 --neurons 1000 --neurons 2000

// Profile with different optimization levels
./benchmark_suite --optimization-level 0
./benchmark_suite --optimization-level 1
./benchmark_suite --optimization-level 2
```

## Real-world Performance Examples

### Embedded Systems
```cpp
// Performance targets for embedded systems
struct EmbeddedConfig {
    size_t maxNeurons = 50;           // Small networks
    double timestep = 0.01;           // 100Hz control loop
    bool enableGPU = false;           // No GPU acceleration
    bool enableSIMD = true;           // Use SIMD when available
    size_t memoryLimitMB = 64;        // Limited memory
    
    // Performance targets
    double maxStepTimeMs = 10.0;      // Must complete in 10ms
    double targetSPS = 100.0;         // 100 steps per second
};
```

### High-Performance Computing
```cpp
// Performance targets for HPC systems
struct HPCConfig {
    size_t maxNeurons = 100000;       // Large networks
    double timestep = 0.0001;         // 10kHz simulation
    bool enableGPU = true;            // Use GPU acceleration
    bool enableSIMD = true;           // Use advanced SIMD
    size_t memoryLimitMB = 16384;     // Large memory
    
    // Performance targets
    double maxStepTimeMs = 1.0;       // Must complete in 1ms
    double targetSPS = 10000.0;       // 10k steps per second
};
```

## Case Studies

### Case Study 1: Small Brain (100 neurons)
```
cpp
// Configuration
brain.maxNeurons = 100
brain.memory.workingMemory.enabled = true
brain.memory.workingMemory.capacity = 20
brain.simulation.maxSteps = 10000

// Performance characteristics
Expected step time: ~0.1ms
Memory usage: ~1MB
Target SPS: >10,000
```

### Case Study 2: Medium Brain (1000 neurons)
```
cpp
// Configuration
brain.maxNeurons = 1000
brain.memory.workingMemory.enabled = true
brain.memory.workingMemory.capacity = 200
brain.memory.episodicMemory.enabled = true
brain.simulation.maxSteps = 5000

// Performance characteristics
Expected step time: ~2ms
Memory usage: ~20MB
Target SPS: ~500
```

### Case Study 3: Large Brain (10000 neurons)
```
cpp
// Configuration
brain.maxNeurons = 10000
brain.memory.workingMemory.enabled = false  // Disable to save memory
brain.memory.episodicMemory.enabled = true
brain.neuromodulation.dopamine.enabled = true
brain.neuromodulation.curiosity.enabled = false
brain.simulation.maxSteps = 1000

// Performance characteristics
Expected step time: ~50ms
Memory usage: ~200MB
Target SPS: ~20
```

## Optimization Checklist

### Pre-Simulation Optimization
- [ ] Enable SIMD instructions
- [ ] Configure memory pools
- [ ] Set optimal time step
- [ ] Enable parallel processing
- [ ] Pre-allocate arrays
- [ ] Use efficient data structures

### During Simulation Optimization
- [ ] Monitor and adjust time step
- [ ] Implement activity-based updates
- [ ] Use spike compression
- [ ] Apply predictive loading
- [ ] Monitor memory usage
- [ ] Tune plasticity parameters

### Post-Simulation Analysis
- [ ] Profile performance metrics
- [ ] Identify bottlenecks
- [ ] Optimize critical paths
- [ ] Update configuration
- [ ] Document findings

## Conclusion

Performance tuning is essential for achieving optimal NLM simulation performance. By understanding the performance characteristics and applying appropriate optimization techniques, you can achieve significant improvements in simulation speed and efficiency.

The key to good performance is:

1. **Understanding Your Use Case**: Different applications have different performance requirements
2. **Incremental Optimization**: Start with basic optimizations and add more as needed
3. **Continuous Monitoring**: Always profile and monitor performance
4. **Balancing Trade-offs**: Optimize for your specific hardware and use case

## Further Reading

### Performance Optimization Resources
- [High Performance C++](https://www.amazon.com/High-Performance-C-Programmers/dp/0321621255)
- [Optimizing Memory Access Patterns](https://www.cs.utexas.edu/users/moore/best-ideas/chapters/memory.pdf)
- [SIMD Programming Guide](https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top-level/compatibility-and-compliance/simd-optimizations-for-intel-architecture.html)

### Related Documentation
- **BEST_PRACTICES.md**: General development best practices
- **TUTORIALS.md**: Step-by-step performance tutorials
- **ARCHITECTURE.md**: System architecture details
- **API_REFERENCE.md**: Complete API documentation

### Performance Tools
- [Intel VTune](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-vtune-perf-profiler.html)
- [Google Performance Tools](https://github.com/google/perftools)
- [AMD CodeXL](https://github.com/GPUOpen-Tools/CodeXL)
- [CUDA Profiling Tools](https://developer.nvidia.com/cuda-profiling)