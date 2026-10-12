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

### Example 2: Advanced Configuration

```python
import pynlm

# Step 1: Create and configure brain
config = pynlm.createDefaultConfig()

# Set up advanced configuration
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("brain.threshold", -50.0)
config.set("plasticity.stdp.enable", true)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.curiosity.enable", true)
config.set("development.enable", true)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()

# Step 2: Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
world.reset()

# Step 3: Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Step 4: Run simulation for 500 steps
for step in range(500):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Advanced simulation complete!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Curiosity level:", agent.getCuriosityLevel())
```

### Example 3: Environment-Specific Configuration

```python
import pynlm

# Create configuration for different environments
config = pynlm.createDefaultConfig()

# Set environment-specific settings
config.set("brain.neuron_count", 1000)  # Smaller brain for mobile
config.set("brain.synapse_density", 0.05)  # Fewer connections
config.set("simulation.max_steps", 100)  # Shorter training
config.set("logging.level", "warning")  # Reduced logging

# For development environment
if pynlm.Config.isDevelopment():
    config.set("simulation.debug", True)
    config.set("logging.level", "debug")

brain = pynlm.createBrain(config)
brain.initialize()

# Run quick test
for i in range(50):
    brain.step(i)

print("Environment-specific simulation complete!")
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

## Command-Line Interface (CLI)

NLM now has a powerful command-line interface that makes it easy to run simulations without writing code. Here's how to use it:

### Basic CLI Usage

The CLI provides a simple command structure:

```bash
nlm [options] <command> [arguments]
```

**Commands:**
- `run` - Run a simulation
- `experiment` - Run an experiment
- `configure` - Manage configuration
- `analyze` - Analyze results
- `benchmark` - Performance benchmarking

### Quick Start with CLI

Here are the easiest ways to use the CLI:

#### 1. Simple Simulation

```bash
# Run a basic simulation with 500 neurons for 100 steps
nlm run --type basic --steps 100 --neurons 500

# Run a plasticity experiment with 1000 neurons for 1000 steps
nlm experiment --type plasticity --steps 1000 --neurons 1000

# Run a development experiment
nlm run --type development --steps 5000
```

#### 2. Using Presets

The CLI comes with preset configurations for common use cases:

```bash
# Simple neuron dynamics preset
nlm run --preset basic --steps 100

# Learning and adaptation preset
nlm experiment --preset plasticity --steps 1000

# Cognitive capabilities preset
nlm run --preset cognitive --steps 5000

# Social interaction preset
nlm run --preset social --steps 3000
```

#### 3. Configuration Management

Manage configuration files easily:

```bash
# Show current configuration
nlm configure --show

# Save current configuration to a file
nlm configure --save my_config.json

# Load a configuration file
nlm run --config my_config.json --type basic
```

#### 4. Help and Information

Get help and information about available options:

```bash
# Show general help
nlm --help

# Show help for a specific command
nlm help run

# List available presets
nlm --list-presets

# Show version information
nlm --version
```

### Advanced CLI Usage

#### Different Simulation Types

The CLI supports multiple simulation types:

```bash
# Basic neuron dynamics
nlm run --type basic --steps 100

# Plasticity learning (STDP and Hebbian)
nlm experiment --type plasticity --steps 1000

# Spike-timing-dependent plasticity (STDP) verification
nlm run --type stdp --steps 500

# Developmental stages with synaptogenesis and pruning
nlm run --type development --steps 5000

# Full cognitive capabilities
nlm run --type cognitive --steps 10000

# Social interaction simulation
nlm run --type social --steps 3000

# Memory-intensive scenarios
nlm run --type memory --steps 20000

# Attention and focus simulation
nlm run --type attention --steps 2000
```

#### Custom Configuration

Create custom simulations with specific parameters:

```bash
# Custom simulation with specific neuron count and steps
nlm run --type basic --steps 200 --neurons 750

# Simulation with custom region count
nlm run --type plasticity --steps 1500 --regions 3

# Combined with preset
nlm run --preset plasticity --steps 2000 --neurons 1200
```

#### Output and Analysis

The CLI provides various output formats and analysis options:

```bash
# Run simulation and save results
nlm run --type basic --steps 100 --output results.json

# Analyze simulation results
nlm analyze --format json --output analysis.json

# Run and analyze in one command
nlm run --type basic --steps 100 --output data.json && nlm analyze --format json --output final_analysis.json
```

#### Performance Benchmarking

Benchmark your system performance:

```bash
# Benchmark for 60 seconds
nlm benchmark --duration 60

# Benchmark with specific neuron count
nlm benchmark --duration 30 --neurons 500

# Benchmark development simulation
nlm benchmark --duration 120 --type development
```

### Example CLI Commands

Here are complete examples of using the CLI:

#### Example 1: Basic Learning Simulation

```bash
# Run a simple learning simulation
nlm experiment --type plasticity --steps 1000 --neurons 1000

# With verbose output for monitoring
nlm experiment --type plasticity --steps 1000 --verbose debug
```

#### Example 2: Development Research

```bash
# Run a developmental simulation
nlm run --type development --steps 5000 --neurons 800

# Save results for later analysis
nlm run --type development --steps 5000 --output developmental_results.json

