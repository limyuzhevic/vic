# NLM (Neural Learning Machine) - API Reference

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that implements a neural system capable of learning and adapting through interaction with an environment. This document provides a comprehensive reference for the NLM Python API.

## Version Information

- **Version**: 1.0.0
- **C++ Version**: 20
- **License**: MIT

## Core API

### Configuration Management

The `Config` class provides a comprehensive configuration system for the NLM brain.

#### Creating Configurations

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create custom configuration
config = pynlm.Config()
```

#### Configuration Methods

| Method | Description | Signature |
|--------|-------------|------------|
| `loadFromFile(filepath)` | Load configuration from file | `def loadFromFile(self, filepath: str)` |
| `loadFromArgs(argc, argv)` | Load configuration from command line arguments | `def loadFromArgs(self, argc: int, argv: List[str])` |
| `saveToFile(filepath)` | Save configuration to file | `def saveToFile(self, filepath: str)` |
| `has(key)` | Check if configuration key exists | `def has(self, key: str) -> bool` |
| `getKeys()` | Get all configuration keys | `def getKeys(self) -> List[str]` |
| `clear()` | Clear all configuration entries | `def clear(self)` |
| `summary()` | Get configuration summary | `def summary(self) -> str` |

#### Python-Friendly Configuration Methods

```python
# Set configuration values
config.setInt("brain.neuron_count", 1000)
config.setFloat("brain.connection_probability", 0.1)
config.setString("brain.name", "MyBrain")

# Get configuration values
neuron_count = config.getInt("brain.neuron_count", 1000)
connection_prob = config.getFloat("brain.connection_probability", 0.1)
brain_name = config.getString("brain.name", "Default")

# Load from Python dictionary
config_dict = {"brain.neuron_count": 500, "brain.connection_probability": 0.05}
config.loadFromDict(config_dict)
```

### Brain Core

The `Brain` class represents the central neural simulation component.

#### Brain Creation and Initialization

```python
import pynlm

# Create brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()

# Create brain with custom configuration
brain_config = pynlm.Config()
brain_config.set("brain.neuron_count", 1000)
brain_config.set("brain.region_count", 2)
brain = pynlm.createBrain(brain_config)
brain.initialize()
```

#### Brain Simulation

| Method | Description | Signature |
|--------|-------------|------------|
| `step(step_number)` | Perform a simulation step | `def step(self, step_number: int)` |
| `step(step_number, time)` | Perform a simulation step with timestamp | `def step(self, step_number: int, time: float)` |
| `receiveSensoryInput(input)` | Inject sensory input into the brain | `def receiveSensoryInput(self, input: SensoryInput)` |
| `injectCurrent(neuron, current)` | Inject current into a specific neuron | `def injectCurrent(self, neuron: NeuronId, current: float)` |
| `injectCurrentToNeurons(type, current)` | Inject current into all neurons of a type | `def injectCurrentToNeurons(self, type: NeuronType, current: float)` |
| `produceAction()` | Produce motor action based on neural activity | `def produceAction(self) -> Action` |
| `reset()` | Reset brain state | `def reset(self)` |
| `save(filepath)` | Save brain state to file | `def save(self, filepath: str)` |
| `load(filepath)` | Load brain state from file | `def load(self, filepath: str)` |
| `addRegion(name)` | Add a new neural region | `def addRegion(self, name: str) -> RegionId` |
| `getRegion(id)` | Get a region by ID | `def getRegion(self, id: RegionId) -> NeuralRegion` |
| `getTotalNeuronCount()` | Get total neuron count | `def getTotalNeuronCount(self) -> int` |
| `getTotalSynapseCount()` | Get total synapse count | `def getTotalSynapseCount(self) -> int` |
| `getActiveNeuronCount()` | Get count of active neurons | `def getActiveNeuronCount(self) -> int` |
| `getFiringNeuronCount()` | Get count of currently firing neurons | `def getFiringNeuronCount(self) -> int` |
| `getAverageFiringRate()` | Get average firing rate across all neurons | `def getAverageFiringRate(self) -> float` |
| `getExcitationInhibitionRatio()` | Get E/I balance ratio | `def getExcitationInhibitionRatio(self) -> float` |
| `getTotalSpikeCount()` | Get total spike count | `def getTotalSpikeCount(self) -> int` |
| `getDevelopmentalStage()` | Get current developmental stage | `def getDevelopmentalStage(self) -> DevelopmentalStage` |
| `setDevelopmentalStage(stage)` | Set developmental stage | `def setDevelopmentalStage(self, stage: DevelopmentalStage)` |

#### Brain Enums and Types

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types (for Actions)
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.MoveLeft
pynlm.ActionType.MoveRight
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.LookUp
pynlm.ActionType.LookDown
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
pynlm.ActionType.Custom

# Motor commands (for Brain output)
pynlm.MotorCommand.MoveForward
pynlm.MotorCommand.MoveBackward
pynlm.MotorCommand.TurnLeft
pynlm.MotorCommand.TurnRight
pynlm.MotorCommand.LookLeft
pynlm.MotorCommand.LookRight
pynlm.MotorCommand.Interact
pynlm.MotorCommand.Wait
```

