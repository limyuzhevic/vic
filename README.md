# NLM - Neural Learning Machine

**An Experimental Artificial Developmental Brain**

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Key Improvements

This version of NLM includes significant improvements to the Python bindings and overall usability:

### ✅ **Python Package Structure**
- Complete Python package with `pynlm/` module
- Comprehensive documentation and examples
- Proper error handling and validation
- Type hints for better IDE support

### ✅ **Enhanced Python API**
- Full implementation of documented Python functions (`createDefaultConfig`, `createBrain`, etc.)
- Proper `__version__` attribute
- Complete class definitions for all major NLM components
- Comprehensive example scripts

### ✅ **Configuration System**
- JSON/YAML file support (implemented as part of Config class)
- Command-line argument parsing
- Runtime configuration with validation
- Configuration persistence

### ✅ **Error Handling**
- Comprehensive exception handling throughout
- Clear error messages
- Graceful failure handling
- Validation for all operations

### ✅ **Documentation**
- Complete API documentation
- Usage examples
- Configuration reference
- Architecture overview

## Installation

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install scikit-build-core pybind11 numpy

# Build and install the Python package
pip install -e .

# Or install in development mode
pip install -e .
```

### Using pip

```bash
# Install from PyPI (when available)
pip install pynlm
```

## Quick Start

### Minimal Example

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete! Neurons firing: {brain.getFiringNeuronCount()}")
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
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
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

## API Reference

### Core Classes

#### `pynlm.Config`
Configuration management for NLM system.

**Methods:**
- `Config()` - Create configuration instance
- `loadFromFile(filepath)` - Load from JSON/YAML file
- `loadFromArgs(argv)` - Load from command line arguments
- `saveToFile(filepath)` - Save to JSON/YAML file
- `has(key)` - Check if key exists
- `get(key, default)` - Get value with default
- `getOr(key, default)` - Get value or default
- `set(key, value, source)` - Set configuration value
- `remove(key)` - Remove key
- `clear()` - Clear all configuration
- `summary()` - Get configuration summary

#### `pynlm.Brain`
Central neural simulation brain.

**Methods:**
- `Brain(config)` - Create brain with configuration
- `initialize()` - Initialize brain
- `step(currentStep, currentTime=None)` - Run simulation step
- `receiveSensoryInput(input_data)` - Inject sensory input
- `injectCurrent(neuron_id, current)` - Inject current into neuron
- `injectCurrentToNeurons(neuron_type, current)` - Inject current by type
- `produceAction()` - Get motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state

**Properties:**
- `getTotalNeuronCount()` - Total neurons
- `getTotalSynapseCount()` - Total synapses
- `getFiringNeuronCount()` - Currently firing
- `getAverageFiringRate()` - Average firing rate
- `getExcitationInhibitionRatio()` - E/I balance
- `getTotalSpikeCount()` - Total spikes
- `getDevelopmentalStage()` - Current developmental stage

#### `pynlm.AgentBrain`
Bridge between brain and world.

**Methods:**
- `AgentBrain(brain)` - Create agent interface
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predicted)` - Apply reward
- `updateDevelopment(timestep)` - Update development
- `reset()` - Reset agent

**Configuration Methods:**
- `enableRewardModulation(bool)` - Enable/disable reward modulation
- `enableStructuralPlasticity(bool)` - Enable/disable structural plasticity
- `enableDevelopment(bool)` - Enable/disable development
- `enableCuriosity(bool)` - Enable/disable curiosity

#### `pynlm.SimpleWorld`
Simple 2D simulation world.

