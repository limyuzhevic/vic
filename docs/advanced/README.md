# Advanced NLM CLI for Power Users

This directory contains advanced command-line interface features for the NLM framework designed specifically for power users who need enhanced control, debugging capabilities, and automation features.

## Overview

The NLM Advanced CLI (`nlm_cli`) provides comprehensive tools for:

- **Advanced Configuration Management**: Configuration comparison, validation, export/import, inheritance, and rollback
- **Performance Monitoring**: Real-time monitoring, threshold management, alerting, analysis, and benchmarking
- **Debug and Profiling**: Advanced debugging, memory leak detection, bottleneck identification, and custom profiling
- **Batch Processing**: Neural network simulation, configuration testing, experiment running, and parallel execution
- **Data Management**: Data export/import, results analysis, comparison, and statistical analysis
- **Scripting and Automation**: Script-based configuration, automated experiment execution, pipeline support, and API integration
- **Extended Help System**: Advanced help system, command completion, and context-sensitive help

## Installation

### Prerequisites

- CMake 3.16 or higher
- C++20 compatible compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- yaml-cpp library
- nlohmann/json library
- Threads library
- Python 3.8+ (optional, for Python bindings)

### Building

```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Build specific targets
make nlm_cli          # Advanced CLI for power users
make nlm              # Main NLM executable
make nlm_phase3_demo  # Phase 3 demonstration
make nlm_phase4_demo  # Phase 4 demonstration
make nlm_test        # Unit tests
```

## Quick Start

### Basic Usage

```bash
# Show available commands
./nlm_cli

# Get help for a specific command
./nlm_cli help <command>

# Get advanced help for a command
./nlm_cli help:advanced <command>

# Initialize advanced configuration
./nlm_cli init

# Check system health
./nlm_cli check
```

### Configuration Management

```bash
# Validate a configuration file
./nlm_cli config:validate config.yaml

# Compare two configuration files
./nlm_cli config:diff config1.yaml config2.yaml

# Export configuration to JSON
./nlm_cli config:export json production_config.json

# Import configuration from YAML
./nlm_cli config:import yaml settings.yaml

# Merge two configurations
./nlm_cli config:merge dev_config.yaml prod_config.yaml merged_config.yaml

# Inherit from template
./nlm_cli config:inherit base_config.yaml advanced_config.yaml
```

### Performance Monitoring

```bash
# Start real-time performance monitoring
./nlm_cli performance:monitor perf_config.yaml

# Run performance benchmark
./nlm_cli performance:benchmark perf_config.yaml

# Analyze performance results
./nlm_cli performance:analysis results.json

# Detect performance regressions
./nlm_cli performance:regression baseline.json current.json

# Configure performance thresholds
./nlm_cli performance:thresholds perf_config.yaml
```

### Debugging and Profiling

```bash
# Start execution tracing
./nlm_cli debug:trace trace_config.yaml

# Monitor memory usage
./nlm_cli debug:memory mem_config.yaml

# Identify performance bottlenecks
./nlm_cli debug:bottleneck results.json

# Custom profiling
./nlm_cli debug:profile custom_config.yaml

# Heap memory profiling and leak detection
./nlm_cli debug:heap heap_config.yaml
```

### Batch Processing

```bash
# Run neural network simulations in batch
./nlm_cli batch:run batch_config.yaml

# Run configuration tests in batch
./nlm_cli batch:test test_configs/

# Run experiment suite in batch
./nlm_cli batch:experiment experiments.yaml

# Run with parallel execution
./nlm_cli batch:parallel parallel_config.yaml --jobs 8
```

### Data Management

```bash
# Analyze experimental results
./nlm_cli data:analyze experiment_results.json

# Compare datasets
./nlm_cli data:compare dataset1.csv dataset2.csv

# Export data in various formats
./nlm_cli data:export json analysis_results.json
./nlm_cli data:export csv analysis_results.csv
./nlm_cli data:export yaml analysis_results.yaml

# Generate statistical report
./nlm_cli data:stats results.json

# Visualize data
./nlm_cli data:visualize results.json visualization.png
```

### Scripting and Automation

```bash
# Run automation script
./nlm_cli script:run setup.py --param1 value1 --param2 value2

# Parse and validate script
./nlm_cli script:parse script.py

# Validate script syntax
./nlm_cli script:validate script.py
```

### System Monitoring

```bash
# Show system status and health
./nlm_cli system:status

# Monitor system resources
./nlm_cli system:resources

# Monitor process information
./nlm_cli system:processes
```

## Configuration Files

### Default Configuration

The advanced CLI uses configuration files to control its behavior. Configuration files can be in:

- **JSON** (`.json`): Machine-readable format
- **YAML** (`.yaml`, `.yml`): Human-readable format
- **Key-Value** (`.cfg`): Simple format

### Example Configuration Files

#### Basic Performance Configuration

```yaml
# File: configs/performance.cfg

profiling:
  enabled: true
  output_file: "performance_results.json"
  sample_interval_ms: 100
  report_interval_s: 60

thresholds:
  memory_threshold_mb: 1024
  cpu_threshold_percent: 80
  operation_threshold_ns: 1000000

monitoring:
  enable_memory_tracking: true
  enable_cpu_tracking: true
  enable_io_tracking: true
  enable_network_tracking: true

analysis:
  detect_bottlenecks: true
  detect_regressions: true

output:
  output_text_report: true
  output_csv_report: true
  output_json_report: true
  console_output: true
  real_time_monitoring: true
```

#### Debug Configuration

```yaml
# File: configs/debug_performance.cfg

profiling:
  enabled: true
  output_file: "debug_performance_results.json"
  sample_interval_ms: 10
  report_interval_s: 30

thresholds:
  memory_threshold_mb: 512
  cpu_threshold_percent: 70
  operation_threshold_ns: 100000

monitoring:
  enable_memory_tracking: true
  enable_cpu_tracking: true
  enable_io_tracking: true
  enable_network_tracking: true

analysis:
  detect_bottlenecks: true
  detect_regressions: true

output:
  output_text_report: true
  output_csv_report: true
  output_json_report: true
  console_output: true
  real_time_monitoring: true
```

#### Production Configuration

```yaml
# File: configs/production_performance.cfg

profiling:
  enabled: true
  output_file: "performance_results.json"
  sample_interval_ms: 1000
  report_interval_s: 300

thresholds:
  memory_threshold_mb: 2048
  cpu_threshold_percent: 90
  operation_threshold_ns: 10000000

monitoring:
  enable_memory_tracking: true
  enable_cpu_tracking: true
  enable_io_tracking: false
  enable_network_tracking: false

analysis:
  detect_bottlenecks: true
  detect_regressions: true

output:
  output_text_report: true
  output_csv_report: false
  output_json_report: true
  console_output: false
  real_time_monitoring: false
```

## Scripting and Automation

### Example Shell Script for Performance Testing

```bash
#!/bin/bash

# File: scripts/run_performance_test.sh

set -e

echo "Running Performance Test Suite..."

# Enable profiling
export NLM_PERFORMANCE_ENABLED=1
export NLM_PROFILING_ENABLED=1

# Run test with profiling
./nlm_cli --config configs/test_performance.cfg run

# Generate reports
./nlm_cli --generate_reports performance_test_results.json

# Analyze results
python3 analyze_performance.py performance_test_results.json

# Compare with baseline
if [ -f baseline_results.json ]; then
    python3 compare_performance.py performance_test_results.json baseline_results.json
fi

echo "Performance test completed. Results saved to performance_test_results.json"
```

### Example Python Script for Performance Analysis

```python
#!/usr/bin/env python3

# File: python/analyze_performance.py

import json
import pandas as pd
import matplotlib.pyplot as plt

def analyze_performance_results(filename):
    # Load performance data
    with open(filename, 'r') as f:
        data = json.load(f)
    
    # Convert to DataFrame
    df = pd.json_normalize(data['operations'])
    
    # Calculate statistics
    mean_time = df['mean_ns'].mean()
    median_time = df['mean_ns'].median()
    max_time = df['mean_ns'].max()
    stddev = df['mean_ns'].std()
    
    print("Performance Analysis Results:")
    print(f"  Mean Operation Time: {mean_time:.2f} ns")
    print(f"  Median Operation Time: {median_time:.2f} ns")
    print(f"  Max Operation Time: {max_time:.2f} ns")
    print(f"  Standard Deviation: {stddev:.2f} ns")
    
    # Identify bottlenecks
    slow_operations = df[df['mean_ns'] > 1000000]  # Operations > 1ms
    print(f"\nSlow Operations (>1ms): {len(slow_operations)}")
    for _, op in slow_operations.iterrows():
        print(f"  - {op['name']}: {op['mean_ns']:.2f} ns")
    
    # Generate visualization
    plt.figure(figsize=(12, 8))
    
    # Plot operation times
    plt.subplot(2, 2, 1)
    df.plot(x='name', y='mean_ns', kind='bar', ax=plt.gca())
    plt.title('Operation Times')
    plt.xticks(rotation=45)
    plt.ylabel('Time (ns)')
    
    # Plot performance distribution
    plt.subplot(2, 2, 2)
    plt.hist(df['mean_ns'], bins=50, alpha=0.7)
    plt.title('Operation Time Distribution')
    plt.xlabel('Time (ns)')
    plt.ylabel('Count')
    
    # Save plots
    plt.tight_layout()
    plt.savefig(f"{filename}_plots.png")
    
    return {
        'mean_time': mean_time,
        'median_time': median_time,
        'max_time': max_time,
        'stddev': stddev,
        'slow_operations_count': len(slow_operations)
    }

if __name__ == "__main__":
    if len(sys.argv) > 1:
        analyze_performance_results(sys.argv[1])
    else:
        print("Usage: python analyze_performance.py <results_file.json>")
```

