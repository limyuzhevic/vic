# NLM Python Bindings Documentation

This document provides comprehensive documentation for the enhanced NLM Python bindings, including all new functionality added to support the full C++ API.

## Overview

The NLM (Neural Learning Machine) Python bindings provide access to the complete C++ neural simulation framework, including:

- **Core Neural Components**: Neurons, synapses, populations, regions
- **Memory Systems**: Working memory, episodic memory, semantic memory, procedural memory, associative memory
- **Prediction System**: Predictive coding and prediction error computation
- **Cognition Systems**: Neural planning, self-model, social learning
- **Development System**: Neural development and maturation
- **Neuromodulation**: Dopamine, acetylcholine, norepinephrine, serotonin, curiosity, novelty, reward
- **Advanced Features**: Checkpoint management, developmental stages, performance optimization

## Quick Start

### Installation

```bash
pip install pynlm
```

Or build from source:

```bash
cd /path/to/nlm
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON
make pynlm
make install
pip install .
```

### Basic Usage

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

## Core Components

### Neuron Class

```python
import pynlm

# Create neuron with ID
neuron = pynlm.Neuron(pynlm.NeuronId(0))

# Configure neuron properties
neuron.setType(pynlm.NeuronType.Excitatory)
neuron.setThreshold(-50.0)
neuron.setRestingPotential(-70.0)
neuron.setLeakConductance(10.0)

# Initialize with random parameters (requires RandomGenerator)
rng = pynlm.RandomGenerator(42)
neuron.initializeRandom(rng)

# Simulate neuron activity
for t in range(100):
    neuron.injectCurrent(5. posteriorInput = 1.0
    fired = neuron.stepLIF(t, 0.1)
    if fired:
        print(f"Neuron fired at time {t}")
```

**Key Methods:**
- `getId()`, `setType()`, `getMembranePotential()`, `setMembranePotential()`
- `injectCurrent()`, `getTotalCurrent()`, `clearTotalCurrent()`
- `stepLIF()`, `checkThreshold()`, `recordSpike()`, `getSpikeHistory()`

### Synapse Class

```python
import pynlm

# Create synapse
synapse = pynlm.Synapse(pynlm.SynapseId(0), pynlm.NeuronId(0), pynlm.NeuronId(1))

# Configure synapse
synapse.setWeight(0.5)
synapse.setDelay(1)
synapse.setType(pynlm.SynapseType.Excitatory)

# Enable plasticity
synapse.enablePlasticity(hebbian=True, stdp=True, rewardModulated=True)

# Update synapse
for t in range(100):
    synapse.step(t)
    synapse.recordPreSpike(t)
    synapse.recordPostSpike(t)
```

### NeuralPopulation Class

```python
import pynlm

# Create population with 100 neurons
population = pynlm.NeuralPopulation(pynlm.PopulationId(0), 100)

# Set neuron type
population.setNeuronType(pynlm.NeuronType.Excitatory)

# Initialize with random parameters
rng = pynlm.RandomGenerator(42)
population.initializeRandom(rng)

# Create neurons with sequential IDs
start_id = pynlm.NeuronId(1000)
population.createNeurons(start_id)

# Step all neurons
for t in range(100):
    population.step(t)
```

### NeuralRegion Class

```python
import pynlm

# Create neural region
region = pynlm.NeuralRegion(pynlm.RegionId(0), "Cortex")

# Set next neuron index
region.setNextNeuronIndex(1000)

# Add population
population_id = region.addPopulation(50, pynlm.NeuronType.Excitatory)

# Add synapses
synapse_id = region.addSynapse(pynlm.NeuronId(1000), pynlm.NeuronId(1001), 0.5, 1)

# Initialize random connectivity
rng = pynlm.RandomGenerator(42)
region.initializeRandomConnectivity(rng, 0.1, 0.5, 0.1)

# Step all components
for t in range(100):
    region.step(t)
```

## Memory Systems

### WorkingMemory Class

```python
import pynlm

# Create working memory
memory = pynlm.WorkingMemory()

# Store values
memory.store(pynlm.NeuronId(1), 0.8)
memory.store(pynlm.NeuronId(2), 0.3)

# Retrieve values
value1 = memory.retrieve(pynlm.NeuronId(1))
value2 = memory.retrieve(pynlm.NeuronId(2))

print(f"Neuron 1 value: {value1}")
print(f"Neuron 2 value: {value2}")

# Check if neuron has stored value
if memory.contains(pynlm.NeuronId(1)):
    print("Neuron 1 has stored value")

# Clear all stored values
memory.clear()
```

### EpisodicMemory Class

