# NLM (Neural Learning Machine) - Python Bindings Documentation

This repository provides the NLM framework with the following Python bindings:

## Quick Start

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install the package
pip install .

# Or install in development mode
pip install -e .
```

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete with {brain.getTotalSpikeCount()} spikes")
```

## Installation from Source

The NLM Python bindings are built using `pybind11` and `scikit-build-core`. To install from source:

```bash
# Install system dependencies (if needed)
sudo apt-get install cmake build-essential python3-dev python3-pip

# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install the package
pip install .

# Or install in development mode (for editing)
pip install -e .
```

The build process will automatically compile the C++ core and create the `pynlm` Python module.

## Python Package Information

### Available Classes

**Brain** - Central neural simulation brain class
- `createBrain(config)` - Create a new brain with configuration
- `initialize()` - Initialize the brain with configuration
- `step(step)` - Perform a simulation step
- `receiveSensoryInput(input)` - Inject sensory input into the brain
- And many more methods for neural control and statistics

**Config** - Configuration management for NLM system
- `createDefaultConfig()` - Create a default configuration
- `loadFromFile(filepath)` - Load configuration from a JSON file
- `saveToFile(filepath)` - Save configuration to a JSON file
- And many more configuration management methods

**SimpleWorld** - Simple 2D world for NLM simulation
- `createSimpleWorld()` - Create a new simple world
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset world to initial state
- `update(timestep)` - Update world state
- And more world management methods

**AgentBrain** - Agent brain interface connecting NLM brain to world
- `createAgentBrain(brain)` - Create a new agent brain interface
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory percept and inject into brain
- `decodeMotorCommand()` - Decode brain motor activity into motor command
- And more agent-brain interface methods

**SensoryPercept** - Sensory data packet
- Contains vision, touch, internal signals, proprioception, and audio data
- `getAllSignals()` - Get all signals concatenated for brain input

**WorldObject** - World object representation
- Position, type, value, and active status for objects in the world

**AgentBody** - Agent body state
- Physical properties and internal state including position, orientation, velocity, and energy

### Working with Agents

For complete agent-based simulation examples, see the documentation in `easy_usage.md` and `HOW_TO_USE.md`.

### Example: Complete Agent Simulation

```python
import pynlm

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

# 5. Initialize agent with world
agent.initialize(world)

# 6. Enable subsystems for learning
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# 7. Run simulation loop
for step in range(1000):
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
    reward = world.getAgentBody().energy / world.getMaxEnergy()
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)

# 8. Print final statistics
print(f"Simulation complete!")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## File I/O Operations

The brain can be saved and loaded for persistence:

```python
import pynlm
import os

# Create and develop a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some steps to develop the brain
for step in range(100):
    brain.step(step)

print("Saving brain state...")
brain.save("my_brain_state.bin")

# Load the saved state into a new brain
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
new_brain.initialize()

if new_brain.load("my_brain_state.bin"):
    print("Successfully loaded brain state!")
else:
    print("Failed to load brain state")
```

### Configuration Management

Configurations can be loaded from and saved to JSON files:

```python
import pynlm
import json

# Create a configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)

# Save to file
config.saveToFile("my_config.json")

# Load from file
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")

# View configuration summary
print(config.summary())
```

## Error Handling

The bindings include proper error handling with Python exceptions:

```python
import pynlm

# Create objects that might be invalid
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Operations that might throw exceptions
try:
    brain.save("invalid/path.bin")
except Exception as e:
    print(f"Error saving brain: {e}")

# Check for valid operations
if config.has("brain.neuron_count"):
    neuron_count = config.get("brain.neuron_count")
    print(f"Brain has {neuron_count} neurons")
```

## Performance Tips

- For batch operations, create fewer brain objects and reuse them
- Configure neuron count appropriately for your use case (1000-5000 typical)
- Enable development and reward modulation for learning simulations
- Use `agent.enableCuriosity(True)` for exploration behavior
- Save brain states periodically during long simulations

## Integration with Other Libraries

The pynlm module can be integrated with:
- NumPy for numerical computations
- Pandas for data analysis
- Matplotlib for visualization
- Custom simulation environments

```python
import pynlm
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# Create brain with numpy arrays for configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Use numpy for data collection
simulation_data = []
for step in range(100):
    brain.step(step)
    simulation_data.append({
        'step': step,
        'spikes': brain.getTotalSpikeCount(),
        'firing': brain.getFiringNeuronCount(),
        'rate': brain.getAverageFiringRate()
    })

# Convert to numpy array for analysis
data_array = np.array(simulation_data)
print(f"Average firing rate: {np.mean(data_array[:, 3])}")
```

## Testing

The Python bindings include comprehensive integration tests:

```bash
# Run Python tests
python3 tests/test_pynlm.py

# Or run with pytest
pytest tests/test_pynlm.py -v
```

The tests cover:
- Basic brain creation and initialization
- Configuration operations
- World simulation
- Agent brain interface
- Save/load functionality
- Python convenience functions
- Comprehensive simulations
- Benchmarking

## Support and Troubleshooting

For issues with the Python bindings:
1. Check that `pybind11` and `scikit-build-core` are installed
2. Verify the package was built correctly: `pip list | grep pynlm`
3. Check logs for compilation errors during installation
4. Refer to `easy_usage.md` for beginner-friendly examples
5. Consult `HOW_TO_USE.md` for detailed usage information

For issues with the C++ core:
1. Check the NLM documentation in the `docs/` directory
2. Review the C++ examples in `src/main.cpp`
3. Check the build logs if compiling from source
4. Visit the NLM project website for additional resources

## Additional Python Features

The enhanced Python bindings include these additional features:

### Convenience Functions

```python
# Create custom world
world = pynlm.createCustomWorld(30, 30, 10, 10)

# Run quick brain test
spike_count = pynlm.runQuickBrainTest(brain, 50)

# Get brain statistics
stats = pynlm.get_brain_statistics(brain)

# Export/import world state
state = pynlm.export_world_state(world)
pynlm.import_world_state(world, state)
```

### Benchmarking

```python
# Benchmark simulation performance
results = pynlm.benchmark_simulation(config, 1000)
print(f"Performance: {results['steps_per_second']:.1f} steps/sec")
```

### Configuration Comparison

```python
# Compare different configurations
configs = [config1, config2, config3]
comparison = pynlm.compare_brain_configs(configs, 500)
```

## Module Exports

The `pynlm` module exports these key components:

```python
# For use in: import pynlm
pynlm.__all__  # List of exported names
```

Key exports include:
- `Brain`, `Config`, `SimpleWorld`, `AgentBrain`
- `SensoryPercept`, `WorldObject`, `AgentBody`
- `ActionResult`, `MotorCommand`, `ActionType`
- `NeuronType`, `SynapseType`, `DevelopmentalStage`
- `NeuronId`, `SynapseId`, `RegionId`, `PopulationId`
- `Action`, `Vision`, `Audio`, `InternalSignals`
- `createDefaultConfig`, `createBrain`, `createSimpleWorld`, `createAgentBrain`

## Version Information

- Python bindings version: 0.1.0
- C++ core: NLM Phase 6 (Final Integration)
- Python compatibility: >= 3.8
- Build dependencies: pybind11 >= 2.11.0, scikit-build-core >= 0.5.0

## License

MIT License

## Authors

Research project — See docs for scientific background.
