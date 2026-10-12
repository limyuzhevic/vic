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

## Part 6: Enhanced Python Bindings for Research

### Overview of Enhanced Bindings

NLM now provides enhanced Python bindings through the `pynlm_enhanced` module that significantly expands the research capabilities of the NLM framework. This enhanced version includes:

- Advanced neuromodulation system APIs
- Memory systems direct access
- Prediction system for predictive coding research
- Cognition and development system APIs
- Performance profiling and analysis tools
- Configuration validation and schema system
- Research utilities for batch experiments and statistical analysis

### Installing Enhanced Bindings

```bash
# Install the enhanced bindings (includes original pynlm)
pip install -e .

# Or install specific optional dependencies for research
pip install pynlm[research]

# For performance analysis
pip install pynlm[benchmark]

# For development and debugging
pip install pynlm[dev]
```

### Key Enhanced Classes

#### EnhancedConfig
The `EnhancedConfig` class provides configuration validation and type-safe access:

```python
import pynlm_enhanced

# Create enhanced configuration with validation
config = pynlm_enhanced.createEnhancedConfig()
config.setNeuronCount(1000)
config.setConnectionProbability(0.1)

# Validate configuration
if config.validate():
    print("Configuration is valid")
else:
    print("Configuration has errors")

# Get configuration schema
print(config.getSchema())
```

#### PerformanceProfiler
Track simulation performance for optimization:

```python
import pynlm_enhanced

# Create and start profiler
profiler = pynlm_enhanced.PerformanceProfiler()
profiler.start()

# Run simulation steps
for step in range(100):
    brain.step(step)
    profiler.step()

# Stop and analyze
profiler.stop()
metrics = profiler.getMetrics()

print(f"Average step time: {profiler.getAverageStepTime():.3f}ms")
print(f"Min step time: {profiler.getMinStepTime():.3f}ms")
print(f"Max step time: {profiler.getMaxStepTime():.3f}ms")
print(f"Total spikes: {metrics.spikeCount}")
```

#### Research Utilities
Batch processing and statistical analysis for research:

```python
import pynlm_enhanced
from pynlm_enhanced import ResearchUtilities

# Run batch experiments
def run_experiment_config(neuron_count, plasticity):
    config = pynlm_enhanced.createEnhancedConfig()
    config.setNeuronCount(neuron_count)
    return config

configs = [
    run_experiment_config(500, True),
    run_experiment_config(1000, True),
    run_experiment_config(2000, False)
]

# Run batch experiments
results = pynlm_enhanced.ResearchUtilities.runBatchExperiments(configs)

# Statistical analysis
data = [result.totalReward for result in results]
stats = pynlm_enhanced.ResearchUtilities.computeStatistics(data)
print(f"Reward statistics: mean={stats['mean']:.3f}, std={stats['stddev']:.3f}")
```

#### Brain Research APIs
Direct access to brain systems for research:

```python
import pynlm_enhanced

# Enhanced brain operations
brain_stats = brain.analyzeNeuralActivity()
print("Neural activity analysis:")
for key, value in brain_stats.items():
    print(f"  {key}: {value}")

# Store experiences for memory research
brain.storeExperience(sensory_pattern, action_pattern, reward)

# Control developmental stages
brain.advanceDevelopmentStage(pynlm_enhanced.DevelopmentalStage.Maturation)
```

### Research Examples

#### Example 1: Neuromodulation Research
```python
import pynlm_enhanced

# Create research-ready brain
brain = pynlm_enhanced.createResearchBrain(500, True)
world = pynlm_enhanced.createSimpleWorld()
world.configure(20, 20, 8, 8)
agent = pynlm_enhanced.createAgentForResearch(brain, world)

# Enable neuromodulation for research
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Conduct neuromodulation experiment
brain.conductNeuromodulationExperiment("dopamine_reward", 1.0)
brain.conductNeuromodulationExperiment("curiosity_drive", 0.5)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Example 2: Memory Research
```python
import pynlm_enhanced

# Create brain for memory research
brain = pynlm_enhanced.createResearchBrain(1000, True)

# Store experiences for memory analysis
for step in range(50):
    pattern = [float(i % 10) / 10.0 for i in range(10)]
    brain.storeExperience(pattern, pattern, 0.5)
    brain.step(step)

# Analyze memory storage
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Current traces: {working_memory.getCurrentSize()}")
```

#### Example 3: Performance Optimization
```python
import pynlm_enhanced

# Create brain and profiler
brain = pynlm_enhanced.createResearchBrain(2000, True)
profiler = pynlm_enhanced.PerformanceProfiler()

# Profile different configurations
configs = [
    {"neuron_count": 500, "plasticity": True},
    {"neuron_count": 1000, "plasticity": True},
    {"neuron_count": 2000, "plasticity": False}
]

for config in configs:
    print(f"\nTesting {config['neuron_count']} neurons...")
    brain = pynlm_enhanced.createResearchBrain(config['neuron_count'], config['plasticity'])
    
    profiler.start()
    for step in range(50):
        brain.step(step)
        profiler.step()
    profiler.stop()
    
    print(f"Performance: {profiler.getAverageStepTime():.3f}ms per step")
    profiler.reset()
