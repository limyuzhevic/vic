# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

### Running the C++ Interface

#### Overview

The C++ interface is the foundation of the NLM system and provides direct access to the neural simulation engine. It includes:

- **Main executable (`nlm`)**: Core neural simulation engine
- **Phase 3/4 demos**: Advanced demonstration of specific NLM phases
- **Unit tests (`nlm_test`)**: Comprehensive test suite

#### Running the Main Simulation

The main executable (`./nlm`) provides:

- **Simple usage**: Run with default configuration
- **Configuration file support**: Load from JSON or key=value format
- **Command-line parameter overrides**: Override configuration values
- **Help system**: Comprehensive help documentation
- **Error handling**: Detailed error reporting and validation

```bash
# Run with default configuration
./nlm

# Show help message
./nlm --help
./nlm -h

# Load custom configuration file
./nlm --config my_config.json
./nlm --config my_config.cfg

# Override parameters from command line
./nlm --neuron_count 2000 --region_count 3

# Combine file and parameter overrides
./nlm --config my_config.json --simulation_timestep 0.0001
```

#### Configuration File Support

The C++ interface supports two configuration file formats:

##### JSON Configuration

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        }
    },
    "logging": {
        "level": "INFO",
        "to_file": false,
        "filename": "nlm.log"
    }
}
```

##### Key=Value Format (Legacy)

```
# Configuration comments supported
brain.neuron_count = 1000
brain.region_count = 1
brain.connection_probability = 0.1
plasticity.stdp.enable = true
logging.level = INFO
```

#### Configuration File Examples

**Example 1: Fast test configuration**

```bash
mkdir -p configs
echo '
brain.neuron_count = 100
brain.region_count = 1
brain.connection_probability = 0.05
plasticity.stdp.learning_rate = 0.01
logging.level = INFO
' > configs/fast_test.cfg
./nlm --config configs/fast_test.cfg
```

**Example 2: JSON experiment configuration**

```bash
mkdir -p configs
echo '{
    "brain": {
        "neuron_count": 500,
        "region_count": 2,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.005
        }
    },
    "logging": {
        "level": "WARNING",
        "to_file": true
    }
}' > configs/experiment.json
./nlm --config configs/experiment.json
```

#### Available Command Line Options

```bash
--help, -h                    Display help message and exit
--config, --config=FILE       Load configuration from file (JSON or key=value format)
--key=VALUE, --key VALUE      Override configuration parameter
--random_seed VALUE            Random seed (integer)
--simulation_timestep VALUE    Simulation timestep in seconds
--neuron_count VALUE           Number of neurons
--region_count VALUE           Number of regions
--connection_probability VALUE Connection probability
--stdp_ltp_weight VALUE        STDP LTP learning rate
--stdp_ltd_weight VALUE        STDP LTD learning rate
--stdp_tau VALUE               STDP time constant
--synaptogenesis_rate VALUE    Synaptogenesis rate
--pruning_rate VALUE           Pruning rate
--log_level LEVEL             Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
```

#### Parameter Validation

The CLI validates parameter types and ranges:

```bash
# Valid parameters
./nlm --neuron_count 1000 --region_count 3

# Invalid parameters (will show warning)
./nlm --neuron_count 0      # Warning: neuron_count must be > 0
./nlm --simulation_timestep 10  # Warning: timestep out of reasonable range
```

#### Error Handling

The C++ interface provides comprehensive error reporting:

```bash
# File not found
./nlm --config nonexistent.cfg
# Error: Failed to load configuration from nonexistent.cfg

# Invalid JSON
./nlm --config malformed.json
# Error: JSON parsing error in malformed.json

# Invalid argument format
./nlm --neuron_count "not_a_number"
# Warning: Invalid neuron_count value: not_a_number. Must be an integer.
```

#### Running Phase 3 and Phase 4 Demos

##### Phase 3 Demo (`./nlm_phase3_demo`)

Phase 3 demonstrates:

- **Neural prediction system**: Implementation of predictive coding
- **Working memory**: Neural working memory with maintenance and competition
- **Episodic-like memory**: Experience encoding and retrieval
- **Concept formation**: Pattern discovery from neural activity
- **Predictive planning**: Action sequence evaluation

```bash
# Run Phase 3 demo
./nlm_phase3_demo

