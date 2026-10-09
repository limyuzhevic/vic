# advanced_features.md

# Advanced NLM Features

## Overview

NLM Phase 6 implements comprehensive advanced cognitive systems that enable complex behaviors to emerge from neural dynamics. This document covers the advanced features including prediction systems, concept formation, planning, spatial and temporal reasoning, and their integration.

## Prediction System

### Neural Prediction (src/prediction/NeuralPrediction.hpp)

The Neural Prediction system enables temporal sequence learning and multi-step prediction capabilities.

**Key Features:**
- **Temporal Prediction**: Learns that state A predicts state B
- **Action Consequence Prediction**: Associates actions with their effects
- **Multi-step Prediction**: Chains predictions to anticipate distant futures
- **Confidence Tracking**: Measures prediction certainty
- **Error Signals**: Computes prediction errors for learning

**API Usage:**

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize prediction system
prediction = pynlm.NeuralPrediction()
prediction.initialize(brain)

# Configure prediction settings
prediction.enableTemporalPrediction(True)
prediction.enableActionConsequencePrediction(True)
prediction.setSequenceMemorySize(100)
prediction.setPredictionHorizon(5)

# Record sensory state for temporal learning
vision = pynlm.Vision(8, 8, 3)
prediction.recordSensoryState(vision, step=0)

# Generate prediction
predicted_state = prediction.generatePrediction(step=0)
prediction_error = prediction.getPredictionError()
confidence = prediction.getPredictionConfidence()

# Multi-step prediction
for future_step in range(1, 4):
    future_pred = prediction.predictMultipleSteps(0, future_step)
    print(f"Step {future_step}: confidence={prediction.getPredictionConfidence()}")

# Update with actual observation
prediction.updateWithObservation(actual_state, step=0)

# Predict action consequences
action = pynlm.Action(pynlm.ActionType.MoveForward)
predicted_consequence = prediction.predictActionConsequence(action, current_state)
```

### Action Consequence Predictor (src/prediction/ActionConsequencePredictor.hpp)

Learns action->consequence mappings through experience.

**Key Features:**
- **Experience Recording**: Stores (action, before, after, reward) tuples
- **Consequence Prediction**: Predicts effects of potential actions
- **Action Quality**: Learns which actions work in which states
- **Update Mechanisms**: Refines predictions based on actual outcomes

**API Usage:**

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
before_state = pynlm.Vision(8, 8, 3)
after_state = pynlm.Vision(8, 8, 3)
pred.recordExperience(action, before_state, after_state, reward=1.0)

# Predict consequence
consequence = pred.predictConsequence(action, current_state)
confidence = pred.getConsequenceConfidence(action, current_state)

# Update with actual outcome
pred.updatePrediction(action, predicted_consequence, actual_consequence, actual_reward)

# Get action quality
quality = pred.getActionQuality(action)
```

### PredictionErrorSignal (src/prediction/PredictionErrorSignal.hpp)

Computes prediction errors that drive learning through neuromodulation.

**Key Features:**
- **Error Computation**: Measures difference between predicted and actual values
- **Error Components**: Identifies what aspect was unexpected
- **Surprise Detection**: Determines if events are surprising
- **Neuromodulation Integration**: Provides error signals to learning systems

**API Usage:**

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize prediction error signal
error_signal = pynlm.PredictionErrorSignal()
error_signal.initialize(brain)

# Configure surprise threshold
error_signal.setSurpriseThreshold(0.7)

# Compute prediction error
predicted = [0.1, 0.2, 0.3, 0.4]
actual = [0.15, 0.25, 0.35, 0.45]
error = error_signal.computeError(predicted, actual)
components = error_signal.getErrorComponents()
modulation_signal = error_signal.getModulationSignal()

# Track error history
error_signal.recordError(error, step=0)
error_history = error_signal.getErrorHistory()

