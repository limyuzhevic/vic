# NLM Python Bindings - Comprehensive Documentation

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python bindings, including advanced usage patterns, examples, and best practices.

## Table of Contents

1. [Quick Start Guide](#quick-start-guide)
2. [Advanced Features](#advanced-features)
3. [Complete Examples](#complete-examples)
4. [Best Practices](#best-practices)
5. [Troubleshooting](#troubleshooting)
6. [API Reference](#api-reference)
7. [Performance Optimization](#performance-optimization)
8. [Migration Guide](#migration-guide)

## Quick Start Guide

This guide provides comprehensive information about using the NLM Python bindings for neural simulation.

### Installation

```bash
# Install from PyPI (if available)
pip install pynlm

# Install from source (development)
pip install -e .

# Install with build dependencies
pip install scikit-build-core pybind11 numpy pytest

# Build from source
python -m build
cd dist && pip install pynlm-0.1.0-py3-none-any.whl
```

### Basic Setup

```python
import pynlm
import numpy as np

# Create default configuration
config = pynlm.createDefaultConfig()

# Optionally customize configuration
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print(f"Setup complete! Brain has {brain.getTotalNeuronCount()} neurons")
```

## Advanced Features

### 1. Convenience Functions

The NLM bindings provide several convenience functions to simplify common tasks:

```python
# Create and initialize brain automatically
brain = pynlm.createDefaultBrain()

# Create brain with custom configuration
brain = pynlm.createBrainWithConfig(config)

# Create agent with world
agent = pynlm.createAgentWithWorld(brain, world)

# Get comprehensive statistics
stats = pynlm.getBrainStats(brain)
print(f"Neurons: {stats['total_neurons']}, Firing: {stats['firing_neurons']}")

# Configure brain with recommended defaults
config = pynlm.configureBrainDefaults()

# Create experiment-optimized configuration
config = pynlm.createExperimentConfig()
```

### 2. Helper Classes

```python
# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Use simulation manager as context manager
with pynlm.SimulationManager(brain, world, agent) as sim_manager:
    # Run simulation with automatic reset
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
    # Save checkpoint
    sim_manager.checkpoint("simulation_checkpoint")
```

### 3. Batch Operations

```python
# Batch inject current into multiple neurons
neuron_ids = [pynlm.INVALID_NEURON_ID, pynlm.INVALID_NEURON_ID]  # Example IDs
currents = [1.0, -0.5]
injections = list(zip(neuron_ids, currents))
pynlm.batchInjectCurrent(brain, injections)
```

### 4. Simulation Management

```python
# Run simulation episode
def run_episode(agent, world, num_steps):
    agent, world = pynlm.runSimulationEpisode(agent, world, num_steps)
    return agent, world

# Run with timing statistics
agent, world = pynlm.runTimedSimulation(brain, world, agent, 10.0)
```

### 5. Development and Learning Features

```python
# Advance developmental stage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)

# Enable all learning systems
agent.enableAllSystems()  # Custom convenience method
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

### 6. Error Handling

```python
# Validate configuration before use
config = pynlm.createDefaultConfig()
if pynlm.validateBrainConfig(config):
    brain = pynlm.createBrain(config)
    brain.initialize()
else:
    print("Invalid configuration!")

# Safe brain stepping with error handling
if pynlm.safeBrainStep(brain, step):
    # Step successful
else:
    # Handle error gracefully
    pass
```

## Complete Examples

### Example 1: Silent Brain Simulation

```python
import pynlm
import time

def silent_brain_simulation(num_steps=1000):
    """Run a simple brain simulation without world interaction."""
    print("Starting silent brain simulation...")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    start_time = time.time()
    
    # Run simulation
    for step in range(num_steps):
        brain.step(step)
        
        if step % 100 == 0:
            stats = pynlm.getBrainStats(brain)
            print(f"Step {step}: {stats['firing_neurons']} firing neurons")
    
    elapsed = time.time() - start_time
    print(f"Simulation complete! {num_steps} steps in {elapsed:.2f} seconds")
    
    return brain

# Run the simulation
brain = silent_brain_simulation()
```

### Example 2: Complete Agent Training

```python
import pynlm
import random

class TrainingEnvironment:
    """Simple training environment with obstacles and rewards."""
    
    def __init__(self, width=30, height=30):
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, visionWidth=7, visionHeight=7)
        self.world.reset()
        
        # Add some obstacles and resources
        for i in range(10):
            x = random.randint(2, width-2)
            y = random.randint(2, height-2)
            self.world.addObject(pynlm.createWorldObject(
                pynlm.WorldObjectType.Wall, x, y))
                
        for i in range(5):
            x = random.randint(2, width-2)
            y = random.randint(2, height-2)
            self.world.addObject(pynlm.createWorldObject(
                pynlm.WorldObjectType.Resource, x, y, 1.0))
    
    def reset(self):
        self.world.reset()
        return self.get_observation()
    
    def get_observation(self):
        return self.world.getSensoryPercept()
    
    def step(self, action, dt=0.1):
        # Apply action
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Update world
        self.world.update(dt)
        
        # Get new observation and reward
        percept = self.world.getSensoryPercept()
        reward = self.calculate_reward(percept)
        
        return percept, reward, self.world.getSimulationTime()
    
    def calculate_reward(self, percept):
        """Calculate reward based on sensory input."""
        internal = percept.getInternal()
        if internal and len(internal) > 0:
            return internal[0]  # Use internal signal as reward
        return 0.0

def train_agent(episodes=100, steps_per_episode=50):
    """Train an agent using the NLM framework."""
    print("Starting agent training...")
    
    # Create NLM agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Training loop
    episode_rewards = []
    
    for episode in range(episodes):
        # Reset environment
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        episode_reward = 0.0
        
        for step in range(steps_per_episode):
            # Get current state
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action and get reward
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get new state and reward
            new_percept = world.getSensoryPercept()
            reward = agent.getNeuromodulationLevel()  # Use neuromodulation as reward proxy
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, 0.0)
            
            episode_reward += reward
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Print progress
            if step % 10 == 0:
                stats = pynlm.getBrainStats(brain)
                print(f"Episode {episode}, Step {step}: Reward {reward:.3f}, "
                      f"Neurons {stats['firing_neurons']}, "
                      f"Curiosity {agent.getCuriosityLevel():.3f}")
        
        episode_rewards.append(episode_reward)
        
        # Print episode summary
        avg_reward = sum(episode_rewards[-10:]) / min(len(episode_rewards), 10)
        print(f"Episode {episode} complete. Total reward: {episode_reward:.3f}, "
              f"Average (last 10): {avg_reward:.3f}")
    
    # Print training summary
    print(f"\nTraining complete!")
    print(f"Total episodes: {episodes}")
    print(f"Average reward: {sum(episode_rewards) / len(episode_rewards):.3f}")
    print(f"Best reward: {max(episode_rewards):.3f}")
    
    return brain, agent, world

# Run training
trained_brain, trained_agent, trained_world = train_agent()
```

### Example 3: Performance Benchmarking

```python
import pynlm
import time

def benchmark_performance():
    """Benchmark NLM performance with different configurations."""
    print("Performance benchmarking...")
    
    # Test different brain sizes
    brain_sizes = [1000, 5000, 10000]
    
    for size in brain_sizes:
        print(f"\nBenchmarking brain with {size} neurons...")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", size)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Benchmark different step counts
        step_counts = [100, 500, 1000]
        
        for step_count in step_counts:
            results = pynlm.benchmarkStep(brain, step_count)
            
            print(f"  {step_count} steps:")
            print(f"    Time: {results['elapsed_time']:.3f}s")
            print(f"    Steps/sec: {results['steps_per_second']:.1f}")
            print(f"    Neurons/sec: {results['neurons_per_second']:.0f}")
    
    # Benchmark complete simulation
    print("\nBenchmarking complete simulation...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 5000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run timed simulation
    start_time = time.time()
    agent, world = pynlm.runTimedSimulation(brain, world, agent, 5.0)
    elapsed = time.time() - start_time
    
    print(f"5 second simulation took {elapsed:.3f}s (expected ~5.0s)")

benchmark_performance()
```

### Example 4: Advanced File Operations

```python
import pynlm
import os

def advanced_file_operations():
    """Demonstrate advanced file I/O operations."""
    print("Advanced file operations demo...")
    
    # Create simulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run some simulation steps
    for step in range(20):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Save with custom prefix
    prefix = "my_simulation"
    state = pynlm.saveSimulationState(brain, world, agent, prefix)
    
    print(f"Saved simulation state to {prefix}*.bin and {prefix}*.json")
    print(f"State metadata: {state}")
    
    # List saved files
    for filename in os.listdir('.'):
        if filename.startswith(prefix):
            print(f"  - {filename}")
    
    # Load state
    print("\nLoading simulation state...")
    success = pynlm.loadSimulationState(brain, world, agent, prefix)
    print(f"Load successful: {success}")
    
    # Cleanup
    for filename in os.listdir('.'):
        if filename.startswith(prefix):
            os.remove(filename)
            print(f"Removed {filename}")

advanced_file_operations()
```

## Best Practices

### 1. Memory Management

```python
# Best practice: Explicit cleanup
import weakref

# Use weak references to avoid memory leaks
brain_ref = weakref.ref(brain)
world_ref = weakref.ref(world)
agent_ref = weakref.ref(agent)

# Clear strong references when done
brain = None
world = None
agent = None

# Check if objects are still alive
if brain_ref() is not None:
    print("Brain still referenced")
```

### 2. Error Handling

```python
import pynlm

def robust_simulation(brain_size=1000):
    """Robust simulation with comprehensive error handling."""
    try:
        # Validate inputs
        if brain_size <= 0:
            raise ValueError("Brain size must be positive")
        
        # Create components with validation
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", brain_size)
        
        if not pynlm.validateBrainConfig(config):
            raise ValueError("Invalid brain configuration")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation with safe stepping
        for step in range(100):
            if not pynlm.safeBrainStep(brain, step):
                print(f"Warning: Brain step {step} failed")
                break
                
        return True
        
    except Exception as e:
        print(f"Simulation error: {e}")
        return False

# Usage
robust_simulation(2000)
```

### 3. Performance Optimization

```python
import pynlm
import time

def optimized_simulation(brain_size=5000):
    """Optimized simulation with performance considerations."""
    start_time = time.time()
    
    # Create components
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", brain_size)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    num_steps = 200
    step_times = []
    
    for step in range(num_steps):
        step_start = time.time()
        
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        step_time = time.time() - step_start
        step_times.append(step_time)
    
    # Performance analysis
    total_time = time.time() - start_time
    avg_step_time = sum(step_times) / len(step_times)
    std_step_time = (sum((t - avg_step_time) ** 2 for t in step_times) / len(step_times)) ** 0.5
    
    stats = pynlm.getPerformanceStats(brain)
    
    print(f"Performance Summary:")
    print(f"  Total time: {total_time:.3f}s")
    print(f"  Average step time: {avg_step_time:.6f}s")
    print(f"  Step time std: {std_step_time:.6f}s")
    print(f"  Steps per second: {num_steps / total_time:.1f}")
    print(f"  Neurons per second: {brain_size * num_steps / total_time:.0f}")
    
    return total_time < (num_steps * 0.01)  # Should complete in < 10ms per step

optimized_simulation()
```

### 4. Reproducibility

```python
import pynlm
import random

def reproducible_simulation(seed=42):
    """Simulation with reproducible random number generation."""
    # Set random seeds for reproducibility
    random.seed(seed)
    pynlm.setRandomSeed(world, seed)
    
    # Create components
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    
    # Run simulation
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Save with timestamp for version control
    timestamp = time.strftime("%Y%m%d_%H%M%S")
    pynlm.saveSimulationState(brain, world, agent, f"simulation_{timestamp}")
    
    return brain

# Run reproducible simulation
reproducible_simulation(123)
```

## Best Practices for Development

1. **Always validate configuration before use**
2. **Use convenience functions for common patterns**
3. **Implement comprehensive error handling**
4. **Profile performance for large simulations**
5. **Use context managers for resource management**
6. **Save checkpoints regularly for long simulations**
7. **Set random seeds for reproducible experiments**
8. **Use batch operations for multiple updates**
9. **Monitor neuromodulation levels during learning**
10. **Clear references when done to prevent memory leaks**

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: "My brain isn't doing anything"

**Problem**: The brain appears inactive or not firing neurons.

**Solutions**:
1. Ensure `brain.initialize()` is called before `brain.step()`
2. Add sensory input using `agent.processSensoryInput(percept)`
3. Enable neuromodulation for learning: `agent.enableRewardModulation(True)`
4. Check developmental stage: `brain.getDevelopmentalStage()`
5. Increase simulation time or steps

#### Issue 2: "The agent isn't moving"

**Problem**: The agent is not taking actions or moving in the world.

**Solutions**:
1. Ensure `world.applyMotorCommand(action, currentTime)` is called
2. Check that `world.update(timestep)` is called
3. Verify that `agent.decodeMotorCommand()` returns a valid action
4. Check that the agent has reached `agent.getMotorOutputSize()` expected outputs

#### Issue 3: "Everything is 0"

**Problem**: All values in the simulation are zero.

**Solutions**:
1. Brains need time to "warm up" - try more steps
2. Some neurons need input to fire - make sure sensory input is connected
3. Check configuration parameters for realistic values
4. Verify that neurons are properly initialized

#### Issue 4: Performance Problems

**Problem**: Simulation is too slow for large brain sizes.

**Solutions**:
1. Use more efficient data structures
2. Enable SIMD optimization if available
3. Reduce simulation complexity where possible
4. Use batch operations instead of individual steps
5. Profile using `pynlm.benchmarkStep()` to identify bottlenecks

#### Issue 5: Import/Installation Problems

**Problem**: Cannot import pynlm or install the module.

**Solutions**:
1. Check that all dependencies are installed:
   ```bash
   pip install scikit-build-core pybind11 numpy pytest
   ```
2. Build from source if installation fails:
   ```bash
   pip install -e .
   ```
3. Check CMake version and compiler support
4. Verify that Python bindings were built correctly

### Debugging Checklist

```python
def debug_simulation():
    """Debug simulation using print statements and helper functions."""
    import pynlm
    
    print("=== NLM Simulation Debug ===")
    
    # Check module version
    print(f"Version: {pynlm.__version__}")
    
    # Create simple components
    config = pynlm.createDefaultConfig()
    print(f"Default config keys: {config.getKeys()}")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Check brain state
    print(f"Brain initialized: Total neurons={brain.getTotalNeuronCount()}, "
          f"Total synapses={brain.getTotalSynapseCount()}")
    
    # Test world creation
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10)
    
    # Test agent creation
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"Agent initialized: Sensory input size={agent.getSensoryInputSize()}, "
          f"Motor output size={agent.getMotorOutputSize()}")
    
    # Run minimal simulation
    for step in range(5):
        world.update(0.1)
        percept = world.getSensoryPercept()
        print(f"Step {step}: Vision width={percept.getVisionWidth()}, "
              f"Has internal={percept.getInternal() is not None}")
        
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("=== Debug Complete ===")

# Run debug
debug_simulation()
```

## API Reference

### Core Classes

#### Config
Configuration management for NLM parameters.

- `createDefaultConfig()` - Create default configuration
- `config.set(key, value)` - Set configuration parameter
- `config.get(key)` - Get configuration parameter
- `config.has(key)` - Check if key exists
- `config.loadFromFile(filepath)` - Load from JSON file
- `config.saveToFile(filepath)` - Save to JSON file
- `config.clear()` - Clear all configuration
- `config.summary()` - Get configuration summary

#### Brain
Central neural simulation brain.

- `createBrain(config)` - Create brain with configuration
- `brain.initialize()` - Initialize brain
- `brain.step(step)` - Perform simulation step
- `brain.step(step, time)` - Perform simulation step with timestamp
- `brain.receiveSensoryInput(input)` - Inject sensory data
- `brain.produceAction()` - Get motor action
- `brain.reset()` - Reset brain state
- `brain.save(filepath)` - Save brain state
- `brain.load(filepath)` - Load brain state
- `brain.getTotalNeuronCount()` - Get total neurons
- `brain.getTotalSynapseCount()` - Get total synapses
- `brain.getFiringNeuronCount()` - Get firing neurons
- `brain.getAverageFiringRate()` - Get average firing rate
- `brain.getTotalSpikeCount()` - Get total spike count
- `brain.getDevelopmentalStage()` - Get developmental stage
- `brain.setDevelopmentalStage(stage)` - Set developmental stage

#### SimpleWorld
Simple 2D world simulation.

- `createSimpleWorld()` - Create world
- `world.configure(width, height, visionWidth, visionHeight)` - Configure world
- `world.reset()` - Reset world
- `world.update(timestep)` - Update world
- `world.applyMotorCommand(cmd, currentTime)` - Apply motor command
- `world.setAgentStart(x, y)` - Set agent start position
- `world.getSensoryPercept()` - Get sensory percept
- `world.getAgentBody()` - Get agent body state
- `world.addObject(obj)` - Add world object
- `world.removeObject(x, y)` - Remove world object
- `world.isValidPosition(x, y)` - Check valid position

#### AgentBrain
Agent brain interface connecting brain to world.

- `createAgentBrain(brain)` - Create agent brain
- `agent.initialize(world)` - Initialize agent
- `agent.processSensoryInput(percept)` - Process sensory input
- `agent.decodeMotorCommand()` - Decode motor command
- `agent.applyRewardModulation(reward, predicted)` - Apply reward modulation
- `agent.updateDevelopment(timestep)` - Update development
- `agent.getCuriosityLevel()` - Get curiosity level
- `agent.getNoveltyLevel()` - Get novelty level
- `agent.getNeuromodulationLevel()` - Get neuromodulation level

### Convenience Functions

#### Factory Functions
- `createDefaultBrain()` - Create and initialize brain with defaults
- `createBrainWithConfig(config)` - Create brain with custom config
- `createAgentWithWorld(brain, world)` - Create agent with world
- `createAction(type)` - Create action with type
- `createActionWithParams(type, params)` - Create action with parameters
- `createWorldObject(type, x, y)` - Create world object
- `createVisionInput(width, height, channels)` - Create vision input
- `createAudioInput(sampleRate, numSamples)` - Create audio input
- `createInternalSignals()` - Create internal signals

#### Helper Functions
- `getBrainStats(brain)` - Get brain statistics dictionary
- `configureBrainDefaults(config)` - Configure with defaults
- `createExperimentConfig()` - Create experiment configuration
- `batchInjectCurrent(brain, injections)` - Batch inject current
- `runSimulationEpisode(agent, world, numSteps)` - Run simulation episode
- `saveSimulationState(brain, world, agent, prefix)` - Save simulation state
- `loadSimulationState(brain, world, agent, prefix)` - Load simulation state
- `getPerformanceStats(brain)` - Get performance statistics
- `benchmarkStep(brain, numSteps)` - Benchmark brain performance
- `safeBrainStep(brain, step)` - Safe brain stepping
- `validateBrainConfig(config)` - Validate configuration
- `setRandomSeed(world, seed)` - Set random seed
- `getVersionInfo()` - Get version information
- `printBuildInfo()` - Print build information

#### Classes
- `SimulationManager` - Context manager for simulation reset and checkpointing

### Enumerations

#### NeuronType
- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons
- `Modulatory` - Modulatory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal neurons

#### ActionType
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `MoveLeft` - Move left
- `MoveRight` - Move right
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Look` - Look
- `LookUp` - Look up
- `LookDown` - Look down
- `Interact` - Interact
- `Eat` - Eat
- `Drink` - Drink
- `Rest` - Rest
- `Wait` - Wait
- `Custom` - Custom action

#### MotorCommand
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `LookLeft` - Look left
- `LookRight` - Look right
- `Interact` - Interact
- `Wait` - Wait

#### WorldObjectType
- `Empty` - Empty space
- `Resource` - Resource
- `Hazard` - Hazard
- `Wall` - Wall
- `Marker` - Marker

#### DevelopmentalStage
- `Initial` - Initial stage
- `CriticalPeriod` - Critical period
- `Maturation` - Maturation
- `Adult` - Adult stage
- `Aging` - Aging stage

#### FiringState
- `Resting` - Resting state
- `Active` - Active state
- `Refractory` - Refractory state
- `Inhibited` - Inhibited state

### Error Classes

- `NLMError` - Base NLM error
- `ConfigurationError` - Configuration error
- `SimulationError` - Simulation error
- `InitializationError` - Initialization error

## Performance Optimization

### Profiling Techniques

1. **Use benchmarking functions**
```python
results = pynlm.benchmarkStep(brain, 1000)
print(f"Performance: {results['steps_per_second']:.1f} steps/sec")
```

2. **Monitor memory usage**
```python
stats = pynlm.getPerformanceStats(brain)
print(f"Memory usage: {stats['memory_usage']}")
```

3. **Use batch operations**
```python
# Instead of individual updates, use batch operations
injections = [(neuron_id1, current1), (neuron_id2, current2)]
pynlm.batchInjectCurrent(brain, injections)
```

### Optimization Strategies

1. **Increase simulation timestep** for longer time scales
2. **Reduce sensory input frequency** for performance
3. **Use simpler world configurations** for faster processing
4. **Enable only necessary systems** (e.g., disable curiosity when not needed)
5. **Use larger brain sizes** for better computational efficiency
6. **Enable SIMD optimization** if compiler supports it
7. **Use shared memory** for multi-agent simulations

## Migration Guide

### From C++ to Python

If migrating from C++ to Python:

1. **Replace include statements**:
   - C++: `#include <brain/Brain.hpp>`
   - Python: `import pynlm`

2. **Replace object creation**:
   - C++: `Brain brain(config);`
   - Python: `brain = pynlm.createBrain(config)`

3. **Replace method calls**:
   - C++: `brain.step(step);`
   - Python: `brain.step(step)`

4. **Replace file operations**:
   - C++: `std::ofstream file("file.txt");`
   - Python: `import pynlm; pynlm.saveSimulationState(brain, world, agent, "prefix")`

### From Old to New API

#### Old Pattern
```python
# Old way (limited functionality)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# No convenience functions
# Limited error handling
# No helper classes
```

#### New Pattern
```python
# New way (full functionality)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrainWithConfig(config)  # Convenience function
brain.initialize()

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Get statistics
stats = pynlm.getBrainStats(brain)

# Error handling
if pynlm.validateBrainConfig(config):
    # Proceed with simulation
else:
    # Handle error
```

## Conclusion

The NLM Python bindings provide a comprehensive interface to the NLM neural simulation framework. The new API offers:

1. **Convenience functions** for common tasks
2. **Helper classes** for simulation management
3. **Comprehensive error handling** with informative exceptions
4. **Batch operations** for improved performance
5. **Extensive documentation** with examples
6. **Performance monitoring** and benchmarking tools
7. **File I/O utilities** for saving and loading simulations
8. **Development and learning features** for advanced use cases

The NLM Python bindings make it easy to:

- Create and run neural simulations
- Integrate brain-world interaction
- Implement learning algorithms
- Analyze brain activity
- Save and reproduce experiments
- Optimize performance for large-scale simulations

This comprehensive documentation should provide everything you need to effectively use the NLM Python bindings for your neural simulation projects.
