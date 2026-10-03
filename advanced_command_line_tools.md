# NLM Advanced Command-Line Tools

This document provides comprehensive documentation for the advanced command-line tools available in the enhanced NLM system.

## Overview

The advanced command-line tools provide enhanced capabilities for:

1. **Simulation Control**
   - Run simulations with advanced configurations
   - Monitor performance in real-time
   - Save and load simulation states

2. **Agent Management**
   - Configure agent behavior
   - Apply neuromodulation strategies
   - Manage developmental stages

3. **Signal Processing**
   - Apply signal filtering
   - Normalize and analyze signals
   - Monitor signal quality

4. **Performance Optimization**
   - Profile simulation performance
   - Optimize neural network parameters
   - Manage memory usage

## Toolset Overview

### 1. NLM Advanced Simulation Runner

**Purpose**: Run advanced NLM simulations with comprehensive monitoring and control.

```bash
nlm-advanced-run [OPTIONS] CONFIG_FILE
```

**Options**:

| Option | Description | Default |
|--------|-------------|---------|
| `--steps N` | Number of simulation steps | 1000 |
| `--world-width W` | World width | 20 |
| `--world-height H` | World height | 20 |
| `--vision-width V` | Vision width | 8 |
| `--vision-height V` | Vision height | 8 |
| `--neuron-count N` | Number of neurons | 1000 |
| `--synapse-density D` | Synapse density | 0.1 |
| `--enable-plasticity` | Enable plasticity mechanisms | false |
| `--enable-neuromod` | Enable neuromodulation | false |
| `--enable-curiosity` | Enable curiosity-driven exploration | false |
| `--enable-development` | Enable developmental processes | false |
| `--checkpoint-interval I` | Checkpoint save interval | 200 |
| `--performance-log FILE` | Log performance data to file | performance.json |
| `--signal-log FILE` | Log signal data to file | signals.json |
| `--verbose` | Enable verbose output | false |
| `--help` | Show help information | |

**Example Usage**:

```bash
# Run advanced simulation with comprehensive monitoring
nlm-advanced-run config.json \
  --steps 2000 \
  --world-width 30 \
  --world-height 30 \
  --vision-width 12 \
  --vision-height 12 \
  --neuron-count 2000 \
  --synapse-density 0.15 \
  --enable-plasticity \
  --enable-neuromod \
  --enable-curiosity \
  --enable-development \
  --checkpoint-interval 300 \
  --performance-log perf_data.json \
  --signal-log signal_data.json \
  --verbose
```

**Configuration File Example** (`config.json`):

```json
{
  "brain": {
    "neuron_count": 2000,
    "synapse_density": 0.15,
    "connection_probability": 0.05,
    "initial_weight_mean": 0.5,
    "initial_weight_std": 0.1
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "enable": true,
      "learning_rate": 0.005
    },
    "structural": {
      "enable": true,
      "synaptogenesis_rate": 0.0001,
      "pruning_rate": 0.00001
    }
  },
  "neuromodulation": {
    "dopamine": {
      "scale": 2.0
    },
    "curiosity": {
      "enable": true,
      "exploration_rate": 0.5
    },
    "novelty": {
      "enable": true,
      "decay_rate": 0.99
    }
  },
  "development": {
    "enable": true,
    "plasticity_modifier": {
      "initial": 1.0,
      "critical_period": 0.8,
      "maturation": 0.5,
      "adult": 0.2
    }
  }
}
```

### 2. NLM Signal Processing Tool

**Purpose**: Apply advanced signal processing to sensory data.

```bash
nlm-signal-process [OPTIONS] INPUT_FILE OUTPUT_FILE
```

**Options**:

| Option | Description | Default |
|--------|-------------|---------|
| `--signal-type TYPE` | Type of signal to process | all |
| `--filter-type TYPE` | Type of filter to apply | moving_average |
| `--filter-size N` | Filter size | 5 |
| `--filter-sigma S` | Filter sigma (for Gaussian) | 1.0 |
| `--normalize` | Normalize signals | false |
| `--zero-mean` | Apply zero-mean transformation | false |
| `--add-noise N` | Add noise with level N | 0.0 |
| `--scale-factor F` | Scale factor for signals | 1.0 |
| `--output-format FORMAT` | Output format (json, binary, csv) | json |
| `--help` | Show help information | |

