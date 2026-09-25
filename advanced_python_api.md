# Advanced Python API Documentation

## Overview

This document provides comprehensive documentation for the advanced Python API features of the NLM (Neural Learning Machine) framework. It covers all the newly implemented advanced features including memory systems, prediction systems, cognition systems, performance monitoring, and configuration builders.

## Advanced Configuration Builder

### ConfigBuilder Class

The `ConfigBuilder` class provides an advanced, fluent interface for creating complex brain configurations.

```python
import pynlm

# Basic configuration builder
config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 5000)\
    .set("brain.synapse_density", 0.2)\
    .set("brain.initial_weight_mean", 0.5)\
    .set("brain.initial_weight_std", 0.1)

# Create brain from builder
brain = pynlm.createBrain(config.build())
brain.initialize()
```

### Configuration Presets

```python
import pynlm

# Apply predefined presets
config = pynlm.createConfigBuilder()

# Performance preset for fast execution
config.applyPreset("performance")

# Accuracy preset for high precision
config = pynlm.createConfigBuilder()
config.applyPreset("accuracy")

# Balanced preset for middle ground
config = pynlm.createConfigBuilder()
config.applyPreset("balanced")
```

### Hierarchical Configuration

```python
import pynlm

# Create parent configuration
parent_config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 2000)\
    .set("plasticity.plasticity_rate", 0.01)

# Create child configuration that inherits from parent
child_config = pynlm.createConfigBuilder()\
    .set("brain.synapse_density", 0.15)

child_config.setParentConfig(parent_config)

# Child inherits parent's neuron_count
print(f"Inherited neurons: {child_config.getOr('brain.neuron_count', 1000)}")
```

## Memory Systems

### Working Memory

The working memory maintains active neural patterns and traces for immediate processing.

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access working memory
wm = brain.getWorkingMemory()
if wm:
    # Store a pattern
    pattern = [0.1, 0.5, 0.9, 0.3, 0.7]
    wm.store(pattern, strength=0.8)
    
    # Retrieve patterns
    retrieved = wm.retrieve()
    print(f"Retrieved pattern: {retrieved}")
    
    # Check memory activity
    activity = wm.getMemoryActivity()
    print(f"Memory activity level: {activity:.3f}")
    
    # Run competition
    wm.runCompetition()
    print(f"Active traces: {wm.getActiveTraces()}")
```

### Episodic Memory

The episodic memory stores and retrieves experiences as episodic items.

```python
import pynlm
from datetime import datetime

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access episodic memory
em = brain.getEpisodicMemory()
if em:
    # Create an episodic memory item
    memory_item = pynlm.EpisodicMemoryItem(
        timestamp=datetime.now().timestamp(),
        positionX=10.0,
        positionY=20.0,
        action=pynlm.ActionType.MoveForward,
        reward=1.0,
        sensoryState=[0.1, 0.2, 0.3, 0.4, 0.5],
        resultingSensoryState=[0.2, 0.3, 0.4, 0.5, 0.6],
        activeNeurons=[123, 456, 789]
    )
    
    # Store the episode
    em.storeEpisode(memory_item)
    
    # Retrieve similar episodes
    query = [0.15, 0.25, 0.35, 0.45, 0.55]
    similar = em.retrieveSimilar(query, maxResults=5)
    
    # Retrieve temporal episodes
    past_episodes = em.retrieveTemporal(0, 100, maxResults=3)
    
    # Get recent episodes
    recent = em.getRecentEpisodes(5)
    
    # Replay episodes
    if recent:
        em.replayEpisode(recent[0])
```

### Associative Memory

The associative memory creates and maintains associations between patterns.

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access associative memory
am = brain.getAssociativeMemory()
if am:
    # Create associations
    pattern_a = [0.1, 0.2, 0.3, 0.4, 0.5]
    pattern_b = [0.6, 0.7, 0.8, 0.9, 1.0]
    
    am.associate(pattern_a, pattern_b, strength=0.8)
    
    # Retrieve patterns
    query = [0.6, 0.7, 0.8, 0.9, 1.0]
    retrieved = am.retrieve(query, maxResults=5)
    
    # Get association strength
    strength = am.getAssociationStrength(pattern_a, pattern_b)
    print(f"Association strength: {strength:.3f}")
    
    # Spread activation
    activated = am.spreadActivation(query, steps=3)
```

