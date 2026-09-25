# NLM Performance Optimization Documentation

## Overview

The NLM Performance Optimization Documentation complements the existing PERFORMANCE.md file by providing comprehensive guidance on performance optimization features, practical examples, and best practices for the enhanced NLM performance infrastructure.

## Performance Optimization Overview

### Key Performance Features

#### 1. Event-Driven Processing
- Only active neurons consume computation
- Immediate spike propagation through event queues
- Time-bucketed delayed spike delivery

#### 2. Memory Efficiency
- Pre-allocated memory pools (SoA layout)
- Cache-aligned data structures
- Sparse connectivity representation

#### 3. Parallel Processing
- Thread pool with work-stealing
- Parallel neural processor
- Static chunk partitioning

#### 4. SIMD Vectorization
- AVX-512: 16 neurons per iteration
- AVX2: 8 neurons per iteration
- SSE: 4 neurons per iteration
- Scalar fallback

#### 5. Sparse Connectivity
- Adjacency list representation
- O(k) lookup complexity (k = synapses per neuron)
- Scalable to millions of neurons

## Performance Optimization Strategies

### 1. Configuration-Based Optimization
```bash
# Enable all optimizations
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..

# Specific optimizations
-DNLM_VECTORIZATION_LEVEL=AVX512
-DNLM_PARALLEL_THREADS=8
-DNLM_SPARSE_CONNECTIVITY=ON
```

### 2. Runtime Optimization
```cpp
#include "core/Config/Config.hpp"

// Enable performance optimizations
config.set("performance.vectorization", "AVX512");
config.set("performance.parallel_threads", 8);
config.set("performance.sparse_connectivity", true);
config.set("performance.event_driven", true);
```

### 3. Memory Pool Configuration
```cpp
// Configure memory pools
config.set("memory.neuron_pool_size", 1000000);
config.set("memory.synapse_pool_size", 10000000);
config.set("memory.event_queue_capacity", 65536);
```

### 4. Thread Pool Configuration
```cpp
// Configure thread pool
config.set("parallel.thread_count", std::thread::hardware_concurrency());
config.set("parallel.grain_size", 32);
config.set("parallel.work_stealing", true);
```

## Performance Benchmark Examples

### Example 1: Micro Scale Benchmark
```bash
# Build with performance flags
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..
make -j$(nproc)

# Run micro benchmark
./build/nlm --benchmark micro --neurons=1000 --steps=1000

# Expected performance: 10,000+ steps/sec
```

### Example 2: Small Scale Benchmark
```bash
# Run small benchmark
./build/nlm --benchmark small --neurons=10000 --steps=10000

# Expected performance: 5,000+ steps/sec
```

### Example 3: Medium Scale Benchmark
```bash
# Run medium benchmark
./build/nlm --benchmark medium --neurons=100000 --steps=100000

# Expected performance: 1,000+ steps/sec
```

### Example 4: Large Scale Benchmark
```bash
# Run large benchmark
./build/nlm --benchmark large --neurons=1000000 --steps=100000

# Expected performance: 100+ steps/sec
```

### Example 5: Scaling Benchmark
```bash
# Run scaling benchmark
./build/nlm --scaling-benchmark

# Compare performance across scales
./build/nlm --compare --baseline results.json
```

## Performance Optimization Examples

### Example 1: Optimizing for Speed
```cpp
#include "benchmark.cpp"
#include "core/Config/Config.hpp"

int main() {
    std::vector<BenchmarkResult> results;
    
    // Micro benchmark
    auto microResult = runBenchmark(1000, 1000);
    results.push_back(microResult);
    
    // Small benchmark
    auto smallResult = runBenchmark(10000, 10000);
    results.push_back(smallResult);
    
    // Medium benchmark
    auto mediumResult = runBenchmark(100000, 100000);
    results.push_back(mediumResult);
    
    // Large benchmark
    auto largeResult = runBenchmark(1000000, 100000);
    results.push_back(largeResult);
    
    printBenchmarkResults(results);
    return 0;
}
```

