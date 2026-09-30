# NLM (熙然) - Neural Learning Machine
# Phase 6 Complete - Implementation Guide

**NLM is a complete experimental computational brain project with comprehensive Python bindings, command-line tools, and advanced features.**

## Overview

NLM (熙然, meaning "serene flow") is a sophisticated brain-inspired neural network simulator that implements **Phase 6: Final Integration**. All cognitive systems work together as a coherent artificial brain system capable of experience-driven learning and development.

### Key Features:

#### Python API (pynlm)
- ✅ Complete Python bindings with C++ extension
- ✅ High-level interface for brain simulation
- ✅ Agent-world interaction framework
- ✅ Advanced configuration and control

#### Command-Line Tools
- ✅ `nlm` - Basic simulation
- ✅ `nlm_phase3_demo` - Phase 3 experiments
- ✅ `nlm_phase4_demo` - Phase 4 cognitive experiments
- ✅ `nlm_phase6_demo` - Phase 6 integration test
- ✅ `nlm_test` - Unit test suite

#### Advanced Features
- ✅ Memory systems (working, episodic, associative)
- ✅ Neuromodulation (dopamine, curiosity, novelty, prediction error)
- ✅ Cognitive functions (attention, planning, concept formation, self-model)
- ✅ Prediction systems (temporal sequence learning)
- ✅ Development and maturation
- ✅ Checkpoint persistence
- ✅ Replay and consolidation
- ✅ Performance optimization

## Building

### Prerequisites
```bash
# C++ build dependencies
apt-get install build-essential cmake g++

# Python build dependencies
pip install pybind11 scikit-build-core pytest numpy
```

