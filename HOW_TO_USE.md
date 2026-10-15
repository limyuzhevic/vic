# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

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
| `nlm` | Executable | Main simulation executable (Phase 6) |
| `nlm_phase6_demo` | Executable | Phase 6 integration demonstration |
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
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

## Part 3: Using the Python Library

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters for Phase 6
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("memory.working_memory_capacity", 1000)
config.set("memory.episodic_memory_capacity", 100)
config.set("development.enable_developmental_stages", True)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Working memory capacity: {brain.getWorkingMemory().getCapacity()}")
```

#### Running a Simulation

```python
import pynlm
import time

# Create brain with Phase 6 configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("enable_development", True)
config.set("enable_curiosity", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 5000 steps
start_time = time.time()
for step in range(5000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 500 == 0:
        firing_rate = brain.getAverageFiringRate()
        curiosity = brain.getCuriosityLevel()
        developmental_stage = brain.getDevelopmentalStage()
        print(f"Step {step}: Firing rate: {firing_rate:.3f}, "
              f"Curiosity: {curiosity:.3f}, "
              f"Stage: {developmental_stage}")

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
```

#### Creating a Simple World

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions for Phase 6 experiments
world.configure(
    width=50,
    height=50,
    visionWidth=10,
    visionHeight=10
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(25.0, 25.0)

# Run world update
world.update(timestep=0.1)
```

### Complete Agent Example (Phase 6 Integration)

```python
import pynlm
import time

def run_complete_experiment(num_steps=5000):
    """Run a complete Phase 6 experiment with all integrated systems."""
    
    # 1. Create Phase 6 configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Configure for Phase 6 experiments
    config.set("neuron_count", 1000)
    config.set("enable_development", True)
    config.set("enable_curiosity", True)
    config.set("enable_reward_modulation", True)
    config.set("enable_structural_plasticity", True)
    config.set("memory.working_memory_capacity", 2000)
    config.set("memory.episodic_memory_capacity", 500)
    config.set("replay.enable_replay", True)
    config.set("replay.consolidation_interval", 1000)
    
    # 3. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 4. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 5. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(25.0, 25.0)
    
    # 6. Initialize agent with world
    agent.initialize(world)
    
    # 7. Enable all Phase 6 subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 8. Run simulation loop
    start_time = time.time()
    reward_accumulator = 0.0
    curiosity_levels = []
    novelty_levels = []
    
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
        
        # Collect metrics for analysis
        reward_accumulator += reward
        curiosity_levels.append(agent.getCuriosityLevel())
        novelty_levels.append(agent.getNoveltyLevel())
        
        # Print progress every 500 steps
        if step % 500 == 0:
            print(f"Step {step}:")
            print(f"  Reward: {reward:.3f}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
            print(f"  Working memory active traces: {brain.getWorkingMemory().getActiveTraces()}")
            print(f"  Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
    
    end_time = time.time()
    
    # Print final summary
    print("\n=== Experiment Results ===")
    print(f"Total simulation time: {end_time - start_time:.2f} seconds")
    print(f"Average reward per step: {reward_accumulator / num_steps:.3f}")
    print(f"Average curiosity level: {sum(curiosity_levels) / len(curiosity_levels):.3f}")
    print(f"Average novelty level: {sum(novelty_levels) / len(novelty_levels):.3f}")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Final working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
    print(f"Final episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
    
    return brain, agent, world

# Run the experiment
if __name__ == "__main__":
    run_complete_experiment(5000)
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

### Memory Configuration (Phase 6)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working_memory_capacity` | int | 1000 | Capacity of working memory |
| `memory.episodic_memory_capacity` | int | 100 | Capacity of episodic memory |
| `memory.associative_memory_enabled` | bool | true | Enable associative memory |
| `memory.replay.enable` | bool | true | Enable memory replay |
| `memory.replay.consolidation_interval` | int | 1000 | Steps between consolidation events |

### Development Configuration (Phase 6)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.enable_developmental_stages` | bool | true | Enable developmental stages |
| `development.initial_stage` | string | "CriticalPeriod" | Starting developmental stage |
| `development.stage_duration` | int | 100 | Duration of each developmental stage |
| `development.synaptogenesis_rate` | float | 0.001 | Rate of synapse formation |
| `development.pruning_rate` | float | 0.0001 | Rate of synapse pruning |
| `development.plasticity_modulation` | bool | true | Enable plasticity modulation by development |

### Prediction Configuration (Phase 6)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction.enable_prediction` | bool | true | Enable prediction system |
| `prediction.prediction_window` | int | 50 | Number of steps to predict ahead |
| `prediction.confidence_threshold` | float | 0.7 | Confidence threshold for predictions |
| `prediction.use_prediction_error` | bool | true | Use prediction error for learning |

## Part 5: Example Scripts

### Minimal Phase 6 Example

```python
import pynlm

# Create and initialize with Phase 6 settings
config = pynlm.createDefaultConfig()
config.set("enable_development", True)
config.set("enable_curiosity", True)
config.set("enable_structural_plasticity", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)
    
    # Log key metrics
    if i % 10 == 0:
        print(f"Step {i}: "
              f"Firing: {brain.getFiringNeuronCount()}, "
              f"Curiosity: {brain.getCuriosityLevel():.3f}, "
              f"Stage: {brain.getDevelopmentalStage()}")

print("Phase 6 simulation complete!")
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
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

## Part 6: Command Line Interface

The NLM project includes a command-line interface for running simulations and experiments:

```bash
# Show help
./nlm --help

# Run Phase 6 demo
./nlm --demo

# Run integration tests
./nlm --tests

# Run with custom configuration
./nlm --config=myconfig.cfg --verbose

# Run benchmark
./nlm --benchmark

# Show available configurations
./nlm --list-configs
```

## Part 7: Troubleshooting

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
- Verify configuration parameters are within valid ranges

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

## Phase 6 Integration

Phase 6 represents the final integration of all neural systems:

1. **Memory Integration**: Working memory, episodic memory, and associative memory are all connected and functional
2. **Neuromodulation Integration**: Dopamine, curiosity, novelty, and prediction error systems are integrated
3. **Prediction System**: Integrated into the brain loop for adaptive behavior
4. **Cognition Systems**: Neural planning, concept formation, and attention are connected
5. **Development Integration**: Developmental stages affect plasticity rates and system behavior
6. **Persistence**: Checkpoint save/load and state persistence are working
7. **Replay and Consolidation**: Memory replay during simulation and consolidation of important episodes

The Phase 6 system represents a complete artificial brain that can process information, learn from experience, and adapt its behavior through multiple integrated mechanisms.
