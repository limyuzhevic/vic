# NLM (Neural Learning Machine) - Enhanced HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20 with **enhanced Python bindings**. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the enhanced Python library
3. Using the **enhanced** NLM Python API with advanced features

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

After successful compilation, you will have enhanced Python bindings with:

| Target | Type | Description |
|--------|------|-------------|
| `pynlm` | Python Module | Enhanced Python bindings with advanced features |
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

## Part 2: Installing the Enhanced Python Library

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

# Create a simple brain with error handling
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    print(f"Neurons: {brain.getTotalNeuronCount()}")
except (pynlm.ConfigurationError, pynlm.SimulationError) as e:
    print(f"Error: {e}")
```

---

## Part 3: Using the Enhanced Python Library

### Enhanced Basic Usage

#### Creating a Brain with Error Handling

```python
import pynlm

# Create a default configuration
try:
    config = pynlm.createDefaultConfig()
except pynlm.ConfigurationError as e:
    print(f"Failed to create config: {e}")
    exit(1)

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain with enhanced error handling
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.SimulationError as e:
    print(f"Failed to initialize brain: {e}")
    exit(1)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation with Monitoring

```python
import pynlm

# Create brain
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
except (pynlm.ConfigurationError, pynlm.SimulationError) as e:
    print(f"Error: {e}")
    exit(1)

# Run simulation for 1000 steps with monitoring
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically with enhanced metrics
    if step % 100 == 0:
        metrics = brain.getMetrics()
        print(f"Step {step}:")
        print(f"  Firing neurons: {metrics['firing_neurons']}")
        print(f"  Total spikes: {metrics['total_spikes']}")
        print(f"  Average firing rate: {metrics['avg_firing_rate']:.2f}")
        print(f"  E/I ratio: {metrics['ei_ratio']:.2f}")
        print(f"  Developmental stage: {metrics['developmental_stage']}")
```

#### Creating a Simple World with Enhanced Features

```python
import pynlm

# Create a simple 2D world
try:
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
    
except pynlm.SimulationError as e:
    print(f"Error creating world: {e}")
    exit(1)

# Validate world state
percept = world.getSensoryPercept()
if percept.isComplete():
    print("World created successfully with valid percept data")
else:
    print("Warning: World percept may not be complete")
```

### Enhanced Complete Agent Example

```python
import pynlm

def run_enhanced_agent_simulation(num_steps=1000):
    """Enhanced agent simulation with comprehensive error handling and monitoring."""
    
    try:
        # 1. Create configuration with validation
        config = pynlm.createDefaultConfig()
        
        # 2. Create brain and initialize with error handling
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
        
        # 6. Enable advanced subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # 7. Run simulation loop with enhanced monitoring
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
                print(f"  Prediction Error: {agent.getPredictionError():.3f}")
                print(f"  Neuromodulation Level: {agent.getNeuromodulationLevel():.3f}")
                print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
        
        # Return enhanced results
        return {
            "success": True,
            "steps_completed": num_steps,
            "final_spikes": brain.getTotalSpikeCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "prediction_error": agent.getPredictionError(),
            "neuromodulation": agent.getNeuromodulationLevel(),
            "developmental_stage": brain.getDevelopmentalStage(),
            "brain_metrics": brain.getMetrics()
        }
        
    except pynlm.ConfigurationError as e:
        return {
            "success": False,
            "error": f"Configuration error: {e}",
            "error_type": "configuration"
        }
    except pynlm.SimulationError as e:
        return {
            "success": False,
            "error": f"Simulation error: {e}",
            "error_type": "simulation"
        }
    except Exception as e:
        return {
            "success": False,
            "error": f"Unexpected error: {e}",
            "error_type": "unexpected"
        }

# Run the enhanced simulation
result = run_enhanced_agent_simulation(1000)
if result["success"]:
    print("Enhanced agent simulation complete!")
    print(f"Final curiosity level: {result['curiosity']:.3f}")
    print(f"Final novelty level: {result['novelty']:.3f}")
else:
    print(f"Simulation failed: {result['error']}")
```

### Enhanced Convenience Functions for Beginners

```python
import pynlm

# Create a simple agent simulation with automatic error handling
try:
    simulation = pynlm.createSimpleAgentSimulation()
    if simulation["success"]:
        print("Simple agent simulation created successfully")
        config = simulation["config"]
        brain = simulation["brain"]
        
        # Run a quick test
        test_result = pynlm.runQuickTest()
        if test_result["success"]:
            print(f"Quick test passed: {test_result['neurons_created']} neurons created")
        else:
            print(f"Quick test failed: {test_result['message']}")
    else:
        print(f"Failed to create simulation: {simulation['message']}")
except Exception as e:
    print(f"Error: {e}")
```

