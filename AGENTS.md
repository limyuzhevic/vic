# NLM (Neural Learning Machine) - Complete Documentation

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project designed to simulate brain-like systems with neurons and synapses. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

## Project Structure

```
NLM/
├── src/                    # C++ source code
│   ├── brain/              # Neural components
│   ├── core/               # Core utilities (Config, Types, etc.)
│   ├── agent/              # Agent interfaces
│   ├── world/              # World simulation
│   ├── sensory/            # Sensory processing
│   ├── motor/              # Motor system
│   └── ...                 # Other subsystems
├── python/                 # Python bindings
│   └── bindings.cpp       # Pybind11 bindings
├── docs/                   # Documentation
│   ├── ARCHITECTURE.md    # System architecture
│   ├── SCIENCE.md         # Scientific background
│   └── ...                # Other docs
├── README.md              # Project overview
├── HOW_TO_USE.md          # Usage guide
├── easy_usage.md          # Beginner guide
└── ...                    # Other docs
```

## Quick Start

### Installation

```bash
# Build from source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install .
```

### First Example

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

## API Reference

### Core Classes

#### Config

**Purpose**: Central configuration management for the NLM system

**Factory Methods**:
- `createDefault()` - Create default configuration
- `fromFile(filepath)` - Load from JSON file
- `fromArgs(argc, argv)` - Load from command line arguments

**Key Methods**:
- `set(key, value)` - Set configuration value
- `get(key)` - Get configuration value with type conversion
- `has(key)` - Check if key exists
- `loadFromFile(filepath)` - Load from JSON/YAML file
- `saveToFile(filepath)` - Save to JSON/YAML file
- `validate()` - Validate configuration against schema

