# NLM (Neural Learning Machine) - Enhanced HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This enhanced documentation covers the **Pythonic API** with all new features, including full access to neural systems, advanced error handling, and performance optimizations.

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
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
print(f"Neurons: {brain.get_total_neuron_count()}")
```

## Part 3: Using the Enhanced Python Library

### Basic Usage with Pythonic API

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.create_default_config()

# Optionally configure specific parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.create_brain(config)
brain.initialize()

print(f"Total neurons: {brain.neuron_count}")
print(f"Total synapses: {brain.synapse_count}")
```

#### Running a Simulation with Enhanced Brain API

```python
import pynlm

# Create brain
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.firing_count}")
        print(f"  Total spikes: {brain.spike_count}")
        print(f"  Average firing rate: {brain.average_firing_rate:.2f}")
```

#### Creating a Simple World with Enhanced Interface

```python
import pynlm

# Create a simple 2D world
world = pynlm.create_simple_world()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    vision_width=8,
    vision_height=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.set_agent_position(10.0, 10.0)

# Check world properties
print(f"World size: {world.size}")  # Returns (width, height)
print(f"Vision size: {world.vision_size}")  # Returns (width, height)
print(f"Agent position: {world.get_agent_position()}")  # Returns (x, y)

# Update world
world.update(timestep=0.1)
```

### Advanced Usage Examples

#### Example 1: Complete Agent Simulation with All Systems

```python
import pynlm

def run_advanced_agent_simulation(num_steps=2000):
    """Advanced agent simulation using all neural systems."""
    
    # 1. Create configuration with advanced settings
    config = pynlm.create_default_config()
    config.set("brain.neuron_count", 500)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # 2. Create brain and initialize
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.create_agent_brain(brain)
    
    # 4. Create and configure world
    world = pynlm.create_simple_world()
    world.configure(width=15, height=15, vision_width=8, vision_height=8)
    world.reset()
    world.set_agent_position(7.5, 7.5)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all subsystems
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # 7. Add a resource to the world
    world.add_object(pynlm.WorldObject(
        x=5.0, y=5.0, 
        type=pynlm.WorldObjectType.Resource, 
        value=10.0
    ))
    
    # 8. Run simulation loop with detailed logging
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.get_sensory_percept()
        
        # Process sensory input in brain
        agent.process_sensory_input(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decode_motor_command()
        
        # Apply motor command to world
        world.apply_motor_command(motor_cmd, world.get_simulation_time())
        
        # Apply reward modulation
        reward = world.get_sensory_percept().get_internal()[0] if world.get_sensory_percept().get_internal() else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        
        # Update development
        agent.update_development(0.1)
        
        # Print comprehensive status every 200 steps
        if step % 200 == 0:
            print(f"\nStep {step}:")
            print(f"  Firing neurons: {brain.firing_count}")
            print(f"  Total spikes: {brain.spike_count}")
            print(f"  Development stage: {brain.get_developmental_stage()}")
            print(f"  Curiosity level: {agent.get_curiosity_level():.3f}")
            print(f"  Novelty level: {agent.get_novelty_level():.3f}")
            print(f"  Prediction error: {agent.get_prediction_error():.3f}")
            print(f"  Neuromodulation level: {agent.get_neuromodulation_level():.3f}")
            
            # Access internal systems for advanced analysis
            if brain.get_prediction_system():
                prediction = brain.get_prediction_system()
                print(f"  Has prediction system: {prediction is not None}")
                
            if brain.get_planner():
                planner = brain.get_planner()
                print(f"  Has planning system: {planner is not None}")
                
            if brain.get_concept_formation():
                concept = brain.get_concept_formation()
                print(f"  Has concept formation: {concept is not None}")
    
    print("\nAdvanced agent simulation complete!")
    return brain, world, agent

# Run the simulation
brain, world, agent = run_advanced_agent_simulation(1000)
```

#### Example 2: Performance Optimization with Batch Operations

