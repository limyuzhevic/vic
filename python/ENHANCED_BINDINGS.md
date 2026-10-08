# Enhanced NLM Python Bindings - API Documentation

## Overview

This document provides comprehensive documentation for the enhanced NLM Python bindings that expose ALL the C++ core APIs. The bindings transform the C++ NLM framework into a fully-featured Python interface with memory systems, prediction systems, neuromodulation systems, cognitive systems, development systems, and comprehensive performance monitoring.

## What Changed

The enhanced Python bindings now expose **all** the missing C++ APIs:

### 1. Memory System APIs (Previously Missing)
- **WorkingMemory**: Transient active information with persistent activity and recurrent connections
- **EpisodicMemory**: Experience storage with replay capabilities  
- **AssociativeMemory**: Pattern association and recall system

### 2. Prediction System APIs (Previously Missing)
- **PredictionSystem**: Real-time prediction and error computation

### 3. Neuromodulation APIs (Previously Missing)
- **Dopamine**: Reward and reinforcement learning signal
- **Curiosity**: Exploration motivation system
- **Novelty**: Novelty detection system
- **PredictionError**: Prediction error signal

### 4. Development APIs (Previously Missing)
- **DevelopmentSystem**: Neural development, stages, and plasticity modulation

### 5. Cognitive APIs (Previously Missing)
- **NeuralPlanner**: Action planning based on predictions
- **ConceptFormation**: Pattern discovery and categorization
- **AttentionalSelection**: Neural attention through competition
- **SelfModel**: Agent's internal model of itself

### 6. Performance APIs (Enhanced)
- Comprehensive system health and performance monitoring
- Memory usage tracking
- Cognitive load analysis
- Neuromedulation statistics

## Core Classes and Methods

### Brain Class - Core Integration

```python
brain = nlm.createBrain(config)

# Memory systems (new APIs)
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Prediction system (new API)
prediction_system = brain.getPredictionSystem()

# Neuromodulation systems (new APIs)
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()

# Cognitive systems (new APIs)
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()
self_model = brain.getSelfModel()

# Development system (new API)
development_system = brain.getDevelopmentSystem()
```

### WorkingMemory Class

```python
# Store patterns in working memory
working_memory.store_pattern([0.1, 0.5, 0.9, 0.2], strength=1.0)

# Retrieve active patterns
patterns = working_memory.get_active_patterns()

# Get memory statistics
stats = working_memory.get_statistics()
# Returns: {'capacity': 100, 'active_traces': 5, 'decay_rate': 0.01, 'activity_level': 0.7}

# Control memory
working_memory.clear_traces()
working_memory.set_capacity(200)
working_memory.set_decay_rate(0.05)
```

### EpisodicMemory Class

```python
# Record experiences
episodic_memory.record_experience(
    pattern=[0.1, 0.5, 0.9],
    features=[0.2, 0.7, 0.3],
    reward=1.0,
    timestamp=current_step
)

# Replay recent episodes
episodes = episodic_memory.replay(n_episodes=10)

# Get episode statistics
episode_stats = episodic_memory.get_episode_stats(episode_id=5)
# Returns: {'id': 5, 'reward': 1.0, 'timestamp': 100}
```

### PredictionSystem Class

```python
# Train on patterns
prediction_system.train_model([
    [0.1, 0.5, 0.9],
    [0.2, 0.6, 0.8],
    [0.3, 0.7, 0.7]
])

# Generate predictions
prediction = prediction_system.predict_next([0.4, 0.8, 0.6])

# Evaluate model performance
metrics = prediction_system.evaluate_model(test_patterns)
# Returns: {'accuracy': 0.85, 'avg_error': 0.15}
```

### Neuromodulation Systems

```python
# Dopamine - reward signaling
dopamine.set_level(0.8)
dopamine.modulate(weight=0.5)  # Reward-based modulation

# Curiosity - exploration
curiosity.explore(strength=1.0)

# Novelty - novelty detection
novelty_level = novelty.compute_novelty(pattern, history)

# Prediction Error
error = prediction_error.analyze_prediction(predicted, actual)
```

