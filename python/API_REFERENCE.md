# Python API Reference for NLM (Neural Learning Machine)

This document provides comprehensive API documentation for the NLM Python bindings, which provide a high-level interface to the C++ neural simulation framework.

## Overview

The NLM Python bindings (`pynlm`) provide a Pythonic interface to the Neural Learning Machine (熙然) framework. This framework implements an experimental artificial developmental brain with spiking neural networks, learning capabilities, and interactive environments.

**Installation:**
```bash
pip install -e .
# or
cd /workspace/app && pip install -e . --no-build-isolation
```

**Basic Usage:**
```python
import pynlm as nlm

# Create configuration with context managers
with nlm.Config() as config:
    config.setDefaults("quick_test")
    config.updateFromDict({"neuronCount": 100})
    
    # Create brain with simulation
    with nlm.Brain(config) as brain:
        brain.initialize()
        
        # Create world environment
        with nlm.SimpleWorld() as world:
            world.createGridWorld(50.0, 50.0, 0.1)
            world.setAgentGoal(40.0, 40.0)
            
            # Create and initialize agent
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                agent.enableCuriosity(True)
                
                # Run simulation
                for step in range(100):
                    world.update(0.1)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
```

## Core Classes

### Config Class

The `Config` class manages NLM simulation parameters and settings. It includes Pythonic convenience methods for easy configuration management.

#### Standard Methods

**Configuration Management:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `set(key, value)` | Set a configuration value | `key: str, value: Any` | `None` |
| `get(key, default=None)` | Get configuration value | `key: str, default: Any` | `Any` |
| `getOr(key, default)` | Get configuration value with default | `key: str, default: Any` | `Any` |
| `has(key)` | Check if key exists | `key: str` | `bool` |
| `getKeys()` | Get all configuration keys | `None` | `List[str]` |
| `clear()` | Clear all configuration entries | `None` | `None` |
| `summary()` | Get configuration summary | `None` | `Dict[str, Any]` |

**Pythonic Convenience Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `setDefaults(profile_name)` | Apply predefined configuration profiles | `profile_name: str` | `None` |
| `mergeConfig(other_config)` | Merge configuration dictionaries | `other_config: Dict[str, Any]` | `None` |
| `validateConfig()` | Validate configuration and return issues | `None` | `List[str]` |
| `getSummary()` | Get configuration as Python dict | `None` | `Dict[str, Any]` |
| `updateFromDict(config_dict)` | Update config from dictionary | `config_dict: Dict[str, Any]` | `None` |

#### Configuration Profiles

The `setDefaults()` method accepts these profile names:

- `"default"` - Standard simulation settings
- `"high_performance"` - Optimized for speed
- `"quick_test"` - Minimal settings for fast testing

#### Examples

**Basic Configuration:**
```python
import pynlm as nlm

with nlm.Config() as config:
    config.set("brain.neuron_count", 1000)
    config.set("brain.connection_probability", 0.2)
    config.set("agent.exploration_rate", 0.3)
    
    # Get values
    neuron_count = config.getOr("brain.neuron_count", 100)
    has_learning = config.has("brain.learning_enabled")
```

**Using Configuration Profiles:**
```python
import pynlm as nlm

with nlm.Config() as config:
    # Use predefined profile
    config.setDefaults("high_performance")
    
    # Override with specific settings
    config.updateFromDict({
        "simulation.max_steps": 5000,
        "simulation.timestep": 0.001,
        "agent.curiosity_level": 0.5
    })
    
    # Validate configuration
    issues = config.validateConfig()
    if issues:
        print("Configuration issues:")
        for issue in issues:
            print(f"  - {issue}")
```

### Brain Class

The `Brain` class represents the neural simulation system. It contains spiking neurons, synapses, and manages neural dynamics and learning processes.

#### Standard Methods

