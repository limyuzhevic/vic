# NLM Python API Documentation

## Overview

The NLM (Neural Learning Machine) Python API provides a comprehensive interface for simulating brain-like neural networks using event-driven spiking neural computation. This module is built on the Phase 6 final integration, providing access to all core neural systems including memory, prediction, cognition, neuromodulation, and development.

## Installation

If you haven't already installed NLM, follow these steps:

```bash
# Install from source (recommended for development)
pip install pybind11 scikit-build-core pytest numpy
pip install -e .

# Or install from wheel
pip install dist/nlm-*.whl
```

## Getting Started

### Basic Import

```python
import pynlm
```

### Creating a Default Configuration

```python
# Create a default configuration
config = pynlm.createDefaultConfig()

# Or load configuration from file
config = pynlm.Config()
config.loadFromFile("config.json")
```

### Creating a Brain

```python
# Create a new brain with default configuration
brain = pynlm.createBrain(config)

# Brain automatically handles all integrated systems:
# - Memory systems (working, episodic, associative)
# - Prediction system
# - Cognition systems (planning, concept formation, attention)
# - Neuromodulation (dopamine, curiosity, novelty, prediction error)
# - Development system
# - Plasticity systems (STDP, Hebbian, structural)
```

### Initializing and Running the Brain

```python
# Step 1: Initialize the brain
brain.initialize()

# Step 2: Run simulation steps
for step in range(100):
    brain.step(step)

# Access brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Core Classes

### 1. Config (Configuration Management)

The `Config` class manages all configuration parameters for the NLM system.

#### Methods

```python
config = pynlm.Config()

# Basic configuration
config.loadFromFile("my_config.json")
config.loadFromArgs(len(sys.argv), sys.argv)  # Command line arguments

# Check configuration keys
if config.has("neuron_count"):
    count = config.get("neuron_count")

# Get all configuration keys
keys = config.getKeys()

# Save configuration
config.saveToFile("output_config.json")

# Clear configuration
config.clear()

# Get configuration summary
print(config.summary())
```

### 2. Brain (Central Neural System)

The `Brain` class implements the complete integrated brain simulation with:
- Real spiking neural computation
- Event-driven spike propagation
- Multiple memory systems
- Prediction and cognition
- Neuromodulation
- Developmental plasticity

#### Basic Usage

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Access integrated systems
working_memory = brain.getWorkingMemory()
prediction_system = brain.getPredictionSystem()
planner = brain.getPlanner()
attention = brain.getAttention()

# Get statistics
print(f"E/I ratio: {brain.getExcitationInhibitionRatio()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

#### Advanced Features

```python
# Inject sensory input
vision = pynlm.Vision(16, 16, 3)  # 16x16 image, 3 channels
vision.setData([0.5] * 256)  # Sample data
brain.receiveSensoryInput(vision)

# Inject current into specific neurons
sensory_neuron_id = brain.getRegion(pynlm.RegionId(1)).getPopulation(pynlm.PopulationId(0)).getNeurons()[0].getId()
brain.injectCurrent(sensory_neuron_id, 5.0)

# Produce motor action
motor_action = brain.produceAction()

# Apply neuromodulation (reward)
brain.applyNeuromodulation(pynlm.Dopamine(), 0.8)

# Save/load brain state
brain.save("my_brain.nlm")
# Later...
brain.load("saved_brain.nlm")

# Log detailed status
brain.logStatus()
```

### 3. AgentBrain (Brain-World Interface)

The `AgentBrain` class bridges the NLM brain with the simulation world, handling:
- Sensory transduction (converting world percepts to neural input)
- Motor decoding (converting neural activity to motor commands)
- Neuromodulation (reward-based learning)
- Development management

#### Basic Agent Usage

```python
import pynlm

# Create brain, world, and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run complete agent-world loop
for step in range(500):
    # Update world state
    world.update(0.1)
    
    # Get sensory percept (what the agent sees)
    percept = world.getSensoryPercept()
    
    # Process sensory input (brain receives it)
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Brain decides action
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Brain learns from result (reward)
    agent.applyRewardModulation(result.reward, 0.0)  # Zero predicted reward
    
    # Update development
    agent.updateDevelopment(0.1)

print(f"Final neuromodulation level: {agent.getNeuromodulationLevel()}")
print(f"Final curiosity level: {agent.getCuriosityLevel()}")
```

#### Agent Features

```python
# Get expected input/output sizes
sensory_input_size = agent.getSensoryInputSize()
motor_output_size = agent.getMotorOutputSize()