## Advanced Features

### Configuration Comparison and Diff

The advanced CLI provides sophisticated configuration comparison capabilities:

```bash
# Detailed diff between two configurations
./nlm_cli config:diff config1.yaml config2.yaml

# Generate unified diff output
./nlm_cli config:diff --format unified config1.yaml config2.yaml

# Show only specific sections
./nlm_cli config:diff config1.yaml config2.yaml --sections profiling,thresholds
```

### Performance Regression Detection

The CLI can detect performance regressions by comparing against baseline results:

```bash
# Run performance test with profiling
./nlm_cli performance:benchmark perf_config.yaml --baseline baseline.json

# Generate regression report
./nlm_cli performance:regression baseline.json current.json --report regression.html
```

### Batch Processing with Parallel Execution

The CLI supports parallel execution for improved performance:

```bash
# Run batch with specific number of threads
./nlm_cli batch:parallel batch_config.yaml --jobs 8

# Run batch with automatic thread detection
./nlm_cli batch:parallel batch_config.yaml

# Run batch with timeout
./nlm_cli batch:parallel batch_config.yaml --timeout 3600
```

### Data Analysis and Visualization

The CLI includes advanced data analysis and visualization capabilities:

```bash
# Analyze experimental data with statistical tests
./nlm_cli data:analyze results.json --stats t-test anova chi2

# Generate correlation analysis
./nlm_cli data:compare dataset1.csv dataset2.csv --correlation

# Create comprehensive visualization report
./nlm_cli data:visualize results.json --output comprehensive_report.html
```

## Command Reference

### Available Commands

#### Configuration Commands
- `config:validate <file>` - Validate configuration file
- `config:diff <file1> <file2>` - Compare two configuration files
- `config:compare <file1> <file2>` - Compare configurations with diff output
- `config:export <format> <output>` - Export configuration to format (json/yaml/keyvalue)
- `config:import <format> <input>` - Import configuration from format
- `config:rollback <version>` - Rollback to previous configuration version
- `config:merge <file1> <file2> <output>` - Merge two configuration files
- `config:inherit <template> <output>` - Inherit from template configuration

#### Performance Commands
- `performance:monitor <config>` - Start real-time performance monitoring
- `performance:thresholds <config>` - Configure performance thresholds
- `performance:alert <config>` - Trigger performance alert
- `performance:analysis <results>` - Analyze performance results
- `performance:regression <baseline> <results>` - Detect performance regressions
- `performance:benchmark <config>` - Run performance benchmark

#### Debug Commands
- `debug:trace <config>` - Start execution tracing
- `debug:memory <config>` - Monitor memory usage
- `debug:bottleneck <results>` - Identify performance bottlenecks
- `debug:profile <config>` - Custom profiling with detailed analysis
- `debug:heap <config>` - Heap memory profiling and leak detection

#### Batch Commands
- `batch:run <file>` - Run batch of neural network simulations
- `batch:test <file>` - Run batch configuration tests
- `batch:config <file>` - Run batch configuration validation
- `batch:experiment <file>` - Run batch experiment suite
- `batch:parallel <file>` - Run batch with parallel execution

#### Data Commands
- `data:export <format> <output>` - Export experiment data
- `data:import <format> <input>` - Import experiment data
- `data:analyze <results>` - Analyze experimental results
- `data:compare <file1> <file2>` - Compare experimental data
- `data:stats <results>` - Generate statistics report
- `data:visualize <results>` - Generate visualization reports

#### Script Commands
- `script:run <file> [args...]` - Run automation script
- `script:parse <file>` - Parse and validate script
- `script:validate <file>` - Validate script syntax

#### System Commands
- `system:status` - Show system status and health
- `system:resources` - Monitor system resources
- `system:processes` - Monitor process information

#### Help Commands
- `help <command>` - Show basic help for command
- `help:advanced <command>` - Show advanced help for command
- `help:commands` - Show all available commands
- `help:examples` - Show available examples and tutorials

