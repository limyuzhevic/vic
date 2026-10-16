# NLM Python Bindings - User Guide

## Overview

The NLM (Neural Learning Machine) Python bindings provide a high-level interface to the NLM C++ neural simulation framework. This guide covers everything you need to get started with using NLM from Python, from basic setup to advanced usage patterns.

## Installation

### Building from Source

The Python bindings are built as part of the main NLM build system. To build and install the Python bindings:

```bash
# Create build directory and configure CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python3)

# Build all targets (including Python bindings)
make -j4

# Install (optional, requires sudo)
sudo make install
```

### Quick Start for Development

If you want to use the Python bindings without installing system-wide, you can build just the Python module:

```bash
mkdir build
cd build
cmake .. -DPYTHON_EXECUTABLE=$(which python3)
make pynlm
```

The Python module will be created in `build/pynlm.so` (or `.pyd` on Windows).

## Basic Usage Examples

### 1. Creating a Simple Brain

```python
import nlm

# Create default configuration
config = nlm.create_default_config()

# Create a brain with the configuration
brain = nlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Print brain status
brain.log_status()
```

### 2. Running a Basic Simulation

```python
import nlm
import time

# Create demo configuration and brain
config = nlm.create_demo_config()
brain = nlm.create_simple_demo_brain(config)

# Create a world
world = nlm.create_simple_demo_world()

# Run a simulation
print("Running simulation for 1000 steps...")
result = nlm.run_simulation(brain, world, 1000)
print(f"Simulation completed: {result}")

# Check brain statistics
stats = brain.get_statistics()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total spikes: {stats['total_spikes']}")
```

### 3. Working with Sensory Input

```python
import nlm

# Create brain and world
brain = nlm.createBrain(nlm.create_default_config())
brain.initialize()

world = nlm.create_simple_demo_world()

# Get sensory perception
percept = world.getSensoryPercept()

# Convert to dictionary for easy inspection
percept_dict = percept.to_dict()
print("Vision data:", percept_dict['vision'])
print("Touch data:", percept_dict['touch'])
print("Internal signals:", percept_dict['internal'])
```

### 4. Creating and Modifying Actions

```python
import nlm

# Create an action
action = nlm.ActionType.MoveForward

# Create Action object with parameters
action_obj = nlm.Action(action)
action_obj.add_param(1.0)  # Move forward distance
action_obj.add_param(0.5)  # Speed

# Modify action parameters
action_obj.set_param(0, 2.0)  # Change distance to 2.0

# Convert to dictionary for analysis
action_dict = action_obj.to_dict()
print("Action:", action_dict)
```

### 5. Saving and Loading Brain States

```python
import nlm
import os

# Create and run a brain
brain = nlm.createBrain(nlm.create_default_config())
brain.initialize()

# Save brain state
save_file = "my_brain.nlm"
success = nlm.save_brain_state(save_file, brain)
if success:
    print(f"Brain saved to {save_file}")

# Load brain state (with new config)
new_config = nlm.create_default_config()
loaded_brain = nlm.load_brain_state(save_file, new_config)
print("Brain loaded successfully")
```

### 6. Configuration Management

```python
import nlm

# Create and modify configuration
config = nlm.create_default_config()

# Use Pythonic convenience methods
config.set_int("brain.neuron_count", 2000)
config.set_float("world.width", 50.0)
config.set_bool("simulation.verbose", True)

# Get values with defaults
neuron_count = config.get_int("brain.neuron_count", 1000)
width = config.get_float("world.width", 10.0)
verbose = config.get_bool("simulation.verbose", False)

print(f"Neuron count: {neuron_count}")
print(f"World width: {width}")
print(f"Verbose mode: {verbose}")

# Validate configuration
errors = config.validate([
    "brain.neuron_count",
    "brain.simulation_steps",
    "world.width",
    "world.height"
])

if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
else:
    print("Configuration is valid!")
```

### 7. Agent-Based Simulation

```python
import nlm

# Create brain and world
config = nlm.create_default_config()
brain = nlm.create_simple_demo_brain(config)
world = nlm.create_simple_demo_world()

# Create agent
agent = nlm.create_agent(brain, world)
agent.initialize(world)

# Run agent episode
print("Running agent episode...")
for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Simulate world step
    world.update(0.01)
    
    # Get brain action
    action = brain.produceAction()
    
    # Execute action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check for episode end
    if step % 10 == 0:
        print(f"Step {step}: Active neurons = {brain.getActiveNeuronCount()}")

print("Agent episode completed!")
```

