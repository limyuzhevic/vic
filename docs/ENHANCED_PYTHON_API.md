# Enhanced Python API Documentation
# NLM (Neural Learning Machine) - Advanced Python Interface

This document provides comprehensive documentation for the enhanced Python API of the NLM framework.

## Overview

The enhanced Python API provides access to all advanced neural simulation capabilities of the NLM framework,
including:

- Complete memory systems (working, episodic, semantic, procedural, associative)
- Advanced prediction and planning systems
- Cognitive mechanisms (concept formation, attention, planning)
- Neuromodulation systems (dopamine, curiosity, novelty, reward)
- Development and plasticity systems
- Performance optimization tools
- Phase 4 and Phase 6 experimental frameworks

## Quick Start

```python
import nlm

# Create a brain with default configuration
config = nlm.Config()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.1)

brain = nlm.create_brain(config)
brain.initialize()

# Create a world
world = nlm.create_simple_world()
world.configure(100, 100, 20, 20)

# Create an agent interface
agent_brain = nlm.create_agent_brain(brain)
agent_brain.initialize(world)
agent_brain.enable_reward_modulation(True)
agent_brain.enable_curiosity(True)
```

## Core Classes

### Brain
The central neural simulation brain class.

```python
brain = nlm.create_brain(config)
brain.initialize()

# Basic operations
brain.step(step)
action = brain.produce_action()

# Sensory input
vision = nlm.Vision(20, 20, 3)
action = brain.produce_action()

# Memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
```

### Config
Configuration system for NLM.

```python
config = nlm.Config()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.1)

# Load from file
config.loadFromFile("config.json")

# Save to file
config.saveToFile("config.json")
```

### AgentBrain
Agent brain interface connecting NLM brain to world.

```python
agent_brain = nlm.create_agent_brain(brain)
agent_brain.initialize(world)
agent_brain.enable_reward_modulation(True)
agent_brain.enable_curiosity(True)

# Process sensory input
percept = world.getSensoryPercept()
agent_brain.processSensoryInput(percept)

# Get motor command
cmd = agent_brain.decodeMotorCommand()

# Apply neuromodulation
agent_brain.applyRewardModulation(reward, predicted_reward)
```

## Memory Systems

### Working Memory
Maintain transient information across steps.

```python
working_memory = brain.getWorkingMemory()
working_memory.setMaxCapacity(1000)
working_memory.addTrace("task_1", 0.5)
strength = working_memory.getTraceStrength("task_1")
traces = working_memory.getTraces()
```

### Episodic Memory
Store and recall experience episodes.

```python
episodic_memory = brain.getEpisodicMemory()
episodic_memory.storeEpisode("experiment_1")
episode = episodic_memory.recallEpisode("experiment_1")
episodes = episodic_memory.searchEpisodes("learning")
```

### Semantic Memory
Store facts and concepts.

```python
semantic_memory = brain.getSemanticMemory()
semantic_memory.storeConcept("temperature", ["hot", "warm", "cold"])
concept = semantic_memory.retrieveConcept("temperature")
similarity = semantic_memory.calculateConceptSimilarity("temperature", "color")
```

## Prediction Systems

### Neural Prediction
Forward model for predicting sensory input.

```python
neural_prediction = nlm.NeuralPrediction()
neural_prediction.setTargetPattern(target_pattern)
error = neural_prediction.getPredictionError()
accuracy = neural_prediction.getPredictionAccuracy()
```

### Prediction System
Multiple forward models.

```python
prediction_system = brain.getPredictionSystem()
prediction_system.addPrediction(neural_prediction)
errors = prediction_system.getPredictionErrors()
accuracies = prediction_system.getPredictionAccuracies()
```

## Cognitive Systems

### Neural Planner
Action planning and decision making.

```python
neural_planner = nlm.NeuralPlanner()
neural_planner.setGoal("explore")
action = neural_planner.planAction(state)
confidence = neural_planner.getPlanningConfidence()
```

### Concept Formation
Pattern discovery and concept learning.

```python
concept_formation = nlm.ConceptFormation()
concept_formation.learnConcept([0.1, 0.5, 0.9], "temperature")
concept = concept_formation.retrieveConcept("temperature")
labels = concept_formation.getConceptLabels()
```

## Neuromodulation Systems

### Dopamine
Reward and reinforcement learning.

```python
dopamine = brain.getDopamine()
dopamine.setBaselineDopamine(0.1)
response = dopamine.getDopamineResponse(reward)
error = dopamine.getRewardPredictionError(predicted_reward, actual_reward)
```

