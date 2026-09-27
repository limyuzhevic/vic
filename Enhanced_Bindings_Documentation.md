# Enhanced NLM Python API Documentation

This document describes the enhanced Python bindings for the NLM (Neural Learning Machine) framework.

## Overview

The enhanced NLM Python bindings provide a Pythonic interface to the C++ NLM neural simulation framework. The improvements include:

- **Context Managers**: Automatic resource management using `with` statements
- **Fluent API**: Method chaining for common operations
- **Property-Style Access**: Easy access to brain state and configuration
- **Configuration Builders**: Factory methods for common configuration scenarios
- **Simulation Runner**: Helper for running complete experiments
- **Visualization Helpers**: Tools for data analysis and visualization
- **Enhanced Error Handling**: Custom exceptions with descriptive messages
- **Type Checking**: Validation utilities for input parameters

## Key Features

### 1. Context Managers

The enhanced bindings support context managers for automatic resource management:

```python
import pynlm

# Using context manager for automatic cleanup
with pynlm.PythonBrain(pynlm.createBrain(pynlm.createDefaultConfig())) as brain:
    brain.initialize()
    brain.step(0)
    print(f"Neurons: {brain.getNeuronCount()}")
    # Brain is automatically reset when exiting the context
```

### 2. Fluent API

Many operations support method chaining for more readable code:

```python
# Fluent API for brain operations
brain.initialize() \
     .step(0) \
     .step(1) \
     .step(2)

# Fluent API for world operations
world.configure(20, 20, 8, 8) \
     .setStart(10, 10) \
     .reset() \
     .update(0.1)
```

### 3. Property-Style Access

Easy access to brain state through properties:

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"Neuron count: {brain.getNeuronCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

### 4. Configuration Builders

Create common configurations with ease:

```python
# Basic configuration for testing
config = pynlm.ConfigBuilder().setBasicConfig().build()

# Advanced configuration for research
config = pynlm.ConfigBuilder().setAdvancedConfig().build()

# Exploration configuration for learning
config = pynlm.ConfigBuilder().setExplorationConfig().build()

# Production configuration for stability
config = pynlm.ConfigBuilder().setProductionConfig().build()

# Custom configuration
config = pynlm.ConfigBuilder() \
    .setBasicConfig() \
    .setCustom("learning_rate", 0.05) \
    .setCustom("neuron_count", 100) \
    .build()
```

### 5. Simulation Runner

Run complete simulations with ease:

```python
import pynlm

runner = pynlm.SimulationRunner()

# Run a simple simulation
results = runner.runSimulation(
    brain, world, agent,
    numSteps=100,
    timestep=0.1,
    enableLearning=True
)

# Process results
for step in results:
    print(f"Step {step.step}: Reward={step.reward}, Success={step.success}")
```

### 6. Visualization Helpers

Create plots and analysis tools:

```python
import pynlm

viz = pynlm.VisualizationHelper()

# Create firing rate plot
times = [i * 0.1 for i in range(100)]
rates = [0.1 * (i % 10) / 10 for i in range(100)]
plot = viz.createFiringRatePlot(times, rates)

# Create spike count plot
spike_counts = [i * 2 for i in range(100)]
plot = viz.createSpikeCountPlot(times, spike_counts)

# String encoding helpers
text = viz.toUtf8("Hello World")
bytes_data = viz.toBytes("data")
```

## Enhanced Classes

### PythonBrain

Enhanced interface to the Brain class with context manager support:

```python
brain = pynlm.PythonBrain(pynlm.createBrain(pynlm.createDefaultConfig()))

with brain:
    brain.initialize()
    
    # Use fluent API
    brain.step(0).step(1).step(2)
    
    # Access properties
    print(f"Neurons: {brain.getNeuronCount()}")
    print(f"Active: {brain.getActiveNeuronCount()}")
    
    # Run multiple steps
    brain.runSteps(100, 0.1)
    
    # Inject currents
    brain.injectCurrent(neuron_id, 1.0)
    brain.injectCurrentToType(NeuronType.Excitatory, 0.5)
```

**Properties:**
- `getNeuronCount()`: Total number of neurons
- `getSynapseCount()`: Total number of synapses
- `getActiveNeuronCount()`: Number of active neurons
- `getFiringNeuronCount()`: Number of firing neurons
- `getAverageFiringRate()`: Average firing rate in Hz
- `getExcitationInhibitionRatio()`: Excitation/inhibition ratio
- `getTotalSpikeCount()`: Total spike count
- `getDevelopmentalStage()`: Current developmental stage

### PythonWorld

Enhanced interface to the SimpleWorld class:

