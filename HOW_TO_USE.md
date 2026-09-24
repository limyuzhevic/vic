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

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

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

## Part 4: Advanced Usage and Best Practices

### Common Error Handling Patterns

#### 1. Initialization Errors
```python
import pynlm

try:
    config = pynlm.createDefaultConfig()
    # Configure parameters with validation
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.2)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Brain initialized successfully")
except Exception as e:
    print(f"Initialization failed: {e}")
    # Common issues: invalid parameter values, memory allocation errors
```

#### 2. Simulation Runtime Errors
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Always call initialize before using the brain
brain.initialize()

# Run simulation with error handling
for step in range(1000):
    try:
        brain.step(step)
        
        # Check for simulation issues
        firing_count = brain.getFiringNeuronCount()
        if firing_count > brain.getTotalNeuronCount() * 0.9:
            print(f"Warning: High firing rate at step {step}: {firing_count} firing neurons")
        
        # Log statistics periodically
        if step % 100 == 0:
            print(f"Step {step}: {firing_count} firing neurons, "
                  f"rate: {brain.getAverageFiringRate():.2f} Hz")
    except RuntimeError as e:
        print(f"Simulation error at step {step}: {e}")
        # Typical errors: invalid timestep, neuron limit exceeded
        break
    except MemoryError as e:
        print(f"Memory error at step {step}: {e}")
        # Typical errors: insufficient memory for large networks
        break
```

### Configuration Examples

#### 1. Custom Neuron Parameters
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Customize neuron dynamics for your use case
config.set("brain.v_thresh", -55.0)      # Raise threshold for slower firing
config.set("brain.v_rest", -65.0)       # Change resting potential  
config.set("brain.tau_mem", 25.0)       # Slower membrane time constant
config.set("brain.tau_ref", 3.0)        # Longer refractory period

# Configure plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.hebbian.enable", True)

# Configure structural plasticity
config.set("synaptogenesis_rate", 0.0001)
config.set("pruning_rate", 0.00005)

brain = pynlm.createBrain(config)
brain.initialize()
```

#### 2. Large-Scale Network Configuration
```python
import pynlm

# Create configuration for large-scale simulation
config = pynlm.createDefaultConfig()

# Scale up for research
config.set("brain.neuron_count", 5000)           # Large network
config.set("brain.region_count", 5)              # Multiple regions
config.set("brain.synapse_density", 0.05)        # Sparse connections
config.set("simulation.timestep", 0.0005)        # Fine time resolution

# Enable advanced features
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("memory.working_memory.capacity", 10.0)
config.set("memory.episodic_memory.capacity", 60.0)

brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created large-scale brain:")
print(f"  - {brain.getTotalNeuronCount()} neurons")
print(f"  - {brain.getTotalSynapseCount()} synapses")
print(f"  - {brain.getTotalRegionCount()} regions")
```

#### 3. Performance Optimization Configuration
```python
import pynlm

# Configuration optimized for performance
config = pynlm.createDefaultConfig()

# Balance between biological accuracy and performance
config.set("brain.neuron_count", 1000)          # Reasonable size
config.set("brain.synapse_density", 0.1)         # Moderate connectivity
config.set("simulation.timestep", 0.001)        # Standard timestep
config.set("random_seed", 42)                   # Reproducible results

# Enable only essential plasticity for speed
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", False)   # Disable for speed

# Disable memory-intensive features
config.set("neuromod.curiosity.enable", False)
config.set("neuromod.novelty.enable", False)
config.set("memory.working_memory.capacity", 0.0)  # Disable

brain = pynlm.createBrain(config)
brain.initialize()

print("Performance-optimized brain created")
```

### Debugging and Troubleshooting

#### 1. Common Issues and Solutions

**Issue**: Brain doesn't learn during plasticity experiments
```python
# Common cause: plasticity not enabled
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
```

**Issue**: Excessive firing causing instability
```python
# Solutions:
config.set("brain.v_thresh", -50.0)       # Raise threshold
config.set("brain.tau_mem", 30.0)         # Slower membrane dynamics
config.set("brain.tau_ref", 5.0)          # Longer refractory period
```

