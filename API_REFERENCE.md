# NLM API Reference

## Overview

This document provides comprehensive documentation for the NLM Python API (`pynlm`). It covers all available classes, functions, enumerations, and usage patterns.

## Module Information

- **Module**: `pynlm`
- **Version**: 0.1.0
- **Author**: NLM Authors
- **Description**: Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework

## Important Notes

### Design Philosophy

NLM is **not** a machine learning framework. It is a **brain simulator** that:

- Implements its own neural computation from scratch (no ML frameworks)
- Models biological brain processes (spiking neurons, synaptic plasticity)
- Provides a research platform for studying learning and cognition
- Focuses on understanding how brains work, not on solving AI tasks

### Key Differences from ML Frameworks

- **No pretrained models**: All learning is from experience
- **No training APIs**: Plasticity is built into neural dynamics
- **Spiking neurons**: Not traditional artificial neural networks
- **Biological plausibility**: Approximate models of brain processes

## Core Types

### NeuronId

```python
# Neuron identifier
neuron_id = NeuronId()  # Default constructor
neuron_id = NeuronId(42)  # With specific value

print(neuron_id.value)  # Get the numeric value
print(neuron_id.index())  # Get the index

# Equality and hashing
id1 == id2  # Equality comparison
id1 != id2  # Inequality comparison
hash(id1)   # Hash for dictionary keys
repr(id1)   # String representation: "<NeuronId: 42>"
```

### SynapseId

Similar to NeuronId but for synapse identifiers.

### RegionId

Identifies brain regions for anatomical organization.

### PopulationId

Identifies neuron populations with shared properties.

## Enumerations

### NeuronType

```python
# Types of neurons in the brain
NeuronType.Excitatory      # Increases firing probability of other neurons
NeuronType.Inhibitory      # Decreases firing probability of other neurons
NeuronType.Modulatory       # Modulates other neurons' activity
NeuronType.Sensory          # Processes sensory input
NeuronType.Motor            # Controls actions
NeuronType.Internal         # Internal brain computations
```

### SynapseType

```python
# Types of synaptic connections
SynapseType.Excitatory      # Increases postsynaptic neuron firing
SynapseType.Inhibitory      # Decreases postsynaptic neuron firing
SynapseType.Modulatory       # Modulates postsynaptic activity
SynapseType.Electrical      # Direct electrical coupling
SynapseType.GapJunction     # Specialized electrical synapse
```

### DevelopmentalStage

```python
# Brain development stages
DevelopmentalStage.Initial           # Initial state (0 neurons)
DevelopmentalStage.CriticalPeriod   # High plasticity phase
DevelopmentalStage.Maturation        # Maturation toward adult properties
DevelopmentalStage.Adult             # Stable adult brain
DevelopmentalStage.Aging            # Age-related decline
```

### FiringState

```python
# Current firing state of a neuron
FiringState.Resting    # Not currently firing
FiringState.Active     # Currently firing action potential
FiringState.Refractory # Cannot fire due to recovery period
FiringState.Inhibited  # Actively prevented from firing
```

### ActionType

```python
# Types of actions an agent can take
ActionType.MoveForward    # Move forward
ActionType.MoveBackward   # Move backward
ActionType.MoveLeft       # Move left
ActionType.MoveRight      # Move right
ActionType.TurnLeft       # Turn left
ActionType.TurnRight      # Turn right
ActionType.Look           # Look forward
ActionType.LookUp         # Look up
ActionType.LookDown       # Look down
ActionType.Interact      # Interact with object
ActionType.Eat           # Eat resource
ActionType.Drink          # Drink resource
ActionType.Rest           # Rest/stay still
ActionType.Wait          # Wait without action
ActionType.Custom         # Custom action
```

### MotorCommand

```python
# Low-level motor commands
MotorCommand.MoveForward  # Move forward
MotorCommand.MoveBackward # Move backward
MotorCommand.TurnLeft     # Turn left
MotorCommand.TurnRight    # Turn right
MotorCommand.LookLeft     # Look left
MotorCommand.LookRight    # Look right
MotorCommand.Interact     # Interact with object
MotorCommand.Wait        # No action
```

