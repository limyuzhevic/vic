# NLM Command Line Interface
# Advanced tools for NLM brain simulation and analysis

## Overview

The NLM Command Line Interface (NLM-CLI) provides advanced tools for researchers and power users to work with the NLM neural simulation framework. It includes:

- Brain debugging and monitoring
- Performance profiling and optimization
- Configuration management and validation
- Data analysis and visualization
- Research experiment execution
- Memory and resource management

## Installation

Build NLM with command line tools:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_CLI_TOOLS=ON
make -j4
```

## Available Commands

### nlm_cli - Main Command Line Interface

The main CLI provides access to all subcommands:

```bash
nlm_cli --help

Subcommands:
  brain         Brain simulation and debugging tools
  profile       Performance profiling and analysis tools
  config        Configuration management and validation
  analysis      Data analysis and visualization tools
  experiment    Research experiment execution tools
  memory        Memory management and monitoring
  system        System resource monitoring
  benchmark     Performance benchmarking tools
```

### nlm_brain - Brain Simulation and Debugging

Brain simulation and debugging tools for working with NLM brains:

```bash
nlm_brain --help

Commands:
  create        Create a new brain from configuration
  load          Load a brain from checkpoint
  save          Save brain state to checkpoint
  inspect       Inspect brain structure and state
  simulate      Run simulation with monitoring
  debug         Debug simulation with detailed output
  analyze       Analyze simulation results
  export        Export simulation data for analysis
  compare       Compare multiple brain simulations
```

**Example usage:**
```bash
# Create a brain from configuration
nlm_brain create --config my_config.cfg --neurons 1000 --regions 5

# Save brain state
nlm_brain save --input brain.bin --output checkpoint.bin

# Run simulation with monitoring
nlm_brain simulate --input brain.bin --steps 1000 --output results.json

# Inspect brain structure
nlm_brain inspect --input brain.bin --details
```

### nlm_profile - Performance Profiling and Analysis

Performance profiling tools for optimizing NLM simulations:

```bash
nlm_profile --help

Commands:
  start         Start performance profiling
  stop          Stop current profiling session
  analyze       Analyze performance data
  benchmark     Run performance benchmarks
  optimize      Generate optimization recommendations
  monitor       Real-time performance monitoring
  compare       Compare performance across runs
  export        Export performance data
```

**Example usage:**
```bash
# Start performance profiling
nlm_profile start --output profile.json --sampling-rate 100

# Run performance benchmark
nlm_profile benchmark --config benchmark_config.json --duration 60

# Analyze performance results
nlm_profile analyze --input benchmark.json --output analysis.html

# Generate optimization recommendations
nlm_profile optimize --input profile.json --recommendations optimize.json
```

### nlm_config - Configuration Management

Configuration management and validation tools:

```bash
nlm_config --help

Commands:
  validate      Validate configuration files
  generate      Generate configuration templates
  compare       Compare configuration files
  merge         Merge configuration files
  export        Export configuration to formats
  import        Import configuration from formats
  validate-keys Validate configuration keys against schema
  check-values  Check configuration values against ranges
```

**Example usage:**
```bash
# Validate configuration
nlm_config validate --input config.json --schema schema.json

# Generate configuration template
nlm_config generate --output template.json --sections memory,plasticity

# Compare two configurations
nlm_config compare --config1 config1.json --config2 config2.json

# Check configuration values
nlm_config check-values --config config.json --ranges ranges.json
```

### nlm_analysis - Data Analysis and Visualization

Data analysis tools for simulation results and experimental data:

```bash
nlm_analysis --help

Commands:
  import        Import analysis data
  export        Export analysis data
  statistics    Compute statistical analysis
  correlate     Compute correlations between datasets
  cluster       Perform clustering analysis
  visualize     Generate data visualizations
  compare       Compare datasets and analyses
  model         Build predictive models
  predict       Make predictions from models
  validate      Validate analysis results
```

**Example usage:**
```bash
# Import analysis data
nlm_analysis import --input results.csv --format csv

# Compute statistics
nlm_analysis statistics --input data.json --output stats.json

# Generate visualization
nlm_analysis visualize --input data.json --output plot.html --type heatmap

# Perform clustering analysis
nlm_analysis cluster --input data.json --k 5 --output clusters.json
```

### nlm_experiment - Research Experiment Execution

Research experiment tools for running and managing scientific experiments:

```bash
nlm_experiment --help

Commands:
  create        Create a new experiment
  run           Run an experiment
  manage        Manage experiment lifecycle
  analyze       Analyze experiment results
  compare       Compare experiment results
  report        Generate experiment reports
  export        Export experiment data
  import        Import experiment configurations
```

**Example usage:**
```bash
# Create a research experiment
nlm_experiment create --name memory_experiment --hypothesis "Memory capacity increases with development"

# Run an experiment
nlm_experiment run --config experiment_config.json --output results.json

# Generate experiment report
nlm_experiment report --input results.json --output report.html

# Compare experiment results
nlm_experiment compare --experiments exp1.json exp2.json --output comparison.json
```

### nlm_memory - Memory Management and Monitoring

Memory management and monitoring tools for NLM simulations:

```bash
nlm_memory --help

Commands:
  monitor       Monitor memory usage in real-time
  profile       Profile memory usage patterns
  leak-detect   Detect memory leaks
  optimize      Optimize memory usage
  snapshot      Take memory snapshots
  compare       Compare memory usage
  report        Generate memory reports
  trace         Trace memory allocation patterns
