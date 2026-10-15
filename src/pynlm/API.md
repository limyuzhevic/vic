# NLM Python API - Enhanced Python Interface

## Overview

The NLM Python API provides a comprehensive Python interface to the NLM (Neural Learning Machine) framework, allowing users to create and simulate artificial developmental brains with Python code.

## Features

### Core Components

#### Brain Simulation
- **Brain Class**: Main neural network simulation engine
- **Real LIF Neurons**: Leaky Integrate-and-Fire neuron dynamics
- **Event-driven Spike Propagation**: Efficient spike transmission with delays
- **Plasticity Rules**: STDP, Hebbian, and structural plasticity
- **Multi-stage Development**: Developmental stages affect learning rates

#### World Interaction
- **World Class**: 2D simulation environment
- **Sensory Processing**: Vision, audio, and internal signals
- **Motor Control**: Action selection and execution
- **Reward System**: Reward prediction error and neuromodulation

#### Cognitive Systems
- **Memory**: Working memory, episodic memory, semantic memory, and procedural memory
- **Prediction**: Temporal and spatial prediction systems
- **Cognition**: Concept formation, attention, and planning
- **Neuromodulation**: Dopamine, curiosity, novelty, and reward

#### Agent-Brain Interface
- **AgentBrain Class**: Bridge between brain and world
- **Sensory Processing**: Convert world input to brain input
- **Action Selection**: Decode motor commands from brain activity
- **Learning**: Reward-based learning and adaptation

### Advanced Features

#### Configuration Management
- **NLMConfig Class**: Comprehensive configuration system
- **JSON Support**: Load and save configurations
- **Command-line Overrides**: Override config values via CLI
- **Validation**: Configuration validation and error handling

#### Development Tools
- **Performance Profiling**: Built-in profiling tools
- **Visualization**: Brain state visualization
- **Checkpointing**: Save and load brain states
- **Replay**: Replay recorded experiences

#### Extensions
- **Network Class**: Complete NLM network interface
- **Custom Components**: Extend with custom neurons, synapses, and plasticity rules
- **Plugin Architecture**: Load external components
- **Distributed Computing**: Scale simulations across multiple machines

## Installation

### Prerequisites
- Python 3.7 or higher
- C++ compiler (for C++ extensions)
- CMake (for building)

### Installation Options

#### Option 1: Using pip (Recommended)

```bash
pip install -e .
```

This will build and install the NLM Python bindings.

#### Option 2: From Source

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install .
```

#### Option 3: Development Installation

```bash
pip install -e .[dev]
```

This includes development dependencies and testing tools.

## Basic Usage

### Quick Start Example

```python
from nlm import Brain, World, AgentBrain, NLMConfig
import time

# Create configuration
config = NLMConfig(
    neuron_count=1000,
    region_count=2,
    environment_width=20.0,
    environment_height=20.0
)

# Create brain
brain = Brain(config)
brain.initialize()

# Create world
world = World(width=20.0, height=20.0, random_seed=42)

# Create agent brain
agent_brain = AgentBrain(brain)
agent_brain.initialize(world)

# Run simulation
for step in range(100):
    # Get sensory input
    percept = world.get_sensory_percept()
    
    # Process sensory input
    agent_brain.process_sensory_input(percept)
    
    # Step brain
    brain.step(step, step * 0.001)
    
    # Decode and apply motor command
    command = agent_brain.decode_motor_command()
    action_result = world.apply_motor_command(command)
    
    # Apply reward modulation
    agent_brain.apply_reward_modulation(action_result.reward)
    
    # Update world and development
    world.update(0.01)
    agent_brain.update_development(0.01)
    
    # Log progress
    if step % 10 == 0:
        print(f"Step {step}: Reward = {action_result.reward:.2f}")

print("Simulation complete!")
```

### Advanced Example

```python
from nlm import Network, NLMConfig

