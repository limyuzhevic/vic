# NLM Python Bindings - Quick Start

This repository contains Python bindings for the NLM (Neural Learning Machine) C++ neural simulation framework.

## Installation

### Prerequisites
- Python 3.8+
- scikit-build-core
- pybind11
- CMake
- C++ compiler

### Installation Methods

#### Method 1: From the repository (Development)

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Install in development mode
pip install -e .
```

#### Method 2: Using the setup script

```bash
# Run the setup script
python install.py
```

#### Method 3: Using pip directly

```bash
# Install with pip (this will build the bindings)
pip install scikit-build-core pybind11
pip install -e .
```

## Basic Usage

### Quick Test

```python
import pynlm

# Quick test to verify bindings work
brain = pynlm.quick_test()
print(f"Brain has {brain.getTotalNeuronCount()} neurons")
```

### Creating a Brain

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Running a Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Example

```python
import pynlm

# Create complete simulation
sim = pynlm.create_simulation()
brain = sim['brain']
world = sim['world']
agent = sim['agent']

# Run agent episode
results = pynlm.run_agent_episode(steps=100, enable_features=True)

print(f"Episode completed: {results['steps_run']} steps")
print(f"Final curiosity: {results['final_stats']['curiosity']:.3f}")
```

### Using the CLI

```bash
# Run quick test
python -m pynlm.cli quick

# Run basic tests
python -m pynlm.cli basic

# Run demo
python -m pynlm.cli demo

# Run agent test
python -m pynlm.cli agent

# Run all tests
python -m pynlm.cli test
```

## Examples

### Basic Examples

```bash
# Run basic examples
python -m pynlm.examples.basic
```

### Advanced Examples

```bash
# Run advanced examples
python -m pynlm.examples.advanced
```

### Demo Examples

```bash
# Run demo examples
python -m pynlm.examples.demo
```

## Testing

### Running Tests

```bash
# Run all tests
python -m pytest pynlm/nlm_tests.py -v

# Or use the CLI
python -m pynlm.cli test
```

### Test Output

The tests will verify:
- Module import and metadata
- Configuration creation
- Brain creation and initialization
- Simulation execution
- Convenience functions
- Enumerations
- Constants

## API Documentation

### Main Classes

#### Config
- `createDefaultConfig()`: Create default configuration
- `loadFromFile(filepath)`: Load configuration from JSON file
- `saveToFile(filepath)`: Save configuration to JSON file
- `has(key)`: Check if configuration key exists
- `getKeys()`: Get all configuration keys
- `set(key, value)`: Set configuration value

#### Brain
- `initialize()`: Initialize the brain
- `step(step)`: Run one simulation step
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state
- `getTotalNeuronCount()`: Get total neurons
- `getTotalSynapseCount()`: Get total synapses
- `getFiringNeuronCount()`: Get currently firing neurons
- `getTotalSpikeCount()`: Get total spike count
- `getAverageFiringRate()`: Get average firing rate
- `getExcitationInhibitionRatio()`: Get E/I ratio

#### SimpleWorld
- `configure(width, height, visionWidth, visionHeight)`: Configure world dimensions
- `update(timestep)`: Update world state
- `reset()`: Reset world to initial state
- `getSensoryPercept()`: Get sensory input
- `getAgentBody()`: Get agent body state

#### AgentBrain
- `initialize(world)`: Initialize agent brain with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command
- `applyRewardModulation(reward, predicted)`: Apply reward modulation
- `updateDevelopment(timestep)`: Update development
- `enableRewardModulation(bool)`: Enable/disable reward modulation
- `enableCuriosity(bool)`: Enable/disable curiosity
- `enableDevelopment(bool)`: Enable/disable development
- `enableStructuralPlasticity(bool)`: Enable/disable structural plasticity

### Enumerations

#### NeuronType
- `Excitatory`
- `Inhibitory`
- `Modulatory`
- `Sensory`
- `Motor`
- `Internal`

#### ActionType
- `MoveForward`
- `MoveBackward`
- `MoveLeft`
- `MoveRight`
- `TurnLeft`
- `TurnRight`
- `Look`
- `LookUp`
- `LookDown`
- `Interact`
- `Eat`
- `Drink`
- `Rest`
- `Wait`
- `Custom`

#### WorldObjectType
- `Empty`
- `Resource`
- `Hazard`
- `Wall`
- `Marker`

### Convenience Functions

#### create_simulation(config_dict)
Create a complete simulation with brain, world, and agent.

#### run_agent_episode(steps, enable_features)
Run a complete agent simulation episode.

#### quick_test()
Quick test of basic NLM functionality.

## Error Handling

### Custom Exceptions

- `NLMError`: Base exception for all NLM errors
- `ConfigurationError`: Error in configuration
- `SimulationError`: Error during simulation
- `ValidationError`: Validation error

### Error Handling Example

```python
import pynlm

