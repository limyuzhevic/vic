# NLM Performance Profiling and Benchmarking

## Overview

This document provides comprehensive guidance for using the Performance Profiling and Benchmarking tools in the NLM (Neural Learning Machine) project. These tools enable detailed performance analysis, benchmarking, and optimization of neural simulations.

## Features

### Performance Profiling Tools

#### Time Measurement
- High-precision timing for critical operations
- Microsecond-level accuracy for simulation steps
- Thread-safe timing measurements
- Profiling of individual brain components

#### Memory Usage Tracking
- Real-time memory monitoring
- Peak memory usage tracking
- Memory allocation/deallocation tracking
- Memory leak detection

#### CPU Profiling
- CPU utilization monitoring
- Core-specific CPU usage tracking
- Instruction and cycle counting
- Branch prediction analysis

#### Timeline Visualization
- Event-based timeline profiling
- Thread-aware timeline generation
- Chrome Trace format export
- Interactive timeline viewing

#### Performance Counters
- Custom performance metric collection
- System performance counter monitoring
- Benchmark-specific metrics
- Multi-dimensional performance tracking

#### Memory Leak Detection
- Dynamic memory leak detection
- Stack trace analysis for leaks
- Automated leak reporting
- Leak trend analysis

### Benchmarking Infrastructure

#### Standard Benchmark Suite
- Predefined benchmark configurations
- Multi-scale network sizes
- Step-based performance measurement
- Reproducible benchmark runs

#### Parameterized Benchmarks
- Configurable parameter grids
- Multi-dimensional parameter exploration
- Statistical analysis of parameter impact
- Automated benchmark combination generation

#### Comparison Capabilities
- Baseline comparison
- Automated regression detection
- Performance trend analysis
- Cross-benchmark comparison

#### Performance Regression Detection
- Threshold-based regression detection
- Statistical significance testing
- Automated alert generation
- Regression trend tracking

#### Multi-threaded Benchmarking
- Parallel benchmark execution
- Thread-local performance metrics
- Synchronization-aware benchmarking
- Resource contention analysis

#### System Resource Monitoring
- System-wide resource monitoring
- Real-time system metrics collection
- Resource utilization analysis
- Bottleneck identification

### Integration Points

#### Integration with Existing Brain Systems

##### Integration with Brain
- Brain initialization profiling
- Simulation step performance analysis
- Neural computation timing
- Brain state performance metrics

##### Integration with Neural Dynamics
- Neural dynamics profiling
- Synapse update performance
- Spike propagation timing
- State update optimization analysis

##### Integration with Plasticity Rules
- STDP rule performance profiling
- Hebbian learning timing
- Structural plasticity analysis
- Eligibility trace performance

##### Integration with Memory Systems
- Working memory performance profiling
- Episodic memory timing
- Associative memory analysis
- Memory consolidation metrics

##### Integration with Neuromodulation
- Dopamine system profiling
- Neuromodulator timing
- Learning signal analysis
- Modulatory effect measurement

### Output and Reporting

#### Console Output with Formatting
- Table-based result display
- Progress indicators
- Timing displays
- Status reporting

#### File Output (CSV, JSON, HTML)
- CSV format for spreadsheet analysis
- JSON format for programmatic access
- HTML format for web viewing
- Compressed output formats

#### Summary Reports
- Executive summary reports
- Detailed performance analysis
- Optimization recommendations
- Comparative analysis reports

#### Statistical Analysis
- Mean, median, and standard deviation
- Confidence intervals
- Statistical significance testing
- Performance distribution analysis

#### Performance Regression Detection
- Automated regression detection
- Detailed regression reports
- Severity classification
- Trend analysis

### Tools

#### Benchmark Runner
- Command-line benchmark execution
- Batch benchmark running
- Result aggregation
- Report generation

#### Profiler Viewer
- Interactive timeline visualization
- Memory usage analysis
- CPU usage analysis
- Event filtering and searching

#### Memory Analyzer
- Memory allocation tracking
- Leak detection and analysis
- Memory usage optimization
- Memory profile comparison

#### CPU Profiler
- CPU timeline visualization
- Function-level profiling
- Hotspot identification
- Optimization recommendations

#### Performance Comparison Tools
- Baseline comparison
- Performance regression analysis
- Cross-version comparison
- Automated comparison reporting

## API Reference

### Core Classes

