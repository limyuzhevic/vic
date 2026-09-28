# NLM Python Bindings

This module provides Python bindings for the NLM (Neural Learning Machine) C++ neural simulation framework.

## Installation

The NLM Python bindings can be installed via pip:

```bash
pip install pynlm
```

## Basic Usage

Here's a simple example to get started with the NLM Python bindings:

```python
import pynlm

# Create a new brain configuration
config = pynlm.Config()
config.set("neuron_count", 1000)
config.set("region_count", 1)
config.set("connection_probability", 0.1)

# Create a brain instance
brain = pynlm.Brain(config)

# Initialize the brain
if not brain.initialize():
    print("Failed to initialize brain")
    exit(1)

print("Brain initialized successfully")
print(f"Total neurons: {brain.get_total_neuron_count()}")
print(f"Total synapses: {brain.get_total_synapse_count()}")

# Run a few simulation steps
for step in range(10):
    brain.step(step, step * 0.001)
    print(f"Step {step}: avg firing rate = {brain.get_average_firing_rate():.3f}")

# Save the brain state
brain.save("test_checkpoint.bin")
print("Brain state saved")

# Log brain status
brain.log_status()
```

## Advanced Usage

### Memory Systems

The NLM brain includes several integrated memory systems:

```python
# Working memory for transient active information
working_memory = brain.get_working_memory()
if working_memory:
    print(f"Working memory traces: {working_memory.get_active_traces()}")

# Episodic memory for experience storage
episodic_memory = brain.get_episodic_memory()
if episodic_memory:
    print(f"Episodic memory episodes: {episodic_memory.get_episode_count()}")

# Associative memory for pattern associations
associative_memory = brain.get_associative_memory()
if associative_memory:
    print(f"Associative memory active patterns: {associative_memory.get_active_patterns()}")
```

### Neuromodulation

The brain includes various neuromodulation systems:

```python
# Dopamine for reward and reinforcement
dopamine = brain.get_dopamine()
if dopamine:
    print(f"Dopamine level: {dopamine.get_level():.3f}")

# Curiosity for exploration motivation
curiosity = brain.get_curiosity()
if curiosity:
    print(f"Curiosity level: {curiosity.get_level():.3f}")

# Novelty detection for surprise
novelty = brain.get_novelty()
if novelty:
    print(f"Novelty level: {novelty.get_level():.3f}")
```

### Prediction and Cognition

The brain includes prediction and cognitive systems:

```python
# Prediction system for sensory prediction and error computation
prediction_system = brain.get_prediction_system()
if prediction_system:
    print(f"Prediction confidence: {prediction_system.get_confidence():.3f}")

# Neural planner for action planning
planner = brain.get_planner()
if planner:
    print(f"Planning horizon: {planner.get_planning_depth()}")

# Concept formation for pattern discovery
concept_formation = brain.get_concept_formation()
if concept_formation:
    print(f"Active concepts: {concept_formation.get_active_concept_count()}")
```

### Development System

The brain includes a development system:

```python
# Get current developmental stage
development_system = brain.get_development_system()
if development_system:
    stage = brain.get_developmental_stage()
    print(f"Developmental stage: {stage}")
```

### Action Selection

The brain can produce motor/action outputs:

```python
# Get the brain's motor command
action = brain.produce_action()
if action:
    print(f"Selected action: {action.get_type()}")
    if action.get_type() == pynlm.ActionType.MoveForward:
        print("Moving forward!")
    elif action.get_type() == pynlm.ActionType.TurnLeft:
        print("Turning left!")
    elif action.get_type() == pynlm.ActionType.TurnRight:
        print("Turning right!")
```

### Checkpointing

Save and load brain states:

```python
# Save brain state
brain.save("my_brain.bin")

# Load brain state into a new brain
brain2 = pynlm.Brain(config)
brain2.load("my_brain.bin")
```

### Integration Testing

Test the integration of all brain systems:

```python
# Run the Phase 6 integration experiment
experiment = pynlm.Phase6IntegratedExperiment()
result = experiment.run()

print(f"Integration test results:")
print(f"  Memory working memory integrated: {result.memory_working_memory_integrated}")
print(f"  Memory episodic memory integrated: {result.memory_episodic_memory_integrated}")
print(f"  Neuromodulation integrated: {result.neuromodulation_integrated}")
print(f"  Prediction integrated: {result.prediction_integrated}")
print(f"  Development integrated: {result.development_integrated}")
print(f"  Checkpointing works: {result.checkpointing_works}")
print(f"  Total reward: {result.total_reward:.3f}")
print(f"  Average firing rate: {result.avg_firing_rate:.3f}")
```

## Error Handling

The NLM Python bindings provide comprehensive error handling:

```python
try:
    brain = pynlm.Brain(config)
    if not brain.initialize():
        raise Exception("Failed to initialize brain")
except Exception as e:
    print(f"Error: {e}")
```

## Performance Tips

For optimal performance:

1. **Use batch operations** when possible
2. **Monitor memory usage** during long simulations
3. **Use checkpointing** to save progress periodically
4. **Profile your code** to identify bottlenecks

## Documentation

For more detailed documentation, see:
- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM GitHub Repository](https://github.com/nlm-project/nlm)

## License

This software is licensed under the MIT License. See LICENSE file for details.