```python
import pynlm

# Create episodic memory
episodic_memory = pynlm.EpisodicMemory()

# Create an episodic memory item
episode = pynlm.EpisodicMemoryItem(
    timestamp=100,
    neurons=[pynlm.NeuronId(1), pynlm.NeuronId(2)],
    values=[0.8, 0.3],
    metadata="Initial exploration"
)

# Store the episode
episodic_memory.storeEpisode(episode)

# Retrieve recent episodes
recent_episodes = episodic_memory.getRecentEpisodes(5)
for i, ep in enumerate(recent_episodes):
    print(f"Episode {i}: timestamp={ep.timestamp}, neurons={ep.neurons}")

# Consolidate relevant episodes
episodic_memory.consolidate(0.5)
```

### SemanticMemory Class

```python
import pynlm

# Create semantic memory
semantic_memory = pynlm.SemanticMemory()

# Store facts
semantic_memory.storeFact("predator_risk", "high")
semantic_memory.storeFact("food_location", "center")

# Retrieve facts
predator_risk = semantic_memory.retrieveFact("predator_risk")

print(f"Predator risk: {predator_risk}")

# Check if fact exists
if semantic_memory.hasFact("food_location"):
    print("Food location is known")

# Get all facts
all_facts = semantic_memory.getAllFacts()
for key, value in all_facts:
    print(f"{key}: {value}")
```

### ProceduralMemory Class

```python
import pynlm

# Create procedural memory
procedural_memory = pynlm.ProceduralMemory()

# Learn a skill
skill_neural_pattern = [pynlm.NeuronId(1), pynlm.NeuronId(2), pynlm.NeuronId(3)]
procedural_memory.learnSkill("escape_route", skill_neural_pattern)

# Update proficiency
procedural_memory.updateProficiency("escape_route", 0.1)

# Get skill
skill = procedural_memory.getSkill("escape_route")
if skill:
    print(f"Skill name: {skill.name}")
    print(f"Neural pattern length: {len(skill.neuralPattern)}")
    print(f"Proficiency: {skill.proficiency}")
```

### AssociativeMemory Class

```python
import pynlm

# Create associative memory
associative_memory = pynlm.AssociativeMemory()

# Create association between neurons
associative_memory.associate(pynlm.NeuronId(1), pynlm.NeuronId(2), 0.7)

# Get associated neurons
associated = associative_memory.getAssociations(pynlm.NeuronId(1))
print(f"Neurons associated with 1: {associated}")

# Get association strength
strength = associative_memory.getAssociationStrength(pynlm.NeuronId(1), pynlm.NeuronId(2))
print(f"Association strength: {strength}")

# Update association
associative_memory.updateAssociation(pynlm.NeuronId(1), pynlm.NeuronId(2), 0.05)
```

## Prediction System

```python
import pynlm

# Create prediction system
prediction_system = pynlm.PredictionSystem()

# Create sensory input
vision = pynlm.Vision(8, 8, 3)

# Set data
data = [0.5] * (8 * 8 * 3)
vision.setData(data)

# Make prediction
predicted_state = prediction_system.predictNextState(vision)

# Train with observation
prediction_system.train(vision)

# Get statistics
error = prediction_system.getPredictionError()
confidence = prediction_system.getConfidence()
print(f"Prediction error: {error}")
print(f"Prediction confidence: {confidence}")
```

## Cognition Systems

### NeuralPlanner Class

```python
import pynlm

# Create neural planner
planner = pynlm.NeuralPlanner()

# Set planning depth
planner.setPlanningDepth(5)

# Set current goal
goal = [10.0, 10.0]
planner.setCurrentGoal(goal)

# Plan action given current state
current_state = [5.0, 5.0, 0.0]
planned_action = planner.planAction(current_state, targetReward=0.5)

print(f"Planned action: {planned_action}")

# Evaluate action sequence
action_sequence = [
    pynlm.ActionType.MoveForward,
    pynlm.ActionType.TurnLeft,
    pynlm.ActionType.MoveForward
]

candidate = planner.evaluateSequence(action_sequence, current_state)
print(f"Sequence reward: {candidate.expectedReward}")
```

### SelfModel Class

```python
import pynlm

# Create self-model
self_model = pynlm.SelfModel()

# Record self-action
before_state = [1.0, 2.0, 0.5]
action = pynlm.ActionType.MoveForward
after_state = [1.5, 1.9, 0.5]

self_model.recordSelfAction(action, before_state, after_state)

# Predict consequence of action
predicted_state = self_model.predictActionConsequence(action, before_state)
print(f"Predicted state: {predicted_state}")
```

### SocialLearning Class

```python
import pynlm

# Create social learning
social_learning = pynlm.SocialLearning()

# Record observation of another agent's action
observed_action = pynlm.ActionType.MoveForward
observer_state = [1.0, 2.0, 0.5]
resulting_state = [1.5, 2.0, 0.5]

social_learning.observeAgentAction(observed_action, observer_state, resulting_state)

# Get best action to imitate
current_state = [1.2, 1.8, 0.5]
imitation_action = social_learning.getImitationAction(current_state)
print(f"Best imitation action: {imitation_action}")
```

## Development System

