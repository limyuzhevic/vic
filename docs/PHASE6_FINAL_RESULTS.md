# NLM - Phase 6: Integration Examples

This document provides practical examples of how the Phase 6 integrated brain system works in different scenarios.

## Example 1: Simple Learning Agent

This example demonstrates a brain learning to navigate a simple environment through trial and error.

```python
import pynlm
import time

# Create and configure the agent
config = pynlm.createDefaultConfig()
# Configure brain with memory systems
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.structural.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("memory.episodic.max_episodes", 100)
config.set("memory.working.capacity", 100)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run learning simulation
print("Starting learning simulation...")
print(f"Initial neurons: {brain.getTotalNeuronCount()}")
print(f"Initial synapses: {brain.getTotalSynapseCount()}")

episode = 0
max_episodes = 50

for episode in range(max_episodes):
    # Reset world for each episode
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Run episode steps
    for step in range(100):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processes information
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning from outcome
        reward = 0.0
        if action.getType() == pynlm.ActionType.Interact:
            reward = 1.0  # Positive reward for interaction
        
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    # Log progress every 10 episodes
    if episode % 10 == 0:
        print(f"Episode {episode}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
        
        # Save checkpoint periodically
        if episode % 20 == 0 and episode > 0:
            checkpoint_file = f"checkpoint_episode_{episode}.bin"
            if brain.save(checkpoint_file):
                print(f"  Checkpoint saved: {checkpoint_file}")

print(f"Learning simulation completed!")
print(f"Final development stage: {brain.getDevelopmentalStage()}")
```

## Example 2: Working Memory and Attention Demonstration

This example shows how working memory and attention mechanisms work together to process sequential information.

```python
import pynlm
import numpy as np

# Create a brain with enhanced working memory
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.03)
config.set("memory.working.capacity", 500)  # Large working memory
config.set("attention.inhibition_strength", 0.7)
config.set("attention.excitation_strength", 1.3)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()
agent.initialize(world)

agent.enableRewardModulation(False)  # Focus on memory without reward
agent.enableDevelopment(False)       # Static for this experiment

print("Working Memory and Attention Demonstration")
print("=" * 50)

# Create a visual pattern to remember
pattern_sequence = [
    [1.0, 0.0, 0.0, 1.0, 0.0],
    [0.0, 1.0, 0.0, 0.0, 1.0],
    [1.0, 0.0, 1.0, 0.0, 0.0]
]

# Simulate pattern presentation with delays
total_patterns = 5
repetition_count = 3

for rep in range(repetition_count):
    print(f"\nRepetition {rep + 1}:")
    
    for pattern_idx, pattern in enumerate(pattern_sequence):
        # Create visual input from pattern
        vision_data = []
        for row in pattern:
            vision_data.extend(row * 10.0)  # Scale to [0, 10] mV
        
        # Add small noise to simulate realism
        vision_noise = [np.random.normal(v, 0.5) for v in vision_data]
        
        # Create sensory percept
        from pynlm import Vision
        percept = Vision(6, 5, 3)  # 6x5 grid, 3 channels (RGB simulation)
        
        # Set visual data (simplified)
        percept.setData(vision_noise)
        
        # Present pattern to brain
        agent.processSensoryInput(percept)
        
        # Process the pattern
        brain.step(pattern_idx * 10)
        
        # Record working memory contents
        if brain.getWorkingMemory():
            traces = brain.getWorkingMemory().getActiveTraces()
            print(f"  Pattern {pattern_idx + 1}: {traces} active traces")
        
        # Simulate delay between patterns
        for delay_step in range(20):
            world.update(0.1)
            agent.processSensoryInput(percept)
            brain.step(pattern_idx * 10 + delay_step)
    
    # Clear working memory for next repetition
    if brain.getWorkingMemory():
        brain.getWorkingMemory().clear()

print(f"\nMemory Statistics:")
print(f"  Final working memory traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0}")
print(f"  Final episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
print(f"  Attention effectiveness: {(brain.getFiringNeuronCount() / brain.getTotalNeuronCount()) * 100:.1f}%")
```

