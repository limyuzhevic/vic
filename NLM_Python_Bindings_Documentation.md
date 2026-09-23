# NLM Python Bindings - Complete Documentation

This document provides comprehensive information for using the NLM (Neural Learning Machine) Python bindings.

## Overview

The NLM Python bindings (`pynlm`) provide full access to the complete NLM neural simulation framework from Python. This enables you to:

- Create and manage neural brains
- Configure simulation parameters
- Process sensory inputs and produce actions
- Access neuromodulation signals
- Use memory systems for experience storage
- Work with advanced cognitive functions

## Installation

```bash
# The bindings are built as part of the main CMake project
mkdir build
cd build
cmake ..
make -j4

# The pynlm module will be available in the build directory
# or can be installed via pip (if setup.py is present)
```

## Quick Start Examples

### 1. Basic Brain Creation

```python
import pynlm as nlm

# Create a brain with default configuration
brain = nlm.createBrainWithDefaultConfig()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Initialize the brain
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step, step * 0.001)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Reset for next experiment
brain.reset()
```

### 2. Complete Agent Setup

```python
import pynlm as nlm

# Create a world with default setup
world = nlm.createWorldWithSetup(width=300, height=300, 
                                  visionWidth=30, visionHeight=30)

# Create a complete agent setup (brain + agent brain interface)
agent_brain = nlm.createCompleteAgentSetup(world)

# Create a sensory processing pipeline
sensory_agent = nlm.createSensoryPipeline(world)

print("Agent setup complete!")
print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Agent body: ({sensory_agent.getBrain().getTotalNeuronCount()} neurons)")
```

### 3. Working with Neurons and Synapses

```python
import pynlm as nlm

# Create a brain
brain = nlm.createBrainWithDefaultConfig()
brain.initialize()

# Get the first region
region = brain.getRegion(brain.getRegionIds()[0])

# Explore neurons
neurons = region.getAllNeurons()
print(f"Total neurons: {len(neurons)}")

if neurons:
    neuron = neurons[0]
    print(f"Neuron ID: {neuron.getId()}")
    print(f"Neuron type: {neuron.getType()}")
    print(f"Membrane potential: {neuron.getMembranePotential()}")
    print(f"Is firing: {neuron.isFiring()}")

# Explore synapses
synapses = region.getSynapses()
print(f"Total synapses: {len(synapses)}")

if synapses:
    synapse = synapses[0]
    print(f"Synapse from: {synapse.getSourceNeuron().getId()} -> {synapse.getDestinationNeuron().getId()}")
    print(f"Synapse weight: {synapse.getWeight()}")
    print(f"Is excitatory: {synapse.isExcitatory()}")
```

### 4. Memory Systems

```python
import pynlm as nlm

# Create working memory for temporary information
working_memory = nlm.NeuralWorkingMemory()

# Store and recall information
working_memory.store("current_position", [10.0, 20.0])
working_memory.store("temperature", 25.5)

position = working_memory.recall("current_position")
temperature = working_memory.recall("temperature")

print(f"Position: {position}")
print(f"Temperature: {temperature}")
print(f"Available keys: {working_memory.getKeys()}")

# Create episodic memory for experiences
episodic_memory = nlm.NeuralEpisodicMemory()

# Record an experience
percept = nlm.SensoryPercept()
action = nlm.Action(nlm.ActionType.MoveForward, [1.0, 0.0])
episodic_memory.storeExperience(1000.0, percept, action, 0.5)

# Get recent experiences
recent = episodic_memory.getRecentExperiences(5)
print(f"Recorded {len(recent)} experiences")

# Create associative memory for pattern associations
assoc_memory = nlm.NeuralAssociativeMemory()

# Associate input pattern to output pattern
input_pattern = [1.0, 0.0, 0.0]
output_pattern = [0.0, 1.0, 0.0]
assoc_memory.associate(input_pattern, output_pattern, 0.8)

# Recall patterns
recall = assoc_memory.recallPattern(input_pattern)
print(f"Recalled pattern: {recall}")
```

