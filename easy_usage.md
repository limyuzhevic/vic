# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (Neural Learning Machine) is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send electrical signals to each other
- **Synapses** = Connections between brain cells that transmit signals
- **Learning** = When connections get stronger (long-term potentiation) or weaker (long-term depression) based on what happens
- **Memory** = The brain remembering patterns, events, or associations

That's it! NLM simulates all of this complex neural activity to create artificial developmental systems.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks, learns, and remembers
2. **World** - The simulated environment where the brain operates
3. **Agent** - The interface that connects the brain to the world, handling input/output

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain with default settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Initialize the brain (create neurons and connections)
brain.initialize()

# Step 3: Make it think/process for 100 simulation steps
for step in range(100):
    brain.step(step)  # One step of thinking/processing

print("Done! Your brain generated", brain.getTotalSpikeCount(), "spikes")
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

# Run for 50 simulation steps
for step in range(50):
    world.update(0.1)  # Update world state
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain receives and processes sensory input
    brain.step(step)  # Brain processes internal computation
    action = agent.decodeMotorCommand()  # Brain decides what action to take
    world.applyMotorCommand(action, world.getSimulationTime())  # Execute action in world

print("Simulation finished!")
print("Currently active neurons:", brain.getFiringNeuronCount())
```

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain with default settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it (creates neurons and establishes basic connectivity)
brain.initialize()

# Make it active (perform one computation step)
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Patterns

### Pattern 1: Run a Complete Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world state for this timestep
        world.update(0.1)
        
        # 2. Get sensory input from the world
        percept = world.getSensoryPercept()
        
        # 3. Process the sensory input with the agent's brain
        agent.processSensoryInput(percept)
        
        # 4. Brain performs internal computation
        brain.step(step)
        
        # 5. Brain produces motor action based on neural activity
        action = agent.decodeMotorCommand()
        
        # 6. Execute the action in the world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Monitor Brain Activity

```python
# Just add these statements inside your simulation loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes generated:", brain.getTotalSpikeCount())
print("Average firing rate:", brain.getAverageFiringRate())
print("Current developmental stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Advanced Brain Features

```python
# These are all optional features that can be enabled:
agent.enableRewardModulation(True)      # Learn from rewards and punishments
agent.enableStructuralPlasticity(True)  # Brain can grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Brain is motivated to explore novel things
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses. Use `pynlm.createDefaultConfig()` for simple setups.

### `brain.initialize()`
Initializes the brain, creating neurons and establishing synaptic connections. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron for firing
- Propagates spikes between connected neurons
- Updates connections based on plasticity rules (learning)

### `world.update(time)`
Updates the virtual world state by the specified time duration (typically in seconds).

### `agent.processSensoryInput(percept)`
Injects sensory information (vision, touch, audio, internal signals) into the brain's sensory neurons for processing.

### `agent.decodeMotorCommand()`
Reads the brain's motor neuron activity and converts it into an executable action/movement command.

---

## Mini Projects

### Project 1: Silent Brain (Basic Neural Activity)

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
print("Average firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Agent with Learning

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

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent simulation
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

| What you want to do | Python code |
|---------------------|-------------|
| Create a brain with default settings | `pynlm.createBrain(pynlm.createDefaultConfig())` |
| Create a brain with custom config | `pynlm.createBrain(my_config)` |
| Initialize brain (create neurons) | `brain.initialize()` |
| Run one simulation step | `brain.step(step_number)` |
| Create a simple world environment | `pynlm.createSimpleWorld()` |
| Create an agent interface | `pynlm.createAgentBrain(brain)` |
| Get number of firing neurons | `brain.getFiringNeuronCount()` |
| Get total spike count | `brain.getTotalSpikeCount()` |
| Get average firing rate | `brain.getAverageFiringRate()` |
| Get sensory input from world | `world.getSensoryPercept()` |
| Process sensory input into brain | `agent.processSensoryInput(percept)` |
| Get motor command from brain | `agent.decodeMotorCommand()` |
| Execute action in world | `world.applyMotorCommand(action, time)` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`? (Required to create neurons and connections)
- Try increasing the number of simulation steps
- Check if neurons have received enough current input

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()` to execute the action?
- Check that `world.update()` is being called before getting percepts
- Verify the world has valid positions and the agent is within bounds

**"Everything is 0"**
- Brains need time to "warm up" - try more simulation steps
- Some neurons need input to fire - ensure sensory input is connected to sensory neurons
- Check if the world has objects that the agent can perceive

---

## Next Steps

When you're comfortable with the basics:

1. **Read `HOW_TO_USE.md` for more advanced usage patterns**
2. **Read `docs/ARCHITECTURE.md` to understand how all the neural systems work together**
3. **Experiment with different configurations and world types**
4. **Create your own custom experiments**

That's it! You're now ready to use NLM to create artificial developmental brains.