### NeuralPlanner Class

```python
# Plan actions based on current state and goal
action = planner.plan_for_goal(current_state=[0.1, 0.5, 0.9], target_reward=0.8)

# Evaluate action sequences
candidate = planner.evaluate_sequence(
    actions=[ActionType.MoveForward, ActionType.TurnLeft],
    start_state=[0.0, 0.0, 0.0]
)

# Manage planning
planner.set_planning_depth(3)
planner.set_current_goal([1.0, 1.0, 1.0])
```

### ConceptFormation Class

```python
# Discover concepts from data patterns
concept_formation.discover_concepts(data_patterns)

# Get concept information
concept = concept_formation.get_concept(concept_id=1)
prototype = concept_formation.get_concept_prototype(1)
instances = concept_formation.get_concept_instances(1)

# Check for novel patterns
is_novel = concept_formation.is_novel(pattern)
```

### AttentionalSelection Class

```python
# Process competing inputs and select winners
competitors = [neuron1, neuron2, neuron3]
winners = attention.select_targets(competitors)

# Apply attention to regions
attention.focus_on_region(region_id)

# Apply biases
attention.apply_top_down_bias(neuron, bias_strength=0.5)
```

### SelfModel Class

```python
# Learn action effects
self_model.learn_action_effects(
    action=ActionType.MoveForward,
    before_state=[0.0, 0.0, 0.0],
    after_state=[1.0, 0.0, 0.0]
)

# Predict consequences
prediction = self_model.predict_action_consequence(
    action=ActionType.TurnLeft,
    current_state=[0.0, 0.0, 0.0]
)

# Get self-awareness
awareness = self_model.getBodyAwareness()
```

### DevelopmentSystem Class

```python
# Get developmental stage information
stage_info = development_system.getStageInfo()
# Returns: {'stage': 'Adult', 'stage_name': 'Adult', 'age': 100.0, 
#          'is_critical_period': False, 'plasticity_modifier': 1.0}

# Control development
development_system.advance_stage()
development_system.accelerate_maturation(factor=2.0)
```

## High-Level API Classes

### LearningAgent Class

```python
# Create learning agent
agent = nlm.LearningAgent()

# Setup with world
agent.create_agent_brain()

# Train the agent
results = agent.train(episodes=100)

# Run simulation
episode_results = agent.run_episode(max_steps=200)

# Monitor system
performance = agent.get_state()

# Save/load state
agent.save("brain_state.nmlb")
agent.load("brain_state.nmlb")
```

### MemoryProfiler Class

```python
profiler = nlm.MemoryProfiler(brain)

# Collect metrics
metrics = profiler.collect_metrics()

# Get statistics
stats = profiler.get_statistics()
# Returns: {'working_memory_active_traces_avg': 5.0, 'working_memory_capacity_max': 100, ...}

# Clear history
profiler.clear_history()
```

### PredictionAnalyzer Class

```python
analyzer = nlm.PredictionAnalyzer(brain)

# Analyze confidence patterns
confidence_analysis = analyzer.analyze_confidence()

# Compute prediction errors
error = analyzer.compute_prediction_error(predicted, actual)

# Monitor training
progress = analyzer.monitor_training_progress()
```

### NeuromodulationController Class

```python
controller = nlm.NeuromodulationController(brain)

# Get all levels
levels = controller.get_all_levels()

# Set levels
controller.set_levels(dopamine=0.8, curiosity=0.5)

# Trigger modulation
controller.trigger_modulation("dopamine", strength=0.3)
```

### DevelopmentManager Class

```python
manager = nlm.DevelopmentManager(brain)

# Get stage info
stage_info = manager.get_stage_info()

# Control development
manager.advance_development()
manager.accelerate_maturation(factor=1.5)
```

### CognitiveProfiler Class

