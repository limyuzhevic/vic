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

## Part 6: NLM Phase 6 Enhancements (NEW!)

### Overview

Phase 6 represents the final integration of all NLM systems into a coherent artificial brain. This major enhancement includes:

#### 1. **Complete Memory Integration**
- **Working Memory**: Transient active information with persistent activity
- **Episodic Memory**: Experience storage with neural pattern encoding
- **Associative Memory**: Pattern associations with Hebbian connectivity

#### 2. **Advanced Neuromodulation Systems**
- **Dopamine**: Reward processing and reinforcement learning
- **Curiosity**: Exploration motivation and intrinsic reward
- **Novelty**: Change detection and adaptation drive

#### 3. **Enhanced Learning Mechanisms**
- **STDP**: Spike-Timing-Dependent Plasticity with precise timing
- **Hebbian Learning**: Co-activation strengthening
- **Structural Plasticity**: Growth and pruning of connections
- **Reward-Modulated Plasticity**: Credit assignment for delayed rewards

#### 4. **Cognition and Prediction Systems**
- **Neural Planner**: Action sequence evaluation
- **Concept Formation**: Pattern discovery and categorization
- **Prediction System**: Forward models and error computation

#### 5. **Development and Lifespan Learning**
- **Developmental Stages**: Initial to aging
- **Critical Period**: Sensitive learning windows
- **Maturation**: Synaptic refinement and optimization

#### 6. **Performance Optimizations**
- **Memory Pools**: Lock-free allocation for improved performance
- **Event Queues**: Efficient spike and event processing
- **SIMD Support**: Vectorized operations for parallel processing
- **Cache Optimization**: Better spatial and temporal locality

### Key Improvements in Python API

#### Enhanced Brain Interface
```python
# Create brain with configuration
brain = pynlm.createBrain(config)

# Advanced brain statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

#### Memory Systems Access
```python
# Working memory (transient active information)
wm = brain.getWorkingMemory()
if wm:
    print(f"Working memory traces: {wm.getActiveTraces()}")
    print(f"Memory capacity: {wm.getCapacity()}")
    print(f"Memory activity level: {wm.getMemoryActivity():.3f}")

# Episodic memory (experience storage)
em = brain.getEpisodicMemory()
if em:
    print(f"Episodic memory episodes: {em.getEpisodeCount()}")
    print(f"Average reward: {em.getAverageReward():.3f}")
    print(f"Replay enabled: {em.isReplayEnabled()}")

# Associative memory (pattern associations)
am = brain.getAssociativeMemory()
if am:
    print(f"Associative memory associations: {am.getAssociationCount()}")
```

#### Neuromodulation Systems
```python
# Create agent with world
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all neuromodulation systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Access neuromodulation levels
print(f"Dopamine level: {agent.getNeuromodulationLevel():.3f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Developmental stage: {agent.getDevelopmentalStage()}")
```

### Enhanced Agent Simulation

#### Basic Agent (Same as before, but with improved error handling)
```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

#### Advanced Agent with All Features (NEW!)
```python
import pynlm

# Setup with enhanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)  # Larger network
config.set("brain.synapse_density", 0.15)  # More connections
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run advanced simulation with all systems
for step in range(1000):
    # 1. Update world
    world.update(0.05)
    
    # 2. Get sensory input
    percept = world.getSensoryPercept()
    
    # 3. Process sensory input in brain
    agent.processSensoryInput(percept)
    
    # 4. Run brain step with timestamp
    brain.step(step, step * 0.05)
    
    # 5. Get action from brain
    action = agent.decodeMotorCommand()
    
    # 6. Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # 7. Apply all neuromodulation systems
    reward = 0.1 if action.type == pynlm.ActionType.Interact else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # 8. Update development system
    agent.updateDevelopment(0.05)

# Print comprehensive statistics
print(f"Simulation completed!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Developmental stage: {agent.getDevelopmentalStage()}")
```

### New Convenience Functions

