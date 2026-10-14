# NLM Advanced Command-Line Reference

This document provides comprehensive reference for the enhanced command-line interface of the NLM (Neural Learning Machine) project.

## Overview

The NLM executable now supports extensive command-line options for flexible configuration, debugging, and experimentation. This document covers all available options and provides examples for different use cases.

## Basic Usage

### Simple Execution

```bash
# Run default integration test (Phase 2)
./nlm

# Help and version information
./nlm --help
./nlm --version
```

### Running Different Test Modes

```bash
# Test basic neural connectivity
./nlm --mode basic

# Test learning and plasticity
./nlm --mode plasticity

# Test only STDP learning
./nlm --mode stdp

# Test only Hebbian learning  
./nlm --mode hebbian

# Run complete integration test (default)
./nlm --mode full_test
```

## Configuration Options

### Core Configuration

| Option | Description | Default | Example |
|--------|-------------|---------|---------|
| `--mode` | Execution mode | `full_test` | `--mode basic` |
| `--config` | Configuration file path | `configs/default.cfg` | `--config my_config.cfg` |
| `--neurons` | Number of neurons | `500` | `--neurons 1000` |
| `--steps` | Simulation steps | `100` | `--steps 500` |
| `--timestep` | Simulation timestep | `0.001` | `--timestep 0.0005` |
| `--seed` | Random seed | `42` | `--seed 12345` |

### Brain Configuration

```bash
# Configure brain parameters
./nlm --neurons 2000 --timestep 0.001

# Set specific brain properties
./nlm --neurons 1500 --steps 200 --config custom_config.cfg
```

### Performance and Output

```bash
# Verbose logging
./nlm --verbose

# Quiet mode (errors only)
./nlm --quiet

# Log output to file
./nlm --log simulation.log

# Save brain state
./nlm --save brain_state.bin

# Load brain state
./nlm --load brain_state.bin
```

## Advanced Configuration Options

### Plasticity Control

```bash
# Enable all plasticity
./nlm --enable-plasticity

# Disable plasticity
./nlm --disable-plasticity

# Test only STDP learning
./nlm --stdp-only

# Test only Hebbian learning
./nlm --hebbian-only

# Test only structural plasticity
./nlm --structural-only
```

### Memory and Replay

```bash
# Enable memory replay
./nlm --enable-replay

# Set replay interval
./nlm --replay-interval 100

# Enable developmental stages
./nlm --enable-development

# Set development stage
./nlm --development-stage adult
```

### Weight and Bounds Configuration

```bash
# Set weight bounds
./nlm --min-weight -0.5 --max-weight 0.5

# Configure checkpoint system
./nlm --checkpoints 20 --checkpoint-interval 50
```

## Configuration File Examples

### Basic Configuration (`configs/basic.cfg`)

```
# Basic NLM configuration
brain.neuron_count = 1000
brain.connection_probability = 0.1
simulation_timestep = 0.001
random_seed = 42

# Plasticity settings
plasticity.stdp.enable = true
plasticity.hebbian.enable = true
plasticity.structural.enable = false

# Memory settings
memory.enable = true
memory.episodic_max_episodes = 1000
```

### Research Configuration (`configs/research.cfg`)

```
# Research-oriented configuration
brain.neuron_count = 10000
brain.connection_probability = 0.15
simulation_timestep = 0.0001
random_seed = 12345

# Advanced plasticity
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.01
plasticity.hebbian.enable = true
plasticity.hebbian.threshold = 0.5
plasticity.structural.enable = true
plasticity.structural.synaptogenesis_rate = 0.0001
plasticity.structural.pruning_rate = 0.00001

# Full memory system
memory.enable = true
memory.episodic_max_episodes = 50000
memory.working_memory_capacity = 1000
memory.enable_replay = true

# Complete neuromodulation
neuromodulation.enable = true
neuromodulation.curiosity.enable = true
neuromodulation.curiosity.exploration_rate = 0.1
neuromodulation.novelty.enable = true
neuromodulation.novelty.sensitivity = 0.5

# Development
development.enable = true
development.initial_plasticity = 1.0
development.aging_rate = 0.01

# Performance
performance.optimization = aggressive
checkpoint_count = 20
```

