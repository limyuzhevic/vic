# NLM Python Bindings - Comprehensive API Documentation

## Overview

This document provides comprehensive documentation for the NLM (熙然) Python bindings, an experimental artificial developmental brain simulation framework. The Python bindings expose the full functionality of the C++ NLM system, enabling Python developers to create, simulate, and experiment with neural brain architectures.

### Key Features

- **Event-driven spiking neural networks**: Real LIF neuron dynamics with millisecond precision
- **Integrated brain systems**: Memory, prediction, cognition, neuromodulation, and development
- **Developmental learning**: Emergent capabilities through developmental stages
- **Advanced exploration**: Curiosity-driven exploration and novelty detection
- **Action parameterization**: Precise motor control with continuous action parameters
- **Vision processing**: Enhanced computer vision capabilities with feature extraction
- **Checkpoint system**: Save and load brain states for reproducible experiments
- **Comprehensive error handling**: Robust error messages and validation
- **Performance monitoring**: Built-in profiling and optimization tools

### Architecture Overview

```
WORLD
  ↓
SENSORY INPUT (Vision, Audio, Internal, Proprioception)
  ↓
AGENT BRAIN (Sensory transduction, Motor decoding)
  ↓
BRAIN (LIF dynamics, Memory, Prediction, Cognition)
  ↓
NEURAL PROCESSING (Spikes, Plasticity, Development)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / ERROR
  ↓
NEUROMODULATION (Dopamine, Curiosity, Novelty)
  ↓
PLASTICITY (STDP, Hebbian, Reward-modulated)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Quick Start Guide

### Installation

```bash
# Install the NLM Python package
pip install pynlm

# Or build from source (if you have the full NLM repository)
# cd /path/to/nlm
# python setup.py build_ext --inplace
```

### Basic Usage Example

```python
import pynlm

# Create a default brain configuration
config = pynlm.createDefaultConfig()

# Create a new brain with the configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Create a world and agent
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation loop
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation complete. Firing neurons: {brain.getFiringNeuronCount()}")
```

### Advanced Example with Learning

```python
import pynlm

# Create a custom configuration for learning
config = pynlm.createDefaultConfig()

# Configure for episodic memory and development
config.set("neuron_count", 500, pynlm.ConfigSource.Runtime)
config.set("region_count", 2, pynlm.ConfigSource.Runtime)
config.set("replay_interval", 50, pynlm.ConfigSource.Runtime)
config.set("consolidation_interval", 500, pynlm.ConfigSource.Runtime)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with enhanced capabilities
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableSocialLearning(True)

# Run training episodes
total_reward = 0.0
for episode in range(20):
    episode_reward = 0.0
    
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Get action result and apply learning
        result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        episode_reward += result.reward
    
    agent.reset()
    world.reset()
    total_reward += episode_reward

