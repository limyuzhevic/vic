# NLM Python Bindings - Enhanced API Guide

## Overview

The NLM (Neural Learning Machine) Python bindings have been enhanced to provide a complete and intuitive API for working with neural simulation, cognitive architectures, and world interaction. This guide covers all the new functionality added to make the Python API complete and developer-friendly.

## What's New

### 1. Added Missing Classes

#### Vision Class
```python
# Create vision input from image data
vision = pynlm.createVision(width, height, channels)
vision.setData(data)  # Accepts list or array of floats

# Vision properties
vision.getWidth()      # Image width in pixels
vision.getHeight()     # Image height in pixels  
vision.getChannels()   # Number of color channels (3 = RGB)
vision.getDimensions() # Total number of pixels
vision.normalize()     # Normalize pixel values to [0, 1]
vision.applyThreshold(0.5)  # Apply threshold to create binary image
vision.convertToGrayscale()  # Convert to grayscale
vision.clone()          # Create a copy
```

#### Audio Class
```python
# Create audio input
audio = pynlm.createAudio(sampleRate, numSamples)
audio.setData(audioData)  # Audio samples as list/array

# Audio properties
audio.getSampleRate()    # Samples per second (e.g., 44100 Hz)
audio.getNumSamples()    # Total number of samples
audio.getDuration()      # Audio duration in seconds
audio.normalize()        # Normalize to [-1, 1] range
audio.applyGain(0.5)      # Apply gain factor
```

#### InternalSignals Class
```python
# Create internal neural signals
internal_signals = pynlm.createInternalSignals()

# Manage signals
internal_signals.addSignal(value)  # Add a new signal
internal_signals.getSignal(index)  # Get signal by index
internal_signals.getAllSignals()   # Get all signals as list
internal_signals.clearSignals()    # Clear all signals
internal_signals.normalize()       # Normalize signal ranges
internal_signals.clone()            # Create copy
```

### 2. Added Action Type Enum
```python
# Action types for motor output
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.MoveLeft
pynlm.ActionType.MoveRight
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.LookUp
pynlm.ActionType.LookDown
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
pynlm.ActionType.Custom  # For custom actions with parameters
```

### 3. Enhanced Action Class
```python
# Create actions
action = pynlm.createAction(pynlm.ActionType.MoveForward)
custom_action = pynlm.createAction(pynlm.ActionType.Custom, [0.5, 1.0, 2.0])

# Action properties
action.getType()         # Get action type
action.setType(new_type)  # Set action type
action.getParameters()   # Get parameter values
action.setParameters(params)  # Set parameter values
action.getName()         # Get human-readable name
action.isValid()         # Check if action is valid

# Parameter manipulation
action.getParameter(index)    # Get specific parameter
action.setParameter(index, value)  # Set specific parameter
action.addParameter(value)    # Add a new parameter
```

### 4. Added MotorCommand Enum
```python
# Motor command enumeration for low-level control
pynlm.MotorCommand.MoveForward
pynlm.MotorCommand.MoveBackward
pynlm.MotorCommand.TurnLeft
pynlm.MotorCommand.TurnRight
pynlm.MotorCommand.LookLeft
pynlm.MotorCommand.LookRight
pynlm.MotorCommand.Interact
pynlm.MotorCommand.Wait
```

### 5. Enhanced AgentBrain Class
```python
# Agent brain with additional features
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Existing methods
agent.getSensoryInputSize()
agent.getMotorOutputSize()
agent.processSensoryInput(percept)
agent.decodeMotorCommand()
agent.applyRewardModulation(reward, predicted_reward)
agent.updateDevelopment(timestep)

# New convenience methods
agent.processVision(vision)  # Directly process vision input
agent.getSensoryInputStats()  # Get sensory input statistics
agent.getMotorOutputStats()   # Get motor output statistics

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Status checking
agent.isRewardModulationEnabled()
agent.isStructuralPlasticityEnabled()
agent.isDevelopmentEnabled()
agent.isCuriosityEnabled()
```

