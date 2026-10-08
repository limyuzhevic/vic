# Enhanced NLM Python Bindings Examples

This directory contains comprehensive examples for the enhanced NLM Python bindings, demonstrating the complete functionality of the integrated artificial brain system.

## Overview

The NLM (Neural Learning Machine) Python bindings provide access to a sophisticated computational brain framework that simulates neural activity, learning, memory, and cognition using brain-inspired architectures.

## Examples Included

### 1. basic_usage.py
**Basic NLM Simulation**

This example demonstrates fundamental NLM usage including:
- Brain initialization and configuration
- Basic simulation control (step, reset)
- Neural activity monitoring
- Simple action production
- Development system usage
- Memory system basics

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### 2. memory_systems.py
**Working Memory, Episodic Memory, and Associative Memory**

This example showcases NLM's memory capabilities:
- Working memory with transient information storage
- Episodic memory for experience recording and replay
- Associative memory for pattern associations
- Memory management and optimization
- Memory backup and restore operations

```python
import pynlm

# Create brain with memory systems
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Store information in working memory
wm = brain.getWorkingMemory()
wm.store([0.1, 0.5, 0.9], 1.0)
wm.store([0.2, 0.6, 0.8], 0.5)

# Retrieve working memory patterns
patterns = wm.get_active_patterns()
print(f"Active patterns: {len(patterns)}")

# Record experience in episodic memory
em = brain.getEpisodicMemory()
em.record_experience([0.1, 0.5, 0.9], [1, 2, 3], 0.8)

# Replay episodes
em.replay(5)
```

### 3. agent_training.py
**Complete Agent Training with Curiosity**

This example demonstrates training an agent using NLM's integrated systems:
- High-level LearningAgent API for simplified training
- Curiosity-driven exploration and reward modulation
- Development-aware learning progression
- Comprehensive performance monitoring
- Environment interaction and action selection

```python
import pynlm

# Create training setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Configure world and agent
world.configure(20, 20, 8, 8)
agent.initialize(world)

# Enable advanced systems
agent.enableCuriosity(True)
agent.enableRewardModulation(True)
agent.enableDevelopment(True)

# Run training episodes
for episode in range(10):
    for step in range(100):
        # Get percept from world
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step)
    
    # Print progress
    print(f"Episode {episode}: Curiosity {agent.getCuriosityLevel():.2f}, "
          f"Novelty {agent.getNoveltyLevel():.2f}")
```

### 4. performance_monitoring.py
**System Health and Performance Tracking**

This example demonstrates comprehensive performance monitoring:
- Real-time system health metrics
- Memory usage analysis and cognitive load monitoring
- Performance bottleneck detection
- Advanced debugging tools
- System optimization capabilities

```python
import pynlm

# Initialize brain with monitoring
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Monitor system performance during simulation
for step in range(100):
    brain.step(step)
    
    if step % 10 == 0:
        # Get comprehensive performance metrics
        perf_stats = brain.get_system_performance()
        
        print(f"Step {step}:")
        print(f"  Memory usage: {perf_stats['memory']}")
        print(f"  Prediction performance: {perf_stats['prediction']}")
        print(f"  System health: {perf_stats['health']}")
```

### 5. checkpointing.py
**Persistence and Backup/Restore**

This example demonstrates NLM's persistence capabilities:
- Checkpoint management for long-running simulations
- Brain state backup and restore operations
- Memory persistence across simulation sessions
- Backup chain management
- Recovery strategies

```python
import pynlm
import tempfile
import os

# Create and train brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some simulation steps
for step in range(50):
    brain.step(step)

# Save checkpoint
with tempfile.TemporaryDirectory() as temp_dir:
    checkpoint_path = os.path.join(temp_dir, "brain_checkpoint.bin")
    pynlm.backup_brain_state(brain, checkpoint_path)
    print(f"Brain state saved to {checkpoint_path}")

# Restore checkpoint
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()
pynlm.restore_brain_state(brain2, checkpoint_path)
print("Brain state restored successfully")
```

### 6. advanced_integration.py
**Using All Cognitive and Neuromodulation Systems**

This example demonstrates integration of all NLM systems:
- Neuromodulation with dopamine, curiosity, novelty, and prediction error
- Cognitive planning with NeuralPlanner
- Concept formation and learning
- Self-model development
- Attentional selection and focus
- Complete system coordination