# Create configuration with advanced settings
config = NLMConfig()
config.neuron_count = 2000
config.region_count = 3
config.connection_probability = 0.15
config.simulation_timestep = 0.001
config.enable_checkpointing = True
config.enable_replay = True
config.enable_development = True

# Save configuration
config.save("my_config.json")

# Create network
network = Network(config)

# Run complete simulation
results = network.run(steps=1000)

# Analyze results
print(f"Total reward: {results['total_reward']:.2f}")
print(f"Steps completed: {results['steps_completed']}")
```

## Configuration

### NLMConfig Class

The `NLMConfig` class provides comprehensive configuration options:

#### Core Parameters
- `neuron_count`: Number of neurons in the brain (default: 1000)
- `region_count`: Number of brain regions (default: 2)
- `connection_probability`: Probability of synaptic connections (default: 0.1)
- `random_seed`: Random seed for reproducible experiments (default: 42)
- `simulation_timestep`: Time step for simulation (default: 0.001)

#### Plasticity Settings
- `plasticity_learning_rate`: Learning rate for plasticity (default: 0.01)
- `stdp_ltp_weight`: STDP long-term potentiation weight (default: 0.02)
- `stdp_ltd_weight`: STDP long-term depression weight (default: 0.015)
- `stdp_time_constant`: STDP time constant (default: 20.0)

#### Development Settings
- `development_synaptogenesis_rate`: Rate of new synapse formation (default: 0.0001)
- `development_pruning_rate`: Rate of synapse elimination (default: 0.00001)

#### Neuromodulation Settings
- `dopamine_baseline`: Baseline dopamine level (default: 0.1)

#### Environment Settings
- `environment_name`: Name of environment (default: "GridWorld")
- `environment_width`: Width of environment (default: 20)
- `environment_height`: Height of environment (default: 20)

#### Simulation Settings
- `max_simulation_steps`: Maximum simulation steps (default: 10000)
- `simulation_time_limit`: Time limit for simulation (default: 0.0)

#### Visualization Settings
- `visualization_enabled`: Enable visualization (default: False)
- `visualization_update_rate`: Visualization update rate (default: 30.0)

### Configuration Loading/Saving

```python
from nlm import NLMConfig

# Create configuration from dictionary
config_dict = {
    "neuron_count": 1000,
    "region_count": 2,
    "random_seed": 42
}
config = NLMConfig.from_dict(config_dict)

# Save configuration to file
config.save("my_config.json")

# Load configuration from file
config2 = NLMConfig.load("my_config.json")
```

## Advanced Features

### Command-Line Interface

The NLM framework includes a comprehensive command-line interface:

```bash
# Show help
nlm --help

# Show version
nlm --version

# Run Phase 2 demo
nlm phase2

# Run Phase 3 demo with custom parameters
nlm phase3 --episodes 10 --steps 500

# Run Phase 4 demo with trials
nlm phase4 --trials 100

# Run Phase 6 demo
nlm phase6
```

### Performance Profiling

The framework includes built-in performance profiling tools:

```python
from nlm import Brain, NLMConfig
import time

config = NLMConfig()
brain = Brain(config)
brain.initialize()

# Profile simulation performance
start_time = time.time()
for step in range(1000):
    brain.step(step, step * 0.001)
end_time = time.time()

print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Average time per step: {(end_time - start_time) / 1000:.6f} seconds")
```

### Visualization

The framework includes visualization tools for monitoring brain state:

```python
from nlm import Brain, NLMConfig

config = NLMConfig(visualization_enabled=True)
brain = Brain(config)
brain.initialize()

# Enable visualization
brain.enable_visualization()

# Run simulation with visualization
for step in range(100):
    brain.step(step, step * 0.001)
```

### Checkpointing and Replay

The framework supports checkpointing and replay:

```python
from nlm import Brain, NLMConfig

config = NLMConfig()
brain = Brain(config)
brain.initialize()

# Run some steps to create a checkpoint
for step in range(100):
    brain.step(step, step * 0.001)