**Example Usage**:

```bash
# Apply moving average filter to vision signals
nlm-signal-process input_signals.json output_signals.json \
  --signal-type vision \
  --filter-type moving_average \
  --filter-size 7 \
  --normalize \
  --output-format json

# Apply Gaussian filtering and add noise to all signals
nlm-signal-process sensor_data.bin processed_data.bin \
  --filter-type gaussian \
  --filter-size 5 \
  --filter-sigma 2.0 \
  --add-noise 0.02 \
  --normalize \
  --zero-mean
```

### 3. NLM Agent Configuration Tool

**Purpose**: Configure agent behavior and neuromodulation strategies.

```bash
nlm-agent-config [OPTIONS] CONFIG_FILE
```

**Options**:

| Option | Description | Default |
|--------|-------------|---------|
| `--behavior-mode MODE` | Set behavior mode (reactive, reflective, proactive, adaptive, autonomous) | adaptive |
| `--motor-strategy STRATEGY` | Set motor command strategy (conservative, balanced, aggressive, adaptive, statistical) | balanced |
| `--exploration-factor F` | Set exploration factor (0.0 - 2.0) | 1.0 |
| `--curiosity-threshold T` | Set curiosity threshold (0.0 - 1.0) | 0.5 |
| `--enable-subsystem SUBSYSTEM` | Enable subsystem (reward, structural, development, curiosity) | reward |
| `--disable-subsystem SUBSYSTEM` | Disable subsystem | reward |
| `--neuromodulation-config CONFIG` | Neuromodulation configuration (JSON string) | {} |
| `--plasticity-config CONFIG` | Plasticity configuration (JSON string) | {} |
| `--output-config OUTPUT_FILE` | Output configuration to file | |
| `--validate` | Validate configuration | false |
| `--help` | Show help information | |

**Example Usage**:

```bash
# Configure agent with adaptive behavior and high exploration
nlm-agent-config config.json \
  --behavior-mode adaptive \
  --motor-strategy adaptive \
  --exploration-factor 1.5 \
  --curiosity-threshold 0.3 \
  --enable-subsystem structural \
  --enable-subsystem development \
  --neuromodulation-config '{"dopamine": {"scale": 2.0}}' \
  --plasticity-config '{"stdp": {"learning_rate": 0.002}}' \
  --validate \
  --output-config optimized_config.json
```

### 4. NLM Performance Profiler

**Purpose**: Profile simulation performance and identify bottlenecks.

```bash
nlm-performance-profile [OPTIONS] SIMULATION_DIR
```

**Options**:

| Option | Description | Default |
|--------|-------------|---------|
| `--time-threshold T` | Time threshold for warnings (seconds) | 0.1 |
| `--memory-threshold M` | Memory threshold for warnings (MB) | 100 |
| `--profile-steps` | Profile individual steps | false |
| `--generate-report` | Generate performance report | false |
| `--report-format FORMAT` | Report format (text, html, json) | text |
| `--output-dir DIR` | Output directory for reports | reports/ |
| `--plot-performance` | Generate performance plots | false |
| `--plot-memory` | Generate memory usage plots | false |
| `--plot-signals` | Generate signal analysis plots | false |
| `--help` | Show help information | |

**Example Usage**:

```bash
# Profile simulation performance with comprehensive analysis
nlm-performance-profile simulation_results/ \
  --time-threshold 0.05 \
  --memory-threshold 50 \
  --profile-steps \
  --generate-report \
  --report-format html \
  --output-dir analysis_reports \
  --plot-performance \
  --plot-memory \
  --plot-signals
```

### 5. NLM State Management Tool

**Purpose**: Manage agent states, checkpoints, and recovery.

```bash
nlm-state-manager [OPTIONS] STATE_DIR
```

**Options**:

| Option | Description | Default |
|--------|-------------|---------|
| `--action ACTION` | Action to perform (save, load, validate, list, delete) | save |
| `--state-file FILE` | State file name | current_state.bin |
| `--checkpoint-interval I` | Checkpoint save interval (for save action) | 100 |
| `--compress` | Compress state files | false |
| `--encrypt` | Encrypt state files | false |
| `--password FILE` | Password file for encryption | |  
| `--validate` | Validate state integrity | false |
| `--list` | List available states | false |
| `--delete FILE` | Delete specified state file | |
| `--cleanup` | Clean up old state files | false |
| `--export-format FORMAT` | Export format (json, xml, txt) | json |
| `--import-format FORMAT` | Import format (json, xml, txt) | json |
| `--help` | Show help information | |

**Example Usage**:

```bash
# Save current state with checkpoints
nlm-state-manager simulation_state/ \
  --action save \
  --checkpoint-interval 200 \
  --compress \
  --encrypt \
  --password password.txt

# Load previous state
nlm-state-manager simulation_state/ \
  --action load \
  --state-file checkpoint_step_500.bin

# List and validate available states
nlm-state-manager simulation_state/ \
  --action list \
  --validate
```

### 6. NLM Simulation Comparison Tool

**Purpose**: Compare different simulation runs and configurations.

```bash
nlm-simulation-compare [OPTIONS] RUN1_DIR RUN2_DIR
```

**Options**:

| Option | Description | Default |
|--------|-------------|---------|
| `--metrics METRICS` | Metrics to compare (spike_rate, firing_rate, curiosity, novelty, development) | all |
| `--normalize` | Normalize metrics for comparison | false |
| `--generate-diff` | Generate difference report | false |
| `--report-format FORMAT` | Report format (text, csv, json) | text |
| `--output-file FILE` | Output file for comparison report | comparison_report.txt |
| `--threshold PERCENT` | Threshold for significant differences | 5.0 |
| `--statistical-test TEST` | Statistical test method (t-test, mann-whitney, chi-square) | t-test |
| `--confidence-level C` | Confidence level for statistical tests | 0.95 |
| `--help` | Show help information | |

**Example Usage**:

```bash
# Compare two simulation runs
nlm-simulation-compare run_v1/ run_v2/ \
  --metrics spike_rate,curiosity,development \
  --normalize \
  --generate-diff \
  --report-format csv \
  --output-file comparison_results.csv \
  --threshold 10.0 \
  --statistical-test mann-whitney \
  --confidence-level 0.99
```

### 7. NLM Batch Processing Tool

**Purpose**: Process multiple simulation runs in batch mode.

```bash
nlm-batch-process [OPTIONS] CONFIG_DIR
```

**Options**:

| Option | Description | Default |
|--------|-------------|---------|
| `--repetitions N` | Number of repetitions for each configuration | 5 |
| `--parallel JOBS` | Number of parallel jobs | 1 |
| `--output-dir DIR` | Output directory | batch_results/ |
| `--continue-on-error` | Continue processing even if some runs fail | false |
| `--save-individual-results` | Save results for each individual run | true |
| `--aggregate-results` | Aggregate results across runs | true |
| `--generate-summary` | Generate summary report | true |
| `--monitoring-interval I` | Monitoring interval (seconds) | 30 |
| `--timeout SECONDS` | Timeout for each run (seconds) | 3600 |
| `--help` | Show help information | |

**Example Usage**:

```bash
# Run batch processing with 3 parallel jobs
nlm-batch-process configurations/ \
  --repetitions 10 \
  --parallel 3 \
  --output-dir batch_experiments/ \
  --continue-on-error \
  --monitoring-interval 60 \
  --timeout 1800
```

### 8. NLM Diagnostic Tool

**Purpose**: Run diagnostic tests and identify system issues.

```bash
nlm-diagnostic [OPTIONS] DIAGNOSTIC_TYPE
```

**Options**:

| Option | Description | Default |
|--------|-------------|---------|
| `--type TYPE` | Diagnostic type (system, neural, agent, world, signal) | all |
| `--depth LEVEL` | Diagnostic depth (shallow, moderate, deep) | moderate |
| `--output-format FORMAT` | Output format (text, json, xml) | text |
| `--output-file FILE` | Output file for diagnostic results | diagnostic_results.txt |
| `--verbose` | Enable verbose diagnostic output | false |
| `--auto-fix` | Attempt automatic fixes for identified issues | false |
| `--create-report` | Generate comprehensive diagnostic report | false |
| `--help` | Show help information | |