### WorldObjectType

```python
# Types of objects in the world
WorldObjectType.Empty      # Empty space
WorldObjectType.Resource   # Consumable resource
WorldObjectType.Hazard     # Dangerous object
WorldObjectType.Wall       # Impassable barrier
WorldObjectType.Marker     # Special location/symbol
```

## Core Classes

### Config

Configuration management for the brain.

#### Creation and Initialization

```python
# Create empty configuration
config = Config()

# Create with default settings
config = Config.createDefaultConfig()

# Load from file
config.loadFromFile("brain_config.json")

# Load from command line
config.loadFromArgs(len(sys.argv), sys.argv)

# Check for configuration keys
if config.has("brain.neuron_count"):
    keys = config.getKeys()
```

#### Configuration Keys

| Key | Type | Default | Description |
|-----|------|---------|-------------|
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

#### Methods

```python
# Get configuration summary
print(config.summary())

# Modify configuration
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Save configuration
config.saveToFile("config.json")

# Clear all settings
config.clear()
```

### SensoryInput (Base Class)

Base class for all sensory inputs.

#### Attributes

- `data`: Raw sensory data vector
- `timestamp`: Time when data was received
- `dimensions`: Number of sensory dimensions

#### Common Methods

```python
# Get sensory properties
sensor_type = sensory_input.getType()
data = sensory_input.getData()
dim = sensory_input.getDimensions()
timestamp = sensory_input.getTimestamp()

# Set timestamp
sensory_input.setTimestamp(time)
```

### Vision

Visual sensory input (grayscale or RGB images).

#### Creation

```python
# Empty vision input
vision = Vision()

# RGB image (3 channels)
vision = Vision(width=64, height=48, channels=3)

# Set visual data
vision.setData([0.1, 0.5, 0.9, ...])  # Flattened RGB values
```

#### Attributes

- `width`: Image width in pixels
- `height`: Image height in pixels
- `channels`: Number of color channels (1=grayscale, 3=RGB)

### Audio

Audio sensory input.

#### Creation

```python
audio = Audio(sampleRate=44100, numSamples=1024)

# Set audio data
audio.setData([0.1, -0.2, 0.3, ...])
```

#### Attributes

- `sampleRate`: Samples per second
- `numSamples`: Number of samples

### Touch

Touch/proximity sensor input.

#### Creation

```python
touch = Touch(numSensors=10)

# Set proximity values
touch.setData([0.0, 0.1, 0.2, ...])
```

#### Attributes

- `numSensors`: Number of touch sensors
- `data`: Array of sensor readings (0.0 to 1.0)

### Proprioception

Body position and velocity sensing.

#### Creation

```python
proprio = Proprioception()

# Set body state
proprio.setBodyState(x=5.0, y=10.0, orientation=0.5,
                    velocityX=0.1, velocityY=0.0,
                    angularVelocity=0.05)
```

#### Attributes

- `x`, `y`: Position coordinates
- `orientation`: Rotation angle
- `velocityX`, `velocityY`: Linear velocities
- `angularVelocity`: Angular velocity

### NeuralWorkingMemory

Persistent activity for temporary information storage.

#### Creation and Initialization

```python
memory = NeuralWorkingMemory()
memory.initialize(brain)  # Reference to brain
```

#### Core Methods

```python
# Store information
memory.store(pattern=[0.5, 0.8, 0.3], strength=1.0)

# Retrieve as activation levels
activations = memory.retrieve()

# Check if specific neuron is stored
if memory.contains(neuron_id):
    activation = memory.getNeuronActivation(neuron_id)

# Update (maintenance and decay)
memory.update(dt=0.1)
```

#### Configuration

```python
# Capacity limits memory traces
memory.setCapacity(100)
memory.getCapacity()  # Returns current capacity

# Decay rate controls how quickly memories fade
memory.setDecayRate(0.01)
memory.getDecayRate()

# Active traces
count = memory.getActiveTraces()
level = memory.getMemoryActivity()
```