## Prediction System

### Basic Prediction

The prediction system forecasts future states and computes prediction errors.

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access prediction system
pred_sys = brain.getPredictionSystem()
if pred_sys:
    # Create initial state
    initial_state = pynlm.SensoryInput()
    initial_state.setData([0.1, 0.2, 0.3, 0.4, 0.5])
    
    # Predict next state
    predicted = pred_sys.predictNextState(initial_state)
    
    # Create actual state
    actual_state = pynlm.SensoryInput()
    actual_state.setData([0.15, 0.25, 0.35, 0.45, 0.55])
    
    # Update predictions
    pred_sys.updatePredictions(predicted, actual_state)
    
    # Get metrics
    error = pred_sys.getPredictionError()
    confidence = pred_sys.getConfidence()
    history = pred_sys.getErrorHistory()
```

## Neuromodulation Systems

### Dopamine System

The dopamine system provides reward prediction and reinforcement learning signals.

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access dopamine system
dopamine = brain.getDopamine()
if dopamine:
    # Get current dopamine level
    level = brain.getDopamineLevel()
    print(f"Dopamine level: {level:.3f}")
    
    # Run simulation to see dopamine dynamics
    for step in range(50):
        brain.step(step)
        
        # Check dopamine level
        if step % 10 == 0:
            print(f"Step {step}: Dopamine={brain.getDopamineLevel():.3f}")
```

### Curiosity System

The curiosity system drives exploration through novelty and prediction error.

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access curiosity system
curiosity = brain.getCuriosity()
if curiosity:
    # Get curiosity level
    level = brain.getCuriosityLevel()
    print(f"Curiosity level: {level:.3f}")
    
    # Get novelty level
    novelty_level = curiosity.getNoveltyLevel()
    print(f"Novelty level: {novelty_level:.3f}")
    
    # Run simulation with curiosity
    for step in range(50):
        brain.step(step)
        
        # Inject sensory input to activate curiosity
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)
        
        if step % 10 == 0:
            print(f"Step {step}: Curiosity={brain.getCuriosityLevel():.3f}")
```

### Prediction Error System

The prediction error system computes differences between predicted and actual outcomes.

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access prediction error system
pred_error = brain.getPredictionError()
if pred_error:
    # Compute prediction error
    predicted = 0.8
    actual = 1.2
    
    pred_error.computeError(predicted, actual)
    
    # Get error metrics
    error = pred_error.getError()
    magnitude = pred_error.getMagnitude()
    history = pred_error.getHistory()
    
    print(f"Prediction error: {error:.3f}")
    print(f"Error magnitude: {magnitude:.3f}")
```

## Cognition Systems

### Attention System

The attention system focuses cognitive resources on relevant information.

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access attention system
attention = brain.getAttention()
if attention:
    # Focus on specific regions
    attention.focusOnRegion(pynlm.RegionId(0))
    
    # Check attended regions
    attended = attention.getAttendedRegions()
    print(f"Attended regions: {[r.index() for r in attended]}")
    
    # Check if neuron is attended
    neuron_id = pynlm.NeuronId(123)
    is_attended = attention.isAttended(neuron_id)
    print(f"Neuron {neuron_id.index()} attended: {is_attended}")
```

### Planner System

The planner system creates action sequences and plans future behavior.

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access planner system
planner = brain.getPlanner()
if planner:
    # Planner can create action sequences based on goals
    # Specific planner methods depend on implementation
    print("Planner system available for action planning")
```

### Concept Formation System

The concept formation system abstracts patterns into concepts.

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access concept formation system
concept_formation = brain.getConceptFormation()
if concept_formation:
    # Concept formation can analyze patterns and create abstract representations
    # Specific methods depend on implementation
    print("Concept formation system available for abstraction")
```

## Performance Monitoring

### PerformanceMonitor Class

The `PerformanceMonitor` class provides comprehensive performance tracking and reporting.

```python
import pynlm

# Create brain and performance monitor
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

monitor = pynlm.createPerformanceMonitor()

# Start monitoring for different phases
monitor.startPhase("initialization")
brain.initialize()
init_time = monitor.endPhase("initialization")
print(f"Initialization time: {init_time:.3f} seconds")

