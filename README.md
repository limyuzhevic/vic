# NLM Python Bindings - Comprehensive User Guide

## Overview

NLM (Neural Learning Machine) Python bindings provide an intuitive interface to the C++ neural simulation framework. This guide covers everything from basic usage to advanced features for expert users.

## Quick Start

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Core API Reference

### Brain Class

The `Brain` class is the central component that manages neural simulation.

#### Creation and Initialization

```python
# Create from default config
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Create with custom config
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.01)
brain = pynlm.createBrain(config)

# Initialize
brain.initialize()
```

#### Simulation Control

```python
# Run single step
brain.step(step=10)

# Run multiple steps with timing
for step in range(1000):
    brain.step(step, time=step * 0.1)

# Reset brain
brain.reset()

# Save and load
brain.save("checkpoint.bin")
# Later...
brain.load("checkpoint.bin")
```

#### Sensory Input

```python
# Inject sensory data
brain.receiveSensoryInput(sensory_input)

# Inject current to specific neurons
brain.injectCurrent(neuron_id, current=5.0)

# Inject current to all neurons of a type
brain.injectCurrentToNeurons(neuron_type=pynlm.NeuronType.Sensory, current=2.0)
```

#### Action Production

```python
# Get motor action
action = brain.produceAction()
print(f"Action type: {action.getType()}")
print(f"Action parameters: {action.getParameters()}")
```

### AgentBrain Class

The `AgentBrain` bridges the brain with the world environment.

#### Initialization

```python
# Create agent brain
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### Sensory Processing

```python
# Process sensor percept
agent.processSensoryInput(percept)
```

#### Motor Decoding

```python
# Get action from brain activity
motor_cmd = agent.decodeMotorCommand()
```

#### Neuromodulation

```python
# Apply reward-based learning
agent.applyRewardModulation(reward=1.0, predicted_reward=0.5)

# Update development
agent.updateDevelopment(timestep=0.1)
```

#### Subsystem Management

```python
# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check subsystem status
status = agent.get_subsystem_status()
print(f"Reward modulation: {status['reward_modulation']}")
```

### SimpleWorld Class

The `SimpleWorld` simulates the environment.

#### Configuration

```python
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
```

#### World Control

```python
world.reset()
world.update(timestep=0.1)
world.setAgentStart(10.0, 10.0)
```

#### Object Management

```python
# Add objects
obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0)
world.addObject(obj)

# Apply motor commands
world.applyMotorCommand(action, world.getSimulationTime())
```

### Config Class

The `Config` class manages simulation parameters.

#### Configuration

```python
config = pynlm.createDefaultConfig()

# Pythonic interface
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.01)

# Query values
neuron_count = config.get("brain.neuron_count", 1000)
```

## Advanced Python Features

### Context Managers

```python
class NLMSimulation:
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
    
    def __enter__(self):
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.brain:
            self.brain.reset()
    
    def run_episode(self, num_steps):
        for step in range(num_steps):
            self.brain.step(step)
```

### Decorators and Utilities

```python
def with_auto_save(filepath):
    def decorator(func):
        def wrapper(*args, **kwargs):
            result = func(*args, **kwargs)
            if args and hasattr(args[0], 'brain'):
                args[0].brain.save(filepath)
            return result
        return wrapper
    return decorator

@with_auto_save("checkpoint.bin")
def run_simulation(brain, world, agent, steps):
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    return brain.getTotalSpikeCount()
```

### Callback System

```python
class SimulationCallback:
    def on_step(self, step, brain, world, agent):
        pass
    
    def on_episode_end(self, brain, world, agent):
        pass

class CallbackSimulation:
    def __init__(self, callback):
        self.callback = callback
    
    def run(self, brain, world, agent, num_steps):
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            self.callback.on_step(step, brain, world, agent)
        self.callback.on_episode_end(brain, world, agent)
```

### Data Export/Import

```python
# Export brain state
brain.save("my_brain.bin")

# Import brain state
imported_brain = pynlm.import_brain_state("my_brain.bin")

