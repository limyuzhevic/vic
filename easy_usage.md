# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (Neural Learning Machine, 熙然 - "serene flow") is a brain simulator for computers. It lets you create virtual brains that can learn and make decisions using spiking neural networks.

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells that strengthen/weaken with learning
- **Learning** = When connections get stronger or weaker based on experience
- **Memory** = The brain remembering patterns

**Important Note:** NLM has comprehensive neural core functionality (neurons, synapses, basic learning) but many higher-level systems (memory, prediction, advanced cognition) are still in development. This guide covers what's actually working.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks (LIF neurons, STDP, Hebbian learning)
2. **World** - The environment the brain lives in (simple 2D grid)
3. **Agent** - The bridge connecting brain to world (sensory/motor processing, reward)

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain with default settings (1000 neurons)
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Initialize the brain (must be done before any simulation)
brain.initialize()

# Step 3: Make the brain think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking - neurons spike, synapses update

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
print("Brain stats: Firing rate =", brain.getAverageFiringRate())
```

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create a simple world (2D grid where agent can move)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Create agent that connects brain to world
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features (optional but recommended)
agent.enableRewardModulation(True)  # Learn from rewards
agent.enableCuriosity(True)        # Explore new things

# Run for 50 steps of interaction
for step in range(50):
    world.update(0.1)  # Update world (agent moves, time passes)
    percept = world.getSensoryPercept()  # Get visual/sensory input
    agent.processSensoryInput(percept)  # Brain receives input
    brain.step(step)  # Brain processes - neurons fire
    action = agent.decodeMotorCommand()  # Brain decides what to do
    world.applyMotorCommand(action, world.getSimulationTime())  # Agent acts

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
| Your brain learns | `agent.enableRewardModulation(True)` |

**Note:** NLM has basic curiosity and novelty detection but most memory systems are not yet fully integrated.

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain with default settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it (required before any simulation)
brain.initialize()

# Make it active for one time step
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
print("Total spikes:", brain.getTotalSpikeCount())
```

---

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees (sensory input)
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks (neurons fire, synapses update)
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
print("E/I ratio:", brain.getExcitationInhibitionRatio())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards via STDP
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things (novelty detection)
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

**What actually works:** Basic LIF neuron dynamics, STDP plasticity, structural plasticity every 100 steps. Most memory systems not connected.

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Updates all neurons (LIF dynamics)
- Detects spikes and schedules spike events
- Applies plasticity rules (STDP and Hebbian)
- Every 100 steps: performs structural plasticity (synaptogenesis/pruning)

**What's currently NOT working:** Memory systems, prediction system, advanced neuromodulation, full development integration.

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

---

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
print("Total neurons:", brain.getTotalNeuronCount())
print("Total synapses:", brain.getTotalSynapseCount())
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

## Important Notes for Beginners

**What's Actually Working:**
- ✅ Basic neural network (1000 neurons, firing)
- ✅ Spike-based computation (LIF neurons)
- ✅ Learning rules (STDP, Hebbian)
- ✅ Sensory-motor loop
- ✅ Reward modulation (limited dopamine effects)

**What Needs Work (Phase 6 integration):**
- ❌ Working memory (NeuralWorkingMemory exists but not connected)
- ❌ Episodic memory (experience storage)
- ❌ Semantic memory (concept formation)
- ❌ Prediction system
- ❌ Full neuromodulation (serotonin, norepinephrine, ACh)
- ❌ Advanced cognition (planning, attention)
- ❌ Memory consolidation and replay
- ❌ Full development integration

**The brain you build is a neural simulator, not a complete AI**

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`? It's required!
- Try increasing the number of steps (more simulation time)

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`? The brain needs to act!
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

**"No such module: pynlm"**
- Try installing: `pip install -e .` (build from source)
- May need: `pip install scikit-build-core pybind11`

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details on configuration options
2. Read `docs/PHASE6_FINAL_AUDIT.md` to understand what's still in development
3. Look at `src/experiments/` for complete Phase 6 integration tests
4. Experiment with different brain configurations!

**Reality Check:** You're building a neural simulator with known limitations, not a full AI system.

That's it! You're now ready to use NLM with accurate expectations.

**The future goal (Phase 6) is to integrate all these disconnected systems into a coherent artificial brain.**
