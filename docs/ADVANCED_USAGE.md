# NLM Advanced Usage Guide

## Overview

NLM (Neural Learning Machine) is a sophisticated neural simulation framework that implements an experimental artificial developmental brain. This guide provides advanced usage patterns, configuration options, and best practices for extending the system.

## Table of Contents

1. [Getting Started with Advanced Features](#getting-started-with-advanced-features)
2. [Configuration Management](#configuration-management)
3. [Advanced Command Line Interface](#advanced-command-line-interface)
4. [Extending NLM](#extending-nlm)
5. [Performance Optimization](#performance-optimization)
6. [Debugging and Monitoring](#debugging-and-monitoring)
7. [Batch Processing and Automation](#batch-processing-and-automation)
8. [Examples](#examples)

## 1. Getting Started with Advanced Features

### 1.1 Adding Custom Configuration Profiles

NLM supports multiple configuration profiles for different use cases:

```bash
# Create a custom configuration profile
cat > custom_config.cfg << EOF
random_seed = 12345
simulation_timestep = 0.0005
neuron_count = 2000
region_count = 3
connection_probability = 0.12
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.018
dopamine_baseline = 0.2
checkpoint_dir = ./my_checkpoints
log_level = DEBUG
log_to_file = true
log_filename = my_debug.log
EOF

# Run with custom profile
./nlm --config custom_config.cfg --debug --verbose
```

### 1.2 Running Multiple Experiments

```bash
#!/bin/bash
# Run a series of experiments with different parameters
for seed in 42 123 456 789 999; do
    echo "Running experiment with seed $seed"
    ./nlm --config baseline.cfg --debug --results-file results_seed_$seed.txt
    ./nlm_phase6_demo --config baseline.cfg --benchmark
    ./nlm_phase6_text_demo --config baseline.cfg
    echo "Completed seed $seed"
done
```

## 2. Configuration Management

### 2. باك Command Line Arguments

Advanced configuration options:

| Option | Description | Default | Example |
|--------|-------------|---------|---------|
| `--config <file>` | Load configuration from file | `configs/default.cfg` | `--config production.cfg` |
| `--debug` | Enable debug logging | `false` | `--debug` |
| `--verbose` | Enable verbose output | `false` | `-v` |
| `--benchmark` | Run in benchmark mode | `false` | `--benchmark` |
| `--save-results` | Save results to file | `false` | `--save-results` |
| `--output-format <format>` | Set output format | `text` | `--output-format json` |
| `--results-file <file>` | Set output file path | `nlm_results.txt` | `--results-file experiment_results.csv` |
| `--interactive` | Enable interactive mode | `false` | `--interactive` |
| `--help` | Show help message | N/A | `--help` |

### 2.2 Configuration File Format

The default configuration file format:

```ini
# NLM Configuration File
# Custom configuration for advanced usage

# Random seed for reproducible experiments
random_seed = 12345

# Simulation timestep in seconds
simulation_timestep = 0.0005

# Number of neurons in the brain
neuron_count = 2000

# Number of brain regions
region_count = 3

# Connection probability for random connectivity
connection_probability = 0.12

# Plasticity settings
plasticity_learning_rate = 0.015
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.018
stdp_time_constant = 25.0

# Development settings
development_synaptogenesis_rate = 0.002
development_pruning_rate = 0.0002

# Neuromodulation settings
dopamine_baseline = 0.2
reward_discount_factor = 0.995

# Environment settings
environment_name = GridWorld
environment_width = 20
environment_height = 20

# Logging settings
log_level = DEBUG
log_to_file = true
log_filename = nlm_advanced.log

# Simulation settings
max_simulation_steps = 50000
simulation_time_limit = 100.0

# Visualization settings
visualization_enabled = true
visualization_update_rate = 60.0
```

### 2.3 Configuration Templates

Create configuration templates for common use cases:

**Template: Fast Testing**
```ini
# Fast testing configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.1
max_simulation_steps = 1000
```

**Template: Production Research**
```ini
# Production configuration for research
random_seed = 12345
simulation_timestep = 0.002
neuron_count = 5000
region_count =  5n connection_probability = 0.15
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.02
dopamine_baseline = 0.0
reward_discount_factor = 0.99
checkpoint_dir = ./research_checkpoints
log_level = INFO
log_to_file = true
log_filename = production.log
visualization_enabled = false
```

## 3. Advanced Command Line Interface

### 3.1 Batch Processing

Run multiple experiments with different configurations:

```bash
#!/bin/bash
# Process configuration files from a directory
for config in configs/*.cfg; do
    echo "Processing $config"
    ./nlm --config "$config" --benchmark --output-format json --results-file "results_${config}.json"
done
```

###  scoopiorning Multiple Demos

Run all Phase 6 demo executables:

```bash
#!/bin/bash
# Run all advanced Phase 6 demos
./nlm_phase6_demo --benchmark --verbose
./nlm_phase6_text_demo --interactive --save-results --results-file text_results.txt
./nlm_benchmark --help  # Show benchmark options
./nlm --help  # Show main demo options
```

### 3.3 Custom Script Examples

**Experiment Runner Script:**
```bash
#!/bin/bash
# experiment_runner.sh - Run and analyze NLM experiments

EXPERIMENT_DIR="experiments"
RESULTS_DIR="results"
mkdir -p "$RESULTS_DIR"

for param in 0.1 0.2 0.3 0.4 0.5; do
    config="$EXPERIMENT_DIR/exp_${param}.cfg"
    results="$RESULTS_DIR/exp_${param}_results.json"
    
    echo "Running experiment with parameter $param"
    ./nlm --config "$config" --verbose --save-results --results-file "$results"
    
    # Analyze results
    python analyze_results.py "$results"
    echo "Completed parameter $param"
done

echo "All experiments completed!"
```

## 4. Extending NLM

### 4.1 Creating Custom Neurons

Extend the neuron system with custom behaviors:

```cpp
// custom_neuron.cpp
#include "brain/Neuron.hpp"

class CustomNeuron : public Neuron {
public:
    CustomNeuron(NeuronId id) : Neuron(id) {}
    
    // Override step behavior for custom dynamics
    bool step(Timestamp currentTime) override {
        // Custom integrate-and-fire dynamics
        return stepCustomLIF(currentTime);
    }
    
    // Custom plasticity rules
    void applyCustomPlasticity() {
        // Implement custom Hebbian or STDP variants
    }
    
private:
    bool stepCustomLIF(Timestamp currentTime) {
        // Your custom neuron dynamics
        // ...
    }
};
```

### 4.2 Adding Custom Brain Regions

Create specialized brain regions:

```cpp
// custom_region.cpp
#include "brain/NeuralRegion.hpp"

class CustomRegion : public NeuralRegion {
public:
    CustomRegion(RegionId id) : NeuralRegion(id) {}
    
    // Override connectivity initialization
    void initializeCustomConnectivity(RandomGenerator& rng) {
        // Custom connectivity patterns
        // ...
    }
    
    // Add specialized neuron types
    void addSpecializedNeurons() {
        // Add region-specific neuron types
        // ...
    }
};
```

### 4.3 Integrating Custom Systems

Add new neuromodulation or memory systems:

```cpp
// custom_sensory.cpp
#include "sensory/SensoryInput.hpp"

class CustomSensoryInput : public SensoryInput {
public:
    CustomSensoryInput(const std::string& name) : SensoryInput(name) {}
    
    // Implement custom sensory processing
    void processInput(const EnvironmentState& state) override {
        // Your custom sensory processing
        // ...
    }
};
```

## 5. Performance Optimization

### 5.1 Parallel Processing

Leverage multi-core systems:

```bash
# Configure for parallel processing
cat > parallel_config.cfg << EOF
# Enable SIMD vectorization
simd_enabled = true
vectorization_factor = 4

# Configure for multi-threading
thread_count = 8
use_openmp = true

# Optimize memory pools
memory_pool_size = 1024
pool_alignment = 64
EOF
```

### 5.2 Memory Optimization

Configure memory-efficient settings:

```ini
# Memory optimization settings
checkpoint_compression = LZ4
memory_pool_size = 2048
use_memory_mapping = true
prefetch_neuron_states = true
disable_visualization = true
```

### 5.3 Batch Processing Optimization

Process large batches efficiently:

```bash
#!/bin/bash
# Batch processing with progress monitoring
process_batch() {
    local batch_size=$1
    local output_prefix=$2
    
    for ((i=0; i<batch_size; i++)); do
        echo "Processing batch item $((i+1))/$batch_size"
        ./nlm --config batch_config.cfg --results-file "${output_prefix}_${i}.json"
        
        # Progress reporting
        local progress=$(( (i * 100) / batch_size ))
        echo "Progress: $progress%"
    done
}

# Run batch processing
process_batch 50 "experiment_batch"
```

## 6. Debugging and Monitoring

### 6.1 Debug Configuration

Enable comprehensive debugging:

```bash
# Comprehensive debugging
cat > debug_config.cfg << EOF
random_seed = 12345
log_level = DEBUG
log_to_file = true
log_filename = comprehensive_debug.log
visualization_enabled = true
checkpoint_dir = ./debug_checkpoints

# Enable performance monitoring
perf_monitor = true
perf_output_interval = 1000
memory_monitor = true

cpu_profile = true
gprof_enabled = true
EOF

./nlm --config debug_config.cfg --debug --verbose
```

### 6.2 Performance Profiling

Profile performance with detailed metrics:

```bash
#!/bin/bash
# Performance profiling script

# Run with performance monitoring
./nlm --benchmark --verbose --output-format json > perf_results.json

# Generate performance report
python generate_perf_report.py perf_results.json > performance_report.html

# Analyze memory usage
valgrind --tool=massif ./nlm --config production.cfg > massif.out 2>&1
ms_strip massif.out

# Analyze CPU usage
./nlm --config production.cfg 2>&1 | grep -E "CPU|Time|Memory" > resource_usage.txt
```

## 7. Batch Processing and Automation

### 7.1 Automated Experiment Pipeline

Complete automated experiment pipeline:

```bash
#!/bin/bash
# automated_pipeline.sh - Complete experiment automation

PIPELINE_CONFIG="pipeline.cfg"
EXPERIMENT_COUNT=0
START_TIME=$(date +%s)

# Initialize pipeline
./nlm --config "$PIPELINE_CONFIG" --verbose

# Run experiment batches
for batch in pre_baseline pre_stdp pre_memory pre_prediction pre_planning pre_development; do
    echo "Running batch: $batch"
    ./nlm --config "configs/${batch}.cfg" --benchmark --save-results
    EXPERIMENT_COUNT=$((EXPERIMENT_COUNT + 1))
done

# Generate final report
./generate_final_report.py --results-dir results.json --output final_analysis.pdf

END_TIME=$(date +%s)
DURATION=$((END_TIME - START_TIME))

echo "Pipeline completed in $DURATION seconds"
echo "Total experiments: $EXPERIMENT_COUNT"
```

### 7.2 Continuous Integration

Set up continuous integration for automated testing:

```bash
#!/bin/bash
# ci_test.sh - Continuous integration test runner

# Test configurations
./nlm --config configs/ci_config.cfg --verbose

# Run performance benchmarks
./nlm_benchmark --help

# Validate output formats
./nlm --config configs/ci_config.cfg --output-format json > test_output.json
python validate_output.py test_output.json

# Integration with CI system
echo "All CI tests passed"
```

## 8. Examples

### 8.1 Custom Configuration Examples

**Example: High-Fidelity Research Configuration**
```bash
cat > research_config.cfg << EOF
# High-fidelity research configuration
random_seed = 123456
simulation_timestep = 0.0001
neuron_count = 10000
region_count = 6
connection_probability = 0.18
plasticity_learning_rate = 0.03
stdp_ltp_weight = 0.028
stdp_ltd_weight = 0.028
dopamine_baseline = 0.15
reward_discount_factor = 0.999
checkpoint_dir = ./high_fidelity_checkpoints
log_level = INFO
log_to_file = true
log_filename = research.log
visualization_enabled = true
EOF
```

**Example: Rapid Prototyping Configuration**
```bash
cat > prototype_config.cfg << EOF
# Quick prototyping configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 200
region_count = 1
connection_probability = 0.05
max_simulation_steps = 2000
log_level = WARNING
log_to_file = false
EOF
```

### 8.2 Script Template Examples

**Example: Configuration Generator**
```bash
#!/bin/bash
# gen_config.sh - Generate custom configuration

PARAMS_FILE="params.txt"
OUTPUT_FILE="custom.cfg"

echo "Generating custom configuration..."
echo "# Custom NLM Configuration" > "$OUTPUT_FILE"
echo "# Generated by NLM Configuration Generator" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Read parameters from file
while read -r line; do
    if [[ $line =~ ^([^=]+)=(.*)$ ]]; then
        key=${BASH_REMATCH[1]}
        value=${BASH_REMATCH[2]}
        echo "$key = $value" >> "$OUTPUT_FILE"
    fi
done < "$PARAMS_FILE"

echo "Configuration generated: $OUTPUT_FILE"
```

**Example: Result Analyzer**
```python
#!/usr/bin/env python3
# analyze_results.py - Analyze NLM experiment results

import json
import sys
import matplotlib.pyplot as plt

def analyze_results(results_file):
    with open(results_file, 'r') as f:
        data = json.load(f)
    
    # Extract key metrics
    metrics = {
        'total_spikes': data.get('total_spikes', 0),
        'avg_firing_rate': data.get('avg_firing_rate', 0.0),
        'learning_effectiveness': data.get('learning_effectiveness', 0.0),
        'plasticity_strength': data.get('plasticity_strength', 0.0)
    }
    
    print(f"Analysis Results:")
    print(f"  Total Spikes: {metrics['total_spikes']}")
    print(f"  Average Firing Rate: {metrics['avg_firing_rate']} Hz")
    print(f"  Learning Effectiveness: {metrics['learning_effectiveness']:.3f}")
    print(f"  Plasticity Strength: {metrics['plasticity_strength']:.3f}")
    
    # Generate visualizations
    generate_visualizations(data)

def generate_visualizations(data):
    # Create learning curves, spike histograms, etc.
    # ...
    pass

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python analyze_results.py <results_file>")
        sys.exit(1)
    
    analyze_results(sys.argv[1])
```

## Conclusion

The NLM framework provides extensive capabilities for neural simulation research and development. By leveraging the advanced configuration options, command-line interface features, and extension mechanisms, researchers can:

1. **Run complex experiments** with custom configurations
2. **Analyze results** using built-in and custom analysis tools
3. **Optimize performance** through parallel processing and memory management
4. **Debug and monitor** system behavior effectively
5. **Extend functionality** with custom components
6. **Automate workflows** for large-scale research

The examples and templates provided in this guide serve as starting points for developing sophisticated neural simulation experiments and research workflows. The flexibility of the NLM architecture allows for continuous improvement and adaptation to specific research requirements.

For the most up-to-date information and additional examples, refer to the NLM documentation and community resources.
