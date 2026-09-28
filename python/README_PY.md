# NLM Pythonic API Documentation

This document provides comprehensive documentation for the improved NLM Python API,
including both the new Pythonic interface and the enhanced C++ bindings.

## Overview

The NLM (Neural Learning Machine) Python API has been significantly improved with:

1. **Enhanced C++ bindings** with proper error handling and type safety
2. **New Pythonic API layer** with intuitive Python conventions
3. **Comprehensive test suite** for both high-level and low-level functionality
4. **Improved documentation** with examples and usage patterns

## What's New

### 1. Enhanced C++ Bindings (`python/bindings.cpp`)

#### Added:
- `__version__` attribute (value: "0.1.0")
- Fixed SynapseId enum bug (was using NeuronId::operator!= instead of SynapseId::operator!=)
- Improved error handling with proper exception registration

#### Fixed:
- SynapseId class now correctly implements `operator!=` (was a bug using NeuronId::operator!=)
- Memory system accessors now properly declared (no more return nullptr inline definitions)

### 2. New Pythonic API (`python/nlm.py`)

#### Key Features:

**Type Hints & Documentation:**
- Full type hints for all public classes and methods
- Comprehensive docstrings with examples
- Python standard naming conventions

**High-Level Interfaces:**
- `NLMBrain`: Pythonic brain wrapper with statistics and convenience methods
- `NLMWorld`: Pythonic world wrapper with sensory input handling
- `NLMEnvironment`: Complete agent-environment simulation framework

**Convenience Functions:**
- `create_simple_brain()`: Quick brain creation with default settings
- `run_simple_simulation()`: Complete simulation with brain, world, and agent

**Error Handling:**
- Custom exception hierarchy: `NLMError` → `ConfigurationError`, `SimulationError`, `MemoryError`
- Clear error messages for common failure scenarios

**Memory Systems Integration:**
- Full integration with Phase 6 memory systems:
  - Working memory (active traces)
  - Episodic memory (experience storage)
  - Associative memory (pattern associations)
  - Neuromodulation (dopamine, curiosity, novelty)
  - Prediction system
  - Cognition systems (planner, concept formation, attention)
  - Development system

### 3. Comprehensive Test Suite (`python/test_nlm.py`)

#### Test Coverage:
- **NLMBrain tests**: Creation, stepping, region management, checkpointing
- **NLMWorld tests**: Configuration, sensory input, agent body state
- **NLMEnvironment tests**: Complete episode simulation
- **Convenience function tests**: Quick setup helpers
- **Error handling tests**: Custom exception classes
- **C++ binding tests**: Direct pynlm module functionality
- **Phase 6 integration tests**: Memory systems, neuromodulation, prediction

### 4. Architecture Improvements

#### Brain.hpp Header Fix:
- Removed `return nullptr` inline method definitions
- Properly declared memory system accessors matching Brain.cpp implementation
- Fixed inconsistencies between header declarations and implementation

#### Phase 6 Integration:
All the following systems are now properly integrated and accessible through the Python API:

- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
- **Cognition**: Neural planner, concept formation, attentional selection
- **Prediction**: Prediction system with real-time updates
- **Development**: Developmental stages and plasticity modulation
- **Persistence**: Checkpoint system with save/load functionality
- **Replay**: Memory replay during rest/consolidation cycles

## Usage Examples

### Basic Usage (New Pythonic API)

```python
from nlm import NLMBrain, NLMWorld, NLMEnvironment

# Create a simple brain
brain = NLMBrain()
print(f"Created brain with {brain._brain.getTotalNeuronCount()} neurons")

# Configure and initialize a world
world = NLMWorld()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain._brain)
agent.initialize(world._world)
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

# Run simulation
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.get_sensory_input()
    
    # Process input in brain
    agent.processSensoryInput(percept)
    brain._brain.step(step, step * 0.001)
    
    # Get action and apply to world
    action = agent.decodeMotorCommand()
    world.apply_action(action, step * 0.1)

# Get statistics
stats = brain.get_stats()
print(f"Simulation complete. Firing rate: {stats['average_firing_rate']:.3f}")
```

### Environment-Based Usage

```python
from nlm import NLMEnvironment

# Create complete environment
env = NLMEnvironment()
env.setup(world_width=15, world_height=15,
          vision_width=6, vision_height=6,
          agent_start_x=7.5, agent_start_y=7.5)

# Run episode
episode_stats = env.run_episode(max_steps=500, timestep=0.1)

print(f"Episode completed: {episode_stats['steps_completed']} steps")
print(f"Total reward: {episode_stats['total_reward']:.2f}")
print(f"Average curiosity: {episode_stats['avg_curiosity']:.3f}")
```

### Convenience Functions

```python
from nlm import create_simple_brain, run_simple_simulation

# Quick brain creation
brain = create_simple_brain(neuron_count=500, region_count=2)

# Quick simulation
world = NLMWorld()
agent = pynlm.createAgentBrain(brain._brain)

results = run_simple_simulation(brain, world, agent, max_steps=200)
print(f"Simulation generated {len(results['steps'])} steps")
```