```python
import pynlm

# Initialize complete brain system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable all advanced systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()

planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
self_model = brain.getSelfModel()
attention = brain.getAttention()

# Demonstrate system integration
print("=== Advanced Integration Demo ===")
print(f"Dopamine level: {dopamine.getLevel()}")
print(f"Curiosity level: {curiosity.getLevel()}")
print(f"Novelty level: {novelty.getLevel()}")
print(f"Prediction error: {prediction_error.getLevel()}")

print(f"Planner available: {planner.getPlanningDepth() > 0}")
print(f"Concept count: {concept_formation.getConceptCount()}")
print(f"Self model active: {self_model.getBodyAwareness()}")
```

### 7. exploration_learning.py
**Curiosity-Driven Exploration**

This example demonstrates curiosity-driven learning and exploration:
- Novelty detection and exploration motivation
- Exploration vs. exploitation strategies
- Adaptive exploration based on development stage
- Curiosity optimization and profiling
- Learning from novel experiences

```python
import pynlm

# Create exploration environment
brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

agent.initialize(world)

# Enable exploration systems
agent.enableCuriosity(True)
agent.enableNoveltyDetection(True)

# Run exploration simulation
for episode in range(20):
    episode_score = 0
    
    for step in range(50):
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action and get reward
        world.applyMotorCommand(action, step)
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        
        agent.applyRewardModulation(reward, 0.0)
        
        episode_score += reward
    
    # Print exploration statistics
    print(f"Episode {episode}: Score {episode_score:.2f}, "
          f"Curiosity {agent.getCuriosityLevel():.2f}, "
          f"Novelty {agent.getNoveltyLevel():.2f}")
```

## Installation and Setup

### Prerequisites
- Python 3.8 or higher
- NLM Python bindings installed

### Installation
```bash
# Install the enhanced NLM Python bindings
pip install -e .

# Or install from source
pip install .
```

### Running Examples

All examples can be run directly:
```bash
python examples/basic_usage.py
python examples/memory_systems.py
python examples/agent_training.py
python examples/performance_monitoring.py
python examples/checkpointing.py
python examples/advanced_integration.py
python examples/exploration_learning.py
```

## Key Features Demonstrated

### Pythonic APIs
- `store_pattern()` vs `store()` for working memory
- `record_experience()` for episodic memory
- `plan_for_goal()` for neural planning
- `discover_concepts()` for concept formation

### Advanced Capabilities
- Memory management and backup/restore
- Performance monitoring and optimization
- Curiosity-driven exploration
- Complete cognitive system integration
- Neuromodulation control

### Development Features
- Development stage progression
- Critical period handling
- Plasticity modulation
- Memory consolidation

## Advanced Usage

### Custom Configuration
```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.01)

brain = pynlm.createBrain(config)
```

### Memory System Optimization
```python
# Optimize memory usage
brain.optimizeMemoryUsage()
stats = brain.get_memory_usage()
print(f"Optimization results: {stats}")
```

### System Health Monitoring
```python
# Monitor system health
health = brain.get_system_health()
print(f"System health: {health}")

# Check specific components
if health['firing_neurons'] == 0:
    print("Warning: No neurons firing!")
```

### Performance Profiling
```python
# Profile system performance
perf = brain.get_system_performance()
print(f"Performance metrics:")
print(f"  Memory usage: {perf['memory']}")
print(f"  Prediction accuracy: {perf['prediction']['training_accuracy']}")
print(f"  Cognitive load: {perf['cognitive']}")
```

## Troubleshooting

### Common Issues

1. **ImportError: No module named 'pynlm'**
   - Ensure NLM Python bindings are installed
   - Run `pip install -e .` from the NLM project root

2. **Brain initialization fails**
   - Check that Config() is properly initialized
   - Ensure required configuration parameters are set

3. **Memory systems return None**
   - Ensure brain has proper initialization
   - Some memory systems require specific configuration

4. **Performance monitoring not working**
   - Ensure all systems are properly initialized
   - Check that brain.step() has been called

### Debug Scripts

For more detailed debugging, see the debug functions in the examples:
- `debug_brain_state()` - Detailed brain state information
- `trace_neural_activity()` - Trace specific neural activity
- `get_system_performance()` - Comprehensive performance metrics

## License

MIT License

## Authors

NLM Authors - Research project for artificial brain development

## Version

Enhanced NLM Python Bindings v2.0

For more information about NLM and artificial brain research, visit the official documentation.
