# pynlm - Python Bindings for NLM

Python bindings for the NLM (Neural Learning Machine) framework, an experimental artificial developmental brain project.

## Overview

This package provides Python bindings for the NLM C++ framework, enabling Python users to:

- Create and control neural simulation brains
- Interact with simulated environments
- Configure neural parameters
- Access memory and cognitive systems
- Visualize and monitor neural activity

## Installation

The pynlm Python bindings are automatically built as part of the NLM C++ framework. After building the project:

### Using pip (if available)
```bash
pip install pynlm
```

### From source
```bash
# Build the project
cd /path/to/nlm-project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install -e .
```

## Basic Usage

```python
import pynlm as nlm

# Create a default configuration
config = nlm.Config()

# Create a brain with the configuration
brain = nlm.Brain(config)

# Initialize the brain
brain.initialize()

# Run a few simulation steps
for step in range(10):
    brain.step(step)

# Get current status
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
```

## Enhanced API

For more advanced usage, the `nlm_python_api` module provides enhanced APIs:

```python
from nlm_python_api import HighLevelNLM, NeuralSimulation, ActionPlanner

# Create a high-level interface
nlm = HighLevelNLM()
nlm.initialize()

# Run simulation
for step in range(100):
    nlm.step()
    if step % 10 == 0:
        print(f"Step {step}")
```

## Configuration

### Configuration Keys

- `random_seed`: Random seed for reproducible experiments
- `simulation_timestep`: Simulation timestep in seconds
- `neuron_count`: Number of neurons in the brain
- `region_count`: Number of brain regions
- `connection_probability`: Connection probability for random connectivity
- `plasticity_learning_rate`: Plasticity learning rate
- `stdp_ltp_weight`: STDP LTP weight
- `stdp_ltd_weight`: STDP LTD weight
- `stdp_tau`: STDP time constant
- `environment_name`: Environment name
- `environment_width`: Environment width
- `environment_height`: Environment height
- `log_level`: Logging level

### Example Configuration

```python
import pynlm as nlm

# Create config from file
config = nlm.Config()
config.loadFromFile("config.json")

# Create config from defaults
config = nlm.Config()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.saveToFile("my_config.json")
```

## Core Classes

### Config
Configuration management for NLM brain parameters.

### Brain
The central neural simulation brain class with integrated memory, prediction, cognition, and neuromodulation systems.

### AgentBrain
Interface connecting NLM brain to world environment.

### SimpleWorld
Simple 2D world for NLM simulation.

### Sensory Input Classes
Vision, Audio, and InternalSignals for sensory input.

### Action
Motor action representation for output control.

### WorldObject
Representation of objects in the simulation world.

## Advanced Usage

### Environment Interaction

```python
import pynlm as nlm

# Create world
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, vision_width=10, vision_height=10)

# Create brain and agent
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
agent_brain = nlm.createAgentBrain(brain)

# Run episode
for step in range(100):
    # Get perception
    percept = world.getSensoryPercept()
    
    # Process with agent brain
    agent_brain.processSensoryInput(percept)
    
    # Get action
    action = agent_brain.decodeMotorCommand()
    
    # Apply to world
    world.applyMotorCommand(action, step)
    
    # Update world
    world.update(step)
```

### Action Planning with Neuromodulation

```python
import pynlm as nlm

# Create systems
config = nlm.Config()
brain = nlm.Brain(config)
agent_brain = nlm.AgentBrain(brain)

# Enable neuromodulation
agent_brain.enableRewardModulation(True)
agent_brain.enableCuriosity(True)

# Run learning episode
for episode in range(50):
    # Reset for new episode
    agent_brain.reset()
    
    # Episode loop
    for step in range(100):
        # Get perception
        percept = world.getSensoryPercept()
        
        # Process and get action
        agent_brain.processSensoryInput(percept)
        action = agent_brain.decodeMotorCommand()
        
        # Apply with reward
        result = world.applyMotorCommand(action, step)
        agent_brain.applyRewardModulation(result.reward)
        
        # Update development
        agent_brain.updateDevelopment(step)
```

### Memory Integration

```python
import pynlm as nlm

# Create brain with memory
config = nlm.Config()
brain = nlm.Brain(config)

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Store experience
working_memory.store(state_vector, action_vector, reward)
episodic_memory.store_episode(state, action, reward, next_state)
associative_memory.store_association(state, action)

# Retrieve experience
retrieved_state = working_memory.retrieve(similarity_threshold)
```

## Enhanced API Examples

### High-Level Neural Simulation

```python
from nlm_python_api import NeuralSimulation

# Create simulation
sim = NeuralSimulation()

# Run simulation
for step in range(100):
    sim.step()
    
    # Get state
    state = sim.get_state()
    neuromod_levels = sim.get_brain().getNeuromodulationLevels()
    
    print(f"Step {step}: State {state}, Neuromod {neuromod_levels}")
```

### Action Planning

```python
from nlm_python_api import ActionPlanner

# Create planner
planner = ActionPlanner(exploration_rate=0.1, learning_rate=0.01)

# Learning loop
for episode in range(100):
    state = sim.reset()
    done = False
    
    while not done:
        action = planner.act(state)
        next_state, reward, done = sim.step(action)
        planner.learn(state, action, reward, next_state, done)
        state = next_state
```

