# NLM Python Bindings - Enhanced Usage Guide

This guide provides comprehensive examples and best practices for using the enhanced NLM Python bindings.

## Overview

The NLM Python bindings provide access to the Neural Learning Machine simulation framework, enabling users to:
- Create and configure neural brains
- Run complex simulations
- Interact with environments
- Monitor and analyze neural activity
- Use multiple memory and learning systems

## Quick Start

```python
import pynlm

# Create a basic brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500, pynlm.ConfigSource.Runtime)
config.set("region_count", 3, pynlm.ConfigSource.Runtime)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
brain.simulate(100, verbose=True)

# Get statistics
stats = brain.getStatistics()
print(f"Active neurons: {stats['active_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f}")
```

## 1. Basic Brain Simulation

### Core Operations

```python
import pynlm
import numpy as np

# Create brain with custom configuration
config = pynlm.createValidatedConfig({
    "neuron_count": 1000,
    "region_count": 5,
    "connection_probability": 0.2,
    "simulation.timestep": 0.001,
    "neuron.LIF.v_threshold": -55.0,
})

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with progress reporting
for step in range(50):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getActiveNeuronCount()} active neurons")

# Save brain state
brain.save("my_brain.json")

# Load and continue
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.load("my_brain.json")
```

### Advanced Configuration Management

```python
# Update configuration from dictionary
config = pynlm.createDefaultConfig()
config.updateFromDict({
    "neuromodulation.reward.enabled": True,
    "neuromodulation.curiosity.enabled": True,
    "memory.episodic.capacity": 100,
    "development.plasticity_rate": 0.8,
})

# Get configuration as dictionary
config_dict = config.to_dict()
print(f"Current configuration: {config_dict}")

# Load from Python dictionary
advanced_config = pynlm.createAdvancedConfig({
    "neuron_count": 2000,
    "region_count": 10,
    "plasticity_rules": ["STDP", "Hebbian", "Structural"],
})
```

## 2. Agent-Environment Interaction

### Basic Agent Setup

```python
import pynlm

# Create world and agent components
world = pynlm.createSimpleWorld()
world.configure(width=200, height=200, visionWidth=21, visionHeight=21)

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

print(f"Agent sensory input size: {agent_brain.getSensoryInputSize()}")
print(f"Agent motor output size: {agent_brain.getMotorOutputSize()}")
```

### Sensory Processing and Action

```python
# Create sensory percept
sensory_input = pynlm.Vision(21, 21, 3)
# ... populate with vision data ...

percept = pynlm.SensoryPercept()
percept.setVision(sensory_input)
percept.setTimestamp(world.getSimulationTime())

# Process input and produce action
action = agent_brain.act(percept, pynlm.ActionType.MoveForward)

if action:
    print(f"Action produced: {action.getName()}")
    print(f"Parameters: {action.getParameters()}")
```

### Complete Training Episode

```python
def train_agent(agent_brain, world, episodes=10, max_steps=100):
    """Train an agent in an environment for multiple episodes."""
    for episode in range(episodes):
        print(f"Episode {episode + 1}/{episodes}")
        
        # Reset for new episode
        agent_brain.reset()
        world.reset()
        
        episode_reward = 0.0
        
        for step in range(max_steps):
            # Get current sensory input
            percept = world.getSensoryPercept()
            
            # Agent decision making
            action = agent_brain.act(percept)
            
            if action:
                # Apply action in environment
                result = world.applyMotorCommand(action.getType(), world.getSimulationTime())
                
                # Get reward feedback
                episode_reward += result.reward
                
                # Train with reward modulation
                agent_brain.applyRewardModulation(result.reward, 0.0)
            
            # Update world
            world.update(1)
            
            # Check termination conditions
            agent_body = world.getAgentBody()
            if agent_body.health <= 0 or agent_body.energy <= 0:
                print(f"  Episode terminated at step {step}")
                break
        
        print(f"  Total reward: {episode_reward}")
    
    return agent_brain

# Train the agent
agent_brain = train_agent(agent_brain, world, episodes=5, max_steps=200)
```

## 3. Memory and Learning Systems

### Working with Memory Systems

