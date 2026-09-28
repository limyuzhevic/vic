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

## Command Line Interface (CLI)

The NLM project includes a powerful CLI tool (`nlm`) that lets you run simulations without writing Python code. The CLI provides several commands for different use cases:

### Basic Commands

```bash
# Show help
nlm --help

# Run a simple simulation
nlm simulate --steps 100

# Run with custom configuration
nlm simulate --config my_config.cfg --steps 500 --output json

# Run a world simulation
nlm simulate-world --width 20 --height 20 --steps 50

# Run batch processing with multiple configs
nlm batch config1.cfg config2.cfg --steps 200 --batches 5

# Export/import brain states
nlm brain save --file brain_state.json
nlm brain load --file saved_brain.json
```

### Advanced CLI Features

#### Simple Simulation

```bash
# Basic usage
nlm simulate --steps 1000

# With configuration file
nlm simulate --config production.cfg --steps 5000

# Different output formats
nlm simulate --steps 100 --output text      # Human readable (default)
nlm simulate --steps 100 --output json      # Machine readable
nlm simulate --steps 100 --output csv       # Spreadsheet format
```

#### World Simulation

```bash
# Standard world simulation
nlm simulate-world --width 30 --height 30 --vision-width 12 --vision-height 12 --steps 100

# Agent navigation with learning
nlm simulate-world --config agent_config.cfg --steps 1000 --output json
```

#### Batch Processing

```bash
# Run multiple configurations
nlm batch config_a.cfg config_b.cfg config_c.cfg --steps 500 --batches 3

# Different output formats for batch processing
nlm batch configs/*.cfg --steps 200 --batches 10 --output json
```

#### Configuration Management

```bash
# Export current configuration
nlm config export --output production_config.json

# Load configuration
nlm config export --config template.cfg --output my_config.json
```

#### Brain State Management

```bash
# Save brain state (checkpoint)
nlm brain save --file checkpoint_1000.json

# Load brain state (resume)
nlm brain load --file checkpoint_500.json
```

#### Example Commands

```bash
# Run built-in examples
nlm example simple          # Silent brain simulation
nlm example world           # Agent in world navigation
nlm example batch           # Batch processing demo
```

### CLI Script Installation

The CLI tool is installed as `nlm` in your system PATH. If you installed NLM via pip, you can run it directly. For development, you can also run it as:

```bash
# From the nlm-cli directory
python -m nlm_cli

# Or run the script directly
./nlm-cli/bin/nlm simulate
```

## Advanced Python API Methods

The enhanced Python bindings provide many more methods for advanced usage:

### Batch Processing

```python
# Run a batch of simulation steps efficiently
brain.runBatch(start_step=100, num_steps=500)

# Run an episode with statistics collection
stats = brain.runEpisode(num_steps=1000, record_stats=True)
for step_stat in stats:
    print(f"Step {step_stat['step']}: {step_stat['firing_neuron_count']} neurons firing")

# Run simulation with reward tracking
rewards = [0.1, 0.2, 0.0, 0.3, 0.1]
cumulative_rewards = brain.runWithRewards(5, rewards)
```

### Advanced Brain Manipulation

```python
# Create custom neurons with specific parameters
neuron_id = brain.createCustomNeuron(
    pynlm.NeuronType.Excitatory,
    x=1.0, y=2.0, z=0.0, radius=1.5
)

# Create custom synapses between specific neurons
brain.createCustomSynapse(
    pre=neuron_id,
    post=other_neuron_id,
    type=pynlm.SynapseType.Excitatory,
    weight=2.5,
    delay=2.0
)

# Add connections between neural regions
brain.addInterRegionConnection(
    source_region1,
    target_region2,
    weight=0.8,
    delay=5
)
```

### Visualization Support

```python
# Export comprehensive visualization data
viz_data = brain.exportForVisualization(format="json")

# Get neuron positions for 3D visualization
neurons = viz_data["neurons"]
for neuron in neurons:
    print(f"Neuron {neuron['id']}: position ({neuron['x']}, {neuron['y']}, {neuron['z']})")

# Get connectivity matrix for network analysis
connectivity = viz_data["connectivity"]

# Get spike train data for temporal analysis
spikes = viz_data["spikes"]
```

### Complete Examples

See the `examples/` directory for complete demonstration scripts:

- `silent_brain.py` - Simple brain simulation
- `agent_world.py` - Agent navigation in world
- `complete_agent.py` - Full agent with learning

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try the new CLI tool for command-line simulations
5. Explore the advanced examples in the examples/ directory

That's it! You're now ready to use NLM at all levels, from simple one-liners to complex, research-grade simulations!
