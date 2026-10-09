# NLM Python API Reference

This document provides comprehensive reference for the NLM Python API, including all available classes, methods, and examples.

## Overview

The NLM Python API provides access to the complete Neural Learning Machine neural simulation framework. Version 6.0 implements all neural systems including memory, prediction, cognition, neuromodulation, and development.

## Module Structure

```python
import pynlm

# Core components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Advanced components  
brain = pynlm.createAdvancedBrain(pynlm.createDefaultConfig())
brain = pynlm.createAdvancedBrain(pynlm.createResearchConfig())
```

## Core Classes

### Config
**Configuration management for NLM system**

```python
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
config.set("brain.neuron_count", 1000)
config.has("brain.synapse_density")  # True
config.saveToFile("my_config.json")
```

**Methods:**
- `loadFromFile(filepath)` - Load configuration from JSON file
- `loadFromArgs(argc, argv)` - Load from command line arguments  
- `saveToFile(filepath)` - Save configuration to JSON file
- `has(key)` - Check if configuration key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration entries
- `summary()` - Get summary string of configuration

### Brain
**Central neural simulation brain class**

```python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Basic simulation
for step in range(1000):
    brain.step(step)

# Advanced simulation with all systems
brain = pynlm.createAdvancedBrain(pynlm.createResearchConfig())
brain.initialize()
```

**Core Methods:**
- `initialize()` - Initialize brain with configuration
- `step(currentStep)` - Perform simulation step
- `step(currentStep, currentTime)` - Perform step with timestamp
- `receiveSensoryInput(input)` - Inject sensory input into brain
- `injectCurrent(neuron, current)` - Inject current into specific neuron
- `injectCurrentToNeurons(type, current)` - Inject to all neurons of type
- `produceAction()` - Produce motor action based on neural activity
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file

**Advanced Methods (accessible via brain object):**
- `getWorkingMemory()` - Get working memory system
- `getEpisodicMemory()` - Get episodic memory system  
- `getAssociativeMemory()` - Get associative memory system
- `getPredictionSystem()` - Get prediction system
- `getPlanner()` - Get neural planner
- `getAttention()` - Get attentional selection
- `getConceptFormation()` - Get concept formation
- `getDevelopmentSystem()` - Get development system
- `getDopamine()` - Get dopamine neuromodulator
- `getNovelty()` - Get novelty detector
- `getCuriosity()` - Get curiosity system
- `getPredictionErrorSignal()` - Get prediction error signal

**Properties:**
- `getTotalNeuronCount()` - Total neuron count across all regions
- `getTotalSynapseCount()` - Total synapse count across all regions
- `getActiveNeuronCount()` - Count of active neurons
- `getFiringNeuronCount()` - Count of currently firing neurons
- `getAverageFiringRate()` - Average firing rate across all neurons
- `getExcitationInhibitionRatio()` - E/I balance ratio
- `getTotalSpikeCount()` - Total spike count
- `getDevelopmentalStage()` - Current developmental stage

