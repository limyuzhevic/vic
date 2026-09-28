# NLM (Neural Learning Machine) - Comprehensive HOW TO USE Guide

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API (complete implementation)
4. Experiment management
5. Training loops and reinforcement learning
6. Visualization and analysis
7. Advanced features

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

## Part 3: Using the NLM Python API

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
brain.produceAction()             # Get motor action

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

# Convenience methods
brain.runSimulationForSteps(500, world)    # Run simulation for steps
brain.runEpisode(world, maxSteps=1000)     # Run complete episode
brain.saveCheckpoint("checkpoint.bin")    # Save state to file
brain.loadCheckpoint("checkpoint.bin")    # Load state from file
stats = brain.getBrainStats()              # Get formatted stats
stats_dict = brain.getBrainStatsMap()      # Get stats as dict

# Neuromodulation
brain.applyNeuromodulation(signal)         # Apply neuromodulation
brain.updatePlasticity()                   # Update plasticity
brain.develop()                            # Apply developmental changes

# Memory systems
brain.getWorkingMemory()                   # Get working memory
brain.getEpisodicMemory()                  # Get episodic memory
brain.getAssociativeMemory()               # Get associative memory
brain.getPredictionSystem()                # Get prediction system
brain.getPlanner()                         # Get neural planner
brain.getConceptFormation()                # Get concept formation
brain.getAttention()                       # Get attentional selection
brain.getDevelopmentSystem()               # Get development system
brain.getDopamine()                       # Get dopamine system
brain.getCuriosity()                      # Get curiosity system
brain.getNovelty()                        # Get novelty system
brain.getPredictionErrorSignal()          # Get prediction error signal
brain.getRandomGenerator()                # Get random generator

# System access
brain.getSpikeSystem()                    # Get spike system
brain.getSTDP()                           # Get STDP plasticity system
brain.getHebbian()                        # Get Hebbian plasticity system
brain.getStructuralPlasticity()           # Get structural plasticity system
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

# Status
status = agent.getStatus()  # Comprehensive status dict
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
world.applyAction(motor_cmd, current_time)  # Alias
world.observe()                             # Alias for getSensoryPercept()
world.computeReward(brain)                 # Compute reward

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

#### High-Level Creation Functions

```python
# Quick test brain for development/testing
brain = pynlm.createQuickTestBrain()
brain.initialize()

# Training environment setup
brain, world, agent = pynlm.createTrainingEnvironment()
brain.initialize()

# Evaluation environment setup
brain, world, agent = pynlm.createEvaluationEnvironment()
brain.initialize()

# Experiment configuration
config = pynlm.ExperimentConfig.createTrainingConfig()
config.setNeuronCount(500)
config.setMaxSteps(10000)

# Experiment runner
runner = pynlm.createExperimentRunner(config)

# Phase 6 experiment
experiment = pynlm.Phase6Experiment()
config = pynlm.Phase6Config()
config.neuronCount = 500
config.maxSteps = 2000
result = experiment.run(config)
```

#### Visualization and Analysis Classes

```python
# Neural visualization
brain = pynlm.createQuickTestBrain()
visualizer = pynlm.createVisualizer(brain, "PNG")
network_data = visualizer.visualizeNetwork()
visualizer.saveVisualization("network.png")

# Activity visualization
activity_data = visualizer.visualizeActivity()

# Neural activity analyzer
analyzer = pynlm.analyzeNetwork(brain, "full")
firing_patterns = analyzer.analyzeFiringPatterns()
oscillations = analyzer.detectOscillations()
cohesion = analyzer.computeCohesion()

# Time series analysis
ts_analyzer = pynlm.createTimeSeriesAnalyzer(metrics, 1000.0)
spectrum = ts_analyzer.computePowerSpectrum()
stationarity = ts_analyzer.detectStationarity("adf")

# Performance metrics
perf_metrics = pynlm.generatePerformanceReport(brain)
efficiency = perf_metrics.computeEfficiency(benchmark)
report = perf_metrics.generateReport("text")
```

#### Experiment Management

