# Advanced NLM Usage Guide

## Overview

This document covers advanced NLM usage patterns for power users and researchers. It builds on the basic usage guide and covers:

- Performance optimization techniques
- Debugging and monitoring tools
- Advanced configuration
- Memory management and checkpointing
- Custom neural network architectures
- Integration with external systems
- Research and experimentation workflows

## Performance Optimization

### Neuron Count Optimization

```python
import pynlm

# Optimized for performance (recommended for large simulations)
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10000)  # Larger network
config.set("brain.synapse_density", 0.05)  # Sparse connections
config.set("plasticity.structural.enable", True)  # Allow structural growth

brain = pynlm.createBrain(config)
brain.initialize()
```

### Memory Management

```python
# Monitor memory usage
brain = pynlm.createBrain(config)
brain.initialize()

# Take checkpoint periodically
brain.save("checkpoint1.bin")
brain.save("checkpoint2.bin")

# Load latest checkpoint
brain.load("checkpoint2.bin")

# Clear memory when done
brain.reset()
```

### Batch Processing

```python
# Process multiple simulation steps efficiently
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation in batches for better performance
for batch_start in range(0, total_steps, batch_size):
    batch_end = min(batch_start + batch_size, total_steps)
    
    for step in range(batch_start, batch_end):
        brain.step(step)
        
        # Periodically save progress
        if step % checkpoint_interval == 0:
            brain.save(f"checkpoint_{step}.bin")
            
    # Periodic consolidation
    if batch_start % consolidation_interval == 0:
        brain.getWorkingMemory().consolidate()
        brain.getEpisodicMemory().consolidate()
```

## Debugging and Monitoring Tools

### Brain State Inspection

```python
# Get detailed brain statistics
brain = pynlm.createBrain(config)
brain.initialize()

# Access all monitoring tools
stats = {
    'neurons': brain.getTotalNeuronCount(),
    'synapses': brain.getTotalSynapseCount(),
    'spikes': brain.getTotalSpikeCount(),
    'firing_rate': brain.getAverageFiringRate(),
    'e_i_ratio': brain.getExcitationInhibitionRatio(),
    'pending_events': brain.getPendingSpikeEventCount(),
    'development_stage': brain.getDevelopmentalStage(),
    'curiosity': brain.getCuriosityLevel(),
    'novelty': brain.getNoveltyLevel(),
    'prediction_error': brain.getPredictionError(),
    'neuromodulation': brain.getNeuromodulationLevel(),
}

# Log detailed status
brain.logStatus()
```

### Memory System Monitoring

```python
# Monitor all memory systems
brain = pynlm.createBrain(config)
brain.initialize()

# Working memory
working_mem = brain.getWorkingMemory()
print(f"Working memory traces: {working_mem.getActiveTraces()}")
print(f"Working memory capacity: {working_mem.getCapacity()}")

# Episodic memory
episodic_mem = brain.getEpisodicMemory()
print(f"Episodic episodes: {episodic_mem.getEpisodeCount()}")
print(f"Max episodes: {episodic_mem.getMaxEpisodes()}")

# Associative memory
assoc_mem = brain.getAssociativeMemory()
print(f"Associations: {assoc_mem.getAssociationCount()}")

# Check for patterns
if episodic_mem.hasPattern():
    pattern = episodic_mem.getPattern()
    print(f"Found pattern with {pattern.length} neurons")
```

### Neuromodulation Control

```python
# Fine-tune neuromodulation parameters
brain = pynlm.createBrain(config)
brain.initialize()

# Access neuromodulators
dopamine = brain.getDopamine()
novelty = brain.getNovelty()
curiosity = brain.getCuriosity()

# Adjust levels
for step in range(1000):
    brain.step(step)
    
    # Gradually increase novelty detection sensitivity
    novelty_level = 0.5 + 0.5 * (step / 1000.0)
    novelty.setSensitivity(novelty_level)
    
    # Apply curiosity-driven exploration
    curiosity.setExplorationRate(0.1 + 0.1 * (step / 1000.0))
    
    if step % 100 == 0:
        print(f"Step {step}: Novelty={novelty_level:.2f}, Curiosity={curiosity.getExplorationRate():.2f}")
```

## Advanced Configuration

### Custom Configuration Schema

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Custom neural parameters
config.set("brain.neuron_count", 5000)
config.set("brain.region_count", 3)
config.set("brain.synapse_density", 0.08)