### 6. Enhanced Brain Class
```python
# Enhanced brain with additional methods
brain = pynlm.createBrain(config)
brain.initialize()

# Existing methods
brain.step(step)                    # Simple step
brain.step(step, time)              # Step with timestamp
brain.stepWithClock(clock)          # Step using simulation clock
brain.receiveSensoryInput(input)    # Inject sensory data
brain.receiveVision(vision)         # Directly inject vision
brain.receiveInternalSignals(signals)  # Directly inject internal signals
brain.injectCurrent(neuron, current)
brain.injectCurrentToNeurons(type, current)
brain.produceAction()
brain.reset()
brain.save(filepath)
brain.load(filepath)

# New statistical methods
brain.getNeuronCountByType(type)      # Get neurons by type
brain.getSynapseCountByType(type)     # Get synapses by type
brain.getPopulationCount(region_id)   # Get population in region
```

### 7. Enhanced Config Class
```python
# Enhanced configuration with type-specific setters/getters
config = pynlm.createDefaultConfig()

# Type-specific setters
config.setBool("key", True)
config.setFloat("key", 3.14)
config.setInt("key", 42)
config.setDouble("key", 3.14159)

# Type-specific getters
value = config.getBool("key")
value = config.getFloat("key")
value = config.getInt("key")
value = config.getDouble("key")

# Generic setters/getters
config.set("key", ConfigValue)  # Works with any supported type
value = config.get("key")       # Returns optional[T]

# Convenience methods
config.set("key", value)        # Automatic type conversion
default_value = config.getOr("key", 100)  # With default value
```

### 8. Enhanced SensoryPercept Class
```python
# Enhanced sensory percept with additional methods
percept = pynlm.SensoryPercept()
percept.setVision(vision)
percept.setAudio(audio)
percept.setInternal(internal_signals)

# Modality checking
percept.hasVision()           # Check if percept has vision data
percept.hasAudio()           # Check if percept has audio data
percept.hasInternal()        # Check if percept has internal signals
percept.hasTouch()           # Check if percept has touch data
percept.hasProprioception()  # Check if percept has proprioception

# Advanced operations
percept.addSignal("vision", data)  # Add signal of specific type
percept.merge(other_percept)      # Merge with another percept
percept.toDict()                  # Convert to Python dictionary
```

### 9. Enhanced WorldObject Class
```python
# Enhanced world object with additional methods
world_object = pynlm.WorldObject(x, y, type, value, radius)

# Object properties
world_object.x              # X coordinate
world_object.y              # Y coordinate
world_object.radius         # Collision radius
world_object.type           # Object type
world_object.value          # Object value
world_object.active         # Whether object is active

# Helper methods
world_object.isResource()   # Check if object is a resource
world_object.isHazard()     # Check if object is a hazard
distance = world_object.getDistanceTo(x, y)  # Distance to position
```

### 10. Enhanced AgentBody Class
```python
# Enhanced agent body with additional methods
body = pynlm.AgentBody()
body.x              # X position
body.y              # Y position
body.orientation    # Facing direction
body.velocityX      # X velocity
body.velocityY      # Y velocity
body.energy         # Current energy
body.health         # Health (0-100)
body.age           # Age in simulation steps

# Helper methods
body.getPosition()  # Get as (x, y) tuple
body.setPosition(x, y)
body.getVelocity()  # Get as (vx, vy) tuple
body.setVelocity(vx, vy)
body.getSpeed()    # Get speed (magnitude of velocity)
body.getDirection()  # Get direction in radians
body.isAlive()     # Check if agent is alive
```

## Factory Functions

### Core Factory Functions
```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain with configuration
brain = pynlm.createBrain(config)

# Create simple world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)
```

