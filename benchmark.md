# NLM Performance Benchmark Suite
# Comprehensive performance benchmarks for the NLM (Neural Learning Machine) project

This benchmark suite provides comprehensive performance analysis of NLM systems,
including CPU performance, memory usage, scalability testing, and configuration optimization.

## Overview

The NLM performance benchmark suite measures and analyzes the performance of NLM
systems under various conditions:

- **CPU Performance**: Simulation speed and computational efficiency
- **Memory Usage**: Memory consumption patterns and optimization
- **Scalability**: Performance scaling with network size
- **Configuration Impact**: Performance differences between configuration presets
- **Real-world Simulation**: Performance in realistic scenarios

## Features

### 1. CPU Performance Benchmarks
- Simulation speed measurement
- Operations per second (OPS) calculation
- CPU cycle utilization
- Parallel processing efficiency

### 2. Memory Usage Analysis
- Memory allocation patterns
- Garbage collection impact
- Memory fragmentation analysis
- Peak memory usage tracking

### 3. Scalability Testing
- Network size scaling (100 to 10,000 neurons)
- Connection density impact
- Simulation time scaling
- Memory scaling analysis

### 4. Configuration Optimization
- Performance comparison between presets
- Optimal parameter discovery
- Trade-off analysis (speed vs. accuracy)
- Resource allocation optimization

### 5. Real-world Performance
- Complex simulation scenarios
- Mixed workload performance
- Long-running simulation stability
- Fault tolerance testing

### 6. Profiling and Analysis
- Detailed performance metrics
- Bottleneck identification
- Optimization recommendations
- Comparative analysis

## Usage

### Command Line Interface

```bash
# Run CPU performance benchmarks
./benchmark cpu

# Run memory usage benchmarks
./benchmark memory

# Run scalability benchmarks
./benchmark scalability

# Run configuration comparison
./benchmark config

# Run all benchmark suites
./benchmark all

# Run specific benchmark with custom parameters
./benchmark cpu --neurons 1000 --steps 5000
./benchmark memory --duration 60
./benchmark scalability --sizes 100,500,1000,2000
```

### Python API

```python
import benchmark

# Run CPU benchmarks
results = benchmark.run_cpu(neuron_counts=[100, 500, 1000, 2000])

# Run memory benchmarks
mem_results = benchmark.run_memory(simulation_time=300)

# Run scalability benchmarks
scale_results = benchmark.run_scalability()

# Compare configurations
config_results = benchmark.compare_configurations()

# Generate performance report
benchmark.generate_report(results, "nlm_performance_report.html")
```

## Components

### 1. Benchmark Engine
- Core benchmarking framework
- Measurement utilities
- Result collection and analysis
- Performance metric calculation

### 2. CPU Benchmarks
- **Speed Benchmark**: Measure simulation time per step
- **OPS Benchmark**: Calculate operations per second
- **Parallel Benchmark**: Test parallel processing efficiency
- **Throughput Benchmark**: Measure data processing capacity

### 3. Memory Benchmarks
- **Allocation Benchmark**: Track memory allocation patterns
- **Retention Benchmark**: Measure memory retention over time
- **Efficiency Benchmark**: Calculate memory efficiency ratios
- **Peak Benchmark**: Identify peak memory usage

### 4. Scalability Benchmarks
- **Size Scaling**: Test performance with different network sizes
- **Density Scaling**: Test performance with different connection densities
- **Time Scaling**: Measure how simulation time scales with network complexity
- **Resource Scaling**: Test resource requirements at different scales

### 5. Configuration Benchmarks
- **Preset Comparison**: Compare performance of different configuration presets
- **Parameter Optimization**: Find optimal parameters for specific workloads
- **Trade-off Analysis**: Balance between performance and accuracy
- **Resource Allocation**: Optimize resource allocation for different scenarios

### 6. Real-world Benchmarks
- **Complex Scenarios**: Test performance in realistic simulation scenarios
- **Mixed Workloads**: Measure performance with diverse workloads
- **Long-running Tests**: Test stability over extended simulation periods
- **Fault Tolerance**: Measure performance under failure conditions

### 7. Analysis Tools
- **Statistical Analysis**: Statistical analysis of benchmark results
- **Trend Analysis**: Identify performance trends over time
- **Bottleneck Analysis**: Identify performance bottlenecks
- **Optimization Recommendations**: Provide optimization suggestions

## Configuration

### Configuration File

```yaml
# benchmark configuration
benchmarks:
  cpu:
    neuron_counts: [100, 500, 1000, 2000]
    simulation_steps: [1000, 5000, 10000]
    warmup_steps: 100
    measurement_repeats: 5
  
  memory:
    simulation_duration: 300  # seconds
    monitor_interval: 1
    baseline_memory: 100  # MB
    
  scalability:
    neuron_sizes: [100, 500, 1000, 2000, 5000]
    connection_densities: [0.05, 0.1, 0.2]
    
  configuration:
    presets: [default, exploration, exploitation, balanced, reproduction, playful]
    comparison_metrics: [speed, memory, accuracy]
  
  reporting:
    format: "html"
    output_dir: "benchmark_results"
    include_charts: true
    include_raw_data: true
```

