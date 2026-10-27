# NLM Python Module Setup and Usage

This guide explains how to set up and use the NLM (Neural Learning Machine) Python bindings.

## Quick Start

### Prerequisites

1. **Clone the repository**
```bash
git clone <repo-url>
cd <repo-directory>
```

2. **Build the project**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

3. **Build Python bindings**
```bash
cd ..
pip install -e .
```

### Basic Usage

```python
from pynlm import createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain

# Create a brain
config = createDefaultConfig()
brain = createBrain(config)
brain.initialize()

# Create world and agent
world = createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## API Reference

### Core Classes

#### `createBrain(config)`
Creates a new neural brain with the given configuration.

**Parameters:**
- `config`: Configuration object for the brain

**Returns:**
- `Brain` object

#### `createDefaultConfig()`
Creates a default configuration for the brain.

**Returns:**
- `Config` object with default settings

#### `createSimpleWorld()`
Creates a simple 2D world for NLM simulation.

**Returns:**
- `SimpleWorld` object

#### `createAgentBrain(brain)`
Creates an agent brain interface that connects the neural brain to the world.

**Parameters:**
- `brain`: The neural brain to interface with

**Returns:**
- `AgentBrain` object

### Brain Class Methods

#### `initialize()`
Initializes the brain with the current configuration.

#### `step(step_number)`
Performs one simulation step.

**Parameters:**
- `step_number`: Current simulation step number

#### `getTotalNeuronCount()`
Gets the total number of neurons in the brain.

**Returns:**
- Number of neurons (int)

#### `getTotalSpikeCount()`
Gets the total number of spikes recorded.

**Returns:**
- Total spike count (int)

#### `getAverageFiringRate()`
Gets the average firing rate across all neurons.

**Returns:**
- Average firing rate (float)

#### `getDevelopmentalStage()`
Gets the current developmental stage of the brain.

**Returns:**
- Development stage (string)

### AgentBrain Class Methods

#### `initialize(world)`
Initializes the agent with a world.

**Parameters:**
- `world`: The world to initialize with

#### `processSensoryInput(percept)`
Processes sensory input from the world.

**Parameters:**
- `percept`: Sensory percept data

#### `decodeMotorCommand()`
Decodes neural activity into a motor command.

**Returns:**
- Motor command

#### `enableRewardModulation(enable)`
Enables or disables reward modulation.

**Parameters:**
- `enable`: Boolean indicating whether to enable reward modulation

#### `enableCuriosity(enable)`
Enables or disables curiosity-driven exploration.

**Parameters:**
- `enable`: Boolean indicating whether to enable curiosity

#### `enableDevelopment(enable)`
Enables or disables developmental processes.

**Parameters:**
- `enable`: Boolean indicating whether to enable development

### SimpleWorld Class Methods

#### `configure(width, height, visionWidth, visionHeight)`
Configures the world dimensions and vision.

**Parameters:**
- `width`: World width in cells
- `height`: World height in cells
- `visionWidth`: Vision width for agent
- `visionHeight`: Vision height for agent

#### `reset()`
Resets the world to its initial state.

#### `update(timestep)`
Updates the world by the given timestep.

**Parameters:**
- `timestep`: Time step duration

#### `applyMotorCommand(command, time)`
Applies a motor command to the world.

**Parameters:**
- `command`: Motor command to apply
- `time`: Current simulation time

### Config Class Methods

#### `loadFromFile(filepath)`
Loads configuration from a file.

**Parameters:**
- `filepath`: Path to configuration file

#### `saveToFile(filepath)`
Saves configuration to a file.

**Parameters:**
- `filepath`: Path to save configuration file

#### `has(key)`
Checks if a configuration key exists.

**Parameters:**
- `key`: Configuration key to check

## Configuration Options

### Default Configuration

The default configuration includes these parameters:

- `neuron_count`: Number of neurons (default: 500)
- `region_count`: Number of brain regions (default: 1)
- `connection_probability`: Probability of connections (default: 0.15)
- `random_seed`: Random seed for reproducibility (default: 42)
- `simulation_timestep`: Time step duration (default: 0.001)

### Custom Configuration

```python
from pynlm import createDefaultConfig