### 5. Neuromodulation

```python
import pynlm as nlm

# Create neuromodulation signals
dopamine = nlm.Dopamine()
 acetylcholine = nlm.Acetylcholine()
 norepinephrine = nlm.Norepinephrine()
 serotonin = nlm.Serotonin()
 curiosity = nlm.Curiosity()
 novelty = nlm.Novelty()
 prediction_error = nlm.PredictionError()

# Set neuromodulation levels
 dopamine.setLevel(0.7)
 curiosity.setCuriosityLevel(0.5)
 novelty.setLevel(0.3)
 prediction_error.setPredictionErrorLevel(0.2)

print(f"Dopamine level: {dopamine.getLevel()}")
print(f"Curiosity level: {curiosity.getCuriosityLevel()}")
print(f"Novelty level: {novelty.getNoveltyLevel()}")
print(f"Prediction error level: {prediction_error.getPredictionErrorLevel()}")
```

### 6. Sensory Processing

```python
import pynlm as nlm

# Create a world
world = nlm.createWorldWithSetup(width=300, height=300)

# Create a complete agent setup
agent = nlm.createCompleteAgentSetup(world)

# Create sensory inputs and process them
vision = nlm.Vision(width=30, height=30, channels=3)
audio = nlm.Audio(sampleRate=44100, numSamples=1024)
internal = nlm.InternalSignals()

# Set up some dummy sensory data
vision_data = [0.1] * (30 * 30 * 3)
vision.setData(vision_data)

audio_data = [0.0] * 1024
for i in range(0, 1024, 4):
    audio_data[i] = 0.5  # Simple beep

audio.setData(audio_data)

# Create a percept
sensory_percept = nlm.SensoryPercept()
sensory_percept.setVision(vision)
sensory_percept.setAudio(audio)
sensory_percept.setInternal(internal)
sensory_percept.setTimestamp(0.0)

# Process the sensory input
agent.processSensoryInput(sensory_percept)

# Get the agent's decision
motor_command = agent.decodeMotorCommand()
action = nlm.Action(nlm.ActionType.Wait, [])

print(f"Processed sensory input with {len(vision_data)} vision pixels")
print(f"Audio data length: {len(audio_data)} samples")
print(f"Action decision: {action.getName()}")
```

### 7. Advanced Configuration

```python
import pynlm as nlm

# Create a brain with custom configuration
config = nlm.Config()

# Set various configuration options
config.set("simulation.steps_per_second", 200)
config.set("brain.region_count", 2)
config.set("brain.population_size", 200)
config.set("brain.initial_excitatory_ratio", 0.7)
config.set("brain.initial_inhibitory_ratio", 0.3)
config.set("brain.synaptic_weight_mean", 0.6)
config.set("brain.synaptic_weight_std", 0.15)
config.set("brain.connection_probability", 0.2)
config.set("brain.enable_structural_plasticity", True)
config.set("brain.enable_reward_modulation", True)
config.set("brain.enable_curiosity", True)
config.set("brain.enable_novelty_detection", True)
config.set("brain.enable_prediction_system", True)
config.set("brain.enable_cognitive_planning", True)
config.set("brain.enable_concept_formation", True)
config.set("brain.development.stage", "CriticalPeriod")

# Create and initialize brain
brain = nlm.Brain(config)
brain.initialize()

print("Custom configuration created successfully")
print(f"Brain status: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
```

## API Reference

### Core Types

#### NeuronId, SynapseId, RegionId, PopulationId
Unique identifiers for neural components.

#### NeuronType
```python
# Available neuron types
NeuronType.Excitatory      # Excitatory neurons
NeuronType.Inhibitory      # Inhibitory neurons
NeuronType.Modulatory      # Modulatory neurons
NeuronType.Sensory         # Sensory neurons
NeuronType.Motor           # Motor neurons
NeuronType.Internal        # Internal neurons
```

