# NLM Python Package - Quick Start

## Overview

This package provides Python bindings for NLM (Neural Learning Machine), a computational brain simulation framework. The bindings enable researchers and developers to create, simulate, and analyze artificial brains using Python.

## Installation

### From Source

```bash
# Install the package in development mode
pip install -e .

# Or install with all optional dependencies
pip install -e .[dev]
```

### Prerequisites

- Python 3.8 or higher
- C++ compiler with C++20 support
- CMake 3.16 or higher

The package includes the C++ NLM framework as a dependency, which will be built automatically during installation.

## Basic Usage

### 1. Quick Start (3 lines)

```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
```

### 2. Complete Simulation Example

```python
import pynlm

# Setup configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation complete! {brain.getTotalSpikeCount()} spikes fired.")
```

## Advanced Usage

### 1. Using Experiment Logger

```python
import pynlm
from python.nlm_utils import NLMExperimentLogger

# Setup experiment logging
logger = NLMExperimentLogger("my_experiment")

# Setup NLM components
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
agent.enableDevelopment(True)

# Run simulation with logging
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

    # Log step data
    logger.log_step({
        'step': step,
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'prediction_error': agent.getPredictionError(),
        'development_stage': brain.getDevelopmentalStage()
    })

# Analyze results
analysis = logger.analyze()
print(f"Analysis complete!")
print(f"Total steps: {analysis['total_steps']}")
print(f"Average firing rate: {analysis['statistics']['average_firing_rate']['mean']:.2f}")
```

### 2. Performance Benchmarking

```python
import pynlm
from python.nlm_utils import NLMBenchmark

# Setup NLM components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Setup benchmark
benchmark = NLMBenchmark(brain, world, agent)

# Run benchmark
benchmark.benchmark_step(steps=1000, timestep=0.1)

# Generate report
report = benchmark.generate_report()
print(report)
```

### 3. Visualization

```python
import pynlm
from python.nlm_utils import NLMExperimentLogger, NLMVisualizer

# Setup experiment and run simulation (similar to previous example)
# ... (setup code here) ...

# Create visualizations
visualizer = NLMVisualizer()

# Create training curves
output_file = visualizer.create_training_curves(logger.step_data)
print(f"Training curves saved to: {output_file}")

# Create interactive dashboard
if output_file:
    dashboard_file = visualizer.create_interactive_dashboard(logger.step_data)
    print(f"Dashboard saved to: {dashboard_file}")
```

## Command-Line Interface

The package includes a comprehensive command-line interface for running NLM simulations.

### Installation

The `nlm` command is automatically installed when you install the package:

```bash
pip install -e .
```

### Usage Examples

```bash
# Quick start
nlm brain create --neuron-count 500
nlm world create --width 20 --height 20
nlm sim run --steps 500

# Advanced usage
nlm sim preset --name standard --steps 1000
nlm brain save my_brain.bin
nlm brain load saved_brain.bin

# With logging and learning
nlm sim run --steps 1000 --enable-learning --log-interval 100
```

### Available Commands

- **brain**: Brain management commands
  - `create`: Create a new brain with custom settings
  - `save`: Save brain state to file
  - `load`: Load brain state from file
  - `config`: Display configuration

- **world**: World management commands
  - `create`: Create a new world with custom dimensions

- **sim**: Simulation commands
  - `run`: Run a simulation with specified parameters
  - `preset`: Run predefined simulation scenarios

- **config**: Configuration file commands
  - `create`: Create configuration files

## API Reference

### Core Classes

#### Brain
- `createBrain(config)` - Create a new brain
- `initialize()` - Initialize the brain
- `step(step_number)` - Run a simulation step
- `receiveSensoryInput(input)` - Inject sensory input
- `produceAction()` - Get motor action
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state
- `addRegion(name)` - Add a neural region
- `getRegion(id)` - Get a region by ID