# Export to JSON (simulation function)
def brain_to_json(brain):
    stats = brain.get_statistics()
    return pyjson.dumps(stats, indent=2)

# Import from JSON (simulation function)
def brain_from_json(json_str):
    import json
    data = json.loads(json_str)
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    # Restore configuration from JSON data
    return brain
```

## Expert Mode Features

### Neural Network Architecture

```python
def create_deep_brain(config=None):
    """Create a brain with multiple regions."""
    config = config or pynlm.createDefaultConfig()
    
    brain = pynlm.createBrain(config)
    
    # Add different regions for different functions
    sensory_region_id = brain.addRegion("sensory")
    association_region_id = brain.addRegion("association")
    motor_region_id = brain.addRegion("motor")
    
    return brain, {
        'sensory': brain.getRegion(sensory_region_id),
        'association': brain.getRegion(association_region_id),
        'motor': brain.getRegion(motor_region_id)
    }

# Use regions
def process_with_regions(brain, sensory_input):
    sensory_region = brain.get_region_for_type('sensory')
    association_region = brain.get_region_for_type('association')
    motor_region = brain.get_region_for_type('motor')
    
    # Process through different regions
    brain.receiveSensoryInput(sensory_input)
    brain.step(0)
    
    return brain.produceAction()
```

### Performance Monitoring

```python
import time
import psutil
import threading

class PerformanceMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.metrics = []
        self.monitoring = False
    
    def start_monitoring(self, interval=0.1):
        self.monitoring = True
        threading.Thread(target=self._monitor_loop, args=(interval,)).start()
    
    def _monitor_loop(self, interval):
        while self.monitoring:
            self.metrics.append({
                'time': time.time(),
                'neurons_firing': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'cpu': psutil.cpu_percent(),
                'memory': psutil.virtual_memory().percent
            })
            time.sleep(interval)
    
    def stop_monitoring(self):
        self.monitoring = False
    
    def get_performance_stats(self):
        if not self.metrics:
            return None
        
        firing_rates = [m['neurons_firing'] for m in self.metrics]
        spikes_per_second = [m['total_spikes'] / (m['time'] - self.metrics[0]['time']) 
                           for m in self.metrics if m['time'] > self.metrics[0]['time']]
        
        return {
            'avg_firing_rate': sum(firing_rates) / len(firing_rates),
            'max_firing_rate': max(firing_rates),
            'avg_spikes_per_second': sum(spikes_per_second) / len(spikes_per_second) if spikes_per_second else 0,
            'duration': len(self.metrics) * 0.1,
            'peak_cpu': max(m['cpu'] for m in self.metrics),
            'peak_memory': max(m['memory'] for m in self.metrics)
        }
```

### Advanced Learning Patterns

```python
class LearningPattern:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
    
    def exploration_based_learning(self, steps):
        """Curiosity-driven exploration."""
        for step in range(steps):
            # Enable curiosity
            self.agent.enableCuriosity(True)
            
            # Update world
            self.world.update(0.1)
            
            # Get percept
            percept = self.world.getSensoryPercept()
            
            # Process with curiosity
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply curiosity-based reward
            novelty = self.agent.getNoveltyLevel()
            self.agent.applyRewardModulation(novelty, 0.0)
    
    def structured_playback(self, checkpoint_file, speed=1.0):
        """Playback simulation with variable speed."""
        self.brain.load(checkpoint_file)
        
        for step in range(1000):
            timestep = step * 0.1 * speed
            self.world.update(timestep)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step, timestep)
            self.world.applyMotorCommand(self.agent.decodeMotorCommand(), timestep)
    
    def curriculum_learning(self, curriculum):
        """Progressive difficulty curriculum."""
        for stage, (config, steps) in enumerate(curriculum):
            print(f"Stage {stage + 1}: {config.get('description', 'Unknown')}")
            
            # Update config
            for key in config.getKeys():
                value = config.get(key)
                self.brain.getConfig().set(key, value)
            
            # Train on this stage
            for step in range(steps):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
