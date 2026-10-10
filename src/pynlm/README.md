# NLM Python Bindings

This directory contains the Python bindings for the NLM (Neural Learning Machine) project.

## Overview

The NLM Python bindings provide a high-level interface to the NLM C++ neural simulation framework. They enable Python users to create and run neural simulations without needing to understand the underlying C++ codebase.

## Installation

### From Source (Recommended for Development)

```bash
# Clone the repository
cd /path/to/nlm

# Install with pip (builds the C++ extension automatically)
pip install -e .

# Or install in development mode
pip install -e .[dev]
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

### Requirements

- Python 3.8+
- pip
- Build tools (gcc, cmake, etc.)

## Basic Usage

### Creating a Simple Brain

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

### Running a Simulation

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

### Using with a World

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

# Run episode
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

## API Reference

### Core Classes

#### Config
Configuration management for NLM system.

- `loadFromFile(filepath)` - Load configuration from file (JSON/YAML/simple format)
- `loadFromArgs(argc, argv)` - Load configuration from command line arguments
- `saveToFile(filepath)` - Save configuration to file
- `has(key)` - Check if configuration key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration
- `summary()` - Get configuration summary
- `set(key, value, source)` - Set configuration value

#### Brain
Central neural simulation brain class.

- `initialize()` - Initialize brain with configuration
- `step(currentStep)` - Perform a simulation step
- `step(currentStep, currentTime)` - Perform a simulation step with timestamp
- `reset()` - Reset brain state
- `receiveSensoryInput(input)` - Inject sensory input into brain
- `produceAction()` - Produce motor action based on neural activity
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file
- Various getters for neuron count, synapse count, spike statistics

#### SimpleWorld
Simple 2D world for NLM simulation.

- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset world to initial state
- `update(timestep)` - Update world by timestep seconds
- `applyMotorCommand(cmd, currentTime)` - Apply motor command to world
- `getSensoryPercept()` - Get sensory percept from world
- `getAgentBody()` - Get agent body state
- `setAgentStart(x, y)` - Set agent starting position

#### AgentBrain
Agent brain interface connecting NLM brain to world.

- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory percept
- `decodeMotorCommand()` - Decode brain activity into motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward signals
- `enableRewardModulation(bool)` - Enable/disable reward modulation
- `enableStructuralPlasticity(bool)` - Enable/disable structural plasticity
- `enableDevelopment(bool)` - Enable/disable development
- `enableCuriosity(bool)` - Enable/disable curiosity

### Factory Functions

- `createDefaultConfig()` - Create default configuration
- `createBrain(config)` - Create new brain with configuration
- `createSimpleWorld()` - Create new simple world
- `createAgentBrain(brain)` - Create agent brain interface

### Enumerations

#### NeuronType
- `Excitatory` (0)
- `Inhibitory` (1)
- `Modulatory` (2)
- `Sensory` (3)
- `Motor` (4)
- `Internal` (5)

#### SynapseType
- `Excitatory` (0)
- `Inhibitory` (1)
- `Modulatory` (2)
- `Electrical` (3)
- `GapJunction` (4)

#### ActionType
- `MoveForward` (0), `MoveBackward` (1), `MoveLeft` (2), `MoveRight` (3)
- `TurnLeft` (4), `TurnRight` (5), `Look` (6), `LookUp` (7), `LookDown` (8)
- `Interact` (9), `Eat` (10), `Drink` (11), `Rest` (12), `Wait` (13), `Custom` (14)

#### MotorCommand
- `MoveForward` (0), `MoveBackward` (1), `TurnLeft` (2), `TurnRight` (3)
- `LookLeft` (4), `LookRight` (5), `Interact` (6), `Wait` (7)

#### WorldObjectType
- `Empty` (0), `Resource` (1), `Hazard` (2), `Wall` (3), `Marker` (4)

#### DevelopmentalStage
- `Initial` (0), `CriticalPeriod` (1), `Maturation` (2), `Adult` (3), `Aging` (4)

#### FiringState
- `Resting` (0), `Active` (1), `Refractory` (2), `Inhibited` (3)

### Type Aliases

- `NeuronId` - Unique identifier for a neuron
- `SynapseId` - Unique identifier for a synapse
- `RegionId` - Unique identifier for a brain region
- `PopulationId` - Unique identifier for a neuron population

### Constants

- `INVALID_NEURON_ID` - Invalid neuron ID
- `INVALID_SYNAPSE_ID` - Invalid synapse ID
- `INVALID_REGION_ID` - Invalid region ID
- `INVALID_POPULATION_ID` - Invalid population ID

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |

## Advanced Usage

### Subsystems

The NLM Python bindings provide access to various learning subsystems:

- **Reward Modulation** - Learn from rewards using dopamine signals
- **Structural Plasticity** - Grow and prune synaptic connections
- **Development** - Brain maturation and stage progression
- **Curiosity** - Drive for exploration and novelty
- **Working Memory** - Temporary storage of neural activity patterns
- **Episodic Memory** - Experience encoding and recall
- **Concept Formation** - Pattern discovery without labels

### Example with Advanced Features

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(1000):
    # Update world and process sensory input
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    
    # Run brain step
    brain.step(step)
    
    # Get action and apply it
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {agent.getDevelopmentalStage()}")
```

## Development

### Building from Source

```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build the extension
pip install .

# Run tests
pytest tests/
```

### Testing

Run the test suite:

```bash
pytest tests/ -v
pytest tests/ --cov=pynlm --cov-report=html
```

### Code Quality

The Python bindings use:

- **Black** for code formatting (line length: 100)
- **Ruff** for linting
- **Mypy** for type checking
- **Pytest** for testing

To run code quality checks:

```bash
# Format code
black src/

# Lint code
ruff check src/

# Type check
cpython -m mypy src/

# Run tests
pytest tests/
```

## Examples

See the `example.py` file in this directory for comprehensive examples of NLM Python usage.

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

## License

MIT License. See the LICENSE file for details.

## Authors

NLM Authors - See docs/ARCHITECTURE.md for more information.

## Further Reading

- [NLM Documentation](https://nlm.readthedocs.io) - Detailed architecture and usage documentation
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
