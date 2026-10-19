# NLM Python API - High-Level Convenience Features

## Overview

This document describes the high-level convenience methods and batch operations available in the NLM Python API. These features provide advanced users with powerful tools for efficient simulation management, batch processing, and experiment control.

## Context Managers

### BrainContextManager

The `BrainContextManager` provides RAII (Resource Acquisition Is Initialization) semantics for automatic brain lifecycle management.

**Purpose:**
- Ensures proper brain initialization and cleanup
- Provides exception-safe resource management
- Simplifies brain lifecycle management in complex applications

**Key Features:**
- Automatic brain initialization on construction
- Automatic brain reset on destruction
- Access to the managed brain instance
- Exception safety with proper cleanup

**Python Usage:**
```python
from pynlm import BrainContextManager, createDefaultConfig

# Configuration
config = createDefaultConfig()

# Automatic resource management
with BrainContextManager(config) as brainCtx:
    brain = brainCtx.getBrain()
    # Use the brain - automatically cleaned up on exit
    brain.logStatus()

# BrainContextManager automatically resets brain on exit
```

### SimulationRunner

The `SimulationRunner` provides context manager functionality for simulation loop management.

**Purpose:**
- Manages simulation state and lifecycle
- Provides controlled start/stop/step functionality
- Ensures proper cleanup of simulation resources

**Key Features:**
- Controlled simulation start/stop
- Single step execution
- Batch step execution
- State tracking and validation
- Automatic cleanup on destruction

**Python Usage:**
```python
from pynlm import SimulationRunner, createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain

# Setup configuration and components
config = createDefaultConfig()
brain = createBrain(config)
world = createSimpleWorld()
world.configure(100, 100, 64, 64)
agentBrain = createAgentBrain(brain)

# Create simulation runner
runner = SimulationRunner(brain, world, agentBrain)

# Start simulation
runner.start(100)
print(f"Simulation started: maxSteps=100, currentStep=0")

# Run simulation steps
for step in range(5):
    if not runner.step():
        break
    print(f"Step {runner.getCurrentStep()} completed")

# Stop simulation
runner.stop()
print(f"Simulation stopped after {runner.getCurrentStep()} steps")
```

## Batch Operations

### BatchOperations Class

The `BatchOperations` class provides static methods for efficient batch processing of brain simulations.

**Purpose:**
- Execute multiple steps efficiently
- Process multiple sensory inputs in batch
- Retrieve statistics for multiple steps
- Optimize performance for batch workloads

**Key Features:**
- Batch step execution with optional timestamps
- Batch sensory input processing
- Batch statistics retrieval
- Memory-efficient processing

**Python Usage:**
```python
from pynlm import BatchOperations, createDefaultConfig, createBrain, Vision, Audio, InternalSignals

# Setup
config = createDefaultConfig()
brain = createBrain(config)

# Batch step operations
steps = [1, 2, 3, 4, 5, 10, 20, 50]
times = [1.0, 1.1, 1.2, 1.3, 1.4, 2.0, 5.0, 10.0]

executed = BatchOperations.runSteps(brain, steps, times)
print(f"Executed {len(executed)} steps: {executed}")

# Batch sensory input processing
vision = Vision(64, 64, 3)
vision.setData([0.1] * 64 * 64 * 3)

audio = Audio(44100, 44100 // 50)  # 50ms of audio
audio.setData([0.0] * audio.getNumSamples())

internal = InternalSignals()
internal.addSignal(0.5)
internal.addSignal(0.8)

inputs = [vision, audio, internal]
processed = BatchOperations.processSensoryInputs(brain, inputs)
print(f"Processed {len(processed)} sensory inputs")

# Get batch statistics
stats = BatchOperations.getBatchStats(brain, 0, 50)
print(f"Batch statistics: {stats}")
```

## Convenience Methods

### ConvenienceMethods Class

The `ConvenienceMethods` class provides static methods for common workflows and convenience operations.

**Purpose:**
- Configure brain from Python dictionary
- Run quick simulations
- Execute complete agent episodes
- Simplify common workflows

**Key Features:**
- Brain configuration from dictionary
- Quick simulation setup and execution
- Complete agent episode management
- Return statistics for analysis

**Python Usage:**
```python
from pynlm import ConvenienceMethods, createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain

# Configure brain from Python dict
config_dict = {
    "neuronCount": "1000",
    "synapticConnectionProbability": "0.1",
    "maxFiringRate": "50.0",
    "enableSTDP": "true",
    "enableRewardModulation": "false"
}

config = createDefaultConfig()
ConvenienceMethods.configureBrain(config, config_dict)
print("Brain configured from dictionary")

# Quick simulation
print("Running quick simulation...")
quickStats = ConvenienceMethods.quickSimulation(500, 100)
print(f"Quick simulation stats: {quickStats}")

# Run complete agent episode
world = createSimpleWorld()
world.configure(200, 200, 128, 128)
world.setAgentStart(100, 100)

brain = createBrain(config)
agentBrain = createAgentBrain(brain)

episodeStats = ConvenienceMethods.runEpisode(agentBrain, world, 1000)
print(f"Episode statistics: {episodeStats}")
```

