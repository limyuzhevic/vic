# NLM - Quick Start Guide

## What is NLM?

NLM is a **brain simulator** that creates virtual brains that can learn, remember things, and make decisions.

Think of it like this:
- **Neurons** = Brain cells that send signals
- **Synapses** = Connections between brain cells  
- **Learning** = Connections get stronger/weaker based on experience
- **Memory** = The brain remembering patterns

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - Bridge connecting brain to world

## Quick Start (Copy & Paste)

### Simplest Brain

```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Start it
brain.initialize()

# Make it think
for i in range(100):
    brain.step(i)

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Complete Agent Example

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
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

## Common Pattern: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

## Next Steps

1. **For more details**: Read `HOW_TO_USE.md`
2. **Architecture**: Read `docs/ARCHITECTURE.md`  
3. **Experiments**: Try the demo executables (`./nlm_phase6_demo`)

That's it! You're now ready to use NLM.
