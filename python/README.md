# NLM Python Bindings

## Overview

The NLM Python bindings provide a Python interface to the Neural Learning Machine (NLM) framework, enabling you to create, simulate, and experiment with artificial brains in Python.

## Installation

### Prerequisites

- Python 3.8 or higher
- pybind11 >= 2.10
- CMake >= 3.16

### Building from Source

```bash
# Clone the repository
git clone <repository-url>
cd nlm-python

# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the module
make -j$(nproc)

# Install (optional)
pip install -e .
```

### Quick Install

If you have pip and pybind11 available:

```bash
pip install -e .
```

## Getting Started

### Basic Import

```python
import pynlm
```

### Create a Default Configuration

```python
# Method 1: Using helper function
config = pynlm.createDefaultConfig()

# Method 2: Creating a new config
config = pynlm.Config()
config.loadFromFile("my_config.json")
```

### Create and Initialize a Brain

```python
import pynlm

# Create brain with configuration
brain = pynlm.createBrain(config)

# Initialize the brain (essential step)
brain.initialize()
```

### Create and Configure a World

```python
# Create world
world = pynlm.createWorld()

# Configure world dimensions and vision
world.configure(
    width=20,        # World size in cells (x-axis)
    height=20,       # World size in cells (y-axis)
    visionWidth=8,   # Agent field of view (x-axis)
    visionHeight=8   # Agent field of view (y-axis)
)

# Reset world to initial state
world.reset()
```

### Create Agent and Connect Components

```python
# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize agent with world
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```

## Complete Examples

### Example 1: Simple Brain Simulation

```python
import pynlm

# Create brain with default settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Simulate brain activity
for step in range(100):
    brain.step(step)
    
print(f"Simulation complete. Generated {brain.getTotalSpikeCount()} spikes")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Example 2: Complete Agent in World

```python
import pynlm
import time

# Setup complete agent system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Main agent loop
print("Starting agent simulation...")
for step in range(200):
    # Update world environment
    world.update(0.05)
    
    # Get sensory input (what agent sees)
    percept = world.getSensoryPercept()
    
    # Process sensory information
    agent.processSensoryInput(percept)
    
    # Brain processes and thinks
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Execute action in world
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward-based learning
    if result.success:
        agent.applyRewardModulation(result.reward, 0.0)
    
    # Print status every 20 steps
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Action: {action.getName()}")
        print(f"  Reward: {result.reward:+.2f}")