### Action Class

The `Action` class represents a motor action with type and parameters.

```python
# Create actions
action1 = pynlm.Action(pynlm.ActionType.MoveForward)
action2 = pynlm.Action(pynlm.ActionType.Custom, [1.0, 0.5, 0.0])

# Action methods
action = pynlm.Action(pynlm.ActionType.MoveForward)
action.setType(pynlm.ActionType.TurnLeft)
action.getType()  # Returns ActionType

action.setParameters([1.0, 2.0, 3.0])
params = action.getParameters()  # Returns List[float]

name = action.getName()  # Returns string like "MoveForward"
action_name = action.getNameStr()  # Alias for getName()
param_count = len(action.getParameters())  # Number of parameters

# Clone action
cloned_action = action.clone()

# String representation
print(repr(action))  # "<Action: MoveForward>"
print(str(action))   # "MoveForward"
```

### AgentBrain Interface

The `AgentBrain` class connects the NLM brain to the world environment.

```python
# Create agent brain interface
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create and initialize with world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Decode motor command
action = agent.decodeMotorCommand()

# Apply neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Update development
agent.updateDevelopment(timestep)

# Get developmental stage
stage = agent.getDevelopmentalStage()

# Get neuromodulation levels
curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()
prediction_error = agent.getPredictionError()
neuromodulation_level = agent.getNeuromodulationLevel()
```

### SimpleWorld Environment

The `SimpleWorld` class represents a 2D simulation world.

```python
# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Control world state
world.reset()
world.setAgentStart(10.0, 10.0)
world.update(timestep=0.1)
world.setRandomSeed(42)

# Apply motor command
world.applyMotorCommand(action, world.getSimulationTime())

# Get sensory percept
percept = world.getSensoryPercept()

# Get agent body state
body = world.getAgentBody()

# Manage world objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Query properties
width = world.getWidth()
height = world.getHeight()
simulation_time = world.getSimulationTime()
max_energy = world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Sensory Input Classes

#### Vision

```python
# Create vision input
vision = pynlm.Vision(width=8, height=8, channels=3)

# Set vision data
vision_data = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8]
vision.setData(vision_data)

# Get vision properties
width = vision.getWidth()
height = vision.getHeight()
channels = vision.getChannels()
```

#### Audio

```python
# Create audio input
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)

# Set audio data
audio_data = [0.0] * 1024
for i in range(1024):
    audio_data[i] = math.sin(2 * math.pi * 440 * i / 44100)

audio.setData(audio_data)
audio.setSampleRate(44100)

sample_rate = audio.getSampleRate()
num_samples = audio.getNumSamples()
```

#### InternalSignals

```python
# Create internal signals
internal = pynlm.InternalSignals()

# Add signals
internal.addSignal(0.1)
internal.addSignal(0.2)
internal.addSignal(0.3)

# Clear signals
internal.clearSignals()
```

### Performance Monitoring

NLM provides comprehensive performance monitoring and profiling capabilities.

#### Basic Performance Stats

```python
# Get current performance statistics
stats = pynlm.getPerformanceStats()
print(stats)  # {'timestamp': 1234567890, 'version': '1.0.0'}
```

#### Simulation Profiling

```python
# Profile simulation with timing
brain = pynlm.createBrain(config)
brain.initialize()

results = pynlm.profileSimulation(brain, 1000)
print(results)
# {'total_time_seconds': 0.123,
#  'steps_per_second': 8130.08,
#  'final_firing_rate': 0.456,
#  'total_spikes': 456}
```

#### Benchmarking

```python
# Run comprehensive benchmark
brain = pynlm.createBrain(config)
brain.initialize()

step_counts = [1000, 5000, 10000, 20000]
benchmark_results = pynlm.benchmarkBrain(brain, step_counts)

for steps, metrics in benchmark_results.items():
    print(f"Steps: {steps}")
    print(f"  Duration: {metrics['duration_seconds']:.3f}s")
    print(f"  Steps/sec: {metrics['steps_per_second']:.0f}")
    print(f"  Avg firing rate: {metrics['avg_firing_rate']:.3f}")
    print(f"  Total spikes: {metrics['total_spikes']}")
```

## World Interaction Patterns

### Complete Agent Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
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

### Error Handling and Best Practices

```python
import pynlm

# Always call initialize() before step()
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Check if initialization succeeded
if brain.initialize():
    print("Brain initialized successfully")
    
    # Run simulation steps
    for i in range(100):
        brain.step(i)
        
    # Check brain status
    if brain.getFiringNeuronCount() > 0:
        print(f"Simulation produced {brain.getFiringNeuronCount()} firing neurons")
else:
    print("Brain initialization failed")
    # Handle error appropriately
```

## Command Line Interface

The NLM project provides command line interfaces for different use cases:

### Basic Usage

```bash
# Build and run the main executable
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run basic demonstration
./nlm

