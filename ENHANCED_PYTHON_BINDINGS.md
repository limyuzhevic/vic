# NLM (Neural Learning Machine) - Enhanced Python Documentation

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. This document provides comprehensive documentation for the enhanced Python bindings of NLM.

## Key Improvements

This enhanced version (3.0.0-enhanced) introduces significant improvements over the original Python bindings:

### 1. Custom Exception Hierarchy
- **NLMConfigurationError**: For configuration-related errors
- **NLMBrainError**: For brain-specific errors
- **NLMFileError**: For file I/O errors
- **NLMWorldError**: For world simulation errors
- **NLMRuntimeError**: Base runtime error class

### 2. Enhanced Python Interface
- **Properties**: Direct attribute access for key classes
- **Context managers**: Automatic resource management with `with` statements
- **Type-safe API**: Methods that check types and throw appropriate exceptions
- **Factory methods**: Convenience methods for common operations

### 3. NumPy Integration
- **Vision**: Direct numpy array support for vision data
- **Audio**: Direct numpy array support for audio data
- **Performance**: Optimized for large data arrays

### 4. Advanced Features
- **Brain state inspection**: Access neurons, synapses, and regions by ID
- **World utilities**: Ray casting, object management, path planning
- **Configuration query API**: Type-safe configuration access methods
- **Action factory methods**: Predefined action creation methods

### 5. Error Handling
- **Detailed error messages**: Contextual error information
- **Exception chaining**: Proper exception hierarchy
- **Type checking**: Type-safe method calls

## API Documentation

### Core Classes

#### Brain Class
```python
# Create brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize brain
brain.initialize()

# Simulation
for step in range(1000):
    brain.step(step)  # Simple step
    # or brain.step(step, current_time)  # With timestamp

# Access statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")

# Brain state inspection
region_id = brain.getRegionIds()[0]
region = brain.getRegion(region_id)
print(f"Region: {region.getName()}")

# Get statistics as string
stats = brain.getStatistics()

# Context manager support
with brain:
    brain.step(0, 0.1)
    # Brain automatically reset when exiting context
```

#### Config Class (Enhanced)
```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Type-safe configuration access
config.setInt("brain.neuron_count", 1000)
config.setDouble("brain.synapse_density", 0.1)
config.setBool("plasticity.stdp.enable", True)
config.setString("world.name", "test_world")

# Type-safe get methods
neuron_count = config.getInt("brain.neuron_count")
synapse_density = config.getDouble("brain.synapse_density")
plasticity_enabled = config.getBool("plasticity.stdp.enable")
world_name = config.getString("world.name")

# Convenience methods for configuration
config.configureBrain(2000, 0.2)  # Set brain.neuron_count and brain.synapse_density
config.configureWorld(30, 30)    # Set world.width and world.height

# Get configuration summary
print(config.summary())

# Check if key exists
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.getOrInt('brain.neuron_count', 1000)}")

# Vector support
config.setIntVector("test.vector", [1, 2, 3, 4, 5])
vector = config.getIntVector("test.vector")
print(f"Vector: {vector}")
```

#### AgentBrain Class (Enhanced)
```python
# Create agent brain
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Process sensory input and update development (convenience method)
agent.processStep(world, world.getSensoryPercept(), 0.1)

# Get agent status
status = agent.getStatus()
print(status)

# Execute action
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

#### SimpleWorld Class (Enhanced)
```python
# Create world
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()
world.setAgentStart(10.0, 10.0)

# World utilities
world.update(0.1)

# Ray casting for object detection
start_pos = world.getAgentBody().getPosition()
result = world.rayCast(start_pos[0], start_pos[1], 0.0, 10.0)
if result[0]:  # If hit
    obj_type = result[1]
    value = result[2]
    distance = result[3]
    print(f"Found {obj_type} at distance {distance:.2f}")

