# NLM Practical Examples and Usage Guide
# Version: 0.1.0 Phase 6

## Introduction

This document provides practical examples and usage guides for the NLM (Neural Learning Machine) unified CLI interface and new features. It covers common workflows, advanced usage patterns, and troubleshooting.

## Quick Start Guide

### 1. Basic Usage

#### Running a Demo
```bash
# Run Phase 2 demo (real neural computation)
./nlm demo phase2

# Run Phase 3 demo (world interaction) with custom parameters
./nlm demo phase3 10 500

# Run Phase 5 demo (personality and sandbox)
./nlm demo phase5

# Run Phase 6 demo (final integration test)
./nlm demo phase6

# Run benchmark to measure performance
./nlm benchmark
```

#### Using Configuration File
```bash
# Use custom configuration file
./nlm --config myconfig.cfg demo phase2

# Use configuration file with verbose output
./nlm --config myconfig.cfg --verbose demo phase2
```

#### Setting Random Seed for Reproducibility
```bash
# Set random seed for reproducible experiments
./nlm --seed 12345 demo phase3 5 200

# Use same seed for comparison
./nlm --seed 12345 demo phase5
./nlm --seed 12345 demo phase5  # Should produce same results
```

### 2. Configuration Management

#### Creating Custom Configuration Files

Create a custom configuration file (`myconfig.cfg`):
```ini
# NLM Configuration File
# Custom settings for Phase 3 demo

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Environment settings
environment_name = GridWorld
environment_width = 30
environment_height = 30

# Vision settings
vision_width = 32
vision_height = 32

# Simulation settings
max_simulation_steps = 5000

# Enable optional features
enable_curiosity = true
enable_development = true
enable_reward = true
enable_planner = true
enable_concepts = true
```

#### Validating Configuration Files
```bash
# Validate configuration file
./nlm config validate myconfig.cfg

# Show configuration schema
./nlm config schema > config_schema.md

# Validate with warnings-only mode
./nlm config validate -w myconfig.cfg
```

### 3. Demo-Specific Usage

#### Phase 2: Real Neural Computation
```bash
# Run basic connectivity test
./nlm demo phase2

# Run with custom neuron count and steps
./nlm demo phase2 --neuron-count 1000 --steps 2000

# Run only STDP verification
./nlm demo phase2 --stdp-only

# Run with plasticity disabled
./nlm demo phase2 --no-plasticity

# Run with connectivity test disabled
./nlm demo phase2 --no-connectivity
```

#### Phase 3: World Interaction
```bash
# Run with custom episode and step counts
./nlm demo phase3 8 300

# Run with larger world
./nlm demo phase3 5 200 --world-size 40

# Run with only curiosity enabled
./nlm demo phase3 5 200 --enable-curiosity --disable-development

# Run with all optional features
./nlm demo phase3 3 100 --enable-all

# Run with specific feature combinations
./nlm demo phase3 10 500 --enable-curiosity --enable-development --enable-reward
```

#### Phase 4: Cognitive Integration
```bash
# Run with working memory focus
./nlm demo phase4 --memory-size 200

# Run with all cognitive features
./nlm demo phase4 --enable-all

# Run with specific cognitive features
./nlm demo phase4 --enable-associative --enable-planning --enable-attention
```

#### Phase 5: Personality & Sandbox
```bash
# Run basic sandbox
./nlm demo phase5

# Run with pattern probe
./nlm demo phase5 --enable-pattern-probe

# Run with checkpointing
./nlm demo phase5 --enable-checkpoint

# Run with replay functionality
./nlm demo phase5 --enable-replay

# Run with custom sandbox settings
./nlm demo phase5 --sandbox-steps 200 --max-resources 5
```

#### Phase 6: Final Integration
```bash
# Run quick integration verification
./nlm demo phase6 --quick

# Run full integration test
./nlm demo phase6 --full

# Run specific integration tests
./nlm demo phase6 --memory-test --neuromod-test

# Run all integration tests
./nlm demo phase6 --memory-test --neuromod-test --checkpoint-test --replay-test
```

### 4. Experiment Management

#### Running Experiment Suites
```bash
# Run experiment (not yet implemented)
./nlm experiment run --type phase6 --episodes 10

# Compare results between phases
./nlm experiment compare phase5 phase6 --runs 3

# Run ablation studies
./nlm experiment ablation --component working_memory --baseline
```

#### Managing Experiment Results
```bash
# Save experiment results to file
./nlm experiment run --type phase3 --episodes 5 --output results.json

# Compare experiment results
./nlm experiment compare phase3 phase5 --runs 5 --metrics reward,performance,learning
```

### 5. Performance Benchmarking

#### Running Benchmarks
```bash
# Run default benchmarks
./nlm benchmark

# Run with custom neuron sizes
./nlm benchmark --neuron-sizes "100,500,1000,2000"

# Run with custom steps
./nlm benchmark --steps 500

# Run with JSON output format
./nlm benchmark --format json --output benchmark_results.json

# Run with quiet mode
./nlm benchmark --quiet
```

#### Interpreting Benchmark Results

Benchmark results include:
- **Neurons**: Number of neurons in the network
- **Synapses**: Number of connections
- **Steps/sec**: Simulation performance
- **Spikes/step**: Average activity level
- **Synapses/Neuron**: Network density

### 6. Advanced Usage

#### Using the Launcher Script

The NLM launcher script (`nlm_launcher.sh`) provides an easy-to-use interface:

```bash
# Run Phase 2 demo with launcher
./nlm_launcher.sh phase2

# Run Phase 3 demo with custom settings
./nlm_launcher.sh phase3 10 500

# Run with custom configuration
./nlm_launcher.sh --config myconfig.cfg phase5

# Run with verbose output
./nlm_launcher.sh --verbose phase2
```

#### Scripted Execution

Create a bash script to run multiple demos:
```bash
#!/bin/bash

# Run all demos sequentially
echo "Running NLM Phase 2 Demo"
./nlm demo phase2 > phase2_output.txt 2>&1

echo "Running NLM Phase 3 Demo"
./nlm demo phase3 5 200 > phase3_output.txt 2>&1

echo "Running NLM Phase 6 Demo"
./nlm demo phase6 --quick > phase6_output.txt 2>&1

echo "All demos completed!"
```

#### Python Integration
```python
#!/usr/bin/env python3
import subprocess
import json
import time

# Run NLM demo programmatically
def run_nlm_demo(phase, **kwargs):
    cmd = ["nlm", "demo", phase]
    
    # Add keyword arguments
    for key, value in kwargs.items():
        cmd.extend([f"--{key.replace('_', '-')}", str(value)])
    
    print(f"Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    return {
        "returncode": result.returncode,
        "stdout": result.stdout,
        "stderr": result.stderr,
        "success": result.returncode == 0
    }

# Example usage
results = []
for phase in ["phase2", "phase3", "phase6"]:
    result = run_nlm_demo(phase, steps=100 if phase == "phase2" else 200)
    results.append({"phase": phase, "result": result})

# Save results
with open("experiment_results.json", "w") as f:
    json.dump(results, f, indent=2)
```

### 7. Troubleshooting

#### Common Issues and Solutions

**Issue: Configuration file not found**
```bash
# Error: Cannot open configuration file
# Solution:
# 1. Ensure the config file exists
# 2. Use absolute path: ./nlm --config /path/to/config.cfg
# 3. Use the launcher script which includes error checking
```

**Issue: Missing dependencies**
```bash
# Error: Compiler not found or missing dependencies
# Solution:
# 1. Ensure all dependencies are installed
# 2. Run: cmake -B build -S . && cmake --build build
# 3. Check README.md for detailed installation instructions
```

**Issue: Invalid configuration**
```bash
# Error: Configuration validation failed
# Solution:
# 1. Check error messages for specific issues
# 2. Use configuration validation: ./nlm config validate config.cfg
# 3. Refer to configuration schema for valid options
```

**Issue: Demo fails to start**
```bash
# Error: Segmentation fault or other crash
# Solution:
# 1. Check available memory: free -h
# 2. Try with smaller network: ./nlm demo phase2 --neuron-count 100
# 3. Check log files for error details
```

#### Getting Help
```bash
# General help
./nlm --help

# Demo-specific help
./nlm demo phase2 --help
./nlm demo phase3 --help

# Configuration help
./nlm config --help

# Benchmark help
./nlm benchmark --help

# Experiment help
./nlm experiment --help
```

### 8. Performance Optimization

#### Optimizing for Different Use Cases

**For Fast Demos**:
```bash
# Use smaller networks for quick testing
./nlm demo phase2 --neuron-count 100 --steps 100
./nlm demo phase3 3 50 --world-size 10
```

**For Detailed Analysis**:
```bash
# Use larger networks for detailed analysis
./nlm demo phase2 --neuron-count 2000 --steps 5000
./nlm demo phase3 10 1000 --world-size 50
```

**For Memory-Constrained Systems**:
```bash
# Disable memory-intensive features
./nlm demo phase3 --disable-concepts --disable-planner
./nlm demo phase4 --no-associative --no-planning
```

### 9. Examples Archive

#### Example 1: Learning Behavior
```bash
# Run Phase 3 demo with curiosity to see learning behavior
./nlm demo phase3 10 500 --enable-curiosity --enable-development
```

#### Example 2: Performance Testing
```bash
# Run benchmarks to compare performance across different network sizes
./nlm benchmark --neuron-sizes "100,500,1000" --format json
```

#### Example 3: Configuration Comparison
```bash
# Compare results with different configurations
./nlm --config configs/optimized.cfg demo phase2
./nlm --config configs/minimal.cfg demo phase2
```

#### Example 4: Automated Experimentation
```bash
#!/bin/bash
# Automated experiment runner
for seed in 42 123 456; do
    echo "Running experiment with seed $seed"
    ./nlm --seed $seed demo phase3 5 100 --enable-curiosity
    sleep 5  # Wait between runs
Done
echo "Experiment completed"
```

## Conclusion

This guide provides comprehensive examples for using the NLM unified CLI interface. The improvements and new features make NLM more accessible and powerful for both casual users and advanced users.

## Key Takeaways

1. **Unified Interface**: Use `./nlm` for all NLM operations
2. **Configuration Management**: Use configuration files for consistent settings
3. **Reproducibility**: Use random seeds for reproducible experiments
4. **Validation**: Use configuration validation to ensure correctness
5. **Testing**: Use the test suite to validate NLM functionality
6. **Scripting**: Use the launcher script or Python integration for automated workflows

## References

1. **NLM Documentation**: `NLM_IMPROVEMENTS_SUMMARY.md` for detailed documentation
2. **CLI Interface**: `NLM_CLI.md` for CLI interface documentation
3. **Configuration Schema**: Generated by `./nlm config schema`
4. **Build Instructions**: See `README.md` for build instructions
5. **Demo Documentation**: See individual demo documentation for specific usage

---
*Document generated by NLM Development Team*
*Version: 0.1.0 Phase 6*
*For support, refer to NLM documentation and community resources*