# NLM - Advanced Usage Guide for Power Users

## What This Guide Covers

This guide goes beyond the basics and shows you how to use advanced NLM features, configure complex experiments, and write more sophisticated agent behaviors.

---

## Advanced Configuration Management

### Setting Configuration with Categories

Configuration keys can be organized using dot notation for better organization:

```python
import pynlm

# Create configuration with categorized keys
config = pynlm.createDefaultConfig()

# General settings
config.set("neuron_count", 2000)
config.set("region_count", 2)
config.set("connection_probability", 0.15)

# Brain-specific settings
config.set("brain.v_thresh", -50.0)
config.set("brain.v_rest", -70.0)
config.set("brain.v_reset", -75.0)

# Plasticity settings
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Neuromodulation settings
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Memory settings
config.set("memory.working.capacity", 200)
config.set("memory.episodic.max_episodes", 1000)

# Save configuration with descriptions
config.saveToFile("advanced_config.json")
```

### Configuration Validation

Validate your configuration before using it:

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("region_count", 2)

# Validate entire configuration
if config.validate():
    print("Configuration is valid!")
else:
    print("Configuration has errors - see log for details")

# Validate specific category
brain_keys = config.getKeysInCategory("brain")
if config.validate("brain"):
    print("Brain configuration is valid")
```

### Getting Configuration Information

Explore your configuration settings:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Get all categories
print("Categories:", config.getCategories())

# Get keys in specific category
brain_keys = config.getKeysInCategory("brain")
print("Brain keys:", brain_keys)

# Get descriptions for keys
descriptions = config.getAllDescriptions()
for key, desc in descriptions.items():
    print(f"{key}: {desc}")

# Get default values
default_neuron_count = config.getDefault("neuron_count")
print(f"Default neuron count: {default_neuron_count}")
```

---

## Advanced Agent Behaviors

### Using the Convenience Functions

The enhanced Python bindings provide several convenience functions:

#### Running Complete Episodes

```python
import pynlm

# Create and configure everything
brain, world, agent = pynlm.createAgentSetup(
    neuronCount=1500,
    worldWidth=30,
    worldHeight=30
)

# Configure with custom settings
brain.getConfig().set("brain.neuron_count", 1500)
brain.getConfig().set("plasticity.stdp.learning_rate", 0.002)

# Run an episode with learning enabled
stats = pynlm.runAgentEpisode(
    brain=agent.getBrain(),
    world=world,
    agent=agent,
    numSteps=500,
    enableLearning=True
)

print(f"Episode completed!")
print(f"Total spikes: {stats[0]}")
print(f"Average firing rate: {stats[1]:.2f} Hz")
print(f"Total reward: {stats[2]:.2f}")
print(f"Curiosity level: {stats[3]:.2f}")
print(f"Novelty level: {stats[4]:.2f}")
```

#### Getting Comprehensive Brain Statistics

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get comprehensive statistics
stats = pynlm.getBrainStats(brain)

