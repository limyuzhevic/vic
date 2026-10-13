# NLM Command Line Interface - Complete User Guide

## Overview

This guide provides comprehensive documentation for using the NLM (Neural Learning Machine) command line interface, including all available commands, options, and usage patterns.

## Quick Start

### Basic Usage

```bash
# Show help
nlm --help

# Run basic simulation
nlm --experiment basic --steps 1000

# Run plasticity learning experiment
nlm --experiment plasticity --neurons 2000 --steps 2000

# Run STDP verification
nlm --experiment stdp --steps 500

# Run complete test suite
nlm --experiment all
```

### Configuration Examples

```bash
# Run with custom configuration file
nlm --config myconfig.cfg

# Override configuration with command line
nlm --experiment basic --steps 2000 --neurons 1500 --regions 3

# Enable/disable specific features
nlm --enable-stdp=false --enable-hebbian=true

# Set random seed for reproducibility
nlm --random-seed 12345

# Save brain state to checkpoint
nlm --experiment all --checkpoint brain_state.nlm

# Enable verbose logging
nlm --verbose

# Set custom log level
nlm --log-level DEBUG

# Log to specific file
nlm --log-file simulation.log
```

## Command Line Options

### Basic Options

| Option | Short Form | Description |
|--------|------------|-------------|
| `--help`, `-h` | -h | Show help message and exit |
| `--version` | | Show version information |

### Experiment Options

| Option | Description |
|--------|-------------|
| `--experiment <type>` | Run specific experiment (basic|plasticity|stdp|all) |
| `--steps <n>` | Number of simulation steps (default: 1000) |
| `--neurons <n>` | Number of neurons (default: 500) |
| `--regions <n>` | Number of regions (default: 1) |

### Plasticity Options

| Option | Description |
|--------|-------------|
| `--enable-stdp` | Enable STDP plasticity (default: enabled) |
| `--disable-stdp` | Disable STDP plasticity |
| `--enable-hebbian` | Enable Hebbian plasticity (default: enabled) |
| `--disable-hebbian` | Disable Hebbian plasticity |
| `--enable-structural` | Enable structural plasticity (default: enabled) |
| `--disable-structural` | Disable structural plasticity |

### Logging Options

| Option | Description |
|--------|-------------|
| `--log-level <level>` | Logging level: TRACE, DEBUG, INFO, WARN, ERROR |
| `--log-file <file>` | Log output to file |
| `--verbose`, `-v` | Enable verbose logging |
| `--quiet`, `-q` | Disable all logging except errors |

### Advanced Options

| Option | Description |
|--------|-------------|
| `--random-seed <seed>` | Random seed for reproducibility |
| `--timestep <ms>` | Simulation timestep in milliseconds |
| `--connection-prob <p>` | Connection probability between neurons |
| `--checkpoint <file>` | Save brain state to file after experiment |
| `--restore <file>` | Restore brain state from file |
| `--config <file>` | Load configuration from file |
| `--no-plasticity` | Disable all plasticity mechanisms |
| `--no-neuromodulation` | Disable all neuromodulation |
| `--no-memory` | Disable memory systems |

### Output Options

| Option | Description |
|--------|-------------|
| `--output <file>` | Save results to output file |
| `--output-format <format>` | Output format: json|csv|text (default: text) |
| `--show-progress` | Show progress indicators |
| `--benchmark` | Run performance benchmark |

## Experiment Types

### Basic Experiment
```bash
nlm --experiment basic --steps 1000 --neurons 500
```

**What it does:**
- Runs basic neural connectivity tests
- Tests spike propagation through the network
- Verifies neuron dynamics and stability

**Output:**
- Spike counts and activity patterns
- Neural connectivity information
- Basic system status

### Plasticity Experiment
```bash
nlm --experiment plasticity --steps 2000 --neurons 1000 --enable-stdp=true --enable-hebbian=true
```

**What it does:**
- Enables all plasticity mechanisms
- Runs learning experiments with repeated input
- Measures synaptic changes and weight updates
- Demonstrates activity-dependent learning

**Key Features:**
- STDP (Spike-Timing Dependent Plasticity)
- Hebbian learning
- Structural plasticity (synaptogenesis/pruning)
- Learning statistics and analysis

### STDP Verification
```bash
nlm --experiment stdp --steps 500 --neurons 300 --enable-stdp=true --enable-hebbian=false
```

**What it does:**
- Focuses specifically on STDP mechanisms
- Tests pre-post synaptic timing correlations
- Verifies potentiation and depression effects
- Validates STDP learning rules

**Key Features:**
- Controlled spike timing patterns
- Measurement of synaptic weight changes
- LTP/LTD verification
- Statistical significance testing