## Example 3: Developmental Learning

This example demonstrates how the brain develops over time, changing its plasticity and behavior.

```python
import pynlm
import matplotlib.pyplot as plt

# Create brain with developmental configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 800)
config.set("brain.synapse_density", 0.04)
config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.0001)
config.set("plasticity.structural.pruning_rate", 0.00005)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
world.reset()
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

print("Developmental Learning Simulation")
print("=" * 40)

# Track development metrics
development_data = []
simulation_steps = 5000

for step in range(simulation_steps):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process input
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward (sparse, infrequent rewards)
    if step % 100 == 0 and step > 0:
        reward = np.random.random() * 2.0 - 1.0  # Random reward between -1 and 1
        agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Record metrics every 100 steps
    if step % 100 == 0:
        development_data.append({
            'step': step,
            'developmental_stage': brain.getDevelopmentalStage(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'average_firing': brain.getAverageFiringRate(),
            'plasticity': agent.getNeuromodulationLevel()
        })
        
        # Log developmental stage
        if step % 500 == 0:
            print(f"Step {step}: {brain.getDevelopmentalStage().name}, "
                  f"Plasticity: {agent.getNeuromodulationLevel():.3f}, "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}")

print(f"\nDevelopment Complete")
print(f"Final developmental stage: {brain.getDevelopmentalStage().name}")
print(f"Total simulation steps: {simulation_steps}")

# Create development summary
print(f"\nDevelopment Summary:")
for i, data in enumerate(development_data[::5]):  # Sample every 5 entries
    print(f"  Step {data['step']}: {data['developmental_stage'].name}, "
          f"Firing: {data['firing_neurons']}, "
          f"Plasticity: {data['plasticity']:.3f}")
```

## Example 4: Prediction and Planning

This example shows how the brain uses prediction and planning to select optimal actions.

```python
import pynlm

# Create brain with prediction capabilities
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1200)
config.set("brain.synapse_density", 0.03)
config.set("prediction.horizon", 5)  # Multi-step prediction
config.set("planning.search_depth", 3)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=12, height=12, visionWidth=5, visionHeight=5)
world.reset()
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableDevelopment(True)

print("Prediction and Planning Demonstration")
print("=" * 45)

# Create a sequence of actions to learn
skill_sequence = [
    (pynlm.ActionType.MoveForward, 2.0),  # Move forward 2 units
    (pynlm.ActionType.TurnLeft, 0.5),   # Turn left
    (pynlm.ActionType.MoveForward, 1.5), # Move forward
    (pynlm.ActionType.Interact, 0.0),    # Interact with environment
]

# Simulate learning of action sequences
total_cycles = 100

for cycle in range(total_cycles):
    # Present action sequence
    for action_idx, (action_type, duration) in enumerate(skill_sequence):
        # Create action command
        action = pynlm.Action(action_type)
        
        # Execute for multiple steps
        for step in range(int(duration * 10)):  # 10 steps per time unit
            world.update(0.1)
            
            # Get current percept
            percept = world.getSensoryPercept()
            
            # Process
            agent.processSensoryInput(percept)
            brain.step(cycle * 100 + step)
            
            # Select action based on brain activity
            selected_action = agent.decodeMotorCommand()
            world.applyMotorCommand(selected_action, world.getSimulationTime())
            
            # Provide reward if correct action is selected
            if selected_action.getType() == action_type:
                agent.applyRewardModulation(1.0, 0.0)  # Positive reward
            else:
                agent.applyRewardModulation(-0.1, 0.0)  # Small penalty
            
            # Update development
            agent.updateDevelopment(0.1)
    
    # Check planning effectiveness
    if cycle % 20 == 0:
        planning_score = agent.getPredictionError()
        print(f"Cycle {cycle}: Planning error: {planning_score:.3f}, "
              f"Stage: {brain.getDevelopmentalStage().name}")

print(f"\nPlanning Performance Summary:")
print(f"  Final planning error: {agent.getPredictionError():.3f}")
print(f"  Final developmental stage: {brain.getDevelopmentalStage().name}")
print(f"  Average curiosity: {sum(d['curiosity'] for d in development_data) / len(development_data):.3f}")
```