### Performance Configuration (`configs/performance.cfg`)

```
# High-performance configuration
brain.neuron_count = 5000
brain.connection_probability = 0.05
simulation_timestep = 0.0005

# Minimal plasticity for speed
plasticity.stdp.enable = true
plasticity.hebbian.enable = false
plasticity.structural.enable = false

# Reduced memory overhead
memory.enable = false

# Aggressive optimization
performance.optimization = aggressive
checkpoint_count = 5
```

## Configuration Inheritance and Merging

The configuration system supports inheritance and merging:

1. **File-based inheritance**: `configs/base.cfg` + `configs/experiment.cfg`
2. **Command-line override**: Command-line args override file settings
3. **Runtime modifications**: Programmatic configuration changes

```bash
# Merge configurations
cp configs/base.cfg .
cp configs/experiment.cfg .

# Load base configuration, override with command line
./nlm --config base.cfg --neurons 2000 --enable-replay

# Programmatic configuration modification
./nlm --config configs/base.cfg --neurons 1000
# Then modify at runtime using NLM API
```

## Debugging and Monitoring Options

### Debugging Options

```bash
# Enable verbose logging
./nlm --verbose

# Enable debug logging
./nlm --log-level debug

# Log to file
./nlm --log simulation.log --verbose

# Performance monitoring
./nlm --monitor-performance

# Memory usage tracking
./nlm --track-memory
```

### Error and Warning Handling

```bash
# Show all messages
./nlm --verbose

# Show only errors
./nlm --quiet

# Custom error handling
./nlm --error-handler strict
```

## Platform-Specific Options

### Linux/macOS

```bash
# Use specific compiler
./nlm --cxx-compiler g++ --std=c++20

# OpenMP support for parallel processing
./nlm --enable-openmp

# SIMD optimization
./nlm --enable-sse4 --enable-avx2
```

### Windows

```bash
# Visual Studio compiler
./nlm --cxx-compiler cl

# Windows-specific optimizations
./nlm --win32-optimized
```

## Script Examples

### Batch Processing Script

```bash
#!/bin/bash

# Run multiple configurations
for config in configs/*.cfg; do
    echo "Running with configuration: $config"
    ./nlm --config "$config" --save "${config%.cfg}_result.bin"
    echo "Completed: $config"
done
```

### Parallel Processing Script

```bash
#!/bin/bash

# Run multiple simulations in parallel
for i in {1..5}; do
    ./nlm --neurons 1000 --seed $i --save "result_$i.bin" &
done

wait

echo "All simulations completed"
```

### Performance Benchmark Script

```bash
#!/bin/bash

# Run performance benchmark
./nlm --config configs/performance.cfg --benchmark

# Compare with research configuration
./nlm --config configs/research.cfg --benchmark

# Generate comparison report
python3 generate_report.py performance_results.json research_results.json
```

## Troubleshooting Common Issues

### Configuration File Issues

**Problem**: Configuration file not found

**Solution**: Use absolute path or ensure file exists:
```bash
./nlm --config ./configs/my_config.cfg
./nlm --config $(pwd)/configs/my_config.cfg
```

**Problem**: Invalid configuration values

**Solution**: Check file format and values:
```bash
# Check configuration file syntax
python3 -m json.tool configs/my_config.cfg
```

### Runtime Errors

**Problem**: Brain initialization failed

**Solution**: Check configuration and system resources:
```bash
./nlm --verbose --neurons 100 --steps 10
```

**Problem**: Memory usage too high

**Solution**: Reduce neuron count or memory usage:
```bash
./nlm --neurons 1000 --memory.enable false
```

### Command-Line Issues

**Problem**: Command-line option not recognized