### All Experiments
```bash
nlm --experiment all --steps 3000 --neurons 800 --checkpoint brain_state.nlm
```

**What it does:**
- Runs the complete test suite
- Includes all experiment types
- Comprehensive system validation
- Integration testing

## Configuration Files

### Configuration File Format

Configuration files use INI format:

```ini
# NLM Configuration Example
# ==================================

# Random seed for reproducible experiments
random_seed = 42

# Simulation settings
simulation_timestep = 0.001
max_simulation_steps = 10000
checkpoint_interval = 1000

# Brain configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.15

# Plasticity configuration
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
stdp_time_constant = 20.0
hebbian_enabled = true
stdp_enabled = true
structural_plasticity_enabled = true

# Development configuration
development_stage = 1
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation configuration
dopamine_baseline = 0.1
curiosity_baseline = 0.0
reward_discount_factor = 0.99

# Memory configuration
working_memory_capacity = 100
episodic_memory_capacity = 50

# Environment configuration
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging configuration
log_level = INFO
log_to_file = true
log_filename = nlm.log
log_max_size = 10485760

# Visualization configuration
visualization_enabled = false
visualization_update_rate = 30.0
```

### Configuration File Commands

```bash
# Create configuration file
nlm --config myconfig.cfg

# Show current configuration
nlm --show-config

# Export configuration
nlm --export-config exported_config.cfg

# Import configuration
nlm --import-config custom_config.cfg

# Validate configuration
nlm --validate-config myconfig.cfg
```

## Scripting Examples

### Shell Script Example

```bash
#!/bin/bash
# run_nlm_experiment.sh

# Configuration
EXPERIMENT_TYPE="plasticity"
NEURON_COUNT=1000
STEPS=2000
RANDOM_SEED=12345
OUTPUT_FILE="results.txt"

# Run experiment
nlm \
  --experiment "$EXPERIMENT_TYPE" \
  --steps "$STEPS" \
  --neurons "$NEURON_COUNT" \
  --random-seed "$RANDOM_SEED" \
  --enable-stdp=true \
  --enable-hebbian=true \
  --enable-structural=true \
  --checkpoint "brain_checkpoint.nlm" \
  --log-level INFO \
  --output "$OUTPUT_FILE"

# Check result
if [ $? -eq 0 ]; then
    echo "Experiment completed successfully"
    echo "Results saved to $OUTPUT_FILE"
else
    echo "Experiment failed"
    exit 1
fi
```

### Python Script Example

