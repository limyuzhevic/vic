# NLM Performance Benchmark

Performance benchmarking tools for NLM brain simulations.

## Overview

The NLM Performance Benchmark suite provides comprehensive performance measurement for neural simulations, including timing, memory usage, and scalability testing.

## Benchmark Types

### 1. Basic Performance Benchmark
Measures core neural computation performance.

```cpp
class BasicPerformanceBenchmark {
public:
    struct Results {
        double total_time_ms;
        size_t steps_per_second;
        size_t total_neurons;
        size_t total_synapses;
        double avg_firing_rate;
        double memory_usage_kb;
    };
    
    Results runBenchmark(const nlm::Config& config, size_t steps = 1000);
    
    void compareConfigs(const std::vector<nlm::Config>& configs, size_t steps = 1000);
};
```

### 2. Memory Usage Benchmark
Measures memory efficiency and leak detection.

```cpp
class MemoryUsageBenchmark {
public:
    struct MemoryProfile {
        size_t peak_memory_kb;
        size_t allocated_objects;
        size_t deallocated_objects;
        double memory_efficiency;
    };
    
    MemoryProfile profileMemoryUsage(const nlm::Config& config, size_t steps = 500);
    
    void detectMemoryLeaks(const nlm::Config& config, size_t steps = 1000);
};
```

### 3. Scalability Benchmark
Tests performance with increasing network sizes.

```cpp
class ScalabilityBenchmark {
public:
    struct ScalabilityResults {
        std::vector<size_t> neuron_counts;
        std::vector<double> step_times_ms;
        std::vector<double> memory_per_neuron_kb;
        std::vector<size_t> steps_per_second;
    };
    
    ScalabilityResults runScalabilityTest(
        const std::vector<size_t>& neuron_counts, 
        size_t steps_per_config = 500
    );
};
```

### 4. Learning Efficiency Benchmark
Measures learning speed and plasticity effectiveness.

```cpp
cpp
class LearningEfficiencyBenchmark {
public:
    struct LearningResults {
        double learning_time_ms;
        double weight_change_rate;
        double convergence_score;
        double stability_score;
    };
    
    LearningResults benchmarkLearning(
        const nlm::Config& config,
        size_t stimulus_duration = 1000,
        size_t consolidation_steps = 2000
    );
};
```

## Usage Examples

### Basic Performance Test
```cpp
#include "benchmark/BasicPerformanceBenchmark.hpp"
#include <iostream>

int main() {
    BasicPerformanceBenchmark benchmark;
    
    // Test with different configurations
    std::vector<nlm::Config> configs = {
        createSmallBrainConfig(),
        createMediumBrainConfig(), 
        createLargeBrainConfig()
    };
    
    benchmark.compareConfigs(configs, 1000);
    
    return 0;
}
```

### Memory Leak Detection
```cpp
#include "benchmark/MemoryUsageBenchmark.hpp"
#include <cassert>

int main() {
    MemoryUsageBenchmark memBenchmark;
    
    auto config = createStandardConfig();
    
    // Run memory profiling
    auto profile = memBenchmark.profileMemoryUsage(config, 1000);
    
    std::cout << "Memory Profile:" << std::endl;
    std::cout << "  Peak memory: " << profile.peak_memory_kb << " KB" << std::endl;
    std::cout << "  Allocated objects: " << profile.allocated_objects << std::endl;
    std::cout << "  Deallocated objects: " << profile.deallocated_objects << std::endl;
    std::cout << "  Efficiency: " << profile.memory_efficiency * 100 << "%" << std::endl;
    
    // Detect memory leaks
    memBenchmark.detectMemoryLeaks(config, 5000);
    
    return 0;
}
```

### Scalability Analysis
```cpp
#include "benchmark/ScalabilityBenchmark.hpp"
#include <iostream>

int main() {
    ScalabilityBenchmark scaleBenchmark;
    
    std::vector<size_t> neuron_counts = {100, 500, 1000, 2000, 5000};
    auto results = scaleBenchmark.runScalabilityTest(neuron_counts, 200);
    
    std::cout << "Scalability Results:" << std::endl;
    std::cout << "Neuron Count | Steps/sec | Time/ms | Memory/Neuron KB" << std::endl;
    std::cout << "------------|-----------|---------|-----------------" << std::endl;
    
    for (size_t i = 0; i < results.neuron_counts.size(); ++i) {
        std::cout << results.neuron_counts[i] << "          | "
                 << results.steps_per_second[i] << "    | "
                 << results.step_times_ms[i] << "    | "
                 << results.memory_per_neuron_kb[i] << std::endl;
    }
    
    return 0;
}
```

