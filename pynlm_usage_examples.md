# pynlm Python Bindings - Usage Examples

This document provides examples of how to use the pynlm Python bindings for the Neural Learning Machine framework.

## Quick Start

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain()
brain.initialize()

# Create a simple world
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, visionWidth=20, visionHeight=20)

# Create agent brain that connects brain to world
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Run simulation loop
for step in range(100):
    # Get sensory percept from world
    percept = world.getSensoryPercept()
    
    # Process sensory input through agent brain
    agent_brain.processSensoryInput(percept)
    
    # Produce action based on brain activity
    action = brain.produceAction()
    
    # Apply action in world
    world.applyMotorCommand(action, step)
```

## Brain Management

### Basic Brain Operations

```python
# Create brain with custom configuration
config = pynlm.Config()
config.loadFromFile("config.json")
brain = pynlm.Brain(config)
brain.initialize()

# Simulation step with timestamp
brain.step(currentStep, currentTime)

# Save and load brain state
brain.save("brain_checkpoint.pkl")
brain.load("brain_checkpoint.pkl")

# Get brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
print(f"Excitation/inhibition ratio: {brain.getExcitationInhibitionRatio()}")
```

### Brain Regions and Neurons

```python
# Add neural regions
brain.addRegion("sensory_cortex")
brain.addRegion("motor_cortex")

# Get regions by ID
region = brain.getRegion(region_id)
region_count = brain.getRegionCount()
region_ids = brain.getRegionIds()

# Get neurons by type
for neuron_id in brain.getNeuronIdsByType(pynlm.NeuronType.Excitatory):
    print(f"Excitatory neuron: {neuron_id}")
```

## World Simulation

### Simple World Operations

```python
# Create and configure world
world = pynlm.SimpleWorld()
world.configure(width=200, height=200, visionWidth=30, visionHeight=30)
world.setAgentStart(10, 10)

# Reset simulation
world.reset()

# Get world state
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent energy: {body.energy}")

# Add objects to world
obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0)
world.addObject(obj)

# Check if position is valid
is_valid = world.isValidPosition(15.0, 15.0)
```

### Sensory Processing

```python
# Create sensory inputs
vision = pynlm.Vision(width=20, height=20, channels=3)
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
internal = pynlm.InternalSignals()

# Set sensory data
vision_data = [[0.1, 0.2, 0.3] for _ in range(20)]
vision.setData(vision_data)

# Create percept from sensory inputs
percept = pynlm.SensoryPercept()
percept.setVision(vision)
percept.setAudio(audio)
percept.setInternal(internal)

# Get individual signals
print(f"Vision width: {percept.getVisionWidth()}")
print(f"Vision height: {percept.getVisionHeight()}")
print(f"Audio samples: {percept.getAudio().getNumSamples()}")
```

## Agent Brain Interface

### Processing and Action

```python
# Process sensory input through agent brain
agent_brain.processSensoryInput(percept)

# Decode motor command from brain activity
command = agent_brain.decodeMotorCommand()
print(f"Motor command: {command}")

# Apply neuromodulation
agent_brain.applyRewardModulation(reward=1.0, predicted_reward=0.5)

# Update development
agent_brain.updateDevelopment(timestep)

# Check system states
print(f"Developmental stage: {agent_brain.getDevelopmentalStage()}")
print(f"Curiosity level: {agent_brain.getCuriosityLevel()}")
print(f"Novelty level: {agent_brain.getNoveltyLevel()}")
```

## Advanced Usage

### Configuration Management

```python
# Create and manipulate configuration
config = pynlm.Config()
config.loadFromFile("nlm_config.json")

# Get configuration summary
print(config.summary())

# Check configuration keys
if config.has("neuron_count"):
    neuron_count = config.get("neuron_count")

# Save configuration
config.saveToFile("custom_config.json")
```

### Action System

```python
# Create actions
move_action = pynlm.Action(pynlm.ActionType.MoveForward, [0.5, 0.0, 0.0])
look_action = pynlm.Action(pynlm.ActionType.Look, [45.0])

# Get action properties
print(f"Action type: {move_action.getType()}")
print(f"Action name: {move_action.getName()}")
print(f"Action parameters: {move_action.getParameters()}")

# Clone action
cloned_action = move_action.clone()
```

### Memory and Learning

```python
# Access brain memory systems
# (Implementation depends on available memory APIs)