# Advanced plasticity configuration
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.stdp.tau_plus", 25.0)
config.set("plasticity.stdp.tau_minus", 25.0)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.001)
config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.0002)
config.set("plasticity.structural.pruning_rate", 0.00001)

# Neuromodulation configuration
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.base_rate", 0.01)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.threshold", 0.3)

# Memory system configuration
config.set("memory.working.capacity", 500)
config.set("memory.episodic.max_episodes", 10000)
config.set("memory.episodic.consolidation_interval", 500)
config.set("memory.associative.learning_rate", 0.1)
config.set("memory.associative.forgetting_rate", 0.01)

# Development configuration
config.set("development.stage.initial_period", 1000.0)
config.set("development.stage.critical_period_end", 5000.0)
config.set("development.plasticity_modulation", True)

# Checkpoint configuration
config.set("checkpoint.dir", "./checkpoints")
config.set("checkpoint.save_interval", 10000)
config.set("checkpoint.max_count", 20)
config.set("checkpoint.compress", True)

brain = pynlm.createBrain(config)
```

### Configuration Templates

```python
# Configuration template factory
def create_experiment_config(base_config, experiment_type="default"):
    """Create configuration for specific experiment type."""
    config = pynlm.createDefaultConfig()
    
    if experiment_type == "memory_formation":
        # For memory experiments
        config.set("brain.neuron_count", 2000)
        config.set("memory.episodic.max_episodes", 5000)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.base_rate", 0.05)
        
    elif experiment_type == "planning":
        # For planning experiments
        config.set("brain.neuron_count", 3000)
        config.set("brain.region_count", 2)
        config.set("plasticity.stdp.enable", True)
        config.set("neuromod.dopamine.scale", 2.0)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.novelty.threshold", 0.2)
        
    elif experiment_type == "social":
        # For social learning experiments
        config.set("brain.neuron_count", 1500)
        config.set("brain.region_count", 3)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.base_rate", 0.08)
    
    # Merge with base config
    for key in base_config.getKeys():
        if base_config.has(key):
            config.set(key, base_config.get(key))
    
    return config

# Usage
base_config = pynlm.createDefaultConfig()
memory_config = create_experiment_config(base_config, "memory_formation")
brain = pynlm.createBrain(memory_config)
```

## Custom Neural Network Architectures

### Multi-Region Brain

```python
import pynlm

# Create custom multi-region brain architecture
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Add specialized regions
region1 = brain.addRegion("SensoryCortex")
region2 = brain.addRegion("MotorCortex") 
region3 = brain.addRegion("AssociationCortex")

# Configure regions with different neuron populations
for region in [region1, region2, region3]:
    # Set different neuron counts per region
    brain.addRegion(region.getName())
    
    # Get region and add populations
    nlm_region = brain.getRegion(region)
    if nlm_region:
        sensory_pop = nlm_region.addPopulation(500, pynlm.NeuronType.Sensory)
        internal_pop = nlm_region.addPopulation(1000, pynlm.NeuronType.Internal)
        motor_pop = nlm_region.addPopulation(300, pynlm.NeuronType.Motor)
```

### Custom Synaptic Connectivity

```python
# Create brain with custom connectivity patterns
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 2)
config.set("connection_probability", 0.15)

brain = pynlm.createBrain(config)
brain.initialize()

# Get regions for connectivity manipulation
region1 = brain.getRegion(RegionId(1))
region2 = brain.getRegion(RegionId(2))

if region1 and region2:
    # Add specific inter-region connections
    brain.addInterRegionConnection(RegionId(1), RegionId(2), 1.5, 5)
    brain.addInterRegionConnection(RegionId(2), RegionId(1), 0.8, 3)
    
    # Get synapses and modify properties
    for synapse in region1.getSynapses():
        # Apply custom plasticity rules
        synapse.enablePlasticity(true, true, true)
```

## Integration with External Systems

### Data Input Pipeline

```python
import pynlm
import numpy as np
from datetime import datetime

# Create agent with world integration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Configure world for data input
world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
world.setAgentStart(25.0, 25.0)

agent.initialize(world)