### Enhanced Brain Class

```python
# Enhanced Brain methods with error handling
brain = pynlm.createBrain(config)
brain.initialize()

# Creation and Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Enhanced Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
brain.getTotalSpikeCount()       # Total spikes
brain.getDevelopmentalStage()    # Current developmental stage

# Enhanced State Management
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Enhanced Region Management
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
brain.getRegionCount()
brain.getRegionIds()
brain.getRegions()

# Enhanced Monitoring
metrics = brain.getMetrics()
statistics = brain.getStatistics()
connectivity = brain.analyzeConnectivity()
anomalies = brain.detectAnomalies()
```

### Enhanced Config Class

```python
# Enhanced Config methods
config = pynlm.createDefaultConfig()

# Create with validation
try:
    config.validate()  # Raises ConfigurationError if invalid
except pynlm.ConfigurationError as e:
    print(f"Config validation failed: {e}")

# Enhanced Get methods
if config.has("brain.neuron_count"):
    # Type-safe get methods
    neurons = config.get_int("brain.neuron_count")
    if neurons.has_value():
        print(f"Neuron count: {neurons.value()}")
    
    density = config.get_float("brain.synapse_density")
    weight = config.get_bool("enable_learning")
    name = config.get_string("agent.name")

# Enhanced Set methods with source tracking
config.set("brain.neuron_count", 1000)  # Default Runtime
config.set("brain.synapse_density", 0.1, pynlm.ConfigSource::File)  # File source

# Enhanced Query
keys = config.getKeys()
config.remove("temp_key")
config.clear()  # Clear all

# Enhanced I/O with error handling
if config.saveToFile("config.json"):
    print("Config saved successfully")

config2 = pynlm.createDefaultConfig()
if config2.loadFromFile("config.json"):
    print("Config loaded successfully")
```

### Enhanced AgentBrain Class

```python
# Enhanced AgentBrain with advanced features
agent = pynlm.createAgentBrain(brain)

# Enhanced initialization
agent.initialize(world)

# Enhanced sensory processing
agent.processSensoryInput(percept)

# Enhanced motor decoding
motor_cmd = agent.decodeMotorCommand()

# Enhanced neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Enhanced development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Enhanced statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()

# Enhanced subsystem control
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Enhanced behavior analysis
activity = agent.getNeuralActivity()
profile = agent.getBehaviorProfile()
should_explore = agent.suggestExploration()
exploration_bonus = agent.getExplorationBonus()
```

### Enhanced SimpleWorld Class

```python
# Enhanced SimpleWorld with additional features
world = pynlm.createSimpleWorld()

# Enhanced configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Enhanced control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Enhanced agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# Enhanced state access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# Enhanced world object management
obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
world.addObject(obj)
world.removeObject(x, y)
world.isValidPosition(x, y)

# Enhanced world utilities
world.getObjectCount()
nearby = world.findObjectsNear(x, y, range)
bounds = world.getWorldBounds()
```

### Enhanced Enumerations

```python
# Enhanced enumerations with better documentation

# Neuron types
pynlm.NeuronType.Excitatory      # Excitatory neurons
pynlm.NeuronType.Inhibitory      # Inhibitory neurons
pynlm.NeuronType.Modulatory      # Modulatory neurons
pynlm.NeuronType.Sensory         # Sensory neurons
pynlm.NeuronType.Motor           # Motor neurons
pynlm.NeuronType.Internal        # Internal neurons

# Developmental stages
pynlm.DevelopmentalStage.Initial         # Initial stage with high plasticity
pynlm.DevelopmentalStage.CriticalPeriod  # Critical period
pynlm.DevelopmentalStage.Maturation      # Maturation phase
pynlm.DevelopmentalStage.Adult          # Adult stage
pynlm.DevelopmentalStage.Aging          # Aging phase

# Action types
pynlm.ActionType.MoveForward    # Move forward
pynlm.ActionType.MoveBackward   # Move backward
pynlm.ActionType.MoveLeft       # Move left
pynlm.ActionType.MoveRight      # Move right
pynlm.ActionType.TurnLeft       # Turn left
pynlm.ActionType.TurnRight      # Turn right
pynlm.ActionType.Interact       # Interact with objects
pynlm.ActionType.Eat            # Eat food/resource
pynlm.ActionType.Drink          # Drink water
pynlm.ActionType.Rest           # Rest and recover energy
pynlm.ActionType.Wait           # Wait without moving
pynlm.ActionType.Custom         # Custom action with parameters

# World object types
pynlm.WorldObjectType.Empty      # Empty space
pynlm.WorldObjectType.Resource    # Consumable resource
pynlm.WorldObjectType.Hazard      # Hazardous object
pynlm.WorldObjectType.Wall        # Immovable wall
pynlm.WorldObjectType.Marker      # Interactive marker
```