```python
import pynlm

# Configure memory systems
config = pynlm.createDefaultConfig()
config.set("memory.working.capacity", 50)
config.set("memory.episodic.capacity", 100)
config.set("memory.associative.enabled", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
if hasattr(brain, 'getWorkingMemory'):
    working_memory = brain.getWorkingMemory()
    print(f"Working memory capacity: {working_memory.getCapacity()}")

if hasattr(brain, 'getEpisodicMemory'):
    episodic_memory = brain.getEpisodicMemory()
    print(f"Episodic memory capacity: {episodic_memory.getCapacity()}")
    
    # Store an experience
    experience = episodic_memory.createExperience(
        timestamp=world.getSimulationTime(),
        percept=sensory_percept,
        action=action,
        reward=action_result.reward,
        next_percept=next_percept
    )
    episodic_memory.storeExperience(experience)

if hasattr(brain, 'getPredictionSystem'):
    prediction_system = brain.getPredictionSystem()
    prediction_error = prediction_system.getPredictionError()
    print(f"Prediction error: {prediction_error}")
```

### Neuromodulation Integration

```python
# Monitor neuromodulation levels
agent_brain = pynlm.createAgentBrain(brain)

print(f"Neuromodulation level: {agent_brain.getNeuromodulationLevel()}")
print(f"Curiosity level: {agent_brain.getCuriosityLevel()}")
print(f"Novelty level: {agent_brain.getNoveltyLevel()}")
print(f"Prediction error: {agent_brain.getPredictionError()}")

# Check which systems are enabled
print(f"Reward modulation enabled: {agent_brain.isRewardModulationEnabled()}")
print(f"Development enabled: {agent_brain.isDevelopmentEnabled()}")
print(f"Structural plasticity enabled: {agent_brain.isStructuralPlasticityEnabled()}")
```

## 4. Developmental Stages

### Working with Development

```python
import pynlm
import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Monitor developmental progression
print("Developmental stage progression:")
for stage_name in ["Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"]:
    # Get enum value
    stage_enum = getattr(pynlm.DevelopmentalStage, stage_name)
    
    brain.setDevelopmentalStage(stage_enum)
    current_stage = brain.getDevelopmentalStage()
    
    print(f"  {stage_name}: {current_stage.name}")
    
    # Run simulation steps for this stage
    for step in range(20):
        brain.step(step)
    
    # Check developmental changes
    new_stage = brain.getDevelopmentalStage()
    if new_stage != current_stage:
        print(f"    Stage automatically progressed to: {new_stage.name}")
```

## 5. Visualization and Monitoring

### Brain Monitoring

```python
import pynlm
import json
import os

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation with data collection
for step in range(100):
    brain.step(step)

# Get comprehensive statistics
stats = brain.getStatistics()
print("Brain Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")

# Get neural activity data
if hasattr(brain, 'getVisualizationInterface'):
    viz = brain.getVisualizationInterface()
    
    # Get neural activity as NumPy arrays
    activity = viz.getNeuralActivity()
    spike_times = viz.getSpikeTimes()
    
    print(f"Collected {len(activity)} activity samples")
    print(f"Recorded {len(spike_times)} spike events")
    
    # Save for external visualization
    viz.saveToFile("brain_activity.json")
    print("Activity data saved to brain_activity.json")

# Compare two brain states
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()
brain2.simulate(50)

comparison = brain.compareWith(brain2)
print("Brain comparison:")
for key, value in comparison.items():
    print(f"  {key}: {value}")
```

## 6. Checkpoint and Persistence

### Save/Load Operations

```python
import pynlm
import os

# Create and run a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(50):
    brain.step(step)

# Create checkpoint with automatic filename
checkpoint_file = brain.createCheckpoint("Training session #1")
print(f"Checkpoint created: {checkpoint_file}")

# Manual save with custom name
brain.save("custom_brain_state.json")

# Load and continue
loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
loaded_brain.load("custom_brain_state.json")
print(f"Loaded brain has {loaded_brain.getTotalNeuronCount()} neurons")

# Save configuration separately
config.saveToFile("brain_config.json")

# Load configuration separately
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("brain_config.json")

# Combine config and brain
combined_brain = pynlm.createBrain(config2)
combined_brain.initialize()
combined_brain.load("custom_brain_state.json")
```