# Data input pipeline
def process_external_data(data, timestamp):
    """Process external data and update brain state."""
    # Convert external data to sensory input
    vision_data = data.get('vision', np.zeros((8, 8, 3)))
    audio_data = data.get('audio', np.zeros(100))
    internal_data = data.get('internal', [])
    
    # Create sensory percept
    percept = pynlm.SensoryPercept()
    
    # Set vision data (assuming Vision class exists)
    vision = pynlm.Vision(8, 8, 3)
    vision.setData(vision_data.flatten())
    percept.setVision(vision)
    
    # Set audio data
    audio = pynlm.Audio(44100, 100)
    audio.setData(audio_data)
    percept.setAudio(audio)
    
    # Set internal signals
    percept.setInternal(internal_data)
    percept.setTimestamp(timestamp)
    
    # Process through agent
    agent.processSensoryInput(percept)
    
    # Brain step
    brain.step(int(timestamp * 1000))
    
    # Get action and apply to world
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, timestamp)
    
    return action

# Example data processing loop
for episode in range(100):
    for step in range(1000):
        # Get external data (could be from files, sensors, etc.)
        data = get_external_data()  # Your data source
        
        # Process data
        action = process_external_data(data, step * 0.1)
        
        # Apply reward modulation
        reward = data.get('reward', 0.0)
        predicted_reward = brain.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"Episode {episode}, Step {step}: Action={action.type}, Reward={reward}")
```

### Custom Visualization

```python
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import pynlm

# Create brain with visualization
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create SimpleWorld for visualization
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(10.0, 10.0)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Collect visualization data
visualization_data = []
time_points = []

for step in range(100):
    world.update(0.1)
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain step
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Collect visualization data
    time_points.append(step * 0.1)
    
    frame_data = {
        'step': step,
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'development_stage': brain.getDevelopmentalStage(),
        'agent_x': world.getAgentBody().x,
        'agent_y': world.getAgentBody().y,
    }
    visualization_data.append(frame_data)

# Create interactive visualization
def create_interactive_plot(data):
    fig, axes = plt.subplots(2, 3, figsize=(15, 10))
    fig.suptitle('NLM Brain State Visualization', fontsize=16)
    
    # Firing neurons over time
    axes[0,0].plot(data['time'], data['firing_neurons'])
    axes[0,0].set_title('Firing Neurons')
    axes[0,0].set_ylabel('Count')
    
    # Total spikes over time
    axes[0,1].plot(data['time'], data['total_spikes'])
    axes[0,1].set_title('Total Spikes')
    axes[0,1].set_ylabel('Count')
    
    # Firing rate over time
    axes[0,2].plot(data['time'], data['average_firing_rate'])
    axes[0,2].set_title('Average Firing Rate')
    axes[0,2].set_ylabel('Hz')
    
    # Curiosity over time
    axes[1,0].plot(data['time'], data['curiosity'])
    axes[1,0].set_title('Curiosity Level')
    axes[1,0].set_ylabel('Level')
    axes[1,0].set_xlabel('Time (s)')
    
    # Novelty over time
    axes[1,1].plot(data['time'], data['novelty'])
    axes[1,1].set_title('Novelty Level')
    axes[1,1].set_ylabel('Level')
    axes[1,1].set_xlabel('Time (s)')
    
    # Agent position over time
    axes[1,2].plot(data['agent_x'], data['agent_y'], 'b-')
    axes[1,2].plot(data['agent_x'][0], data['agent_y'][0], 'go', markersize=10)
    axes[1,2].plot(data['agent_x'][-1], data['agent_y'][-1], 'ro', markersize=10)
    axes[1,2].set_title('Agent Position')
    axes[1,2].set_xlabel('X')
    axes[1,2].set_ylabel('Y')
    axes[1,2].set_aspect('equal')
    
    plt.tight_layout()
    return fig

# Create and display visualization
fig = create_interactive_plot({
    'time': time_points,
    'firing_neurons': [d['firing_neurons'] for d in visualization_data],
    'total_spikes': [d['total_spikes'] for d in visualization_data],
    'average_firing_rate': [d['average_firing_rate'] for d in visualization_data],
    'curiosity': [d['curiosity'] for d in visualization_data],
    'novelty': [d['novelty'] for d in visualization_data],
    'agent_x': [d['agent_x'] for d in visualization_data],
    'agent_y': [d['agent_y'] for d in visualization_data],
})

plt.show()
```

## Research and Experimentation

### Controlled Experiment Framework

```python
import pynlm
from typing import List, Dict, Callable
import json
from datetime import datetime