# Get objects in range
objects = world.getObjectsInRange(10.0, 10.0, 5.0)
for obj in objects:
    print(f"Object at ({obj.x}, {obj.y}): {obj.type}")

# Get closest object
closest_result = world.getClosestObject(10.0, 10.0)
if closest_result[0]:
    obj = closest_result[1]
    print(f"Closest object: {obj.type} at ({obj.x}, {obj.y})")

# Create world objects
resource = world.createResource(15.0, 15.0, 1.0)
hazard = world.createHazard(18.0, 18.0, 2.0)
wall = world.createWall(5.0, 5.0, 1.0)
```

#### Action Class (Enhanced)
```python
# Create actions using factory methods
action1 = Action.createMoveForward()
action2 = Action.createTurnLeft()
action3 = Action.createInteract()
action4 = Action.createWait()

# Create actions with parameters
look_action = Action.createLook(ActionType.LookUp)
eat_action = Action.createEat()
rest_action = Action.createRest()

# Manual action creation
action = Action(ActionType.Custom)
action.setType(ActionType.Custom)
action.setParameters([0.1, 0.2, 0.3])

# Action utilities
if action.isComplete():
    print("Action is complete")

if action.hasParameter(0):
    param_value = action.getParameterValue(0)
    print(f"Parameter 0: {param_value}")

if action.validateParameters():
    print("Action parameters are valid")

print(f"Action name: {action.getName()}")
```

#### Vision Class (NumPy Enhanced)
```python
# Create vision with numpy support
vision = Vision(8, 8, 3)

# Set vision data using numpy array
import numpy as np
vision_data = np.random.rand(8, 8, 3).astype(np.float32)
vision.setData(vision_data)  # Accepts numpy array directly

# Get vision data as numpy array
vision_numpy = vision.getDataAsNumpy()  # Returns numpy array
print(f"Vision shape: {vision_numpy.shape}")
print(f"Vision dtype: {vision_numpy.dtype}")

# Alternative: use Python lists
vision_data_list = [[0.1, 0.2, 0.3] for _ in range(8 * 8)]
vision.setDataList(vision_data_list)
```

#### Audio Class (NumPy Enhanced)
```python
# Create audio with numpy support
audio = Audio(44100, 1024)

# Set audio data using numpy array
import numpy as np
audio_data = np.random.randn(44100, 1024).astype(np.float32)
audio.setData(audio_data)

# Get audio data as numpy array
audio_numpy = audio.getDataAsNumpy()
print(f"Audio shape: {audio_numpy.shape}")
print(f"Audio sample rate: {audio.getSampleRate()}")
```

#### SensoryPercept Class (Enhanced)
```python
# Create sensory percept
percept = SensoryPercept()

# Set vision data
vision = Vision(8, 8, 3)
vision_data = np.random.rand(8, 8, 3).astype(np.float32)
vision.setData(vision_data)
percept.setVision(vision)

# Check what data is present
if percept.hasVision():
    print(f"Vision dimensions: {percept.getVisionWidth()}x{percept.getVisionHeight()}")

if percept.hasAudio():
    print(f"Audio samples: {percept.getAudio().getNumSamples()}")

if percept.hasInternal():
    print(f"Internal signals: {percept.getInternal().getAllSignals()}")

if percept.hasProprioception():
    print(f"Proprioception signals: {percept.getProprioception().getAllSignals()}")

# Get all signals as combined vector
all_signals = percept.getAllSignals()
```

### Module-Level Functions

#### Factory Functions
```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)

# Create world
world = pynlm.createSimpleWorld()

# Create agent brain
agent = pynlm.createAgentBrain(brain)
```

#### Utility Functions
```python
# Get pre-configured default brain config
default_config = pynlm.getDefaultBrainConfig()

# Get quick test config
test_config = pynlm.getQuickTestConfig()

# Format error messages
error_msg = pynlm.formatError("brain_step", "Invalid step number")
print(error_msg)