```

## Utility Functions

### Simulation Utilities

```python
def run_complete_simulation(config=None, world_config=None, num_steps=1000):
    """Run a complete simulation with brain, world, and agent."""
    # Create brain
    config = config or pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    if world_config:
        world.configure(**world_config)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward if available
        percept = world.getSensoryPercept()
        if percept.getInternal():
            reward = percept.getInternal()[0]
            agent.applyRewardModulation(reward, 0.0)
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'steps_completed': num_steps,
        'final_neurons_firing': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount()
    }

def create_brain_from_checkpoint(checkpoint_file):
    """Create a brain from a checkpoint file."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.load(checkpoint_file)
    return brain
```

### Visualization Utilities

```python
def create_brain_statistics(brain):
    """Create comprehensive brain statistics."""
    stats = brain.get_statistics()
    
    # Add derived statistics
    stats['e_i_balance'] = brain.getExcitationInhibitionRatio()
    stats['activity_level'] = stats['firing_neurons'] / max(stats['total_neurons'], 1)
    stats['development_progress'] = int(stats['developmental_stage']) / 4.0  # Normalize
    
    return stats

def plot_brain_dynamics(brain_history):
    """Plot brain dynamics over time."""
    import matplotlib.pyplot as plt
    
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Neurons firing
    axes[0, 0].plot(brain_history['steps'], brain_history['neurons_firing'])
    axes[0, 0].set_xlabel('Step')
    axes[0, 0].set_ylabel('Neurons Firing')
    axes[0, 0].set_title('Brain Activity')
    
    # Firing rate
    axes[0, 1].plot(brain_history['steps'], brain_history['firing_rate'])
    axes[0, 1].set_xlabel('Step')
    axes[0, 1].set_ylabel('Firing Rate')
    axes[0, 1].set_title('Firing Rate Over Time')
    
    # E/I ratio
    axes[1, 0].plot(brain_history['steps'], brain_history['e_i_ratio'])
    axes[1, 0].set_xlabel('Step')
    axes[1, 0].set_ylabel('E/I Ratio')
    axes[1, 0].set_title('Excitation-Inhibition Balance')
    
    # Spikes
    axes[1, 1].plot(brain_history['steps'], brain_history['total_spikes'])
    axes[1, 1].set_xlabel('Step')
    axes[1, 1].set_ylabel('Total Spikes')
    axes[1, 1].set_title('Cumulative Spikes')
    
    plt.tight_layout()
    return fig
```

## Migration Guide

### From Old Version to New Version

This section covers changes between major versions:

#### Version 0.1.0 Changes

1. **Enhanced Config class**: Added Pythonic `set()` and `get()` methods
2. **Better error handling**: More informative exceptions
3. **Additional utility functions**: `run_agent_simulation()`, `create_agent_from_config()`
4. **Enhanced statistical access**: `brain.get_statistics()` for comprehensive metrics
5. **Improved object creation**: Better constructors for `WorldObject` and `Action`
6. **Context manager support**: Added context manager support for simulation lifecycle
7. **Advanced Python features**: Callback system, decorators, performance monitoring

#### Migration Examples

```python
# Old way
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# New way (same, but with enhanced features)
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)  # New Pythonic interface
brain = pynlm.createBrain(config)
brain.initialize()

# Old way
action = brain.produceAction()

# New way
action = brain.produceAction()
action_with_params = action.with_parameters([0.5, 0.8, 1.0])  # New convenience method
```

## Troubleshooting

### Common Issues

**Issue: ImportError - No module named 'pynlm'**

```bash
pip install pynlm
```

**Issue: Segmentation fault** (C++ crash)

```python
# Ensure proper initialization
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this first!
brain.reset()      # Reset before running
```

**Issue: High memory usage** (large brain simulation)

```python
# Use smaller configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Reduce neurons
```

**Issue: Slow simulation performance**

```python
# Use performance monitoring
monitor = PerformanceMonitor(brain)
monitor.start_monitoring()
```

**Issue: Complex simulation code**

```python
# Use context managers
with NLMSimulation(config) as sim:
    # Run simulation
    sim.run_episode(1000)
```

### Advanced Debugging

```python
import traceback

def safe_simulation_run(brain, world, agent, num_steps):
    """Run simulation with error handling."""
    try:
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    except Exception as e:
        print(f"Simulation error at step {step}: {e}")
        traceback.print_exc()
        raise
    
    return brain.getTotalSpikeCount()
```

## Examples Gallery

### Example 1: Simple Brain

```python
import pynlm

# Create and run simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### Example 2: Complete Agent

```python
import pynlm

# Setup complete agent simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Example 3: Advanced Learning

```python
import pynlm

class AdvancedAgent:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Initialize with learning
        self.brain.initialize()
        self.agent.initialize(self.world)
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
    
    def run_curriculum(self):
        """Run curriculum learning."""
        curriculum = [
            {"stage": "exploration", "steps": 200},
            {"stage": "exploitation", "steps": 300},
            {"stage": "consolidation", "steps": 200}
        ]
        
        for stage in curriculum:
            print(f"Stage: {stage['stage']}")
            
            for step in range(stage['steps']):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
    
    def save_progress(self, filename):
        """Save agent progress."""
        self.brain.save(filename)
    
    def load_progress(self, filename):
        """Load agent progress."""
        self.brain.load(filename)

# Run advanced agent
agent = AdvancedAgent()
agent.run_curriculum()
agent.save_progress("my_agent.bin")
```

## API Summary

### Classes

- **Brain**: Central neural simulation brain
- **Config**: Configuration management
- **AgentBrain**: Agent-brain interface
- **SimpleWorld**: Simple 2D world simulation
- **Action**: Motor action representation
- **WorldObject**: World object representation
- **AgentBody**: Agent body state
- **SensoryPercept**: Sensory perception data
- **Vision**: Vision sensory input
- **Audio**: Audio sensory input
- **InternalSignals**: Internal signals
- **ActionResult**: Action result from world

### Enums

- **NeuronType**: Neuron type enumeration
- **SynapseType**: Synapse type enumeration
- **ActionType**: Action type enumeration
- **MotorCommand**: Motor command enumeration
- **WorldObjectType**: World object type enumeration
- **DevelopmentalStage**: Developmental stage enumeration
- **FiringState**: Neuron firing state enumeration

### Factory Functions

- **createDefaultConfig()**: Create default configuration
- **createBrain()**: Create brain with config
- **createSimpleWorld()**: Create simple world
- **createAgentBrain()**: Create agent brain interface

### Utility Functions

- **run_agent_simulation()**: Run complete agent simulation
- **create_agent_from_config()**: Create agent from config
- **get_brain_version()**: Get version information
- **simulate_step()**: Convenience function for single step
- **batch_process()**: Process multiple steps
- **export_brain_state()**: Export brain state
- **import_brain_state()**: Import brain state

### Advanced Features

- **Context managers**: `NLMSimulation` class
- **Decorators**: `@with_auto_save`, `@callback`
- **Callback system**: `SimulationCallback`, `CallbackSimulation`
- **Performance monitoring**: `PerformanceMonitor`
- **Learning patterns**: `ExplorationBasedLearning`, `CurricularLearning`
- **Utilities**: `run_complete_simulation()`, `brain_to_json()`, `create_brain_from_checkpoint()`
- **Visualization**: `create_brain_statistics()`, `plot_brain_dynamics()`

## Conclusion

The NLM Python bindings provide a comprehensive interface to the neural simulation framework, supporting users from beginners to experts. This guide covers all aspects of using the library effectively, from basic examples to advanced patterns.

The bindings have been significantly improved with:

1. **Pythonic APIs**: Intuitive interfaces that match Python conventions
2. **Enhanced error handling**: Better exceptions and debugging support
3. **Advanced features**: Context managers, callbacks, performance monitoring
4. **Comprehensive documentation**: Detailed examples and migration guides
5. **Utility functions**: Common operations made simple
6. **Expert tools**: Advanced learning patterns and debugging utilities

These improvements make it easier than ever to create, simulate, and analyze neural systems using Python.