```python
import pynlm
import time

def benchmark_batch_vs_sequential():
    """Benchmark batch operations vs sequential processing."""
    
    config = pynlm.create_default_config()
    
    # Sequential processing
    brain1 = pynlm.create_brain(config)
    brain1.initialize()
    
    start_time = time.time()
    for step in range(100):
        brain1.step(step)
    sequential_time = time.time() - start_time
    
    # Batch processing
    brain2 = pynlm.create_brain(config)
    brain2.initialize()
    
    start_time = time.time()
    pynlm.batch_step(brain2, list(range(100)))
    batch_time = time.time() - start_time
    
    print(f"Sequential processing time: {sequential_time:.4f}s")
    print(f"Batch processing time: {batch_time:.4f}s")
    print(f"Speedup: {sequential_time/batch_time:.2f}x")
    
    return sequential_time, batch_time

benchmark_batch_vs_sequential()
```

#### Example 3: Memory System Analysis

```python
import pynlm

def analyze_memory_systems():
    """Demonstrate access to all memory systems."""
    
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Access memory systems
    working_memory = brain.get_working_memory()
    episodic_memory = brain.get_episodic_memory()
    associative_memory = brain.get_associative_memory()
    
    print("Memory System Analysis:")
    print(f"Working Memory: {working_memory is not None}")
    print(f"Episodic Memory: {episodic_memory is not None}")
    print(f"Associative Memory: {associative_memory is not None}")
    
    # Check prediction system
    prediction_system = brain.get_prediction_system()
    if prediction_system:
        print(f"Prediction System: Available")
        
    # Check cognitive systems
    planner = brain.get_planner()
    concept_formation = brain.get_concept_formation()
    attention = brain.get_attention()
    
    print(f"Neural Planner: {planner is not None}")
    print(f"Concept Formation: {concept_formation is not None}")
    print(f"Attentional Selection: {attention is not None}")
    
    # Check development system
    development = brain.get_development_system()
    if development:
        print(f"Development System: Available")
        print(f"Current stage: {brain.get_developmental_stage()}")
    
    # Check neuromodulation systems
    dopamine = brain.get_dopamine()
    curiosity = brain.get_curiosity()
    novelty = brain.get_novelty()
    prediction_error = brain.get_prediction_error_signal()
    
    print(f"Dopamine neuromodulator: {dopamine is not None}")
    print(f"Curiosity neuromodulator: {curiosity is not None}")
    print(f"Novelty neuromodulator: {novelty is not None}")
    print(f"Prediction error signal: {prediction_error is not None}")
    
    # Check plasticity systems
    stdp = brain.get_stdp()
    hebbian = brain.get_hebbian()
    structural = brain.get_structural_plasticity()
    
    print(f"STDP plasticity: {stdp is not None}")
    print(f"Hebbian plasticity: {hebbian is not None}")
    print(f"Structural plasticity: {structural is not None}")

analyze_memory_systems()
```

#### Example 4: Error Handling and Context

```python
import pynlm

def demonstrate_error_handling():
    """Demonstrate enhanced error handling with context."""
    
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    try:
        # Normal operation
        brain.step(0)
        print("Step completed successfully")
        
        # Attempt invalid operation
        # brain.get_region(INVALID_ID)  # This would raise an error
        
    except pynlm.InvalidOperationError as e:
        print(f"Invalid operation: {e}")
        # Add context for debugging
        context_error = pynlm.create_error_context("region_access", "Invalid region ID")
        print(f"Contextual error: {context_error}")
        
    except pynlm.ConfigurationError as e:
        print(f"Configuration error: {e}")
        
    except Exception as e:
        print(f"Unexpected error: {e}")

demonstrate_error_handling()
```

#### Example 5: Region and Population Management

```python
import pynlm

def demonstrate_advanced_region_management():
    """Demonstrate advanced brain region management."""
    
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Add regions
    cortex_id = brain.add_region("cortex")
    hippocampus_id = brain.add_region("hippocampus")
    amygdala_id = brain.add_region("amygdala")
    
    print(f"Added regions:")
    print(f"  Cortex: {cortex_id}")
    print(f"  Hippocampus: {hippocampus_id}")
    print(f"  Amygdala: {amygdala_id}")
    
    # Access regions
    cortex = brain.get_region(cortex_id)
    hippocampus = brain.get_region(hippocampus_id)
    
    if cortex:
        print(f"Cortex region accessible")
        
    if hippocampus:
        print(f"Hippocampus region accessible")
    
    # List all regions
    region_ids = brain.get_region_ids()
    print(f"Total regions: {len(region_ids)}")
    print(f"Region IDs: {[str(rid) for rid in region_ids]}")
    
    # Get regions list
    regions = brain.get_regions()
    print(f"Number of regions objects: {len(regions)}")
    
    # Add inter-region connections
    brain.add_inter_region_connection(cortex_id, hippocampus_id, weight=0.5)
    brain.add_inter_region_connection(hippocampus_id, amygdala_id, weight=0.3)
    
    print("Inter-region connections established")

demonstrate_advanced_region_management()
```