### AttentionalSelection

Competitive selection of sensory inputs.

#### Creation and Initialization

```python
attention = AttentionalSelection()
attention.initialize(brain)
```

#### Core Methods

```python
# Process competing inputs and select winners
winners = attention.processCompetition(competitors, global_inhibition=0.5)

# Focus attention on specific region
attention.focusOnRegion(region_id)

# Release attention (allow all equal processing)
attention.releaseAttention()

# Update attention dynamics
attention.update(dt=0.1)
```

#### Selection Attributes

```python
# Attention parameters
attention.setInhibitionStrength(0.5)
attention.setExcitationStrength(1.0)
attention.setCompetitionThreshold(0.8)

# Check what is attended
if attention.isAttended(neuron_id):
    inhibition = attention.getInhibitionFor(neuron_id)
    excitation = attention.getExcitationFor(neuron_id)

# Get current focus
attended_regions = attention.getAttendedRegions()
```

### SimpleWorld

2D simulation world for agent interaction.

#### Creation and Configuration

```python
world = SimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
```

#### Control Methods

```python
# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(x=10.0, y=10.0)

# Update world physics
world.update(timestep=0.1)
```

#### State Access

```python
# Get what the agent sees (sensory percept)
percept = world.getSensoryPercept()

# Get agent's physical state
body = world.getAgentBody()

# Place or remove objects
world.addObject(WorldObjectType.Resource, x=5, y=5, value=1.0)
world.removeObject(x=5, y=5)
```

#### Simulation Properties

```python
time = world.getSimulationTime()
width = world.getWidth()
height = world.getHeight()
energy_decay = world.getEnergyDecayRate()
max_energy = world.getMaxEnergy()
```

#### World Control

```python
# Control simulation
world.setRandomSeed(42)
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.01)
```

### AgentBrain

Bridge connecting brain to world sensory/motor interfaces.

#### Creation

```python
agent = createAgentBrain(brain)
```

#### Initialization

```python
agent.initialize(world)
```

#### Core Methods

```python
# Process sensory input from world
agent.processSensoryInput(percept)

# Get motor command for action
motor_cmd = agent.decodeMotorCommand()

# Apply reward signals for learning
agent.applyRewardModulation(reward, predicted_reward)

# Update developmental stage
agent.updateDevelopment(timestep)
```

#### Subsystem Control

```python
# Enable/disable learning subsystems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

#### Subsystem Statistics

```python
curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()
error = agent.getPredictionError()
neuromodulation = agent.getNeuromodulationLevel()
```

### Brain

Central neural simulation brain with complete functionality.

#### Creation

```python
config = createDefaultConfig()
brain = createBrain(config)
```

#### Initialization

```python
brain.initialize()
```

#### Core Methods

```python
# Run simulation step
brain.step(current_step)
brain.step(current_step, current_time)

# Input/output
brain.receiveSensoryInput(sensory_input)
action = brain.produceAction()

# State management
brain.reset()
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")
```

#### Statistics

```python
neurons = brain.getTotalNeuronCount()
synapses = brain.getTotalSynapseCount()
active = brain.getActiveNeuronCount()
firing = brain.getFiringNeuronCount()
rate = brain.getAverageFiringRate()
spikes = brain.getTotalSpikeCount()
e_ratio = brain.getExcitationInhibitionRatio()
```

#### Access to Subsystems

```python
# Memory systems
wm = brain.getWorkingMemory()
em = brain.getEpisodicMemory()
am = brain.getAssociativeMemory()

# Neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
pred_error = brain.getPredictionErrorSignal()

# Development system
dev = brain.getDevelopmentSystem()

# Cognitive systems
attn = brain.getAttention()
planner = brain.getPredictionSystem()
concept = brain.getConceptFormation()

# Sensory systems
vision_sys = brain.getVisionSystem()
audio_sys = brain.getAudioSystem()
internal_sys = brain.getInternalSignals()
```

#### Development Stage

```python
stage = brain.getDevelopmentalStage()
stage_name = brain.getDevelopmentalStage().name  # "Initial", "CriticalPeriod", etc.
```

#### Region Management

```python
# Create brain regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)

