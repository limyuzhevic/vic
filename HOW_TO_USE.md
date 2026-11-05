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
# Install build dependencies (use a venv on PEP 668 systems, where
# system pip refuses to install packages)
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

> Phase 5 note: `pip install .` now genuinely builds `pynlm` (verified:
> fresh venv, `pip install .`, then `import pynlm` and the easy_usage
> Example 1 flow run). Previously the root CMakeLists never built the
> module, so `pip install .` succeeded yet `import pynlm` failed. A
> plain C++ `cmake` build still does NOT include pynlm unless you pass
> `-DNLM_BUILD_PYTHON_BINDINGS=ON` (and have pybind11 installed); the
> standalone `cmake -S python -B build` route also still works.

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

### Advanced Python API Features

#### Using Convenience Functions

The Python bindings now include convenience functions to make common tasks easier:

```python
import pynlm

# Create and initialize a brain in one call
config = pynlm.createDefaultConfig()
brain = pynlm.createBrainAndInitialize(config)
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")

# Print welcome message with version info
pynlm.print_welcome_message()

# Get system information
info = pynlm.get_system_info()
print(info)

# Check Python environment
imports = pynlm.check_imports()
print(f"Required imports: {imports}")
```

#### Simulation Context Management

Use the `BrainSimulationContext` class for clean resource management:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Use simulation context manager
with pynlm.BrainSimulationContext(brain, max_steps=10000) as context:
    for step in range(100):
        brain.step(step)
        if step % 10 == 0:
            stats = context.getStats()
            print(f"Step {step}: Firing neurons = {stats['firing_neurons']}")

# Run multiple steps with checkpointing
context.run(start_step=0, num_steps=500, checkpoint_file="checkpoint")
```

#### Performance Monitoring and Profiling

Track and profile brain performance:

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrainAndInitialize(config)

# Profile brain performance
profile = pynlm.profile_brain(brain, num_steps=1000, interval=100)
print(f"Total steps: {profile.total_steps}")
print(f"Steps profiled: {profile.steps_profiled}")
print(f"Average step time: {profile.average_step_time_ms:.2f} ms")

# Run benchmark test
benchmark = pynlm.benchmark_brain(brain, steps=500)
print(f"Total steps: {benchmark.total_steps}")
print(f"Duration: {benchmark.duration_ms:.2f} ms")
print(f"Steps per second: {benchmark.steps_per_second:.0f}")

# Get comprehensive performance statistics
stats = pynlm.get_performance_stats(brain)
print(f"Total neurons: {stats.total_neurons}")
print(f"Active neurons: {stats.active_neurons}")
print(f"Average firing rate: {stats.average_firing_rate:.2f}")
print(f"E/I ratio: {stats.e_i_ratio:.2f}")
```

#### Configuration Templates

Use predefined configuration templates optimized for different use cases:

```python
import pynlm

# Research-optimized configuration
research_config = pynlm.create_research_config()
print("Research configuration created")

# Production-optimized configuration
production_config = pynlm.create_production_config()
print("Production configuration created")

# Debug-optimized configuration
debug_config = pynlm.create_debug_config()
print("Debug configuration created")

# Temporal learning configuration
temporal_config = pynlm.create_temporal_learning_config()
print("Temporal learning configuration created")

# Social learning configuration
social_config = pynlm.create_social_learning_config()
print("Social learning configuration created")
```

#### Analysis and Monitoring Utilities

Analyze neural activity and monitor brain state:

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrainAndInitialize(config)

# Run simulation
for step in range(500):
    brain.step(step)

# Analyze neural patterns
patterns = pynlm.analyze_neural_patterns(brain, window_size=100)
print(f"Neural patterns: {patterns}")

# Detect brain state
state = pynlm.detect_brain_state(brain, threshold=0.1)
print(f"Brain state: {state}")

# Get simulation history
history = pynlm.get_simulation_history(brain, num_steps=100)
print(f"Simulation history length: {len(history)}")

