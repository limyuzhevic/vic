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

## Part 1.5: Advanced Python API Enhancements (Phase 6.5)

NLM now includes advanced Python bindings and utilities that make the neural simulation framework even more powerful and accessible for research and development.

### Overview of Advanced Features

The advanced Python API provides:

1. **Convenience Factory Functions**: Easy setup for common simulation scenarios
2. **Simulation Presets**: Pre-configured brains for specific research goals
3. **Advanced Training Tools**: Custom reward functions, curriculum learning, exploration strategies
4. **Performance Profiling**: Detailed simulation performance analysis
5. **Brain Activity Visualization**: Pattern detection and statistical analysis
6. **Comprehensive Debug Monitoring**: Integrated logging and debugging tools

### Quick Start Example

```python
import pynlm
from pynlm_advanced import (
    SimulationPreset, BrainVisualizer, AgentTrainer,
    SimulationProfiler, create_preset_brain,
    create_agent_world_setup, setup_debug_monitor
)

# Create a brain using a preset
brain_config = create_preset_brain(SimulationPreset.LEARNING_EXPERIMENT)
brain = brain_config['brain']

# Set up complete agent-world system
system = create_agent_world_setup(vision_width=16, vision_height=16)
brain = system['brain']
world = system['world']
agent = system['agent']

# Set up comprehensive monitoring
monitor = setup_debug_monitor(
    brain=brain, world=world, agent=agent,
    log_level="INFO", enable_profiling=True
)

# Run enhanced simulation
visualizer = BrainVisualizer(brain)
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Collect activity data periodically
    if step % 100 == 0:
        visualizer.collect_activity_data(1000, 100)

# Analyze results
activity_data = visualizer.collect_activity_data(1000, 100)
analysis = visualizer.analyze_patterns(activity_data)
print(f"Firing rate trend: {analysis['firing_rate_stats']['trend']:.4f}")
```

### Key Advanced Features

#### 1. Simulation Presets
The `create_preset_brain()` function provides pre-configured brains optimized for different research scenarios:

| Preset | Neuron Count | Learning Focus | Best Use Case |
|--------|--------------|----------------|--------------|
| `FAST_TEST` | 200 | Minimal | Quick validation, debugging |
| `LEARNING_EXPERIMENT` | 1000 | Maximum | Studying learning processes |
| `CONTINUAL_LEARNING` | 1500 | Long-term | Extended learning studies |
| `BEHAVIORAL_TEST` | 800 | Behavior | Real-world behavior validation |
| `DEVELOPMENTAL` | 1200 | Growth | Brain development studies |
| `EXPLORATION` | 1000 | Discovery | Curiosity-driven research |
| `CONSOLIDATION` | 900 | Memory | Memory consolidation studies |

#### 2. Advanced Training Tools
The `AgentTrainer` class provides sophisticated training capabilities:

- **Custom Reward Functions**: Define your own reward logic with flexible callbacks
- **Curriculum Learning**: Progressive training through staged difficulty levels
- **Exploration Strategies**: Multiple exploration methods comparison
- **Detailed Performance Tracking**: Comprehensive metrics and analysis

#### 3. Performance and Analysis Tools
The `SimulationProfiler` provides detailed performance analysis:

- **Timing Analysis**: Precise measurement of simulation performance
- **Resource Usage**: CPU time and step-by-step profiling
- **Performance Assessment**: Automatic evaluation of simulation efficiency
- **Comparative Analysis**: Benchmark your simulations against standards

#### 4. Brain Activity Visualization and Analysis
The `BrainVisualizer` class provides sophisticated activity analysis:

- **Pattern Detection**: Identify oscillations, trends, and periodic behaviors
- **Stability Analysis**: Quantify firing rate stability and reliability
- **Statistical Analysis**: Compute mean, variance, and correlation metrics
- **Activity Collection**: Systematically record neural activity over time

#### 5. Debug and Monitoring
The `setup_debug_monitor()` function provides comprehensive debugging tools:

- **Integrated Logging**: Configurable log levels and formatting
- **Real-time Monitoring**: Track simulation progress during execution
- **Performance Profiling**: Automatic resource usage tracking
- **Error Detection**: Identify performance bottlenecks and issues

### Installation and Setup

The advanced features are automatically included when installing the pynlm package:

```bash
# Install the full package with all features
pip install pynlm

# Or install in development mode for editing
pip install -e .
```

To use the advanced features, import from the `nlm_advanced` module:

