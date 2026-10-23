# NLM (Neural Learning Machine) - Power User Documentation

This document provides comprehensive documentation for the NLM Python package, including advanced features, power user tools, and usage examples.

## Overview

The NLM Python package provides a comprehensive interface to the NLM (Neural Learning Machine) neural simulation framework. It includes both basic and advanced functionality for researchers, developers, and scientists working with artificial neural systems.

## Installation

### Basic Installation

```bash
pip install -e .
```

### Development Installation

```bash
pip install -e .[dev]
```

### Advanced Features

```bash
pip install -e .[dev,documentation]
```

## Package Structure

The NLM package is organized into the following modules:

### Core Modules (`nlm/`)
- **`pynlm.py`**: Main Python interface with basic functionality
- **`nlm_examples.py`**: Example simulations and demonstrations
- **`nlm_power_user.py`**: Advanced tools for power users
- **`__init__.py`**: Package initialization and exports
- **`__main__.py`**: Command-line interface

### Supporting Files
- **`pyproject.toml`**: Python package configuration
- **`setup.py`**: Setup script for installation
- **`README.md`**: Main documentation
- **`easy_usage.md`**: Quick start guide
- **`HOW_TO_USE.md`**: Detailed usage instructions

## API Reference

### Core Classes

#### Config
Configuration management for NLM systems.

**Methods:**
- `create_default_config()`: Create a default configuration
- `config.set(key, value, source)`: Set a configuration value
- `config.get(key)`: Get a configuration value
- `config.loadFromFile(filepath)`: Load configuration from file
- `config.saveToFile(filepath)`: Save configuration to file

#### Brain
Neural simulation core with spiking neurons and plasticity.

**Methods:**
- `create_brain(config)`: Create a new brain instance
- `brain.initialize()`: Initialize the brain
- `brain.step(step)`: Perform a simulation step
- `brain.step(step, time)`: Perform a simulation step with timestamp
- `brain.receiveSensoryInput(input)`: Inject sensory input
- `brain.produceAction()`: Generate motor output
- `brain.getTotalNeuronCount()`: Get total neuron count
- `brain.getTotalSynapseCount()`: Get total synapse count
- `brain.getFiringNeuronCount()`: Get firing neuron count
- `brain.getAverageFiringRate()`: Get average firing rate

#### SimpleWorld
2D simulation environment for agents.

**Methods:**
- `create_simple_world()`: Create a new world instance
- `world.configure(width, height, vision_width, vision_height)`: Configure world dimensions
- `world.update(timestep)`: Update world state
- `world.applyMotorCommand(cmd, time)`: Apply agent action
- `world.getSensoryPercept()`: Get sensory input from environment
- `world.getAgentBody()`: Get agent state

#### AgentBrain
Interface connecting brain to world environment.

**Methods:**
- `create_agent_brain(brain)`: Create agent interface
- `agent.initialize(world)`: Initialize agent with world
- `agent.processSensoryInput(percept)`: Process sensory input
- `agent.decodeMotorCommand()`: Decode action from neural activity
- `agent.applyRewardModulation(reward, predicted)`: Apply reward learning
- `agent.enableLearning()`: Enable learning subsystems

### Builder Classes

#### ConfigBuilder
Fluent interface for creating configuration objects.

**Example:**
```python
from nlm import ConfigBuilder

config = (ConfigBuilder()
    .set_neuron_count(500)
    .set_connection_probability(0.1)
    .enable_stdp(learning_rate=0.002)
    .set_random_seed(42)
    .build())
```

#### BrainBuilder
Builder for creating and configuring brain instances.

**Example:**
```python
from nlm import BrainBuilder

brain = (BrainBuilder()
    .with_neuron_count(300)
    .with_plasticity(stdp_enabled=True, hebbian_enabled=True)
    .build().get())
```

#### AgentBuilder
Builder for creating complete agent-world systems.

