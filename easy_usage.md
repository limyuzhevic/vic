# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

NLM has evolved significantly with **Phase 6 enhancements** including:

**NEW in Phase 6:**
- **JSON Configuration Support** - Modern JSON config files with full backward compatibility
- **Advanced Configuration Management** - Comprehensive config system for all parameters
- **Improved Documentation** - Clear examples and comprehensive guides
- **Enhanced Performance** - Optimized for research-scale neural simulations

**Key Features:**
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)
- Working memory and episodic memory systems
- Neuromodulation (dopamine, curiosity, novelty detection)
- Predictive planning and attention mechanisms
- Developmental stages (Initial → CriticalPeriod → Maturation → Adult → Aging)

That's it! NLM simulates all of this.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain with JSON Config

```python
import pynlm

# Step 1: Create brain with JSON config file
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Step 2: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain with Custom JSON Configuration

```python
import pynlm

# Create config with custom settings
config = pynlm.createDefaultConfig()
config.set("random_seed", 123)
config.set("neuron_count", 100)
config.set("simulation_timestep", 0.001)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(200):
    brain.step(step)

print("Custom brain simulation complete!")
print("Neurons:", brain.getTotalNeuronCount())
print("Firing rate:", brain.getAverageFiringRate())
```

### Example 3: Brain in a Simple World (JSON Config)

```python
import pynlm
import json

# Create world with JSON config
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run interaction loop
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Development stage:", brain.getDevelopmentalStage())
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

### Example 4: JSON Configuration Files

NLM supports both JSON and key=value format for configuration files.

#### JSON Config Example:

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001
}
```

You can load this with:
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.getConfig().loadFromFile("my_config.json")
```

#### key=value Config (Legacy Support):

```
# configs/legacy.cfg
# Random seed
random_seed = 42
# Simulation timestep (seconds)
simulation_timestep = 0.001
# Number of neurons
neuron_count = 500
# Number of brain regions
region_count = 1
# Connection probability
connection_probability = 0.15
```

Both formats are supported for backward compatibility.

---

## Common Patterns

### Pattern 1: Run a Simulation with Config

```python
def run_simulation_with_config(brain, world, agent, config_file, num_steps):
    # Load configuration
    config = pynlm.createDefaultConfig()
    config.loadFromFile(config_file)
    
    # Create brain with config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it
run_simulation_with_config(brain, world, agent, "my_config.json", 1000)
```

### Pattern 2: Brain Statistics with Config

```python
# Get configuration info
config = brain.getConfig()
print("Config keys:", config.getKeys())
print("Random seed:", config.getOr("random_seed", 42))
print("Neuron count:", config.getOr("neuron_count", 500))

# Print full config
print(config.summary())
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

## What Each Part Does (Updated)

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)
- Config can be JSON-based for advanced settings or key=value format for legacy compatibility

### `brain.getConfig()`
Gets the brain's configuration object for advanced settings:
```python
config = brain.getConfig()
config.set("random_seed", 123)  # Set a specific seed
config.set("neuron_count", 1000)  # Override default count
config.loadFromFile("my_config.json")  # Load from JSON file
```

### `config.loadFromFile(filepath)`
Loads configuration from file. Now supports both:
- **JSON format** (modern): `configs/modern.json`
- **key=value format** (legacy): `configs/legacy.cfg`

### `config.loadFromArgs(args)`
Override config with command line arguments:
```bash
./program --neuron_count=1000 --random_seed=42
```

---

## Mini Projects (Enhanced)

### Project 1: JSON Configuration

```python
import pynlm
import json

# Option 1: Create config programmatically
config = pynlm.createDefaultConfig()
config.set("random_seed", 123)
config.set("neuron_count", 500)

# Option 2: Load from JSON file
config = pynlm.createDefaultConfig()
with open("my_config.json", "w") as f:
    json.dump({
        "random_seed": 123,
        "neuron_count": 500,
        "simulation_timestep": 0.001
    }, f)

config.loadFromFile("my_config.json")

brain = pynlm.createBrain(config)
brain.initialize()
print("Loaded config:", config.summary())
```

### Project 2: Brain with Advanced JSON Settings

```python
import pynlm

# Create brain with full JSON config
config = pynlm.createDefaultConfig()

# Set multiple values (equivalent to JSON)
settings = {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.2,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0
}

for key, value in settings.items():
    config.set(key, value)

brain = pynlm.createBrain(config)
brain.initialize()

print("Configured brain:")
print(f"  Neurons: {brain.getTotalNeuronCount()}")
print(f"  Regions: {brain.getRegionCount()}")
print(f"  Random seed: {config.getOr('random_seed', 42)}")
```

### Project 3: Complete Agent with JSON Configuration

```python
import pynlm

# Create brain with JSON-like configuration
config = pynlm.createDefaultConfig()

# Enable learning features
config.set("enable_reward_modulation", True)
config.set("enable_curiosity", True)
config.set("enable_structural_plasticity", True)
config.set("enable_development", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable features via agent interface (overrides config)
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
print("Config used:", config.summary())
```

---

## Quick Reference (Enhanced)

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Create default config | `pynlm.createDefaultConfig()` |
| Load config from file | `config.loadFromFile("file.json")` |
| Set config value | `config.set("key", value)` |
| Get config value | `config.getOr("key", default)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Get config info | `brain.getConfig().summary()` |

---

## Troubleshooting (Enhanced)

**"My config file isn't loading properly"**
- Is the file at the right path?
- Is it valid JSON or key=value format?
- Try `config.loadFromFile("configs/default.cfg")` for legacy format

**"Config values aren't working"**
- Config must be loaded before brain initialization
- Call `brain = pynlm.createBrain(config)` BEFORE `brain.initialize()`
- Check `config.summary()` to verify values

**"Brain parameters aren't right"**
- Check `config.getOr("parameter_name", default_value)`
- Use `brain.getConfig()` to inspect current configuration
- Save config to file for debugging: `config.saveToFile("debug.json")`

---

## Next Steps

When you're comfortable:

1. **Explore JSON capabilities**: Read `docs/ARCHITECTURE.md` to understand advanced JSON configuration options
2. **Create custom configs**: Design your own JSON configs for specific experiments
3. **Performance tuning**: Use JSON to configure brain parameters for different performance levels
4. **Experiment recording**: Use `config.saveToFile()` to save experiment configurations

**NEW in Phase 6**: JSON configuration support makes it easier to:
- Share configurations between experiments
- Create reusable config templates
- Programmatically configure brains
- Document experiment parameters

That's it! You're now ready to use NLM with modern JSON configuration support.