class NLMLabExperiment:
    """Framework for NLM experiments with proper controls."""
    
    def __init__(self, name: str):
        self.name = name
        self.results = []
        self.brain = None
        self.agent = None
        self.world = None
    
    def setup_experiment(self, 
                        config: pynlm.Config,
                        environment_type: str = "simple"):
        """Setup experiment with given configuration."""
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        
        if environment_type == "simple":
            self.world = pynlm.createSimpleWorld()
            self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        self.world.setAgentStart(10.0, 10.0)
        self.agent.initialize(self.world)
        
        return self
    
    def run_episode(self, 
                    episode_length: int = 1000,
                    random_seed: int = 42) -> Dict:
        """Run single experiment episode."""
        if self.brain:
            self.brain.getRandomGenerator().setSeed(random_seed)
        
        episode_data = {
            'episode': len(self.results),
            'steps': [],
            'total_spikes': 0,
            'final_firing_rate': 0,
            'average_curiosity': 0,
            'total_reward': 0,
            'novelty_events': 0,
        }
        
        for step in range(episode_length):
            # Update world
            self.world.update(0.1)
            
            # Get percept
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get action and apply
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Collect metrics
            step_data = {
                'step': step,
                'time': step * 0.1,
                'spikes': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'action_type': action.type,
                'action_params': action.getParameters(),
                'agent_x': self.world.getAgentBody().x,
                'agent_y': self.world.getAgentBody().y,
            }
            episode_data['steps'].append(step_data)
            
            episode_data['total_spikes'] = step_data['spikes']
            episode_data['average_curiosity'] += step_data['curiosity']
            episode_data['novelty_events'] += 1 if step_data['novelty'] > 0.5 else 0
        
        episode_data['final_firing_rate'] = episode_data['steps'][-1]['firing_rate']
        episode_data['average_curiosity'] /= episode_length
        
        self.results.append(episode_data)
        return episode_data
    
    def run_experiment(self, 
                       num_episodes: int = 10,
                       episode_length: int = 1000) -> Dict:
        """Run complete experiment with multiple episodes."""
        experiment_data = {
            'name': self.name,
            'num_episodes': num_episodes,
            'episode_length': episode_length,
            'episodes': [],
            'summary': {},
            'timestamp': datetime.now().isoformat()
        }
        
        for episode_idx in range(num_episodes):
            print(f"Running episode {episode_idx + 1}/{num_episodes}")
            episode_data = self.run_episode(episode_length, episode_idx)
            experiment_data['episodes'].append(episode_data)
        
        # Calculate summary statistics
        experiment_data['summary'] = self.calculate_summary()
        
        return experiment_data
    
    def calculate_summary(self) -> Dict:
        """Calculate experiment summary statistics."""
        if not self.results:
            return {}
        
        num_episodes = len(self.results)
        total_spikes = sum(ep['total_spikes'] for ep in self.results)
        final_firing_rates = [ep['final_firing_rate'] for ep in self.results]
        avg_curiosities = [ep['average_curiosity'] for ep in self.results]
        total_rewards = [ep.get('total_reward', 0) for ep in self.results]
        novelty_events = [ep['novelty_events'] for ep in self.results]
        
        return {
            'total_episodes': num_episodes,
            'mean_total_spikes': total_spikes / num_episodes,
            'mean_final_firing_rate': sum(final_firing_rates) / num_episodes,
            'std_final_firing_rate': self.calculate_std(final_firing_rates),
            'mean_average_curiosity': sum(avg_curiosities) / num_episodes,
            'std_average_curiosity': self.calculate_std(avg_curiosities),
            'mean_total_reward': sum(total_rewards) / num_episodes,
            'total_novelty_events': sum(novelty_events),
            'mean_novelty_events_per_episode': sum(novelty_events) / num_episodes,
        }
    
    def calculate_std(self, values: List[float]) -> float:
        """Calculate standard deviation."""
        if len(values) <= 1:
            return 0.0
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return variance ** 0.5
    
    def save_results(self, filepath: str):
        """Save experiment results to JSON file."""
        with open(filepath, 'w') as f:
            json.dump(self.results, f, indent=2)
    
    def save_experiment_data(self, filepath: str):
        """Save complete experiment data."""
        data = {
            'name': self.name,
            'results': self.results,
            'summary': self.calculate_summary(),
            'timestamp': datetime.now().isoformat()
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
        
        print(f"Experiment data saved to {filepath}")

# Example usage of experiment framework
experiment = NLMLabExperiment("Memory Formation Study")

# Custom configuration for memory experiment
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 2)
config.set("plasticity.structural.enable", True)
config.set("memory.episodic.max_episodes", 5000)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.base_rate", 0.05)