# Get all regions
regions = brain.getRegions()
count = brain.getRegionCount()
ids = brain.getRegionIds()
```

### Dopamine

Reward and reinforcement learning signal.

```python
dopamine = Dopamine()
dopamine.signalReward(reward_value)           # Signal reward
dopamine.signalRewardPredictionError(error)   # Signal prediction error
```

### Curiosity

Exploration motivation.

```python
curiosity = Curiosity()
curiosity.initialize(brain)

# Update based on novelty and prediction error
curiosity.update(novelty, prediction_error, dt)

level = curiosity.getLevel()
drive = curiosity.getExplorationDrive()
```

### Novelty

Novelty detection signal.

```python
novelty = Novelty()
novelty.initialize(brain)

# Detect novelty from input patterns
novelty.detectNovelty(current_pattern, previous_pattern)

level = novelty.getLevel()
history = novelty.getHistory()
```

### PredictionError

Prediction error signal.

```python
pred_error = PredictionError()
pred_error.initialize(brain)

# Compute error between prediction and reality
error = pred_error.computeError(predicted, actual)

error_val = pred_error.getError()
magnitude = pred_error.getMagnitude()
history = pred_error.getHistory()
```

### DevelopmentSystem

Orchestrates neural development.

```python
dev = DevelopmentSystem()

stage = dev.getStage()
dev.setStage(DevelopmentalStage.CriticalPeriod)
dev.advanceStage()

name = dev.getStageName()
age = dev.getDevelopmentalStage()
dev.setDevelopmentalStage(age)

# Update developmental processes
dev.update(brain, current_step)
dev.update(brain, rng, dt)
```

## Module-Level Functions

### Factory Functions

```python
# Brain creation
brain = createBrain(config)
config = createDefaultConfig()

# World creation
world = createSimpleWorld()

# Agent creation
agent = createAgentBrain(brain)
```

### Advanced Functions

#### Session Management

```python
# Save brain state with metadata
metadata = saveBrainSession("checkpoint.bin", "my_session", brain, include_config=True)

# Load brain session
brain, metadata = loadBrainSession("checkpoint.bin")

# List saved sessions
sessions = listBrainSessions("/path/to/sessions")
```

#### Performance Profiling

```python
# Profile brain performance
metrics = profileBrain(brain, num_steps=1000, include_memory=True, include_plasticity=True)

# Export detailed report
exportPerformanceReport(brain, "report.txt", benchmark_steps=100)
```

#### Network Analysis

```python
# Compute comprehensive network metrics
metrics = computeNetworkMetrics(brain)

# Analyze brain topographic organization
topography = analyzeBrainTopography(brain, regions_of_interest={"Region_1": 0.8})
```

#### Experiment Creation

```python
# Create learning experiment configuration
experiment = createLearningExperiment(
    name="TemporalPrediction",
    brain_template=brain,
    environment_config={"reward_decay": 0.9},
    max_steps=500,
    metrics_config={"spike_rate": True, "memory_retention": True}
)

# Run learning experiment
results = runLearningExperiment(brain, environment_config, num_episodes=10, max_steps_per_episode=100)
```

## Error Handling

NLM raises standard Python exceptions:

```python
try:
    brain.initialize()
    for step in range(1000):
        brain.step(step)
except Exception as e:
    print(f"Error: {e}")