### Key Classes and Methods

#### Brain Class (Enhanced API)

```python
# Creation
brain = pynlm.create_brain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)     # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receive_sensory_input(input)  # Inject sensory data
action = brain.produce_action()      # Get motor action

# Statistics (Pythonic properties)
print(brain.neuron_count)      # Total neurons
print(brain.synapse_count)     # Total synapses
print(brain.firing_count)     # Currently firing
print(brain.spike_count)       # Total spikes
print(brain.average_firing_rate)  # Average firing rate
print(brain.region_count)      # Number of regions

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Systems access (NEW!)
working_memory = brain.get_working_memory()
episodic_memory = brain.get_episodic_memory()
prediction_system = brain.get_prediction_system()
planner = brain.get_planner()
concept_formation = brain.get_concept_formation()
attention = brain.get_attention()
development = brain.get_development_system()

# Neuromodulation access (NEW!)
dopamine = brain.get_dopamine()
curiosity = brain.get_curiosity()
novelty = brain.get_novelty()
prediction_error = brain.get_prediction_error_signal()

# Plasticity access (NEW!)
stdp = brain.get_stdp()
hebbian = brain.get_hebbian()
structural = brain.get_structural_plasticity()

# Context helpers
brain.get_most_active_neurons(10)  # Get most active neurons
```

#### Config Class (Enhanced API)

```python
# Create
config = pynlm.create_default_config()

# Query with Pythonic methods
print(config.has("brain.neuron_count"))
keys = config.get_keys()

# Type-safe getters
print(config.get("brain.neuron_count"))           # Returns Optional[int64_t]
print(config.get("brain.connection_probability")) # Returns Optional[float]
print(config.get("brain.enable_learning"))       # Returns Optional[bool]
print(config.get("brain.name"))                   # Returns Optional[str]

# Pythonic get_or methods
print(config.get_or_int("brain.neuron_count", 1000))
print(config.get_or_float("brain.learning_rate", 0.001))
print(config.get_or_bool("brain.enable_curiosity", True))
print(config.get_or_str("brain.name", "default"))

# Set with source parameter
config.set("brain.neuron_count", 1000, pynlm.ConfigSource.Runtime)

# Remove
config.remove("brain.unused_key")
```

#### AgentBrain Class (Enhanced API)

```python
# Creation
agent = pynlm.create_agent_brain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.process_sensory_input(percept)

# Motor decoding
motor_cmd = agent.decode_motor_command()

# Enhanced methods (NEW!)
agent.decode_from_motor_neurons()  # Direct motor decoding
agent.select_with_curiosity(default_cmd)  # Curiosity-based selection

# Neuromodulation
agent.apply_reward_modulation(reward, predicted_reward)

# Development
agent.update_development(timestep)
agent.get_developmental_stage()

# Enhanced status queries (NEW!)
print(agent.is_reward_modulation_enabled())
print(agent.is_structural_plasticity_enabled())
print(agent.is_development_enabled())
print(agent.is_curiosity_enabled())

# Pythonic properties (NEW!)
print(agent.reward_modulation_enabled)
print(agent.structural_plasticity_enabled)
print(agent.development_enabled)
print(agent.curiosity_enabled)

# Statistics
print(agent.get_curiosity_level())
print(agent.get_novelty_level())
print(agent.get_prediction_error())
print(agent.get_neuromodulation_level())
```

#### SimpleWorld Class (Enhanced API)

