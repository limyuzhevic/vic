# NLM (Neural Learning Machine) - Comprehensive Documentation

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is a computational brain project that aims to create a neural system beginning in a primitive developmental state and acquiring increasingly complex abilities through interaction with an environment.

### Key Philosophy
- **Not**: A transformer, LLM, chatbot, or pretrained AI system
- **Is**: A brain-inspired architecture with neurons and synapses that develops and adapts over time
- **Goal**: Cognition emerging from neural dynamics

## Installation and Build

### Quick Start (CMake)

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Package Installation

```bash
# Install from source
setuptools install .

# Or using pip (if available)
pip install -e .
```

### Build Options

```bash
# Configure with specific options
cmake \
  -DNLM_BUILD_PYTHON_BINDINGS=ON \
  -DNLM_BUILD_TESTS=ON \
  -DNLM_ENABLE_ASAN=ON \
  -DCMAKE_BUILD_TYPE=Debug \
  ..
```

## Python API Quick Start

### Basic Usage

```python
import nlm
import numpy as np

# Create default configuration
config = nlm.create_default_config()

# Create brain with configuration
brain = nlm.createBrain(config)
brain.initialize()

# Create simple world
world = nlm.createSimpleWorld()
world.configure(100, 100, 16, 16)
world.setAgentStart(50, 50)

# Create agent brain
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation loop for an episode
print("Starting simulation...")
for step in range(100):
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Get action from agent
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    world.applyMotorCommand(action, step * 0.1)
    
    # Get reward (simplified)
    reward = np.random.random()
    agent.applyRewardModulation(reward, agent.getPredictionError())
    
    # Update development
    agent.updateDevelopment(step * 0.1)

print("Simulation completed!")
```

### Advanced Usage

```python
import nlm
import pyarrow as pa

# Create a complete demo scenario
scenario = nlm.create_demo_scenario()
brain = scenario["brain"]
world = scenario["world"]
agent = scenario["agent"]

# Run multiple episodes
result = nlm.run_simulation_loop(agent, world, num_episodes=10, max_steps_per_episode=100)

print(f"Average reward per episode: {result['average_reward_per_episode']:.2f}")
print(f"Total episodes run: {result['total_episodes']}")
```

## Core Components

### 1. Brain (`nlm.Brain`)

The central neural simulation brain class implementing real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

#### Key Methods
- `initialize()`: Initialize brain with configuration
- `step(step, time=None)`: Perform simulation step
- `receiveSensoryInput(input)`: Inject sensory input into brain
- `produceAction()`: Produce motor action based on neural activity
- `save(filepath)`: Save brain state to file
- `load(filepath)`: Load brain state from file
- `get_stats()`: Get brain statistics as dictionary

#### Pythonic Features
- `brain.simulate_step(step)`: Convenience alias for stepping
- `brain.enable_all_systems()`: Enable all plasticity systems
- `brain.disable_all_systems()`: Disable all plasticity systems

### 2. AgentBrain (`nlm.AgentBrain`)

Connects NLM brain to the world, handling sensory transduction and motor decoding.

#### Key Methods
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory percept and inject into brain
- `decodeMotorCommand()`: Decode brain motor activity into motor command
- `applyRewardModulation(reward, predicted_reward)`: Apply reward-based neuromodulation
- `updateDevelopment(timestep)`: Update development system

#### Pythonic Features
- `agent.simulate_step(world, timestep)`: Simulate one step and return command
- `agent.process_observation(observation_dict)`: Process observation from Python dictionary
- `agent.take_action(world, timestep)`: Take action and return command
- `agent.get_agent_stats()`: Get agent statistics as dictionary

### 3. SimpleWorld (`nlm.SimpleWorld`)

Simple 2D world for NLM simulation with agent body and sensory perception.

#### Key Methods
- `configure(width, height, vision_width, vision_height)`: Configure world dimensions
- `reset()`: Reset world state
- `setAgentStart(x, y)`: Set agent starting position
- `update(timestep)`: Update world state
- `applyMotorCommand(cmd, currentTime)`: Apply motor command
- `getSensoryPercept()`: Get sensory percept
- `getAgentBody()`: Get agent body

#### Pythonic Features
- `world.add_resource(x, y, value)`: Add resource object
- `world.add_hazard(x, y, hazard_level)`: Add hazard object
- `world.simulate_step(timestep)`: Simulate one step and return percept
- `world.get_stats()`: Get world statistics as dictionary

