# NLM - Comprehensive API Documentation

## Overview

This document provides detailed documentation for the NLM (Neural Learning Machine) Python API. It covers all available classes, their methods, parameters, and usage examples.

## Available Classes

### 1. Configuration Class

The `Config` class manages configuration parameters for the NLM system. All configuration is stored as key-value pairs using dot notation for hierarchical access.

#### Key Methods

```python
# Create configuration
config = pynlm.Config()

# Basic configuration operations
config.has("brain.neuron_count")                    # Check if key exists
config.getKeys()                                    # Get all keys
config.clear()                                      # Clear all settings
config.saveToFile("config.json")                   # Save to JSON
config.loadFromFile("config.json")                 # Load from JSON
config.loadFromArgs(argc, argv)                    # Load from command line

# Get and set configuration values
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Get configuration values
if config.has("brain.neuron_count"):
    # Note: Config doesn't have get() method, use parameter checking
    pass

# Get summary
print(config.summary())
```

#### Configuration Categories

| Category | Key Pattern | Type | Default | Description |
|----------|-------------|------|---------|-------------|
| Brain | `brain.neuron_count` | int | 1000 | Total number of neurons |
| Brain | `brain.synapse_density` | float | 0.1 | Connectivity density |
| Brain | `brain.v_thresh` | float | -50.0 | Threshold potential |
| Plasticity | `plasticity.stdp.enable` | bool | true | STDP learning |
| Plasticity | `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| Neuromodulation | `neuromod.dopamine.scale` | float | 1.0 | Dopamine effect |
| Development | `development.critical_period` | float | 10.0 | Critical period duration |

### 2. Sensory Input Classes

NLM supports multiple sensory modalities:

#### Vision Class

```python
# Create vision input
vision = pynlm.Vision(width, height, channels=3)

# Set vision data
vision.setData([0.1, 0.2, 0.3, ...])  # Flattened array

# Get vision properties
width = vision.getWidth()
height = vision.getHeight()
channels = vision.getChannels()
dim = vision.getDimensions()

# Get raw data
data = vision.getData()
```

#### Audio Class

```python
# Create audio input
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)

# Set audio properties
audio.setSampleRate(48000)
sampleRate = audio.getSampleRate()
samples = audio.getNumSamples()

# Set audio data
audio.setData([0.0, 0.1, -0.2, ...])
```

#### InternalSignals Class

```python
# Create internal signals
internal = pynlm.InternalSignals()

# Add signals (representing internal states)
internal.addSignal(0.5)  # Some internal state
internal.addSignal(0.8)  # Another state

internal.clearSignals()  # Clear all signals
```

### 3. Action Classes

#### Action Class

```python
# Create actions
action = pynlm.Action()                          # Empty action
action = pynlm.Action(pynlm.ActionType.MoveForward)  # Predefined type
action = pynlm.Action(pynlm.ActionType.Custom, [1.0, 0.0, 0.5])  # Custom with parameters

# Get and set action properties
action_type = action.getType()
action.setType(pynlm.ActionType.TurnLeft)

parameters = action.getParameters()
action.setParameters([0.1, 0.9, 0.0])

name = action.getName()
action_clone = action.clone()  # Create copy
```

#### ActionType Enum

```python
# Available action types
action_type = pynlm.ActionType.MoveForward
action_type = pynlm.ActionType.TurnLeft
action_type = pynlm.ActionType.Interact
action_type = pynlm.ActionType.Eat
# ... and many more
```

### 4. World Object Classes

#### WorldObject Class

```python
# Create world objects
obj = pynlm.WorldObject(x=5.0, y=5.0, type=pynlm.WorldObjectType.Resource, value=1.0)

# Properties
obj.x = 5.0  # Position X
obj.y = 5.0  # Position Y
obj.radius = 0.5  # Size
obj.type = pynlm.WorldObjectType.Wall  # Type
obj.value = 1.0  # Value/resource amount
obj.active = True  # Is active