```python
import pynlm

# Create development system
development = pynlm.DevelopmentSystem()

# Set current developmental stage
development.setStage(pynlm.DevelopmentalStage.Initial)
print(f"Current stage: {development.getStage()}")

# Advance to next stage
development.advanceStage()
print(f"Advanced stage: {development.getStage()}")

# Get plasticity modifier
plasticity_modifier = development.getPlasticityModifier()
print(f"Plasticity modifier: {plasticity_modifier}")

# Check critical period
critical_period = development.isCriticalPeriod()
print(f"Is critical period: {critical_period}")
```

## Neuromodulation Systems

### Dopamine Class

```python
import pynlm

# Create dopamine neuromodulator
dopamine = pynlm.Dopamine()

# Set reward level
dopamine.setLevel(0.8)

# Signal reward
dopamine.signalReward(1.0)

# Signal reward prediction error
dopamine.signalRewardPredictionError(0.5)

# Update neuromodulator state
for t in range(100):
    dopamine.update(0.1)
```

### Acetylcholine Class

```python
import pynlm

# Create acetylcholine neuromodulator
ach = pynlm.Acetylcholine()

# Set level
ach.setLevel(0.6)

# Update neuromodulator state
for t in range(100):
    ach.update(0.1)
```

### Curiosity Class

```python
import pynlm

# Create curiosity system
curiosity = pynlm.Curiosity()

# Update curiosity based on novelty and prediction error
novelty = 0.3
prediction_error =  अधिकारी.4
curiosity.update(novelty, prediction_error, 0.1)

print(f"Curiosity level: {curiosity.getLevel()}")
```

### Reward Class

```python
import pynlm

# Create reward signal
reward = pynlm.Reward()

# Add reward
reward.add(0.5)

# Get current value
print(f"Current reward value: {reward.getValue()}")
```

### PredictionError Class

```python
import pynlm

# Create prediction error system
pred_error = pynlm.PredictionError()

# Compute prediction error
pred_error.computeError(0.5, 0.8)

print(f"Prediction error: {pred_error.getError()}")

# Update prediction
pred_error.updatePrediction(0.6)
```

## Advanced Features

### Checkpoint Management

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation steps
for step in range(100):
    brain.step(step)

# Save brain state (checkpoint)
brain.save("checkpoint.bin")

# Reset and reload brain state
brain.reset()
brain.load("checkpoint.bin")

print(f"Total neurons after load: {brain.getTotalNeuronCount()}")
```

### Developmental Stage Management

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Monitor developmental stage changes
print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")

# Update development for several steps
for step in range(50):
    brain.step(step)
    
    if step % 10 == 0:
        print(f"Step {step}: {brain.getDevelopmentalStage()}")
```

### Complete Agent Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration and brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 6. Run simulation loop
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

# Run the simulation
run_agent_simulation(1000)
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
- `Initial` - Initial developmental stage
- `CriticalPeriod` - Critical period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

### FiringState
- `Resting` - Resting state
- `Active` - Active state
- `Refractory` - Refractory state
- `Inhibited` - Inhibited state

### ActionType
- `MoveForward`, `MoveBackward`, `MoveLeft`, `MoveRight`
- `TurnLeft`, `TurnRight`, `Look`, `LookUp`, `LookDown`
- `Interact`, `Eat`, `Drink`, `Rest`, `Wait`, `Custom`

### MotorCommand
- `MoveForward`, `MoveBackward`, `TurnLeft`, `TurnRight`
- `LookLeft`, `LookRight`, `Interact`, `Wait`

### WorldObjectType
- `Empty`, `Resource`, `Hazard`, `Wall`, `Marker`

## Error Messages

**ImportError**
- "cannot import name 'SomeClass'" - Class not available in current version
- Check available classes or use alternative

**RuntimeError**
- "initialization failed" - Ensure `brain.initialize()` is called before `brain.step()`
- "Invalid neuron ID" - Check that neurons are properly initialized with valid IDs
- "Synapse connection failed" - Check that source and destination neurons exist

## Performance Tips

### Memory Management
1. **Clear memory systems periodically** when they are no longer needed
2. **Limit history size** for systems with historical data
3. **Use constant references** for read-only access

### Computational Efficiency
1. **Batch operations** when possible to reduce overhead
2. **Use numpy arrays** for large numerical data
3. **Enable only appropriate subsystems** for your simulation

### Simulation Optimization
1. **Adjust time step sizes** appropriate for your simulation scale
2. **Monitor and optimize memory usage** regularly
3. **Profile code** to identify bottlenecks

## Conclusion

The enhanced NLM Python bindings provide comprehensive access to the full C++ neural simulation framework. This documentation covers all available functionality from basic neural simulation to advanced cognitive modeling, prediction systems, and neuromodulation.

The Python API maintains the power and flexibility of the C++ implementation while providing a more accessible and user-friendly interface for researchers and developers. Whether you're building simple neural models or complex agent simulations, the NLM Python bindings provide all the tools you need.

For further questions or support, please refer to the main NLM documentation and the C++ source code for implementation details.
