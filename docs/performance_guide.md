// Performance testing documentation for NLM

# NLM Performance Profiling Guide

This document describes the comprehensive performance profiling capabilities implemented for the NLM (Neural Learning Machine) project.

## Overview

The NLM performance profiling system provides comprehensive performance analysis capabilities including:

- **Core Performance Profiling**: Wall-clock time, CPU time, memory usage, thread-level profiling
- **Advanced Profiling Features**: Performance comparison, benchmarking, scalability analysis
- **Memory Profiling**: Allocation tracking, leak detection, memory usage statistics
- **Thread-Level Profiling**: Thread synchronization analysis, contention detection, performance metrics
- **Performance Benchmarking**: Standardized benchmark suites with statistical analysis
- **Visualization**: HTML dashboards, charts, performance reports in multiple formats

## Architecture

### Core Components

1. **PerformanceProfiler** (`src/profiler/PerformanceProfiler.hpp/.cpp`)
   - Main performance profiling engine
   - Provides comprehensive timing measurements (wall-clock, CPU)
   - Memory usage tracking and allocation analysis
   - Thread-level profiling and synchronization analysis
   - Integration with configuration system

2. **MemoryProfiler** (`src/profiler/MemoryProfiler.hpp/.cpp`)
   - Memory allocation tracking and leak detection
   - Real-time memory monitoring during execution
   - Memory usage statistics and fragmentation analysis
   - Heap allocation profiling

3. **ThreadProfiler** (`src/profiler/ThreadProfiler.hpp/.cpp`)
   - Thread creation and destruction tracking
   - Thread execution time measurements
   - Thread synchronization analysis
   - Contention detection and reporting

4. **Benchmark** (`src/profiler/Benchmark.hpp/.cpp`)
   - Performance benchmarking tools
   - Statistical analysis of benchmark results
   - Performance regression detection
   - Scalability analysis across different input sizes

5. **Visualization** (`src/profiler/Visualization.hpp/.cpp`)
   - Performance visualization and reporting
   - Web-based performance dashboards
   - Multi-format output (JSON, CSV, HTML)
   - Performance trend analysis

6. **ProfilerIntegration** (`src/cli/ProfilerIntegration.hpp/.cpp`)
   - Enhanced CLI profiler integration
   - Integration with batch processing for comparative analysis
   - Real-time performance monitoring
   - Post-execution performance analysis

7. **PerformanceExperiment** (`src/experiments/PerformanceExperiment.hpp/.cpp`)
   - Performance experiments for NLM
   - Configurable performance experiments
   - Integration with existing experiment system
   - Performance data collection and analysis

### Integration Points

- **CLI Integration**: Enhanced CLI with performance profiling options
- **Batch Processing**: Performance comparison across batch configurations
- **Existing Logging System**: Profile output through NLM logging
- **Configuration System**: Profiler settings via configuration files
- **Real-time Monitoring**: Live performance monitoring during execution

## Usage

### Basic Usage

```bash
# Enable performance profiling
nlm --profile

# Enable profiling with specific configuration
nlm --profile --profile-config=performance_config.json

# Enable profiling with real-time monitoring
nlm --profile --real-time

# Enable performance benchmarking
nlm --benchmark

# Enable comprehensive performance analysis
nlm --profile --benchmark --visualize
```

### Configuration File

Create a performance configuration file (`performance_config.json`):