# Check if position is valid
is_valid = world.isValidPosition(10.0, 10.0)
```

#### WorldObjectType Enum

```python
# Available object types
obj_type = pynlm.WorldObjectType.Empty
obj_type = pynlm.WorldObjectType.Resource
obj_type = pynlm.WorldObjectType.Hazard
obj_type = pynlm.WorldObjectType.Wall
obj_type = pynlm.WorldObjectType.Marker
```

### 5. Agent Body Class

#### AgentBody Class

```python
# Agent body represents physical state
body = pynlm.AgentBody()

# Physical properties
body.x = 10.0
body.y = 10.0
body.orientation = 0.0  # Radians
body.velocityX = 0.5
body.velocityY = 0.0
body.angularVelocity = 0.1

# Agent state
body.energy = 100.0
body.health = 1.0
body.age = 0
body.isMoving = True
body.isTurning = False

# Update body state
body.lastActionTime = 0.0
body.reset()  # Reset to initial state
```

### 6. Action Result Class

#### ActionResult Class

```python
# Represents result of executing an action
result = pynlm.ActionResult(reward=1.0, success=True, message="Collected resource")

# Result properties
reward = result.reward
success = result.success  # Was action successful?
message = result.message  # Optional description
```

### 7. Sensory Percept Class

#### SensoryPercept Class

```python
# Combined sensory perception
percept = pynlm.SensoryPercept()

# Vision component
vision = percept.getVision()
percept.setVision(vision)

# Touch/Proprioception
proprioception = percept.getProprioception()
percept.setProprioception(proprioception)

# Internal signals
internal = percept.getInternal()
percept.setInternal(internal)

# Audio component
audio = percept.getAudio()
percept.setAudio(audio)

# Timestamp handling
time = percept.getTimestamp()
percept.setTimestamp(time)

# Get all signals
signals = percept.getAllSignals()
```

### 8. SimpleWorld Class

#### SimpleWorld Class

```python
# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control methods
world.reset()                           # Reset to initial state
world.update(timestep=0.1)              # Update world physics
world.setAgentStart(x=10.0, y=10.0)    # Set agent starting position

# Agent interaction
world.applyMotorCommand(action, time)   # Execute action

# Get sensory information
percept = world.getSensoryPercept()     # What agent perceives
body = world.getAgentBody()             # Agent physical state

# World objects
world.addObject(obj)                    # Add object to world
world.removeObject(x, y)                # Remove object at position
world.isValidPosition(x, y)             # Check if position is valid

# World properties
width = world.getWidth()
height = world.getHeight()
simTime = world.getSimulationTime()     # Current simulation time
maxEnergy = world.getMaxEnergy()
energyDecay = world.getEnergyDecayRate()
world.setMaxEnergy(200.0)
world.setEnergyDecayRate(0.1)

# Randomness control
world.setRandomSeed(42)                 # Reproducible simulation
seed = world.getRandomSeed()
```

### 9. Brain Class

#### Brain Class

```python
# Create brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()                        # Must be called before use

# Core simulation
brain.step(step_number)                   # Advance one step
brain.step(step_number, time)             # Advance with timestamp
brain.reset()                            # Reset brain state

# Sensory and motor processing
brain.receiveSensoryInput(percept)       # Inject sensory data
brain.injectCurrent(neuron_id, current)  # Inject current into specific neuron
brain.injectCurrentToNeurons(neuron_type, current)  # Inject into all neurons of type

brain.produceAction()                    # Generate motor output

# State management
brain.save("checkpoint.bin")              # Save to file
brain.load("checkpoint.bin")              # Load from file

# Region management
region_id = brain.addRegion("cortex")    # Add new region
region = brain.getRegion(region_id)      # Get region by ID
region_count = brain.getRegionCount()
region_ids = brain.getRegionIds()

# Statistics and monitoring
total_neurons = brain.getTotalNeuronCount()
total_synapses = brain.getTotalSynapseCount()
active_neurons = brain.getActiveNeuronCount()
firing_neurons = brain.getFiringNeuronCount()
avg_firing_rate = brain.getAverageFiringRate()
ei_ratio = brain.getExcitationInhibitionRatio()
total_spikes = brain.getTotalSpikeCount()

