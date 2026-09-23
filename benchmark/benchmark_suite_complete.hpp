# Performance Benchmark Suite for NLM

This benchmark suite provides comprehensive performance measurement for the NLM neural simulation framework.

## Overview

The NLM Performance Benchmark Suite measures and analyzes performance across:

1. **Memory Systems**: Working memory, episodic memory, associative memory
2. **Simulation Core**: Brain::step() performance, neural computation
3. **Component Integration**: Neuromodulation, development, cognition systems
4. **Memory Management**: Allocation efficiency, cache utilization
5. **Scaling Capabilities**: Performance at different neuron/synapse scales

## Components

### 1. Benchmark Configuration

```cpp
// nlm/benchmark/BenchmarkConfig.hpp

#pragma once

#include <string>
#include <unordered_map>

namespace nlm {

struct BenchmarkConfig {
    // Scale settings
    enum Scale { MICRO, SMALL, MEDIUM, LARGE };
    Scale scale;
    
    // Simulation parameters
    size_t neuronCount;
    size_t synapseCount;
    size_t simulationSteps;
    size_t warmupSteps;
    
    // Performance targets
    double targetStepsPerSecond;
    size_t memoryBudgetMB;
    
    // Measurement settings
    bool measureMemoryUsage;
    bool measureCacheEfficiency;
    bool measureContention;
    bool measureSIMDEfficiency;
    
    // Configuration from file
    static BenchmarkConfig loadFromFile(const std::string& filename);
    static BenchmarkConfig getDefault(Scale scale);
    
    // Performance targets per scale
    double getTargetStepsPerSecond() const;
    size_t getMaxMemoryUsage() const;
    
private:
    // Scale-specific parameters
    std::unordered_map<Scale, size_t> scaleNeurons_;
    std::unordered_map<Scale, size_t> scaleSynapses_;
    std::unordered_map<Scale, double> scalePerformance_;
};

} // namespace nlm
```

### 2. Performance Measurement

```cpp
// nlm/benchmark/PerformanceProfiler.hpp

#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <thread>

namespace nlm {

struct PerformanceMetrics {
    // Timing measurements
    double wallTime;           // Real-time execution
    double cpuTime;            // CPU-only time
    double stepTimeAverage;    // Average per-step time
    double stepTimeMin;        // Minimum per-step time
    double stepTimeMax;        // Maximum per-step time
    
    // Throughput measurements
    double stepsPerSecond;     // Steps / wall time
    double neuronsPerSecond;   // Neurons processed / wall time
    double spikesPerSecond;    // Spikes processed / wall time
    
    // Memory measurements
    size_t memoryBytes;        // Total memory usage
    size_t memoryPeakBytes;    // Peak memory usage
    size_t allocationCount;    // Number of memory allocations
    
    // Cache and concurrency
    float cacheMissRate;       // L1/L2 cache miss rate
    uint64_t lockContention;  // Lock/contention events
    uint64_t atomicOperations; // Atomic operation count
    
    // SIMD and vectorization
    bool simdEnabled;          // SIMD support enabled
    float simdUtilization;     // SIMD instructions per operation
    
    // Scalability metrics
    double efficiency;         // Parallel efficiency (0-1)
    size_t activeThreads;      // Number of active threads
    
    PerformanceMetrics() : wallTime(0), cpuTime(0), stepTimeAverage(0), 
                          stepTimeMin(0), stepTimeMax(0), stepsPerSecond(0),
                          neuronsPerSecond(0), spikesPerSecond(0), 
                          memoryBytes(0), memoryPeakBytes(0), allocationCount(0),
                          cacheMissRate(0), lockContention(0), atomicOperations(0),
                          simdUtilization(0), efficiency(0), activeThreads(0),
                          simdEnabled(false) {}
};

class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Start/stop profiling
    void start(const std::string& phaseName);
    void stop();
    void reset();
    
    // Record metrics
    void recordStep(size_t neuronCount, size_t spikeCount);
    void recordMemory(size_t bytes);
    void recordCacheMiss(bool l1Miss, bool l2Miss);
    void recordContention();
    void recordSIMD(bool used, float utilization);
    
    // Get results
    PerformanceMetrics getCurrentMetrics() const;
    PerformanceMetrics getFinalMetrics() const;
    
    // Export results
    void exportToJSON(const std::string& filename) const;
    void exportToCSV(const std::string& filename) const;
    
    // Statistical analysis
    double calculateStandardDeviation(const std::vector<double>& values) const;
    double calculateEfficiency(double parallelTime, double serialTime, size_t threads) const;
    
private:
    struct PhaseMetrics {
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        std::vector<double> stepTimes;
        size_t totalSpikes;
        size_t totalNeurons;
        size_t memoryMeasurements;
    };
    
    std::unordered_map<std::string, PhaseMetrics> phaseMetrics_;
    PerformanceMetrics currentMetrics_;
    bool profilingEnabled_;
    size_t stepCount_;
};

} // namespace nlm
```