```

**Example usage:**
```bash
# Monitor memory usage in real-time
nlm_memory monitor --interval 1 --output memory_log.csv

# Profile memory usage patterns
nlm_memory profile --input brain.bin --output memory_profile.json

# Detect memory leaks
nlm_memory leak-detect --input simulation.log --output leak_report.json

# Generate memory optimization recommendations
nlm_memory optimize --input profile.json --output optimization.json
```

### nlm_system - System Resource Monitoring

System resource monitoring tools for NLM simulations:

```bash
nlm_system --help

Commands:
  monitor       Monitor system resources
  profile       Profile system usage
  analyze       Analyze system performance
  optimize      Optimize system configuration
  benchmark     Run system benchmarks
  compare       Compare system configurations
  report        Generate system reports
```

**Example usage:**
```bash
# Monitor system resources in real-time
nlm_system monitor --interval 0.1 --output system_metrics.json

# Analyze system performance
nlm_system analyze --input system_metrics.json --output performance_analysis.json

# Run system benchmarks
nlm_system benchmark --config benchmark_config.json --duration 30
```

### nlm_benchmark - Performance Benchmarking

Performance benchmarking tools for NLM simulations:

```bash
nlm_benchmark --help

Commands:
  run          Run performance benchmarks
  analyze      Analyze benchmark results
  compare      Compare benchmark results
  report       Generate benchmark reports
  export       Export benchmark data
  import       Import benchmark configurations
  schedule     Schedule benchmark runs
  monitor      Monitor benchmark execution
```

**Example usage:**
```bash
# Run performance benchmarks
nlm_benchmark run --config benchmark_config.json --output results.json

# Analyze benchmark results
nlm_benchmark analyze --input results.json --output analysis.html

# Compare benchmark results
nlm_benchmark compare --runs run1.json run2.json --output comparison.json

# Generate benchmark report
nlm_benchmark report --input results.json --output report.pdf
```

## Configuration Files

### Command Line Configuration

NLM-CLI can be configured using command line options, configuration files, or environment variables:

#### Configuration File Format

```json
{
    "nlm_cli": {
        "default_config": "~/.nlm/cli_config.json",
        "output_directory": "/tmp/nlm_output",
        "verbose_logging": true,
        "quiet_mode": false,
        "auto_save": true
    },
    "brain_tools": {
        "default_neurons": 1000,
        "default_regions": 2,
        "default_timestep": 0.001
    },
    "profile_tools": {
        "default_sampling_rate": 100,
        "default_duration": 60,
        "enable_detailed_profiling": true
    }
}
```

#### Environment Variables

```bash
export NLM_CLI_CONFIG="~/.nlm/cli_config.json"
export NLM_OUTPUT_DIR="/tmp/nlm_output"
export NLM_LOG_LEVEL="INFO"
export NLM_DEFAULT_NEURONS="1000"
```

## Advanced Features

### Scripting and Automation

NLM-CLI supports scripting for automation and batch processing:

```bash
#!/bin/bash

# Run a series of experiments
nlm_experiment create --name exp1 --config config1.json
nlm_experiment run --config exp1.json
nlm_profile benchmark --config benchmark1.json
nlm_analysis export --input results.json --output exp1_results.json

# Generate summary report
nlm_benchmark analyze --input benchmark_results.json --output summary.html
```

### API Integration

NLM-CLI can be integrated into Python, R, or other scientific computing environments:

```python
import subprocess
import json

# Run an experiment via API
result = subprocess.run([
    "nlm_experiment", "run",
    "--config", "experiment_config.json",
    "--output", "results.json"
], capture_output=True, text=True)

if result.returncode == 0:
    with open("results.json", "w") as f:
        json.dump(json.loads(result.stdout), f)
```

### Custom Plugins

NLM-CLI supports custom plugins for extending functionality:

```bash
# Load custom plugins
export NLM_PLUGINS="~/plugins/my_plugin.so"

# Use custom plugin
nlm_custom --plugin my_plugin --config custom_config.json
```

## Troubleshooting

### Common Issues

1. **"Command not found"**
   - Ensure NLM-CLI is installed and in PATH
   - Run `make install` if using package manager

2. **"Configuration file not found"**
   - Check file path and permissions
   - Use absolute paths or ensure working directory is correct

3. **"Permission denied"**
   - Check file permissions
   - Run with appropriate privileges

4. **"Invalid configuration"**
   - Validate configuration file format
   - Use `nlm_config validate` to check configuration

### Getting Help

For detailed help on specific commands:

```bash
nlm_brain --help
nlm_profile --help
nlm_config --help
nlm_analysis --help
nlm_experiment --help
nlm_memory --help
nlm_system --help
nlm_benchmark --help
```

For general help and examples:

```bash
nlm_cli --help
```

## License

NLM-CLI is licensed under the MIT License. See LICENSE file for details.

## Authors

NLM-CLI is maintained by the NLM project team. Contributions are welcome.

## Version

Current version: 2.0.0

Last updated: 2026-11- бизнес

## Migration Guide

For migrating from older versions:

1. **Configuration Changes**: Update configuration files to new format
2. **Command Structure**: New command structure with enhanced functionality
3. **Plugin System**: New plugin architecture for extending functionality
4. **API Integration**: Enhanced API support for integration with other tools

See CHANGELOG for detailed migration information.
