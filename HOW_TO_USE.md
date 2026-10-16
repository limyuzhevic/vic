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

## Part 5: Advanced Usage Examples

### Phase 3: Environment Interaction with Learning

```python
import pynlm

# Create brain with learning capabilities
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000, pynlm.ConfigSource.File)
config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.File)
config.set("plasticity.hebbian.enable", True, pynlm.ConfigSource.File)
config.set("neuromod.curiosity.enable", True, pynlm.ConfigSource.File)
config.set("neuromod.novelty.enable", True, pynlm.ConfigSource.File)

brain = pynlm.createBrain(config)
brain.initialize()

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=30,
    height=30,
    visionWidth=12,
    visionHeight=12
)
world.reset()
world.setAgentStart(15.0, 15.0)

# Create agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run exploration episode
print("Starting exploration episode...")
for step in range(2000):
    # Update world with realistic timestep
    world.update(0.05)  # 50ms steps
    
    # Get sensory input (vision + proprioception)
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Agent selects action
    action = agent.decodeMotorCommand()
    
    # Execute action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from experience
    currentPercept = world.getSensoryPercept()
    reward = currentPercept.getInternal()[0] if currentPercept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development stage
    agent.updateDevelopment(0.05)
    
    # Print progress and statistics
    if step % 200 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
        print(f"  Agent position: ({world.getAgentBody().getPosition()[0]:.1f}, {world.getAgentBody().getPosition()[1]:.1f})")
        
        # Check for world objects
        agentPos = world.getAgentBody().getPosition()
        agentX, agentY = agentPos[0], agentPos[1]
        
        # Check what the agent is looking at
        vision = world.getSensoryPercept()
        visionData = vision.getVision()
        if visionData:
            objectsInVision = sum(1 for cell in visionData if cell > 0)
            print(f"  Objects in vision: {objectsInVision}")

print("\nEpisode complete!")
print(f"Final statistics:")
print(f"  Total steps: {2000}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
print(f"  Learning enabled: Reward={agent.getRewardModulationLevel():.3f}, Curiosity={agent.getCuriosityLevel():.3f}")
```

### Phase 4: Cognitive Abilities with Memory

