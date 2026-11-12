# NLM (Neural Learning Machine) - Utility and Development Guide

## Overview

NLM is a sophisticated brain simulation framework that implements a complete artificial developmental brain with spiking neural networks. This document provides comprehensive guidance on using NLM, including basic usage, advanced utilities, and development tools.

## Quick Start

### Basic Usage (Beginner)

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Simulation (Intermediate)

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    return brain, agent, world
```

## Installation

```bash
# Install from source
pip install -e .

# Install with all optional dependencies
pip install -e .[dev]
```

## Core Python API

### Main Classes

#### Brain
- `pynlm.createBrain(config)` - Create a new brain instance
- `brain.initialize()` - Initialize the brain
- `brain.step(step)` - Perform one simulation step
- `brain.save(filename)` - Save brain state to file
- `brain.load(filename)` - Load brain state from file
- `brain.getTotalNeuronCount()` - Get total neuron count
- `brain.getTotalSynapseCount()` - Get total synapse count
- `brain.getFiringNeuronCount()` - Get currently firing neurons
- `brain.getAverageFiringRate()` - Get average firing rate

#### Config
- `pynlm.createDefaultConfig()` - Create default configuration
- `config.set(key, value)` - Set configuration value
- `config.get(key)` - Get configuration value
- `config.has(key)` - Check if key exists
- `config.loadFromFile(filename)` - Load from file
- `config.saveToFile(filename)` - Save to file

#### AgentBrain
- `pynlm.createAgentBrain(brain)` - Create agent interface
- `agent.initialize(world)` - Initialize with world
- `agent.processSensoryInput(percept)` - Process sensory input
- `agent.decodeMotorCommand()` - Get motor command
- `agent.applyRewardModulation(reward, predicted)` - Apply reward
- Various enable/disable methods for subsystems

#### SimpleWorld
- `pynlm.createSimpleWorld()` - Create simulation world
- `world.configure(width, height, visionWidth, visionHeight)` - Configure world
- `world.update(timestep)` - Update world state
- `world.applyMotorCommand(command, time)` - Apply action
- `world.getSensoryPercept()` - Get sensory percept
- `world.reset()` - Reset world

## Utility Modules

NLM provides several utility modules for advanced usage:

### `pynlm.utils` - Core Utilities

#### BrainAnalyzer
- `BrainAnalyzer(brain)` - Analyze brain statistics
- `get_comprehensive_stats()` - Get detailed brain statistics
- `export_to_csv(filename)` - Export stats to CSV
- `export_to_json(filename)` - Export stats to JSON
- `print_formatted_stats()` - Print formatted statistics

#### ConfigManager
- `create_preset_config(name)` - Create preset configurations
- `config_to_dict(config)` - Convert config to dictionary
- `dict_to_config(config_dict, config=None)` - Create config from dictionary
- `print_config_summary(config)` - Print config summary

#### WorldUtils
- `create_test_scenario(width, height, vision_width, vision_height)` - Create test world
- `simulate_agent_with_world(world, agent, brain, steps)` - Run agent simulation

#### ExperimentRunner
- `ExperimentRunner(name)` - Manage experiments
- `run_trial(name, config, world, agent, steps)` - Run experimental trial
- `get_summary()` - Get experiment summary

### `pynlm.viz` - Visualization

#### Visualization
- `Visualization()` - Main visualization class
- `plot_firing_rate_over_time(brain, steps, title)` - Plot firing rate
- `plot_spike_distribution(brain, bins, title)` - Plot spike distribution
- `plot_development_stages(brain, stages, title)` - Plot development stages
- `create_dashboard(brain, world=None, agent=None, title)` - Create comprehensive dashboard

#### Convenience Functions
- `create_quick_dashboard(brain, world=None, agent=None, filename)` - Create quick dashboard
- `plot_brain_stats(brain, filename=None)` - Plot brain statistics

### `pynlm.config` - Configuration Management

#### BrainConfigurator
- Built-in presets: `tiny`, `small`, `medium`, `large`, `research`, `debug`
- Method chaining for configuration
- JSON/YAML file support
- Parameter validation
- Configuration comparison

#### ParameterSweep
- Systematic parameter sweeps
- Generate multiple configurations
- Batch processing utilities

#### ExperimentManager
- Experiment lifecycle management
- Trial tracking and results storage
- Parameter management
- Progress tracking

## Advanced Usage Examples

### 1. Batch Processing

```python
from pynlm.utils import batch_brain_creation

