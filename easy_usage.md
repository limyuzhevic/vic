# NLM - Simple Usage Guide

This document provides a quick start guide for using the NLM (Neural Learning Machine) Python bindings.

## Quick Start

### Installation

1. **Build the project**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Install Python bindings**
   ```bash
   # If installed system-wide
   pip install pynlm
   
   # Or if building from source
   pip install python/pynlm*
   ```

### Basic Usage

```python
import pynlm as nlm

# Create default configuration and brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")

# Run a simple simulation
for step in range(10):
    brain.step(step, step * 0.001)
    print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")

print("Simulation completed!")
```

## Getting Started Examples

### 1. Basic Neural Network
```python
import pynlm as nlm

# Create and initialize brain
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step, step * 0.001)

print(f"Final spike count: {brain.getTotalSpikeCount()}")
```

### 2. With Sensory Input
```python
import pynlm as nlm

# Create world and brain
world = nlm.createSimpleWorld()
world.configure(64, 64, 16, 16)

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize(world)

# Create and process sensory input
percept = nlm.SensoryPercept()
vision = nlm.Vision(16, 16, 1)
vision.setData([0.5] * 256)  # Simple gray background
percept.setVision(vision)

brain.processSensoryInput(percept)
print("Sensory input processed!")
```

### 3. Motor Control
```python
import pynlm as nlm

# Create agent brain
brain = nlm.createBrain(nlm.createDefaultConfig())
world = nlm.createSimpleWorld()
agent_brain = nlm.AgentBrain(brain)

# Initialize
world.configure(100, 100, 32, 32)
brain.initialize(world)
agent_brain.initialize(world)

# Run control loop
for step in range(10):
    world.update(step * 0.01)
    command = agent_brain.decodeMotorCommand()
    
    if command != nlm.MotorCommand.Wait:
        world.applyMotorCommand(command, world.getSimulationTime())
    
    print(f"Step {step}: Action = {command}")
```

## Advanced Features

### Configuration
```python
import pynlm as nlm

config = nlm.createDefaultConfig()

# Set custom parameters
config.set("random_seed", 123)
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)

brain = nlm.createBrain(config)
brain.initialize()
```

### Error Handling
```python
import pynlm as nlm

try:
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
except nlm.InvalidArgumentError as e:
    print(f"Configuration error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.NLMError as e:
    print(f"General NLM error: {e}")
```

### Type Conversions
```python
import pynlm as nlm

# Convert between Python and NLM types
float_list = [0.1, 0.5, 0.9]
double_list = nlm.to_double_vector(float_list)
float_list2 = nlm.to_float_vector(double_list)

print(f"Conversion: {float_list} -> {double_list} -> {float_list2}")
```

## API Reference

### Core Classes

| Class | Description |
|-------|-------------|
| `createDefaultConfig()` | Factory function for default configuration |
| `createBrain(config)` | Factory function for brain with error checking |
| `createSimpleWorld()` | Factory function for world |
| `AgentBrain(brain)` | Agent interface for brain-to-world control |

### Key Methods

**Brain Class:**
- `initialize()` - Initialize the brain
- `step(step, time)` - Run a simulation step
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSpikeCount()` - Get total spike count

**AgentBrain Class:**
- `decodeMotorCommand()` - Select action based on neural activity
- `processSensoryInput(percept)` - Process sensory input
- `applyRewardModulation(reward, predicted)` - Learning with rewards

**Configuration Class:**
- `set(key, value, source)` - Set configuration parameter
- `getOr(key, default)` - Get value with default
- `has(key)` - Check if parameter exists

### Enum Types

All enum types are available as Python enums:
- `nlm.NeuronType.Excitatory`
- `nlm.ActionType.MoveForward`
- `nlm.MotorCommand.TurnLeft`
- `nlm.DevelopmentalStage.Maturation`

### Constants

| Constant | Description |
|----------|-------------|
| `nlm.INVALID_NEURON_ID` | Invalid neuron identifier |
| `nlm.INVALID_SYNAPSE_ID` | Invalid synapse identifier |
| `nlm.PYNLM_VERSION` | Python bindings version |

## Examples Directory

The `examples/` directory contains additional advanced usage examples:

- `advanced_usage.py` - Comprehensive examples
- `basic_simulation.py` - Basic neural simulation
- `sensory_processing.py` - Sensory input processing
- `motor_control.py` - Action selection and control

## Troubleshooting

### Common Issues

**Issue**: Python bindings not found
**Solution**: Ensure the module is installed correctly:
```bash
pip install pynlm
# or for development:
pip install -e .
```

**Issue**: Building fails
**Solution**: Ensure dependencies are installed:
```bash
# Ubuntu/Debian
 sudo apt-get install cmake g++ python3-dev

# macOS
brew install cmake

# Then rebuild:
mkdir build && cd build && cmake .. && make -j4
```

**Issue**: Memory errors
**Solution**: Reduce brain size for testing:
```python
config.set("neuron_count", 100)  # Smaller for faster testing
```

## Further Reading

- [NLM Documentation](https://nlm.readthedocs.io)
- [Source Code Repository](https://github.com/nlm-project/nlm)
- [Phase 6 Integration Guide](docs/PHASE6_FINAL_AUDIT.md)

## License

MIT License - See LICENSE file for details.
