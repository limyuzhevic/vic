# NLM Python Bindings - Enhanced API

## Overview

The NLM Python bindings have been significantly enhanced to provide **convenience methods**, **higher-level abstractions**, **performance optimizations**, and **advanced features** while maintaining 100% backward compatibility.

### Key Improvements

| Category | Features | Benefits |
|----------|----------|----------|
| **Convenience** | Simplified constructors, helper methods | Easier setup and common operations |
| **Abstractions** | Episode recording, memory management | Higher-level programming interface |
| **Performance** | Batch operations, vectorized operations | Faster and more efficient computations |
| **Advanced** | Direct C++ access, flexible config | Power users can access low-level control |
| **Integration** | Context managers, Python ecosystem | Better resource management and interoperability |

## Quick Start - Original API (Still Works)

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

## Quick Start - Enhanced Pythonic API

```python
import pynlm

# Enhanced: Create brain with default config
brain = pynlm.Brain()  # Same as before, but more Pythonic!
brain.initialize()

# Enhanced: Convenience injection methods
brain.injectToExcitatory(0.5)  # More convenient than injectCurrentToNeurons
brain.injectToInhibitory(-0.2)
brain.injectToSensory(1.0)

# Enhanced: Context manager for automatic cleanup
with pynlm.Brain() as brain:
    brain.initialize()
    brain.step(0)
    # Brain automatically reset when exiting context!

# Enhanced: Get brain statistics
stats = brain.getBrainStats()
print(f"Healthy: {stats['healthy']}")
print(f"Active neurons: {stats['active_neuron_count']}")
```

## Enhanced Features

### 1. Pythonic Brain Class

#### Convenience Constructors
```python
# Multiple ways to create a brain
brain1 = pynlm.Brain()  # Default config
brain2 = pynlm.Brain(config)  # With explicit config
brain3 = pynlm.Brain.createTutorial()  # Pre-configured for learning
brain4 = pynlm.Brain.createBenchmark(1000, 3)  # For performance testing
```

#### Enhanced Methods
```python
# More intuitive method names
brain.injectToExcitatory(current)  # vs injectCurrentToNeurons
brain.injectToInhibitory(current)   # vs injectCurrentToNeurons
brain.injectToSensory(current)      # vs injectCurrentToNeurons
brain.injectToMotor(current)        # vs injectCurrentToNeurons

# Batch operations for performance
brain.batchInjectCurrent([(neuron_id, current), ...])
brain.batchInjectToNeurons([NeuronType.Excitatory, NeuronType.Internal], 0.5)

# Vectorized operations
brain.injectCurrentVectorized([0.1, 0.2, 0.3], scaling=2.0)
```

### 2. Pythonic Config Class

```python
import pynlm

# Config with Pythonic API
config = pynlm.Config()

# Pythonic getters
config.setInt("neuron_count", 1000)
config.setFloat("learning_rate", 0.01)
config.setBool("enable_curiosity", True)

# Pythonic operators
config["max_neurons"] = 2000  # Set via key
del config["old_key"]         # Delete via key
for key in config:            # Iterate over keys
    value = config[key]       # Get via key

# Batch operations
config.update({"key1": value1, "key2": value2})
```

### 3. Enhanced AgentBrain

```python
import pynlm

# Create integrated system (all-in-one helper)
brain, world, agent = pynlm.createIntegratedSystem()

# Pythonic state management
state = agent.getStateDict()
agent.setStateFromDict(state)

# Enhanced action selection cycle
percept = world.getSensoryPercept()
action = agent.completeLearningCycle(percept, reward=1.0)

# Episode management
agent.startEpisode()
agent.endEpisode()
agent.isEpisodeComplete(threshold=0.5)
```

### 4. Context Managers

```python
import pynlm

# Brain context manager - automatic cleanup
with pynlm.Brain() as brain:
    brain.initialize()
    brain.step(0)
    # Brain automatically reset when exiting!

# Experiment context manager
with pynlm.runExperiment(pynlm.createPhase6Experiment()):
    # Run experiment with automatic cleanup
    pass

# Nested context managers for parallel processing
with pynlm.Brain() as brain1, pynlm.Brain() as brain2:
    # Process in parallel with automatic cleanup
    pass
```

### 5. Episode Recording and Memory Management

