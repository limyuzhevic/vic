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

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

### Example 2: Brain with Learning

```python
import pynlm
import time

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable learning capabilities
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(100):
    brain.step(step)
    
    # Show progress
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Development: {brain.getDevelopmentalStage()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")

print("Simulation complete!")
```

### Example 3: Complete Agent in World

```python
import pynlm
import random

class SimpleTraining:
    def __init__(self):
        # Create brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("brain.connection_probability", 0.05)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        # Initialize agent
        agent.initialize(world)
        
        # Enable all learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        self.brain = brain
        self.agent = agent
        self.world = world
        self.steps = 0
    
    def run_episode(self, max_steps=1000):
        print(f"Running episode {self.steps + 1}")
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process in brain
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward
            reward = self.world.getSensoryPercept().getInternal()[0] if \
                    self.world.getSensoryPercept().getInternal() else 0.0
            
            # Apply learning
            self.agent.applyRewardModulation(reward, 0.0)
            self.agent.updateDevelopment(0.1)
            
            # Print stats
            if step % 100 == 0:
                print(f"  Step {step}:")
                print(f"    Neurons firing: {self.brain.getFiringNeuronCount()}")
                print(f"    Firing rate: {self.brain.getAverageFiringRate():.2f}")
                print(f"    Reward: {reward:.2f}")
                print(f"    Curiosity: {self.agent.getCuriosityLevel():.3f}")
                print(f"    Development: {self.brain.getDevelopmentalStage()}")
        
        self.steps += 1
        return step

# Run training
print("Starting NLM Brain Training...")
trainer = SimpleTraining()
trainer.run_episode()
print("Training complete!")
```
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

### Pattern 1: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 2: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

### Pattern 3: Enable Advanced Learning

```python
# Enable sophisticated learning capabilities:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Explore novel stimuli
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enablePrediction(True)            # Learn action consequences
agent.enableAttention(True)             # Focus on important stimuli
```

### Pattern 4: Complete Simulation Loop

```python
def run_complete_simulation(brain, world, agent, num_steps):
    """Run a complete agent simulation with all learning enabled."""
    
    # Main simulation loop
    for step in range(num_steps):
        # 1. Update environment
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process sensory information
        agent.processSensoryInput(percept)
        
        # 4. Run brain computation
        brain.step(step)
        
        # 5. Get motor commands
        action = agent.decodeMotorCommand()
        
        # 6. Execute actions in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Apply learning
        reward = world.getSensoryPercept().getInternal()[0] if \
                world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # 8. Print diagnostics
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Firing rate: {brain.getAverageFiringRate():.2f}")
            print(f"  Reward: {reward:.2f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")

# Run the simulation
run_complete_simulation(brain, world, agent, 1000)
```

### Pattern 1: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 2: Enable Brain Features

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