### Enhanced Factory Functions
```python
# Create sensory inputs with specified dimensions
vision = pynlm.createVision(8, 8, 3)  # 8x8 RGB image
audio = pynlm.createAudio(44100, 1024)  # 44.1kHz, 1 second
internal_signals = pynlm.createInternalSignals()

# Create actions
move_action = pynlm.createAction(pynlm.ActionType.MoveForward)
custom_action = pynlm.createAction(pynlm.ActionType.Custom, [0.5, 1.0, 2.0])

# Create action from name (for human-readable interface)
action = pynlm.createActionFromParams("MoveForward")
```

## Convenience Methods

### Brain Step Processing
```python
# Multiple ways to step the brain
brain.step(0)                    # Simple step
brain.step(0, 0.1)                # Step with time
brain.step(0, 0.1)                # Step with time
brain.stepWithClock(clock)       # Use simulation clock

# Step with error handling
try:
    brain.step(step)
except pynlm.RuntimeError as e:
    print(f"Brain step failed: {e}")
```

### Reward Modulation
```python
# Apply reward modulation
agent.applyRewardModulation(reward, predicted_reward)

# Check neuromodulation level
level = agent.getNeuromodulationLevel()
curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()
prediction_error = agent.getPredictionError()
```

### Development Controls
```python
# Update development system
agent.updateDevelopment(0.1)  # Update with timestep

# Check developmental stage
stage = brain.getDevelopmentalStage()
stage_name = ["Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"][stage.value]

# Enable development features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

## Error Handling

The enhanced bindings provide Pythonic error handling:

```python
import pynlm

try:
    # Code that might fail
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(0)
except pynlm.InvalidArgument as e:
    print(f"Invalid argument: {e}")
except pynlm.IndexError as e:
    print(f"Index error: {e}")
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Type Hints and IDE Support

The enhanced bindings provide comprehensive type hints for better IDE support:

```python
# All classes have proper type hints
config: pynlm.Config
brain: pynlm.Brain
agent: pynlm.AgentBrain
world: pynlm.SimpleWorld
vision: pynlm.Vision
audio: pynlm.Audio
internal_signals: pynlm.InternalSignals
action: pynlm.Action
world_object: pynlm.WorldObject
agent_body: pynlm.AgentBody
percept: pynlm.SensoryPercept

# Enums have proper type hints
neuron_type: pynlm.NeuronType
action_type: pynlm.ActionType
motor_command: pynlm.MotorCommand
```

## Example Usage

### Complete Agent Simulation
```python
import pynlm
import numpy as np

def run_complete_simulation():
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning and development
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run simulation
    for step in range(1000):
        world.update(0.1)
        
        # Get vision input
        vision = pynlm.createVision(8, 8, 3)
        image_data = np.random.randn(8*8*3).astype(np.float32)
        vision.setData(image_data.tolist())
        
        # Create percept
        percept = pynlm.SensoryPercept()
        percept.setVision(vision)
        
        # Process and interact
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = np.random.randn()
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Firing={brain.getFiringNeuronCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"DevStage={brain.getDevelopmentalStage().name}")
    
    print("Simulation complete!")
    
    # Save brain state
    brain.save("brain_checkpoint.bin")

# Run the simulation
run_complete_simulation()
```

### Vision Processing Example
```python
import pynlm
import numpy as np

# Create and configure vision
vision = pynlm.createVision(64, 64, 3)  # 64x64 RGB image

# Generate test image (gradient)
for x in range(64):
    for y in range(64):
        for c in range(3):
            idx = (y * 64 + x) * 3 + c
            vision.setDataPtr(np.random.randn(64*64*3).astype(np.float32))

# Process vision
vision.normalize()           # Normalize to [0, 1]
vision.applyThreshold(0.5)   # Create binary image
vision.convertToGrayscale()  # Convert to single channel

# Get statistics
width, height, channels = vision.getWidth(), vision.getHeight(), vision.getChannels()
print(f"Vision: {width}x{height}x{channels}")

# Create percept
percept = pynlm.SensoryPercept()
percept.setVision(vision)

print(f"Percept has vision: {percept.hasVision()}")
```

