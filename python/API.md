# NLM Python API Documentation

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python bindings (`pynlm`). NLM is an experimental artificial developmental brain project that implements a neural system for learning from experience through interaction with an environment.

## Quick Start

```python
import pynlm as nlm

# Create a default configuration
config = nlm.Config()

# Create a brain with the configuration
brain = nlm.Brain(config)

# Initialize the brain
brain.initialize()

# Run a few simulation steps
for step in range(10):
    brain.step(step)

# Get current status
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")

# Get neuromodulation levels
neuromod_levels = brain.getNeuromodulationLevels()
print(f"Neuromodulation levels: {neuromod_levels}")
```

## Installation

The pynlm Python bindings are automatically built as part of the NLM C++ framework. After building the project, you can import pynlm in Python.

## Core Classes

### Config
Configuration management for NLM brain parameters.

**Methods:**
- `__init__()` - Create default configuration
- `loadFromFile(filepath)` - Load configuration from JSON file
- `loadFromArgs(argc, argv)` - Load configuration from command line arguments
- `saveToFile(filepath)` - Save configuration to JSON file
- `has(key)` - Check if configuration key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration entries
- `summary()` - Get configuration summary string

**Example:**
```python
config = nlm.Config()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.saveToFile("my_config.json")
```

### Brain
The central neural simulation brain class.

**Constructor:**
- `Brain(config)` - Create brain with configuration

**Key Methods:**
- `initialize()` - Initialize brain with configuration
- `step(current_step)` - Perform simulation step
- `step(current_step, current_time)` - Perform simulation step with timestamp
- `receiveSensoryInput(input)` - Inject sensory input into brain
- `produceAction()` - Produce motor action based on neural activity
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file

**Statistics and Monitoring:**
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getActiveNeuronCount()` - Get count of active neurons
- `getFiringNeuronCount()` - Get count of currently firing neurons
- `getAverageFiringRate()` - Get average firing rate
- `getExcitationInhibitionRatio()` - Get excitation/inhibition balance
- `getTotalSpikeCount()` - Get total spike count
- `getNeuromodulationLevels()` - Get all neuromodulation levels as dict
- `logStatus()` - Log comprehensive brain status

**Region Management:**
- `addRegion(name)` - Add a new neural region
- `getRegion(id)` - Get region by ID
- `getRegionCount()` - Get number of regions
- `getRegionIds()` - Get all region IDs

### AgentBrain
Interface connecting NLM brain to world environment.

**Constructor:**
- `AgentBrain(brain)` - Create agent brain with underlying brain

**Key Methods:**
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory percept
- `decodeMotorCommand()` - Decode brain activity into motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward neuromodulation

### SimpleWorld
Simple 2D world for NLM simulation.

**Constructor:**
- `SimpleWorld()` - Create new simple world

**Key Methods:**
- `configure(width, height, vision_width, vision_height)` - Configure world dimensions
- `update(timestep)` - Update world state
- `applyMotorCommand(cmd, current_time)` - Apply motor command
- `getSensoryPercept()` - Get current sensory percept
- `getAgentBody()` - Get agent body state

### Sensory Input Classes
Base classes for sensory inputs:

- `Vision(width, height, channels)` - Visual input
- `Audio(sample_rate, num_samples)` - Audio input
- `InternalSignals()` - Internal signals

### Action
Motor action representation.

**Constructors:**
- `Action()` - Create empty action
- `Action(type)` - Create action with type
- `Action(type, parameters)` - Create action with type and parameters

**Methods:**
- `getType()` - Get action type
- `getParameters()` - Get action parameters
- `getName()` - Get action name
- `clone()` - Clone action

## Advanced Usage

### Creating Custom Brains

```python
def create_research_brain(neuron_count=500, region_count=3):
    """Create a custom brain for research purposes."""
    config = nlm.Config()
    config.set("neuron_count", neuron_count)
    config.set("region_count", region_count)
    config.set("connection_probability", 0.05)
    config.set("plasticity_learning_rate", 0.005)
    
    brain = nlm.Brain(config)
    brain.initialize()
    return brain

