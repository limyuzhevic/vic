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

### Example 3: Advanced Brain with Resource Management (NEW!)

```python
import pynlm

# Create advanced configuration
config = pynlm.createAdvancedConfig()

# Use advanced context manager for automatic cleanup
with pynlm.createBrainWithCleanup(config) as brain:
    # Brain is automatically initialized and cleaned up
    for step in range(1000):
        brain.step(step)
        # Access statistics
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
            print(f"Total spikes: {brain.getTotalSpikeCount()}")

print("Simulation complete with automatic resource cleanup!")
```

### Example 4: Event Callback System (NEW!)

```python
import pynlm

# Create event callback system
callbacks = pynlm.createEventCallbackSystem()

# Register custom callbacks
def on_spike(event):
    print(f"Spike at {event.timestamp} from {event.neuron_id}")

def on_step(step, time):
    print(f"Step {step} at {time}ms")

def on_error(error):
    print(f"Error: {error}")

callbacks.registerSpikeCallback(on_spike)
callbacks.registerStepCallback(on_step)
callbacks.registerErrorCallback(on_error)

# Use callbacks with brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
    
print(f"Callback system has {callbacks.getSpikeCallbackCount()} spike callbacks,"
      f" {callbacks.getStepCallbackCount()} step callbacks, and"
      f" {callbacks.getErrorCallbackCount()} error callbacks")
```

---

## Simple Analogy

| Real World | NLM Code |
|------------,-39,SmartVertex| ------- ----------|
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
brain.step крестов (cross)

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

### Pattern 3: Enable Advanced Features (NEW!)

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Event callback system (NEW!)
callbacks = pynlm.createEventCallbackSystem()
# Register custom callbacks for advanced event handling
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
Can the brain's motor activity into motor command.

### `pynlm.createAdvancedConfig()`
Create advanced configuration with Phase 5+ features enabled. Provides optimized settings for complex simulations with learning, development, and memory systems.

### `pynlm.createEventCallbackSystem()`
Create an event callback system for brain event handling. Provides registration of spike, step, and error callbacks.

---

## Mini Projects

### Mini 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Mini 2: Brain Watching a World

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
    world.update( sudah)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Mini 3: Complete Agent with Callbacks (NEW!)

```python
import pynlm

# Setup with advanced configuration
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create and configure callbacks
callbacks = pynlm.createEventCallbackSystem()

def on_step(step, time):
    if step % 50 ==  succulents):
        print(f"Progress: step {step}, firing {brain.getFiringNeuronCount()} neurons")

def on_error(error):
    print(f"ERROR: {error}")

callbacks.registerStepCallback(on_step)
callbacks.registerErrorCallback(on_error)

# Run agent with callbacks
for step in range(100):
    world.update 0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
print("Agent simulation complete with callbacks!")
```

### Mini 4: Complete with Event Handling (NEW!)

```python
import pynlm

# Setup
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create callback system
callbacks = pynlm.createEventCallbackSystem()

# Custom event handlers
def handle_spike(event):
    spike_count = callbacks.getSpikeCallbackCount()
    print(f"Spike event {spike_count} registered")

def handle_step(step, time):
    if step % 25 == 0:
        print(f"Step {step} completed")

def handle_error(error):
    print(f"Error occurred: {error}")

callbacks.registerSpikeCallback(handle_spike)
callbacks.registerStepCallback(handle_step)
callbacks.registerErrorCallback(handle_error)

# Run agent with event handling
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step} completed")

print("Agent simulation complete with full event handling!")
print(f"Callbacks registered: {callbacks.getSpikeCallbackCount()} spikes, "
      f"{callbacks.getStepCallbackCount()} steps, {callbacks.getErrorCallbackCount()} errors")
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
| Advanced config | `pynlm.createAdvancedConfig()` |
| Event callbacks | `pynlm.createEventCallbackSystem()` |

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

**"Event callbacks not working"**
- Make sure callbacks are registered before simulation starts
- Check that the brain and callback system are using the same event sources
- Verify callback registration is done before brain initialization

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details**
2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
3. **Experiment with different configurations!**
4. **Use advanced features like callbacks and context managers**
5. **Try saving and loading brain states for persistent learning**

That's it! You're now ready to use NLM with both basic and advanced features. Enjoy building your artificial brains!