#### Config
- `createDefaultConfig()` - Create default configuration
- `loadFromFile(filepath)` - Load configuration from file
- `loadFromArgs(argc, argv)` - Load configuration from command line
- `saveToFile(filepath)` - Save configuration to file
- `has(key)` - Check if key exists
- `getKeys()` - Get all configuration keys
- `get(key)` - Get configuration value (with type detection)
- `getOr(key, default)` - Get configuration value with default
- `remove(key)` - Remove configuration key

#### SimpleWorld
- `createSimpleWorld()` - Create a new world
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `update(timestep)` - Update world simulation
- `reset()` - Reset world to initial state
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body state

#### AgentBrain
- `createAgentBrain(brain)` - Create agent brain interface
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward modulation
- `updateDevelopment(timestep)` - Update development system

### Advanced Utilities

#### NLMExperimentLogger
- `__init__(experiment_name, log_dir)` - Initialize experiment logger
- `log_step(step_data)` - Log step data
- `set_config(config)` - Set experiment configuration
- `set_metadata(**kwargs)` - Set experiment metadata
- `analyze()` - Analyze experiment data

#### NLMBenchmark
- `__init__(brain, world, agent)` - Initialize benchmark
- `benchmark_step(steps, timestep)` - Benchmark simulation
- `memory_profile()` - Profile memory usage
- `generate_report()` - Generate benchmark report

#### NLMVisualizer
- `__init__(output_dir)` - Initialize visualizer
- `create_training_curves(step_data, output_file)` - Create training curves
- `create_heatmaps(step_data, output_file)` - Create heatmaps
- `create_interactive_dashboard(step_data, output_file)` - Create dashboard

## Configuration Options

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

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Configuration File Examples

### Basic Configuration (`config.json`)

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "connection_probability": 0.05,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true
        },
        "structural": {
            "enable": true
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    }
}
```

### World Configuration (`world_config.json`)

```json
{
    "world": {
        "width": 20,
        "height": 20,
        "vision_width": 8,
        "vision_height": 8,
        "max_energy": 100.0,
        "energy_decay_rate": 0.01
    },
    "agent": {
        "start_x": 10.0,
        "start_y": 10.0,
        "start_orientation": 0.0
    }
}
```

## Common Patterns

### Pattern 1: Run a Simulation

```python
import pynlm

def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Statistics

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

## Troubleshooting

### Installation Issues

**Error: ImportError: No module named 'pynlm'**

```bash
pip install --force-reinstall .
```

**Error: CMake cannot find Python**

```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors**

- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**Error: Segmentation fault**

- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

**Error: Brain isn't doing anything**

- Did you call `brain.initialize()`?
- Try increasing the number of steps

**Error: Agent isn't moving**

- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

## Advanced Usage

### 1. Custom Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()

# Set custom parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.curiosity.enable", True)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Custom World Configuration

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(
    width=50,          # Large world
    height=50,
    vision_width=20,   # Better vision
    vision_height=20
)
world.setMaxEnergy(200.0)  # More energy
world.setAgentStart(25.0, 25.0)  # Center start
world.reset()
```

### 3. Batch Processing

```python
import pynlm
import time
from python.nlm_utils import NLMBenchmark

def run_batch_experiments(experiment_configs):
    """Run multiple experiments with different configurations."""
    results = []
    
    for i, config_dict in enumerate(experiment_configs):
        print(f"\nRunning experiment {i+1}/{len(experiment_configs)}")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        for key, value in config_dict.items():
            config.set(key, value)
        
        # Create NLM components
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(**config_dict.get('world', {}))
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        start_time = time.time()
        for step in range(config_dict.get('steps', 1000)):
            world.update(config_dict.get('timestep', 0.1))
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        
        # Store results
        results.append({
            'experiment_id': i,
            'config': config_dict,
            'duration': end_time - start_time,
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'learning_enabled': agent.isRewardModulationEnabled()
        })
    
    return results

# Example experiment configurations
experiment_configs = [
    {
        'brain.neuron_count': 500,
        'world': {'width': 10, 'height': 10, 'vision_width': 4, 'vision_height': 4},
        'steps': 500,
        'timestep': 0.1
    },
    {
        'brain.neuron_count': 2000,
        'brain.synapse_density': 0.05,
        'world': {'width': 30, 'height': 30, 'vision_width': 8, 'vision_height': 8},
        'steps': 1000,
        'timestep': 0.05
    }
]

# Run batch experiments
results = run_batch_experiments(experiment_configs)

# Analyze results
for result in results:
    print(f"Experiment {result['experiment_id']}: {result['duration']:.1f}s, "
          f"final firing rate: {result['final_firing_rate']:.2f} Hz")
```

