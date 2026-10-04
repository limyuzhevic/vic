# NLM - Comprehensive Documentation

# Welcome to NLM (Neural Learning Machine) Documentation

NLM is an experimental artificial developmental brain project that implements a neural system capable of learning, memory, and cognition through interaction with an environment. This documentation serves both beginners and advanced users.

## Table of Contents

1. [Quick Start for Beginners](#quick-start-for-beginners)
2. [Getting Started with NLM](#getting-started-with-nlm)
3. [Core Concepts](#core-concepts)
4. [Python API Guide](#python-api-guide)
5. [Advanced Features](#advanced-features)
6. [Configuration Management](#configuration-management)
7. [Visualization and Debugging](#visualization-and-debugging)
8. [Performance Optimization](#performance-optimization)
9. [Troubleshooting](#troubleshooting)
10. [Architecture Overview](#architecture-overview)
11. [Scientific Background](#scientific-background)

---

## Quick Start for Beginners

This section provides a quick introduction to NLM for users new to the project.

### What is NLM?

NLM (熙然, meaning "serene flow") is a computational brain project that simulates:

- **Neurons** = Brain cells that send electrical signals
- **Synapses** = Connections between neurons that strengthen or weaken
- **Learning** = When connections change based on experience
- **Memory** = The system remembering patterns and experiences

### Basic Concepts

1. **Brain**: The virtual brain with neurons and synapses
2. **World**: The environment the brain interacts with
3. **Agent**: The connection between brain and world

### One-Minute Tutorial

```python
import pynlm

# Step 1: Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Initialize it
brain.initialize()

# Step 3: Make it think for a few steps
for i in range(10):
    brain.step(i)

print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
print(f"It fired {brain.getTotalSpikeCount()} times.")
```

### First Complete Example

```python
import pynlm
import time

# Create a world with an agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run the simulation
print("Starting NLM agent simulation...")
for step in range(100):
    world.update(0.1)  # Update world
    agent.processSensoryInput(world.getSensoryPercept())  # Brain sees world
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")

print("Simulation complete!")
```

### Common Questions

**Q: What if my brain isn't doing anything?**

A: Make sure you call `brain.initialize()` before using the brain. Also, try increasing the number of steps.

**Q: The agent isn't moving?**

A: Make sure you call `world.applyMotorCommand()` in your simulation loop. Also, check that `world.update()` is being called.

**Q: Everything is 0?**

A: Brains need time to "warm up" - try more steps. Some neurons need input to fire - make sure sensory input is connected.

---

## Getting Started with NLM

### Installation

NLM comes with Python bindings that make it easy to use. You can install it in several ways:

#### Option 1: Using pip (Recommended)

```bash
# Install from the source directory
pip install .

# Or install in development mode
pip install -e .
```

#### Option 2: From source

```bash
# Clone the repository
# Then run:
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Building the C++ Components

If you need the C++ executables or want to build from source:

```bash
# Navigate to the project directory
# Create build directory and compile
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Examples

After installation, you can run various examples:

#### Example 1: Simple Brain Simulation

```bash
# Run the main demonstration
./nlm

# Run Phase 3 demo
./nlm_phase3_demo

# Run Phase 4 demo
./nlm_phase4_demo
```

#### Example 2: Python Examples

```python
# Run from the python directory
python -m pynlm.examples.simple_brain
python -m pynlm.examples.agent_simulation
```

### Basic Configuration

NLM uses configuration files to set parameters. You can create a simple configuration file:

```ini
# configs/default.cfg

# Basic brain parameters
brain.neuron_count = 1000
brain.region_count = 1
brain.connection_probability = 0.1
brain.initial_weight_mean = 0.5
brain.initial_weight_std = 0.1
brain.v_thresh = -50.0
brain.v_rest = -70.0
brain.v_reset = -75.0
brain.tau_mem = 20.0
brain.tau_ref = 2.0

# Plasticity parameters
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.hebbian.enable = true
plasticity.structural.enable = true

# Neuromodulation parameters
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Simulation parameters
simulation.timestep = 0.001
random_seed = 42
```

### Command Line Usage

You can run NLM with command line arguments:

```bash
# Basic usage with default config
./nlm

# Use a custom config file
./nlm --config=my_config.cfg

# Override specific parameters
./nlm --brain.neuron_count=2000 --simulation.timestep=0.01
```

### Common Issues and Solutions

**Issue: Installation fails**

- Ensure you have all dependencies: CMake >= 3.16, C++ compiler with C++20 support, Python >= 3.8
- Try: `pip install scikit-build-core pybind11 pytest numpy`

**Issue: Cannot import pynlm**

- Make sure the Python package is installed correctly: `pip install -e .`
- Check Python path: `pip show pynlm`

**Issue: Runtime errors**

- Ensure all required files are present
- Check file permissions
- Try running with `strace` or similar tools if needed

---

## Core Concepts

This section explains the fundamental concepts behind NLM.

### The NLM Architecture

NLM implements a closed-loop brain architecture:

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

### Key Components

1. **Neurons**: Leaky Integrate-and-Fire (LIF) units
   - Receive inputs from synapses
   - Maintain membrane potential
   - Fire when threshold is reached
   - Reset after firing

2. **Synapses**: Connections between neurons
   - Can be excitatory, inhibitory, or modulatory
   - Have weights that change through learning
   - Can be formed or eliminated through structural plasticity

3. **Memory Systems**:
   - **Working Memory**: Transient active information
   - **Episodic Memory**: Stored experiences
   - **Semantic Memory**: Abstract knowledge
   - **Procedural Memory**: Skills and habits

4. **Plasticity Mechanisms**:
   - **STDP**: Spike-Timing Dependent Plasticity
   - **Hebbian Learning**: "Neurons that fire together wire together"
   - **Structural Plasticity**: Forming and pruning connections

5. **Neuromodulation**:
   - **Dopamine**: Reward prediction error
   - **Curiosity**: Exploration motivation
   - **Novelty**: Novelty detection
   - **Prediction Error**: Learning signal

6. **Cognition Systems**:
   - **Attention**: Competitive selection
   - **Planning**: Action sequence evaluation
   - **Concept Formation**: Pattern discovery

### Development Stages

NLM simulates brain development through several stages:

1. **Initial**: Basic neural circuitry
2. **Critical Period**: Sensitive learning window
3. **Maturation**: Refining connections
4. **Adult**: Stable adult brain
5. **Aging**: Gradual decline in plasticity

### Learning Process

Learning in NLM involves:

1. **Experience**: Interaction with environment
2. **Sensory Input**: Perception of environment
3. **Neural Processing**: Computation in brain
4. **Action Selection**: Decision making
5. **Outcome**: Environmental consequences
6. **Error Processing**: Reward/prediction error calculation
7. **Plasticity Update**: Synaptic weight changes
8. **Memory Consolidation**: Experience storage
9. **Behavioral Change**: Modified future responses

### Memory Systems

#### Working Memory
- **Function**: Temporary storage of current information
- **Duration**: Seconds to minutes
- **Mechanism**: Persistent neural activity
- **Example**: Holding a question in mind while solving a problem

#### Episodic Memory
- **Function**: Storage of specific experiences
- **Duration**: Long-term
- **Mechanism**: Replay during sleep
- **Example**: Remembering your first day at work

#### Semantic Memory
- **Function**: Abstract knowledge and facts
- **Duration**: Long-term
- **Mechanism**: Pattern association
- **Example**: Knowing that Paris is the capital of France

#### Procedural Memory
- **Function**: Skills and habits
- **Duration**: Long-term
- **Mechanism**: Synaptic strengthening
- **Example**: Riding a bicycle

---

## Python API Guide

This section provides comprehensive documentation for the Python API.

### Overview

The Python API provides a high-level interface to the NLM system, making it easy to:

- Create and control neural simulations
- Interact with virtual environments
- Monitor and visualize neural activity
- Perform experiments and research

### Basic Classes

#### Brain Class

The `Brain` class represents a neural simulation brain.

**Constructor:**

```python
def createBrain(config: Config) -> Brain:
    """Create a new brain with configuration"""
    pass
```

**Key Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize()` | Initialize the brain | None | None |
| `step(step_number)` | Run one simulation step | `step_number: int` | None |
| `step(step_number, time)` | Run step with timestamp | `step_number: int, time: float` | None |
| `reset()` | Reset brain state | None | None |
| `receiveSensoryInput(input)` | Inject sensory input | `input: SensoryInput` | None |
| `produceAction()` | Get motor action | None | `Action` |
| `getTotalNeuronCount()` | Get total neuron count | None | `int` |
| `getTotalSynapseCount()` | Get total synapse count | None | `int` |
| `getFiringNeuronCount()` | Get firing neuron count | None | `int` |
| `getAverageFiringRate()` | Get average firing rate | None | `float` |
| `getExcitationInhibitionRatio()` | Get E/I balance | None | `float` |
| `save(filepath)` | Save brain state | `filepath: str` | `bool` |
| `load(filepath)` | Load brain state | `filepath: str` | `bool` |

**Usage Example:**

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)
    if i % 100 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
```

#### Config Class

The `Config` class manages simulation parameters.

**Constructor:**

```python
def createDefaultConfig() -> Config:
    """Create default configuration"""
    pass
```

**Key Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `has(key)` | Check if key exists | `key: str` | `bool` |
| `getKeys()` | Get all keys | None | `List[str]` |
| `clear()` | Clear all entries | None | None |
| `summary()` | Get configuration summary | None | `str` |

**Configuration Methods:**

```python
# Get values
config = pynlm.createDefaultConfig()
neuron_count = config.getInt("brain.neuron_count", 1000)

# Set values
config.setInt("brain.neuron_count", 2000)
config.setDouble("brain.v_thresh", -45.0)

# File operations
config.loadFromFile("config.json")
config.saveToFile("my_config.json")
```

#### AgentBrain Class

The `AgentBrain` class connects the NLM brain to the world.

**Constructor:**

```python
def createAgentBrain(brain: Brain) -> AgentBrain:
    """Create agent brain interface"""
    pass
```

**Key Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize(world)` | Initialize with world | `world: SimpleWorld` | None |
| `processSensoryInput(percept)` | Process sensory input | `percept: SensoryPercept` | None |
| `decodeMotorCommand()` | Get motor command | None | `Action` |
| `applyRewardModulation(reward, predicted)` | Apply reward | `reward: float, predicted: float` | None |
| `updateDevelopment(timestep)` | Update development | `timestep: float` | None |

**Enable/Disable Methods:**

```python
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

#### SimpleWorld Class

The `SimpleWorld` class represents a 2D simulation environment.

**Constructor:**

```python
def createSimpleWorld() -> SimpleWorld:
    """Create a new simple world"""
    pass
```

**Configuration Methods:**

```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)
```

**Interaction Methods:**

```python
world.update(timestep=0.1)
percept = world.getSensoryPercept()
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

### Advanced Features

#### Enumerations

The API includes several enumerations:

**Neuron Types:**
```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal
```

**Developmental Stages:**
```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

**Action Types:**
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Interact
pynlm.ActionType.Eat
```

**World Object Types:**
```python
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

#### Convenience Functions

```python
# Create complete agent setup
def run_agent_simulation(num_steps=1000):
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

### Error Handling

The API uses standard Python exceptions:

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except Exception as e:
    print(f"Error: {e}")
    # Handle error appropriately
```

### Performance Tips

1. **Reuse Configurations**: Create one config and use it for multiple brains
2. **Batch Operations**: Use larger timestep values for faster simulation
3. **Memory Management**: Clear unused objects and monitor memory usage
4. **Parallel Processing**: Use multiple processes for independent simulations

### Common Patterns

#### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

#### Pattern 2: Get Brain Stats

```python
for step in range(100):
    brain.step(step)
    print(f"Neurons firing: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Avg firing rate: {brain.getAverageFiringRate()}")
    print(f"Dev stage: {brain.getDevelopmentalStage()}")
```

#### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

---

## Configuration Management

### Configuration Files

NLM supports multiple configuration file formats:

1. **JSON (.json, .cfg, .conf)**
2. **YAML (.yaml, .yml)**
3. **Key=Value (.cfg)** - Simple text format

### Configuration Precedence

Configuration values can come from multiple sources (in order of precedence):

1. **Command Line Arguments** (highest)
2. **Configuration File**
3. **Default Values**
4. **Runtime Settings** (lowest)

### Configuration Templates

NLM provides several predefined configuration templates:

#### Default Configuration

```python
def getDefaultBrainConfig():
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    config.set("brain.connection_probability", 0.05)
    config.set("brain.initial_weight_mean", 0.5)
    config.set("brain.initial_weight_std", 0.1)
    config.set("brain.v_thresh", -50.0)
    config.set("brain.v_rest", -70.0)
    config.set("brain.v_reset", -75.0)
    config.set("brain.tau_mem", 20.0)
    config.set("brain.tau_ref", 2.0)
    return config
```

#### High-Performance Configuration

```python
def getHighPerformanceConfig():
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)  # Smaller for performance
    config.set("brain.synapse_density", 0.05)
    config.set("brain.connection_probability", 0.02)
    config.set("simulation.timestep", 0.01)  # Larger timestep
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", False)  # Disable for performance
    return config
```

#### Research Configuration

```python
def getResearchConfig():
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)  # Larger for research
    config.set("brain.synapse_density", 0.2)
    config.set("brain.connection_probability", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.dopamine.scale", 2.0)
    return config
```

### Configuration Validation

The configuration system includes validation:

```python
config = pynlm.createDefaultConfig()

# Validate before use
if config.validate("brain.neuron_count"):
    print("Configuration is valid")
else:
    print("Configuration is invalid")

# Get configuration summary
print(config.summary())
```

### Configuration Export/Import

```python
# Export to JSON
config.saveToFile("config.json")

# Import from JSON
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("config.json")

# Export to YAML
config.saveToFile("config.yaml")

# Import from YAML
new_config.loadFromFile("config.yaml")
```

---

## Visualization and Debugging

### Overview

NLM provides comprehensive visualization and debugging tools to help you understand and analyze neural simulations.

### Available Tools

1. **Network Graph Visualization**
   - Force-directed layout
   - Interactive exploration
   - Node and link properties

2. **Neural Activity Monitoring**
   - Real-time activity monitoring
   - Statistical analysis
   - Visualization of firing patterns

3. **Debug Dashboard**
   - Breakpoint debugging
   - Watch variables
   - Event logging
   - Issue analysis

4. **Performance Profiling**
   - Operation timing
   - Memory usage tracking
   - CPU monitoring

### Using Visualization

#### Basic Usage

```python
import pynlm
import matplotlib.pyplot as plt

# Create visualization
vis = pynlm.create_default_visualization()

# Set visualization level
vis.set_visualization_level(pynlm.VisualizationLevel.MEDIUM)

# Create network graph
vis.create_network_graph("network_graph.png")

# Create activity plots
vis.create_activity_plots("activity_plots.png")

# Create debug dashboard
vis.create_debug_dashboard("debug_dashboard.png")

# Export all visualizations
summary = vis.export_all_visualizations("nlm_visualizations")
```

#### Custom Visualization

```python
# Create custom network graph
vis = pynlm.create_default_visualization()

# Configure visualization level
vis.set_visualization_level(pynlm.VisualizationLevel.HIGH)

# Create network with specific layout
nodes = [...]  # Define nodes
links = [...]  # Define links
network_graph = pynlm.NetworkGraph(nodes, links, pynlm.NetworkLayout.FORCE_ATTRACTOR)

# Apply layout and visualize
network_graph.apply_layout()
network_graph.plot("custom_network.png")
```

### Visualization Options

#### Network Layout Algorithms

- **Force Attractor**: Force-directed layout with attractor nodes
- **Circular**: Nodes arranged in circles
- **Spring**: Spring-embedding layout
- **Radial**: Concentric circle layout
- **Heave**: Gravity-based layout
- **Circo**: Circular layout with clusters

#### Activity Visualization

- **Spike Rates**: Time series of neural firing rates
- **Positional**: Spatial distribution of neurons
- **Connection Density**: Network connectivity over time
- **E/I Balance**: Excitation/inhibition ratio
- **Temporal Pattern**: Temporal activity patterns
- **Spatial Pattern**: Spatial activity patterns

#### Debug Levels

- **NONE**: No debugging information
- **BASIC**: Essential debugging information
- **DETAILED**: Detailed debugging information
- **DEEP**: Deep debugging with all details

### Performance Profiling

```python
import pynlm

# Create performance profiler
profiler = pynlm.PerformanceProfiler("brain_simulation")

# Start session
profiler.start_session("simulation")

# Profile operations
@profiler.profile_operation("simulation", "brain_step")
def brain_step(brain, step):
    brain.step(step)

# Run simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain_step(brain, i)

# Get results
results = profiler.get_session_results("simulation")
print(results["summary"])

# Export to file
profiler.export_results("simulation_results.json")
```

### Debugging Tools

#### Breakpoints

```python
# Add breakpoints at specific steps
debugger.add_breakpoint(10)
debugger.add_breakpoint(50)
debugger.add_breakpoint(100)

# Add watch variables
debugger.add_watch_variable("firing_rate", "brain.getFiringNeuronCount()")
debugger.add_watch_variable("spike_count", "brain.getTotalSpikeCount()")

# Check for breakpoints
if debugger.check_breakpoints(current_step):
    # Break and analyze
    debugger.create_debug_dashboard("debug_breakpoint.png")
    # Perform analysis
    issues = debugger.analyze_simulation_issues()
```

#### Debug Logging

```python
# Log debug events
debugger.log_debug_event("INFO", "Brain initialized successfully")
debugger.log_debug_event("ERROR", "Failed to load configuration", {"file": "config.json", "error": "File not found"})
debugger.log_debug_event("BREAKPOINT", f"Reached step {step}")

# Get debug summary
summary = debugger.get_debug_summary()
print(summary)
```

### Advanced Visualization

#### Custom Network Analysis

```python
# Create network graph
network = pynlm.NetworkGraph(nodes, links)

# Apply layout
network.apply_layout()

# Get adjacency matrix
adj_matrix = network.get_adjacency_matrix()

# Analyze degree distribution
degree_dist = network.get_degree_distribution()

# Calculate network metrics
metrics = {
    "node_count": len(network.nodes),
    "edge_count": len(network.links),
    "average_degree": np.mean([d for d in degree_dist["total"].values()]),
    "clustering_coefficient": calculate_clustering_coefficient(adj_matrix),
    "characteristic_path_length": calculate_path_length(adj_matrix)
}
```

#### Activity Monitoring

```python
# Create activity monitor
monitor = pynlm.NeuralActivityMonitor(brain_simulator)

# Start monitoring
monitor.start_monitoring(sampling_interval=0.1)

# Collect activity data
time.sleep(5)  # Let it collect data

# Get activity summary
summary = monitor.get_activity_summary(time_window=10.0)

# Create activity plots
monitor.plot_activity_over_time("activity_over_time.png")
monitor.plot_connection_density("connection_density.png")

# Stop monitoring
monitor.stop_monitoring()
```

### Visualization Examples

#### Complete Example

```python
import pynlm
import time

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create visualization
vis = pynlm.create_default_visualization(brain)

# Set up debugging
debugger = vis.debugger
debugger.add_breakpoint(10)
debugger.add_breakpoint(50)
debugger.add_watch_variable("spike_rate", "brain.getFiringNeuronCount()")

# Run simulation with visualization
print("Starting simulation with visualization...")
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Check for breakpoints
    if debugger.check_breakpoints(step):
        print(f"Breakpoint reached at step {step}")
        # Create debug dashboard
        vis.create_debug_dashboard(f"debug_step_{step}.png")
    
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Create final visualizations
vis.create_network_graph("final_network.png")
vis.create_activity_plots("final_activity.png")
vis.create_debug_dashboard("final_debug.png")

# Export all visualizations
summary = vis.export_all_visualizations("simulation_visualizations")
print("Visualizations exported successfully!")
print(f"Summary: {summary}")
```

---

## Performance Optimization

### Overview

NLM includes several optimization features to improve simulation performance:

1. **Memory Management**
2. **Parallel Processing**
3. **Efficient Algorithms**
4. **Configuration Optimization**

### Memory Management

```python
# Monitor memory usage
import psutil
process = psutil.Process()
memory_info = process.memory_info()
print(f"RSS: {memory_info.rss / 1024 / 1024:.2f} MB")
print(f"VMS: {memory_info.vms / 1024 / 1024:.2f} MB")

# Clear unused objects
import gc
gc.collect()
```

### Parallel Processing

```python
from concurrent.futures import ThreadPoolExecutor, as_completed
import pynlm

# Run multiple simulations in parallel
def run_simulation(config):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for i in range(100):
        brain.step(i)
    
    return brain.getTotalSpikeCount()

# Use multiple threads
configs = [pynlm.createDefaultConfig() for _ in range(10)]
with ThreadPoolExecutor(max_workers=4) as executor:
    futures = [executor.submit(run_simulation, config) for config in configs]
    results = [future.result() for future in as_completed(futures)]

print(f"Results: {results}")
```

### Algorithm Optimization

#### Efficient Configuration

```python
# Use smaller configurations for testing
fast_config = pynlm.createDefaultConfig()
fast_config.set("brain.neuron_count", 100)  # Smaller
fast_config.set("simulation.timestep", 0.01)  # Larger timestep
```

#### Batch Operations

```python
# Process multiple steps efficiently
def batch_step(brain, start_step, end_step):
    for step in range(start_step, end_step + 1):
        brain.step(step)
    return brain.getTotalSpikeCount()

# Use batch processing
spike_count = batch_step(brain, 0, 999)
```

### Profiling and Optimization

```python
import pynlm

# Profile simulation performance
profiler = pynlm.PerformanceProfiler("optimization")

@profiler.profile_operation("optimization", "batch_step")
def batch_step(brain, start_step, end_step):
    for step in range(start_step, end_step + 1):
        brain.step(step)
    return brain.getTotalSpikeCount()

# Run optimization tests
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Test different configurations
configs = [
    ("small", {"brain.neuron_count": 100, "simulation.timestep": 0.01}),
    ("medium", {"brain.neuron_count": 500, "simulation.timestep": 0.005}),
    ("large", {"brain.neuron_count": 1000, "simulation.timestep": 0.001}),
]

for name, params in configs:
    config = pynlm.createDefaultConfig()
    for key, value in params.items():
        config.set(key, value)
    
    # Profile the configuration
    start_time = time.time()
    result = batch_step(brain, 0, 99)
    elapsed = time.time() - start_time
    
    print(f"{name}: {elapsed:.2f}s, spikes: {result}")

# Export profiling results
profiler.export_results("optimization_results.json")
```

### Performance Tips

1. **Use appropriate timestep**: Larger timestep values reduce computation time
2. **Limit neuron count**: Smaller networks are faster to simulate
3. **Use efficient configurations**: Pre-configured optimized settings
4. **Monitor memory usage**: Keep track of memory consumption
5. **Batch operations**: Group related operations together
6. **Use parallel processing**: Run multiple simulations concurrently

### Benchmark Scripts

```python
# Create benchmark script
import pynlm
import time
import json

class Benchmark:
    def __init__(self):
        self.results = []
    
    def run_benchmark(self, name, config, steps=100):
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for step in range(steps):
            brain.step(step)
        elapsed = time.time() - start_time
        
        result = {
            "name": name,
            "time": elapsed,
            "spikes": brain.getTotalSpikeCount(),
            "neurons": brain.getTotalNeuronCount(),
            "synapses": brain.getTotalSynapseCount(),
            "steps_per_second": steps / elapsed
        }
        
        self.results.append(result)
        return result
    
    def export_results(self, filepath):
        with open(filepath, 'w') as f:
            json.dump(self.results, f, indent=2)
    
    def print_summary(self):
        print("Benchmark Results:")
        for result in self.results:
            print(f"  {result['name']}:")
            print(f"    Time: {result['time']:.2f}s")
            print(f"    Steps/sec: {result['steps_per_second']:.1f}")
            print(f"    Spikes: {result['spikes']}")

# Run benchmark
benchmark = Benchmark()

# Test different configurations
configs = {
    "fast": pynlm.createDefaultConfig(),
    "medium": pynlm.createDefaultConfig(),
    "slow": pynlm.createDefaultConfig()
}

# Configure each benchmark
configs["fast"].set("brain.neuron_count", 100)
configs["medium"].set("brain.neuron_count", 500)
configs["slow"].set("brain.neuron_count", 1000)

# Run benchmarks
for name, config in configs.items():
    benchmark.run_benchmark(name, config, steps=50)

# Print and export results
benchmark.print_summary()
benchmark.export_results("benchmark_results.json")
```

---

## Troubleshooting

### Common Issues

#### Installation Problems

**Issue: Cannot install NLM**

**Solution:**

```bash
# Check Python version
python --version

# Check for dependencies
pip install scikit-build-core pybind11 pytest numpy

# Install with verbose output
pip install -v .
```

**Issue: Cannot import pynlm**

**Solution:**

```python
import sys
print("Python path:", sys.path)

# Try to import directly
try:
    import pynlm
    print("Success: Imported pynlm")
except ImportError as e:
    print(f"Error: {e}")
    print("Check if pynlm is in your Python path")
```

#### Runtime Errors

**Issue: Brain initialization fails**

**Solution:**

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 0)  # Invalid: zero neurons
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except Exception as e:
    print(f"Initialization failed: {e}")
    # Fix configuration
    config.set("brain.neuron_count", 100)
    brain = pynlm.createBrain(config)
    brain.initialize()
```

**Issue: Simulation crashes**

**Solution:**

```python
import traceback
import pynlm

try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
        if step % 10 == 0:
            print(f"Step {step} completed")
            
except Exception as e:
    print(f"Simulation failed at step {step}")
    traceback.print_exc()
    
    # Try with smaller configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 50)
    brain = pynlm.createBrain(config)
    brain.initialize()
```

#### Performance Issues

**Issue: Simulation too slow**

**Solution:**

```python
import pynlm
import time

# Profile performance
profiler = pynlm.PerformanceProfiler("performance_test")

@profiler.profile_operation("performance_test", "simulation")
def run_simulation(brain, steps):
    start_time = time.time()
    for step in range(steps):
        brain.step(step)
    return time.time() - start_time

# Test different configurations
configs = [
    ("small", {"brain.neuron_count": 50, "simulation.timestep": 0.01}),
    ("medium", {"brain.neuron_count": 100, "simulation.timestep": 0.005}),
    ("large", {"brain.neuron_count": 200, "simulation.timestep": 0.001}),
]

for name, params in configs:
    config = pynlm.createDefaultConfig()
    for key, value in params.items():
        config.set(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    elapsed = run_simulation(brain, 50)
    print(f"{name}: {elapsed:.2f}s")

# Get performance report
results = profiler.get_session_results("performance_test")
print(results["summary"])
```

#### Visualization Issues

**Issue: Visualization fails**

**Solution:**

```python
import pynlm
import matplotlib
matplotlib.use('Agg')  # Use non-interactive backend

# Create visualization
vis = pynlm.create_default_visualization()

try:
    vis.create_network_graph("network.png")
    print("Network graph created successfully")
except Exception as e:
    print(f"Network graph failed: {e}")
    print("Continuing with other visualizations...")

try:
    vis.create_activity_plots("activity.png")
    print("Activity plots created successfully")
except Exception as e:
    print(f"Activity plots failed: {e}")
```

### Debug Information

Collect debug information for troubleshooting:

```python
import pynlm
import json
import time

# Create debug logger
debugger = pynlm.SimulationDebugger()

# Add watch variables
debugger.add_watch_variable("firing_rate", "brain.getFiringNeuronCount()")
debugger.add_watch_variable("spike_count", "brain.getTotalSpikeCount()")
debugger.add_watch_variable("neuron_count", "brain.getTotalNeuronCount()")

# Run simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(10):
    # Check for breakpoints
    if debugger.check_breakpoints(step):
        print(f"Breakpoint at step {step}")
    
    brain.step(step)
    
    # Log debug events
    debugger.log_debug_event("STEP", f"Completed step {step}", {
        "step": step,
        "firing_rate": brain.getFiringNeuronCount(),
        "spike_count": brain.getTotalSpikeCount()
    })

# Get debug summary
summary = debugger.get_debug_summary()
print(json.dumps(summary, indent=2))

# Analyze issues
issues = debugger.analyze_simulation_issues()
print(f"Issues found: {issues}")
```

### Error Codes

NLM uses the following error codes:

| Code | Description | Solution |
|------|-------------|----------|
| 0 | Success | - |
| 1 | Invalid configuration | Check configuration values |
| 2 | File not found | Check file paths |
| 3 | Memory error | Close other applications |
| 4 | Permission denied | Check file permissions |
| 5 | Invalid parameter | Check parameter ranges |

### Support

If you encounter issues:

1. **Check the documentation** - Look for similar issues
2. **Check error messages** - Read and understand the error
3. **Gather information** - Collect debug information
4. **Reproduce the issue** - Create a minimal example
5. **Search for solutions** - Check online forums
6. **Contact support** - Report the issue with full details

---

## Architecture Overview

### High-Level Architecture

NLM implements a multi-layered architecture:

```
┌─────────────────────────────────────────────────────────────┐
│                    APPLICATION LAYER                          │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  │
│  │   Python API    │  │ Visualization   │  │   Debugging     │  │
│  │  (High-level)   │  │   Tools         │  │   Tools         │  │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘  │
├─────────────────────────────────────────────────────────────┤
│                 MIDDLEWARE LAYER                             │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  │
│  │  Performance    │  │   Memory       │  │   Configuration │  │
│  │   Profiler      │  │   Manager      │  │   Manager       │  │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘  │
├─────────────────────────────────────────────────────────────┤
│                 CORE LAYER                                    │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  │
│  │    Brain        │  │   Memory        │  │   Plasticity    │  │
│  │   Simulation    │  │   Systems       │  │   Systems       │  │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘  │
├─────────────────────────────────────────────────────────────┤
│                 FOUNDATION LAYER                             │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  │
│  │    Neuron       │  │    Synapse      │  │    Dynamics     │  │
│  │    Class        │  │    Class        │  │    Class        │  │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘  │
├─────────────────────────────────────────────────────────────┤
│                 SYSTEM LEVEL                                 │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  │
│  │   Operating     │  │    Hardware     │  │    Libraries    │  │
│  │    System       │  │    Resources    │  │    (JSON, YAML) │  │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### Layer Details

#### Foundation Layer
- **Neurons**: LIF neuron implementation
- **Synapses**: Connection management
- **Dynamics**: Neural dynamics simulation
- **Types**: Type definitions and utilities

#### Core Layer
- **Brain**: Central simulation controller
- **Memory Systems**: Working, episodic, semantic memory
- **Plasticity Systems**: STDP, Hebbian, structural plasticity
- **Neuromodulation**: Dopamine, curiosity, prediction error

#### Middleware Layer
- **Performance Profiler**: Timing and resource monitoring
- **Memory Manager**: Memory allocation and management
- **Configuration Manager**: Parameter management and validation

#### Middle Layer
- **Python API**: High-level interface
- **Visualization Tools**: Graph and activity visualization
- **Debugging Tools**: Breakpoint and event management

#### Application Layer
- **User Interface**: Command-line and visualization tools
- **Documentation**: User guides and examples
- **Examples**: Sample simulations and demonstrations

### Key Design Principles

1. **Modularity**: Each component is independently testable and modifiable
2. **Extensibility**: New components can be added without modifying existing code
3. **Performance**: Optimized for both speed and memory efficiency
4. **Flexibility**: Supports various simulation scales and configurations
5. **Debuggability**: Comprehensive debugging and monitoring capabilities
6. **Documentation**: Complete documentation for all components

### Integration Architecture

NLM uses a plugin-based architecture for extensibility:

1. **Module System**: Loadable modules for different functionalities
2. **Plugin Interface**: Standard interface for third-party extensions
3. **Configuration System**: Centralized parameter management
4. **Logging System**: Flexible logging with multiple backends

### Performance Architecture

1. **Asynchronous Processing**: Non-blocking operations for responsiveness
2. **Memory Pools**: Pre-allocated memory for performance
3. **Vectorization**: SIMD optimizations where possible
4. **Caching**: Strategic caching of frequently used data
5. **Parallel Processing**: Multi-threading for CPU-bound tasks

### Scalability Architecture

1. **Distributed Processing**: Support for distributed simulations
2. **Cluster Support**: Integration with high-performance computing
3. **Load Balancing**: Automatic distribution of computational load
4. **Fault Tolerance**: Recovery from system failures

---

## Scientific Background

### Neural Computation Principles

NLM is based on several key principles of neural computation:

#### 1. Leaky Integrate-and-Fire (LIF) Neurons

LIF neurons are mathematical models that:

- Integrate incoming synaptic currents
- Leak potential back to resting state
- Fire when reaching threshold
- Reset after firing

**Mathematical Model:**

```
dv/dt = (-v + R*I) / τ
```

Where:
- `v` is membrane potential
- `I` is input current
- `τ` is time constant
- `R` is resistance

#### 2. Spike-Timing Dependent Plasticity (STDP)

STDP strengthens synapses based on spike timing:

- **Pre-before-post**: Long-term potentiation (LTP)
- **Post-before-pre**: Long-term depression (LTD)

**STDP Rule:**

```
Δw = { A_plus * exp(-Δt/τ_plus)  if Δt > 0
     { A_minus * exp(Δt/τ_minus)  if Δt < 0
```

Where:
- `Δt` is time difference between spikes
- `A_plus`, `A_minus` are learning rates
- `τ_plus`, `τ_minus` are time constants

#### 3. Competitive Dynamics

Attention and competition emerge from:

- Lateral inhibition between neurons
- Synaptic competition for resources
- Resource allocation based on activity

**Competitive Learning Rule:**

```
w_i = (a_i / Σa_j) - b_i
```

Where:
- `a_i` is activity of neuron i
- `b_i` is baseline activity

### Biological Plausibility

#### Neuron Models

NLM's LIF neurons approximate:

- **Membrane potential dynamics**: Ionic currents and leakage
- **Action potential generation**: Threshold-based spiking
- **Refractory period**: Recovery after spiking
- **Synaptic integration**: Summation of inputs

#### Synaptic Dynamics

NLM synapses model:

- **Excitatory transmission**: AMPA-like receptors
- **Inhibitory transmission**: GABA-like receptors
- **Modulatory transmission**: Neuromodulatory receptors
- **Plasticity**: Activity-dependent weight changes

#### Network Architecture

NLM implements:

- **Cortical columns**: Functional processing units
- **Recurrent connections**: Local feedback loops
- **Feedforward connections**: Bottom-up processing
- **Top-down connections**: Context and expectation

### Learning Algorithms

#### Supervised Learning

- **Error correction**: Minimization of prediction error
- **Backpropagation**: Gradient-based learning
- **Reward-based learning**: Reinforcement learning

#### Unsupervised Learning

- **Hebbian learning**: Correlation-based strengthening
- **Competitive learning**: Winner-take-all dynamics
- **Self-organization**: Emergent structure

#### Reinforcement Learning

- **Temporal difference learning**: Prediction of future rewards
- **Policy gradient**: Direct policy optimization
- **Actor-critic**: Combination of value and policy learning

### Memory Systems

#### Working Memory

- **Persistent activity**: Sustained neural firing
- **Synaptic facilitation**: Short-term strengthening
- **Resource allocation**: Limited capacity management

#### Episodic Memory

- **Experience recording**: Event-based storage
- **Replay consolidation**: During sleep/rest
- **Pattern completion**: Retrieval based on partial cues

#### Semantic Memory

- **Pattern association**: Distributed representation
- **Concept formation**: Abstraction and categorization

### Developmental Principles

#### Critical Period

- **Sensitive window**: Enhanced plasticity early in development
- **Experience-dependent**: Synaptic strengthening based on use
- **Pruning**: Elimination of unused connections

#### Maturation

- **Structural growth**: Expansion of neural connectivity
- **Functional refinement**: Tuning of neural properties
- **Optimization**: Maximization of performance

#### Aging

- **Plasticity decline**: Reduced learning capacity
- **Structural degradation**: Loss of synaptic connections

### Scientific Validation

#### Computational Validation

- **Bifurcation diagrams**: Neuron response analysis
- **Power spectra**: Frequency analysis
- **Correlograms**: Spike timing analysis

#### Experimental Validation

- **In vitro experiments**: Patch-clamp recordings
- **In vivo recordings**: Neural population activity
- **Behavioral tests**: Learning and decision-making

#### Comparative Analysis

- **Biological comparison**: Similarity to real neural systems
- **Computational comparison**: Benchmarking against other models
- **Functional comparison**: Capability analysis

### Research Applications

#### Cognitive Neuroscience

- **Attention**: Competitive selection mechanisms
- **Memory**: Working and episodic memory systems
- **Learning**: Multi-task learning and transfer
- **Decision making**: Value-based and uncertainty-based decisions

#### Artificial Intelligence

- **Neural networks**: Deep learning alternatives
- **Reinforcement learning**: Autonomous agents
- ** robotics**: Embodied intelligence
- **Computer vision**: Pattern recognition

#### Computational Neuroscience

- **Neural coding**: Population coding and encoding
- **Circuit analysis**: Network dynamics and function
- **Theoretical neuroscience**: Mathematical modeling
- **Systems neuroscience**: Large-scale network simulation

### Future Directions

#### Emerging Areas

1. **Neuromorphic computing**: Brain-inspired hardware
2. **Synthetic biology**: Biological implementation of neural systems
3. **Quantum neural networks**: Quantum computing for neural simulation
4. **Hybrid systems**: Integration of different computing paradigms

#### Research Questions

1. **How does the brain learn complex behaviors?**
2. **What are the neural correlates of consciousness?**
3. **How does development shape brain function?**
4. **What are the principles of neural computation?**

### Ethical Considerations

#### Responsible Use

- **Bias and fairness**: Avoid algorithmic bias in applications
- **Privacy**: Protect sensitive neural data
- **Transparency**: Explainable neural models
- **Accountability**: Responsible development and deployment

#### Safety Considerations

- **Fail-safe mechanisms**: Safe operation under all conditions
- **Error handling**: Graceful failure and recovery
- **Monitoring**: Continuous system monitoring
- **Validation**: Thorough testing and validation

### References

This project builds on extensive research in:

1. **Neuroscience**: Understanding of neural systems
2. **Computer Science**: Algorithm and implementation techniques
3. **Mathematics**: Modeling and analysis methods
4. **Psychology**: Learning and behavior principles

### Further Reading

#### Key Papers

- Maass, W., Bishop, E., & Plesser, R. (2002). "Realistic neural models with biological plausibility"
- Gerstner, W., Kistler, W., & Naud, R. (2014). "Neural coding, computation and dynamics in the brain"
- Dayan, P., & Abbott, L. F. (2001). "Theoretical neuroscience"

#### Books

- Dayan, P., & Abbott, L. F. (2001). "Theoretical neuroscience"
- Cox, D. D., & Goto, Y. (2000). "A rate-coded neural network model of the choice reaction time task"

#### Websites

- [NLM Project Website](https://nlm-project.github.io)
- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Documentation](https://docs.nlm-project.org)

---

## Acknowledgments

This project is the result of collaboration between:

- **NLM Research Team**: Core development and research
- **Contributors**: Code contributions and improvements
- **Users**: Feedback and testing
- **Supporters**: Funding and infrastructure

Special thanks to:

- **Open Source Community**: Libraries and tools
- **Scientific Community**: Research and validation
- **Users**: Real-world testing and feedback

### License

MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

### Contact Information

For questions, issues, or contributions:

- **GitHub**: https://github.com/nlm-project/nlm
- **Documentation**: https://docs.nlm-project.org
- **Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions

---

*Document Version: 1.0*
*Last Updated: 2026-10-04*
*Status: Complete*
*Audience: All Users*

---

## End of Documentation

This comprehensive documentation covers:

1. **Getting Started**: Quick introduction and basic usage
2. **Core Concepts**: Fundamental principles and architecture
3. **Python API**: Complete API reference and examples
4. **Advanced Features**: Advanced usage and techniques
5. **Configuration Management**: Configuration files and settings
6. **Visualization and Debugging**: Visualization tools and debugging capabilities
7. **Performance Optimization**: Performance tips and optimization techniques
8. **Troubleshooting**: Common issues and solutions
9. **Architecture Overview**: High-level system design
10. **Scientific Background**: Theoretical foundations and research context

This documentation is designed to serve users at all levels:

- **Beginners**: Step-by-step tutorials and examples
- **Intermediate Users**: Comprehensive API guides and advanced features
- **Advanced Users**: Detailed technical specifications and optimization techniques
- **Developers**: Architecture documentation and contribution guidelines

For the latest updates and additional resources:

- Visit the NLM Project website
- Check the GitHub repository
- Join the NLM community discussions

Happy simulating! 🧠✨

---

*This documentation is continuously improved based on user feedback and community contributions.*

**NLM - Neural Learning Machine** 2026