### 3. Memory System Benchmarks

```cpp
// nlm/benchmark/MemorySystemBenchmark.hpp

#pragma once

#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "BenchmarkConfig.hpp"
#include "PerformanceProfiler.hpp"

namespace nlm {

class WorkingMemoryBenchmark {
public:
    WorkingMemoryBenchmark(BenchmarkConfig& config, PerformanceProfiler& profiler);
    ~WorkingMemoryBenchmark();
    
    // Benchmark storage and retrieval
    void benchmarkStorage(size_t patterns, size_t iterations);
    void benchmarkRetrieval(size_t patterns, size_t iterations);
    void benchmarkCompetition(size_t neurons, size_t iterations);
    
    // Memory efficiency benchmarks
    void benchmarkMemoryFootprint(size_t patterns);
    void benchmarkAllocationOverhead(size_t iterations);
    void benchmarkCacheLocality(size_t patterns);
    
    // Competition algorithm performance
    void benchmarkWinnerSelection(size_t competitors, size_t iterations);
    void benchmarkInhibitionEfficiency(size_t neurons, size_t iterations);
    
private:
    BenchmarkConfig& config_;
    PerformanceProfiler& profiler_;
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::vector<std::vector<float>> testPatterns_;
    
    void setupTestData(size_t count);
    float calculateMemoryEfficiency(const std::vector<float>& activations) const;
    double calculateCompetitionComplexity(size_t neurons) const;
};

class EpisodicMemoryBenchmark {
public:
    EpisodicMemoryBenchmark(BenchmarkConfig& config, PerformanceProfiler& profiler);
    ~EpisodicMemoryBenchmark();
    
    // Search operation benchmarks
    void benchmarkTemporalSearch(size_t episodes, size_t queries);
    void benchmarkSpatialSearch(size_t episodes, size_t queries);
    void benchmarkSimilaritySearch(size_t episodes, size_t queries);
    
    // Replay performance benchmarks
    void benchmarkReplaySequence(size_t episodeCount, size_t replayLength);
    void benchmarkRelevancyCalculation(size_t episodes, size_t iterations);
    
    // Memory usage benchmarks
    void benchmarkMemoryConsolidation(size_t episodes, size_t iterations);
    void benchmarkStorageCapacity(size_t maxEpisodes);
    
private:
    BenchmarkConfig& config_;
    PerformanceProfiler& profiler_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::vector<EpisodicMemoryItem> testEpisodes_;
    
    void setupTestEpisodes(size_t count);
    float calculateSearchEfficiency(const std::vector<const EpisodicMemoryItem*>& results) const;
    double calculateReplayOverhead(size_t replaySteps) const;
};

class AssociativeMemoryBenchmark {
public:
    AssociativeMemoryBenchmark(BenchmarkConfig& config, PerformanceProfiler& profiler);
    ~AssociativeMemoryBenchmark();
    
    // Association benchmarks
    void benchmarkAssociationCreation(size_t patterns, size_t iterations);
    void benchmarkAssociationRetrieval(size_t patterns, size_t iterations);
    void benchmarkAssociationUpdate(size_t patterns, size_t iterations);
    
    // Spread activation benchmarks
    void benchmarkActivationSpread(size_t patterns, size_t steps);
    void benchmarkPatternCompletion(size_t cues, size_t patterns);
    
    // Memory efficiency benchmarks
    void benchmarkMemoryCompression(size_t patterns);
    void benchmarkSparseStorage(size_t density);
    
private:
    BenchmarkConfig& config_;
    PerformanceProfiler& profiler_;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory_;
    std::vector<std::vector<float>> patternSet_;
    
    void setupPatternSet(size_t count);
    float calculateAssociationDensity() const;
    double calculateSpreadEfficiency(size_t steps) const;
};

} // namespace nlm
```