### Build Instructions
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake (Release for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets (use all available cores)
make -j$(nproc)

# Install Python package
pip install .

# Or install in development mode
pip install -e .
```

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

## Installation

After building, the Python package will be installed in your Python environment:

```python
>>> import pynlm
>>> print(pynlm.__version__)
0.1.0
```

## Python API Usage

### Basic Usage

```python
import pynlm

# Create and initialize a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("Simulation complete!")
```

### Agent-World Simulation

```python
def run_complete_simulation(num_steps=1000):
    """Run a complete agent simulation with all subsystems."""
    
    # 1. Create configuration with optimized settings
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)  # Larger brain for complex tasks
    config.set("brain.synapse_density", 0.08)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.curiosity.enable", True)
    
    # 2. Create and initialize the brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=32, height=32, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(16.0, 16.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all learning subsystems
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world environment
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step (includes all cognitive processing)
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = (percept.getInternal()[0] if percept.getInternal() else 0.0) * 0.01
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Reward: {reward:.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            
            # Memory system statistics
            if brain.getWorkingMemory():
                print(f"  Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
            if brain.getEpisodicMemory():
                print(f"  Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
    
    # 8. Return final statistics
    return {
        'total_reward': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'episodes_stored': (brain.getEpisodicMemory().getEpisodeCount() 
                           if brain.getEpisodicMemory() else 0),
        'curiosity_level': agent.getCuriosityLevel(),
        'development_stage': brain.getDevelopmentalStage()
    }

# Run the simulation
results = run_complete_simulation(2000)
print("\n=== SIMULATION RESULTS ===")
for key, value in results.items():
    print(f"{key}: {value}")
```

### Advanced Configuration

```python
import pynlm
import json

# Create configuration from JSON
config_json = {
    "brain": {
        "neuron_count": 5000,
        "synapse_density": 0.1,
        "connection_probability": 0.05,
        "initial_weight_mean": 0.6,
        "initial_weight_std": 0.2
    },
    "plasticity": {
        "stdp": {
            "enable": True,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": True,
            "learning_rate": 0.0005
        },
        "structural": {
            "enable": True,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.5
        },
        "curiosity": {
            "enable": True,
            "threshold": 0.3
        },
        "novelty": {
            "enable": True,
            "decay_rate": 0.95
        }
    }
}

# Create configuration from JSON
config = pynlm.createConfigFromJSON(json.dumps(config_json))

# Or create default and modify
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.learning_rate", 0.001)
```

### Command Line Usage

#### Basic Simulation
```bash
# Run basic simulation (phase 2)
./nlm --neuron-count 1000 --steps 500

# With custom configuration file
./nlm --config my_config.cfg
```

#### Phase 3 Experiments
```bash
# Phase 3 world interaction demo
./nlm_phase3_demo --width 20 --height 20 --duration 100
```

#### Phase 4 Cognitive Experiments
```bash
# Phase 4 cognitive function demo
./nlm_phase4_demo --neurons 2000 --experiments memory_planning
```

#### Phase 6 Integration Test
```bash
# Phase 6 comprehensive integration test
./nlm_phase6_demo --neurons 2000 --steps 2000 --checkpoint

# With detailed logging
./nlm_phase6_demo --verbose --log-level INFO
```

#### Unit Tests
```bash
# Run all unit tests
./nlm_test

# Run specific test suites
./nlm_test --suite memory
./nlm_test --suite neuromodulation
./nlm_test --suite cognition
```

#### Advanced Features
```bash
# Multi-agent simulation
./nlm --multi-agent --agent-count 5 --world-size 50

# Development experiments
./nlm --development --stages 3 --plasticity

# Performance benchmarking
./nlm --benchmark --profile cpu --iterations 10

# Visualization
./nlm --visualize --format png --output brain_simulation.png

# Replay experiments
./nlm --replay --checkpoint-file brain_state.bin --speed 2.0
```

### Configuration Management

```python
import pynlm
import json

# Save configuration to file
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.saveToFile("my_config.json")

# Load configuration from file
loaded_config = pynlm.createConfigFromFile("my_config.json")

# Get configuration parameters
if loaded_config.has("brain.neuron_count"):
    neurons = loaded_config.get("brain.neuron_count")
    
if loaded_config.has("plasticity.stdp.enable"):
    stdp_enabled = loaded_config.get("plasticity.stdp.enable")

# List all configuration keys
keys = loaded_config.getKeys()
for key in keys:
    print(f"Key: {key}, Value: {loaded_config.get(key)}")
```

### Enumerations

#### Neuron Types
```python
import pynlm

# Neuron type constants
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal
```

#### Developmental Stages
```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

#### Action Types
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.LookLeft
pynlm.ActionType.LookRight
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Wait
```

#### World Object Types
```python
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

## Quick Reference

### Core Classes

#### Brain Class
```python
# Creation and initialization
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation
brain.step(step_number)           # With timestep
brain.step(step_number, time)     # With explicit timestamp
brain.reset()                     # Reset state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()            # Get motor action

# Statistics
brain.getTotalNeuronCount()      # Total neurons
brain.getTotalSynapseCount()     # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")      # Save state
brain.load("checkpoint.bin")      # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class
```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class
```python
# Creation
agent = pynlm.createAgentBrain(brain)

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
```

#### SimpleWorld Class
```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

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
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

## Advanced Features

### Multi-Agent Simulation

```python
import pynlm

def run_multi_agent_simulation(num_agents=3, num_steps=1000):
    """Run simulation with multiple competing agents."""
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create multiple agents
    agents = []
    brains = []
    
    for i in range(num_agents):
        # Each agent gets its own brain
        config = pynlm.createDefaultConfig()
        config.set("random_seed", 42 + i)  # Different seed for each agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        agents.append(agent)
        brains.append(brain)
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        
        # Each agent perceives the world
        percept = world.getSensoryPercept()
        
        for i, agent in enumerate(agents):
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain step
            brains[i].step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Agent-specific movement (avoid collisions)
            if i < num_agents - 1:
                # Move each agent differently
                world.applyMotorCommand(action, world.getSimulationTime())
    
    # Compare agent performance
    results = []
    for i, agent in enumerate(agents):
        results.append({
            'agent_id': i,
            'total_reward': brains[i].getTotalSpikeCount(),
            'firing_rate': brains[i].getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel()
        })
    
    return results

# Run multi-agent simulation
results = run_multi_agent_simulation(3, 500)
print("Multi-agent results:")
for result in results:
    print(f"Agent {result['agent_id']}: Reward={result['total_reward']}, "
          f"Firing={result['firing_rate']:.2f}, "
          f"Curiosity={result['curiosity']:.3f}")
```

### Development Experiments

```python
import pynlm

def run_development_experiment(num_steps=2000):
    """Test brain development and maturation."""
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Track development stages
    stage_changes = []
    plasticity_changes = []
    
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record development state
        current_stage = brain.getDevelopmentalStage()
        current_plasticity = agent.getNeuromodulationLevel()
        
        if step == 0 or current_stage != stage_changes[-1][0]:
            stage_changes.append((current_stage, step))
        
        if step % 100 == 0:
            plasticity_changes.append((step, current_plasticity))
    
    return {
        'stage_transitions': stage_changes,
        'plasticity_history': plasticity_changes,
        'final_stage': brain.getDevelopmentalStage(),
        'total_spikes': brain.getTotalSpikeCount(),
        'final_firing_rate': brain.getAverageFiringRate()
    }

# Run development experiment
development_results = run_development_experiment(2000)
print("Development experiment results:")
print(f"Final developmental stage: {development_results['final_stage']}")
print(f"Stage transitions: {len(development_results['stage_transitions'])}")
print(f"Final firing rate: {development_results['final_firing_rate']:.2f} Hz")
```

### Performance Benchmarking

```python
import pynlm
import time
import psutil
import os

def benchmark_performance():
    """Benchmark brain performance with different configurations."""
    
    test_configs = [
        {"neurons": 500, "steps": 1000},
        {"neurons": 1000, "steps": 2000},
        {"neurons": 2000, "steps": 3000},
        {"neurons": 5000, "steps": 5000}
    ]
    
    results = []
    
    for config in test_configs:
        print(f"Benchmarking {config['neurons']} neurons, {config['steps']} steps...")
        
        # Get current process for memory tracking
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Run simulation
        brain_config = pynlm.createDefaultConfig()
        brain_config.set("brain.neuron_count", config['neurons'])
        
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        
        start_time = time.time()
        
        for step in range(config['steps']):
            brain.step(step)
        
        end_time = time.time()
        
        final_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        simulation_time = end_time - start_time
        steps_per_second = config['steps'] / simulation_time
        neurons_per_second = config['neurons'] / simulation_time
        memory_usage = final_memory - initial_memory
        
        results.append({
            'neurons': config['neurons'],
            'steps': config['steps'],
            'simulation_time': simulation_time,
            'steps_per_second': steps_per_second,
            'neurons_per_second': neurons_per_second,
            'memory_usage_mb': memory_usage,
            'avg_firing_rate': brain.getAverageFiringRate()
        })
        
        print(f"  Time: {simulation_time:.2f}s")
        print(f"  Speed: {steps_per_second:.0f} steps/sec")
        print(f"  Memory: {memory_usage:.1f} MB")
    
    return results

# Run benchmark
benchmark_results = benchmark_performance()
print("\nBenchmark Summary:")
print("Neurons | Steps | Time(s) | Steps/s | Neurons/s | Memory(MB) | Firing Rate")
print("-" * 80)
for result in benchmark_results:
    print(f"{result['neurons']:7d} | {result['steps']:5d} | "
          f"{result['simulation_time']:6.2f} | "
          f"{result['steps_per_second']:8.0f} | "
          f"{result['neurons_per_second']:10.0f} | "
          f"{result['memory_usage_mb']:8.1f} | "
          f"{result['avg_firing_rate']:10.2f}")
```

### Machine Learning Integration

```python
import pynlm
import numpy as np
from sklearn.preprocessing import StandardScaler

def integrate_with_ml(data_samples=1000, prediction_horizon=10):
    """Use NLM for sequence prediction and integrate with ML models."""
    
    # Prepare training data
    training_sequences = []
    target_sequences = []
    
    for _ in range(data_samples):
        # Generate random neural activity pattern
        pattern_length = prediction_horizon + 5
        pattern = np.random.randn(pattern_length) * 0.1
        
        # Add some correlations to simulate temporal structure
        for i in range(1, pattern_length):
            pattern[i] += pattern[i-1] * 0.3 + np.random.randn() * 0.05
        
        # Create input and target
        input_seq = pattern[:prediction_horizon]
        target_seq = pattern[prediction_horizon:]
        
        training_sequences.append(input_seq)
        target_sequences.append(target_seq)
    
    # Scale data
    scaler = StandardScaler()
    training_sequences = scaler.fit_transform(training_sequences)
    
    # Train NLM on sequences
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Train with sequence data
    for step in range(len(training_sequences)):
        # Create sensory input from sequence
        sensory_input = pynlm.createSensoryInput(training_sequences[step])
        
        # Process sequence in brain
        brain.receiveSensoryInput(sensory_input)
        brain.step(step)
        
        # Get predictions
        if brain.getPredictionSystem():
            prediction = brain.getPredictionSystem().getCurrentPrediction()
            target = target_sequences[step][0]
            
            # Compute prediction error
            error = abs(prediction - target)
            
            # Store for later analysis
            if step % 100 == 0:
                print(f"Step {step}: Prediction={prediction:.4f}, Target={target:.4f}, Error={error:.4f}")
    
    # Evaluate on test data
    test_sequences = np.random.randn(10, prediction_horizon) * 0.1
    test_sequences = scaler.transform(test_sequences)
    
    predictions = []
    targets = []
    
    for seq in test_sequences:
        sensory_input = pynlm.createSensoryInput(seq)
        brain.receiveSensoryInput(sensory_input)
        brain.step(0)  # Use same step for all test sequences
        
        if brain.getPredictionSystem():
            predictions.append(brain.getPredictionSystem().getCurrentPrediction())
    
    return {
        'training_loss': np.mean([abs(p - t) for p, t in zip(predictions, target_sequences.flatten())]),
        'predictions': predictions,
        'targets': target_sequences.flatten(),
        'prediction_accuracy': 1.0 - np.mean(np.abs(np.array(predictions) - np.array(target_sequences.flatten())))
    }

# Integrate with ML
ml_results = integrate_with_ml(data_samples=500, prediction_horizon=5)
print(f"ML Integration Results:")
print(f"Prediction Accuracy: {ml_results['prediction_accuracy']:.3f}")
print(f"Training Loss: {ml_results['training_loss']:.4f}")
```

## Experiment Framework

### Phase 6 Integration Experiment

```python
import pynlm
import json

class NLMExperimentRunner:
    """Run comprehensive NLM experiments with detailed analysis."""
    
    def __init__(self, config_file=None):
        self.config = pynlm.createDefaultConfig()
        if config_file:
            self.config.loadFromFile(config_file)
    
    def run_phase6_experiment(self, neurons=2000, steps=2000, enable_checkpointing=True):
        """Run complete Phase 6 integration experiment."""
        
        # Create brain with specified configuration
        brain_config = pynlm.createDefaultConfig()
        brain_config.set("brain.neuron_count", neurons)
        brain_config.set("brain.synapse_density", 0.08)
        brain_config.set("plasticity.stdp.enable", True)
        brain_config.set("plasticity.hebbian.enable", True)
        brain_config.set("plasticity.structural.enable", True)
        brain_config.set("neuromod.dopamine.scale", 1.2)
        brain_config.set("neuromod.curiosity.enable", True)
        
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        
        # Create environment
        world = pynlm.createSimpleWorld()
        world.configure(width=32, height=32, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(16.0, 16.0)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation with detailed logging
        experiment_data = {
            'steps': [],
            'rewards': [],
            'firing_rates': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'development_stages': [],
            'working_memory_traces': [],
            'episodic_episodes': []
        }
        
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = (percept.getInternal()[0] if percept.getInternal() else 0.0) * 0.01
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Record experiment data
            if step % 10 == 0:  # Sample every 10 steps to reduce data size
                experiment_data['steps'].append(step)
                experiment_data['rewards'].append(reward)
                experiment_data['firing_rates'].append(brain.getAverageFiringRate())
                experiment_data['curiosity_levels'].append(agent.getCuriosityLevel())
                experiment_data['novelty_levels'].append(agent.getNoveltyLevel())
                experiment_data['development_stages'].append(brain.getDevelopmentalStage())
                experiment_data['working_memory_traces'].append(
                    brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0
                )
                experiment_data['episodic_episodes'].append(
                    brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0
                )
        
        # Test checkpointing if enabled
        if enable_checkpointing:
            checkpoint_file = "nlm_checkpoint.bin"
            if brain.save(checkpoint_file):
                print(f"Checkpoint saved to {checkpoint_file}")
                
                # Create new brain and load checkpoint
                brain2 = pynlm.createBrain(brain_config)
                brain2.initialize()
                
                if brain2.load(checkpoint_file):
                    print("Checkpoint loaded successfully")
                    # Compare brain states
                    print(f"Original brain neurons: {brain.getTotalNeuronCount()}")
                    print(f"Loaded brain neurons: {brain2.getTotalNeuronCount()}")
                    print(f"Neuron count match: {brain.getTotalNeuronCount() == brain2.getTotalNeuronCount()}")
                else:
                    print("ERROR: Failed to load checkpoint")
            else:
                print("ERROR: Failed to save checkpoint")
        
        return experiment_data
    
    def analyze_results(self, experiment_data):
        """Analyze experiment results and generate insights."""
        
        analysis = {
            'summary': {},
            'memory_analysis': {},
            'learning_curves': {},
            'integration_score': 0.0
        }
        
        # Basic statistics
        analysis['summary'] = {
            'total_steps': len(experiment_data['steps']),
            'avg_reward': np.mean(experiment_data['rewards']),
            'max_reward': np.max(experiment_data['rewards']),
            'avg_firing_rate': np.mean(experiment_data['firing_rates']),
            'final_curiosity': experiment_data['curiosity_levels'][-1],
            'final_development_stage': experiment_data['development_stages'][-1]
        }
        
        # Memory analysis
        analysis['memory_analysis'] = {
            'working_memory_trend': self._calculate_trend(experiment_data['working_memory_traces']),
            'episodic_memory_growth': experiment_data['episodic_episodes'][-1] - experiment_data['episodic_episodes'][0],
            'memory_capacity_utilization': min(experiment_data['episodic_episodes'][-1] / 100.0, 1.0)
        }
        
        # Learning curves
        analysis['learning_curves'] = {
            'reward_stability': self._calculate_stability(experiment_data['rewards']),
            'firing_rate_stability': self._calculate_stability(experiment_data['firing_rates']),
            'curiosity_dynamics': self._calculate_curiosity_dynamics(experiment_data['curiosity_levels'])
        }
        
        # Integration score (0-1, higher is better)
        integration_score = 0.0
        if experiment_data['working_memory_traces'][-1] > experiment_data['working_memory_traces'][0]:
            integration_score += 0.25
        if experiment_data['episodic_episodes'][-1] > experiment_data['episodic_episodes'][0]:
            integration_score += 0.25
        if experiment_data['curiosity_levels'][-1] > 0.5:
            integration_score += 0.25
        if experiment_data['development_stages'][-1] != experiment_data['development_stages'][0]:
            integration_score += 0.25
        
        analysis['integration_score'] = integration_score
        
        return analysis
    
    def _calculate_trend(self, values):
        """Calculate trend (positive/negative/zero) for a series of values."""
        if len(values) < 2:
            return "stable"
        
        x = np.arange(len(values))
        y = np.array(values)
        
        # Simple linear regression
        slope = np.polyfit(x, y, 1)[0]
        
        if slope > 0.01:
            return "increasing"
        elif slope < -0.01:
            return "decreasing"
        else:
            return "stable"
    
    def _calculate_stability(self, values):
        """Calculate stability of a series of values (higher = more stable)."""
        if len(values) < 2:
            return 1.0
        
        return 1.0 / (1.0 + np.std(values))
    
    def _calculate_curiosity_dynamics(self, curiosity_values):
        """Analyze curiosity dynamics over time."""
        if len(curiosity_values) < 2:
            return "unknown"
        
        # Check if curiosity peaked and then decreased (exploration -> exploitation)
        max_idx = np.argmax(curiosity_values)
        if max_idx > 0 and max_idx < len(curiosity_values) - 1:
            return "exploration_then_exploitation"
        elif curiosity_values[-1] > 0.7:
            return "high_curiosity_maintained"
        else:
            return "low_curiosity"
    
    def export_results(self, experiment_data, analysis, filename):
        """Export experiment results and analysis to file."""
        
        export_data = {
            'experiment_data': experiment_data,
            'analysis': analysis,
            'metadata': {
                'timestamp': time.time(),
                'config': self.config.toDict() if hasattr(self.config, 'toDict') else str(self.config)
            }
        }
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2, default=str)
        
        print(f"Results exported to {filename}")

# Usage example
runner = NLMExperimentRunner()

# Run experiment
experiment_data = runner.run_phase6_experiment(neurons=2000, steps=2000)

# Analyze results
analysis = runner.analyze_results(experiment_data)

# Print analysis
print("\n=== EXPERIMENT ANALYSIS ===")
print(f"Integration Score: {analysis['integration_score']:.2f}/1.00")
print(f"Average Reward: {analysis['summary']['avg_reward']:.4f}")
print(f"Final Development Stage: {analysis['summary']['final_development_stage']}")
print(f"Memory Trend: {analysis['memory_analysis']['working_memory_trend']}")
print(f"Curiosity Dynamics: {analysis['learning_curves']['curiosity_dynamics']}")

# Export results
runner.export_results(experiment_data, analysis, "experiment_results.json")
```

## Troubleshooting

### Common Issues and Solutions

#### "ImportError: No module named 'pynlm'"
```bash
# Install the package
pip install -e .

# Or reinstall
pip install --force-reinstall .
```

#### "Segmentation fault" in Python code
```python
# Ensure you initialize the brain before use
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this!

# Don't call step on uninitialized brain
brain.step(0)  # Safe after initialization
```

#### Memory usage too high
```python
# Use smaller configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Reduce neurons
config.set("brain.synapse_density", 0.05)  # Reduce connections

# Or use smaller simulation
for step in range(100):  # Fewer steps
    brain.step(step)
```

#### Slow performance
```python
# Use Release build for best performance
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Enable performance optimizations
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
```

#### Configuration file not found
```python
# Create a sample configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.saveToFile("config.json")

# Then load it
config.loadFromFile("config.json")
```

#### GPU acceleration not available
NLM is designed to run on CPU with optimized performance. For GPU acceleration, consider using alternative frameworks.

### Build Issues

#### CMake cannot find Python
```bash
# Install Python development packages
apt-get install python3-dev python3-pip

# Or configure explicitly
cmake .. -DPython_EXECUTABLE=$(which python3)
```

#### Compilation errors
```bash
# Ensure you have required dependencies
apt-get install build-essential cmake g++

# Check CMake version
cmake --version
# Should be >= 3.16

# Verify all source files are present
ls -la src/ | grep -E "\.cpp$"
```

## Supported Platforms

- **Linux**: x86_64, ARM64
- **macOS**: x86_64, ARM64 (Apple Silicon)
- **Windows**: x86_64 (via WSL or native)

## Python Version Support

- **Python 3.8**: Basic functionality
- **Python 3.9**: Full functionality
- **Python 3.10**: Full functionality
- **Python 3.11**: Full functionality
- **Python 3.12**: Full functionality

## Memory Requirements

| Neuron Count | Approximate Memory Usage | Recommended RAM |
|--------------|--------------------------|-----------------|
| 1,000        | 50-100 MB                | 2 GB            |
| 5,000        | 250-500 MB               | 4 GB            |
| 10,000       | 500-1000 MB              | 8 GB            |
| 20,000       | 1-2 GB                   | 16 GB           |
| 50,000       | 2.5-5 GB                 | 32 GB           |

## Performance Tips

1. **Use Release Build**: For optimal performance, always build with `-DCMAKE_BUILD_TYPE=Release`
2. **Multi-threading**: Use `make -j$(nproc)` to utilize all CPU cores
3. **Memory Management**: Monitor memory usage with `top` or `htop` during long simulations
4. **Configuration**: Use specific configurations for different use cases (small for testing, large for production)
5. **Profiling**: Use `perf` or `valgrind` to identify performance bottlenecks

## License

MIT License. See LICENSE file for details.

## Copyright

Copyright (c) 2026 NLM Project. All rights reserved.

## Acknowledgements

- The NLM project builds on decades of research in computational neuroscience
- Special thanks to the open-source community for contributing tools and libraries
- Inspired by biological brain architectures and learning principles

## Future Developments

### Planned Enhancements:

1. **Multi-Agent Frameworks**: Built-in support for multi-agent simulations
2. **Advanced Learning**: Incorporate reinforcement learning and supervised learning
3. **Real-world Integration**: Bridge with robotics and sensor systems
4. **Visualization Tools**: Advanced 3D visualization and analysis tools
5. **Cloud Computing**: Distributed computing capabilities for large-scale simulations
6. **Mobile/Edge Computing**: Optimized versions for resource-constrained devices
7. **Educational Tools**: Interactive learning and teaching tools
8. **Research APIs**: Advanced APIs for computational neuroscience research

### Current Limitations:

1. **Simplified Biology**: Based on LIF neurons rather than detailed biophysics
2. **Limited Size**: Current limitations on very large networks
3. **Single Agent**: Primarily designed for single-agent simulations
4. **Environment Constraints**: Simplified world models

## Support and Community

For support and community discussions:

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Issues**: Report bugs and feature requests
- **Discussions**: Community forums and technical discussions
- **Examples**: Sample scripts and tutorials

## Contact

For technical questions, bug reports, or collaboration opportunities:

- **Email**: contact@nlm-project.org
- **GitHub**: https://github.com/nlm-project/nlm/issues

## Version History

### Version 0.1.0
- Initial release with Phase 6 integration
- Complete Python bindings
- Comprehensive command-line tools
- Full cognitive architecture integration

## Legal Notice

NLM (熙然) is a research project and should be used for scientific and educational purposes only. The project makes no claims about reproducing biological brains or achieving artificial general intelligence.

---

*This documentation was automatically generated from the NLM source code.*
