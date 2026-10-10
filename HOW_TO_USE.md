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

### Advanced Phase 6 Configuration Examples

This section provides advanced configuration examples for Phase 6 systems integration.

#### 1. Memory-Optimized Configuration

```python
import pynlm

def create_memory_optimized_config():
    """Create a configuration optimized for memory-intensive tasks."""
    
    config = pynlm.createDefaultConfig()
    
    # Increase neural resources for memory-intensive learning
    config.set("brain.neuron_count", 3000)
    config.set("brain.synapse_density", 0.2)
    config.set("brain.connection_probability", 0.08)
    
    # Enhanced working memory configuration
    config.set("working_memory.capacity_neuron_count", 800)
    config.set("working_memory.persistence_strength", 0.8)
    config.set("working_memory.decay_rate", 0.05)
    config.set("working_memory.competition_strength", 1.0)
    
    # Enhanced episodic memory configuration
    config.set("episodic_memory.capture_interval", 5)
    config.set("episodic_memory.consolidation_threshold", 0.7)
    config.set("episodic_memory.replay_frequency", 0.1)
    config.set("episodic_memory.max_episodes", 1000)
    
    # Neuromodulation for memory enhancement
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.dopamine.working_memory_boost", 0.8)
    config.set("neuromod.dopamine.memory_consolidation_strength", 1.2)
    
    return config

# Usage example
memory_config = create_memory_optimized_config()
brain = pynlm.createBrain(memory_config)
brain.initialize()
```

#### 2. High-Performance Phase 6 Configuration

```python
import pynlm

def create_high_performance_config():
    """Create a configuration optimized for high-performance simulation."""
    
    config = pynlm.createDefaultConfig()
    
    # Performance optimizations
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.initial_weight_mean", 0.4)
    config.set("brain.initial_weight_std", 0.08)
    
    # Fast plasticity configuration
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.003)
    config.set("plasticity.stdp.tau_plus", 15.0)
    config.set("plasticity.stdp.tau_minus", 15.0)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.002)
    
    # Neuromodulation for performance
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Fast development stages
    config.set("development.critical_period_duration", 5000)
    config.set("development.maturation_duration", 15000)
    
    # Performance profiling
    config.set("performance.enable_profiling", True)
    config.set("performance.sampling_interval", 1000)
    
    return config

# Usage example
perf_config = create_high_performance_config()
brain = pynlm.createBrain(perf_config)
brain.initialize()
```

#### 3. Development-Optimized Configuration

```python
import pynlm

def create_development_config():
    """Create a configuration optimized for brain development and learning."""
    
    config = pynlm.createDefaultConfig()
    
    # Brain development parameters
    config.set("brain.neuron_count", 2500)
    config.set("brain.synapse_density", 0.18)
    config.set("brain.initial_weight_mean", 0.5)
    config.set("brain.initial_weight_std", 0.12)
    
    # Plasticity that supports development
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.stdp.tau_plus", 25.0)
    config.set("plasticity.stdp.tau_minus", 25.0)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0002)
    config.set("plasticity.structural.pruning_rate", 0.00001)
    
    # Development-specific neuromodulation
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.dopamine.metaplasticity", 0.3)
    
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.base_level", 0.1)
    
    # Developmental stages
    config.set("development.initial_duration", 1000)
    config.set("development.critical_period_duration", 5000)
    config.set("development.maturation_duration", 20000)
    config.set("development.adult_duration", 10000)
    
    # Memory systems for development
    config.set("working_memory.enable_trace_learning", True)
    config.set("episodic_memory.enable_experience_storage", True)
    config.set("episodic_memory.enable_replay", True)
    
    return config

# Usage example
dev_config = create_development_config()
brain = pynlm.createBrain(dev_config)
brain.initialize()
```

---

## Advanced Debugging Examples

This section provides advanced debugging tools and techniques for Phase 6 systems.

### 1. Memory System Debugging

