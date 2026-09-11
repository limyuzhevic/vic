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

### Example 3: Learning Brain with Reward (Advanced)

```python
import pynlm

# Create a brain with learning enabled
config = pynlm.createDefaultConfig()
config.set("plasticity_learning_rate", 0.01)  # Enable learning
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run learning simulation
print("Starting learning simulation...")
for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward based on agent's performance
    reward = 0.0
    if action != pynlm.MotorCommand.Wait:
        reward += 0.1  # Small reward for acting
    
    # Apply neuromodulation for learning
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)
    
    # Log progress every 50 steps
    if step % 50 == 0:
        print(f"  Step {step}: "
              f"Curiosity={agent.getCuriosityLevel():.2f}, "
              f"Novelty={agent.getNoveltyLevel():.2f}, "
              f"Development Stage={agent.getDevelopmentalStage().name}")

### Example 5: Advanced Learning with All Features (Phase 6 Style)

```python
import pynlm
import time

print("=== Advanced NLM Learning Simulation ===\n")

# Phase 6: Create a development-ready brain with all features
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)  # Larger brain for complex behavior
config.set("plasticity_learning_rate", 0.008)
config.set("development_synaptogenesis_rate", 0.002)
config.set("random_seed", 12345)

# Create the brain
brain = pynlm.createBrain(config)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Setup world with rich sensory capabilities
world = pynlm.createSimpleWorld()
world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
world.setMaxEnergy(200.0)
world.setEnergyDecayRate(0.005)
world.reset()
print(f"World configured: {world.getWidth()}x{world.getHeight()} with {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()} vision")

# Create agent with all subsystems enabled
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)  # Brain growth and pruning
agent.enableDevelopment(True)           # Developmental stages
agent.enableCuriosity(True)             # Exploration behavior

print("Learning systems enabled:\n")
print("  ✓ Reward Modulation (dopamine-based learning)")
print("  ✓ Structural Plasticity (synaptogenesis/pruning)")
print("  ✓ Development (developmental stages)")
print("  ✓ Curiosity (exploration/exploitation tradeoff)\n")

# Create a more complex world with dynamic challenges
print("Creating dynamic world environment...")
for i in range(8):
    import random
    world.addObject(pynlm.WorldObject(
        random.uniform(5.0, 35.0),
        random.uniform(5.0, 35.0),
        pynlm.WorldObjectType.Resource,
        value=25.0,
        radius=0.8
    ))
    world.addObject(pynlm.WorldObject(
        random.uniform(5.0, 35.0),
        random.uniform(5.0, 35.0),
        pynlm.WorldObjectType.Hazard,
        value=-20.0,
        radius=0.6
    ))

print("World populated with resources and hazards\n")

# Run comprehensive learning simulation
print("Starting comprehensive learning simulation...")
print("(This demonstrates Phase 6 integration features)\n")

learning_stats = []
episode_rewards = []
novelty_levels = []
development_stages = []

for episode in range(1, 6):  # 5 learning episodes
    print(f"--- Episode {episode} ---")
    
    # Reset world and agent for new episode
    world.reset()
    agent.reset()
    
    # Place agent in different location each episode
    world.setAgentStart(
        random.uniform(10.0, 30.0),
        random.uniform(10.0, 30.0)
    )
    
    episode_reward = 0.0
    steps_in_episode = 0
    
    for step in range(500):  # 500 steps per episode
        steps_in_episode = step + 1
        
        # Update world physics
        world.update(0.1)
        
        # Get sensory input (what agent sees/hears/feels)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes sensory input and makes decisions
        brain.step(step)
        
        # Agent decodes motor commands from brain activity
        action = agent.decodeMotorCommand()
        
        # Apply action in world (move, turn, interact, etc.)
        result = world.applyMotorCommand(action, world.getSimulationTime())
        episode_reward += result.reward
        
        # Apply reward modulation based on outcome
        # Use prediction error if available, otherwise use actual reward
        prediction_error = agent.getPredictionError()
        agent.applyRewardModulation(result.reward, prediction_error)
        
        # Update developmental stage
        agent.updateDevelopment(0.1)
        
        # Log key metrics every 100 steps
        if step % 100 == 0 and step > 0:
            learning_stats.append({
                'episode': episode,
                'step': step,
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'development_stage': agent.getDevelopmentalStage().name,
                'energy': world.getAgentBody().energy,
                'health': world.getAgentBody().health
            })
            
            print(f"  Step {step:3d}: "
                  f"Spikes={brain.getTotalSpikeCount():6d}, "
                  f"Firing={brain.getFiringNeuronCount():4d}, "
                  f"Stage={agent.getDevelopmentalStage().name:12s}, "
                  f"Curiosity={agent.getCuriosityLevel():5.2f}, "
                  f"Novelty={agent.getNoveltyLevel():5.2f}, "
                  f"Reward={result.reward:+5.2f}, "
                  f"Total Reward={episode_reward:+7.2f}")

    # Store episode results
    episode_rewards.append(episode_reward)
    novelty_levels.append(agent.getNoveltyLevel())
    development_stages.append(agent.getDevelopmentalStage().name)
    
    print(f"Episode {episode} complete:")
    print(f"  Total reward: {episode_reward:+.2f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.2f}")
    print(f"  Development stage: {agent.getDevelopmentalStage().name}")
    print(f"  Final energy: {world.getAgentBody().energy:.1f}/200")
    print(f"  Final health: {world.getAgentBody().health:.1f}/1.0\n")

# Summary and analysis
print("=== Learning Simulation Complete ===\n")
print("Learning Performance Summary:")
print(f"  Episodes completed: {len(episode_rewards)}")
print(f"  Average reward per episode: {sum(episode_rewards)/len(episode_rewards):+.2f}")
print(f"  Average final novelty: {sum(novelty_levels)/len(novelty_levels):.2f}")
print(f"  Development stage progression: {', '.join(set(development_stages))}")
print(f"  Total simulation time: ~{len(episode_rewards) * 50} world steps\n")

print("Final Brain Statistics:")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"  Developmental stage: {agent.getDevelopmentalStage().name}")
print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
print(f"  Curiosity level: {agent.getCuriosityLevel():.2f}")
print(f"  Novelty level: {agent.getNoveltyLevel():.2f}")
print(f"  Prediction error: {agent.getPredictionError():.3f}")

# Save brain state for future use
print("\nSaving brain state for future sessions...")
brain.save("advanced_learning_brain.bin")
print("Brain state saved successfully!")

# Export world state for analysis
print("\nExporting learning data for analysis...")
with open("learning_data.json", "w") as f:
    import json
    json.dump({
        'episodes': len(episode_rewards),
        'rewards': episode_rewards,
        'novelty_levels': novelty_levels,
        'development_stages': list(set(development_stages)),
        'final_stats': {
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio()
        }
    }, f, indent=2)
print("Learning data saved to learning_data.json")

print("\n=== Advanced Simulation Complete ===")
print("The brain has demonstrated complex learning behavior with:")
print("  ✓ Developmental progression through stages")
print("  ✓ Neuromodulation-based learning")
print("  ✓ Curiosity-driven exploration")
print("  ✓ Structural plasticity (brain growth)")
print("  ✓ Memory integration")
print("  ✓ Adaptive behavior based on rewards and prediction errors")


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