# Get current agent state
stage = agent.getDevelopmentalStage()
dopamine = agent.getNeuromodulationLevel()
curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()
prediction_error = agent.getPredictionError()

# Reset agent for new episode
agent.reset()
```

### 4. SimpleWorld (Simulation Environment)

The `SimpleWorld` class provides a 2D simulation environment with:
- Physics simulation (agent movement, collisions)
- Sensory generation (vision, touch, proprioception)
- Object management (resources, hazards, walls)
- Energy and health systems

#### Basic World Usage

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world simulation
for step in range(200):
    world.update(0.1)  # Advance world by 0.1 time units
    
    # Get current sensory percept
    percept = world.getSensoryPercept()
    
    # Access agent state
    agent_body = world.getAgentBody()
    print(f"Agent position: ({agent_body.x:.2f}, {agent_body.y:.2f})")
    print(f"Agent energy: {agent_body.energy:.2f}")
    print(f"Agent health: {agent_body.health:.2f}")
    
    # Apply actions
    action = pynlm.MotorCommand.MoveForward
    result = world.applyMotorCommand(action, world.getSimulationTime())
    print(f"Action result: {result.message} (reward: {result.reward})")
```

#### World Features

```python
# Access world properties
width = world.getWidth()
height = world.getHeight()
max_energy = world.getMaxEnergy()
energy_decay = world.getEnergyDecayRate()

# Object management
# Add resource (positive reward)
resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0)
world.addObject(resource)

# Add hazard (negative reward)
hazard = pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, -15.0)
world.addObject(hazard)

# Check position validity
if world.isValidPosition(5.0, 5.0):
    print("Position is valid")

# Set random seed for reproducible simulations
world.setRandomSeed(42)
world.reset()
```

### 5. SensoryInput Classes (Sensory Processing)

The sensory input classes provide structured access to different types of sensory data.

#### Vision Processing

```python
# Create vision input
vision = pynlm.Vision(16, 16, 3)  # 16x16 pixel, 3 color channels

# Set vision data (example: simple gradient)
data = []
for y in range(16):
    for x in range(16):
        intensity = (x + y) / 32.0  # 0.0 to 1.0
        data.extend([intensity, intensity, intensity])  # RGB

vision.setData(data)

# Access vision dimensions
width = vision.getWidth()
height = vision.getHeight()
channels = vision.getChannels()
```

#### Audio Processing

```python
# Create audio input
audio = pynlm.Audio(44100, 1024)  # 44.1kHz sample rate, 1024 samples
audio.setSampleRate(22050)  # Set sample rate
audio.setData([0.1, 0.2, 0.3, ...])  # Audio samples
```

#### Internal Signals

```python
# Create internal signals (energy, health, etc.)
internal = pynlm.InternalSignals()
internal.addSignal(0.8)  # Energy level
internal.addSignal(0.9)  # Health level
internal.addSignal(0.5)  # Movement signal
```

### 6. SensoryPercept (Processed Sensory Data)

The `SensoryPercept` class combines all sensory inputs into a unified perception for the agent.

#### Access Sensory Data

```python
percept = world.getSensoryPercept()

# Vision data
vision_data = percept.getVision()
vision_width = percept.getVisionWidth()
vision_height = percept.getVisionHeight()

# Touch data (proximity sensors)
touch_data = percept.getTouch()

# Internal signals
internal_data = percept.getInternal()

# Proprioception (body position)
proprioception_data = percept.getProprioception()

# Audio data
audio_data = percept.getAudio()

# All signals at once
all_signals = percept.getAllSignals()

# Timestamp
timestamp = percept.getTimestamp()
```

### 7. Action Classes (Motor Output)

The action classes represent motor commands and their parameters.

#### Motor Actions

```python
# Create basic actions
action = pynlm.Action(pynlm.ActionType.MoveForward)
action = pynlm.Action(pynlm.ActionType.Custom, [0.5, 1.0, 2.0])

# Action types
action_type = action.getType()
action_name = action.getName()  # Human-readable name

# Action parameters
parameters = action.getParameters()
action.setParameters([1.0, 2.0, 3.0])

# Clone actions
action_copy = action.clone()
```

### 8. WorldObject (Environment Objects)

The `WorldObject` class represents objects in the simulation world.

#### Object Management