```python
#!/usr/bin/env python3
"""
NLM Python Wrapper Script

This script provides a Python interface to NLM command line functionality,
allowing for programmatic control and automation of NLM experiments.
"""

import subprocess
import json
import time
from pathlib import Path
class NLMExperiment:
    """NLM experiment runner"""
    
    def __init__(self, config_file=None):
        self.config_file = config_file
        self.results = {}
    
    def run_experiment(self, **kwargs):
        """Run NLM experiment with given parameters"""
        # Build command
        cmd = ["nlm"]
        
        if self.config_file:
            cmd.extend(["--config", self.config_file])
        
        # Add parameters
        if kwargs.get('experiment'):
            cmd.extend(["--experiment", kwargs['experiment']])
        
        if kwargs.get('steps'):
            cmd.extend(["--steps", str(kwargs['steps'])])
        
        if kwargs.get('neurons'):
            cmd.extend(["--neurons", str(kwargs['neurons'])])
        
        if kwargs.get('regions'):
            cmd.extend(["--regions", str(kwargs['regions'])])
        
        if kwargs.get('random_seed'):
            cmd.extend(["--random-seed", str(kwargs['random_seed'])])
        
        if kwargs.get('enable_stdp'):
            cmd.append("--enable-stdp")
        
        if kwargs.get('enable_hebbian'):
            cmd.append("--enable-hebbian")
        
        if kwargs.get('enable_structural'):
            cmd.append("--enable-structural")
        
        if kwargs.get('verbose'):
            cmd.append("--verbose")
        
        if kwargs.get('checkpoint'):
            cmd.extend(["--checkpoint", kwargs['checkpoint']])
        
        if kwargs.get('output'):
            cmd.extend(["--output", kwargs['output']])
        
        # Run command
        print(f"Running NLM command: {' '.join(cmd)}")
        start_time = time.time()
        
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, check=True)
            end_time = time.time()
            
            self.results = {
                'command': ' '.join(cmd),
                'exit_code': result.returncode,
                'stdout': result.stdout,
                'stderr': result.stderr,
                'execution_time': end_time - start_time,
                'success': True
            }
            
            print(f"Experiment completed in {end_time - start_time:.2f} seconds")
            
        except subprocess.CalledProcessError as e:
            end_time = time.time()
            
            self.results = {
                'command': ' '.join(cmd),
                'exit_code': e.returncode,
                'stdout': e.stdout if e.stdout else '',
                'stderr': e.stderr if e.stderr else '',
                'execution_time': end_time - start_time,
                'success': False
            }
            
            print(f"Experiment failed after {end_time - start_time:.2f} seconds")
            print(f"Error: {e.stderr}")
        
        return self.results
    
    def run_multiple_experiments(self, experiment_configs):
        """Run multiple experiments with different configurations"""
        all_results = []
        
        for i, config in enumerate(experiment_configs):
            print(f"\nRunning experiment {i+1}/{len(experiment_configs)}:")
            print(f"Config: {config}")
            
            result = self.run_experiment(**config)
            all_results.append(result)
            
            # Save intermediate results
            if config.get('save_interval') and (i + 1) % config['save_interval'] == 0:
                self.save_results(f"intermediate_results_{i+1}.json")
        
        return all_results
    
    def save_results(self, filename):
        """Save results to JSON file"""
        with open(filename, 'w') as f:
            json.dump(self.results, f, indent=2)
        print(f"Results saved to {filename}")
    
    def get_summary(self):
        """Get summary of all results"""
        if not self.results:
            return "No results available"
        
        summary = {
            'success_rate': sum(1 for r in [self.results] if r['success']) / len([self.results]),
            'average_execution_time': self.results['execution_time'],
            'command': self.results['command']
        }
        
        return summary
def main():
    """Example usage"""
    # Example 1: Single experiment
    experiment = NLMExperiment()
    
    result = experiment.run_experiment(
        experiment='plasticity',
        steps=1000,
        neurons=500,
        random_seed=42,
        enable_stdp=True,
        enable_hebbian=True,
        checkpoint='brain_checkpoint.nlm',
        verbose=True
    )
    
    if result['success']:
        print("Experiment completed successfully!")
    else:
        print("Experiment failed!")
    
    # Example 2: Multiple experiments
    experiment_configs = [
        {
            'experiment': 'basic',
            'steps': 500,
            'neurons': 300,
            'random_seed': 42
        },
        {
            'experiment': 'plasticity',
            'steps': 1000,
            'neurons': 500,
            'random_seed': 123,
            'enable_stdp': True,
            'enable_hebbian': True
        },
        {
            'experiment': 'stdp',
            'steps': 300,
            'neurons': 200,
            'random_seed': 456,
            'enable_stdp': True,
            'enable_hebbian': False
        }
    ]
    
    all_results = experiment.run_multiple_experiments(experiment_configs)
    
    # Analyze results
    success_count = sum(1 for r in all_results if r['success'])
    print(f"\nSummary: {success_count}/{len(all_results)} experiments succeeded")

if __name__ == "__main__":
    main()
```

### MATLAB/Octave Script Example

```matlab
% NLM MATLAB/Octave Interface
% This script provides MATLAB/Octave access to NLM functionality

function nlm_experiment_runner()
    % Configuration
    config = {
        'neuron_count', 1000;
        'region_count', 2;
        'simulation_timestep', 0.001;
        'random_seed', 42;
    };
    
    % Run NLM command
    cmd = sprintf('nlm --experiment basic --steps 1000 --neurons %d --random-seed %d', 
                  config.neuron_count, config.random_seed);
    
    fprintf('Running command: %s\n', cmd);
    
    % Execute command
    [status, output] = system(cmd);
    
    if status == 0
        fprintf('Experiment completed successfully\n');
        fprintf('Output:\n%s\n', output);
    else
        fprintf('Experiment failed with error: %d\n', status);
        fprintf('Output:\n%s\n', output);
    end
end

function analyze_results(results_file)
    % Load and analyze NLM results
    data = load(results_file);
    
    % Plot learning curves
    figure;
    subplot(2,2,1);
    plot(data.results.timesteps, data.results.spike_counts);
    xlabel('Step'); ylabel('Spike Count'); title('Spike Activity');
    
    subplot(2,2,2);
    plot(data.results.timesteps, data.results.firing_rates);
    xlabel('Step'); ylabel('Firing Rate'); title('Firing Rate');
    
    subplot(2,2,3);
    plot(data.results.timesteps, data.results.weight_changes);
    xlabel('Step'); ylabel('Weight Change'); title('Synaptic Plasticity');
    
    subplot(2,2,4);
    histogram(data.results.learning_efficiency);
    xlabel('Learning Efficiency'); ylabel('Frequency'); title('Learning Distribution');
    
    saveas(gcf, 'nlm_results_analysis.png');
end
```

## Batch Processing

### Batch Processing Script

