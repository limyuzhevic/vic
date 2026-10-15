# NLM Python Bindings Documentation

# Overview

This directory contains the Python bindings for the NLM (Neural Learning Machine) project. These bindings provide comprehensive access to the C++ neural simulation framework through Python, enabling users to create, run, and analyze neural simulations programmatically.

# Quick Start

## Installation

The NLM Python bindings are installed via pip as part of the `pynlm` package. If you have the source code, you can install it in development mode:

```bash
pip install -e .
```

This will compile the C++ extensions and install them as a Python package.

## Basic Usage

### Creating a Simple Simulation

```python
import nlm

# Create a default configuration
config = nlm.Config()

# Create a brain with the configuration
brain = nlm.Brain(config)

# Initialize the brain
brain.initialize()

# Run simulation
sim_step = 0
while sim_step < 100:
    brain.step(sim_step, sim_step * 0.1)
    sim_step += 1

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Running with Command Line Interface

```bash
# Run a basic simulation
nlm simulate --steps 100 --timestep 0.1

# Run with configuration file
nlm simulate --config config.json --steps 500

# Run an experiment
nlm experiment run --name "learning_test" --iterations 10
```

# Python Module Structure

## Core Classes

### Brain
The main neural simulation brain class. This is the central component that coordinates all neural systems.

**Key Methods:**
- `initialize()` - Initialize the brain with configuration
- `step(step, time)` - Perform a simulation step
- `receiveSensoryInput(input)` - Inject sensory input
- `produceAction()` - Produce motor output
- `save(filepath)` / `load(filepath)` - Save/load brain state

**Attributes:**
- `getTotalNeuronCount()` - Total number of neurons
- `getTotalSynapseCount()` - Total number of synapses
- `getActiveNeuronCount()` - Number of active neurons
- `getFiringNeuronCount()` - Number of currently firing neurons

### AgentBrain
Interface class that connects the NLM brain to a simulated world.

**Key Methods:**
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Convert neural activity to motor commands
- `applyRewardModulation(reward, predicted_reward)` - Apply reward signals

### Neuron
Represents a single neuron in the neural network.

**Key Properties:**
- `getId()` - Get neuron identifier
- `getType()` - Get neuron type (Excitatory, Inhibitory, etc.)
- `getMembranePotential()` - Get current membrane potential
- `getFiringState()` - Get current firing state
- `stepLIF(time, dt)` - Perform LIF simulation step

### Synapse
Represents a connection between neurons.

**Key Properties:**
- `getSource()` / `getTarget()` - Get source and target neurons
- `getWeight()` - Get synaptic weight
- `getDelay()` - Get transmission delay

## Memory Systems

### WorkingMemory
Short-term active information storage.

### EpisodicMemory
Storage of events and experiences with timestamps.

### AssociativeMemory
Relationships between representations.

## Plasticity Systems

### STDP (Spike-Timing-Dependent Plasticity)
Core learning mechanism based on spike timing.

### Hebbian
Classical Hebbian learning rule.

### StructuralPlasticity
Rules for creating new connections.

## Neuromodulation Systems

### Dopamine
Reward and reinforcement learning signal.

### Curiosity
Exploration motivation signal.

### Novelty
Novelty detection signal.

## Prediction System

### NeuralPrediction
Temporal prediction based on neural activity patterns.

### PredictionErrorSignal
Computes prediction error for learning.

## Configuration

### Config
Configuration management system.

**Key Methods:**
- `loadFromFile(filepath)` - Load configuration from JSON file
- `saveToFile(filepath)` - Save configuration to JSON file
- `has(key)` - Check if configuration key exists
- `getKeys()` - Get all configuration keys

# Advanced Features

## Real-Time Control

```python
from nlm import RealTimeController

controller = RealTimeController()
controller.start(brain)
controller.pause()
controller.resume()
controller.stop()
```

## Model Loading/Saving

```python
# Save brain state
brain.save("my_brain_state.nlm")

# Load brain state
brain.load("my_brain_state.nlm")
```

## Callback System

```python
# Register memory event callback
brain.setMemoryEventCallback(lambda memory: print(f"Memory event: {memory}"))