# Save checkpoint
brain.save_checkpoint("my_checkpoint.bin")

# Later, restore from checkpoint
brain.load_checkpoint("my_checkpoint.bin")

# Replay experiences
brain.replay(100)
```

## API Reference

### Core Classes

#### Brain
- `__init__(config)`: Initialize brain with configuration
- `initialize()`: Initialize brain
- `step(step_num, time)`: Perform one simulation step
- `reset()`: Reset brain to initial state
- `log_status()`: Log current brain status
- `get_total_neuron_count()`: Get total number of neurons
- `get_total_synapse_count()`: Get total number of synapses
- `get_total_spike_count()`: Get total number of spikes

#### World
- `__init__(width, height, vision_width, vision_height, random_seed)`: Initialize world
- `reset()`: Reset world
- `update(dt)`: Update world simulation
- `apply_motor_command(command)`: Apply motor command
- `get_sensory_percept()`: Get sensory percept

#### AgentBrain
- `__init__(brain)`: Initialize agent brain with brain
- `initialize(world)`: Initialize agent brain with world
- `process_sensory_input(percept)`: Process sensory input
- `decode_motor_command()`: Decode motor command
- `apply_reward_modulation(reward, learning_rate)`: Apply reward modulation
- `update_development(dt)`: Update developmental state

#### NLMConfig
- `from_dict(config_dict)`: Create configuration from dictionary
- `to_dict()`: Convert configuration to dictionary
- `save(filepath)`: Save configuration to file
- `load(filepath)`: Load configuration from file

### Helper Classes

#### MotorCommand
- `MOVE_FORWARD`: Move forward
- `MOVE_BACKWARD`: Move backward
- `TURN_LEFT`: Turn left
- `TURN_RIGHT`: Turn right
- `INTERACT`: Interact with environment
- `WAIT`: Wait (no action)

#### DevelopmentalStage
- `INITIAL`: Initial developmental stage
- `CRITICAL_PERIOD`: Critical period for learning
- `MATURATION`: Maturation phase
- `ADULT`: Adult phase
- `AGING`: Aging phase

### Utility Functions

#### get_version()
Get NLM Python API version.

#### get_info()
Get NLM Python API information.

## Advanced Usage

### Custom Neurons

Create custom neurons by extending the base Neuron class:

```python
from nlm import Neuron, NeuronType

class CustomNeuron(Neuron):
    def __init__(self, neuron_id):
        super().__init__(neuron_id)
        self.custom_parameter = 0.0
    
    def custom_update(self, dt):
        self.custom_parameter += dt
```

### Custom Synaptic Plasticity

Create custom plasticity rules by extending the base plasticity classes:

```python
from nlm import PlasticityRule

class CustomPlasticity(PlasticityRule):
    def __init__(self):
        super().__init__()
        self.custom_strength = 1.0
    
    def update(self, synapse, pre_neuron, post_neuron, dt):
        # Custom plasticity rule
        self.custom_strength *= 0.99
```

### Plugin Architecture

The framework supports plugin architecture for extending functionality:

```python
# Create plugin
plugin = MyCustomPlugin()

# Register plugin
from nlm import register_plugin
register_plugin(plugin)

# Use plugin
brain.apply_plugin(plugin)
```

## Error Handling

The NLM Python API provides comprehensive error handling:

```python
from nlm import NLMError, ConfigError, BrainInitializationError

import sys

try:
    brain = Brain(config)
    brain.initialize()
    brain.step(0, 0.0)
except NLMError as e:
    print(f"NLM Error: {e}")
    sys.exit(1)
except ConfigError as e:
    print(f"Configuration Error: {e}")
    sys.exit(1)
except BrainInitializationError as e:
    print(f"Brain Initialization Error: {e}")
    sys.exit(1)
```

## Examples

### Example 1: Simple World Navigation

```python
from nlm import Brain, World, AgentBrain, NLMConfig