experiment.setup_experiment(config)
experiment.run_experiment(num_episodes=5, episode_length=500)
experiment.save_experiment_data("memory_experiment_results.json")
print("Experiment completed successfully!")
```

## Troubleshooting Guide

### Common Issues and Solutions

#### Issue: Brain not responding to sensory input

**Symptoms:** Brain remains inactive despite sensory input

**Solutions:**
1. **Check initialization order:** Always call `brain.initialize()` before using
2. **Verify sensory neurons:** Ensure sensory neurons exist in the brain
3. **Check input values:** Validate that sensory input contains non-zero values
4. **Debug with logging:** Enable debug logging to see what's happening

**Debug code:**
```python
brain = pynlm.createBrain(config)
brain.initialize()

# Test with simple input
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Sensory neurons: {brain.getTotalNeuronCount() // 4}")

# Create test sensory input
test_input = [0.5, 0.6, 0.7, 0.8, 0.9]

for i, value in enumerate(test_input):
    print(f"Testing input {i}: {value}")
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, value)
    brain.step(i)
    print(f"Spikes after input {i}: {brain.getTotalSpikeCount()}")
```

#### Issue: Memory systems not storing data

**Symptoms:** Memory systems appear to be initialized but not storing/retrieving data

**Solutions:**
1. **Verify memory access:** Check that memory systems are properly connected
2. **Test memory APIs:** Use memory system debugging functions
3. **Check memory capacity:** Ensure there's enough capacity for data
4. **Validate input data:** Verify that input data is within expected ranges

**Debug code:**
```python
brain = pynlm.createBrain(config)
brain.initialize()

# Check memory system connectivity
working_mem = brain.getWorkingMemory()
episodic_mem = brain.getEpisodicMemory()

print(f"Working memory initialized: {working_mem is not None}")
print(f"Episodic memory initialized: {episodic_mem is not None}")

# Test memory operations
if working_mem:
    print(f"Working memory capacity: {working_mem.getCapacity()}")
    working_mem.clear()

if episodic_mem:
    print(f"Max episodes: {episodic_mem.getMaxEpisodes()}")
    episodic_mem.clear()
```

#### Issue: Performance bottlenecks

**Symptoms:** Simulation runs slowly or memory usage is high

**Solutions:**
1. **Profile performance:** Use timing measurements to identify bottlenecks
2. **Optimize configuration:** Reduce unnecessary complexity
3. **Monitor memory usage:** Track memory allocation and deallocation
4. **Use checkpoints:** Save intermediate states to avoid recomputation

**Performance monitoring:**
```python
import time

brain = pynlm.createBrain(config)
brain.initialize()

# Performance measurement
start_time = time.time()
steps_completed = 0
memory_samples = []

for step in range(total_steps):
    step_start = time.time()
    
    # Run simulation step
    brain.step(step)
    
    step_end = time.time()
    step_duration = step_end - step_start
    
    steps_completed += 1
    
    # Monitor every 100 steps
    if step % 100 == 0:
        memory_samples.append(brain.getTotalNeuronCount())
        
        if step % 1000 == 0:
            elapsed = time.time() - start_time
            steps_per_second = steps_completed / elapsed
            print(f"Step {step}: {steps_per_second:.1f} steps/sec, "
                  f"avg step time: {step_duration*1000:.2f}ms")
            
            # Save progress periodically
            if step % 5000 == 0:
                brain.save(f"checkpoint_{step}.bin")
                print(f"Saved checkpoint at step {step}")
```

#### Issue: Checkpoint failures

**Symptoms:** Checkpoint save/load operations fail

**Solutions:**
1. **Verify directory permissions:** Ensure write permissions for checkpoint directory
2. **Check disk space:** Ensure sufficient disk space for checkpoints
3. **Validate checkpoint format:** Ensure checkpoint files are not corrupted
4. **Handle exceptions:** Add proper error handling for checkpoint operations

**Robust checkpoint code:**
```python
import os
import traceback

def safe_save(brain, filepath):
    """Safely save brain state with error handling."""
    try:
        # Ensure directory exists
        directory = os.path.dirname(filepath)
        if directory and not os.path.exists(directory):
            os.makedirs(directory)
        
        # Save brain state
        success = brain.save(filepath)
        if success:
            print(f"Successfully saved brain state to {filepath}")
            return True
        else:
            print(f"Failed to save brain state to {filepath}")
            return False
            
    except Exception as e:
        print(f"Error saving brain state: {e}")
        print(traceback.format_exc())
        return False