# Check for surprising events
is_surprising = error_signal.isSurprising()
```

## Concept Formation System

### ConceptFormation (src/cognition/ConceptFormation.hpp)

Discovers patterns and forms categories without predefined labels.

**Key Features:**
- **Pattern Discovery**: Groups similar patterns into concepts
- **Prototype Formation**: Creates average representations
- **Stability Measurement**: Tracks concept consistency
- **Generalization**: Applies learned concepts to new variations
- **Concept Merging**: Combines similar concepts

**API Usage:**

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize concept formation
concept = pynlm.ConceptFormation()
concept.initialize(brain)

# Present experiences
pattern1 = [0.1, 0.5, 0.9, 0.3]
features1 = [0.2, 0.6, 0.8, 0.4]
concept.presentExperience(pattern1, features1, reward=1.0, current_time=0.0)

pattern2 = [0.15, 0.55, 0.85, 0.35]
features2 = [0.25, 0.65, 0.75, 0.45]
concept.presentExperience(pattern2, features2, reward=0.8, current_time=1.0)

pattern3 = [0.9, 0.8, 0.7, 0.6]
features3 = [0.8, 0.7, 0.6, 0.5]
concept.presentExperience(pattern3, features3, reward=2.0, current_time=2.0)

# Access concepts
concept_count = concept.getConceptCount()
concept_id = concept.getMatchingConcept(pattern1)
prototype = concept.getConceptPrototype(concept_id)
instances = concept.getConceptInstances(concept_id)
stability = concept.getConceptStability(concept_id)

# Check novelty and similarity
is_novel = concept.isNovel(pattern1)
similarity = concept.computeSimilarity(pattern1, pattern2)

# Get specific concept
found_concept = concept.getConcept(concept_id)

# Update concept with new instance
concept.updateConcept(concept_id, new_pattern, new_features, new_reward)

# Concept operations
concept.mergeConcepts(concept_a, concept_b)
concept.clear()
```

## Planning System

### NeuralPlanner (src/cognition/NeuralPlanner.hpp)

Multi-step action planning with evaluation of potential futures.

**Key Features:**
- **Action Planning**: Plans sequences to achieve goals
- **Sequence Evaluation**: Assesses quality of action sequences
- **Planning Depth**: Configures look-ahead horizon
- **Adaptive Planning**: Updates plans based on outcomes
- **Goal Setting**: Specifies target rewards or states

**API Usage:**

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize neural planner
planner = pynlm.NeuralPlanner()
planner.initialize(brain)

# Configure planning
planner.setPlanningDepth(3)
planner.setCurrentGoal(0.8)  # Target reward

# Plan action
current_state = pynlm.Vision(8, 8, 3)
planned_action = planner.planAction(current_state, targetReward=0.7)

# Evaluate potential sequences
actions = [
    pynlm.ActionType.MoveForward,
    pynlm.ActionType.TurnLeft,
    pynlm.ActionType.Interact
]
evaluation = planner.evaluateSequence(actions, current_state)

# Set action quality function
def action_quality(action):
    # Custom action evaluation logic
    return 0.5 if action == pynlm.ActionType.MoveForward else 0.3

planner.setActionQuality(pynlm.ActionType.MoveForward, 0.9)

# Update planning based on outcomes
actual_actions = [pynlm.ActionType.MoveForward, pynlm.ActionType.TurnLeft]
actual_reward = 1.5
planner.updatePlanQuality(actual_actions, actual_reward)

