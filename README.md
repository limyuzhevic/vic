# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Enhanced NLM Features

This document describes the enhanced features added to the NLM (Neural Learning Machine) framework, including improved CLI, modular experiments, performance monitoring, and advanced configuration management.

## Overview

The enhanced NLM Phase 2 system now provides:

1. **Enhanced Command Line Interface** - Modular experiment selection with comprehensive options
2. **Advanced Configuration System** - Better validation, type checking, and CLI parsing
3. **Modular Experiment Framework** - Dynamic experiment selection and execution
4. **Performance Monitoring** - Real-time metrics, resource tracking, and analysis
5. **Improved Error Handling** - Better diagnostics and recovery mechanisms
6. **Enhanced Output Formats** - Multiple output formats for different use cases

## Enhanced CLI Usage

### Basic Usage

```bash
# Run all enhanced experiments with default configuration
./nlm

# Run specific experiments
./nlm --experiment=basic --experiment=plasticity --experiment=stdp

# Run experiments with enhanced configuration
./nlm --experiment=all --format=json --performance --debug --verbose=2

# Run with custom configuration
./nlm --config=my_config.cfg --experiment=learning --output_dir=./results
```

### Available Experiment Types

#### Core Experiments (always available):
- **`all`** - Run all core experiments (basic, plasticity, stdp, learning)
- **`basic`** - Test 1: Basic Neural Connectivity
- **`plasticity`** - Test 2: Plasticity Learning Experiment
- **`stdp`** - Test 3: STDP Verification
- **`learning`** - Test 4: Comprehensive Learning Analysis

#### Advanced Experiments:
- **`phase3`** - Phase 3 complex behaviors
- **`phase4`** - Phase 4 cognitive functions
- **`benchmark`** - Performance benchmarking
- **`ablation`** - Ablation studies
- **`scaling`** - Scaling analysis

### Output Formats

- **`text`** - Human-readable console output (default)
- **`json`** - JSON format for machine processing
- **`csv`** - CSV format for data analysis
- **`markdown`** - Markdown report format

### Configuration Options

#### Core Settings:
- `--config=<file>` - Load configuration from file
- `--random_seed=<seed>` - Random seed for reproducible experiments
- `--neuron_count=<n>` - Number of neurons in the network
- `--timestep=<ms>` - Simulation timestep in milliseconds

#### Plasticity Settings:
- `--stdp_ltp=<w>` - STDP LTP weight factor
- `--stdp_ltd=<w>` - STDP LTD weight factor
- `--synaptogenesis_rate=<r>` - Synaptogenesis rate
- `--pruning_rate=<r>` - Pruning rate

#### Advanced Options:
- `--performance` / `--perf` - Enable performance monitoring
- `--verbose=<n>` - Verbose logging level (0-3)
- `--debug` - Enable debug mode with additional diagnostics
- `--output_dir=<dir>` - Output directory for experiment results

## Enhanced Configuration System

### Configuration File Format

NLM uses a simple key=value format for configuration files:

```ini
# Example configuration file
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.2
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.02
stdp_tau = 25.0
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002
performance_monitoring = true
debug_mode = false
verbose_level = 1
output_format = json
experiment = all
output_dir = results/
```

### CLI Argument Parsing

The enhanced configuration system supports multiple CLI formats:

```bash
# Standard format
./nlm --neuron_count=2000 --timestep=0.002 --experiment=learning

# Short format
./nlm -c 3000 -t 0.001 -e plasticity -f csv

# File loading
./nlm --config=/path/to/config.txt

# Environment variable overrides
export NLM_NEURON_COUNT=5000
./nlm
```

### Configuration Validation

The enhanced configuration system includes:

1. **Type Validation** - Ensures configuration values match expected types
2. **Range Validation** - Validates values are within acceptable ranges
3. **Dependency Validation** - Checks that required configurations are present
4. **Custom Validators** - User-defined validation rules
5. **Source Tracking** - Tracks configuration source (default, file, CLI, runtime)

### Advanced Configuration Features

#### Configuration Aliases

Many configuration options support aliases:

```bash
# Multiple ways to set the same configuration
./nlm --neuron_count=1000
./nlm --neurons=1000
./nlm -n 1000
```

#### Configuration Interpolation

Configuration files can use interpolation:

```ini
# config.cfg
base_neurons = 1000
scaled_neurons = ${base_neurons}  # References base_neurons
final_neurons = ${scaled_neurons}  # References scaled_neurons
```