# Run simulation with monitoring
monitor.startPhase("simulation")
for step in range(100):
    brain.step(step)
    
    # Record step time
    monitor.recordStepTime(0.001)
    
    # Update neural counts
    monitor.updateNeuralCounts(brain.getActiveNeuronCount(), brain.getTotalSynapseCount())

sim_time = monitor.endPhase("simulation")
print(f"Simulation time: {sim_time:.3f} seconds")

# Get performance metrics
print(f"\nPerformance Summary:")
print(f"Average step time: {monitor.getAverageStepTime():.6f} s")
print(f"Total simulation time: {monitor.getTotalSimulationTime():.3f} s")
print(f"Steps per second: {monitor.getStepCount()/monitor.getTotalSimulationTime():.1f}")
print(f"Active neurons: {monitor.getActiveNeuronCount()}")
print(f"Active synapses: {monitor.getActiveSynapseCount()}")

# Get phase times
phase_times = monitor.getPhaseTimes()
if phase_times:
    print(f"\nPhase Times:")
    for phase, time in phase_times.items():
        print(f"  {phase}: {time:.3f} s")

# Get full report
print(f"\nFull Performance Report:")
print(monitor.getPerformanceReport())
```

### Batch Processing

The `BatchProcessor` class handles multiple simulation runs with comprehensive reporting.

```python
import pynlm

# Create complete agent system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Create batch processor and monitor
batch_processor = pynlm.createBatchProcessor()
monitor = pynlm.createPerformanceMonitor()

# Run multiple episodes
print("Running 20 episodes with 100 steps each...")
result = batch_processor.runEpisodes(agent, world, 20, 100, monitor)

# Print comprehensive report
print(f"\nBatch Processing Report:")
print(batch_processor.getBatchReport(result))

print(f"\nPerformance Report:")
print(monitor.getPerformanceReport())
```

## Factory Methods

### Advanced Factory Methods

The Python bindings provide advanced factory methods for creating different brain types and configurations.

```python
import pynlm

# Create different brain types using factory methods

# Standard brain with custom configuration
standard_config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 3000)\
    .set("brain.synapse_density", 0.1)
standard_brain = pynlm.createBrain(standard_config.build())

# Minimal brain for testing
minimal_brain = pynlm.createBrain(pynlm.createBrainWithType("minimal"))

# Complex brain with advanced features
complex_brain = pynlm.createBrain(pynlm.createBrainWithType("complex"))

# Create specific configuration presets
performance_brain = pynlm.createBrain(pynlm.createBrainWithType("performance"))
accuracy_brain = pynlm.createBrain(pynlm.createBrainWithType("accuracy"))
```

### Helper Classes

```python
import pynlm

# Create performance monitor
monitor = pynlm.createPerformanceMonitor()

# Create batch processor
batch_processor = pynlm.createBatchProcessor()

# Create serializer for state management
serializer = pynlm.createSerializer()

# Create debugger for troubleshooting
debugger = pynlm.createDebugger()

# Create experiment manager
experiment_manager = pynlm.createExperimentManager()
```

## Neuroscience Analysis

### Network Topology Analysis

Analyze the neural network structure and connectivity.

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(50):
    brain.step(i)

# Analyze network topology
topology = pynlm.analyzeNetworkTopology(brain)
print(topology)
```

### Development Pattern Analysis

Analyze brain development patterns and trajectories.

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Analyze development pattern
development = pynlm.analyzeDevelopmentPattern(brain)
print(development)
```

### Plasticity State Analysis

Analyze plasticity system state and parameters.

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Analyze plasticity state
plasticity = pynlm.analyzePlasticityState(brain)
print(plasticity)
```

### Memory Usage Analysis

Analyze memory system usage and statistics.

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Analyze memory usage
memory_usage = pynlm.Debugger.analyzeMemoryUsage(brain)
print(memory_usage)
```

## Serialization and State Management

### Complete State Management

Manage brain state, configuration, and memory through serialization.

```python
import pynlm
import tempfile

# Create and initialize complex brain
config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 1000)\
    .set("brain.synapse_density", 0.1)\
    .set("plasticity.plasticity_rate", 0.05)\
    .build()

brain1 = pynlm.createBrain(config)
brain1.initialize()

# Run simulation
for i in range(200):
    brain1.step(i)

# Save brain state
serializer = pynlm.createSerializer()

with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
    checkpoint_path = f.name