# Register prediction error callback
brain.setPredictionEventCallback(lambda error: print(f"Prediction error: {error}"))
```

## Performance Monitoring

```python
from nlm import PerformanceProfiler

profiler = PerformanceProfiler()
profiler.start()

# Run simulation
brain.step(step, time)

profiler.stop()

# Get performance statistics
stats = profiler.get_statistics()
```

## Visualization

```python
from nlm import Visualizer

visualizer = Visualizer()

# Create visualization
visualizer.create_network_plot(brain, "network_plot.png")
visualizer.create_activity_plot(brain, "activity_plot.png")
visualizer.create_connection_plot(brain, "connections_plot.png")
```

# Command-Line Interface (CLI)

The NLM CLI provides comprehensive command-line access to the simulation framework:

## Available Commands

### simulate
Run neural simulations with various options:

```bash
nlm simulate --steps 100 --timestep 0.1
nlm simulate --config config.json --steps 500 --save_checkpoint
nlm simulate --max_neurons 1000 --timestep 0.05
```

### config
Manage configuration files:

```bash
nlm config create --name my_config --output config.json
nlm config list
nlm config load --config config.json
nlm config save
nlm config reset
```

### experiment
Run experiments with multiple iterations:

```bash
nlm experiment run --name "learning_test" --iterations 10 --steps 50
nlm experiment analyze --input experiment_results.json
```

### profile
Performance profiling:

```bash
nlm profile --duration 60 --output profile.json
nlm profile --steps 1000 --output perf_data.json
```

### debug
Debugging tools:

```bash
nlm debug --type status
nlm debug --type memory
nlm debug --type plasticity
nlm debug --type all
```

### visualize
Generate visualizations and plots:

```bash
nlm visualize --output network_visualization.png
nlm visualize --type activity --output activity_plot.png
```

### export
Export simulation data:

```bash
nlm export results.npy --format numpy
nlm export data.json --format json
```

### import
Import models and data:

```bash
nlm import brain_state.nlm --format nlm
nlm import data.csv --format csv
```

### batch
Batch processing:

```bash
nlm batch run --iterations 10 --output batch_results/
nlm batch analyze --input batch_results/ --output summary.json
```

### analyze
Data analysis:

```bash
nlm analyze --input simulation_data.json --type statistics
nlm analyze --input batch_results.json --type comparison
```

### system
System control and management:

```bash
nlm system status
nlm system stats
nlm system log
```

# Development and Testing

## Running Tests

To run the Python tests, use:

```bash
pytest
```

## Building from Source

If you need to rebuild the C++ extensions:

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j4
```

## Advanced Development

### Adding New Python Bindings

To add new bindings to the Python module, modify the `python/bindings.cpp` file:

1. Include the appropriate C++ header
2. Use pybind11 to expose the C++ class
3. Add the class to the `PYBIND11_MODULE(pynlm, m)` section
4. Rebuild the module

### Extending the CLI

To add new CLI commands, create new command classes in `nlm_cli.py` and add them to the `COMMANDS` dictionary.

# Troubleshooting

## Common Issues

### "ModuleNotFoundError: No module named 'nlm'"

Make sure you have installed the package:

```bash
pip install -e .
```

### "ImportError: libnlm.so: cannot open shared object file"

Make sure the C++ extensions are in your library path:

```bash
export LD_LIBRARY_PATH=/path/to/nlm/lib:$LD_LIBRARY_PATH
```

### "ModuleNotFoundError: cannot import name 'SomeClass'"

Check if the class is properly exported in the C++ bindings. You may need to rebuild the module.

## Performance Tips

1. Use batch operations when processing multiple neurons or synapses
2. Pre-allocate memory for large simulations
3. Use efficient data structures for storing large datasets
4. Consider using the `RealTimeController` for interactive simulations

# References

1. NLM Documentation: https://nlm.readthedocs.io
2. NLM GitHub Repository: https://github.com/nlm-project/nlm
3. Pybind11 Documentation: https://pybind11.readthedocs.io
4. scikit-build-core Documentation: https://scikit-build-core.readthedocs.io

# License

MIT License. See the LICENSE file for details.
