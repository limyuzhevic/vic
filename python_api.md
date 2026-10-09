# python_api.md

# NLM Python API Documentation

## Overview

NLM provides a comprehensive Python API for interacting with the Neural Learning Machine. The Python bindings are built using pybind11 and provide access to all core brain functionality, advanced cognitive systems, memory systems, neuromodulation, and development capabilities.

## Installation

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

## Basic Usage

### 1. Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### 2. Running a Simple Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### 3. Creating a Simple World

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)
```

### 4. Complete Agent Example

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

## Key Classes and Methods

### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

### SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

## Enumerations

### Neuron Types

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal
```

### Developmental Stages

```python
# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

### Action Types

```python
# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more
```

### Motor Commands

```python
# Low-level motor commands
pynlm.MotorCommand.MoveForward
pynlm.MotorCommand.MoveBackward
pynlm.MotorCommand.TurnLeft
pynlm.MotorCommand.TurnRight
pynlm.MotorCommand.Look
pynlm.MotorCommand.Interact
pynlm.MotorCommand.Wait
```

### World Object Types

```python
# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

## Advanced Systems

### Neural Prediction System

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize prediction system
prediction = pynlm.NeuralPrediction()
prediction.initialize(brain)

# Record sensory state for prediction
vision = pynlm.Vision(8, 8, 3)
prediction.recordSensoryState(vision, step=0)

# Generate prediction
predicted_state = prediction.generatePrediction(step=0)
error = prediction.getPredictionError()
confidence = prediction.getPredictionConfidence()
```

### Action Consequence Prediction

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize action consequence predictor
pred = pynlm.ActionConsequencePredictor()
pred.initialize(brain)

# Record experience
action = pynlm.Action(pynlm.ActionType.MoveForward)
before = pynlm.Vision(8, 8, 3)
after = pynlm.Vision(8, 8, 3)
pred.recordExperience(action, before, after, reward=1.0)

# Predict consequence
predicted_consequence = pred.predictConsequence(action, before)
confidence = pred.getConsequenceConfidence(action, before)
```

### Concept Formation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize concept formation
concept = pynlm.ConceptFormation()
concept.initialize(brain)

# Present experience
pattern = [0.1, 0.5, 0.9, 0.3]
features = [0.2, 0.6, 0.8, 0.4]
concept.presentExperience(pattern, features, reward=1.0, current_time=0.0)

# Get concepts
concept_count = concept.getConceptCount()
concept_id = concept.getMatchingConcept(pattern)
prototype = concept.getConceptPrototype(concept_id)
stability = concept.getConceptStability(concept_id)
```

### Neuromodulators

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create neuromodulators
dopamine = pynlm.Dopamine()
dopamine.initialize(brain)

curiosity = pynlm.Curiosity()
curiosity.initialize(brain)

novelty = pynlm.Novelty()
novelty.initialize(brain)

# Use neuromodulators
dopamine.signalReward(1.0)
dopamine.signalRewardPredictionError(0.1)

curiosity_level = curiosity.getLevel()
curiosity.update(novelty_level=0.5, prediction_error=0.2, dt=0.1)

novelty_level = novelty.getLevel()
novelty.detectNovelty(current_pattern=[0.1, 0.2, 0.3], previous_pattern=[0.0, 0.0, 0.0])
```

## Working Memory

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize working memory
wm = pynlm.NeuralWorkingMemory()
wm.initialize(brain)

# Store information
pattern = [0.1, 0.5, 0.9, 0.3]
wm.store(pattern, strength=1.0)

# Retrieve content
activity_levels = wm.retrieve()
active_traces = wm.getActiveTraces()
memory_neuron_count = wm.getMemoryNeurons()

# Update working memory
wm.update(dt=0.1)

# Competition between memory traces
wm.runCompetition()
winning_neuron = wm.isWinning(neuron_id)
```

## Episodic Memory

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize episodic memory
mem = pynlm.NeuralEpisodicMemory()
mem.initialize(brain)

# Store episode
episode = []  # Array of sensory states
mem.storeEpisode(episode)

# Retrieve similar episodes
query = pynlm.Vision(8, 8, 3)
similar = mem.retrieveSimilar(query, maxResults=5)

# Temporal retrieval
temporal = mem.retrieveTemporal(start_time=0, end_time=100, maxResults=10)

# Location-based retrieval
location = mem.retrieveByLocation(x=10.0, y=10.0, radius=2.0, maxResults=5)

# Replay episode
mem.replayEpisode(episode)

# Consolidation
mem.consolidate(relevanceThreshold=0.5)
```

## Neural Planning

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize neural planner
planner = pynlm.NeuralPlanner()
planner.initialize(brain)

# Plan action
current_state = pynlm.Vision(8, 8, 3)
goal = 0.8
planned_action = planner.planAction(current_state, targetReward=goal)

# Evaluate sequence
actions = [pynlm.ActionType.MoveForward, pynlm.ActionType.TurnLeft]
evaluation = planner.evaluateSequence(actions, current_state)

# Update plans
actual_actions = [pynlm.ActionType.MoveForward, pynlm.ActionType.TurnLeft]
actual_reward = 1.0
planner.updatePlanQuality(actual_actions, actual_reward)
```

## Development System

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get current developmental stage
stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {stage}")

# Get developmental modifiers
stage = brain.getDevelopmentalStage()
excitability = stage.getExcitabilityModifier()
attention_stability = stage.getAttentionStabilityModifier()
working_memory = stage.getWorkingMemoryCapacityModifier()
consolidation = stage.getMemoryConsolidationModifier()

# Update development
agent = pynlm.createAgentBrain(brain)
agent.updateDevelopment(timestep=0.1)

# Get developmental milestone
current_milestone = stage.getCurrentDevelopmentalMilestone()
milestone_reached = stage.hasMilestonePassed(current_milestone)
```