```python
import pynlm

# Create brain
brain = pynlm.Brain()
brain.initialize()

# Record episode with rich data
episode_data = brain.recordEpisode(
    step=100,
    reward=1.5,
    action=pynlm.Action(pynlm.ActionType.MoveForward),
    sensory_input=[0.1, 0.2, 0.3, 0.4]
)

# Memory management
brain.clearMemorySystems()
brain.consolidateMemories(threshold=0.3)
brain.optimizeWorkingMemoryCapacity()

# Retrieve episode data
recorded_episode = brain.getEpisodeData(100)
```

### 6. Experiment Setup Helpers

```python
import pynlm

# Pre-configured experiment setups
brain = pynlm.Brain()

# Learning experiment
brain.setupForExperiment("learning")

# Memory experiment
brain.setupForExperiment("memory")

# Cognition experiment
brain.setupForExperiment("cognition")

# Social experiment
brain.setupForExperiment("social")

# Development experiment
brain.setupForExperiment("development")
```

### 7. Performance Monitoring

```python
import pynlm

# Create brain
brain = pynlm.Brain()
brain.initialize()

# Performance monitoring
performance_stats = brain.monitorPerformance(steps=100)
print(f"Neurons firing: {performance_stats['firing_neurons']}")
print(f"Average firing rate: {performance_stats['avg_firing_rate']}")

# Memory usage
memory_info = brain.checkMemoryUsage()
print(f"Memory usage: {memory_info['neurons']} neurons, {memory_info['synapses']} synapses")

# Validation
is_healthy = brain.isHealthy()
print(f"Brain healthy: {is_healthy}")
```

### 8. Direct C++ Access

```python
import pynlm

# Create brain
brain = pynlm.Brain()
brain.initialize()

# Access low-level C++ systems
spike_system = brain.getSpikeSystem()
stdp = brain.getSTDP()
hebbian = brain.getHebbian()
structural_plasticity = brain.getStructuralPlasticity()

# Neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()

# Development system
development_system = brain.getDevelopmentSystem()
```

### 9. Educational Helper Functions

```python
import pynlm

# Tutorial brain for learning
brain = pynlm.Brain.createTutorial()
print(f"Created tutorial brain with {brain.getTotalNeuronCount()} neurons")

# Benchmark brain for performance testing
brain = pynlm.Brain.createBenchmark(neuron_count=500, region_count=2)
print(f"Created benchmark brain with {brain.getTotalNeuronCount()} neurons")

# Research brain with specific properties
brain = pynlm.Brain.createResearch(
    neuron_count=2000,
    plasticity_enabled=True,
    memory_capacity=10000
)
```

## Complete Examples

### Example 1: Learning Experiment

```python
import pynlm

# Setup learning experiment
brain = pynlm.Brain()
brain.setupForExperiment("learning")
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run learning simulation
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    action = agent.act(percept, reward=1.0)
    brain.step(step)
    world.applyMotorCommand(action, world.getSimulationTime())

# Save results
brain.save("learning_experiment.nlm")
print("Learning experiment completed!")
```

### Example 2: Memory Consolidation Study

```python
import pynlm

# Setup memory experiment
brain = pynlm.Brain()
brain.setupForExperiment("memory")
brain.initialize()

# Enable memory systems
brain.setupForExperiment("memory")  # Uses replay intervals

# Run memory experiment
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        # Record and consolidate memories
        brain.consolidateMemories(threshold=0.3)
        print(f"Step {step}: {brain.getEpisodicMemoryStats()['episode_count']} episodes")

print("Memory consolidation experiment completed!")
```

### Example 3: Agent with Episode Recording

```python
import pynlm

# Create integrated system
brain, world, agent = pynlm.createIntegratedSystem()
agent.enableRewardModulation(True)

# Run episode with recording
episode_number = 0
for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    
    # Agent acts and learns
    reward = 1.0 if step % 50 == 0 else 0.1
    action = agent.act(percept, reward, predicted_reward=0.5)
    brain.step(step)
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Record episode periodically
    if step % 50 == 0:
        episode_data = brain.recordEpisode(step, reward, action)
        agent.endEpisode()
        episode_number += 1
        print(f"Episode {episode_number} completed: {episode_number} neurons firing")

print("Agent with episode recording completed!")
```

## Performance Tips