## Example 5: Memory Replay and Consolidation

This example demonstrates how the brain replays and consolidates memories during rest.

```python
import pynlm
import time

# Create brain with strong memory capabilities
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)
config.set("brain.synapse_density", 0.02)
config.set("replay_interval", 50)        # Replay every 50 steps
config.set("consolidation_interval", 500) # Consolidate every 500 steps
config.set("memory.episodic.max_episodes", 200)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=8, height=8, visionWidth=3, visionHeight=3)
world.reset()
agent.initialize(world)

agent.enableRewardModulation(False)  # Focus on memory without reward
agent.enableDevelopment(False)       # Static for memory study

print("Memory Replay and Consolidation")
print("=" * 35)

# Generate and store multiple episodes
episodes_generated = 0
replay_events = 0
consolidation_events = 0

total_simulation = 2000

for step in range(total_simulation):
    # Generate new episodes periodically
    if step % 20 == 0 and step > 0:
        # Create episode with current brain state
        # (In real implementation, this would capture full state)
        episodes_generated += 1
        print(f"Generated episode {episodes_generated} at step {step}")
    
    # Run normal world interaction
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check for replay events
    if step % config.getOr<size_t>("replay_interval", 50) == 0:
        replay_events += 1
        print(f"  Replay event at step {step} ({replay_events} total)")
        
        # Simulate replay (brain replays important memories)
        if brain.getEpisodicMemory():
            episodes_to_replay = brain.getEpisodicMemory().getEpisodesForReplay(3)
            print(f"    Replaying {len(episodes_to_replay)} episodes")
    
    # Check for consolidation events
    if step % config.getOr<size_t>("consolidation_interval", 500) == 0:
        consolidation_events += 1
        print(f"  Consolidation event at step {step} ({consolidation_events} total)")
        
        # Simulate memory consolidation
        if brain.getEpisodicMemory():
            important_episodes = brain.getEpisodicMemory().getImportantEpisodes()
            print(f"    Consolidated {len(important_episodes)} important episodes")
    
    # Log memory statistics periodically
    if step % 300 == 0:
        print(f"  Memory stats - Episodes: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}, "
              f"Working traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0}")

print(f"\nMemory System Summary:")
print(f"  Episodes generated: {episodes_generated}")
print(f"  Replay events: {replay_events}")
print(f"  Consolidation events: {consolidation_events}")
print(f"  Final memory efficiency: {MemoryManagement::calculateMemoryEfficiency(*brain.getEpisodicMemory()) if brain.getEpisodicMemory() else 0:.1f}%")
```

## Example 6: Neuromodulation Effects

This example demonstrates how neuromodulators (dopamine, curiosity, novelty) affect brain behavior.