### 4. Sensory Inputs

#### Vision (`nlm.Vision`)
- 2D array of visual input
- `setDataFromArray(array)`: Set from 2D Python array
- `getDataAsList()`: Get as Python list
- `normalize(min_val=0.0, max_val=1.0)`: Normalize to [0,1] range
- `enhance(factor=1.5)`: Enhance contrast

#### Audio (`nlm.Audio`)
- 1D array of audio samples
- `setDataFromArray(array)`: Set from 1D Python array
- `getDataAsList()`: Get as Python list
- `normalize(min_val=0.0, max_val=1.0)`: Normalize to [0,1] range
- `applyGain(gain)`: Apply gain
- `filterLowPass(cutoff, sampleRate)`: Apply low-pass filter

#### InternalSignals (`nlm.InternalSignals`)
- Internal neural signals
- `addSignal(value)`: Add signal value
- `clearSignals()`: Clear all signals

### 5. Actions (`nlm.Action`)

Motor action representation with parameters.

#### Key Methods
- `getType()`: Get action type
- `setType(type)`: Set action type
- `getParameters()`: Get parameters
- `setParameters(params)`: Set parameters
- `getName()`: Get action name
- `clone()`: Create copy

#### Pythonic Features
- `action.getParameter(index)`: Get parameter by index
- `action.setParameter(index, value)`: Set parameter by index
- `action.hasParameter(value)`: Check if action has specific parameter value

### 6. Configuration (`nlm.Config`)

Configuration system for NLM parameters.

#### Key Methods
- `loadFromFile(filepath)`: Load from JSON file
- `loadFromArgs(argc, argv)`: Load from command line arguments
- `saveToFile(filepath)`: Save to JSON file
- `has(key)`: Check if key exists
- `getKeys()`: Get all configuration keys
- `summary()`: Get summary string

## Advanced Features

### 1. Simulation Utilities

```python
# Validate observation data
observation = {"vision": list, "touch": list, ...}
validation = nlm.validate_observation(observation)
print(f"Valid: {validation['valid']}")
print(f"Errors: {validation['errors']}")

# Create action from dictionary
action_dict = {"type": "MoveForward", "parameters": [0.5, 0.3]}
action = nlm.create_action_from_dict(action_dict)

# Get available type names
action_types = nlm.get_action_type_names()
world_object_types = nlm.get_world_object_type_names()
```

### 2. Episode Management

```python
# Run single episode with detailed data
episode_result = nlm.simulate_episode(agent, world, max_steps=100, reward_threshold=5.0)
print(f"Total reward: {episode_result['total_reward']}")
print(f"Steps taken: {episode_result['steps_taken']}")

# Run multiple episodes with summary
all_episodes = nlm.run_simulation_loop(agent, world, num_episodes=5)
print(f"Average reward: {all_episodes['average_reward_per_episode']}")
```

### 3. Development and Learning

```python
# Enable/disable learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Get developmental information
agent.getDevelopmentalStage()
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
```

## API Reference

### Core Classes

#### nlm.Brain
- `__init__(config)`: Constructor
- All methods as described above

#### nlm.AgentBrain
- `__init__(brain)`: Constructor
- All methods as described above

#### nlm.SimpleWorld
- `__init__()`: Constructor
- All methods as described above

#### nlm.Vision
- `__init__(width, height, channels)`: Constructor
- All methods as described above

#### nlm.Audio
- `__init__(sampleRate, numSamples)`: Constructor
- All methods as described above

#### nlm.Action
- `__init__(type, parameters)`: Constructor
- All methods as described above

#### nlm.Config
- `__init__()`: Constructor
- All methods as described above

#### nlm.SensoryPercept
- `__init__()`: Constructor
- All methods as described above

#### nlm.AgentBody
- `__init__()`: Constructor
- All methods as described above

#### nlm.WorldObject
- `__init__(x, y, type, value=0.0, radius=0.5)`: Constructor
- All methods as described above

### Constants

```python
nlm.INVALID_NEURON_ID
nlm.INVALID_SYNAPSE_ID
nlm.INVALID_REGION_ID
nlm.INVALID_POPULATION_ID
nlm.DEFAULT_VISION_SIZE  # (16, 16, 3)
nlm.DEFAULT_WORLD_SIZE   # (100, 100)
nlm.MAX_EPISODE_STEPS
nlm.DEFAULT_REWARD_THRESHOLD
```