**Example:**
```python
from nlm import AgentBuilder

agent = (AgentBuilder()
    .configure_world(width=20, height=20, vision_width=8, vision_height=8)
    .with_start_position(10.0, 10.0)
    .enable_learning(reward_modulation=True, curiosity=True)
    .build().get())
```

### SimulationRunner
High-level interface for running simulations.

**Methods:**
- `runner.run_episode(steps, timestep, log_interval)`: Run complete simulation
- `runner.get_statistics_history()`: Get historical statistics
- `runner.get_final_stats()`: Get final simulation statistics

### Advanced Power User Tools

#### PerformanceMonitor
Comprehensive performance monitoring system.

**Key Features:**
- Real-time metric collection
- Bottleneck analysis
- Step-time statistics
- Data export to JSON

**Example:**
```python
from nlm import PerformanceMonitor

monitor = PerformanceMonitor()
monitor.start_monitoring()

for step in range(100):
    brain.step(step)
    monitor.record_step(step, brain, world)

summary = monitor.stop_monitoring()
print(f"Average step time: {summary['avg_step_time']:.4f}s")
```

#### AdvancedBrainBuilder
Fine-grained control over brain architecture.

**Key Features:**
- Custom region definitions
- Connectivity pattern specification
- Advanced plasticity rules
- Neural dynamics customization

**Example:**
```python
from nlm import AdvancedBrainBuilder

brain = (AdvancedBrainBuilder()
    .add_region('cortex', 'excitatory', 200, position=(0, 0, 0))
    .add_region('hippocampus', 'inhibitory', 50, position=(10, 0, 0))
    .set_connectivity('cortex', 'hippocampus', 0.2, weight_mean=1.5)
    .set_plasticity_rules('stdp', {'learning_rate': 0.005, 'tau': 40.0})
    .build())
```

#### SimulationDebugger
Advanced debugging and state inspection.

**Key Features:**
- Variable monitoring
- Break condition setting
- Debug logging
- State analysis

**Example:**
```python
from nlm import SimulationDebugger

debugger = SimulationDebugger(brain, world)
debugger.add_watch('firing_rate', lambda step, brain, world: brain.getFiringNeuronCount() / brain.getTotalNeuronCount())
debugger.add_break_condition('too_many_spikes', lambda step, brain, world: brain.getTotalSpikeCount() > 1000)

results = debugger.run_with_debugging(100, timestep=0.1)
```

#### CustomSimulationEnvironment
User-defined simulation environments.

**Key Features:**
- Custom object types
- Physics rule definition
- State history tracking
- Export capabilities

**Example:**
```python
from nlm import CustomSimulationEnvironment

env = CustomSimulationEnvironment(width=30, height=30)
env.add_object('resource', 10, 10, {'value': 10.0, 'radius': 1.0})
env.add_object('hazard', 20, 20, {'value': -5.0, 'radius': 1.5})
env.add_physics_rule('collision_detection', collision_detection_function)
```

#### ExperimentDesigner
Scientific experiment design and management.

**Key Features:**
- Parameter management
- Condition specification
- Trial tracking
- Results analysis

**Example:**
```python
from nlm import ExperimentDesigner

exp = ExperimentDesigner("Neural Plasticity Study")
exp.add_parameter("neuron_count", [100, 200, 300], "Number of neurons")
exp.add_parameter("learning_rate", [0.001, 0.01, 0.1], "STDP learning rate")
exp.add_condition("high_plasticity", lambda config, params: config.set("plasticity.stdp.enable", True))
exp.add_trial("trial_1", {"neuron_count": 200, "learning_rate": 0.01}, "high_plasticity")

results = exp.run_experiment()
analysis = exp.analyze_results(results)
```

## Examples and Demos

### Basic Examples (`nlm_examples.py`)

#### 1. Silent Brain Example
Demonstrates basic brain functionality without world interaction.

**Key Concepts:**
- Brain initialization and configuration
- Simulation stepping
- Neural activity statistics