#### Advanced Convenience Functions (NEW!)
```python
import pynlm
import time

# 1. Run simple simulation and return statistics
def run_simple_simulation(brain, steps):
    if not brain:
        raise ValueError("Brain cannot be null")
    if not brain.initialize():
        raise RuntimeError("Failed to initialize brain")
    for i in range(steps):
        brain.step(i)
    return brain.getAverageFiringRate()

# 2. Run complete agent simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Run complete simulation and get results
result = pynlm.run_agent_simulation(brain, world, agent, 1000)
print(f"Simulation results: {result}")

# 3. Benchmark brain performance
def benchmark_brain(brain, steps):
    if not brain:
        raise ValueError("Brain cannot be null")
    
    start_time = time.time()
    if not brain.initialize():
        raise RuntimeError("Failed to initialize brain for benchmarking")
    
    for step in range(steps):
        brain.step(step, step * 0.001)
    
    end_time = time.time()
    duration = end_time - start_time
    
    return {
        'avg_firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount(),
        'duration': duration,
        'steps_per_sec': steps / duration if duration > 0 else 0,
        'microseconds_per_step': duration / steps * 1000000 if steps > 0 else 0
    }

# 4. Create brain from configuration file
brain = pynlm.create_brain_from_config("my_config.json")

# 5. Save and restore brain checkpoints
brain.save("checkpoint1.bin")
brain2 = pynlm.load_brain_checkpoint("checkpoint1.bin")
```

### Memory Integration Features

#### Working Memory
```python
# Check working memory availability
wm = brain.getWorkingMemory()
if wm:
    print(f"Working memory active traces: {wm.getActiveTraces()}")
    print(f"Working memory capacity: {wm.getCapacity()}")
    print(f"Working memory activity level: {wm.getMemoryActivity():.3f}")
```

#### Episodic Memory
```python
# Check episodic memory functionality
em = brain.getEpisodicMemory()
if em:
    print(f"Episodic memory episodes: {em.getEpisodeCount()}")
    print(f"Episodic memory average reward: {em.getAverageReward():.3f}")
    print(f"Episodic memory replay enabled: {em.isReplayEnabled()}")
```

#### Memory Replay
```python
# Replay stored memories
em = brain.getEpisodicMemory()
if em and em.getEpisodeCount() > 0:
    episodes = em.getEpisodesForReplay(5)
    print(f"Ready to replay {len(episodes)} episodes")
    for episode in episodes:
        em.replayEpisode(episode)
```

### Development and Growth

#### Developmental Stages
```python
# Check and manage developmental stages
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")

# Simulate development
for step in range(500):
    brain.step(step, step * 0.001)
    
    # Progress through developmental stages
    if step % 100 == 0:
        stage = brain.getDevelopmentalStage()
        print(f"Step {step}: {stage}")
```

### Performance Monitoring

#### System Health Check
```python
# Comprehensive system health monitoring
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print("\n=== SYSTEM HEALTH CHECK ===")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spike count: {brain.getTotalSpikeCount()}")
```

### Error Handling and Validation

#### Enhanced Error Handling
```python
import pynlm

# Create and initialize components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

agent = pynlm.createAgentBrain(brain)

# This will raise exceptions if any component is invalid
agent.initialize(world)

# Check agent status
if agent.isRewardModulationEnabled():
    print("✓ Reward modulation is enabled")
else:
    print("✗ Reward modulation is disabled")

if agent.isDevelopmentEnabled():
    print("✓ Development is enabled")
else:
    print("✗ Development is disabled")
```

#### Configuration Validation
```python
# Validate configuration before use
config = pynlm.createDefaultConfig()

# Check if configuration has required keys
if config.has("brain.neuron_count"):
    neurons = config.get("brain.neuron_count")
    print(f"Configuration: {neurons} neurons")

# Create brain with validated configuration
brain = pynlm.createBrain(config)
if not brain.initialize():
    print("Error: Failed to initialize brain")
    exit(1)
```

---

## Part 7: Phase 6 Integration Testing (NEW!)

### Integration Test Overview

Phase 6 includes comprehensive integration tests to verify that all systems work together correctly:

1. **Integration Verification**: Tests basic connectivity between systems
2. **Memory Integration**: Verifies memory systems are properly connected
3. **Neuromodulation Integration**: Tests neuromodulation systems are functional
4. **Checkpoint System**: Validates save/load functionality
5. **Replay System**: Tests memory replay capabilities

### Running Integration Tests

```bash
# Build with tests enabled
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run Phase 6 integration demo
./nlm_phase6_demo

# Run unit tests
./nlm_test

# Run Phase 3 demo (original demo, still available)
./nlm_phase3_demo

# Run Phase 4 demo (original demo, still available)
./nlm_phase4_demo
```

### Integration Test Results

The integration tests verify:

- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction**: Forward models and error computation
- **Development**: Lifespan learning and maturation
- **Checkpointing**: Save/load functionality
- **Replay**: Memory consolidation and retrieval

---

## Part 8: Building and Installation (UPDATED)

### Building with Performance Optimizations

```bash
# Configure with performance optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release \
          -DNLM_PERFORMANCE_ENABLED=ON \
          -DNLM_SIMD_AVX2=ON \
          -DNLM_THREAD_COUNT=4

# Build with optimizations
make -j4
```

### Python Installation with Performance

```bash
# Install Python bindings with performance features
pip install . --config-settings=cmake.define.NLM_PERFORMANCE_ENABLED=ON

# Or install in development mode
pip install -e . --config-settings=cmake.define.NLM_SIMD_AVX2=ON
```

### Build Outputs (Enhanced)

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable (Phase 6) |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | **NEW** Phase 6 integration test |
| `nlm_test` | Executable | Unit test suite |

---

## Part 9: Advanced Configuration Options (ENHANCED)

### Performance Configuration

```bash
# Configure performance features
cmake .. -DCMAKE_BUILD_TYPE=Release \
          -DNLM_PERFORMANCE_ENABLED=ON \
          -DNLM_USE_MEMORY_POOL=ON \
          -DNLM_USE_SOA=ON \
          -DNLM_USE_EVENT_QUEUE=ON \
          -DNLM_USE_SIMD=ON \
          -DNLM_THREAD_COUNT=4
```

### Runtime Configuration

```python
import pynlm

# Configure brain for performance
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)  # Larger network
config.set("brain.synapse_density", 0.2)  # More connections
config.set("plasticity.stdp.enable", True)  # Enable STDP
config.set("plasticity.hebbian.enable", True)  # Enable Hebbian
config.set("plasticity.structural.enable", True)  # Enable structural plasticity
config.set("neuromod.curiosity.enable", True)  # Enable curiosity
config.set("neuromod.novelty.enable", True)  # Enable novelty detection

# Create optimized brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### World Configuration

```python
import pynlm

# Configure world for optimal performance
world = pynlm.createSimpleWorld()
world.configure(
    width=50,      # Larger world
    height=50,     # More space for exploration
    visionWidth=16,  # Better vision range
    visionHeight=16
)

# Optimize world parameters
world.setMaxEnergy(200.0)
world.setEnergyDecayRate(0.005)
world.setRandomSeed(12345)  # Reproducible results
world.reset()
```

---

## Part 10: Performance and Scalability (NEW!)

### Performance Features

NLM includes comprehensive performance optimizations:

#### 1. **Memory Pool System**
- Lock-free memory allocation for improved performance
- Pre-allocated pools reduce allocation overhead
- Cache-aligned structures for better locality

#### 2. **Event Queue System**
- Ring buffer for immediate spike events
- Time-bucketed delayed spike processing
- Lock-free operations for thread safety

#### 3. **SIMD Vectorization**
- AVX2 and SSE4.1 instruction support
- Vectorized neural computations
- Parallel processing capabilities

#### 4. **Sparse Connectivity**
- Efficient adjacency lists
- Memory-efficient storage for large networks
- Optimized synapse lookup

### Scalability

NLM is designed to scale from:
- **Small networks**: 100-1,000 neurons (real-time simulation)
- **Medium networks**: 1,000-10,000 neurons (real-time with optimizations)
- **Large networks**: 10,000-100,000+ neurons (high-performance with parallel processing)

### Performance Guidelines

#### For Real-time Applications
```python
# Use smaller timesteps and optimized configurations
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("simulation_timestep", 0.01)  # Larger timestep
brain = pynlm.createBrain(config)
brain.initialize()
```

#### For Research and Analysis
```python
# Use larger networks for comprehensive analysis
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10000)
config.set("simulation_timestep", 0.001)  # Precise timesteps
brain = pynlm.createBrain(config)
brain.initialize()
```

#### For Long-running Simulations
```python
# Use checkpointing for long simulations
for step in range(100000):
    brain.step(step)
    
    if step % 10000 == 0:
        brain.save(f"checkpoint_{step}.bin")
        print(f"Checkpoint saved at step {step}")