```json
{
  "session_name": "nlm_performance_test",
  "enable_core_profiler": true,
  "enable_memory_profiler": true,
  "enable_thread_profiler": true,
  "enable_benchmarking": true,
  "enable_visualization": true,
  
  "core_profiler_config": {
    "sampling_interval_ms": 100,
    "max_memory_samples": 10000,
    "output_format": "json",
    "output_file": "performance_data.json"
  },
  
  "memory_profiler_config": {
    "enable_allocation_tracking": true,
    "enable_leak_detection": true,
    "max_allocations_to_track": 100000,
    "heap_dump_interval": 1000
  },
  
  "thread_profiler_config": {
    "enable_thread_tracking": true,
    "enable_synchronization_tracking": true,
    "enable_contention_detection": true,
    "sampling_interval_ms": 100
  },
  
  "benchmark_config": {
    "warmup_iterations": 5,
    "benchmark_iterations": 10,
    "cooldown_iterations": 5,
    "timeout_seconds": 60.0,
    "enable_statistics": true,
    "enable_regression_detection": true
  },
  
  "visualization_config": {
    "title": "NLM Performance Dashboard",
    "output_path": "./performance_reports",
    "generate_html": true,
    "generate_json": true,
    "generate_csv": true,
    "theme": "dark"
  },
  
  "integration_config": {
    "enable_baseline_comparison": true,
    "baseline_file": "baseline_performance.json",
    "enable_regression_testing": true,
    "regression_threshold": 0.1,
    "enable_bottleneck_detection": true,
    "enable_scalability_analysis": true,
    "enable_optimization_recommendations": true
  }
}
```

### Command Line Options

Enhanced CLI options for performance profiling:

```
Performance & Profiling:
    --profile                     Enable performance profiling
    --profile-file=<file>         Write profiling data to file
    --time-stats                  Enable timing statistics
    --memory-stats                Enable memory usage tracking
    --thread-stats                Enable thread performance tracking
    --benchmark                   Enable performance benchmarking
    --visualize                   Enable performance visualization
    --real-time                   Enable real-time monitoring
    --baseline=<file>             Load baseline performance data for comparison
    --regression                  Enable performance regression testing
    --bottleneck-detection         Enable bottleneck detection
    --scalability-analysis         Enable scalability analysis
    --optimization-recommendations Enable optimization recommendations

Configuration:
    --profile-config=<file>       Load performance profiler configuration
    --session-name=<name>         Set profiling session name
    --output-directory=<dir>      Set output directory for performance reports
    --sampling-interval=<ms>       Set sampling interval in milliseconds
    --max-samples=<n>            Set maximum number of samples to collect
    --theme=<theme>               Set visualization theme (light, dark, auto)
```

### Example Usage

```bash
# Basic performance profiling with file output
nlm --profile --profile-file=performance_data.json

# Performance profiling with real-time monitoring
nlm --profile --real-time --verbose

# Performance benchmarking
nlm --benchmark --output=benchmark_results.csv

# Comprehensive performance analysis with visualization
nlm --profile --benchmark --visualize --output=performance_dashboard.html

# Performance regression testing
nlm --profile --baseline=baseline_performance.json --regression

# Thread performance analysis
nlm --profile --thread-stats --output=thread_performance.json

# Memory profiling
nlm --profile --memory-stats --leak-detection --output=memory_profile.txt

# Scalability analysis
nlm --profile --scalability-analysis --output=scalability_report.json
```

## Performance Reports

### Output Formats

The performance profiler supports multiple output formats:

1. **JSON**: Structured data for analysis and integration
2. **CSV**: Tabular data for spreadsheet import
3. **HTML**: Interactive dashboards with charts
4. **Text**: Human-readable reports

### JSON Report Structure

```json
{
  "sessionName": "nlm_performance_test",
  "durationMs": 45000,
  "isActive": false,
  "dataPoints": 450,
  
  "samples": [
    {
      "timestamp": 0,
      "wallClockTime": 1000.0,
      "cpuTime": 800.0,
      "memoryUsage": 10485760,
      "throughput": 1.0,
      "metadata": {"session": "nlm_performance_test"}
    }
  ],
  
  "bottlenecks": [
    "High memory usage detected (>1GB average)",
    "Performance degradation over time detected"
  ],
  
  "recommendations": [
    "Optimize memory usage - consider memory pooling or data structure optimization",
    "Performance degradation over time detected. Consider optimization"
  ],
  
  "regressions": [
    "Wall clock time regression (more than 10% slower)"
  ]
}
```

### HTML Dashboard

The visualization system generates interactive HTML dashboards with:

- **Performance Trends**: Line charts showing performance over time
- **Memory Usage**: Bar charts showing memory consumption
- **Thread Performance**: Charts showing thread synchronization and contention
- **Benchmark Results**: Comparison charts for different benchmarks
- **Bottleneck Analysis**: Heat maps highlighting performance bottlenecks
- **Scalability Analysis**: Charts showing scalability characteristics

