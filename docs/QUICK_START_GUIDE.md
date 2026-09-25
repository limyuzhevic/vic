# NLM Quick Start Guide

## What is NLM?

**NLM (Neural Learning Machine)** is an experimental brain-inspired spiking neural network simulator. It implements biological learning principles including:

- Leaky Integrate-and-Fire (LIF) neurons
- Spike-Timing-Dependent Plasticity (STDP)
- Hebbian learning
- Neuromodulation (dopamine, curiosity, novelty)
- Developmental processes

Think of it as creating a virtual brain that learns from experience, just like biological systems.

## Quick Start (5-Minute Tutorial)

### Step 1: Install (if needed)
```bash
pip install .  # Install Python bindings
```

### Step 2: Create Your First Brain
```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate for 100 steps
for i in range(100):
    brain.step(i)
    
print(f"Brain completed {i+1} steps with {brain.getFiringNeuronCount()} firing neurons")
```

### Step 3: Add a World (Optional)
```python
# If you want your brain to interact with an environment
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Key Concepts

| Concept | What It Does | Code |
|---------|-------------|------|
| **Brain** | Virtual spiking neural network | `pynlm.createBrain(config)` |
| **World** | Environment with sensory input | `pynlm.createSimpleWorld()` |
| **Agent** | Interface between brain and world | `pynlm.createAgentBrain(brain)` |
| **Config** | Brain parameters | `pynlm.createDefaultConfig()` |

## First Steps Examples

### 1. Silent Brain (Learning through Stimulation)
```python
# Just watch neurons fire
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(50):
    brain.step(i)
    
print("Neurons firing:", brain.getFiringNeuronCount())
```

### 2. Simple Learning
```python
# Brain learns from its own activity
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(200):
    brain.step(i)
    
print("Learning complete - Synaptic changes:", brain.getPlasticityStats())
```

### 3. Complete Agent (Brain in World)
```python
import pynlm

def run_agent():
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    # Run
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return brain, world, agent

# Start your first NLM simulation!
result = run_agent()
```

## Next Steps

1. **Read `HOW_TO_USE.md`** - Complete Python API guide
2. **Read `docs/ARCHITECTURE.md`** - System architecture
3. **Try `docs/EXAMPLES.md`** - More complex examples
4. **Check `docs/ROADMAP.md`** - Current implementation status

## Common Settings

```python
# Change these to customize your brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)      # More neurons = more complex
config.set("plasticity.stdp.enable", True)  # Enable learning
config.set("neuromod.dopamine.scale", 1.0)  # Reward sensitivity
```

## Troubleshooting

**"Nothing happens"**: Call `brain.initialize()` before `brain.step()`
**"Too slow"**: Use fewer neurons or run in debug mode
**"Brain crashes"**: Check that all `initialize()` calls are made

Your brain is ready! Start experimenting and discover what your virtual neurons can learn.