**Neural Simulation:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize()` | Initialize brain with configuration | `None` | `None` |
| `step(step_number)` | Perform simulation step | `step_number: int` | `None` |
| `step(step_number, current_time)` | Perform step with timestamp | `step_number: int, current_time: float` | `None` |
| `receiveSensoryInput(input)` | Inject sensory input | `input: SensoryInput` | `None` |
| `produceAction()` | Generate motor action | `None` | `Action` |
| `reset()` | Reset brain state | `None` | `None` |
| `save(filepath)` | Save brain state to file | `filepath: str` | `None` |
| `load(filepath)` | Load brain state from file | `filepath: str` | `None` |

**Brain Statistics:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `getTotalNeuronCount()` | Get total neuron count | `None` | `int` |
| `getActiveNeuronCount()` | Get active neuron count | `None` | `int` |
| `getTotalSpikeCount()` | Get total spike count | `None` | `int` |
| `getTotalSynapseCount()` | Get total synapse count | `None` | `int` |
| `getFiringNeuronCount()` | Get firing neuron count | `None` | `int` |
| `getAverageFiringRate()` | Get average firing rate | `None` | `float` |
| `getRegionCount()` | Get region count | `None` | `int` |
| `getRegion(region_id)` | Get region by ID | `region_id: int` | `Region` |

**Pythonic Convenience Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `runSimulation(steps)` | Run simulation for N steps (chainable) | `steps: int` | `Brain` |
| `createStandardBrain(config)` | Factory method for common brain configurations | `config: Config` | `Brain` |
| `getNeuralStats()` | Returns brain state as Python dict | `None` | `Dict[str, Any]` |
| `simulateWithInput(inputs, steps)` | Convenience for running with sensory input | `inputs: List[SensoryInput], steps: int` | `None` |
| `trainAgent(world, steps)` | High-level agent training method | `world: SimpleWorld, steps: int` | `Dict[str, Any]` |

#### Examples

**Basic Brain Simulation:**
```python
import pynlm as nlm

with nlm.Config() as config:
    config.setDefaults("quick_test")
    
    with nlm.Brain(config) as brain:
        brain.initialize()
        
        # Run simulation for 100 steps
        for step in range(100):
            brain.step(step)
        
        # Get statistics
        stats = brain.getNeuralStats()
        print(f"Total neurons: {stats['total_neurons']}")
        print(f"Total spikes: {stats['total_spikes']}")
        print(f"Average firing rate: {stats['average_firing_rate']:.3f} Hz")
```

**Brain with Sensory Input:**
```python
import pynlm as nlm

with nlm.Config() as config:
    config.setDefaults("default")
    
    with nlm.Brain(config) as brain:
        brain.initialize()
        
        # Create sensory input
        with nlm.Vision(20, 20) as vision:
            # Generate test image data
            image_data = []
            for y in range(20):
                for x in range(20):
                    brightness = (x + y) / 40.0
                    image_data.extend([brightness, brightness * 0.8, brightness * 0.6])
            
            vision.setData(image_data)
            
            # Run simulation with input
            brain.simulateWithInput(vision, 50)
        
        # Get final statistics
        final_stats = brain.getNeuralStats()
        print(f"After simulation: {final_stats['total_spikes']} spikes")
```

### AgentBrain Class

The `AgentBrain` class manages the interface between the neural brain and the world environment, implementing action selection and sensory processing.

#### Standard Methods

**Agent Control:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize(world)` | Initialize with world | `world: SimpleWorld` | `None` |
| `decodeMotorCommand()` | Decode brain activity to motor command | `None` | `Action` |
| `processSensoryInput(percept)` | Process sensory input | `percept: SensoryInput` | `None` |
| `applyRewardModulation(reward, predicted_reward)` | Apply reward-based learning | `reward: float, predicted_reward: float` | `None` |

**Agent Properties:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `enableRewardModulation(enabled)` | Enable/disable reward modulation | `enabled: bool` | `None` |
| `enableStructuralPlasticity(enabled)` | Enable/disable structural plasticity | `enabled: bool` | `None` |
| `enableDevelopment(enabled)` | Enable/disable development | `enabled: bool` | `None` |
| `enableCuriosity(enabled)` | Enable/disable curiosity | `enabled: bool` | `None` |
| `getNeuromodulationLevel()` | Get neuromodulation level | `None` | `float` |
| `getCuriosityLevel()` | Get curiosity level | `None` | `float` |
| `getPerformanceMetrics()` | Get training metrics | `None` | `Dict[str, Any]` |

**Pythonic Convenience Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `learnFromWorld(world, steps)` | High-level learning method | `world: SimpleWorld, steps: int` | `Dict[str, Any]` |
| `createAgentSequence(world, num_episodes)` | Create training sequences | `world: SimpleWorld, num_episodes: int` | `List[Dict[str, Any]]` |
| `getPerformanceMetrics()` | Get training metrics as Python dict | `None` | `Dict[str, Any]` |
| `setLearningParameters(params)` | Set learning parameters from dict | `params: Dict[str, Any]` | `None` |

#### Examples