### 4. Simulation Loop Benchmark

```cpp
// nlm/benchmark/SimulationLoopBenchmark.hpp

#pragma once

#include "../brain/Brain.hpp"
#include "BenchmarkConfig.hpp"
#include "PerformanceProfiler.hpp"
#include <functional>

namespace nlm {

class BrainStepBenchmark {
public:
    BrainStepBenchmark(BenchmarkConfig& config, PerformanceProfiler& profiler);
    ~BrainStepBenchmark();
    
    // Core Brain::step() benchmarks
    void benchmarkStepPerformance(size_t steps, size_t iterations);
    void benchmarkStepScalability(size_t neuronCounts[], size_t count);
    void benchmarkStepMemoryAccess(size_t steps);
    
    // Memory system integration benchmarks
    void benchmarkMemorySystemIntegration(size_t steps);
    void benchmarkNeuromodulationOverhead(size_t steps);
    void benchmarkPlasticityOverhead(size_t steps);
    
    // Subsystem interaction benchmarks
    void benchmarkMemorySynapticInteraction(size_t steps);
    void benchmarkCognitiveSystemIntegration(size_t steps);
    void benchmarkDevelopmentEffects(size_t steps);
    
private:
    BenchmarkConfig& config_;
    PerformanceProfiler& profiler_;
    std::unique_ptr<Brain> brain_;
    std::vector<SimulationStep> testSteps_;
    
    void setupBrain(const Config& brainConfig);
    void benchmarkCoreNeurons(size_t steps);
    void benchmarkConnectivity(size_t steps);
    void benchmarkPlasticity(size_t steps);
    void benchmarkAllSystems(size_t steps);
};

class MemoryAccessPatternBenchmark {
public:
    MemoryAccessPatternBenchmark(BenchmarkConfig& config, PerformanceProfiler& profiler);
    ~MemoryAccessPatternBenchmark();
    
    // Memory access pattern benchmarks
    void benchmarkSpatialLocality(size_t neuronCount, size_t steps);
    void benchmarkTemporalLocality(size_t neuronCount, size_t steps);
    void benchmarkCacheBlocking(size_t neuronCount, size_t blockSize);
    void benchmarkNUMAAffinity(size_t neuronCount, size_t cpuCount);
    
    // Object pool performance
    void benchmarkPoolAllocation(size_t iterations);
    void benchmarkPoolDeallocation(size_t iterations);
    void benchmarkPoolReuse(size_t objectCount, size_t iterations);
    
private:
    BenchmarkConfig& config_;
    PerformanceProfiler& profiler_;
    std::unique_ptr<Brain> brain_;
    
    void profileMemoryAccessPattern(const std::function<void()>& testFunc);
    double calculateCacheHitRate(size_t testSteps) const;
    size_t calculateWorkingSetSize(size_t neuronCount) const;
};

} // namespace nlm
```

### 5. Component-Specific Benchmarks