```python
# Create objects
resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0)
hazard = pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, -15.0)
wall = pynlm.WorldObject(20.0, 20.0, pynlm.WorldObjectType.Wall, 0.0, 1.0)

# Access object properties
print(f"Object type: {obj.type}")
print(f"Position: ({obj.x}, {obj.y})")
print(f"Value: {obj.value}")  # Positive for resources, negative for hazards
print(f"Radius: {obj.radius}")
print(f"Active: {obj.active}")
```

### 9. AgentBody (Agent Physical State)

The `AgentBody` class represents the physical state of the agent in the world.

#### Agent State Access

```python
agent_body = world.getAgentBody()

# Position and orientation
x = agent_body.x
y = agent_body.y
orientation = agent_body.orientation  # radians

# Velocity
velocity_x = agent_body.velocityX
velocity_y = agent_body.velocityY
angular_velocity = agent_body.angularVelocity

# Physical attributes
energy = agent_body.energy
health = agent_body.health
age = agent_body.age

# State flags
is_moving = agent_body.isMoving
is_turning = agent_body.isTurning

# Reset agent state
agent_body.reset()
```

## Advanced API Features

### 1. Performance Profiling

```python
# Profile brain performance
stats = pynlm.profileBrain(brain, 1000)
total_spikes, avg_firing_rate, seconds = stats
print(f"Simulation: {seconds:.2f}s, {total_spikes} spikes, {avg_firing_rate:.2f} Hz")
```

### 2. Memory Statistics

```python
# Get comprehensive memory statistics
memory_stats = pynlm.getMemoryStats(brain)
working_memory_traces, episodic_episodes, associative_patterns = memory_stats
print(f"Working memory: {working_memory_traces} traces")
print(f"Episodic memory: {episodic_episodes} episodes")
print(f"Associative memory: {associative_patterns} patterns")
```

### 3. Development Simulation

```python
# Simulate brain development over time
development_stages = pynlm.simulateDevelopment(agent, 1000.0, 0.1)
for i, stage in enumerate(development_stages):
    print(f"Step {i}: {stage}")
```

### 4. Reward Shaping

```python
# Apply sophisticated reward shaping with discounted returns
rewards = [1.0, 0.5, 0.0, -0.5, -1.0]
total_return = pynlm.applyRewardShaping(agent, rewards, gamma=0.9)
print(f"Total discounted return: {total_return:.2f}")
```

### 5. Sensory Pattern Analysis

```python
# Analyze sensory patterns over time
patterns = pynlm.analyzeSensoryPatterns(brain, 500)
print(f"Sensory pattern analysis: {patterns}")
```

### 6. Network Topology Analysis

```python
# Analyze network properties
neurons, synapses, connections = pynlm.analyzeNetworkTopology(brain)
print(f"Network: {neurons} neurons, {synapses} synapses, {connections} regions")
```

### 7. Learning Curve Visualization

```python
# Generate learning curve data for visualization
learning_curve = pynlm.getLearningCurve(agent, 200)
for step, neuromodulation, curiosity in learning_curve:
    print(f"Step {step:.1f}: Dopamine={neuromodulation:.3f}, Curiosity={curiosity:.3f}")
```

## Specialized Classes

### NeuronId, SynapseId, RegionId, PopulationId

These identifier classes provide unique references to neural components:

```python
neuron_id = pynlm.NeuronId()
synapse_id = pynlm.SynapseId()
region_id = pynlm.RegionId()
population_id = pynlm.PopulationId()

# Compare identifiers
if neuron_id == other_id:
    print("Same neuron")

# Get unique hash for dictionary keys
id_dict = {neuron_id: "some_data"}
```

### Enumeration Types

```python
# Neuron types
neuron_type = pynlm.NeuronType.Excitatory
neuron_type = pynlm.NeuronType.Inhibitory
neuron_type = pynlm.NeuronType.Sensory
neuron_type = pynlm.NeuronType.Motor

# Synapse types
synapse_type = pynlm.SynapseType.Excitatory
synapse_type = pynlm.SynapseType.GapJunction

# Action types
action_type = pynlm.ActionType.MoveForward
action_type = pynlm.ActionType.Interact

# World object types
object_type = pynlm.WorldObjectType.Resource
object_type = pynlm.WorldObjectType.Hazard
```

## Integration Patterns

### Pattern 1: Complete Agent Simulation