def safe_load(brain, filepath):
    """Safely load brain state with error handling."""
    try:
        if not os.path.exists(filepath):
            print(f"Checkpoint file not found: {filepath}")
            return False
            
        # Load brain state
        success = brain.load(filepath)
        if success:
            print(f"Successfully loaded brain state from {filepath}")
            return True
        else:
            print(f"Failed to load brain state from {filepath}")
            return False
            
    except Exception as e:
        print(f"Error loading brain state: {e}")
        print(traceback.format_exc())
        return False

# Usage with error handling
save_success = safe_save(brain, "checkpoints/ep1_checkpoint.bin")

if save_success:
    print("Simulation running...")
    
    # ... run simulation ...
    
    load_success = safe_load(brain, "checkpoints/ep1_checkpoint.bin")
```

### Advanced Debugging Techniques

#### Brain State Debugging

```python
def debug_brain_state(brain):
    """Print detailed brain state information."""
    print("=== BRAIN STATE DEBUG ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Pending spike events: {brain.getPendingSpikeEventCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"Excitatory/inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Debug memory systems
    working_mem = brain.getWorkingMemory()
    if working_mem:
        print(f"Working memory traces: {working_mem.getActiveTraces()}")
        print(f"Working memory capacity: {working_mem.getCapacity()}")
    
    episodic_mem = brain.getEpisodicMemory()
    if episodic_mem:
        print(f"Episodic memory episodes: {episodic_mem.getEpisodeCount()}")
        print(f"Max episodes: {episodic_mem.getMaxEpisodes()}")
    
    # Debug neuromodulation
    dopamine = brain.getDopamine()
    if dopamine:
        print(f"Dopamine level: {dopamine.getLevel():.3f}")
    
    curiosity = brain.getCuriosity()
    if curiosity:
        print(f"Curiosity level: {curiosity.getLevel():.3f}")
    
    print("=== END BRAIN STATE DEBUG ===\n")

# Usage during simulation
for step in range(100):
    brain.step(step)
    
    if step % 10 == 0:
        debug_brain_state(brain)
```

#### Performance Profiling

```python
import cProfile
import pstats
import io
import time

def profile_simulation(brain, num_steps=1000):
    """Profile simulation performance."""
    print(f"Profiling {num_steps} simulation steps...")
    
    # Memory profiling
    import tracemalloc
    tracemalloc.start()
    
    # Time profiling
    pr = cProfile.Profile()
    pr.enable()
    
    start_time = time.time()
    
    # Run simulation
    for step in range(num_steps):
        brain.step(step)
        
        if step % 100 == 0:
            current, peak = tracemalloc.get_traced_memory()
            print(f"Step {step}: Current memory: {current/1024/1024:.2f} MB, "
                  f"Peak: {peak/1024/1024:.2f} MB")
    
    end_time = time.time()
    
    pr.disable()
    
    # Get memory statistics
    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    
    print(f"\n=== PERFORMANCE SUMMARY ===")
    print(f"Total time: {end_time - start_time:.2f} seconds")
    print(f"Average steps per second: {num_steps/(end_time - start_time):.1f}")
    print(f"Average memory usage: {current/1024/1024:.2f} MB")
    print(f"Peak memory usage: {peak/1024/1024:.2f} MB")
    
    # Generate profiling report
    s = io.StringIO()
    ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
    ps.print_stats(20)  # Top 20 functions
    
    print("\n=== TOP 20 FUNCTIONS BY CUMULATIVE TIME ===")
    print(s.getvalue())

# Run performance profiling
profile_simulation(brain, 1000)
```

## Next Steps

### For Beginners:
1. Complete the basic usage guide
2. Run the provided examples
3. Understand the core brain components

### For Intermediate Users:
1. Explore advanced configuration options
2. Implement custom neural architectures
3. Set up experiment frameworks
4. Use debugging and monitoring tools

### For Advanced Users:
1. Implement custom neuromodulation strategies
2. Create complex environment interactions
3. Develop specialized visualization tools
4. Conduct rigorous scientific experiments
5. Optimize performance for large-scale simulations

### Research Integration:
1. Use the experiment framework for controlled studies
2. Implement longitudinal studies with checkpointing
3. Compare different configurations systematically
4. Validate results with statistical analysis
5. Publish findings using the comprehensive documentation

## Additional Resources

- **API Documentation**: See python/bindings.cpp for complete API reference
- **Source Code**: Explore src/ directory for implementation details
- **Examples**: Check experiment/ directory for working examples
- **Community**: Join NLM community for support and collaboration

This guide provides comprehensive coverage of advanced NLM usage patterns. Start with the basics and gradually incorporate advanced features as you become more familiar with the system.