**Methods:**
- `SimpleWorld()` - Create world
- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `reset()` - Reset world
- `setAgentStart(x, y)` - Set agent start position
- `update(timestep)` - Update world
- `applyMotorCommand(cmd, time)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body state
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove object
- `isValidPosition(x, y)` - Check position validity

### Enumerations

#### Neuron Types
- `pynlm.NeuronType.EXCITATORY`
- `pynlm.NeuronType.INHIBITORY`
- `pynlm.NeuronType.MODULATORY`
- `pynlm.NeuronType.SENSORY`
- `pynlm.NeuronType.MOTOR`
- `pynlm.NeuronType.INTERNAL`

#### Developmental Stages
- `pynlm.DevelopmentalStage.INITIAL`
- `pynlm.DevelopmentalStage.CRITICALPERIOD`
- `pynlm.DevelopmentalStage.MATURATION`
- `pynlm.DevelopmentalStage.ADULT`
- `pynlm.DevelopmentalStage.AGING`

#### Action Types
- `pynlm.ActionType.MOVEFORWARD`
- `pynlm.ActionType.MOVEBACKWARD`
- `pynlm.ActionType.MOVELEFT`
- `pynlm.ActionType.MOVERIGHT`
- `pynlm.ActionType.TURNLEFT`
- `pynlm.ActionType.TURNRIGHT`
- `pynlm.ActionType.INTERACT`
- `pynlm.ActionType.EAT`
- `pynlm.ActionType.DRINK`
- `pynlm.ActionType.REST`
- `pynlm.ActionType.WAIT`
- `pynlm.ActionType.CUSTOM`

#### World Object Types
- `pynlm.WorldObjectType.EMPTY`
- `pynlm.WorldObjectType.RESOURCE`
- `pynlm.WorldObjectType.HAZARD`
- `pynlm.WorldObjectType.WALL`
- `pynlm.WorldObjectType.MARKER`

### Helper Functions

#### Factory Functions
- `pynlm.createDefaultConfig()` - Create default configuration
- `pynlm.createBrain(config)` - Create brain instance
- `pynlm.createSimpleWorld()` - Create world instance
- `pynlm.createAgentBrain(brain)` - Create agent interface

#### Global Constants
- `pynlm.INVALID_NEURON_ID`
- `pynlm.INVALID_SYNAPSE_ID`
- `pynlm.INVALID_REGION_ID`
- `pynlm.INVALID_POPULATION_ID`

## Configuration

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.v_thresh` | float | -50.0 | Neuron threshold (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |

### Configuration Examples

#### Command Line Arguments
```bash
./nlm --brain.neuron_count=2000 --plasticity.stdp.enable=true --neuromod.curiosity.enable=false
```

#### JSON Configuration
```json
{
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.15,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        },
        "hebbian": {
            "enable": true
        }
    },
    "neuromod": {
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    }
}
```

## Building from Source

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

## Testing

### Running Examples

```bash
# Run example scripts
python examples.py

# Run specific examples
python -c "import pynlm; print('pynlm version:', pynlm.__version__)"
```

### Running Tests

```bash
# Install testing dependencies
pip install pytest numpy

# Run tests
pytest

# Run tests with verbose output
pytest -v

# Run specific test module
pytest tests/test_config.py
```

## Troubleshooting

### ImportError: No module named 'pynlm'

```bash
# Install the package
pip install -e .

# Or force reinstall
pip install --force-reinstall -e .
```

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

**"My brain isn't doing anything":**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving":**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0":**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

## Development

### Adding New Features

1. **Python API**: Add new functions to the appropriate module in `pynlm/`
2. **C++ Integration**: Implement corresponding functionality in the C++ codebase
3. **Documentation**: Update the API reference and examples
4. **Tests**: Add unit tests for new functionality

### Contributing

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests and documentation
5. Submit a pull request

## License

MIT License. See LICENSE file for details.

## Authors

Research project — See docs for scientific background.

## Version History

### Version 0.1.0
- Initial release with comprehensive Python bindings
- Full API matching documentation
- Complete configuration system
- Extensive example scripts
- Error handling and validation

## Future Development

### Planned Features
1. **Advanced Python API**: More sophisticated configuration and scripting capabilities
2. **Performance Optimizations**: SIMD, parallelization, and memory improvements
3. **Visualization Tools**: Better graphical visualization of brain activity
4. **Distributed Computing**: Support for multi-agent and distributed simulations
5. **Learning Experiments**: Built-in experiments for studying learning dynamics

### Research Goals
- Understanding how neural systems develop and learn
- Investigating the relationship between structure and function
- Exploring the emergence of complex behavior from simple principles
- Developing practical applications of brain-inspired computation

## Support

For questions, issues, or discussions, please:
- Check the documentation in the `docs/` directory
- Visit the project repository
- Create issues on GitHub
- Join the discussion community

---

*This project is an experimental research project investigating computational brain-like systems. Current limitations include simplified neuron models and limited biological realism. Results should not be extrapolated beyond the scope of this research.*