### AgentBrain
**Agent brain interface connecting NLM brain to world**

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Complete simulation loop
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
```

**Methods:**
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory percept and inject into brain
- `decodeMotorCommand()` - Decode brain motor activity into motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward-based neuromodulation
- `updateDevelopment(timestep)` - Update development system
- `getDevelopmentalStage()` - Get current developmental stage
- `getNeuromodulationLevel()` - Get current neuromodulation level
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error
- `reset()` - Reset agent for new episode
- `getBrain()` - Get the underlying brain

**Configuration Methods:**
- `enableRewardModulation(True/False)` - Enable/disable reward modulation
- `enableStructuralPlasticity(True/False)` - Enable/disable structural plasticity
- `enableDevelopment(True/False)` - Enable/disable development
- `enableCuriosity(True/False)` - Enable/disable curiosity

### SimpleWorld
**Simple 2D world for NLM simulation**

```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    print(f"Step {step}: Reward={result.reward:.2f}, Success={result.success}")
```

**Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset world to initial state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world state
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get current sensory percept
- `getAgentBody()` - Get agent body state
- `addObject(obj)` - Add object to world
- `removeObject(x, y)` - Remove object at position
- `isValidPosition(x, y)` - Check if position is valid
- `getObjectAt(x, y)` - Get object at position
- `getWidth()`, `getHeight()` - Get world dimensions
- `getMaxEnergy()`, `setMaxEnergy(e)` - Get/set max energy
- `getEnergyDecayRate()`, `setEnergyDecayRate(r)` - Get/set energy decay rate
- `getSimulationTime()` - Get current simulation time
- `setRandomSeed(seed)` - Set random seed
- `getRandomSeed()` - Get random seed

### SensoryInput Classes

#### SensoryInput (Base)
**Base class for all sensory input**

```python
sensory = SensoryInput()
data = sensory.getData()
dim = sensory.getDimensions()
timestamp = sensory.getTimestamp()
```

#### Vision
**Visual sensory input**

```python
vision = pynlm.Vision(width=8, height=8, channels=3)
data = [0.1, 0.2, 0.3, ...]  # Flattened array
vision.setData(data)
width = vision.getWidth()
height = vision.getHeight()
channels = vision.getChannels()
```

#### Audio
**Audio sensory input**

```python
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
samples = [0.5, 0.3, 0.8, ...]  # Audio samples
audio.setData(samples)
sample_rate = audio.getSampleRate()
num_samples = audio.getNumSamples()
```

#### InternalSignals
**Internal state signals (body, energy, etc.)**

```python
internal = pynlm.InternalSignals()
internal.addSignal(0.5)  # Add internal state value
internal.addSignal(0.8)
signals = internal.getAllSignals()
```

### Action Classes

#### Action
**Action representation for motor output**

```python
action = pynlm.Action(pynlm.ActionType.MoveForward)
params = action.getParameters()  # Movement parameters
action.setParameters([1.0, 0.5])  # Set parameters
name = action.getName()
```

#### MotorCommand
**Low-level motor command enumeration**

```python
from enum import Enum

class MotorCommand(Enum):
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    LookLeft = 4
    LookRight = 5
    Interact = 6
    Wait = 7