**Basic Agent Training:**
```python
import pynlm as nlm

# Create world environment
with nlm.SimpleWorld() as world:
    world.createGridWorld(100.0, 100.0, 0.1)
    world.setAgentGoal(90.0, 90.0)
    
    # Create and initialize brain and agent
    with nlm.Config() as config:
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                agent.enableCuriosity(True)
                agent.enableRewardModulation(True)
                
                # High-level training with convenience method
                metrics = agent.learnFromWorld(world, 200)
                print(f"Training completed: {metrics['success_rate']*100:.1f}% success rate")
                
                # Show performance metrics
                perf_metrics = agent.getPerformanceMetrics()
                print(f"Neuromodulation: {perf_metrics['neuromodulation_level']:.3f}")
                print(f"Curiosity: {perf_metrics['curiosity_level']:.3f}")
```

**Setting Learning Parameters:**
```python
import pynlm as nlm

with nlm.Config() as config:
    config.setDefaults("default")
    
    with nlm.Brain(config) as brain:
        brain.initialize()
        
        with nlm.AgentBrain(brain) as agent:
            # Set learning parameters using convenience method
            learning_params = {
                "reward_modulation": True,
                "structural_plasticity": True,
                "development": True,
                "curiosity": True
            }
            agent.setLearningParameters(learning_params)
            
            print("Learning parameters set:")
            print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
            print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
            print(f"  Development: {agent.isDevelopmentEnabled()}")
            print(f"  Curiosity: {agent.isCuriosityEnabled()}")
```

### SimpleWorld Class

The `SimpleWorld` class represents the simulation environment where the agent operates and interacts with the world.

#### Standard Methods

**World Configuration:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `configure(width, height, vision_width, vision_height)` | Configure world dimensions | `width: float, height: float, vision_width: int, vision_height: int` | `None` |
| `update(timestep)` | Update world state | `timestep: float` | `None` |
| `applyMotorCommand(cmd, current_time)` | Apply action to agent | `cmd: Action, current_time: float` | `Dict[str, Any]` |
| `getSensoryPercept()` | Get agent's sensory perception | `None` | `SensoryInput` |
| `getAgentBody()` | Get agent state | `None` | `AgentBody` |
| `getWidth()` | Get world width | `None` | `float` |
| `getHeight()` | Get world height | `None` | `float` |
| `getSimulationTime()` | Get simulation time | `None` | `float` |
| `setAgentStart(x, y)` | Set agent start position | `x: float, y: float` | `None` |
| `setAgentGoal(x, y)` | Set agent goal position | `x: float, y: float` | `None` |

**World Features:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `createGridWorld(width, height, obstacle_density)` | Create world with obstacles | `width: float, height: float, obstacle_density: float` | `None` |
| `addResource(x, y, value)` | Convenience method for adding resources | `x: float, y: float, value: float` | `None` |
| `getObservationMap()` | Get vision as 2D array for easy visualization | `None` | `List[List[float]]` |
| `addObject(obj)` | Add world object | `obj: WorldObject` | `None` |
| `getObject(x, y)` | Get object at position | `x: float, y: float` | `WorldObject` |

#### Examples

**World Creation and Visualization:**
```python
import pynlm as nlm

# Create a grid world with obstacles
with nlm.SimpleWorld() as world:
    world.createGridWorld(50.0, 50.0, 0.15)  # 15% obstacle density
    
    # Add resources at specific positions
    world.addResource(10.0, 10.0, 5.0)
    world.addResource(30.0, 30.0, 10.0)
    world.addResource(40.0, 20.0, 7.5)
    
    # Set agent goal
    world.setAgentGoal(45.0, 45.0)
    
    # Get observation map for visualization
    observation_map = world.getObservationMap()
    
    print("Observation map (first 5x5 section):")
    for i in range(min(5, len(observation_map))):
        row_str = "  ["
        for j in range(min(5, len(observation_map[i]))):
            row_str += f"{observation_map[i][j]:6.2f} "
        row_str += "]"
        print(f"    {row_str}")
    
    print("✓ World created with obstacles, resources, and goals")
```

**Complete World Simulation:**
```python
import pynlm as nlm

# Create complete simulation
with nlm.Config() as config:
    config.setDefaults("high_performance")
    
    with nlm.SimpleWorld() as world:
        world.createGridWorld(100.0, 100.0, 0.1)
        world.setAgentStart(10.0, 10.0)
        world.setAgentGoal(90.0, 90.0)
        
        # Add resources to collect
        for x in [20.0, 40.0, 60.0, 80.0]:
            for y in [20.0, 40.0, 60.0, 80.0]:
                world.addResource(x, y, 1.0)
        
        print(f"World size: {world.getWidth()} x {world.getHeight()}")
        print(f"Vision dimensions: {world.getVisionWidth()} x {world.getVisionHeight()}")
        
        # Simulate a few steps
        for step in range(10):
            world.update(0.1)
            percept = world.getSensoryPercept()
            body = world.getAgentBody()
            print(f"Step {step}: Agent at ({body.x:.1f}, {body.y:.1f})")
```

