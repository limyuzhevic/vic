# README for Python Development

This directory contains the Python bindings for NLM (Neural Learning Machine).

## Building and Installation

### Using PyProject.toml (Recommended)

```bash
# Build the Python bindings
cd /workspace/app
pip install -e .

# Or build in development mode
pip install -e . --no-build-isolation
```

### Using CMake Directly

```bash
# Create build directory
cd /workspace/app
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=/usr/bin/python3

# Build the Python module
make -j$(nproc)

# Install the module
make install
```

## Python Usage

### Basic Import

```python
import pynlm as nlm

# Create a configuration
with nlm.Config() as config:
    config.set("learning_rate", 0.01)
    config.set("neuron_count", 500)
    
# Create a brain with configuration
with nlm.Brain(config) as brain:
    brain.initialize()
    
# Create a world
with nlm.SimpleWorld() as world:
    world.configure(100.0, 100.0, 10, 10)
    
# Create agent brain
with nlm.AgentBrain(brain) as agent:
    agent.initialize(world)
```

### Context Managers

The Python bindings include context managers for proper resource management:

1. **ConfigContextManager** - Manages `Config` objects
2. **BrainContextManager** - Manages `Brain` objects
3. **AgentBrainContextManager** - Manages `AgentBrain` objects
4. **SimpleWorldContextManager** - Manages `SimpleWorld` objects
5. **SensoryInputContextManager** - Manages `SensoryInput` objects
6. **ActionContextManager** - Manages `Action` objects

Each context manager implements Python's `__enter__()` and `__exit__()` methods for automatic resource cleanup.

### Example Usage

```python
import pynlm as nlm

# Complete simulation example
with nlm.Config() as config:
    config.set("agent.exploration", 0.2)
    
    with nlm.SimpleWorld() as world:
        world.configure(200.0, 200.0, 20, 20)
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                
                for step in range(100):
                    world.update(0.1)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action.getType(), world.getSimulationTime())
```

## Testing

### Running Tests

```bash
cd /workspace/app

# Run Python tests
pytest python/tests/

# Run with verbose output
pytest python/tests/ -v

# Run all tests with coverage
pytest python/tests/ --cov=pynlm
```

### Test Files

- `python/tests/test_context_managers.py` - Comprehensive context manager tests
- `python/examples/context_manager_examples.py` - Real-world examples

## Testing Requirements

Install testing dependencies:

```bash
cd /workspace/app
pip install -e .[dev]
```

## API Reference

### Core Classes

#### Config
- `loadFromFile(filepath)`: Load configuration from JSON
- `loadFromArgs(argc, argv)`: Load from command line arguments
- `saveToFile(filepath)`: Save to JSON file
- `has(key)`: Check if key exists
- `getKeys()`: Get all configuration keys
- `clear()`: Clear all configuration entries
- `summary()`: Get configuration summary

**Pythonic Convenience Methods:**
- `setDefaults(config_type)`: Apply predefined configuration profiles ("default", "high_performance", "quick_test")
- `mergeConfig(other_config)`: Merge configuration dictionaries
- `validateConfig()`: Validate configuration and return list of issues
- `getSummary()`: Get configuration as Python dict
- `updateFromDict(config_dict)`: Update config from dictionary

#### Brain
- `initialize()`: Initialize brain with configuration
- `step(currentStep)`: Perform simulation step
- `step(currentStep, currentTime)`: Perform step with timestamp
- `receiveSensoryInput(input)`: Inject sensory input
- `produceAction()`: Generate motor action
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state

**Pythonic Convenience Methods:**
- `runSimulation(steps)`: Run simulation for N steps (chainable)
- `createStandardBrain(config)`: Factory method for common brain configurations
- `getNeuralStats()`: Returns brain state as Python dict
- `simulateWithInput(inputs, steps)`: Convenience for running with sensory input
- `trainAgent(world, steps)`: High-level agent training method

#### AgentBrain
- `initialize(world)`: Initialize with world
- `decodeMotorCommand()`: Decode brain activity to motor command
- `processSensoryInput(percept)`: Process sensory input
- `applyRewardModulation(reward, predictedReward)`: Apply reward-based learning

**Pythonic Convenience Methods:**
- `learnFromWorld(world, steps)`: High-level learning method
- `createAgentSequence(world, num_episodes)`: Create training sequences
- `getPerformanceMetrics()`: Get training metrics as Python dict
- `setLearningParameters(params)`: Set learning parameters from dict