# Access planning state
confidence = planner.getPlanningConfidence()
current_goal = planner.getCurrentGoal()
is_successful = planner.wasRecentPlanSuccessful()
```

## Spatial Reasoning System

### SpatialRepresentation (src/cognition/SpatialRepresentation.hpp)

Place cell formation and spatial navigation with path integration.

**Key Features:**
- **Position Recording**: Stores experienced positions with sensory cues
- **Path Integration**: Maintains position estimate based on movement
- **Place Cells**: Forms neural representations of locations
- **Grid Cells**: Detects periodic spatial patterns
- **Spatial Prediction**: Predicts positions based on movement

**API Usage:**

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize spatial representation
spatial = pynlm.SpatialRepresentation()
spatial.initialize(brain)

# Record position with sensory cues
sensory_cues = [0.1, 0.5, 0.9, 0.2, 0.6]
spatial.recordPosition(x=10.0, y=10.0, sensoryCues=sensoryCues)

# Get predicted position (path integration)
predicted = spatial.getPredictedPosition()
print(f"Predicted position: {predicted[0]:.1f}, {predicted[1]:.1f}")

# Integrate movement based on action
spatial.integrateMovement(dx=2.0, dy=1.0)

# Get position activation (place cell firing)
activation = spatial.getPositionActivation(x=12.0, y=11.0)
print(f"Place cell activation at (12, 11): {activation:.3f}")

# Find nearby place cells
nearby_place_cells = spatial.getPlaceNeuronsNear(x=12.0, y=11.0, radius=2.0)
print(f"Number of nearby place cells: {len(nearby_place_cells)}")

# Check spatial properties
has_place_cells = spatial.hasPlaceCells()
grid_spacing = spatial.getGridSpacing()
```

## Temporal Reasoning System

### TemporalRelation (src/cognition/TemporalRelation.hpp)

Temporal sequence learning and prediction for understanding cause and effect.

**Key Features:**
- **Sequence Recording**: Stores event relationships (A → B)
- **Temporal Prediction**: Predicts what comes next/after
- **Confidence Tracking**: Measures temporal prediction certainty
- **Sequence Length**: Tracks learned temporal patterns

**API Usage:**

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize temporal relations
temporal = pynlm.TemporalRelation()
temporal.initialize(brain)

# Record temporal sequences
event_a = "button_pressed"
event_b = "light_on"
temporal.recordSequence(event_a, event_b, reward=1.0)

# Temporal prediction
predicted_next = temporal.predictNext(event_a)
print(f"After '{event_a}', predict: '{predicted_next}'")

predicted_previous = temporal.predictPrevious(event_b)
print(f"Before '{event_b}', predict: '{predicted_previous}'")

# Get prediction confidence
confidence = temporal.getPredictionConfidence(event_a)
print(f"Confidence in '{event_a} → next': {confidence:.3f}")

# Check sequence statistics
sequence_length = temporal.getSequenceLength()
print(f"Total sequences learned: {sequence_length}")
```

## Working Memory System

### NeuralWorkingMemory (src/memory/NeuralWorkingMemory.hpp)

Persistent activity system for maintaining information across time steps.

**Key Features:**
- **Pattern Storage**: Maintains neural patterns across simulation steps
- **Activity Maintenance**: Persistent neural firing without continuous input
- **Competitive Dynamics**: Winner-take-most for selective retention
- **Rehearsal**: Strengthens important memories
- **Capacity Management**: Limits memory load

**API Usage:**

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize working memory
wm = pynlm.NeuralWorkingMemory()
wm.initialize(brain)

# Configure working memory
wm.setCapacity(100)
wm.setDecayRate(0.01)

# Store patterns
pattern1 = [0.1, 0.5, 0.9, 0.3]
wm.store(pattern1, strength=1.0)

pattern2 = [0.9, 0.8, 0.7, 0.6]
wm.store(pattern2, strength=0.8)

# Store specific neuron activations
neuron_id = 42
wm.storeToNeuron(neuron_id, activation=0.7)

# Retrieve working memory content
activity_levels = wm.retrieve()
print(f"Working memory activity: {len(activity_levels)} traces")

# Check what neurons are in working memory
is_in_memory = wm.contains(neuron_id)
activation_level = wm.getNeuronActivation(neuron_id)

# Update working memory (maintain/decay)
wm.update(dt=0.1)

# Competition between memory traces
wm.runCompetition()
winning_neuron = wm.isWinning(neuron_id)

# Rehearse important memories
wm.strengthenMemory(factor=1.5)

# Access statistics
active_traces = wm.getActiveTraces()
current_capacity = wm.getCapacity()
current_decay = wm.getDecayRate()
total_activity = wm.getMemoryActivity()
```

## Episodic Memory System

### NeuralEpisodicMemory (src/memory/NeuralEpisodicMemory.hpp)

