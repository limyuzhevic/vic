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

## Enhanced Examples with New Features

### New Feature 1: Quick Training with runEpisode

```python
import pynlm

# Create a brain and agent with a resource collection world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = pynlm.createAgentBrain(brain)

# Create a world with resources and start position
world = pynlm.createResourceWorld(width=30, height=30, numResources=15, numHazards=5)
agent.initialize(world)

# Run a complete episode (100 steps) with learning enabled
total_reward, curiosity_events, prediction_error = agent.runEpisode(world, steps=100, enableLearning=True)

print(f"Episode completed!")
print(f"  Total reward: {total_reward:.2f}")
print(f"  Curiosity events: {curiosity_events}")
print(f"  Prediction error: {prediction_error:.3f}")
```

### New Feature 2: Advanced Training with train Method

```python
import pynlm
import random

# Create agent and world
agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))

# Use different world scenarios
for scenario in ["simple", "resource_collection", "obstacle_course"]:
    world = pynlm.createTrainingWorld(scenario)
    agent.initialize(world)
    
    print(f"\nTraining with {scenario} world...")
    
    # Train for 50 episodes with verbose output
    episode_rewards, curiosity_levels, avg_reward, avg_curiosity, best_episode, best_reward = agent.train(
        world, episodes=50, stepsPerEpisode=100, verbose=True
    )
    
    print(f"  Average reward: {avg_reward:.2f}")
    print(f"  Average curiosity: {avg_curiosity:.3f}")
    print(f"  Best reward: {best_reward:.2f} (episode {best_episode})")
```

### New Feature 3: Quick Training with trainSimple

```python
import pynlm

# Very simple training method for beginners
brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
agent.initialize(world)

# Quick training with just one line
total_reward = agent.trainSimple(world)
print(f"Simple training completed with total reward: {total_reward:.2f}")
```

### New Feature 4: Configuration Presets

```python
import pynlm

# Young brain (high plasticity, fast learning)
young_brain = pynlm.createBrain(pynlm.createYoungConfig())

# Adult brain (stable, mature performance)
adult_brain = pynlm.createBrain(pynlm.createAdultConfig())

# Compare them
print("Young brain:", young_brain.getTotalNeuronCount(), "neurons")
print("Adult brain:", adult_brain.getTotalNeuronCount(), "neurons")
```

### New Feature 5: Complete Agent Setup

```python
import pynlm

# One-line creation of complete agent setup
brain, world, agent = pynlm.createAgent()

# Use the agent
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation complete! Firing neurons: {brain.getFiringNeuronCount()}")
```

### New Feature 6: Advanced World Creation

```python
import pynlm

# Arena world (empty space for exploration)
arena_world = pynlm.createArenaWorld(width=50, height=50, visionWidth=10, visionHeight=10)

# Maze world (complex with predefined layout)
maze_layout = [
    "##################",
    "#             #",
    "# #############",
    "# #           #",
    "# # ########## #",
    "# # #         # #",
    "# # # #####   # #",
    "# # #   #     # #",
    "# # ##### ##### #",
    "# #           # #",
    "# ############# #",
    "#             #",
    "##################"
]
maze_world = pynlm.createMazeWorld(width=20, height=12, mazeLayout=maze_layout)

# Obstacle course world (challenges for agent)
obstacle_world = pynlm.createObstacleWorld(width=60, height=30, visionWidth=12, visionHeight=8)
```

### New Feature 7: Combined Advanced Training

```python
import pynlm

# Create agent with young brain (more plastic)
agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createYoungConfig()))

# Training loop with different scenarios
scenarios = ["simple", "resource_collection", "obstacle_course"]
for scenario in scenarios:
    print(f"\nTraining with {scenario} scenario...")
    
    # Create fresh world for each scenario
    world = pynlm.createTrainingWorld(scenario)
    
    # Quick training session
    reward = agent.trainSimple(world)
    print(f"  Got reward: {reward:.2f}")
    
    # Reset for next scenario
    agent.reset()
    agent.initialize(world)

print("\nTraining complete!")
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `pynlm.createSimpleWorld()`
Creates a basic world with random objects.

### `pynlm.createResourceWorld(width, height, numResources, numHazards)`
Creates a world with collectible resources and hazards.

### `pynlm.createAgentBrain(brain)`
Creates an agent interface that connects the brain to the world.

### `pynlm.createAgent()`
Creates a complete agent setup (brain, world, agent) in one line.

### `agent.runEpisode(world, steps=100, enableLearning=True)`
Run a complete training episode and return statistics.

### `agent.train(world, episodes=100, stepsPerEpisode=100, verbose=False)`
Advanced training method with detailed statistics and learning curve analysis.

### `agent.trainSimple(world)`
Quick training method for common scenarios.

### `pynlm.createYoungConfig()` and `pynlm.createAdultConfig()`
Create brains with different development characteristics.

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create basic brain | `pynlm.createBrain(pynlm.createDefaultConfig())` |
| Create complete agent | `pynlm.createAgent()` |
| Create resource world | `pynlm.createResourceWorld()` |
| Run training episode | `agent.runEpisode(world)` |
| Quick training | `agent.trainSimple(world)` |
| Advanced training | `agent.train(world, episodes=50)` |
| Young brain | `pynlm.createBrain(pynlm.createYoungConfig())` |
| Adult brain | `pynlm.createBrain(pynlm.createAdultConfig())` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if learning features are enabled

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Make sure the world has a valid starting position

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if the world has objects to interact with

---

## Next Steps

When you're comfortable:
1. Experiment with different configuration presets (young vs adult)
2. Try the advanced training methods (`train`, `runEpisode`)
3. Create custom worlds with different scenarios
4. Use the convenience functions for quick setup
5. Read `HOW_TO_USE.md` for more detailed documentation
6. Read `docs/ARCHITECTURE.md` to understand how it all works

That's it! You're now ready to use NLM with all its enhanced features.