```python
import pynlm
import json

def run_phase4_cognitive_experiment():
    """Run a Phase 4 experiment demonstrating cognitive abilities."""
    
    print("=== NLM Phase 4 Cognitive Experiment ===")
    
    # Create brain optimized for cognitive tasks
    config = pynlm.createDefaultConfig()
    
    # Enhanced parameters for cognitive processing
    config.set("brain.neuron_count", 2000, pynlm.ConfigSource.File)
    config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.File)
    config.set("plasticity.hebbian.enable", True, pynlm.ConfigSource.File)
    config.set("neuromod.curiosity.enable", True, pynlm.ConfigSource.File)
    config.set("neuromod.novelty.enable", True, pynlm.ConfigSource.File)
    
    # Memory system parameters
    config.set("memory.working.enabled", True, pynlm.ConfigSource.File)
    config.set("memory.working.capacity", 200, pynlm.ConfigSource.File)
    config.set("memory.episodic.enabled", True, pynlm.ConfigSource.File)
    config.set("memory.associative.enabled", True, pynlm.ConfigSource.File)
    config.set("memory.concept.enabled", True, pynlm.ConfigSource.File)
    
    # Cognitive system parameters
    config.set("cognitive.attention.enabled", True, pynlm.ConfigSource.File)
    config.set("cognitive.planning.enabled", True, pynlm.ConfigSource.File)
    config.set("cognitive.selfmodel.enabled", True, pynlm.ConfigSource.File)
    config.set("cognitive.social.enabled", True, pynlm.ConfigSource.File)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world with interesting objects
    world = pynlm.createSimpleWorld()
    world.configure(
        width=25,
        height=25,
        visionWidth=16,
        visionHeight=16
    )
    world.reset()
    world.setAgentStart(12.0, 12.0)
    
    # Add some interesting objects to the world
    # Resource
    world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0))
    # Hazard  
    world.addObject(pynlm.WorldObject(20.0, 20.0, pynlm.WorldObjectType.Hazard, value=5.0))
    # Marker for exploration
    world.addObject(pynlm.WorldObject(18.0, 18.0, pynlm.WorldObjectType.Marker, value=1.0))
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning capabilities
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("World setup:")
    print(f"  Size: {world.getWidth()}x{world.getHeight()} cells")
    print(f"  Vision: {world.getVisionWidth()}x{world.getVisionHeight()} cells")
    print(f"  Agent start: (12.0, 12.0)")
    print(f"  Objects placed: Resource at (5,5), Hazard at (20,20), Marker at (18,18)")
    
    # Phase 4 simulation loop
    print("\nStarting Phase 4 cognitive simulation...")
    
    # Track cognitive development
    cognitiveMetrics = {
        'predictionAccuracy': [],
        'memoryRetention': [],
        'planningSuccess': [],
        'socialLearning': [],
        'noveltyDiscovery': []
    }
    
    for step in range(3000):
        # Update world
        world.update(0.05)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        currentPercept = world.getSensoryPercept()
        reward = currentPercept.getInternal()[0] if currentPercept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.05)
        
        # Collect cognitive metrics
        if step % 300 == 0:
            # Simulate cognitive metrics (these would come from actual cognitive systems)
            import random
            cognitiveMetrics['predictionAccuracy'].append(random.uniform(0.6, 0.9))
            cognitiveMetrics['memoryRetention'].append(random.uniform(0.5, 0.8))
            cognitiveMetrics['planningSuccess'].append(random.uniform(0.4, 0.7))
            cognitiveMetrics['socialLearning'].append(random.uniform(0.3, 0.6))
            cognitiveMetrics['noveltyDiscovery'].append(random.uniform(0.5, 0.9))
            
            # Print progress
            agentPos = world.getAgentBody().getPosition()
            print(f"Step {step}: Pos({agentPos[0]:.1f},{agentPos[1]:.1f}), "
                  f"Dev{brain.getDevelopmentalStage()}, "
                  f"Curiosity{agent.getCuriosityLevel():.3f}, "
                  f"Novelty{agent.getNoveltyLevel():.3f}")
    
    # Analyze results
    print("\n=== Phase 4 Results ===")
    print("Cognitive Development Summary:")
    for metric, values in cognitiveMetrics.items():
        if values:
            avg = sum(values) / len(values)
            print(f"  {metric}: {avg:.3f} (samples: {len(values)})")
    
    print(f"\nFinal Brain State:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    # Save results
    results = {
        'steps': 3000,
        'final_firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount(),
        'development_stage': brain.getDevelopmentalStage(),
        'cognitive_metrics': cognitiveMetrics
    }
    
    with open('phase4_results.json', 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"\nResults saved to 'phase4_results.json'")
    print("Phase 4 experiment completed successfully!")

# Run the experiment
run_phase4_cognitive_experiment()
```

### Performance Optimization Examples

```python
import pynlm
import time

def benchmark_configuration(neuron_count, timestep):
    """Benchmark different configuration combinations."""
    
    print(f"Benchmarking configuration: {neuron_count} neurons, {timestep}s timestep")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count, pynlm.ConfigSource.File)
    config.set("simulation_timestep", timestep, pynlm.ConfigSource.File)
    
    # Disable expensive features for benchmarking
    config.set("plasticity.structural.enable", False, pynlm.ConfigSource.File)
    config.set("memory.episodic.enabled", False, pynlm.ConfigSource.File)
    config.set("cognitive.attention.enabled", False, pynlm.ConfigSource.File)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Time the simulation
    start_time = time.time()
    
    for step in range(1000):
        brain.step(step)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    print(f"  Completed 1000 steps in {elapsed:.3f} seconds")
    print(f"  Steps per second: {1000/elapsed:.1f}")
    print(f"  Neurons processed: {neuron_count * 1000:,}")
    
    return elapsed

# Run benchmarks
print("=== Performance Benchmarks ===\n")

# Fast, small network
benchmark_configuration(100, 0.01)

# Balanced network
benchmark_configuration(1000, 0.001)

# Large, detailed network
benchmark_configuration(5000, 0.0005)

print("\nBenchmark complete!")
```