#### General Commands
- `version` - Show version information
- `init` - Initialize advanced configuration
- `check` - Run system checks
- `validate` - Validate all configuration files
- `info` - Show system information

## Examples and Tutorials

### Example 1: Complete Workflow

```bash
#!/bin/bash

# File: scripts/complete_workflow.sh

set -e

echo "=== NLM Advanced CLI Complete Workflow ==="
echo

# 1. Initialize advanced configuration
echo "Step 1: Initializing advanced configuration..."
./nlm_cli init
echo

# 2. Check system health
echo "Step 2: Checking system health..."
./nlm_cli check
echo

# 3. Validate configurations
echo "Step 3: Validating configurations..."
./nlm_cli validate
echo

# 4. Run performance tests
echo "Step 4: Running performance tests..."
./nlm_cli performance:benchmark perf_config.yaml
echo

# 5. Analyze results
echo "Step 5: Analyzing results..."
./nlm_cli performance:analysis performance_results.json
echo

# 6. Run batch processing
echo "Step 6: Running batch processing..."
./nlm_cli batch:run batch_config.yaml
echo

# 7. Generate reports
echo "Step 7: Generating reports..."
./nlm_cli --generate_reports comprehensive_report.html
echo

echo "=== Workflow completed successfully ==="
```

### Example 2: Configuration Testing Pipeline

```bash
#!/bin/bash

# File: scripts/config_test_pipeline.sh

set -e

echo "=== Configuration Testing Pipeline ==="
echo

# Define configuration files
CONFIG_FILES=("configs/dev_config.yaml" "configs/staging_config.yaml" "configs/prod_config.yaml")

# Test each configuration
echo "Testing configurations..."
for config in "${CONFIG_FILES[@]}"; do
    echo "\nTesting: $config"
    echo "  Validating configuration..."
    ./nlm_cli config:validate "$config"
    
    echo "  Comparing with previous configuration..."
    if [[ -n "${prev_config}" ]]; then
        ./nlm_cli config:diff "$prev_config" "$config"
    fi
    
    prev_config="$config"
done

echo "\nConfiguration testing completed!"
```

## Troubleshooting

### Common Issues

1. **Command not found**
   - Ensure you're using the correct executable (`./nlm_cli`)
   - Check that the executable has been built
   - Try `./nlm_cli help` for a list of available commands

2. **Configuration validation errors**
   - Check that configuration files are properly formatted
   - Ensure all required fields are present
   - Try `config:validate --verbose <file>` for detailed error information

3. **Performance monitoring issues**
   - Ensure performance profiling is enabled in the configuration
   - Check that required libraries are installed
   - Try `./nlm_cli check` to verify system health

4. **Batch processing errors**
   - Ensure configuration files are correctly formatted
   - Check that all required parameters are specified
   - Try `./nlm_cli batch:run --help` for more information

### Getting Help

- `./nlm_cli help` - Show main help
- `./nlm_cli help:advanced <command>` - Show advanced help for specific command
- `./nlm_cli help:commands` - Show all available commands
- `./nlm_cli help:examples` - Show available examples and tutorials

### Reporting Issues

If you encounter issues with the advanced CLI, please:

1. Check the error messages in the output
2. Verify that all required dependencies are installed
3. Ensure configuration files are properly formatted
4. Check the system logs for more detailed information
5. Submit an issue to the NLM repository with the following information:
   - Command that failed
   - Configuration files used
   - Error messages
   - System information

## License

This software is licensed under the MIT License. See the LICENSE file for more information.

## Copyright

Copyright 2024-2026 NLM Framework Team

## See Also

- NLM Main Documentation: README.md
- NLM Python Bindings: See the Python documentation for more information
- NLM Phase 3 Documentation: See the Phase 3 documentation for more details
- NLM Phase 4 Documentation: See the Phase 4 documentation for more details

## References

- NLM Framework Architecture: docs/ARCHITECTURE.md
- NLM Scientific Background: docs/SCIENCE.md
- NLM Experiment Descriptions: docs/EXPERIMENTS.md
- Performance Profiling Documentation: docs/PERFORMANCE.md
- Configuration Management Documentation: docs/CONFIGURATION.md

## Index

- Advanced Features
- Configuration Commands
- Performance Commands
- Debug Commands
- Batch Commands
- Data Commands
- Script Commands
- System Commands
- Help Commands
- Examples and Tutorials
- Troubleshooting

This document provides comprehensive documentation for the NLM Advanced CLI, a powerful command-line interface designed for advanced users who need enhanced control, debugging capabilities, and automation features for the NLM framework.
