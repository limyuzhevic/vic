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

### `brain.getTotalNeuronCount()`
Returns the total number of neurons in the brain.

### `brain.getTotalSynapseCount()`
Returns the total number of synapses in the brain.

### `brain.getFiringNeuronCount()`
Returns the number of neurons currently firing.

### `brain.getTotalSpikeCount()`
Returns the total number of spikes fired by all neurons.

### `brain.getAverageFiringRate()`
Returns the average firing rate across all neurons.

### `brain.getExcitationInhibitionRatio()`
Returns the balance between excitation and inhibition.

### `brain.getDevelopmentalStage()`
Returns the current developmental stage of the brain.

## What Each Part Does (Expanded)

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

## Neuron API (NEW - Added)

### Accessing Individual Neurons

```python
# Get neurons from brain regions
neurons = brain.getRegions()
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    print(f"Region {region_id} has {region.getPopulationCount()} neurons")

# Create and work with individual Neuron objects
import pynlm
neuron_id = pynlm.INVALID_NEURON_ID  # Get from actual neurons
neuron = pynlm.Neuron(neuron_id)

# Query neuron state
print(f"Neuron ID: {neuron.getId()}")
print(f"Neuron type: {neuron.getType()}")
print(f"Membrane potential: {neuron.getMembranePotential()} mV")
print(f"Is firing: {neuron.isFiring()}")
print(f"Is refractory: {neuron.isRefractory()}")
print(f"Threshold: {neuron.getThreshold()} mV")
print(f"Firing rate: {neuron.getFiringRate()} Hz")
print(f"Last spike time: {neuron.getLastSpikeTime()}")

# Check plasticity flags
flags = neuron.getPlasticityFlags()
print(f"Hebbian plasticity: {flags.hebbian}")
print(f"STDP plasticity: {flags.stdp}")
print(f"Reward-modulated: {flags.reward_modulated}")

# Neuron location and organization
print(f"Region ID: {neuron.getRegionId()}")
print(f"Population ID: {neuron.getPopulationId()}")
print(f"Incoming synapses: {len(neuron.getIncomingSynapses())}")
print(f"Outgoing synapses: {len(neuron.getOutgoingSynapses())}")
print(f"Spike history: {len(neuron.getSpikeHistory())} recent spikes")

# Modify neuron state
neuron.setType(pynlm.NeuronType.Excitatory)
neuron.addToMembranePotential(1.5)
neuron.setFiringRate(10.0)

# Enable plasticity
neuron.enablePlasticity(True, True, True)

# Get neuron state object for detailed inspection
state = neuron.getState()
print(f"Resting potential: {state.restingPotential} mV")
print(f"Refractory period: {state.refractoryPeriod} steps")
print(f"Adaptation variable: {state.adaptationVariable}")
```

## Mini Project: Neuron Investigation

### Explore Brain Structure

```python
import pynlm

# Setup a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Explore brain regions and neurons
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    print(f"\nRegion: {region_id}")
    print(f"  Population count: {region.getPopulationCount()}")
    
    # This would require accessing neurons, but current API doesn't expose them directly
    # In future versions, you could iterate through neurons and examine their properties
    
print("\nBrain statistics:")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio()}")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

## Next Steps (Updated)

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details on building and installation
2. Read `docs/ARCHITECTURE.md` to understand how the brain regions and neurons work together
3. Experiment with neuron-specific features like plasticity and development
4. Build advanced applications that leverage the full Neuron API
5. Read `docs/SCIENCE.md` for the scientific background of neural dynamics

That's it! You're now ready to use NLM with both high-level brain operations and low-level neuron control.

**Note:** The Neuron API is new and expanding. Future versions will include more methods for detailed neuron manipulation and analysis.

(End of file - total 329 lines)