```cpp
// nlm/benchmark/ComponentBenchmarks.hpp

#pragma once

#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "BenchmarkConfig.hpp"
#include "PerformanceProfiler.hpp"

namespace nlm {

class DevelopmentSystemBenchmark {
public:
    DevelopmentSystemBenchmark(BenchmarkConfig& config, PerformanceProfiler& profiler);
    ~DevelopmentSystemBenchmark();
    
    // Stage advancement benchmarks
    void benchmarkStageAdvancement(size_t iterations);
    void benchmarkStageTransitions(size_t iterations);
    void benchmarkStageConsistency(size_t iterations);
    
    // Plasticity modulation benchmarks
    void benchmarkPlasticityModulation(size_t steps);
    void benchmarkCriticalPeriodDynamics(size_t steps);
    void benchmarkMaturationEffects(size_t steps);
    
private:
    BenchmarkConfig& config_;
    PerformanceProfiler& profiler_;
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
};

class NeuromodulationBenchmark {
public:
    NeuromodulationBenchmark(BenchmarkConfig& config, PerformanceProfiler& profiler);
    ~NeuromodulationBenchmark();
    
    // Signal processing benchmarks
    void benchmarkSignalProcessing(size_t iterations);
    void benchmarkPlasticityModulation(size_t iterations);
    void benchmarkAttentionModulation(size_t iterations);
    
    // Integration benchmarks
    void benchmarkNeuromodBrainIntegration(size_t steps);
    void benchmarkNeuroCognitiveIntegration(size_t steps);
    void benchmarkNeuroMotorIntegration(size_t steps);
    
private:
    BenchmarkConfig& config_;
    PerformanceProfiler& profiler_;
    std::vector<std::unique_ptr<Neuromodulator>> neuromodulators_;
};

class CognitiveSystemBenchmark {
public:
    CognitiveSystemBenchmark(BenchmarkConfig& config, PerformanceProfiler& profiler);
    ~CognitiveSystemBenchmark();
    
    // Attention benchmarks
    void benchmarkAttentionAllocation(size_t iterations);
    void benchmarkAttentionShifts(size_t iterations);
    void benchmarkAttentionPersistence(size_t iterations);
    
    // Planning benchmarks
    void benchmarkPlanningComplexity(size_t tasks, size_t depth);
    void benchmarkConceptFormation(size_t concepts, size_t iterations);
    void benchmarkLearningSpeed(size_t iterations);
    
private:
    BenchmarkConfig& config_;
    PerformanceProfiler& profiler_;
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
};

} // namespace nlm
```

### 6. Benchmark Suite Controller

```cpp
// nlm/benchmark/BenchmarkSuite.hpp

#pragma once

#include "BenchmarkConfig.hpp"
#include "PerformanceProfiler.hpp"
#include "MemorySystemBenchmark.hpp"
#include "SimulationLoopBenchmark.hpp"
#include "ComponentBenchmarks.hpp"
#include <string>
#include <vector>
#include <map>

namespace nlm {

class BenchmarkSuite {
public:
    BenchmarkSuite(const std::string& configFile = "benchmark_config.json");
    ~BenchmarkSuite();
    
    // Run benchmarks
    void runAllBenchmarks();
    void runBenchmark(const std::string& name);
    void runScaleBenchmark(BenchmarkConfig::Scale scale);
    void runSpecificBenchmarks(const std::vector<std::string>& benchmarks);
    
    // Benchmark management
    void setConfig(const BenchmarkConfig& config);
    const BenchmarkConfig& getConfig() const { return config_; }
    
    // Results management
    void saveResults(const std::string& filename) const;
    void printResults() const;
    void generateReport(const std::string& filename) const;
    
    // Performance analysis
    void analyzeScalingPerformance() const;
    void analyzeMemoryEfficiency() const;
    void analyzeParallelEfficiency() const;
    void identifyBottlenecks() const;
    
    // Configuration
    static void initializeDefaultConfig();
    static void saveDefaultConfig(const std::string& filename);
    
private:
    BenchmarkConfig config_;
    PerformanceProfiler profiler_;
    
    // Benchmark modules
    std::unique_ptr<WorkingMemoryBenchmark> workingMemoryBench_;
    std::unique_ptr<EpisodicMemoryBenchmark> episodicMemoryBench_;
    std::unique_ptr<AssociativeMemoryBenchmark> associativeMemoryBench_;
    std::unique_ptr<BrainStepBenchmark> brainStepBench_;
    std::unique_ptr<MemoryAccessPatternBenchmark> memoryAccessBench_;
    std::unique_ptr<DevelopmentSystemBenchmark> developmentBench_;
    std::unique_ptr<NeuromodulationBenchmark> neuromodulationBench_;
    std::unique_ptr<CognitiveSystemBenchmark> cognitiveBench_;
    
    // Results storage
    std::map<std::string, PerformanceMetrics> results_;
    std::map<std::string, double> scalingAnalysis_;
    
    void initializeBenchmarks();
    void runBenchmarkSuite();
    void aggregateResults();
    PerformanceMetrics runSingleBenchmark(const std::string& name);
};

} // namespace nlm
```