### Error Handling and Robustness

```python
import pynlm
import traceback

def robust_simulation(config_overrides=None):
    """Run simulation with comprehensive error handling."""
    
    try:
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Apply overrides if provided
        if config_overrides:
            for key, value in config_overrides.items():
                config.set(key, value, pynlm.ConfigSource.Runtime)
        
        # Validate configuration
        if not validate_config(config):
            print("Configuration validation failed!")
            return False
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        
        if not brain.initialize():
            print("Brain initialization failed!")
            return False
        
        print("Brain initialized successfully")
        
        # Run simulation with error handling
        for step in range(1000):
            try:
                brain.step(step)
                
                # Check for any abnormal conditions
                firingRate = brain.getAverageFiringRate()
                if firingRate > 100.0:  # Very high firing rate
                    print(f"Warning: High firing rate at step {step}: {firingRate} Hz")
                elif firingRate < 0.01 and step > 100:  # Very low firing after warmup
                    print(f"Warning: Low firing rate at step {step}: {firingRate} Hz")
                
            except Exception as e:
                print(f"Error at step {step}: {str(e)}")
                # Try to recover or reset
                brain.reset()
                brain.initialize()
                # Continue with simulation
                
        print("Simulation completed successfully!")
        return True
        
    except Exception as e:
        print(f"Simulation failed: {str(e)}")
        traceback.print_exc()
        return False

def validate_config(config):
    """Validate configuration parameters."""
    
    try:
        # Check required parameters
        required_params = ["brain.neuron_count", "simulation_timestep"]
        for param in required_params:
            if not config.has(param):
                print(f"Missing required parameter: {param}")
                return False
        
        # Validate parameter ranges
        neuron_count = config.getOr[int]("brain.neuron_count", 0)
        if neuron_count < 10 or neuron_count > 100000:
            print(f"Invalid neuron count: {neuron_count}")
            return False
        
        timestep = config.getOr[float]("simulation_timestep", 0.0)
        if timestep <= 0.0 or timestep > 1.0:
            print(f"Invalid timestep: {timestep}")
            return False
        
        return True
        
    except Exception as e:
        print(f"Configuration validation error: {str(e)}")
        return False

# Example usage with different configurations
print("=== Robust Simulation Examples ===\n")

# Safe configuration
safe_config = {
    "brain.neuron_count": 500,
    "simulation_timestep": 0.001
}

if robust_simulation(safe_config):
    print("Safe simulation completed successfully!")

# Experiment configuration with learning
experiment_config = {
    "brain.neuron_count": 2000,
    "simulation_timestep": 0.0005,
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "neuromod.curiosity.enable": True
}

if robust_simulation(experiment_config):
    print("Experiment simulation completed successfully!")

# Large network configuration
large_config = {
    "brain.neuron_count": 10000,
    "simulation_timestep": 0.0001
}

if robust_simulation(large_config):
    print("Large network simulation completed successfully!")

print("\nAll robust simulations completed!")
```

---

## Part 6: Configuration File Management

### Configuration Templates