### SensoryInput Classes

The `SensoryInput` classes provide sensory information from the environment. These are base classes with specific implementations.

#### Vision Class

The `Vision` class represents visual sensory input.

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `__init__(width, height)` | Create vision input | `width: int, height: int` | `Vision` |
| `setData(data)` | Set vision data | `data: List[float]` | `None` |
| `setTimestamp(timestamp)` | Set timestamp | `timestamp: float` | `None` |
| `getType()` | Get input type | `None` | `str` |
| `getData()` | Get raw data | `None` | `List[float]` |
| `getDimensions()` | Get dimensions | `None` | `Tuple[int, int, int]` |
| `getWidth()` | Get width | `None` | `int` |
| `getHeight()` | Get height | `None` | `int` |
| `getChannels()` | Get channels | `None` | `int` |
| `getTimestamp()` | Get timestamp | `None` | `float` |
| `getVisionArray()` | Get vision data as 2D array (Pythonic) | `None` | `List[List[List[float]]]` |

**Example:**
```python
import pynlm as nlm

with nlm.Vision(10, 10) as vision:
    # Create 2D RGB image data
    image_data = []
    for y in range(10):
        for x in range(10):
            brightness = (x + y) / 20.0
            image_data.extend([brightness, brightness * 0.8, brightness * 0.6])
    
    vision.setData(image_data)
    
    # Get vision as 2D array for easy processing
    vision_array = vision.getVisionArray()
    
    print("Vision array (top-left 3x3 section):")
    for i in range(min(3, len(vision_array))):
        print(f"    Row {i}: {[int(pixel[0] * 255) for pixel in vision_array[i][:3]]}")
```

#### Audio Class

The `Audio` class represents auditory sensory input.

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `__init__(sample_rate, num_samples)` | Create audio input | `sample_rate: int, num_samples: int` | `Audio` |
| `setData(data)` | Set audio data | `data: List[float]` | `None` |
| `setSampleRate(sample_rate)` | Set sample rate | `sample_rate: int` | `None` |
| `getType()` | Get input type | `None` | `str` |
| `getData()` | Get raw data | `None` | `List[float]` |
| `getSampleRate()` | Get sample rate | `None` | `int` |
| `getNumSamples()` | Get number of samples | `None` | `int` |

#### InternalSignals Class

The `InternalSignals` class represents internal brain state signals (e.g., neuromodulators).

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `__init__()` | Create internal signals | `None` | `InternalSignals` |
| `addSignal(value)` | Add signal value | `value: float` | `None` |
| `getType()` | Get input type | `None` | `str` |
| `getData()` | Get raw data | `None` | `List[float]` |
| `getInternalState()` | Get internal state as Python dict (Pythonic) | `None` | `Dict[str, float]` |

### Action Class

The `Action` class represents motor commands generated by the brain and executed in the world.

#### Standard Methods

**Action Properties:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `getType()` | Get action type | `None` | `ActionType` |
| `setType(type)` | Set action type | `type: ActionType` | `None` |
| `getParameters()` | Get action parameters | `None` | `List[float]` |
| `setParameters(params)` | Set action parameters | `params: List[float]` | `None` |
| `getName()` | Get human-readable action name | `None` | `str` |
| `clone()` | Create copy of action | `None` | `Action` |

**Pythonic Convenience Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `createMove(x, y)` | Create movement action | `x: float, y: float` | `Action` |
| `createInteract()` | Create interaction action | `None` | `Action` |
| `createCustom(params)` | Create custom action from dict | `params: Dict[str, Any]` | `Action` |
| `actionFromDict(action_dict)` | Create action from dictionary | `action_dict: Dict[str, Any]` | `Action` |

#### Action Types

The `ActionType` enum contains different types of motor commands:

- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `MoveUp` - Move up
- `MoveDown` - Move down
- `Interact` - Interact with environment
- `Eat` - Consume food
- `Sleep` - Rest
- `Custom` - Custom action

#### Examples

