# NLM (Neural Learning Machine) - Documentation

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Quick Start (Beginner)

If you're new to NLM, follow these basic steps:

```python
import pynlm

# Create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simulation
for i in range(100):
    brain.step(i)

print(f"Your brain thought {brain.getTotalSpikeCount()} times")
```

## Complete Agent Example

Here's a full example of using all three main components:

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

## Building the C++ Project

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

## Installing the Python Library

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

## Configuration Options

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

### Default Configuration

The default configuration file is at `configs/default.cfg`:

```ini
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

## Running Demos

### Phase 6 Integration Demo

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Phase 3 Demo

```bash
./nlm_phase3_demo
```

### Phase 4 Demo

```bash
./nlm_phase4_demo
```

## Python API Reference

### Brain Class

| Method | Description |
|--------|-------------|
| `brain = pynlm.createBrain(config)` | Create a new brain with configuration |
| `brain.initialize()` | Initialize the brain with configuration |
| `brain.step(step)` | Perform a simulation step |
| `brain.step(step, time)` | Perform a simulation step with timestamp |
| `brain.reset()` | Reset brain state |
| `brain.receiveSensoryInput(input)` | Inject sensory input into the brain |
| `action = brain.produceAction()` | Produce motor action based on neural activity |
| `brain.save(filepath)` | Save brain state to file |
| `brain.load(filepath)` | Load brain state from file |
| `brain.getTotalNeuronCount()` | Get total neuron count |
| `brain.getTotalSynapseCount()` | Get total synapse count |
| `brain.getActiveNeuronCount()` | Get count of active neurons |
| `brain.getFiringNeuronCount()` | Get count of currently firing neurons |
| `brain.getAverageFiringRate()` | Get average firing rate |
| `brain.getExcitationInhibitionRatio()` | Get excitation/inhibition balance |
| `brain.getTotalSpikeCount()` | Get total spike count |
| `brain.getDevelopmentalStage()` | Get current developmental stage |

### Config Class

| Method | Description |
|--------|-------------|
| `config = pynlm.createDefaultConfig()` | Create a default configuration |
| `config.has(key)` | Check if a configuration key exists |
| `config.set(key, value)` | Set configuration value |
| `config.get(key)` | Get configuration value |
| `config.loadFromFile(filepath)` | Load configuration from JSON file |
| `config.saveToFile(filepath)` | Save configuration to JSON file |
| `config.getKeys()` | Get all configuration keys |
| `config.clear()` | Clear all configuration entries |

### AgentBrain Class

| Method | Description |
|--------|-------------|
| `agent = pynlm.createAgentBrain(brain)` | Create a new agent brain interface |
| `agent.initialize(world)` | Initialize with world |
| `agent.getSensoryInputSize()` | Get expected sensory input size |
| `agent.getMotorOutputSize()` | Get expected motor output size |
| `agent.processSensoryInput(percept)` | Process sensory percept and inject into brain |
| `agent.decodeMotorCommand()` | Decode brain motor activity into motor command |
| `agent.applyRewardModulation(reward, predicted_reward)` | Apply reward-based neuromodulation |
| `agent.updateDevelopment(timestep)` | Update development system |
| `agent.getDevelopmentalStage()` | Get current developmental stage |
| `agent.getNeuromodulationLevel()` | Get current neuromodulation level |
| `agent.getCuriosityLevel()` | Get curiosity level |
| `agent.getNoveltyLevel()` | Get novelty level |
| `agent.getPredictionError()` | Get prediction error |
| `agent.reset()` | Reset agent for new episode |
| `agent.enableRewardModulation(enable)` | Enable/disable reward modulation |
| `agent.enableStructuralPlasticity(enable)` | Enable/disable structural plasticity |
| `agent.enableDevelopment(enable)` | Enable/disable development |
| `agent.enableCuriosity(enable)` | Enable/disable curiosity |

### SimpleWorld Class

| Method | Description |
|--------|-------------|
| `world = pynlm.createSimpleWorld()` | Create a new simple world |
| `world.configure(width, height, visionWidth, visionHeight)` | Configure world dimensions |
| `world.reset()` | Reset world to initial state |
| `world.setAgentStart(x, y)` | Set agent starting position |
| `world.update(timestep)` | Update world state |
| `world.applyMotorCommand(cmd, currentTime)` | Apply motor command to world |
| `world.getSensoryPercept()` | Get current sensory percept |
| `world.getAgentBody()` | Get agent body state |
| `world.addObject(obj)` | Add object to world |
| `world.removeObject(x, y)` | Remove object from world |
| `world.isValidPosition(x, y)` | Check if position is valid |
| `world.getWidth()`, `world.getHeight()` | Get world dimensions |
| `world.getSimulationTime()` | Get current simulation time |
| `world.setRandomSeed(seed)` | Set random seed |
| `world.getRandomSeed()` | Get random seed |

### Enumerations

| Enumeration | Values |
|-------------|--------|
| `NeuronType` | `Excitatory`, `Inhibitory`, `Modulatory`, `Sensory`, `Motor`, `Internal` |
| `DevelopmentalStage` | `Initial`, `CriticalPeriod`, `Maturation`, `Adult`, `Aging` |
| `ActionType` | `MoveForward`, `MoveBackward`, `TurnLeft`, `TurnRight`, `Interact`, `Eat`, `Drink`, `Rest`, `Wait`, `Custom` |
| `MotorCommand` | `MoveForward`, `MoveBackward`, `TurnLeft`, `TurnRight`, `LookLeft`, `LookRight`, `Interact`, `Wait` |
| `WorldObjectType` | `Empty`, `Resource`, `Hazard`, `Wall`, `Marker` |

## Running Tests

### Running All Tests

```bash
cd build
ctest --output-on-failure
```

### Running Specific Test Suites

You can run individual test executables if they were built:

```bash
# Unit tests
./nlm_test

# Performance benchmarks
./src/benchmark.cpp
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

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

## Architecture

NLM implements a closed-loop brain system architecture:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

## Project Status

Current Phase: **6 - FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## License

MIT

## Authors

Research project — See docs for scientific background.