```python
import pynlm
import numpy as np

# Create brain with enhanced neuromodulation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.exploration_bonus", 2.0)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.sensitivity", 1.2)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
world.reset()
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print("Neuromodulation Effects Demonstration")
print("=" * 40)

# Create environment with varying levels of novelty and exploration
world_scenarios = [
    ("Familiar Room", np.array([1.0, 0.0, 0.0])),
    ("New Environment", np.array([0.2, 0.5, 0.8])),
    ("Highly Novel", np.array([0.9, 0.8, 0.7])),
    ("Structured Task", np.array([0.8, 0.3, 0.2])),
]

for scenario_name, novelty_profile in world_scenarios:
    print(f"\nScenario: {scenario_name}")
    print(f"  Novelty profile: [{novelty_profile[0]:.1f}, {novelty_profile[1]:.1f}, {novelty_profile[2]:.1f}]")
    
    # Run simulation in this scenario
    scenario_steps = 200
    for step in range(scenario_steps):
        world.update(0.1)
        
        # Modify environment based on novelty
        if scenario_name == "Highly Novel":
            # Add random exploration opportunities
            if np.random.random() < 0.3:
                world.setAgentStart(np.random.random() * 8, np.random.random() * 8)
        
        percept = world.getSensoryPercept()
        
        # Apply neuromodulation based on scenario
        if scenario_name == "Familiar Room":
            # Low exploration in familiar environments
            pass
        elif scenario_name == "New Environment":
            # Moderate exploration for new areas
            pass
        elif scenario_name == "Highly Novel":
            # High curiosity-driven exploration
            pass
        elif scenario_name == "Structured Task":
            # Goal-directed exploration
            pass
        
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = np.dot(action.getParameters(), novelty_profile) * 0.1
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    # Record neuromodulation levels
    print(f"  Neuromodulation levels - Dopamine: {brain.getDopamine().getLevel():.3f}, "
          f"Curiosity: {agent.getCuriosityLevel():.3f}, "
          f"Novelty: {agent.getNoveltyLevel():.3f}")

print(f"\nNeuromodulation Summary:")
print(f"  Average dopamine level: {brain.getDopamine().getLevel():.3f}")
print(f"  Maximum curiosity reached: {agent.getCuriosityLevel():.3f}")
print(f"  Novelty sensitivity: {agent.getNoveltyLevel():.3f}")
print(f"  Final developmental stage: {brain.getDevelopmentalStage().name}")
```

## Best Practices for Using NLM Phase 6

### 1. Configuration Management
```python
# Always configure your brain before using it
config = pynlm.createDefaultConfig()

# Set appropriate values for your use case
config.set("brain.neuron_count", 1000)  # Adjust based on complexity
config.set("brain.synapse_density", 0.05)  # Connectivity
config.set("memory.working.capacity", 100)  # Working memory size
config.set("plasticity.structural.enable", True)  # Allow structural changes
config.set("neuromod.curiosity.enable", True)  # Enable exploration

brain = pynlm.createBrain(config)
```

### 2. Simulation Best Practices
```python
# Always initialize before use
brain.initialize()

# Use consistent timestep for stability
time_step = 0.1

# Progress monitoring
for step in range(total_steps):
    world.update(time_step)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Log periodically
    if step % 100 == 0:
        print(f"Step {step}: Firing {brain.getFiringNeuronCount()} neurons")
```

### 3. Learning and Development
```python
# Enable subsystems for learning
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Monitor development
for step in range(total_steps):
    agent.updateDevelopment(time_step)
    
    # Save checkpoints for later analysis
    if step % 1000 == 0:
        brain.save(f"checkpoint_step_{step}.bin")
```

### 4. Performance Considerations
```python
# Use appropriate neuron count for your task
small_brain = pynlm.createBrain(config)  # ~500 neurons
medium_brain = pynlm.createBrain(config) # ~1000 neurons
large_brain = pynlm.createBrain(config)  # ~2000 neurons

# Monitor performance metrics
print(f"Brain status: {brain.getTotalNeuronCount()} neurons, "
      f"{brain.getTotalSynapseCount()} synapses, "
      f"Average firing: {brain.getAverageFiringRate():.2f}")
```

### 5. Error Handling
```python
# Always check for errors
try:
    brain.initialize()
    # Your simulation code here
except Exception as e:
    print(f"Error: {e}")
    # Handle error appropriately

# Check brain health
if not brain.hasMemory():
    print("Warning: Brain memory system not properly initialized")
```

## Quick Reference Guide

### Key Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.05 | Connectivity probability |
| `memory.working.capacity` | int | neuron_count/10 | Working memory size |
| `memory.episodic.max_episodes` | int | 1000 | Maximum stored experiences |
| `plasticity.structural.enable` | bool | true | Allow structural changes |
| `neuromod.curiosity.enable` | bool | true | Enable exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `replay_interval` | int | 100 | Steps between memory replay |
| `consolidation_interval` | int | 1000 | Steps between consolidation |

### Common Actions

```python
# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(total_steps):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(reward, predicted_reward)
    agent.updateDevelopment(0.1)
```