print("\nSimulation completed!")
print(f"Final statistics:")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Development stage: {brain.getDevelopmentalStage()}")
```

### Example 3: Advanced Agent with Learning

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

def run_advanced_simulation(config_file=None):
    """Run an advanced simulation with learning and visualization"""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Customize configuration for advanced learning
    config.set("brain.plasticity.stdp.enable", True)
    config.set("brain.plasticity.hebbian.enable", True)
    config.set("brain.development.enabled", True)
    config.set("brain.motivation.curiosity.weight", 0.7)
    config.set("brain.sensory.vision.enabled", True)
    config.set("brain.sensory.audio.enabled", True)
    config.set("brain.sensory.internal.enabled", True)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world with resources and hazards
    world = pynlm.createWorld()
    world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
    
    # Add interesting world features
    # Resources (food) - positive reward
    for x in range(0, 20, 4):
        for y in range(0, 20, 4):
            world.addObject(pynlm.WorldObject(
                x, y, 
                pynlm.WorldObjectType.Resource, 
                value=10.0
            ))
    
    # Hazards (walls, dangers) - negative reward  
    for y in [5, 10, 15]:
        for x in range(5, 15):
            world.addObject(pynlm.WorldObject(
                x, y,
                pynlm.WorldObjectType.Hazard,
                value=5.0
            ))
    
    world.reset()
    
    # Create and initialize agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Track metrics for analysis
    metrics = {
        'steps': [],
        'firing_rates': [],
        'rewards': [],
        'curiosity_levels': [],
        'novelty_levels': []
    }
    
    print("Starting advanced simulation...")
    
    # Run simulation
    for step in range(500):
        # Update world
        world.update(0.05)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process and learn
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Decide and act
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reinforcement learning
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Record metrics
        metrics['steps'].append(step)
        metrics['firing_rates'].append(brain.getAverageFiringRate())
        metrics['rewards'].append(result.reward)
        metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        metrics['novelty_levels'].append(agent.getNoveltyLevel())
        
        # Print progress
        if step % 50 == 0:
            print(f"Step {step}: "
                  f"Rate: {brain.getAverageFiringRate():.2f}Hz, "
                  f"Action: {action.getName()}, "
                  f"Reward: {result.reward:+.2f}")
    
    return brain, world, agent, metrics

# Run the simulation
brain, world, agent, metrics = run_advanced_simulation()

# Analyze results
print(f"\nSimulation Results:")
print(f"Total steps: {len(metrics['steps'])}")
print(f"Average firing rate: {np.mean(metrics['firing_rates']):.2f} Hz")
print(f"Final development stage: {brain.getDevelopmentalStage()}")

# Plot results
plt.figure(figsize=(15, 10))

plt.subplot(2, 2, 1)
plt.plot(metrics['steps'], metrics['firing_rates'])
plt.xlabel('Step')
plt.ylabel('Average Firing Rate (Hz)')
plt.title('Brain Activity Over Time')
plt.grid(True)

plt.subplot(2, 2, 2)
plt.plot(metrics['steps'], metrics['rewards'])
plt.xlabel('Step')
plt.ylabel('Reward')
plt.title('Reward History')
plt.grid(True)

plt.subplot(2, 2, 3)
plt.plot(metrics['steps'], metrics['curiosity_levels'])
plt.xlabel('Step')
plt.ylabel('Curiosity Level')
plt.title('Exploration Drive Over Time')
plt.grid(True)

plt.subplot(2, 2, 4)
plt.plot(metrics['steps'], metrics['novelty_levels'])
plt.xlabel('Step')
plt.ylabel('Novelty Level')
plt.title('Novelty Detection Over Time')
plt.grid(True)

plt.tight_layout()
plt.show()

print("\nAdvanced simulation complete!")
return brain, metrics

# Run if executed as script
if __name__ == "__main__":
    run_advanced_simulation()
```

## API Reference

### Classes

#### pynlm.Brain
Central neural simulation brain class.

**Attributes:**
- `config`: Configuration object

**Methods:**
- `initialize()`: Initialize the brain
- `step(step_number, current_time)`: Perform a simulation step
- `produceAction()`: Generate motor action
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state
- `getTotalNeuronCount()`: Get total neuron count
- `getTotalSynapseCount()`: Get total synapse count
- `getActiveNeuronCount()`: Get active neuron count
- `getFiringNeuronCount()`: Get firing neuron count
- `getAverageFiringRate()`: Get average firing rate
- `getExcitationInhibitionRatio()`: Get E/I balance
- `getTotalSpikeCount()`: Get total spike count
- `getDevelopmentalStage()`: Get developmental stage
- `setDevelopmentalStage(stage)`: Set developmental stage
- `getNeuromodulationLevel()`: Get neuromodulation level
- `getCuriosityLevel()`: Get curiosity level
- `getNoveltyLevel()`: Get novelty level
- `getPredictionError()`: Get prediction error
- `enableRewardModulation(enabled)`: Enable/disable reward modulation
- `enableStructuralPlasticity(enabled)`: Enable/disable structural plasticity
- `enableDevelopment(enabled)`: Enable/disable development
- `enableCuriosity(enabled)`: Enable/disable curiosity
- `applyNeuromodulation(signal)`: Apply neuromodulation

#### pynlm.SimpleWorld
Simple 2D world for NLM simulation.

**Methods:**
- `configure(width, height, visionWidth, visionHeight)`: Configure world dimensions
- `reset()`: Reset world
- `setAgentStart(x, y)`: Set agent starting position
- `update(timestep)`: Update world
- `applyMotorCommand(action, currentTime)`: Apply motor command
- `getSensoryPercept()`: Get sensory perception
- `getAgentBody()`: Get agent body state
- `addObject(obj)`: Add world object
- `removeObject(x, y)`: Remove object
- `isValidPosition(x, y)`: Check position validity
- `getWidth()`, `getHeight()`: Get world dimensions
- `getSimulationTime()`: Get simulation time