# With custom configuration
./nlm_phase3_demo --config configs/phase3.json
```

##### Phase 4 Demo (`./nlm_phase4_demo`)

Phase 4 demonstrates:

- **Self-model**: Neural representation of self
- **Social learning**: Learning from others
- **Continual learning**: Adapting to new experiences
- **Neural attention**: Focused attention mechanisms

```bash
# Run Phase 4 demo
./nlm_phase4_demo

# With custom configuration
./nlm_phase4_demo --config configs/phase4.json
```

#### Running Tests

The test executable (`./nlm_test`) runs comprehensive unit tests:

```bash
# Run all tests
./nlm_test

# Run tests with verbose output
./nlm_test -v

# Run specific test suites
./nlm_test --config test_config
```

#### Example Configuration Files

**Minimal configuration for fast testing**

```bash
cat > config_minimal.cfg << EOF
brain.neuron_count = 100
brain.region_count = 1
brain.connection_probability = 0.05
plasticity.stdp.enable = false
plasticity.structural.enable = false
logging.level = WARNING
EOF

./nlm --config config_minimal.cfg
```

**Large network configuration**

```bash
cat > config_large.cfg << EOF
brain.neuron_count = 10000
brain.region_count = 5
brain.connection_probability = 0.15
plasticity.stdp.learning_rate = 0.001
plasticity.structural.enable = true
logging.level = INFO
EOF

./nlm --config config_large.cfg
```

#### Working with Different Configuration Formats

**Converting between formats**

```bash
# Convert key=value to JSON
#!/bin/bash

cat > keyvalue_to_json.py << 'EOF'
import json
import sys

# Read key=value file
config = {}
with open(sys.argv[1], 'r') as f:
    for line in f:
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        if '=' in line:
            key, value = line.split('=', 1)
            key = key.strip()
            value = value.strip()
            
            # Convert value to appropriate type
            if value.lower() in ['true', 'false']:
                value = value.lower() == 'true'
            elif value.isdigit():
                value = int(value)
            else:
                try:
                    value = float(value)
                except ValueError:
                    # Keep as string (remove quotes)
                    if (value.startswith('"') and value.endswith('"')) or \
                       (value.startswith("'") and value.endswith("'")):
                        value = value[1:-1]
            
            config[key] = value

# Write JSON file
with open(sys.argv[2], 'w') as f:
    json.dump(config, f, indent=4)

print(f"Converted {sys.argv[1]} to {sys.argv[2]}")
EOF

# Usage: python3 keyvalue_to_json.py config.keyvalue config.json
```

#### Scripting Examples

**Bash script for batch processing**

```bash
#!/bin/bash

# Run multiple simulations with different configurations
for seed in 42 123 456 789 999; do
    echo "Running simulation with seed $seed"
    ./nlm --random_seed $seed --neuron_count 1000
    mv checkpoint.bin checkpoint_seed$seed.bin
    echo "Saved: checkpoint_seed$seed.bin"
done
```

**Python script for experiment management**

```python
import subprocess
import time
import os

def run_experiments(config_file, param_sets):
    """Run multiple experiments with different parameters."""
    results = []
    
    for i, params in enumerate(param_sets):
        print(f"Running experiment {i+1}/{len(param_sets)}")
        
        # Build command
        cmd = ['./nlm', f'--config', config_file]
        for key, value in params.items():
            cmd.extend([f'--{key}', str(value)])
        
        # Run simulation
        start_time = time.time()
        result = subprocess.run(cmd, capture_output=True, text=True)
        elapsed = time.time() - start_time
        
        results.append({
            'experiment': i,
            'parameters': params,
            'success': result.returncode == 0,
            'stdout': result.stdout,
            'stderr': result.stderr,
            'time': elapsed
        })
        
        if result.returncode != 0:
            print(f"Experiment {i} failed: {result.stderr}")
        else:
            print(f"Experiment {i} completed in {elapsed:.2f}s")
    
    return results

# Example usage
param_sets = [
    {'neuron_count': 500, 'region_count': 1},
    {'neuron_count': 1000, 'region_count': 2},
    {'neuron_count': 2000, 'region_count': 3},
    {'neuron_count': 5000, 'region_count': 5},
]

results = run_experiments('configs/base.json', param_sets)
```

This comprehensive C++ interface documentation provides users with detailed information about building, configuring, and running NLM simulations using the native C++ interface.

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

---

## Part 3: Using the Python Library

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Creating a Simple World

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)
```

### Complete Agent Example

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
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
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

### Key Classes and Methods

#### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

---

## Part 4: Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

---

## Part 5: Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction

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

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

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

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