**Pythonic Access**:
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
neuron_count = config.get("brain.neuron_count")  # Returns int
neuron_count = config.asInt("brain.neuron_count")  # Explicit int conversion
```

#### Brain

**Purpose**: Central neural simulation brain implementing spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

**Constructor**:
- `Brain(config)` - Create brain with configuration

**Key Methods**:
- `initialize()` - Initialize brain with configuration
- `step(currentStep)` - Main simulation step
- `step(currentStep, currentTime)` - Simulation step with timestamp
- `receiveSensoryInput(input)` - Inject sensory input
- `injectCurrent(neuron, current)` - Inject current to specific neuron
- `injectCurrentToNeurons(type, current)` - Inject current to neurons of specific type
- `produceAction()` - Produce motor action based on neural activity
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file

**Subsystems**:
- Working memory: `getWorkingMemory()`
- Episodic memory: `getEpisodicMemory()`
- Associative memory: `getAssociativeMemory()`
- Prediction: `getPredictionSystem()`
- Cognition: `getPlanner()`, `getConceptFormation()`, `getAttention()`
- Development: `getDevelopmentSystem()`
- Neuromodulation: `getDopamine()`, `getCuriosity()`, `getNovelty()`

#### AgentBrain

**Purpose**: Interface connecting NLM brain to the world, handling sensory transduction and motor decoding.

**Constructor**:
- `AgentBrain(brain)` - Create with reference to brain

**Key Methods**:
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode into motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward neuromodulation
- `updateDevelopment(timestep)` - Update development system

**Configuration Methods**:
- `enableRewardModulation(True/False)`
- `enableStructuralPlasticity(True/False)`
- `enableDevelopment(True/False)`
- `enableCuriosity(True/False)`

#### SimpleWorld

**Purpose**: Simple 2D world providing sensory experience without semantic labels.

**Constructor**:
- `SimpleWorld()` - Create empty world

**Key Methods**:
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset world to initial state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world state
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get current sensory percept
- `getAgentBody()` - Get agent body state

#### SensoryInput and Subclasses

**SensoryInput (Base Class)**:
- `Vision(width, height, channels)` - Visual input
- `Audio(sampleRate, numSamples)` - Audio input
- `InternalSignals()` - Internal brain state signals

**Key Methods**:
- `getData()` - Get raw data
- `getDimensions()` - Get dimensionality
- `setTimestamp(timestamp)` - Set timestamp
- `getTimestamp()` - Get timestamp

#### Action

**Purpose**: Action representation for motor output.

**Constructor**:
- `Action(type)` - Create with action type
- `Action(type, parameters)` - Create with parameters

**Key Methods**:
- `getType()` - Get action type
- `setType(type)` - Set action type
- `getParameters()` - Get action parameters
- `setParameters(params)` - Set action parameters
- `getName()` - Get human-readable name
- `clone()` - Create copy

#### WorldObject

**Purpose**: Object in the world with position and properties.

**Constructor**:
- `WorldObject(x, y, type, value, radius)` - Create with position and properties

**Properties**:
- `x, y` - Position
- `radius` - Radius
- `type` - Object type (Resource, Hazard, Wall, etc.)
- `value` - Reward value or damage
- `active` - Whether object is active

### Enumerations

#### NeuronType
- `Excitatory` - Neuron that excites other neurons
- `Inhibitory` - Neuron that inhibits other neurons
- `Modulatory` - Neuron that modulates neural activity
- `Sensory` - Neuron that processes sensory input
- `Motor` - Neuron that produces motor output
- `Internal` - Internal state neuron

#### SynapseType
- `Excitatory` - Excitatory synapse
- `Inhibitory` - Inhibitory synapse
- `Modulatory` - Modulatory synapse
- `Electrical` - Electrical synapse
- `GapJunction` - Gap junction synapse

#### DevelopmentalStage
- `Initial` - Initial developmental stage
- `CriticalPeriod` - Critical period for learning
- `Maturation` - Maturation phase
- `Adult` - Adult stage
- `Aging` - Aging stage

#### ActionType
- `MoveForward`, `MoveBackward` - Movement actions
- `TurnLeft`, `TurnRight` - Rotation actions
- `Look`, `LookUp`, `LookDown` - Looking actions
- `Interact` - Interaction action
- `Eat`, `Drink` - Consumption actions
- `Rest`, `Wait` - Inactivity actions
- `Custom` - Custom action

#### WorldObjectType
- `Empty` - Empty space
- `Resource` - Resource object
- `Hazard` - Hazardous object
- `Wall` - Wall object
- `Marker` - Marker object

### Configuration Parameters

#### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

#### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Python API Features

#### Type Conversion Utilities
```python
config = pynlm.createDefaultConfig()
int_val = config.asInt("key")          # Get as int
float_val = config.asDouble("key")     # Get as double
str_val = config.asString("key")       # Get as string
bool_val = config.asBool("key")        # Get as bool
vec_int = config.asIntVector("key")     # Get as int vector
vec_float = config.asDoubleVector("key")  # Get as float vector
vec_str = config.asStringVector("key")   # Get as string vector
```

#### Configuration Operations
```python
# Dictionary access
config["key"] = value
value = config["key"]
del config["key"]
"key" in config
len(config)

# Iteration
for key in config:
    print(f"{key}: {config[key]}")

for key, value in config.items():
    print(f"{key}: {value}")
```

#### Serialization
```python
# To Python dict
config_dict = config.toDict()

# From Python dict
config.fromDict(dict_obj)

# To JSON
json_str = config.toJson()

# From JSON (needs JSON parsing implementation)
# config.fromJson(json_str)
```

#### Configuration Validation
```python
# Validate entire configuration
is_valid = config.validate()

# Get validation errors
errors = config.getErrors()

# Add schema
config.addSchema(schema_dict)
```

#### Batch Operations
```python
# Batch set values
config.batchSet({"key1": value1, "key2": value2})

# Batch get values
values = config.batchGet(["key1", "key2"])
```

## User Guides

### Tutorial: Building a Simple Agent

This tutorial demonstrates how to create a simple agent that learns to navigate a world.

```python
import pynlm

def create_agent():
    """Create and initialize an agent."""
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
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    return brain, agent, world