### 8. Advanced Usage Patterns

```python
import nlm
import json

# Create a complex simulation with multiple scenarios
def run_research_simulation():
    """Run a research simulation studying neural plasticity."""
    
    # Experiment 1: High plasticity
    print("=== Experiment 1: High Plasticity ===")
    config1 = nlm.create_demo_config()
    config1.set_float("plasticity.learning_rate", 0.1)
    brain1 = nlm.createBrain(config1)
    brain1.initialize()
    
    # Experiment 2: Low connectivity
    print("=== Experiment 2: Low Connectivity ===")
    config2 = nlm.create_demo_config()
    config2.set_float("brain.connection_probability", 0.01)
    brain2 = nlm.createBrain(config2)
    brain2.initialize()
    
    # Experiment 3: Fast development
    print("=== Experiment 3: Fast Development ===")
    config3 = nlm.create_demo_config()
    config3.set_int("brain.development_rate", 3)  # Development stage advances faster
    brain3 = nlm.createBrain(config3)
    brain3.initialize()
    
    return [brain1, brain2, brain3]

# Run research simulation
brains = run_research_simulation()
print(f"Created {len(brains)} research brains")
```

### 9. Performance Profiling

```python
import nlm

# Create brain
brain = nlm.createBrain(nlm.create_default_config())
brain.initialize()

# Profile performance
print("Profiling brain performance...")
profile_result = nlm.profile_brain(brain, 1000)
print(f"Performance results:")
print(f"  Total steps: {profile_result['total_steps']}")
print(f"  Elapsed time: {profile_result['elapsed_time_ms']:.2f} ms")
print(f"  Steps per second: {profile_result['steps_per_second']:.2f}")
print(f"  Average time per step: {profile_result['avg_time_per_step_ms']:.4f} ms")
```

## API Reference

### Core Classes

#### Brain
- **Brain(config)**: Creates a new brain with configuration
- **brain.initialize()**: Initializes the brain with configuration
- **brain.step(step)**: Performs a simulation step
- **brain.receiveSensoryInput(input)**: Injects sensory input
- **brain.produceAction()**: Produces motor action
- **brain.save(filepath)**: Saves brain state
- **brain.load(filepath)**: Loads brain state
- **brain.get_statistics()**: Returns comprehensive statistics

#### Config
- **Config()**: Creates new configuration
- **config.loadFromFile(filepath)**: Loads from JSON file
- **config.set(key, value)**: Sets configuration value
- **config.get(key)**: Gets configuration value
- **config.has(key)**: Checks if key exists
- **config.validate(requiredKeys)**: Validates configuration

#### Action
- **Action(type)**: Creates action with type
- **Action(type, parameters)**: Creates action with parameters
- **action.set_type(type)**: Sets action type
- **action.set_parameters(params)**: Sets parameters
- **action.add_param(value)**: Adds parameter
- **action.to_dict()**: Converts to dictionary

#### SensoryPercept
- **percept.to_dict()**: Converts to dictionary
- **percept.get_vision()**: Gets vision data
- **percept.get_touch()**: Gets touch data

### Convenience Functions

#### Module-level Functions
- **create_default_config()**: Creates default configuration
- **create_demo_config()**: Creates demo configuration
- **createBrain(config)**: Creates brain
- **createSimpleWorld()**: Creates world
- **createAgentBrain(brain)**: Creates agent brain
- **run_simulation(brain, world, steps)**: Runs simulation
- **save_brain_state(filepath, brain)**: Saves brain
- **load_brain_state(filepath, config)**: Loads brain
- **create_agent(brain, world)**: Creates agent
- **validate_config(config)**: Validates configuration
- **profile_brain(brain, steps)**: Profiles performance

#### Constants
- **nlm.NEURON_TYPE_EXCITATORY**: Excitatory neuron type
- **nlm.NEURON_TYPE_INHIBITORY**: Inhibitory neuron type
- **nlm.ACTION_TYPE_MOVE_FORWARD**: Move forward action
- **nlm.WORLD_OBJECT_WALL**: Wall object type

## Advanced Features

### Custom Experiment Framework