### Memory Management

```python
# Check memory status
if brain.getWorkingMemory():
    print(f"Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
    print(f"Working memory capacity: {brain.getWorkingMemory().getCapacity()}")

if brain.getEpisodicMemory():
    print(f"Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
    print(f"Max episodes: {brain.getEpisodicMemory().getMaxEpisodes()}")

if brain.getAssociativeMemory():
    print(f"Associative memory associations: {brain.getAssociativeMemory().getAssociationCount()}")
```

### Development Tracking

```python
# Monitor developmental progression
stage = brain.getDevelopmentalStage()
print(f"Current developmental stage: {stage.name}")

if stage == pynlm.DevelopmentalStage.CriticalPeriod:
    print("Critical period: high plasticity, rapid learning")
elif stage == pynlm.DevelopmentalStage.Adult:
    print("Adult stage: stable behavior, slower learning")
```

### Performance Monitoring

```python
# Check brain performance metrics
print(f"Brain Status:")
print(f"  Neurons: {brain.getTotalNeuronCount()}")
print(f"  Synapses: {brain.getTotalSynapseCount()}")
print(f"  Active neurons: {brain.getActiveNeuronCount()}")
print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Pending events: {brain.getPendingSpikeEventCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")

# Memory system status
print(f"\nMemory System:")
if brain.getWorkingMemory():
    print(f"  Working memory: {brain.getWorkingMemory().getActiveTraces()} active traces")
if brain.getEpisodicMemory():
    print(f"  Episodic memory: {brain.getEpisodicMemory().getEpisodeCount()} episodes")
if brain.getAssociativeMemory():
    print(f"  Associative memory: {brain.getAssociativeMemory().getAssociationCount()} associations")

# Neuromodulation status
print(f"\nNeuromodulation:")
if brain.getDopamine():
    print(f"  Dopamine: {brain.getDopamine().getLevel():.3f}")
if brain.getCuriosity():
    print(f"  Curiosity: {brain.getCuriosity().getLevel():.3f}")
if brain.getNovelty():
    print(f"  Novelty: {brain.getNovelty().getLevel():.3f}")
```

## Troubleshooting

### Common Issues and Solutions

**Issue: Brain not firing neurons**
```
# Check initialization
brain.initialize()

# Verify configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Ensure sensory input is provided
agent.processSensoryInput(world.getSensoryPercept())
```

**Issue: No learning occurring**
```python
# Check that learning subsystems are enabled
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Provide appropriate rewards
if action_correct:
    agent.applyRewardModulation(1.0, 0.0)
else:
    agent.applyRewardModulation(-0.1, 0.0)
```

**Issue: Memory not being stored**
```python
# Check memory system configuration
config.set("memory.episodic.max_episodes", 1000)
config.set("replay_interval", 100)

# Verify memory system is working
if brain.getEpisodicMemory():
    print(f"Episodic memory: {brain.getEpisodicMemory().getEpisodeCount()} episodes")
```

**Issue: High memory usage**
```python
# Reduce working memory capacity
config.set("memory.working.capacity", 100)

# Limit episodic memory
config.set("memory.episodic.max_episodes", 500)
```

### Performance Optimization Tips

1. **Use appropriate neuron count**: Start with 1000 neurons and adjust based on task complexity
2. **Monitor memory usage**: Check working memory capacity and episodic memory limits
3. **Balance exploration and exploitation**: Use curiosity for exploration, reward for exploitation
4. **Save checkpoints periodically**: Preserve important brain states for later analysis
5. **Profile simulation performance**: Identify bottlenecks and optimize

## Conclusion

Phase 6 represents the complete integration of all neural systems into a coherent artificial brain. The examples above demonstrate how memory, prediction, cognition, and neuromodulation work together to create intelligent behavior. By understanding these integration mechanisms, you can design and implement sophisticated brain simulations for research, education, or practical applications.

The NLM framework provides a powerful platform for studying biological intelligence and developing artificial systems that learn, adapt, and grow from experience.