### Example 2: Optimization Script
```bash
#!/bin/bash
# NLM Performance Optimization Script

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Function to print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Build with optimizations
build_optimized() {
    print_info "Building NLM with performance optimizations..."
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON \
             -DNLM_VECTORIZATION_LEVEL=AVX512 \
             -DNLM_PARALLEL_THREADS=$(nproc) \
             -DNLM_SPARSE_CONNECTIVITY=ON
    make -j$(nproc)
    cd ..
}

# Run benchmarks
run_benchmarks() {
    print_info "Running performance benchmarks..."
    
    # Micro benchmark
    print_info "Running micro benchmark..."
    ./build/nlm --benchmark micro --neurons=1000 --steps=1000
    
    # Small benchmark
    print_info "Running small benchmark..."
    ./build/nlm --benchmark small --neurons=10000 --steps=10000
    
    # Medium benchmark
    print_info "Running medium benchmark..."
    ./build/nlm --benchmark medium --neurons=100000 --steps=100000
    
    # Large benchmark
    print_info "Running large benchmark..."
    ./build/nlm --benchmark large --neurons=1000000 --steps=100000
}

# Compare with baseline
compare_with_baseline() {
    if [ -f "baseline_results.json" ]; then
        print_info "Comparing with baseline..."
        ./build/nlm --compare --baseline baseline_results.json
    else
        print_warning "Baseline results not found. Skipping comparison."
    fi
}

# Export results
export_results() {
    print_info "Exporting benchmark results..."
    ./build/nlm --export benchmark_results.json
}

# Main execution
case "$1" in
    build)
        build_optimized
        ;;
    benchmarks)
        run_benchmarks
        ;;
    compare)
        compare_with_baseline
        ;;
    export)
        export_results
        ;;
    all)
        build_optimized
        run_benchmarks
        compare_with_baseline
        export_results
        ;;
    *)
        echo "Usage: $0 {build|benchmarks|compare|export|all}"
        exit 1
        ;;
esac
```

### Example 3: Performance Monitoring Script
```bash
#!/bin/bash
# NLM Performance Monitoring Script

set -e

# Install required tools
if ! command -v perf &> /dev/null; then
    echo "perf not found. Installing performance tools..."
    sudo apt-get update
    sudo apt-get install -y perf
fi

# Function to monitor performance
monitor_performance() {
    local process_name=$1
    local duration=$2
    local output_file=$3
    
    echo "Monitoring performance for $process_name for $duration seconds..."
    
    # Start perf monitoring
    sudo perf record -g ./build/nlm --benchmark --neurons=10000 --steps=10000 &
    local perf_pid=$!
    
    # Run the process
    timeout $duration ./build/nlm --benchmark --neurons=10000 --steps=10000
    local exit_code=$?
    
    # Wait for perf to finish
    wait $perf_pid
    
    # Analyze performance data
    sudo perf report > $output_file
    
    return $exit_code
}

# Function to monitor memory usage
monitor_memory() {
    local process_name=$1
    local duration=$2
    local output_file=$3
    
    echo "Monitoring memory usage for $process_name for $duration seconds..."
    
    # Start memory monitoring
    valgrind --tool=massif --stacks=yes ./build/nlm --benchmark --neurons=10000 --steps=10000 &
    local valgrind_pid=$!
    
    # Run the process
    timeout $duration ./build/nlm --benchmark --neurons=10000 --steps=10000
    local exit_code=$?
    
    # Wait for valgrind to finish
    wait $valgrind_pid
    
    # Analyze memory data
    ms_print massif.out.12345 > $output_file
    
    return $exit_code
}

# Main execution
case "$1" in
    perf)
        monitor_performance "$2" "$3" "perf_output.txt"
        ;;
    memory)
        monitor_memory "$2" "$3" "memory_output.txt"
        ;;
    *)
        echo "Usage: $0 {perf|memory} <process_name> <duration>"
        exit 1
        ;;
esac
```

## Performance Optimization Best Practices

### DO