#### Profiler
```cpp
// Start/stop profiling sessions
profiler.startSession("session_name");
profiler.endSession();

// Record metrics
profiler.recordTimer("operation_name", duration);
profiler.recordMemoryUsage(memory_usage);
profiler.recordCPUStats(cpu_stats);
```

#### BenchmarkSuite
```cpp
// Add benchmarks
benchmark_suite.addBenchmark(config);

// Run benchmarks
auto results = benchmark_suite.runBenchmarks({"benchmark1", "benchmark2"});

// Export results
benchmark_suite.exportResults("results.csv", BenchmarkSuite::Format::CSV);
```

#### OutputFormatter
```cpp
// Format results
OutputFormatter::printBenchmarkResults(results);
OutputFormatter::printDetailedResults(result);

// Export formats
OutputFormatter::exportToCSV(results, "results.csv");
OutputFormatter::exportToJSON(results, "results.json");
OutputFormatter::exportToHTML(results, "results.html");
```

### Python Integration

#### NLMProfiler
```python
from nlm.profiling_tools import NLMProfiler

# Create profiler
profiler = NLMProfiler()

# Start session
profiler.start_session("my_benchmark")

# Add benchmarks
profiler.add_benchmark("memory_test", memory_benchmark, "memory")
profiler.add_benchmark("cpu_test", cpu_benchmark, "cpu")

# Run benchmarks
results = profiler.run_benchmarks()

# Generate reports
profiler.generate_report("performance_report.html")
profiler.export_results("results.csv")
```

#### BrainPerformanceProfiler
```python
from nlm.profiling_tools import BrainPerformanceProfiler

# Profile brain operations
profiler = BrainPerformanceProfiler()
metrics = profiler.profile_brain_step(brain, steps=100)
```

#### PerformanceComparison
```python
from nlm.profiling_tools import PerformanceComparison

# Compare performance
spec = PerformanceComparison()
spec.add_baseline("baseline", baseline_results)
spec.add_current("current", current_results)

comparisons = spec.compare_all()
spec.export_comparisons("comparison.csv")
```

## Configuration Files

### Benchmark Configuration (JSON)
```json
{
    "benchmarks": [
        {
            "name": "brain_performance",
            "description": "Basic brain performance test",
            "neuron_counts": [100, 500, 1000, 2000],
            "step_counts": [1000, 1000, 500, 200],
            "parameters": {
                "regions": "1",
                "connection_probability": "0.1"
            },
            "tags": ["brain", "performance", "scalability"],
            "enable_profiler": false
        }
    ]
}
```

### Profiler Configuration (INI)
```ini
[profiler]
output_format = html
output_file = profiler_results.html
max_memory_mb = 1024
cpu_sampling_interval = 100

[benchmarks]
brain_performance = true
memory_efficiency = true
multithreading = true
event_driven = true

[regression_detection]
enabled = true
threshold_percent = 10
alert_on_regression = true
```

## Example Scripts

### 1. Basic Benchmarking
```bash
#!/bin/bash

# Run NLM performance benchmarks
./benchmark_runner --config benchmark_config.json --output results.html

# Run specific benchmarks
./benchmark_runner --benchmarks brain_performance memory_efficiency --profiler

# Run regression analysis
./benchmark_runner --regression --baseline baseline.json --current current.json
```

### 2. Python Integration Example
```python
#!/usr/bin/env python3

from nlm.profiling_tools import NLMProfiler, PerformanceComparison
import time
import random

def memory_benchmark():
    """Benchmark memory allocation patterns."""
    data = []
    for i in range(1000):
        data.append([random.random() for _ in range(100)])
    return sum(sum(row) for row in data)

def cpu_benchmark():
    """Benchmark CPU-intensive operations."""
    result = 0
    for i in range(10000):
        result += i * 0.001
    return result

def main():
    # Create profiler
    profiler = NLMProfiler()
    
    # Define benchmarks
    benchmarks = {
        "memory": memory_benchmark,
        "cpu": cpu_benchmark
    }
    
    # Add to profiler
    for name, func in benchmarks.items():
        profiler.add_benchmark(name, func)
    
    # Run benchmarks
    results = profiler.run_benchmarks()
    
    # Export results
    profiler.export_results("results.csv")
    profiler.generate_report("performance_report.html")
    
    print(f"Benchmarks completed: {len(results)}")
    print("Results exported to results.csv and performance_report.html")

if __name__ == "__main__":
    main()
```