```

### WorldObject
**World object representation**

```python
obj = pynlm.WorldObject(x=5.0, y=5.0, type=pynlm.WorldObjectType.Resource, value=1.0)
obj.type = pynlm.WorldObjectType.Hazard
obj.value = -0.5  # Damage for hazards
obj.active = True
```

### AgentBody
**Agent body state (part of world, not brain)**

```python
body = pynlm.AgentBody()
body.x = 10.0
body.y = 10.0
body.orientation = 0.0
body.energy = 1.0
body.health = 1.0
body.isMoving = True
body.isTurning = False
```

### ActionResult
**Action result from world**

```python
result = ActionResult(reward=1.0, success=True, message="Picked up resource")
print(f"Reward: {result.reward}, Success: {result.success}")
```

## Advanced Classes

### Memory Systems

#### NeuralWorkingMemory
**Neural working memory for transient active information**

```python
working_mem = brain.getWorkingMemory()
working_mem.store(pattern_id)  # Store pattern
recalled = working_mem.recall(pattern_id)  # Recall pattern
active_patterns = working_mem.getActivePatterns()
strength = working_mem.getPatternStrength(pattern_id)
prob = working_mem.getRecallProbability(pattern_id)
```

#### NeuralEpisodicMemory
**Neural episodic memory for experience storage**

```python
episodic_mem = brain.getEpisodicMemory()
episodic_mem.store(experience)  # Store experience
experiences = episodic_mem.recall(timestamp_range)  # Recall by time
episodic_mem.replay(start_time, end_time)  # Replay experiences
consolidation_candidates = episodic_mem.getConsolidationCandidates()
```

#### NeuralAssociativeMemory
**Neural associative memory for pattern associations**

```python
assoc_mem = brain.getAssociativeMemory()
assoc_mem.store(key_pattern, value_pattern)  # Store association
value = assoc_mem.recall(key_pattern)  # Recall associated value
strength = assoc_mem.getAssociationStrength(key, value)
assoc_mem.removeAssociation(key_pattern)
```

#### ConceptFormation
**Neural concept formation for pattern discovery**

```python
concept_form = brain.getConceptFormation()
concept_form.processPattern(pattern)  # Process pattern to form concepts
concept = concept_form.getConcept(concept_id)
stability = concept_form.getConceptStability(concept_id)
prototype = concept_form.getConceptPrototype(concept_id)
active_concepts = concept_form.getActiveConcepts()
```

### Prediction System

#### PredictionSystem
**Temporal sequence prediction and error computation**

```python
pred_sys = brain.getPredictionSystem()
predicted = pred_sys.predict(current_pattern)  # Make prediction
error = pred_sys.getPredictionError(pattern)  # Get prediction error
pred_sys.updatePrediction(pattern, actual)  # Update with actual result
accuracy = pred_sys.getPredictionAccuracy()
pred_sys.reset()
```

#### NeuralPrediction
**Individual prediction model**

```python
prediction = pynlm.NeuralPrediction()
pattern = prediction.getPattern()
confidence = prediction.getConfidence()
prediction.setPattern(pattern)
prediction.setConfidence(0.95)
prob = prediction.getProbability(pattern_id)
```

### Cognitive Systems

#### NeuralPlanner
**Multi-step action planning and evaluation**

```python
planner = brain.getPlanner()
value = planner.evaluateAction(action)  # Evaluate potential action
best_action = planner.getBestAction()  # Get best planned action
planner.addActionPlan(plan)  # Add action plan
active_plans = planner.getActivePlans()
```

#### AttentionalSelection
**Competitive attention mechanism (NOT Transformer QKV)**

```python
attention = brain.getAttention()
selected = attention.select(input_patterns)  # Select focus
pattern = attention.getSelectedPattern()  # Get selected pattern
strength = attention.getSelectionStrength()  # Get selection strength
score = attention.getCompetitionScore(pattern_id)
```

### Development System

#### DevelopmentSystem
**Orchestrates developmental stages**

```python
dev_sys = brain.getDevelopmentSystem()
dev_sys.update(timestep)  # Update development
stage = dev_sys.getDevelopmentalStage()  # Get current stage
dev_sys.setTargetStage(stage)  # Set target stage
progress = dev_sys.getDevelopmentProgress()  # Get progress (0-1)
critical_period = dev_sys.getCriticalPeriodActive()  # Check critical period
```

#### Maturation
**Adult property development**

```python
maturation = Maturation()
maturation.update(timestep)  # Update maturation
growth_rate = maturation.getGrowthRate()
plasticity_change = maturation.getPlasticityChange()
age = maturation.getAge()
```

### Neuromodulation Systems

#### Dopamine
**Reward and reinforcement modulation**

```python
dopamine = brain.getDopamine()
dopamine.update(reward, prediction_error)  # Update with reward/error
level = dopamine.getLevel()  # Get current dopamine level
strength = dopamine.getModulationStrength()  # Get modulation strength
modulation = dopamine.getActionValueModulation()  # Get action value modulation
learning_mod = dopamine.getLearningRateModulation()  # Get learning rate modulation
```

#### Novelty
**Novelty detection for exploration**

```python
novelty = brain.getNovelty()
novelty.update(input_pattern)  # Update with new input
level = novelty.getLevel()  # Get current novelty level
surprise = novelty.getSurpriseScore()  # Get surprise score
threshold = novelty.getNoveltyThreshold()  # Get detection threshold
novelty.setNoveltyThreshold(0.5)  # Set threshold
```

#### Curiosity
**Exploration motivation**

```python
curiosity = brain.getCuriosity()
curiosity.update(environment_state)  # Update with environment
curiosity_level = curiosity.getLevel()  # Get current curiosity level
exploration = curiosity.getExplorationDrive()  # Get exploration drive
motivation = curiosity.getIntrinsicMotivation()  # Get intrinsic motivation
should_explore = curiosity.shouldExplore()  # Check if should explore
```

#### PredictionError
**Error signal computation for learning**

```python
pred_error = brain.getPredictionErrorSignal()
pred_error.update(predicted, actual)  # Update prediction error
level = pred_error.getLevel()  # Get current error level
magnitude = pred_error.getErrorMagnitude()  # Get error magnitude
derivative = pred_error.getTemporalDerivative()  # Get temporal derivative
learning_mod = pred_error.getLearningRateModulation()  # Get learning modulation
```

### Environment System

#### GridWorldEnvironment
**Grid-world environment for testing**

```python
env = pynlm.GridWorldEnvironment(width=10, height=10)
sensory = env.reset()  # Reset environment
sensory = env.step(action)  # Take action
state = env.getState()  # Get current state
reward = env.getLastReward()  # Get last reward
done = env.isDone()  # Check if episode is done
env.addObject(object)  # Add object
env.removeObject(x, y)  # Remove object
obj = env.getObjectAt(x, y)  # Get object at position
```

### Visualization

#### VisualizationInterface
**Neural activity visualization**

```python
viz = VisualizationInterface()
viz.renderBrain(brain)  # Render brain activity
viz.renderNetwork(brain)  # Render neural network structure
viz.renderActivityHeatmap(brain)  # Render activity heatmap
viz.saveVisualization("brain_activity.png")  # Save visualization
stats = viz.getVisualizationStats()  # Get visualization statistics
```

## Specialized Functions

### Factory Functions

#### createDefaultConfig()
```python
config = pynlm.createDefaultConfig()
```

#### createBrain(config)
```python
brain = pynlm.createBrain(config)
```

#### createSimpleWorld()
```python
world = pynlm.createSimpleWorld()
```

#### createAgentBrain(brain)
```python
agent = pynlm.createAgentBrain(brain)
```

#### createAdvancedBrain(config, enableAdvancedFeatures)
```python
brain = pynlm.createAdvancedBrain(config)
brain = pynlm.createAdvancedBrain(pynlm.createResearchConfig())
```

#### createResearchConfig()
```python
config = pynlm.createResearchConfig()
# Optimized for research with many features enabled
```

#### createMultiAgentSimulation(agentTypes, configs)
```python
brains = pynlm.createMultiAgentSimulation(agentTypes, configs)
```

#### saveAllBrains(brains, directory)
```python
pynlm.saveAllBrains([brain1, brain2, brain3], "checkpoints")
```

#### loadAllBrains(filepaths)
```python
brains = pynlm.loadAllBrains(["brain1.bin", "brain2.bin"])
```

### System Functions

#### getSystemStatistics()
```python
stats = pynlm.getSystemStatistics()
```

#### getPerformanceMetrics()
```python
metrics = pynlm.getPerformanceMetrics()
```

#### getResourceUsage()
```python
usage = pynlm.getResourceUsage()
```

## Enumerations

### NeuronType
- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons
- `Modulatory` - Modulatory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal neurons

### SynapseType
- `Excitatory` - Excitatory synapses
- `Inhibitory` - Inhibitory synapses
- `Modulatory` - Modulatory synapses
- `Electrical` - Electrical synapses
- `GapJunction` - Gap junctions

### DevelopmentalStage
- `Initial` - Initial stage
- `CriticalPeriod` - Critical period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

### FiringState
- `Resting` - Resting state
- `Active` - Active firing
- `Refractory` - Refractory period
- `Inhibited` - Inhibited state

### ActionType
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `MoveLeft` - Move left
- `MoveRight` - Move right
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Look` - Look
- `LookUp` - Look up
- `LookDown` - Look down
- `Interact` - Interact
- `Eat` - Eat
- `Drink` - Drink
- `Rest` - Rest
- `Wait` - Wait
- `Custom` - Custom action