## Spatial Representation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize spatial representation
spatial = pynlm.SpatialRepresentation()
spatial.initialize(brain)

# Record position
spatial.recordPosition(x=10.0, y=10.0, sensoryCues=[0.1, 0.5, 0.9])

# Get predicted position
predicted = spatial.getPredictedPosition()

# Integrate movement
spatial.integrateMovement(dx=1.0, dy=0.0)

# Get position activation
activation = spatial.getPositionActivation(x=10.0, y=10.0)

# Check for place cells
place_neuron_count = len(spatial.getPlaceNeuronsNear(10.0, 10.0, 2.0))
has_place_cells = spatial.hasPlaceCells()
```

## Temporal Relations

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize temporal relations
temporal = pynlm.TemporalRelation()
temporal.initialize(brain)

# Record sequence
event_a = "button_pressed"
event_b = "light_on"
temporal.recordSequence(event_a, event_b, reward=1.0)

# Predict next
predicted_next = temporal.predictNext(event_a)
prediction_confidence = temporal.getPredictionConfidence(event_a)

# Sequence length
sequence_length = temporal.getSequenceLength()
```

## Complete Simulation Example

```python
import pynlm
import time

def run_advanced_simulation():
    """Run a complete simulation with all advanced systems."""
    
    # Create configuration with enhanced settings
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("prediction.horizon", 3)
    config.set("working_memory.capacity", 150)
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.setRandomSeed(12345)
    
    # Initialize agent with all subsystems enabled
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Initialize advanced systems
    prediction = pynlm.NeuralPrediction()
    prediction.initialize(brain)
    
    working_memory = pynlm.NeuralWorkingMemory()
    working_memory.initialize(brain)
    
    episodic_memory = pynlm.NeuralEpisodicMemory()
    episodic_memory.initialize(brain)
    
    concept_formation = pynlm.ConceptFormation()
    concept_formation.initialize(brain)
    
    neural_planner = pynlm.NeuralPlanner()
    neural_planner.initialize(brain)
    
    spatial_repr = pynlm.SpatialRepresentation()
    spatial_repr.initialize(brain)
    
    temporal_rel = pynlm.TemporalRelation()
    temporal_rel.initialize(brain)
    
    # Create neuromodulators
    dopamine = pynlm.Dopamine()
    dopamine.initialize(brain)
    
    serotonin = pynlm.Serotonin()
    serotonin.initialize(brain)
    
    norepinephrine = pynlm.Norepinephrine()
    norepinephrine.initialize(brain)
    
    acetylcholine = pynlm.Acetylcholine()
    acetylcholine.initialize(brain)
    
    curiosity = pynlm.Curiosity()
    curiosity.initialize(brain)
    
    novelty = pynlm.Novelty()
    novelty.initialize(brain)
    
    reward = pynlm.Reward()
    reward.initialize(brain)
    
    prediction_error = pynlm.PredictionError()
    prediction_error.initialize(brain)
    
    # Reset world and start simulation
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    print("Starting advanced NLM simulation with all systems...")
    print(f"Initial developmental stage: {agent.getDevelopmentalStage()}")
    
    # Run simulation
    for step in range(500):
        # World update
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process through agent
        agent.processSensoryInput(percept)
        
        # Update advanced systems with sensory input
        prediction.recordSensoryState(percept.getVision(), step)
        working_memory.store(percept.getVision().getData())
        episodic_memory.storeEpisode([percept.getVision()])
        
        # Update prediction error
        predicted = prediction.generatePrediction(step)
        actual = percept.getVision()
        prediction_error.computeError(predicted, actual)
        
        # Update neuromodulators
        current_reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        reward.signalReward(current_reward)
        dopamine.signalRewardPredictionError(prediction_error.getError())
        curiosity.update(novelty.getLevel(), prediction_error.getError(), 0.1)
        
        # Run brain step
        brain.step(step)
        
        # Motor control
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        agent.applyRewardModulation(current_reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Periodic logging
        if step % 100 == 0:
            print(f"\nStep {step}:")
            print(f"  Position: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")
            print(f"  Development: {agent.getDevelopmentalStage()}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
            print(f"  Working Memory Traces: {working_memory.getActiveTraces()}")
            print(f"  Episodic Episodes: {episodic_memory.getEpisodeCount()}")
            print(f"  Concepts Formed: {concept_formation.getConceptCount()}")
    
    print("\nSimulation completed!")
    
    # Save final state
    brain.save("advanced_simulation_checkpoint.bin")
    print("State saved to advanced_simulation_checkpoint.bin")

# Run the simulation
run_advanced_simulation()
```

## Configuration Options

For detailed configuration options, see `configuration_guide.md`.

## Further Reading

- [NLM_architecture.md](NLM_architecture.md) - System architecture overview
- [advanced_features.md](advanced_features.md) - Advanced cognitive systems
- [development_stages.md](development_stages.md) - Developmental stages
- [memory_systems.md](memory_systems.md) - Memory systems
- [neuromodulation_guide.md](neuromodulation_guide.md) - Neuromodulation
- [example_experiments.md](example_experiments.md) - Experiment examples
