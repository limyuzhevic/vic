# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

---

---

## Part 4: Advanced Usage Patterns

### Advanced Agent Examples

```python
import pynlm

# Example 1: Curiosity-driven exploration with developmental learning

def curiosity_driven_exploration(world_size=20, max_steps=1000):
    """Demonstrates curiosity-driven exploration with developmental learning."""
    
    # Configure brain for development and curiosity
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.growth_rate", 0.01)
    config.set("brain.development.critical_period.enabled", True)
    
    # Create agent with development features
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        
        with pynlm.createAgentBrain(brain) as agent:
            # Enable advanced subsystems
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            agent.enableStructuralPlasticity(True)
            agent.enableRewardModulation(True)
            
            # Create and configure world
            with pynlm.createSimpleWorld() as world:
                world.configure(world_size, world_size, 8, 8)
                world.reset()
                world.setAgentStart(world_size/2, world_size/2)
                
                agent.initialize(world)
                
                # Track exploration metrics
                exploration_metrics = {
                    'steps': 0,
                    'novelty_sum': 0.0,
                    'curiosity_sum': 0.0,
                    'total_reward': 0.0,
                    'structural_plasticity_events': 0
                }
                
                # Run exploration episode
                for step in range(max_steps):
                    # Get sensory input and process it
                    percept = world.getSensoryPercept()
                    agent.processSensoryInput(percept)
                    
                    # Check for novel stimuli (curiosity driver)
                    novelty_level = agent.getNoveltyLevel()
                    curiosity_level = agent.getCuriosityLevel()
                    exploration_metrics['novelty_sum'] += novelty_level
                    exploration_metrics['curiosity_sum'] += curiosity_level
                    
                    # Brain processes and produces action
                    brain.step(step)
                    action = agent.decodeMotorCommand()
                    
                    # Execute action and get reward
                    result = world.applyMotorCommand(action, world.getSimulationTime())
                    if result.success:
                        exploration_metrics['total_reward'] += result.reward
                    
                    # Apply reward modulation
                    agent.applyRewardModulation(result.reward, 0.0)
                    
                    # Update development (brain matures)
                    agent.updateDevelopment(0.1)
                    
                    # Check for structural plasticity (brain grows)
                    if agent.isStructuralPlasticityEnabled():
                        # Track structural changes
                        if step % 100 == 0:
                            exploration_metrics['structural_plasticity_events'] += 1
                    
                    exploration_metrics['steps'] += 1
                    
                    # Print progress
                    if step % 100 == 0:
                        print(f"Step {step}:")
                        print(f"  Novelty: {novelty_level:.3f}")
                        print(f"  Curiosity: {curiosity_level:.3f}")
                        print(f"  Development Stage: {brain.getDevelopmentalStage()}")
                        print(f"  Total Neurons: {brain.getTotalNeuronCount()}")
                
                return exploration_metrics

# Example 2: Reward shaping with predictive coding

def reward_shaping_example():
    """Demonstrates advanced reward shaping using predictive coding."""
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        
        with pynlm.createAgentBrain(brain) as agent:
            # Enable reward modulation and development
            agent.enableRewardModulation(True)
            agent.enableDevelopment(True)
            
            with pynlm.createSimpleWorld() as world:
                world.configure(15, 15, 6, 6)
                world.reset()
                agent.initialize(world)
                
                # Predictive coding setup
                prediction_history = []
                reward_history = []
                prediction_errors = []
                
                for step in range(500):
                    # World update
                    world.update(0.1)
                    
                    # Get percept
                    percept = world.getSensoryPercept()
                    agent.processSensoryInput(percept)
                    
                    # Brain processes (predicts)
                    brain.step(step)
                    
                    # Get action
                    action = agent.decodeMotorCommand()
                    result = world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Advanced reward shaping using prediction errors
                    current_prediction = agent.getPredictionError()
                    actual_reward = result.reward if result.success else 0.0
                    
                    # Calculate prediction error for learning
                    prediction_error = abs(actual_reward - current_prediction)
                    prediction_errors.append(prediction_error)
                    
                    # Apply shaped reward
                    shaped_reward = current_prediction * 0.5 + actual_reward * 0.5
                    agent.applyRewardModulation(shaped_reward, current_prediction)
                    
                    # Update development based on prediction accuracy
                    agent.updateDevelopment(prediction_error * 0.1)
                    
                    prediction_history.append(current_prediction)
                    reward_history.append(actual_reward)
                
                # Analyze results
                avg_prediction_error = sum(prediction_errors) / len(prediction_errors)
                print(f"Average prediction error: {avg_prediction_error:.3f}")
                return prediction_history, reward_history, prediction_errors

# Example 3: Memory system usage and consolidation

def memory_system_example():
    """Demonstrates working memory, episodic memory, and memory consolidation."""
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        
        # Access memory systems
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        associative_memory = brain.getAssociativeMemory()
        
        with pynlm.createAgentBrain(brain) as agent:
            with pynlm.createSimpleWorld() as world:
                world.configure(10, 10, 4, 4)
                world.reset()
                agent.initialize(world)
                
                # Store memory patterns
                memory_patterns = []
                
                for episode in range(10):
                    # Run episode and capture experiences
                    percept = world.getSensoryPercept()
                    agent.processSensoryInput(percept)
                    brain.step(episode)
                    action = agent.decodeMotorCommand()
                    
                    # Store in working memory
                    working_memory.add(f"episode_{episode}_action", action)
                    
                    # Create episodic memory record
                    episodic_entry = {
                        'episode': episode,
                        'percept': percept,
                        'action': action,
                        'timestamp': world.getSimulationTime()
                    }
                    episodic_memory.store(episode, episodic_entry)
                    
                    # Build associative connections
                    if episode > 0:
                        prev_action = working_memory.get(f"episode_{episode-1}_action")
                        associative_memory.connect(prev_action, action, strength=0.1)
                    
                    memory_patterns.append(episode)
                    world.update(0.1)
                
                # Memory consolidation (replay and strengthen)
                print(f"Working memory capacity: {working_memory.getCapacity()}")
                print(f"Episodic memories stored: {episodic_memory.getCount()}")
                print(f"Associative connections: {associative_memory.getConnectionCount()}")
                
                return memory_patterns
```