**Solution**: Check option spelling and use help:
```bash
./nlm --help | grep -i "neurons"
```

**Problem**: Multiple conflicting options

**Solution**: Use consistent options and check for conflicts:
```bash
./nlm --neurons 1000 --stdp-only --hebbian-only
# This will enable both STDP and Hebbian
```

## Advanced Scripting Examples

### Automated Experiment Runner

```python
#!/usr/bin/env python3

import subprocess
import json
import time

def run_experiments(configurations, output_dir="results"):
    """Run multiple experiments with different configurations"""
    
    results = []
    
    for name, config in configurations.items():
        print(f"Running experiment: {name}")
        
        # Create config file
        config_file = f"{output_dir}/{name}.cfg"
        with open(config_file, 'w') as f:
            json.dump(config, f, indent=2)
        
        # Run simulation
        cmd = [
            "./nlm",
            f"--config {config_file}",
            f"--save {output_dir}/{name}_brain.bin",
            f"--log {output_dir}/{name}.log"
        ]
        
        # Execute
        start_time = time.time()
        process = subprocess.run(cmd, capture_output=True, text=True)
        end_time = time.time()
        
        results.append({
            'name': name,
            'success': process.returncode == 0,
            'duration': end_time - start_time,
            'output': process.stdout,
            'error': process.stderr
        })
        
        print(f"  Completed in {end_time - start_time:.2f}s")
        print(f"  Success: {process.returncode == 0}")
    
    return results

# Define experiment configurations
configurations = {
    'baseline': {
        'neurons': 1000,
        'plasticity': {'stdp': True, 'hebbian': False},
        'memory': False
    },
    'experimental': {
        'neurons': 2000,
        'plasticity': {'stdp': True, 'hebbian': True},
        'memory': True,
        'development': True
    }
}

# Run experiments
results = run_experiments(configurations)

# Analyze results
print("\nExperiment Summary:")
for result in results:
    print(f"{result['name']}: {result['success']}")
```

### Configuration Generator