## 7. Advanced Simulation Patterns

### Batch Processing

```python
import pynlm
import time

def batch_simulate(num_simulations, steps_per_simulation, config=None):
    """Run multiple simulations in batch."""
    if config is None:
        config = pynlm.createDefaultConfig()
    
    results = []
    
    for sim in range(num_simulations):
        print(f"Running simulation {sim + 1}/{num_simulations}")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        sim_start_time = time.time()
        
        for step in range(steps_per_simulation):
            brain.step(step)
        
        sim_end_time = time.time()
        
        stats = brain.getStatistics()
        stats['simulation_time'] = sim_end_time - sim_start_time
        stats['simulation_number'] = sim + 1
        
        results.append(stats)
        
        # Save intermediate results
        if (sim + 1) % 10 == 0:
            brain.save(f"batch_sim_{sim + 1}.json")
    
    return results

# Run batch simulation
batch_results = batch_simulate(20, 100)

# Analyze results
avg_neurons = sum(r['total_neurons'] for r in batch_results) / len(batch_results)
print(f"Average neurons across simulations: {avg_neurons:.1f}")
```

### Parameter Sweeps

```python
import pynlm

def parameter_sweep(param_name, param_values, config_template=None):
    """Run simulations with different parameter values."""
    if config_template is None:
        config_template = pynlm.createDefaultConfig()
    
    results = []
    
    for value in param_values:
        print(f"Testing {param_name} = {value}")
        
        config = config_template.clone()
        config.set(param_name, value, pynlm.ConfigSource.Runtime)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        brain.simulate(50)
        stats = brain.getStatistics()
        stats[param_name] = value
        
        results.append(stats)
    
    return results

# Example: sweep neuron count
neuron_counts = [100, 500, 1000, 2000]
sweep_results = parameter_sweep("neuron_count", neuron_counts)

for result in sweep_results:
    print(f"Neurons: {result['neuron_count']}, "
          f"Active: {result['active_neurons']}, "
          f"Firing Rate: {result['average_firing_rate']:.2f}")
```

## 8. Error Handling and Robustness

### Safe Operations

```python
import pynlm

def safe_simulation(brain, max_steps, verbose=False):
    """Run simulation with error handling."""
    try:
        for step in range(max_steps):
            brain.step(step)
            
            if verbose and step % 10 == 0:
                stats = brain.getStatistics()
                print(f"Step {step}: {stats['active_neurons']} active neurons")
                
    except Exception as e:
        print(f"Simulation error: {e}")
        return False
    
    return True

# Use safe simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

success = safe_simulation(brain, 100, verbose=True)
if success:
    print("Simulation completed successfully")
else:
    print("Simulation failed")
```

## 9. Utility Functions

### Helper Functions

```python
import pynlm

# Format identifiers for display
neuron_id = pynlm.NeuronId(12345)
print(f"Neuron ID: {neuron_id}")

# Create configurations from dictionaries
config = pynlm.createAdvancedConfig({
    "learning_rate": 0.01,
    "batch_size": 32,
    "epochs": 100,
})

# Quick testing function
brain = pynlm.runQuickTest()
print(f"Quick test brain has {brain.getTotalNeuronCount()} neurons")

# Get version information
print(f"NLM Python Bindings: {pynlm.getVersion()}")
```

## 10. Performance Optimization

### Efficient Data Handling

```python
import pynlm
import numpy as np

# Use NumPy arrays for large data
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000, pynlm.ConfigSource.Runtime)

brain = pynlm.createBrain(config)
brain.initialize()

# Process data in chunks
chunk_size = 1000
for chunk_start in range(0, 5000, chunk_size):
    chunk_end = min(chunk_start + chunk_size, 5000)
    
    # Process chunk of neurons
    for neuron_id in range(chunk_start, chunk_end):
        # Individual neuron processing
        pass
    
    # Update simulation
    brain.step(chunk_start)
```

## Complete Example: Full Training Pipeline