```python
import pynlm

def debug_memory_systems(brain):
    """Debug and analyze memory system performance."""
    
    print("Memory System Debugging")
    print("=" * 30)
    
    # Working memory debugging
    if brain.getWorkingMemory():
        wm = brain.getWorkingMemory()
        
        print("\nWorking Memory Analysis:")
        print(f"  Active neurons: {wm.getMemoryNeurons().size()}")
        print(f"  Memory activity: {wm.getMemoryActivity():.3f}")
        print(f"  Activation strength: {wm.getActivationStrength():.3f}")
        print(f"  Competition level: {wm.getCompetitionLevel():.3f}")
        print(f"  Decay rate: {wm.getDecayRate():.3f}")
        
        # Debug memory traces
        neurons = wm.getMemoryNeurons();
        for neuron_id : neurons {
            activation = wm.getNeuronActivation(neuron_id);
            std::cout << f"    Neuron {neuron_id}: activation = {activation:.3f}" << std::endl;
        }
    
    # Episodic memory debugging
    if brain.getEpisodicMemory():
        episodic = brain.getEpisodicMemory();
        
        print("\nEpisodic Memory Analysis:")
        print(f"  Stored episodes: {episodic.getEpisodeCount()}")
        print(f"  Consolation strength: {episodic.getConsolidationStrength():.3f}")
        print(f"  Replay interval: {episodic.getReplayInterval():.3f}")
        print(f"  Recent episodes: {episodic.getRecentEpisodesCount()}")
        
        # Get recent episodes for analysis
        recent = episodic.getRecentEpisodes(10);
        for episode : recent {
            std::cout << f"    Episode timestamp: {episode.timestamp}" << std::endl;
            std::cout << f"    Active neurons: {episode.activeNeurons.size()}" << std::endl;
            std::cout << f"    Reward signal: {episode.reward}" << std::endl;
        }
    
    # Neuromodulation debugging
    print("\nNeuromodulation Analysis:")
    print(f"  Dopamine level: {brain.getDopamine() : .3f}")
    print(f"  Novelty level: {brain.getNovelty() : .3f}")
    print(f"  Curiosity level: {brain.getCuriosity() : .3f}")
    print(f"  Prediction error: {brain.getPredictionError() : .3f}")
    
    return brain

# Example usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
debug_memory_systems(brain)
```

### 2. Performance Profiling

```python
import pynlm
import time

def profile_brain_performance(brain, duration_steps=1000):
    """Profile brain performance and identify bottlenecks."""
    
    print("Brain Performance Profiling")
    print("=" * 35)
    
    # Step timing measurements
    step_times = []
    memory_usage = []
    firing_rate_samples = []
    
    start_time = time.time()
    
    for step in range(duration_steps):
        step_start = time.time()
        brain.step(step)
        step_end = time.time()
        
        step_time = step_end - step_start
        step_times.append(step_time)
        
        # Collect system metrics
        firing_rate_samples.append(brain.getAverageFiringRate())
        
        # Memory usage (estimated)
        memory_usage.append(brain.getTotalNeuronCount() + brain.getTotalSynapseCount())
        
        # Progress reporting
        if step % 100 == 0:
            avg_step_time = sum(step_times[-100:]) / 100
            print(f"  Step {step}: {avg_step_time*1000:.2f}ms average")
    
    end_time = time.time()
    total_time = end_time - start_time
    
    # Performance analysis
    print("\nPerformance Analysis:")
    print(f"  Total time: {total_time:.2f} seconds")
    print(f"  Total steps: {duration_steps}")
    print(f"  Steps per second: {duration_steps/total_time:.1f}")
    print(f"  Average step time: {total_time/duration_steps*1000:.2f}ms")
    print(f"  Min step time: {min(step_times)*1000:.2f}ms")
    print(f"  Max step time: {max(step_times)*1000:.2f}ms")
    print(f"  Step time std dev: {((sum((t - total_time/duration_steps)**2 for t in step_times) / duration_steps)**0.5)*1000:.2f}ms")
    
    # Firing rate analysis
    print(f"\nFiring Rate Analysis:")
    print(f"  Average firing rate: {sum(firing_rate_samples)/len(firing_rate_samples):.3f}")
    print(f"  Min firing rate: {min(firing_rate_samples):.3f}")
    print(f"  Max firing rate: {max(firing_rate_samples):.3f}")
    
    # Memory usage analysis
    print(f"\nMemory Usage Analysis:")
    print(f"  Peak memory estimate: {max(memory_usage)}")
    print(f"  Average memory usage: {sum(memory_usage)/len(memory_usage):.0f}")
    
    return {
        'total_time': total_time,
        'steps_per_second': duration_steps/total_time,
        'avg_step_time_ms': total_time/duration_steps*1000,
        'step_time_std_ms': ((sum((t - total_time/duration_steps)**2 for t in step_times) / duration_steps)**0.5)*1000,
        'firing_rate_avg': sum(firing_rate_samples)/len(firing_rate_samples),
        'memory_peak': max(memory_usage)
    }

# Example usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
profile_brain_performance(brain, 1000)
```

### 3. Checkpoint Debugging