print(f"Training complete. Average reward: {total_reward/20}")
print(f"Final curiosity level: {agent.getCuriosityLevel()}")
print(f"Final developmental stage: {agent.getDevelopmentalStage()}")
```

## API Reference

### Core Classes

#### Config
Configuration management for NLM systems.

**Methods:**
- `createDefaultConfig()`: Create optimized default configuration
- `loadFromFile(filepath)`: Load configuration from JSON file
- `saveToFile(filepath)`: Save configuration to JSON file
- `has(key)`: Check if configuration key exists
- `get(key)`: Get configuration value with type safety
- `set(key, value)`: Set configuration value
- `getKeys()`: Get all configuration keys
- `validateAll()`: Validate all configuration values
- `summary()`: Get configuration summary with validation status

**Common Configuration Keys:**
- `neuron_count`: Total number of neurons (default: 1000)
- `region_count`: Number of brain regions (default: 1)
- `connection_probability`: Initial connection probability (default: 0.1)
- `stdp_ltp_weight`: STDP long-term potentiation weight
- `stdp_ltd_weight`: STDP long-term depression weight
- `stdp_tau`: STDP time constant
- `synaptogenesis_rate`: New synapse formation rate
- `pruning_rate`: Synapse removal rate
- `simulation_timestep`: Integration timestep (default: 0.001)
- `replay_interval`: Memory replay interval (default: 100)
- `consolidation_interval`: Memory consolidation interval (default: 1000)
- `random_seed`: Random number generator seed

#### Action
Action representation for motor output with parameter support.

**Properties:**
- `type`: Action type (enum)
- `parameters`: List of action parameters (continuous values)

**Methods:**
- `getType()`: Get action type
- `getParameters()`: Get parameter list
- `setType(type)`: Set action type
- `setParameters(params)`: Set parameter list
- `getParameter(index)`: Get specific parameter by index
- `addParameter(param)`: Add a parameter
- `getName()`: Get action name as string
- `clone()`: Create action copy
- `__repr__()`: String representation

**Action Types and Parameters:**
- **MoveForward**: [speed] (0.0-1.0)
- **MoveBackward**: [speed] (0.0-1.0)
- **MoveLeft**: [speed] (0.0-1.0)
- **MoveRight**: [speed] (0.0-1.0)
- **TurnLeft**: [angular_speed] (0.0-6.28 radians/sec)
- **TurnRight**: [angular_speed] (0.0-6.28 radians/sec)
- **Look**: [direction] (0.0-6.28 radians)
- **LookUp**: [angle] (-1.57 to 1.57 radians)
- **LookDown**: [angle] (-1.57 to 1.57 radians)
- **Interact**: [strength] (0.0-1.0)
- **Eat**: [quantity] (0.0-1.0)
- **Drink**: [quantity] (0.0-1.0)
- **Rest**: [duration] (0.0-1.0)
- **Wait**: [probability] (0.0-1.0)
- **Custom**: [parameters...] (arbitrary parameters)

#### Vision
Vision sensory input processing and enhancement.

**Methods:**
- `Vision(width, height, channels)`: Create vision instance
- `setData(data)`: Set pixel data
- `getWidth()`: Get image width
- `getHeight()`: Get image height
- `getChannels()`: Get number of channels
- `getData()`: Get raw pixel data
- `getDimensions()`: Get data dimensionality

**Advanced Vision Processing:**

##### VisionProcessor
Advanced computer vision capabilities:

**Methods:**
- `process(input)`: Process vision input and extract features
- `getFeatures()`: Get feature representation
- `getEdges()`: Get edge detection results
- `getCorners()`: Get corner detection results
- `getMotion()`: Get motion detection results
- `getContrast()`: Get contrast enhancement results
- `reset()`: Reset processor state

##### VisionEnhancement
Image processing techniques:

**Methods:**
- `applyGaussianBlur(input, output, sigma)`: Apply Gaussian blur
- `detectEdges(input, output, threshold)`: Detect edges
- `detectCorners(input, output, threshold)`: Detect corners
- `enhanceContrast(input, output, factor)`: Enhance contrast
- `detectMotion(current, previous, output, threshold)`: Detect motion

#### SensoryInput
Base class for all sensory modalities.

**Subclasses:**
- **Vision**: Visual input processing
- **Audio**: Audio input processing
- **InternalSignals**: Internal state signals (homeostatic)
- **SensoryPercept**: Complete sensory data packet

#### WorldObject
World object representation for simulation.

**Properties:**
- `x`, `y`: Position coordinates
- `radius`: Object radius
- `type`: Object type (Resource, Hazard, Wall, Marker)
- `value`: Object value (e.g., reward amount)
- `active`: Object activation state

#### AgentBrain
Agent brain interface connecting NLM brain to world with advanced learning systems.

**Learning System Enablement:**
- `enableRewardModulation(True/False)`: Turn reward learning on/off
- `enableStructuralPlasticity(True/False)`: Turn connection growth on/off
- `enableDevelopment(True/False)`: Turn developmental learning on/off
- `enableCuriosity(True/False)`: Turn exploration on/off
- `enableSocialLearning(True/False)`: Turn social observation on/off
- `enableStressSystem(True/False)`: Turn stress response on/off
- `enableFatigueSystem(True/False)`: Turn fatigue system on/off
- `enableEnergySystem(True/False)`: Turn energy system on/off
- `enableHealthSystem(True/False)`: Turn health system on/off
- `enableAttentionSystem(True/False)`: Turn attention system on/off
- `enableLearningSystem(True/False)`: Turn learning system on/off

**State Access Methods:**
- `getDevelopmentalStage()`: Current developmental stage
- `getNeuromodulationLevel()`: Current neuromodulation signal
- `getCuriosityLevel()`: Current curiosity/exploration level
- `getNoveltyLevel()`: Current novelty detection level
- `getPredictionError()`: Current prediction error signal
- `getStressLevel()`: Current stress level
- `getFatigueLevel()`: Current fatigue level
- `getEnergyLevel()`: Current energy level
- `getHealthLevel()`: Current health level
- `getAttentionLevel()`: Current attention level

**Core Methods:**
- `initialize(world)`: Set up the agent with a world
- `processSensoryInput(percept)`: Inject sensory input into the brain
- `decodeMotorCommand()`: Select an action based on brain activity
- `applyRewardModulation(reward, predicted_reward)`: Learn from outcomes
- `updateDevelopment(timestep)`: Update developmental state
- `reset()`: Reset for a new episode
- `getBrain()`: Access the underlying brain

### System Integration Examples

#### Example 1: Simple Reactive Agent

```python
import pynlm