try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
        
except pynlm.NLMError as e:
    print(f"Error: {e}")
    print("Make sure to call brain.initialize() before running simulation")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
```

## Troubleshooting

### "My brain isn't doing anything"

**Solution:** Did you call `brain.initialize()`? Brains need to be initialized before they can simulate.

### "The agent isn't moving"

**Solution:** Make sure `world.applyMotorCommand()` is being called. Check that `world.update()` is being called in your main loop.

### "Everything is 0"

**Solution:** Brains need time to "warm up" - try more steps. Some neurons need input to fire - make sure sensory input is connected.

### "ImportError: No module named 'pynlm'"

**Solution:**
```bash
pip install --force-reinstall .
```

### "Build issues"

**Solution:** Ensure you have:
- A C++ compiler (g++, clang, or MSVC)
- CMake >= 3.16
- Python development headers
- pybind11
- scikit-build-core

### "Build errors"

**Solution:** Check that all source files are present and the build system is properly configured.

## Advanced Features

### Configuration

The NLM framework supports extensive configuration options. Common options include:

```python
# Neural parameters
brain.neuron_count = 1000
brain.synapse_density = 0.1
brain.v_thresh = -50.0
brain.v_reset = -70.0

# Plasticity parameters
plasticity.stdp.enable = True
plasticity.stdp.learning_rate = 0.001
plasticity.hebbian.enable = True

# Neuromodulation parameters
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = True
neuromod.novelty.enable = True
```

### Development and Plasticity

```python
# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation with development
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.updateDevelopment(0.1)
```

### State Management

```python
# Save brain state
brain.save("checkpoint.bin")

# Load brain state
brain.load("checkpoint.bin")
```

### Multi-Agent Simulation

```python
# Create multiple agents
agents = []
brains = []

for i in range(3):
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    agents.append(agent)
    brains.append(brain)

# Share world
world = pynlm.createSimpleWorld()
world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
world.reset()

# Initialize agents
for i, agent in enumerate(agents):
    agent.initialize(world)

# Run simulation with all agents
for step in range(100):
    world.update(0.1)
    
    for i, agent in enumerate(agents):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brains[i].step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

## License

MIT

## Authors

NLM Authors

## Project Structure

```
nlm/
├── python/
│   ├── __init__.py              # Main Python module
│   ├── python_bindings.cpp       # C++ pybind11 bindings
│   └── CMakeLists.txt           # Python bindings build configuration
│
├── src/                         # C++ source code
│   ├── brain/                  # Brain components
│   ├── core/                   # Core utilities
│   ├── world/                  # World simulation
│   └── ...                     # Other components
│
├── tests/                       # C++ tests
│   ├── test_brain.cpp
│   └── ...
│
├── docs/                        # Documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   └── ...
│
├── README.md
├── HOW_TO_USE.md
└── easy_usage.md
```

## Acknowledgements

This project uses the following technologies:
- pybind11 for Python bindings
- scikit-build-core for building
- C++20 for the core simulation framework
- CMake for build system

## Further Reading

- [NLM Documentation](https://nlm.readthedocs.io)
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

## Contact

For questions, issues, or feature requests, please contact:
- Email: contact@nlm.org
- GitHub: https://github.com/nlm-project/nlm

---

## Quick Start Summary

1. **Install**: `pip install -e .`
2. **Import**: `import pynlm`
3. **Test**: `python test_nlm_bindings.py`
4. **Demo**: `python demo.py`
5. **Examples**: `python -m pynlm.cli demo`

Enjoy using the NLM Python bindings!