```

Common exceptions:
- `RuntimeError`: General runtime errors
- `InvalidArgument`: Invalid function arguments
- `OutOfRange`: Array index out of bounds
- `LengthError`: Invalid length specifications
- `LogicError`: Logical errors in computation
- `DomainError`: Domain-specific validation errors
- `OverflowError`: Numerical overflow

## Thread Safety

NLM objects are **not thread-safe**. Concurrent access to the same brain or world from multiple threads will produce undefined behavior.

## Memory Management

NLM uses reference counting for Python objects. No explicit memory management is required.

## Configuration File Format

NLM configuration can be saved/loaded as JSON:

```json
{
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1,
    "v_thresh": -50.0
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001
    }
  }
}
```

## Common Patterns

### 1. Basic Learning Loop

```python
def run_learning_episode(config, world, num_steps):
    """Complete learning episode with all subsystems."""
    # Setup
    brain = createBrain(config)
    brain.initialize()
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Learning loop
    for step in range(num_steps):
        world.update(0.1)
        
        # Sense
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Think
        brain.step(step)
        
        # Act
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    return brain
```

### 2. Performance Monitoring

```python
def run_monitored_simulation(brain, num_steps):
    """Run simulation with performance monitoring."""
    import time
    
    metrics = {
        "spike_count": [],
        "firing_rate": [],
        "step_time": [],
        "memory_usage": []
    }
    
    brain.initialize()
    
    for step in range(num_steps):
        start_time = time.time()
        
        brain.step(step)
        
        end_time = time.time()
        metrics["step_time"].append(end_time - start_time)
        metrics["spike_count"].append(brain.getTotalSpikeCount())
        metrics["firing_rate"].append(brain.getAverageFiringRate())
        
        if step % 100 == 0:
            wm = brain.getWorkingMemory()
            if wm:
                metrics["memory_usage"].append({
                    "capacity": wm.getCapacity(),
                    "active": wm.getActiveTraces(),
                    "activity": wm.getMemoryActivity()
                })
    
    return metrics
```

### 3. Experimental Protocol

```python
def run_temporal_prediction_experiment(config, trials=10):
    """Test temporal prediction learning."""
    import random
    
    results = []
    
    for trial in range(trials):
        brain = createBrain(config)
        brain.initialize()
        
        # Create temporal sequences
        pattern_a = [random.random() for _ in range(10)]
        pattern_b = [random.random() for _ in range(10)]
        
        # Present A followed by B repeatedly
        for epoch in range(50):
            for p in [pattern_a, pattern_b]:
                percept = Vision()
                percept.setData(p)
                brain.receiveSensoryInput(percept)
                brain.step(epoch * 2 + (0 if p == pattern_a else 1))
        
        # Test prediction
        prediction = Novelty()
        prediction.initialize(brain)
        test_error = prediction.computeError(pattern_a, pattern_b)
        
        results.append({
            "trial": trial,
            "final_error": test_error,
            "improvement": 1.0 - test_error  # Simplified
        })
    
    return results
```

## Troubleshooting

### Common Issues

#### "Brain initialization failed"
- Ensure `brain.initialize()` is called before `brain.step()`
- Check that configuration is valid

#### "No neurons firing"
- Increase simulation steps for warmup
- Add sensory input to stimulate neurons
- Check that neurons have correct properties

#### "Memory not updating"
- Ensure working memory is initialized with brain reference
- Check memory capacity settings
- Verify decay rate is reasonable

#### "Performance too slow"
- Reduce number of neurons in configuration
- Use smaller world dimensions
- Profile with `profileBrain()` to identify bottlenecks

#### "Incorrect actions"
- Check that motor commands are being decoded
- Verify action mapping in world
- Ensure agent's motor system is properly connected

### Debug Tips

1. **Add logging**: Use `brain.logStatus()` to see brain state
2. **Check statistics**: Monitor `getFiringNeuronCount()`, `getAverageFiringRate()`
3. **Visual inspection**: Use visualization tools if available
4. **Incremental testing**: Start with simple patterns and build complexity
5. **Save checkpoints**: Use `brain.save()` to debug specific states

## Further Reading

For deeper understanding of NLM architecture and scientific background:

- `docs/ARCHITECTURE.md` - System architecture overview
- `docs/SCIENCE.md` - Scientific background and limitations
- `docs/EXPERIMENTS.md` - Experimental framework and results

## Version History

- Version 0.1.0: Initial release
- Added core brain, world, and agent classes
- Implemented basic learning (STDP, Hebbian)
- Added neuromodulation systems (dopamine, curiosity)
- Implemented developmental learning
- Added working memory and attention

## License

MIT License. See LICENSE file for details.