#### pynlm.AgentBrain
Agent brain interface connecting NLM brain to world.

**Methods:**
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command
- `applyRewardModulation(reward, predictedReward)`: Apply reward modulation
- `updateDevelopment(timestep)`: Update development
- `getDevelopmentalStage()`: Get developmental stage
- `enableRewardModulation(enabled)`: Enable/disable reward modulation
- `enableStructuralPlasticity(enabled)`: Enable/disable structural plasticity
- `enableDevelopment(enabled)`: Enable/disable development
- `enableCuriosity(enabled)`: Enable/disable curiosity

#### pynlm.Action
Represents a motor action.

**Attributes:**
- `type`: ActionType enum
- `parameters`: List of parameters

**Methods:**
- `getName()`: Get action name
- `clone()`: Clone action

#### pynlm.SensoryPercept
Sensory percept data.

**Methods:**
- `getVision()`: Get vision data
- `getTouch()`: Get touch data
- `getInternal()`: Get internal signals
- `getProprioception()`: Get proprioception
- `getAudio()`: Get audio data
- `getAllSignals()`: Get all signals

#### pynlm.Config
Configuration object.

**Methods:**
- `loadFromFile(filepath)`: Load from JSON
- `loadFromArgs(argc, argv)`: Load from args
- `saveToFile(filepath)`: Save to JSON
- `has(key)`: Check if key exists
- `get(key, default)`: Get value
- `getKeys()`: Get all keys
- `clear()`: Clear configuration

#### pynlm.Reward
Reward signal for reinforcement learning.

**Methods:**
- `getValue()`: Get current value
- `setValue(value)`: Set value
- `add(delta)`: Add to value
- `reset()`: Reset to zero
- `computeReward(observation)`: Compute from observation
- `getAverageReward(window)`: Get average reward
- `isPositive()`: Check if positive
- `getMagnitude()`: Get magnitude

#### pynlm.Novelty
Novelty detection signal.

**Methods:**
- `initialize(brain)`: Initialize with brain
- `getLevel()`: Get level
- `setLevel(level)`: Set level
- `detectNovelty(observation, previousObservation)`: Detect from observations
- `detectNovelty(currentPattern, previousPattern)`: Detect from patterns
- `update(dt)`: Update with timestep

#### pynlm.Curiosity
Curiosity drive for exploration.

**Methods:**
- `initialize(brain)`: Initialize with brain
- `getLevel()`: Get level
- `update(novelty, predictionError, dt)`: Update
- `getExplorationDrive()`: Get exploration drive
- `setNoveltyWeight(weight)`: Set novelty weight
- `setPredictionErrorWeight(weight)`: Set prediction error weight

#### pynlm.PredictionError
Prediction error signal.

**Methods:**
- `initialize(brain)`: Initialize with brain
- `getError()`: Get error value
- `computeError(predicted, actual)`: Compute error
- `updatePrediction(newPrediction)`: Update prediction
- `getMagnitude()`: Get magnitude

#### pynlm.Dopamine
Dopamine neuromodulator.

**Methods:**
- `signalReward(reward)`: Signal reward
- `signalRewardPredictionError(error)`: Signal prediction error

#### pynlm.Acetylcholine
Acetylcholine neuromodulator.

**Methods:**
- `enhanceMemory(strength)`: Enhance memory
- `regulateAttention(focus)`: Regulate attention

#### pynlm.Norepinephrine
Norepinephrine neuromodulator.

**Methods:**
- `increaseAlertness(amount)`: Increase alertness
- `decreaseStress(amount)`: Decrease stress

#### pynlm.Serotonin
Serotonin neuromodulator.

**Methods:**
- `improveMood(amount)`: Improve mood
- `reduceImpulsivity(amount)`: Reduce impulsivity

### Enums

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

#### NeuronType
- `Excitatory`
- `Inhibitory`
- `Modulatory`
- `Sensory`
- `Motor`
- `Internal`

#### WorldObjectType
- `Empty`
- `Resource`
- `Hazard`
- `Wall`
- `Marker`

