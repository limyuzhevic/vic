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

# Advanced Brain Operations
brain.injectCurrent(neuron_id, current)                    # Inject current to specific neuron
brain.injectCurrentToNeurons(neuron_type, current)         # Inject current to neuron type
brain.addRegion(name)                                      # Add new brain region
brain.getRegion(region_id)                                 # Get region by ID
brain.getTotalSpikeCount()                                 # Get total spike count
brain.getAverageFiringRate()                              # Get average firing rate
brain.getExcitationInhibitionRatio()                      # Get E/I balance
brain.getDevelopmentalStage()                            # Get current development stage
brain.setDevelopmentalStage(stage)                       # Set development stage

# Memory Systems Access
brain.getWorkingMemory()           # Access working memory
brain.getEpisodicMemory()          # Access episodic memory
brain.getAssociativeMemory()       # Access associative memory
brain.getPredictionSystem()        # Access prediction system
brain.getPlanner()                 # Access neural planner
brain.getConceptFormation()        # Access concept formation
brain.getAttention()               # Access attention system

# Neuromodulation Systems
brain.getDopamine()                # Access dopamine system
brain.getCuriosity()               # Access curiosity system
brain.getNovelty()                 # Access novelty detection
brain.getPredictionErrorSignal()   # Access prediction error

# State Management
brain.save("checkpoint.bin")       # Save brain state
brain.load("checkpoint.bin")       # Load brain state
brain.logStatus()                  # Log brain status
brain.develop()                    # Update development
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

# Advanced Configuration Options
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("development.initial_plasticity", 1.0)
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

### Advanced Learning and Memory Example

```python
import pynlm
import time

def advanced_learning_example():
    """Advanced example demonstrating memory systems and learning"""
    
    # 1. Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent and enable all subsystems
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 3. Configure memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    # 4. Run learning episodes
    episodes_completed = 0
    for episode in range(10):
        print(f"=== Episode {episode + 1} ===")
        
        # Simulate with simple world
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        agent.initialize(world)
        
        # Run episode steps
        for step in range(200):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            if step % 50 == 0:
                print(f"  Step {step}: Firing={brain.getFiringNeuronCount()}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}")
        
        episodes_completed += 1
        
        # 5. Analyze learning progress
        print(f"  Episode completed!")
        print(f"  Memory traces: {working_memory.getActiveTraces() if working_memory else 'N/A'}")
        print(f"  Episodes stored: {episodic_memory.getEpisodeCount() if episodic_memory else 'N/A'}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    # 6. Save learned brain state
    brain.save("advanced_learning_checkpoint.bin")
    print("Advanced learning complete! Brain saved to advanced_learning_checkpoint.bin")
    return episodes_completed

# Run advanced example
advanced_learning_example()
```

### Development and Plasticity Example

```python
import pynlm

def developmental_plasticity_example():
    """Example showing developmental changes and plasticity effects"""
    
    # 1. Create brain with plasticity enabled
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    
    print("=== Developmental Plasticity Study ===")
    print("Initial state:")
    brain.logStatus()
    
    # 3. Simulate developmental progression
    for day in range(10):  # Simulate 10 "days"
        # Simulate one "day" of experience
        for step in range(100):
            brain.step(step)
            
            # Apply reward to drive plasticity
            reward = 1.0 if step % 20 == 0 else 0.1
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(1.0)  # 1 "day"
        
        # Record developmental stage
        stage = brain.getDevelopmentalStage()
        plasticity_modifier = agent.getBrain().getDevelopmentSystem().getPlasticityModifier()
        
        print(f"Day {day + 1}: Stage={stage}, Plasticity={plasticity_modifier:.3f}")
        
        # Log structural plasticity effects
        sp = brain.getStructuralPlasticity()
        if sp:
            print(f"  Synapses: {brain.getTotalSynapseCount()}, "
                  f"Synaptogenesis rate: {sp.getSynaptogenesisRate():.6f}, "
                  f"Pruning rate: {sp.getPruningRate():.6f}")
    
    # 4. Compare before/after development
    print("\n=== Developmental Effects Analysis ===")
    print("Final state:")
    brain.logStatus()
    
    # Save developed brain
    brain.save("developed_brain.bin")
    print("Developmental study complete! Brain saved to developed_brain.bin")

developmental_plasticity_example()
```

### Performance and Benchmarking Example

```python
import pynlm
import time
import statistics

def performance_benchmark(brain_size=1000, simulation_steps=1000, num_runs=5):
    """Benchmark brain performance with different configurations"""
    
    print(f"=== Performance Benchmark ===")
    print(f"Brain size: {brain_size} neurons, Steps: {simulation_steps}, Runs: {num_runs}")
    print()
    
    times = []
    firing_rates = []
    spike_counts = []
    
    for run in range(num_runs):
        # Create brain with specified size
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", brain_size)
        config.set("region_count", 2)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time the simulation
        start_time = time.time()
        
        for step in range(simulation_steps):
            brain.step(step)
        
        end_time = time.time()
        elapsed = end_time - start_time
        times.append(elapsed)
        firing_rates.append(brain.getAverageFiringRate())
        spike_counts.append(brain.getTotalSpikeCount())
        
        print(f"Run {run + 1}: {elapsed:.3f}s, "
              f"Firing rate: {brain.getAverageFiringRate():.3f}, "
              f"Total spikes: {brain.getTotalSpikeCount()}")
    
    # Calculate statistics
    print()
    print("=== Performance Statistics ===")
    print(f"Time per step (average): {statistics.mean(times)/simulation_steps:.6f}s")
    print(f"Time per step (median): {statistics.median(times)/simulation_steps:.6f}s")
    print(f"Time per step (min): {min(times)/simulation_steps:.6f}s")
    print(f"Time per step (max): {max(times)/simulation_steps:.6f}s")
    print(f"Time coefficient of variation: {statistics.stdev(times)/statistics.mean(times):.3f}")
    print()
    print(f"Average firing rate: {statistics.mean(firing_rates):.3f} ± {statistics.stdev(firing_rates):.3f}")
    print(f"Average spike count: {statistics.mean(spike_counts):.0f} ± {statistics.stdev(spike_counts):.0f}")
    
    return {
        'times': times,
        'firing_rates': firing_rates,
        'spike_counts': spike_counts,
        'mean_time_per_step': statistics.mean(times)/simulation_steps,
        'std_time_per_step': statistics.stdev(times)/(simulation_steps * statistics.mean(times))
    }

# Run benchmark with different brain sizes
results_small = performance_benchmark(brain_size=500, simulation_steps=500, num_runs=3)
results_medium = performance_benchmark(brain_size=1000, simulation_steps=1000, num_runs=3)
results_large = performance_benchmark(brain_size=2000, simulation_steps=1000, num_runs=3)

print("\n=== Scaling Analysis ===")
print(f"Small brain (500 neurons): {results_small['mean_time_per_step']*1000:.3f}ms per step")
print(f"Medium brain (1000 neurons): {results_medium['mean_time_per_step']*1000:.3f}ms per step")
print(f"Large brain (2000 neurons): {results_large['mean_time_per_step']*1000:.3f}ms per step")
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