# Create action with parameters
action = pynlm.createActionWithParams(ActionType.Custom, [0.1, 0.2, 0.3])
```

### Constants

#### ActionType Enum
```python
from pynlm import ActionType

# Available action types
actions = [
    ActionType.MoveForward,
    ActionType.MoveBackward,
    ActionType.MoveLeft,
    ActionType.MoveRight,
    ActionType.TurnLeft,
    ActionType.TurnRight,
    ActionType.Look,
    ActionType.LookUp,
    ActionType.LookDown,
    ActionType.Interact,
    ActionType.Eat,
    ActionType.Drink,
    ActionType.Rest,
    ActionType.Wait,
    ActionType.Custom
]
```

#### MotorCommand Enum
```python
from pynlm import MotorCommand

# Available motor commands
commands = [
    MotorCommand.MoveForward,
    MotorCommand.MoveBackward,
    MotorCommand.TurnLeft,
    MotorCommand.TurnRight,
    MotorCommand.LookLeft,
    MotorCommand.LookRight,
    MotorCommand.Interact,
    MotorCommand.Wait
]
```

#### WorldObjectType Enum
```python
from pynlm import WorldObjectType

# Available world object types
object_types = [
    WorldObjectType.Empty,
    WorldObjectType.Resource,
    WorldObjectType.Hazard,
    WorldObjectType.Wall,
    WorldObjectType.Marker
]
```

### Complete Example

#### Complete Agent Simulation
```python
import pynlm
import numpy as np

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with enhanced Python bindings."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    config.configureBrain(500, 0.05)  # 500 neurons, 5% synapse density
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Create initial vision data
    vision = pynlm.Vision(8, 8, 3)
    vision_data = np.random.rand(8, 8, 3).astype(np.float32)
    vision.setData(vision_data)
    
    # 8. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        action = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.1 if step % 10 == 0 else 0.0  # Reward every 10th step
        agent.applyRewardModulation(reward, 0.05)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    # 9. Print final statistics
    print("\nSimulation Complete!")
    print(brain.getStatistics())
    print(agent.getStatus())

# Run the simulation
if __name__ == "__main__":
    run_agent_simulation(1000)
```

#### Context Manager Example
```python
import pynlm

# Use brain with context manager
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

with brain:
    brain.initialize()
    brain.step(0, 0.1)
    brain.step(1, 0.2)
    # Brain automatically reset when exiting context
    # No need to call brain.reset() manually

print("Brain context completed successfully")
```

#### Error Handling Example
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Example of error handling
try:
    config.setInt("invalid_key", "not_an_int")  # This will fail type checking
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")

try:
    # Try to get non-existent key
    value = config.getInt("nonexistent.key")
except pynlm.ConfigurationError as e:
    print(f"Key not found error: {e}")

try:
    # Try to get region by name that doesn't exist
    region = brain.getRegionByName("nonexistent.region")
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
```

#### NumPy Performance Example
```python
import pynlm
import numpy as np
import time

# Create vision
vision = pynlm.Vision(100, 100, 3)

# Create large vision data using numpy
print("Creating large vision data...")
start_time = time.time()
vision_data = np.random.rand(100, 100, 3).astype(np.float32)
vision.setData(vision_data)
print(f"Vision data creation time: {time.time() - start_time:.3f} seconds")

# Process vision data
start_time = time.time()
processed_data = vision.getDataAsNumpy() * 0.5  # Simple processing
print(f"Vision processing time: {time.time() - start_time:.3f} seconds")

# Use processed data
mean_value = np.mean(processed_data)
print(f"Mean vision value: {mean_value:.3f}")
```

## Migration Guide

### From Original to Enhanced API

#### Configuration Access
**Original:**
```python
config.set("brain.neuron_count", 1000)
neuron_count = config.get("brain.neuron_count", 0)
```

**Enhanced:**
```python
config.setInt("brain.neuron_count", 1000)
neuron_count = config.getInt("brain.neuron_count")
```