### Console Output

Example console output:

```
=== Performance Profiling Report ===
Session: nlm_performance_test
Duration: 45000ms
Status: Stopped
Data Points: 450

Performance Metrics:
  Throughput: 1.0 ops/sec
  Average Latency: 1000.0ms
  95th Percentile Latency: 1500.0ms
  Peak Memory Usage: 10.0MB
  CPU Utilization: 80.0%

Bottlenecks:
  - High memory usage detected (>1GB average)
  - Performance degradation over time detected

Recommendations:
  - Optimize memory usage - consider memory pooling or data structure optimization
  - Consider reducing sampling frequency to improve performance
```

## Advanced Features

### Performance Comparison

Compare current performance with baseline data:

```bash
nlm --profile --baseline=baseline_performance.json
```

The profiler will generate a comparison report showing improvements and regressions.

### Regression Testing

Enable automatic performance regression detection:

```bash
nlm --profile --baseline=baseline_performance.json --regression
```

The profiler will detect regressions in key performance metrics and report them.

### Scalability Analysis

Analyze scalability characteristics:

```bash
nlm --profile --scalability-analysis
```

The profiler will analyze how performance scales with input size and identify scaling bottlenecks.

### Bottleneck Detection

Automatically detect performance bottlenecks:

```bash
nlm --profile --bottleneck-detection
```

The profiler will identify CPU-bound, memory-bound, and thread-bound bottlenecks.

### Optimization Recommendations

Generate optimization recommendations based on performance analysis:

```bash
nlm --profile --optimization-recommendations
```

The profiler will provide specific recommendations for improving performance.

## Configuration Options

### Core Profiler Configuration

- `sampling_interval_ms`: Sampling interval for real-time monitoring (default: 100ms)
- `max_memory_samples`: Maximum number of memory samples to collect (default: 10000)
- `max_thread_samples`: Maximum number of thread samples to collect (default: 10000)
- `output_format`: Output format (json, csv, html) (default: json)
- `output_file`: Output file path (default: performance_data.json)

### Memory Profiler Configuration

- `enable_allocation_tracking`: Enable memory allocation tracking (default: true)
- `enable_deallocation_tracking`: Enable memory deallocation tracking (default: true)
- `enable_leak_detection`: Enable memory leak detection (default: true)
- `enable_fragmentation_analysis`: Enable memory fragmentation analysis (default: false)
- `enable_peak_memory_tracking`: Enable peak memory tracking (default: true)
- `max_allocations_to_track`: Maximum number of allocations to track (default: 100000)
- `heap_dump_interval`: Interval between heap dumps (default: 1000)

### Thread Profiler Configuration

- `enable_thread_tracking`: Enable thread tracking (default: true)
- `enable_synchronization_tracking`: Enable synchronization event tracking (default: true)
- `enable_contention_detection`: Enable contention detection (default: true)
- `enable_affinity_tracking`: Enable CPU affinity tracking (default: false)
- `sampling_interval_ms`: Sampling interval for real-time monitoring (default: 100ms)
- `max_thread_data_points`: Maximum number of thread data points (default: 10000)
- `max_synchronization_events`: Maximum number of synchronization events (default: 100000)

### Benchmarking Configuration

- `warmup_iterations`: Number of warm-up iterations (default: 5)
- `benchmark_iterations`: Number of benchmark iterations (default: 10)
- `cooldown_iterations`: Number of cool-down iterations (default: 5)
- `timeout_seconds`: Timeout for each benchmark (default: 60.0)
- `auto_scale`: Enable automatic scaling of benchmarks (default: false)
- `min_expected_duration`: Minimum expected benchmark duration (default: 0.0)
- `max_expected_duration`: Maximum expected benchmark duration (default: 1000.0)
- `enable_statistics`: Enable statistical analysis (default: true)
- `enable_regression_detection`: Enable regression detection (default: true)

### Visualization Configuration

