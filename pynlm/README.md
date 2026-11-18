# NLM Python Package - Advanced API

## Overview

This document provides comprehensive documentation for the improved NLM (Neural Learning Machine) Python package. The package has been restructured to provide better organization, maintainability, and advanced features while maintaining full backward compatibility.

## Package Structure

```
pynlm/
├── __init__.py              # Main package initialization
├── core/                   # Core types and utilities
│   ├── __init__.py
│   ├── types.py           # NeuronId, SynapseId, RegionId, PopulationId
│   └── enums.py           # All enumerations
├── brain.py               # Brain class with NeuralRegion and simulation engine
├── world.py              # SimpleWorld class with physics simulation and perception
├── agent.py              # AgentBrain class with sensory-motor integration
├── config.py             # Config class for configuration management
├── sensory.py            # Vision, Audio, and InternalSignals classes
├── motor.py              # Action, WorldObject, AgentBody, ActionResult classes
├── visualization.py      # Visualization utilities and analysis tools
├── utils.py              # Factory functions, simulation utilities, and advanced features
└── README.md             # This file

pynlm_cli.py             # Command-line interface tools
```

## Key Improvements

### 1. **Modern Python Package Structure**
- Organized into logical modules for better maintainability
- Follows Python best practices (PEP 8, type hints, comprehensive docstrings)
- Provides clear separation of concerns

### 2. **Backward Compatibility**
- Full API compatibility with existing `pynlm` module
- All existing factory functions preserved:
  - `createDefaultConfig()`
  - `createBrain(config)`
  - `createSimpleWorld()`
  - `createAgentBrain(brain)`
- All core types and enumerations maintained

### 3. **Enhanced Error Handling**
- Custom exception classes for better error management
- More descriptive error messages
- Improved type validation

### 4. **Advanced CLI Tools**
- `pynlm_cli.py` provides comprehensive command-line interfaces for:
  - Configuration management
  - Brain simulation operations
  - World simulation operations
  - Performance benchmarking

### 5. **Convenience Utilities**
- `run_simulation()` - High-level simulation runner
- `run_agent_simulation()` - Complete agent-environment simulation
- `benchmark_brain()` - Performance benchmarking utility

## API Documentation

### Core Types

#### NeuronId, SynapseId, RegionId, PopulationId
```python
# Create identifiers
neuron_id = pynlm.NeuronId(42)
synapse_id = pynlm.SynapseId(123)
region_id = pynlm.RegionId(999)
population_id = pynlm.PopulationId(777)

# Access values
print(neuron_id.value)  # 42
print(neuron_id.index())  # 42

# Equality and hashing
id1 = pynlm.NeuronId(42)
id2 = pynlm.NeuronId(42)
print(id1 == id2)  # True
print(hash(id1) == hash(id2))  # True
```

### Enumerations

#### NeuronType
```python
from pynlm import NeuronType

# Available neuron types
neuron_types = [
    NeuronType.Excitatory,
    NeuronType.Inhibitory,
    NeuronType.Modulatory,
    NeuronType.Sensory,
    NeuronType.Motor,
    NeuronType.Internal
]

# Use in brain configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
```

#### ActionType
```python
from pynlm import ActionType

# Available action types
actions = [
    ActionType.MoveForward,
    ActionType.MoveBackward,
    ActionType.TurnLeft,
    ActionType.TurnRight,
    ActionType.Look,
    ActionType.Interact,
    ActionType.Eat,
    ActionType.Drink,
    ActionType.Rest,
    ActionType.Wait,
    ActionType.Custom
]

# Create actions
action = pynlm.Action(ActionType.MoveForward)
parameters = action.getParameters()  # []
```

### Brain Operations

#### Basic Brain Usage
```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
```

#### Brain Configuration
```python
# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

### World Operations

#### Simple World Usage
```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Set agent start position
world.setAgentStart(10.0, 10.0)

# Run world simulation
for step in range(50):
    world.update(0.1)  # Update world state
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Process sensory input in brain
    # (requires agent brain interface)
```

### Agent Operations

#### Complete Agent-Environment Simulation
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
        
        # Print progress periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

### Configuration Management

#### Config Class
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Check if configuration key exists
if config.has("brain.neuron_count"):
    print(f"Current neuron count: {config.get('brain.neuron_count')}")

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.001)

# Get all configuration keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")
```

### Sensory Processing

#### Vision Input
```python
import pynlm

# Create vision sensory input
vision = pynlm.Vision(width=10, height=10, channels=3)

# Set visual data (e.g., RGB image)
# data should be flattened list of pixel values
test_data = [0.1, 0.2, 0.3] * 100  # 10x10x3 = 300 elements
vision.setData(test_data)

# Access visual properties
print(f"Vision dimensions: {vision.getDimensions()}")
print(f"Vision width: {vision.getWidth()}")
print(f"Vision height: {vision.getHeight()}")
print(f"Vision channels: {vision.getChannels()}")
```

### Action System

#### Action Class
```python
import pynlm

# Create action with type only
action1 = pynlm.Action(pynlm.ActionType.MoveForward)
print(f"Action type: {action1.getType()}")
print(f"Action parameters: {action1.getParameters()}")

# Create action with type and parameters
params = [0.1, 0.2, 0.3]
action2 = pynlm.Action(pynlm.ActionType.TurnLeft, params)
print(f"Action name: {action2.getName()}")

# Modify action
action2.setType(pynlm.ActionType.TurnRight)
action2.setParameters([0.5, 0.5])

# Clone action
action3 = action2.clone()
```

