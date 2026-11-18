# NLM API Documentation

This document provides comprehensive documentation for the NLM (熙然) Neural Learning Machine API,
covering both the core C++ API and the enhanced Python Python bindings.

## Core C++ API

### Classes

#### Brain
**Location:** `src/brain/Brain.hpp` (Phase 2)

**Description:** The central neural simulation brain class implementing real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

**Key Features:**
- Real LIF neuron dynamics
- Event-driven spike propagation with delays
- Integrated STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)
- Memory systems (working, episodic, associative)
- Prediction system
- Neuromodulation (dopamine, curiosity, novelty)
- Developmental system
- Checkpoint save/load functionality

**Main Methods:**
- `Brain(std::shared_ptr<Config> config)` - Constructor with configuration
- `bool initialize()` - Initialize brain with configuration
- `void step(SimulationStep currentStep)` - Main simulation step
- `void step(SimulationStep currentStep, Timestamp currentTime)` - Simulation step with timestamp
- `void receiveSensoryInput(const SensoryInput& input)` - Inject sensory input
- `void injectCurrent(NeuronId neuron, MembranePotential current)` - Inject current to specific neuron
- `void injectCurrentToNeurons(NeuronType type, MembranePotential current)` - Inject current to neurons of specific type
- `std::unique_ptr<Action> produceAction()` - Produce motor action
- `void applyNeuromodulation(const Neuromodulator& signal)` - Apply neuromodulation
- `void updatePlasticity()` - Update plasticity rules
- `void develop()` - Apply developmental changes
- `void reset()` - Reset brain state
- `bool save(const std::string& filepath)` - Save brain state to file
- `bool load(const std::string& filepath)` - Load brain state from file

#### Config
**Location:** `src/core/Config/Config.hpp` (Phase 2)

**Description:** Configuration management system for NLM with support for multiple configuration sources and value validation.

**Key Features:**
- Support for multiple configuration sources (default, file, command line, runtime)
- Type-safe value storage (int, int64_t, double, bool, string, vectors)
- Configuration validation and error handling
- JSON/YAML file support (Phase 2)
- Command line argument parsing
- Save/load configuration to/from files

**Main Methods:**
- `Config()` - Default constructor
- `bool loadFromFile(const std::string& filepath)` - Load from file
- `bool loadFromArgs(int argc, char** argv)` - Load from command line arguments
- `bool saveToFile(const std::string& filepath)` - Save to file
- `void set(const std::string& key, const ConfigValue& value, ConfigSource source)` - Set configuration value
- `bool has(const std::string& key)` - Check if key exists
- `std::vector<std::string> getKeys()` - Get all configuration keys
- `void clear()` - Clear all configuration
- `std::string summary()` - Get configuration summary
- Template methods: `get<T>(key)` and `getOr<T>(key, defaultValue)`

#### Neuron
**Location:** `src/brain/Neuron.hpp` (Phase 2)

**Description:** Represents a single neuron implementing Leaky Integrate-and-Fire (LIF) dynamics.

**Key Features:**
- Real LIF neuron dynamics
- Membrane potential integration
- Spike detection and refractory period handling
- Synaptic input processing (excitatory, inhibitory, modulatory)
- Plasticity support (STDP, Hebbian, reward-modulated)
- Random parameter initialization

**Main Methods:**
- `Neuron(NeuronId id)` - Constructor with neuron ID
- `void step(Timestamp currentTime)` - Update neuron for simulation step
- `bool checkThreshold() const` - Check if neuron should fire
- `void injectCurrent(MembranePotential current)` - Inject external current
- `void receiveExcitatoryInput(MembranePotential amplitude)` - Receive excitatory input
- `void receiveInhibitoryInput(MembranePotential amplitude)` - Receive inhibitory input
- `void recordSpike(Timestamp timestamp)` - Record spike timestamp
- `void reset()` - Reset to initial state
- `void initializeRandom(RandomGenerator& rng)` - Initialize with random parameters