def run_episode(agent, world, brain, num_steps=100):
    """Run a single episode."""
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input
        agent.processSensoryInput(percept)
        
        # 4. Run brain step
        brain.step(step)
        
        # 5. Get motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # 6. Apply motor command
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # 7. Apply reward modulation
        reward = 0.0  # In real implementation, get reward from world
        agent.applyRewardModulation(reward, 0.0)
        
        # 8. Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev stage: {brain.getDevelopmentalStage()}")

# Run the complete example
brain, agent, world = create_agent()
run_episode(agent, world, brain, 1000)
```

### Tutorial: Custom Configuration

This tutorial shows how to create custom configurations for specific tasks.

```python
import pynlm

# Create configuration with custom parameters
config = pynlm.createDefaultConfig()

# Set brain parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
config.set("brain.v_thresh", -45.0)
config.set("brain.v_reset", -60.0)

# Enable specific plasticity mechanisms
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("plasticity.hebbian.enable", True)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"STDP enabled: {brain.getSTDP() != None}")
```

### Tutorial: Sensory Processing

This tutorial demonstrates how to work with sensory inputs.

```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for several steps
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    
    # Check what sensory modalities are available
    vision = percept.getVision()
    if vision:
        print(f"Vision dimensions: {vision.getDimensions()}")
        print(f"Vision data: {vision.getData()[:10]}...")  # First 10 values
    
    # Process sensory input
    agent.processSensoryInput(percept)
    brain.step(step)
    
    if step % 20 == 0:
        print(f"Step {step} processed")
```

## Best Practices

### 1. Always Initialize Before Use
```python
brain.initialize()  # Never skip this!
for step in range(100):
    brain.step(step)
```

### 2. Reset Before Reuse
```python
brain.reset()  # Reset for new episode
for step in range(100):
    brain.step(step)
```

### 3. Use Configuration Wisely
```python
# Use defaults for simple cases
config = pynlm.createDefaultConfig()