# Development
stage = brain.getDevelopmentalStage()    # Get current stage
brain.setDevelopmentalStage(stage)      # Set stage

# Configuration
brain_config = brain.getConfig()         # Get configuration
brain.logStatus()                        # Log brain status
```

#### Brain Configuration

```python
# Configure brain parameters before creation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)           # More neurons = more complex
config.set("brain.synapse_density", 0.15)       # More connections
config.set("brain.v_thresh", -45.0)            # Higher threshold
config.set("plasticity.stdp.learning_rate", 0.01)  # Faster learning
config.set("neuromod.dopamine.scale", 1.5)     # Stronger reward learning
config.set("development.critical_period", 20.0) # Longer critical period

brain = pynlm.createBrain(config)
brain.initialize()
```

### 10. AgentBrain Class

#### AgentBrain Class

```python
# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)       # Feed percept to brain

# Motor decoding
action = agent.decodeMotorCommand()     # Get action from brain

# Learning and development
agent.applyRewardModulation(reward, predicted_reward)  # Learn from rewards
agent.updateDevelopment(timestep)       # Update development

# Development stage
stage = agent.getDevelopmentalStage()   # Get stage

# Neuromodulation levels
curiosity = agent.getCuriosityLevel()    # Exploration drive
novelty = agent.getNoveltyLevel()        # Response to novelty
prediction_error = agent.getPredictionError()  # Learning signal
mod_level = agent.getNeuromodulationLevel()    # Overall neuromodulation

# Subsystem control
agent.enableRewardModulation(True)       # Enable reward learning
agent.enableStructuralPlasticity(True)   # Enable connection growth
agent.enableDevelopment(True)            # Enable maturation
agent.enableCuriosity(True)              # Enable exploration

# Check subsystem status
reward_enabled = agent.isRewardModulationEnabled()
plasticity_enabled = agent.isStructuralPlasticityEnabled()
development_enabled = agent.isDevelopmentEnabled()
curiosity_enabled = agent.isCuriosityEnabled()

# Reset agent
agent.reset()                            # Reset for new episode

# Access underlying brain
brain = agent.getBrain()                 # Get the brain object
```

## Enumerations

### Neuron Types

```python
# Neuron classification
neuron_type = pynlm.NeuronType.Excitatory    # Increases firing probability
neuron_type = pynlm.NeuronType.Inhibitory    # Decreases firing probability
neuron_type = pynlm.NeuronType.Sensory       # Receives external input
neuron_type = pynlm.NeuronType.Motor         # Produces motor output
neuron_type = pynlm.NeuronType.Modulatory     # Releases neuromodulators
neuron_type = pynlm.NeuronType.Internal       # Internal brain communication
```

### Synapse Types

```python
# Connection types
synapse_type = pynlm.SynapseType.Excitatory    # Positive current flow
synapse_type = pynlm.SynapseType.Inhibitory    # Negative current flow
synapse_type = pynlm.SynapseType.Modulatory    # Neuromodulatory signals
synapse_type = pynlm.SynapseType.Electrical    # Direct electrical connection
synapse_type = pynlm.SynapseType.GapJunction  # Electrical synapse
```

### Developmental Stages

```python
# Brain development stages
stage = pynlm.DevelopmentalStage.Initial      # Initial state
stage = pynlm.DevelopmentalStage.CriticalPeriod  # High plasticity period
stage = pynlm.DevelopmentalStage.Maturation   # Physical growth
stage = pynlm.DevelopmentalStage.Adult       # Full maturity
stage = pynlm.DevelopmentalStage.Aging       # Decline phase
```

## System Accessors

### 8. Prediction System

```python
# Access to prediction system for sensory prediction
# The prediction system predicts future sensory states and calculates prediction errors

prediction = brain.getPredictionSystem()

# Make prediction for next state based on current sensory input
predicted = prediction.predictNextState(percept)

# Update predictions based on actual observation
prediction.updatePredictions(predicted, percept)

# Get prediction error for learning
error = prediction.getPredictionError()