**Creating Actions with Convenience Methods:**
```python
import pynlm as nlm

# Create different types of actions using convenience methods
move_action = nlm.Action.createMove(5.0, 0.0)
print(f"Move action: {move_action.getName()} with params {move_action.getParameters()}")

interact_action = nlm.Action.createInteract()
print(f"Interact action: {interact_action.getName()}")

# Create custom action from parameters
custom_action = nlm.Action.createCustom({
    "speed": 1.5,
    "direction": 45.0,
    "strength": 0.8
})
print(f"Custom action: {custom_action.getType()} with params {custom_action.getParameters()}")

# Create action from dictionary
action_from_dict = nlm.Action.actionFromDict({
    "type": "MoveForward",
    "parameters": [1.0, 0.5]
})
print(f"Action from dict: {action_from_dict.getName()}")

# Create action using traditional method (backward compatibility)
traditional_action = nlm.Action(nlm.ActionType.Eat)
traditional_action.setParameters([0.5])
print(f"Traditional action: {traditional_action.getName()}")
```

## Serialization

The NLM Python bindings support serialization of brain states and world data for checkpointing and experiment reproducibility.

### Serialization Classes

#### JsonSerializer

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `save_to_file(filepath, data)` | Save data to JSON file | `filepath: str, data: Dict[str, Any]` | `None` |
| `load_from_file(filepath)` | Load data from JSON file | `filepath: str` | `Dict[str, Any]` |
| `serialize_brain(brain, filepath)` | Serialize brain state | `brain: Brain, filepath: str` | `None` |
| `deserialize_brain(filepath)` | Deserialize brain state | `filepath: str` | `Brain` |
| `serialize_world(world, filepath)` | Serialize world state | `world: SimpleWorld, filepath: str` | `None` |
| `deserialize_world(filepath)` | Deserialize world state | `filepath: str` | `SimpleWorld` |

#### BinarySerializer

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `save_to_file(filepath, data)` | Save data to binary file | `filepath: str, data: Dict[str, Any]` | `None` |
| `load_from_file(filepath)` | Load data from binary file | `filepath: str` | `Dict[str, Any]` |
| `serialize_brain(brain, filepath)` | Serialize brain state | `brain: Brain, filepath: str` | `None` |
| `deserialize_brain(filepath)` | Deserialize brain state | `filepath: str` | `Brain` |

#### IncrementalSerializer

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `save_to_file(filepath, data)` | Save data to file in chunks | `filepath: str, data: Dict[str, Any]` | `None` |
| `load_from_file(filepath)` | Load data from file in chunks | `filepath: str` | `Dict[str, Any]` |
| `get_chunk_count(filepath)` | Get number of chunks in file | `filepath: str` | `int` |
| `get_chunk_size(filepath, chunk_index)` | Get size of specific chunk | `filepath: str, chunk_index: int` | `int` |

### Serialization Examples

**Save and Load Brain State:**
```python
import pynlm as nlm

with nlm.Config() as config:
    config.setDefaults("default")
    
    with nlm.Brain(config) as brain:
        brain.initialize()
        
        # Run simulation to generate state
        for step in range(100):
            brain.step(step)
        
        # Save brain state
        nlm.JsonSerializer.serialize_brain(brain, "brain_state.json")
        print("✓ Brain state saved to brain_state.json")
        
        # Save world state separately
        with nlm.SimpleWorld() as world:
            world.configure(100.0, 100.0, 10, 10)
            nlm.JsonSerializer.serialize_world(world, "world_state.json")
            print("✓ World state saved to world_state.json")

# Load brain state later
with nlm.Config() as config:
    config.setDefaults("default")
    
    with nlm.Brain(config) as brain:
        brain.load("brain_state.json")
        print("✓ Brain state loaded from brain_state.json")
        
        # Verify loaded state
        stats = brain.getNeuralStats()
        print(f"Loaded brain has {stats['total_neurons']} neurons")
```

**Binary Serialization for Performance:**
```python
import pynlm as nlm

with nlm.Config() as config:
    config.setDefaults("default")
    
    with nlm.Brain(config) as brain:
        brain.initialize()
        
        # Run simulation
        for step in range(500):
            brain.step(step)
        
        # Use binary serialization for better performance
        nlm.BinarySerializer.serialize_brain(brain, "brain_state.bin")
        print("✓ Brain state saved as binary")
        
        # Load using binary serialization
        with nlm.Brain(config) as loaded_brain:
            nlm.BinarySerializer.deserialize_brain("brain_state.bin", loaded_brain)
            print("✓ Brain state loaded from binary")
```

## Error Handling and Validation

