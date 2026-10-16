# NLM Python Bindings

The NLM (Neural Learning Machine) Python bindings provide a comprehensive Python API for the NLM C++ neural simulation framework.

## Overview

This module provides both low-level bindings for core NLM classes and high-level cognitive function wrappers that make it easy to create and run cognitive experiments.

## Installation

To build and install the Python bindings:

```bash
mkdir build && cd build
cmake ..
make
pip install pynlm
```

Or if using Python package management:

```bash
cd python
pip install -e .
```

## Basic Usage

### Creating a Simple Brain

```python
import pynlm as nlm

# Create a brain with default configuration
brain = nlm.createBrain()

# Create a world
world = nlm.createWorld(width=100.0, height=100.0, visionWidth=20, visionHeight=20)

# Create an agent that connects brain to world
agent = nlm.createAgent(brain, world)

# Initialize the world
agent.initialize(world)

# Run a simulation episode
nlm.simulateEpisode(brain, world, maxSteps=1000, timestep=0.1)
```

### Working with Sensory Data

```python
import pynlm as nlm
import numpy as np

# Create vision input (grayscale image)
vision_data = np.random.rand(20, 20).flatten().astype(float)
vision = nlm.Vision(20, 20)
vision.setData(vision_data)

# Create audio input
audio_data = np.random.rand(44100).astype(float)  # 1 second of audio
audio = nlm.Audio(sampleRate=44100, numSamples=44100)
audio.setData(audio_data)

# Create sensory percept
sensory_percept = nlm.SensoryPercept()
sensory_percept.setVision(vision_data)
sensory_percept.setAudio(audio_data)
sensory_percept.setTimestamp(0.0)

# Process sensory input
agent.processSensoryInput(sensory_percept)
```

### Monitoring and Statistics

```python
import pynlm as nlm

# Get brain statistics
brain_stats = nlm.getBrainStats(brain)
print(f"Brain has {brain_stats['total_neurons']} neurons")
print(f"Average firing rate: {brain_stats['avg_firing_rate']:.2f} Hz")

# Get world statistics
world_stats = nlm.getWorldStats(world)
print(f"Agent position: ({world_stats['agent_x']:.1f}, {world_stats['agent_y']:.1f})")
print(f"Agent energy: {world_stats['agent_energy']:.1f}")
```

## High-Level Cognitive Functions

### Exploration Task

```python
import pynlm as nlm

# Create a brain and world
brain = nlm.createBrain()
world = nlm.createWorld()

# Create exploration task with resources
nlm.createExplorationTask(brain, world, resourceDensity=0.3)

# Run the experiment
experiment = nlm.Experiment("exploration_task")
result = nlm.runExperiment(experiment, brain, world, maxSteps=500)

print(f"Experiment completed with success: {result}")
```

### Navigation Task

```python
import pynlm as nlm

# Create a brain and world
brain = nlm.createBrain()
world = nlm.createWorld()

# Create navigation task with obstacles
nlm.createNavigationTask(brain, world, obstacleDensity=0.2)

# Run the experiment
experiment = nlm.Experiment("navigation_task")
result = nlm.runExperiment(experiment, brain, world, maxSteps=1000)

print(f"Navigation experiment completed with success: {result}")
```

## Cognitive Systems

The bindings provide access to various cognitive systems through the brain object:

### Neural Working Memory

```python
import pynlm as nlm

brain = nlm.createBrain()
working_memory = nlm.getNeuralWorkingMemory(brain)

# Store information
working_memory.store([0.1, 0.2, 0.3, 0.4, 0.5])

# Retrieve information
memory_content = working_memory.retrieve()
```

### Prediction System

```python
import pynlm as nlm

brain = nlm.createBrain()
prediction_system = nlm.getPredictionSystem(brain)

# Generate prediction for next step
prediction = prediction_system.generatePrediction(step=0)

# Update predictions with actual observation
error = prediction_system.updateWithObservation(prediction, step=1)
```

### Neural Planner

```python
import pynlm as nlm

brain = nlm.createBrain()
planner = nlm.getPlanner(brain)

# Set current goal
current_goal = [0.8, 0.2, 0.5]
planner.setCurrentGoal(current_goal)

# Plan next action
action = planner.planAction(current_goal, targetReward=0.7)
```

### Concept Formation

```python
import pynlm as nlm

brain = nlm.createBrain()
concept_formation = nlm.getConceptFormation(brain)

# Present experience
pattern = [0.1, 0.9, 0.2, 0.8]
features = [0.5, 0.3]
concept_id = concept_formation.presentExperience(pattern, features, reward=0.8, time=0)

# Get concept prototype
if concept_id > 0:
    prototype = concept_formation.getConceptPrototype(concept_id)
```

### Neuromodulation

```python
import pynlm as nlm

brain = nlm.createBrain()

# Access different neuromodulators
dopamine = nlm.getDopamine(brain)
curiosity = nlm.getCuriosity(brain)
novelty = nlm.getNovelty(brain)

# Apply reward modulation
brain.applyRewardModulation(reward=1.0, predicted_reward=0.8)
```

## Experiment Management

### Creating and Running Experiments

```python
import pynlm as nlm

# Create an experiment
experiment = nlm.Experiment("cognitive_test")
experiment.setName("Testing cognitive abilities")
experiment.setSeed(42)

# Create brain and world
brain = nlm.createBrain()
world = nlm.createWorld()

# Run experiment
max_steps = 1000
success = nlm.runExperiment(experiment, brain, world, max_steps)

# Access experiment results
metrics = experiment.getMetrics()
for name, value in metrics.items():
    print(f"{name}: {value}")
```

### Recording Metrics