### 3. Automated Performance Testing
```python
#!/usr/bin/env python3

from nlm.profiling_tools import BrainPerformanceProfiler, PerformanceComparison
import json
import os
from datetime import datetime

def run_performance_suite(brain_config):
    """Run a comprehensive performance suite."""
    profiler = BrainPerformanceProfiler()
    
    # Test different network sizes
    network_sizes = [100, 500, 1000, 2000]
    step_counts = [100, 500, 1000]
    
    results = {}
    
    for neurons in network_sizes:
        for steps in step_counts:
            key = f"{neurons}_neurons_{steps}_steps"
            print(f"Testing {key}...")
            
            # This would create and configure a brain
            # brain = create_brain(brain_config)
            
            # Profile the brain
            # metrics = profiler.profile_brain_step(brain, steps)
            metrics = {
                'neurons': neurons,
                'steps': steps,
                'time': steps * 0.001,  # Mock timing
                'spikes': neurons * steps * 0.1
            }
            
            results[key] = metrics
    
    return results

def compare_with_baseline(current_results, baseline_file="baseline.json"):
    """Compare current results with baseline."""
    spec = PerformanceComparison()
    
    # Load baseline
    if os.path.exists(baseline_file):
        with open(baseline_file, 'r') as f:
            baseline_results = json.load(f)
        spec.add_baseline("baseline", baseline_results)
    
    # Add current results
    spec.add_current("current", current_results)
    
    # Compare
    comparisons = spec.compare_all()
    
    # Export and report
    spec.export_comparisons("comparison_results.csv")
    spec.generate_regression_report("regression_report.html")
    
    return comparisons

def main():
    # Run performance suite
    results = run_performance_suite({})
    
    # Save current results
    with open(f"current_results_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json", 'w') as f:
        json.dump(results, f, indent=2)
    
    # Compare with baseline
    comparisons = compare_with_baseline(results)
    
    print(f"Performance suite completed")
    print(f"Results saved to current_results_*.json")
    print(f"Comparison exported to comparison_results.csv")
    print(f"Regression report generated to regression_report.html")

if __name__ == "__main__":
    main()
```

## Getting Started

### 1. Build with Profiling Support
```bash
# Build with profiling enabled
cmake -DNLM_PERFORMANCE_ENABLED=ON -DCMAKE_BUILD_TYPE=Debug ..
make -j4
```

### 2. Install Python Dependencies
```bash
# Install required Python packages
pip install pybind11 numpy pandas matplotlib

# Install the profiling module
pip install -e .
```

### 3. Run Basic Benchmarks
```bash
# Run the built benchmark executable
./benchmark

# Run with Python integration
python profiling_tools.py --benchmark
```

### 4. Use the Python API
```python
from nlm.profiling_tools import get_profiler, get_benchmark_suite

profiler = get_profiler()
benchmark_suite = get_benchmark_suite()

# Define and run benchmarks
profiler.add_benchmark("test", my_function)
results = benchmark_suite.runBenchmarks(["test"])
```

## Troubleshooting

### Common Issues

#### 1. Module Not Found
```bash
# Install the profiling module
pip install -e .

# Add to Python path
export PYTHONPATH=/path/to/nlm/src:$PYTHONPATH
```

#### 2. Build Errors
```bash
# Check if required dependencies are installed
pip install pybind11 numpy pandas matplotlib
```

#### 3. Profiler Not Working
```cpp
// Ensure performance infrastructure is enabled
// Add -DNLM_PERFORMANCE_ENABLED to compiler flags
```

### Performance Tips

#### For Large Networks
- Use `OptimizedBrain` instead of basic `Brain`
- Enable multithreading for large networks
- Use sparse connectivity for memory efficiency

#### For Detailed Profiling
- Use event-driven profiling for fine-grained analysis
- Enable memory leak detection for long-running simulations
- Monitor system resources during benchmarking

#### For Regression Detection
- Set appropriate regression thresholds
- Use statistical significance testing
- Regular baseline updates for comparison

## License

This performance profiling and benchmarking suite is part of the NLM project and is licensed under the MIT License.

## Credits

- NLM Performance Team
- Contributors to the NLM project
- Open source contributors to performance analysis tools

## Version History

- Version 1.0.0: Initial release
- Version 1.1.0: Enhanced Python integration
- Version 1.2.0: Added comprehensive benchmarking suite
- Version 1.3.0: Added regression detection
- Version 2.0.0: Major overhaul with new profiling infrastructure
