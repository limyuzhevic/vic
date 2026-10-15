# NLM Python Context Managers Documentation

## Overview

This document describes the implementation of Python context managers for the NLM (Neural Learning Machine) Python bindings. Context managers provide a reliable way to manage resources by ensuring that cleanup code is executed automatically when exiting a `with` block, even if exceptions occur.

## Why Context Managers?

The C++ classes in NLM (Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action) manage internal resources that need proper cleanup. Without context managers:

1. **Resource Leaks**: Objects might not be properly destroyed if exceptions occur
2. **Manual Cleanup**: Developers must remember to explicitly destroy objects
3. **Error-Prone**: Cleanup logic can be missed in complex error handling scenarios

Context managers solve these problems by:
- **Automatic Cleanup**: Resources are cleaned up when exiting the `with` block
- **Exception Safety**: Cleanup happens even if exceptions occur
- **Readable Code**: Intent is clear from the `with` statement syntax

## Context Manager Implementation

### Base Pattern

Each context manager implements the Python context manager protocol with two methods:

1. **`__enter__()`**: Called when entering the `with` block, returns the resource
2. **`__exit__(exc_type, exc_val, exc_tb)`**: Called when exiting the `with` block, performs cleanup

### Core Context Manager Classes

The implementation uses wrapper classes that delegate to the actual C++ objects while ensuring proper cleanup:

1. **ConfigContextManager**: Manages `Config` objects
2. **BrainContextManager**: Manages `Brain` objects  
3. **AgentBrainContextManager**: Manages `AgentBrain` objects
4. **SimpleWorldContextManager**: Manages `SimpleWorld` objects
5. **SensoryInputContextManager**: Manages `SensoryInput` objects
6. **ActionContextManager**: Manages `Action` objects

### Cleanup Strategy

The context managers use `std::shared_ptr` and `std::unique_ptr` smart pointers for automatic memory management:

- **`std::shared_ptr`**: For objects with reference counting (Brain, Config, AgentBrain, SimpleWorld)
- **`std::unique_ptr`**: For exclusive ownership (SensoryInput, Action)

The `__exit__()` method calls `reset()` on these smart pointers, which triggers the automatic cleanup of the underlying C++ objects and their resources.

## Usage Examples

### Basic Context Manager Usage

```python
import pynlm as nlm

# Example 1: Using Config context manager
with nlm.Config() as config:
    config.set("learning_rate", 0.01)
    config.set("neuron_count", 1000)
    print(f"Learning rate: {config.getOr('learning_rate', 0.0)}")

# Config is automatically cleaned up here
```

### Nested Context Managers

```python
import pynlm as nlm

# Example 2: Nested context managers
with nlm.Config() as config, \
     nlm.SimpleWorld() as world, \
     nlm.Brain(config) as brain:
    
    world.configure(100.0, 100.0, 10, 10)
    brain.initialize()
    
    # Perform simulation
    for step in range(100):
        world.update(0.1)
    
# All resources cleaned up here
```

### Error Handling with Context Managers

```python
import pynlm as nlm

# Example 3: Automatic cleanup even if errors occur
try:
    with nlm.Config() as config:
        with nlm.SimpleWorld() as world:
            with nlm.Brain(config) as brain:
                brain.initialize()
                # Simulate an error
                raise RuntimeError("Simulation failed")
                
except RuntimeError as e:
    print(f"Error handled: {e}")
    # All context managers have cleaned up their resources
```

### Advanced Usage with Nested Blocks

```python
import pynlm as nlm

# Example 4: Complex simulation with multiple agents
with nlm.Config() as config:
    config.set("agent.exploration", 0.3)
    
    with nlm.SimpleWorld() as world:
        world.configure(200.0, 200.0, 20, 20)
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                agent.enableCuriosity(True)
                
                # Run agent in world
                for step in range(50):
                    world.update(0.05)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action.getType(), world.getSimulationTime())
```

## Available Context Managers

### Config Context Manager

**Purpose**: Manages configuration objects that control NLM simulation parameters.

**Methods**:
- `loadFromFile(filepath)`: Load configuration from JSON file
- `loadFromArgs(argc, argv)`: Load from command line arguments
- `saveToFile(filepath)`: Save to JSON file
- `has(key)`: Check if key exists
- `getKeys()`: Get all configuration keys
- `clear()`: Clear all configuration entries
- `summary()`: Get configuration summary

**Example**:
```python
with nlm.Config() as config:
    config.set("simulation.max_steps", 1000)
    config.set("brain.neuron_count", 500)
    steps = config.getOr("simulation.max_steps", 0)
```

### Brain Context Manager

**Purpose**: Manages neural simulation brains that coordinate neuron dynamics and learning.

**Methods**:
- `initialize()`: Initialize the brain with configuration
- `step(currentStep)`: Perform a simulation step
- `step(currentStep, currentTime)`: Perform step with timestamp
- `receiveSensoryInput(input)`: Inject sensory input
- `produceAction()`: Generate motor action based on neural activity
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state to file
- `load(filepath)`: Load brain state from file

**Example**:
```python
with nlm.Brain(config) as brain:
    brain.initialize()
    for step in range(100):
        brain.step(step)
    action = brain.produceAction()
```

### AgentBrain Context Manager

**Purpose**: Manages agent brain interfaces that connect NLM brains to world environments.

**Methods**:
- `initialize(world)`: Initialize with world
- `decodeMotorCommand()`: Decode brain activity to motor command
- `processSensoryInput(percept)`: Process sensory percept
- `applyRewardModulation(reward, predictedReward)`: Apply reward-based learning

**Example**:
```python
with nlm.AgentBrain(brain) as agent:
    agent.initialize(world)
    agent.enableCuriosity(True)
    action = agent.decodeMotorCommand()
```