Experience storage with temporal, spatial, and causal associations.

**Key Features:**
- **Episode Storage**: Stores complete experiences with all sensory data
- **Pattern Completion**: Retrieves full episodes from partial cues
- **Temporal Indexing**: Organizes episodes by time
- **Spatial Association**: Links episodes to locations
- **Replay**: Reactivates patterns for consolidation

**API Usage:**

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize episodic memory
mem = pynlm.NeuralEpisodicMemory()
mem.initialize(brain)

# Configure memory limits
mem.setMaxEpisodes(1000)
mem.enableReplay(True)

# Create episode data
episode1 = []  # List of sensory observations over time
episode1.append(pynlm.Vision(8, 8, 3))
episode1.append(pynlm.Vision(8, 8, 3))
episode1.append(pynlm.Vision(8, 8, 3))

mem.storeEpisode(episode1)

# Create more episodes
for i in range(5):
    episode = []
    for j in range(10):
        # Create random sensory pattern
        data = [0.1 * j + 0.05 * (i % 3) for _ in range(32)]
        vision = pynlm.Vision(8, 8, 3)
        vision.setData(data)
        episode.append(vision)
    mem.storeEpisode(episode)

# Retrieve similar episodes
query = pynlm.Vision(8, 8, 3)
similar_episodes = mem.retrieveSimilar(query, maxResults=5)
print(f"Found {len(similar_episodes)} similar episodes")

# Temporal retrieval
temporal_episodes = mem.retrieveTemporal(startTime=100, endTime=200, maxResults=10)
print(f"Retrieved {len(temporal_episodes)} episodes from time 100-200")

# Spatial retrieval
location_episodes = mem.retrieveByLocation(x=10.0, y=10.0, radius=2.0, maxResults=5)
print(f"Found {len(location_episodes)} episodes at location (10, 10)")

# Action-based retrieval
action_episodes = mem.retrieveAfterAction(pynlm.ActionType.MoveForward, maxResults=3)
print(f"Found {len(action_episodes)} episodes after moving forward")

# Replay specific episode
mem.replayEpisode(episode1)

# Replay sequence for consolidation
important_episodes = mem.getEpisodesForReplay(10)
mem.replaySequence(important_episodes)

# Memory management
mem.updateRelevance(episode_id, relevanceDelta=0.1)
mem.consolidate(relevanceThreshold=0.5)

# Access memory statistics
total_episodes = mem.getEpisodeCount()
recent = mem.getRecentEpisodes(5)
average_reward = mem.getAverageReward()
```

## Neuromodulation System

### Neuromodulator Base Class

All neuromodulators inherit from this abstract base class.

**Key Features:**
- **Abstract Interface**: Common methods for all neuromodulators
- **Level Control**: Sets neuromodulator concentration/level
- **Plasticity Effects**: Modulates synaptic plasticity
- **State Updates**: Decays or changes over time

**API Usage:**

```python
import pynlm

# Neuromodulator interface (use specific subclasses)
neuromod = pynlm.Neuromodulator()  # Abstract base

# Set neuromodulator level
neuromod.setLevel(0.5)

# Get current level
level = neuromod.getLevel()

# Get plasticity factor (how it affects learning)
plasticity_factor = neuromod.getPlasticityFactor()

# Update neuromodulator state
neuromod.update(dt=0.1)
```

### All Neuromodulators

NLM implements 8 specialized neuromodulators:

#### Dopamine
- **Function**: Reward prediction error, reinforcement learning
- **Effects**: Neural excitability, STDP scaling, action selection

#### Serotonin
- **Function**: Mood regulation, attention, learning, avoidance
- **Effects**: Attention modulation, avoidance learning

#### Norepinephrine
- **Function**: Arousal, attention, working memory
- **Effects**: Attention enhancement, working memory

#### Acetylcholine
- **Function**: Attention, memory consolidation, sleep
- **Effects**: Memory consolidation, cortical activation

#### Curiosity
- **Function**: Exploration motivation from novelty and prediction error
- **Effects**: Exploration drive, behavioral flexibility

#### Novelty
- **Function**: Detection of new sensory patterns
- **Effects**: Curiosity input, novelty-based learning

#### PredictionError
- **Function**: Error signal for prediction learning
- **Effects**: Learning modulation, attention focus

#### Reward
- **Function**: Direct reward signaling
- **Effects**: Immediate reinforcement, motivation

**All neuromodulators follow the same API pattern:**

```python
# Create and initialize
neuromod = pynlm.Dopamine()  # or Serotonin, Norepinephrine, etc.
neuromod.initialize(brain)

