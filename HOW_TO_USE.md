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

# Check if module loaded successfully
print("Available classes:", dir(pynlm))

# Create a simple brain with validation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Config summary: {config.summary()}")
```

---

## Part 3: Using the Enhanced Python Library

### Basic Usage with Enhanced Features

#### Creating a Brain

```python
import pynlm

# Create a default configuration with sensible defaults
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.connection_probability", 0.05)

# Validate configuration before using it
if pynlm.validate_config(config):
    print("Configuration is valid!")
else:
    print("Configuration has errors")

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Brain stats: {brain.get_simulation_stats()}")
```

#### Running a Simulation with Enhanced Features

```python
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.region_count", 3)
brain = pynlm.createBrain(config)
brain.initialize()

# Enhanced simulation loop with progress tracking
print("Starting simulation...")
print(f"Initial state: {brain}")

for step in range(1000):
    brain.step(step)
    
    # Enhanced logging with validation
    if step % 100 == 0:
        firing = brain.getFiringNeuronCount()
        rate = brain.getAverageFiringRate()
        total_spikes = brain.getTotalSpikeCount()
        stage = brain.getDevelopmentalStage()
        
        print(f"Step {step:4d}:")
        print(f"  Firing neurons: {firing:5d}")
        print(f"  Average firing rate: {rate:.3f} Hz")
        print(f"  Total spikes: {total_spikes:6d}")
        print(f"  Developmental stage: {stage}")
        
        # Enhanced validation
        brain.validate_state()
        
        # Log enhanced status
        brain.logStatus()

print(f"Final brain state: {brain}")
print(f"Final simulation statistics: {brain.get_simulation_stats()}")
```

#### Creating a Simple World with Enhanced Features

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions with validation
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Validate world configuration
print(f"World dimensions: {world.getWidth()}x{world.getHeight()} cells")
print(f"Vision dimensions: {world.getVisionWidth()}x{world.getVisionHeight()}")

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Set random seed for reproducible experiments
world.setRandomSeed(42)

# Add some test objects to the world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create test objects
test_resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0)
test_hazard = pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, value=5.0)

world.addObject(test_resource)
world.addObject(test_hazard)

print(f"Added {world.getObjectCount()} objects to world")

# Run world update with enhanced logging
for step in range(50):
    world.update(timestep=0.1)
    
    # Get enhanced sensory percept
    percept = world.getSensoryPercept()
    
    # Check if percept is valid
    if percept.is_valid():
        print(f"Step {step}: Valid percept with {percept.get_sensory_count()} signals")
    
    # Get agent body state
    body = world.getAgentBody()
    if body.is_alive():
        speed = body.get_speed()
        energy = body.get_energy()
        print(f"  Agent: pos({body.x:.1f}, {body.y:.1f}), speed={speed:.2f}, energy={energy:.1f}")

print(f"Final environment stats: {world.get_environment_stats()}")
```

### Complete Agent Example with Enhanced Features

```python
import pynlm
import random

def run_enhanced_agent_simulation(num_steps=1000):
    """Run a complete enhanced agent simulation with brain and world."""
    
    # 1. Create configuration with validation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 4)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable enhanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Add some world objects
    for _ in range(10):
        x = random.uniform(5, 25)
        y = random.uniform(5, 25)
        obj_type = random.choice([
            pynlm.WorldObjectType.Resource,
            pynlm.WorldObjectType.Hazard
        ])
        obj = pynlm.WorldObject(x, y, obj_type, value=random.uniform(1, 10))
        world.addObject(obj)
    
    # 7. Run simulation loop with enhanced features
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
        
        # Apply reward modulation with enhanced error handling
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        
        try:
            agent.applyRewardModulation(reward, 0.0)
        except Exception as e:
            print(f"Warning: Reward modulation failed: {e}")
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print enhanced progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            
            # Enhanced validation
            if not brain.validate_state():
                print(f"  WARNING: Brain state validation failed!")
            
            if not agent.validate_configuration():
                print(f"  WARNING: Agent configuration validation failed!")
            
            # Log enhanced status
            brain.logStatus()
            
            # Get agent statistics
            stats = agent.get_agent_stats()
            learning_metrics = agent.get_learning_metrics()
            print(f"  Agent stats: {stats}")
            print(f"  Learning metrics: {learning_metrics}")
    
    # Final analysis
    final_stats = brain.get_simulation_stats()
    final_agent_stats = agent.get_agent_stats()
    
    print(f"\n=== FINAL RESULTS ===")
    print(f"Simulation completed: {num_steps} steps")
    print(f"Final brain stats: {final_stats}")
    print(f"Final agent stats: {final_agent_stats}")
    
    # Export final state
    brain_state = pynlm.export_brain_state(brain)
    print(f"Brain state exported: {brain_state}")
    
    return brain, agent, world

# Run the enhanced simulation
brain, agent, world = run_enhanced_agent_simulation(1000)
```