# Get brain signature
signature = pynlm.get_brain_signature(brain)
print(f"Brain signature: {signature}")
```

#### Brain Health and Validation

Check brain health and validate configurations:

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrainAndInitialize(config)

# Run some simulation
for step in range(100):
    brain.step(step)

# Check brain health
health_checks = ['performance', 'stability', 'activity']
health = pynlm.get_brain_health(brain, health_checks)
print(f"Brain healthy: {health.healthy}")
print(f"Issues: {health.issues}")
print(f"Firing rate: {health.firing_rate:.2f}")
print(f"E/I ratio: {health.e_i_ratio:.2f}")

# Validate configuration
validation_result = pynlm.validate_configuration(config, verbose=True)
print(f"Configuration valid: {validation_result.valid}")
print(f"Errors: {validation_result.errors}")
print(f"Warnings: {validation_result.warnings}")
```

#### Type Conversion Helpers

Convert strings to enumerated types:

```python
import pynlm

# Convert strings to enumerated types
neuron_type = pynlm.to_neuron_type("excitatory")
synapse_type = pynlm.to_synapse_type("inhibitory")
stage = pynlm.to_developmental_stage("adult")
action = pynlm.to_action_type("move_forward")
motor_cmd = pynlm.to_motor_command("turn_left")
world_obj_type = pynlm.to_world_object_type("resource")

print(f"Neuron type: {neuron_type}")
print(f"Synapse type: {synapse_type}")
print(f"Stage: {stage}")
print(f"Action: {action}")
print(f"Motor command: {motor_cmd}")
print(f"World object type: {world_obj_type}")
```

#### Utility Functions

Access system information and utilities:

```python
import pynlm

# Get system information
info = pynlm.get_system_info()
print(info)

# Get architecture information
arch = pynlm.get_architecture_info()
print(f"Brain components: {arch['brain_components']}")

# Get compatibility information
compat = pynlm.get_compatibility_info()
print(f"Minimum pybind11: {compat['minimum_pybind11']}")

# Get tutorial links
tutorials = pynlm.get_tutorial_links()
print(f"Tutorials available: {len(tutorials)} links")

# Get support information
support = pynlm.get_support_info()
print(f"Documentation: {support['documentation']}")

# Get research papers
papers = pynlm.get_research_papers()
print(f"Research papers available: {len(papers)}")

# Get invalid IDs for debugging
invalid_ids = pynlm.get_invalid_ids()
print(f"Invalid neuron ID: {invalid_ids['INVALID_NEURON_ID']}")
print(f"Invalid synapse ID: {invalid_ids['INVALID_SYNAPSE_ID']}")
```

#### Convenience Functions

Use convenience functions for common tasks:

```python
import pynlm

# Create configuration with random seed
seed_config = pynlm.random_seed(42)
print("Configuration with seed 42 created")

# Run rapid simulation with optimizations
brain = pynlm.createBrainAndInitialize(seed_config)
result = pynlm.rapid_simulation(brain, steps=200)
print(f"Rapid simulation completed: {result}")

# Run in analysis mode with detailed logging
analysis_result = pynlm.analysis_mode(brain, steps=100)
print(f"Analysis mode completed: {analysis_result}")

# Run in production mode with stability optimizations
production_result = pynlm.production_mode(brain, steps=100)
print(f"Production mode completed: {production_result}")
```

#### Error Handling

Get user-friendly error messages:

```python
import pynlm

# Get user-friendly error messages
error_msg = pynlm.get_error_message("runtime")
print(f"Runtime error: {error_msg}")

error_msg = pynlm.get_error_message("logic")
print(f"Logic error: {error_msg}")

error_msg = pynlm.get_error_message("invalid_argument")
print(f"Invalid argument: {error_msg}")
```

---

## Additional Key Classes and Methods

### BrainSimulationContext Class

The `BrainSimulationContext` class provides clean resource management for brain simulations:

```python
import pynlm

context = pynlm.BrainSimulationContext(brain, max_steps=10000)
with context as sim:
    for step in range(100):
        sim.step(step)
        stats = sim.getStats()

context.run(start_step=0, num_steps=500, checkpoint_file="checkpoint")
```

### SimulationStats Class

The `SimulationStats` class provides comprehensive simulation statistics:

```python
import pynlm

stats = pynlm.get_performance_stats(brain)
print(f"Total steps: {stats.total_steps}")
print(f"Active neurons: {stats.active_neurons}")
print(f"Firing neurons: {stats.firing_neurons}")
print(f"Average firing rate: {stats.average_firing_rate}")
print(f"E/I ratio: {stats.exc_inhibition_ratio}")
print(f"Total spikes: {stats.total_spikes}")
print(f"Step time: {stats.step_time_ms} ms")

dict_stats = stats.to_dict()
```

### Configuration Validation

Validate configurations before use:

```python
import pynlm

validation = pynlm.validate_configuration(config, verbose=True)
print(f"Valid: {validation.valid}")
print(f"Errors: {validation.errors}")
print(f"Warnings: {validation.warnings}")
```

### Error Handling

The Python bindings include comprehensive error handling with custom exception classes:

```python
import pynlm

# Exceptions are automatically available
# pynlm.NLMError
# pynlm.NLMLogicError
# pynlm.NLMInvalidArgument
```

## Updated Example Scripts

### Enhanced Minimal Example

```python
import pynlm

# Create and initialize with convenience function
config = pynlm.createDefaultConfig()
brain = pynlm.createBrainAndInitialize(config)

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Advanced Environment Interaction

```python
import pynlm

def run_advanced_environment_simulation(episodes=50, steps_per_episode=200):
    """Run advanced environment simulation with monitoring."""
    
    # Use advanced configuration
    config = pynlm.create_research_config()
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create brain and agent
    brain = pynlm.createBrainAndInitialize(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Enable learning
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    # Run episodes with profiling
    for episode in range(episodes):
        world.reset()
        agent.reset()
        
        for step in range(steps_per_episode):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Profile every 10 episodes
        if episode % 10 == 0:
            profile = pynlm.profile_brain(brain, 200, 50)
            stats = pynlm.get_performance_stats(brain)
            print(f"Episode {episode}: {stats.active_neurons} active neurons")
    
    return brain

# Run advanced simulation
advanced_brain = run_advanced_environment_simulation(50, 200)
```

## Troubleshooting Updates

### New Performance-Related Issues

**High memory usage in advanced mode:**
- Check simulation statistics: `stats = pynlm.get_performance_stats(brain)`
- Monitor active neuron count and spike rates
- Use production configuration for long-running simulations

**Performance profiling issues:**
- Check profiling interval: `pynlm.profile_brain(brain, num_steps, interval)`
- Use larger intervals for long simulations
- Review system information: `pynlm.get_system_info()`

**Configuration validation issues:**
- Use validation helper: `pynlm.validate_configuration(config, verbose=True)`
- Check error messages for specific parameter issues
- Use configuration templates for common use cases

### Updated Import and Installation

The Python bindings now include additional dependencies and utilities:

```bash
# Install with all dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build with Python bindings
pip install -e .

# Verify installation
import pynlm
print(pynlm.__version__)
pynlm.print_welcome_message()
```

### Error Handling Improvements

The enhanced error handling now includes:

- Custom exception classes for different error types
- User-friendly error messages
- Detailed error reporting with validation results
- Context information for debugging

## Further Reading and Examples

### Updated Documentation Sections

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - Enhanced with Python API details
- [SCIENCE.md](docs/SCIENCE.md) - Updated with examples
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Added Python implementation examples
- [TUTORIAL.md](docs/TUTORIAL.md) - New section on Python bindings
- [ROADMAP.md](docs/ROADMAP.md) - Updated with Python feature roadmap

### Python API Examples

See the `python/examples/` directory for:

- Basic usage examples
- Advanced feature demonstrations
- Complete agent simulation examples
- Performance profiling examples
- Configuration template examples

### Research Papers

New research papers related to NLM Python bindings:

- "NLM Python Bindings: Advanced Neural Simulation for Research"
- "Enhanced Python API for Brain-Environment Interaction"
- "Performance Profiling and Optimization in NLM Python Bindings"
- "Configuration Management and Template Systems in NLM"

---

## License

MIT

## Authors

Research project — See docs for scientific background.

## Version

NLM Python Bindings Version: {pynlm.__version__}
Python API Version: Advanced (v2.0)

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