serializer.saveBrainState(brain1, checkpoint_path)
print(f"Brain state saved to {checkpoint_path}")

# Load brain state
brain2 = pynlm.createBrain(config)
brain2.initialize()
serializer.loadBrainState(brain2, checkpoint_path)
print("Brain state loaded successfully")
```

## Experiment Management

### Complete Experiment Workflow

Manage complex experiments with multiple runs and comprehensive analysis.

```python
import pynlm

# Create experiment configuration
experiment_config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 800)\
    .set("max_steps", 500)\
    .set("learning_rate", 0.01)\
    .set("enable_reward_modulation", True)\
    .set("enable_curiosity", True)\
    .set("enable_structural_plasticity", True)\
    .build()

# Create experiment
experiment_name = "memory_and_learning_experiment"
experiment = pynlm.createExperiment(experiment_name, experiment_config)

# Create experiment runner with monitoring
monitor = pynlm.createPerformanceMonitor()
runner = pynlm.createExperimentRunner(experiment, monitor)

# Run experiment
print(f"Running experiment: {experiment_name}")
pynlm.runExperiment(runner)

# Get experiment summary
experiment_summary = pynlm.getExperimentSummary(experiment)
print(f"\nExperiment Summary:")
print(experiment_summary)
```

## Advanced Integration

### Phase 6 Integration

Run the complete Phase 6 integration experiment with all systems.

```python
import pynlm

# Run Phase 6 integration experiment
from pynlm.Phase6IntegratedExperiment import Phase6IntegratedExperiment

experiment = Phase6IntegratedExperiment()

# Configure for integration test
config = pynlm.Phase6Config()
config.neuronCount = 500
config.maxSteps = 1000
config.regionCount = 3
config.connectionProbability = 0.05
config.enableDevelopment = True
config.enableCheckpointing = True

# Run integration experiment
print("Running Phase 6 Integration Experiment...")
result = experiment.run(config)

print(f"\nIntegration Results:")
print(f"Average reward: {result.totalReward:.3f}")
print(f"Average firing rate: {result.avgFiringRate:.3f}")
print(f"Episodes stored: {result.memoryEpisodesStored:.1f}")
print(f"Wall time: {result.totalWallClockTime:.3f} s")

# Verify integration
print(f"\nIntegration Verification:")
print(f"Working Memory: {'YES' if result.memoryWorkingMemoryIntegrated else 'NO'}")
print(f"Episodic Memory: {'YES' if result.memoryEpisodicMemoryIntegrated else 'NO'}")
print(f"Neuromodulation: {'YES' if result.neuromodulationIntegrated else 'NO'}")
print(f"Prediction: {'YES' if result.predictionIntegrated else 'NO'}")
print(f"Development: {'YES' if result.developmentIntegrated else 'NO'}")
```

### Complete Neuroscience Analysis

Run comprehensive neuroscience analysis.

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Perform comprehensive analysis
print("=== Neuroscience Analysis ===")

# Network topology
topology = pynlm.analyzeNetworkTopology(brain)
print(topology)

# Development pattern
development = pynlm.analyzeDevelopmentPattern(brain)
print(development)

# Plasticity state
plasticity = pynlm.analyzePlasticityState(brain)
print(plasticity)

# Memory usage
memory_usage = pynlm.Debugger.analyzeMemoryUsage(brain)
print(memory_usage)

# Profiling
profiler = pynlm.createDebugger()
profile_report = profiler.profileBrain(brain, steps=50)
print(profile_report)
```

## Debugging and Troubleshooting

### Complete Debugging Workflow

Debug and troubleshoot NLM systems with comprehensive tools.