```python
import pynlm
import json
import time

def complete_training_pipeline(config_path=None, output_dir="training_output"):
    """Complete training pipeline with all features."""
    import os
    os.makedirs(output_dir, exist_ok=True)
    
    print("=== NLM Complete Training Pipeline ===")
    
    # 1. Load or create configuration
    if config_path and os.path.exists(config_path):
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_path)
    else:
        config = pynlm.createAdvancedConfig({
            "neuron_count": 2000,
            "region_count": 8,
            "connection_probability": 0.15,
            "neuromodulation.reward.enabled": True,
            "neuromodulation.curiosity.enabled": True,
            "memory.episodic.capacity": 200,
            "development.enabled": True,
        })
        config.saveToFile(os.path.join(output_dir, "config.json"))
    
    # 2. Create brain and environment
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(300, 300, 21, 21)
    
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # 3. Training loop
    num_episodes = 20
    steps_per_episode = 100
    
    training_log = []
    
    for episode in range(num_episodes):
        episode_start_time = time.time()
        
        print(f"Episode {episode + 1}/{num_episodes}")
        
        # Reset environment and agent
        agent_brain.reset()
        world.reset()
        
        episode_data = {
            'episode': episode + 1,
            'steps': 0,
            'reward': 0.0,
            'duration': 0.0,
            'final_stage': None,
            'checkpoints': []
        }
        
        for step in range(steps_per_episode):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Agent decision making
            action = agent_brain.act(percept)
            
            if action:
                # Apply action
                result = world.applyMotorCommand(action.getType(), world.getSimulationTime())
                
                # Train with reward
                agent_brain.applyRewardModulation(result.reward, 0.0)
                
                episode_data['reward'] += result.reward
                
                # Store experience if memory system is available
                if hasattr(agent_brain, 'storeExperience'):
                    agent_brain.storeExperience(percept, action, result)
            
            # Update world
            world.update(1)
            episode_data['steps'] += 1
        
        episode_end_time = time.time()
        episode_data['duration'] = episode_end_time - episode_start_time
        episode_data['final_stage'] = agent_brain.getDevelopmentalStage().name
        
        # Create checkpoint
        checkpoint_file = agent_brain.createCheckpoint(f"Episode {episode + 1}")
        episode_data['checkpoints'].append(checkpoint_file)
        
        training_log.append(episode_data)
        
        print(f"  Steps: {episode_data['steps']}, Reward: {episode_data['reward']:.2f}, "
              f"Duration: {episode_data['duration']:.2f}s, Stage: {episode_data['final_stage']}")
        
        # Save episode checkpoint
        brain.save(os.path.join(output_dir, f"episode_{episode + 1}_brain.json"))
    
    # 4. Save training log
    log_file = os.path.join(output_dir, "training_log.json")
    with open(log_file, 'w') as f:
        json.dump(training_log, f, indent=2)
    
    # 5. Analyze results
    avg_reward = sum(e['reward'] for e in training_log) / len(training_log)
    total_steps = sum(e['steps'] for e in training_log)
    
    print(f"\n=== Training Complete ===")
    print(f"Average reward per episode: {avg_reward:.2f}")
    print(f"Total steps executed: {total_steps}")
    print(f"Final developmental stage: {training_log[-1]['final_stage']}")
    print(f"Training log saved to: {log_file}")
    
    return agent_brain, training_log

# Run complete pipeline
agent, log = complete_training_pipeline()
```

## Installation and Setup

### Prerequisites

- Python 3.8+
- NumPy (optional but recommended)
- Build system with CMake
- C++ compiler with C++20 support

### Building and Installing

```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install -e .  # from project root
```

### Development Version

```bash
# For development, use the enhanced bindings
pip install -e . --config-settings=build-dir=build
```

## Troubleshooting

### Common Issues

1. **Import errors**: Ensure the pynlm module is in your Python path
2. **Build failures**: Check that you have a C++ compiler with C++20 support
3. **NumPy compatibility**: Install NumPy if you want to use array conversions
4. **Memory issues**: Use smaller brain configurations for debugging

### Getting Help

- Check the examples directory for additional usage patterns
- Review the API documentation for specific method signatures
- Use the `__repr__` and `__str__` methods for debugging
- Enable verbose logging for detailed progress information

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Acknowledgments

This enhanced version builds upon the original NLM framework and provides significant improvements in:
- Pythonic API design
- Configuration management
- Error handling
- Visualization and monitoring
- Performance optimization
- Documentation and examples