print("Brain Statistics:")
print(f"  Total neurons: {stats['total_neurons']}")
print(f"  Total synapses: {stats['total_synapses']}")
print(f"  Firing neurons: {stats['firing_neurons']}")
print(f"  Active neurons: {stats['active_neurons']}")
print(f"  Total spikes: {stats['total_spikes']}")
print(f"  Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
print(f"  E/I ratio: {stats['e/i_ratio']:.2f}")
print(f"  Pending spikes: {stats['pending_spikes']}")

if 'working_memory_size' in stats:
    print(f"  Working memory capacity: {stats['working_memory_size']}")

if 'episodic_episodes' in stats:
    print(f"  Episodic episodes stored: {stats['episodic_episodes']}")

if 'planner_action_count' in stats:
    print(f"  Planner action count: {stats['planner_action_count']}")
```

#### Saving and Loading Agent State

```python
import pynlm

# Create and run an agent
brain, world, agent = pynlm.createAgentSetup()

# Run some steps to generate state
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Save state
pynlm.saveAgentState(
    brain=agent.getBrain(),
    world=world,
    brainFile="agent_brain.checkpoint",
    worldFile="agent_world.state"
)

print("Agent state saved!")

# Later, load state
pynlm.loadAgentState(
    brain=agent.getBrain(),
    world=world,
    brainFile="agent_brain.checkpoint",
    worldFile="agent_world.state"
)

print("Agent state loaded - brain re-initialized")
```

### Advanced Action Selection

Using custom action probabilities:

```python
import pynlm

# Define action probabilities (forward, backward, turn_left, turn_right, interact, wait)
probabilities = [0.4, 0.1, 0.1, 0.1, 0.2, 0.1]  # Total: 1.0

# Generate random action based on probabilities
for i in range(10):
    action = pynlm.generateRandomAction(probabilities)
    action_name = str(action).split('.')[-1]
    print(f"Random action: {action_name}")

# Example with learning-adjusted probabilities
probabilities_with_learning = [0.3, 0.05, 0.05, 0.05, 0.4, 0.15]  # Higher interact if learned
```

---

## Advanced Brain Manipulation

### Brain Modification Functions

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Modify brain parameters directly
config = brain.getConfig()

# Increase neuron count dynamically
config.set("neuron_count", 3000)

# Adjust plasticity
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.hebbian.enable", True)

# Change neuromodulation
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.curiosity.enable", True)

print("Brain configuration modified!")
print(f"New neuron count: {config.getOr('neuron_count', 0)}")
```

### Working with Brain Regions

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access and modify brain regions
regions = brain.getRegions()
print(f"Number of regions: {brain.getRegionCount()}")

for i, region in enumerate(regions):
    print(f"Region {i+1}: {region.getTotalNeuronCount()} neurons")
    print(f"  Populations: {region.getPopulationCount()}")

# Add new region
new_region_id = brain.addRegion("cortex")
print(f"Added new region with ID: {new_region_id}")
```

---

## Experiment Configuration

### Setting Up Advanced Experiments

```python
import pynlm

# Create configuration for complex experiment
experiment_config = {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.12,
    "simulation_timestep": 0.001,
    "replay_interval": 50,
    "consolidation_interval": 500,
    "enable_checkpointing": True,
    "enable_replay": True,
    "enable_development": True,
    "plasticity.stdp.enable": True,
    "plasticity.stdp.learning_rate": 0.001,
    "plasticity.hebbian.enable": True,
    "neuromod.dopamine.scale": 1.5,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True,
}

# Apply configuration
config = pynlm.createDefaultConfig()
for key, value in experiment_config.items():
    config.set(key, value)

brain = pynlm.createBrain(config)
brain.initialize()

print("Advanced experiment configuration applied!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### Multi-Agent Experiment Setup

```python
import pynlm

def create_multi_agent_experiment(num_agents=3, neurons_per_agent=500):
    """Create a multi-agent experiment setup."""
    
    agents = []
    brains = []
    worlds = []
    
    for i in range(num_agents):
        # Create individual agent setup
        brain, world, agent = pynlm.createAgentSetup(
            neuronCount=neurons_per_agent,
            worldWidth=20,
            worldHeight=20
        )
        
        # Customize agent configuration
        config = brain.getConfig()
        config.set("agent.id", i)  # Unique agent ID
        config.set("agent.learning_rate", 0.001 / (i + 1))  # Different learning rates
        
        agents.append(agent)
        brains.append(brain)
        worlds.append(world)
    
    return brains, agents, worlds

# Run multi-agent experiment
brains, agents, worlds = create_multi_agent_experiment(2, 800)

# Run experiment
for episode in range(5):
    print(f"\n=== Episode {episode + 1} ===")
    
    for i, (brain, agent, world) in enumerate(zip(brains, agents, worlds)):
        # Run individual agent episode
        stats = pynlm.runAgentEpisode(
            brain=agent.getBrain(),
            world=world,
            agent=agent,
            numSteps=100,
            enableLearning=True
        )
        
        print(f"Agent {i+1}: {stats[2]:.2f} reward, {stats[1]:.2f} Hz firing rate")

print("\nMulti-agent experiment completed!")
```

---

## Debugging and Analysis Tools

### Brain State Analysis

```python
import pynlm
import json

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(200):
    brain.step(step)

# Analyze brain state
analysis = {
    "total_neurons": brain.getTotalNeuronCount(),
    "total_synapses": brain.getTotalSynapseCount(),
    "firing_neurons": brain.getFiringNeuronCount(),
    "active_neurons": brain.getActiveNeuronCount(),
    "total_spikes": brain.getTotalSpikeCount(),
    "avg_firing_rate": brain.getAverageFiringRate(),
    "e/i_ratio": brain.getExcitationInhibitionRatio(),
    "pending_spikes": brain.getPendingSpikeEventCount(),
    "developmental_stage": brain.getDevelopmentalStage(),
    "learned_preference": brain.getLearnedPreference(),
}

print("Brain State Analysis:")
print(json.dumps(analysis, indent=2))

# Check if brain is in stable state
is_stable = (
    analysis["pending_spikes"] < 100 and
    analysis["avg_firing_rate"] > 1.0 and
    analysis["firing_neurons"] > analysis["total_neurons"] * 0.1
)

print(f"\nBrain is {'stable' if is_stable else 'unstable'}")
```

### Memory System Analysis

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

print("Memory System Analysis:")

if working_memory:
    print(f"Working memory:")
    print(f"  Capacity: {working_memory.getMemoryCapacity()}")
    print(f"  Current size: {working_memory.getMemorySize()}")
    print(f"  Current neurons: {len(working_memory.getMemoryNeurons())}")

if episodic_memory:
    print(f"Episodic memory:")
    print(f"  Max episodes: {episodic_memory.getMaxEpisodes()}")
    print(f"  Current episodes: {episodic_memory.getEpisodeCount()}")
    print(f"  Memory efficiency: {episodic_memory.getMemoryEfficiency()}")

if associative_memory:
    print(f"Associative memory:")
    print(f"  Patterns stored: {associative_memory.getPatternCount()}")
    print(f"  Similarity threshold: {associative_memory.getSimilarityThreshold()}")

# Analyze memory integration
print(f"\nMemory integration indicators:")
print(f"  Working memory active: {working_memory and working_memory.getMemorySize() > 0}")
print(f"  Episodic memory populated: {episodic_memory and episodic_memory.getEpisodeCount() > 0}")
print(f"  Associative memory populated: {associative_memory and associative_memory.getPatternCount() > 0}")
```

---

## Performance Monitoring

### Real-time Brain Monitoring

```python
import pynlm
import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print("Starting performance monitoring...")
print("Step | Spikes | Firing Hz | E/I Ratio | Time (ms)")
print("-" * 60)

start_time = time.time()
last_log = start_time

for step in range(100):
    brain.step(step)
    
    current_time = time.time()
    elapsed = current_time - last_log
    
    if elapsed >= 1.0:  # Log every second
        firing_rate = brain.getAverageFiringRate()
        e_i_ratio = brain.getExcitationInhibitionRatio()
        time_ms = (current_time - start_time) * 1000
        
        print(f"{step:4d} | {brain.getTotalSpikeCount():6d} | "
              f"{firing_rate:9.2f} | {e_i_ratio:9.2f} | {time_ms:7.0f}")
        
        last_log = current_time

# Performance metrics
end_time = time.time()
total_time = end_time - start_time
total_spikes = brain.getTotalSpikeCount()

print(f"\nPerformance Summary:")
print(f"  Total simulation time: {total_time:.2f} seconds")
print(f"  Total spikes processed: {total_spikes}")
print(f"  Average spikes per second: {total_spikes / total_time:.0f}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Memory usage efficiency: {brain.getTotalSynapseCount() / brain.getTotalNeuronCount():.2f} synapses/neuron")
```

### Configuration Validation

```python
import pynlm

# Test different configuration scenarios
print("Testing configuration validation...")

# Valid configuration
config1 = pynlm.createDefaultConfig()
config1.set("neuron_count", 1000)
config1.set("region_count", 2)

if config1.validate():
    print("✓ Configuration 1: Valid")
else:
    print("✗ Configuration 1: Invalid")

# Configuration with empty key
config2 = pynlm.createDefaultConfig()
config2.set("", "invalid_value")  # Empty key should cause validation error

if config2.validate():
    print("✓ Configuration 2: Valid")
else:
    print("✗ Configuration 2: Invalid (as expected)")

# Category-specific validation
config3 = pynlm.createDefaultConfig()
config3.set("brain.neuron_count", 1500)
config3.set("plasticity.stdp.learning_rate", 0.002)

if config3.validate("brain"):
    print("✓ Configuration 3 brain category: Valid")
else:
    print("✗ Configuration 3 brain category: Invalid")

if config3.validate("plasticity"):
    print("✓ Configuration 3 plasticity category: Valid")
else:
    print("✗ Configuration 3 plasticity category: Invalid")
```

---

## Best Practices and Tips

### Configuration Best Practices

1. **Use dot notation for organization**: `brain.v_thresh`, `plasticity.stdp.learning_rate`
2. **Validate before use**: Always call `validate()` after setting configuration
3. **Use meaningful category names**: "brain", "plasticity", "memory", "neuromodulation"
4. **Document configuration keys**: Set descriptions for all custom configuration keys
5. **Use default values**: Get defaults before setting custom values

### Performance Tips

1. **Use appropriate neuron counts**: 1000-2000 neurons for most tasks, increase for complex behaviors
2. **Configure memory systems**: Set appropriate capacities for working and episodic memory
3. **Enable development**: Allow the brain to develop for better long-term performance
4. **Monitor plasticity**: Adjust STDP and Hebbian learning rates based on task requirements
5. **Use checkpointing**: Save brain state periodically for long-running experiments

### Debugging Tips

1. **Check firing rates**: Normal firing rates are 1-20 Hz, much higher indicates runaway excitation
2. **Monitor E/I balance**: Excitation/inhibition ratio should be around 1.0-2.0
3. **Track spike counts**: Sudden changes in spike generation indicate potential issues
4. **Validate memory integration**: Ensure memory systems are properly connected
5. **Use logging**: Enable detailed logging for troubleshooting

---

## Advanced Examples

### Complete Advanced Agent Example

```python
import pynlm
import time

def advanced_agent_example():
    """Complete example demonstrating advanced NLM features."""
    
    print("=== Advanced NLM Agent Example ===")
    
    # 1. Create advanced configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 2000)
    config.set("region_count", 3)
    config.set("connection_probability", 0.12)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # 2. Create agent setup
    brain, world, agent = pynlm.createAgentSetup(2000, 30, 30)
    
    # 3. Configure agent with custom settings
    agent_config = brain.getConfig()
    agent_config.set("agent.id", 1)
    agent_config.set("agent.learning_rate", 0.001)
    
    # 4. Initialize and run extended simulation
    brain.initialize()
    agent.initialize(*world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Agent initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"World size: {world.getWidth()} x {world.getHeight()}")
    
    # 5. Run extended simulation with monitoring
    total_reward = 0.0
    start_time = time.time()
    
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action and get reward
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Simple reward function
        reward = 0.1 if world.getAgentBody().x > 0 and world.getAgentBody().x < world.getWidth() else -0.1
        agent.applyRewardModulation(reward, 0.0)
        total_reward += reward
        
        # Log progress
        if step % 100 == 0:
            stats = pynlm.getBrainStats(brain)
            print(f"Step {step}: {stats['firing_neurons']} firing neurons, "
                  f"{stats['avg_firing_rate']:.2f} Hz, reward: {reward:.2f}")
    
    # 6. Analysis and reporting
    end_time = time.time()
    duration = end_time - start_time
    
    final_stats = pynlm.getBrainStats(brain)
    
    print(f"\n=== Simulation Complete ===")
    print(f"Duration: {duration:.2f} seconds")
    print(f"Total reward: {total_reward:.2f}")
    print(f"Average reward per step: {total_reward / 500:.3f}")
    print(f"Final firing rate: {final_stats['avg_firing_rate']:.2f} Hz")
    print(f"Total spikes: {final_stats['total_spikes']}")
    print(f"E/I ratio: {final_stats['e/i_ratio']:.2f}")
    
    # 7. Save results
    pynlm.saveAgentState(
        brain=agent.getBrain(),
        world=world,
        brainFile="advanced_agent_brain.checkpoint",
        worldFile="advanced_agent_world.state"
    )
    
    print(f"\nResults saved to checkpoint files!")
    
    return total_reward, duration, final_stats

# Run the example
advanced_agent_example()
```

---

## Constants and Available Values

### Motor Command Types

```python
# Motor command constants (from bindings.cpp)
pynlm.MotorCommand.MoveForward
pynlm.MotorCommand.MoveBackward
pynlm.MotorCommand.TurnLeft
pynlm.MotorCommand.TurnRight
pynlm.MotorCommand.LookLeft
pynlm.MotorCommand.LookRight
pynlm.MotorCommand.Interact
pynlm.MotorCommand.Wait
```

### Neuron Types

```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal
```

### Developmental Stages

```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

### Action Types

```python
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.MoveLeft
pynlm.ActionType.MoveRight
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.LookUp
pynlm.ActionType.LookDown
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
pynlm.ActionType.Custom
```

---

## Next Steps

When you're ready to use these advanced features:

1. **Read `HOW_TO_USE.md`**: For detailed technical documentation
2. **Read `docs/ARCHITECTURE.md`**: To understand system architecture
3. **Experiment with configurations**: Use the examples above as starting points
4. **Join the community**: Share your experiences and contribute back to the project
5. **Report issues**: If you find bugs or have feature requests

The advanced features in NLM make it a powerful tool for creating sophisticated artificial brains. Use these features responsibly and ethically.