```python
# Experiment metrics
metrics = pynlm.ExperimentMetrics()
metrics.recordStep(step, reward, firing_rate, active_neurons, firing_neurons, total_spikes)
metrics.recordEpisode(total_reward, avg_firing_rate, total_active_neurons, total_firing_neurons, total_spikes)

# Experiment runner
runner.setup(config, brain, agent, world, environment)
runner.runEpisode()
runner.runExperiment(maxEpisodes=10)
results = runner.getExperimentMetrics()

# Phase 6 experiment
phase6 = pynlm.Phase6Experiment()
success, result = pynlm.runPhase6Experiment(config)
print(result.getIntegrationSummary())
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
pynlm.ActionType.MoveBackward
pynlm.ActionType.MoveLeft
pynlm.ActionType.MoveRight
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.LookUp
pynlm.ActionType.LookDown
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
pynlm.ActionType.Custom

# Motor commands
pynlm.MotorCommand.MoveForward
pynlm.MotorCommand.MoveBackward
pynlm.MotorCommand.TurnLeft
pynlm.MotorCommand.TurnRight
pynlm.MotorCommand.LookLeft
pynlm.MotorCommand.LookRight
pynlm.MotorCommand.Interact
pynlm.MotorCommand.Wait

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker

# Visualization formats
pynlm.NeuralVisualization.OutputFormat.PNG
pynlm.NeuralVisualization.OutputFormat.SVG
pynlm.NeuralVisualization.OutputFormat.ASCII

# Visualization color schemes
pynlm.NeuralVisualization.ColorScheme.Default
pynlm.NeuralVisualization.ColorScheme.Thermal
pynlm.NeuralVisualization.ColorScheme.CoolWarm
pynlm.NeuralVisualization.ColorScheme.Viridis
pynlm.NeuralVisualization.ColorScheme.Plasma
pynlm.NeuralVisualization.ColorScheme.Custom
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

### Experiment Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `experiment.name` | string | "" | Experiment name |
| `experiment.max_steps` | int | -1 | Maximum simulation steps |
| `experiment.neuron_count` | int | 1000 | Number of neurons |
| `experiment.region_count` | int | 1 | Number of brain regions |
| `experiment.connection_probability` | float | 0.05 | Connection probability |
| `experiment.checkpoint_enabled` | bool | false | Enable checkpointing |
| `experiment.replay_enabled` | bool | false | Enable experience replay |
| `experiment.development_enabled` | bool | true | Enable developmental processes |
| `experiment.checkpoint_path` | string | "" | Checkpoint file path |
| `experiment.seed` | int | 0 | Random seed (0 = random) |

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

### Training Example

```python
import pynlm

# Create training environment
brain, world, agent = pynlm.createTrainingEnvironment()
brain.initialize()

# Setup experiment
config = pynlm.ExperimentConfig.createTrainingConfig()
config.setNeuronCount(500)
config.setMaxSteps(5000)

runner = pynlm.createExperimentRunner(config)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Train
runner.runExperiment(maxEpisodes=100)

# Get results
metrics = runner.getExperimentMetrics()
print(f"Total reward: {metrics.getTotalReward()}")
print(f"Average reward: {metrics.getAverageRewardPerStep()}")
```

### Experiment Management Example

```python
import pynlm

# Create experiment configuration
config = pynlm.ExperimentConfig.createBenchmarkConfig()
config.setName("benchmark_test")
config.setNeuronCount(200)
config.setMaxSteps(1000)

# Validate configuration
if not config.validate():
    print(f"Config error: {config.validateAndGetError()}")
    exit(1)

# Create experiment metrics
metrics = pynlm.ExperimentMetrics()

# Create experiment runner
runner = pynlm.createExperimentRunner(config)

# Setup and run experiment (would need actual brain, agent, world components)
# runner.setup(config, brain, agent, world, environment)
# results = runner.runExperiment()

# Compare experiments
# experiments = [runner1, runner2, runner3]
# comparisons = pynlm.compareExperiments(experiments, "total_reward")

print("Experiment setup complete!")
```

---

## Part 6: Error Handling

### Custom Exceptions

```python
import pynlm

try:
    # Any NLM operation can throw NLMRuntimeError
    brain.runEpisode(world, 1000)
except pynlm.NLMRuntimeError as e:
    print(f"Error: {e}")
    # Handle specific error cases
    if "invalid configuration" in str(e):
        print("Fix your configuration and try again")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Error Handling Best Practices