### WorldObjectType
- `Empty` - Empty space
- `Resource` - Resource item
- `Hazard` - Hazardous item
- `Wall` - Wall boundary
- `Marker` - Navigation marker

### MotorCommand
- `MoveForward` - Propel forward
- `MoveBackward` - Propel backward
- `TurnLeft` - Rotate left
- `TurnRight` - Rotate right
- `LookLeft` - Pan sensor left
- `LookRight` - Pan sensor right
- `Interact` - Interact with nearby object
- `Wait` - No movement

## Constants

- `NLM_VERSION` - "Phase 6 Final Integration"
- `MAX_NEURONS` - 100000 (maximum neuron count)
- `MAX_SYNAPSES` - 1000000 (maximum synapse count)
- `DEFAULT_SIMULATION_STEPS` - 1000 (default simulation steps)
- `RESEARCH_MODE_ENABLED` - True (research features enabled)

## Examples

### Basic Simulation
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete. Firing neurons: {brain.getFiringNeuronCount()}")
```

### Complete Agent Simulation
```python
import pynlm

def run_agent_simulation(num_steps=1000):
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

### Advanced Research Simulation
```python
import pynlm
import time

# Create research-optimized configuration
config = pynlm.createResearchConfig()

# Create advanced brain with all systems enabled
brain = pynlm.createAdvancedBrain(config)
brain.initialize()

# Create agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
world.reset()
agent.initialize(world)

print("Starting advanced research simulation...")
start_time = time.time()

# Extended simulation with all systems
for step in range(5000):
    world.update(0.05)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step, step * 0.05)
    
    # Agent decision making
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Neuromodulation
    agent.applyRewardModulation(result.reward, 0.0)
    
    # Development updates
    agent.updateDevelopment(0.05)
    
    # Logging
    if step % 500 == 0:
        print(f"Step {step}:")
        print(f"  Spiking neurons: {brain.getFiringNeuronCount()}")
        print(f"  Memory usage: {pynlm.getResourceUsage().memory_mb:.1f} MB")
        print(f"  Prediction accuracy: {brain.getPredictionSystem().getPredictionAccuracy():.3f}")
        print(f"  Development progress: {brain.getDevelopmentSystem().getDevelopmentProgress():.2f}")

end_time = time.time()
print(f"Simulation complete in {end_time - start_time:.1f} seconds")

# Save final state
brain.save("research_simulation_checkpoint.bin")
print("Checkpoint saved")
```