#### Configuration Commands

You can also use configuration commands:

```bash
./nlm --set=synapse_rate=0.001 --set=debug=true
./nlm --add=memory_pool_size=1024
./nlm --remove=temporary_setting
```

## Modular Experiment Framework

### Experiment Selection

The enhanced experiment framework allows flexible experiment selection:

#### By Experiment Name:
```bash
./nlm --experiment=basic --experiment=plasticity
```

#### By Category:
```bash
./nlm --experiment_category=core --experiment_category=advanced
```

#### Conditional Experiments:
```bash
./nlm --experiment_conditional=performance_threshold=100ms
```

### Experiment Information

```bash
# List all available experiments
./nlm --list_experiments

# Get experiment details
./nlm --experiment_info=basic

# Get experiments by category
./nlm --experiments_by_category=core
```

### Experiment Dependencies

Experiments can have dependencies that must be satisfied before execution:

```ini
# experiments.cfg
experiment.basic.dependencies = 
experiment.plasticity.dependencies = basic
experiment.stdp.dependencies = plasticity
experiment.learning.dependencies = stdp
```

### Experiment Priority

Experiments can have priority levels for execution order:

```bash
./nlm --experiment_priority=basic=10 --experiment_priority=plasticity=20
```

## Performance Monitoring

### Real-Time Monitoring

The enhanced performance monitoring system provides:

#### Basic Monitoring:
```bash
./nlm --performance
```

#### Detailed Performance Reports:
```bash
./nlm --performance --format=markdown --output_dir=./perf_reports
```

### Performance Metrics

The performance monitor tracks:

- **Execution Time** - Time taken for each operation
- **Memory Usage** - Memory consumption during experiments
- **CPU Utilization** - CPU usage during experiments
- **Performance Levels** - Operations per second
- **Resource Usage** - Overall system resource consumption

### Performance Analysis

```bash
# Get performance statistics
./nlm --performance --analyze=true

# Export performance data
./nlm --performance --export_json=performance_data.json
./nlm --performance --export_csv=performance_data.csv
```

## Output Formats

### Text Format (Default)

Human-readable console output with detailed progress information:

```
=== Basic Neural Connectivity Test ===
Injecting current into 10 neurons...
Step 0 | Spikes: 0 | Firing: 0 | Synapses: 123
Step 100 | Spikes: 5 | Firing: 2 | Synapses: 124
...
✓ Spikes propagate through network
```

### JSON Format

Machine-readable output for automated processing:

```json
{
  "experiments_run": "basic,plasticity",
  "test1_duration": "1.2s",
  "test2_duration": "45.3s",
  "total_execution_time": "46.5s",
  "performance_metrics": {
    "cpu_usage": 45.2,
    "memory_usage": 128.5
  },
  "experiment_results": {
    "basic": "success",
    "plasticity": "success"
  }
}
```

### CSV Format

Spreadsheet-compatible format for data analysis:

```csv
experiment,timestamp,duration,status,spikes_generated,memory_usage
basic,2024-01-01T12:00:00Z,1.2s,success,5,128.5
plasticity,2024-01-01T12:01:00Z,45.3s,success,150,256.8
learning,2024-01-01T12:02:30Z,90.1s,success,300,512.3
```

### Markdown Format

Rich-text documentation format:

```markdown
# NLM Phase 2 Experiment Results

## Overview
- Total Experiments: 4
- Success Rate: 100%
- Total Duration: 2.5 hours
- Average Performance: 98.5%

## Detailed Results

### Basic Neural Connectivity
- Duration: 1.2s
- Status: ✓ Success
- Spikes Generated: 5

### Plasticity Learning Experiment
- Duration: 45.3s
- Status: ✓ Success
- Spikes Generated: 150
```

## Advanced Error Handling

### Error Detection and Recovery

The enhanced system includes:

1. **Early Validation** - Validates configurations before execution
2. **Graceful Degradation** - Continues operation with degraded functionality
3. **Automatic Recovery** - Attempts to recover from errors automatically
4. **Detailed Diagnostics** - Provides detailed error information
5. **Error Logging** - Logs all errors for later analysis

### Error Types

- **Configuration Errors** - Invalid configuration values
- **Resource Errors** - Insufficient memory or CPU
- **Operation Errors** - Failed experiments or operations
- **System Errors** - System-level failures