```python
# Advanced utilities for enhanced simulation
from pynlm_advanced import (
    SimulationPreset, BrainVisualizer, AgentTrainer,
    SimulationProfiler, create_preset_brain,
    create_agent_world_setup, setup_debug_monitor
)
```

### Advanced Usage Examples

#### Example 1: Training with Custom Rewards

```python
import pynlm
from pynlm_advanced import AgentTrainer, create_agent_world_setup

def curiosity_reward(world, agent, step):
    """Reward exploration and learning behaviors."""
    novelty = agent.getNoveltyLevel()
    curiosity = agent.getCuriosityLevel()
    prediction_error = abs(agent.getPredictionError())
    
    # Combine multiple reward signals
    reward = novelty * 0.5 + curiosity * 0.3 + prediction_error * 0.2
    return reward

# Create training system
system = create_agent_world_setup(vision_width=8, vision_height=8)
trainer = AgentTrainer(system['brain'], system['world'], system['agent'])

# Train with custom reward function
results = trainer.train_with_custom_reward(
    reward_function=curiosity_reward,
    num_episodes=50,
    exploration_factor=1.5
)

print(f"Training complete! Mean reward: {results['mean_reward']:.3f}")
```

#### Example 2: Curriculum Learning

```python
from pynlm_advanced import AgentTrainer, create_agent_world_setup

def create_learning_curriculum():
    """Create progressive difficulty curriculum."""
    return [
        {
            'name': 'Sensory Foundation',
            'episodes': 5,
            'steps': 200,
            'world_config': {'visionWidth': 4, 'visionHeight': 4},
            'features': {'RewardModulation': True, 'Curiosity': True}
        },
        {
            'name': 'Motor Coordination',
            'episodes': 8,
            'steps': 400,
            'world_config': {'visionWidth': 8, 'visionHeight': 8},
            'features': {'RewardModulation': True, 'Curiosity': True, 'StructuralPlasticity': True}
        },
        {
            'name': 'Complex Navigation',
            'episodes': 10,
            'steps': 600,
            'world_config': {'visionWidth': 16, 'visionHeight': 16},
            'features': {'RewardModulation': True, 'Curiosity': True, 'Development': True}
        }
    ]

# Run curriculum training
system = create_agent_world_setup(vision_width=16, vision_height=16)
trainer = AgentTrainer(system['brain'], system['world'], system['agent'])

print("Starting curriculum learning...")
curriculum_results = trainer.train_with_curriculum(
    create_learning_curriculum(), 
    base_steps=1000
)

print(f"Curriculum training complete! Final reward: {curriculum_results['final_performance']['mean_reward']:.3f}")
```

#### Example 3: Simulation Profiling

```python
import time
import pynlm
from pynlm_advanced import SimulationProfiler, create_agent_world_setup

# Create system for profiling
system = create_agent_world_setup(vision_width=8, vision_height=8)
brain = system['brain']
world = system['world']
agent = system['agent']

# Create profiler and run simulation with profiling
profiler = SimulationProfiler(brain, world, agent)

print("Running simulation with profiling...")
start_time = time.time()

for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Generate performance report
profile_data = profiler.profile_simulation(brain, world, agent, 200, log_interval=50)
report = profiler.generate_performance_report(profile_data)
print(report)
```

### Module Summary

All advanced features are contained in the `pynlm_advanced` module, which provides:

| Class/Function | Purpose | Key Features |
|----------------|---------|-------------|
| `SimulationPreset` | Enum of predefined configurations | 7 research-specific presets |
| `create_preset_brain()` | Create brain with preset config | Easy setup, configurable overrides |
| `create_agent_world_setup()` | Complete system setup | Vision, motor, world configuration |
| `BrainVisualizer` | Neural activity analysis | Pattern detection, statistics |
| `AgentTrainer` | Advanced training utilities | Custom rewards, curriculum, exploration |
| `SimulationProfiler` | Performance measurement | Timing, resource analysis |
| `setup_debug_monitor()` | Debugging and monitoring | Logging, real-time monitoring |

### Relationship to Core API

The advanced features build upon the core pynlm API:

- All advanced functions use the same core classes (`Brain`, `SimpleWorld`, `AgentBrain`)
- Advanced features provide enhanced interfaces and utilities for these core classes
- The core API remains unchanged and fully backward compatible
- All advanced features work seamlessly with existing NLM code

### Further Reading

- [Phase 6 Final Integration](README.md#phase-6-final-integration) - Understand the core system integration
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - Learn about the NLM brain architecture
- [SCIENCE.md](docs/SCIENCE.md) - Explore the scientific background and principles

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