### Curiosity
Exploration motivation.

```python
curiosity = brain.getCuriosity()
curiosity.setExplorationRate(0.5)
score = curiosity.getNoveltyDetection(input_pattern)
exploration_prob = curiosity.getExplorationProbability()
```

### Novelty
Novelty detection.

```python
novelty = brain.getNovelty()
novelty.setNoveltyThreshold(0.3)
score = novelty.getNoveltyScore(observation)
history = novelty.getNoveltyHistory()
```

### Prediction Error
Prediction error signal.

```python
prediction_error = brain.getPredictionErrorSignal()
prediction_error.setBaselinePredictionError(0.0)
error = prediction_error.getPredictionError()
prediction_error.update(prediction, target)
```

## Development System

```python
development_system = brain.getDevelopmentSystem()
development_system.setDevelopmentalStage("Maturation")
plasticity_rate = development_system.getPlasticityRate()
development_system.update(0.001)
age = development_system.getAge()
```

## Plasticity Systems

### STDP
Spike-Timing-Dependent Plasticity.

```python
stdp = nlm.STDP()
stdp.setLTPWeight(0.02)
stdp.setLTDWeight(0.015)
weight_change = stdp.getWeightChange(pre_spike_time, post_spike_time)
```

### Hebbian
Hebbian plasticity.

```python
hb = nlm.Hebbian()
hb.setLearningRate(0.01)
hb.setMaxWeightChange(1.0)
weight_change = hb.getWeightChange(pre_activity, post_activity)
```

## Performance Systems

### Event Queue
High-performance event queue.

```python
from nlm import get_event_queue_stats
stats = get_event_queue_stats()
print(f"Queue size: {stats['size']}")
print(f"Throughput: {stats['throughput']} events/sec")
```

### Memory Pool
Neural memory pool allocator.

```python
from nlm import get_memory_usage
usage = get_memory_usage()
print(f"Available memory: {usage['available']} MB")
print(f"Total memory: {usage['total']} MB")
```

## Experimental Frameworks

### Phase 6 Integration Experiment
Complete integration test for all brain systems.

```python
phase6_config = nlm.Phase6Config()
phase6_config.neuronCount = 500
phase6_config.maxSteps = 2000
phase6_config.enableCheckpointing = True
phase6_config.enableReplay = True

experiment = nlm.create_phase6_experiment()
result = experiment.run(phase6_config)

print(f"Total reward: {result.total_reward}")
print(f"Average firing rate: {result.avg_firing_rate}")
print(f"Memory working memory integrated: {result.memory_working_memory_integrated}")
```

### Phase 4 Demo
Demonstration of Phase 4 emergent cognition.

```python
from nlm import Phase4Demo
demo = Phase4Demo()
demo.run(numTrials=100)
```

## Advanced Usage

### Creating Custom Experiments

```python
def create_custom_experiment(brain, config):
    """Create a custom experiment with specific brain configuration."""
    # Configure brain for specific experiment
    config.set("neuron_count", 500)
    config.set("region_count", 3)
    config.set("stdp_ltp_weight", 0.02)
    
    brain = nlm.create_brain(config)
    brain.initialize()
    
    # Add custom memory systems
    working_memory = brain.getWorkingMemory()
    working_memory.setMaxCapacity(1000)
    
    # Add custom neuromodulation
    dopamine = brain.getDopamine()
    dopamine.setBaselineDopamine(0.1)
    
    return brain
```

### Real-time Simulation Loop

```python
def real_time_simulation(world, brain, agent_brain, steps=1000):
    """Real-time simulation loop with world interaction."""
    for step in range(steps):
        # Get observation
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent_brain.processSensoryInput(percept)
        
        # Brain step
        brain.step(step, step * 0.001)
        
        # Get motor command
        cmd = agent_brain.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(cmd, step * 0.001)
        
        # Compute reward
        reward = world.computeReward(cmd)
        
        # Apply reward modulation
        agent_brain.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent_brain.updateDevelopment(0.001)
        
        if step % 100 == 0:
            print(f"Step {step}, Reward: {reward:.3f}")
    
    return brain
```

### Performance Monitoring

```python
def monitor_performance(brain, steps=1000):
    """Monitor performance metrics during simulation."""
    import time
    
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
        
        if step % 100 == 0:
            # Get performance metrics
            from nlm import get_memory_usage, get_event_queue_stats
            memory_usage = get_memory_usage()
            event_stats = get_event_queue_stats()
            
            print(f"Step {step}:")
            print(f"  Memory usage: {memory_usage['used_percentage']:.1f}%")
            print(f"  Event queue size: {event_stats['size']}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    
    end_time = time.time()
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
```