```python
#!/usr/bin/env python3

import argparse
import json
import sys

def generate_configuration(args):
    """Generate configuration file based on command line arguments"""
    
    config = {
        'brain': {
            'neuron_count': args.neurons,
            'connection_probability': args.connection_prob,
            'region_count': args.regions
        },
        'simulation': {
            'timestep': args.timestep,
            'steps': args.steps
        },
        'plasticity': {
            'stdp': {
                'enable': args.stdp,
                'learning_rate': args.stdp_rate,
                'tau': args.stdp_tau
            },
            'hebbian': {
                'enable': args.hebbian,
                'threshold': args.hebbian_threshold
            },
            'structural': {
                'enable': args.structural,
                'synaptogenesis_rate': args.synaptogenesis_rate,
                'pruning_rate': args.pruning_rate
            }
        },
        'memory': {
            'enable': args.memory,
            'episodic_max_episodes': args.episodes,
            'working_memory_capacity': args.working_memory_capacity
        },
        'neuromodulation': {
            'enable': args.neuromodulation,
            'curiosity': {
                'enable': args.curiosity,
                'exploration_rate': args.curiosity_rate
            },
            'novelty': {
                'enable': args.novelty,
                'sensitivity': args.novelty_sensitivity
            }
        }
    }
    
    # Apply presets if requested
    if args.preset == 'minimal':
        config['brain']['neuron_count'] = 500
        config['plasticity']['hebbian']['enable'] = False
        config['memory']['enable'] = False
    elif args.preset == 'balanced':
        config['brain']['neuron_count'] = 1000
        config['plasticity']['structural']['enable'] = True
        config['memory']['enable'] = True
    elif args.preset == 'high_performance':
        config['brain']['neuron_count'] = 5000
        config['simulation']['timestep'] = 0.0001
        config['memory']['enable'] = False
    elif args.preset == 'research':
        config['brain']['neuron_count'] = 10000
        config['plasticity']['structural']['enable'] = True
        config['memory']['enable'] = True
        config['memory']['episodic_max_episodes'] = 50000
        config['neuromodulation']['curiosity']['enable'] = True
        config['development']['enable'] = True
    
    return config

def main():
    parser = argparse.ArgumentParser(description='Generate NLM configuration files')
    
    # Core configuration
    parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
    parser.add_argument('--connection-prob', type=float, default=0.1, help='Connection probability')
    parser.add_argument('--regions', type=int, default=1, help='Number of regions')
    parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
    parser.add_argument('--timestep', type=float, default=0.001, help='Simulation timestep')
    
    # Plasticity configuration
    parser.add_argument('--stdp', action='store_true', help='Enable STDP')
    parser.add_argument('--hebbian', action='store_true', help='Enable Hebbian')
    parser.add_argument('--structural', action='store_true', help='Enable structural plasticity')
    parser.add_argument('--stdp-rate', type=float, default=0.01, help='STDP learning rate')
    parser.add_argument('--stdp-tau', type=float, default=20.0, help='STDP time constant')
    parser.add_argument('--hebbian-threshold', type=float, default=0.5, help='Hebbian threshold')
    parser.add_argument('--synaptogenesis-rate', type=float, default=0.0001, help='Synaptogenesis rate')
    parser.add_argument('--pruning-rate', type=float, default=0.00001, help='Pruning rate')
    
    # Memory configuration
    parser.add_argument('--memory', action='store_true', help='Enable memory system')
    parser.add_argument('--episodes', type=int, default=1000, help='Max episodic episodes')
    parser.add_argument('--working-memory-capacity', type=int, default=100, help='Working memory capacity')
    
    # Neuromodulation configuration
    parser.add_argument('--neuromodulation', action='store_true', help='Enable neuromodulation')
    parser.add_argument('--curiosity', action='store_true', help='Enable curiosity')
    parser.add_argument('--curiosity-rate', type=float, default=0.1, help='Curiosity exploration rate')
    parser.add_argument('--novelty', action='store_true', help='Enable novelty detection')
    parser.add_argument('--novelty-sensitivity', type=float, default=0.5, help='Novelty sensitivity')
    
    # Development configuration
    parser.add_argument('--development', action='store_true', help='Enable development')
    
    # Presets
    parser.add_argument('--preset', choices=['minimal', 'balanced', 'high_performance', 'research'], 
                       help='Use preset configuration')
    
    # Output
    parser.add_argument('--output', type=str, default='config.json', help='Output file path')
    
    args = parser.parse_args()
    
    # Generate configuration
    config = generate_configuration(args)
    
    # Write to file
    with open(args.output, 'w') as f:
        json.dump(config, f, indent=2)
    
    print(f"Configuration written to {args.output}")
    
    # Print summary
    print(f"\nConfiguration Summary:")
    print(f"  Neurons: {config['brain']['neuron_count']}")
    print(f"  Connection probability: {config['brain']['connection_probability']}")
    print(f"  Simulation timestep: {config['simulation']['timestep']}s")
    print(f"  STDP enabled: {config['plasticity']['stdp']['enable']}")
    print(f"  Hebbian enabled: {config['plasticity']['hebbian']['enable']}")
    print(f"  Memory enabled: {config['memory']['enable']}")
    print(f"  Neuromodulation enabled: {config['neuromodulation']['enable']}")

if __name__ == '__main__':
    main()
```

## Conclusion

The enhanced NLM command-line interface provides comprehensive control over neural simulation experiments. From simple basic usage to complex multi-configuration studies, the system supports:

- **Flexible execution modes** for different research scenarios
- **Extensive configuration options** for fine-tuning simulations
- **Advanced scripting capabilities** for automated experiments
- **Robust debugging and monitoring** tools for performance analysis
- **Platform-specific optimizations** for maximum efficiency

This comprehensive interface enables researchers and developers to conduct sophisticated neural network experiments with full control over parameters, configuration, and execution.