1. **Profile First**: Use perf/valgrind before optimizing
2. **Measure Impact**: Compare before/after benchmarks
3. **Maintain Correctness**: Verify numerical results
4. **Document Tradeoffs**: Note speed vs. precision costs
5. **Keep It Modular**: Easy to disable optimizations
6. **Test at Scale**: Test optimizations at target scale
7. **Use Appropriate Optimizations**: Choose optimizations based on hardware
8. **Monitor Resources**: Monitor CPU, memory, and I/O usage

### DON'T

1. **Optimize Blindly**: Measure first, optimize second
2. **Sacrifice Correctness**: Numerical stability matters
3. **Over-Engineer**: Simple optimizations first
4. **Assume Scaling**: Test at target scale
5. **Ignore Limits**: Some code is inherently serial
6. **Optimize Prematurely**: Optimize after identifying bottlenecks
7. **Optimize Everything**: Focus on critical path only
8. **Ignore Dependencies**: Consider hardware and software dependencies

## Hardware-Specific Optimizations

### CPU Optimization
```cpp
// CPU feature detection
#ifdef __AVX512F__
    // AVX-512 optimized code
#elif __AVX2__
    // AVX2 optimized code
#elif __SSE4_1__
    // SSE4.1 optimized code
#else
    // Scalar fallback
#endif
```

### Memory Optimization
```cpp
// Cache-friendly data layout
struct alignas(64) NeuronState {
    float v_membrane;      // Membrane potential
    uint32_t refractory;   // Refractory counter
    uint8_t firing_state;  // Firing state
    // Pad to 64 bytes for cache line alignment
    char padding[64 - 3*sizeof(float)];
};
```

### NUMA Optimization
```cpp
// NUMA-aware memory allocation
void* allocateNUMAMemory(size_t size, int node = -1) {
    if (node == -1) {
        node = getNUMANode();
    }
    return numalib::allocate(size, node);
}
```

## Performance Measurement Commands

### Standard Measurement Commands
```bash
# Build with performance flags
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..
make -j$(nproc)

# Run benchmark
./build/nlm --benchmark neuron_performance --neurons=10000 --steps=10000

# Run scaling experiment
./build/nlm --scaling-benchmark

# Run experiment with specific configuration
./build/nlm --experiment phase6 --neurons=5000 --steps=2000

# Save benchmark results
./build/nlm --benchmark neuron_performance --export results.json

# Compare with baseline
./build/nlm --benchmark neuron_performance --compare --baseline baseline.json
```

### Performance Analysis Commands
```bash
# Analyze performance with perf
perf record -g ./build/nlm --benchmark --neurons=10000 --steps=10000
perf report

# Analyze memory usage with valgrind
valgrind --tool=massif --stacks=yes ./build/nlm --benchmark --neurons=10000 --steps=10000
ms_print massif.out.12345

# Analyze I/O performance with strace
strace -o strace_output.txt ./build/nlm --benchmark --neurons=10000 --steps=10000

# Analyze thread performance with oprofile
opcontrol --start-daemon
./build/nlm --benchmark --neurons=10000 --steps=10000
opreport -l
opcontrol --shutdown-daemon
```

## Performance Metrics

### Simulation Performance
```cpp
struct PerformanceMetrics {
    double neuronsPerSecond;    // Neurons per second
    double spikesPerStep;       // Average spikes per simulation step
    double realTimeFactor;      // Simulated_time / wall_clock_time
    double memoryUsage;         // Memory usage in MB
    double cpuUsage;            // CPU usage percentage
};

// Calculate performance metrics
PerformanceMetrics calculatePerformance(const BenchmarkResult& result) {
    PerformanceMetrics metrics;
    metrics.neuronsPerSecond = result.neurons / result.totalTime;
    metrics.spikesPerStep = result.spikesPerStep;
    metrics.realTimeFactor = result.totalTime * result.steps / result.steps;
    metrics.memoryUsage = calculateMemoryUsage(result.neurons);
    metrics.cpuUsage = calculateCPUUsage(result.totalTime);
    return metrics;
}
```

