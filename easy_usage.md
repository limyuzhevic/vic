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

---

## Advanced Examples

### Advanced 1: Learning with Development and Memory

```python
import pynlm

# Create brain with development enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)    # Brain can grow
agent.enableDevelopment(True)            # Brain matures over time
agent.enableCuriosity(True)              # Explore novel things

# Run development simulation
print("Running brain development simulation...")
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward and update development
    reward = 0.0
    if percept and percept.getInternal():
        reward = percept.getInternal()[0]
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Memory episodes: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")

print("Development simulation complete!")
print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
```

### Advanced 2: Complete Agent Simulation with All Systems

```python
import pynlm

def run_advanced_agent_simulation(num_steps=2000):
    """Run a complete agent simulation with all brain systems integrated."""
    
    # 1. Create and configure brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 3. Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Starting advanced simulation with {num_steps} steps...")
    print(f"Initial neuron count: {brain.getTotalNeuronCount()}")
    print(f"Initial synapse count: {brain.getTotalSynapseCount()}")
    
    # 4. Run the simulation
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step, step * 0.001)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Compute and apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept() and world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress periodically
        if step % 500 == 0:
            print(f"\nStep {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            
            if world.getSensoryPercept():
                print(f"  Vision size: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")
    
    # 5. Print final results
    print(f"\n=== FINAL RESULTS ===")
    print(f"Total reward accumulated: {agent.applyRewardModulation(0.0, 0.0):.3f}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"Prediction error: {agent.getPredictionError():.3f}")
    
    # 6. Show brain statistics
    print(f"\n=== BRAIN STATISTICS ===")
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Synapses: {brain.getTotalSynapseCount()}")
    print(f"Regions: {brain.getRegionCount()}")
    print(f"Working memory traces: {brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces() or 0}")
    print(f"Episodic memory episodes: {brain.getEpisodicMemory() and brain.getEpisodicMemory().getEpisodeCount() or 0}")
    print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    return {
        'totalReward': agent.applyRewardModulation(0.0, 0.0),
        'avgFiringRate': brain.getAverageFiringRate(),
        'curiosityLevel': agent.getCuriosityLevel(),
        'noveltyLevel': agent.getNoveltyLevel(),
        'developmentStage': brain.getDevelopmentalStage()
    }

# Run the advanced simulation
results = run_advanced_agent_simulation(2000)
print(f"\nSimulation completed successfully!")
```

### Advanced 3: Checkpoint and Replay System

```python
import pynlm
import os

# Create and run a simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()
world.setAgentStart(7.5, 7.5)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableDevelopment(True)

print("Running simulation with checkpointing...")
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(0.1, 0.0)  # Small reward for exploration
    agent.updateDevelopment(0.1)

print("Saving checkpoint...")
brain.save("test_checkpoint.bin")
print("Checkpoint saved.")

# Create new brain and load checkpoint
print("Creating new brain from checkpoint...")
new_brain = pynlm.createBrain(config)
new_brain.initialize()
new_brain.load("test_checkpoint.bin")

new_agent = pynlm.createAgentBrain(new_brain)
new_agent.initialize(world)
new_agent.enableRewardModulation(True)
new_agent.enableDevelopment(True)

print("Verifying checkpoint restoration...")
print(f"Original brain neurons: {brain.getTotalNeuronCount()}")
print(f"Loaded brain neurons: {new_brain.getTotalNeuronCount()}")
print(f"Brains match: {brain.getTotalNeuronCount() == new_brain.getTotalNeuronCount()}")

# Clean up
if os.path.exists("test_checkpoint.bin"):
    os.remove("test_checkpoint.bin")
    print("Checkpoint file removed.")
```

### Advanced 4: Multi-Agent System

```python
import pynlm

# Create multiple agents sharing the same world
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)

world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=6, visionHeight=6)
world.reset()

# Create multiple agents
agents = []
brains = []

for i in range(3):
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.setAgentStart(i * 8.0, i * 8.0)  # Different starting positions
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    agents.append(agent)
    brains.append(brain)

print("Multi-agent simulation started...")
print("Agents are competing for resources in the same world.")

# Run simulation
for step in range(800):
    world.update(0.1)
    
    # Each agent perceives and acts independently
    for i, agent in enumerate(agents):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brains[i].step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.05, 0.0)  # Small reward for action
        agent.updateDevelopment(0.1)
    
    # Every agent can observe other agents' actions
    if step % 200 == 0:
        print(f"\nStep {step}:")
        for i, agent in enumerate(agents):
            print(f"  Agent {i}: Firing={brains[i].getFiringNeuronCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Stage={brains[i].getDevelopmentalStage()}")

print("\n=== Multi-Agent Results ===")
for i, agent in enumerate(agents):
    print(f"Agent {i}:")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Final development stage: {brains[i].getDevelopmentalStage()}")
    print(f"  Total spikes: {brains[i].getTotalSpikeCount()}")
```
