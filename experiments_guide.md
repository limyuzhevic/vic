# NLM Phase 6 Experimental System - User Guide

## Overview

The NLM (Neural Learning Machine) Phase 6 system is an advanced neural simulation framework that integrates multiple brain systems including memory, neuromodulation, prediction, development, and structural plasticity.

## Quick Start

### Basic Usage

```bash
# Quick test with default settings
./nlm_phase6_demo

# Quick demo mode with shortcuts
./nlm_phase6_demo --quick  # 100 neurons, 500 steps
./nlm_phase6_demo --full  # 1000 neurons, 5000 steps with checkpointing

# Custom configuration
./nlm_phase6_demo --neurons 200 --steps 2000 --checkpoint ./my_checkpoint.bin

# View all options
./nlm_phase6_demo --help
```

## Configuration Profiles

The system supports three main configuration profiles designed for different user levels:

### Beginner Profile
- **Neuron Count**: 200 neurons
- **Steps**: 1000 simulation steps
- **Features**: Basic checkpointing only
- **Use Case**: Learning and testing the core integration

### Intermediate Profile
- **Neuron Count**: 500 neurons
- **Steps**: 3000 simulation steps
- **Features**: Checkpointing + Replay + Development
- **Use Case**: More comprehensive experiments with memory replay

### Advanced Profile
- **Neuron Count**: 1000 neurons
- **Steps**: 10000 simulation steps
- **Features**: Full suite including profiling and metrics
- **Use Case**: Production experiments with performance analysis

## Advanced Configuration Options

### Command Line Arguments

| Argument | Short Form | Description |
|----------|------------|-------------|
| `--quick` | `-q` | Quick test mode (100 neurons, 500 steps) |
| `--full` | `-f` | Full experiment mode (1000 neurons, 5000 steps) |
| `--checkpoint <file>` | - | Enable checkpointing with custom file path |
| `--neurons <count>` | - | Set custom neuron count (10-5000) |
| `--steps <count>` | - | Set custom step count (100-100000) |
| `--help` | `-h` | Show this help message |

### Configuration Validation

The system validates configuration parameters and provides warnings:

- **Low neuron counts** (< 10) trigger warnings about insufficient data
- **High neuron counts** (> 5000) warn about increased computational requirements
- **Unrealistic step counts** generate warnings about experiment duration

### Advanced Configuration Fields

| Field | Description | Valid Range |
|-------|-------------|-------------|
| `maxSteps` | Number of simulation steps | 100 - 100000 |
| `neuronCount` | Number of neurons in the brain | 10 - 5000 |
| `regionCount` | Number of neural regions | 1 - 10 |
| `connectionProbability` | Connection density between neurons | 0.01 - 1.0 |
| `enableCheckpointing` | Save/load brain state | true/false |
| `enableReplay` | Enable memory replay functionality | true/false |
| `enableDevelopment` | Enable developmental plasticity | true/false |

## Experiment Configuration

### Example Configuration Files

Create a configuration file `nlm_config.json`:

```json
{
  "maxSteps": 5000,
  "neuronCount": 500,
  "regionCount": 2,
  "connectionProbability": 0.12,
  "enableCheckpointing": true,
  "enableReplay": true,
  "enableDevelopment": true,
  "enableProfiling": false,
  "enableMetricsExport": true,
  "enableVisualization": false,
  "checkpointPath": "./experiments/checkpoint.bin",
  "metricsOutputPath": "./experiments/metrics.csv",
  "visualizationOutputPath": "./experiments/viz/"
}
```

### Programming API

For C++ developers, use the `Phase6Config` class:

```cpp
#include "experiments/Phase6IntegratedExperiment.hpp"

// Create from profile
Phase6Config config = Phase6Config::createFromProfile(
    Phase6Config::ADVANCED_EXPERIMENT
);

// Modify individual parameters
config.neuronCount = 800;
config.maxSteps = 8000;
config.enableProfiling = true;
config.enableMetricsExport = true;
config.metricsOutputPath = "./my_metrics.csv";
```

## Running Experiments

### 1. Quick Testing

```bash
# Minimal test for system verification
./nlm_phase6_demo --quick

# With checkpointing
./nlm_phase6_demo --quick --checkpoint ./quick_test.bin
```