### Helper Functions

```python
import pynlm

# Create helper functions for common operations

# 1. Run complete agent simulation
def run_agent_simulation(config, world_size=20, max_steps=1000):
    """High-level function to run a complete agent simulation."""
    
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        
        with pynlm.createAgentBrain(brain) as agent:
            # Enable common subsystems
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            
            with pynlm.createSimpleWorld() as world:
                world.configure(world_size, world_size, 8, 8)
                world.reset()
                world.setAgentStart(world_size/2, world_size/2)
                agent.initialize(world)
                
                # Run simulation
                for step in range(max_steps):
                    world.update(0.1)
                    agent.processSensoryInput(world.getSensoryPercept())
                    brain.step(step)
                    
                    action = agent.decodeMotorCommand()
                    result = world.applyMotorCommand(action, world.getSimulationTime())
                    
                    if result.success:
                        agent.applyRewardModulation(result.reward, 0.0)
                    
                    agent.updateDevelopment(0.1)
                    
                    # Log progress
                    if step % 100 == 0:
                        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
                
                # Return final metrics
                return {
                    'total_spikes': brain.getTotalSpikeCount(),
                    'firing_neurons': brain.getFiringNeuronCount(),
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel(),
                    'development': brain.getDevelopmentalStage()
                }

# 2. Create standardized sensory inputs
def create_standard_percept(vision_data, internal_signals):
    """Create a standard sensory percept with vision and internal signals."""
    
    # Create vision input
    vision = pynlm.create_vision_input(8, 8, 3)
    vision.setData(vision_data)
    
    # Create internal signals
    internal = pynlm.create_internal_signals()
    for signal in internal_signals:
        internal.addSignal(signal)
    
    # Create percept
    percept = pynlm.SensoryPercept()
    percept.setVision(vision)
    percept.setInternal(internal)
    
    return percept

# 3. Run episode with metrics
def run_monitored_episode(brain, agent, world, max_steps=100):
    """Run a single episode with monitoring and metrics collection."""
    
    if not brain.getConfig() or not brain.getTotalNeuronCount():
        brain.initialize()
    
    world.reset()
    agent.reset()
    agent.initialize(world)
    
    metrics = {
        'steps': 0,
        'total_reward': 0.0,
        'total_spikes': 0,
        'avg_prediction_error': 0.0,
        'neuromodulation': 0.0
    }
    
    for step in range(max_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        if result.success:
            total_reward += result.reward
        
        agent.applyRewardModulation(result.reward, 0.0)
        agent.updateDevelopment(0.1)
        
        metrics['total_reward'] += result.reward
        metrics['total_spikes'] += brain.getTotalSpikeCount()
        metrics['avg_prediction_error'] += agent.getPredictionError()
        metrics['neuromodulation'] += agent.getNeuromodulationLevel()
    
    # Calculate averages
    metrics['avg_prediction_error'] /= max_steps
    metrics['steps'] = max_steps
    
    return metrics

# 4. Create complex world setups
def create_deterministic_world(width, height, vision_width, vision_height, seed=42):
    """Create a deterministic world with fixed random seed."""
    
    world = pynlm.createSimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    world.setRandomSeed(seed)
    
    # Add resources at specific positions
    resource_positions = [(2, 2), (width-3, 2), (2, height-3), (width-3, height-3)]
    for x, y in resource_positions:
        world.addObject(pynlm.create_world_object(x, y, pynlm.WorldObjectType.Resource, value=10.0, radius=0.5))
    
    # Add walls
    for x in range(width):
        world.addObject(pynlm.create_world_object(x, 0, pynlm.WorldObjectType.Wall, value=1.0, radius=0.5))
        world.addObject(pynlm.create_world_object(x, height-1, pynlm.WorldObjectType.Wall, value=1.0, radius=0.5))
    
    for y in range(height):
        world.addObject(pynlm.create_world_object(0, y, pynlm.WorldObjectType.Wall, value=1.0, radius=0.5))
        world.addObject(pynlm.create_world_object(width-1, y, pynlm.WorldObjectType.Wall, value=1.0, radius=0.5))
    
    return world
```

