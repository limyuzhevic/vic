# NLM - How to Use It

**Neural Learning Machine (熙然) - Quick Start Guide**

NLM is an experimental artificial brain simulator that lets you create virtual brains that can learn, remember things, and make decisions. Think of it as giving a computer a brain that can:

- Process information (like sensory input)
- Make decisions (like motor actions)
- Learn from experience (like synaptic plasticity)
- Remember things (like memory systems)

## Quick Start (3 minutes)

1. **Brain** - The virtual brain that thinks (contains neurons and synapses)
2. **World** - The environment the brain lives in (where things happen)
3. **Agent** - The bridge connecting brain to world (controls interaction)

## Basic Workflow (Copy & Paste)

### Step 1: Create Everything
```python
import pynlm

# Create brain with default settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create the world where the brain lives
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Create agent to control interaction
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

### Step 2: Run the Brain in the World
```python
# Main simulation loop
for step in range(100):
    # 1. Update world state
    world.update(0.1)  # Advance 0.1 seconds
    
    # 2. Get what the agent sees (sensory input)
    percept = world.getSensoryPercept()
    
    # 3. Tell the brain what it sees
    agent.processSensoryInput(percept)
    
    # 4. Brain thinks and processes
    brain.step(step)
    
    # 5. Brain decides what to do
    action = agent.decodeMotorCommand()
    
    # 6. Do the action in the world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Optional: Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
```

### Step 3: Check Results
```python
print("Simulation finished!")
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Development stage: {brain.getDevelopmentalStage().name}")
```

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with thousands of neurons and connections.
- `config` = brain settings (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. **Always call this before using the brain!**

### `brain.step(step_number)`
Makes the brain process one moment in time:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

### `world.applyMotorCommand(action, time)`
Executes the brain's decision in the world.

## Easy Examples

### Example 1: Silent Brain
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print(f"Silent brain test done! Brain has {brain.getTotalNeuronCount()} neurons")
```

### Example 2: Brain Watching a World
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

print(f"Watched world for 30 steps. Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Example 3: Learning Agent
```python
import pynlm

# Setup with learning enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Run agent with learning
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
print(f"Final development stage: {brain.getDevelopmentalStage().name}")
```

## Common Patterns

### Pattern 1: Run a Complete Simulation
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

### Pattern 2: Monitor Brain Stats
```python
# Add these inside your loop to monitor brain:
print("=== Brain Status ===")
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Development stage: {brain.getDevelopmentalStage().name}")
print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

### Pattern 3: Enable Learning Features
```python
# Enable these features for advanced usage:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
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

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`? (Always call this!)
- Try increasing the number of steps
- Enable learning features for more activity

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps (100+ recommended)
- Some neurons need input to fire - make sure sensory input is connected

## Advanced Usage

### Multi-Agent Systems
```python
# Create multiple brains and agents for parallel processing
brains = [pynlm.createBrain(pynlm.createDefaultConfig()) for _ in range(5)]
for brain in brains:
    brain.initialize()

agents = [pynlm.createAgentBrain(brain) for brain in brains]
# Each agent can control a different world or same world
```

### Save and Load Brain States
```python
# Save brain for later use
brain.save("my_brain.nlm")

# Load brain later
loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
loaded_brain.load("my_brain.nlm")
```

### Custom Configuration
```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)  # More neurons
config.set("plasticity_learning_rate", 0.05)  # Faster learning
config.set("log_level", "DEBUG")  # More verbose logging

brain = pynlm.createBrain(config)
```

## Next Steps

When you're comfortable:
1. **Read `README.md`** for detailed technical information
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Experiment with different configurations!**
4. **Try the demo executables** (`./nlm_phase6_demo`)

## Installation

```bash
# Install from source
mkdir build && cd build
cmake ..
make -j4
pip install .

# Or install via pip
pip install pynlm
```

That's it! You're now ready to use NLM and create your own artificial brains!