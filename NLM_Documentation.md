# NLM (Neural Learning Machine) - Enhanced Documentation

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that creates virtual brains with neurons that can learn, remember, and make decisions. The system implements a brain-inspired architecture with real spiking neural computation.

## Project Structure

The NLM project consists of several key components:

1. **Core C++ Library**: Implements the neural simulation engine with real LIF neuron dynamics
2. **Python Bindings**: Provides easy access to NLM functionality via Python
3. **Command Line Interface**: Provides command-line tools for running simulations
4. **Benchmarking Tools**: Performance analysis and optimization tools
5. **Documentation**: Comprehensive guides and examples

## Installation

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install the Python package
pip install .

# Or install in development mode
pip install -e .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Building the C++ Components

For users who want to build and use the C++ components:

```bash
# Navigate to the project directory
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run tests (if built)
ctest --output-on-failure
```

## Basic Usage (Python API)

### Quick Start

```python
import pynlm

# Create a default brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Brain in a Simple World

```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
```

## Advanced Usage

### Configuration Management

```python
import pynlm
import json

# Create configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("my_config.json")

# Create brain with custom configuration
brain = pynlm.createBrain(loaded_config)
```

### Using Memory Systems

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory capacity: {working_memory.getCapacity()} traces")

episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")

# Access prediction system
prediction_system = brain.getPredictionSystem()
if prediction_system:
    print("Prediction system active")
```

### Neuromodulation and Learning

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run learning episode
for step in range(1000):
    # ... simulation code ...
    
    # Apply reward-based learning
    reward = 0.1 if task_completed else -0.1
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
```

## Command Line Interface (CLI)

The NLM project includes a command-line interface for basic operations:

### Installation

```bash
# Make the CLI executable
chmod +x nlm_cli.py

# Run with Python
python nlm_cli.py

# Or create a wrapper script
#!/usr/bin/env python3
exec(open('nlm_cli.py').read())
chmod +x nlm_cli_wrapper
ln -sf $(pwd)/nlm_cli_wrapper /usr/local/bin/nlm
```

### Basic Commands

```bash
# Create and run a simple brain
nlm create --neurons 1000 --steps 100

# Save brain state to file
nlm save my_brain.bin

# Load brain state from file
nlm load my_brain.bin

# Inspect brain state
nlm inspect

# Show version information
nlm version
```

### Advanced CLI Usage

```bash
# Run simulation with verbose output
nlm run --steps 1000 --verbose 100

# Create brain with custom parameters
nlm create --neurons 5000 --regions 4 --variance 0.15
```

## Benchmarking and Performance Tools

### Installation

```bash
pip install psutil memory-profiler tqdm
```

### Running Benchmarks

```bash
# Quick benchmark with common configurations
python nlm_benchmark.py

# Generate detailed report
python nlm_benchmark.py --report benchmark_report.txt

# Use custom configurations
custom_config.py
```

### Benchmark Reports

Benchmarks generate detailed reports with:
- Performance metrics (steps/sec, spikes/sec)
- Configuration comparisons
- Resource usage analysis
- Efficiency calculations

## Python API Reference

### Core Classes

#### Brain

| Method | Description |
|--------|-------------|
| `createBrain(config)` | Create a new brain with configuration |
| `initialize()` | Initialize the brain |
| `step(step, time=None)` | Perform a simulation step |
| `receiveSensoryInput(input)` | Inject sensory input |
| `produceAction()` | Generate motor action |
| `reset()` | Reset brain state |
| `save(filepath)` | Save brain state to file |
| `load(filepath)` | Load brain state from file |
| `getTotalNeuronCount()` | Get total neuron count |
| `getTotalSynapseCount()` | Get total synapse count |
| `getFiringNeuronCount()` | Get currently firing neurons |
| `getTotalSpikeCount()` | Get total spike count |

#### Config

| Method | Description |
|--------|-------------|
| `createDefaultConfig()` | Create default configuration |
| `loadFromFile(filepath)` | Load from JSON file |
| `saveToFile(filepath)` | Save to JSON file |
| `set(key, value)` | Set configuration value |
| `get(key)` | Get configuration value |
| `has(key)` | Check if key exists |
| `getKeys()` | Get all configuration keys |

#### AgentBrain

| Method | Description |
|--------|-------------|
| `createAgentBrain(brain)` | Create agent brain interface |
| `initialize(world)` | Initialize with world |
| `processSensoryInput(percept)` | Process sensory input |
| `decodeMotorCommand()` | Decode motor command |
| `applyRewardModulation(reward, predicted)` | Apply reward learning |
| `updateDevelopment(timestep)` | Update development |

#### SimpleWorld

| Method | Description |
|--------|-------------|
| `createSimpleWorld()` | Create simple world |
| `configure(width, height, visionWidth, visionHeight)` | Configure world |
| `reset()` | Reset world state |
| `update(timestep)` | Update world |
| `applyMotorCommand(cmd, time)` | Apply motor command |
| `getSensoryPercept()` | Get sensory percept |
| `getAgentBody()` | Get agent body state |

### Error Handling

The NLM Python bindings provide comprehensive error handling with custom exceptions:

```python
import pynlm
from pynlm import NLMError, NLMInitializationError, NLMInvalidArgumentError

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except NLMInitializationError as e:
    print(f"Brain initialization failed: {e}")
except NLMInvalidArgumentError as e:
    print(f"Invalid argument: {e}")
except NLMError as e:
    print(f"NLM error: {e}")
```

### Configuration Parameters

#### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.region_count` | int | 1 | Number of brain regions |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential |
| `brain.v_rest` | float | -70.0 | Resting potential |
| `brain.v_reset` | float | -75.0 | Reset potential after spike |
| `brain.tau_mem` | float | 20.0 | Membrane time constant |
| `brain.tau_ref` | float | 2.0 | Refractory period |

#### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (positive) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (negative) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Advanced Features

### 1. Memory System Integration

NLM provides integrated memory systems:

- **Working Memory**: Transient active information
- **Episodic Memory**: Experience storage and replay
- **Associative Memory**: Pattern associations
- **Procedural Memory**: Skill learning

### 2. Prediction and Planning

The brain includes prediction systems for:
- Sensory prediction
- Action planning
- Decision making

### 3. Neuromodulation

Multiple neuromodulators control behavior:
- **Dopamine**: Reward and reinforcement
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **Prediction Error**: Learning signal

### 4. Development

The brain undergoes developmental stages:
- **Initial**: High plasticity
- **Critical Period**: Rapid learning
- **Maturation**: Stabilizing learning
- **Adult**: Stable performance
- **Aging**: Declining plasticity

## Examples and Tutorials

### Mini Project 1: Silent Brain

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Mini Project 2: Brain Watching a World

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

### Mini Project 3: Complete Agent

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

### Runtime Issues

**Brain isn't doing anything:**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**The agent isn't moving:**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**Everything is 0:**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

## License

MIT License. See LICENSE file for details.

## Authors

Research project — See docs for scientific background.