```bash
#!/bin/bash
# batch_nlm_processing.sh

# Batch processing configuration
CONFIG_DIR="configs/batch"
OUTPUT_DIR="results/batch"
EXPERIMENT_TYPES=("basic" "plasticity" "stdp" "all")

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Process each configuration
for config_file in "$CONFIG_DIR"/*.cfg; do
    config_name=$(basename "$config_file" .cfg)
    echo "Processing configuration: $config_name"
    
    # Create output subdirectory
    output_subdir="$OUTPUT_DIR/$config_name"
    mkdir -p "$output_subdir"
    
    # Run experiments for this configuration
    for experiment in "${EXPERIMENT_TYPES[@]}"; do
        echo "  Running $experiment experiment..."
        
        # Run experiment
        nlm \
          --config "$config_file" \
          --experiment "$experiment" \
          --steps 2000 \
          --checkpoint "$output_subdir/brain_checkpoint_${experiment}.nlm" \
          --output "$output_subdir/results_${experiment}.txt" \
          --verbose
        
        # Check result
        if [ $? -eq 0 ]; then
            echo "    ✓ $experiment completed successfully"
        else
            echo "    ✗ $experiment failed"
        fi
    done
    
    # Combine results
    echo "Combining results for $config_name..."
    python combine_results.py "$output_subdir"
    
    echo "Configuration $config_name completed"
    echo

done

echo "Batch processing completed!"
```

### Result Analysis Script