```python
import pynlm

# Create and initialize complex brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Create debugger
debugger = pynlm.createDebugger()

# Profile brain activity
print("Profiling brain activity...")
profile_report = debugger.profileBrain(brain, steps=50)
print(profile_report)

# Analyze memory usage
print("\nAnalyzing memory usage...")
memory_analysis = debugger.analyzeMemoryUsage(brain)
print(memory_analysis)

# Dump state information
print("\nDumping neuron states...")
debugger.dumpNeuronStates(brain, "neuron_states.txt")

print("\nDumping synapse weights...")
debugger.dumpSynapseWeights(brain, "synapse_weights.txt")

print("\nDumping configuration...")
debugger.dumpConfiguration(brain.getConfig(), "config_dump.txt")

# Common troubleshooting patterns
print("\n=== Troubleshooting Patterns ===")

# Check brain initialization
if brain.getTotalNeuronCount() == 0:
    print("ERROR: Brain has no neurons. Did you call initialize()?")
else:
    print(f"✓ Brain has {brain.getTotalNeuronCount()} neurons")

# Check firing activity
if brain.getFiringNeuronCount() == 0:
    print("WARNING: No neurons are firing. Check for input sources.")
else:
    print(f"✓ {brain.getFiringNeuronCount()} neurons are firing")

# Check memory systems
if brain.getWorkingMemory() is None:
    print("WARNING: Working memory not available")
else:
    print(f"✓ Working memory: {brain.getWorkingMemory().getActiveTraces()} traces")

if brain.getEpisodicMemory() is None:
    print("WARNING: Episodic memory not available")
else:
    print(f"✓ Episodic memory: {brain.getEpisodicMemory().getEpisodeCount()} episodes")

# Check neuromodulation
if brain.getCuriosityLevel() == 0.0:
    print("WARNING: Curiosity system not activated")
else:
    print(f"✓ Curiosity: {brain.getCuriosityLevel():.3f}")

# Check performance
if brain.getAverageFiringRate() < 0.1:
    print("WARNING: Low firing rate. Learning may be ineffective.")
else:
    print(f"✓ Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
```

## Visualization and Analysis

### Network Visualization

Generate visualizations of neural networks.

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(50):
    brain.step(i)

# Generate visualizations
print("=== Network Visualization ===")

# Network topology
topology = pynlm.VisualizationHelper.generateNetworkTopology(brain)
print(topology)

# Activity heatmap
heatmap = pynlm.VisualizationHelper.generateActivityHeatmap(brain, threshold=0.5)
print(heatmap)

# Development timeline
timeline = pynlm.VisualizationHelper.generateDevelopmentTimeline(brain)
print(timeline)
```

### Sensory Analysis

Analyze sensory input and processing.

```python
import pynlm

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(50):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)

# Analyze sensory processing
print("=== Sensory Analysis ===")

# Get sensory percept
percept = world.getSensoryPercept()

# Generate sensory spectrogram
spectrogram = pynlm.VisualizationHelper.generateSensorySpectrogram(percept)
print(spectrogram)
```

## Error Handling and Exceptions

### Advanced Error Handling

NLM provides comprehensive error handling with custom exception classes.

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Try operations that might throw exceptions
try:
    # Access prediction system
    pred_sys = brain.getPredictionSystem()
    if pred_sys:
        state = pred_sys.predictNextState(pynlm.SensoryInput())
except pynlm.PredictionError as e:
    print(f"Prediction error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")

# Error classes available:
# - pynlm.BrainCreationError
# - pynlm.ConfigurationError
# - pynlm.ProfilingError
# - pynlm.ExperimentError
# - pynlm.SerializationError
```

## Configuration Schema and Validation

### Schema Validation

Validate configuration against schemas.

```python
import pynlm

# Create configuration
config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 1000)\
    .set("plasticity.plasticity_rate", 0.05)\
    .build()

# Validate configuration
validation_result = config.validate()
if validation_result.isValid():
    print("Configuration is valid!")
else:
    print("Configuration errors:")
    for error in validation_result.getErrors():
        print(f"  - {error}")

# Check specific key
if config.validateKey("brain.neuron_count"):
    print("Brain neuron count is valid")
```

### Schema Application

Apply configuration schemas for advanced validation.

```python
import pynlm

# Create configuration
config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 1000)\
    .set("plasticity.plasticity_rate", 0.05)\
    .build()

# Apply schema validation
if config.validateAgainstSchema("default_schema"):
    print("Configuration passes schema validation")
else:
    print("Configuration does not pass schema validation")
```

## Performance Optimization

### Performance Optimization Examples

Examples for optimizing NLM performance.

```python
import pynlm

# Create performance-optimized configuration
config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 2000)\
    .set("brain.synapse_density", 0.05)\
    .set("plasticity.plasticity_rate", 0.01)\
    .build()

# Create performance monitor
monitor = pynlm.createPerformanceMonitor()

# Initialize with monitoring
monitor.startPhase("initialization")
brain = pynlm.createBrain(config)
brain.initialize()
init_time = monitor.endPhase("initialization")

# Run optimized simulation
monitor.startPhase("simulation")
for step in range(1000):
    brain.step(step)
    monitor.recordStepTime(0.001)

sim_time = monitor.endPhase("simulation")
print(f"Optimized simulation completed in {sim_time:.3f} seconds")
```