### Multi-Agent Simulation
```python
import pynlm

def create_multi_agent_team(num_agents=5):
    """Create a team of agents with different specializations"""
    configs = []
    
    # Create different agent types
    for i in range(num_agents):
        config = pynlm.createResearchConfig()
        
        # Customize agent based on type
        if i == 0:
            config.set("brain.neuron_count", 3000)
            config.set("neuromod.curiosity.enable", True)
            config.set("neuromod.novelty.enable", True)
            # Exploration specialist
        elif i == 1:
            config.set("brain.neuron_count", 2000)
            config.set("memory.associative.enable", True)
            # Social learning specialist
        elif i == 2:
            config.set("brain.neuron_count", 4000)
            config.set("plasticity.stdp.enable", True)
            # Fast learner specialist
        else:
            config.set("brain.neuron_count", 2500)
            config.set("development.enable", True)
            # General purpose
        
        configs.append(config)
    
    # Create brains for all agents
    brains = pynlm.createMultiAgentSimulation(
        ["exploration", "social", "fast_learner", "general", "general"][:num_agents],
        configs
    )
    
    return brains

# Create team and run simulation
team = create_multi_agent_team(3)
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
world.reset()

print("Multi-agent simulation started")
for step in range(2000):
    world.update(0.1)
    
    # Each agent processes the world
    for i, brain in enumerate(team):
        if brain is None:
            continue
            
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Different agents take different actions
        world.applyMotorCommand(action, world.getSimulationTime())

print(f"Multi-agent simulation complete. Agents processed {len(team)} brains")
```

## Error Handling

### Common Exceptions

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    # ... simulation code
except std::runtime_error as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Validation Functions

```python
# Check configuration before use
if not config.has("brain.neuron_count"):
    print("Warning: neuron count not set, using default")

# Check brain state before operations
if brain.getTotalNeuronCount() == 0:
    print("Error: brain has no neurons")
    # Handle or initialize
```

## Performance Tips

1. **Batch Processing**: Use `BatchProcessor` for multiple simulations
2. **Memory Management**: Clear unused memories periodically
3. **Configuration Caching**: Reuse configurations for multiple brains
4. **Simulation Timing**: Use consistent timestep throughout simulation
5. **Resource Monitoring**: Use `getResourceUsage()` to monitor memory

## Troubleshooting

### Common Issues and Solutions

**"My brain isn't doing anything"
```python
# Solution: Check initialization
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this!

for step in range(100):
    brain.step(step)
```

**"The agent isn't moving"
```python
# Solution: Check world and agent setup
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Ensure world is updating
for step in range(100):
    world.update(0.1)  # This is required!
```

**"Everything is 0"
```python
# Solution: Brains need time to "warm up"
for step in range(1000):  # Try more steps
    brain.step(step)
    
# Also ensure sensory input is connected
agent.processSensoryInput(world.getSensoryPercept())
```

**"ImportError: No module named 'pynlm'"
```bash
pip install --force-reinstall .
```

**"Segmentation faults"
```python
# Solution: Check initialization order
brain = pynlm.createBrain(config)
brain.initialize()  # Must come before any steps

# Ensure reset is called between runs
brain.reset()
brain.initialize()
```

## Additional Resources

### Documentation
- `README.md` - Project overview and basic usage
- `easy_usage.md` - Beginner guide
- `docs/ARCHITECTURE.md` - Technical architecture
- `docs/SCIENCE.md` - Scientific background
- `docs/EXPERIMENTS.md` - Experiment descriptions
- `docs/ROADMAP.md` - Future roadmap
- `docs/LIMITATIONS.md` - Current limitations
- `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 integration audit

### Testing
- `tests/` directory - Unit tests for core components
- `src/experiments/` - Phase 6 integrated experiment
- `benchmark.cpp` - Performance benchmarks

### Community
- GitHub repository: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Discussions and issues: GitHub Issues

## License
MIT License - See `LICENSE` file for details.

---
*This documentation is generated for NLM Python API Version 6.0.0. For the latest updates, visit the official documentation repository.*