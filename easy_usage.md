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

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

## New Features in NLM

This version of NLM includes several new improvements:

### 1. Enhanced Configuration Support

NLM now supports multiple configuration file formats:

- **JSON** (`brain_config.json`): Modern, structured configuration
- **YAML** (`brain_config.yaml`): Human-readable configuration
- **Plain text** (`configs/default.cfg`): Simple key=value format

#### Configuration Examples:

**JSON Configuration:**
```json
{
    "nlm": {
        "version": "0.1.0",
        "parameters": {
            "neuron_count": 1000,
            "connection_probability": 0.15,
            "plasticity_enabled": true,
            "stdp_ltp_weight": 0.02
        }
    }
}
```

**YAML Configuration:**
```yaml
# NLM Configuration File
nlm:
  version: "0.1.0"
  parameters:
    neuron_count: 1000
    connection_probability: 0.15
    plasticity_enabled: true
    stdp_ltp_weight: 0.02
```

**Plain Text Configuration:**
```
# NLM Configuration File
neuron_count = 1000
connection_probability = 0.15
plasticity_enabled = true
stdp_ltp_weight = 0.02
```

### 2. Advanced Command-Line Interface

NLM now has a comprehensive command-line interface with:

- **Help and Version Options:**
  ```bash
  nlm --help           # Show detailed help
  nlm --version        # Show version information
  ```

- **Configuration Options:**
  ```bash
  nlm --config brain_config.json    # Load JSON configuration
  nlm -c my_config.yaml              # Load YAML configuration
  nlm --config my_settings.cfg       # Load plain text configuration
  ```

- **Performance Tuning:**
  ```bash
  nlm --neuron-count 2000           # Override neuron count
  nlm --num-steps 100                # Override simulation steps
  nlm --random-seed 12345            # Override random seed
  ```

- **Test Mode Selection:**
  ```bash
  nlm --plasticity-only            # Run plasticity experiment only
  nlm --stdp-only                  # Run STDP verification only
  nlm --connectivity-only          # Run connectivity test only
  ```

- **Logging and Output Control:**
  ```bash
  nlm --log-level Info              # Set logging level
  nlm --output-file nlm.log         # Write logs to file
  nlm --validate-config             # Validate configuration file
  ```

### 3. Advanced Tools Script

NLM includes a comprehensive tools script (`scripts/nlm-tools.sh`) that provides:

#### Command Categories:

**Basic Operations:**
```bash
nlm-tools run --help              # Show help for run command
nlm-tools config create json       # Create JSON configuration
nlm-tools config validate file.json # Validate configuration
```

**Analysis and Export:**
```bash
nlm-tools analysis results.csv    # Analyze simulation results
nlm-tools stats                    # Show system statistics
nlm-tools export results/          # Export simulation results
```

**Configuration Management:**
```bash
nlm-tools config list             # List all configuration keys
nlm-tools config get neuron_count # Get configuration value
nlm-tools config set neuron_count 2000 # Set configuration value
```

### 4. Real Neural Dynamics

NLM Phase 2 now includes real integrate-and-fire (LIF) neuron dynamics:

- **Biologically realistic neuron behavior**
- **Spike-frequency adaptation**
- **Refractory period handling**
- **Plasticity-based learning**

### 5. Utility Functions

NLM provides extensive utility functions for:

- **File operations**: Path handling, file existence checks, directory creation
- **Configuration utilities**: Validation, template generation, value formatting
- **Simulation utilities**: Firing rate calculation, data analysis, statistical functions
- **Progress monitoring**: Progress bars, timing, metrics collection

### 6. Error Handling and Recovery

Improved error handling with:

- **Comprehensive error codes:** 0-7 covering different failure scenarios
- **Recovery mechanisms:** Automatic recovery where possible
- **Detailed error messages:** Context-specific error reporting
- **Validation:** Configuration file validation before execution

### 7. Enhanced Documentation

- **Comprehensive examples**: Multiple usage scenarios
- **Quick reference guide**: Essential commands at a glance
- **Troubleshooting section**: Common issues and solutions
- **Architecture documentation**: Understanding NLM's design

## Installation and Setup

### Building NLM

```bash
# Build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running NLM

```bash
# Basic usage
./nlm

# With configuration
./nlm --config brain_config.json

# With custom options
./nlm --neuron-count 2000 --log-level Info --plasticity-only

# Using tools script
./scripts/nlm-tools.sh run --help
./scripts/nlm-tools.sh config create json my_config.json
./scripts/nlm-tools.sh validate my_config.json
```

### Running Tests

```bash
# Run unit tests
./nlm_benchmark

# Run demonstration modes
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo
```

## Configuration Examples

### Basic Configuration (JSON)
```json
{
    "nlm": {
        "parameters": {
            "neuron_count": 500,
            "region_count": 1,
            "simulation_timestep": 0.001,
            "plasticity_enabled": true,
            "stdp_ltp_weight": 0.02
        }
    }
}
```

### Advanced Configuration (YAML)
```yaml
nlm:
  version: "0.1.0"
  parameters:
    neuron_count: 1000
    connection_probability: 0.15
    plasticity_learning_rate: 0.01
    stdp_ltp_weight: 0.02
    stdp_ltd_weight: 0.015
    synaptogenesis_rate: 0.0001
    pruning_rate:  grávidade
```

## Getting Started

1. **Install NLM** using the build instructions above
2. **Run basic demo**: `./nlm` 
3. **Explore examples** in the `examples/` directory
4. **Try tools script**: `./scripts/nlm-tools.sh`
5. **Read documentation** in `docs/`
6. **Experiment with configurations** to discover NLM's capabilities

## Troubleshooting

### Common Issues:

**"My brain isn't doing anything"**
- Ensure you call `brain.initialize()` before simulation
- Try increasing the number of simulation steps
- Check that neurons have proper connectivity

**"Configuration file not found"**
- Ensure the file path is correct
- Verify file permissions
- Try using absolute paths

**"Logging level not recognized"**
- Use valid logging levels: Debug, Info, Warning, Error, Critical
- Check for typos in the log-level argument

**"Neuron count too high"**
- Try reducing the neuron count for faster testing
- Monitor system resources during simulation

## Next Steps

- **Join the NLM community** on GitHub
- **Provide feedback** on the NLM website
- **Contribute to development** by submitting pull requests
- **Report bugs** and feature requests
- **Explore Phase 6** integration for advanced functionality

That's it! You're now ready to use NLM with all its new improvements and features.

### Version Information

NLM Phase 2 v0.1.0
- **Phase**: 2: Real Neural Computation
- **License**: MIT
- **Date**: $(date +%Y-%m-%d)
- **Documentation**: https://nlm.readthedocs.io