The NLM Python bindings provide comprehensive error handling with custom exception types and validation methods.

### Exception Types

#### NLMError (Base Class)

Base class for all NLM-related exceptions.

#### NLMConfigError

Thrown for configuration-related errors.

#### NLMInitializationError

Thrown for initialization-related errors.

#### NLMResourceError

Thrown for resource management errors.

#### NLMValidationError

Thrown for validation errors.

### Error Handling Examples

**Exception Handling:**
```python
import pynlm as nlm

try:
    # This may throw various exceptions
    with nlm.Config() as config:
        with nlm.Brain(config) as brain:
            brain.initialize()
            # Perform operations that may fail
            action = brain.produceAction()
            
except nlm.NLMConfigError as e:
    print(f"Configuration error: {e}")
    print(f"  Error type: {e.type}")
    print(f"  Context: {e.context}")
    
except nlm.NLMInitializationError as e:
    print(f"Initialization error: {e}")
    
except nlm.NLMResourceError as e:
    print(f"Resource error: {e}")
    
except nlm.NLMValidationError as e:
    print(f"Validation error: {e}")
    
except Exception as e:
    print(f"Unexpected error: {e}")
```

**Validation:**
```python
import pynlm as nlm

with nlm.Config() as config:
    # Set configuration values
    config.setDefaults("default")
    config.updateFromDict({
        "brain.neuron_count": 1000,
        "agent.exploration_rate": 0.5
    })
    
    # Validate configuration
    issues = config.validateConfig()
    if issues:
        print("Configuration validation failed:")
        for i, issue in enumerate(issues, 1):
            print(f"  {i}. {issue}")
        # Fix issues and retry
        config.updateFromDict({
            "agent.exploration_rate": 0.3  # Reduce exploration rate
        })
        
        # Validate again
        remaining_issues = config.validateConfig()
        if remaining_issues:
            print("Still has issues, exiting")
        else:
            print("Configuration is now valid")
    else:
        print("Configuration is valid")
```

## Performance Optimization Tips

This section provides tips for optimizing performance when using the NLM Python bindings.

### Memory Management

**1. Use Context Managers:**
```python
# Recommended - automatic cleanup
with nlm.Brain(config) as brain:
    brain.initialize()
    # ... use brain

# Avoid - potential resource leaks
brain = nlm.Brain(config)
brain.initialize()
# ... use brain
# Forgetting to explicitly delete brain
```

**2. Reuse Objects:**
```python
import pynlm as nlm

# Create objects once and reuse them
with nlm.Config() as config:
    config.setDefaults("high_performance")
    
    with nlm.Brain(config) as brain:
        brain.initialize()
        
        # Reuse brain for multiple simulations
        for episode in range(10):
            # Reset brain state
            brain.reset()
            brain.initialize()
            
            # Run simulation
            for step in range(100):
                brain.step(step)
```

### Simulation Performance

**1. Use Pythonic Convenience Methods:**
```python
# More efficient - batch operations
brain.runSimulation(1000)

# Less efficient - individual steps
for step in range(1000):
    brain.step(step)
```

**2. Pre-compile Operations:**
```python
import pynlm as nlm

with nlm.Config() as config:
    config.setDefaults("high_performance")
    
    with nlm.Brain(config) as brain:
        brain.initialize()
        
        # Pre-compile frequently used operations
        for step in range(100):
            # Use batch operations when possible
            brain.step(step)
            
            # Get statistics periodically, not every step
            if step % 10 == 0:
                stats = brain.getNeuralStats()
```

### Configuration Optimization

**1. Use Appropriate Profiles:**
```python
import pynlm as nlm

# Use "quick_test" for development/testing
with nlm.Config() as config:
    config.setDefaults("quick_test")
    
    with nlm.Brain(config) as brain:
        brain.initialize()
        brain.runSimulation(100)  # Fast simulation

# Use "high_performance" for production
with nlm.Config() as config:
    config.setDefaults("high_performance")
    
    with nlm.Brain(config) as brain:
        brain.initialize()
        brain.runSimulation(10000)  # More neurons, slower but better
```

**2. Cache Configuration:**
```python
import pynlm as nlm

# Cache frequently used configurations
if 'test_config' not in globals():
    test_config = nlm.Config()
    test_config.setDefaults("quick_test")
    test_config.updateFromDict({
        "brain.neuron_count": 100,
        "simulation.max_steps": 200
    })

# Reuse cached configuration
with test_config as config:
    # ... use config
```

## Code Organization Best Practices

