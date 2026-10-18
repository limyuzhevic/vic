# NLM Python API Reference

## Overview

This document provides comprehensive API documentation for the NLM (Neural Learning Machine) Python bindings (`pynlm`). The Python bindings provide a high-level interface to the C++ neural simulation framework, enabling users to create, simulate, and analyze neural networks and artificial brains.

The bindings are implemented using pybind11 and provide access to all core NLM functionality including neural computation, learning, memory, cognition, and world interaction.

## Installation

The NLM Python bindings are installed as part of the main NLM project:

```bash
# From source (recommended for development)
pip install -e .

# Or install from built package
pip install pynlm
```

Verify installation:
```python
import pynlm
print(f"NLM version: {pynlm.__version__}")
```

## Core Classes

### Config
Configuration management for NLM systems.

**Methods:**
- `loadFromFile(filepath)` - Load configuration from JSON file
- `loadFromArgs(argc, argv)` - Load configuration from command line arguments
- `saveToFile(filepath)` - Save configuration to JSON file
- `has(key)` - Check if configuration key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration entries
- `summary()` - Get configuration summary
- `set(key, value)` - Set configuration value (inherited from C++ Config)

**Properties:**
- `__repr__()` - String representation

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Configure specific parameters
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.1)
config.set("stdp_ltp_weight", 0.01)

# Save configuration
config.saveToFile("my_config.json")
```

### Brain
Central neural simulation brain class with LIF neuron dynamics and plasticity.

**Creation:**
```python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
```

**Methods:**
- `initialize()` - Initialize the brain with configuration
- `step(currentStep[, currentTime])` - Perform simulation step
- `receiveSensoryInput(input)` - Inject sensory input into brain
- `injectCurrent(neuron, current)` - Inject current into specific neuron
- `injectCurrentToNeurons(type, current)` - Inject current to all neurons of type
- `produceAction()` - Produce motor action based on neural activity
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file
- `addRegion(name)` - Add a new neural region
- `getRegion(id)` - Get region by ID
- `getRegionCount()` - Get number of regions
- `getRegionIds()` - Get all region IDs
- `getRegions()` - Get all regions
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getActiveNeuronCount()` - Get active neuron count
- `getFiringNeuronCount()` - Get firing neuron count
- `getAverageFiringRate()` - Get average firing rate
- `getExcitationInhibitionRatio()` - Get E/I balance ratio
- `getTotalSpikeCount()` - Get total spike count
- `getDevelopmentalStage()` - Get developmental stage
- `setDevelopmentalStage(stage)` - Set developmental stage
- `getConfig()` - Get configuration
- `logStatus()` - Log brain status

**Properties:**
- `isInitialized()` - Check if brain is initialized
- `getNeuronCount()` - Get neuron count (alias for `getTotalNeuronCount()`)
- `getSynapseCount()` - Get synapse count (alias for `getTotalSynapseCount()`)

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### AgentBrain
Interface connecting NLM brain to world environment with sensory-motor processing.

**Creation:**
```python
agent = pynlm.createAgentBrain(brain)
```

**Methods:**
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory percept and inject to brain
- `decodeMotorCommand()` - Decode brain motor activity to motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward-based neuromodulation
- `updateDevelopment(timestep)` - Update development system
- `getDevelopmentalStage()` - Get developmental stage
- `getNeuromodulationLevel()` - Get neuromodulation level
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error
- `reset()` - Reset agent for new episode
- `getBrain()` - Get underlying brain
- `enableRewardModulation(enable)` - Enable/disable reward modulation
- `enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `enableDevelopment(enable)` - Enable/disable development
- `enableCuriosity(enable)` - Enable/disable curiosity
- `isRewardModulationEnabled()` - Check if reward modulation enabled
- `isStructuralPlasticityEnabled()` - Check if structural plasticity enabled
- `isDevelopmentEnabled()` - Check if development enabled
- `isCuriosityEnabled()` - Check if curiosity enabled

```python
import pynlm

# Create complete agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

### SimpleWorld
Simple 2D world for NLM simulation with entity interaction.