#### DevelopmentalStage
- `Initial`
- `CriticalPeriod`
- `Maturation`
- `Adult`
- `Aging`

### Error Handling

The module defines the following exceptions:

- `pynlm.RuntimeError`: General runtime errors
- `ValueError`: Invalid parameter values
- `FileNotFoundError`: Missing files
- `RuntimeError`: Simulation errors

### Configuration Examples

#### Basic Configuration
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.sensory.vision.enabled", True)
config.set("brain.motivation.curiosity.weight", 0.5)
```

#### Custom Configuration
```python
import json

# Create custom config
config = pynlm.createDefaultConfig()
config.clear()

# Add custom settings
custom_settings = {
    "brain": {
        "plasticity": {
            "stdp": {"enable": True, "learningRate": 0.01},
            "hebbian": {"enable": True, "strength": 0.005}
        },
        "development": {"enabled": True, "plasticityRate": 0.1},
        "motivation": {
            "dopamine": {"weight": 0.8},
            "curiosity": {"weight": 0.2}
        },
        "sensory": {
            "vision": {"enabled": True, "resolution": "high"},
            "audio": {"enabled": False},
            "internal": {"enabled": True}
        }
    }
}

# Save to file
with open("my_config.json", "w") as f:
    json.dump(custom_settings, f, indent=2)

config.loadFromFile("my_config.json")
```

## Installation from Source

If you want to build from source, here's the process:

```bash
# Clone the repository
git clone <repository-url>
cd nlm-python

# Create build directory
mkdir build
cd build

# Configure CMake with options
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DPYTHON_EXECUTABLE=/usr/bin/python3 \
    -DPYTHON_LIBRARY=/usr/lib/x86_64-linux-gnu/libpython3.10 \
    -DPYTHON_INCLUDE_DIR=/usr/include/python3.10

# Build
make -j$(nproc)

# Install (optional)
pip install -e .

# Run tests
ctest --output-on-failure
```

## Testing

Run the included tests to verify the Python bindings:

```bash
cd build
./test_pynlm  # If tests are built separately
python -m pytest tests/  # If Python tests exist
```

## Performance Considerations

1. **Memory Management**: The module uses smart pointers to avoid memory leaks
2. **Threaded Operations**: Avoid using the bindings in multi-threaded environments
3. **Large Simulations**: For long simulations, consider checkpointing with `brain.save()`
4. **Configuration**: Cache configuration objects for reuse

## Troubleshooting

### Common Issues

1. **Module not found**
   ```python
   # Ensure you're in the correct directory
   # Check that the module is built and installed
   python -c "import pynlm; print(pynlm.__version__)"
   ```

2. **Import error**
   ```python
   # Check that pybind11 is properly installed
   python -c "import pybind11; print(pybind11.__version__)"
   ```

3. **Simulation errors**
   ```python
   # Check for common issues:
   - Ensure brain is initialized before use
   - Verify world configuration
   - Check for memory leaks
   ```

### Debug Mode

Enable debug mode for troubleshooting:

```python
import pynlm

# Create config with debug options
config = pynlm.createDefaultConfig()
config.set("brain.logger.level", "DEBUG")
config.set("brain.debug.enabled", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

## Examples Directory

Check the `examples/` directory for additional usage patterns and complete applications:

- `basic_agent.py`: Simple agent example
- `advanced_simulation.py`: Complex simulation with visualization
- `custom_brain.py`: Custom brain configurations
- `world_interactions.py`: World interaction patterns

## Citing NLM

If you use NLM in your research, please cite:

```bibtex
@article{nlm2024,
  title={NLM: An Experimental Artificial Developmental Brain},
  author={NLM Research Team},
  journal={arXiv preprint arXiv:XXXX.XXXXX},
  year={2024},
  url={https://arxiv.org/abs/XXXX.XXXXX}
}
```

## License

NLM is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgements

This module is based on the NLM (Neural Learning Machine) framework developed by the NLM Research Team. The Python bindings were generated using pybind11.

## Support

For support with the Python bindings, please:
1. Check the documentation
2. Review examples
3. Search for existing issues
4. Create a new issue if you encounter problems