### 1. Modular Design

**Separate Concerns:**
```python
import pynlm as nlm

class NLMSimulation:
    def __init__(self):
        self.config = None
        self.brain = None
        self.world = None
        self.agent = None
    
    def __enter__(self):
        # Initialize all components
        self.config = nlm.Config()
        self.config.setDefaults("default")
        
        self.brain = nlm.Brain(self.config)
        self.brain.initialize()
        
        self.world = nlm.SimpleWorld()
        self.world.configure(100.0, 100.0, 10, 10)
        
        self.agent = nlm.AgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        # Cleanup is automatic due to context managers
        pass
    
    def run_episode(self, steps):
        # Run a single simulation episode
        for step in range(steps):
            self.world.update(0.1)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
    
    def run_multiple_episodes(self, num_episodes, steps_per_episode):
        # Run multiple episodes
        results = []
        for episode in range(num_episodes):
            # Reset and run episode
            self.brain.reset()
            self.agent.initialize(self.world)
            self.run_episode(steps_per_episode)
            
            # Collect metrics
            metrics = self.agent.getPerformanceMetrics()
            results.append(metrics)
        
        return results
```

### 2. Configuration Management

**Centralized Configuration:**
```python
import pynlm as nlm

# Configuration profiles
CONFIG_PROFILES = {
    "quick_test": {
        "brain.neuron_count": 50,
        "simulation.max_steps": 100,
        "agent.exploration_rate": 0.1
    },
    "development": {
        "brain.neuron_count": 500,
        "simulation.max_steps": 1000,
        "agent.exploration_rate": 0.3
    },
    "production": {
        "brain.neuron_count": 2000,
        "simulation.max_steps": 10000,
        "agent.exploration_rate": 0.5
    }
}

def create_config(profile_name="development"):
    """Create configuration with specified profile."""
    with nlm.Config() as config:
        config.setDefaults("default")
        
        if profile_name in CONFIG_PROFILES:
            config.updateFromDict(CONFIG_PROFILES[profile_name])
        
        return config
```

### 3. Testing and Validation

**Unit Testing:**
```python
import pynlm as nlm
import unittest

class TestNLMSimulation(unittest.TestCase):
    def setUp(self):
        # Setup for each test
        self.config = nlm.Config()
        self.config.setDefaults("quick_test")
        
        self.brain = nlm.Brain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        # Cleanup after each test
        pass
    
    def test_brain_initialization(self):
        # Test brain initialization
        self.assertTrue(self.brain is not None)
        stats = self.brain.getNeuralStats()
        self.assertGreater(stats['total_neurons'], 0)
    
    def test_simulation_steps(self):
        # Test simulation steps
        initial_spikes = self.brain.getTotalSpikeCount()
        
        for step in range(10):
            self.brain.step(step)
        
        final_spikes = self.brain.getTotalSpikeCount()
        self.assertGreater(final_spikes, initial_spikes)
    
    def test_context_manager_cleanup(self):
        # Test that context managers cleanup properly
        with nlm.Config() as config:
            with nlm.Brain(config) as brain:
                brain.initialize()
                self.assertIsNotNone(brain)
        
        # Objects should be cleaned up automatically

if __name__ == '__main__':
    unittest.main()
```

### 4. Documentation and Examples

**Documenting Code:**
```python
import pynlm as nlm

def example_usage():
    """
    Example function demonstrating NLM usage.
    
    This example shows how to create and use a complete NLM simulation
    with all major components.
    
    Returns:
        Dict[str, Any]: Performance metrics from the simulation.
    
    Example:
        >>> metrics = example_usage()
        >>> print(f"Success rate: {metrics['success_rate']*100:.1f}%")
    """
    with nlm.Config() as config:
        config.setDefaults("development")
        
        with nlm.SimpleWorld() as world:
            world.createGridWorld(50.0, 50.0, 0.1)
            world.setAgentGoal(40.0, 40.0)
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                with nlm.AgentBrain(brain) as agent:
                    agent.initialize(world)
                    agent.enableCuriosity(True)
                    agent.enableRewardModulation(True)
                    
                    # Run simulation
                    for step in range(100):
                        world.update(0.1)
                        action = agent.decodeMotorCommand()
                        world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Return metrics
                    return agent.getPerformanceMetrics()
```

## Migration Guide

This section provides guidance for migrating from older versions of the NLM Python bindings to the current version.

### From Version 0.1.0 to 0.2.0

**Changes:**

