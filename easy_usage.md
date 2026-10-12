# NLM - Easy Usage Guide for Beginners

## Quick Overview

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other  
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## The 3 Key Components

Understanding NLM requires knowing about three main parts:

### 1. **Brain** - The virtual brain that thinks
- Contains neurons and synapses
- Processes sensory input
- Makes decisions and produces actions
- Learns from experience

### 2. **World** - The environment the brain lives in
- Contains objects and resources
- Provides sensory input to the brain
- Accepts actions from the brain
- Gives feedback/rewards

### 3. **Agent** - The bridge connecting brain to world
- Takes sensory input from world
- Passes it to brain
- Takes brain's decisions and applies them to world
- Handles learning and development

---

## Getting Started

### Example 1: Simplest Brain (Just Neurons)

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
print("Your brain thought", brain.getTotalSpikeCount(), "times!")
```

### Example 2: Brain Learning from Environment

```python
import pynlm

# Setup everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)                    # Update world
    percept = world.getSensoryPercept()   # What does agent see?
    agent.processSensoryInput(percept)   # Brain sees it
    brain.step(step)                      # Brain thinks
    action = agent.decodeMotorCommand()   # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

### Example 3: Using Convenience Functions

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable convenient learning features
agent.enableRewardModulation(True)    # Learn from rewards
agent.enableCuriosity(True)           # Explore new things
agent.enableDevelopment(True)         # Brain matures over time

# Run enhanced simulation with convenience features
for step in range(200):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Convenience: Get automatic curiosity about environment
    curiosity_level = agent.getCuriosityLevel()
    if curiosity_level > 0.8:
        print(f"Step {step}: Agent is highly curious!")
    
    # Convenience: Check development progress
    dev_stage = brain.getDevelopmentalStage()
    if dev_stage == pynlm.DevelopmentalStage.Adult:
        print(f"Step {step}: Brain has reached adult stage!")

print("Enhanced simulation complete!")
print("Final curiosity:", agent.getCuriosityLevel())
print("Development stage:", brain.getDevelopmentalStage())
```

---

## Common Patterns

### Pattern 1: Complete Agent Simulation

```python
def run_complete_simulation(brain, world, agent, num_steps):
    """Run a complete agent simulation with learning."""
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
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

run_complete_simulation(brain, world, agent, 1000)
```

### Pattern 2: Monitoring and Statistics

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
```

### Pattern 3: Enabling Learning Features

```python
# These are all optional learning features:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
agent.enableNoveltyDetection(True)      # Detect novel stimuli
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
- Make sure world is being updated

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