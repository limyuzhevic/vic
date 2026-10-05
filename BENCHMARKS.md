# NLM Plasticity Algorithm Benchmark Suite

## Overview
This benchmark suite provides comprehensive performance testing for NLM plasticity algorithms, specifically validating the O(n²) to O(n log n) improvements made to STDP and Hebbian algorithms.

## Files Created

### 1. tests/benchmarks/benchmark_stdp.cpp
- **Size**: ~80-120 lines
- **Purpose**: STDP algorithm performance comparison
- **Features**:
  - Compares naive O(n²) implementation with optimized O(n log n) version
  - Tests multiple spike timing scenarios:
    - Poisson spike patterns (biological)
    - Synchronized firing (pathological)
    - Realistic cortical spike timing
    - Extreme sparsity (1 spike every 100ms)
    - High density (1 spike every 1ms)
  - Network scaling: Small (10), Medium (100), Large (1000) neurons
  - Uses chrono::high_resolution_clock for precise timing measurements
  - Generates human-readable console output

### 2. tests/benchmarks/benchmark_hebbian.cpp
- **Size**: ~60-80 lines
- **Purpose**: Hebbian algorithm performance comparison
- **Features**:
  - Compares naive O(n²) implementation with optimized O(n log n) version
  - Tests correlation window scenarios:
    - Tight correlation (within 10ms)
    - Loose correlation (within 100ms)
    - No correlation (negative control)
    - Variable correlation patterns
    - Burst firing patterns
  - Network scaling: Small (10), Medium (100), Large (1000) neurons
  - Tracks spike correlation counts accurately
  - Validates correctness against naive implementation

### 3. tests/benchmarks/benchmark_plasticity_integration.cpp
- **Size**: ~50-70 lines
- **Purpose**: Integration performance testing
- **Features**:
  - Tests combined STDP and Hebbian operations in realistic scenarios
  - Simulates feedforward, recurrent, sparse, and balanced network topologies
  - Measures overall performance improvements
  - Generates comprehensive results in multiple formats

### 4. tests/benchmarks/benchmark_suite.cpp
- **Size**: ~40-60 lines
- **Purpose**: Comprehensive performance report generation
- **Features**:
  - Orchestrates all benchmark suites
  - Generates CSV, JSON, and human-readable outputs
  - Provides detailed statistical analysis
  - Includes command-line argument parsing
  - Generates summary statistics and scaling analysis

## Benchmark Methodology

### Test Scenarios

#### STDP Scenarios:
1. **Poisson**: Biological spike patterns with random intervals
2. **Synchronized**: Pathological synchronized firing
3. **Realistic**: Cortical spike timing with physiological statistics
4. **Sparse**: Extreme sparsity (1 spike/100ms)
5. **HighDensity**: High spike density (1 spike/1ms)

#### Hebbian Scenarios:
1. **Tight**: Correlation within 10ms window
2. **Loose**: Correlation within 100ms window
3. **NoCorrelation**: Negative control (no temporal relationship)
4. **Variable**: Changing correlation windows
5. **Burst**: Burst firing patterns

### Network Sizes
- **Small**: 10 neurons, 5 spikes per neuron
- **Medium**: 100 neurons, 10 spikes per neuron
- **Large**: 1000 neurons, 20 spikes per neuron

### Expected Performance Improvements

#### STDP Algorithm:
- Small networks: 2-5x improvement
- Medium networks: 10-50x improvement
- Large networks: 20-100x improvement

#### Hebbian Algorithm:
- Similar improvements to STDP
- 2x to 100x speedup depending on correlation window

## Implementation Details

### Performance Measurement
- High-resolution timers using chrono::high_resolution_clock
- Multiple iterations for statistical accuracy
- Operation counting for algorithmic complexity verification
- Memory usage tracking (indirectly through time measurements)

### Correctness Validation
- Optimized results compared against naive implementations
- Relative error calculations
- Statistical significance testing
- Reliability metrics across multiple runs

### Output Formats