### Action Creation Example
```python
import pynlm

# Create different types of actions
move_forward = pynlm.createAction(pynlm.ActionType.MoveForward)
move_backward = pynlm.createAction(pynlm.ActionType.MoveBackward)
turn_left = pynlm.createAction(pynlm.ActionType.TurnLeft)
turn_right = pynlm.createAction(pynlm.ActionType.TurnRight)
interact = pynlm.createAction(pynlm.ActionType.Interact)

# Create actions with parameters (e.g., for custom behavior)
walk_to_target = pynlm.createAction(pynlm.ActionType.Custom, [0.5, 2.0, 1.0])  # speed, duration, angle

# Validate actions
for action in [move_forward, move_backward, turn_left, turn_right, interact]:
    if action.isValid():
        print(f"{action.getName()}: VALID")
    else:
        print(f"{action.getName()}: INVALID")

# Use action in world
world.applyMotorCommand(move_forward, 1.0)
```

## Performance Tips

### Efficient Memory Usage
```python
# Reuse objects instead of creating new ones
vision = pynlm.createVision(8, 8, 3)
data = np.random.randn(8*8*3).astype(np.float32)

# Process multiple times
for i in range(100):
    vision.setData(data.tolist())
    # Process vision...
```

### Vectorized Operations
```python
# Use numpy for batch operations
images = np.random.randn(100, 8*8*3).astype(np.float32)

for i in range(100):
    vision = pynlm.createVision(8, 8, 3)
    vision.setData(images[i].tolist())
    # Process...
```

### Configuration Optimization
```python
# Configure once, reuse
config = pynlm.createDefaultConfig()
config.setInt("brain.neuron_count", 1000)
config.setFloat("brain.synapse_density", 0.1)
config.setBool("plasticity.stdp.enable", True)

# Create multiple brains with same config
brain1 = pynlm.createBrain(config)
brain2 = pynlm.createBrain(config)
```

## Migration Guide

### From Old API to New API

| Old API | New API | Notes |
|---------|---------|-------|
| `pynlm.SensoryInput` | `pynlm.SensoryInput` | Same base class |
| `pynlm.Vision` | `pynlm.Vision` | Same class, enhanced |
| `pynlm.Audio` | `pynlm.Audio` | Same class, enhanced |
| `pynlm.InternalSignals` | `pynlm.InternalSignals` | Same class, enhanced |
| `pynlm.Action` | `pynlm.Action` | Same class, enhanced |
| `pynlm.AgentBrain` | `pynlm.AgentBrain` | Same class, enhanced |
| `pynlm.Brain` | `pynlm.Brain` | Same class, enhanced |
| `pynlm.createAction(type)` | `pynlm.createAction(type, params)` | Added parameters |
| `pynlm.createAction(type, params)` | `pynlm.createAction(type, params)` | Same |

## Troubleshooting

### Common Issues

1. **Module not found**
   - Ensure you're using the right Python environment
   - Rebuild the module with `pip install -e .` (if pip available)

2. **Import errors**
   - Check that all dependencies are installed
   - Verify the module was built correctly

3. **Type errors**
   - Use type hints to catch errors early
   - Check parameter types in documentation

4. **Performance issues**
   - Profile your code to identify bottlenecks
   - Use object reuse for repeated operations
   - Consider batch processing for large datasets

### Error Messages

- `InvalidArgument`: Thrown when invalid arguments are provided
- `IndexError`: Thrown when array index is out of bounds
- `RuntimeError`: Thrown for runtime errors (e.g., initialization without calling `initialize()`)

## References

- NLM Documentation: https://nlm.readthedocs.io
- Easy Usage Guide: `easy_usage.md`
- HOW TO USE: `HOW_TO_USE.md`

## License

MIT License - See LICENSE file for details.

---

*Enhanced Python bindings for NLM neural learning machine*
*Version 0.2.0*
*For technical support, see NLM documentation or contact the development team.*