```

---

## Part 11: Scientific Background and Theory (ENHANCED)

### Neural Model

NLM implements the following biological principles:

#### 1. **Leaky Integrate-and-Fire (LIF) Neurons**
- Membrane potential integration with exponential decay
- Spike generation when threshold is reached
- Refractory period after spike
- Adaptation for spike-frequency regulation

#### 2. **Spike-Timing-Dependent Plasticity (STDP)**
- Pre-before-post spikes strengthen connections (LTP)
- Post-before-pre spikes weaken connections (LTD)
- Time-dependent plasticity rules

#### 3. **Hebbian Learning**
- "Neurons that fire together, wire together"
- Co-activation strengthening
- Activity-dependent synaptic modification

#### 4. **Structural Plasticity**
- Growth of new synaptic connections
- Pruning of weak connections
- Synaptogenesis and retraction

### Memory Systems

#### 1. **Working Memory**
- Persistent neural activity for information maintenance
- Competitive selection among memories
- Dynamic updating and refreshing

#### 2. **Episodic Memory**
- Neural pattern storage of experiences
- Temporal ordering and context encoding
- Pattern completion for retrieval

#### 3. **Associative Memory**
- Relational encoding between experiences
- Spreading activation for inference
- Pattern completion and completion

### Neuromodulation

#### 1. **Dopamine**
- Reward prediction error signaling
- Reinforcement learning modulation
- Incentive motivation

#### 2. **Curiosity**
- Intrinsic motivation for exploration
- Novelty detection and reward
- Information seeking behavior

#### 3. **Novelty**
- Change detection and adaptation
- Arousal and attention modulation
- Learning rate adjustment

### Developmental Principles

#### 1. **Critical Periods**
- Sensitive windows for learning
- Plasticity reduction with age
- Experience-dependent maturation

#### 2. **Synaptic Refinement**
- Pruning of weak connections
- Strengthening of important connections
- Optimization of neural circuits

#### 3. **Lifespan Learning**
- Continuous adaptation throughout life
- Age-dependent plasticity
- Cumulative learning and memory

---

## Part 12: Future Directions and Extensions (NEW!)

### Research Applications

#### 1. **Cognitive Modeling**
- Implement complex cognitive tasks
- Study decision-making processes
- Model memory formation and retrieval

#### 2. **Neurological Disorders**
- Simulate disease states
- Study abnormal neural dynamics
- Test therapeutic interventions

#### 3. **Machine Learning Integration**
- Hybrid neural-symbolic systems
- Bio-inspired artificial intelligence
- Adaptive learning algorithms

### Technical Extensions

#### 1. **Hardware Acceleration**
- GPU implementation of neural dynamics
- Neuromorphic chip designs
- Specialized hardware accelerators

#### 2. **Distributed Computing**
- Multi-agent simulations
- Distributed memory systems
- Parallel processing of large networks

#### 3. **Virtual Reality Integration**
- Immersive neural simulation environments
- Real-time interaction with simulated brains
- Educational and training applications

### Community Resources

#### Documentation
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - Detailed system architecture
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background and theory
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions and protocols
- [LIMITATIONS.md](docs/LIMITATIONS.md) - Current limitations and future directions

#### Software Resources
- GitHub repository with examples and tutorials
- Community forums for discussion and collaboration
- Issue tracking for bug reports and feature requests
- Regular updates and new releases

---

## Conclusion

NLM Phase 6 represents a comprehensive advancement in brain-inspired neural simulation. Key improvements include:

### 1. **Enhanced Memory Systems**
- Complete integration of working, episodic, and associative memory
- Neural pattern storage and retrieval mechanisms
- Replay and consolidation capabilities

### 2. **Advanced Neuromodulation**
- Comprehensive reward and intrinsic motivation systems
- Novelty detection and curiosity-driven exploration
- Real-time adaptation and learning

### 3. **Performance Optimizations**
- Memory pools and event queues for efficient processing
- SIMD vectorization and cache optimization
- Scalability from small to very large networks

### 4. **Enhanced Python Interface**
- Comprehensive API with convenience functions
- Improved error handling and validation
- Extensive documentation and examples

### 5. **Scientific Accuracy**
- Biologically plausible neuron models
- Real neural plasticity mechanisms
- Developmental and aging processes

NLM provides a powerful platform for neural research, education, and applications. Whether you're a beginner learning about neural systems or a researcher exploring advanced brain models, NLM offers the tools and capabilities you need.

**For the latest updates and documentation, visit the NLM project website and contribute to the growing community of neural simulation researchers and practitioners!**