### Advanced Usage Patterns

#### Pattern 1: Config Management with Validation

```python
def create_reproducible_brain(seed=42):
    """Create a reproducible brain configuration."""
    config = pynlm.createDefaultConfig()
    
    # Set deterministic values
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 2)
    config.set("brain.connection_probability", 0.05)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    
    # Validate configuration
    if not pynlm.validate_config(config):
        raise ValueError("Invalid brain configuration")
    
    return config

def run_multiple_experiments(base_config, variations):
    """Run multiple experiments with small variations."""
    results = []
    
    for i, variation in enumerate(variations):
        # Create config from base with variation
        config = pynlm.create_config_from_dict(base_config.getAllSettings())
        config.apply_variation(variation)  # Custom method
        
        # Validate
        if not pynlm.validate_config(config):
            print(f"Skipping variation {i}: invalid configuration")
            continue
        
        # Run experiment
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Simulate and collect results
        for step in range(100):
            brain.step(step)
        
        results.append({
            'variation': i,
            'config': config,
            'final_stats': brain.get_simulation_stats(),
            'brain': brain
        })
    
    return results
```

#### Pattern 2: Benchmarking and Performance Analysis

```python
import time
import matplotlib.pyplot as plt

def benchmark_brain_performance(brain, steps=1000, warmup=100):
    """Benchmark brain performance with warmup."""
    
    # Warmup
    for i in range(warmup):
        brain.step(i)
    
    # Benchmark
    times = []
    for i in range(steps):
        start = time.perf_counter()
        brain.step(i)
        end = time.perf_counter()
        times.append((end - start) * 1000)  # Convert to milliseconds
    
    avg_time = sum(times) / len(times)
    min_time = min(times)
    max_time = max(times)
    
    return {
        'avg_time_ms': avg_time,
        'min_time_ms': min_time,
        'max_time_ms': max_time,
        'times': times,
        'steps_per_second': steps / (sum(times) / 1000)
    }

def profile_simulation(brain, steps=1000):
    """Profile simulation with detailed metrics."""
    metrics = {
        'step_times': [],
        'neuron_counts': [],
        'firing_rates': [],
        'spike_counts': [],
        'memory_usage': []
    }
    
    for i in range(steps):
        start = time.perf_counter()
        brain.step(i)
        end = time.perf_counter()
        
        metrics['step_times'].append(end - start)
        metrics['neuron_counts'].append(brain.getTotalNeuronCount())
        metrics['firing_rates'].append(brain.getAverageFiringRate())
        metrics['spike_counts'].append(brain.getTotalSpikeCount())
    
    return metrics

def analyze_performance(benchmark_results, profile_results):
    """Analyze and compare performance results."""
    analysis = {}
    
    # Step time analysis
    step_times = benchmark_results['step_times']
    analysis['avg_step_time'] = sum(step_times) / len(step_times)
    analysis['step_time_std'] = (sum((t - analysis['avg_step_time'])**2 for t in step_times) / len(step_times))**0.5
    
    # Throughput analysis
    analysis['steps_per_second'] = len(step_times) / sum(step_times)
    
    # Neuron activity analysis
    firing_rates = profile_results['firing_rates']
    analysis['avg_firing_rate'] = sum(firing_rates) / len(firing_rates)
    analysis['max_firing_rate'] = max(firing_rates)
    analysis['min_firing_rate'] = min(firing_rates)
    
    return analysis
```

#### Pattern 3: State Management and Persistence