- `title`: Dashboard title (default: "Performance Visualization")
- `output_path`: Output directory for visualization files (default: ".")
- `generate_html`: Generate HTML dashboard (default: true)
- `generate_json`: Generate JSON data (default: true)
- `generate_csv`: Generate CSV data (default: true)
- `generate_pdf`: Generate PDF report (default: false)
- `generate_png`: Generate PNG charts (default: false)
- `theme`: Visualization theme (light, dark, auto) (default: "light")
- `chart_width`: Chart width in pixels (default: 800.0)
- `chart_height`: Chart height in pixels (default: 600.0)
- `x_axis_label`: X-axis label (default: "Time")
- `y_axis_label`: Y-axis label (default: "Value")
- `show_legend`: Show chart legend (default: true)
- `show_grid`: Show chart grid (default: true)
- `show_annotations`: Show chart annotations (default: true)
- `interactive`: Enable interactive charts (default: true)

## Performance Experiment System

### Experiment Types

The performance experiment system supports the following experiment types:

1. **BASIC_PERFORMANCE**: Basic performance testing
2. **MEMORY_PERFORMANCE**: Memory performance testing
3. **THREAD_PERFORMANCE**: Thread performance testing
4. **SCALABILITY**: Scalability analysis
5. **REGRESSION_TESTING**: Performance regression testing
6. **OPTIMIZATION**: Performance optimization testing
7. **BENCHMARKING**: Performance benchmarking
8. **REAL_TIME_MONITORING**: Real-time performance monitoring
9. **THROUGHTPUT_ANALYSIS**: Throughput analysis
10. **LATENCY_ANALYSIS**: Latency analysis

### Experiment Configuration

Experiments can be configured with the following parameters:

- `name`: Experiment name (required)
- `type`: Experiment type (required)
- `duration`: Experiment duration in milliseconds (required)
- `warmup_duration`: Warm-up duration in milliseconds (default: 0)
- `cooldown_duration`: Cool-down duration in milliseconds (default: 0)
- `sampling_interval`: Sampling interval in milliseconds (required)
- `warmup_sampling_interval`: Warm-up sampling interval (required)
- `num_threads`: Number of threads for thread performance experiments
- `num_neurons`: Number of neurons for scalability experiments
- `load_factor`: Load factor for performance experiments
- `enable_real_time_monitoring`: Enable real-time monitoring
- `enable_detailed_metrics`: Enable detailed metrics collection
- `target_throughput`: Target throughput (ops/sec)
- `target_latency`: Target latency (milliseconds)
- `target_memory_usage`: Target memory usage (bytes)
- `validate_regression`: Enable regression validation
- `validate_optimization`: Enable optimization validation
- `regression_threshold`: Regression threshold (default: 0.1)
- `optimization_threshold`: Optimization threshold (default: 0.1)
- `enable_file_output`: Enable file output
- `output_file`: Output file path
- `enable_console_output`: Enable console output
- `enable_statistical_analysis`: Enable statistical analysis
- `enable_trend_analysis`: Enable trend analysis
- `enable_bottleneck_detection`: Enable bottleneck detection
- `enable_recommendation_generation`: Enable recommendation generation

## Integration with NLM Systems

### Batch Processing Integration

Performance profiling integrates with NLM batch processing to enable comparative analysis across different configurations:

```bash
nlm --batch --profile --baseline=baseline_performance.json
```

The profiler will automatically compare performance across batch configurations and generate comprehensive reports.

### Configuration System Integration

Performance profiler settings are integrated with the NLM configuration system:

```ini
[performance]
enabled = true
session_name = nlm_performance_test
output_format = json
output_file = performance_data.json

[profiler.core]
sampling_interval_ms = 100
max_memory_samples = 10000

[profiler.memory]
enable_allocation_tracking = true
enable_leak_detection = true

[profiler.thread]
enable_thread_tracking = true
enable_synchronization_tracking = true
```

### Logging System Integration

Performance profiler output is integrated with the NLM logging system:

```bash
nlm --profile --verbose
```

The profiler will output performance information through the NLM logging system, which can be redirected to files or other destinations.

### Real-time Monitoring

Enable real-time performance monitoring:

```bash
nlm --profile --real-time --verbose
```

The profiler will monitor performance in real-time and output updates at the specified sampling interval.

## Performance Testing

