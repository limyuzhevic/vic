# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

---

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

---

## Advanced Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

### Project 4: Advanced Agent with Complete Learning

```python
import pynlm
import numpy as np

def run_advanced_agent():
    """Run an advanced agent with all learning systems enabled."""
    
    # Create configuration with enhanced settings
    config = pynlm.createConfigWithDefaults()
    
    # Customize configuration for specific task
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.08f)
    config.set("plasticity.stdp.learning_rate", 0.002f)
    config.set("plasticity.hebbian.learning_rate", 0.001f)
    
    # Create and initialize systems
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(pynlm.createWorldWithDefaults())
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Advanced agent initialized with enhanced configuration")
    print(f"Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    # Run learning episode
    num_steps = 1000
    rewards = []
    curiosities = []
    novelties = []
    
    for step in range(num_steps):
        # Update world
        agent.world.update(0.1f)
        
        # Get sensory input
        percept = agent.world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step, agent.world.getSimulationTime())
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        agent.world.applyMotorCommand(action, agent.world.getSimulationTime())
        
        # Calculate reward based on agent state
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        reward = curiosity * 0.1f + novelty * 0.05f
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0f)
        
        # Update development
        agent.updateDevelopment(0.1f)
        
        # Record metrics
        rewards.append(reward)
        curiosities.append(curiosity)
        novelties.append(novelty)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: Reward={reward:.3f}, "
                  f"Curiosity={curiosity:.3f}, Novelty={novelty:.3f}")
    
    # Return training statistics
    return {
        'total_reward': sum(rewards),
        'average_reward': np.mean(rewards),
        'max_reward': np.max(rewards),
        'min_reward': np.min(rewards),
        'curiosity_trend': curiosities[-100:],
        'novelty_trend': novelties[-100:],
        'final_brain_state': {
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount(),
            'spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio()
        }
    }

if __name__ == "__main__":
    results = run_advanced_agent()
    print(f"\nTraining Results:")
    print(f"Total Reward: {results['total_reward']:.3f}")
    print(f"Average Reward: {results['average_reward']:.3f}")
    print(f"Max Reward: {results['max_reward']:.3f}")
    print(f"Min Reward: {results['min_reward']:.3f}")
    print(f"Final E/I Ratio: {results['final_brain_state']['e_i_ratio']:.3f}")
```

### Project 5: System Validation and Monitoring

```python
import pynlm
import time
import numpy as np

def validate_and_monitor_brain():
    """Validate brain configuration and monitor system health."""
    
    # Create brain with configuration
    config = pynlm.createConfigWithDefaults()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Validate brain state
    is_valid = pynlm.validateBrainState(brain)
    print(f"Brain validation: {'PASSED' if is_valid else 'FAILED'}")
    
    # Get comprehensive statistics
    stats = pynlm.getSystemStatistics(brain, None)
    
    # Monitor system over time
    monitoring_duration = 5.0  # seconds
    samples = []
    
    start_time = time.time()
    while time.time() - start_time < monitoring_duration:
        step = len(samples)
        brain.step(step)
        
        # Collect system metrics
        current_stats = {
            'step': step,
            'time': time.time() - start_time,
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'pending_spikes': brain.getPendingSpikeEventCount()
        }
        
        samples.append(current_stats)
        time.sleep(0.1)  # Sample every 100ms
    
    # Analyze monitoring data
    print(f"\nSystem Monitoring Results ({len(samples)} samples):")
    print(f"Average Firing Rate: {np.mean([s['average_firing_rate'] for s in samples]):.3f}")
    print(f"Average E/I Ratio: {np.mean([s['e_i_ratio'] for s in samples]):.3f}")
    print(f"Spike Activity Range: {min(s['total_spikes'] for s in samples)} - {max(s['total_spikes'] for s in samples)}")
    print(f"Average Pending Spikes: {np.mean([s['pending_spikes'] for s in samples]):.1f}")
    
    # Detect anomalies
    avg_firing_rate = np.mean([s['average_firing_rate'] for s in samples])
    high_firing_episodes = sum(1 for s in samples if s['average_firing_rate'] > avg_firing_rate * 1.5)
    
    print(f"\nAnomaly Detection:")
    print(f"High firing rate episodes: {high_firing_episodes} out of {len(samples)}")
    print(f"Validation status: {'Good' if is_valid and high_firing_episodes < len(samples) * 0.1 else 'Needs Attention'}")
    
    return samples, is_valid

if __name__ == "__main__":
    samples, is_valid = validate_and_monitor_brain()
```

### Project 6: Advanced System Control

```python
import pynlm
from pathlib import Path

def advanced_system_control_example():
    """Demonstrate advanced system control and management."""
    
    # Create multiple brain configurations
    configs = [
        pynlm.createConfigWithDefaults(),  # Default config
        pynlm.createDefaultConfig(),       # Minimal config
    ]
    
    # Customize second config for high plasticity
    configs[1].set("plasticity.stdp.learning_rate", 0.01f)
    configs[1].set("plasticity.hebbian.learning_rate", 0.005f)
    configs[1].set("plasticity.structural.synaptogenesis_rate", 0.001f)
    
    brains = []
    agents = []
    worlds = []
    
    # Create and initialize multiple system instances
    for i, config in enumerate(configs):
        print(f"Creating system instance {i + 1}...")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createWorldWithDefaults()
        
        agent.initialize(world)
        
        # Enable different learning systems based on config
        if i == 0:
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
        else:
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
        
        brains.append(brain)
        agents.append(agent)
        worlds.append(world)
        
        print(f"  Initialized: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    # Run comparative experiment
    print(f"\nRunning comparative experiment...")
    
    for episode in range(5):
        print(f"\nEpisode {episode + 1}:")
        
        for i, (brain, agent, world) in enumerate(zip(brains, agents, worlds)):
            # Reset systems
            pynlm.resetSystem(brain, agent, world)
            
            # Run episode
            for step in range(100):
                world.update(0.1f)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Different reward strategies per system
                if i == 0:  # First system: curiosity-based rewards
                    reward = agent.getCuriosityLevel() * 0.1f
                else:  # Second system: novelty-based rewards
                    reward = agent.getNoveltyLevel() * 0.05f
                
                agent.applyRewardModulation(reward, 0.0f)
                agent.updateDevelopment(0.1f)
            
            # Get performance metrics
            stats = pynlm.getSystemStatistics(brain, agent)
            print(f"  System {i + 1}: Reward={stats['curiosity']:.3f}, "
                  f"Novelty={stats['novelty']:.3f}, "
                  f"DevStage={stats['developmental_stage']}")
    
    # Save final states
    for i, (brain, agent, world) in enumerate(zip(brains, agents, worlds)):
        pynlm.saveBrainWithTimestamp(brain, f"system_{i + 1}_final")
    
    print(f"\nComparative experiment completed!")
    print("All system states saved with timestamps.")
    
    return brains, agents, worlds

if __name__ == "__main__":
    advanced_system_control_example()
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