```python
#!/usr/bin/env python3
"""
NLM Batch Result Analyzer

This script analyzes and compares results from multiple NLM experiments,
providing statistical analysis and visualization.
"""

import json
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from pathlib import Path
from typing import Dict, List, Any
import sys
class NLMResultAnalyzer:
    """Analyze NLM experiment results"""
    
    def __init__(self, results_dir: str):
        self.results_dir = Path(results_dir)
        self.results = []
        self.load_results()
    
    def load_results(self):
        """Load all result files from directory"""
        for result_file in self.results_dir.glob("*.json"):
            with open(result_file, 'r') as f:
                data = json.load(f)
                self.results.append(data)
    
    def get_experiment_summary(self, experiment_name: str) -> Dict:
        """Get summary for specific experiment"""
        experiment_results = [r for r in self.results if r.get('experiment_name') == experiment_name]
        
        if not experiment_results:
            return {}
        
        # Calculate statistics
        execution_times = [r.get('execution_time', 0) for r in experiment_results]
        spike_counts = [r.get('final_spike_count', 0) for r in experiment_results]
        learning_rates = [r.get('learning_rate', 0) for r in experiment_results]
        success_rates = [r.get('success', False) for r in experiment_results]
        
        summary = {
            'experiment_name': experiment_name,
            'num_runs': len(experiment_results),
            'success_rate': np.mean(success_rates) * 100,
            'avg_execution_time': np.mean(execution_times),
            'std_execution_time': np.std(execution_times),
            'avg_spike_count': np.mean(spike_counts),
            'std_spike_count': np.std(spike_counts),
            'avg_learning_rate': np.mean(learning_rates),
            'std_learning_rate': np.std(learning_rates),
            'min_spike_count': np.min(spike_counts),
            'max_spike_count': np.max(spike_counts),
            'median_execution_time': np.median(execution_times)
        }
        
        return summary
    
    def compare_experiments(self) -> Dict:
        """Compare all experiments"""
        experiment_types = ['basic', 'plasticity', 'stdp', 'all']
        comparisons = {}
        
        for exp_type in experiment_types:
            summary = self.get_experiment_summary(exp_type)
            if summary:
                comparisons[exp_type] = summary
        
        return comparisons
    
    def create_performance_comparison(self):
        """Create performance comparison plot"""
        comparisons = self.compare_experiments()
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('NLM Experiment Performance Comparison', fontsize=16, fontweight='bold')
        
        exp_types = list(comparisons.keys())
        
        # Plot 1: Success Rate
        success_rates = [comparisons[exp]['success_rate'] for exp in exp_types]
        axes[0, 0].bar(exp_types, success_rates, color='green', alpha=0.7)
        axes[0, 0].set_ylabel('Success Rate (%)')
        axes[0, 0].set_title('Experiment Success Rate')
        axes[0, 0].set_ylim(0, 100)
        
        # Plot 2: Execution Time
        exec_times = [comparisons[exp]['avg_execution_time'] for exp in exp_types]
        axes[0, 1].bar(exp_types, exec_times, color='blue', alpha=0.7)
        axes[0, 1].set_ylabel('Average Execution Time (s)')
        axes[0, 1].set_title('Execution Time Comparison')
        
        # Plot 3: Spike Count
        spike_counts = [comparisons[exp]['avg_spike_count'] for exp in exp_types]
        axes[1, 0].bar(exp_types, spike_counts, color='red', alpha=0.7)
        axes[1, 0].set_ylabel('Average Spike Count')
        axes[1, 0].set_title('Spike Count Comparison')
        
        # Plot 4: Learning Rate
        learning_rates = [comparisons[exp]['avg_learning_rate'] for exp in exp_types]
        axes[1, 1].bar(exp_types, learning_rates, color='orange', alpha=0.7)
        axes[1, 1].set_ylabel('Average Learning Rate')
        axes[1, 1].set_title('Learning Rate Comparison')
        
        plt.tight_layout()
        
        # Save figure
        output_file = self.results_dir / 'performance_comparison.png'
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        plt.close()
        
        print(f"Performance comparison saved to: {output_file}")
    
    def generate_report(self, output_file: str = None):
        """Generate comprehensive analysis report"""
        if output_file is None:
            output_file = self.results_dir / 'analysis_report.txt'
        
        comparisons = self.compare_experiments()
        
        with open(output_file, 'w') as f:
            f.write("NLM Experiment Analysis Report\n")
            f.write("=" * 50 + "\n\n")
            
            f.write("Summary Statistics:\n")
            f.write("-" * 30 + "\n")
            
            for exp_type, data in comparisons.items():
                f.write(f"\n{exp_type.capitalize()} Experiment:\n")
                f.write(f"  Runs: {data['num_runs']}\n")
                f.write(f"  Success Rate: {data['success_rate']:.1f}%\n")
                f.write(f"  Avg Execution Time: {data['avg_execution_time']:.2f}s ± {data['std_execution_time']:.2f}s\n")
                f.write(f"  Avg Spike Count: {data['avg_spike_count']:.1f} ± {data['std_spike_count']:.1f}\n")
                f.write(f"  Avg Learning Rate: {data['avg_learning_rate']:.4f} ± {data['std_learning_rate']:.4f}\n")
                f.write(f"  Min/Max Spike Count: {data['min_spike_count']:.0f}/{data['max_spike_count']:.0f}\n")
                f.write(f"  Median Execution Time: {data['median_execution_time']:.2f}s\n")
            
            f.write("\n\nPerformance Rankings:\n")
            f.write("-" * 30 + "\n")
            
            # Rank experiments by performance score
            for exp_type, data in comparisons.items():
                # Calculate performance score (higher is better)
                score = (
                    data['success_rate'] / 100 * 0.4 +  # Success rate contribution
                    (1 / (1 + data['avg_execution_time'])) * 0.3 +  # Speed contribution
                    (data['avg_learning_rate'] / max([d['avg_learning_rate'] for d in comparisons.values()])) * 0.3  # Learning contribution
                )
                f.write(f"{exp_type.capitalize()}: Score = {score:.3f}\n")
        
        print(f"Analysis report saved to: {output_file}")
    
    def run_automatic_analysis(self):
        """Run complete automatic analysis"""
        print("Running automatic NLM analysis...")
        
        # Generate comparisons
        comparisons = self.compare_experiments()
        
        # Create performance plot
        self.create_performance_comparison()
        
        # Generate comprehensive report
        self.generate_report()
        
        print("Analysis completed successfully!")
        
        return comparisons
def main():
    """Example usage"""
    if len(sys.argv) < 2:
        print("Usage: python nlm_analyzer.py <results_directory>")
        return
    
    results_dir = sys.argv[1]
    
    if not Path(results_dir).exists():
        print(f"Error: Results directory '{results_dir}' does not exist")
        return
    
    analyzer = NLMResultAnalyzer(results_dir)
    results = analyzer.run_automatic_analysis()
    
    print("\nAnalysis Results:")
    for exp_type, data in results.items():
        print(f"\n{exp_type.capitalize()}:")
        print(f"  Success Rate: {data['success_rate']:.1f}%")
        print(f"  Avg Execution Time: {data['avg_execution_time']:.2f}s")
        print(f"  Avg Spike Count: {data['avg_spike_count']:.1f}")
        print(f"  Avg Learning Rate: {data['avg_learning_rate']:.4f}")
if __name__ == "__main__":
    main()
```

## Configuration File Examples

### Basic Configuration

```ini
# Basic NLM Configuration
[basic]
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.15
```

### Learning Configuration

```ini
# Learning-Optimized Configuration
[learning]
random_seed = 123
plasticity_learning_rate = 0.05
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.025
stdp_time_constant = 20.0
hebbian_enabled = true
stdp_enabled = true
structural_plasticity_enabled = true
development_synaptogenesis_rate = 0.002
```

### Performance Configuration

```ini
# Performance-Optimized Configuration
[performance]
random_seed = 456
simulation_timestep = 0.001
simd_enabled = true
parallel_processing_enabled = false
memory_pool_size = 2000
event_queue_size = 20000
```