**Example Usage**:

```bash
# Run comprehensive diagnostic
nlm-diagnostic system \
  --type system,neural,agent,world,signal \
  --depth deep \
  --output-format json \
  --output-file diagnostics.json \
  --verbose \
  --auto-fix \
  --create-report
```

## Installation and Setup

### Prerequisites

```bash
# Install required dependencies
apt-get update
apt-get install -y build-essential cmake git python3 python3-pip

# Install Python dependencies
pip install pybind11 numpy matplotlib pandas scipy
```

### Building from Source

```bash
# Clone the NLM repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python3)

# Build the project
make -j$(nproc)

# Install Python bindings
pip install -e .
```

### Installing Command Tools

```bash
# Install advanced command-line tools
cd nlm
pip install -e .[cli-tools]

# The following tools should now be available in your PATH:
# - nlm-advanced-run
# - nlm-signal-process
# - nlm-agent-config
# - nlm-performance-profile
# - nlm-state-manager
# - nlm-simulation-compare
# - nlm-batch-process
# - nlm-diagnostic
```

## Configuration Files

### Default Configuration Template

Create a file named `nlm_config.json` in your working directory:

```json
{
  "simulation": {
    "default_steps": 1000,
    "default_world_width": 20,
    "default_world_height": 20,
    "default_vision_width": 8,
    "default_vision_height": 8,
    "default_neuron_count": 1000,
    "default_synapse_density": 0.1
  },
  "performance": {
    "default_time_threshold": 0.1,
    "default_memory_threshold": 100,
    "enable_profiling": true,
    "generate_plots": false
  },
  "signal_processing": {
    "default_filter_type": "moving_average",
    "default_filter_size": 5,
    "default_add_noise": 0.0,
    "enable_normalization": true,
    "enable_zero_mean": false
  },
  "state_management": {
    "default_checkpoint_interval": 100,
    "enable_compression": false,
    "enable_encryption": false,
    "backup_count": 5
  }
}
```

### Tool Configuration

Create a directory named `nlm_tools/` in your working directory with the following files:

#### `tool_config.json`

```json
{
  "advanced_simulation": {
    "default_config": "nlm_config.json",
    "default_output_dir": "simulation_results/",
    "default_checkpoint_interval": 300
  },
  "signal_processing": {
    "default_output_format": "json",
    "default_filter_size": 7
  },
  "performance_profiling": {
    "default_time_threshold": 0.05,
    "default_memory_threshold": 50,
    "enable_plot_generation": true
  },
  "batch_processing": {
    "default_repetitions": 5,
    "default_parallel_jobs": 2,
    "default_continue_on_error": true
  }
}
```

## Usage Examples

### Example 1: Running Advanced Simulation

```bash
#!/bin/bash
# advanced_simulation.sh

# Set working directory
cd /path/to/nlm/workspace

# Run advanced simulation with comprehensive monitoring
./nlm-advanced-run nlm_config.json \
  --steps 3000 \
  --world-width 40 \
  --world-height 40 \
  --vision-width 16 \
  --vision-height 16 \
  --neuron-count 3000 \
  --synapse-density 0.2 \
  --enable-plasticity \
  --enable-neuromod \
  --enable-curiosity \
  --enable-development \
  --checkpoint-interval 500 \
  --performance-log perf_data.json \
  --signal-log signal_data.json \
  --verbose

# Wait for completion
echo "Simulation completed!"
cat performance_log.json | jq '.total_time'
```

### Example 2: Signal Processing Pipeline

```bash
#!/bin/bash
# signal_processing_pipeline.sh

# Create processing directory
mkdir -p processed_signals

# Process all input files
for input_file in raw_signals/*.json; do
    filename=$(basename "$input_file" .json)
    output_file="processed_signals/${filename}_processed.json"
    
    # Apply moving average filter
    ./nlm-signal-process "$input_file" "$output_file" \
      --signal-type vision \
      --filter-type moving_average \
      --filter-size 7 \
      --normalize \
      --output-format json
    
    echo "Processed $filename -> $output_file"
done

# Generate processing report
./nlm-performance-profile processed_signals/ \
  --generate-report \
  --report-format html \
  --output-dir analysis_reports \
  --plot-signals
```

