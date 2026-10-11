# NLM - Easy Usage Guide for Beginners (Simplified Interface)

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

## New: Simplified PyNLM Interface

Starting with version 0.1.0, NLM now includes a **simplified Python interface** (`pynlm`) that makes it much easier to use NLM for experimentation and research.

### Key Improvements:

1. **Simplified API**: Uses Pythonic `snake_case` instead of `camelCase`
2. **Beginner-Friendly**: Provides sensible defaults and high-level convenience functions
3. **Development Mode**: Includes mock implementation when C++ bindings aren't available
4. **Advanced Features**: Still accessible for expert users with `create_advanced_config()`
5. **Better Documentation**: Comprehensive examples and troubleshooting

### Quick Start with Simplified Interface

```python
import pynlm

# Create a simple brain (one line!)
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Make it think for 100 steps
for i in range(100):
    brain.step(i)

print("Done! Your brain thought", brain.get_total_spike_count(), "times")
```

**Important**: Use `pynlm` for the simplified Python interface!

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

### Project 2: Brain Watching a World

```python
import pynlm

# Setup (using simplified interface)
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()
world = pynlm.create_simple_world()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.get_average_firing_rate())
```

### Project 3: Complete Agent

```python
import pynlm

# Setup (using simplified interface)
config = pynlm.create_advanced_config()
brain = pynlm.create_brain(config)
brain.initialize()
world = pynlm.create_simple_world()
world.configure(width=15, height=15, vision_width=8, vision_height=8)
world.reset()
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Enable learning
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.get_firing_neuron_count()} neurons firing")

print("Agent simulation complete!")
```

---

## Quick Reference (PyNLM Interface)

| What you want | PyNLM Code |
|--------------|------------|
| Create brain | `pynlm.create_brain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.create_simple_world()` |
| Create agent | `pynlm.create_agent_brain(brain)` |
| Get brain stats | `brain.get_firing_neuron_count()` |
| See world | `world.get_sensory_percept()` |
| Make action | `agent.decode_motor_command()` |

---

## Troubleshooting (PyNLM)

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.apply_motor_command()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Next Steps (PyNLM)

When you're comfortable with the simplified interface:

1. **Read `HOW_TO_USE.md` for more details** - Contains advanced usage patterns
2. **Explore `docs/ARCHITECTURE.md` to understand how it all works**
3. **Experiment with different configurations!** - Try various `create_advanced_config()` settings
4. **Try the command-line interface!** - Use `nlm` command for batch processing
5. **Create your own mini-projects** - Apply NLM to your specific use case

That's it! You're now ready to use the simplified NLM interface.