### Enhanced Error Handling

```python
import pynlm

# Use enhanced exception types
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for i in range(100):
        brain.step(i)
        
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    # Handle configuration issues
    
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
    # Handle simulation issues
    
except pynlm.NLMError as e:
    print(f"General NLM error: {e}")
    # Handle general NLM issues
    
except Exception as e:
    print(f"Unexpected error: {e}")
    # Handle unexpected errors
```

---

## Part 4: Enhanced Configuration Options

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

### Enhanced Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Enhanced Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.curiosity.threshold` | float | 0.5 | Curiosity threshold for exploration |
| `neuromod.novelty.decay_rate` | float | 0.1 | Novelty decay rate |

---

## Part 5: Enhanced Example Scripts

### Enhanced Minimal Example

```python
import pynlm

# Create and initialize with error handling
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Simulate
    for i in range(100):
        brain.step(i)
    
    print("Simulation complete!")
    print(f"Final metrics: {brain.getMetrics()}")
    
except (pynlm.ConfigurationError, pynlm.SimulationError) as e:
    print(f"Error: {e}")
```

### Enhanced Environment Interaction

```python
import pynlm

# Enhanced environment interaction with monitoring
def run_enhanced_environment_simulation():
    try:
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run episode
        for step in range(500):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(0)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            
            if step % 100 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
                print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        
        return True
        
    except (pynlm.ConfigurationError, pynlm.SimulationError) as e:
        print(f"Simulation error: {e}")
        return False

run_enhanced_environment_simulation()
```

---

## Part 6: Enhanced Troubleshooting

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
- Check for Python and pybind11 dependencies

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Check for null pointers or invalid references
- Verify configuration is valid

**Enhanced Configuration Issues:**
- Invalid configuration keys
- Type mismatches in configuration values
- Missing required configuration parameters

**Enhanced Simulation Issues:**
- Division by zero in calculations
- Invalid neuron IDs or synapse IDs
- Memory access violations
- Configuration validation failures

### Enhanced Debugging Tips

1. **Use enhanced error messages**: The enhanced bindings provide detailed error messages with context
2. **Check module information**: Use `pynlm.getModuleInfo()` to verify installation
3. **Run quick tests**: Use `pynlm.runQuickTest()` to verify basic functionality
4. **Validate configurations**: Always validate configurations before use
5. **Monitor metrics**: Use `brain.getMetrics()` for debugging and monitoring

---

## Part 7: Enhanced Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - Enhanced system architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Enhanced experiment descriptions

---

## Enhanced Version Information

- **Python Bindings Version**: 2.0.0-enhanced
- **Author**: NLM Development Team
- **Description**: Enhanced Python bindings for NLM with improved API, error handling, and advanced features
- **Key Enhancements**:
  - Enhanced error handling with detailed exception types
  - Convenience methods and factory functions
  - Advanced monitoring and analysis capabilities
  - Type safety and validation
  - Comprehensive documentation and examples
  - Simulation session management
  - Enhanced configuration management
  - Power-user tools and utilities

---

## Installation and Setup

### Quick Installation

```bash
# Install with all dependencies
pip install -e .[dev]

# Build only
pip install build
python -m build

# Verify installation
python -c "import pynlm; print(pynlm.__version__); print(pynlm.getModuleInfo())"
```

### Development Setup

```bash
# Clone the repository
cd /path/to/nlm

# Create build directory
mkdir -p build
cd build

# Configure with enhanced options
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python)

# Build with optimizations
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

### Enhanced Documentation

The enhanced documentation includes:
- Comprehensive API reference
- Detailed examples with error handling
- Advanced usage patterns
- Performance optimization tips
- Troubleshooting guides
- Migration guides from older versions

This enhanced documentation provides everything needed to get started with NLM and make the most of the enhanced Python bindings.