### Python Bindings (pynlm)

#### Overview
The Python bindings provide access to the NLM C++ neural simulation framework through an intuitive Python API. The bindings are organized into several modules:

#### Core Classes

**Config** - Configuration class
- `loadFromFile(filepath)` - Load configuration from file
- `loadFromArgs(args)` - Load from command line arguments (Python list)
- `saveToFile(filepath)` - Save configuration to file
- `has(key)` - Check if configuration key exists
- `getKeys()` - Get all configuration keys
- `set(key, value, source)` - Set configuration value

**SensoryInput** - Base class for sensory inputs
- `getType()` - Get input type
- `getData()` - Get raw data
- `getDimensions()` - Get dimensionality
- `getTimestamp()` - Get timestamp

**Vision** - Visual sensory input
- `Vision(width, height, channels)` - Constructor
- `setData(data)` - Set image data
- `getWidth()`, `getHeight()`, `getChannels()` - Dimension accessors

**Audio** - Audio sensory input
- `Audio(sampleRate, numSamples)` - Constructor
- `setData(data)` - Set audio data
- `setSampleRate(rate)` - Set sample rate

**Action** - Motor action output
- `Action(type)` - Constructor with action type
- `Action(type, parameters)` - Constructor with parameters
- `getType()`, `setType(type)` - Type accessors
- `getParameters()`, `setParameters(params)` - Parameter accessors
- `getName()` - Get action name
- `clone()` - Create a copy of the action

**WorldObject** - World object representation
- `WorldObject(x, y, type, value, radius)` - Constructor
- `x`, `y`, `radius`, `type`, `value`, `active` - Position and properties

#### Enhanced Brain Interface

**Brain** - Main neural simulation brain
- `Brain(config)` - Constructor with configuration
- `initialize()` - Initialize brain
- `step(currentStep)` - Perform simulation step
- `step(currentStep, currentTime)` - Perform simulation step with timestamp
- `receiveSensoryInput(input)` - Inject sensory input
- `injectCurrent(neuron, current)` - Inject current to specific neuron
- `injectCurrentToNeurons(type, current)` - Inject current to neurons of specific type
- `produceAction()` - Produce motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state

#### Agent Interface

**AgentBrain** - Interface connecting NLM brain to world
- `AgentBrain(brain)` - Constructor
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode brain activity to motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward-based neuromodulation
- `updateDevelopment(timestep)` - Update development system

#### SimpleWorld

**SimpleWorld** - Simple 2D world for NLM simulation
- `SimpleWorld()` - Constructor
- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `reset()` - Reset world
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world state
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body state
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove object at position

### Constants and Enumerations

**NeuronType**
- `Excitatory` - Excitatory neuron
- `Inhibitory` - Inhibitory neuron
- `Modulatory` - Modutive neuron
- `Sensory` - Sensory neuron
- `Motor` - Motor neuron
- `Internal` - Internal neuron

**SynapseType**
- `Excitatory` - Excitatory synapse
- `Inhibitory` - Inhibitory synapse
- `Modulatory` - Modulatory synapse
- `Electrical` - Electrical synapse
- `GapJunction` - Gap junction synapse

**DevelopmentalStage**
- `Initial` - Initial developmental stage
- `CriticalPeriod` - Critical period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

