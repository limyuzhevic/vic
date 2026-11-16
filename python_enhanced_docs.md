# NLM Python Bindings - Enhanced Documentation

## Overview

This document describes the enhanced Python bindings for the NLM (Neural Learning Machine) framework. The bindings have been improved to provide more Pythonic APIs, better error handling, and convenience methods for common use cases.

## Key Improvements

### 1. Enhanced Constructor APIs

Many classes now have additional Python-friendly constructors:

#### Vision
```python
# RGB vision (default 3 channels)
vision = Vision(width, height)
vision = Vision.createRgb(width, height)

# Grayscale vision
vision = Vision.createGrayscale(width, height)

# Full RGB specification
vision = Vision(width, height, 3)
```

#### Audio
```python
# Create from sample rate and samples list
audio = Audio.createFromSamples(sample_rate, samples)

# Or use standard constructor
audio = Audio(sample_rate, len(samples))
```

#### InternalSignals
```python
# Add multiple signals at once
signals.addSignals([0.5, 0.8, 1.2])

# Or add individually
for value in [0.5, 0.8, 1.2]:
    signals.addSignal(value)
```

### 2. SensoryPercept Convenience Methods

```python
# Create empty percept
percept = SensoryPercept.createEmpty()

# Create from Vision data alone
percept = SensoryPercept.createFromVision(vision)

# Create from Vision and Touch data
percept = SensoryPercept.createFromVisionTouch(vision, touch_data)
```

### 3. Action Builder Pattern

```python
# Create actions with different patterns
action = Action.createMove(ActionType.MoveForward)
action = Action.createWithParams(ActionType.Custom, [0.5, 0.8, 1.2])

# Modify parameters conveniently
action.setParamsFromList([0.1, 0.2, 0.3, 0.4])
action.addParameter(0.5)
```

### 4. AgentBody Utility Methods

```python
# Set position conveniently
body.setPosition(10.0, 5.0)

# Set velocity with automatic movement flag
body.setVelocity(2.5, 1.8)

# Simplified movement (requires dt parameter)
body.moveTo(new_x, new_y, dt)
```

### 5. ActionResult Factory Methods

```python
# Create success/failure results easily
result = ActionResult.createSuccess(10.5, "Successfully collected resource")
result = ActionResult.createFailure(-5.0, "Hit hazard zone")

# Check reward types
if result.isPositiveReward():
    print("Got positive reward!")
elif result.isNegativeReward():
    print("Got negative reward!")
elif result.isNeutralReward():
    print("Neutral reward")
```

### 6. Enhanced Config API

```python
# Type-safe configuration access
config.setValue("learning_rate", 0.01)
config.setValue("population_size", "1000")
config.setValue("enable_plasticity", "true")

# Get with defaults (no KeyError if key doesn't exist)
learning_rate = config.getFloat("learning_rate", 0.001)
population_size = config.getInt("population_size", 100)
enabled = config.getBool("enable_plasticity", False)

# Safe string access
name = config.getString("agent_name", "DefaultAgent")
```

### 7. SimpleWorld Builder Methods

```python
# Create common world objects conveniently
world.addResource(2.0, 3.0)
world.addHazard(5.0, 7.0)
world.addWall(10.0, 10.0)
world.addWall(15.0, 5.0, 2.0, 3.0)  # Custom size

# Remove objects easily
world.removeObjectAt(2.0, 3.0)

# Check collisions
if world.checkCollision(agent_x, agent_y, agent_radius):
    print("Collision detected!")
```

## Example Usage

### Complete NLM Agent Example

```python
import nlm

# Create default configuration
config = nlm.createDefaultConfig()

# Configure key parameters
config.setValue("neuron_count", 500)
config.setValue("region_count", 1)
config.setValue("connection_probability", 0.1)
config.setValue("random_seed", 42)

# Create brain and agent
brain = nlm.createBrain(config)
agent_brain = nlm.createAgentBrain(brain)

# Create and configure world
world = nlm.createSimpleWorld()
world.configure(100, 100, 32, 32)
world.setAgentStart(10, 10)
agent_brain.initialize(world)

# Simulation loop
for step in range(1000):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent_brain.processSensoryInput(percept)
    
    # Generate action
    action = brain.produceAction()
    world.applyMotorCommand(action, step)
    
    # Get reward and apply neuromodulation
    # (In a real scenario, this would come from the world)
    reward = 0.0  # Example reward
    agent_brain.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent_brain.updateDevelopment(0.001)
    
    if step % 100 == 0:
        print(f"Step {step}: Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")

print("Simulation completed!")
print(f"Final brain stats: {brain.logStatus()}")
```