# Get prediction confidence
confidence = prediction.getConfidence()

# Get prediction error history
error_history = prediction.getErrorHistory()

# Train prediction model with new observations
prediction.train(percept)
```

**Integration with Sensory Processing:**

The prediction system is automatically integrated with sensory input processing in the Brain class. When `receiveSensoryInput()` is called:

1. **Prediction Making**: The system makes predictions about the next sensory state based on current input
2. **Error Calculation**: Prediction errors are calculated and used to influence neuromodulation
3. **Learning**: The prediction model is trained with actual sensory observations
4. **Attention Modulation**: Prediction errors influence attentional selection

**Key Benefits:**

- **Faster Adaptation**: Predicts upcoming sensory states for proactive processing
- **Novelty Detection**: Higher prediction errors signal novel or unexpected input
- **Efficient Learning**: Uses prediction errors for supervised learning
- **Attention Guidance**: Directs attention to unexpected sensory events

**Usage in Agent Simulations:**

```python
# Complete agent with prediction system integration
def run_prediction_driven_agent():
    # Initialize components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable subsystems for prediction integration
    agent.enablePrediction(True)
    agent.enableCuriosity(True)
    agent.enableAttention(True)
    
    print("Prediction-driven agent initialized")
    print(f"Prediction system available: {brain.getPredictionSystem() is not None}")
    
    # Run simulation
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input (this automatically updates prediction system)
        agent.processSensoryInput(percept)
        
        # Access prediction system
        prediction = brain.getPredictionSystem()
        if prediction:
            # Get prediction error for monitoring
            error = prediction.getPredictionError()
            confidence = prediction.getConfidence()
            
            print(f"  Step {step}: Prediction error={error:.3f}, confidence={confidence:.3f}")
        
        # Continue with normal brain processing
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from prediction errors
        agent.applyRewardModulation(percept.getInternal()[0] if percept.getInternal() else 0.0, 0.0)
    
    return brain, world, agent

# Run the example
brain, world, agent = run_prediction_driven_agent()
```

**Prediction System Configuration:**

The prediction system can be configured through the brain configuration:

```python
config = pynlm.createDefaultConfig()

# Prediction system configuration
config.set("prediction.enabled", True)
config.set("prediction.horizon", 5)              # Predict 5 steps ahead
config.set("prediction.learning_rate", 0.01)     # Learning rate for predictions
config.set("prediction.error_threshold", 0.1)   # Threshold for significant prediction errors
config.set("prediction.noise_level", 0.01)       # Amount of noise in predictions