### Example 3: Batch Configuration Experiment

```bash
#!/bin/bash
# batch_experiment.sh

# Create batch processing directory
mkdir -p batch_experiments

# Run batch processing with different configurations
./nlm-batch-process configurations/ \
  --repetitions 20 \
  --parallel 4 \
  --output-dir batch_experiments/ \
  --continue-on-error \
  --generate-summary \
  --monitoring-interval 120

# Analyze batch results
./nlm-simulation-compare \
  batch_experiments/run_1/ \
  batch_experiments/run_2/ \
  --metrics spike_rate,curiosity,development \
  --normalize \
  --generate-diff \
  --report-format csv \
  --output-file batch_comparison.csv
```

### Example 4: Performance Optimization

```bash
#!/bin/bash
# optimize_performance.sh

# Profile current performance
./nlm-performance-profile current_simulation/ \
  --time-threshold 0.05 \
  --memory-threshold 50 \
  --profile-steps \
  --generate-report \
  --report-format html

# Generate optimization recommendations
python3 optimize_recommendations.py performance_report.html > optimization_plan.txt

# Apply optimizations
echo "Applying optimization plan..."
# (Manual steps would be needed here based on the generated plan)
```

## API Reference

### Command Line Interface

All tools follow a consistent interface pattern:

```bash
# Basic usage with help
tool-name --help

# Basic usage with positional arguments
tool-name input_file output_file

# Advanced usage with options
tool-name input_file output_file --option1 value1 --option2 value2

# Help for specific options
tool-name --help | grep option1
```

### Configuration Files

All tools support JSON configuration files with the following structure:

```json
{
  "tool_name": {
    "parameter1": value1,
    "parameter2": value2,
    "nested_config": {
      "sub_parameter1": "value"
    }
  }
}
```

### Output Formats

Tools support multiple output formats:

1. **Text**: Human-readable console output
2. **JSON**: Structured data for programmatic access
3. **CSV**: Comma-separated values for spreadsheet analysis
4. **HTML**: Formatted reports for web viewing

## Troubleshooting

### Common Issues and Solutions

#### Issue: Command not found
**Solution**: Install the tools using the installation instructions above.

#### Issue: Configuration file not found
**Solution**: Create the configuration file or specify the correct path:

```bash
nlm-advanced-run /path/to/config.json
```

#### Issue: Simulation takes too long
**Solution**: Reduce simulation parameters:

```bash
nlm-advanced-run config.json --steps 500 --neuron-count 500
```

#### Issue: Signal processing fails
**Solution**: Check input file format and signal types:

```bash
# Validate signal file format
python3 validate_signals.py input_file.json
```

#### Issue: Performance profiling fails
**Solution**: Ensure dependencies are installed:

```bash
pip install matplotlib scipy pandas
```

### Getting Help

For additional help, refer to:

1. **Command-line help**: `tool-name --help`
2. **Documentation**: See the `nlm_tools/` directory for tool-specific documentation
3. **Installation guide**: See the installation section in this document
4. **Troubleshooting guide**: See the troubleshooting section in this document

## Conclusion

The NLM advanced command-line tools provide comprehensive capabilities for:

1. **Advanced Simulation Control**
   - Real-time monitoring and control
   - Checkpoint-based state management
   - Performance optimization and profiling

2. **Signal Processing**
   - Advanced filtering and normalization
   - Signal analysis and statistics
   - Batch processing capabilities

3. **Agent Configuration**
   - Behavior mode control
   - Neuromodulation strategy management
   - Subsystem enable/disable

4. **Performance Analysis**
   - Comprehensive profiling
   - Bottleneck identification
   - Comparative analysis

5. **Batch Processing**
   - Parallel execution
   - Result aggregation
   - Summary generation

These tools make the enhanced NLM system more accessible, powerful, and suitable for both research and production applications.