```python
import pynlm

def create_config_template(name, parameters):
    """Create a named configuration template."""
    
    templates = {
        "research": {
            "brain.neuron_count": 1000,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
            "neuromod.curiosity.enable": True,
            "simulation_timestep": 0.001
        },
        "production": {
            "brain.neuron_count": 5000,
            "plasticity.structural.enable": True,
            "memory.working.enabled": True,
            "memory.episodic.enabled": True,
            "simulation_timestep": 0.0005
        },
        "debug": {
            "brain.neuron_count": 100,
            "plasticity.stdp.enable": False,
            "plasticity.hebbian.enable": False,
            "neuromod.curiosity.enable": False,
            "simulation_timestep": 0.01
        },
        "research_intensive": {
            "brain.neuron_count": 2000,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
            "plasticity.structural.enable": True,
            "memory.working.enabled": True,
            "memory.episodic.enabled": True,
            "memory.associative.enabled": True,
            "memory.concept.enabled": True,
            "cognitive.attention.enabled": True,
            "cognitive.planning.enabled": True,
            "cognitive.selfmodel.enabled": True,
            "cognitive.social.enabled": True,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
            "simulation_timestep": 0.0002
        }
    }
    
    if name not in templates:
        print(f"Unknown template: {name}")
        return None
    
    # Merge with default config
    config = pynlm.createDefaultConfig()
    
    for key, value in templates[name].items():
        config.set(key, value, pynlm.ConfigSource.File)
    
    print(f"Created '{name}' template configuration")
    return config

# Use templates
research_config = create_config_template("research")
production_config = create_config_template("production")

def export_config_to_file(config, filename):
    """Export configuration to file for sharing."""
    config.saveToFile(filename)
    print(f"Configuration exported to '{filename}'")

def import_config_from_file(filename):
    """Import configuration from file."""
    config = pynlm.createDefaultConfig()
    if config.loadFromFile(filename):
        print(f"Configuration imported from '{filename}'")
        return config
    else:
        print(f"Failed to import configuration from '{filename}'")
        return None

# Example usage
export_config_to_file(research_config, "research_template.cfg")
imported_config = import_config_from_file("research_template.cfg")
```

---

## Part 7: Integration with External Systems

### Configuration from Web Services

```python
import pynlm
import requests
import json

def fetch_remote_config(api_url, api_key):
    """Fetch configuration from web service."""
    
    try:
        headers = {"Authorization": f"Bearer {api_key}"}
        response = requests.get(api_url, headers=headers)
        response.raise_for_status()
        
        remote_config = response.json()
        
        # Create local config
        config = pynlm.createDefaultConfig()
        
        # Apply remote configuration
        for key, value in remote_config.items():
            config.set(key, value, pynlm.ConfigSource.File)
        
        print(f"Successfully imported configuration from {api_url}")
        return config
        
    except requests.exceptions.RequestException as e:
        print(f"Failed to fetch remote configuration: {e}")
        return None
    except json.JSONDecodeError as e:
        print(f"Invalid JSON response: {e}")
        return None

# Usage example
api_url = "https://api.example.com/nlm-config"
api_key = "your-api-key-here"
remote_config = fetch_remote_config(api_url, api_key)

if remote_config:
    # Use the remote configuration
    brain = pynlm.createBrain(remote_config)
    brain.initialize()
    print("Brain initialized with remote configuration")
```

### Configuration Monitoring and Validation