# Integration with other systems
config.set("prediction.integrate_with_attention", True)
config.set("prediction.integrate_with_curiosity", True)
config.set("prediction.integrate_with_memory", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Action Types

```python
# Available actions
action = pynlm.ActionType.MoveForward
action = pynlm.ActionType.MoveBackward
action = pynlm.ActionType.TurnLeft
action = pynlm.ActionType.TurnRight
action = pynlm.ActionType.Interact
action = pynlm.ActionType.Eat
action = pynlm.ActionType.Drink
action = pynlm.ActionType.Rest
# ... and many more
```

### Motor Commands

```python
# Low-level motor outputs
motor = pynlm.MotorCommand.MoveForward
motor = pynlm.MotorCommand.MoveBackward
motor = pynlm.MotorCommand.TurnLeft
motor = pynlm.MotorCommand.TurnRight
motor = pynlm.MotorCommand.Interact
motor = pynlm.MotorCommand.Wait
```

### World Object Types

```python
# Object classifications
obj_type = pynlm.WorldObjectType.Empty
obj_type = pynlm.WorldObjectType.Resource  # Valuable item
obj_type = pynlm.WorldObjectType.Hazard    # Dangerous item
obj_type = pynlm.WorldObjectType.Wall      # Impassable barrier
obj_type = pynlm.WorldObjectType.Marker    # Special location
```

## Factory Functions

### Default Configuration

```python
# Create default configuration
config = pynlm.createDefaultConfig()
```

### Object Creation

```python
# Create brain with default config
brain = pynlm.createBrain()

# Create brain with custom config
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
brain = pynlm.createBrain(config)

# Create world
world = pynlm.createSimpleWorld()

# Create agent interface
agent = pynlm.createAgentBrain(brain)
```

### Special Values

```python
# Invalid IDs for initialization/in validation
neuron_id = pynlm.INVALID_NEURON_ID
synapse_id = pynlm.INVALID_SYNAPSE_ID
region_id = pynlm.INVALID_REGION_ID
population_id = pynlm.INVALID_POPULATION_ID
```

## Best Practices

### 1. Configuration Management

```python
# Always create configuration before brain
config = pynlm.createDefaultConfig()

# Set parameters appropriately for your use case
config.set("brain.neuron_count", 1000)  # Start with 1000 neurons
config.set("brain.synapse_density", 0.1)  # Moderate connectivity

# Save configurations for reproducibility
config.saveToFile("my_config.json")
```

### 2. Initialization Pattern

```python
# Proper initialization order
def setup_simulation():
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create world
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    # 4. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    return brain, world, agent
```

### 3. Simulation Loop Pattern

```python
# Typical simulation pattern
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world (environment changes)
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # 4. Get action
        action = agent.decodeMotorCommand()
        
        # 5. Apply to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 6. Learn from experience
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # 7. Update development
        agent.updateDevelopment(0.1)
        
        # 8. Optional: Save checkpoint periodically
        if step % 1000 == 0:
            brain.save(f"checkpoint_step_{step}.bin")
```

### 4. Error Handling

```python
# Always handle potential errors
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
except Exception as e:
    print(f"Simulation error: {e}")
    # Attempt recovery or graceful shutdown
    brain.reset() if 'brain' in locals() else None
```

## Performance Tips

### 1. Memory Management

```python
# Reuse objects when possible
brain = pynlm.createBrain(config)
brain.initialize()

# Reset instead of recreating for multiple runs
brain.reset()
```

### 2. Configuration Optimization

```python
# Pre-compile configurations for different use cases
fast_config = pynlm.createDefaultConfig()
fast_config.set("brain.neuron_count", 500)  # Fewer neurons = faster
fast_config.set("brain.synapse_density", 0.05)  # Less connectivity

complex_config = pynlm.createDefaultConfig()
complex_config.set("brain.neuron_count", 5000)  # More neurons = more complex
complex_config.set("brain.synapse_density", 0.2)  # More connectivity
```

### 3. Monitoring and Debugging

```python
# Monitor simulation progress
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Firing: {brain.getFiringNeuronCount()}")
        print(f"  Spikes: {brain.getTotalSpikeCount()}")
        print(f"  Development: {brain.getDevelopmentalStage()}")
        
    # Log status periodically
    if step % 500 == 0:
        brain.logStatus()
```

## Migration Guide

### From C++ to Python

If you're migrating from C++:

1. **Brain Creation**: C++ `Brain(config)` → Python `pynlm.createBrain(config)`
2. **Initialization**: C++ `brain.initialize()` → Python `brain.initialize()`
3. **Simulation**: C++ `brain.step(step)` → Python `brain.step(step)`
4. **Configuration**: C++ config object → Python `pynlm.createDefaultConfig()`

### From MATLAB to Python

If you're migrating from MATLAB:

1. **Matrix operations**: Use NumPy arrays
2. **Sensory data**: `Vision`, `Audio`, `InternalSignals` classes
3. **World simulation**: `SimpleWorld` class with `update()` method
4. **Actions**: `Action` class with type and parameters

## Changelog

### Version 1.0 (Current)
- Initial release with all core classes
- Comprehensive API documentation
- Examples for common patterns

### Future Versions
- Enhanced error handling
- Performance monitoring tools
- Advanced visualization capabilities
- Multi-agent support

## Contact and Support

For issues or questions:
1. Check the documentation
2. Review examples in `easy_usage.md`
3. Look at `HOW_TO_USE.md` for detailed instructions
4. Consult the source code in `python/bindings.cpp`

## Legal and License

This software is part of the NLM project. All rights reserved.