# Create brain
brain = Brain()
brain.initialize()

# Create world
world = World(20.0, 20.0)

# Create agent brain
agent_brain = AgentBrain(brain)
agent_brain.initialize(world)

# Run simulation
for step in range(100):
    percept = world.get_sensory_percept()
    agent_brain.process_sensory_input(percept)
    
    command = agent_brain.decode_motor_command()
    action_result = world.apply_motor_command(command)
    
    agent_brain.apply_reward_modulation(action_result.reward)
    
    world.update(0.01)
    agent_brain.update_development(0.01)

print("Navigation complete!")
```

### Example 2: Learning to Navigate

```python
from nlm import Network, NLMConfig

# Configure for learning
config = NLMConfig(
    neuron_count=1000,
    environment_width=30.0,
    environment_height=30.0,
    random_seed=42
)

# Create network
network = Network(config)

# Run learning simulation
results = network.run(steps=500)

# Analyze results
print(f"Total reward accumulated: {results['total_reward']:.2f}")
print(f"Average reward per step: {results['total_reward'] / results['steps_completed']:.4f}")
```

### Example 3: Advanced Configuration

```python
from nlm import NLMConfig

# Create advanced configuration
config = NLMConfig(
    neuron_count=2000,
    region_count=4,
    connection_probability=0.2,
    simulation_timestep=0.0005,
    plasticity_learning_rate=0.02,
    enable_checkpointing=True,
    enable_replay=True,
    enable_development=True
)

# Save configuration
config.save("advanced_config.json")

# Load configuration
config2 = NLMConfig.load("advanced_config.json")

# Verify configuration
print(f"Neuron count: {config2.neuron_count}")
print(f"Region count: {config2.region_count}")
print(f"Learning rate: {config2.plasticity_learning_rate}")
```

## Troubleshooting

### Common Issues

#### Issue: Import Error
```bash
ImportError: cannot import name '_pynlm'
```

**Solution**: The C++ extensions are not available. Install the package from source:

```bash
pip install -e .
```

#### Issue: Missing Dependencies
```bash
ModuleNotFoundError: No module named 'numpy'
```

**Solution**: Install required dependencies:

```bash
pip install numpy
```

#### Issue: Brain Not Initialized
```bash
NLMError: Brain not initialized. Call initialize() first.
```

**Solution**: Always initialize the brain before using it:

```python
brain = Brain(config)
brain.initialize()
```

### Performance Issues

#### Issue: Slow Simulation
**Solution**: Use more efficient configuration:

```python
config = NLMConfig(
    neuron_count=500,  # Reduce neuron count
    simulation_timestep=0.001,  # Increase timestep
    connection_probability=0.05,  # Reduce connectivity
)
```

### Debugging

#### Enable Debug Logging
```python
import logging
logging.basicConfig(level=logging.DEBUG)

from nlm import Brain
brain = Brain()
brain.initialize()
```

## Support

### Documentation
- [GitHub Repository](https://github.com/nlm-project/pynlm)
- [Documentation](https://docs.nlm.org)
- [API Reference](https://docs.nlm.org/api)

### Contact
- Email: support@nlm.org
- GitHub Issues: [GitHub Repository](https://github.com/nlm-project/pynlm/issues)

### Contributing
- Fork the repository
- Create a feature branch
- Submit a pull request

## License

MIT License. See LICENSE file for details.

## Acknowledgements

This project is based on the NLM (Neural Learning Machine) research framework developed by the NLM Research Team.

## Version History

### Version 0.1.0
- Initial release with core NLM functionality
- Basic brain simulation
- World interaction
- Agent-brain interface
- Memory and prediction systems

### Version 0.2.0
- Enhanced Python API
- Configuration management
- Performance profiling
- Visualization tools
- Checkpointing and replay

### Future Versions
- Distributed computing
- Advanced plasticity rules
- Enhanced visualization
- Custom plugin architecture
- Integration with other frameworks