### Benchmark Results
```cpp
struct BenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond; // neurons per second
    double spikesPerStep;  // average spikes per simulation step
    double synapsesPerNeuron; // synapses per neuron
    PerformanceMetrics metrics; // detailed performance metrics
};
```

## Performance Scaling

### Scaling Behavior
```cpp
// Scaling analysis
struct ScalingAnalysis {
    double neurons1;        // Scale 1: 10k neurons
    double time1;           // Time for scale 1
    double neurons2;        // Scale 2: 100k neurons
    double time2;           // Time for scale 2
    double neurons3;        // Scale 3: 1M neurons
    double time3;           // Time for scale 3
    
    double efficiency1;     // Scaling efficiency for scale 1
    double efficiency2;     // Scaling efficiency for scale 2
    double efficiency3;     // Scaling efficiency for scale 3
};

// Calculate scaling analysis
ScalingAnalysis analyzeScaling(const BenchmarkResult& micro,
                               const BenchmarkResult& small,
                               const BenchmarkResult& large) {
    ScalingAnalysis analysis;
    
    analysis.neurons1 = micro.neurons;
    analysis.time1 = micro.totalTime;
    
    analysis.neurons2 = small.neurons;
    analysis.time2 = small.totalTime;
    
    analysis.neurons3 = large.neurons;
    analysis.time3 = large.totalTime;
    
    // Calculate efficiency
    analysis.efficiency1 = (micro.time1 * micro.neurons2 / small.time2) / micro.neurons2;
    analysis.efficiency2 = (small.time2 * small.neurons3 / large.time3) / small.neurons3;
    
    return analysis;
}
```

### Expected Scaling Results
```cpp
// Expected performance targets
struct PerformanceTargets {
    // Scale: Neurons, Synapses, Memory, Steps/sec
    struct Scale {
        size_t neurons;
        size_t synapses;
        size_t memoryMB;
        double stepsPerSecond;
    };
    
    static const Scale MICRO_SCALE;    // 10k neurons, ~100k synapses, ~10 MB, 5,000+ steps/sec
    static const Scale SMALL_SCALE;    // 100k neurons, ~1M synapses, ~100 MB, 1,000+ steps/sec
    static const Scale LARGE_SCALE;    // 1M neurons, ~10M synapses, ~1 GB, 100+ steps/sec
};
```

## Performance Optimization Tutorials

### Tutorial 1: Getting Started with Performance Optimization

#### Step 1: Build with Optimizations
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON
make -j$(nproc)
```

#### Step 2: Run Basic Benchmark
```bash
./build/nlm --benchmark micro --neurons=1000 --steps=1000
```

#### Step 3: Analyze Performance
```bash
./build/nlm --benchmark micro --export results.json
# View results with:
cat results.json
```

### Tutorial 2: Advanced Performance Optimization

#### Step 1: Configure Optimizations
```cpp
// Configure all optimizations
config.set("performance.enabled", true);
config.set("performance.vectorization", "AVX512");
config.set("performance.parallel_threads", 8);
config.set("performance.sparse_connectivity", true);
config.set("performance.event_driven", true);
config.set("memory.neuron_pool_size", 1000000);
config.set("memory.synapse_pool_size", 10000000);
```

#### Step 2: Run Advanced Benchmark
```bash
./build/nlm --benchmark large --neurons=1000000 --steps=100000 --export results.json
```

#### Step 3: Compare with Baseline
```bash
./build/nlm --benchmark large --compare --baseline baseline_results.json
```

### Tutorial 3: Hardware-Specific Optimization

#### Step 1: Detect Hardware Capabilities
```cpp
#include <cpuid.h>

bool hasAVX512() {
    unsigned int eax, ebx, ecx, edx;
    return __get_cpuid(7, &eax, &ebx, &ecx, &edx) && 
           (ebx & bit_AVX512F);
}

