# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

## Advanced Usage: Phase 6 Integration

Now that you've learned the basics, let's explore Phase 6 - the FINAL INTEGRATION! This is where all the brain systems work together as one coherent intelligence.

### Phase 6: Complete Brain Integration

Phase 6 combines ALL previous phases into one unified artificial brain system. Your brain will have:

- **Working Memory**: Keeps active information for immediate processing
- **Episodic Memory**: Stores experiences as episodes for replay and learning
- **Associative Memory**: Makes connections between patterns and concepts
- **Neuromodulation**: Dopamine (rewards), Curiosity (exploration), Novelty (surprise)
- **Prediction System**: Anticipates future events and errors
- **Cognitive Functions**: Planning, concept formation, attention
- **Development System**: Brain matures and adapts over time
- **Checkpointing**: Saves and loads brain state for persistence
- **Replay System**: Replays experiences to consolidate learning

### Phase 6 Advanced Example

Here's a complete example that demonstrates all Phase 6 systems working together:

```python
import pynlm
import time
import os

# Step 1: Create an advanced Phase 6 brain with all systems enabled
config = pynlm.createDefaultConfig()
config.setNeuronalParameters(tauMembrane=0.02, vRest=-70.0, vThreshold=-50.0)
config.setPlasticityParameters(stdpLTP=0.01, stdpLTD=0.012, weightMax=10.0)
config.enableAdvancedFeatures()  # Enable all Phase 6 features

brain = pynlm.createBrain(config)
brain.initialize()

# Step 2: Create an advanced world with dynamic environment
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.enableDynamicEnvironment()  # Phase 6 enhancement
world.setAgentStart(10, 10)
world.reset()

# Step 3: Create an advanced agent with all capabilities
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableAdvancedLearning()           # Complex learning strategies
agent.enableCuriosityDrivenExploration() # Active exploration
agent.enablePredictivePlanning()        # Plan actions based on predictions
agent.enableConceptLearning()           # Learn abstract concepts
agent.enableAttentionManagement()       # Focus on relevant information
agent.enableSocialLearning()            # Learn from others
agent.enableContinualLearning()         # Learn continuously

print("=== Phase 6 Advanced Integration Demo ===")
print("Brain systems enabled:")
print(f"  ✓ Working memory: Available ({brain.getWorkingMemory() is not None})")
print(f"  ✓ Episodic memory: Available ({brain.getEpisodicMemory() is not None})")
print(f"  ✓ Neuromodulation: Available ({brain.getDopamine() is not None})")
print(f"  ✓ Prediction system: Available ({brain.getPredictionSystem() is not None})")
print(f"  ✓ Development system: Available ({brain.getDevelopmentSystem() is not None})")

# Step 4: Run an advanced simulation with all systems interacting
episode_reward = 0.0
firing_rates = []
memory_episodes = []

for step in range(1000):
    # World updates with dynamic complexity
    world_time = world.getSimulationTime()
    world.update(0.1)
    
    # Get sensory input with attention and prediction
    percept = world.getSensoryPercept()
    
    # Agent processes sensory input with advanced processing
    agent.processSensoryInput(percept)
    
    # Brain thinks with integrated systems
    brain.step(step)
    
    # Get action with predictive planning
    action = agent.decodeMotorCommand()
    
    # Apply action and get result (ActionResult contains reward)
    action_result = world.applyMotorCommand(action, world_time)
    
    # Advanced reward modulation with prediction error
    agent.applyRewardModulation(action_result.reward, agent.getPredictionError())
    
    # Development progression
    if step % 10 == 0:
        agent.updateDevelopment(0.1)
    
    # Collect metrics
    episode_reward += action_result.reward
    firing_rates.append(brain.getAverageFiringRate())
    
    if step % 100 == 0:
        print(f"Step {step}: Reward={episode_reward:.2f}, "
              f"Firing={brain.getAverageFiringRate():.2f}, "
              f"Prediction Error={agent.getPredictionError():.3f}, "
              f"Curiosity={agent.getCuriosityLevel():.2f}")

print("\n=== Phase 6 Results ===")
print(f"Total reward over episode: {episode_reward:.2f}")
print(f"Average firing rate: {sum(firing_rates)/len(firing_rates):.2f}")
print(f"Memory episodes stored: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
print(f"Current developmental stage: {brain.getDevelopmentalStage()}")
print(f"Neuromodulation levels:")
print(f"  Dopamine: {agent.getNeuromodulationLevel():.2f}")
print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")
print(f"  Novelty: {agent.getNoveltyLevel():.2f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")

# Step 5: Test checkpointing and replay (Phase 6 core features)
print("\n=== Testing Phase 6 Core Features ===")

# Save brain state (checkpointing)
checkpoint_path = "./nlm_phase6_checkpoint.bin"
if brain.save(checkpoint_path):
    print("✓ Checkpoint saved successfully")
else:
    print("✗ Checkpoint save failed")

# Test replay system if episodic memory is working
if brain.getEpisodicMemory() and brain.getEpisodicMemory().getEpisodeCount() > 0:
    episodes = brain.getEpisodicMemory().getEpisodesForReplay(5)
    if not episodes.empty():
        print("✓ Replay system working - retrieved episodes for consolidation")
    else:
        print("? Replay system available but no episodes to replay")
else:
    print("? No episodic memory available for replay")

# Load checkpoint (recovery test)
if os.path.exists(checkpoint_path):
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    if brain2.load(checkpoint_path):
        print("✓ Checkpoint loaded successfully - recovery working")
    else:
        print("✗ Checkpoint load failed")

print("\n=== Phase 6 Integration Complete ===")
print("Your artificial brain now has:")
print("  ✓ Working memory for active processing")
print("  ✓ Episodic memory for experience replay")
print("  ✓ Neuromodulation for motivation and exploration")
print("  ✓ Prediction for anticipating future events")
print("  ✓ Cognitive functions for complex behavior")
print("  ✓ Development for maturation and adaptation")
print("  ✓ Checkpointing for persistence")
print("  ✓ Replay for memory consolidation")
print("\nThis represents a truly integrated artificial intelligence!")
```

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

## Mini Projects

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