### Exception Types

```python
nlm.RuntimeError
nlm.ValueError
```

## Examples Directory

The `examples/` directory contains additional usage examples:

1. **basic_example.cpp**: Basic C++ usage
2. **simulation_example.cpp**: Complete simulation example
3. **benchmark_example.cpp**: Performance benchmarking

## Building Examples

```bash
# Build examples
mkdir build
cd build
cmake -DNLM_BUILD_EXAMPLES=ON ..
make nlm_example_basic
make nlm_example_simulation
```

## Testing

### Running Tests

```bash
# Build and run tests
mkdir build
cd build
cmake -DNLM_BUILD_TESTS=ON ..
make
ctest -V
```

### Python Tests

```bash
# Run Python tests
pytest tests/ -v
```

## Performance Considerations

### Memory Management
- Use smart pointers where possible
- Reuse objects when possible
- Monitor memory usage during long simulations

### Simulation Speed
- Use appropriate timestep duration
- Balance complexity with simulation speed
- Consider using vectorization for sensory processing

### Parallel Processing
- Enable OpenMP if available
- Use SIMD instructions for mathematical operations
- Consider multi-threading for complex simulations

## Advanced Python Integration

### Using NumPy
```python
import numpy as np

# Create vision data as numpy array
vision_array = np.random.rand(16, 16)
vision = nlm.Vision(16, 16, 3)
vision.setDataFromArray(vision_array)

# Normalize using numpy operations
vision_data = vision.getDataAsList()
normalized = (np.array(vision_data) - np.min(vision_data)) / (np.max(vision_data) - np.min(vision_data))
vision.setData(normalized.tolist())
```

### Using Pandas for Data Analysis
```python
import pandas as pd

# Collect simulation data into DataFrame
episode_data = episode_result['episode_data']
df = pd.DataFrame(episode_data)
df['timestamp'] = pd.to_datetime(df['step'], unit='s')

# Analyze learning progress
reward_trend = df.groupby('step')['reward'].mean()
learning_rate = reward_trend.corr(df['step'])
```

## Migration Guide

### From C++ to Python

If you're migrating from C++ to Python, consider:

1. **Memory Management**: Python handles garbage collection automatically
2. **Error Handling**: Use try-catch blocks for Python exceptions
3. **Type Conversion**: Be explicit about type conversions
4. **Performance**: Profile your code for Python-specific bottlenecks

### Upgrading from Version 0.0.1 to 0.1.0

- Python API improvements with more Pythonic features
- Enhanced documentation and examples
- Better error handling
- Improved simulation utilities

## Troubleshooting

### Common Issues

**1. Import Error**
```bash
ModuleNotFoundError: No module named 'nlm'
```
Solution: Install the package with `pip install -e .` or `python setup.py install`

**2. Build Errors**
```bash
cmake: Could NOT find Python3 (missing: Python3_INCLUDE_DIRS Python3_LIBRARY Python3_INTERPRETER)
```
Solution: Install Python development packages: `sudo apt install python3-dev`

**3. Runtime Errors**
```
RuntimeError: Brain not initialized
```
Solution: Call `brain.initialize()` before using the brain

### Getting Help

- **Documentation**: See `docs/` directory for detailed documentation
- **GitHub Issues**: Report bugs and request features
- **Community**: Join discussions for questions and help
- **Examples**: Examine `examples/` directory for usage patterns

## Future Directions

### Planned Features
1. **Enhanced World Models**: More complex environments
2. **Multi-Agent Systems**: Multiple agents interacting
3. **Advanced Sensory Processing**: More sophisticated perception models
4. **Hierarchical Learning**: Multi-level learning architectures
5. **Transfer Learning**: Knowledge transfer between tasks
6. **Meta-Learning**: Learning to learn

### Research Areas
1. **Neural Development**: Studying developmental processes
2. **Cognitive Architecture**: Understanding cognitive development
3. **Emergent Behaviors**: Studying complex behaviors emerging from simple rules
4. **Social Learning**: Learning from other agents
5. **Skill Acquisition**: How skills are acquired and refined

## License

MIT License. See `LICENSE` file for details.

## Authors

NLM Research Team. See `docs/` directory for scientific background.

## Acknowledgments

Thanks to all contributors and the open-source community for their support and contributions.

---

*This documentation was generated automatically from the NLM source code.*