# Customize only what's necessary
config.set("brain.neuron_count", 2000)  # Custom value
```

### 4. Enable Appropriate Learning Mechanisms
```python
# Enable based on your task
agent.enableRewardModulation(True)      # For reward-based learning
agent.enableCuriosity(True)             # For exploration
agent.enableStructuralPlasticity(True)  # For structural changes
agent.enableDevelopment(True)           # For developmental changes
```

### 5. Monitor Progress
```python
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Active neurons: {brain.getActiveNeuronCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
```

## Troubleshooting

### Common Issues and Solutions

#### 1. "Brain isn't doing anything"
**Symptoms**: No neurons firing, no learning
**Causes**:
- Forgot to call `initialize()`
- No sensory input connected
- Configuration parameters are too restrictive

**Solutions**:
```python
# Always initialize first
brain = pynlm.createBrain(config)
brain.initialize()  # Critical!

# Ensure sensory input
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### 2. Segmentation Faults
**Symptoms**: Program crashes unexpectedly
**Causes**:
- Calling `step()` before `initialize()`
- Invalid configuration values
- Memory corruption from invalid operations

**Solutions**:
```python
# Follow proper initialization sequence
brain.initialize()  # Always first
brain.step(0)        # Then step
```

#### 3. Import Error
**Error**: "No module named 'pynlm'"
**Solutions**:
```bash
# Reinstall the module
pip install --force-reinstall .

# Or install in development mode
pip install -e .
```

#### 4. "The agent isn't moving"
**Symptoms**: Agent stays in place
**Causes**:
- Motor command not applied
- `world.update()` not called
- No motor neurons active

**Solutions**:
```python
# Ensure world update is called
world.update(0.1)  # Each simulation step

# Apply motor command
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

#### 5. "Everything is 0"
**Symptoms**: No neural activity
**Causes**:
- Need time for neurons to "warm up"
- Insufficient input to neurons
- Configuration parameters too restrictive

**Solutions**:
```python
# Give neurons time to warm up
for step in range(100):
    brain.step(step)  # Let them settle

# Ensure sensory input is provided
agent.processSensoryInput(percept)
```

### Debugging Tips

#### 1. Check Brain State
```python
# Print brain status
brain.logStatus()

# Check key statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Firing: {brain.getFiringNeuronCount()}")
print(f"Active: {brain.getActiveNeuronCount()}")
```

#### 2. Monitor Development
```python
# Track developmental stage
for step in range(1000):
    brain.step(step)
    
    if brain.getDevelopmentalStage() != prev_stage:
        print(f"Development stage changed: {prev_stage} -> {brain.getDevelopmentalStage()}")
    
    prev_stage = brain.getDevelopmentalStage()
```

#### 3. Check Memory Systems
```python
# Monitor memory systems
working_mem = brain.getWorkingMemory()
episodic_mem = brain.getEpisodicMemory()

if working_mem:
    print(f"Working memory entries: {working_mem.getSize()}")
    
if episodic_mem:
    print(f"Episodic memory episodes: {episodic_mem.getEpisodeCount()}")
```

## Migration Guides

### From v0.0.x to v0.1.0

#### API Changes

1. **Configuration Structure**: Major changes to configuration parameter naming
2. **Brain Initialization**: Added optional parameter validation
3. **Agent Interface**: Enhanced with new neuromodulation methods

#### Code Examples

**Before**:
```python
# Old API
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
brain = pynlm.createBrain(config)
```

**After**:
```python
# New API
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
```

#### Migration Steps

1. Update configuration parameter names
2. Add `initialize()` calls where missing
3. Update agent initialization
4. Test thoroughly

### Customizing Configuration

#### Creating Custom Configuration Classes

```python
import pynlm

class CustomConfig:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.customize()
    
    def customize(self):
        """Customize configuration for specific task."""
        self.config.set("brain.neuron_count", 5000)
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("neuromod.curiosity.enable", True)
    
    def getBrain(self):
        """Create brain with this configuration."""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        return brain

# Use custom configuration
custom_config = CustomConfig()
brain = custom_config.getBrain()
```

## Installation and Setup

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release build (recommended)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

### Installing Python Bindings

#### From Source

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Install in development mode
pip install -e .

# Or install normally
pip install .
```

#### Using Pre-built Wheels

```bash
# Install from PyPI (if available)
pip install pynlm
```

### Development Setup

#### Environment Setup

```bash
# Create virtual environment
python -m venv nlm_env
source nlm_env/bin/activate  # On Windows: nlm_env\Scripts\activate

# Install dependencies
pip install -r requirements.txt
pip install pytest numpy
```

#### Building Documentation

```bash
# Install documentation dependencies
pip install sphinx sphinx-rtd-theme

# Build documentation
make docs  # If Makefile exists
```

### Running Demos

#### Phase 6 Demo

```bash
# Run integration test
./nlm_phase6_demo
```

#### Python Demos

```python
import pynlm

# Run simple demo
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)

print("Phase 6 demo completed successfully!")
```

### Performance Optimization

#### Recommended Settings for Production

```python
# Use optimized configuration for production
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

# Consider these for high-performance scenarios
# config.set("brain.tau_mem", 10.0)  # Faster dynamics
# config.set("brain.tau_ref", 1.0)   # Higher firing rates
```

#### Profiling and Benchmarking

```python
import time
import pynlm

# Profile brain performance
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

print(f"Processed {1000} steps in {end_time - start_time:.2f} seconds")
print(f"Performance: {(end_time - start_time) / 1000 * 1000:.2f} ms/step")
```

## References

- [NLM Project Website](https://github.com/nlm-project/nlm)
- [Python API Documentation](https://nlm.readthedocs.io)
- [Scientific Papers](docs/SCIENCE.md)
- [Architecture Documentation](docs/ARCHITECTURE.md)

## Contributing

### Code Contributions

1. Fork the repository
2. Create feature branch
3. Implement changes
4. Add tests
5. Submit pull request

### Documentation Contributions

1. Fork the repository
2. Update documentation files
3. Add examples and tutorials
4. Submit pull request

### Testing

Run the test suite:

```bash
# Run unit tests
cd tests
pytest

# Or run all tests
make test
```

## License

MIT License

---

*Documentation generated automatically from source code docstrings and comments.*
*Last updated: $(date)*
