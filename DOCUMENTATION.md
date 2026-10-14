# NLM Documentation

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

This documentation covers the improvements made to the codebase, including enhanced logging, configuration management, neural architecture, Python bindings, and high-level APIs.

## Table of Contents

1. [Improved Logging System](#improved-logging-system)
2. [Enhanced Configuration Management](#enhanced-configuration-management)
3. [Refactored Neural Architecture](#refactored-neural-architecture)
4. [Enhanced Python Bindings](#enhanced-python-bindings)
5. [High-Level APIs](#high-level-apis)
6. [Examples and Tutorials](#examples-and-tutorials)
7. [Migration Guide](#migration-guide)

## Improved Logging System

The logging system has been completely refactored with the following improvements:

### Key Features

1. **Multiple Log Levels**: Debug, Info, Warning, Error, Critical
2. **Color Support**: ANSI color codes for console output
3. **File Output**: Log to file in addition to console
4. **Thread Safety**: Thread-safe logging operations
5. **Log Formatting**: Customizable log format and timestamps
6. **Log Filtering**: Filter logs by level, source file, function
7. **Statistics**: Log usage statistics and patterns

### Usage Examples

```python
import pynlm

# Basic logging
pynlm.Logger.info("Starting neural network training")
pynlm.Logger.debug("Training step 1")
pynlm.Logger.warning("High error rate detected")

# File logging
logger = pynlm.Logger()
console_logger = pynlm.ConsoleLogger()
console_logger.setOutputFile("brain_simulation.log")
logger.addLogger(console_logger)
pynlm.Logger.setGlobal(logger)

# Custom logger
logger = pynlm.Logger()
logger.setLevel(pynlm.LogLevel.Debug)
logger.addLogger(console_logger)
```

### Advanced Features

```python
# Create logger with custom settings
logger = pynlm.Logger()
logger.addLogger(console_logger, level=pynlm.LogLevel.Info)

# Log with file/line/function information
def train_model():
    pynlm.Logger.info("Training model", __file__, __line__, __func__)
    # This adds source location information to the log

# Log with stream style
pynlm.Logger.debug("Training metrics: ", metrics)
```

## Enhanced Configuration Management

The configuration system now supports JSON, YAML, and key-value formats with the following improvements:

### Key Features

1. **Multiple Formats**: JSON, YAML, and key-value formats
2. **Type Safety**: Strong type checking for configuration values
3. **Validation**: Configuration schema validation
4. **Environment Variables**: Environment variable substitution
5. **Inheritance**: Configuration inheritance support
6. **Persistence**: Save/load configuration from files
7. **Command Line**: Command-line argument parsing

### Usage Examples

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("random_seed", 42)
config.set("connection_probability", 0.1)

# Load from JSON file
config = pynlm.Config()
if config.loadFromFile("config.json"):
    print("Configuration loaded successfully")

# Load from command line arguments
config.loadFromArgs(["--neuron_count", "500", "--random_seed", "42"])

# Save to YAML file
config.saveToFile("config.yaml")

# Get configuration values
neuron_count = config.getOr("neuron_count", 1000)
connection_prob = config.getOr("connection_probability", 0.1)
```

### Advanced Configuration

```python
# Nested configuration (JSON/YAML style)
config.set("brain", {
    "neuron_count": 1000,
    "regions": [
        {"name": "cortex", "neurons": 500},
        {"name": "subcortex", "neurons": 500}
    ],
    "plasticity": {
        "stdp": {"learning_rate": 0.01},
        "hebbian": {"strength": 0.1}
    }
})

# Configuration validation
schema = {
    "neuron_count": {"type": "int", "min": 1, "max": 10000},
    "random_seed": {"type": "int", "min": 0},
    "connection_probability": {"type": "float", "min": 0, "max": 1}
}

for key, value in config.getKeys():
    if not validate_config(key, value, schema):
        print(f"Invalid configuration: {key} = {value}")
```

## Refactored Neural Architecture

The neural brain architecture has been refactored with the following improvements:

### Key Improvements

1. **Separation of Concerns**: Each brain system has a single responsibility
2. **Modular Design**: Systems can be developed and tested independently
3. **Reduced Complexity**: Complex step logic broken into manageable pieces
4. **Extensibility**: New systems can be added without modifying existing code
5. **Maintainability**: Clear boundaries between systems

### System Architecture

```
BrainOrchestrator (Coordinator)
├── NeuronalSystem (LIF dynamics, spikes)
├── MemorySystem (Working, Episodic, Associative)
├── PredictionSystem (Sensory prediction, error)
├── NeuromodulationSystem (Dopamine, curiosity, novelty)
├── PlasticitySystem (STDP, Hebbian, structural)
├── DevelopmentSystem (Growth, maturation, aging)
├── CognitionSystem (Planning, concept formation, attention)
└── CheckpointSystem (Persistence, recovery)
```

### Neural System Design

#### NeuronalSystem
- Handles Leaky Integrate-and-Fire (LIF) neuron dynamics
- Manages event-driven spike propagation
- Implements synaptic plasticity rules

#### MemorySystem
- **Working Memory**: Transient active information storage
- **Episodic Memory**: Experience storage and replay
- **Associative Memory**: Pattern associations and recall

#### PredictionSystem
- Sensory prediction and error computation
- Forward models for action planning
- Confidence tracking and uncertainty

#### NeuromodulationSystem
- **Dopamine**: Reward prediction error and reinforcement
- **Curiosity**: Exploration motivation and novelty detection
- **Novelty**: Surprise detection and learning triggers

#### PlasticitySystem
- **STDP**: Spike-Timing Dependent Plasticity
- **Hebbian**: Activity-dependent synaptic strengthening
- **Structural Plasticity**: Synaptogenesis and pruning

#### DevelopmentSystem
- **Developmental Stages**: Initial, Critical Period, Maturation, Adult, Aging
- **Plasticity Modulation**: Plasticity rates change with development
- **Structural Changes**: Dendritic growth, pruning, and connectivity

#### CognitionSystem
- **Neural Planner**: Action sequence evaluation and planning
- **Concept Formation**: Pattern discovery and categorization
- **Attention**: Competitive selection and focus

### Usage Examples

```python
import pynlm

# Create brain with configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 3)
config.set("connection_probability", 0.1)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Check for completion
    if brain.getTotalSpikeCount() > 100:
        break

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Memory system access
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Neuromodulation access
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
```

## Enhanced Python Bindings

The Python bindings have been enhanced with convenience methods and improved usability:

### Key Improvements

1. **Pythonic API**: Methods use snake_case instead of camelCase
2. **Context Managers**: Automatic resource management with `with` statements
3. **Properties**: Easy access to statistics via properties
4. **Convenience Methods**: High-level methods for common operations
5. **Error Handling**: Pythonic exceptions with helpful messages
6. **Type Hints**: Improved type annotations
7. **Documentation**: Comprehensive docstrings with examples

### Basic Usage

```python
import pynlm

# Create brain with Pythonic API
brain_config = {
    "random_seed": 42,
    "neuron_count": 1000,
    "region_count": 3,
    "connection_probability": 0.1
}

brain = pynlm.Brain(brain_config)
brain.initialize()

# Get statistics (using properties)
print(f"Total neurons: {brain.total_neuron_count}")
print(f"Total synapses: {brain.total_synapse_count}")
print(f"Firing rate: {brain.average_firing_rate:.4f}")

# Run simulation
for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.total_spike_count} spikes")

# Send sensory input
sensory_data = [0.5, 0.8, 0.3, 0.9, 0.2]
brain.receive_sensory_input(sensory_data)

# Get action
action = brain.produce_action()
print(f"Action: {action}")
```

### Advanced Usage

```python
# Use context manager for automatic cleanup
with pynlm.brain_context(neuron_count=500) as brain:
    brain.initialize()
    
    # Do some work
    for step in range(50):
        brain.step(step)
        
    # Context automatically cleans up

# Quick start with default configuration
brain = pynlm.quick_start(neuron_count=1000, regions=2)
brain.initialize()

# Save/load brain state
brain.save("my_brain.json")
loaded_brain = pynlm.createBrainFromFile("my_brain.json")

# Experiment runner for automated testing
experiment = pynlm.ExperimentRunner(brain)
experiment.run_episode(
    steps=100,
    reward_threshold=0.8,
    save_results=True
)
```

## High-Level APIs

The high-level APIs provide machine learning-style interfaces for neural networks:

### NeuralNetwork Class

```python
from examples.high_level_api import NeuralNetwork, NeuralNetworkConfig

# Create neural network with configuration
config = NeuralNetworkConfig(
    name="MyNN",
    neuron_count=1000,
    region_count=3,
    connection_probability=0.1,
    epochs=100,
    learning_rate=0.01
)

# Build and train
nn = NeuralNetwork(config).build()

# Generate training data
X = [{'features': [random.random() for _ in range(10)]} for _ in range(100)]
y = [sum(x['features']) / 10 for x in X]

# Train
nn.fit(X, y, epochs=50)

# Make predictions
predictions = nn.predict([{'features': [0.1, 0.2, 0.3]}])
print(f"Prediction: {predictions[0]:.4f}")

# Evaluate
metrics = nn.evaluate(X, y)
print(f"Accuracy: {metrics['accuracy']:.4f}")
print(f"Loss: {metrics['loss']:.4f}")

# Save model
nn.save("my_model.json")

# Load model
loaded_nn = NeuralNetwork().load("my_model.json")
```

### Quick Start Example

```python
from examples.high_level_api import quick_start, benchmark_networks

# Quick start with minimal configuration
nn = quick_start(neuron_count=500, regions=2, epochs=20)
nn.fit(training_data, training_labels)

# Benchmark multiple configurations
configs = [
    NeuralNetworkConfig(name="Small", neuron_count=200),
    NeuralNetworkConfig(name="Medium", neuron_count=500),
    NeuralNetworkConfig(name="Large", neuron_count=1000)
]

results = benchmark_networks(configs, training_data, training_labels, n_runs=3)

for result in results:
    print(f"{result['config'].name}: {result['avg_accuracy']:.4f} accuracy")
```

## Examples and Tutorials

Several example files are included to demonstrate the features:

### Basic Examples

1. **high_level_api.py**: Complete neural network implementation
2. **example_high_level_api.py**: Demonstration of all high-level APIs

### Advanced Examples

1. **Phase 6 Demo**: Integration test for all brain systems
2. **Experiment Runner**: High-level experiment management
3. **Configuration Management**: JSON/YAML configuration examples

### Tutorial Files

1. **TUTORIAL.md**: Step-by-step tutorial for getting started
2. **API_REFERENCE.md**: Complete API reference
3. **EXAMPLES.md**: Collection of practical examples

## Migration Guide

### From C++ to Python

If you're migrating from C++ to Python, here are the key changes:

```python
# C++ (before)
auto brain = std::make_shared<Brain>(config);
brain->initialize();
for (int step = 0; step < 100; ++step) {
    brain->step(step);
}

# Python (after)
import pynlm

brain = pynlm.createBrain(config)
brain.initialize()
for step in range(100):
    brain.step(step)
```

### Configuration Changes

```python
# C++ (old format)
config->set("neuron_count", 1000, ConfigSource::Default);
config->set("random_seed", 42, ConfigSource::CommandLine);

# Python (new format)
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("random_seed", 42)
```

### Logging Changes

```python
# C++ (before)
NLM_LOG_INFO("Starting simulation");

# Python (new)
import pynlm
pynlm.Logger.info("Starting simulation")

# File logging
logger = pynlm.Logger()
console_logger = pynlm.ConsoleLogger()
console_logger.setOutputFile("simulation.log")
logger.addLogger(console_logger)
```

## Conclusion

The NLM project has been significantly improved with:

1. **Better Logging**: Enhanced logging system with multiple features
2. **Robust Configuration**: JSON/YAML support with validation
3. **Cleaner Architecture**: Modular design with separation of concerns
4. **Pythonic Bindings**: User-friendly Python API
5. **High-Level APIs**: Machine learning-style interfaces
6. **Comprehensive Documentation**: Examples and tutorials

These improvements make NLM more accessible to users, easier to maintain, and more extensible for future development.

## References

- [NLM Documentation](https://github.com/nlm-project/nlm)
- [Python Bindings](https://github.com/nlm-project/nlm-python)
- [Phase 6 Integration](docs/PHASE6_FINAL_AUDIT.md)
- [Architecture Documentation](docs/ARCHITECTURE.md)
- [Scientific Background](docs/SCIENCE.md)