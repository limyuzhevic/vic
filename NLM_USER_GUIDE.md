#!/usr/bin/env python3
"""
NLM Complete User Guide
========================

This comprehensive guide provides complete documentation for using the NLM (Neural Learning Machine) framework,
including both basic and advanced features, examples, best practices, and troubleshooting.

Copyright: NLM Development Team
License: MIT
Version: 1.0.0 (Complete Guide)
Date: 2026-09-27
"""

import warnings
warnings.filterwarnings('ignore')

print("NLM Complete User Guide")
print("=" * 80)

# Basic usage guide
print("\n1. BASIC USAGE")
print("-" * 40)

print("""
## Getting Started with NLM

NLM (Neural Learning Machine) is a brain-inspired spiking neural network simulator.
It provides a complete framework for creating and simulating artificial brains.

### Installation

Install the NLM Python bindings:

```bash
pip install pynlm

# Or install from source
pip install -e .
```

### Basic Imports

```python
import pynlm as nlm
from pynlm_enhanced import NLMSimulationContext
from nlm_cli_advanced import NLMCommandLine
```

### Creating a Simple Brain

```python
import pynlm as nlm

# Create configuration
config = nlm.createDefaultConfig()

# Create and initialize brain
brain = nlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Simulation

```python
import pynlm as nlm
from nlm_cli_advanced import NLMCommandLine

# Setup simulation
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation with progress monitoring
def progress_callback(event):
    if event.progress % 10 == 0:
        print(f"Progress: {event.progress:.1f}% (Step {event.step})")

for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
print("Simulation completed!")
```

### Using Enhanced Bindings

```python
from pynlm_enhanced import NLMSimulationContext, NLMBatchContext
from pynlm_enhanced import get_statistics_collector, NLMProfiler

# Create enhanced simulation context
with NLMSimulationContext(brain, world, agent) as ctx:
    stats = ctx.run(1000, timestep=0.1)
    
print(f"Simulation statistics: {stats}")

# Create batch context for multiple experiments
with NLMBatchContext(brain, world, agent, num_episodes=10, steps_per_episode=500) as batch:
    results = batch.run_batch()
    
print(f"Batch results: {len(results)} episodes completed")
```

# Advanced features guide
print("\n2. ADVANCED FEATURES")
print("-" * 40)

print("""
## Neuromodulation System

The NLM framework includes sophisticated neuromodulation systems that regulate learning and behavior.

### Dopamine System

```python
from pynlm_enhanced import NeuromodulatorManager

# Access neuromodulator manager
neuromod_manager = NeuromodulatorManager(brain)

# Control dopamine levels for reward-based learning
dopamine_level = neuromod_manager.get_dopamine()
neuromod_manager.set_dopamine(1.0)  # Strong reward signal
neuromod_manager.set_dopamine(-0.5, scale=0.5)  # Weak punishment signal
```

### Curiosity System

```python
# Enable curiosity-driven exploration
agent.enableCuriosity(True)

# Monitor curiosity level
curiosity_level = agent.getCuriosityLevel()
print(f"Curiosity level: {curiosity_level:.3f}")

# Set curiosity threshold for exploration
neuromod_manager.set_curiosity_threshold(0.3)
```

### Novelty Detection

```python
# Enable novelty detection
agent.enableNovelty(True)

# Monitor novelty levels
novelty_level = neuromod_manager.get_novelty_level()
print(f"Novelty level: {novelty_level:.3f}")
```

## Memory Systems

The NLM framework implements multiple memory systems for different types of information storage.

### Working Memory

```python
from pynlm_enhanced import MemoryManager

# Access memory manager
memory_manager = MemoryManager(brain)

# Store and retrieve working memory entries
memory_manager.store_working_memory("current_goal", "explore_area", importance=0.9)
memory_manager.store_working_memory("threat_level", 0.3, importance=0.8)

# Retrieve memory entries
target = memory_manager.retrieve_working_memory("current_goal")
threat = memory_manager.retrieve_working_memory("threat_level")

print(f"Goal: {target}")
print(f"Threat level: {threat}")
```

### Episodic Memory

```python
# Store episodic memory (experience)
experience = {
    'timestamp': 1234567890,
    'location': (10.0, 10.0),
    'action': 'move_forward',
    'reward': 1.0,
    'outcome': 'success'
}

memory_manager.store_episodic_memory(experience)

# Recall similar experiences
query = {
    'location': (10.0, 10.0),
    'action': 'move_forward',
    'reward': 1.0
}

memories = memory_manager.recall_episodic_memory(query)
print(f"Found {len(memories)} similar experiences")
```

### Associative Memory

```python
# Strengthen associations between patterns
memory_manager.strengthen_association(
    pattern1="red_object",
    pattern2="reward",
    strength=0.8
)

memory_manager.strengthen_association(
    pattern1="blue_object",
    pattern2="punishment",
    strength=0.9
)

# Complete patterns based on associations
completed = memory_manager.complete_pattern("red_")
print(f"Completed pattern: {completed}")
```

## Cognitive Systems

The NLM framework includes advanced cognitive systems for planning, concept formation, and attention.

### Neural Planner

```python
from pynlm_enhanced import CognitiveSystemManager

# Access cognitive system manager
cognitive_manager = CognitiveSystemManager(brain)

# Set goals and plan actions
goals = ["reach_target", "avoid_obstacle", "maximize_reward"]
constraints = ["stay_within_bounds", "avoid_pitfalls"]

# Generate action sequence
action_sequence = cognitive_manager.plan_action(goals, constraints)

# Evaluate action sequence quality
score = cognitive_manager.evaluate_action_sequence(action_sequence)
print(f"Action sequence score: {score:.2f}")
```

### Concept Formation

```python
# Learn concepts from patterns
cognitive_manager.get_concept_formation().learn_concept(
    pattern=[1.0, 0.0, 1.0, 0.0],
    abstraction_level=2
)

# Categorize new patterns
categories = cognitive_manager.get_concept_formation().categorize(
    pattern=[0.8, 0.2, 0.9, 0.1]
)

print(f"Categories: {categories}")
```

### Attentional Selection

```python
# Manage attentional allocation
attention_manager = cognitive_manager.get_attention_system()

# Allocate attention based on stimulus salience
stimuli = ["red_object", "blue_object", "moving_target", "static_wall"]
allocation = attention_manager.allocate_attention(stimuli)

print("Attention allocation:")
for stimulus, weight in allocation.items():
    print(f"  {stimulus}: {weight:.3f}")

# Set focus on specific region
attention_manager.set_attention_focus("red_object", 0.9)
```

## Development System

The NLM framework includes a developmental system that affects plasticity rates and neural maturation.

### Developmental Stages

```python
from pynlm_enhanced import DevelopmentSystemManager

# Access development system manager
development_manager = DevelopmentSystemManager(brain)

# Check current developmental stage
stage = development_manager.get_developmental_stage()
print(f"Current developmental stage: {stage}")

# Set developmental stage
development_manager.set_developmental_stage("Adult")
```

### Critical Period

```python
# Enable critical period for enhanced learning
development_manager.enable_critical_period(True, duration=100.0)

# During critical period, plasticity is much higher
plasticity_modifier = development_manager.get_plasticity_modifier()
print(f"Plasticity modifier: {plasticity_modifier:.3f}")  # Should be > 1.0 during critical period
```

## Plasticity Controls

The NLM framework provides fine control over learning mechanisms.

### STDP (Spike-Timing-Dependent Plasticity)

```python
from pynlm_enhanced import PlasticityManager

# Access plasticity manager
plasticity_manager = PlasticityManager(brain)

# Enable STDP with custom parameters
plasticity_manager.enable_stdp(True, learning_rate=0.001)
plasticity_manager.set_stdp_parameters(tau_plus=20.0, tau_minus=20.0)
```

### Hebbian Learning

```python
# Enable Hebbian learning
plasticity_manager.enable_hebbian(True, learning_rate=0.01)
```

### Structural Plasticity

```python
# Enable structural plasticity (creation/destruction of connections)
plasticity_manager.enable_structural_plasticity(True)

# Apply custom plasticity rules
plasticity_manager.apply_custom_plasticity(
    rule="homeostatic",
    parameters={"target_rate": 0.1, "learning_rate": 0.001}
)
```

# Context managers guide
print("\n3. CONTEXT MANAGERS")
print("-" * 40)

print("""
## NLMSimulationContext

The NLMSimulationContext provides automatic resource management and enhanced features for simulation.

### Basic Usage

```python
from pynlm_enhanced import NLMSimulationContext

with NLMSimulationContext(brain, world, agent) as ctx:
    # Automatically initialize
    stats = ctx.run(1000, timestep=0.1)
    print(f"Simulation completed: {stats.total_spikes} spikes")
```

### With Progress Monitoring

```python
def progress_callback(event):
    if event.step % 100 == 0:
        print(f"Step {event.step}: {event.progress:.1f}% progress")

with NLMSimulationContext(brain, world, agent) as ctx:
    stats = ctx.run(1000, timestep=0.1, progress_callback=progress_callback)
```

### With Statistics Collection

```python
from pynlm_enhanced import get_statistics_collector

with NLMSimulationContext(brain, world, agent) as ctx:
    stats = ctx.run(1000, timestep=0.1)
    
    # Add to global statistics collector
    collector = get_statistics_collector()
    collector.add_session(stats)
    
    # Get analysis
    summary = collector.get_session_summary(len(collector.session_data) - 1)
```

## NLMBatchContext

The NLMBatchContext provides utilities for running multiple simulation episodes.

### Basic Usage

```python
from pynlm_enhanced import NLMBatchContext

with NLMBatchContext(brain, world, agent, num_episodes=10, steps_per_episode=500) as batch:
    results = batch.run_batch()
    
print(f"Completed {len(results)} episodes")
for i, result in enumerate(results):
    print(f"Episode {i}: {result['episode_spikes']} spikes")
```

### With Episode Callbacks

```python
def episode_callback(episode_idx, episode_stats):
    print(f"Episode {episode_idx} completed with {episode_stats['episode_spikes']} spikes")

with NLMBatchContext(brain, world, agent, num_episodes=20) as batch:
    results = batch.run_batch(episode_callback=episode_callback)
```

# Configuration guide
print("\n4. CONFIGURATION MANAGEMENT")
print("-" * 40)

print("""
## Basic Configuration

```python
import pynlm as nlm

# Create default configuration
config = nlm.createDefaultConfig()

# Modify configuration parameters
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 3)
config.set("brain.connection_probability", 0.05)
config.set("plasticity.stdp.learning_rate", 0.001)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config.loadFromFile("my_config.json")
```

## Configuration Templates

```python
from pynlm_enhanced import ConfigurationTemplate

# Create template for small experiments
template = ConfigurationTemplate("small_experiment")
template.add_parameter("brain.neuron_count", 100, "Number of neurons")
template.add_parameter("brain.connection_probability", 0.1, "Connection density")
template.add_parameter("plasticity.stdp.enable", True, "Enable STDP")

# Apply template to configuration
config = nlm.createDefaultConfig()
template.apply_to_config(config)
```

## Configuration Validation

```python
from pynlm_enhanced import validate_config

# Validate configuration
if validate_config(config):
    print("Configuration is valid")
else:
    print("Configuration is invalid")
```

# CLI guide
print("\n5. COMMAND-LINE INTERFACE")
print("-" * 40)

print("""
## NLM Command Line Tool

The NLM framework includes an advanced command-line interface for running simulations,
managing experiments, and analyzing results.

### Basic Commands

```bash
# Run basic simulation
nlm simulate --steps 1000 --timestep 0.1

# Run batch simulation with parameter sweeps
nlm batch --config config.json --param-sweep sweep1.json sweep2.json

# Run experiment
nlm experiment my_experiment --config config.json

# Analyze results
nlm analyze results.json --output analysis.json

# Export configuration
nlm export my_config.json

# Import configuration
nlm import other_config.json
```

### Configuration Commands

```bash
# Show configuration
nlm config show my_config.json

# Validate configuration
nlm config validate my_config.json
```

### Configuration File Examples

#### Basic configuration (config.json)
```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.05,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.01
        },
        "structural": {
            "enable": true
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    }
}
```

#### Parameter sweep configuration (sweep1.json)
```json
{
    "brain": {
        "neuron_count": 500
    },
    "plasticity": {
        "stdp": {
            "learning_rate": 0.0005
        }
    }
}
```

# Advanced usage guide
print("\n6. ADVANCED USAGE")
print("-" * 40)

print("""
## Performance Profiling

Track and analyze simulation performance.

```python
from pynlm_enhanced import NLMProfiler

profiler = NLMProfiler()

# Profile specific operations
start_time = profiler.start_timing("simulation_setup")

# ... simulation setup ...

profiler.stop_timing(start_time, "simulation_setup")

# Run simulation and profile step times
for step in range(1000):
    step_start = profiler.start_timing(f"step_{step}")
    # ... simulation step ...
    profiler.stop_timing(step_start, f"step_{step}")
    profiler.record_step_time(elapsed_time)

# Generate performance report
report = profiler.generate_report()
print(report)
```

## Event-Driven Programming

Respond to events generated by the simulation.

```python
from pynlm_enhanced import EventManager, BrainEvent, WorldEvent

# Create event manager
event_manager = EventManager()

# Add event listeners
def on_brain_event(event):
    print(f"Brain event: {type(event).__name__} at step {getattr(event, 'step', 'N/A')}")

def on_world_event(event):
    print(f"World event: {type(event).__name__}")

event_manager.add_brain_listener("step_completed", on_brain_event)
event_manager.add_world_listener("update", on_world_event)

# Emit events (automatically done by NLM)
# ... simulation code ...
```

## Error Handling and Logging

Implement robust error handling and logging.

```python
import logging
from pynlm_enhanced import NLMError, NLMValidationError

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)

# Handle NLM-specific errors
try:
    # ... simulation code ...
    pass
except NLMError as e:
    print(f"NLM error: {e}")
    # Handle specific error types
    if isinstance(e, NLMValidationError):
        print("Configuration validation failed")
except Exception as e:
    print(f"Unexpected error: {e}")
    logging.exception("Unexpected error in simulation")
```

## Custom Neuron and Synapse Control

For advanced users who need direct control over neural elements.

```python
# Direct neuron manipulation
neuron_id = NeuronId(0)  # Assuming neuron 0 exists
brain.inject_current(neuron_id, 10.0)  # Inject current

# Synapse management
# ... access via brain's internal systems ...

# Region control
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

## Memory Management

Control memory allocation and checkpointing.

```python
# Save and load simulation state
brain.save("checkpoint.bin")
# ... later ...
brain.load("checkpoint.bin")

# Memory monitoring
# ... access via system's memory managers ...
```

# Best practices guide
print("\n7. BEST PRACTICES")
print("-" * 40)

print("""
## General Best Practices

### 1. Resource Management

```python
# Always use context managers for simulations
with NLMSimulationContext(brain, world, agent) as ctx:
    stats = ctx.run(1000, timestep=0.1)
    # Resources are automatically cleaned up
```

### 2. Configuration Validation

```python
# Validate configuration before use
config = nlm.createDefaultConfig()
if validate_config(config):
    # Safe to use
    pass
```

### 3. Error Handling

```python
# Handle errors gracefully
try:
    # Run simulation
    stats = ctx.run(1000, timestep=0.1)
except NLMError as e:
    print(f"Simulation failed: {e}")
    # Recover or cleanup
```

### 4. Progress Monitoring

```python
# Use progress monitoring for long simulations
def progress_callback(event):
    if event.progress % 10 == 0:
        print(f"Progress: {event.progress:.1f}%")

with NLMSimulationContext(brain, world, agent) as ctx:
    stats = ctx.run(10000, timestep=0.1, progress_callback=progress_callback)
```

## Performance Best Practices

### 1. Memory Management

- Use checkpointing for long-running simulations
- Monitor memory usage during simulation
- Consider batch processing for multiple simulations

### 2. Simulation Optimization

```python
# Enable performance optimizations
agent.enableDevelopment(True)  # Reduces unnecessary computations
agent.enableCuriosity(True)    # Focuses computation on novel stimuli
```

### 3. Data Management

```python
# Use efficient data structures
# Avoid repeated configuration lookups
# Reuse objects when possible
```

## Testing and Debugging

### 1. Unit Testing

```python
# Test individual components
def test_brain_creation():
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    assert brain is not None
    brain.initialize()
    assert brain.getTotalNeuronCount() > 0

# Test simulation
def test_simulation():
    with NLMSimulationContext(brain, world, agent) as ctx:
        stats = ctx.run(100, timestep=0.1)
        assert stats.total_spikes >= 0
```

### 2. Debugging

```python
# Enable debug logging
logging.getLogger().setLevel(logging.DEBUG)

# Use profiling tools
profiler = NLMProfiler()
# ... run simulation with profiler ...
report = profiler.generate_report()
print(report)
```

## Advanced Usage Tips

### 1. Experiment Design

```python
# Design experiments with parameter sweeps
param_sweeps = [
    {"brain.neuron_count": 100, "plasticity.stdp.learning_rate": 0.001},
    {"brain.neuron_count": 500, "plasticity.stdp.learning_rate": 0.01},
    {"brain.neuron_count": 1000, "plasticity.stdp.learning_rate": 0.1}
]

# Run parameter sweep experiment
with NLMBatchContext(brain, world, agent, num_episodes=len(param_sweeps)) as batch:
    results = batch.run_batch()
```

### 2. Result Analysis

```python
from pynlm_enhanced import get_statistics_collector

# Collect statistics across multiple runs
collector = get_statistics_collector()

# Analyze results
summary = collector.get_overall_statistics()
print(f"Average firing rate: {summary['avg_firing_rate']:.2f} Hz")
print(f"Neuron count std: {summary['neurons']['std']:.0f}")
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: "Brain isn't doing anything"

**Problem:** The brain doesn't appear to be firing neurons.

**Solutions:**
1. Ensure `brain.initialize()` is called before simulation
2. Check that neurons have proper initial conditions
3. Verify configuration parameters are reasonable
4. Enable development or curiosity features to increase activity

#### Issue 2: Segmentation Fault

**Problem:** Program crashes during simulation.

**Solutions:**
1. Check for null pointer dereferences
2. Ensure all objects are properly initialized
3. Use memory debugging tools
4. Run with smaller simulation sizes first

#### Issue 3: Slow Performance

**Problem:** Simulation is too slow.

**Solutions:**
1. Enable development and curiosity to focus computation
2. Reduce neuron count if not needed
3. Use batch processing for parallel simulations
4. Profile performance to identify bottlenecks

#### Issue 4: Memory Issues

**Problem:** Memory usage is too high.

**Solutions:**
1. Use checkpointing to save memory
2. Monitor memory usage during simulation
3. Use smaller configurations for testing
4. Enable memory-efficient settings

# Migration Guide
print("\n8. MIGRATION GUIDE")
print("-" * 40)

print("""
## From Basic to Enhanced API

### Basic API

```python
# Basic simulation
brain = nlm.createBrain(config)
brain.initialize()
for step in range(1000):
    brain.step(step)
```

### Enhanced API

```python
# Enhanced simulation with resource management
with NLMSimulationContext(brain, world, agent) as ctx:
    stats = ctx.run(1000, timestep=0.1)
```

## Version Migration

### Version 1.0 to 1.0 (Enhanced)

**New Features:**
- Neuromodulator managers
- Memory system access
- Cognitive system controls
- Development system management
- Context managers
- Progress monitoring
- Event listeners
- Configuration templates
- Statistics collection
- Performance profiling

**Backward Compatibility:**
- All existing APIs remain unchanged
- New features are additive
- Enhanced features are opt-in
- Default behavior is unchanged

### Upgrading Guide

1. **Install enhanced bindings:**
   ```bash
   pip install -e .
   ```

2. **Import enhanced modules:**
   ```python
   from pynlm_enhanced import NLMSimulationContext, NeuromodulatorManager
   ```

3. **Update simulation code:**
   ```python
   # Old way
   with NLMSimulationContext(brain, world, agent) as ctx:
       stats = ctx.run(1000, timestep=0.1)
   
   # New way (same, but with enhancements)
   with NLMSimulationContext(brain, world, agent) as ctx:
       stats = ctx.run(1000, timestep=0.1)
   ```

## Appendix: API Reference

### Core Classes

- **Config**: Configuration management
- **Brain**: Neural simulation core
- **AgentBrain**: Brain-world interface
- **SimpleWorld**: Environment simulation
- **SensoryInput**: Sensory data container
- **Action**: Motor output container
- **SensoryPercept**: Perception container
- **WorldObject**: World object representation
- **AgentBody**: Agent state container
- **ActionResult**: Action outcome container

### Enhanced Managers

- **NeuromodulatorManager**: Neuromodulation control
- **MemoryManager**: Memory system access
- **CognitiveSystemManager**: Cognitive system access
- **DevelopmentSystemManager**: Development system control
- **PlasticityManager**: Plasticity system control
- **EventManager**: Event system management
- **ProgressMonitor**: Progress monitoring
- **ConfigurationTemplate**: Configuration template management
- **StatisticsCollector**: Statistics collection and analysis
- **NLMProfiler**: Performance profiling

### Context Managers

- **NLMSimulationContext**: Simulation resource management
- **NLMBatchContext**: Batch processing management

### Global Utilities

- **get_event_manager()**: Get global event manager
- **get_config_template(name)**: Get configuration template
- **get_statistics_collector()**: Get global statistics collector
- **get_profiler()**: Get global profiler

## References

1. nlm-core Documentation: https://nlm.readthedocs.io
2. NLM Scientific Papers: Available in docs/ directory
3. GitHub Repository: https://github.com/nlm-project/nlm
4. Issue Tracker: https://github.com/nlm-project/nlm/issues
5. Mailing List: nlm-dev@nlm.org

# End of Guide
print("\n" + "=" * 80)
print("NLM Complete User Guide - End of Document")
print("=" * 80)
print(f"Version: {__doc__.split('Version:')[1].split('(')[0].strip()} (Enhanced)")
print(f"Date: {__doc__.split('Date:')[1].split('\\n')[0].strip()}")
print("License: MIT")
print("=" * 80)
"""