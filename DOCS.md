# NLM (熙然) - Neural Learning Machine Documentation

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain that implements real spiking neural computation. This documentation provides comprehensive guidance for using, configuring, and extending the NLM system.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Configuration System](#configuration-system)
3. [Command Line Interface](#command-line-interface)
4. [Running Experiments](#running-experiments)
5. [Code Structure](#code-structure)
6. [Advanced Topics](#advanced-topics)
7. [Troubleshooting](#troubleshooting)
8. [References](#references)

## Quick Start

### Installation

The NLM system is already installed and ready to use. The source code is available in the `src/` directory.

### Basic Usage

Run the basic demonstration to see NLM in action:

```bash
./nlm
```

Or run with custom configuration:

```bash
./nlm --config myconfig.cfg --neurons 1000
```

### Dry Run Mode

Test your configuration without running the simulation:

```bash
./nlm --dry-run --neurons 2000 --seed 12345
```

## Configuration System

The NLM uses a flexible configuration system that supports multiple input formats:

### Configuration File Format

Save your configuration to a file (e.g., `configs/myconfig.cfg`):

```ini
# NLM Configuration File (Generated)
# Source: 0 (default)
# neuron_count = 500
# region_count = 1
# simulation_timestep = 0.001
# connection_probability = 0.15
# random_seed = 42
# stdp_ltp_weight = 0.02
# stdp_ltd_weight = 0.015
# stdp_tau = 20.0
# synaptogenesis_rate = 0.0001
# pruning_rate = 0.00001
```

### Supported Configuration Values

| Key | Type | Default | Description | Validation |
|-----|------|---------|-------------|------------|
| `neuron_count` | int64_t | 500 | Number of neurons | >0, ≤100,000 |
| `region_count` | int64_t | 1 | Number of brain regions | >0, ≤1000 |
| `simulation_timestep` | double | 0.001 | Simulation timestep (seconds) | >0, <1.0 |
| `connection_probability` | float | 0.15 | Connection probability | 0.0-1.0 |
| `random_seed` | int64_t | 42 | Random seed for reproducibility | ≥0 |
| `stdp_ltp_weight` | float | 0.02 | STDP LTP weight | >0 |
| `stdp_ltd_weight` | float | 0.015 | STDP LTD weight | >0 |
| `stdp_tau` | float | 20.0 | STDP time constant | >0 |
| `synaptogenesis_rate` | float | 0.0001 | Synaptogenesis rate | ≥0 |
| `pruning_rate` | float | 0.00001 | Pruning rate | ≥0 |
| `checkpoint_dir` | string | "./checkpoints" | Directory for checkpoint files | Directory must exist |

## Command Line Interface

### Help and Version

```bash
./nlm --help          # Show complete help information
./nlm --version       # Show version information
```

### Basic Configuration

```bash
# Basic usage with defaults
./nlm

# Custom neuron count
./nlm --neurons 1000

# Custom timestep and seed
./nlm --timestep 0.002 --seed 12345
```

### Advanced Configuration

```bash
# Complex configuration with multiple parameters
./nlm \
  --neurons 2000 \
  --regions 3 \
  --timestep 0.001 \
  --seed 42 \
  --connection-prob 0.2 \
  --stdp-ltp 0.025 \
  --stdp-ltd 0.018 \
  --stdp-tau 25.0

# Using configuration file with overrides
./nlm --config configs/experiment1.cfg --neurons 500

# Dry run to validate configuration
./nlm --dry-run --neurons 1000 --checkpoint ./mycheckpoints
```

### Configuration File Examples

#### Small-Scale Experiment (configs/small.cfg)

```ini
# Small-scale neural network for quick testing
neuron_count = 100
region_count = 1
simulation_timestep = 0.001
connection_probability = 0.1
random_seed = 123
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 15.0
synaptogenesis_rate = 0.00005
pruning_rate = 0.000005
```

#### Large-Scale Experiment (configs/large.cfg)

```ini
# Large-scale neural network for comprehensive testing
neuron_count = 10000
region_count = 5
simulation_timestep = 0.0005
connection_probability = 0.05
random_seed = 456
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.018
stdp_tau = 30.0
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002
```

## Running Experiments

The NLM system includes several built-in experiments:

### 1. Basic Connectivity Test

Tests whether spikes propagate through the neural network:

```bash
# Run only connectivity test
# (Modify main.cpp to skip other tests for focused testing)
```

**Expected Output:**
```
=== Test 1: Basic Neural Connectivity ===
  Injecting current into 10 neurons...
  Spikes generated: 42
  ✓ Spikes propagate through network
```

### 2. Plasticity Learning Experiment

Demonstrates activity-dependent synaptic changes:

```bash
# Run plasticity experiment
./nlm --neurons 500 --seed 42
```

**Expected Output:**
```
=== Test 2: Plasticity Learning Experiment ===
Applying repeated input patterns (1000 steps)...
  Step 0 | Spikes: 0 | Firing: 0
  Step 100 | Spikes: 42 | Firing: 0.15
  Step 200 | Spikes: 78 | Firing: 0.23
  ...
=== Learning Experiment Results ===
Weight Statistics:
  Initial mean weight: 0.1500
  Final mean weight: 0.1682
  Change: 0.0182

Synaptic Changes:
  Strengthened: 15
  Weakened: 8
  Unchanged: 30

✓ LEARNING DETECTED: Synaptic weights changed through experience
```

### 3. STDP Verification

Validates Spike-Timing-Dependent Plasticity:

```bash
# Run STDP verification
./nlm --neurons 300 --seed 123
```

**Expected Output:**
```
=== Test 3: STDP Verification ===
  Testing STDP on 5 synapses:
    Synapse 0 before: 0.1500
    Synapse 1 before: 0.1480
    ...
  Creating correlated pre->post activity (potentiation)...
  After correlated activity:
    Synapse 0 after: 0.1725 (Δ=0.0225)
    Synapse 1 after: 0.1492 (Δ=0.0012)
    ...
  ✓ STDP WORKING: Pre-before-post produced potentiation
```

## Code Structure

### Core Architecture

```
src/
├── core/              # Core utilities and types
│   ├── Config/        # Configuration system
│   ├── Random/        # Random number generation
│   ├── Logger/        # Logging system
│   └── SimulationClock/ # Time management
├── brain/             # Brain components
│   ├── Neuron.cpp     # LIF neuron implementation
│   ├── Synapse.cpp    # Synaptic connection
│   ├── NeuralPopulation.cpp # Neuron groups
│   ├── NeuralRegion.cpp # Brain regions
│   └── Brain.cpp      # Main brain controller
├── dynamics/          # Neural dynamics
│   ├── NeuralDynamics.cpp # General dynamics
│   └── SpikeSystem.cpp   # Event-driven spike processing
├── plasticity/         # Plasticity rules
│   ├── PlasticityRule.cpp # Base class
│   ├── Hebbian.cpp     # Hebbian learning
│   ├── STDP.cpp        # Spike-timing dependent plasticity
│   └── StructuralPlasticity.cpp # Structural changes
├── neuromodulation/    # Neuromodulators
├── memory/            # Memory systems
├── cognition/          # Cognitive functions
├── sensory/            # Sensory processing
├── motor/              # Motor output
├── prediction/         # Prediction systems
├── environment/        # Environment interface
├── experiments/        # Experiment framework
└── visualization/     # Visualization tools
```

### Key Components

#### 1. Config Class (`src/core/Config/Config.hpp`)

The configuration system provides:
- Type-safe configuration value storage
- Validation rules for configuration parameters
- Multiple input formats (file, command-line, runtime)
- Comprehensive error reporting

#### 2. Brain Class (`src/brain/Brain.hpp`)

The main brain controller implements:
- 8-phase integrated processing loop
- Memory systems (working, episodic, associative)
- Prediction and cognition systems
- Neuromodulation
- Development
- Plasticity

#### 3. NeuralWorkingMemory (`src/memory/NeuralWorkingMemory.hpp`)

Implements working memory using neural dynamics:
- Persistent neural activity
- Recurrent connections
- Activity-dependent short-term plasticity
- Neural competition

#### 4. AttentionalSelection (`src/memory/AttentionalSelection.hpp`)

Implements neural attention through competition:
- Competition between neural populations
- Salience-based selection
- Recurrent amplification
- Inhibitory surround suppression

## Advanced Topics

### 1. Extending the Configuration System

To add new configuration parameters, modify `src/core/Config/Config.hpp`:

```cpp
// Add to validation rules in Config::Impl::initializeValidationRules()
validationRules["new_parameter"] = [](const ConfigValue& v) {
    if (const auto* val = std::get_if<double>(&v)) {
        return *val >= 0.0 && *val <= 1000.0;
    }
    return false;
};
```

### 2. Custom Experiments

Create custom experiments by extending the `Experiment` base class in `src/experiments/Experiment.hpp`:

```cpp
class CustomExperiment : public Experiment {
public:
    CustomExperiment(std::shared_ptr<Brain> brain) : Experiment(brain) {}
    
    void run() override {
        // Your custom experiment logic here
    }
};
```

### 3. Checkpoint System

The checkpoint system allows saving and loading brain states:

```cpp
// Save brain state
brain->save("./checkpoints/brain_state.chk");

// Load brain state
brain->load("./checkpoints/brain_state.chk");
```

### 4. Performance Optimization

For large-scale simulations, consider:
- Increasing the simulation timestep
- Reducing the number of neurons/regions
- Using hardware-specific optimizations
- Parallel processing for spike propagation

## Troubleshooting

### Common Issues

#### 1. Configuration File Not Found

```
Error: Cannot open config file: configs/default.cfg
```

**Solution:** Create the config file or use `--dry-run` to use defaults.

#### 2. Invalid Configuration Values

```
Warning: Invalid configuration value for key 'neurons'. Value will not be set.
```

**Solution:** Check your configuration file syntax and valid ranges.

#### 3. Memory Usage Too High

**Solution:** Reduce the number of neurons or regions, or increase the simulation timestep.

#### 4. No Spikes Generated

**Solution:** Check neuron parameters (threshold, resting potential) or increase input current.

### Debug Mode

To enable more detailed debugging, modify the logger level in `main.cpp`:

```cpp
auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
```

## References

### Papers and Resources

1. **Basic Neuroscience:** Dayan, P., & Abbott, L. F. (2001). *Theoretical Neuroscience*. MIT Press.
2. **Neural Dynamics:** Gerstner, W., Kistler, W., Naud, R., & Paninski, L. (2014). *Neuronal Dynamics*. Cambridge University Press.
3. **Plasticity:** Song, S., Miller, K. D., & Abbott, L. F. (2000). *Competitive Hebbian learning through spike-timing-dependent synaptic plasticity*. Neuron, 32(6), 1125-1136.

### External Resources

- [GitHub Repository](https://github.com/example/nlm)
- [Documentation Wiki](https://wiki.example.com/nlm)
- [API Reference](https://api.example.com/nlm)

### Version History

- **Version 2.0 (Phase 2):** Real neural computation with LIF neurons
- **Version 1.0 (Phase 1):** Basic neural network simulation

## License

This software is licensed under the MIT License. See `LICENSE` file for details.

## Acknowledgments

This work is based on research in:
- Neural dynamics and computation
- Artificial developmental systems
- Neuromorphic engineering
- Machine learning and pattern recognition

## Contact

For questions, issues, or contributions:
- Email: support@example.com
- GitHub: https://github.com/example/nlm/issues
- Forum: https://forum.example.com/nlm

---

*Document generated automatically by NLM Documentation System*
*Last updated: 2024-01-11*
*Version: 2.0.0*