---

## Part 5: Advanced Examples

### Example 1: Curiosity-Driven Exploration

```python
import pynlm
import time

def curiosity_driven_exploration():
    """Comprehensive curiosity-driven exploration example."""
    
    # Create brain optimized for curiosity
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("plasticity.structural.growth_rate", 0.02)
    config.set("brain.development.critical_period.enabled", True)
    
    # Run with context managers for resource management
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        
        with pynlm.createAgentBrain(brain) as agent:
            # Enable all relevant subsystems
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            agent.enableStructuralPlasticity(True)
            agent.enableRewardModulation(True)
            
            with pynlm.createSimpleWorld() as world:
                world.configure(25, 25, 10, 10)
                world.reset()
                world.setAgentStart(12.5, 12.5)
                agent.initialize(world)
                
                # Exploration metrics
                novelty_history = []
                curiosity_history = []
                reward_history = []
                neuron_count_history = []
                
                start_time = time.time()
                
                for step in range(1000):
                    # Get world state
                    world.update(0.1)
                    percept = world.getSensoryPercept()
                    
                    # Process and learn
                    agent.processSensoryInput(percept)
                    brain.step(step)
                    
                    # Get metrics before action
                    novelty = agent.getNoveltyLevel()
                    curiosity = agent.getCuriosityLevel()
                    novelty_history.append(novelty)
                    curiosity_history.append(curiosity)
                    
                    # Take action
                    action = agent.decodeMotorCommand()
                    result = world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Reward and learn
                    reward = result.reward if result.success else 0.0
                    reward_history.append(reward)
                    agent.applyRewardModulation(reward, 0.0)
                    
                    # Development (brain matures)
                    agent.updateDevelopment(0.1)
                    
                    # Track neural growth
                    neuron_count_history.append(brain.getTotalNeuronCount())
                    
                    # Print progress
                    if step % 100 == 0:
                        elapsed = time.time() - start_time
                        print(f"Step {step} ({elapsed:.1f}s):")
                        print(f"  Novelty: {novelty:.3f}, Curiosity: {curiosity:.3f}")
                        print(f"  Reward: {reward:.3f}, Neurons: {neuron_count_history[-1]}")
                        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
                
                # Analysis
                print("\nExploration Analysis:")
                print(f"Average novelty: {sum(novelty_history)/len(novelty_history):.3f}")
                print(f"Average curiosity: {sum(curiosity_history)/len(curiosity_history):.3f}")
                print(f"Total reward: {sum(reward_history):.3f}")
                print(f"Neuron growth: {neuron_count_history[-1] - neuron_count_history[0]}")
                
                return novelty_history, curiosity_history, reward_history, neuron_count_history

# Run the example
curiosity_driven_exploration()
```