```python
profiler = nlm.CognitiveProfiler(brain)

# Analyze different cognitive systems
planning = profiler.analyze_planning()
concept_formation = profiler.analyze_concept_formation()
attention = profiler.analyze_attention()
self_model = profiler.analyze_self_model()

# Get overall cognitive load
cognitive_load = profiler.get_cognitive_load()
```

## Performance Monitoring

### System Health

```python
# Get comprehensive system health
health = brain.get_system_health()
# Returns: {'neurons': 10000, 'synapses': 50000, 'active_neurons': 2500, 
#          'firing_neurons': 500, 'average_firing_rate': 2.5, 
#          'total_spikes': 100000, 'pending_spikes': 50, 
#          'excitatory_inhibitory_ratio': 1.2}
```

### Memory Usage

```python
# Get detailed memory usage
memory_usage = brain.getMemoryUsage()
# Returns comprehensive stats for all memory systems
```

### Performance Statistics

```python
# Get overall performance metrics
perf_stats = brain.get_system_performance()
# Returns: {'memory': {...}, 'prediction': {...}, 'neuromodulation': {...}, 
#          'cognitive': {...}, 'health': {...}}
```

## Utility Functions

### Simulation Control

```python
# Run simulation with results
results = nlm.run_simulation(brain, world, steps=100)

# Train agent
agent_results = nlm.train_agent(agent_brain, world, episodes=50)
```

### Memory Management

```python
# Backup/restore brain state
brain.save("state1.nmlb")
brain.load("state2.nmlb")

# Clear all memory
nlm.clear_all_memory(brain)
```

### Debugging Tools

```python
# Debug brain state
debug_info = nlm.debug_brain_state(brain)

# Trace neural activity
traces = nlm.trace_neural_activity(brain, [neuron1, neuron2])
```

## Example Applications

### 1. Memory Learning Agent

```python
class MemoryLearningAgent:
    def __init__(self):
        self.brain = nlm.createBrain(nlm.createDefaultConfig())
        self.memory_profiler = nlm.MemoryProfiler(self.brain)
        self.development_manager = nlm.DevelopmentManager(self.brain)
        
    def learn_from_experience(self, pattern, reward):
        """Learn from pattern-reward pairs."""
        # Store in working memory
        self.brain.getWorkingMemory().store_pattern(pattern, strength=reward)
        
        # Store in episodic memory
        self.brain.getEpisodicMemory().record_experience(
            pattern, pattern, reward, self.development_manager.get_stage_info()['age']
        )
        
        # Track performance
        self.memory_profiler.collect_metrics()
        
    def optimize_memory(self):
        """Optimize memory usage."""
        self.brain.optimizeMemoryUsage()
        return self.memory_profiler.get_statistics()
```

### 2. Prediction-Based Decision Maker

```python
class PredictionBasedAgent:
    def __init__(self):
        self.brain = nlm.createBrain(nlm.createDefaultConfig())
        self.prediction_analyzer = nlm.PredictionAnalyzer(self.brain)
        self.planner = nlm.NeuralPlanner()
        
    def predict_future_state(self, current_state):
        """Predict next state using prediction system."""
        return self.brain.getPredictionSystem().predict_next(current_state)
        
    def plan_action(self, current_state, goal):
        """Plan optimal action."""
        return self.planner.planAction(current_state, goal)
        
    def evaluate_plan(self, action_sequence, start_state):
        """Evaluate action sequence."""
        return self.planner.evaluateSequence(action_sequence, start_state)
        
    def analyze_prediction_quality(self):
        """Analyze prediction system performance."""
        return self.prediction_analyzer.analyze_confidence()
```

### 3. Neuromodulation-Based Adaptive Agent