### Error Handling

The pynlm package includes custom exception classes for better error management:

```python
import pynlm

# Custom exception classes are available for specific error types
try:
    # Brain initialization
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.RuntimeError as e:
    print(f"Brain initialization failed: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Command-Line Interface

The package includes an advanced command-line interface tool `pynlm_cli.py` with the following commands:

### Configuration Management
```bash
# Create a new configuration
pynlm_cli config create --neuron-count 500 --synapse-density 0.2

# List available configuration templates
pynlm_cli config list

# Export configuration to JSON
pynlm_cli config export --format json
```

### Brain Simulation
```bash
# Run brain simulation
pynlm_cli brain simulate --steps 1000 --neuron-count 200 --verbose

# Analyze brain state
pynlm_cli brain analyze brain_state.bin --format json

# Create new brain
pynlm_cli brain create --config my_config.json
```

### World Simulation
```bash
# Create and configure world
pynlm_cli world create --width 30 --height 25 --vision-width 12 --vision-height 10

# Add objects to world
pynlm_cli world add-object --x 5.0 --y 5.0 --type resource --value 1.5

# Run world simulation with agent
pynlm_cli world run --steps 500 --verbose
```

### Performance Benchmarking
```bash
# Benchmark NLM performance
pynlm_cli benchmark --steps 1000 --neuron-count 100 --trials 3

# Save benchmark results
pynlm_cli benchmark --steps 1000 --neuron-count 100 --output benchmark_results.json
```

## Convenience Functions

### High-Level Simulation Utilities

The `pynlm.utils` module provides convenient factory functions for common tasks:

#### run_simulation()
```python
from pynlm.utils import run_simulation

# Run a simple simulation
def my_simulation_loop(brain, num_steps):
    for step in range(num_steps):
        brain.step(step)
        yield step, brain.getFiringNeuronCount()

# Run simulation
results = run_simulation(my_simulation_loop, brain=100, num_steps=100)
```

#### run_agent_simulation()
```python
from pynlm.utils import run_agent_simulation

# Run complete agent simulation (see example above)
run_agent_simulation(1000)
```

#### benchmark_brain()
```python
from pynlm.utils import benchmark_brain

# Benchmark brain performance
benchmark_results = benchmark_brain(brain, steps=1000, trials=5)
```

## Installation and Usage

### Install from Source
```bash
# Install dependencies
pip install pybind11 scikit-build-core pytest numpy

# Install pynlm in development mode
pip install -e .

# Or build and install
pip install .
```

### Basic Usage
```python
import pynlm

# Create and run simple simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

### Advanced Usage
```python
import pynlm
from pynlm.utils import run_agent_simulation

# Run advanced agent simulation with learning
run_agent_simulation(num_steps=5000, 
                    neuron_count=200, 
                    enable_learning=True)
```

## Migration Guide

### From Legacy pynlm to New Structure

The new package structure maintains full backward compatibility. Existing code will continue to work without modifications:

```python
# Old code (still works)
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

# New code (optional, more organized)
from pynlm import Brain, createBrain, createDefaultConfig

brain = createBrain(createDefaultConfig())
brain.initialize()
brain.step(0)
```

### Available Modules

For advanced users who want to use specific modules:

```python
# Import specific modules
from pynlm.brain import Brain
from pynlm.world import SimpleWorld
from pynlm.agent import AgentBrain
from pynlm.config import Config
from pynlm.core.types import NeuronId, SynapseId
from pynlm.core.enums import NeuronType

# Import all utilities
from pynlm.utils import run_simulation, run_agent_simulation, benchmark_brain
```

## Performance Tips

1. **Reuse Configurations**: Create configuration objects once and reuse them
2. **Batch Operations**: Use vectorized operations where possible
3. **Memory Management**: Clear references to large objects when no longer needed
4. **Simulation Control**: Use appropriate time steps for your application

## Troubleshooting

### Common Issues

**ImportError: No module named 'pynlm'**
- Ensure pynlm is installed: `pip install -e .`
- Check Python path: `pip install -e .` should install to site-packages

**RuntimeError during brain initialization**
- Ensure `brain.initialize()` is called before `brain.step()`
- Check that configuration parameters are valid

**Memory issues with large simulations**
- Monitor memory usage during long simulations
- Consider using smaller brain configurations for testing

### Getting Help

For additional assistance:
1. Check the documentation in `easy_usage.md` and `HOW_TO_USE.md`
2. Use the command-line interface: `pynlm_cli.py --help`
3. Review the examples in the `examples/` directory (if created)
4. Check the source code for detailed docstrings and type hints

## Future Enhancements

The package is designed for extensibility. Future enhancements may include:

1. **Additional Visualization Tools**: More advanced plotting and analysis utilities
2. **Plugin Architecture**: Support for user-defined brain components
3. **Distributed Computing**: Support for parallel and distributed simulations
4. **Advanced Learning Algorithms**: More sophisticated plasticity and learning rules
5. **Hardware Acceleration**: GPU support for large-scale simulations

## License

MIT License - See LICENSE file for details.

## Authors

NLM Authors - Neural Learning Machine Research Project

## Version Information

- Package version: 0.1.0
- Build date: Current
- Dependencies: pybind11, scikit-build-core, numpy, pytest