### Advanced Usage with Vision and Audio

```python
import nlm

# Create brain and agent
brain = nlm.createBrain(nlm.createDefaultConfig())
agent_brain = nlm.createAgentBrain(brain)

# Create world and add visual features
world = nlm.createSimpleWorld()
world.configure(64, 64, 16, 16)
world.setAgentStart(5, 5)

# Add visual features
world.addResource(10, 10)      # Bright resource
world.addHazard(20, 20)        # Dark hazard
world.addWall(30, 30, 2, 2)    # Wall

# Create rich sensory input
vision_data = [0.0] * 256  # 16x16 vision
# Set some vision values
vision_data[10*16 + 10] = 0.9  # Bright pixel at (10, 10)
vision_data[20*16 + 20] = 0.2  # Dark pixel at (20, 20)

vision = nlm.Vision.createRgb(16, 16)
vision.setData(vision_data)

# Create percept
touch_data = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
percept = nlm.SensoryPercept.createFromVisionTouch(vision, touch_data)

# Process and get action
agent_brain.processSensoryInput(percept)
action = brain.produceAction()

print(f"Action chosen: {action.getType()}")
print(f"Action parameters: {action.getParameters()}")
```

### Checkpointing and Persistence

```python
import nlm

# Create and initialize brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Perform some steps
for step in range(1000):
    # ... simulation steps ...
    pass

# Save checkpoint
brain.save("my_brain_checkpoint.bin")
print(f"Saved checkpoint: {brain.save('my_brain_checkpoint.bin')}")

# Later, reload
brain.load("my_brain_checkpoint.bin")
print(f"Loaded checkpoint: {brain.load('my_brain_checkpoint.bin')}")
```

## Migration Guide

### From Old API to New API

#### Vision Constructor
```python
# Old
vision = Vision()

# New (recommended)
vision = Vision.createRgb(width, height)
vision = Vision.createGrayscale(width, height)
```

#### Action Creation
```python
# Old - verbose
action = Action()
action.setType(ActionType.MoveForward)

# New - fluent
action = Action.createMove(ActionType.MoveForward)
```

#### Config Value Access
```python
# Old - error-prone
try:
    value = config.getValue("key")
    if value:
        float_val = value  # Could fail at runtime
except:
    pass

# New - type-safe with defaults
float_val = config.getFloat("key", 0.0)
```

## Best Practices

1. **Use Convenience Methods**: Always prefer `createRgb()`, `createGrayscale()`, etc. over raw constructors
2. **Type-Safe Config Access**: Use `getString()`, `getInt()`, `getFloat()`, `getBool()` for better error handling
3. **Factory Patterns**: Use the builder patterns for complex object creation
4. **Error Handling**: Let the Pythonic exceptions handle validation errors gracefully
5. **Memory Management**: The bindings use smart pointers internally, so Python code doesn't need explicit cleanup

## Performance Considerations

1. **Batch Operations**: Use `addSignals()` for multiple signal additions instead of individual calls
2. **Data Reuse**: Reuse Vision/Audio objects rather than creating new ones frequently
3. **Memory Views**: For very large datasets, consider using direct array operations
4. **Pre-allocation**: Pre-allocate vectors when creating large sensory inputs

## Troubleshooting

### Common Issues

1. **Configuration Key Errors**
```python
# Problem: KeyError when accessing non-existent config keys
# Solution: Use get* methods with defaults
value = config.getFloat("non_existent_key", 0.0)
```

2. **Vision Channel Mismatch**
```python
# Problem: Vision constructor with wrong channel count
# Solution: Use convenience methods or specify channels explicitly
vision = Vision.createRgb(width, height)  # Always 3 channels
vision = Vision(width, height, 1)        # Grayscale
```

3. **Action Parameter Errors**
```python
# Problem: Wrong parameter types in Action constructors
# Solution: Ensure parameter types match ActionType expectations
```

## Future Enhancements

The following improvements are planned for future releases:

1. **Async APIs**: Non-blocking versions of key operations
2. **Tensor Integration**: Direct NumPy array support for Vision/Audio
3. **Advanced Builder Patterns**: More sophisticated configuration builders
4. **Pipeline APIs**: Stream processing for continuous sensory input
5. **Distributed Computing**: Multi-agent coordination APIs

## License

This enhanced documentation is provided under the MIT License, consistent with the NLM framework.