### 2. Comprehensive Testing

```bash
# Intermediate experiment
./nlm_phase6_demo --full --checkpoint ./intermediate_test.bin

# Custom configuration
./nlm_phase6_demo --neurons 750 --steps 3000 --checkpoint ./custom_test.bin
```

### 3. Advanced Experiments

```bash
# Advanced experiment with profiling
./nlm_phase6_demo --neurons 1000 --steps 10000 --checkpoint ./advanced_test.bin

# Performance analysis mode
./nlm_phase6_demo --full --checkpoint ./perf_test.bin
```

## Output and Results

### Console Output

The demo provides detailed feedback during execution:

- **Initialization**: System setup and configuration details
- **Progress Updates**: Every 1000 steps show performance metrics
- **Final Results**: Complete integration status and metrics

### Generated Files

1. **Checkpoint Files**: `./checkpoint.bin` (default) or custom path
2. **Metrics Files**: `./metrics.csv` (when enabled)
3. **Visualization Files**: `./visualization/` directory (when enabled)

### Metrics Output

When enabled, the system generates CSV metrics with:

| Metric | Description |
|--------|-------------|
| `step` | Simulation step number |
| `reward` | Accumulated reward |
| `firingRate` | Average neural firing rate |
| `workingMemorySize` | Working memory trace count |
| `episodicMemoryCount` | Number of stored episodes |
| `dopamineLevel` | Neuromodulation level |
| `timePerStep` | Average time per simulation step |

## Troubleshooting

### Common Issues

1. **Low Performance**
   - Ensure minimum 100 neurons for meaningful results
   - Increase step count for better convergence
   - Check system resources (memory, CPU)

2. **Checkpoint Failures**
   - Verify write permissions in checkpoint directory
   - Ensure sufficient disk space
   - Try a different checkpoint file path

3. **Memory Issues**
   - Reduce neuron count for limited memory systems
   - Increase step count incrementally
   - Monitor system memory during long runs

### Error Messages

- `Brain initialization failed`: Check configuration parameters
- `Failed to save checkpoint`: Verify file permissions
- `Checkpoint validation failed`: Corrupt checkpoint file, delete and restart

## Advanced Features

### Performance Profiling

Enable with `--profile` flag or in configuration:

```cpp
config.enableProfiling = true;
```

Collects:
- Step execution times
- Memory usage patterns
- CPU utilization
- I/O performance metrics

### Metrics Export

Export experimental data for analysis:

```cpp
config.enableMetricsExport = true;
config.metricsOutputPath = "./experiment_data.csv";
```

### Visualization Generation

Generate visual outputs for results:

```cpp
config.enableVisualization = true;
config.visualizationOutputPath = "./visualizations/";
```

## Development and Research

### Experiment Customization

For research purposes, the system provides hooks for:

- Custom reward functions
- Modified neuromodulation parameters
- Alternative plasticity rules
- Specialized memory architectures

### API Reference

Key classes and methods:

- `Phase6Config`: Configuration management
- `Phase6IntegratedExperiment`: Main experiment runner
- `Phase6IntegrationResult`: Experiment results
- `createBeginnerConfig()`: Create beginner-friendly configuration
- `createIntermediateConfig()`: Create intermediate configuration
- `createAdvancedConfig()`: Create advanced configuration

## FAQ

### Q: What's the difference between `--quick` and `--full`?

**A**: `--quick` runs 100 neurons for 500 steps (fast testing), while `--full` runs 1000 neurons for 5000 steps (comprehensive testing with checkpointing).

### Q: How do I save experiment results?

**A**: Enable checkpointing with `--checkpoint <file>` or configure via the API. Results include neural states, memories, and performance metrics.

### Q: Can I run multiple experiments?

**A**: Yes, each run creates its own checkpoint file. Use different checkpoint paths for separate experiments.

### Q: What's the maximum neuron count?

**A**: 5000 neurons is recommended for most systems. Higher counts may require more memory and computation time.

## Support and Examples

For additional examples and support:

1. Check the `docs/` directory for detailed technical specifications
2. Review experiment examples in `src/experiments/`
3. Join the NLM community for research collaboration
4. Report issues through the GitHub repository

---

*Last Updated: November 2026*
*NLM Phase 6 Experimental System v1.0.0*