# Create basic brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simple reactive behavior
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Example 2: Learning Agent with Curiosity

```python
import pynlm

# Create learning agent with exploration
agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
agent.initialize(pynlm.createSimpleWorld())

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)  # Curiosity-driven exploration
agent.enableDevelopment(True)  # Developmental learning
agent.enableSocialLearning(True)  # Social observation learning

# Train with exploration
for episode in range(50):
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        agent.getBrain()->step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
    
    agent.reset()
    world.reset()
```

#### Example 3: Vision-Based Object Navigation

```python
import pynlm
import numpy as np

# Create agent and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)  # 8x8 vision grid
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create a target and obstacles in the world
target = pynlm.WorldObject()
target.x = 15.0
target.y = 15.0
target.type = pynlm.WorldObjectType.Resource
target.value = 10.0
target.radius = 0.5
world.addObject(target)

# Run navigation simulation
for step in range(500):
    world.update(0.1)
    
    # Get visual input
    percept = world.getSensoryPercept()
    vision_data = percept.getVision()
    
    # Process through agent
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action and apply
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check if reached target
    agent_body = world.getAgentBody()
    dist_to_target = np.sqrt((agent_body.x - target.x)**2 + (agent_body.y - target.y)**2)
    if dist_to_target < 1.0:  # Reached target
        print(f"Target reached in {step} steps!")
        break
```

#### Example 4: Developmental Learning Simulation

```python
import pynlm

# Create development-enabled agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Enable developmental learning
agent.enableDevelopment(True)
agent.enableLearning(True)
agent.enableCuriosity(True)

# Track development over time
development_stages = []
curiosity_levels = []
learning_rates = []

for episode in range(100):
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
    
    # Record developmental state
    development_stages.append(agent.getDevelopmentalStage())
    curiosity_levels.append(agent.getCuriosityLevel())
    learning_rates.append(brain.getAverageFiringRate())
    
    agent.reset()
    world.reset()

print(f"Development completed. Final stage: {development_stages[-1]}")
print(f"Final curiosity: {curiosity_levels[-1]}")
print(f"Final learning rate: {learning_rates[-1]}")
```

### Performance Monitoring and Debugging

#### Performance Statistics

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get performance statistics
stats = pynlm.getPerformanceStats(brain)
print("Performance Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")

# Check brain health
health_report = pynlm.checkBrainHealth(brain)
print(f"\nHealth Report:")
print(f"  Neuron count: {health_report['neuron_count']}")
print(f"  Synapse count: {health_report['synapse_count']}")
print(f"  Active ratio: {health_report['active_ratio']:.3f}")
if health_report['warnings']:
    print(f"  Warnings: {health_report['warnings']}")
```

#### Debug Information

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Print detailed brain status
print("Brain Status:")
pynlm.printBrainStatus(brain)

# Check specific components
print(f"\nComponent Status:")
print(f"  Regions: {brain.getRegionCount()}")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Working memory traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 'N/A'}")
print(f"  Episodic episodes: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 'N/A'}")
```

### Error Handling and Validation

#### Input Validation

```python
import pynlm

# Validate input data
data = [0.1, 0.2, 0.3, 0.4, 0.5]
try:
    pynlm.validateInput(data, 5)
    print("Input validation passed")
except Exception as e:
    print(f"Input validation failed: {e}")

# Validate configuration
config = pynlm.createDefaultConfig()
try:
    config.set("neuron_count", -100)  # Invalid value
    print("ERROR: Should have failed validation")
except ValueError as e:
    print(f"Configuration validation caught error: {e}")
```

#### Exception Handling

```python
import pynlm

try:
    # Create brain with invalid configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", -100)  # Invalid
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except pynlm.InvalidArgumentError as e:
    print(f"Invalid argument error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Advanced Features

#### Checkpoint Management

```python
import pynlm
import tempfile
import os

# Create and simulate brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Save brain state
with tempfile.NamedTemporaryFile(delete=False, suffix='.nlm') as f:
    checkpoint_path = f.name

success = pynlm.saveBrainState(brain, checkpoint_path)
if success:
    print(f"Brain state saved to {checkpoint_path}")

# Load brain state
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
success = pynlm.loadBrainState(new_brain, checkpoint_path)
if success:
    print(f"Brain state loaded successfully")

# Clean up
os.unlink(checkpoint_path)
```

#### Development and Exploration Systems

```python
import pynlm