```python
import nlm
import numpy as np

def run_plasticity_experiment():
    """Custom experiment studying synaptic plasticity."""
    
    # Create brain with controlled environment
    config = nlm.create_demo_config()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Record initial state
    initial_stats = brain.get_statistics()
    initial_spike_count = brain.getTotalSpikeCount()
    
    # Apply controlled stimulus
    for step in range(500):
        # Inject correlated pre-post activity (LTP)
        brain.injectCurrentToNeurons(nlm.NEURON_TYPE_EXCITATORY, 30.0)
        brain.step(step)
    
    # Measure changes
    final_stats = brain.get_statistics()
    final_spike_count = brain.getTotalSpikeCount()
    
    # Analyze results
    spike_increase = final_spike_count - initial_spike_count
    weight_change = final_stats.get('avg_weight_change', 0.0)
    
    return {
        'spike_increase': spike_increase,
        'weight_change': weight_change,
        'final_stats': final_stats
    }

# Run multiple trials
results = []
for trial in range(5):
    result = run_plasticity_experiment()
    results.append(result)
    print(f"Trial {trial + 1}: Spike increase = {result['spike_increase']}, Weight change = {result['weight_change']:.4f}")
```

### Machine Learning Integration

```python
import nlm
import sklearn.preprocessing

def prepare_neural_data(brain, steps=100):
    """Extract features from brain simulation for ML analysis."""
    
    features = []
    targets = []
    
    for step in range(steps):
        # Get brain statistics
        stats = brain.get_statistics()
        
        # Extract features
        feature_vector = [
            stats['total_neurons'],
            stats['total_synapses'],
            stats['active_neurons'],
            stats['firing_neurons'],
            stats['total_spikes'],
            stats['avg_firing_rate'],
            stats['e_i_ratio'],
            brain.getDevelopmentalStage()
        ]
        
        features.append(feature_vector)
        
        # Simulate world step
        brain.step(step)
    
    # Normalize features
    features_array = np.array(features)
    scaler = sklearn.preprocessing.StandardScaler()
    normalized_features = scaler.fit_transform(features_array)
    
    return normalized_features

# Example usage
brain = nlm.createBrain(nlm.create_default_config())
brain.initialize()

ml_features = prepare_neural_data(brain, 200)
print(f"Extracted {ml_features.shape[0]} feature vectors with {ml_features.shape[1]} dimensions each")
```

## Troubleshooting

### Common Issues

#### "ModuleNotFoundError: No module named 'nlm'"

Make sure the Python bindings are built and in your Python path:

```bash
# If you built in the build directory
export PYTHONPATH=/workspace/app/build:$PYTHONPATH
python -c "import nlm; print('Success!')"
```

#### "ImportError: libnlm.so: cannot open shared object file"

The Python bindings depend on the NLM C++ library. Make sure both are built:

```bash
# Build everything
mkdir build
cd build
cmake ..
make -j4

# The Python module should be in build/pynlm*
export LD_LIBRARY_PATH=/workspace/app/build:$LD_LIBRARY_PATH
python -c "import nlm; print('Success!')"
```

#### "Simulation too slow"

For large simulations, consider:
- Reducing neuron count in configuration
- Using smaller simulation timesteps
- Running in batches
- Using the profiler to identify bottlenecks

#### "Configuration validation errors"

Check the error messages from `validate_config()` or manually verify required keys:

```python
import nlm

config = nlm.create_default_config()
config.set("brain.neuron_count", 100)
config.set("brain.simulation_steps", 1000)

# Validate
errors = nlm.validate_config(config)
if errors:
    print("Fix these issues:")
    for error in errors:
        print(f"  - {error}")
```

## Development

### Building Python Bindings

To build only the Python bindings:

```bash
mkdir build
cd build
cmake .. -DPYTHON_EXECUTABLE=/usr/bin/python3
make pynlm
```

### Running Tests

The C++ tests can be run with:

```bash
mkdir build
cd build
make test
```

### Adding New Features

To add new features to the Python bindings:

1. Modify `python/bindings.cpp`
2. Re-build the Python module
3. Test with Python examples

### Python Package Structure

For a proper Python package, consider creating:

```
nlm/
├── __init__.py          # Python package initialization
├── examples/            # Example scripts
├── docs/                # Documentation
├── tests/               # Python tests
└── utils/               # Utility functions
```

## License

This project is licensed under the MIT License.

## Authors

This is a research project - see the main NLM documentation for scientific background.
