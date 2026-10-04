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

---

## NEW: Command-Line Interface (CLI)

The NLM project now includes a powerful command-line interface that makes it even easier to use and automate brain simulations.

### Quick CLI Usage

```bash
# Show help
nlm_cli_exec --help

# Show version
nlm_cli_exec --version

# Create a new brain (1000 neurons, 1 region)
nlm_cli_exec new --neurons 1000 --regions 1

# Run a 2000-step simulation
nlm_cli_exec run --steps 2000

# Get brain status and statistics
nlm_cli_exec status

# Save current brain state to file
nlm_cli_exec save /tmp/brain_state.bin

# Load brain state from file
nlm_cli_exec load /tmp/brain_state.bin

# Export brain data to text format
nlm_cli_exec export txt /tmp/brain_data.txt

# Start interactive session for real-time control
nlm_cli_exec interactive
```

### CLI Commands

| Command | Description | Example |
|---------|-------------|---------|
| `new` | Create a new brain | `nlm_cli_exec new --neurons 500 --regions 2` |
| `run` | Run simulation | `nlm_cli_exec run --steps 1000` |
| `status` | Show brain status | `nlm_cli_exec status` |
| `save` | Save brain state | `nlm_cli_exec save my_brain.bin` |
| `load` | Load brain state | `nlm_cli_exec load my_brain.bin` |
| `export` | Export data | `nlm_cli_exec export txt data.txt` |
| `interactive` | Start interactive mode | `nlm_cli_exec interactive` |
| `help` | Show help | `nlm_cli_exec help new` |
| `version` | Show version | `nlm_cli_exec --version` |

### Integrating CLI with Python API

You can combine the Python API for complex scripting with the CLI for quick operations:

```python
import pynlm
import subprocess

# Create and run simulation with Python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Save to file
brain.save("/tmp/temp_brain.bin")

# Run continued simulation with CLI (if needed)
# subprocess.run(["nlm_cli_exec", "load", "/tmp/temp_brain.bin"])
# subprocess.run(["nlm_cli_exec", "run", "--steps", "500"])

# Or use Python entirely for this example
print("Python API usage demonstrated above.")
print("CLI available for command-line operations.")
```

### Interactive Mode Features

In interactive mode, you can:
- Run commands like `run --steps 1000`
- Get status with `status`
- Save brains with `save filename`
- Load brains with `load filename`
- Change configuration with `set key value`
- Exit with `exit` or `quit`

### Configuration Options

The CLI supports these configuration options:

```bash
nlm_cli_exec new [options]
Options:
  --neurons N     Number of neurons (default: 1000)
  --regions R     Number of regions (default: 1)

nlm_cli_exec run [options]
Options:
  --steps S       Number of simulation steps (default: 1000)
  --config C      Configuration file path
  --output O      Output file path

nlm_cli_exec export FORMAT [filepath]
Formats:
  txt            Text format (default)
  json           JSON format (not yet implemented)
  bin            Binary checkpoint format
```

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md`** for advanced usage details
2. **Read `docs/ARCHITECTURE.md`** to understand system design
3. **Experiment with different configurations** using CLI
4. **Try the interactive mode** for real-time control
5. **Combine Python API and CLI** for complex workflows
6. **Use examples in `EXAMPLES.md`** for practical usage

**The NLM project now offers the best of both worlds:**

- **Python API**: For programmatic control and complex scripting
- **Command-Line Interface**: For quick operations, automation, and interactive use
- **Both work together**: Seamlessly for maximum flexibility

That's it! You're now ready to use NLM with both powerful programming interfaces.