# Create development-enabled agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Enable comprehensive developmental and exploration systems
agent.enableDevelopmentSystem(True)  # Full developmental system
agent.enableExplorationSystem(True)  # Curiosity-driven exploration
agent.enableSocialLearning(True)  # Social observation
agent.enableStressSystem(True)     # Stress response
agent.enableFatigueSystem(True)    # Fatigue system
agent.enableEnergySystem(True)     # Energy management
agent.enableHealthSystem(True)     # Health monitoring
agent.enableAttentionSystem(True)  # Attention control
agent.enableLearningSystem(True)   # Learning optimization

# Run extended exploration
exploration_data = pynlm.exploreEnvironment(agent, world, 1000)
print(f"Exploration completed. Final curiosity: {exploration_data}")
```

#### Training and Learning

```python
import pynlm

# Create training environment
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run training
print("Starting training...")
training_results = pynlm.trainAgent(agent, world, 50)

print(f"Training completed:")
print(f"  Average reward: {training_results['avg_reward']:.3f}")
print(f"  Best reward: {training_results['best_reward']:.3f}")
print(f"  Episodes: {training_results['episodes']}")
print(f"  Final curiosity: {training_results['final_curiosity']:.3f}")
```

### Utility Functions

#### Mathematical Operations

```python
import pynlm

# Value clamping
clamped = pynlm.clamp(1.5, 0.0, 1.0)
print(f"Clamped value: {clamped}")  # 1.0

# Normalization
normalized = pynlm.normalize(0.5, 0.0, 1.0)
print(f"Normalized value: {normalized}")  # 0.5
```

#### Demonstration Scenes

```python
import pynlm

# Create educational demonstration scene
demo_world = pynlm.createDemonstrationScene()
print("Created demonstration scene with walls and goals")

# Create example agent with learning
agent = pynlm.create_example_agent(
    pynlm.createBrain(pynlm.createDefaultConfig()),
    demo_world
)
print("Created example agent with learning enabled")
```

### Version Information

```python
import pynlm

print(f"NLM Python Bindings Version: {pynlm.__version__}")
print(f"Description: {pynlm.__description__}")
print(f"Author: {pynlm.__author__}")
print(f"License: {pynlm.__license__}")
```

## Best Practices

### 1. Configuration Management

```python
# Always create configurations with appropriate parameters
config = pynlm.createDefaultConfig()

# Validate configuration before use
if not config.validateAll():
    print("Configuration validation failed!")
    # Handle validation errors
```

### 2. Error Handling

```python
try:
    # Your code here
    pass
except pynlm.RuntimeError as e:
    print(f"Runtime error occurred: {e}")
except pynlm.InvalidArgumentError as e:
    print(f"Invalid argument provided: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### 3. Performance Optimization

```python
# Profile performance
stats = pynlm.getPerformanceStats(brain)
print(f"Current performance: {stats}")

# Check for potential issues
health_report = pynlm.checkBrainHealth(brain)
if health_report['warnings']:
    print("Performance warnings detected!")
```

### 4. Simulation Best Practices

```python
# Use appropriate timestep for your simulation
def runSimulation(agent, world, steps):
    for step in range(steps):
        world.update(0.1)  # Fixed timestep
        agent.processSensoryInput(world.getSensoryPercept())
        agent.getBrain()->step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: Brain initialization fails
**Solution:**
- Check configuration parameters
- Ensure required configuration values are set
- Validate configuration before initialization

#### Issue: Simulation becomes unstable
**Solution:**
- Reduce timestep size
- Check for infinite loops
- Monitor neuron activity and firing rates

#### Issue: Memory usage too high
**Solution:**
- Monitor performance statistics
- Consider checkpointing for long simulations
- Optimize configuration for memory constraints

#### Issue: Learning doesn't progress
**Solution:**
- Verify learning system is enabled
- Check reward and prediction error signals
- Ensure neuromodulation systems are functioning

### Debugging Tools

```python
# Print detailed information
pynlm.printBrainStatus(brain)

# Check specific component health
health = pynlm.checkBrainHealth(brain)
print(f"Health report: {health}")

# Profile performance
stats = pynlm.getPerformanceStats(brain)
print(f"Performance: {stats}")
```

## References

- NLM (熙然) Project Documentation
- Phase 6: Final Integration Report
- Python Bindings Development Guide
- NLM Architecture and Design Specifications

## Acknowledgments

This Python implementation is based on the NLM (熙然) research project, an experimental artificial developmental brain simulation framework. The Python bindings provide a user-friendly interface to the powerful C++ neural simulation capabilities while maintaining compatibility with Python programming paradigms.

## License

MIT License - See LICENSE file for details.

---

*This documentation is part of the NLM (熙然) Python Bindings project. For support and updates, visit the NLM project documentation.*