### Advanced Configuration

```ini
# Advanced Configuration
[advanced]
random_seed = 789
# Neuromodulation
curiosity_baseline = 0.1
novelty_threshold = 0.5
reward_discount_factor = 0.95

# Memory systems
working_memory_capacity = 500
episodic_memory_capacity = 200
associative_memory_capacity = 1000

# Development system
development_stage = 3
development_maturation_rate = 0.001

# Agent system
agent_sensory_resolution = 128
agent_exploration_rate = 0.2
agent_exploitation_rate = 0.8

# Experiment control
experiment_type = plasticity
experiment_seed = 42
experiment_max_steps = 2000
experiment_reward_threshold = 200
```

## Configuration Management

### Configuration Template

```bash
#!/bin/bash
# create_config_template.sh

cat > nlm_config_template.ini << EOF
# NLM Configuration Template
# =============================

# Basic Settings
[general]
random_seed = 42
simulation_timestep = 0.001
log_level = INFO

# Brain Configuration
[neurons]
neuron_count = 1000
region_count = 2
connection_probability = 0.15

# Plasticity Settings
[plasticity]
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
hebbian_enabled = true
stdp_enabled = true
structural_plasticity_enabled = true

# Development Settings
[development]
development_stage = 1
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation Settings
[neuromodulation]
dopamine_baseline = 0.0
curiosity_baseline = 0.0
reward_discount_factor = 0.99

# Memory Settings
[memory]
working_memory_capacity = 100
episodic_memory_capacity = 50
associative_memory_capacity = 200

# Environment Settings
[environment]
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Visualization Settings
[visualization]
visualization_enabled = false
visualization_update_rate = 30.0

# Performance Settings
[performance]
simd_enabled = true
parallel_processing_enabled = false
sparse_connectivity_enabled = true
memory_pool_size = 1000
event_queue_size = 10000

# Agent Settings
[agent]
agent_sensory_resolution = 64
agent_motor_range = 10
agent_action_space = 4
agent_exploration_rate = 0.1
agent_exploitation_rate = 0.9

# Experiment Settings
[experiment]
experiment_type = basic
experiment_seed = 42
experiment_max_steps = 1000
experiment_reward_threshold = 100
EOF

echo "Configuration template created: nlm_config_template.ini"
```

### Configuration Validation Script