# Track learning progress
brain.logStatus()

# Get prediction error
prediction_error = agent_brain.getPredictionError()
print(f"Prediction error: {prediction_error}")
```

## Simulation Examples

### Complete Episode

```python
def run_simulation_episode(brain, world, agent_brain, max_steps=1000):
    """Run a complete simulation episode."""
    world.reset()
    
    for step in range(max_steps):
        # Get current world state
        percept = world.getSensoryPercept()
        body = world.getAgentBody()
        
        # Agent processing loop
        agent_brain.processSensoryInput(percept)
        action = brain.produceAction()
        
        # Execute action in world
        world.applyMotorCommand(action, step)
        
        # Check termination conditions
        if body.health <= 0 or step >= max_steps - 1:
            break
    
    return step

# Usage
episode_length = run_simulation_episode(brain, world, agent_brain)
print(f"Episode completed in {episode_length} steps")
```

### Experiment Framework

```python
def run_experiments(brain, world, agent_brain, num_episodes=10):
    """Run multiple experimental episodes."""
    results = []
    
    for episode in range(num_episodes):
        episode_length = run_simulation_episode(brain, world, agent_brain)
        
        # Collect metrics
        metrics = {
            'episode': episode,
            'length': episode_length,
            'final_energy': world.getAgentBody().energy,
            'total_spikes': brain.getTotalSpikeCount(),
            'reward_modulation': agent_brain.getNeuromodulationLevel()
        }
        
        results.append(metrics)
    
    return results

# Run experiments
experiment_results = run_experiments(brain, world, agent_brain, 20)
```

## Performance Monitoring

### System Status

```python
# Log brain status (prints diagnostic information)
brain.logStatus()

# Get comprehensive system statistics
stats = {
    'neurons': brain.getTotalNeuronCount(),
    'synapses': brain.getTotalSynapseCount(),
    'active': brain.getActiveNeuronCount(),
    'firing': brain.getFiringNeuronCount(),
    'average_rate': brain.getAverageFiringRate(),
    'excitatory_inhibitory_ratio': brain.getExcitationInhibitionRatio(),
    'total_spikes': brain.getTotalSpikeCount(),
    'developmental_stage': brain.getDevelopmentalStage(),
    'simulation_time': world.getSimulationTime()
}

print("Brain Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")
```

## Error Handling

### Common Exceptions

```python
try:
    # Operations that might fail
    brain.save("invalid/path/brain.pkl")
    action = brain.produceAction()
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Integration with External Systems

### Custom Sensory Data

```python
# Example: Integrating with real-world sensors
import numpy as np

# Simulate camera input (vision)
camera_frame = np.random.rand(20, 20, 3).astype(np.float32)
vision = pynlm.Vision(20, 20, 3)
vision.setData(camera_frame.reshape(-1).tolist())

# Simulate microphone input (audio)
audio_samples = np.random.randn(1024).astype(np.float32)
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
audio.setData(audio_samples.tolist())
audio.setSampleRate(44100)

# Combine into percept
percept = pynlm.SensoryPercept()
percept.setVision(vision)
percept.setAudio(audio)
```

## Migration Guide

### From C++ to Python

If you're migrating from the C++ NLM API to Python, here are key differences:

**C++:** `brain->step(currentStep, currentTime)`
**Python:** `brain.step(currentStep, currentTime)`

**C++:** `config->loadFromFile("file.json")`
**Python:** `config.loadFromFile("file.json")`

**C++:** `std::shared_ptr<NeuronId> id(new NeuronId(value))`
**Python:** `id = pynlm.NeuronId(value)`

**C++:** Direct pointer access to internal structures
**Python:** Encapsulated Pythonic interfaces with automatic memory management

## Next Steps

### For Advanced Users

1. **Custom Sensory Inputs**: Implement custom sensory classes extending SensoryInput
2. **Plugin System**: Create custom action types and neuromodulators
3. **Performance Optimization**: Use batch operations for large-scale simulations
4. **Distributed Computing**: Implement multi-agent systems using Python's multiprocessing

### For Research

1. **Experiment Tracking**: Use the experiment framework for systematic studies
2. **Parameter Tuning**: Leverage configuration system for hyperparameter optimization
3. **Visualization**: Integrate with Python's visualization libraries
4. **Data Analysis**: Export simulation data for analysis with scientific Python stack
```