#### 2. Agent-World Example
Shows agent perception and interaction with environment.

**Key Concepts:**
- Sensory processing
- Action selection
- Environment interaction

#### 3. Complete Learning Example
Demonstrates integrated learning with all subsystems.

**Key Concepts:**
- Reward modulation
- Curiosity-driven exploration
- Structural plasticity
- Developmental processes

#### 4. Curiosity Exploration Example
Shows how curiosity drives exploration behavior.

**Key Concepts:**
- Novelty detection
- Exploration-exploitation balance
- Behavioral flexibility

#### 5. Plasticity Demonstration
Demonstrates synaptic plasticity mechanisms.

**Key Concepts:**
- STDP (Spike-Timing-Dependent Plasticity)
- Hebbian learning
- Structural plasticity

### Power User Examples

#### Advanced Simulation with Monitoring
Combines advanced features with performance monitoring.

**Example:**
```python
from nlm import create_advanced_simulation, PerformanceMonitor

# Create advanced simulation
sim = create_advanced_simulation({
    'neuron_count': 500,
    'learning_rate': 0.002
})

# Run with monitoring
monitor = sim['monitor']
monitor.start_monitoring()

for step in range(100):
    sim['agent'].processSensoryInput(sim['world'].getSensoryPercept())
    sim['agent'].getBrain().step(step)
    sim['world'].update(0.1)
    monitor.record_step(step, sim['agent'].getBrain(), sim['world'])

summary = monitor.stop_monitoring()
```

#### Custom Environment Design
Create custom simulation environments with specific dynamics.

**Example:**
```python
from nlm import CustomSimulationEnvironment

# Design a maze environment
env = CustomSimulationEnvironment(width=15, height=15)

# Add walls
for x in [3, 7, 11, 15]:
    env.add_object('wall', x, 5, {'radius': 1.0})

# Add resources
for x in [2, 6, 10, 14]:
    env.add_object('resource', x, 10, {'value': 5.0, 'radius': 0.8})

# Add hazards
for x in [4, 8, 12]:
    env.add_object('hazard', x, 8, {'value': -3.0, 'radius': 1.2})

# Add agent start
env.add_object('agent', 1, 1, {'initial_x': 1, 'initial_y': 1})
```

#### Experimental Design
Design and run controlled scientific experiments.

**Example:**
```python
from nlm import ExperimentDesigner

# Design experiment to test plasticity effects
exp = ExperimentDesigner("Plasticity Effects on Learning")

# Add parameters
exp.add_parameter("plasticity_enabled", [True, False], "Enable plasticity")
exp.add_parameter("neuron_count", [100, 200, 300], "Number of neurons")
exp.add_parameter("learning_rate", [0.001, 0.01], "Learning rate")

# Add conditions
exp.add_condition("plasticity_condition", lambda config, params: config.set("plasticity.stdp.enable", params['plasticity_enabled']))
exp.add_condition("network_condition", lambda config, params: config.set("brain.neuron_count", params['neuron_count']))

# Add trials
for i, plasticity in enumerate([True, False]):
    for j, neurons in enumerate([100, 200, 300]):
        for k, rate in enumerate([0.001, 0.01]):
            trial_name = f"plasticity_{plasticity}_neurons_{neurons}_rate_{rate}"
            exp.add_trial(trial_name, {
                'plasticity_enabled': plasticity,
                'neuron_count': neurons,
                'learning_rate': rate
            }, "plasticity_condition")

# Run experiment
results = exp.run_experiment()
analysis = exp.analyze_results(results)
```

## Command Line Interface

The NLM package includes a comprehensive command-line interface:

### Available Commands

#### `nlm check`
Check environment setup and dependencies.

```bash
nlm check
```

#### `nlm demo`
Run a quick demonstration of NLM capabilities.

```bash
nlm demo
```

#### `nlm example <name>`
Run specific examples.

```bash
nlm example silent
nlm example agent-world
nlm example learning
nlm example curiosity
nlm example plasticity
nlm example all
```