```

#### Example 4: Batch Experiment Analysis
```python
import pynlm_enhanced

# Create multiple configurations for batch analysis
base_config = pynlm_enhanced.createEnhancedConfig()
configs = []

# Generate parameter space
for neuron_count in [100, 500, 1000]:
    for plasticity in [True, False]:
        config = pynlm_enhanced.createEnhancedConfig()
        config.setNeuronCount(neuron_count)
        configs.append(config)

# Run batch experiment
print("Running batch experiments...")
results = pynlm_enhanced.ResearchUtilities.runBatchExperiments(configs)

# Analyze results
rewards = [result.totalReward for result in results]
firing_rates = [result.avgFiringRate for result in results]

stats = pynlm_enhanced.ResearchUtilities.computeStatistics(rewards)
print(f"\n=== Batch Experiment Results ===")
print(f"Average reward: {stats['mean']:.3f} ± {stats['stddev']:.3f}")
print(f"Average firing rate: {sum(firing_rates)/len(firing_rates):.3f}")

# Export for external analysis
pynlm_enhanced.exportForAnalysis(brain, "batch_results.csv")
```

### Migration Guide

#### From Original to Enhanced API

**Original pynlm (unchanged):**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
```

**Enhanced pynlm_enhanced:**
```python
import pynlm_enhanced

# Same basic functionality, plus enhanced features
brain = pynlm_enhanced.createBrain(pynlm_enhanced.createEnhancedConfig())
brain.initialize()

# Access enhanced features
profiler = pynlm_enhanced.PerformanceProfiler()
metrics = brain.analyzeNeuralActivity()
```

** Gradual Migration:**
```python
# Start with enhanced API (includes original functionality)
import pynlm_enhanced as pynlm

# Use enhanced features
config = pynlm.createEnhancedConfig()
config.setNeuronCount(500)

brain = pynlm.createBrain(config.getInternalConfig())
brain.initialize()

# Your existing code works unchanged
# Plus new research capabilities
profiler = pynlm.PerformanceProfiler()
profiler.start()
for step in range(100):
    brain.step(step)
    profiler.step()
profiler.stop()
```

### Advanced Research Features

#### Concept Formation Research
```python
import pynlm_enhanced

brain = pynlm_enhanced.createResearchBrain(1000, True)

# Access concept formation (if available)
if hasattr(brain, 'getConceptFormation'):
    concept_formation = brain.getConceptFormation()
    
    # Present experiences to concept formation
    for step in range(50):
        pattern = [float(i % 10) / 10.0 for i in range(10)]
        features = [1.0, 2.0, 3.0]  # Example features
        reward = 0.5
        concept_id = concept_formation.presentExperience(pattern, features, reward, step)
        
        if concept_id > 0:
            print(f"New concept discovered: ID {concept_id}")
```

#### Prediction Research
```python
import pynlm_enhanced

brain = pynlm_enhanced.createResearchBrain(500, True)

# Access prediction system (if available)
if hasattr(brain, 'getPredictionSystem'):
    prediction_system = brain.getPredictionSystem()
    
    # Make predictions and analyze prediction error
    for step in range(50):
        # Get current state
        sensory_input = brain.getCurrentSensoryInput()
        
        # Make prediction
        prediction = prediction_system.predictNextState(sensory_input)
        
        # Get prediction error
        error = prediction_system.getPredictionError()
        confidence = prediction_system.getConfidence()
        
        print(f"Prediction error: {error:.3f}, Confidence: {confidence:.3f}")
```

#### Development Research
```python
import pynlm_enhanced

brain = pynlm_enhanced.createResearchBrain(200, True)

# Study developmental stages
for stage in pynlm_enhanced.DevelopmentalStage:
    brain.advanceDevelopmentStage(stage)
    print(f"Current stage: {stage}, Firing rate: {brain.getAverageFiringRate():.3f}")
    
    # Run experiment at each stage
    for step in range(20):
        brain.step(step)
```

### Performance Considerations

#### Memory Profiling
```python
import pynlm_enhanced
import psutil
import os

process = psutil.Process(os.getpid())

brain = pynlm_enhanced.createResearchBrain(1000, True)
profiler = pynlm_enhanced.PerformanceProfiler()

profiler.start()
for step in range(100):
    brain.step(step)
    profiler.step()
    
    # Monitor memory usage
    memory_mb = process.memory_info().rss / 1024 / 1024
    print(f"Step {step}: Memory usage: {memory_mb:.2f} MB")

profiler.stop()
metrics = profiler.getMetrics()
print(f"Peak memory: {memory_mb:.2f} MB")
```

#### Data Export for External Analysis
```python
import pynlm_enhanced

brain = pynlm_enhanced.createResearchBrain(500, True)
world = pynlm_enhanced.createSimpleWorld()
world.configure(10, 10, 8, 8)
agent = pynlm_enhanced.createAgentForResearch(brain, world)

# Run experiment and collect data
experiment_data = []
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Collect neural activity data
    activity = brain.analyzeNeuralActivity()
    activity['step'] = step
    activity['world_time'] = world.getSimulationTime()
    experiment_data.append(activity)

# Export for analysis
pynlm_enhanced.exportForAnalysis(brain, "experiment_data.csv", "csv")
```

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
