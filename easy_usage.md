# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

## ✨ NEW: Enhanced Python Features

NLM now includes advanced Python features to make development easier and more powerful:

### 1. Context Managers (Automatic Resource Management)
```python
# Create brain with automatic initialization and cleanup
with pynlm.with_brain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    for step in range(100):
        brain.step(step)
    # Brain automatically cleaned up
```

### 2. Enhanced Configuration (Pythonic Access)
```python
# Configure with intuitive syntax
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Set value
config["plasticity.rate"] = 0.01        # Pythonic indexing
if "brain.neuron_count" in config:     # Check existence
    value = config["brain.neuron_count"]
```

### 3. Advanced Simulation Utilities
```python
# Run complete simulation with one call
results = pynlm.run_simulation(
    brain, world, agent, 
    steps=1000, timestep=0.1
)
# Returns list of step data with spike counts, firing rates, etc.

# Run training episode with all learning enabled
results = pynlm.run_training_episode(
    brain, world, agent, 
    steps=500, timestep=0.1,
    verbose=True
)
```

### 4. Performance Monitoring
```python
# Profile critical operations
time_info = pynlm.profile_operation(
    lambda: brain.step(10), 
    "brain_step_10"
)
print(f"Operation took {time_info[1]} ms")
```

### 5. Data Analysis
```python
# Analyze simulation results
analysis = pynlm.analyze_simulation_results(results)
print(f"Mean spikes: {analysis['spike_count_mean']}")
print(f"Spike std: {analysis['spike_count_std']}")
print(f"Max firing rate: {analysis['firing_rate_mean']}")
```

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain (NEW - with context manager)

```python
import pynlm

# Step 1: Create brain with automatic management
with pynlm.with_brain(pynlm.createDefaultConfig()) as brain:
    # Step 2: Make it think for 100 steps
    for step in range(100):
        brain.step(step)  # One step of thinking
    
    print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Advanced Brain in World (NEW - with utilities)

```python
import pynlm

# Create everything with enhanced features
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with advanced utilities
results = pynlm.run_simulation(
    brain, world, agent,
    steps=50, timestep=0.1
)

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Collected", len(results), "data points")
```

### Example 3: Training with Learning (NEW - with training utilities)

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run complete training episode with all learning
results = pynlm.run_training_episode(
    brain, world, agent,
    steps=200, timestep=0.1,
    verbose=True
)

print("Training complete!")
print("Final spike count:", results[-1]['spike_count'])
print("Average curiosity:", sum(r['curiosity_level'] for r in results) / len(results))
```

---

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    return pynlm.run_simulation(brain, world, agent, num_steps)

# Use it like this:
results = run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Stats are automatically collected in run_simulation
for step_data in results:
    print(f"Step {step_data['step']}: {step_data['spike_count']} spikes,")
    print(f"  firing: {step_data['firing_neurons']},")
    print(f"  curiosity: {step_data['curiosity_level']:.2f}")
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

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain with automatic setup
with pynlm.with_brain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    brain.step(0)
    print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

with pynlm.with_brain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    for i in range(10):
        brain.step(i)
    print("Silent brain test done!")
```

### Project 2: Brain Watching a World (NEW - with utilities)

```python
import pynlm

# Setup with enhanced config
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 200)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world with data collection
results = pynlm.run_simulation(brain, world, agent, 30)

print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
print("Data points collected:", len(results))
```

### Project 3: Complete Agent with Learning (NEW - with training)

```python
import pynlm

# Setup with training capabilities
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent with complete training
results = pynlm.run_training_episode(brain, world, agent, 100)

print("Agent simulation complete!")
for step_data in results:
    if step_data['step'] % 20 == 0:
        print(f"Step {step_data['step']}: {step_data['firing_neurons']} neurons firing")
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
| | **NEW FEATURE** |
| Run simulation | `pynlm.run_simulation(brain, world, agent, steps)` |
| Run training | `pynlm.run_training_episode(brain, world, agent, steps)` |
| Profile code | `pynlm.profile_operation(lambda: ..., "name")` |
| Analyze data | `pynlm.analyze_simulation_results(results)` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try using `pynlm.run_simulation()` for complete control

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Make sure `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Use `pynlm.run_training_episode()` to enable learning
- Some neurons need input to fire - make sure sensory input is connected

**"Performance issues"**
- Use `pynlm.profile_operation()` to identify bottlenecks
- For large simulations, use `pynlm.run_simulation()` with efficient loops

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try advanced features with `demo_improvements.py`

That's it! You're now ready to use NLM with all its powerful new features.

### Bonus: Check out the improvements demo!
```bash
python demo_improvements.py
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

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