### Curiosity-Driven Exploration

```python
from nlm_python_api import CuriosityDrivenExploration

# Create curiosity system
curiosity = CuriosityDrivenExploration(learning_rate=0.01)

# Exploration loop
for step in range(100):
    state = nlm.get_perception()
    action = curiosity.select_action(state)
    result = nlm.execute_action(action)
```

## Visualization and Monitoring

### Basic Monitoring

```python
import pynlm as nlm

# Create brain
config = nlm.Config()
brain = nlm.Brain(config)
brain.initialize()

# Monitor progress
for step in range(100):
    brain.step(step)
    
    if step % 10 == 0:
        # Get statistics
        stats = {
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount(),
            'active': brain.getActiveNeuronCount(),
            'firing': brain.getFiringNeuronCount(),
            'avg_rate': brain.getAverageFiringRate(),
            'neuromod': brain.getNeuromodulationLevels()
        }
        
        print(f"Step {step}: {stats}")
```

### Visualization

```python
import matplotlib.pyplot as plt
import pynlm as nlm

# Create and run simulation
nlm = HighLevelNLM()
simulation_data = nlm.run_simulation(max_steps=1000, visualize=True)

# Plot results
plt.figure(figsize=(15, 10))

plt.subplot(2, 3, 1)
plt.plot(simulation_data['rewards'])
plt.title('Reward Over Time')
plt.xlabel('Step')
plt.ylabel('Reward')

plt.subplot(2, 3, 2)
plt.hist(simulation_data['action_distribution'], bins=range(len(nlm.ActionType)))
plt.title('Action Distribution')
plt.xlabel('Action Type')
plt.ylabel('Frequency')

plt.subplot(2, 3, 3)
for neuromod_name, levels in simulation_data['neuromod_levels']:
    plt.plot(levels, label=neurom_name)
plt.title('Neuromodulation Levels')
plt.legend()

plt.tight_layout()
plt.show()
```

## Error Handling

```python
import pynlm as nlm

try:
    # Create configuration
    config = nlm.Config()
    config.loadFromFile("config.json")
    
    # Create brain
    brain = nlm.Brain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
except nlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except FileNotFoundError as e:
    print(f"File not found: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Considerations

### Memory Management
```python
import pynlm as nlm
import gc

# Create brain with configuration
config = nlm.Config()
brain = nlm.Brain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Periodically clean up
    if step % 100 == 0:
        gc.collect()
```

### Batch Processing
```python
def batch_process(states, actions, config):
    """Process multiple states and actions efficiently."""
    results = []
    
    for state, action in zip(states, actions):
        brain = nlm.Brain(config)
        brain.initialize()
        brain.receiveSensoryInput(state)
        next_action = brain.produceAction()
        results.append(next_action)
    
    return results
```

## Serialization and Persistence

```python
import pynlm as nlm

# Save brain state
brain = nlm.Brain(config)
brain.initialize()
for step in range(100):
    brain.step(step)

brain.save("brain_checkpoint.bin")

# Load brain state
brain.load("brain_checkpoint.bin")
```

## Testing

### Basic Test

```python
import pynlm as nlm

def test_basic_functionality():
    # Create configuration
    config = nlm.Config()
    config.set("neuron_count", 100)
    config.set("region_count", 2)
    
    # Create and initialize brain
    brain = nlm.Brain(config)
    assert brain.initialize()
    
    # Run simulation steps
    for step in range(10):
        brain.step(step)
    
    # Verify brain is still functional
    assert brain.getTotalNeuronCount() == 100
    assert brain.getTotalSynapseCount() > 0
    
    print("All tests passed!")

if __name__ == "__main__":
    test_basic_functionality()
```

## Development

### Building from Source

1. Clone the repository
2. Create build directory: `mkdir build && cd build`
3. Run CMake: `cmake .. -DCMAKE_BUILD_TYPE=Release`
4. Build: `make -j4`
5. Install Python bindings: `pip install -e .`

### Running Tests

```bash
# Run C++ tests
./nlm_test

# Run Python tests
python -m pytest tests/ -v
```

### Adding New Features

1. Add new C++ classes to `src/`
2. Update pybind11 bindings in `python/bindings.cpp`
3. Update Python API documentation in `python/API.md`
4. Add Python convenience classes to `python/nlm_python_api.py`
5. Add tests in `tests/`

## Documentation

### Generated Documentation

- **API Reference**: `python/API.md`
- **High-Level API**: `python/nlm_python_api.md`
- **Usage Examples**: This README file

### Documentation Sources

- C++ header files: `src/**/*.hpp`
- Configuration documentation: `configs/default.cfg`
- Project documentation: `docs/`
- API documentation: Generated from docstrings

## Contributing

### Guidelines

1. Follow the existing code style
2. Add documentation for new features
3. Write tests for new functionality
4. Update API documentation
5. Use descriptive variable and function names

### Pull Request Process

1. Fork the repository
2. Create feature branch
3. Implement changes
4. Write tests
5. Update documentation
6. Submit pull request

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgments

This project builds upon the work of the NLM research team and the pybind11 library.

## Support

For support with the NLM Python bindings, please:

1. Check the documentation
2. Review examples
3. Check the issue tracker
4. Submit bug reports with detailed information

---

**Note**: The NLM framework is experimental and under active development. APIs may change between versions.
