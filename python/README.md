# NLM Python Bindings

## Overview

This module provides Python bindings for NLM (Neural Learning Machine) - a brain-inspired spiking neural network simulator. The Python bindings allow users to create and interact with virtual brains using Python, providing a high-level interface to the underlying C++ neural simulation framework.

## Key Features

### Core Classes

- **Config**: Configuration management system for brain parameters
- **Brain**: Main brain simulation class with neurons, synapses, and plasticity
- **AgentBrain**: Interface between brain and world simulation
- **SimpleWorld**: Environment simulation for agent testing

### Enumerations

- **NeuronType**: Types of neurons (Excitatory, Inhibitory, Sensory, Motor, etc.)
- **DevelopmentalStage**: Brain development stages
- **ActionType**: Possible actions an agent can take
- **WorldObjectType**: Types of objects in the world
- **MotorCommand**: Motor commands for agent movement

### Key Methods

#### Config
- `createDefaultConfig()`: Create default configuration
- `set(key, value, source)`: Set configuration parameter
- `get(key)`: Get configuration parameter
- `loadFromFile(filepath)`: Load configuration from file
- `saveToFile(filepath)`: Save configuration to file

#### Brain
- `createBrain(config)`: Create brain with configuration
- `initialize()`: Initialize brain
- `step(step_number)`: Run simulation step
- `step(step_number, timestamp)`: Run simulation step with timestamp
- `reset()`: Reset brain state
- `receiveSensoryInput(input)`: Inject sensory input
- `produceAction()`: Get motor action
- `getTotalNeuronCount()`: Get total neurons
- `getTotalSynapseCount()`: Get total synapses
- `getFiringNeuronCount()`: Get firing neurons
- `getAverageFiringRate()`: Get average firing rate
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state

#### AgentBrain
- `createAgentBrain(brain)`: Create agent interface
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command
- `applyRewardModulation(reward, predicted_reward)`: Apply reward
- `updateDevelopment(timestep)`: Update development
- `enableRewardModulation(enabled)`: Enable/disable reward modulation
- `enableStructuralPlasticity(enabled)`: Enable/disable structural plasticity
- `enableDevelopment(enabled)`: Enable/disable development
- `enableCuriosity(enabled)`: Enable/disable curiosity
- `getCuriosityLevel()`: Get curiosity level
- `getNoveltyLevel()`: Get novelty level
- `getPredictionError()`: Get prediction error
- `getNeuromodulationLevel()`: Get neuromodulation level
- `getDevelopmentalStage()`: Get developmental stage
- `getThoughtTrace()`: Get thought trace
- `getLearnedPreference()`: Get learned preference
- `getConceptCount()`: Get concept count
- `getSelfModelConfidence()`: Get self model confidence
- `getSocialObservationCount()`: Get social observation count
- `encodeText(text)`: Encode text into pattern
- `getInternalFiringRates()`: Get internal firing rates
- `getMotorGroupActivities()`: Get motor group activities
- `observeOtherAgent(observed, beforeVision, afterVision)`: Observe other agent
- `reset()`: Reset agent

#### SimpleWorld
- `createSimpleWorld()`: Create world
- `configure(width, height, visionWidth, visionHeight)`: Configure world
- `reset()`: Reset world
- `setAgentStart(x, y)`: Set agent start position
- `update(timestep)`: Update world
- `applyMotorCommand(cmd, currentTime)`: Apply motor command
- `getSensoryPercept()`: Get sensory percept
- `getAgentBody()`: Get agent body
- `addObject(obj)`: Add object
- `removeObject(x, y)`: Remove object at position
- `isValidPosition(x, y)`: Check position validity
- `getObjectAt(x, y)`: Get object at position

### Convenience Functions

- `run_simulation(brain, world, agent, num_steps)`: Run complete simulation
- `run_agent_simulation(config, num_steps)`: Run agent simulation
- `minimal_example()`: Minimal example

## Installation

### From Source (Recommended for Development)

1. Install build dependencies:
   ```bash
   pip install pybind11 scikit-build-core pytest numpy
   ```

2. Build and install:
   ```bash
   pip install .
   # or in development mode:
   pip install -e .
   ```

3. Verify installation:
   ```python
   import pynlm
   print(pynlm.__version__)
   ```

### From Binary (Pre-built wheels)

If pre-built wheels are available for your platform:

```bash
pip install pynlm
```

## Basic Usage

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(1000):
    brain.step(i)

print("Simulation complete!")
```

### Agent Simulation

```python
import pynlm

def run_agent_simulation(num_steps=1000):
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
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
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

## Advanced Features

### Configuration Management

```python
config = pynlm.createDefaultConfig()

# Set various configuration parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.dopamine.scale", 1.0)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config.loadFromFile("my_config.json")

# Get configuration summary
print(config.summary())
```

### Brain Control

```python
# Brain statistics
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Region management
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)

# Save/load brain state
brain.save("brain_checkpoint.bin")
brain.load("brain_checkpoint.bin")

# Developmental control
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
stage = brain.getDevelopmentalStage()

# Learned preference (Phase 5)
brain.setLearnedPreference(0.5)
preference = brain.getLearnedPreference()
```

### Agent Control

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(False)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Get agent statistics
print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Novelty: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"Development stage: {agent.getDevelopmentalStage()}")

# Thought trace
print(f"Thought trace: {agent.getThoughtTrace()}")

# Text encoding (Phase 6)
text_pattern = agent.encodeText("hello")
print(f"Text pattern length: {len(text_pattern)}")

# Internal firing rates
internal_rates = agent.getInternalFiringRates()
print(f"Internal rates: {len(internal_rates)} values")

# Motor group activities
motor_activities = agent.getMotorGroupActivities()
for activity in motor_activities:
    print(f"Motor group {activity.name}: activity={activity.activity:.3f}, "
          f"firingRate={activity.firingRate:.3f}")
```

### World Simulation

```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# World operations
print(f"Width: {world.getWidth()}, Height: {world.getHeight()}")
print(f"Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
print(f"Simulation time: {world.getSimulationTime()}")

# Add objects
resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0)
world.addObject(resource)

hazard = pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, value=-1.0)
world.addObject(hazard)

# Check positions
print(f"Position (5,5) valid: {world.isValidPosition(5.0, 5.0)}")
print(f"Position (10,10) valid: {world.isValidPosition(10.0, 10.0)}")

# Get objects
obj = world.getObjectAt(5.0, 5.0)
if obj:
    print(f"Object at (5,5): type={obj.type}, value={obj.value}")

# Set random seed for reproducibility
world.setRandomSeed(42)
```

## Configuration Parameters

### Brain Configuration

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

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float |  deve | STDP time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= Money 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

## Examples

For complete usage examples, refer to:
- `HOW_TO_USE.md` - Comprehensive documentation
- `easy_usage.md` - Beginner-friendly guide
- `test_bindings.py` - Comprehensive test script

## Testing

The module includes comprehensive tests. You can run tests using:

```bash
pytest tests/  # If test directory exists
python test_bindings.py  # Custom test script
```

## License

MIT License

Copyright (c) 2026 NLM Authors

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.