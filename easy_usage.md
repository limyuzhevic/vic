# NLM Python Quick Start - Easy Usage Guide

## TL;DR - What You Need to Know

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think for a while
for i in range(100):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

That's literally everything you need for a basic brain simulation!

## What Makes NLM Special

NLM is unlike traditional AI frameworks:

- **Not a transformer or LLM** - It's a biologically-inspired brain
- **Not a chatbot** - It's a learning simulation system  
- **Not deep learning** - It uses event-driven spiking neurons
- **NOT pretrained** - It develops from scratch

It's more like giving a computer a brain that learns and adapts over time.

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks and learns
2. **World** - The environment the brain interacts with
3. **Agent** - The bridge connecting brain to world

## Quick Examples (Copy & Paste)

### Example 1: Silent Brain (Phase 2)
```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it "think" for 100 steps
for i in range(100):
    brain.step(i)

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in World (Phase 3)
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

### Example 3: Complete Agent with Learning (Phase 6)
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

## One Line Version

```python
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)
print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

## What Each Command Does

| Command | What it does |
|---------|--------------|
| `pynlm.createBrain(config)` | Creates a virtual brain with neurons |
| `brain.initialize()` | Starts up the brain (ALWAYS call this!) |
| `brain.step(step)` | Makes brain process one moment in time |
| `pynlm.createSimpleWorld()` | Creates a 2D simulation environment |
| `world.configure(...)` | Sets world dimensions and vision |
| `world.reset()` | Resets world to initial state |
| `pynlm.createAgentBrain(brain)` | Creates bridge between brain and world |
| `agent.initialize(world)` | Connects agent to world |
| `agent.processSensoryInput(percept)` | Gives brain sensory information |
| `agent.decodeMotorCommand()` | Gets action decision from brain |

## Common Patterns

### Pattern 1: Run a Simulation
```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

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

## Troubleshooting

### "My brain isn't doing anything"
**Fix:** Did you call `brain.initialize()`?
**Solution:** Add `brain.initialize()` right after creating the brain.

### "The agent isn't moving"
**Fix:** Did you call `world.applyMotorCommand()`?
**Solution:** Make sure both `world.update()` and `world.applyMotorCommand()` are being called.

### "Everything is 0"
**Fix:** Brains need time to "warm up"
**Solution:** Try more steps (100+ usually enough). Some neurons need input to fire.

## Advanced Quick Start

### Create a Learning Agent
```python
import pynlm

# Create config with learning
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

# Create and initialize
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Run learning session
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {agent.getCuriosityLevel():.2f} curiosity")

print("Learning complete!")
```

### Monitor Development
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"Starting development stage: {brain.getDevelopmentalStage()}")

for step in range(200):
    brain.step(step)
    
    if step % 50 == 0:
        stage = brain.getDevelopmentalStage()
        firing = brain.getFiringNeuronCount()
        rate = brain.getAverageFiringRate()
        print(f"Step {step}: Stage {stage}, {firing} firing, {rate:.2f} Hz")
```

## That's It!

You've now used NLM. The hardest part is remembering to call `brain.initialize()` first!

## Next Steps

1. **Read `HOW_TO_USE.md`** - More detailed information
2. **Read `docs/ARCHITECTURE.md`** - How it all fits together
3. **Experiment** - Try different configurations
4. **Build from source** - See `CMakeLists.txt` for build instructions

Good luck with your artificial brain!

# Quick Reference Table

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

You now have everything needed to start using NLM! 🎉