#### `nlm docs`
Build documentation.

```bash
nlm docs
```

### Command Line Options

All commands support:
- `--help`: Show help information
- `--version`: Show version information

## Configuration

### Power User Configuration

Power user features can be configured through the `POWER_USER_CONFIG` dictionary in `nlm_power_user.py`:

```python
from nlm.nlm_power_user import configure_power_user_settings

configure_power_user_settings({
    'enable_detailed_logging': True,
    'enable_performance_monitoring': True,
    'enable_visualization': True,
    'debug_mode': False
})
```

### Environment Variables

The package respects standard Python package environment variables:
- `PYNLM_DEBUG`: Enable debug mode
- `PYNLM_VERBOSE`: Enable verbose logging

## Performance Considerations

### Simulation Optimization

1. **Batch Processing**: Process multiple steps before updating the world
2. **Memory Management**: Monitor memory usage with `PerformanceMonitor`
3. **Parallel Processing**: Use multiple threads for independent simulations
4. **Hardware Acceleration**: Leverage GPU capabilities if available

### Scaling Guidelines

- **Small Scale** (< 100 neurons): Use basic builders and simple environments
- **Medium Scale** (100-1000 neurons): Use standard builders with monitoring
- **Large Scale** (> 1000 neurons): Use advanced builders and custom environments
- **Research Scale** (> 10000 neurons): Use specialized hardware and distributed computing

## Troubleshooting

### Common Issues

#### "ModuleNotFoundError: No module named 'pynlm'"

**Solution:** Install the package properly:
```bash
pip install -e .
```

#### "ImportError: libstdc++.so.6: cannot open shared object file"

**Solution:** Ensure C++ dependencies are installed:
```bash
# On Ubuntu/Debian
 sudo apt-get update
 sudo apt-get install g++-9

# On macOS (using Homebrew)
brew install gcc
```

#### "Segmentation fault"

**Solution:**
1. Check for null pointer dereferences
2. Ensure proper initialization order
3. Verify memory allocation
4. Check for infinite loops

#### "Performance issues"

**Solution:**
1. Use `PerformanceMonitor` to identify bottlenecks
2. Reduce simulation complexity
3. Use efficient data structures
4. Consider parallel processing

### Getting Help

#### Online Resources
- **Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Issue Tracker**: https://github.com/nlm-project/nlm/issues
- **Examples**: See `nlm_examples.py` for comprehensive examples

#### Community Support
- Join the NLM discussion forum
- Participate in GitHub discussions
- Report issues and feature requests

#### Technical Support
- For technical issues, consult the source code
- Review performance monitoring output
- Use debugging tools provided in `nlm_power_user.py`

## License

This project is licensed under the MIT License. See `LICENSE` file for details.

## Acknowledgments

The NLM project acknowledges the contributions of:
- The open-source community
- Contributors to neural network research
- Users who provide feedback and report issues
- Supporters of computational neuroscience

## Future Development

### Planned Features
1. **Enhanced Visualization**: Advanced plotting and analysis tools
2. **Distributed Computing**: Support for multi-machine simulations
3. **Machine Learning Integration**: Connection to modern ML frameworks
4. **Cloud Deployment**: Support for cloud-based simulations
5. **Real-time Interaction**: Live visualization and control

### Research Directions
1. **Neuromorphic Computing**: Integration with neuromorphic hardware
2. **Explainable AI**: Tools for understanding neural network behavior
3. **Transfer Learning**: Applications to real-world problems
4. **Cognition Modeling**: Advanced cognitive architectures
5. **Developmental Psychology**: Modeling cognitive development

## Contact Information

For support, questions, or collaboration opportunities:

- **Email**: support@nlm-project.org
- **GitHub**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io

---

*Last updated: October 2026*
*Version: 0.1.0*

This documentation provides a comprehensive guide to using and extending the NLM neural simulation framework. For the most up-to-date information, visit the official documentation portal.