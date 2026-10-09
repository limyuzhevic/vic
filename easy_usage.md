# NLM - Improved Documentation and User Guide

This document provides comprehensive information about the NLM (Neural Learning Machine) project, including installation, usage, and advanced features.

## Table of Contents

1. [Introduction](#introduction)
2. [Installation and Build](#installation-and-build)
3. [Python API](#python-api)
4. [C++ API](#cplusplus-api)
5. [Configuration](#configuration)
6. [Advanced Features](#advanced-features)
7. [Visualization](#visualization)
8. [Performance Monitoring](#performance-monitoring)
9. [Troubleshooting](#troubleshooting)
10. [Examples](#examples)

## Introduction

NLM (熙然) is an experimental artificial developmental brain system that implements:

- **Real spiking neural computation** with Leaky Integrate-and-Fire (LIF) neurons
- **Event-driven spike propagation** with synaptic delays
- **Multiple plasticity mechanisms** including STDP, Hebbian learning, and structural plasticity
- **Integrated memory systems** (working, episodic, semantic, procedural, associative)
- **Neuromodulation** (dopamine, curiosity, novelty detection)
- **Prediction and planning systems**
- **Cognitive mechanisms** (concept formation, attention, neural planning)
- **Development and maturation** systems
- **Sensory and motor processing** capabilities

The NLM brain operates as a closed-loop system where:

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE → MEMORY/PREDICTION
    ↓                                        ↓
  MOTIVATION/NEUROMODULATION → ACTION SELECTION → MOTOR OUTPUT → WORLD CONSEQUENCE
    ↓
  REWARD/SURPRISE/ERROR → PLASTICITY → CHANGED BRAIN → CHANGED FUTURE BEHAVIOR
```

## Installation and Build

### Prerequisites

- C++ compiler supporting C++20 (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)
- pybind11 2.11.0+ (for Python bindings)

### Building from Source

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd nlm
   ```

2. Create and enter build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure with CMake:
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

4. Build all targets:
   ```bash
   make -j$(nproc)
   ```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3` | Static Library | Phase 3 experiments |
| `nlm_phase4` | Static Library | Phase 4 experiments |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_visualization` | Static Library | Advanced visualization capabilities |
| `nlm_performance` | Static Library | Performance monitoring and profiling |

### Python Installation

#### From Source (Recommended for Development)

1. Install build dependencies:
   ```bash
   pip install pybind11 scikit-build-core pytest numpy
   ```

2. Build and install Python bindings:
   ```bash
   pip install .
   ```

#### Using Modern Python Packaging

```bash
pip install scikit-build-core
python -m build
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
print(f"Synapses: {brain.getTotalSynapseCount()}")
```

## Python API

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

| Method | Description |
|--------|-------------|
| `createBrain(config)` | Create a new brain with configuration |
| `initialize()` | Initialize the brain with configuration |
| `step(step_number)` | Run one simulation step |
| `step(step_number, time)` | Run step with timestamp |
| `receiveSensoryInput(input)` | Inject sensory data |
| `produceAction()` | Get motor action |
| `reset()` | Reset brain state |
| `save(filepath)` | Save brain state |
| `load(filepath)` | Load brain state |
| `addRegion(name)` | Add neural region |
| `getRegion(id)` | Get region by ID |

#### Config Class

| Method | Description |
|--------|-------------|
| `createDefaultConfig()` | Create default configuration |
| `set(key, value)` | Set configuration value |
| `getOr(key, default)` | Get value with default |
| `loadFromFile(filepath)` | Load from JSON file |
| `saveToFile(filepath)` | Save to JSON file |

#### AgentBrain Class

| Method | Description |
|--------|-------------|
| `createAgentBrain(brain)` | Create agent interface |
| `initialize(world)` | Initialize with world |
| `processSensoryInput(percept)` | Process sensory input |
| `decodeMotorCommand()` | Get motor command |
| `applyRewardModulation(reward, predicted)` | Apply reward learning |
| `updateDevelopment(timestep)` | Update developmental state |
| `enableRewardModulation(True/False)` | Enable/disable reward learning |
| `enableStructuralPlasticity(True/False)` | Enable/disable structural changes |
| `enableDevelopment(True/False)` | Enable/disable development |
| `enableCuriosity(True/False)` | Enable/disable exploration |

#### SimpleWorld Class

| Method | Description |
|--------|-------------|
| `createSimpleWorld()` | Create world instance |
| `configure(width, height, visionWidth, visionHeight)` | Set dimensions |
| `reset()` | Reset to initial state |
| `setAgentStart(x, y)` | Set starting position |
| `update(timestep)` | Update world state |
| `applyMotorCommand(cmd, time)` | Apply agent action |
| `getSensoryPercept()` | Get what agent senses |
| `getAgentBody()` | Get agent physical state |
| `addObject(obj)` | Place object in world |
| `isValidPosition(x, y)` | Check if position is accessible |

## Configuration

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential |
| `brain.v_rest` | float | -70.0 | Resting potential |
| `brain.v_reset` | float | -75.0 | Reset potential after spike |
| `brain.tau_mem` | float | 20.0 | Membrane time constant |
| `brain.tau_ref` | float | 2.0 | Refractory period |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP LTP time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP LTD time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Advanced Features

### 1. Advanced Command-Line Options

The main executable `nlm` now supports:

- `--config=FILE` - Load configuration from file
- `--verbose` - Enable verbose logging
- `--benchmark` - Run performance benchmarks
- `--save=path` - Save brain state after simulation
- `--load=path` - Load brain state for simulation
- `--steps=N` - Number of simulation steps
- `--neuron_count=N` - Number of neurons
- `--help` - Display help information

### 2. Performance Monitoring

Built-in performance monitoring tools:

- **Profiling**: Measure execution time of brain components
- **Benchmarking**: Compare different configurations
- **Memory tracking**: Monitor memory usage
- **System metrics**: Track CPU, memory, disk usage
- **Performance analysis**: Generate performance reports

### 3. Advanced Visualization

The visualization system provides:

- **Network visualization**: Graph-based representation of brain connectivity
- **Activity visualization**: Time series of neural firing rates and spikes
- **Weight matrix visualization**: Synaptic weight patterns
- **Multi-panel displays**: Simultaneous visualization of multiple aspects
- **Real-time updates**: Live visualization during simulation
- **Export capabilities**: Save visualizations to files

### 4. Enhanced Experiment Framework

Advanced experiment capabilities:

- **Multi-environment testing**: Run experiments in different world configurations
- **Parameter sweeps**: Systematically vary parameters to study effects
- **Cross-validation**: Test generalization across different conditions
- **Automated data collection**: Record comprehensive metrics
- **Statistical analysis**: Compute summary statistics and confidence intervals

### 5. Advanced Configuration Management

Enhanced configuration system:

- **Hierarchical configuration**: Nested parameter groups
- **Validation**: Type and range checking of parameters
- **Defaults management**: Easy switching between presets
- **Randomized testing**: Generate random parameter sets for exploration
- **Configuration comparison**: Compare different configuration sets

## Visualization

The visualization system provides comprehensive analysis capabilities:

### Network Visualization

Displays the brain as a graph with nodes representing neurons and edges representing synapses. Nodes are colored by type and sized by activity level.

### Activity Visualization

Shows neural firing rates over time with:

- **Firing rate plots**: Time series of average firing rates
- **Spike rasters**: Individual neuron spike trains
- **Heat maps**: Activity patterns across the network
- **Multi-neuron views**: Simultaneous visualization of multiple neurons

### Weight Matrix Visualization

Visualizes synaptic connectivity patterns:

- **Adjacency matrices**: Dense connectivity representations
- **Network graphs**: Force-directed layouts of the brain graph
- **Weight distributions**: Statistical summaries of synaptic weights
- **Path analysis**: Identification of important pathways

### Usage

```python
import pynlm

# Create visualization interface
from pynlm import VisualizationInterface

vis = VisualizationInterface()
vis.initialize()

# Run simulation and collect data
brain = pynlm.createBrain(config)
brain.initialize()

# Update visualization with brain state
vis.updateFromBrain(brain)

# Render visualization
vis.render()
```

## Performance Monitoring

The performance monitoring system provides comprehensive analysis of simulation performance:

### Profiling

Measure execution time of different brain components:

```python
from pynlm import Profiler

profiler = Profiler()
profiler.startProfile("brain_step")
brain.step(step)
profiler.stopProfile("brain_step")

# Export profiling data
profiler.exportProfileToFile("profile.json")
```

### Benchmarking

Compare different configurations:

```python
from pynlm import PerformanceAnalyzer

analyzer = PerformanceAnalyzer()

# Benchmark different neuron counts
for neuron_count in [100, 500, 1000, 5000]:
    config.set("brain.neuron_count", neuron_count)
    result = analyzer.runBenchmark(f"neuron_count_{neuron_count}", 
                                  lambda: run_simulation(config, 100))
    print(f"Neuron count {neuron_count}: {result.executionTime:.2f}s")
```

### System Monitoring

Track system resource usage:

```python
from pynlm import PerformanceMonitor

monitor = PerformanceMonitor()
monitor.startMonitoring(samplingInterval=100)  # Every 100 steps

# Run simulation
for step in range(10000):
    brain.step(step)
    monitor.recordSystemSnapshot()
    monitor.recordNeuralSnapshot(brain)

# Generate performance report
report = monitor.generatePerformanceReport()
print(report)
```

## Troubleshooting

### Common Issues and Solutions

#### "My brain isn't doing anything"

**Possible causes:**
- Did you call `brain.initialize()`?
- Are you calling `brain.step()`?
- Is there sufficient sensory input?

**Solutions:**
- Always call `brain.initialize()` before using the brain
- Call `brain.step()` to run the simulation
- Add `agent.processSensoryInput(percept)` to inject sensory input

#### "The agent isn't moving"

**Possible causes:**
- Did you call `world.applyMotorCommand()`?
- Is `world.update()` being called?
- Is the brain producing motor commands?

**Solutions:**
- Always call `world.applyMotorCommand()` to execute actions
- Ensure `world.update()` is called each step
- Check that the brain is decoding motor commands correctly

#### "Everything is 0"

**Possible causes:**
- The brain needs time to "warm up"
- Neurons need input to fire
- The simulation might be stuck

**Solutions:**
- Run more simulation steps (brains need time to develop)
- Ensure sensory input is connected to the brain
- Check for simulation errors or deadlocks

#### "ImportError: No module named 'pynlm'"

**Solution:**
```bash
pip install --force-reinstall .
```

#### "Segmentation faults"

**Possible causes:**
- Forgetting to call `initialize()`
- Not calling `reset()` between simulations
- Memory corruption

**Solutions:**
- Always call `initialize()` before `step()`
- Call `reset()` before re-running simulations
- Check for null pointer dereferences

### Debugging Techniques

#### Enable Debug Logging

```python
import pynlm

# Create config with debug logging
config = pynlm.createDefaultConfig()
config.set("log_level", "debug")

# Create brain and enable debug
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation - should see detailed debug output
for step in range(100):
    brain.step(step)
```

#### Use Validation Scripts

Run the provided validation scripts:

```bash
# Run unit tests
python -m pytest tests/ -v

# Run integration tests
cd build
ctest --output-on-failure
```

#### Check Configuration

Ensure your configuration is valid:

```python
import pynlm

config = pynlm.createDefaultConfig()
print(config.summary())

# Check for required parameters
required_params = ["brain.neuron_count", "brain.connection_probability"]
for param in required_params:
    if not config.has(param):
        print(f"Missing required parameter: {param}")
```

## Examples

### Example 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Example 2: Brain Watching a World

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

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Example 3: Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

### Example 4: Performance Benchmark

```python
import pynlm
import time

from pynlm import PerformanceAnalyzer

# Setup performance analyzer
analyzer = PerformanceAnalyzer()

# Benchmark different brain sizes
neuron_counts = [100, 500, 1000, 5000]

for neuron_count in neuron_counts:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    
    # Run benchmark
    result = analyzer.runBenchmark(
        f"neurons_{neuron_count}",
        lambda: run_long_simulation(config, 100),
        iterations=5
    )
    
    print(f"Neuron count {neuron_count}: {result.executionTime:.2f}s, "
          f"{result.operationsPerSecond:.0f} ops/sec")

# Export analysis
analyzer.exportAnalysisToFile("performance_report.json")
```

### Example 5: Visualization Demo

```python
import pynlm
from pynlm import VisualizationInterface

def run_visualization_demo():
    # Create brain and visualization
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    vis = VisualizationInterface()
    if vis.initialize():
        print("Visualization initialized successfully")
        
        # Run simulation and visualize
        for step in range(100):
            brain.step(step)
            vis.updateFromBrain(brain)
            vis.render()
            
            if step % 20 == 0:
                print(f"Step {step}, active neurons: {brain.getFiringNeuronCount()}")
        
        print("Visualization demo complete")
    else:
        print("Failed to initialize visualization")

run_visualization_demo()
```

## Next Steps

When you're comfortable with the basics:

1. **Read the Architecture documentation** (`docs/ARCHITECTURE.md`) to understand how it all works
2. **Read the Science documentation** (`docs/SCIENCE.md`) for the scientific background
3. **Experiment with different configurations** in `configs/` directory
4. **Try the Phase 6 integration experiment** using the `nlm` executable
5. **Explore the Python API** further by looking at the examples
6. **Contribute to the project** by adding new features or improving existing ones

That's it! You're now ready to use NLM effectively.