# Run specific demo modes
./nlm --mode connectivity --neurons 1000 --steps 5000
./nlm --mode plasticity --neurons 2000 --steps 10000 --verbose
./nlm --mode benchmark --neurons 5000 --steps 20000
```

### Phase 6 Demo

```bash
# Quick integration verification
./nlm_phase6_demo --mode quick

# Full integration test with custom settings
./nlm_phase6_demo --mode full --neurons 750 --steps 3000

# Interactive demo with progress monitoring
./nlm_phase6_demo --mode interactive --neurons 1000 --steps 5000 --no-checkpoint

# Show help
./nlm_phase6_demo --help
```

## Testing and Validation

### Unit Tests

The project includes comprehensive unit tests for C++ components:

```bash
# Run C++ unit tests
./nlm_test

# Run tests with verbose output
./nlm_test --verbose
```

### Python Testing

```python
# Test Python bindings
python3 test_bindings.py

# Import and verify basic functionality
import pynlm

# Create and test brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Verify brain properties
assert brain.getTotalNeuronCount() == config.getOr("brain.neuron_count", 1000)
assert brain.getTotalSynapseCount() > 0
assert brain.getFiringNeuronCount() == 0

print("All basic tests passed!")
```

## Examples and Tutorials

### 1. Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### 2. Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### 3. Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## Performance Considerations

### Configuration Optimization

```python
# For small-scale testing
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("brain.region_count", 1)

# For performance testing
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.region_count", 4)
```

### Memory Management

```python
# Brain should be reset between simulations
brain.reset()

# Check if reset worked
assert brain.getTotalNeuronCount() > 0

# Save and load for large simulations
brain.save("checkpoint.bin")
# ... later ...
brain2 = pynlm.createBrain(config)
brain2.initialize()
brain2.load("checkpoint.bin")
```

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

**"ImportError: No module named 'pynlm'"**
```bash
pip install --force-reinstall .
```

**"Segmentation faults"**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

### Debugging Tips

```python
import pynlm
import traceback

# Create brain with error handling
config = pynlm.createDefaultConfig()
try:
    brain = pynlm.createBrain(config)
    if brain.initialize():
        print("Brain created successfully")
        
        # Run with try-catch for simulation errors
        try:
            for i in range(100):
                brain.step(i)
                
            print("Simulation completed successfully")
            print(f"Final stats: {brain.getFiringNeuronCount()} firing neurons")
            
        except Exception as e:
            print(f"Simulation error: {e}")
            traceback.print_exc()
            
    else:
        print("Brain initialization failed")
        
except Exception as e:
    print(f"Brain creation error: {e}")
    traceback.print_exc()
```

## Advanced Features

### Custom Configuration

```python
# Create custom configuration
config = pynlm.Config()

# Set brain configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.connection_probability", 0.05)
config.set("brain.initial_weight_mean", 0.5)
config.set("brain.initial_weight_std", 0.1)
config.set("brain.v_thresh", -50.0)
config.set("brain.v_rest", -70.0)
config.set("brain.v_reset", -75.0)
config.set("brain.tau_mem", 20.0)
config.set("brain.tau_ref", 2.0)

# Set plasticity configuration
config.set("plasticity.stdp.enable", true)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)
config.set("plasticity.hebbian.enable", true)

# Set neuromodulation configuration
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", true)
config.set("neuromod.novelty.enable", true)

# Save configuration
config.saveToFile("custom_config.json")
```

### Development Features

```python
# Create brain with development enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable development in agent
agent = pynlm.createAgentBrain(brain)
agent.enableDevelopment(True)

# Update development over time
for timestep in [0.1, 0.2, 0.3, 0.4, 0.5]:
    agent.updateDevelopment(timestep)
    stage = agent.getDevelopmentalStage()
    print(f"Development stage: {stage}")
```

### Visualization (if available)

```python
# The visualization module can be used to visualize brain activity
# (Requires appropriate compilation flags)

from pynlm import VisualizationInterface

# Create visualization interface
viz = VisualizationInterface()
viz.initialize()

# Update visualization with brain state
viz.update(brain)
viz.render()
```

## Future Enhancements

The NLM project is actively developing and may include:

1. **Neural Architecture Search**: Automated discovery of optimal brain architectures
2. **Meta-Learning**: Learning how to learn across multiple tasks
3. **Transfer Learning**: Applying knowledge from one domain to another
4. **Hybrid Systems**: Integration with symbolic AI systems
5. **Neuromorphic Hardware**: Support for spiking neural network hardware platforms
6. **Cloud Computing**: Distributed brain simulation capabilities

## References

- NLM Documentation: https://nlm.readthedocs.io
- NLM GitHub: https://github.com/nlm-project/nlm
- Scientific Papers: See docs/SCIENCE.md

## Contact and Support

For questions, issues, or contributions:
- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Documentation: https://nlm.readthedocs.io
- Email: support@nlm-project.org

---

*This API Reference is continuously updated. For the latest information, visit the official NLM documentation site.*