# Analyze the results
nlm analyze --format json --output developmental_analysis.json
```

#### Example 3: Batch Processing with Python Scripts

For more complex workflows, use the Python scripts included:

```bash
# Run batch of simulations
python scripts/batch_run.py --presets basic plasticity --neurons 500 1000 --steps 100 500

# Run parameter sweep
python scripts/parameter_sweep.py --param neurons=500,1000 --param steps=100,500

# Run experiment suite
python scripts/experiment_runner.py --type plasticity,development --neurons 1000 1500
```

### CLI Command Reference

| Command | Description | Example |
|---------|-------------|---------|
| `nlm --help` | Show help information | `nlm --help` |
| `nlm --list-presets` | List available presets | `nlm --list-presets` |
| `nlm help <command>` | Get help for specific command | `nlm help run` |
| `nlm configure --show` | Show current configuration | `nlm configure --show` |
| `nlm configure --save <file>` | Save configuration to file | `nlm configure --save config.json` |
| `nlm run --type <type> --steps <n>` | Run simulation | `nlm run --type basic --steps 100` |
| `nlm experiment --type <type> --steps <n>` | Run experiment | `nlm experiment --type plasticity --steps 1000` |
| `nlm analyze --format <format> --output <file>` | Analyze results | `nlm analyze --format json --output results.json` |
| `nlm benchmark --duration <sec>` | Performance benchmarking | `nlm benchmark --duration 60` |

## Quick Reference

| What you want | Code | CLI Equivalent |
|--------------|------|----------------|
| Create brain | `pynlm.createBrain(config)` | `nlm run --type basic --steps 100` |
| Start brain | `brain.initialize()` | `nlm run --type basic --steps 100` |
| Make brain think | `brain.step(step)` | `nlm run --type basic --steps 100` |
| Create world | `pynlm.createSimpleWorld()` | Not directly available in CLI |
| Create agent | `pynlm.createAgentBrain(brain)` | Not directly available in CLI |
| Get brain stats | `brain.getFiringNeuronCount()` | `nlm analyze --format text` |
| See world | `world.getSensoryPercept()` | `nlm analyze --format text` |
| Make action | `agent.decodeMotorCommand()` | `nlm analyze --format text` |

---

## Troubleshooting

### CLI Issues

**Issue: "Command not found: nlm"**
```
Solution: Ensure NLM is installed and in your PATH
```

**Issue: Unknown command**
```
Solution: Check available commands with 'nlm --help'
```

**Issue: Invalid parameter**
```
Solution: Check parameter names and values with 'nlm help run'
```

**Issue: Simulation takes too long**
```
Solution: Use smaller step counts or different presets
```

**Issue: Memory usage too high**
```
Solution: Use fewer neurons or increase system memory
```

### Python API Issues

**Issue: "ModuleNotFoundError: No module named 'pynlm'"**
```
Solution: Install NLM with 'pip install .' or 'pip install -e .'
```

**Issue: "ImportError: cannot import name 'createDefaultConfig'"**
```
Solution: Check NLM version and installation
```

**Issue: "Brain initialization failed"**
```
Solution: Check configuration file format and required parameters
```

### Comparing CLI vs Python API

| Feature | CLI | Python API |
|---------|-----|------------|
| **Ease of use for simple tasks** | ✅ Easy with command-line options | ✅ Requires writing code |
| **Batch processing** | ✅ Python scripts included | ✅ Python scripts included |
| **Scripting and automation** | ✅ Python scripts included | ✅ Python scripts included |
| **Complex workflows** | ⚠️ Requires understanding of CLI | ✅ Flexible and expressive |
| **Debugging and development** | ⚠️ Limited debugging options | ✅ Full debugging capabilities |
| **Integration with other tools** | ⚠️ Limited | ✅ Excellent integration |

## When to Use CLI vs Python API

### Use CLI when:
- You want to run simple simulations quickly
- You need to process multiple simulations with similar parameters
- You prefer command-line interfaces
- You need to create batch workflows
- You want to use preset configurations

### Use Python API when:
- You need complex, custom workflows
- You want fine-grained control over the simulation process
- You need to integrate with other Python code
- You want to create custom analysis pipelines
- You need to debug or modify simulation behavior

### Hybrid Approach (Recommended)

For complex projects, combine both approaches:

```bash
# Use CLI for batch processing
python scripts/batch_run.py --presets basic plasticity --neurons 500 1000

# Use Python API for custom post-processing
python analyze_results.py results.json --output stats.json
```

## Next Steps

1. **Get started with CLI**: Try `nlm run --type basic --steps 100`
2. **Explore presets**: Use `nlm --list-presets` to see all available presets
3. **Create custom configurations**: Use `nlm configure --save custom.json`
4. **Advanced automation**: Use the Python scripts in `scripts/` directory
5. **Experiment design**: Use `nlm experiment --type plasticity --steps 1000`
6. **Performance testing**: Use `nlm benchmark --duration 60`

That's it! You're now ready to use NLM both through the command line and Python API. The CLI provides an excellent starting point for simple simulations, while the Python API offers full flexibility for complex workflows.

Enjoy experimenting with neural network simulations using NLM!

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
