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

## Advanced Features (Added to Python Bindings)

The following advanced features have been added to enhance the usability and functionality of the NLM Python API:

### 1. Configuration Presets

Quick access to pre-tuned configurations for different use cases:

```python
import pynlm

# Create brains with different configurations
small_config = pynlm.createConfigurationPreset("small")   # For testing (100 neurons)
medium_config = pynlm.createConfigurationPreset("medium") # For standard experiments (1000 neurons)
large_config = pynlm.createConfigurationPreset("large")   # For complex simulations (5000 neurons)
```

### 2. Enhanced Validation and Error Handling

New functions with improved validation and error checking:

```python
# Create and initialize brain with validation
brain = pynlm.createBrainWithValidation(config)

# Create complete setup with validation
setup = pynlm.createCompleteSetupWithValidation()

# Run simulation with comprehensive validation and error checking
pynlm.runCompleteSimulationWithValidation(brain, world, agent, 1000)
```

### 3. Performance Profiling

Detailed performance monitoring during simulations:

```python
# Run simulation with performance profiling and monitoring
spikes = pynlm.createPerformanceProfile(
    brain, world, agent, 5000, enableProfiling=True
)
print(f"Simulation generated {spikes} total spikes")
```

### 4. Custom Experiments

Experiment tracking with metadata collection:

```python
# Create a custom experiment with metadata
results = pynlm.createCustomExperiment(
    "Curiosity Development Test",
    brain, world, agent, 3000
)

print(f"Experiment: {results['name']}")
print(f"Duration: {results['duration']:.2f}s")
print(f"Final spike count: {results['final_spike_count']}")
```

### 5. Batch Experiment Running

Run multiple experiments with consistent error handling:

```python
# Batch run multiple experiments
experiment_names = ["Test A", "Test B", "Test C"]
results = pynlm.batchRunExperiments(
    experiment_names,
    [brain1, brain2, brain3],
    [world1, world2, world3],
    [agent1, agent2, agent3],
    1000
)

for result in results:
    print(f"{result['name']}: {'SUCCESS' if result['success'] else 'FAILED'} in {result['duration']:.2f}s")
```

### 6. Quick Test Utilities

Specialized functions for rapid testing and development:

```python
# Create a small test brain
brain = pynlm.createQuickTestBrain()

# Create a test world
world = pynlm.createWorldForTesting(10)

# Get comprehensive brain information
info = pynlm.getBrainInfo(brain)
for key, value in info.items():
    print(f"{key}: {value}")
```

### 7. Enhanced Convenience Functions

```python
# Create and initialize agent with world (NEW!)
agent = pynlm.createAgent(brain, world)

# Run single simulation step with all components
pynlm.runSimulationStep(brain, world, agent, step_number)
```

---

## Comparison of Configuration Presets

| Preset | Neuron Count | Use Case | Typical Applications |
|--------|-------------|----------|---------------------|
| `small` | 100 | Quick testing | Unit tests, debugging, rapid prototyping |
| `medium` | 1000 | Standard experiments | Basic learning, behavior testing |
| `large` | 5000 | Complex simulations | Advanced cognitive tasks, research experiments |

### Configuration Details

**Small Preset:**
- Fast execution, minimal resources
- Suitable for testing and development
- Includes basic STDP and Hebbian plasticity
- Development disabled for faster learning

**Medium Preset:**
- Balanced performance and complexity
- Includes all plasticity mechanisms
- Development enabled with standard parameters
- Memory pool and event queue sizes optimized for typical use

**Large Preset:**
- High performance and complexity
- Extended critical period for enhanced learning
- Larger memory pools and event queues
- Advanced maturation parameters for complex behaviors

---

## Performance Monitoring

The performance profiling feature provides detailed metrics including:

- Total simulation time
- Steps per second (performance)
- Maximum time per step (bottleneck identification)
- Total spikes generated
- Average spikes per second
- Memory usage patterns

Example output:
```
=== Performance Profile ===
Total simulation time: 2.5s
Steps per second: 200.0
Max time per step: 0.012s
Total spikes generated: 1500
Average spikes per second: 600.0
```

---

## Error Handling Improvements

Enhanced error handling includes:

1. **Input validation**: Null checks for all required components
2. **Parameter validation**: Range checking for step counts and other parameters
3. **Exception handling**: Detailed error messages with context
4. **Graceful degradation**: Partial failures in batch processing

Error handling examples:
```python
# Invalid input - descriptive error message
try:
    pynlm.runCompleteSimulationWithValidation(None, world, agent, 1000)
except std::invalid_argument as e:
    print(f"Error: {e}")  # "Brain cannot be null"

# Invalid parameter - range checking
try:
    pynlm.runCompleteSimulationWithValidation(brain, world, agent, 10000000)
except std::invalid_argument as e:
    print(f"Error: {e}")  # "Number of steps too large (max 1,000,000)"
```

---

## Advanced Experiment Features

### Custom Experiment Metadata

Each custom experiment collects and stores metadata:

- **Name**: Experiment identifier
- **Start time**: When experiment began
- **Duration**: Total execution time
- **Final spike count**: Total neural activity
- **Final firing neurons**: Active neurons at end
- **Final developmental stage**: Brain maturity level

### Batch Processing Features

Batch experiment processing includes:

- **Individual experiment isolation**: One experiment failure doesn't affect others
- **Error reporting**: Detailed success/failure status for each experiment
- **Performance tracking**: Individual timing for each experiment
- **Result aggregation**: Summary statistics across all experiments

Example use cases:
- Parameter sweeps
- A/B testing
- Parallel experiment execution
- Performance benchmarking

---

## Integration with Existing Features

All advanced features integrate seamlessly with existing NLM functionality:

1. **Backward compatibility**: All existing code continues to work unchanged
2. **Enhanced defaults**: New factory functions use sensible defaults
3. **Optional features**: Advanced features are available when needed
4. **Consistent API**: All new functions follow NLM naming conventions

Example of gradual adoption:
```python
# Original code (still works)
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enhanced code (optional improvements)
import pynlm
brain = pynlm.createBrainWithValidation(pynlm.createConfigurationPreset("medium"))
```

---

## Migration Guide

### From Basic to Advanced Usage

**Step 1**: Replace basic factory functions with enhanced versions
```python
# Before
brain = pynlm.createBrain(config)
brain.initialize()

# After
brain = pynlm.createBrainWithValidation(config)
```

**Step 2**: Use configuration presets for common scenarios
```python
# Before
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# After
config = pynlm.createConfigurationPreset("medium")
```

**Step 3**: Add performance monitoring for production use
```python
# Before
for step in range(1000):
    brain.step(step)

# After
spikes = pynlm.createPerformanceProfile(brain, world, agent, 1000)
```

### Troubleshooting Advanced Features

**Common Issues and Solutions:**

1. **Configuration errors**: Ensure config presets are used correctly
2. **Performance issues**: Use smaller presets for testing, larger for production
3. **Validation errors**: Check that all required components are provided
4. **Batch processing failures**: Verify that all experiment vectors have matching lengths

**Error Codes:**
- Invalid argument: "All components must be provided", "Number of steps must be positive"
- Configuration errors: "Unknown preset name", "Experiment vectors must have same length"
- Runtime errors: Detailed step-by-step error messages during simulation

---

## Performance Recommendations

### For Development and Testing

1. **Use small presets**: Quick iteration with minimal resources
2. **Enable profiling**: Identify bottlenecks in custom experiments
3. **Batch testing**: Run multiple configurations efficiently
4. **Memory monitoring**: Track memory usage during long simulations

### For Production Use

1. **Use medium or large presets**: Balance performance and capability
2. **Disable profiling**: Remove overhead in production runs
3. **Monitor performance**: Use `createPerformanceProfile()` periodically
4. **Error handling**: Implement comprehensive error catching

### For Research and Advanced Use

1. **Use large presets**: Maximum capability for complex experiments
2. **Custom experiments**: Collect detailed metadata for analysis
3. **Batch processing**: Run parameter sweeps and systematic studies
4. **Performance optimization**: Profile and optimize critical paths

---

## Version Information

**NLM Python API Version**: 0.1.0
**Phase**: 6 (Final Integration)
**Added**: Advanced features to Python bindings
**Status**: Production ready

---

All advanced features are backward compatible and can be adopted gradually as needed.
