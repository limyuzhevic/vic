# High-Level Convenience APIs for NLM

This section documents the new high-level convenience APIs added to make Python development with NLM easier and more productive.

## Quick Start Examples

### Create a Basic Brain (One-liner)
```python
import pynlm

# Create a basic learning brain with sensible defaults
brain = pynlm.createQuickBrain("basic")
brain.initialize()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Create a Training-Optimized Brain
```python
import pynlm

# Create a brain optimized for reinforcement learning training
brain = pynlm.createTrainingBrain(1500, enableDevelopment=True)
brain.initialize()
```

### Simple Experiment Templates
```python
import pynlm

# Create an exploration experiment (pre-configured)
agent, brain, world = pynlm.createExplorationExperiment()

# Run a complete episode
reward = pynlm.runEpisode(agent, world, maxSteps=1000, verbose=True)
print(f"Episode completed with total reward: {reward}")
```

### Memory Management
```python
import pynlm

# Create and save brain checkpoints
brain = pynlm.createDefaultConfig()
brain = pynlm.createBrain(brain)
brain.initialize()

# Save intermediate progress
pynlm.saveBrainCheckpoint(brain, "checkpoint_1000.pt")

# Later load and resume
brain = pynlm.loadBrainCheckpoint("checkpoint_1000.pt")
```

### Get Brain Metrics for Visualization
```python
import pynlm

# Collect comprehensive metrics for visualization
brain = pynlm.createTrainingBrain()
brain.initialize()

metrics = pynlm.collectBrainMetrics(brain)
for key, value in metrics.items():
    print(f"{key}: {value}")
```

### Debug Brain Health
```python
import pynlm

# Check if brain is healthy before starting simulation
if pynlm.checkBrainHealth(brain):
    print("Brain is healthy!")
else:
    print("Brain has issues - investigation needed")
```

## Complete Example: Training Loop

### Full Training Session with Monitoring
```python
import pynlm