#### SynapseType
```python
# Available synapse types
SynapseType.Excitatory     # Chemical excitatory synapse
SynapseType.Inhibitory     # Chemical inhibitory synapse
SynapseType.Modulatory     # Chemical modulatory synapse
SynapseType.Electrical    # Electrical synapse
SynapseType.GapJunction   # Gap junction synapse
```

#### DevelopmentalStage
```python
# Available developmental stages
DevelopmentalStage.Initial         # Initial development
DevelopmentalStage.CriticalPeriod # Critical learning period
DevelopmentalStage.Maturation      # Maturation phase
DevelopmentalStage.Adult           # Adult stage
DevelopmentalStage.Aging           # Aging phase
```

### Main Classes

#### Brain
The central neural simulation brain class.

**Constructor**: `Brain(config)` - Create a brain with configuration
**Methods**:
- `initialize()` - Initialize the brain
- `step(currentStep, currentTime)` - Perform simulation step
- `receiveSensoryInput(input)` - Inject sensory input
- `produceAction()` - Produce motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file
- `addRegion(name)` - Add a neural region
- `getRegion(id)` - Get a region by ID
- Many more methods for statistics and status

#### AgentBrain
Interface connecting NLM brain to world.

**Constructor**: `AgentBrain(brain)` - Create with brain
**Methods**:
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward modulation
- `updateDevelopment(timestep)` - Update development
- Many neuromodulation and development methods

#### SimpleWorld
Simple 2D world for NLM simulation.

**Constructor**: `SimpleWorld()` - Create empty world
**Methods**:
- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `update(timestep)` - Update world simulation
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get current sensory percept
- `getAgentBody()` - Get agent body state
- Many methods for object and agent management

### Factory Functions

#### createBrainWithDefaultConfig()
Creates a brain with optimized default configuration.

#### createCompleteAgentSetup(world)
Creates a complete agent setup with brain, world, and agent brain interface.

#### createWorldWithSetup(width, height, visionWidth, visionHeight)
Creates a world with default setup and configuration.

#### createSensoryPipeline(world)
Creates a sensory processing pipeline with vision, audio, and internal signals.

## Common Patterns

### Pattern 1: Continuous Learning Loop

```python
import pynlm as nlm

# Create world and agent
world = nlm.createWorldWithSetup(width=300, height=300)
agent = nlm.createCompleteAgentSetup(world)

# Main learning loop
for episode in range(100):
    # Reset for new episode
    agent.reset()
    world.reset()
    
    # Episode loop
    for step in range(200):
        # Get current perception
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Take action based on brain output
        motor_command = agent.decodeMotorCommand()
        action = nlm.Action(motor_command, [])
        
        # Apply action in world
        result = world.applyMotorCommand(action.getType(), step * 0.001)
        
        # Apply reward modulation
        reward = result.reward if result else 0.0
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.001)
        
        if step % 50 == 0:
            print(f"Episode {episode}, Step {step}, "
                  f"Firing: {agent.getBrain().getFiringNeuronCount()}")

print("Learning completed!")
```

### Pattern 2: Multi-Agent System

```python
import pynlm as nlm

# Create shared world
world = nlm.createWorldWithSetup(width=500, height=500)

# Create multiple agents
agents = []
for i in range(3):
    # Offset each agent
    agent = nlm.createCompleteAgentSetup(world)
    agent.getBrain().addRegion(f"Agent_{i}")
    agents.append(agent)

print(f"Created {len(agents)} agents in shared world")

# Run multi-agent simulation
for episode in range(50):
    for agent_idx, agent in enumerate(agents):
        agent.reset()
        
        # Each agent behaves differently
        if agent_idx == 0:
            # Agent 1: Explore
            target_x, target_y = 450, 450
            behavior = "EXPLORATION"
        elif agent_idx == 1:
            # Agent 2: Exploit
            target_x, target_y = 100, 100
            behavior = "EXPLOITATION"
        else:
            # Agent 3: Balance
            import random
            target_x, target_y = random.uniform(200, 300), random.uniform(200, 300)
            behavior = "BALANCING"
        
        # Agent moves toward target
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        motor_command = agent.decodeMotorCommand()
        action = nlm.Action(motor_command, [target_x, target_y])
        
        result = world.applyMotorCommand(action.getType(), episode * 0.001)
        
        if episode % 10 == 0 and agent_idx == 0:
            print(f"Agent {agent_idx} ({behavior}): {action.getName()} -> {result.message if result else 'No result'}")
```