**Creation:**
```python
world = pynlm.createSimpleWorld()
```

**Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `configureWithDefaults()` - Configure with default settings
- `reset()` - Reset world to initial state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world by timestep (alias: `step()`)
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `applyAction(action, currentTime)` - Apply action (alias for `applyMotorCommand()`)
- `getSensoryPercept()` - Get sensory percept (alias: `getPercept()`)
- `getAgentBody()` - Get agent body state
- `addObject(obj)` - Add world object
- `addResource(x, y, value)` - Add resource object
- `removeObject(x, y)` - Remove object
- `isValidPosition(x, y)` - Check if position is valid
- `getWidth()`, `getHeight()` - Get world dimensions
- `getMaxEnergy()`, `setMaxEnergy(e)` - Get/set max energy
- `getEnergyDecayRate()`, `setEnergyDecayRate(r)` - Get/set energy decay rate
- `getSimulationTime()` - Get current simulation time
- `setRandomSeed(seed)`, `getRandomSeed()` - Get/set random seed

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Run world update
for step in range(100):
    world.update(0.1)
    
    # Get agent's view
    percept = world.getSensoryPercept()
    
    # Add resources periodically
    if step % 50 == 0:
        world.addResource(5.0, 5.0, 10.0)
```

### SensoryInput Classes

#### Vision
Visual sensory input with RGB channels.

```python
vision = pynlm.Vision(width=8, height=8, channels=3)
```

**Methods:**
- `setData(data)` - Set raw pixel data
- `getWidth()`, `getHeight()`, `getChannels()` - Get dimensions

#### Audio
Audio sensory input with samples.

```python
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
```

**Methods:**
- `setData(data)` - Set audio samples
- `setSampleRate(sampleRate)` - Set sample rate
- `getSampleRate()`, `getNumSamples()` - Get properties

#### InternalSignals
Internal state signals (e.g., homeostatic variables).

```python
signals = pynlm.InternalSignals()
signals.addSignal(value)
```

**Methods:**
- `addSignal(value)` - Add signal value
- `clearSignals()` - Clear all signals

### Action
Motor action representation for the agent.

```python
action = pynlm.Action(pynlm.ActionType.MoveForward)
```

**Constructors:**
- `Action()` - Empty action
- `Action(type)` - Action with type
- `Action(type, parameters)` - Action with parameters

**Methods:**
- `getType()`, `setType(type)` - Get/set action type
- `getParameters()`, `setParameters(params)` - Get/set parameters
- `getName()` - Get human-readable name
- `clone()` - Clone action

### WorldObject
Representation of objects in the world.

```python
obj = pynlm.WorldObject(x=5.0, y=5.0, type=pynlm.WorldObjectType.Resource, value=1.0)
```

**Properties:**
- `x`, `y`, `radius`, `type`, `value`, `active` - Position and properties

### AgentBody
Agent state representation including position, orientation, and capabilities.

```python
body = world.getAgentBody()
```

**Properties:**
- `x`, `y`, `orientation` - Position and orientation
- `velocityX`, `velocityY`, `angularVelocity` - Motion
- `energy`, `health`, `age` - Physiological state
- `isMoving`, `isTurning` - Action flags
- `lastActionTime` - Timing

### SensoryPercept
Combined sensory data from all modalities.

```python
percept = world.getSensoryPercept()
```

**Methods:**
- `getVision()`, `setVision(vision)` - Vision data
- `getVisionWidth()`, `getVisionHeight()` - Vision dimensions
- `getTouch()`, `setTouch(touch)` - Touch data
- `getInternal()`, `setInternal(internal)` - Internal signals
- `getProprioception()`, `setProprioception(proprioception)` - Proprioception
- `getAudio()`, `setAudio(audio)` - Audio data
- `getAllSignals()` - Get all signals
- `getTimestamp()`, `setTimestamp(timestamp)` - Timestamp

## Enumerations

### NeuronType
```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal
```

### SynapseType
```python
pynlm.SynapseType.Excitatory
pynlm.SynapseType.Inhibitory
pynlm.SynapseType.Modulatory
pynlm.SynapseType.Electrical
pynlm.SynapseType.GapJunction
```

### DevelopmentalStage
```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

