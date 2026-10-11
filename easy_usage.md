# NLM - Enhanced Python API Quick Start

## Overview

This is the quick start guide for the enhanced NLM Python bindings. This version includes significant improvements over the basic API with enhanced validation, convenience methods, and Pythonic interfaces.

## Key Enhancements

### 1. **Enhanced Validation and Error Handling**
- Comprehensive parameter validation for all classes
- Helpful error messages with specific guidance
- Type-safe operations with proper error catching

### 2. **Convenience Factory Methods**
- Action factory methods: `create_move_forward()`, `create_turn_left()`, etc.
- Result factory methods: `success()`, `failure()`, `neutral()`
- Default configuration creation

### 3. **Pythonic Interfaces**
- Dictionary-like configuration access (`config["key"] = value`)
- Property access for enhanced classes
- Mathematical helper utilities (`clamp`, `lerp`, `distance`)

### 4. **Comprehensive Statistics**
- Brain statistics aggregation
- World statistics and analysis
- Agent performance metrics

### 5. **Batch Processing**
- Run multiple simulation steps efficiently
- Process multiple sensory inputs
- Compare multiple agent configurations

## Installation

```bash
# Install from source
pip install .

# Or install in development mode
pip install -e .

# Build with all enhancements
mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4
```

## Quick Examples

### Example 1: Basic Enhanced Brain
```python
import pynlm

# Create brain with enhanced API
brain = pynlm.createBrain()
brain.initialize()

# Run simulation with status reporting
for i in range(100):
    brain.step(i)
    if i % 20 == 0:
        firing = brain.getFiringNeuronCount()
        print(f"Step {i}: {firing} firing neurons")
```

### Example 2: Advanced Configuration with Validation
```python
import pynlm

# Create configuration with validation
config = pynlm.Config()
config.set("neuron_count", 2000)
config["region_count"] = 5
config["learning_rate"] = 0.01

# Validate configuration
try:
    config.validate()
    print("Configuration is valid!")
except ValueError as e:
    print(f"Configuration error: {e}")

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### Example 3: Enhanced Action Management
```python
import pynlm

# Create actions using factory methods
actions = [
    pynlm.Action.create_move_forward(2.0),
    pynlm.Action.create_turn_left(0.5),
    pynlm.Action.create_interact(),
    pynlm.Action.create_wait()
]

# Process actions
for action in actions:
    result = pynlm.ActionResult(
        reward=1.0 if action.getType() != pynlm.ActionType.Wait else 0.5,
        success=True,
        message=f"Executed {action.getName()}"
    )
    print(f"Action: {action.getName()}, Reward: {result.reward}")
```

### Example 4: Complete Agent with World
```python
import pynlm

# Setup world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(10.0, 10.0)

# Add environment objects
world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource))
world.addObject(pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard))

# Create and configure agent
brain = pynlm.createBrain()
agent = pynlm.createAgentBrain(brain)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run episode
agent.initialize(world)
episode = world.runEpisode(agent, num_steps=100)

print(f"Episode completed with {episode['total_reward']:.2f} total reward")
```

## Advanced Features

### Batch Processing
```python
# Run multiple brains in parallel
brains = []
for i in range(5):
    config = pynlm.createDefaultConfig()
    config["neuron_count"] = 500 + i * 200
    brain = pynlm.createBrain(config)
    brain.initialize()
    brains.append(brain)

# Run steps for all brains
for brain in brains:
    results = brain.runSteps(50, timestep=0.1)
    print(f"Completed {len(results)} steps")
```

### Statistics and Analysis
```python
# Get comprehensive statistics
brain_stats = brain.getBrainStatistics()
world_stats = world.getWorldStatistics()
agent_stats = agent.getStatistics()

print(f"Brain: {brain_stats['total_neurons']} neurons, {brain_stats['avg_firing_rate']:.2f} avg firing rate")
print(f"World: {world_stats['width']}x{world_stats['height']}, {world_stats['total_objects']} objects")
```

### Validation and Error Handling
```python
# Use validation utilities
from pynlm import validate_float_range, validate_action_type, validate_neuron_id

# Validate parameters
try:
    validate_float_range(0.5, 0.0, 1.0, "learning_rate")
    print("Learning rate is valid")
except ValueError as e:
    print(f"Invalid learning rate: {e}")