```python
def run_agent_simulation(config, world_params, steps):
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(**world_params)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Simulation loop
    episode_rewards = []
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, 0.0)
        agent.updateDevelopment(0.1)
        episode_rewards.append(result.reward)
    
    return {
        'rewards': episode_rewards,
        'final_state': {
            'neuromodulation': agent.getNeuromodulationLevel(),
            'curiosity': agent.getCuriosityLevel(),
            'stage': agent.getDevelopmentalStage()
        }
    }
```

### Pattern 2: Multi-Agent Coordination

```python
def run_multi_agent_simulation(agent_configs, world_size):
    # Create multiple brains and agents
    agents = []
    for config in agent_configs:
        brain = pynlm.createBrain(config)
        brain.initialize()
        agents.append({
            'brain': brain,
            'agent': pynlm.createAgentBrain(brain)
        })
    
    world = pynlm.createSimpleWorld()
    world.configure(**world_size)
    world.reset()
    
    # Initialize all agents with world
    for agent_info in agents:
        agent_info['agent'].initialize(world)
    
    # Coordinated simulation
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Each agent processes sensory input
        for agent_info in agents:
            agent_info['agent'].processSensoryInput(percept)
        
        # All brains step concurrently
        for agent_info in agents:
            agent_info['brain'].step(step)
        
        # Agents coordinate actions (simple averaging for demo)
        actions = []
        for agent_info in agents:
            actions.append(agent_info['agent'].decodeMotorCommand())
        
        # Average action
        if actions:
            final_action = actions[0]  # Simplified coordination
            result = world.applyMotorCommand(final_action, world.getSimulationTime())
            
            # All agents learn from shared reward
            for agent_info in agents:
                agent_info['agent'].applyRewardModulation(result.reward, 0.0)
    
    return agents
```

### Pattern 3: Reinforcement Learning Environment

```python
class RLEnvironment:
    def __init__(self, config):
        self.config = config
        self.reset()
    
    def reset(self):
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        self.agent.enableRewardModulation(True)
    
    def step(self, action):
        self.world.update(0.1)
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        self.brain.step(self.step_count)
        
        result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
        self.agent.applyRewardModulation(result.reward, 0.0)
        self.agent.updateDevelopment(0.1)
        
        self.step_count += 1
        return percept, result.reward, self.world.getSimulationTime() >= 100.0
    
    def render(self):
        agent_body = self.world.getAgentBody()
        print(f"Position: ({agent_body.x:.1f}, {agent_body.y:.1f}), "
              f"Energy: {agent_body.energy:.1f}, "
              f"Health: {agent_body.health:.1f}")
        
        # Visualize vision
        percept = self.world.getSensoryPercept()
        vision = percept.getVision()
        print("Vision (16x16):")
        for y in range(16):
            row = ""
            for x in range(16):
                idx = y * 16 + x
                intensity = vision[idx]
                row += "█" if intensity > 0.5 else "░"
            print(row)
```

## Performance Tips

### 1. Memory Optimization

```python
# For long-running simulations
brain.save("checkpoint.nlm")  # Save periodically
brain.load("checkpoint.nlm")  # Restore from checkpoint

# Monitor memory usage
stats = pynlm.getMemoryStats(brain)
working_memory_traces, episodic_episodes, associative_patterns = stats
```

### 2. Performance Profiling

```python
import time

# Profile different components
start_time = time.time()
brain.step(100)
step_time = time.time() - start_time
print(f"Step time: {step_time:.4f}s")

# Profile entire simulation
start_time = time.time()
for i in range(1000):
    brain.step(i)
total_time = time.time() - start_time
print(f"Total time for 1000 steps: {total_time:.2f}s")
print(f"Average step time: {total_time/1000:.4f}s")
```

### 3. Concurrent Processing

```python
# For parallel processing (if using multiprocessing)
def process_brain_async(brain, steps, callback):
    import multiprocessing as mp
    
    def worker(q_in, q_out):
        brain_copy = brain
        for step in range(steps):
            brain_copy.step(step)
            q_out.put(brain_copy.getTotalSpikeCount())
    
    q_in = mp.Queue()
    q_out = mp.Queue()
    
    process = mp.Process(target=worker, args=(q_in, q_out))
    process.start()
    process.join()
    
    results = []
    while not q_out.empty():
        results.append(q_out.get())
    
    return sum(results)
```

## Extending the API

### Custom Action Types

```python
# Extend the ActionType enumeration
# This would require modifying the C++ bindings
# and recompiling the Python module

class CustomActionType:
    def __init__(self, name, value):
        self.name = name
        self.value = value

# Add custom action types to the bindings
# (This requires C++ code modification)
```