bool hasAVX2() {
    unsigned int eax, ebx, ecx, edx;
    return __get_cpuid(7, &eax, &ebx, &ecx, &edx) && 
           (ebx & bit_AVX2);
}
```

#### Step 2: Configure for Hardware
```cpp
// Select appropriate optimization level
if (hasAVX512()) {
    config.set("performance.vectorization", "AVX512");
} else if (hasAVX2()) {
    config.set("performance.vectorization", "AVX2");
} else {
    config.set("performance.vectorization", "SSE");
}
```

## Future Performance Enhancements

### Phase 6 Performance Improvements

1. **GPU Acceleration**: CUDA/HIP for large-scale neuron updates
2. **Distributed Simulation**: Multi-machine scaling for millions of neurons
3. **Approximate Computing**: Trading precision for speed
4. **Adaptive Load Balancing**: Dynamic work distribution across cores
5. **Memory Compression**: Advanced memory compression techniques

### Experimental Optimizations

1. **SIMD Optimization**: Advanced SIMD vectorization techniques
2. **Cache Optimization**: Advanced cache-aware algorithms
3. **Branch Prediction**: Optimized branch prediction
4. **Prefetching**: Intelligent data prefetching
5. **Speculative Execution**: Speculative optimization techniques

## Performance Monitoring Tools

### Built-in Performance Monitoring
```cpp
// Enable performance monitoring
config.set("performance.monitoring", true);
config.set("performance.profiling", true);
config.set("performance.tracing", true);

// Get performance profile
auto profile = nlm::getPerformanceProfile();
```

### External Performance Tools
```bash
# Use Linux performance tools
perf record -g ./build/nlm
perf report

# Use Valgrind for memory profiling
valgrind --tool=massif ./build/nlm
ms_print massif.out.12345

# Use OProfile for CPU profiling
opcontrol --start-daemon
./build/nlm
opreport -l
opcontrol --shutdown-daemon

# Use Intel VTune for advanced profiling
amplxe-cl -collect hotspots --result-dir results ./build/nlm
```

## Performance Optimization FAQ

### Common Questions

#### Q: How do I enable performance optimizations?
A: Use `-DNLM_PERFORMANCE_ENABLED=ON` when configuring CMake, or set `performance.enabled` to true in the configuration.

#### Q: What's the difference between performance modes?
A: There are different optimization levels:
- **Default**: Balanced performance
- **Speed**: Maximum performance
- **Memory**: Memory efficiency
- **Adaptive**: Dynamic optimization based on workload

#### Q: How do I measure performance?
A: Use benchmarks with `--benchmark` flag, or use performance monitoring tools like perf, valgrind, or Intel VTune.

#### Q: How do I compare with baseline?
A: Use the `--compare` flag with `--baseline` option to compare with baseline results.

#### Q: How do I export results?
A: Use the `--export` flag to export results to JSON, XML, or other formats.

### Troubleshooting

#### Performance Issues
1. **Problem**: Slow performance
   - Solution: Enable optimizations, increase thread count, check hardware capabilities

2. **Problem**: High memory usage
   - Solution: Configure memory pools, enable sparse connectivity, reduce network size

3. **Problem**: CPU bottlenecks
   - Solution: Optimize code, use vectorization, check cache efficiency

#### Hardware Issues
1. **Problem**: AVX-512 not available
   - Solution: Use fallback optimization levels (AVX2, SSE)

2. **Problem**: NUMA issues
   - Solution: Use NUMA-aware memory allocation

3. **Problem**: Memory bandwidth limits
   - Solution: Optimize memory access patterns, use cache-friendly data structures

## Conclusion

Performance optimization is essential for achieving NLM's scaling goals. By following the strategies and examples in this documentation, you can:

1. **Achieve Target Performance**: Meet or exceed performance targets for different scales
2. **Optimize for Hardware**: Configure optimizations for specific hardware
3. **Measure and Analyze**: Use comprehensive performance measurement and analysis
4. **Scale Effectively**: Scale from thousands to millions of neurons
5. **Maintain Correctness**: Ensure optimizations don't compromise numerical stability

The performance optimization features in NLM enable efficient simulation of large-scale neural networks, making it possible to study complex neural systems that were previously infeasible.