### Unit Tests

The performance profiler includes unit tests in `tests/performance_test.cpp`:

```bash
cmake --build build --target test
./build/nlm_test --gtest_filter="Performance*"
```

### Integration Tests

Integration tests verify the performance profiler integration with other NLM systems:

```bash
# Run all performance tests
cmake --build build --target test

# Run specific performance tests
./build/nlm_test --gtest_filter="PerformanceIntegration*"
```

### Performance Regression Testing

Enable performance regression testing:

```bash
nlm --profile --baseline=baseline_performance.json --regression
```

The profiler will automatically detect and report performance regressions compared to the baseline.

## Troubleshooting

### Common Issues and Solutions

1. **Performance profiler not starting**
   - Solution: Check if profiling is enabled via command line (`--profile`)
   - Solution: Verify configuration file syntax

2. **Performance data not being collected**
   - Solution: Check sampling interval settings
   - Solution: Verify that the profiler is active

3. **Visualization not working**
   - Solution: Check if visualization is enabled
   - Solution: Verify output directory permissions

4. **High memory usage**
   - Solution: Increase sampling interval
   - Solution: Reduce the number of samples to collect

5. **Performance bottlenecks**
   - Solution: Enable bottleneck detection (`--bottleneck-detection`)
   - Solution: Generate optimization recommendations (`--optimization-recommendations`)

### Debug Mode

Enable debug mode for detailed performance information:

```bash
nlm --profile --debug --verbose
```

## Configuration Examples

### Simple Performance Profiling

```bash
nlm --profile --output=performance_data.json
```

### Advanced Performance Profiling with Visualization

```bash
nlm --profile --benchmark --visualize --output=performance_dashboard.html
```

### Performance Regression Testing

```bash
nlm --profile --baseline=baseline_performance.json --regression --output=regression_report.json
```

### Thread Performance Analysis

```bash
nlm --profile --thread-stats --output=thread_performance.json
```

### Memory Profiling

```bash
nlm --profile --memory-stats --leak-detection --output=memory_profile.txt
```

### Scalability Analysis

```bash
nlm --profile --scalability-analysis --output=scalability_report.json
```

## Performance Monitoring Scripts

### Performance Monitoring Script

Create a performance monitoring script (`monitor_performance.sh`):

```bash
#!/bin/bash

# Start NLM with performance profiling
nlm --profile --real-time --verbose &

# Monitor performance for 1 hour
for i in {1..360}; do
    echo "Monitoring iteration $i"
    sleep 10
    # Check performance metrics
    # Generate reports
    # Send alerts if needed
    
    # Stop after 1 hour
    if [ $i -eq 360 ]; then
        echo "Monitoring completed"
        break
    fi
    
done

# Generate final performance report
nlm --profile --generate-report --output=final_performance_report.html
```

### Performance Alerting Script

Create a performance alerting script (`alert_performance.sh`):

```bash
#!/bin/bash

# Check if performance degradation has occurred
if [ -f "baseline_performance.json" ]; then
    # Compare current performance with baseline
    # Check for regressions
    # Send alerts if needed
    
    echo "Performance alerts generated"
fi
```

## Conclusion

The NLM performance profiling system provides comprehensive performance analysis capabilities including:

- **Core Performance Profiling**: Comprehensive timing and resource measurements
- **Advanced Profiling Features**: Performance comparison, benchmarking, and scalability analysis
- **Memory Profiling**: Memory allocation tracking and leak detection
- **Thread-Level Profiling**: Thread synchronization and contention analysis
- **Performance Benchmarking**: Standardized benchmark suites with statistical analysis
- **Visualization**: Interactive dashboards and performance reports
- **Integration**: Seamless integration with NLM batch processing and configuration systems
- **Real-time Monitoring**: Live performance monitoring during execution

This system enables developers to:

1. Identify and resolve performance bottlenecks
2. Compare performance across different configurations
3. Detect performance regressions early
4. Generate optimization recommendations
5. Visualize performance trends and characteristics
6. Monitor performance in real-time

The performance profiling system is fully integrated with the NLM architecture and provides comprehensive performance analysis capabilities for neural network simulations.