### 7. Integration with NLM CLI

```cpp
// src/cli/BenchmarkCommand.cpp

#include "../benchmark/BenchmarkSuite.hpp"
#include <iostream>

namespace nlm {

class BenchmarkCommand {
public:
    static void execute(int argc, char* argv[]);
    static void printHelp();
    static BenchmarkSuite createBenchmarkSuite(int argc, char* argv[]);
    
private:
    static void parseArguments(int argc, char* argv[], BenchmarkSuite& suite);
    static void runBenchmarks(BenchmarkSuite& suite, const std::string& command);
    static void exportResults(BenchmarkSuite& suite, const std::string& outputFile);
};

} // namespace nlm
```

### 8. Python Bindings

```cpp
// python/benchmark_module.cpp

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../benchmark/BenchmarkSuite.hpp"

namespace py = pybind11;

PYBIND11_MODULE(benchmark, m) {
    py::class_<nlm::BenchmarkSuite>(m, "BenchmarkSuite")
        .def(py::init<const std::string&>(), py::arg("config_file") = "benchmark_config.json")
        .def("run_all_benchmarks", &nlm::BenchmarkSuite::runAllBenchmarks)
        .def("run_benchmark", &nlm::BenchmarkSuite::runBenchmark, py::arg("name"))
        .def("run_scale_benchmark", &nlm::BenchmarkSuite::runScaleBenchmark)
        .def("save_results", &nlm::BenchmarkSuite::saveResults, py::arg("filename"))
        .def("print_results", &nlm::BenchmarkSuite::printResults)
        .def("get_config", &nlm::BenchmarkSuite::getConfig);
    
    // Configuration constants
    py::enum_<nlm::BenchmarkConfig::Scale>(m, "Scale")
        .value("MICRO", nlm::BenchmarkConfig::Scale::MICRO)
        .value("SMALL", nlm::BenchmarkConfig::Scale::SMALL)
        .value("MEDIUM", nlm::BenchmarkConfig::Scale::MEDIUM)
        .value("LARGE", nlm::BenchmarkConfig::Scale::LARGE)
        .export_values();
    
    // Performance metrics
    py::class_<nlm::PerformanceMetrics>(m, "PerformanceMetrics")
        .def_readonly("steps_per_second", &nlm::PerformanceMetrics::stepsPerSecond)
        .def_readonly("neurons_per_second", &nlm::PerformanceMetrics::neuronsPerSecond)
        .def_readonly("spikes_per_second", &nlm::PerformanceMetrics::spikesPerSecond)
        .def_readonly("memory_usage", &nlm::PerformanceMetrics::memoryBytes)
        .def_readonly("cache_miss_rate", &nlm::PerformanceMetrics::cacheMissRate)
        .def_readonly("simd_enabled", &nlm::PerformanceMetrics::simdEnabled)
        .def_readonly("efficiency", &nlm::PerformanceMetrics::efficiency);
    
    m.doc() = "Performance benchmarking for NLM neural simulation framework";
}
```