### Checkpointing and Recovery

```python
def save_and_restore_simulation(world, brain, agent_brain, steps=500):
    """Save simulation state and restore from checkpoint."""
    checkpoint_path = "/tmp/nlm_checkpoint.bin"
    
    # Run simulation
    for step in range(steps):
        # ... simulation code ...
        
        # Save checkpoint periodically
        if step % 100 == 0:
            brain.save(checkpoint_path)
    
    # Restore from checkpoint
    new_brain = nlm.create_brain(config)
    new_brain.initialize()
    new_brain.load(checkpoint_path)
    
    return new_brain
```

## Research Tools

### Parameter Sweeps

```python
def parameter_sweep(param_name, param_values, brain_config, experiment_func):
    """Run parameter sweeps for research experiments."""
    results = {}
    
    for value in param_values:
        # Set parameter
        brain_config.set(param_name, value)
        
        # Run experiment
        brain = nlm.create_brain(brain_config)
        brain.initialize()
        result = experiment_func(brain)
        
        results[value] = result
        print(f"Completed {param_name}={value}: {result}")
    
    return results
```

### Data Analysis

```python
def analyze_simulation_data(brain, steps):
    """Analyze simulation data and extract insights."""
    import numpy as np
    
    # Collect metrics
    spike_counts = []
    firing_rates = []
    reward_history = []
    
    for step in range(steps):
        brain.step(step)
        
        spike_counts.append(brain.getTotalSpikeCount())
        firing_rates.append(brain.getAverageFiringRate())
        reward_history.append(brain.getReward())
    
    # Analyze results
    analysis = {
        'mean_spike_count': np.mean(spike_counts),
        'std_spike_count': np.std(spike_counts),
        'mean_firing_rate': np.mean(firing_rates),
        'std_firing_rate': np.std(firing_rates),
        'mean_reward': np.mean(reward_history),
        'std_reward': np.std(reward_history),
        'total_reward': sum(reward_history),
    }
    
    return analysis
```

## API Reference

### Function Reference

| Function | Description | Returns |
|----------|-------------|---------|
| `create_default_config()` | Create default configuration | `Config` |
| `create_brain(config)` | Create brain with configuration | `Brain` |
| `create_simple_world()` | Create simple world | `SimpleWorld` |
| `create_agent_brain(brain)` | Create agent brain interface | `AgentBrain` |
| `create_phase6_experiment()` | Create Phase 6 experiment | `Phase6IntegratedExperiment` |
| `get_memory_usage()` | Get current memory usage | `dict` |
| `get_event_queue_stats()` | Get event queue statistics | `dict` |

### Class Reference

See individual class documentation above for detailed API information.

## Best Practices

### For Research Users

1. **Use parameter sweeps for systematic exploration**
2. **Save checkpoints regularly during long simulations**
3. **Use descriptive experiment names and configurations**
4. **Monitor performance metrics during simulation**
5. **Document experimental parameters and results**

### For Performance Optimization

1. **Use memory pools for intensive memory operations**
2. **Configure event queues appropriately**
3. **Monitor memory usage during simulations**
4. **Use checkpointing for long-running experiments**
5. **Profile critical sections for optimization**

### For Integration

1. **Use configuration files for experiment reproducibility**
2. **Implement proper error handling**
3. **Use appropriate simulation step sizes**
4. **Monitor system resources**
5. **Implement logging for debugging**

## Troubleshooting

### Common Issues

1. **Memory errors**: Use memory pools and monitor memory usage
2. **Performance issues**: Profile critical sections and optimize event queue
3. **Configuration errors**: Validate configuration parameters
4. **Simulation instability**: Check for infinite loops and proper termination conditions

### Error Handling

```python
try:
    brain = nlm.create_brain(config)
    brain.initialize()
except Exception as e:
    print(f"Error creating brain: {e}")
    # Implement error recovery
```

## Further Reading

- **NLM Documentation**: https://nlm.readthedocs.io
- **Phase 4 Paper**: "Emergent Cognition in Artificial Neural Systems"
- **Phase 6 Paper**: "Integrated Neural Simulation Framework"
- **API Examples**: examples/ directory
- **Research Demos**: demos/ directory

## License

MIT License - See LICENSE file for details.

---

*This documentation is continuously updated with new features and improvements.*
*For questions and support, visit the NLM GitHub repository.*