### FiringState
```python
pynlm.FiringState.Resting
pynlm.FiringState.Active
pynlm.FiringState.Refractory
pynlm.FiringState.Inhibited
```

### ActionType
```python
pynlm.ActionType.MoveForward, MoveBackward, MoveLeft, MoveRight
pynlm.ActionType.TurnLeft, TurnRight, Look
pynlm.ActionType.LookUp, LookDown, LookLeft, LookRight
pynlm.ActionType.Interact, Eat, Drink, Rest, Wait
pynlm.ActionType.Custom
```

### MotorCommand
```python
pynlm.MotorCommand.MoveForward, MoveBackward, TurnLeft, TurnRight
pynlm.MotorCommand.LookLeft, LookRight, Interact, Wait
```

### WorldObjectType
```python
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

### Identity Types
```python
pynlm.NeuronId()  # Unique neuron identifier
pynlm.SynapseId()  # Unique synapse identifier
pynlm.RegionId()   # Unique region identifier
pynlm.PopulationId()  # Unique population identifier
```

## Factory Functions

### Configuration Factory Functions
```python
# Create different configuration profiles
config = pynlm.createDefaultConfig()      # Basic configuration
config = pynlm.createStandardConfig()    # Standard for experiments
config = pynlm.createExplorationConfig() # Optimized for learning
```

### Component Factory Functions
```python
# Create main components
brain = pynlm.createBrain(config)    # Neural brain
world = pynlm.createSimpleWorld()   # Environment
agent = pynlm.createAgentBrain(brain) # Brain-world interface
```

## Convenience Functions

### runSimpleExperiment
```python
brain, world, agent = create_components()
spikes, curiosity, novelty = pynlm.runSimpleExperiment(
    brain, world, agent, numSteps=1000
)
```

## Constants

### Module Constants
```python
pynlm.MAX_WORLD_SIZE          # Maximum world size
pynlm.DEFAULT_VISION_SIZE     # Default vision size
pynlm.DEFAULT_WORLD_SIZE      # Default world size
pynlm.DEFAULT_MAX_ENERGY      # Default maximum energy
```

### Invalid Identifiers
```python
pynlm.INVALID_NEURON_ID
pynlm.INVALID_SYNAPSE_ID
pynlm.INVALID_REGION_ID
pynlm.INVALID_POPULATION_ID
```

## Error Handling

The Python bindings raise `RuntimeError` for:
- Invalid operations (e.g., using uninitialized brain)
- File I/O errors (e.g., loading non-existent files)
- Configuration errors

```python
try:
    brain.load("non_existent_file.bin")
except RuntimeError as e:
    print(f"Error loading brain: {e}")
```

## Module Metadata

```python
print(f"NLM Version: {pynlm.__version__}")
print(f"Description: {pynlm.__description__}")
print(f"Author: {pynlm.__author__}")
print(f"License: {pynlm.__license__}")
```

## Complete Example

This example demonstrates all major NLM features:

```python
import pynlm
import time