# Create multiple brains with different configurations
brains = batch_brain_creation(
    num_brains=10,
    config=None,  # Use default config
    steps=100
)

# Analyze all brains
total_spikes = sum(brain.getTotalSpikeCount() for brain in brains)
print(f"Average spikes per brain: {total_spikes / len(brains):.1f}")
```

### 2. Configuration Management

```python
from pynlm.config import BrainConfigurator, ParameterSweep

# Create custom configuration
config = BrainConfigurator()
config.apply_preset("research")
config.update({
    'brain.neuron_count': 5000,
    'random_seed': 12345
})

# Save configuration
config.save_to_json("research_config.json")

# Parameter sweep
base_config = BrainConfigurator().apply_preset("medium")
sweep = ParameterSweep(base_config)
sweep.add_parameter("brain.neuron_count", [1000, 2000, 5000])
sweep.add_parameter("random_seed", [42, 123, 456])

configurations = sweep.generate_configurations()
print(f"Generated {len(configurations)} configurations")
```

### 3. Advanced Visualization

```python
from pynlm.viz import Visualization, create_quick_dashboard

# Create visualization instance
viz = Visualization()

# Run a simulation and create dashboard
brain, agent, world = run_agent_simulation(num_steps=500)

# Create comprehensive dashboard
dashboard_file = viz.save_dashboard(
    brain, world, agent, "simulation_dashboard.png"
)
print(f"Dashboard saved to: {dashboard_file}")

# Create quick stats plot
fig = plot_brain_stats(brain, "brain_stats.png")
```

### 4. Experiment Management

```python
from pynlm.config import ExperimentManager

# Create experiment manager
experiment = ExperimentManager("learning_experiment", "experiments/")
experiment.add_parameter("brain.neuron_count", 2000)
experiment.add_parameter("random_seed", 42)

# Run multiple trials
for trial_num in range(5):
    config = experiment.create_config()
    # Modify config for each trial if needed
    config.set("random_seed", trial_num + 100)
    
    trial_result = experiment.run_trial(
        f"trial_{trial_num}",
        config,
        steps=500
    )
    
    print(f"Trial {trial_num}: {trial_result['duration_seconds']:.1f}s, "
          f"{trial_result['brain_stats']['total_spikes']:,} spikes")

# View experiment summary
experiment.print_experiment_summary()
```

## Command-Line Tools

### `nlm-utils` - Main Utility Program

A comprehensive command-line tool for working with NLM brains:

```bash
# Help and available commands
nlm-utils --help

# Create and analyze a brain
nlm-utils brain create --neurons 1000 --steps 100
nlm-utils brain analyze brain.bin

# Configuration management
nlm-utils config presets          # List all presets
nlm-utils config create tiny      # Create tiny brain configuration
nlm-utils config save tiny.json   # Save configuration to file

# Experiment utilities
nlm-utils experiment create my_exp          # Create new experiment
nlm-utils experiment run my_exp --trials 10 # Run experiment with 10 trials
nlm-utils experiment summary my_exp          # Show experiment results

# Visualization
nlm-utils viz dashboard brain.bin dashboard.png
nlm-utils viz stats brain.bin stats.png