### 4. Integration with Other Tools

```python
import pynlm
import numpy as np
from python.nlm_utils import NLMExperimentLogger
from sklearn.decomposition import PCA
from sklearn.cluster import KMeans

# Run experiment
logger = NLMExperimentLogger("ml_integration")
# ... (run simulation and log data) ...

# Extract data for machine learning
firing_rates = [step['average_firing_rate'] for step in logger.step_data]
curiosity_levels = [step['curiosity_level'] for step in logger.step_data]

# Apply PCA for dimensionality reduction
pca = PCA(n_components=2)
ml_data = np.array([firing_rates, curiosity_levels]).T
pca_result = pca.fit_transform(ml_data)

# Cluster the data
kmeans = KMeans(n_clusters=3, random_state=42)
clusters = kmeans.fit_predict(ml_data)

print(f"Explained variance ratio: {pca.explained_variance_ratio_}")
print(f"Cluster centers: {kmeans.cluster_centers_}")
```

## Performance Tips

### 1. For Better Performance

- Use larger batch sizes for simulation steps
- Enable structural plasticity only when needed
- Use larger worlds for better spatial exploration
- Adjust neuron counts based on available memory

### 2. Memory Optimization

- Use checkpointing for long simulations
- Monitor memory usage with `NLMBenchmark.memory_profile()`
- Use smaller worlds for rapid prototyping
- Consider using `world.setRandomSeed()` for reproducible experiments

### 3. Learning Optimization

- Balance exploration (curiosity) and exploitation (reward)
- Adjust learning rates based on task requirements
- Use developmental stages to modulate learning speed
- Monitor prediction error for adaptive learning

## Examples and Demos

The repository includes several example scripts:

1. **Basic Examples**: Simple brain creation and simulation
2. **Complete Agent Examples**: Full agent-world interaction
3. **Advanced Examples**: Complex scenarios with learning
4. **Utility Examples**: Using NLM with other tools

### Example Commands

```bash
# Run basic examples
python examples/basic_example.py

# Run complete agent example
python examples/complete_agent.py

# Run advanced example with learning
python examples/advanced_learning.py

# Run utility example
python examples/utility_example.py
```

## Conclusion

The NLM Python package provides a comprehensive interface to the NLM computational brain simulation framework. It combines the power of C++ implementation with the flexibility and ease of use of Python, making it suitable for:

- **Education**: Teaching neuroscience and AI concepts
- **Research**: Testing hypotheses about neural computation
- **Development**: Building new brain-inspired algorithms
- **Prototyping**: Rapidly prototyping neural architectures
- **Data Analysis**: Analyzing simulation results with modern Python tools

The package includes advanced utilities for experiment logging, performance benchmarking, and visualization, making it a complete solution for NLM research and development.

## Next Steps

1. **Get Started**: Run the basic examples to understand the API
2. **Explore Features**: Try the advanced examples and utilities
3. **Customize**: Modify the configuration for your specific needs
4. **Extend**: Use the advanced utilities to add new functionality
5. **Share**: Share your NLM experiments and results with the community

For more information and examples, visit the NLM documentation at: https://nlm.readthedocs.io