#### SimpleWorld
- `configure(width, height, visionWidth, visionHeight)`: Configure world
- `update(timestep)`: Update world state
- `applyMotorCommand(cmd, currentTime)`: Apply action to agent
- `getSensoryPercept()`: Get agent's sensory perception
- `getAgentBody()`: Get agent state

**Pythonic Convenience Methods:**
- `createGridWorld(width, height, obstacle_density)`: Create world with obstacles
- `addResource(x, y, value)`: Convenience method for adding resources
- `setAgentGoal(x, y)`: Set agent goal position
- `getObservationMap()`: Get vision as 2D array for easy visualization

#### SensoryInput (Base Class)
- `Vision`: Visual sensory input
- `Audio`: Audio sensory input
- `InternalSignals`: Internal brain state signals

**Pythonic Convenience Methods:**
- `getVisionArray()`: Get vision data as 2D array
- `getTouchStrength()`: Get touch data as Python list
- `getInternalState()`: Get internal signals as Python dict

#### Action
- `getType()`: Get action type
- `setType(type)`: Set action type
- `getParameters()`: Get action parameters
- `setParameters(params)`: Set action parameters
- `getName()`: Get human-readable action name

**Pythonic Convenience Methods:**
- `createMove(x, y)`: Create movement action
- `createInteract()`: Create interaction action
- `createCustom(params)`: Create custom action from dict
- `actionFromDict(action_dict)`: Create action from dictionary

## Development

### Adding New Context Managers

1. Create a wrapper class following the pattern of existing context managers
2. Implement `__enter__()` and `__exit__()` methods
3. Add the class binding in `bindings.cpp`
4. Update test files as needed

### Modifying Context Managers

1. Edit the wrapper class definition
2. Update the context manager binding in `bindings.cpp`
3. Run tests to verify changes

### Code Style

Follow these guidelines:
- Use C++20 standard
- Include proper error handling
- Use smart pointers for resource management
- Follow pybind11 best practices

## Pythonic Convenience Methods Examples

### Brain Class Examples

#### Running a Simulation with Convenience Methods

```python
import pynlm as nlm

# Create configuration with Pythonic convenience
with nlm.Config() as config:
    config.setDefaults("quick_test")  # Use predefined profile
    config.updateFromDict({
        "neuronCount": 100,
        "connectionProbability": 0.2,
        "learningRate": 0.05
    })  # Merge and update

# Create brain and get stats in one go
with nlm.Brain(config) as brain:
    brain.initialize()
    
    # Get neural statistics as Python dict
    stats = brain.getNeuralStats()
    print(f"Brain has {stats['total_neurons']} neurons")
    
    # Create sensory input
    with nlm.Vision(20, 20) as vision:
        vision.setData([0.5] * (20 * 20 * 3))
        
        # Run simulation with input
        brain.simulateWithInput(vision, 50)
    
    # Get final stats
    final_stats = brain.getNeuralStats()
    print(f"After simulation: {final_stats['total_spikes']} spikes")

print("✓ Simulation completed with Pythonic convenience methods")
```

#### High-Level Agent Training

```python
import pynlm as nlm

# Create world with convenience methods
with nlm.SimpleWorld() as world:
    world.createGridWorld(100.0, 100.0, 0.1)  # World with obstacles
    world.setAgentGoal(90.0, 90.0)  # Set agent goal
    
    # Create brain and agent
    with nlm.Config() as config:
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                
                # High-level training with convenience methods
                metrics = agent.learnFromWorld(world, 200)
                print(f"Training completed: {metrics['success_rate']*100:.1f}% success rate")
                
                # Show performance metrics
                perf_metrics = agent.getPerformanceMetrics()
                print(f"Neuromodulation: {perf_metrics['neuromodulation_level']:.3f}")
                print(f"Curiosity: {perf_metrics['curiosity_level']:.3f}")
```

### Config Class Examples

#### Configuration Management

```python
import pynlm as nlm

# Create and manage configuration with convenience methods
with nlm.Config() as config:
    # Apply predefined profile
    config.setDefaults("high_performance")
    
    # Get configuration as Python dict for inspection
    summary = config.getSummary()
    print(f"Configuration keys: {list(summary.keys())}")
    
    # Merge with additional settings
    config.mergeConfig({
        "agent.exploration": 0.3,
        "agent.learning_rate": 0.001
    })
    
    # Validate configuration
    issues = config.validateConfig()
    if issues:
        print("Configuration issues:")
        for issue in issues:
            print(f"  - {issue}")
    else:
        print("✓ Configuration is valid")
    
    # Update from dictionary
    config.updateFromDict({
        "simulation.max_steps": 5000,
        "simulation.timestep": 0.001
    })
    
    print(f"Final summary: {len(summary)} configuration items")
```