### Use Batch Operations
```python
# For large-scale operations, use batch methods
brain.batchInjectCurrent([(id1, current1), (id2, current2), ...])
brain.batchInjectToNeurons([NeuronType.Excitatory, NeuronType.Inhibitory], 0.5)

# Vectorized operations for numerical computations
brain.injectCurrentVectorized([0.1, 0.2, 0.3, 0.4], scaling=2.0)
```

### Enable Memory Optimization
```python
# Enable memory-efficient operations
brain.optimizeMemoryUsage()
brain.clearUnusedTraces()
brain.consolidateMemories(threshold=0.2)
```

### Use Context Managers
```python
# Always use context managers for resource management
with pynlm.Brain() as brain:
    brain.initialize()
    # Operations here
    # Automatic cleanup when done
```

## Backward Compatibility

**All existing code continues to work unchanged:**

```python
# Old API (still works)
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)
action = brain.produceAction()

# New Pythonic API (available)
import pynlm

brain = pynlm.Brain()
brain.initialize()
brain.step(0)
action = brain.produceAction()
```

## Migration Guide

| Old Method | New Method | Notes |
|------------|------------|-------|
| `injectCurrentToNeurons(NeuronType.Excitatory, 0.5)` | `injectToExcitatory(0.5)` | More convenient alias |
| `brain.getTotalNeuronCount()` | `brain.getTotalNeuronCount()` | Unchanged |
| `config.getOr<int>("key", default)` | `config.getIntOr("key", default)` | Pythonic naming |
| Multiple config access methods | `config[key]` | Simple dictionary-style access |
| Manual memory management | `brain.clearMemorySystems()` | Convenience method |

## Error Handling

The enhanced bindings include better error handling:

```python
import pynlm

# Specific error types
try:
    brain.initialize()
except pynlm.BrainInitializationError as e:
    print(f"Brain init failed: {e}")

try:
    config.set("invalid_key", "invalid_value")
except pynlm.InvalidConfigError as e:
    print(f"Invalid config: {e}")

# Validation
if brain.isHealthy():
    print("Brain is ready for simulation")
else:
    print("Brain needs initialization")
```

## Advanced Features

### Direct C++ System Access
```python
# Access low-level C++ systems for advanced users
spike_system = brain.getSpikeSystem()
spike_system.registerHandler(lambda event: print(f"Spike: {event}"))

stdp = brain.getSTDP()
stdp.configure(ltp_weight=0.02, ltd_weight=0.012, tau=20.0)
```

### Complex Configuration
```python
import pynlm

# Complex configuration with schema validation
config = pynlm.Config()
config.setInt("neuron_count", 1000)
config.setFloat("learning_rate", 0.01)
config.setBool("plasticity_enabled", True)

# Validate configuration
if config.validate():
    print("Configuration is valid")
```

### Custom Extensions
```python
# Extend NLM with custom components
from pynlm import ExtensionManager

manager = ExtensionManager()
manager.install("community_neural_extensions")

# Create custom neuron type
custom_neuron = pynlm.NeuronType.register("custom")
```

## Testing and Verification

Run the comprehensive test suite:

```bash
# Basic functionality tests
pytest test_enhanced_bindings.py::test_basic_functionality

# Pythonic API tests  
pytest test_enhanced_bindings.py::test_config_pythonic
pytest test_enhanced_bindings.py::test_context_manager

# Performance tests
pytest test_enhanced_bindings.py::test_batch_operations
pytest test_enhanced_bindings.py::test_vectorized_operations

# Integration tests
pytest test_enhanced_bindings.py::test_agent_brain
pytest test_enhanced_bindings.py::test_experiment_setup

# Complete system tests
pytest test_enhanced_bindings.py::test_complete_integrated_system
```

## Installation

The enhanced bindings are automatically installed with NLM:

```bash
pip install pynlm
```

For development and testing:

```bash
pip install -e .[dev]  # Install with development dependencies
```

## Documentation

For detailed documentation, see:

- `python/ENHANCED_BINDINGS_SUMMARY.md` - Complete feature documentation
- `easy_usage.md` - Original quick start guide  
- `HOW_TO_USE.md` - Extended usage instructions
- `docs/` - Architecture and research documentation

## Support

For questions, issues, or feedback:

- GitHub Issues: Report bugs and feature requests
- Documentation: Check the docs/ directory for technical details
- Examples: Run the examples in this guide

The enhanced NLM Python bindings provide a powerful, user-friendly interface for neural simulation research while maintaining the full flexibility of the underlying C++ implementation.