# Use specific methods
if isinstance(neuromod, pynlm.Dopamine):
    neuromod.signalReward(1.0)
    neuromod.signalRewardPredictionError(0.1)
elif isinstance(neuromod, pynlm.Curiosity):
    neuromod.update(novelty_level=0.5, prediction_error=0.2, dt=0.1)
elif isinstance(neuromod, pynlm.Novelty):
    neuromod.detectNovelty(current_pattern, previous_pattern)
elif isinstance(neuromod, pynlm.PredictionError):
    neuromod.computeError(predicted, actual)
```

## Development System

### Developmental Stages

NLM implements 5 developmental stages:

- **Initial** (0-60s): High plasticity, low capabilities
- **CriticalPeriod** (60-360s): Rapid learning, sensitive periods
- **Maturation** (360-960s): Peak capabilities, refined skills
- **Adult** (960s+): Stable performance, fine-tuning
- **Aging** (Adult+): Gradual decline, experience-based learning

**Key Development Effects:**
- Neural excitability modulation
- Plasticity rule modification
- Attention system development
- Memory system maturation
- Neuromodulation profile changes
- Sleep/wake cycle development
- Life history tracking

**API Usage:**

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Update development over time
for step in range(100):
    agent.updateDevelopment(0.1)
    
    # Get current stage
    stage = agent.getDevelopmentalStage()
    stage_enum = brain.getDevelopmentalStage()
    
    # Get developmental effects
    excitability_modifier = stage.getExcitabilityModifier()
    attention_stability = stage.getAttentionStabilityModifier()
    working_memory_capacity = stage.getWorkingMemoryCapacityModifier()
    memory_consolidation = stage.getMemoryConsolidationModifier()
    
    # Get current milestone
    milestone = stage.getCurrentDevelopmentalMilestone()
    milestone_reached = stage.hasMilestonePassed(milestone)
    
    # Print developmental progress
    print(f"Stage: {stage_enum}, Milestone: {milestone:.1%}")
```

## System Integration Examples

### Example 1: Sensorimotor Learning with All Systems

```python
import pynlm

def run_advanced_learning_simulation():
    """Run simulation with all advanced cognitive systems."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    
    # Initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Initialize agent
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=8, visionHeight=8)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Initialize all advanced systems
    systems = {
        'prediction': pynlm.NeuralPrediction(),
        'working_memory': pynlm.NeuralWorkingMemory(),
        'episodic_memory': pynlm.NeuralEpisodicMemory(),
        'concept': pynlm.ConceptFormation(),
        'planner': pynlm.NeuralPlanner(),
        'spatial': pynlm.SpatialRepresentation(),
        'temporal': pynlm.TemporalRelation(),
    }
    
    neuromodulators = {
        'dopamine': pynlm.Dopamine(),
        'serotonin': pynlm.Serotonin(),
        'norepinephrine': pynlm.Norepinephrine(),
        'acetylcholine': pynlm.Acetylcholine(),
        'curiosity': pynlm.Curiosity(),
        'novelty': pynlm.Novelty(),
        'prediction_error': pynlm.PredictionError(),
        'reward': pynlm.Reward(),
    }
    
    # Initialize all systems
    for system in systems.values():
        system.initialize(brain)
    
    for neuromod in neuromodulators.values():
        neuromod.initialize(brain)
    
    # Run learning simulation
    world.reset()
    world.setAgentStart(20.0, 20.0)
    
    for episode in range(20):
        print(f"\n=== Episode {episode} ===")
        
        # Run episode
        for step in range(200):
            # World update
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process through agent
            agent.processSensoryInput(percept)
            
            # Update all systems
            systems['prediction'].recordSensoryState(percept.getVision(), step)
            systems['working_memory'].store(percept.getVision().getData())
            systems['episodic_memory'].storeEpisode([percept.getVision()])
            
            # Neural prediction and learning
            predicted = systems['prediction'].generatePrediction(step)
            error = systems['prediction'].getPredictionError()
            neuromodulators['prediction_error'].computeError(predicted, percept.getVision())
            
            # Neuromodulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            neuromodulators['reward'].signalReward(reward)
            neuromodulators['dopamine'].signalRewardPredictionError(error)
            neuromodulators['curiosity'].update(neuromodulators['novelty'].getLevel(), error, 0.1)
            neuromodulators['novelty'].detectNovelty(percept.getVision().getData(), [])
            
            # Run brain step
            brain.step(step)
            
            # Action selection
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Log episode statistics
        print(f"  Final stage: {agent.getDevelopmentalStage()}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Prediction error: {agent.getPredictionError():.3f}")
        print(f"  Concepts formed: {systems['concept'].getConceptCount()}")
        print(f"  Episodes stored: {systems['episodic_memory'].getEpisodeCount()}")
    
    print("\n=== Simulation Complete ===")
    brain.save("advanced_simulation_checkpoint.bin")

run_advanced_learning_simulation()
```