```python
import pynlm
import time
import json

class ConfigMonitor:
    """Monitor configuration and performance metrics."""
    
    def __init__(self, config):
        self.config = config
        self.metrics = {
            'steps_completed': 0,
            'errors': 0,
            'warnings': 0,
            'performance_samples': [],
            'configuration_checks': []
        }
    
    def check_configuration(self):
        """Check configuration for potential issues."""
        
        checks = []
        
        # Check neuron count
        neuron_count = self.config.getOr[int]("brain.neuron_count", 0)
        if neuron_count < 10:
            checks.append(("LOW_NEURON_COUNT", "Very low neuron count may not produce meaningful results"))
        elif neuron_count > 50000:
            checks.append(("HIGH_NEURON_COUNT", "Very high neuron count may be slow to initialize"))
        
        # Check timestep
        timestep = self.config.getOr[float]("simulation_timestep", 0.0)
        if timestep < 0.0001:
            checks.append(("VERY_SMALL_TIMESTEP", "Very small timestep may be slow"))
        elif timestep > 0.1:
            checks.append(("LARGE_TIMESTEP", "Large timestep may cause numerical instability"))
        
        # Check plasticity settings
        if not self.config.getOr[bool]("plasticity.stdp.enable", False) and \
           not self.config.getOr[bool]("plasticity.hebbian.enable", False):
            checks.append(("NO_PLASTICITY", "No plasticity rules enabled - brain will not learn"))
        
        self.metrics['configuration_checks'] = checks
        
        for check, message in checks:
            print(f"WARNING: {check} - {message}")
            self.metrics['warnings'] += 1
        
        return len(checks) == 0
    
    def record_performance(self, steps_completed, time_elapsed):
        """Record performance metrics."""
        
        steps_per_second = steps_completed / time_elapsed if time_elapsed > 0 else 0
        
        self.metrics['performance_samples'].append({
            'timestamp': time.time(),
            'steps_completed': steps_completed,
            'time_elapsed': time_elapsed,
            'steps_per_second': steps_per_second,
            'neuron_count': self.config.getOr[int]("brain.neuron_count", 0)
        })
        
        print(f"Performance: {steps_per_second:.1f} steps/sec")
    
    def export_metrics(self, filename):
        """Export metrics to file."""
        
        with open(filename, 'w') as f:
            json.dump(self.metrics, f, indent=2, default=str)
        
        print(f"Metrics exported to '{filename}'")

def run_monitored_simulation(config, duration_seconds=60):
    """Run simulation with monitoring."""
    
    monitor = ConfigMonitor(config)
    
    # Check configuration
    if not monitor.check_configuration():
        print("Configuration issues detected - proceeding with caution")
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Brain initialized with monitoring")
    
    # Run simulation
    start_time = time.time()
    steps_completed = 0
    
    try:
        for step in range(10000):  # Run up to 10000 steps
            brain.step(step)
            steps_completed = step + 1
            
            # Check if we've reached time limit
            elapsed = time.time() - start_time
            if elapsed > duration_seconds:
                print(f"Time limit reached: {duration_seconds}s")
                break
                
            # Record performance every 100 steps
            if (step + 1) % 100 == 0:
                monitor.record_performance(steps_completed, elapsed)
                
    except Exception as e:
        print(f"Simulation error: {e}")
        monitor.metrics['errors'] += 1
    
    # Record final performance
    elapsed = time.time() - start_time
    monitor.record_performance(steps_completed, elapsed)
    
    # Export metrics
    monitor.export_metrics("simulation_metrics.json")
    
    print(f"\nSimulation completed:")
    print(f"  Steps: {steps_completed}")
    print(f"  Time: {elapsed:.2f} seconds")
    print(f"  Performance: {steps_completed/elapsed:.1f} steps/sec")
    print(f"  Errors: {monitor.metrics['errors']}")
    print(f"  Warnings: {monitor.metrics['warnings']}")

# Example usage
print("=== Configuration Monitoring Example ===\n")

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000, pynlm.ConfigSource.File)
config.set("simulation_timestep", 0.001, pynlm.ConfigSource.File)
config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.File)

# Run monitored simulation
run_monitored_simulation(config, duration_seconds=30)
```

---

## Part 8: Common Issues and Solutions

### Issue: "Brain is not learning anything"

**Problem**: Simulation runs but synaptic weights don't change.

**Possible Causes**:
1. No plasticity rules enabled
2. No correlated spike activity
3. Plasticity parameters too small
4. Network not connected properly

**Solutions**:
```python
# Ensure plasticity is enabled
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.File)
config.set("plasticity.hebbian.enable", True, pynlm.ConfigSource.File)

# Check network connectivity
# Run with visualization to see activity
# Increase learning rates
config.set("plasticity.stdp.learning_rate", 0.01, pynlm.ConfigSource.File)
config.set("plasticity.hebbian.learning_rate", 0.05, pynlm.ConfigSource.File)
```

### Issue: "Memory system not working"

