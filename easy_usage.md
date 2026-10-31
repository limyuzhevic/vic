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
## Advanced Usage: Command Line Interface (C++)

If you're using the C++ version of NLM (not the Python bindings), you can run simulations directly from the command line with extensive configuration options.

### Basic Command Line Usage

```bash
# Run the default Phase 2 demo (neuron learning experiments)
nlm

# Run with custom configuration
nlm --config=myconfig.cfg

# Run with specific parameters
nlm --neuron-count=1000 --timestep=0.01 --seed=123

# View help information
nlm --help
```

### Configuration File Examples

**Simple key=value format** (`configs/default.cfg`):
```
# NLM Configuration File
# ===============

# Brain parameters
neuron_count = 500
region_count = 1
connection_probability = 0.15
simulation_timestep = 0.001
random_seed = 42

# STDP plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

**JSON format** (`configs/brain_config.json`):
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    "stdp_ltp_weight": 0.025,
    "stdp_ltd_weight": 0.02,
    "stdp_tau": 25.0,
    "synaptogenesis_rate": 0.0002,
    "pruning_rate": 0.00002,
    "enable_plasticity": true
}
```

### Command Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `--config[=FILE]` | Load configuration from FILE | `configs/default.cfg` |
| `--neuron-count=N` | Set number of neurons | 500 |
| `--region-count=N` | Set number of brain regions | 1 |
| `--timestep=T` | Set simulation timestep | 0.001 |
| `--seed=N` | Set random seed | 42 |
| `--verbose` | Enable verbose logging | false |
| `--quick-run` | Skip detailed experiments, run quick validation | false |
| `--stdp-ltp-weight=W` | Set STDP LTP weight | 0.02 |
| `--stdp-ltd-weight=W` | Set STDP LTD weight | 0.015 |
| `--stdp-tau=T` | Set STDP time constant | 20.0 |
| `--synaptogenesis-rate=R` | Set synaptogenesis rate | 0.0001 |
| `--pruning-rate=R` | Set pruning rate | 0.00001 |

### Practical Examples

```bash
# Run a larger brain with more neurons
nlm --neuron-count=2000 --region-count=2

# Run with faster simulation
nlm --timestep=0.01 --seed=123

# Run with enhanced plasticity
nlm --stdp-ltp-weight=0.03 --stdp-ltd-weight=0.018 --synaptogenesis-rate=0.0002

# Quick validation (skips detailed experiments)
nlm --quick-run --verbose

# Load configuration from JSON file
nlm --config=brain_config.json

# Run specific plasticity experiments
nlm --enable-plasticity=hebbian --enable-plasticity=stdp
```

### Configuration Parameters Reference

The following configuration parameters are available:

- **`random_seed`**: Random number generator seed for reproducible results
- **`simulation_timestep`**: Neural simulation timestep in seconds
- **`neuron_count`**: Total number of neurons in the brain
- **`region_count`**: Number of brain regions
- **`connection_probability`**: Probability of forming synaptic connections
- **`stdp_ltp_weight`**: STDP LTP learning rate (long-term potentiation)
- **`stdp_ltd_weight`**: STDP LTD learning rate (long-term depression)
- **`stdp_tau`**: STDP time constant for learning decay
- **`synaptogenesis_rate`**: Rate of new synapse formation
- **`pruning_rate`**: Rate of synapse elimination

### Advanced Usage Tips

1. **Combine options**: Mix and match configuration options for custom experiments
2. **Use relative paths**: Configuration files can use relative paths: `--config=./config/my_brain.cfg`
3. **Override defaults**: Command-line options override configuration file settings
4. **Logging levels**: Use `--verbose` to see detailed simulation progress
5. **Experiment modes**: Use `--quick-run` for fast testing or omit for full experiments

### Configuration File Format Support

NLM supports multiple configuration file formats:

- **Simple key=value**: Traditional format, human-readable
- **JSON**: Structured format, good for complex configurations
- **YAML**: YAML format, good for readable configurations

To use a different format, simply save your configuration file with the appropriate extension and specify it with `--config`.

### Debugging and Troubleshooting

If you encounter issues:

```bash
# Run with verbose logging
nlm --verbose

# Use a simpler configuration
nlm --neuron-count=100

# Check configuration syntax
nlm --config=config_file.cfg
```

See `HOW_TO_USE.md` for more detailed documentation and building instructions.