## Builder Pattern

### ConfigBuilder

The `ConfigBuilder` provides a fluent interface for constructing configurations.

**Purpose:**
- Create complex configurations with a readable API
- Support method chaining for configuration setup
- Validate configuration values during construction

**Key Features:**
- Method chaining for configuration setup
- Support for different data types (int, double, string, bool)
- Add values to existing configuration keys
- Build and return completed configuration

**Python Usage:**
```python
from pynlm import ConfigBuilder, createBrain

# ConfigBuilder example
config = (ConfigBuilder()
          .set("neuronCount", 2000)
          .set("synapseCount", 50000)
          .set("simulationSteps", 500)
          .set("enableDevelopment", "true")
          .set("enableCuriosity", "true")
          .add("learningRate", 0.01)
          .add("learningRate", 0.001)
          .build())

print("Configuration built using ConfigBuilder")

brain = createBrain(config)
```

### SimulationBuilder

The `SimulationBuilder` provides a fluent interface for setting up complete simulations.

**Purpose:**
- Create complete simulation environments
- Configure all simulation components
- Simplify simulation setup

**Key Features:**
- Configure world size and dimensions
- Set configuration from dictionary
- Attach brain and agent brain
- Build and return simulation components

**Python Usage:**
```python
from pynlm import SimulationBuilder, createBrain, createSimpleWorld, createAgentBrain

# ConfigBuilder example
config = (ConfigBuilder()
          .set("neuronCount", 2000)
          .set("synapseCount", 50000)
          .set("simulationSteps", 500)
          .set("enableDevelopment", "true")
          .set("enableCuriosity", "true")
          .add("learningRate", 0.01)
          .add("learningRate", 0.001)
          .build())

# SimulationBuilder example
brain = createBrain(config)
world = createSimpleWorld()
world.configure(256, 256, 256, 256)

agentBrain = createAgentBrain(brain)

simulation = (SimulationBuilder()
              .withWorldSize(256, 256)
              .withConfig({"test": "value"})
              .withBrain(brain)
              .withAgentBrain(agentBrain)
              .build())

print(f"Simulation built with world size {simulation[0].getWidth()}x{simulation[0].getHeight()}")
print(f"AgentBrain configured: {simulation[2] is not None}")
```

## Experiment Management

### ExperimentLogger

The `ExperimentLogger` provides utilities for recording simulation results and metadata.

**Purpose:**
- Record metrics and statistics
- Track simulation steps and performance
- Manage experiment notes and comments
- Retrieve experiment data for analysis

**Key Features:**
- Record metric values
- Record step data with brain statistics
- Add experiment notes and comments
- Retrieve experiment and metrics objects

**Python Usage:**
```python
from pynlm import ExperimentLogger, createDefaultConfig, createBrain

# Create experiment logger
logger = ExperimentLogger()

# Record metrics
logger.recordMetric("accuracy", 0.85)
logger.recordMetric("loss", 0.15)
logger.recordMetric("learning_rate", 0.001)

# Record step data
config = createDefaultConfig()
brain = createBrain(config)

for step in range(0, 100, 10):
    logger.recordStep(brain, step)

# Add notes
logger.addNote("Experiment started with 1000 neurons")
logger.addNote("Training completed successfully")

# Analyze results
experiment = logger.getExperiment()
analysis = ResultsAnalyzer.analyze(experiment)
print(f"Experiment analysis: {analysis}")

metrics = logger.getMetrics()
metricsAnalysis = ResultsAnalyzer.analyze(metrics)
print(f"Metrics analysis: {metricsAnalysis}")
```

### ResultsAnalyzer

The `ResultsAnalyzer` provides utilities for analyzing experiment results and metrics.

**Purpose:**
- Analyze experiment results
- Analyze metrics collections
- Compute statistical summaries
- Extract meaningful insights from data

**Key Features:**
- Analyze experiment step data
- Analyze metrics collections
- Compute statistical measures (mean, standard deviation, min, max)
- Return formatted analysis results

**Python Usage:**
```python
from pynlm import ResultsAnalyzer, Experiment, Metrics

# Analyze experiment results
experiment = Experiment("TestExperiment")
analysis = ResultsAnalyzer.analyze(experiment)
print(f"Experiment analysis: {analysis}")

# Analyze metrics collection
metrics = Metrics()
metricsAnalysis = ResultsAnalyzer.analyze(metrics)
print(f"Metrics analysis: {metricsAnalysis}")
```

## Advanced Workflow Example