### Example 2: Prediction and Planning

```python
import pynlm

def run_prediction_planning_demo():
    """Demonstrate prediction and planning capabilities."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Initialize prediction and planning
    prediction = pynlm.NeuralPrediction()
    prediction.initialize(brain)
    
    planner = pynlm.NeuralPlanner()
    planner.initialize(brain)
    
    # Configure planning
    planner.setPlanningDepth(2)
    planner.setCurrentGoal(0.8)  # Want to achieve reward > 0.8
    
    # Create a simple environment
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setRandomSeed(42)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("=== Prediction and Planning Demo ===")
    
    for step in range(100):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        
        # Update prediction
        prediction.recordSensoryState(percept.getVision(), step)
        predicted_state = prediction.generatePrediction(step)
        
        # Plan action
        planned_action = planner.planAction(percept.getVision())
        
        # Get action and execute
        brain_action = agent.decodeMotorCommand()
        world.applyMotorCommand(brain_action, world.getSimulationTime())
        
        # Run brain step
        brain.step(step)
        
        # Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update planner based on outcome
        planner.updatePlanQuality([brain_action], reward)
        
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Planned action: {planner.getCurrentGoal():.3f}")
            print(f"  Prediction error: {prediction.getPredictionError():.3f}")
            print(f"  Planning confidence: {planner.getPlanningConfidence():.3f}")
    
    print("Demo completed!")

run_prediction_planning_demo()
```

## Integration Benefits

When all advanced systems work together, they produce emergent behaviors:

1. **Prediction-Guided Action**: Plans actions based on predicted consequences
2. **Memory-Enhanced Learning**: Uses episodic memory to guide decisions
3. **Concept-Based Generalization**: Applies learned concepts to new situations
4. **Development-Adaptive Behavior**: Changes strategies based on developmental stage
5. **Curiosity-Driven Exploration**: Balances exploitation and exploration
6. **Neuromodulation-Integrated Learning**: All learning signals coordinate effectively
7. **Spatial-Temporal Reasoning**: Plans in both space and time
8. **Social-Learning Potential**: Can observe and imitate others

## Performance Considerations

- **Memory Usage**: Advanced systems consume significant memory; tune parameters
- **Computation Time**: Multi-step prediction and planning add computational cost
- **System Coupling**: Components must be initialized in correct order
- **Parameter Tuning**: Optimal performance requires configuration of horizons, capacities, and thresholds

## References

- NLM_architecture.md - System architecture
- python_api.md - Python API documentation
- development_stages.md - Development system details
- memory_systems.md - Memory system documentation
- neuromodulation_guide.md - Neuromodulation details
- example_experiments.md - Experiment examples