1. **Context Manager API:** All major classes now use context managers for resource management
2. **New Pythonic Convenience Methods:** Added high-level methods for common operations
3. **Enhanced Serialization:** Added multiple serialization formats (JSON, Binary, Incremental)
4. **Improved Error Handling:** Custom exception types and better error messages

**Migration Examples:**

**Before (Version 0.1.0):**
```python
# Old way - manual resource management
config = nlm.Config()
config.set("key", "value")

brain = nlm.Brain(config)
brain.initialize()

world = nlm.SimpleWorld()
world.configure(100.0, 100.0, 10, 10)

# ... use objects ...

# Manual cleanup
brain = None
world = None
config = None
```

**After (Version 0.2.0):**
```python
# New way - automatic resource management
with nlm.Config() as config:
    config.set("key", "value")
    
    with nlm.SimpleWorld() as world:
        world.configure(100.0, 100.0, 10, 10)
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # ... use objects ...
```

**Before (Version 0.1.0):**
```python
# Old way - basic simulation
brain = nlm.Brain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
    action = brain.produceAction()
```

**After (Version 0.2.0):**
```python
# New way - using convenience methods
with nlm.Brain(config) as brain:
    brain.initialize()
    brain.runSimulation(100)  # Batch operation
```

**Before (Version 0.1.0):**
```python
# Old way - manual action creation
action = nlm.Action(nlm.ActionType.MoveForward)
action.setParameters([1.0, 0.0])
```

**After (Version 0.2.0):**
```python
# New way - convenience methods
action = nlm.Action.createMove(1.0, 0.0)
```

### From Basic to Advanced Usage

**Beginner Level:**
```python
import pynlm as nlm

# Simple "Hello World" example
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()
brain.step(0)

print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
```

**Intermediate Level:**
```python
import pynlm as nlm

# Create a complete simulation
with nlm.Config() as config:
    config.setDefaults("development")
    
    with nlm.SimpleWorld() as world:
        world.configure(20.0, 20.0, 8, 8)
        world.setAgentStart(10.0, 10.0)
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                agent.enableCuriosity(True)
                
                # Run simulation
                for step in range(50):
                    world.update(0.1)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
```

**Advanced Level:**
```python
import pynlm as nlm

# Advanced simulation with multiple agents and optimization
class AdvancedNLMSimulation:
    def __init__(self, config_profile="production"):
        self.config = nlm.Config()
        self.config.setDefaults(config_profile)
        self.episodes = []
    
    def run_experiment(self, num_agents=3, episodes=10):
        """Run advanced experiment with multiple agents."""
        # Create shared world
        with nlm.SimpleWorld() as world:
            world.createGridWorld(100.0, 100.0, 0.1)
            world.setAgentGoal(80.0, 80.0)
            
            # Run multiple agents
            for agent_id in range(num_agents):
                with nlm.Brain(self.config) as brain:
                    brain.initialize()
                    
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        agent.enableCuriosity(True)
                        agent.enableRewardModulation(True)
                        
                        # Run multiple episodes for this agent
                        for episode in range(episodes):
                            self.run_episode(agent, world, episode)
                            metrics = agent.getPerformanceMetrics()
                            self.episodes.append(metrics)
    
    def run_episode(self, agent, world, episode):
        """Run a single episode."""
        for step in range(100):
            world.update(0.1)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
    
    def analyze_results(self):
        """Analyze experiment results."""
        if not self.episodes:
            return {}
        
        # Calculate statistics
        total_success = sum(episode['success_rate'] for episode in self.episodes)
        avg_success = total_success / len(self.episodes)
        
        return {
            'total_episodes': len(self.episodes),
            'average_success_rate': avg_success,
            'max_success_rate': max(episode['success_rate'] for episode in self.episodes),
            'min_success_rate': min(episode['success_rate'] for episode in self.episodes)
        }
```

## Summary

This Python API Reference provides comprehensive documentation for the NLM Python bindings, covering:

1. **Core Classes:** Config, Brain, AgentBrain, SimpleWorld, Vision, Audio, InternalSignals, Action
2. **API Reference:** Detailed method documentation with examples
3. **Serialization:** Support for JSON, Binary, and Incremental serialization
4. **Error Handling:** Custom exceptions and validation
5. **Performance Optimization:** Tips for memory and simulation performance
6. **Code Organization:** Best practices for modular design
7. **Migration Guide:** Guidance for upgrading from previous versions

The NLM Python bindings provide a powerful and Pythonic interface to the Neural Learning Machine framework, enabling researchers and developers to create sophisticated neural simulations with ease.