# Batch processing
nlm-utils batch create --count 5 --steps 100
nlm-utils batch analyze brains/*.bin
```

### Available Commands

#### `brain` - Brain Operations
- `create` - Create new brain
- `analyze` - Analyze brain statistics
- `save` - Save brain state
- `load` - Load brain state
- `export` - Export brain data
- `compare` - Compare two brains

#### `config` - Configuration Management
- `presets` - List built-in presets
- `create` - Create configuration
- `save` - Save configuration
- `load` - Load configuration
- `validate` - Validate configuration
- `compare` - Compare configurations

#### `world` - World Operations
- `create` - Create simple world
- `configure` - Configure world parameters
- `reset` - Reset world state
- `analyze` - Analyze world

#### `agent` - Agent Operations
- `create` - Create agent
- `initialize` - Initialize agent
- `process` - Process sensory input
- `decode` - Decode motor command
- `train` - Train agent with learning

#### `experiment` - Experiment Management
- `create` - Create new experiment
- `run` - Run experiment
- `add-trial` - Add trial to experiment
- `summary` - Show experiment summary
- `results` - Export experiment results

#### `viz` - Visualization
- `dashboard` - Create comprehensive dashboard
- `stats` - Create statistics plot
- `network` - Plot network topology
- `development` - Plot developmental stages
- `spikes` - Plot spike activity

#### `batch` - Batch Processing
- `create` - Create multiple brains
- `process` - Process multiple brains
- `analyze` - Analyze multiple brains
- `export` - Export multiple brain states

## Configuration Presets

NLM provides several pre-configured presets for different use cases:

### `tiny`
- **Neurons**: 50
- **Description**: Minimal brain for testing and debugging
- **Use case**: Quick tests, unit testing

### `small`
- **Neurons**: 200
- **Description**: Small brain for basic simulations
- **Use case**: Basic learning demonstrations

### `medium`
- **Neurons**: 1000
- **Description**: Standard-sized brain for most simulations
- **Use case**: General purpose simulations

### `large`
- **Neurons**: 5000
- **Description**: Large brain for complex simulations
- **Use case**: Advanced learning scenarios

### `research`
- **Neurons**: 10000
- **Description**: High-density brain for research
- **Use case**: Scientific research, parameter sweeps

### `debug`
- **Neurons**: 100
- **Description**: Debug-friendly with deterministic behavior
- **Use case**: Debugging, development

## File Formats

### JSON Configuration
```json
{
  "brain.neuron_count": 1000,
  "brain.synapse_density": 0.1,
  "random_seed": 42,
  "simulation_timestep": 0.001
}
```

### YAML Configuration
```yaml
brain:
  neuron_count: 1000
  synapse_density: 0.1
  connection_probability: 0.05
random_seed: 42
simulation_timestep: 0.001
```

### CSV Statistics
```csv
metric,value
neuron_count,1000
synapse_count,50000
firing_neuron_count,45
average_firing_rate,12.5
```

## Performance Tips

### For Large Brains
1. **Use appropriate presets**: Choose `medium` or `large` presets for most use cases
2. **Limit simulation steps**: Run fewer steps for rapid testing
3. **Monitor memory usage**: Use `BrainAnalyzer` to track memory consumption
4. **Batch processing**: Process multiple brains in parallel when possible

### For Debugging
1. **Use debug preset**: Start with the debug preset for easier debugging
2. **Enable logging**: Add logging to track brain state changes
3. **Use checkpoints**: Save brain state frequently during long simulations
4. **Validate configuration**: Use `config.validate()` to catch issues early

### For Research
1. **Use research preset**: Start with the research preset for complex scenarios
2. **Parameter sweeps**: Use `ParameterSweep` for systematic exploration
3. **Experiment management**: Use `ExperimentManager` for organized research
4. **Visualization**: Use comprehensive dashboards for result analysis

## Troubleshooting

### Common Issues

#### **"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if neurons have appropriate input

#### **"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify the agent has motor neurons

#### **"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

#### **"Configuration validation failed"**
- Check for invalid parameter values
- Use `config.validate()` to see specific errors
- Ensure required parameters are set

### Getting Help

1. **Documentation**: Check `HOW_TO_USE.md` for detailed API reference
2. **Examples**: Look in the `examples/` directory for practical examples
3. **Utilities**: Use `nlm-utils --help` for command-line tool help
4. **Issue Tracking**: Report issues on the project repository

## Development Guide

### Adding New Features

1. **Python Utilities**: Add new modules to `pynlm/` directory
2. **Command-Line Tools**: Add new commands to `nlm-utils`
3. **Documentation**: Update `HOW_TO_USE.md` and `easy_usage.md`
4. **Examples**: Add new examples to `examples/` directory

### Contributing

1. **Fork the repository**
2. **Create feature branch**
3. **Implement changes**
4. **Add tests**
5. **Update documentation**
6. **Submit pull request**

## License

MIT License - See LICENSE file for details

## Acknowledgements

This project builds on extensive research in:
- Computational neuroscience
- Neural network simulation
- Artificial developmental systems
- Machine learning and pattern recognition

The NLM project represents a significant effort to create a biologically-inspired artificial brain capable of learning and adaptation through interaction with its environment.