```python
import pynlm

def test_checkpoint_system(brain, test_steps=500, checkpoint_interval=100):
    """Test checkpoint system functionality and debugging capabilities."""
    
    print("Checkpoint System Testing")
    print("=" * 30)
    
    # Phase 1: Run simulation and save checkpoints
    print(f"\nPhase 1: Running {test_steps} steps with checkpoints every {checkpoint_interval} steps")
    
    checkpoint_files = []
    
    for step in range(test_steps):
        brain.step(step)
        
        if step % checkpoint_interval == 0 && step > 0:
            # Save checkpoint
            checkpoint_file = f"checkpoint_step_{step}.bin"
            brain.save(checkpoint_file)
            checkpoint_files.append(checkpoint_file)
            
            print(f"  Saved checkpoint at step {step} to {checkpoint_file}")
            
            # Verify checkpoint can be loaded
            brain.load(checkpoint_file)
            print(f"  ✓ Successfully loaded checkpoint from {checkpoint_file}")
    
    # Phase 2: Restore from last checkpoint and verify
    print(f"\nPhase 2: Restoring from last checkpoint")
    
    if checkpoint_files:
        # Load final state
        brain.load(checkpoint_files[-1])
        
        # Verify state integrity
        final_step = test_steps
        brain.step(final_step)
        
        print(f"  Restored and ran to step {final_step}")
        print(f"  Neuron count preserved: {brain.getTotalNeuronCount()}")
        print(f"  Synapse count preserved: {brain.getTotalSynapseCount()}")
    
    # Phase 3: Test checkpoint manager
    print(f"\nPhase 3: Testing checkpoint manager")
    
    if brain.getCheckpointManager():
        manager = brain.getCheckpointManager()
        
        print(f"  Manager status: {manager.getStatus()}")
        print(f"  Checkpoints created: {manager.getCheckpointCount()}")
        
        # Test cleanup
        manager.cleanupOldCheckpoints(3);
        print(f"  Cleaned up old checkpoints")
        
        print(f"  ✓ Checkpoint system fully functional")
    
    return brain, checkpoint_files

# Example usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
test_checkpoint_system(brain, 500, 100)
```

### 4. Advanced Simulation Control

```python
import pynlm

def advanced_simulation_control(brain, world, agent):
    """Advanced simulation control with debugging and monitoring."""
    
    print("Advanced Simulation Control")
    print("=" * 35)
    
    # Simulation state tracking
    simulation_state = {
        'current_step': 0,
        'total_steps': 0,
        'simulation_time': 0.0,
        'start_time': time.time(),
        'events_triggered': 0,
        'memory_events': 0,
        'development_events': 0,
        'checkpoint_events': 0
    }
    
    def record_event(event_type):
        simulation_state['events_triggered'] += 1
        simulation_state['event_types'][event_type] = simulation_state['event_types'].get(event_type, 0) + 1
    
    # Main simulation loop with advanced control
    for step in range(simulation_state['total_steps']):
        simulation_state['current_step'] = step
        simulation_state['simulation_time'] = step * 0.1
        
        # Record performance metrics
        step_start = time.time()
        
        # Update world
        world.update(0.1)
        record_event('world_update')
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        record_event('sensory_input')
        
        # Apply neuromodulation effects
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        record_event('neuromodulation')
        
        # Run brain step (main integration point)
        brain.step(step)
        record_event('brain_step')
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        record_event('action_selection')
        
        # Apply action to world
        world.applyMotorCommand(action, simulation_state['simulation_time'])
        record_event('action_execution')
        
        # Update development
        agent.updateDevelopment(0.1)
        simulation_state['development_events'] += 1
        
        # Check for special events
        if step % 100 == 0:
            # Memory consolidation event
            if brain.getEpisodicMemory():
                brain.getEpisodicMemory()->consolidate(0.3f);
                simulation_state['memory_events'] += 1
                record_event('memory_consolidation')
        
        if step % 1000 == 0 && brain.getCheckpointManager():
            # Checkpoint event
            brain.getCheckpointManager()->update(step, simulation_state['simulation_time']);
            simulation_state['checkpoint_events'] += 1
            record_event('checkpoint')
        
        # Performance monitoring
        step_end = time.time()
        step_duration = step_end - step_start
        
        if step % 100 == 0:
            avg_step_time = (step_duration * 1000)
            firing_rate = brain.getAverageFiringRate()
            
            print(f"  Step {step}: {avg_step_time:.2f}ms, Firing: {firing_rate:.3f}")
            
            # Alert on performance issues
            if avg_step_time > 10.0:  # 10ms threshold
                print(f"    ⚠️  Slow step detected: {avg_step_time:.2f}ms")
                record_event('performance_warning')
    
    # Simulation statistics
    end_time = time.time()
    total_duration = end_time - simulation_state['start_time']
    
    print(f"\nSimulation Statistics:")
    print(f"  Total duration: {total_duration:.2f} seconds")
    print(f"  Average step time: {total_duration/simulation_state['total_steps']*1000:.2f}ms")
    print(f"  Total events: {simulation_state['events_triggered']}")
    print(f"  Memory events: {simulation_state['memory_events']}")
    print(f"  Development events: {simulation_state['development_events']}")
    print(f"  Checkpoint events: {simulation_state['checkpoint_events']}")
    
    if 'performance_warning' in simulation_state.get('event_types', {}):
        print(f"  ⚠️  Performance warnings: {simulation_state['event_types']['performance_warning']}")
    
    return brain, world, agent, simulation_state

# Example usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

brain, world, agent, stats = advanced_simulation_control(brain, world, agent)
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