```python
#!/usr/bin/env python3
"""
NLM Configuration Validator

Validates NLM configuration files and provides recommendations.
"""

import configparser
import argparse
import sys
from pathlib import Path
class NLMConfigValidator:
    """Validate NLM configuration files"""
    
    def __init__(self, config_file: str):
        self.config_file = Path(config_file)
        self.config = configparser.ConfigParser()
        self.errors = []
        self.warnings = []
        self.recommendations = []
    
    def load_config(self) -> bool:
        """Load configuration file"""
        try:
            self.config.read(self.config_file)
            return True
        except Exception as e:
            self.errors.append(f"Failed to load configuration file: {e}")
            return False
    
    def validate_sections(self) -> bool:
        """Validate configuration sections"""
        required_sections = ['general', 'neurons', 'plasticity', 'environment']
        
        for section in required_sections:
            if not self.config.has_section(section):
                self.errors.append(f"Missing required section: {section}")
        
        return len(self.errors) == 0
    
    def validate_parameters(self) -> bool:
        """Validate parameter values"""
        validation_rules = {
            'random_seed': (1, 1000000, "Random seed should be positive"),
            'simulation_timestep': (0.0001, 0.1, "Timestep should be between 0.0001s and 0.1s"),
            'neuron_count': (10, 100000, "Neuron count should be between 10 and 100,000"),
            'region_count': (1, 1000, "Region count should be between 1 and 1,000"),
            'connection_probability': (0.0, 1.0, "Connection probability should be between 0.0 and 1.0"),
            'plasticity_learning_rate': (0.0, 1.0, "Learning rate should be between 0.0 and 1.0"),
        }
        
        for section, rules in validation_rules.items():
            if self.config.has_section(section):
                for param, (min_val, max_val, message) in rules.items():
                    if self.config.has_option(section, param):
                        try:
                            value = float(self.config.get(section, param))
                            if value < min_val or value > max_val:
                                self.errors.append(f"{section}.{param}: {message}")
                        except ValueError:
                            self.errors.append(f"{section}.{param}: Invalid numeric value")
        
        return len(self.errors) == 0
    
    def validate_dependencies(self) -> bool:
        """Validate parameter dependencies"""
        # Check timestep compatibility
        if (self.config.has_section('general') and 
            self.config.has_option('general', 'simulation_timestep')):
            timestep = float(self.config.get('general', 'simulation_timestep'))
            if timestep > 0.01:
                self.warnings.append("Large timestep (>0.01s) may reduce simulation accuracy")
        
        # Check neuron count vs region count
        if (self.config.has_section('neurons') and 
            self.config.has_option('neurons', 'neuron_count') and
            self.config.has_option('neurons', 'region_count')):
            neurons = int(self.config.get('neurons', 'neuron_count'))
            regions = int(self.config.get('neurons', 'region_count'))
            if neurons / regions > 10000:
                self.warnings.append("High neuron-to-region ratio may cause performance issues")
        
        return True
    
    def validate_compatibility(self) -> bool:
        """Validate compatibility between different settings"""
        # Check plasticity settings
        if self.config.has_section('plasticity'):
            hebbian = self.config.get('plasticity', 'hebbian_enabled', fallback='true').lower() == 'true'
            stdp = self.config.get('plasticity', 'stdp_enabled', fallback='true').lower() == 'true'
            structural = self.config.get('plasticity', 'structural_plasticity_enabled', fallback='true').lower() == 'true'
            
            if not (hebbian or stdp or structural):
                self.errors.append("At least one plasticity mechanism should be enabled")
        
        # Check environment settings
        if self.config.has_section('environment'):
            env_name = self.config.get('environment', 'environment_name', fallback='GridWorld')
            if env_name not in ['GridWorld', 'Continuous', 'Discrete']:
                self.warnings.append(f"Unknown environment name: {env_name}")
        
        return len(self.errors) == 0
    
    def validate_plasticity_consistency(self) -> bool:
        """Validate plasticity parameter consistency"""
        if not self.config.has_section('plasticity'):
            return True
        
        # Check STDP parameters
        if self.config.has_option('plasticity', 'stdp_ltp_weight'):
            ltp = float(self.config.get('plasticity', 'stdp_ltp_weight'))
            ltd = float(self.config.get('plasticity', 'stdp_ltd_weight', fallback='0.0'))
            
            if ltp <= 0 or ltd <= 0:
                self.errors.append("STDP weights should be positive")
            
            if abs(ltp - ltd) > 0.1:
                self.warnings.append("STDP LTP and LTD weights differ significantly (>0.1)")
        
        return True
    
    def validate_development_consistency(self) -> bool:
        """Validate development parameter consistency"""
        if not self.config.has_section('development'):
            return True
        
        # Check development stage
        stage = self.config.get('development', 'development_stage', fallback='1')
        try:
            stage_num = int(stage)
            if stage_num < 1 or stage_num > 10:
                self.warnings.append("Development stage should be between 1 and 10")
        except ValueError:
            self.errors.append("Invalid development stage value")
        
        return True
    
    def validate_neuromodulation_consistency(self) -> bool:
        """Validate neuromodulation parameter consistency"""
        if not self.config.has_section('neuromodulation'):
            return True
        
        # Check dopamine baseline
        dopamine = float(self.config.get('neuromodulation', 'dopamine_baseline', fallback='0.0'))
        if dopamine < 0 or dopamine > 1:
            self.warnings.append("Dopamine baseline should be between 0.0 and 1.0")
        
        # Check reward discount factor
        discount = float(self.config.get('neuromodulation', 'reward_discount_factor', fallback='0.99'))
        if discount < 0.5 or discount > 1.0:
            self.warnings.append("Reward discount factor should be between 0.5 and 1.0")
        
        return True
    
    def validate_memory_consistency(self) -> bool:
        """Validate memory parameter consistency"""
        if not self.config.has_section('memory'):
            return True
        
        # Check memory capacities
        if (self.config.has_option('memory', 'working_memory_capacity') and
            self.config.has_option('memory', 'episodic_memory_capacity') and
            self.config.has_option('memory', 'associative_memory_capacity')):
            
            working = int(self.config.get('memory', 'working_memory_capacity'))
            episodic = int(self.config.get('memory', 'episodic_memory_capacity'))
            associative = int(self.config.get('memory', 'associative_memory_capacity'))
            
            total_memory = working + episodic + associative
            if total_memory > 10000:
                self.warnings.append("Total memory capacity (>10,000) may cause memory issues")
        
        return True
    
    def validate_performance_consistency(self) -> bool:
        """Validate performance parameter consistency"""
        if not self.config.has_section('performance'):
            return True
        
        # Check SIMD compatibility
        if (self.config.has_option('performance', 'simd_enabled') and
            self.config.has_option('performance', 'parallel_processing_enabled')):
            
            simd = self.config.get('performance', 'simd_enabled', fallback='true').lower() == 'true'
            parallel = self.config.get('performance', 'parallel_processing_enabled', fallback='false').lower() == 'true'
            
            if not simd and parallel:
                self.warnings.append("Parallel processing may be less effective without SIMD")
        
        return True
    
    def validate_agent_consistency(self) -> bool:
        """Validate agent parameter consistency"""
        if not self.config.has_section('agent'):
            return True
        
        # Check action space compatibility
        if self.config.has_option('agent', 'action_space'):
            action_space = self.config.get('agent', 'action_space', fallback='4')
            try:
                actions = int(action_space)
                if actions < 1 or actions > 100:
                    self.warnings.append("Action space should be between 1 and 100")
            except ValueError:
                self.errors.append("Invalid action space value")
        
        # Check exploration-exploitation balance
        if (self.config.has_option('agent', 'agent_exploration_rate') and
            self.config.has_option('agent', 'agent_exploitation_rate')):
            
            exploration = float(self.config.get('agent', 'agent_exploration_rate'))
            exploitation = float(self.config.get('agent', 'agent_exploitation_rate'))
            
            if exploration + exploitation != 1.0:
                self.warnings.append(f"Exploration ({exploration}) + exploitation ({exploitation}) should equal 1.0")
        
        return True
    
    def validate_experiment_consistency(self) -> bool:
        """Validate experiment parameter consistency"""
        if not self.config.has_section('experiment'):
            return True
        
        # Check experiment type
        experiment_type = self.config.get('experiment', 'experiment_type', fallback='basic')
        if experiment_type not in ['basic', 'plasticity', 'stdp', 'all']:
            self.warnings.append(f"Unknown experiment type: {experiment_type}")
        
        # Check experiment parameters
        if self.config.has_option('experiment', 'experiment_max_steps'):
            steps = int(self.config.get('experiment', 'experiment_max_steps'))
            if steps < 10 or steps > 1000000:
                self.warnings.append("Experiment max steps should be between 10 and 1,000,000")
        
        return True
    
    def validate_all(self) -> bool:
        """Run all validation checks"""
        self.errors.clear()
        self.warnings.clear()
        self.recommendations.clear()
        
        if not self.load_config():
            return False
        
        if not self.validate_sections():
            return False
        
        if not self.validate_parameters():
            return False
        
        self.validate_dependencies()
        self.validate_compatibility()
        self.validate_plasticity_consistency()
        self.validate_development_consistency()
        self.validate_neuromodulation_consistency()
        self.validate_memory_consistency()
        self.validate_performance_consistency()
        self.validate_agent_consistency()
        self.validate_experiment_consistency()
        
        return len(self.errors) == 0
    
    def print_report(self):
        """Print validation report"""
        print("NLM Configuration Validation Report")
        print("=" * 50)
        
        if self.errors:
            print("\nERRORS:")
            print("-" * 20)
            for error in self.errors:
                print(f"  ✗ {error}")
        
        if self.warnings:
            print("\nWARNINGS:")
            print("-" * 20)
            for warning in self.warnings:
                print(f"  ⚠️  {warning}")
        
        if self.recommendations:
            print("\nRECOMMENDATIONS:")
            print("-" * 20)
            for recommendation in self.recommendations:
                print(f"  💡 {recommendation}")
        
        if not self.errors and not self.warnings:
            print("\n✓ Configuration is valid and optimized!")
        
        return len(self.errors) == 0
def main():
    """Main function"""
    parser = argparse.ArgumentParser(description="NLM Configuration Validator")
    parser.add_argument("config_file", help="Path to configuration file")
    parser.add_argument("-v", "--verbose", action="store_true", help="Verbose output")
    parser.add_argument("-f", "--fix", action="store_true", help="Attempt to fix common issues")
    
    args = parser.parse_args()
    
    validator = NLMConfigValidator(args.config_file)
    
    if validator.validate_all():
        print(f"\n✓ Configuration validation passed!")
        validator.print_report()
        return 0
    else:
        print(f"\n✗ Configuration validation failed!")
        validator.print_report()
        return 1
if __name__ == "__main__":
    exit(main())
```

## Conclusion

The NLM command line interface provides powerful and flexible control over neural network simulations. This guide covers:

1. **Basic Usage**: Simple commands for common tasks
2. **Advanced Options**: Detailed options for fine-tuned control
3. **Configuration Management**: Configuration file creation and validation
4. **Scripting Examples**: Examples for automation and batch processing
5. **Result Analysis**: Tools for analyzing and comparing experiments

With these comprehensive examples and documentation, users can effectively:
- Run experiments with various configurations
- Automate repetitive tasks
- Analyze and compare results
- Validate configurations
- Customize their NLM experience

The command line interface is designed to be both user-friendly for beginners and powerful for advanced users, providing the flexibility needed for research and experimentation.