def main():
    # 1. Create components with advanced configuration
    config = pynlm.createExplorationConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 2. Enable learning capabilities
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 3. Add some resources to environment
    world.addResource(5.0, 5.0, 10.0)
    world.addResource(25.0, 25.0, 15.0)
    
    # 4. Run simulation with comprehensive monitoring
    metrics = []
    start_time = time.time()
    
    for step in range(1000):
        # Update environment
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward based on position and resource access
        body = world.getAgentBody()
        x, y = body.x, body.y
        
        # Reward for staying in bounds and finding resources
        position_reward = 0.1 if (0 <= x < 30 and 0 <= y < 30) else -1.0
        resource_reward = 0.0
        
        # Check if agent is near a resource
        for obj in [world.getAgentBody()]:  # In real implementation, you'd iterate all objects
            pass
        
        # Apply reward modulation
        agent.applyRewardModulation(position_reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record metrics
        if step % 50 == 0:
            metrics.append({
                'step': step,
                'time': time.time() - start_time,
                'reward': position_reward,
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'dev_stage': agent.getDevelopmentalStage()
            })
            print(f"Step {step}: Curiosity={agent.getCuriosityLevel():.2f}, "
                  f"Novelty={agent.getNoveltyLevel():.2f}, "
                  f"Firing={brain.getFiringNeuronCount()}")
    
    # 5. Analyze results
    print("\n=== SIMULATION RESULTS ===")
    print(f"Total simulation time: {time.time() - start_time:.2f}s")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.2f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.2f}")
    print(f"Final developmental stage: {agent.getDevelopmentalStage()}")
    print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f}")
    
    # 6. Save results
    brain.save("final_brain_state.bin")
    config.saveToFile("final_config.json")
    
    # 7. Save metrics to file
    with open("simulation_metrics.json", 'w') as f:
        import json
        json.dump(metrics, f, indent=2)

if __name__ == "__main__":
    main()
```

## API Compatibility

The Python API maintains compatibility with:
- **PEP 8** coding standards
- **Python 3.8+** syntax
- **Type hints** where applicable
- **Consistent naming conventions** across all classes

The bindings are designed to be intuitive for researchers and developers familiar with neural simulation or Python programming.

## Advanced Usage

### Custom Configuration Schema
```python
# Load custom configuration
config = pynlm.createDefaultConfig()
config.loadFromFile("experiment_config.json")
```

### Batch Processing
```python
# Process multiple brains in parallel
brains = [pynlm.createBrain(pynlm.createDefaultConfig()) for _ in range(10)]
for brain in brains:
    brain.initialize()

# Process multiple simulation steps
for step in range(1000):
    for brain in brains:
        brain.step(step)
```

### Performance Monitoring
```python
import time
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Time simulation
start = time.perf_counter()
for step in range(10000):
    brain.step(step)
end = time.perf_counter()

print(f"Simulated {10000} steps in {end-start:.2f} seconds")
print(f"Performance: {10000/(end-start):.0f} steps/second")
```

## Troubleshooting

### Common Issues

**Q: "Brain not initialized error"**
```python
# Error: Brain not initialized (neuron_count = 0)
# Solution: Always call brain.initialize() after creation
brain = pynlm.createBrain(config)
brain.initialize()  # <- Required!
```

**Q: "ImportError: No module named pynlm"**
```bash
pip install pynlm
# Or for development:
pip install -e .
```

**Q: "Segmentation fault during simulation"**
```python
# Check for proper initialization and reset
brain.reset()  # Required before reusing brain
```

**Q: "Configuration key not found"**
```python
# Check if key exists before accessing
if config.has("my.parameter"):
    value = config.get("my.parameter")
```

### Debug Mode
```python
import pynlm

# Enable verbose logging
config.set("brain.debug", True)
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        brain.logStatus()
```

## Migration Guide

### From C++ to Python

The Python API provides similar functionality to the C++ API but with:
- Simplified constructor syntax
- Automatic memory management
- Easier file I/O operations
- Built-in convenience functions

### Key Differences

| C++ Feature | Python Equivalent |
|-------------|-------------------|
| `std::shared_ptr<Config>` | `Config()` instance |
| `Brain::step(Step currentStep)` | `brain.step(currentStep)` |
| `world.update(Time dt)` | `world.update(dt)` |
| Manual memory management | Automatic garbage collection |
| Raw pointers | Python object references |

## Future Enhancements

The Python bindings will continue to evolve with:
- Advanced visualization APIs
- Machine learning integration
- Cloud simulation support
- Real-time streaming capabilities
- Enhanced debugging tools

## Support

For issues and questions:
1. Check the [NLM Documentation](https://nlm.readthedocs.io)
2. Review example scripts in `nlm_examples.py`
3. Consult the CLI tool in `nlm_cli.py`
4. Visit the NLM project repository

This API reference is continuously updated to reflect new features and improvements to the NLM Python bindings.