### Memory Optimization

Examples for optimizing memory usage.

```python
import pynlm

# Create memory-optimized configuration
config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 500)\
    .set("memory.capacity", 1000)\
    .set("plasticity.plasticity_rate", 0.005)\
    .build()

brain = pynlm.createBrain(config)
brain.initialize()

# Analyze memory usage
analysis = pynlm.Debugger.analyzeMemoryUsage(brain)
print(analysis)
```

## Advanced Use Cases

### Reinforcement Learning Integration

Integrate NLM with reinforcement learning frameworks.

```python
import pynlm

# Create RL-compatible configuration
config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 2000)\
    .set("enable_reward_modulation", True)\
    .set("enable_curiosity", True)\
    .set("enable_structural_plasticity", True)\
    .build()

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# RL simulation loop
for episode in range(10):
    # Reset environment
    world.reset()
    agent.initialize(world)
    
    # Run episode
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Get action and apply RL
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Compute reward
        reward = world.computeReward(brain.getRegions()[0].get())
        agent.applyRewardModulation(reward, 0.0)
```

### Cognitive Task Training

Train NLM on cognitive tasks.

```python
import pynlm

# Create task-optimized configuration
config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 3000)\
    .set("enable_reward_modulation", True)\
    .set("enable_curiosity", True)\
    .set("enable_structural_plasticity", True)\
    .set("enable_development", True)\
    .build()

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=40, height=40)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Task training loop
for task in range(5):
    # Reset for new task
    world.reset()
    agent.initialize(world)
    
    # Train on task
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward and development
        reward = world.computeReward(brain.getRegions()[0].get())
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
```

### Multi-Agent System

Create and coordinate multiple agent brains.

```python
import pynlm

# Create multiple agents with shared world
config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 1000)\
    .build()

# Create shared world
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100)
world.reset()

# Create multiple agents
agents = []
for i in range(5):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agents.append(agent)

# Run multi-agent simulation
for step in range(100):
    world.update(0.1)
    
    # Each agent processes input independently
    for agent in agents:
        agent.processSensoryInput(world.getSensoryPercept())
        agent.decodeMotorCommand()
    
    # Agents interact through world
    # World applies effects from all agents
```

## Conclusion

This comprehensive documentation covers all advanced Python API features of NLM, including:

- **Advanced Configuration Builder**: ConfigBuilder for fluent configuration creation
- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Prediction System**: State prediction and error computation
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error systems
- **Cognition Systems**: Attention, planner, and concept formation systems
- **Performance Monitoring**: PerformanceMonitor and BatchProcessor classes
- **Factory Methods**: Advanced factory methods for different brain types
- **Neuroscience Analysis**: Network topology, development patterns, and plasticity analysis
- **Serialization**: Complete state management with save/load capabilities
- **Experiment Management**: Comprehensive experiment creation and running
- **Integration**: Phase 6 integration with all systems working together
- **Visualization**: Network topology, activity heatmaps, and development timelines
- **Error Handling**: Custom exception classes and advanced error handling
- **Schema Validation**: Configuration schema validation and application
- **Performance Optimization**: Performance and memory optimization examples
- **Advanced Use Cases**: Reinforcement learning, cognitive task training, and multi-agent systems

NLM provides a complete, advanced neuroscience simulation framework with comprehensive Python bindings for research, experimentation, and application development. The advanced features enable complex simulations of brain function, learning, memory, and cognition with full control over all aspects of the neural system.

---

## References

- [ENHANCED_CONFIG_IMPLEMENTATION.md](ENHANCED_CONFIG_IMPLEMENTATION.md) - Enhanced configuration implementation
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [docs/EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

## Next Steps

1. Read `HOW_TO_USE.md` for comprehensive usage examples
2. Read `docs/ARCHITECTURE.md` for system architecture
3. Experiment with different configurations and features
4. Refer to `advanced_python_api.md` for detailed API documentation
5. Check the [README.md](README.md) for installation and basic usage