**ActionType**
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `MoveLeft` - Move left
- `MoveRight` - Move right
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Look` - Look
- `LookUp` - Look up
- `LookDown` - Look down
- `Interact` - Interact
- `Eat` - Eat
- `Drink` - Drink
- `Rest` - Rest
- `Wait` - Wait
- `Custom` - Custom action

**MotorCommand**
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `LookLeft` - Look left
- `LookRight` - Look right
- `Interact` - Interact
- `Wait` - Wait

**WorldObjectType**
- `Empty` - Empty space
- `Resource` - Resource
- `Hazard` - Hazard
- `Wall` - Wall
- `Marker` - Marker

**ActionResult**
- `reward` - Reward value
- `success` - Success status
- `message` - Result message

**FiringState**
- `Resting` - Resting state
- `Active` - Active state
- `Refractory` - Refractory state
- `Inhibited` - Inhibited state

**LogLevel**
- `Debug` - Debug level
- `Info` - Info level
- `Warning` - Warning level
- `Error` - Error level
- `Critical` - Critical level

### Factory Functions

**createDefaultConfig()** - Create a default configuration object
**createBrain(config)** - Create a new brain with configuration
**createSimpleWorld()** - Create a new simple world
**createAgentBrain(brain)** - Create a new agent brain interface

### Example Usage

```python
import pynlm

# Create configuration and brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize world
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, visionWidth=32, visionHeight=32)

# Run simulation
brain.initialize()

# Process sensory input
percept = world.getPercept()
brain.processSensoryInput(percept)

# Generate action
action = brain.produceAction()
world.applyCommand(action, 0.001)

# Update world
world.update(0.001)

# Get statistics
stats = brain.getStatistics()
print(f"Neurons: {stats['total_neurons']}, Spikes: {stats['total_spikes']}")
```

## Testing and Validation

### Unit Tests

The NLM framework includes comprehensive unit tests covering:
- Core functionality (Brain, Neuron, Config, Logger)
- Plasticity mechanisms (STDP, Hebbian)
- Memory systems
- Prediction and cognition
- Development and neuromodulation
- World and agent interfaces

### Integration Tests

Integration tests verify the interaction between different components:
- Brain-world interaction
- Agent-brain interface
- Sensory-motor loop
- Learning and plasticity
- Developmental processes

### Performance Tests

Performance tests validate the framework's computational efficiency:
- Simulation speed benchmarks
- Memory usage analysis
- Scalability testing
- Parallel processing validation

## Future Enhancements

### Python API Enhancements

1. **Context Managers**
   - Brain context manager for automatic cleanup
   - World context manager for simulation lifecycle

2. **Callback System**
   - Python callbacks for neuron events
   - Custom action selection callbacks
   - Reward and feedback callbacks

3. **NumPy Integration**
   - Enhanced vector and matrix operations
   - Optimized data transfer between Python and C++

4. **Advanced Configuration**
   - Schema-based configuration validation
   - Dynamic configuration reloading
   - Configuration inheritance and templating

5. **Machine Learning Integration**
   - TensorFlow/PyTorch compatibility
   - Custom training loops
   - Policy gradient integration

### C++ API Enhancements

1. **Advanced Configuration**
   - JSON/YAML support (Phase 2)
   - Configuration validation
   - Schema-based configuration

2. **Performance Optimizations**
   - SIMD vectorization
   - Parallel processing
   - Memory pooling

3. **Advanced Simulation Features**
   - Multi-scale simulation
   - Hierarchical brain models
   - Custom neuron models

## Building and Installation

### Prerequisites
- C++17 compatible compiler
- CMake 3.16 or higher
- Python 3.8 or higher
- pybind11
- scikit-build-core

### Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Installing Python Bindings

```bash
cd build
pip install .
```

### Running Tests

```bash
cd build
ctest --output-on-failure
```

## Troubleshooting

### Common Issues

**Python Import Error**
- Ensure pybind11 is properly installed
- Check that the Python module is built
- Verify Python version compatibility

**Configuration File Issues**
- Ensure file exists and is readable
- Check file format (key=value pairs)
- Verify file permissions

**Simulation Errors**
- Check configuration parameters
- Validate input ranges
- Ensure all required components are initialized

### Getting Help

- GitHub repository: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Issue tracker: https://github.com/nlm-project/nlm/issues
- Community forums: https://community.nlm.org

## License

MIT License - See LICENSE file for details.

## Copyright

© 2026 NLM Project Authors. All rights reserved.