### 9. Example Usage

```python
# python/example_benchmark.py

import nlm.benchmark
import time

# Create benchmark suite with default configuration
bench = nlm.benchmark.BenchmarkSuite()

# Run all benchmarks
print("Running comprehensive benchmark suite...")
start_time = time.time()
bench.run_all_benchmarks()
end_time = time.time()

print(f"Benchmark completed in {end_time - start_time:.2f} seconds")

# Print results
bench.print_results()

# Export to file
bench.save_results("nlm_benchmark_results.json")

# Generate detailed report
bench.generate_report("nlm_benchmark_report.html")

# Run specific scale benchmarks
print("\nRunning medium scale benchmark...")
bench.run_scale_benchmark(nlm.benchmark.Scale.MEDIUM)

# Export medium scale results
bench.save_results("nlm_medium_benchmark_results.json")

print("Benchmarking complete!")
```

### 10. Configuration Files

#### benchmark_config.json

```json
{
    "benchmark": {
        "config_file": "benchmark_config.json",
        "output_dir": "./benchmark_results",
        "enable_python_bindings": true,
        "enable_profiling": true
    },
    
    "scales": {
        "MICRO": {
            "neuron_count": 1000,
            "synapse_count": 10000,
            "steps": 1000,
            "target_steps_per_second": 10000
        },
        "SMALL": {
            "neuron_count": 10000,
            "synapse_count": 100000,
            "steps": 5000,
            "target_steps_per_second": 5000
        },
        "MEDIUM": {
            "neuron_count": 100000,
            "synapse_count": 1000000,
            "steps": 10000,
            "target_steps_per_second": 1000
        },
        "LARGE": {
            "neuron_count": 1000000,
            "synapse_count": 10000000,
            "steps": 5000,
            "target_steps_per_second": 100
        }
    },
    
    "benchmarks": [
        "working_memory_storage",
        "working_memory_retrieval",
        "episodic_memory_search",
        "associative_memory_learning",
        "brain_step_performance",
        "memory_access_patterns",
        "development_system",
        "neuromodulation",
        "cognitive_systems"
    ],
    
    "measurement": {
        "measure_memory_usage": true,
        "measure_cache_efficiency": true,
        "measure_content": true,
        "measure_simd_efficiency": true,
        "measure_parallel_efficiency": true
    }
}
```

#### benchmark_runner.py

```python
#!/usr/bin/env python3
"""
Benchmark runner script for NLM performance testing.
"""

import sys
import os
import time
import json
import argparse
from pathlib import Path

# Add the benchmark module to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import nlm.benchmark as benchmark

def run_benchmark_suite(config_file=None, scale=None, benchmarks=None, output_dir=None):
    """Run NLM benchmark suite with specified configuration."""
    
    print("NLM Performance Benchmark Suite")
    print("=" * 50)
    
    # Create benchmark suite
    bench = benchmark.BenchmarkSuite(config_file or "benchmark_config.json")
    
    # Set output directory if specified
    if output_dir:
        # Note: This would require modification to save results to custom directory
        pass
    
    # Run specified benchmarks or all
    if benchmarks:
        print(f"Running {len(benchmarks)} specified benchmarks...")
        for b in benchmarks:
            print(f"  Running: {b}")
            bench.run_benchmark(b)
    elif scale:
        print(f"Running {scale} scale benchmark...")
        bench.run_scale_benchmark(scale)
    else:
        print("Running all benchmarks...")
        bench.run_all_benchmarks()
    
    # Print summary
    print("\nBenchmark Summary:")
    bench.print_results()
    
    # Export results
    output_file = output_dir or "benchmark_results.json"
    print(f"\nExporting results to {output_file}")
    bench.save_results(output_file)
    
    # Generate report
    report_file = output_dir or "benchmark_report.html"
    print(f"Generating report to {report_file}")
    bench.generate_report(report_file)
    
    print("\nBenchmarking complete!")
    
def main():
    parser = argparse.ArgumentParser(description="NLM Performance Benchmark Suite")
    parser.add_argument("--config", help="Path to benchmark configuration file", 
                       default="benchmark_config.json")
    parser.add_argument("--scale", choices=["MICRO", "SMALL", "MEDIUM", "LARGE"],
                       help="Run benchmarks at specific scale")
    parser.add_argument("--benchmark", action="append", 
                       help="Run specific benchmark (can be specified multiple times)")
    parser.add_argument("--output", help="Output directory for benchmark results")
    parser.add_argument("--python", action="store_true", help="Run with Python bindings")
    
    args = parser.parse_args()
    
    # Convert string scale to enum
    scale = None
    if args.scale:
        scale = getattr(benchmark.Scale, args.scale)
    
    run_benchmark_suite(args.config, scale, args.benchmark, args.output)

if __name__ == "__main__":
    main()
```