```python
import pynlm as nlm

experiment = nlm.Experiment("learning_experiment")

# Record various metrics
experiment.recordMetric("learning_rate", 0.001)
experiment.recordMetric("accuracy", 0.85)
experiment.recordMetric("exploration_exploitation_ratio", 0.3)
experiment.recordMetric("cognitive_load", 0.7)

# Record step data
experiment.recordStep(step=100, neuron_count=1000, synapse_count=5000,
                     firing_count=500, avg_firing_rate=8.5)
```

## Visualization and Monitoring

```python
import pynlm as nlm

# Create visualizer (placeholder - extends VisualizationInterface)
visualizer = nlm.createVisualizer()

# Plot brain activity
brain_stats = nlm.getBrainStats(brain)
nlm.plotBrainActivity(brain)

# Plot learning curve
nlm.plotLearningCurve(experiment)

# Export results
nlm.exportResults(experiment, format="json")
```

## Error Handling

The bindings provide proper error handling with the following exceptions:

- `RuntimeError`: For general runtime errors
- `ValueError`: For invalid parameter values
- `IndexError`: For out-of-range access

## Type Conversion Utilities

The bindings provide automatic conversion between Python and C++ types:

- `numpy.ndarray` ↔ `std::vector<float>`
- `list` ↔ `std::vector<float>`
- Primitive types (int, float, bool) ↔ C++ types

## Performance Considerations

1. **Memory Management**: Python objects hold references to C++ objects, so explicit cleanup may be needed
2. **Data Transfer**: Large arrays should be converted to numpy arrays for efficient transfer
3. **Thread Safety**: The C++ core is not thread-safe; use separate Python threads carefully

## Advanced Usage

### Custom Configuration

```python
import pynlm as nlm

# Create custom configuration
config = nlm.Config()
config.set("learning_rate", 0.01)
config.set("hidden_size", 512)
config.set("discount_factor", 0.99)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration from file
config2 = nlm.Config()
config2.loadFromFile("my_config.json")
```

### State Persistence

```python
import pynlm as nlm

# Create and train a brain
brain = nlm.createBrain()
world = nlm.createWorld()

# Train the brain
nlm.simulateEpisode(brain, world, maxSteps=1000)

# Save brain state
nlm.exportBrainState(brain, "brain_state.json")

# Later, restore the brain
brain2 = nlm.createBrain()
nlm.importBrainState(brain2, "brain_state.json")
```

### Batch Experiments

```python
import pynlm as nlm

# Create multiple experiments
experiments = [
    nlm.Experiment("task_1"),
    nlm.Experiment("task_2"),
    nlm.Experiment("task_3")
]

# Create brain template
brain_template = nlm.createBrain()

# Create world template
world_template = nlm.createWorld()

# Run batch of experiments
results = nlm.runBatch(experiments, brain_template, world_template, maxSteps=500)
```

## API Reference

### Classes

- **Config**: Configuration management
- **Brain**: Main neural simulation engine
- **SimpleWorld**: 2D simulation environment
- **AgentBrain**: Connects brain to world
- **Vision**, **Audio**, **InternalSignals**: Sensory inputs
- **Action**: Motor outputs
- **AgentBody**: Agent physical state
- **SensoryPercept**: Agent percepts
- **WorldObject**: Objects in the world
- **ActionResult**: Results of actions
- **NeuronId**, **SynapseId**, **RegionId**, **PopulationId**: Strongly-typed identifiers

### Enums

- **NeuronType**: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
- **SynapseType**: Excitatory, Inhibitory, Modulatory, Electrical, GapJunction
- **DevelopmentalStage**: Initial, CriticalPeriod, Maturation, Adult, Aging
- **FiringState**: Resting, Active, Refractory, Inhibited
- **ActionType**: Movement, perception, interaction actions
- **MotorCommand**: Low-level motor commands
- **WorldObjectType**: Empty, Resource, Hazard, Wall, Marker

### Functions

- **createBrain(configJson)**: Create and initialize a brain
- **createBrainWithConfig(config)**: Create brain with configuration object
- **createWorld(width, height, visionWidth, visionHeight)**: Create simulation world
- **createAgent(brain, world)**: Create agent interface
- **createExplorationTask(brain, world, resourceDensity)**: Setup exploration task
- **createNavigationTask(brain, world, obstacleDensity)**: Setup navigation task
- **simulateStep(brain, step, time)**: Run simulation step
- **simulateEpisode(brain, world, maxSteps, timestep)**: Run complete episode
- **runExperiment(experiment, brain, world, maxSteps)**: Run experiment
- **getBrainStats(brain)**: Get brain statistics
- **getWorldStats(world)**: Get world state
- **recordMetrics(experiment, brain, world, step)**: Record metrics
- **resetAll(brain, world)**: Reset brain and world
- **addRewardForAction(experiment, action, reward)**: Add reward for action

### Constants

- **INVALID_NEURON_ID**: Invalid neuron identifier
- **INVALID_SYNAPSE_ID**: Invalid synapse identifier
- **INVALID_REGION_ID**: Invalid region identifier
- **INVALID_POPULATION_ID**: Invalid population identifier

## Future Enhancements

The Python bindings will be expanded with:

1. **Advanced Visualization**: Integration with matplotlib and other plotting libraries
2. **Experiment Templates**: Pre-built templates for common cognitive tasks
3. **Distributed Computing**: Support for multi-process and distributed experiments
4. **Real-time Monitoring**: Web-based monitoring dashboards
5. **Automated Experiment Design**: AI-assisted experiment creation
6. **Cloud Integration**: Support for cloud-based training and storage

## License

The NLM Python bindings are part of the NLM framework and are licensed under the same terms as the core C++ library.

## Contact

For questions or support, please visit the NLM documentation or contact the development team.