### AgentBrain Class Examples

#### Advanced Training Sequences

```python
import pynlm as nlm

# Create training environment
with nlm.SimpleWorld() as world:
    world.configure(200.0, 200.0, 20, 20)
    
    # Create training sequences
    sequences = nlm.createAgentSequence(world, 5, 100)
    
    print("Training sequences created:")
    for i, seq in enumerate(sequences):
        print(f"  Episode {seq['episode']}: {seq['success_rate']*100:.1f}% success rate")
    
    # Get average performance
    total_success = sum(seq['success_rate'] for seq in sequences) / len(sequences)
    print(f"Average success rate: {total_success*100:.1f}%")
```

#### Setting Learning Parameters

```python
import pynlm as nlm

# Create agent brain
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

### SimpleWorld Class Examples

#### World Creation and Visualization

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

### SensoryInput Class Examples

#### Processing Different Sensory Inputs

```python
import pynlm as nlm

# Process vision data
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
    
    print("✓ Vision processed as 2D array")

# Process internal signals
with nlm.InternalSignals() as internal:
    internal.addSignal(0.8)  # Dopamine level
    internal.addSignal(0.3)  # Curiosity level
    internal.addSignal(0.5)  # Novelty level
    internal.addSignal(0.1)  # Prediction error
    
    # Get internal state as Python dict
    internal_state = internal.getInternalState()
    
    print("Internal state:")
    for key, value in internal_state.items():
        print(f"  {key}: {value:.3f}")
    
    print("✓ Internal signals extracted as Python dict")
```

### Action Class Examples

#### Creating Actions with Convenience Methods

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

print("✓ All action types created using Pythonic convenience methods")
```

## Complete Example: Pythonic Agent Training

Here's a complete example showing how to use the Pythonic convenience methods for training an agent:

```python
import pynlm as nlm

print("=== Pythonic Agent Training Example ===\n")

# 1. Create and configure the world
print("1. Creating simulation world...")
with nlm.SimpleWorld() as world:
    world.createGridWorld(100.0, 100.0, 0.1)  # World with obstacles
    world.setAgentStart(10.0, 10.0)  # Agent start position
    world.setAgentGoal(90.0, 90.0)  # Agent goal
    
    # Add some resources to collect
    for x in [20.0, 40.0, 60.0, 80.0]:
        for y in [20.0, 40.0, 60.0, 80.0]:
            world.addResource(x, y, 1.0)
    
    print("   ✓ World configured with obstacles, resources, and goals\n")

# 2. Create brain with configuration
print("2. Setting up neural configuration...")
with nlm.Config() as config:
    config.setDefaults("high_performance")  # Use predefined profile
    config.updateFromDict({
        "agent.exploration": 0.3,
        "agent.learning_rate": 0.01
    })  # Merge additional settings
    
    config_summary = config.getSummary()
    print(f"   ✓ Configuration created with {len(config_summary)} parameters\n")

# 3. Create and initialize agent
print("3. Creating and initializing agent...")
with nlm.Brain(config) as brain:
    brain.initialize()
    
    with nlm.AgentBrain(brain) as agent:
        agent.initialize(world)
        agent.enableCuriosity(True)
        agent.enableRewardModulation(True)
        
        initial_metrics = agent.getPerformanceMetrics()
        print(f"   ✓ Agent initialized - Curiosity: {initial_metrics['curiosity_level']:.3f}\n")

# 4. Run training episode
print("4. Running training episode...")
with nlm.AgentBrain(brain) as agent:
    # Use high-level convenience method for training
    training_metrics = agent.learnFromWorld(world, 100)
    
    print("   Training Results:")
    print(f"   - Success rate: {training_metrics['success_rate']*100:.1f}%")
    print(f"   - Average reward: {training_metrics['average_reward']:.3f}")
    print(f"   - Total reward: {training_metrics['total_reward']:.3f}")
    print("   ✓ Training completed\n")

# 5. Show final performance
print("5. Final performance analysis...")
with nlm.AgentBrain(brain) as agent:
    final_metrics = agent.getPerformanceMetrics()
    
    print("   Final Agent State:")
    for key, value in final_metrics.items():
        if isinstance(value, float):
            print(f"   - {key}: {value:.3f}")
        else:
            print(f"   - {key}: {value}")
    
    print("\n=== Training Complete ===")
    print("✓ Pythonic convenience methods successfully used for agent training")
```