#### Action Creation
**Original:**
```python
action = Action(ActionType.MoveForward)
```

**Enhanced:**
```python
action = Action.createMoveForward()
```

#### Vision Processing
**Original:**
```python
vision.setData(vision_data)  # Python list
```

**Enhanced:**
```python
vision.setData(vision_data)  # numpy array (recommended)
```

#### Error Handling
**Original:**
```python
# Basic error handling
```

**Enhanced:**
```python
# Specific exception types
except pynlm.ConfigurationError as e:
except pynlm.BrainError as e:
except pynlm.FileError as e:
except pynlm.WorldError as e:
```

## Troubleshooting

### Common Issues and Solutions

#### "ImportError: No module named 'pynlm'"
**Solution:** Install the enhanced Python bindings
```bash
pip install -e .
```

#### "Segmentation fault" when using brain.step()
**Solution:** Ensure brain is properly initialized
```python
brain = pynlm.createBrain(config)
brain.initialize()  # Must call this before step()
brain.step(0)  # Now safe to use
```

#### "AttributeError: 'Vision' object has no attribute 'setData'"
**Solution:** Use enhanced Vision class with numpy support
```python
from pynlm import Vision
import numpy as np

vision = Vision(8, 8, 3)
vision_data = np.random.rand(8, 8, 3).astype(np.float32)
vision.setData(vision_data)  # This should work with enhanced bindings
```

#### "ConfigurationError: No int value found for key"
**Solution:** Use type-safe configuration methods
```python
config = pynlm.createDefaultConfig()
# Instead of config.get("brain.neuron_count"), use:
neuron_count = config.getInt("brain.neuron_count")
```

## Performance Tips

### NumPy Integration
1. **Use numpy arrays for large data:** Vision, Audio, and other sensory data support numpy arrays for better performance.

2. **Batch operations:** Process multiple frames or audio segments using numpy vectorization.

3. **Memory efficiency:** NumPy arrays are more memory-efficient than Python lists for large numerical data.

### Configuration Optimization
1. **Pre-configure:** Use `pynlm.getDefaultBrainConfig()` or `pynlm.getQuickTestConfig()` for common configurations.

2. **Type safety:** Use type-safe methods (`getInt()`, `getDouble()`, etc.) to avoid runtime errors.

### Simulation Performance
1. **Context managers:** Use context managers for automatic resource cleanup.

2. **Statistics:** Use `brain.getStatistics()` for comprehensive performance monitoring.

3. **Development stages:** Monitor developmental stages for performance optimization.

## Future Enhancements

The enhanced Python bindings provide a foundation for future improvements:

### 1. Advanced Features
- **Real-time visualization:** Integration with matplotlib for real-time brain visualization
- **Machine learning integration:** Connect NLM with deep learning frameworks
- **Distributed computing:** Support for multi-GPU and cluster computing
- **Web interfaces:** REST API for web-based NLM simulations

### 2. Performance Optimizations
- **Cython integration:** Further performance optimizations using Cython
- **SIMD optimizations:** Vectorized operations using SIMD instructions
- **Memory pooling:** Advanced memory management for long-running simulations

### 3. Enhanced APIs
- **Fluent interface:** Chainable method calls for more readable code
- **Decorator patterns:** Add decorators for common functionality
- **Protocol-based interfaces:** Use Python protocols for type hints

## Acknowledgments

The enhanced Python bindings build upon the excellent work of the NLM research team. Special thanks to:

- The pybind11 developers for providing the binding framework
- The NLM C++ core developers for their excellent architecture
- The open-source community for their contributions

## License

This enhanced Python bindings package is part of the NLM (Neural Learning Machine) project. See the main NLM documentation for license information.

## Support

For issues, questions, or feature requests, please:

1. Check the main NLM documentation
2. Visit the NLM GitHub repository
3. Contact the NLM research team

---

*Document Version: 3.0.0-enhanced*
*Last Updated: 2026-10-20*
*Author: NLM Research Team*