**Issue**: Simulation too slow
```python
# Optimize performance:
config.set("brain.neuron_count", 500)      # Smaller network
config.set("brain.synapse_density", 0.05)  # Fewer connections
config.set("simulation.timestep", 0.002)   # Larger timestep
config.set("random_seed", 123)             # Pre-compute if deterministic
```

#### 2. Error Code Reference

| Error Type | Description | Solution |
|------------|-------------|----------|
| `RuntimeError` | Invalid timestep or invalid state | Check timestep configuration and call initialize() |
| `MemoryError` | Insufficient memory for network size | Reduce neuron count or disable memory features |
| `std::exception` | Configuration error | Validate parameter ranges using getParameterInfo() |

### Best Practices

#### 1. Configuration Management
```python
# Always validate configuration before use
import pynlm

config = pynlm.createDefaultConfig()

# Set multiple parameters with validation
try:
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    # Check parameter bounds
    for param in ["brain.neuron_count", "brain.synapse_density"]:
        info = config.getParameterInfo(param)
        print(f"{param}: {info.description}")
except Exception as e:
    print(f"Configuration error: {e}")
```

#### 2. Resource Management
```python
import pynlm
import gc
import psutil

# Monitor memory usage
process = psutil.Process()
config = pynlm.createDefaultConfig()

for epoch in range(5):
    # Create and use brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Clean up
    del brain
    gc.collect()
    
    # Check memory
    mem_mb = process.memory_info().rss / 1024 / 1024
    print(f"Epoch {epoch + 1}: Memory usage: {mem_mb:.1f} MB")
```

#### 3. Testing and Validation
```python
import pynlm

# Test configuration with predefined values
def test_configuration():
    config = pynlm.createDefaultConfig()
    
    # Test with known valid values
    test_params = {
        "brain.neuron_count": 100,
        "brain.synapse_density": 0.1,
        "plasticity.stdp.learning_rate": 0.001,
        "simulation.timestep": 0.001,
        "random_seed": 42
    }
    
    for param, value in test_params.items():
        try:
            config.set(param, value)
            retrieved = config.get(type(value).__name__, param)
            if retrieved == value:
                print(f"✓ {param}: {value} - OK")
            else:
                print(f"✗ {param}: Expected {value}, got {retrieved}")
        except Exception as e:
            print(f"✗ {param}: {e}")

test_configuration()
```

### Advanced Features

#### 1. Batch Configuration
```python
import pynlm

config = pynlm.createDefaultConfig()

# Set multiple parameters at once
batch_params = [
    ("brain.neuron_count", 1000),
    ("brain.synapse_density", 0.1),
    ("plasticity.stdp.learning_rate", 0.001),
    ("plasticity.hebbian.enable", True),
    ("simulation.timestep", 0.001)
]

config.setBatch(batch_params)

# Verify all parameters were set
for param in ["brain.neuron_count", "brain.synapse_density", "plasticity.stdp.learning_rate"]:
    value = config.getOr(param, 0)
    print(f"{param}: {value}")
```

#### 2. Configuration Validation
```python
import pynlm

config = pynlm.createDefaultConfig()

# Test parameter validation
error_msg = ""
if not config.validateParameter("brain.neuron_count", 1000, error_msg):
    print(f"Validation failed: {error_msg}")
else:
    print("Parameter validation passed")

# List all valid parameters
print("\nValid configuration parameters:")
for param in config.getParameterInfo(param).name for param in config.getKeys()]:
    info = config.getParameterInfo(param)
    print(f"  {param}: {info.type} - {info.description}")
```

### Migration Guide

#### From Phase 1 to Phase 2

If you're migrating from NLM Phase 1:

1. **Configuration Changes**: 
   - Old: `config.set("synapse_density", 0.1)`
   - New: `config.set("brain.synapse_density", 0.1)`

2. **Brain Initialization**:
   - Old: `brain.initialize()`
   - New: Same, but more robust error handling

3. **Parameter Names**:
   - Changed to dot notation for clarity
   - Added parameter metadata for validation

### Troubleshooting Common Issues

#### Build Issues
```bash
# CMake cannot find Python
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)

# Compilation errors
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present
```

#### Runtime Issues
```python
# ImportError: No module named 'pynlm'
pip install --force-reinstall .

# Segmentation faults
- Check that initialize() is called before step()
- Ensure reset() is called before re-running simulation
- Validate parameter ranges before creating brain
```

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