## Performance Optimization Integration

The benchmark suite integrates with NLM's existing performance infrastructure:

1. **Memory Pool Optimization**: Uses MemoryPool for efficient memory management
2. **SIMD Vectorization**: Leverages SIMD operations through SIMDVectorization
3. **Parallel Processing**: Integrates with ParallelProcessing for multithreading
4. **Event Queue Optimization**: Uses EventQueue for efficient spike event handling
5. **Configuration**: Respects NLM performance configuration options

## Usage Instructions

### Building and Running

```bash
# Build with benchmark support
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON -DBUILD_BENCHMARKS=ON ..
make -j$(nproc)

# Or build Python bindings with benchmarks
python -m pip install -e .
```

### Running Benchmarks

```bash
# Run from command line
./nlm benchmark --scale MEDIUM --output results/

# Run specific benchmarks
./nlm benchmark --benchmark working_memory_storage --benchmark episodic_memory_search

# Run all benchmarks
./nlm benchmark

# Python bindings
python benchmark_runner.py --scale MEDIUM --benchmark working_memory_storage episodic_memory_search
```

### Analyzing Results

The benchmark suite generates:

1. **JSON results**: Machine-readable performance metrics
2. **CSV results**: Spreadsheet-compatible data
3. **HTML report**: Visual performance analysis
4. **Console output**: Human-readable summary

### Performance Target Scales

| Scale | Neurons | Synapses | Target Steps/sec | Memory Target |
|-------|---------|----------|-----------------|---------------|
| Micro | 1,000 | ~10,000 | 10,000+ | < 10 MB |
| Small | 10,000 | ~100,000 | 5,000+ | < 100 MB |
| Medium | 100,000 | ~1,000,000 | 1,000+ | < 1 GB |
| Large | 1,000,000 | ~10,000,000 | 100+ | < 10 GB |

## Optimization Opportunities

The benchmark suite helps identify:

1. **Memory bottlenecks**: High allocation counts, poor cache utilization
2. **Computation bottlenecks**: Low SIMD utilization, serial bottlenecks
3. **Concurrency bottlenecks**: Poor parallel efficiency, lock contention
4. **Scaling issues**: Performance degradation with increasing scale
5. **Configuration opportunities**: Under/over-utilized optimizations

## Future Enhancements

1. **Adaptive benchmarking**: Dynamic test selection based on available resources
2. **Hardware-specific optimizations**: CPU feature detection and tuning
3. **GPU benchmarking**: CUDA-enabled benchmarks for large-scale simulations
4. **Real-time monitoring**: Live performance tracking during simulation runs
5. **Regression testing**: Automated performance comparison across versions

This comprehensive benchmark suite provides the tools needed to optimize NLM's performance across all identified areas while maintaining correctness and providing actionable insights for further improvements.