### Learning Efficiency Test
```cpp
#include "benchmark/LearningEfficiencyBenchmark.hpp"
#include <iostream>

int main() {
    LearningEfficiencyBenchmark learnBenchmark;
    
    auto config = createStandardConfig();
    config.set("plasticity_learning_rate", 0.01f);
    
    auto results = learnBenchmark.benchmarkLearning(config, 1000, 3000);
    
    std::cout << "Learning Efficiency Results:" << std::endl;
    std::cout << "  Learning time: " << results.learning_time_ms << " ms" << std::endl;
    std::cout << "  Weight change rate: " << results.weight_change_rate << std::endl;
    std::cout << "  Convergence score: " << results.convergence_score << std::endl;
    std::cout << "  Stability score: " << results.stability_score << std::endl;
    
    return 0;
}
```

## Configuration File

Create `benchmark.cfg` for benchmark configuration:

```ini
# Performance benchmark settings
benchmark {
    default_steps = 1000
    warmup_steps = 100
    memory_test_steps = 500
    leak_detection_steps = 2000
}

# Neuron count configurations
sizes {
    small = 100
    medium = 1000
    large = 5000
    massive = 20000
}

# Performance thresholds
thresholds {
    min_steps_per_second = 100
    max_memory_growth_kb = 1024
    max_learning_time_ms = 10000
}
```

## Automated Benchmarking

### Benchmark Script
```bash
#!/bin/bash
# run_benchmarks.sh - Run all NLM benchmarks

set -e

echo "Running NLM Performance Benchmark Suite..."

echo "1. Running Basic Performance Benchmark..."
./nlm_benchmark --basic --steps 1000

echo "2. Running Memory Usage Benchmark..."
./nlm_benchmark --memory --steps 500

echo "3. Running Scalability Benchmark..."
./nlm_benchmark --scalability --sizes 100,500,1000,2000

echo "4. Running Learning Efficiency Benchmark..."
./nlm_benchmark --learning --stimulus 1000 --consolidation 3000

echo "Benchmark suite complete!"
echo "Results saved to benchmark_results.json"
```

### Results Analysis
```cpp
#include "benchmark/BenchmarkResults.hpp"
#include <nlohmann/json.hpp>

class BenchmarkAnalyzer {
public:
    void analyzeResults(const std::string& filename);
    
    void generateReport(const BenchmarkResults& results);
    
    void compareBenchmarkRuns(const std::vector<BenchmarkResults>& runs);
    
    void identifyPerformanceBottlenecks(const BenchmarkResults& results);
};
```

## Integration with CMake

Add to your CMakeLists.txt:

```cmake
# Add benchmark target
add_executable(nlm_benchmark
    benchmark/main.cpp
    benchmark/BasicPerformanceBenchmark.cpp
    benchmark/MemoryUsageBenchmark.cpp
    benchmark/ScalabilityBenchmark.cpp
    benchmark/LearningEfficiencyBenchmark.cpp
)

target_link_libraries(nlm_benchmark PRIVATE nlm_core nlohmann_json)

# Add benchmark tests
enable_testing()
add_test(NAME benchmark_basic COMMAND nlm_benchmark --basic --steps 100)
add_test(NAME benchmark_memory COMMAND nlm_benchmark --memory --steps 50)
add_test(NAME benchmark_scalability COMMAND nlm_benchmark --scalability --sizes 100)
```

## Performance Metrics

The benchmark suite measures the following metrics:

### Performance Metrics
- **Steps per second**: Simulation speed
- **Time per step**: Average step execution time
- **Memory per neuron**: Memory efficiency
- **CPU utilization**: Processor usage

### Learning Metrics
- **Weight change rate**: Speed of synaptic modification
- **Convergence score**: How quickly learning stabilizes
- **Stability score**: Resistance to catastrophic forgetting
- **Learning efficiency**: Information learned per unit time

### Memory Metrics
- **Peak memory usage**: Maximum RAM consumption
- **Memory growth**: Memory increase over time
- **Allocation efficiency**: Objects allocated/deallocated ratio
- **Leak detection**: Memory that wasn't freed

## Custom Benchmarks

Create custom benchmarks by implementing the `Benchmark` interface:

```cpp
class CustomBenchmark : public Benchmark {
public:
    void setup(const nlm::Config& config) override {
        // Initialize benchmark-specific state
    }
    
    void run(nlm::Brain& brain) override {
        // Custom benchmarking logic
    }
    
    void teardown() override {
        // Cleanup benchmark state
    }
};
```

This benchmark suite provides comprehensive performance analysis for NLM, helping users optimize their neural simulations for speed, memory efficiency, and learning effectiveness.