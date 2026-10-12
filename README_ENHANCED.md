# NLM (熙然) — Neural Learning Machine
## Comprehensive Documentation

The NLM (Neural Learning Machine) is an experimental artificial developmental brain system. This documentation covers the enhanced features and improvements made to the framework.

---

## Table of Contents

### 1. [Overview](#1-overview)
   - [What is NLM?](#what-is-nlm)
   - [Key Features](#key-features)
   - [Architecture Philosophy](#architecture-philosophy)

### 2. [Quick Start](#2-quick-start)
   - [Installation](#installation)
   - [Running Basic Examples](#running-basic-examples)
   - [Python Bindings](#python-bindings)

### 3. [Enhanced Python API](#3-enhanced-python-api)
   - [Context Managers](#context-managers)
   - [Convenience Factory Methods](#convenience-factory-methods)
   - [Enhanced Configuration](#enhanced-configuration)
   - [Batch Processing](#batch-processing)
   - [Error Handling](#error-handling)

### 4. [Enhanced Command-Line Interface](#4-enhanced-command-line-interface)
   - [Execution Modes](#execution-modes)
   - [Configuration Management](#configuration-management)
   - [Output Options](#output-options)
   - [Interactive Console](#interactive-console)

### 5. [Configuration System](#5-configuration-system)
   - [Configuration Schema](#configuration-schema)
   - [Validation](#validation)
   - [Defaults](#defaults)

### 6. [Visualization & Debugging](#6-visualization--debugging)
   - [Brain Visualization](#brain-visualization)
   - [Debug Console](#debug-console)
   - [Simulation Recorder](#simulation-recorder)
   - [Parameter Explorer](#parameter-explorer)
   - [Error Analyzer](#error-analyzer)

### 7. [Advanced Agent Features](#7-advanced-agent-features)
   - [Agent Architecture Designer](#agent-architecture-designer)
   - [Brain Architecture Designer](#brain-architecture-designer)
   - [Environment Simulator](#environment-simulator)
   - [Learning Experiment Framework](#learning-experiment-framework)
   - [Advanced Neural Models](#advanced-neural-models)

### 8. [Utility Tools](#8-utility-tools)
   - [Configuration Validation](#configuration-validation)
   - [Benchmarking](#benchmarking)
   - [Example Projects](#example-projects)
   - [Tutorials](#tutorials)

### 9. [Performance & Optimization](#9-performance--optimization)
   - [Memory Management](#memory-management)
   - [Parallel Processing](#parallel-processing)
   - [Profiling Tools](#profiling-tools)

### 10. [Migration Guide](#10-migration-guide)
   - [From Old to New API](#from-old-to-new-api)
   - [Upgrading Guide](#upgrading-guide)

---

## 1. Overview

### What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Key Features

- **Phase-Based Development**: 6 complete phases from basic neural computation to full integration
- **Real Neural Dynamics**: Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
- **Multiple Memory Systems**: Working, episodic, semantic, procedural, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error systems
- **Developmental Learning**: Age-dependent plasticity and maturation
- **Python Integration**: Comprehensive Python bindings via pybind11
- **Advanced Agent Framework**: Brain-world interface with sensory processing and motor control

### Architecture Philosophy

The NLM brain operates as a closed-loop system:

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

## 2. Quick Start

### Installation

```bash
# Clone the repository
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings (if using pip package)
# python -m pip install pynlm
```

### Running Basic Examples

#### Command Line

```bash
# Run Phase 6 integration demo
./nlm_phase6_demo

# Run Phase 2 basic tests
./nlm

# Run Phase 3 experiments
./nlm_phase3_demo

# Run Phase 4 experiments
./nlm_phase4_demo

# Run all tests
./tests
```

#### Python API

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think
for i in range(100):
    brain.step(i)

print(f"Brain generated {brain.getTotalSpikeCount()} spikes")

# Create a complete agent simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
```

## 3. Enhanced Python API

### Context Managers

Context managers ensure proper resource cleanup:

```python
# Configuration context manager
with pynlm.config_context() as config:
    config.set("neuron_count", 1000)
    config.set("seed", 42)
    brain = pynlm.createBrain(config)
    brain.initialize()

# Brain context manager  
with pynlm.brain_context() as brain:
    brain.initialize()
    for i in range(50):
        brain.step(i)
    print(f"Generated {brain.getTotalSpikeCount()} spikes")

# World context manager
with pynlm.world_context() as world:
    world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
    world.reset()
    # ... use world ...
```

### Convenience Factory Methods

```python
# Create default configuration
config = pynlm.create_config()

# Create configuration from file
config = pynlm.create_config_from_file("my_config.json")

# Create world with specific dimensions
world = pynlm.create_world_with_size(20, 20, 8, 8)

# Create brain with specific configuration
brain = pynlm.create_brain_with_config(config)

# Create agent brain
agent = pynlm.create_agent_brain(brain)
```

### Enhanced Configuration

```python
# Get and set values with type safety
config = pynlm.create_config()
config.set_int("random_seed", 42)
config.set_double("timestep", 0.001)
config.set_string("log_level", "info")

# Typed getters
seed = config.getInt("random_seed", 42)
timestep = config.getDouble("timestep", 0.001)
log_level = config.getString("log_level", "info")

# Configuration validation
if config.validate("neuron_count", "int"):
    count = config.getInt("neuron_count")

# Configuration merge from Python dict
config_data = {
    "neuron_count": 1000,
    "timestep": 0.01,
    "enable_curiosity": True
}
config.merge_from_dict(config_data)

# Get all configuration as dictionary
all_config = config.getAllAsDict()
```

### Batch Processing

```python
# Multiple simulation steps
brain.step_batch(100, 0.0)  # 100 steps starting at time 0.0

# Batch current injections
injections = [
    (brain.getNeuron(0), 10.0),
    (brain.getNeuron(10), 5.0),
    (brain.getNeuron(50), 15.0)
]
brain.injectCurrentBatch(injections)

# Complete simulation
brain.run_simulation(1000, world)
```

### Error Handling

```python
import pynlm

try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # This may raise exceptions
    brain.step(0)
    
except pynlm.NLMError as e:
    print(f"NLM Error: {e}")
except pynlm.ConfigError as e:
    print(f"Configuration Error: {e}")
except pynlm.BrainError as e:
    print(f"Brain Error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation Error: {e}")
```

## 4. Enhanced Command-Line Interface

### Execution Modes

```bash
# Phase 6 integration demo (default)
nlm_phase6_demo

# Run all test suites
tests

# Custom simulation with parameters
./nlm --mode sim --steps 500 --neurons 1000

# Batch simulations
./nlm --mode batch --config-list configs/*.cfg --output-dir results

# Interactive mode
./nlm --mode interactive

# Python embedding
./nlm --mode python

# Help information
./nlm --help
./nlm --mode help
```

### Configuration Management

```bash
# Load configuration from file
./nlm --config my_config.json

# Override configuration values
./nlm --set neuron_count=2000 --set timestep=0.01

# Show current configuration
./nlm --show-config

# Save current configuration
./nlm --save-config my_config.json
```

### Output Options

```bash
# Text output (default)
./nlm --mode demo

# JSON output
./nlm --mode demo --json --output results.json

# CSV output
./nlm --mode demo --csv --output results.csv

# Tabular output
./nlm --mode demo --table

# Verbose output
./nlm --mode demo --verbose

# Quiet output
./nlm --mode demo --quiet
```

### Runtime Options

```bash
# Number of simulation steps
./nlm --steps 1000

# Time step size
./nlm --timestep 0.01

# World dimensions
./nlm --world-width 50 --world-height 50

# Number of neurons
./nlm --neurons 2000

# Number of regions
./nlm --regions 3

# Enable/disable features
./nlm --enable-curiosity ./nlm --disable-plasticity
```

### Interactive Console

The interactive console provides real-time brain control:

```bash
# Start interactive mode
./nlm --mode interactive

# Available commands:
help                    Show all available commands
run [steps]             Run simulation for specified steps
status                  Show current brain status
config [key] [value]    Get or set configuration values
step [n]                Run n simulation steps
plot                    Generate visualizations
restart                 Reset brain to initial state
quit                    Exit interactive mode

# Example usage:
> status
Brain Status:
  Neurons: 500
  Regions: 1
  Total Synapses: 1245
  Active Neurons: 12
  Developmental Stage: CriticalPeriod

> step 100
Executed 100 simulation steps

> plot --type network --output brain_network.png
```

## 5. Configuration System

### Configuration Schema

The configuration system uses an INI-style schema with sections and key-value pairs. Here are the main configuration categories:

#### Core Simulation Parameters
- `seed`: Random seed for reproducible results
- `timestep`: Simulation time step size
- `simulation_steps`: Number of simulation steps to run

#### Neural Network Architecture
- `neuron_count`: Total number of neurons
- `region_count`: Number of brain regions
- `connection_probability`: Probability of synaptic connections

#### Neuron Dynamics
- `v_rest`: Resting membrane potential
- `v_thresh`: Threshold potential for spiking
- `v_reset`: Reset potential after spike
- `v_decay`: Membrane potential decay factor

#### Memory Systems
- `working_memory_enabled`: Enable working memory
- `working_memory_capacity`: Working memory capacity
- `episodic_memory_enabled`: Enable episodic memory
- `semantic_memory_enabled`: Enable semantic memory

#### Neuromodulation
- `dopamine_enabled`: Enable dopamine system
- `curiosity_enabled`: Enable curiosity system
- `novelty_enabled`: Enable novelty detection

#### Sensory Processing
- `vision_enabled`: Enable vision processing
- `vision_width`: Vision width in grid cells
- `vision_height`: Vision height in grid cells

#### Motor Control
- `action_enabled`: Enable action selection
- `max_actions`: Maximum number of actions

### Configuration Validation

Configuration validation ensures consistency and correctness:

```bash
# Validate configuration file
python scripts/validate_config.py configs/my_config.json

# Validate and fix issues
python scripts/validate_config.py configs/my_config.json --fix

# Generate configuration report
python scripts/validate_config.py configs/my_config.json --report
```

### Default Configuration

The default configuration provides sensible values for most use cases:

```python
{
    "core": {
        "seed": 42,
        "timestep": 0.001,
        "simulation_steps": 1000
    },
    "neuron": {
        "v_rest": -65.0,
        "v_thresh": -50.0,
        "v_reset": -65.0,
        "v_decay": 0.95
    },
    "memory": {
        "working_memory_enabled": True,
        "working_memory_capacity": 100,
        "episodic_memory_enabled": True,
        "semantic_memory_enabled": True
    },
    "neuromodulation": {
        "dopamine_enabled": True,
        "curiosity_enabled": True,
        "novelty_enabled": True
    }
}
```

## 6. Visualization & Debugging

### Brain Visualization

```bash
# Create brain visualizations
python scripts/viz/brain_viz.py --brain my_brain.bin --output brain_viz.png
python scripts/viz/brain_viz.py --brain my_brain.bin --mode network --output network.png
python scripts/viz/brain_viz.py --brain my_brain.bin --mode activity --output activity.png

# Real-time visualization
python scripts/viz/brain_viz.py --real-time --brain my_brain.bin --update-interval 100
```

### Debug Console

```bash
# Start debug console
python scripts/debug/debug_console.py

# Connect to running simulation
python scripts/debug/debug_console.py --host localhost --port 8080

# Debug with recording
python scripts/debug/debug_console.py --record session1.debug --breakpoint step 100
```

### Simulation Recorder

```bash
# Record simulation
python scripts/recorder/simulator_recorder.py --brain my_brain.bin --output session1.rec

# Replay simulation
python scripts/recorder/simulator_recorder.py --input session1.rec --speed 2.0

# Generate simulation report
python scripts/recorder/simulator_recorder.py --input session1.rec --report session1_report.html
```

### Parameter Explorer

```bash
# Explore parameter space
python scripts/explore/param_explorer.py --parameter-space config_space.json --output results.json

# Automated parameter search
python scripts/explore/param_explorer.py --brain my_brain.bin --method bayesian --iterations 100

# Visualize parameter effects
python scripts/explore/param_explorer.py --brain my_brain.bin --parameter learning_rate --output learning_rate_effects.png
```

### Error Analyzer

```bash
# Analyze simulation errors
python scripts/analyze/error_analyzer.py --brain my_brain.bin --output error_report.html

# Correlate errors with parameters
python scripts/analyze/error_analyzer.py --brain my_brain.bin --parameter learning_rate --error-patterns

# Generate error suggestions
python scripts/analyze/error_analyzer.py --brain my_brain.bin --suggestions
```

## 7. Advanced Agent Features

### Agent Architecture Designer

```bash
# Design custom agent architecture
python scripts/agent_builder.py --template explorer --output explorer_agent.json
python scripts/agent_builder.py --template planner --custom-params "max_actions=20,exploration_rate=0.3" --output custom_agent.json

# Optimize agent parameters
python scripts/agent_builder.py --brain my_brain.bin --optimize --method evolutionary --generations 50
```

### Brain Architecture Designer

```bash
# Design neural circuit architecture
python scripts/brain_designer.py --circuit-type recurrent --output recurrent_brain.json
python scripts/brain_designer.py --circuit-type modular --regions 3 --output modular_brain.json

# Analyze circuit performance
python scripts/brain_designer.py --brain my_brain.bin --analyze --output analysis.json
```

### Environment Simulator

```bash
# Create custom environment
python scripts/env_simulator.py --type grid --width 20 --height 20 --output grid_env.json
python scripts/env_simulator.py --type continuous --size 100 --output continuous_env.json

# Run multi-agent simulation
python scripts/env_simulator.py --multi-agent --agents 3 --env grid_env.json --output results.json
```

### Learning Experiment Framework

```bash
# Design and run learning experiment
python scripts/experiment_framework.py --type longitudinal --duration 1000 --output exp_results.json
python scripts/experiment_framework.py --type intervention --control-group --treatment-group --output comparison.json

# Analyze experimental results
python scripts/experiment_framework.py --input exp_results.json --analyze --output statistical_analysis.json
```

### Advanced Neural Models

```bash
# Simulate advanced neuron models
python scripts/advanced_neural.py --neuron-type adaptive-threshold --output adaptive_neuron_results.json
python scripts/advanced_neural.py --neuron-type spike-frequency-adaptation --parameters "tau=100,beta=0.5" --output sfa_results.json

# Compare neural models
python scripts/advanced_neural.py --compare --models "adaptive-threshold,spike-frequency-adaptation,regular-spiking" --output model_comparison.json
```

## 8. Utility Tools

### Configuration Validation

```bash
# Validate all configuration files
python scripts/validate_config.py --validate-all

# Generate configuration compliance report
python scripts/validate_config.py --report --output compliance_report.html
```

### Benchmarking

```bash
# Run performance benchmarks
python scripts/benchmark.py --brain my_brain.bin --config configs/benchmark.cfg --output benchmark_results.json

# Compare different configurations
python scripts/benchmark.py --config configs/config1.json configs/config2.json --output comparison.json

# Generate performance report
python scripts/benchmark.py --input benchmark_results.json --report --output performance_report.html
```

### Example Projects

The `scripts/examples/` directory contains comprehensive examples:

- `memory_learning.py` - Memory system usage examples
- `curiosity_agent.py` - Curiosity-driven learning
- `development_simulation.py` - Brain development simulation
- `world_navigation.py` - Agent navigation and pathfinding

### Tutorials

#### Beginner Tutorials
- [Introduction to NLM](tutorials/beginner.md#introduction-to-nlm)
- [Basic Python API](tutorials/beginner.md#basic-python-api)
- [Simple Simulations](tutorials/beginner.md#simple-simulations)

#### Intermediate Tutorials
- [Advanced Configuration](tutorials/intermediate.md#advanced-configuration)
- [Memory Systems](tutorials/intermediate.md#memory-systems)
- [Neuromodulation](tutorials/intermediate.md#neuromodulation)

#### Advanced Tutorials
- [Custom Neuron Models](tutorials/advanced.md#custom-neuron-models)
- [Neural Architecture Search](tutorials/advanced.md#neural-architecture-search)
- [Multi-Agent Systems](tutorials/advanced.md#multi-agent-systems)

## 9. Performance & Optimization

### Memory Management

The enhanced NLM framework includes several memory optimization techniques:

- **Memory Pools**: Reusable memory blocks for common objects
- **Event Queues**: Efficient event processing
- **Move Semantics**: Efficient object transfer
- **Reference Counting**: Automatic memory management

### Parallel Processing

NLM supports parallel processing for large-scale simulations:

```bash
# Enable parallel processing
./nlm --mode demo --parallel --threads 8

# Configure thread pool size
python scripts/benchmark.py --thread-count 16 --output performance.json
```

### Profiling Tools

```bash
# Profile simulation performance
python scripts/profiler/profile_simulation.py --brain my_brain.bin --output profile_results.html

# Analyze memory usage
python scripts/profiler/profile_simulation.py --brain my_brain.bin --memory --output memory_profile.json

# Generate performance report
python scripts/profiler/profile_simulation.py --brain my_brain.bin --report --output performance_report.html
```

## 10. Migration Guide

### From Old to New API

#### Configuration Changes

**Old API:**
```python
config = Config()
config.loadFromFile("config.json")
config.set("neuron_count", 500)
brain = Brain(config)
```

**New API:**
```python
config = pynlm.create_config_from_file("config.json")
config.setInt("neuron_count", 500)
brain = pynlm.create_brain_with_config(config)
```

#### Command Line Changes

**Old CLI:**
```bash
./nlm --config config.json --set neuron_count=500
```

**New CLI:**
```bash
./nlm --config config.json --set neuron_count=500
./nlm --mode sim --config config.json --set neuron_count=500
```

#### Python Function Changes

**Old API:**
```python
brain.step(step)
brain.injectCurrent(neuron, current)
brain.processSensoryInput(percept)
```

**New API:**
```python
brain.step(step)
brain.injectCurrent(neuron, current)
brain.run_simulation(100)
brain.injectCurrentBatch([(neuron, current)])
```

## Support and Community

For questions, bug reports, and feature requests:

- **GitHub Issues**: [https://github.com/nlm-project/nlm/issues](https://github.com/nlm-project/nlm/issues)
- **Documentation**: [https://nlm.readthedocs.io](https://nlm.readthedocs.io)
- **Discussions**: [https://github.com/nlm-project/nlm/discussions](https://github.com/nlm-project/nlm/discussions)

## License

MIT License. See `LICENSE` file for details.
