# NLM Phase 6 - Comprehensive Documentation
# Neural Learning Machine - Final Integration

This document provides comprehensive information about NLM Phase 6, the final integration phase of the Neural Learning Machine project. It covers:

- System architecture and integration
- Command-line interface and usage
- Configuration and customization
- Advanced features and capabilities
- Examples and tutorials

## Table of Contents

1. [Overview and Architecture](#overview-and-architecture)
2. [Key Features](#key-features)
3. [Command-Line Interface](#command-line-interface)
   - [Available Modes](#available-modes)
   - [Command-Line Options](#command-line-options)
   - [Usage Examples](#usage-examples)
4. [Configuration](#configuration)
   - [Configuration File Format](#configuration-file-format)
   - [Default Settings](#default-settings)
   - [Advanced Configuration](#advanced-configuration)
5. [Systems Integration](#systems-integration)
6. [Advanced Usage](#advanced-usage)
7. [Troubleshooting](#troubleshooting)
8. [API Reference](#api-reference)

## Overview and Architecture

NLM (熙然, meaning "serene flow") is an experimental computational brain project. Phase 6 represents the final integration of all components into a coherent artificial brain system.

### System Architecture

The NLM Phase 6 brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

### Integrated Systems

Phase 6 integrates the following systems:

#### Memory Systems
- **Working Memory**: Transient active information using neural persistent activity
- **Episodic Memory**: Experience storage and replay capabilities
- **Associative Memory**: Pattern associations and schema formation

#### Neuromodulation Systems
- **Dopamine**: Reward prediction error and reinforcement learning
- **Curiosity**: Exploration motivation and novelty detection
- **Novelty**: Novelty detection and change detection
- **Prediction Error**: Sensory prediction error computation

#### Prediction System
- **Prediction System**: Sensory prediction and error computation
- **Neural Prediction**: Pattern prediction and error signals

#### Cognition Systems
- **Neural Planner**: Action planning and sequence evaluation
- **Concept Formation**: Pattern discovery and abstraction
- **Attentional Selection**: Competitive selection and focus

#### Development System
- **Development Stages**: Initial, Critical Period, Maturation, Adult
- **Plasticity Modulation**: Age-dependent learning rates
- **Structural Plasticity**: Synaptogenesis and pruning

#### Core Processing
- **Neurons**: Leaky Integrate-and-Fire (LIF) dynamics
- **Synapses**: Event-driven spike propagation with delays
- **Plasticity**: STDP, Hebbian, and structural plasticity rules

## Key Features

### Enhanced Command-Line Interface
- **Multiple execution modes**: demo, debug, experiment, visualization, plugin, benchmark, integration
- **Advanced configuration**: Extensive options for customization
- **Performance monitoring**: Built-in benchmarking and profiling
- **Plugin system**: Extensible architecture for adding new modules

### Comprehensive Integration
- **All systems connected**: No system operates in isolation
- **Cross-system interactions**: Neuromodulators affect plasticity across systems
- **Feedback loops**: Multiple feedback and feedforward connections
- **Emergent behavior**: Complex behavior emerges from simple interactions

### Advanced Capabilities
- **Checkpointing**: Save/load brain state for persistence
- **Replay**: Memory replay during sleep/rest cycles
- **Consolidation**: Memory consolidation for important episodes
- **Development**: Stage-based plasticity and learning curves

### Performance Optimizations
- **Event-driven processing**: Efficient spike propagation
- **Cache-friendly access patterns**: Optimized for modern CPUs
- **Parallel processing**: Scalable for large networks
- **Memory pools**: Reduced allocation overhead

## Command-Line Interface

The NLM Phase 6 CLI provides multiple modes for different use cases.

### Available Modes

#### Demo Mode (`demo`)
Run a comprehensive Phase 6 integration demo with automatic verification.

**Command:**
```bash
nlm demo [--neurons N] [--steps S] [--regions R] [--seed VAL] [--checkpoint FILE] [--replay] [--develop] [--output FILE]
```

**Examples:**
```bash
nlm demo --neurons 500 --steps 2000
nlm demo --config myconfig.cfg --verbose
nlm demo --neurons 1000 --steps 5000 --checkpoint /tmp/brain.bin
```

#### Debug Mode (`debug`)
Interactive debugging with detailed logging and step-by-step execution.

**Command:**
```bash
nlm debug [--config FILE] [--verbose] [--quiet]
```

**Example:**
```bash
nlm debug --verbose --steps 500
```

#### Experiment Mode (`experiment`)
Run custom experiments with specific parameters.

**Command:**
```bash
nlm experiment [--neurons N] [--steps S] [--config FILE] [--output FILE]
```

#### Visualization Mode (`visualization`)
Interactive visualization of brain activity and memory traces.

**Command:**
```bash
nlm visualization [--neurons N] [--steps S] [--output FILE]
```

#### Plugin Mode (`plugin`)
Plugin development and testing mode for creating new modules.

**Command:**
```bash
nlm plugin [PLUGIN_NAME] [--config FILE]
```

#### Benchmark Mode (`benchmark`)
Performance benchmarking of the integrated brain.

**Command:**
```bash
nlm benchmark [--neurons N] [--steps S]
```

**Example:**
```bash
nlm benchmark --neurons 2000 --steps 10000
```

#### Integration Test Mode (`integration`)
Comprehensive integration verification tests.

**Command:**
```bash
nlm integration
```

### Command-Line Options

Common options across all modes:

| Option | Description | Default |
|--------|-------------|---------|
| `--neurons N` | Number of neurons | 1000 |
| `--steps S` | Number of simulation steps | 1000 |
| `--regions R` | Number of brain regions | 1 |
| `--seed VAL` | Random seed | 42 |
| `--config FILE` | Load configuration from file | (none) |
| `--checkpoint FILE` | Enable checkpointing, save to FILE | (disabled) |
| `--replay` | Enable memory replay | (disabled) |
| `--develop` | Enable development stages | (disabled) |
| `--output FILE` | Save results to FILE | (none) |
| `--verbose, -V` | Enable verbose logging | (disabled) |
| `--quiet, -q` | Minimal output | (disabled) |

Advanced options:

| Option | Description |
|--------|-------------|
| `--memory-capacity N` | Working memory capacity |
| `--plasticity-mod MOD` | Plasticity modulation factor |
| `--reward-weight W` | Reward weight for learning |
| `--curiosity-threshold T` | Curiosity threshold |
| `--attention-strength S` | Attention system strength |
| `--sensory-gain G` | Sensory input gain |
| `--motor-response R` | Motor response rate |

### Usage Examples

#### Basic Demo
```bash
nlm demo
```
This runs a comprehensive Phase 6 integration demo with default settings (1000 neurons, 1000 steps).

#### Large-Scale Experiment
```bash
nlm demo --neurons 5000 --steps 10000 --regions 4 --develop --replay --checkpoint /tmp/large_brain.bin
```
This runs a large-scale experiment with 5000 neurons across 4 regions, with development and replay enabled.

#### Debug Session
```bash
nlm debug --config experiment.cfg --verbose
```
This runs a debug session with detailed logging using a custom configuration file.

#### Performance Benchmark
```bash
nlm benchmark --neurons 2000 --steps 5000
```
This benchmarks the performance of the NLM Phase 6 system with 2000 neurons for 5000 steps.

#### Integration Verification
```bash
nlm integration
```
This runs comprehensive integration verification tests to ensure all systems are properly connected.

## Configuration

### Configuration File Format

NLM uses a simple key=value configuration format (JSON format is planned for future phases).

Example configuration file (`config.cfg`):
```
# NLM Phase 6 Configuration
# ======================

# Brain parameters
neuron_count = 1000
region_count = 1
connection_probability = 0.1

# Simulation parameters
simulation_timestep = 0.001
random_seed = 42

# Plasticity parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0

# Structural plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory system parameters
working_memory_capacity = 100
episodic_memory_max_episodes = 1000

# Neuromodulation parameters
dopamine_initial_level = 0.5
curiosity_threshold = 0.3
novelty_sensitivity = 0.8

# Development parameters
enable_development = true
initial_plasticity = 1.0
critical_period_plasticity = 0.8
maturation_plasticity = 0.5
adult_plasticity = 0.2

# Replay and consolidation parameters
replay_interval = 100
consolidation_interval = 1000

# World parameters
world_width = 16
world_height = 16
world_seed = 123
```

### Default Settings

The following are the default configuration values:

| Parameter | Value | Description |
|-----------|-------|-------------|
| neuron_count | 1000 | Number of neurons |
| region_count | 1 | Number of brain regions |
| connection_probability | 0.1 | Probability of synaptic connections |
| simulation_timestep | 0.001 | Simulation timestep in seconds |
| random_seed | 42 | Random seed for reproducibility |
| stdp_ltp_weight | 0.01 | STDP long-term potentiation weight |
| stdp_ltd_weight | 0.012 | STDP long-term depression weight |
| stdp_tau | 20.0 | STDP time constant |
| synaptogenesis_rate | 0.0001 | Synaptogenesis rate |
| pruning_rate | 0.00001 | Pruning rate |
| working_memory_capacity | 100 | Working memory capacity |
| episodic_memory_max_episodes | 1000 | Maximum number of episodic memories |

### Advanced Configuration

Advanced configuration options allow fine-tuning of the brain behavior:

#### Memory System Configuration
```
working_memory_capacity = 200
working_memory_decay_rate = 0.01
episodic_memory_max_episodes = 5000
episodic_memory_store_interval = 10
episodic_memory_replay_episodes = 5
```

#### Neuromodulation Configuration
```
dopamine_initial_level = 0.7
dopamine_plasticity_factor = 1.2
curiosity_threshold = 0.2
curiosity_exploration_bonus = 0.5
novelty_sensitivity = 1.0
novelty_decay_rate = 0.05
```

#### Development Configuration
```
enable_development = true
developmental_stage_duration = 1000
plasticity_modulation_rate = 0.01
structural_plasticity_enabled = true
```

## Systems Integration

### Memory Integration
The memory systems work together to store and retrieve information:

1. **Working Memory**: Maintains active neural representations
2. **Episodic Memory**: Stores experiences as episodes
3. **Associative Memory**: Forms associations between patterns

### Neuromodulation Integration
Neuromodulators affect plasticity and learning across systems:

1. **Dopamine**: Signals reward prediction errors
2. **Curiosity**: Drives exploration of novel stimuli
3. **Novelty**: Detects changes in the environment
4. **Prediction Error**: Computes prediction accuracy

### Prediction and Cognition Integration
Prediction and cognition systems work together for intelligent behavior:

1. **Prediction System**: Generates predictions of sensory input
2. **Neural Planner**: Plans actions based on predictions
3. **Concept Formation**: Forms abstract concepts from experiences
4. **Attentional Selection**: Selects relevant information for processing

### Development Integration
Development affects plasticity and learning throughout the lifespan:

1. **Stage-based Plasticity**: Plasticity changes with developmental stage
2. **Structural Plasticity**: Synaptogenesis and pruning based on activity
3. **Maturation**: Progressive specialization and refinement

## Advanced Usage

### Custom Experiments
Create custom experiments using the experiment mode:

```bash
nlm experiment --neurons 500 --steps 2000 --config custom.cfg --output results.txt
```

### Plugin Development
Develop new modules by creating plugins:

1. Create a new plugin source file
2. Implement the required interfaces
3. Compile and link with NLM libraries
4. Test using the plugin mode

### Performance Tuning
Optimize performance using the benchmark mode:

```bash
nlm benchmark --neurons 10000 --steps 50000
```

### Debugging
Use debug mode to troubleshoot issues:

```bash
nlm debug --config debug.cfg --verbose
```

## Troubleshooting

### Common Issues

**Issue: Brain initialization fails**
- Check configuration file format
- Verify sufficient system resources
- Ensure required libraries are installed

**Issue: Integration verification fails**
- Check that all systems are properly linked
- Verify component dependencies
- Review integration test output

**Issue: Performance is slow**
- Consider reducing neuron count
- Check for memory leaks
- Optimize configuration parameters

**Issue: Memory usage is high**
- Reduce working memory capacity
- Disable unnecessary systems
- Monitor memory usage with system tools

### Getting Help

For additional help:

1. Use `nlm --help` for command-line help
2. Check the NLM documentation in the docs directory
3. Visit the NLM project website for more information
4. Submit issues to the NLM GitHub repository

## API Reference

### Core Classes

- **Brain**: Main neural system controller
- **Config**: Configuration system
- **Logger**: Logging system
- **RandomGenerator**: Random number generation
- **SimulationClock**: Time management

### Memory System Classes

- **NeuralWorkingMemory**: Working memory system
- **NeuralEpisodicMemory**: Episodic memory system
- **NeuralAssociativeMemory**: Associative memory system

### Neuromodulation System Classes

- **Dopamine**: Reward and reinforcement learning signal
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **PredictionError**: Prediction error signal

### Prediction System Classes

- **PredictionSystem**: Sensory prediction and error computation

### Cognition System Classes

- **NeuralPlanner**: Action planning
- **ConceptFormation**: Pattern discovery
- **AttentionalSelection**: Attentional focus

### Development System Classes

- **DevelopmentSystem**: Developmental stages and plasticity

### Core Processing Classes

- **Neuron**: Leaky Integrate-and-Fire neuron model
- **Synapse**: Synaptic connection with plasticity
- **NeuralRegion**: Brain region management
- **NeuralPopulation**: Neuron population management

### Utility Classes

- **SpikeSystem**: Event-driven spike processing
- **STDP**: Spike-Timing-Dependent Plasticity
- **Hebbian**: Hebbian learning
- **StructuralPlasticity**: Structural plasticity

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Authors

NLM Authors - Research project investigating computational brain-like systems.

---

*For more information, visit the NLM project website or consult the project documentation.*