This example demonstrates combining all new features for an advanced workflow:

```python
from pynlm import (
    ConfigBuilder, createBrain, createSimpleWorld, createAgentBrain,
    SimulationRunner, BatchOperations, ConvenienceMethods,
    ExperimentLogger, ResultsAnalyzer
)

# Build configuration
config = (ConfigBuilder()
          .set("neuronCount", 5000)
          .set("enableDevelopment", "true")
          .set("enableCuriosity", "true")
          .build())

# Create components
brain = createBrain(config)
world = createSimpleWorld()
world.configure(500, 500, 128, 128)
world.setAgentStart(250, 250)

agentBrain = createAgentBrain(brain)

# Setup experiment logger
logger = ExperimentLogger()
logger.addNote("Advanced workflow test started")

# Create simulation runner
runner = SimulationRunner(brain, world, agentBrain)
runner.start(200)

# Run simulation with batch operations
batchSteps = [i * 10 for i in range(1, 21)]  # Steps: 10, 20, 30, ..., 200
BatchOperations.runSteps(brain, batchSteps)

# Process batch sensory inputs
from pynlm import Vision
visionBatch = [Vision(128, 128, 3) for _ in range(5)]
for i, vis in enumerate(visionBatch):
    vis.setData([float(j) / 255.0 for j in range(128 * 128 * 3)])

BatchOperations.processSensoryInputs(brain, visionBatch)

# Run episode
episodeStats = ConvenienceMethods.runEpisode(agentBrain, world, 100)
logger.addNote(f"Episode completed: {episodeStats['steps_completed']} steps")

# Record final metrics
logger.recordMetric("final_accuracy", 0.95)
logger.recordMetric("training_time", 125.5)

# Analyze everything
experiment = logger.getExperiment()
finalAnalysis = ResultsAnalyzer.analyze(experiment)
print(f"Final experiment analysis: {finalAnalysis}")

runner.stop()
print("Advanced workflow completed successfully!")
```

## Performance Considerations

1. **Batch Operations**: Use `BatchOperations.runSteps()` for multiple steps to reduce overhead
2. **Context Managers**: Use `BrainContextManager` and `SimulationRunner` for automatic resource management
3. **Memory Efficiency**: Batch operations reduce memory allocation overhead
4. **Thread Safety**: High-level classes are not thread-safe; use appropriate synchronization when needed

## Error Handling

- Context managers ensure proper cleanup even if exceptions occur
- Batch operations continue processing even if individual steps fail
- Experiment logger gracefully handles missing data
- Builder pattern validates configuration values during construction

## Best Practices

1. **Use Context Managers**: Always use `BrainContextManager` and `SimulationRunner` for resource management
2. **Use Batch Operations**: Use `BatchOperations` for multiple steps and sensory inputs
3. **Leverage Builder Pattern**: Use `ConfigBuilder` and `SimulationBuilder` for complex setups
4. **Track Experiments**: Use `ExperimentLogger` and `ResultsAnalyzer` for reproducible research
5. **Combine Features**: Advanced workflows can combine multiple features for maximum efficiency

## Migration Guide

### From Traditional Approach to High-Level API

**Before:**
```python
# Traditional approach
config = createDefaultConfig()
brain = createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
    # Process sensory input
    # Decode motor command
    # Apply to world
```

**After:**
```python
# Using high-level features
config = createDefaultConfig()
with BrainContextManager(config) as brainCtx:
    brain = brainCtx.getBrain()
    
    # Run batch steps
    BatchOperations.runSteps(brain, range(100))
    
    # Process sensory inputs
    # ...
```

### Key Benefits

1. **Simpler Code**: Less boilerplate for common workflows
2. **Better Resource Management**: Automatic cleanup and exception safety
3. **Improved Performance**: Batch operations reduce overhead
4. **Enhanced Maintainability**: Builder pattern for complex configurations
5. **Better Experiment Tracking**: Integrated experiment management

## Troubleshooting

### Common Issues and Solutions

1. **Resource Leaks**: Always use context managers (`BrainContextManager`, `SimulationRunner`)
2. **Performance Issues**: Use batch operations instead of individual step calls
3. **Configuration Errors**: Use `ConfigBuilder` for method chaining and validation
4. **Experiment Tracking**: Use `ExperimentLogger` and `ResultsAnalyzer` for reproducible research

## Further Reading

- [NLM Python API Reference](https://nlm.readthedocs.io/en/latest/python_api.html)
- [High-Level Features Example](examples/high_level_features.py)
- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [Phase 6 Integration Documentation](docs/PHASE6_ARCHITECTURE.md)

## API Reference

For detailed API reference documentation, see:
- Python API documentation
- Individual class documentation
- Method-specific documentation

The high-level convenience methods and batch operations provide powerful tools for advanced users to work with the NLM in a more efficient and maintainable way.