```python
# Creation
world = pynlm.create_simple_world()

# Configuration
world.configure(width=20, height=20, vision_width=8, vision_height=8)

# Control
world.reset()
world.update(timestep)
world.set_random_seed(42)

# Agent interaction
world.set_agent_position(x, y)  # Pythonic alias
world.apply_motor_command(motor_cmd, current_time)

# State access
percept = world.get_sensory_percept()
body = world.get_agent_body()

# Pythonic properties (NEW!)
print(world.size)               # (width, height)
print(world.vision_size)        # (vision_width, vision_height)
print(world.get_agent_position())  # (x, y)

# Enhanced object management
world.add_object(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.remove_object(x, y)
world.is_valid_position(x, y)
```

### Enumerations

All enumerations now include string representations for better debugging:

```python
# Neuron types
print(pynlm.NeuronType.Excitatory)      # NeuronType.Excitatory
print(str(pynlm.NeuronType.Excitatory))  # "Excitatory"

# Developmental stages
print(pynlm.DevelopmentalStage.Adult)   # DevelopmentalStage.Adult
print(str(pynlm.DevelopmentalStage.Adult))  # "Adult"

# Action types
print(pynlm.ActionType.MoveForward)     # ActionType.MoveForward
print(str(pynlm.ActionType.MoveForward))  # "MoveForward"
```

### Configuration Options

#### Brain Configuration

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

#### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Part 4: Performance Utilities

### Batch Operations

The enhanced bindings include batch operations for performance optimization:

```python
import pynlm

# Batch simulation steps
brain = pynlm.create_brain(config)
brain.initialize()

# Process multiple steps at once
steps = list(range(1000))
pynlm.batch_step(brain, steps)

# Batch with timestamps
step_pairs = [(i, i * 0.001) for i in range(1000)]
pynlm.batch_step_with_time(brain, step_pairs)
```

### Performance Monitoring

The enhanced API provides access to performance monitoring utilities:

```python
# Get spike system for monitoring
spike_system = brain.get_spike_system()
if spike_system:
    # Access spike system statistics
    print(f"Pending spike events: {brain.get_pending_spike_event_count()}")
    most_active = brain.get_most_active_neurons(10)
```

## Part 5: Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction (Advanced)

```python
import pynlm

# Setup
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()
agent = pynlm.create_agent_brain(brain)
world = pynlm.create_simple_world()
world.configure(width=10, height=10, vision_width=8, vision_height=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# Run episode
for step in range(500):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step)
    world.apply_motor_command(agent.decode_motor_command(), world.get_simulation_time())
```

## Part 6: Troubleshooting

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

### Common Python API Issues

**"AttributeError: 'Brain' object has no attribute 'some_new_property'"
- The Pythonic properties are available in the enhanced version
- Use `brain.get_total_neuron_count()` or `brain.neuron_count` (alias)

**"MemoryError: out of memory"
- The enhanced bindings include better memory management
- Use batch operations to reduce memory overhead
- Monitor system resources during long simulations

## Part 7: Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

## Quick Reference

| What you want | Old Code | New Code |
|--------------|----------|----------|
| Create brain | `pynlm.createBrain(config)` | `pynlm.create_brain(config)` |
| Start brain | `brain.initialize()` | `brain.initialize()` |
| Make brain think | `brain.step(step)` | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` | `pynlm.create_world()` |
| Create agent | `pynlm.createAgentBrain(brain)` | `pynlm.create_agent_brain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` | `brain.firing_count` |
| See world | `world.getSensoryPercept()` | `world.get_sensory_percept()` |
| Make action | `agent.decodeMotorCommand()` | `agent.decode_motor_command()` |

## Migration Guide

The enhanced Python bindings maintain backward compatibility while adding new features:

- **Method names**: Original names still work, new snake_case names available
- **Return values**: All original return types preserved
- **API additions**: New methods and properties for enhanced functionality
- **Error handling**: Original exceptions preserved, new custom exceptions added

To migrate:
1. Keep existing code working unchanged
2. Gradually adopt Pythonic properties and new methods
3. Use new error handling for better debugging
4. Leverage batch operations for performance
5. Explore new system access methods

That's it! You're now ready to use the enhanced NLM with Pythonic API, custom exceptions, batch operations, and full system access.