```python
world = pynlm.PythonWorld(pynlm.createSimpleWorld())

with world:
    world.configure(20, 20, 8, 8) \
         .setStart(10, 10) \
         .reset()
    
    # Update world
    world.update(0.1)
    
    # Get state
    percept = world.getPercept()
    agent_body = world.getAgentBody()
    
    print(f"Position: ({agent_body.x}, {agent_body.y})")
    print(f"Time: {world.getSimulationTime()}")
```

### PythonAgentBrain

Enhanced interface to the AgentBrain class:

```python
agent = pynlm.PythonAgentBrain(pynlm.createAgentBrain(brain))

with agent:
    agent.initialize(world)
    
    # Process input
    agent.processInput(percept)
    
    # Get command
    action = agent.decodeCommand()
    
    # Access properties
    print(f"Developmental stage: {agent.getDevelopmentalStage()}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.2f}")
    
    # Enable features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
```

### ConfigBuilder

Build common configurations:

```python
# Basic configuration
config = pynlm.ConfigBuilder().setBasicConfig().build()

# Advanced configuration
config = pynlm.ConfigBuilder().setAdvancedConfig().build()

# Exploration configuration
config = pynlm.ConfigBuilder().setExplorationConfig().build()

# Production configuration
config = pynlm.ConfigBuilder().setProductionConfig().build()

# Custom configuration
config = pynlm.ConfigBuilder() \
    .setCustom("learning_rate", 0.01) \
    .setCustom("threshold", 1.0) \
    .build()
```

### SimulationRunner

Run complete simulations:

```python
runner = pynlm.SimulationRunner()

# Run simulation with callback
runner.runWithCallback(
    brain, world, agent,
    numSteps=50,
    timestep=0.1,
    callback=lambda step, time, percept, action, result: print(f"Step {step}: {result.reward}")
)

# Capture visualization data
data = runner.captureVisualizationData(brain, maxNeurons=50)
```

### VisualizationHelper

Create visualization and analysis tools:

```python
viz = pynlm.VisualizationHelper()

# Create plots
rate_plot = viz.createFiringRatePlot(times, rates)
spike_plot = viz.createSpikeCountPlot(times, spike_counts)

# Create animation frames
frames = viz.createAnimationFrames(brain, numFrames=100)

# String encoding
utf8_text = viz.toUtf8("Hello")
bytes_data = viz.toBytes("data")
```

## Error Handling

The enhanced bindings include custom exceptions:

```python
import pynlm

# Base exception
raise pynlm.NLMException("Something went wrong")

# Configuration error
raise pynlm.ConfigurationError("Invalid configuration")

# Simulation error
raise pynlm.SimulationError("Simulation failed")

# Initialization error
raise pynlm.InitializationError("Failed to initialize")
```

## Type Checking

Validate inputs with type checking helpers:

```python
# Validate NeuronId
pynlm.validateNeuronId(neuron_id)

# Validate SynapseId
pynlm.validateSynapseId(synapse_id)

# Validate RegionId
pynlm.validateRegionId(region_id)

# Validate Config
pynlm.validateConfig(config)

# Validate Brain
pynlm.validateBrain(brain)

# Validate World
pynlm.validateWorld(world)

# Validate Agent
pynlm.validateAgent(agent)
```

## Backward Compatibility

The enhanced bindings maintain backward compatibility with the original API:

```python
# Original API still works
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

# Enhanced API available
py_brain = pynlm.PythonBrain(brain)
with py_brain:
    py_brain.initialize()
    py_brain.step(0)
```

## Module Metadata

The module includes version information:

```python
import pynlm

print(f"Version: {pynlm.__version__}")
print(f"API Version: {pynlm.__api_version__}")
```

## Example Scripts

See the `examples/` directory for comprehensive example scripts:

- `enhanced_api_example.py`: Complete examples of all features
- `basic_example.py`: Simple usage examples
- `simulation_example.py`: Simulation examples
- `configuration_example.py`: Configuration examples

## Building and Installation

The bindings are built using scikit-build with CMake:

```bash
# Build the module
mkdir build
cd build
cmake ..
make -j4

# Install (if needed)
sudo make install
```

Or using pip:

```bash
pip install -e .
```

## References

For more information about the underlying NLM framework, see:

- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM Architecture](docs/ARCHITECTURE.md)
- [NLM Science](docs/SCIENCE.md)

## Changelog

### Version 0.1.0 (Enhanced)
- Added context manager support
- Added fluent API
- Added property-style access
- Added configuration builders
- Added simulation runner
- Added visualization helpers
- Added enhanced error handling
- Added type checking
- Added Unicode string support
- Maintained backward compatibility

## License

MIT License

Copyright (c) 2024 NLM Authors