```python
class NeuromodulatedAgent:
    def __init__(self):
        self.brain = nlm.createBrain(nlm.createDefaultConfig())
        self.controller = nlm.NeuromodulationController(self.brain)
        self.development = nlm.DevelopmentManager(self.brain)
        
    def adapt_based_on_feedback(self, reward, prediction_error):
        """Adapt behavior based on feedback."""
        # Adjust neuromodulators
        self.controller.set_levels(
            dopamine=reward,
            curiosity=prediction_error,
            novelty=abs(prediction_error)
        )
        
        # Advance development if needed
        if reward > 0.8:
            self.development.advance_development()
            
    def explore_novelty(self, pattern):
        """Explore novel patterns."""
        novelty_level = self.brain.getNovelty().computeNovelty(pattern, [])
        self.controller.trigger_modulation("curiosity", strength=novelty_level)
        
    def learn_from_error(self, predicted, actual):
        """Learn from prediction errors."""
        error = self.brain.getPredictionErrorSignal().computeError(predicted, actual)
        self.controller.set_levels(prediction_error=error)
```

## Migration Guide

### From Old API to New API

**Before (Limited Python bindings):**
```python
# Basic API only - missing many important features
brain = nlm.createBrain(config)

# Basic operations only
brain.step(0)
brain.produceAction()

# Missing memory systems, prediction, neuromodulation, cognition
```

**After (Complete API):**
```python
# Full-featured API with all C++ capabilities
brain = nlm.createBrain(config)

# Memory systems
working_memory = brain.getWorkingMemory()
working_memory.store_pattern(pattern)

# Prediction system  
prediction_system = brain.getPredictionSystem()
prediction_system.train(pattern)

# Neuromodulation
dopamine = brain.getDopamine()
dopamine.modulate(weight)

# Cognitive systems
planner = brain.getPlanner()
action = planner.planAction(state)

# Development
brain.getDevelopmentSystem().advanceDevelopment()

# High-level utilities
agent = nlm.LearningAgent()
results = agent.train(100)
```

## Performance Considerations

1. **Memory Management**: The enhanced bindings maintain memory efficiency through proper reference counting and cleanup.

2. **Performance Monitoring**: Built-in performance metrics help identify bottlenecks and optimize performance.

3. **Scalability**: The bindings support large-scale simulations with thousands of neurons and complex memory systems.

4. **Real-time Capabilities**: Suitable for real-time applications with low-latency operation.

## Installation and Setup

### Requirements

```bash
pip install pybind11 numpy
```

### Building the Module

```bash
cd python
python setup.py build_ext --inplace
```

### Basic Usage

```python
import pynlm as nlm

# Quick test
config = nlm.Config()
brain = nlm.createBrain(config)
print("Version:", nlm.version())

# Test all systems
print("Available systems:")
print("- Memory:", "WorkingMemory, EpisodicMemory, AssociativeMemory")
print("- Prediction:", "PredictionSystem")  
print("- Neuromodulation:", "Dopamine, Curiosity, Novelty, PredictionError")
print("- Cognitive:", "NeuralPlanner, ConceptFormation, AttentionalSelection, SelfModel")
print("- Development:", "DevelopmentSystem")
```

## Troubleshooting

### Common Issues

1. **Module not found**: Ensure the compiled module is in your Python path.

2. **Missing dependencies**: Install required dependencies (pybind11, numpy).

3. **Memory issues**: Monitor memory usage with `brain.getMemoryUsage()` and optimize as needed.

4. **Performance issues**: Use `brain.getSystemHealth()` to identify bottlenecks.

### Getting Help

For additional support:

- **Documentation**: This file and the embedded docstrings
- **Examples**: See the example applications section
- **Community**: Check the NLM documentation for additional resources

## Future Enhancements

The enhanced Python bindings provide a foundation for future improvements:

1. **Distributed Computing**: Scale simulations across multiple machines
2. **GPU Acceleration**: Leverage GPU for neural computations
3. **Web Interface**: Create web-based visualization and control
4. **Machine Learning Integration**: Connect with modern ML frameworks
5. **Real-time APIs**: Add real-time data streaming capabilities

## License

This enhanced Python bindings implementation is provided under the same license as the NLM framework, allowing for both academic and commercial use.