### Example 2: Reward Shaping with Predictive Coding

```python
import pynlm

def reward_shaping_with_predictive_coding():
    """Demonstrates advanced reward shaping using predictive coding."""
    
    # Configure brain for predictive coding
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        
        with pynlm.createAgentBrain(brain) as agent:
            # Enable reward modulation
            agent.enableRewardModulation(True)
            
            with pynlm.createSimpleWorld() as world:
                world.configure(20, 20, 8, 8)
                world.reset()
                agent.initialize(world)
                
                # Track predictions and rewards
                predictions = []
                rewards = []
                prediction_errors = []
                neuromodulation = []
                
                for step in range(800):
                    # World dynamics
                    world.update(0.1)
                    percept = world.getSensoryPercept()
                    agent.processSensoryInput(percept)
                    brain.step(step)
                    
                    # Get prediction and action
                    prediction = agent.getPredictionError()
                    action = agent.decodeMotorCommand()
                    result = world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Actual reward
                    reward = result.reward if result.success else 0.0
                    
                    # Predictive coding reward shaping
                    # Combine prediction error with actual reward
                    prediction_error = abs(reward - prediction)
                    shaped_reward = reward * 0.7 + prediction * 0.3
                    
                    # Apply shaped reward
                    agent.applyRewardModulation(shaped_reward, prediction)
                    
                    # Update development based on prediction accuracy
                    agent.updateDevelopment(prediction_error * 0.05)
                    
                    # Store metrics
                    predictions.append(prediction)
                    rewards.append(reward)
                    prediction_errors.append(prediction_error)
                    neuromodulation.append(agent.getNeuromodulationLevel())
                    
                    if step % 100 == 0:
                        print(f"Step {step}:")
                        print(f"  Prediction error: {prediction:.3f}")
                        print(f"  Actual reward: {reward:.3f}")
                        print(f"  Shaped reward: {shaped_reward:.3f}")
                        print(f"  Neuromodulation: {neuromodulation[-1]:.3f}")
                
                # Analysis
                print("\nPredictive Coding Analysis:")
                avg_prediction_error = sum(prediction_errors) / len(prediction_errors)
                print(f"Average prediction error: {avg_prediction_error:.3f}")
                print(f"Prediction-reward correlation: {calculate_correlation(predictions, rewards):.3f}")
                
                return predictions, rewards, prediction_errors, neuromodulation

def calculate_correlation(preds, rewards):
    """Calculate correlation between predictions and rewards."""
    if len(preds) != len(rewards):
        return 0.0
    
    mean_pred = sum(preds) / len(preds)
    mean_reward = sum(rewards) / len(rewards)
    
    covariance = sum((p - mean_pred) * (r - mean_reward) for p, r in zip(preds, rewards))
    variance_pred = sum((p - mean_pred) ** 2 for p in preds)
    variance_reward = sum((r - mean_reward) ** 2 for r in rewards)
    
    if variance_pred == 0 or variance_reward == 0:
        return 0.0
    
    return covariance / ((variance_pred * variance_reward) ** 0.5)

# Run the example
reward_shaping_with_predictive_coding()
```

### Example 3: Memory System Integration