# Use the custom brain
brain = create_research_brain()
```

### Sensory Integration

```python
# Create vision input
vision = nlm.Vision(84, 84, 3)  # For Atari-style environments
vision_data = [0.1, 0.2, 0.3, 0.4, 0.5] * 84 * 84  # Example data
vision.setData(vision_data)

# Inject into brain
brain.receiveSensoryInput(vision)
```

### Action Planning and Execution

```python
# Run simulation loop
time_step = 0
for episode in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    brain.receiveSensoryInput(percept)
    
    # Produce action
    action = brain.produceAction()
    
    # Apply action to world
    world.applyMotorCommand(action.getType(), time_step)
    
    # Update world
    world.update(time_step)
    
    # Step brain
    brain.step(time_step, world.getSimulationTime())
    
    # Log status occasionally
    if time_step % 100 == 0:
        neuromod_levels = brain.getNeuromodulationLevels()
        print(f"Step {time_step}: Neuromod levels: {neuromod_levels}")
    
    time_step += 1
```

### Brain Checkpointing

```python
# Save brain state
brain.save("checkpoint_brain_001.bin")

# Load brain state
brain.load("checkpoint_brain_001.bin")
```

## Configuration Management

### Available Configuration Keys

- `random_seed` - Random seed for reproducible experiments
- `simulation_timestep` - Simulation timestep in seconds
- `neuron_count` - Number of neurons in the brain
- `region_count` - Number of brain regions
- `connection_probability` - Connection probability for random connectivity
- `plasticity_learning_rate` - Plasticity learning rate
- `stdp_ltp_weight` - STDP LTP weight
- `stdp_ltd_weight` - STDP LTD weight
- `stdp_tau` - STDP time constant
- `development_synaptogenesis_rate` - Synaptogenesis rate
- `development_pruning_rate` - Pruning rate
- `dopamine_baseline` - Dopamine baseline
- `curiosity_baseline` - Curiosity baseline
- `reward_discount_factor` - Reward discount factor
- `environment_name` - Environment name
- `environment_width` - Environment width
- `environment_height` - Environment height
- `log_level` - Logging level
- `log_to_file` - Whether to log to file
- `log_filename` - Log filename
- `max_simulation_steps` - Maximum simulation steps
- `simulation_time_limit` - Simulation time limit
- `visualization_enabled` - Whether visualization is enabled
- `visualization_update_rate` - Visualization update rate

### Loading Configurations

```python
# Create config from file
config = nlm.Config()
config.loadFromFile("my_config.json")

# Create config from command line
import sys
config = nlm.Config()
config.loadFromArgs(len(sys.argv), sys.argv)
```

## Type Enumerations

### NeuronType
- `Excitatory` - Excitatory neuron
- `Inhibitory` - Inhibitory neuron
- `Modulatory` - Modulatory neuron
- `Sensory` - Sensory neuron
- `Motor` - Motor neuron
- `Internal` - Internal neuron

### SynapseType
- `Excitatory` - Excitatory synapse
- `Inhibitory` - Inhibitory synapse
- `Modulatory` - Modulatory synapse
- `Electrical` - Electrical synapse
- `GapJunction` - Gap junction synapse

### ActionType
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `MoveLeft` - Move left
- `MoveRight` - Move right
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Look` - Look
- `LookUp` - Look up
- `LookDown` - Look down
- `Interact` - Interact
- `Eat` - Eat
- `Drink` - Drink
- `Rest` - Rest
- `Wait` - Wait
- `Custom` - Custom action

### DevelopmentalStage
- `Initial` - Initial developmental stage
- `CriticalPeriod` - Critical period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

### FiringState
- `Resting` - Resting state
- `Active` - Active state
- `Refractory` - Refractory state
- `Inhibited` - Inhibited state