### Pattern 3: Memory-Enhanced Learning

```python
import pynlm as nlm

# Create agent with memory systems
world = nlm.createWorldWithSetup(width=300, height=300)
agent = nlm.createCompleteAgentSetup(world)

# Get memory systems
working_memory = agent.getBrain().getWorkingMemory()  # If available
episodic_memory = agent.getBrain().getEpisodicMemory()  # If available
assoc_memory = agent.getBrain().getAssociativeMemory()  # If available

# Learning loop with memory
for episode in range(200):
    # Store episode start state
    working_memory.store(f"episode_{episode}_start", {
        'time': episode,
        'neuron_count': agent.getBrain().getTotalNeuronCount(),
        'firing_rate': agent.getBrain().getAverageFiringRate()
    })
    
    # Run episode
    agent.reset()
    
    for step in range(100):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Use working memory for decision making
        current_state = working_memory.recall(f"episode_{episode}_start")
        
        # Store experience in episodic memory
        action = agent.decodeMotorCommand()
        reward = 0.5  # Simplified reward
        episodic_memory.storeExperience(step * 0.001, percept, action, reward)
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, agent.getPredictionError())
    
    # Use episodic memory for replay/consolidation
    recent_experiences = episodic_memory.getRecentExperiences(5)
    
    # Use associative memory for pattern completion
    # (simplified example)
    if episode % 50 == 0:
        print(f"Episode {episode}: Stored {len(recent_experiences)} experiences")

print("Memory-enhanced learning completed!")
```

## Performance Tips

1. **Reuse Brains**: Create a brain once and reset it for multiple simulations to avoid re-initialization overhead.

2. **Batch Processing**: Process multiple sensory inputs in sequence rather than creating new objects each time.

3. **Memory Management**: Clear memory systems periodically to prevent memory buildup.

4. **Configuration Caching**: Reuse configuration objects for multiple brain instances.

5. **Parallel Processing**: Use multiple threads for independent agent simulations.

## Troubleshooting

### Common Issues and Solutions

1. **Import Error**
```python
# Make sure the pynlm module is in your Python path
import sys
sys.path.insert(0, '/path/to/build/directory')
import pynlm as nlm
```

2. **Simulation Instability**
```python
# Check configuration parameters
config = nlm.Config()
config.set("simulation.steps_per_second", 50)  # Reduce timestep
config.set("brain.population_size", 50)        # Reduce neuron count
config.set("brain.connection_probability", 0.05)  # Reduce connectivity
```

3. **Memory Issues**
```python
# Clear memory systems periodically
if hasattr(agent_brain, 'getWorkingMemory'):
    working_memory = agent_brain.getWorkingMemory()
    working_memory.clear()
```

4. **Performance Issues**
```python
# Profile your code
import cProfile, pstats
import io

pr = cProfile.Profile()
pr.enable()

# Your simulation code here

pr.disable()
s = io.StringIO()
ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
ps.print_stats()
print(s.getvalue())
```

## License and Acknowledgements

The NLM Python bindings are part of the NLM (Neural Learning Machine) project, an experimental artificial developmental brain.

For more information about the NLM project, visit:
- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Documentation](https://nlm.readthedocs.io)

The Python bindings provide a bridge between the high-performance C++ neural simulation framework and the accessibility of Python programming.