### Error Handling Examples

```bash
# Run with enhanced error handling
./nlm --experiment=all --continue_on_error=true

# Get detailed error reports
./nlm --experiment=all --error_details=true

# Log errors to file
./nlm --experiment=all --log_file=experiment_errors.log
```

## Advanced Usage Examples

### Example 1: Performance Benchmarking

```bash
./nlm \
  --experiment=all \
  --performance \
  --format=json \
  --output_dir=./benchmark_results \
  --verbose=2 \
  --debug \
  --analysis=true
```

### Example 2: Custom Experiment Pipeline

```bash
./nlm \
  --config=production.cfg \
  --experiment=basic,plasticity,learning \
  --experiment_conditional=performance_threshold=50ms \
  --output_format=csv \
  --output_directory=./production_results \
  --performance_monitoring=true \
  --debug_mode=true
```

### Example 3: Development and Testing

```bash
# Run with debug mode and verbose logging
./nlm \
  --experiment=basic,plasticity,learning,benchmark \
  --debug \
  --verbose=3 \
  --performance \
  --output_directory=./debug_results \
  --continue_on_error=true
```

## Configuration File Examples

### Minimal Configuration

```ini
# minimal.cfg
neuron_count = 500
timestep = 0.001
experiment = all
```

### Advanced Configuration

```ini
# advanced.cfg
# Core settings
simulation_timestep = 0.001
neuron_count = 2000
region_count = 2
connection_probability = 0.15

# Plasticity settings
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Advanced features
performance_monitoring = true
debug_mode = false
verbose_level = 1
output_format = json
experiment = all
output_dir = results/

# Experimental settings
test_repeats = 3
learning_threshold = 0.01
max_steps = 1000
```

## Performance Monitoring Examples

### Real-Time Monitoring

```bash
# Start real-time monitoring
./nlm --performance --realtime=true

# Get real-time statistics
./nlm --performance --statistics=true

# Export real-time data
./nlm --performance --export_rt=rt_metrics.json
```

### Historical Analysis

```bash
# Analyze historical performance
./nlm --performance --analysis=historical

# Compare performance across runs
./nlm --performance --compare=baseline,current
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: Configuration file not found
**Solution:** Specify correct path or use `--config=` with relative path

#### Issue: Too many neurons
**Solution:** Reduce neuron_count or increase available memory

#### Issue: Performance monitoring not working
**Solution:** Check system capabilities or disable with `--no-performance`

#### Issue: Output directory not writable
**Solution:** Check permissions or specify different output directory

### Debug Mode

```bash
# Run with full debug mode
./nlm --debug --verbose=3 --continue_on_error=true

# Enable additional debugging
./nlm --experiment=all --debug --detailed_tracing=true
```

## Migration Guide

### From Version 1.x to 2.x

The enhanced version 2.x includes breaking changes:

#### Changes:
1. **CLI Arguments** - New experiment selection options
2. **Configuration Format** - Enhanced with validation
3. **Experiment Selection** - New dependency system
4. **Output Formats** - New JSON and CSV support
5. **Performance Monitoring** - New monitoring system

#### Migration Steps:
1. Update configuration files to new format
2. Modify experiment selection in scripts
3. Update output processing for new formats
4. Add performance monitoring where needed
5. Test thoroughly with enhanced validation

## License and Attribution

This enhanced version is part of the NLM research project. Please cite:

```
@software{nlm_enhanced_2.0,
  title={NLM Phase 2 Enhanced Experimental Framework},
  author={NLM Research Team},
  year={2026},
  version={2.0.0},
  note={Enhanced CLI, modular experiments, performance monitoring}
}
```

## Support and Community

For support with the enhanced features:

1. **Documentation:** https://nlm.readthedocs.io/en/2.0/
2. **Issues:** https://github.com/nlm-project/nlm/issues
3. **Discussions:** https://github.com/nlm-project/nlm/discussions
4. **Examples:** Check examples/ directory for more usage patterns

## Acknowledgments

Special thanks to contributors who enhanced the NLM framework with:

- Advanced CLI and configuration system
- Modular experiment framework
- Performance monitoring and statistics
- Enhanced error handling and diagnostics
- Multiple output format support

This enhanced version continues the tradition of innovation in artificial neural computation research.

---

*For more information about the original NLM project, visit https://github.com/nlm-project/nlm*