### SimpleWorld Context Manager

**Purpose**: Manages world simulation environments for agent interaction.

**Methods**:
- `configure(width, height, visionWidth, visionHeight)`: Configure world geometry
- `update(timestep)`: Update world state
- `applyMotorCommand(cmd, currentTime)`: Apply action to agent
- `getSensoryPercept()`: Get agent's sensory perception
- `getAgentBody()`: Get agent state

**Example**:
```python
with nlm.SimpleWorld() as world:
    world.configure(100.0, 100.0, 10, 10)
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
```

### SensoryInput Context Manager

**Purpose**: Manages sensory input data (Vision, Audio, InternalSignals) from the environment.

**Methods**:
- `getType()`: Get input type (Vision, Audio, InternalSignals)
- `getData()`: Get raw data as vector
- `getDimensions()`: Get dimensionality
- `setTimestamp(timestamp)`: Set timestamp

**Example**:
```python
with nlm.Vision(10, 10) as vision:
    vision.setData([0.1] * 300)  # RGB image data
    print(f"Vision type: {vision.getType()}")
```

### Action Context Manager

**Purpose**: Manages motor actions generated by the brain and executed in the world.

**Methods**:
- `getType()`: Get action type (MoveForward, TurnLeft, etc.)
- `setType(type)`: Set action type
- `getParameters()`: Get action parameters
- `setParameters(params)`: Set action parameters
- `getName()`: Get human-readable action name
- `clone()`: Create copy of action

**Example**:
```python
with nlm.Action(nlm.ActionType.MoveForward) as action:
    action.setParameters([1.0, 0.5])  # speed, direction
    print(f"Action: {action.getName()}")
```

## Testing Context Managers

### Test Files

1. **`python/tests/test_context_managers.py`**: Comprehensive test suite for all context managers
2. **`python/examples/context_manager_examples.py`**: Real-world usage examples

### Running Tests

```bash
# Navigate to the workspace root
cd /workspace/app

# Install Python dependencies (if needed)
pip install -r requirements.txt

# Run context manager tests
python -m pytest python/tests/test_context_managers.py -v

# Run example demonstrations
python python/examples/context_manager_examples.py
```

### Test Coverage

The test suite includes:
- Basic context manager functionality
- Error handling and exception safety
- Nested context managers
- Resource cleanup verification
- Real-world simulation scenarios

## Best Practices

### 1. Use Context Managers for Resource Management

```python
# Recommended
with nlm.Brain(config) as brain:
    brain.initialize()
    # ... use brain

# Avoid (may cause resource leaks)
brain = nlm.Brain(config)
brain.initialize()
# ... use brain
# Forgetting to explicitly delete brain
```

### 2. Use Multiple Context Managers Together

```python
# For multiple related resources, use multiple with statements
with nlm.Config() as config, \
     nlm.SimpleWorld() as world, \
     nlm.Brain(config) as brain:
    # All three resources are managed together
```

### 3. Handle Exceptions Gracefully

```python
try:
    with nlm.Config() as config:
        # Configuration and usage
        pass
except Exception as e:
    # Handle error, resources are already cleaned up
    print(f"Error handled: {e}")
```

### 4. Combine with Other Python Features

```python
# Context managers work well with other Python features
import pynlm as nlm

# Generator with context manager
def simulate_steps(world, brain, steps):
    for step in range(steps):
        with nlm.Config() as config:
            config.set("step", step)
            yield step, brain.step(step)

# Exception handling
try:
    with nlm.SimpleWorld() as world:
        with nlm.Brain(config) as brain:
            # Complex simulation
            pass
except KeyboardInterrupt:
    print("Simulation interrupted, cleaning up...")
```

## Migration Guide

If you're migrating from existing code that doesn't use context managers:

### Before (Manual Resource Management)

```python
# Old way - potential resource leaks
config = nlm.Config()
config.set("key", "value")

brain = nlm.Brain(config)
brain.initialize()

world = nlm.SimpleWorld()
world.configure(100.0, 100.0, 10, 10)

# ... use objects ...

# Must remember to clean up
brain = None
world = None
config = None
```

### After (Context Manager)

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

## Troubleshooting

### Common Issues

1. **Resource Leaks**: If tests show memory leaks, ensure all context managers are being used correctly.

2. **Dangling References**: Context managers don't extend object lifetime beyond the `with` block.

3. **Exception Safety**: Context managers handle exceptions correctly, but verify your exception handling logic.

### Debugging Tips

```python
# Add debugging to verify cleanup
import pynlm as nlm

class DebugConfig:
    def __enter__(self):
        print("Config entered")
        return self
    
    def __exit__(self, *args):
        print("Config exiting and cleaning up")
        self.cleanup()
```

## Performance Considerations

1. **Overhead**: Context managers have minimal performance overhead
2. **Memory Management**: Smart pointers ensure proper cleanup without manual memory management
3. **Thread Safety**: Each context manager is independent and thread-safe for its scope

## Future Enhancements

1. **Smart Pointer Refactoring**: Further improve memory management
2. **Resource Counting**: Add resource counting for more complex scenarios
3. **Custom Exception Handling**: Extend exception handling with custom cleanup logic
4. **Async Context Managers**: Support for async/await patterns

## References

1. Python Documentation: [Context Managers](https://docs.python.org/3/reference/datamodel.html#with-statement-context-managers)
2. pybind11 Documentation: [Context Managers](https://pybind11.readthedocs.io/en/stable/advanced/pycpp_classes.html#context-managers)
3. C++ Smart Pointers: [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr), [std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr)

## License

MIT License

Copyright (c) 2024 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