```python
import pynlm

def safe_simulation_run(brain, world, agent, steps=1000):
    """Safely run a simulation with error handling."""
    try:
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        total_reward = brain.runEpisode(world, steps)
        return total_reward
        
    except pynlm.NLMRuntimeError as e:
        print(f"NLM Error: {e}")
        return None
    except MemoryError:
        print("Out of memory - reduce simulation size or use smaller brain")
        return None
    except KeyboardInterrupt:
        print("Simulation interrupted by user")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None

# Use it
reward = safe_simulation_run(brain, world, agent, 500)
if reward is not None:
    print(f"Simulation completed with reward: {reward}")
```

---

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

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Memory issues:**
- Reduce brain size in configuration
- Use smaller world dimensions
- Disable unnecessary plasticity mechanisms

**Performance issues:**
- Use Release build for better performance
- Increase `make -j` for parallel compilation
- Monitor memory usage during long simulations

### Runtime Issues

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Ensure neurons have proper connectivity
- Check configuration parameters

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Ensure `world.update()` is being called
- Check agent's motor neuron connectivity

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Enable neuromodulation for better activity

**"Segmentation faults"**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Validate configuration parameters

---

## Part 8: Performance Tips

### Memory Optimization
```python
# Use minimal configuration for testing
config = pynlm.ExperimentConfig.createQuickTestConfig()

# Disable unnecessary features
agent.enableStructuralPlasticity(False)
agent.enableDevelopment(False)
agent.enableCuriosity(False)
```

### Speed Optimization
```python
# Use Release build
# Use larger -j for parallel compilation
# Monitor memory usage
# Use efficient data structures
```

### Monitoring Performance
```python
import pynlm

# Create performance metrics
perf_metrics = pynlm.generatePerformanceReport(brain)

# Get system information
profile = perf_metrics.profilePerformance()
print(f"CPU usage: {profile.cpuUtilization}%")
print(f"Memory usage: {profile.memoryUtilization}%")
print(f"Steps per second: {profile.stepsPerSecond}")

# Generate detailed report
report = perf_metrics.generateReport("text")
with open("performance_report.txt", "w") as f:
    f.write(report)
```

---

## Part 9: Best Practices

### Configuration Management
```python
# Always validate configurations
config = pynlm.ExperimentConfig.createTrainingConfig()
if not config.validate():
    raise ValueError(config.validateAndGetError())

# Use factory methods for common configurations
config = pynlm.ExperimentConfig.createQuickTestConfig()
config = pynlm.ExperimentConfig.createTrainingConfig()
config = pynlm.ExperimentConfig.createBenchmarkConfig()
```

### Error Handling
```python
# Always handle errors gracefully
try:
    # Your NLM code here
    pass
except pynlm.NLMRuntimeError as e:
    print(f"NLM error: {e}")
    # Retry or abort
except Exception as e:
    print(f"Unexpected error: {e}")
    # Log and abort
```

### Resource Management
```python
# Always clean up resources
try:
    # Your NLM code here
    pass
finally:
    # Clean up
    if 'brain' in locals():
        brain.reset()
    if 'world' in locals():
        world.reset()
    if 'agent' in locals():
        agent.reset()
```

### Reproducibility
```python
# Set random seeds for reproducibility
config.setSeed(42)
world.setRandomSeed(42)

# Save checkpoints for recovery
brain.saveCheckpoint("checkpoint_before_crash.bin")
```

---

## Part 10: Further Reading

- [EXPERIMENT_MANAGEMENT.md](EXPERIMENT_MANAGEMENT.md) - Detailed experiment management documentation
- [TRAINING_LOOP.md](TRAINING_LOOP.md) - Training loop and reinforcement learning documentation
- [VISUALIZATION_ANALYSIS.md](VISUALIZATION_ANALYSIS.md) - Visualization and analysis APIs documentation
- [ADVANCED_FEATURES.md](ADVANCED_FEATURES.md) - Advanced features and use cases
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

---

*Last updated: 2026-09-28*

This documentation has been updated to match the actual NLM implementation, including all new features such as experiment management, training loops, visualization and analysis APIs, and advanced convenience methods.