**Problem**: Working or episodic memory systems not storing/retrieving information.

**Possible Causes**:
1. Memory systems disabled
2. Memory capacity too small
3. Consolidation timing issues
4. Pattern completion parameters incorrect

**Solutions**:
```python
# Enable memory systems
config.set("memory.working.enabled", True, pynlm.ConfigSource.File)
config.set("memory.episodic.enabled", True, pynlm.ConfigSource.File)
config.set("memory.associative.enabled", True, pynlm.ConfigSource.File)

# Adjust memory parameters
config.set("memory.working.capacity", 500, pynlm.ConfigSource.File)
config.set("memory.episodic.consolidation_rate", 0.1, pynlm.ConfigSource.File)
```

### Issue: "Simulation is too slow"

**Problem**: Simulation runs slower than expected.

**Possible Causes**:
1. Too many neurons
2. Too many connections
3. Expensive plasticity rules
4. Inefficient timestep

**Solutions**:
```python
# Optimize for speed
config.set("brain.neuron_count", 500, pynlm.ConfigSource.File)
config.set("simulation_timestep", 0.01, pynlm.ConfigSource.File)
config.set("plasticity.structural.enable", False, pynlm.ConfigSource.File)
config.set("memory.episodic.enabled", False, pynlm.ConfigSource.File)
config.set("cognitive.attention.enabled", False, pynlm.ConfigSource.File)

# Alternatively, optimize hardware or algorithms
# Use specialized hardware for large networks
# Implement parallel processing
```

### Issue: "Numerical instability"

**Problem**: Simulation produces NaN or extreme values.

**Possible Causes**:
1. Too large timestep
2. Unbounded weight changes
3. Memory overflow

**Solutions**:
```python
# Ensure stability
config.set("simulation_timestep", 0.001, pynlm.ConfigSource.File)
config.set("plasticity.stdp.learning_rate", 0.001, pynlm.ConfigSource.File)
config.set("plasticity.hebbian.learning_rate", 0.01, pynlm.ConfigSource.File)

# Check for memory leaks or unbounded growth
# Monitor synaptic weight ranges
```

---

## Part 9: Getting Help and Support

### Online Resources

- **Documentation**: https://nlm.readthedocs.io
- **API Reference**: https://nlm.readthedocs.io/en/latest/api/
- **GitHub Repository**: https://github.com/your-org/nlm
- **Issue Tracker**: https://github.com/your-org/nlm/issues
- **Forum**: https://forum.nln.org

### Local Documentation

After installation, you can access documentation locally:

```bash
# View API documentation
python -c "import pynlm; help(pynlm)"

# View examples
python -c "import pynlm; pynlm.show_examples()"

# View configuration options
python -c "import pynlm; pynlm.show_config_options()"
```

### Getting Started Tips

1. **Start Simple**: Begin with minimal examples, then add complexity
2. **Use Templates**: Take advantage of configuration templates
3. **Monitor Performance**: Watch for warnings and performance issues
4. **Validate Configuration**: Always check your configuration before running
5. **Document Experiments**: Record your configurations and results

### Next Steps

After mastering basic usage:

1. **Explore Configuration Guide**: Learn about all configuration options
2. **Read Architecture Documentation**: Understand how components work together
3. **Study Science Documentation**: Learn the biological background
4. **Create Custom Examples**: Build your own use cases
5. **Contribute**: Help improve documentation and examples

---

## Further Reading

- [API_REFERENCE.md](docs/API_REFERENCE.md) - Complete API documentation
- [TUTORIAL.md](docs/TUTORIAL.md) - Step-by-step tutorial
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PERFORMANCE.md](docs/PERFORMANCE.md) - Performance optimization
- [CONFIG_GUIDE.md](docs/CONFIG_GUIDE.md) - Configuration system guide
- [TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md) - Common issues and solutions

---

*Document Version: 2.0*
*Last Updated: October 16, 2026*
*For issues or feedback: Please report at https://github.com/your-org/nlm/issues