config = createDefaultConfig()
# Customize parameters
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.2)
config.set("random_seed", 123)
```

## Development Guide

### Building from Source

1. **Clone the repository**
```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

2. **Create build directory**
```bash
mkdir build
cd build
```

3. **Configure with CMake**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python3)
```

4. **Build**
```bash
make -j4
```

5. **Install Python bindings**
```bash
cd ..
pip install -e .
```

### Adding New Features

#### Adding Configuration Options

1. Modify `src/core/Config/Config.hpp` to add new configuration keys
2. Implement default values in `src/core/Config/Config.cpp`
3. Update Python bindings in `python/bindings.cpp` if needed

#### Adding New Brain Systems

1. Add new brain components to `Brain.hpp` and `Brain.cpp`
2. Update integration logic in the `step()` method
3. Ensure all systems are properly initialized in the constructor

### Testing

#### Running Examples

```bash
python3 examples.py
```

#### Running Tests

```bash
cd build
ctest
```

## Advanced Features

### 1. Multi-Agent Simulation

```python
from pynlm import createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain

# Create multiple brains
brains = [createBrain(createDefaultConfig()) for _ in range(3)]
for brain in brains:
    brain.initialize()

# Create shared world
world = createSimpleWorld()
world.configure(width=50, height=50)
world.reset()

# Create multiple agents
agents = [createAgentBrain(brain) for brain in brains]
for agent in agents:
    agent.initialize(world)

# Run multi-agent simulation
for step in range(100):
    world.update(0.1)
    
    # Each agent processes sensory input
    for i, agent in enumerate(agents):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
    
    # All brains think simultaneously
    for i, brain in enumerate(brains):
        brain.step(step)
    
    # Each agent selects actions
    for i, agent in enumerate(agents):
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

### 2. Checkpointing

```python
from pynlm import createBrain, createDefaultConfig

config = createDefaultConfig()
brain = createBrain(config)
brain.initialize()

# Save checkpoint
if brain.save("brain_checkpoint.bin"):
    print("Checkpoint saved successfully")
    
    # Load checkpoint
    new_brain = createBrain(config)
    new_brain.initialize()
    
    if new_brain.load("brain_checkpoint.bin"):
        print("Checkpoint loaded successfully")
```

### 3. Performance Monitoring

```python
from pynlm import createBrain, createDefaultConfig
import time

config = createDefaultConfig()
brain = createBrain(config)
brain.initialize()

start_time = time.time()
step_start = time.time()

for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        step_time = time.time() - step_start
        print(f"Step {step}: {step_time*1000:.2f}ms")
        step_start = time.time()
        
        # Print statistics
        print(f"  - Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  - Average firing rate: {brain.getAverageFiringRate():.3f}")

total_time = time.time() - start_time
print(f"\nTotal simulation time: {total_time:.2f}s")
print(f"Steps per second: {1000/total_time:.1f}")
```

## Troubleshooting

### Common Issues

#### "ModuleNotFoundError: No module named 'pynlm'"

**Solution:** Install the Python module
```bash
pip install -e .
```

#### "ImportError: libnlm_core.so not found"

**Solution:** Ensure the C++ library is built and in the library path
```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./build
```

#### "Brain initialization failed"

**Solution:** Check configuration parameters
```python
from pynlm import createBrain, createDefaultConfig

config = createDefaultConfig()
print(config.summary())  # Print current configuration
brain = createBrain(config)
if not brain.initialize():
    print("Brain initialization failed")
```

#### "Performance issues"

**Solution:** Optimize configuration
```python
config = createDefaultConfig()
config.set("neuron_count", 100)  # Smaller brain
config.set("region_count", 1)    # Fewer regions
config.set("connection_probability", 0.1)  # Fewer connections
```

### Getting Help

- **Documentation:** Check the `docs/` directory for detailed information
- **Examples:** Run `python3 examples.py` for working examples
- **Issues:** Report bugs at the project repository
- **Community:** Join the NLM discussion forum

## License

MIT License. See the LICENSE file for details.

## Version

Version: 0.1.0
Build date: $(date)