def run_training_session(num_episodes=100, steps_per_episode=500):
    """Complete training loop with monitoring and checkpointing."""
    
    # 1. Create and initialize the system
    print("Initializing training system...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createTrainingBrain(1000, enableDevelopment=True)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    agent.initialize(world)
    
    # 2. Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 3. Run training episodes
    episode_rewards = []
    average_rewards = []
    
    for episode in range(num_episodes):
        # Run complete episode
        reward = pynlm.runEpisode(agent, world, steps_per_episode, verbose=(episode % 10 == 0))
        episode_rewards.append(reward)
        
        # Calculate moving average
        avg = sum(episode_rewards[-10:]) / min(10, len(episode_rewards))
        average_rewards.append(avg)
        
        # Checkpoint every 20 episodes
        if episode % 20 == 0 and episode > 0:
            pynlm.saveBrainCheckpoint(brain, f"checkpoint_{episode*steps_per_episode}.pt")
            print(f"Checkpoint saved at episode {episode}")
    
    # 4. Print training summary
    print("\n=== TRAINING SUMMARY ===")
    print(f"Total episodes: {num_episodes}")
    print(f"Final average reward: {average_rewards[-1]:.3f}")
    print(f"Best episode reward: {max(episode_rewards):.3f}")
    print(f"Average reward trend: {((average_rewards[-1] - average_rewards[0]) / average_rewards[0] * 100):+.1f}%")
    
    return episode_rewards, average_rewards

# Run the complete training
rewards, averages = run_training_session(100, 500)
```

## Individual API Reference

### Brain Creation APIs

#### `createQuickBrain(configName="basic")`
Creates a brain with predefined configurations optimized for different use cases.

**Parameters:**
- `configName`: String specifying configuration ("basic", "advanced", or "focused")

**Basic Configuration:**
- 500 neurons, STDP + Hebbian plasticity
- Curiosity and novelty enabled

**Advanced Configuration:**
- 2000 neurons, all plasticity mechanisms
- Higher dopamine modulation

**Focused Configuration:**
- 200 neurons, focused STDP learning
- Novelty only (no curiosity)

#### `createTrainingBrain(neuronCount=1000, enableDevelopment=True)`
Creates a brain specifically optimized for training/learning.

**Parameters:**
- `neuronCount`: Number of neurons (default: 1000)
- `enableDevelopment`: Whether to start in CriticalPeriod (default: True)

### Experiment Template APIs

#### `createExplorationExperiment()`
Creates a complete exploration experiment setup.

**Returns:** Tuple of (agent, brain, world)

**Configuration:**
- 1500 neurons with structural plasticity
- CriticalPeriod developmental stage
- Full neuromodulation suite

#### `createMemoryExperiment()`
Creates a memory consolidation experiment setup.

**Returns:** Tuple of (agent, brain, world)

**Configuration:**
- 2000 neurons for memory storage
- Maturation developmental stage
- Strong curiosity drive

### Training Loop APIs

#### `runEpisode(agent, world, maxSteps=1000, verbose=False)`
Runs a complete reinforcement learning episode.

**Parameters:**
- `agent`: AgentBrain instance
- `world`: SimpleWorld instance  
- `maxSteps`: Maximum steps per episode
- `verbose`: Enable progress logging

**Returns:** Total reward accumulated during episode

#### `runTrainingLoop(agent, world, numEpisodes, stepsPerEpisode, verbose=False)`
Runs multiple episodes with detailed tracking.

**Parameters:**
- `agent`: AgentBrain instance
- `world`: SimpleWorld instance
- `numEpisodes`: Number of episodes to run
- `stepsPerEpisode`: Steps per episode
- `verbose`: Enable progress logging

**Returns:** Tuple of (episode_rewards, average_rewards)

### Memory Management APIs

#### `saveBrainCheckpoint(brain, filepath)`
Saves brain state for checkpointing and resume capability.

**Parameters:**
- `brain`: Brain instance to save
- `filepath`: Path to save checkpoint file

#### `loadBrainCheckpoint(filepath, config=None)`
Loads a saved brain checkpoint.

**Parameters:**
- `filepath`: Path to checkpoint file
- `config`: Optional Config (uses default if not provided)

**Returns:** Loaded Brain instance

#### `createMemoryReplay(brain, replaySteps=100, replayRate=0.5)`
Replays stored experiences for memory consolidation.

**Parameters:**
- `brain`: Brain instance with memory system
- `replaySteps`: Number of replay steps
- `replayRate`: Speed of replay (0.5 = half speed)

**Returns:** Status message

### Visualization and Monitoring APIs

#### `collectBrainMetrics(brain)`
Collects comprehensive brain metrics for visualization.

**Parameters:**
- `brain`: Brain instance to analyze

**Returns:** Dictionary containing metrics:
- `total_neurons`: Total neuron count
- `total_synapses`: Total synapse count
- `active_neurons`: Currently active neurons
- `firing_neurons`: Currently firing neurons
- `average_firing_rate`: Average firing rate
- `e_i_ratio`: Excitation/inhibition ratio
- `total_spikes`: Total spike count
- `developmental_stage`: Current developmental stage (0-4)

#### `createVisualizationData(brain, steps, timestep=0.1)`
Generates time series data for visualization.

**Parameters:**
- `brain`: Brain instance
- `steps`: Number of simulation steps
- `timestep`: Time between steps (default: 0.1)

**Returns:** List of time series points with metrics

### Debugging and Monitoring APIs

#### `inspectBrainState(brain)`
Logs detailed brain state information for debugging.

**Parameters:**
- `brain`: Brain instance to inspect

**Returns:** Status message

#### `checkBrainHealth(brain, neuronHealthThreshold=0.1, synapseHealthThreshold=0.05)`
Checks brain system health.

**Parameters:**
- `brain`: Brain instance to check
- `neuronHealthThreshold`: Neuron health threshold (0.1 = 10%)
- `synapseHealthThreshold`: Synapse health threshold (0.05 = 5%)

**Returns:** Boolean indicating if brain is healthy

#### `debugAgentWorldInteraction(agent, world, steps=100)`
Debugs agent-world interaction with detailed logging.

**Parameters:**
- `agent`: AgentBrain instance
- `world`: SimpleWorld instance
- `steps`: Number of steps to debug

**Returns:** Status message

### Advanced APIs

#### `createMultiRegionBrain(regionNames)`
Creates a brain with multiple specialized regions.

**Parameters:**
- `regionNames`: List of region names to create

#### `enableAdvancedLearning(brain, enablePredictiveLearning=True, enableMetaLearning=True)`
Enables advanced learning mechanisms.

**Parameters:**
- `brain`: Brain instance
- `enablePredictiveLearning`: Enable prediction system
- `enableMetaLearning`: Enable meta-learning capabilities

#### `createCustomExperiment(name, setup, run)`
Creates a custom experiment with setup and run functions.

**Parameters:**
- `name`: Experiment name
- `setup`: Setup function
- `run`: Run function

## Benefits of High-Level APIs

1. **Reduced Boilerplate:** Eliminate repetitive setup code
2. **Sensible Defaults:** Optimized configurations for common use cases
3. **Consistent Patterns:** Standard interfaces across experiment types
4. **Built-in Best Practices:** Pre-configured with proven settings
5. **Easy Debugging:** Built-in monitoring and health checking
6. **Flexible Scaling:** From quick prototypes to production training
7. **Research Ready:** Includes advanced features for sophisticated experiments

## Migration Guide

### From Traditional to High-Level API

**Traditional (6 lines):**
```python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

**High-Level (1 line):**
```python
agent, brain, world = pynlm.createExplorationExperiment()
```

**Traditional (training loop):**
```python
for step in range(max_steps):
    world.update(timestep)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, time)
```

**High-Level (one function call):**
```python
reward = pynlm.runEpisode(agent, world, maxSteps=1000)
```

## Performance Considerations

- **Quick APIs** are optimized for rapid prototyping
- **Training APIs** include monitoring overhead for analysis
- **Advanced APIs** provide flexibility at the cost of simplicity
- Always call `initialize()` before simulation
- Use checkpointing for long-running experiments

## Best Practices

1. **Always initialize** before running simulations
2. **Use checkpointing** for long experiments
3. **Monitor metrics** regularly during training
4. **Start with simple configurations** before scaling up
5. **Use verbose mode** during debugging
6. **Check brain health** before large simulations
7. **Save intermediate results** frequently