### Command Line Options

```
--cpu, -c                    Run CPU performance benchmarks
--memory, -m                Run memory usage benchmarks
--scalability, -s           Run scalability benchmarks
--configuration, -C         Run configuration comparison benchmarks
--realworld, -r             Run real-world simulation benchmarks
--all, -a                   Run all benchmark suites
--output DIR, -o DIR        Output directory for benchmark results
--format FORMAT, -f FORMAT   Output format (html, json, csv, text)
--include-charts, -c         Include performance charts in reports
--include-raw, -r           Include raw benchmark data
--config-file FILE, -cf FILE Configuration file path
--help, -h                  Show help message
--version, -v               Show version information
```

## Examples

### CPU Performance Benchmark

```bash
# Run CPU benchmarks with different network sizes
./benchmark cpu --neurons 100,500,1000,2000 --steps 1000,5000,10000,10000

# Detailed CPU benchmark report
./benchmark cpu --output benchmark_results/cpu --format html --include-charts
```

**Example Output:**

```
NLM CPU Performance Benchmarks
===============================

Network Size | Steps | Total Time | Steps/sec | OPS | Memory (MB) | Speed (Mops/sec)
-------------|-------|-----------|-----------|-----|-------------|-----------------
100 neurons  | 1,000 | 0.12s      | 8,333     | 8.3 | 45.2         | 0.04
500 neurons  | 5,000 | 0.68s      | 7,353     | 7.4 | 224.7        | 0.03
1,000 neurons | 10,000| 1.45s      | 6,897     | 6.9 | 448.3        | 0.03
2,000 neurons | 10,000| 3.21s      | 3,113     | 3.1 | 892.7        | 0.01

Performance Analysis:
- Scaling efficiency: 32% (compared to ideal scaling)
- Memory efficiency: 45.2 MB per 1,000 neurons
- Average OPS: 6.4 Mops/sec per neuron
- Bottleneck: Memory bandwidth at larger network sizes
```

### Memory Usage Benchmark

```bash
# Run memory usage benchmarks
./benchmark memory --duration 300 --output benchmark_results/memory
```

**Example Output:**

```
NLM Memory Usage Benchmarks
===========================

Memory Metrics (300-second simulation):
- Peak memory usage: 892.7 MB
- Average memory usage: 423.1 MB
- Memory growth rate: 0.0012 MB/step
- Memory efficiency: 0.474 neurons/MB
- Garbage collection overhead: 2.3%

Memory Profile:
- Allocation pattern: Linear with step count
- Peak allocation: 892.7 MB at step 298
- Average allocation rate: 2.98 MB/sec
- Memory fragmentation: Low (2.1%)
```

### Scalability Benchmark

```bash
# Run scalability benchmarks
./benchmark scalability --output benchmark_results/scalability
```

**Example Output:**

```
NLM Scalability Benchmarks
==========================

Network Size Scalability:
Size   | Time (s) | Steps/sec | Memory (MB) | Efficiency
-------|----------|-----------|-------------|-----------
100    | 0.12     | 8,333     | 45.2        | 100%
500    | 0.68     | 7,353     | 224.7       | 99%
1,000  | 1.45     | 6,897     | 448.3       | 98%
2,000  | 3.21     | 3,113     | 892.7       | 97%
5,000  | 8.76     | 570       | 2,232       | 95%

Connection Density Impact:
Density | Time (s) | Steps/sec | Memory (MB)
--------|----------|-----------|-------------
0.05    | 0.89     | 5,619     | 235.8
0.10    | 1.45     | 6,897     | 448.3
0.20    | 2.91     | 3,448     | 896.7
```

### Configuration Comparison

```bash
# Compare different configuration presets
./benchmark configuration --output benchmark_results/config --format html
```

**Example Output:**

```
NLM Configuration Performance Comparison
========================================

Performance Comparison (1,000 neurons, 5,000 steps):

Configuration  | Speed (steps/sec) | Memory (MB) | Accuracy | Power
--------------|------------------|-------------|----------|-------
Default       | 6,897            | 448.3       | 0.85      | Medium
Exploration   | 7,234            | 512.7       | 0.92      | High
Exploitation  | 6,456            | 398.2       | 0.88      | Medium
Balanced      | 7,012            | 475.6       | 0.90      | Medium
Reproduction  | 5,876            | 362.4       | 0.86      | Low
Playful      | 6,543            | 421.9       | 0.89      | Medium

Optimal Configuration: Exploration
- Best balance of speed and accuracy
- Moderate memory usage
- High exploration motivation
```

### Real-world Simulation Benchmark

```bash
# Run real-world simulation benchmarks
./benchmark realworld --output benchmark_results/realworld
```

**Example Output:**