```python
import pynlm

def memory_system_integration():
    """Demonstrates integration of all memory systems."""
    
    # Create brain with enhanced memory capacity
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        
        # Access all memory systems
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        associative_memory = brain.getAssociativeMemory()
        
        with pynlm.createAgentBrain(brain) as agent:
            with pynlm.createSimpleWorld() as world:
                world.configure(15, 15, 6, 6)
                world.reset()
                agent.initialize(world)
                
                # Memory consolidation cycle
                consolidation_cycles = []
                
                for cycle in range(20):
                    cycle_metrics = {
                        'working_memory_used': 0,
                        'episodic_memories': 0,
                        'associative_connections': 0,
                        'consolidation_efficiency': 0.0
                    }
                    
                    # Episode experience
                    percept = world.getSensoryPercept()
                    agent.processSensoryInput(percept)
                    brain.step(cycle)
                    
                    # Store in working memory
                    working_memory.add(f"perception_{cycle}", percept)
                    cycle_metrics['working_memory_used'] = working_memory.getUsage()
                    
                    # Create episodic memory
                    episodic_entry = {
                        'cycle': cycle,
                        'percept_data': percept.getAllSignals(),
                        'timestamp': world.getSimulationTime(),
                        'importance': calculate_importance(percept)
                    }
                    episodic_memory.store(cycle, episodic_entry)
                    cycle_metrics['episodic_memories'] = episodic_memory.getCount()
                    
                    # Build associative connections
                    if cycle > 0:
                        prev_percept = working_memory.get(f"perception_{cycle-1}")
                        if prev_percept:
                            associative_memory.connect(prev_percept, percept, strength=0.05)
                    
                    cycle_metrics['associative_connections'] = associative_memory.getConnectionCount()
                    
                    # Consolidation phase
                    consolidated = episodic_memory.consolidate()
                    cycle_metrics['consolidation_efficiency'] = consolidated / episodic_memory.getCount()
                    
                    # Retrieve consolidated memories
                    if cycle % 5 == 0:
                        retrieved = episodic_memory.retrieve_recent(5)
                        print(f"Cycle {cycle}: Retrieved {len(retrieved)} episodic memories")
                    
                    consolidation_cycles.append(cycle_metrics)
                    world.update(0.1)
                
                # Memory system analysis
                print("\nMemory System Analysis:")
                avg_working_memory = sum(c['working_memory_used'] for c in consolidation_cycles) / len(consolidation_cycles)
                avg_consolidation = sum(c['consolidation_efficiency'] for c in consolidation_cycles) / len(consolidation_cycles)
                
                print(f"Average working memory usage: {avg_working_memory:.1f}")
                print(f"Average consolidation efficiency: {avg_consolidation:.3f}")
                print(f"Total associative connections: {associative_memory.getConnectionCount()}")
                print(f"Total episodic memories: {episodic_memory.getCount()}")
                
                return consolidation_cycles

def calculate_importance(percept):
    """Calculate importance of a percept for memory storage."""
    
    # Combine novelty and neuromodulation level
    novelty = percept.getNoveltyLevel() if hasattr(percept, 'getNoveltyLevel') else 0.0
    internal_signals = percept.getInternal()
    neuromodulation = sum(internal_signals.getAllSignals()) if internal_signals else 0.0
    
    return (novelty + neuromodulation) / 2.0

# Run the example
memory_system_integration()
```

### Example 4: Performance Benchmarking

```python
import pynlm
import time
import psutil
import os

def performance_benchmark():
    """Comprehensive performance benchmarking of NLM."""
    
    process = psutil.Process(os.getPID())
    
    # Test different brain sizes
    brain_sizes = [500, 1000, 2000, 5000]
    results = []
    
    for size in brain_sizes:
        print(f"\nBenchmarking with {size} neurons...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", size)
        
        start_time = time.time()
        start_memory = process.memory_info().rss / 1024 / 1024
        
        with pynlm.createBrain(config) as brain:
            brain.initialize()
            
            # Run simulation
            for step in range(100):
                brain.step(step)
                if step % 20 == 0:
                    # Get various metrics
                    firing_rate = brain.getAverageFiringRate()
                    spike_count = brain.getTotalSpikeCount()
                    neuron_count = brain.getTotalNeuronCount()
                    
                    if step == 0:
                        print(f"  Initial state: {neuron_count} neurons")
                    
                    print(f"  Step {step}: {firing_rate:.2f} Hz, {spike_count} spikes")
        
        end_time = time.time()
        end_memory = process.memory_info().rss / 1024 / 1024
        
        elapsed = end_time - start_time
        memory_used = end_memory - start_memory
        
        result = {
            'size': size,
            'elapsed_time': elapsed,
            'memory_used': memory_used,
            'spikes_per_second': brain.getTotalSpikeCount() / elapsed,
            'neurons_per_second': size / elapsed
        }
        
        results.append(result)
        print(f"  Results: {elapsed:.2f}s, {memory_used:.2f}MB")
        print(f"  Performance: {result['spikes_per_second']:.0f} spikes/s")
    
    # Compare results
    print("\nPerformance Comparison:")
    print(f"{'Neurons':<10} {'Time (s)':<10} {'Memory (MB)':<12} {'Spikes/s':<12} {'Neurons/s':<12}")
    print("-" * 60)
    for result in results:
        print(f"{result['size']:<10} {result['elapsed_time']:<10.2f} {result['memory_used']:<12.2f} "
              f"{result['spikes_per_second']:<12.0f} {result['neurons_per_second']:<12.0f}")
    
    return results

# Run benchmark
performance_benchmark()
```