### WorldObjectType
- `Empty` - Empty object
- `Resource` - Resource object
- `Hazard` - Hazard object
- `Wall` - Wall object
- `Marker` - Marker object

## Utility Functions

### Factory Functions
```python
# Create default configuration
config = nlm.createDefaultConfig()

# Create brain from config
brain = nlm.createBrain(config)

# Create world
world = nlm.createSimpleWorld()

# Create agent brain
agent_brain = nlm.createAgentBrain(brain)
```

### Constants
```python
# Invalid IDs
INVALID_NEURON_ID = nlm.INVALID_NEURON_ID
INVALID_SYNAPSE_ID = nlm.INVALID_SYNAPSE_ID
INVALID_REGION_ID = nlm.INVALID_REGION_ID
INVALID_POPULATION_ID = nlm.INVALID_POPULATION_ID
```

## Error Handling

The Python bindings use standard Python exceptions for error reporting. Common exceptions include:

- `RuntimeError` - General runtime errors
- `ValueError` - Invalid parameter values
- `FileNotFoundError` - Missing files
- `OSError` - System-level errors

## Performance Considerations

1. **Memory Management**: Python bindings use smart pointers and automatic memory management
2. **Event-Driven Processing**: The brain uses event-driven processing for efficiency
3. **Parallel Processing**: Some operations leverage parallel processing when available
4. **Memory Pools**: Memory pools are used for efficient allocation/deallocation

## Extending the API

### Adding Custom Neural Regions
```python
# Add a custom region to an existing brain
region_id = brain.addRegion("Custom_Region")
region = brain.getRegion(region_id)

# You can extend the brain by adding new methods that
# interact with existing brain systems
```

### Creating Custom Sensory Inputs
```python
class CustomSensoryInput(nlm.SensoryInput):
    def __init__(self, data):
        super().__init__()
        self.custom_data = data
    
    def getType(self):
        return "Custom"
    
    def getData(self):
        return self.custom_data
```

## Migration Guide

### From C++ to Python

**C++ Code:**
```cpp
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 1000);
auto brain = std::make_shared<nlm::Brain>(config);
brain->initialize();
```

**Python Code:**
```python
config = nlm.Config()
config.set("neuron_count", 1000)
brain = nlm.Brain(config)
brain.initialize()
```

### From Single Threads to Python

The Python bindings provide thread-safe access to brain components. All methods are safe to call from multiple threads, though concurrent modification of the same brain components should be synchronized.

## Troubleshooting

### Common Issues

1. **Import Error**: Make sure the pynlm module is built and in your Python path
2. **Configuration Errors**: Check that configuration keys match expected types
3. **Memory Issues**: The bindings use automatic memory management, but large simulations may need optimization
4. **Performance Issues**: For large-scale simulations, consider using the C++ API directly

### Debugging

```python
import pynlm as nlm
import logging

# Enable debug logging
logging.basicConfig(level=logging.DEBUG)

# Create and run brain
config = nlm.Config()
brain = nlm.Brain(config)
brain.initialize()

# Add a handler for errors
def handle_error(error):
    logging.error(f"Brain error: {error}")

# The brain will log errors automatically
```

## Future Enhancements

The following features are planned for future releases:

1. **Enhanced Visualization**: Integration with matplotlib and other plotting libraries
2. **Neural Network Visualization**: Tools for visualizing brain connectivity
3. **Machine Learning Integration**: Integration with scikit-learn, TensorFlow, and PyTorch
4. **Distributed Computing**: Support for distributed brain simulation
5. **Web Interface**: Web-based visualization and control
6. **Plugin System**: Extensible plugin architecture for custom functionality
7. **Advanced Configuration**: YAML and other configuration formats
8. **Performance Profiling**: Built-in profiling and benchmarking tools

## License

This project is licensed under the MIT License. See the LICENSE file for details.