# Validate configuration
config = pynlm.Config()
if not config.has("required_key"):
    raise ValueError("Missing required key: required_key")
```

## Helper Utilities

The enhanced bindings include mathematical and utility functions:

```python
import pynlm

# Mathematical utilities
value = pynlm.clamp(0.8, 0.0, 1.0)  # Returns 0.8
interpolated = pynlm.lerp(0.0, 1.0, 0.75)  # Returns 0.75
distance = pynlm.distance([0, 0], [3, 4])  # Returns 5.0
time_str = pynlm.format_time(125.5)  # Returns "2:05.50"

# Validation helpers
pynlm.validate_action_type(pynlm.ActionType.MoveForward)
pynlm.validate_neuron_id(neuron_id)
```

## Configuration Parameters

### Brain Configuration
```python
config = pynlm.Config()
config["neuron_count"] = 1000
config["region_count"] = 2
config["connection_probability"] = 0.1
```

### Plasticity Configuration
```python
config["plasticity.stdp.enable"] = True
config["plasticity.stdp.learning_rate"] = 0.001
config["neuromod.curiosity.enable"] = True
config["neuromod.novelty.enable"] = True
```

## Complete Example Script

For a comprehensive demonstration, see `examples/enhanced_api_example.py` which includes:

- Basic brain simulation
- Advanced configuration
- Action management
- World operations
- Batch processing
- Validation and error handling
- Helper utilities
- Complete agent training and evaluation

```bash
python examples/enhanced_api_example.py
```

## Migration Guide

### From Basic to Enhanced API

**Basic API (original):**
```python
import pynlm

brain = pynlm.Brain()
brain.initialize()
brain.step(0)
```

**Enhanced API:**
```python
import pynlm

# Enhanced API with factory methods and validation
brain = pynlm.createBrain()
brain.initialize()
brain.step(0)

# Additional features available
config = pynlm.createDefaultConfig()
```

**Key Changes:**
- `pynlm.Brain()` → `pynlm.createBrain()`
- `pynlm.Config()` → `pynlm.Config(filepath)`
- `pynlm.Action(ActionType.MoveForward)` → `pynlm.Action.create_move_forward()`
- `ActionResult(reward, success, message)` → `ActionResult.success()`, `ActionResult.failure()`

All changes are backward compatible - existing code will continue to work.

## Error Handling

### Common Errors and Solutions

**Error**: `ValueError: Configuration key cannot be empty`
**Solution**: Check all configuration key assignments

**Error**: `ValueError: Learning rate must be between 0.0 and 1.0`
**Solution**: Use `validate_float_range()` for validation

**Error**: `FileNotFoundError: Configuration file not found`
**Solution**: Ensure file exists or use try/except

### Error Handling Pattern
```python
import pynlm

try:
    config = pynlm.Config("config.json")
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(0)
except (ValueError, FileNotFoundError, RuntimeError) as e:
    print(f"Error: {e}")
    # Handle error appropriately
```

## Performance Tips

### Memory Management
- Use `brain.reset()` to free memory between simulations
- Monitor memory usage with system tools
- Consider smaller configurations for real-time applications

### Processing Speed
- Use `brain.runSteps()` for batch processing
- Enable only necessary subsystems
- Monitor with `brain.logStatus()`

### Configuration Optimization
```python
# For speed
fast_config = pynlm.Config()
fast_config["neuron_count"] = 500

# For accuracy  
accurate_config = pynlm.Config()
accurate_config["neuron_count"] = 5000
accurate_config["region_count"] = 10
```

## Getting Help

### Documentation
- **API Reference**: See `docs/API_REFERENCE.md`
- **Architecture**: See `docs/ARCHITECTURE.md`
- **Usage Guide**: See `HOW_TO_USE.md`
- **Beginner Guide**: See `easy_usage.md`

### Examples
- **Basic Examples**: Look in the `examples/` directory
- **Advanced Examples**: See `examples/enhanced_api_example.py`

### Support
- **GitHub Issues**: Report bugs and feature requests
- **Documentation**: Check for common problems

## License

MIT License. See LICENSE file for details.

---

*This quick start guide demonstrates the enhanced NLM Python API with validation, convenience methods, and Pythonic interfaces. For the latest documentation and updates, visit the NLM project repository.*