---

## Part 5: Example Scripts

### Example 1: Minimal Brain Simulation

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Example 2: Environment Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions


#### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Context manager usage (recommended)
with pynlm.createBrain(config) as brain:
    brain.initialize()
    # Your code here
    brain.step(step)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action
brain.injectCurrent(neuron, current)       # Inject current into specific neuron
brain.injectCurrentToNeurons(type, current)  # Inject current to all neurons of type

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getActiveNeuronCount()      # Currently active neurons
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
brain.getTotalSpikeCount()        # Total spikes
brain.getPendingSpikeEventCount() # Pending spike events
brain.getDevelopmentalStage()    # Current developmental stage

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Memory systems
brain.getWorkingMemory()           # Get working memory
brain.getEpisodicMemory()          # Get episodic memory
brain.getAssociativeMemory()       # Get associative memory
brain.getPredictionSystem()        # Get prediction system
brain.getPlanner()                 # Get planner
brain.getConceptFormation()        # Get concept formation
brain.getAttention()               # Get attention system
brain.getDevelopmentSystem()       # Get development system
brain.getDopamine()                # Get dopamine system
brain.getCuriosity()               # Get curiosity system
brain.getNovelty()                 # Get novelty detection
brain.getPredictionErrorSignal()   # Get prediction error signal
brain.getRandomGenerator()         # Get random generator

# Other
brain.logStatus()                  # Log brain status
brain.addRegion(name)              # Add brain region
brain.getRegion(id)                # Get region by ID
brain.getRegionCount()             # Get number of regions
brain.getRegionIds()               # Get all region IDs
brain.getRegions()                 # Get all regions
```

#### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query
config.has("brain.neuron_count")   # Check if key exists
config.getKeys()                   # Get all configuration keys

# Modify (type-specific setters)
config.set("brain.neuron_count", 1000)           # Integer
config.set("learning.rate", 0.001)              # Float
config.set("neuromod.curiosity.enable", True)   # Boolean
config.set("synapse.density", "0.1")            # String

# File I/O
config.loadFromFile("config.json")   # Load from JSON
config.saveToFile("config.json")     # Save to JSON

# Get values (with defaults)
config.get("brain.neuron_count")                 # Integer
config.get("brain.synapse_density", 0.1)         # Float with default
config.get("neuromod.curiosity.enable", False)  # Boolean with default
config.get("name", "default")                   # String with default
```

#### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Context manager usage (recommended)
with pynlm.createAgentBrain(brain) as agent:
    agent.initialize(world)
    # Your code here

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()

# State
agent.reset()
agent.getBrain()                   # Get underlying brain
```

#### SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Context manager usage (recommended)
with pynlm.createSimpleWorld() as world:
    world.configure(...)
    world.reset()
    # Your code here

# Configuration
world.configure(width, height, visionWidth, visionHeight)
world.initialize(width=20, height=20, visionWidth=16, visionHeight=16, maxEnergy=100.0)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(world_object)      # Add world object
world.removeObject(x, y)           # Remove object at position
world.isValidPosition(x, y)        # Check if position is valid

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
world.getEnergyDecayRate()
world.setEnergyDecayRate(0.01)
world.getRandomSeed()
world.setRandomSeed(seed)
```

### Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

---

## Part 4: Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

---

## Part 5: Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