```python
def save_experiment_results(brain, agent, world, filename):
    """Save experiment results with comprehensive metadata."""
    
    # Create experiment state
    experiment_state = {
        'timestamp': time.time(),
        'brain_config': brain.getConfig().getAllSettings(),
        'brain_stats': brain.get_simulation_stats(),
        'agent_stats': agent.get_agent_stats(),
        'world_stats': world.get_environment_stats(),
        'final_brain_state': pynlm.export_brain_state(brain),
        'neural_activity': {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'development_stage': brain.getDevelopmentalStage()
        },
        'learning_metrics': agent.get_learning_metrics()
    }
    
    # Save to file
    with open(filename, 'w') as f:
        json.dump(experiment_state, f, indent=2, default=str)
    
    return experiment_state

def load_experiment_results(filename):
    """Load experiment results from file."""
    with open(filename, 'r') as f:
        experiment_state = json.load(f)
    
    # Recreate brain from saved state
    config = pynlm.create_config_from_dict(experiment_state['brain_config'])
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # In a real implementation, you would load the neural network state
    # brain.import_state(experiment_state['final_brain_state'])
    
    return brain, experiment_state
```

### Key Classes and Methods Reference

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

# Advanced methods
brain.get_simulation_stats()      # Get comprehensive statistics
brain.validate_state()             # Validate brain state
brain.logStatus()                  # Log brain status
brain.export_state()               # Export state as Python dict
brain.import_state(state_dict)     # Import state from Python dict
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

# Convenience methods
config.getInt("key", default)
config.getDouble("key", default)
config.getBool("key", default)
config.getString("key", default)

# Validation and utilities
config.validate()
config.getAllSettings()
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

# Advanced methods
agent.get_agent_stats()            # Get comprehensive statistics
agent.get_learning_metrics()       # Get learning metrics
agent.validate_configuration()     # Validate configuration
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

# Advanced methods
world.get_environment_stats()      # Get environment statistics
world.get_cell_count()             # Get total number of cells
world.get_population_density(values)  # Calculate population density
```

### Enumerations

#### Neuron Types

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal

# Utility methods
neuron_type.is_excitatory()
neuron_type.is_inhibitory()
neuron_type.is_sensory()
neuron_type.is_motor()
neuron_type.is_modulatory()
neuron_type.is_internal()
```

#### Synapse Types

```python
# Synapse types
pynlm.SynapseType.Excitatory
pynlm.SynapseType.Inhibitory
pynlm.SynapseType.Modulatory
pynlm.SynapseType.Electrical
pynlm.SynapseType.GapJunction

# Utility methods
synapse_type.is_excitatory()
synapse_type.is_inhibitory()
synapse_type.is_modulatory()
synapse_type.is_electrical()
synapse_type.is_gap_junction()
```

#### Developmental Stages

```python
# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Utility methods
stage.is_plastic()
stage.is_critical()
stage.is_mature()
stage.is_aging()
```

### Troubleshooting

#### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

#### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Validate configuration before using it
- Check for invalid parameters in bindings

**Common errors and solutions:**

1. **Configuration validation errors:**
   ```python
   if not pynlm.validate_config(config):
       print("Configuration is invalid!")
       print("Errors:", config.get_validation_errors())
   ```

2. **Brain state validation errors:**
   ```python
   if not brain.validate_state():
       print("Brain state is invalid!")
       brain.logStatus()  # Log detailed error information
   ```

3. **Agent configuration validation errors:**
   ```python
   if not agent.validate_configuration():
       print("Agent configuration is invalid!")
   ```

4. **Memory issues with large brains:**
   ```python
   # Create smaller brain for testing
   config = pynlm.createDefaultConfig()
   config.set("brain.neuron_count", 100)  # Smaller for testing
   config.set("brain.region_count", 1)
   
   # Validate before use
   if pynlm.validate_config(config):
       brain = pynlm.createBrain(config)
       brain.initialize()
   ```

### Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [README.md](README.md) - Project overview and build instructions

## Advanced User Guide

For advanced usage patterns, performance optimization, and research applications, see the Advanced User Guide which includes:

- Performance benchmarking and profiling
- State management and persistence
- Multi-experiment workflows
- Integration with external tools
- Custom configurations and modifications

To access the Advanced User Guide, visit the documentation directory or consult the advanced usage documentation.