### Direct C++ Bindings

```python
import pynlm

# Check version
print(f"NLM Python bindings version: {pynlm.__version__}")

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")

# Test memory systems
print(f"Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
print(f"Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
```

## API Reference

### NLMBrain Class

| Method | Description |
|--------|-------------|
| `__init__(config=None)` | Create brain with optional config |
| `step(step_number, timestep)` | Advance simulation by one time step |
| `get_stats()` | Get comprehensive brain statistics |
| `add_region(name)` | Add a new brain region |
| `get_region(region_id)` | Get region by ID |
| `get_all_regions()` | Get all regions |
| `reset()` | Reset brain to initial state |
| `save(filepath)` | Save checkpoint to file |
| `load(filepath)` | Load checkpoint from file |

### NLMWorld Class

| Method | Description |
|--------|-------------|
| `__init__()` | Create world |
| `configure(width, height, vision_width, vision_height)` | Set world dimensions |
| `reset()` | Reset world to initial state |
| `set_agent_start(x, y)` | Set agent starting position |
| `update(timestep)` | Update world simulation |
| `get_sensory_input()` | Get current sensory data |
| `apply_action(action, timestep)` | Apply action to world |
| `get_agent_body()` | Get agent body state |

### NLMEnvironment Class

| Method | Description |
|--------|-------------|
| `__init__(config=None)` | Create environment with optional config |
| `setup(world_width, world_height, vision_width, vision_height, agent_start_x, agent_start_y)` | Setup complete environment |
| `run_episode(max_steps, timestep)` | Run complete simulation episode |

### Convenience Functions

| Function | Description |
|----------|-------------|
| `create_simple_brain(neuron_count, region_count, connection_probability)` | Quick brain creation |
| `run_simple_simulation(brain, world, agent, max_steps, timestep)` | Quick complete simulation |

### Custom Exceptions

- `NLMError`: Base exception class
- `ConfigurationError`: Configuration-related errors
- `SimulationError`: Simulation-related errors
- `MemoryError`: Memory system errors

## Performance Considerations

### Memory Usage:
- The Pythonic API adds minimal overhead (one wrapper object per brain/world/agent)
- Memory systems are directly accessed from the C++ layer
- Checkpointing uses efficient binary format

### Simulation Speed:
- Core simulation loop runs in C++ (unchanged)
- Python overhead only for high-level interface calls
- Use direct pynlm bindings for maximum performance

### Recommendations:
1. **For research**: Use the Pythonic API for easier experiment setup
2. **For production**: Use direct C++ bindings for maximum performance
3. **For prototyping**: Use convenience functions to quickly get started

## Migration Guide

### From Old to New API:

#### Old Usage (C++ bindings directly):
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)
```

#### New Usage (Pythonic API):
```python
from nlm import NLMBrain

brain = NLMBrain()
for i in range(100):
    brain.step(i)
```

### Migration Benefits:
1. **Type safety**: Python type hints catch errors early
2. **Better documentation**: Comprehensive docstrings and examples
3. **Convenience**: Methods like `get_stats()` provide quick insights
4. **Error handling**: Clear exception types and messages
5. **Memory integration**: Access to all Phase 6 systems

## Troubleshooting

### Common Issues:

1. **"pynlm module not available"**
   - Solution: Install with `pip install pynlm` or `cd python && pip install -e .`

2. **Import errors**
   - Solution: Ensure you're running from the correct directory and all dependencies are installed

3. **Memory system returns None**
   - Solution: This should not happen with the updated headers. If it does, there may be a build issue

4. **Performance issues**
   - Solution: For maximum performance, use direct pynlm bindings instead of the Pythonic API

### Getting Help:
- Documentation: This file and the examples in `docs/`
- Tests: Run `python -m unittest python.test_nlm` for comprehensive tests
- Examples: Look at the Phase 6 demo executables for C++ usage patterns

## Future Improvements

### Planned Enhancements:
1. **Type checking**: Add static type checking with mypy
2. **Async support**: Asynchronous simulation methods
3. **Visualization**: Integration with visualization libraries
4. **Distributed computing**: Multi-agent simulations
5. **Advanced analytics**: Performance monitoring and profiling

### Research Integration:
The improved API is designed to support:
- Neuroscience research with proper memory system tracking
- Machine learning experiments with configurable architectures
- Educational tools with intuitive interfaces
- Real-time applications with efficient callbacks

## Conclusion

The NLM Python API has been significantly enhanced to provide:

1. **Better integration** with Phase 6 systems (memory, prediction, cognition)
2. **More intuitive interface** following Python conventions
3. **Comprehensive error handling** and type safety
4. **Extensive testing** with full coverage
5. **Documentation and examples** for easy adoption

The API now provides both high-level convenience for rapid prototyping and low-level access for performance-critical applications, making NLM more accessible to a wider range of users while maintaining full access to the underlying C++ power.
