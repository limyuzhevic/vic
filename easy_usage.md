# NLM - Easy Usage Guide for Beginners (Updated)

## What is NLM?

NLM (熙然, meaning "serene flow") is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

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
brain = pynlm.create_brain(pynlm.create_default_config())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.spike_count, "times")
```

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

world = pynlm.create_world()
world.configure(width=10, height=10, vision_width=8, vision_height=8)
world.reset()

agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.get_sensory_percept()  # What does the agent see?
    agent.process_sensory_input(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decode_motor_command()  # Brain decides action
    world.apply_motor_command(action, world.get_simulation_time())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.firing_count)
```

---

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.get_sensory_percept()` |
| Your brain processes it | `agent.process_sensory_input(percept)` |
| You decide to act | `action = agent.decode_motor_command()` |
| You do the action | `world.apply_motor_command(action, time)` |
| Your brain learns from results | `agent.apply_reward_modulation(reward, predicted)` |

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.create_brain(pynlm.create_default_config())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.neuron_count, "neurons!")
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
        percept = world.get_sensory_percept()
        
        # 3. Tell the brain
        agent.process_sensory_input(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decode_motor_command()
        
        # 6. Do action in world
        world.apply_motor_command(action, world.get_simulation_time())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.firing_count)
print("Total spikes:", brain.spike_count)
print("Avg firing rate:", brain.average_firing_rate)
print("Development stage:", brain.get_developmental_stage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enable_reward_modulation(True)      # Learn from rewards
agent.enable_structural_plasticity(True)  # Grow new connections
agent.enable_development(True)           # Brain matures over time
agent.enable_curiosity(True)             # Explore new things
```

---

## What Each Part Does

### `pynlm.create_brain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `create_default_config()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.process_sensory_input(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decode_motor_command()`
Reads the brain's motor neurons to decide what action to take.

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()
world = pynlm.create_world()
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
print("Firing rate:", brain.average_firing_rate)
```

### Project 3: Complete Agent

```python
import pynlm

# Setup
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()
world = pynlm.create_world()
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
        print(f"Step {step}: {brain.firing_count} neurons firing")

print("Agent simulation complete!")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.create_brain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.create_world()` |
| Create agent | `pynlm.create_agent_brain(brain)` |
| Get brain stats | `brain.firing_count` |
| See world | `world.get_sensory_percept()` |
| Make action | `agent.decode_motor_command()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.apply_motor_command()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

**"New Pythonic API not working"**
- The new Pythonic API (snake_case) is available in the enhanced version
- Use `brain.firing_count` instead of `brain.getFiringNeuronCount()`
- Use `world.get_agent_position()` instead of `world.setAgentStart()`

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

## Pythonic API Note

The enhanced version includes a Pythonic API with snake_case naming:

- `brain.neuron_count` (instead of `brain.getTotalNeuronCount()`)
- `brain.synapse_count` (instead of `brain.getTotalSynapseCount()`)
- `world.get_agent_position()` (instead of `world.setAgentStart()`)
- `agent.is_reward_modulation_enabled()` (instead of `agent.isRewardModulationEnabled()`)

This makes the API more Pythonic and easier to use.

That's it! You're now ready to use NLM with both the original and enhanced APIs.