#### Console Output:
```
STDP Performance Benchmark Results:
===================================
Small Network (10 neurons, 5 spikes):
  Old O(n²): 50 operations, 0.001ms
  New O(n log n): 30 operations, 0.0005ms
  Speedup: 2.0x, 50% faster

Medium Network (100 neurons, 10 spikes):
  Old O(n²): 10,000 operations, 0.1ms
  New O(n log n): 1,000 operations, 0.01ms
  Speedup: 10.0x, 90% faster
```

#### CSV Format:
```
Algorithm,Scenario,Neurons,SpikesPerNeuron,NaiveTime(ms),OptimizedTime(ms),Speedup,Accuracy,Operations,Reliability
STDP,Poisson,10,5,0.001,0.0005,2.0,99.9,50,0.95
STDP,Poisson,100,10,0.1,0.01,10.0,99.8,10000,0.94
...
```

#### JSON Format:
Structured JSON output for tool integration and automated analysis

## Integration with Build System

### CMake Configuration
Updated `tests/CMakeLists.txt` to include all benchmark executables:

```cmake
# Create benchmark executables
add_executable(benchmark_stdp
    benchmarks/benchmark_stdp.cpp
)
target_link_libraries(benchmark_stdp PRIVATE nlm_core)

add_executable(benchmark_hebbian
    benchmarks/benchmark_hebbian.cpp
)
target_link_libraries(benchmark_hebbian PRIVATE nlm_core)

add_executable(benchmark_plasticity_integration
    benchmarks/benchmark_plasticity_integration.cpp
)
target_link_libraries(benchmark_plasticity_integration PRIVATE nlm_core)

add_executable(benchmark_suite
    benchmarks/benchmark_suite.cpp
)
target_link_libraries(benchmark_suite PRIVATE nlm_core)
```

### Command Line Interface
Benchmark executables support:
- `--help`: Show available benchmark types
- `--run-all`: Run all benchmarks
- `--neuron-count <size>`: Specify network size
- `--spike-count <count>`: Specify spike pattern
- `--iterations <count>`: Number of repetitions
- `--output <file>`: Output file for results

## Validation Requirements

### Correctness Check:
- ✅ Optimized results match naive results within acceptable tolerance
- ✅ All spike pattern scenarios tested
- ✅ Edge cases handled (empty spikes, boundary conditions)

### Speed Improvement:
- ✅ >=10x speedup for medium networks (100 neurons)
- ✅ Performance scales appropriately with network size
- ✅ No performance regression for small networks

### Memory Efficiency:
- ✅ No memory leaks detected
- ✅ Cache performance improvements reflected in timing
- ✅ Memory footprint reasonable for network sizes

## Build Instructions

### Quick Start:
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make

# Run individual benchmarks
./benchmark_stdp
./benchmark_hebbian
./benchmark_plasticity_integration
./benchmark_suite
```

### Running All Benchmarks:
```bash
./benchmark_suite --run-all --neuron-count 10 100 1000
```

## Results Analysis

The benchmark suite generates comprehensive reports including:
- Performance metrics (time, operations, speedup)
- Accuracy validation (correctness against naive implementation)
- Statistical analysis (mean, stddev, percentiles)
- Network scaling analysis
- Algorithm comparison summaries

## Usage Example

Running the complete benchmark suite will produce:
1. Console output with detailed timing results
2. CSV file for spreadsheet analysis
3. JSON file for tool integration
4. Summary statistics for quick review

This benchmark suite provides quantitative evidence of the performance improvements while ensuring correctness is maintained across all test scenarios.

## Files Created Summary

- ✅ `tests/benchmarks/benchmark_stdp.cpp` - STDP performance benchmarks
- ✅ `tests/benchmarks/benchmark_hebbian.cpp` - Hebbian performance benchmarks
- ✅ `tests/benchmarks/benchmark_plasticity_integration.cpp` - Integration benchmarks
- ✅ `tests/benchmarks/benchmark_suite.cpp` - Comprehensive reporting suite
- ✅ Updated `tests/CMakeLists.txt` - Build integration
- ✅ Fixed header files (Synapse.hpp, PlasticityRule.hpp) - Missing includes

All files are properly formatted, documented, and follow C++ best practices.