### Custom Sensory Inputs

```python
# Create custom sensory input types
class CustomSensoryInput(pynlm.SensoryInput):
    def __init__(self, data, timestamp=0.0):
        super().__init__()
        self.custom_data = data
        self.custom_timestamp = timestamp
    
    def getCustomType(self):
        return "custom"
    
    def getCustomData(self):
        return self.custom_data

# Use in brain
brain.receiveSensoryInput(custom_input)
```

## Error Handling

### Common Exceptions

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(0)
except std::runtime_error as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Validation and Safety

```python
# Validate inputs before use
if config.has("neuron_count") and config.get("neuron_count") > 100000:
    print("Warning: Large brain configuration may impact performance")

# Check brain state before operations
if brain.getTotalNeuronCount() == 0:
    print("Warning: Brain has no neurons")
    brain.initialize()
```

## Migration Guide

### From Version 0.1.0 to 0.1.1+

#### API Changes

1. **New Development Features**: The API now includes comprehensive development system integration
2. **Enhanced Memory Systems**: All memory systems are now accessible through the brain interface
3. **Advanced Agent Features**: AgentBrain now includes curiosity and development tracking

#### Migration Examples

```python
# Old way (version 0.1.0)
brain = pynlm.createBrain()
brain.initialize()

# New way (version 0.1.1+)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# You can now access integrated systems
working_memory = brain.getWorkingMemory()
planner = brain.getPlanner()
attention = brain.getAttention()

# Agent features have been enhanced
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

## Troubleshooting

### Common Issues

#### 1. "ModuleNotFoundError: No module named 'pynlm'"

```bash
# Install the module
pip install pynlm

# Or install in development mode
pip install -e .
```

#### 2. "ImportError: libpynlm.so: cannot open shared object file"

```bash
# On Linux, you may need to set LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/nlm/build

# On macOS, you may need to set DYLD_LIBRARY_PATH
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/path/to/nlm/build

# Or install the library system-wide
 sudo cp /path/to/nlm/build/*.so /usr/local/lib/
 sudo ldconfig
```

#### 3. "Segmentation fault" or "Bus error"

```python
# Check for common causes
brain = pynlm.createBrain(config)
brain.initialize()

# Validate configuration
if config.get("neuron_count", 1000) > 10000:
    print("Warning: Large neuron count may cause performance issues")

# Start with small simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")
```

#### 4. Slow Performance

```python
import time

# Profile performance
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

print(f"Average step time: {(end_time - start_time) / 1000:.4f}s")
print(f"Steps per second: {1000 / (end_time - start_time):.1f}")

# Optimization suggestions
# - Reduce simulation timestep
# - Disable unused systems
# - Use checkpointing for long simulations
# - Monitor memory usage
```

## References

### Related Documentation

- [NLM Architecture Documentation](/docs/ARCHITECTURE.md)
- [NLM Scientific Background](/docs/SCIENCE.md)
- [NLM Research Methods](/docs/RESEARCH_METHODS.md)

### C++ API Documentation

For detailed C++ API documentation, see:
- [C++ Header Files](src/brain/Brain.hpp, src/agent/AgentBrain.hpp)
- [Implementation Details](src/brain/Brain.cpp, src/agent/AgentBrain.cpp)

### Research Papers

The NLM framework is based on the following research:
- "Phase 6: Final Integration of Artificial Developmental Brains" (in progress)
- "Neural Learning Machine: A Brain-Inspired Spiking Neural Network Simulator" (current implementation)

## Change Log

### Version 0.1.1 (Current)

- Complete refactoring of Python bindings for better organization
- Added comprehensive documentation and examples
- Enhanced error handling and type safety
- Added advanced features for research and performance profiling
- Improved memory management and checkpointing support

### Version 0.1.0 (Initial)

- Basic Python bindings for NLM
- Core brain, agent, and world interfaces
- Basic sensory and motor processing
- Simple configuration management

## Acknowledgments

This Python API is based on the NLM (熙然) artificial brain project, an experimental computational brain system implementing Phase 6 final integration. The project combines:

- Real spiking neural computation with LIF neurons
- Event-driven spike propagation and synaptic delays
- Multiple plasticity mechanisms (STDP, Hebbian, structural)
- Integrated memory, prediction, cognition, and neuromodulation systems
- Developmental plasticity and adaptive learning

## License

MIT License - See LICENSE file for details.

## Copyright

Copyright (c) 2024 NLM Project Authors. All rights reserved.