```
NLM Real-world Simulation Benchmarks
===================================

Real-world Simulation Results (200 steps, 20x20 world):
- Successful actions: 168/200 (84.0%)
- Failed actions: 32/200 (16.0%)
- Average episode length: 4.2 steps
- Total spikes: 2,847
- Energy efficiency: 0.734 J/spike
- Development stage progression: 0.0 → 4.0

Performance Under Stress:
- At 150 steps: 92.5% success rate
- At 180 steps: 84.0% success rate
- At 200 steps: Agent energy depletion
- Memory usage peak: 876.3 MB

Real-world Performance Score: 8.2/10
```

## Analysis Tools

### Performance Analysis
- **Statistical Analysis**: Calculate mean, median, standard deviation, confidence intervals
- **Trend Analysis**: Identify performance trends over time or across configurations
- **Bottleneck Analysis**: Identify performance bottlenecks and optimization opportunities
- **Comparative Analysis**: Compare performance across different runs or configurations

### Visualization Tools
- **Charts**: Generate performance charts (line charts, bar charts, scatter plots)
- **Graphs**: Create performance graphs with various metrics
- **Dashboards**: Interactive dashboards for real-time monitoring
- **Reports**: Generate comprehensive performance reports

### Optimization Tools
- **Parameter Optimization**: Find optimal parameters for specific workloads
- **Resource Allocation**: Optimize CPU and memory allocation
- **Performance Tuning**: Identify and fix performance bottlenecks
- **Scaling Analysis**: Analyze performance scaling characteristics

## Integration with Build System

### Makefile Integration

```makefile
# Benchmark targets
benchmark: benchmark/cpu benchmark/memory benchmark/scalability benchmark/configuration

benchmark/cpu:
	./benchmark --cpu --output results/cpu

benchmark/memory:
	./benchmark --memory --output results/memory

benchmark/scalability:
	./benchmark --scalability --output results/scalability

benchmark/configuration:
	./benchmark --configuration --output results/configuration

benchmark/all:
	./benchmark --all --output results/all

# Clean benchmark results
clean-benchmarks:
	rm -rf results/*

# Generate benchmark report
generate-report:
	./benchmark --all --output results/full_report --format html
```

### CMake Integration

```cmake
# Add benchmark target
add_custom_target(benchmark ALL
    COMMAND ${CMAKE_COMMAND} -E chdir ${PROJECT_BINARY_DIR} ./benchmark --all
    DEPENDS nlm nlm_cli benchmark
    COMMENT "Running NLM performance benchmarks"
)

# Add benchmark dependency to main target
add_dependencies(nlm benchmark)
add_dependencies(nlm_cli benchmark)
```

## Results Analysis

### Performance Metrics

The benchmark suite collects the following performance metrics:

1. **Speed Metrics**
   - Simulation steps per second (steps/sec)
   - Operations per second (OPS)
   - CPU cycles per operation
   - Throughput (data per second)

2. **Memory Metrics**
   - Peak memory usage (MB)
   - Average memory usage (MB)
   - Memory allocation rate (MB/sec)
   - Memory efficiency (neurons/MB)
   - Garbage collection overhead (%)

3. **Scalability Metrics**
   - Scaling efficiency (%)
   - Performance per neuron (steps/sec/neuron)
   - Memory per neuron (MB/neuron)
   - Resource utilization (%)

4. **Quality Metrics**
   - Simulation accuracy (%)
   - Learning effectiveness (%)
   - Convergence time (steps)
   - Stability (fluctuation range)

### Performance Classification

Based on benchmark results, NLM performance is classified as follows:

- **Excellent**: > 10,000 steps/sec, < 100 MB, > 95% accuracy
- **Good**: 5,000-10,000 steps/sec, 100-500 MB, 90-95% accuracy
- **Fair**: 2,000-5,000 steps/sec, 500-1,000 MB, 85-90% accuracy
- **Poor**: < 2,000 steps/sec, > 1,000 MB, < 85% accuracy

### Performance Optimization Recommendations

Based on benchmark analysis, the following optimization recommendations are provided:

1. **Memory Optimization**
   - Use more efficient data structures
   - Implement memory pooling
   - Optimize garbage collection

2. **CPU Optimization**
   - Use SIMD instructions for vector operations
   - Optimize memory access patterns
   - Reduce synchronization overhead

3. **Configuration Optimization**
   - Choose appropriate configuration for specific workloads
   - Balance between performance and accuracy
   - Consider hardware characteristics

4. **Scaling Optimization**
   - Distribute workload across CPU cores
   - Use hierarchical neural organization
   - Implement efficient communication patterns

## Conclusion

The NLM performance benchmark suite provides comprehensive performance analysis
for NLM systems, helping users understand performance characteristics, identify
bottlenecks, and optimize configurations for specific workloads. The suite includes
benchmarks for CPU performance, memory usage, scalability, configuration optimization,
and real-world simulation, providing a complete picture of NLM performance.

The benchmark suite is designed to be:

- **Comprehensive**: Covers all aspects of NLM performance
- **Flexible**: Supports different benchmarking scenarios
- **Actionable**: Provides clear recommendations for optimization
- **Integratable**: Easily integrates with existing build systems
- **Scalable**: Supports benchmarking at different scales

Performance benchmarks are essential for understanding NLM capabilities and
optimizing system configuration for specific applications and hardware platforms.
```
