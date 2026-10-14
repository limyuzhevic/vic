# NLM (Neural Learning Machine) - API Reference

**Comprehensive reference for the enhanced NLM Python API**

## Overview

This API reference documents the enhanced NLM Python bindings (pynlm) with Phase 6 functionality. It covers all classes, functions, and utilities available for brain simulation, experimentation, and analysis.

## Table of Contents

- [Configuration Classes](#configuration-classes)
- [Brain Simulation Classes](#brain-simulation-classes)
- [Agent and World Classes](#agent-and-world-classes)
- [Sensory and Motor Classes](#sensory-and-motor-classes)
- [Utility Classes](#utility-classes)
- [Advanced Functions](#advanced-functions)
- [Experimental Features](#experimental-features)
- [Data Structures](#data-structures)
- [Error Handling](#error-handling)
- [Migration Guide](#migration-guide)

## Configuration Classes

### Config

**Configuration class for NLM system with batch operations**

```python
# Creation
config = Config()  # Default configuration

# File I/O
config.loadFromFile("config.json")     # Load from JSON or TOML
config.saveToFile("config.json")       # Save to JSON or TOML
config.loadFromArgs(argc, argv)       # Load from command line args

# Query operations
config.has("brain.neuron_count")        # Check if key exists
config.getKeys()                        # Get all configuration keys
config.clear()                          # Clear all entries
config.summary()                        # Get configuration summary

# Batch operations
config.batchUpdate({"neuron_count": 1000})  # Batch update from dict
config.toDictionary()                   # Convert to Python dictionary
config.clone()                          # Create copy
config.merge(other_config)             # Merge two configurations

# Direct value access (conceptual - actual API may vary)
config.set("brain.neuron_count", 1000)
config.getOr<int64_t>("brain.neuron_count", 1000)
```

#### Configuration Keys

Common configuration keys include:

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential |
| `brain.v_rest` | float | -70.0 | Resting potential |
| `brain.v_reset` | float | -75.0 | Reset potential after spike |
| `brain.tau_mem` | float | 20.0 | Membrane time constant |
| `brain.tau_ref` | float | 2.0 | Refractory period |

#### Specialized Configuration Builders

```python
# High-performance configuration
perf_config = pynlm.createHighPerformanceConfig()

# Learning-focused configuration  
learning_config = pynlm.createDevelopmentConfig()

# Exploration-focused configuration
explore_config = pynlm.createExplorationConfig()

# Balanced general-purpose configuration
balanced_config = pynlm.createDefaultConfig()
```

## Brain Simulation Classes

### Brain

**Central neural simulation brain class with Phase 6 integration**

```python
# Creation and initialization
config = Config()
brain = Brain(config)
brain.initialize()

# Core simulation methods
brain.step(step_number)                    # Perform simulation step
brain.step(step_number, time)              # Step with timestamp
brain.reset()                              # Reset brain state
brain.save("checkpoint.bin")               # Save state
brain.load("checkpoint.bin")               # Load state

# Input/output
brain.receiveSensoryInput(input)           # Inject sensory data
brain.injectCurrent(neuron_id, current)   # Inject current to specific neuron
brain.injectCurrentToNeurons(type, current) # Inject to all neurons of type
brain.produceAction()                      # Get motor action

# Statistics
brain.getTotalNeuronCount()                # Total neurons
brain.getTotalSynapseCount()               # Total synapses
brain.getActiveNeuronCount()               # Active neurons
brain.getFiringNeuronCount()               # Currently firing
brain.getAverageFiringRate()               # Average firing rate
brain.getExcitationInhibitionRatio()      # E/I balance
brain.getTotalSpikeCount()                 # Total spikes
brain.getDevelopmentalStage()             # Current developmental stage
brain.setDevelopmentalStage(stage)       # Set developmental stage

# Memory systems
brain.getWorkingMemory()                   # Access working memory
brain.getEpisodicMemory()                  # Access episodic memory
brain.getAssociativeMemory()              # Access associative memory

# Prediction systems
brain.getPredictionSystem()                # Access prediction system
brain.getPredictionErrorSignal()          # Get prediction error

# Cognition systems
brain.getPlanner()                         # Access neural planner
brain.getConceptFormation()                # Access concept formation
brain.getAttention()                       # Access attentional selection

# Neuromodulation systems
brain.getDopamine()                        # Access dopamine system
brain.getCuriosity()                       # Access curiosity system
brain.getNovelty()                         # Access novelty detection

# Configuration access
brain.getConfig()                          # Get configuration

# Advanced methods
brain.benchmark(steps)                     # Performance benchmarking
brain.extractNeuronStates()                # Extract detailed neuron states
brain.exportNetwork(format)                # Export network data
brain.runExperiment(world, agent, steps)  # Run complete experiment
brain.generateReport(metrics)              # Generate analysis report
brain.clone()                             # Create brain copy
brain.logStatus()                          # Log brain status
```

### NeuralRegion

**Neural region containing populations of neurons**

```python
region = brain.getRegion(region_id)
region.getId()                             # Get region ID
region.getName()                          # Get region name
region.getNeuronCount()                   # Number of neurons
region.getPopulationCount()               # Number of populations
region.getNeurons()                       # Get all neurons
region.getPopulations()                   # Get all populations
region.addNeuron(neuron)                  # Add neuron to region
region.removeNeuron(neuron_id)            # Remove neuron
```

### NeuralPopulation

**Population of neurons with shared properties**

```python
population = region.getPopulation(pop_id)
population.getId()                        # Get population ID
population.getNeuronType()                # Get neuron type
population.getNeuronCount()               # Number of neurons
population.getNeurons()                   # Get neurons in population
population.setNeuronType(type)            # Set neuron type
```

### Neuron

**Individual neuron with LIF dynamics**

```python
neuron = population.getNeuron(neuron_id)
neuron.getId()                            # Get neuron ID
neuron.getType()                          # Get neuron type
neuron.getMembranePotential()             # Get membrane potential
neuron.getThreshold()                     # Get firing threshold
neuron.getRefractoryTime()                # Get refractory time
neuron.getRestingPotential()              # Get resting potential
neuron.getFiringState()                   # Get firing state
neuron.injectCurrent(current)             # Inject current
neuron.step(timestep)                     # Take simulation step
```

### Synapse

**Synaptic connection between neurons**

```python
synapse = region.getSynapse(synapse_id)
synapse.getId()                           # Get synapse ID
nsynapse.getSource()                      # Get source neuron
nsynapse.getTarget()                      # Get target neuron
synapse.getWeight()                       # Get synaptic weight
synapse.getType()                         # Get synapse type
synapse.getDelay()                        # Get transmission delay
synapse.setWeight(weight)                 # Set synaptic weight
synapse.enablePlasticity(hebbian, stdp)   # Enable plasticity
```

## Agent and World Classes

### AgentBrain

**Agent brain interface connecting NLM brain to world**

```python
agent = AgentBrain(brain)
agent.initialize(world)                    # Initialize with world
getSensoryInputSize()                     # Get expected sensory input size
agent.getMotorOutputSize()                 # Get expected motor output size
agent.processSensoryInput(percept)        # Process sensory input
agent.decodeMotorCommand()                # Decode motor command
agent.applyRewardModulation(reward, pred) # Apply reward-based neuromodulation
agent.updateDevelopment(timestep)         # Update development system
getDevelopmentalStage()                   # Get current developmental stage
agent.getNeuromodulationLevel()           # Get neuromodulation level
agent.getCuriosityLevel()                 # Get curiosity level
agent.getNoveltyLevel()                   # Get novelty level
agent.getPredictionError()                # Get prediction error
agent.reset()                             # Reset agent
agent.getBrain()                          # Get underlying brain

# Subsystem control
agent.enableRewardModulation(True/False)
agent.enableCuriosity(True/False)
agent.enableDevelopment(True/False)
agent.enableStructuralPlasticity(True/False)

# Custom agent (extends AgentBrain)
custom_agent = CustomAgentBrain(brain)
```

### SimpleWorld

**Simple 2D world for NLM simulation**

```python
world = SimpleWorld()
world.configure(width, height, vision_width, vision_height)  # Configure dimensions
world.reset()                               # Reset to initial state
world.setAgentStart(x, y)                   # Set agent starting position
world.update(timestep)                      # Update world
world.applyMotorCommand(cmd, current_time) # Apply motor command
world.getSensoryPercept()                   # Get sensory percept
world.getAgentBody()                        # Get agent body state
world.addObject(obj)                        # Add object to world
world.removeObject(x, y)                    # Remove object
world.isValidPosition(x, y)                 # Check position validity

# Advanced methods
world.batchUpdate(commands, timestep)     # Batch updates
world.simulateEpisode(agent, steps, timestep) # Simulate complete episode
world.createRandomEnvironment(resource_density, hazard_density) # Create random environment
world.exportState(format)                  # Export world state
```

### AgentBody

**Agent body state containing physical properties**

```python
body = world.getAgentBody()
body.x, body.y                         # Position
body.orientation                        # Orientation (radians)
body.velocityX, body.velocityY           # Velocity
body.angularVelocity                     # Angular velocity
body.energy, body.health                 # Energy and health (0-1)
body.age                               # Developmental age
body.isMoving, body.isTurning            # Movement state
body.reset()                           # Reset to initial state
```

## Sensory and Motor Classes

### SensoryPercept

**Sensory data packet - what the agent perceives**

```python
percept = world.getSensoryPercept()
percept.getVision()                      # Get vision data
percept.setVision(data)                  # Set vision data
percept.getVisionWidth()                 # Get vision width
percept.getVisionHeight()                # Get vision height
percept.getTouch()                       # Get touch data
percept.setTouch(data)                   # Set touch data
percept.getInternal()                    # Get internal signals
percept.setInternal(data)                # Set internal signals
percept.getProprioception()              # Get proprioception
percept.setProprioception(data)          # Set proprioception
percept.getAudio()                       # Get audio data
percept.setAudio(data)                   # Set audio data
percept.getAllSignals()                  # Get all signals concatenated
percept.getTimestamp()                   # Get timestamp
percept.setTimestamp(timestamp)          # Set timestamp
```

### Action

**Action representation for motor output**

```python
action = brain.produceAction()
action.getType()                         # Get action type
action.setType(type)                     # Set action type
action.getParameters()                   # Get parameters
action.setParameters(params)             # Set parameters
action.getName()                         # Get action name
action.clone()                           # Create copy
```

### SensoryInput

**Base class for sensory input**

```python
input = brain.produceAction()  # This is actually Action, not SensoryInput
# SensoryInput is base class for Vision, Audio, InternalSignals
sensory_input.getType()              # Get sensory type
sensory_input.getData()               # Get raw data
sensory_input.getDimensions()         # Get dimensionality
sensory_input.getTimestamp()          # Get timestamp
```

## Utility Classes

### Timer

**Performance timer for benchmarking**

```python
timer = Timer()
elapsed = timer.elapsed()             # Get elapsed time in seconds
timer.reset()                          # Reset timer
```

### PerformanceMetrics

**Performance monitoring utilities**

```python
metrics = PerformanceMetrics()
metrics.totalTime = 1.0                  # Total time in seconds
metrics.stepsCompleted = 1000           # Steps completed
metrics.totalSpikes = 5000              # Total spikes
metrics.avgFiringRate = 5.0             # Average firing rate
metrics.avgNeuromodulation = 0.3        # Average neuromodulation
metrics.reset()                         # Reset all metrics
toString()                             # Get formatted string
metrics.toDictionary()                 # Convert to dictionary
saveToFile("metrics.txt")              # Save to file
```

### SimulationBatch

**Batch processing utilities**

```python
batch = SimulationBatch("ExperimentBatch")
batch.addBrain(brain)                   # Add brain to batch
batch.addWorld(world)                    # Add world to batch
batch.addAgent(agent)                    # Add agent to batch
batch.enableParallel(True/False)         # Enable parallel execution
batch.setNumThreads(num)                # Set thread count
batch.runBatch(steps, timestep)         # Run complete batch
batch.runSequential(steps, timestep)    # Run sequentially
batch.runParallel(steps, timestep)      # Run in parallel
batch.clear()                           # Clear batch
batch.size()                            # Get batch size
```

## Advanced Functions

### Configuration Utilities

```python
pynlm.createDefaultConfig()                    # Default configuration
pynlm.createHighPerformanceConfig()            # Performance-optimized config
pynlm.createDevelopmentConfig()                # Learning-focused config
pynlm.createExplorationConfig()               # Curiosity-driven config

# Configuration analysis
pynlm.analyzeConfigEfficiency(config1, config2, steps)  # Compare configurations
pynlm.generateConfigReport(config)               # Generate configuration report
```

### Brain Analysis Functions

```python
# Feature extraction
features = pynlm.extractFeatures(brain)      # Extract neural features
network_data = pynlm.exportNetwork(brain)    # Export network data

# Performance analysis
metrics = pynlm.runExperiment(brain, world, agent, steps)  # Run experiment and get metrics
report = pynlm.generateReport(brain, metrics, format)      # Generate report

# Benchmarking
benchmark_result = brain.benchmark(steps)   # Benchmark brain performance
```

### Experiment Management

```python
# Phase 6 experiments
pynlm.runPhase6Experiment("exp_name", config, steps, timestep)  # Run Phase 6 experiment

# Multi-agent experiments
pynlm.runMultiAgentExperiment(brains, worlds, agents, steps, timestep)  # Multi-agent experiment

# Batch benchmarks
pynlm.runBatchBenchmark(brains, steps, timestep)  # Batch benchmark
```

### Visualization and Analysis

```python
# Visualization
viz = VisualizationTools()
fig = viz.plotSimulationResults(data, save_path="results.png")  # Create plots
viz.create_network_visualization(brain)                      # Create network visualization

# Analysis
analysis = viz.analyzeLearningCurves(data)                    # Analyze learning curves
report = viz.create_analysis_report(brain, metrics)           # Create comprehensive report
```

### Experimental Tools

```python
# Architecture exploration
explorer = NeuralNetworkExplorer()
results = explorer.run_architectural_search(architectures, world_size, trials_per_arch)

# Protocol management
protocol = ExperimentalProtocol("learning")
comparison = protocol.run_comparison_protocol(configs, world_size, duration_steps)

# Advanced simulation
manager = AdvancedSimulationManager()
experiment_data = manager.run_adaptive_simulation(config, world_size, max_steps)
```

## Data Structures

### ID Classes

```python
# Neuron identifier
neuron_id = NeuronId()                    # Default constructor
neuron_id = NeuronId(42)                  # Value constructor
print(neuron_id.value)                    # Access value
print(neuron_id.index())                  # Get index

# Synapse identifier  
synapse_id = SynapseId(100)

# Region identifier
region_id = RegionId(5)

# Population identifier
pop_id = PopulationId(3)

# Serialization support
print(repr(neuron_id))                    # String representation
print(hash(neuron_id))                    # Hash support
print(neuron_id == other_id)              # Equality comparison
```

### Enumeration Types

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait

# Motor commands
pynlm.MotorCommand.MoveForward
pynlm.MotorCommand.TurnLeft
pynlm.MotorCommand.Interact
pynlm.MotorCommand.Wait

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

## Error Handling

### Custom Exceptions

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Error Categories

| Error Type | Description | Typical Causes |
|------------|-------------|----------------|
| `RuntimeError` | Runtime execution errors | Invalid configuration, initialization failures |
| `ValueError` | Invalid parameter values | Out-of-range parameters, invalid types |
| `NotImplementedError` | Not implemented features | Experimental features not compiled |
| `MemoryError` | Memory allocation failures | Large brain configurations |

## Migration Guide

### 1. Basic Migration (Phase 2 → Phase 6)

```python
# OLD CODE (Phase 2)
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")

# NEW CODE (Phase 6 enhanced)
import pynlm

# Use enhanced configuration for Phase 6
config = pynlm.createDevelopmentConfig()  # Better for learning
brain = pynlm.createBrain(config)
brain.initialize()

# Run with monitoring and statistics
import time
start_time = time.time()
for i in range(100):
    brain.step(i)
elapsed = time.time() - start_time

print(f"Enhanced simulation completed in {elapsed:.3f}s")
print(f"Your brain thought {brain.getTotalSpikeCount()} times")
print(f"Average firing rate: {brain.getAverageFiringRate():.4f} Hz")

# Get additional insights
features = pynlm.extractFeatures(brain)
print(f"Brain features: {features}")

# Export for analysis
brain.exportNetwork("brain_network.json")
print("Network exported for analysis")
```

### 2. Advanced Migration Examples

```python
# Example 1: Learning-focused migration
def migrate_to_advanced_learning():
    """Migrate to advanced learning capabilities."""
    
    # OLD: Simple learning
    old_config = pynlm.createDefaultConfig()
    old_brain = pynlm.createBrain(old_config)
    old_brain.initialize()
    
    # NEW: Advanced learning with development
    new_config = pynlm.createDevelopmentConfig()
    new_config.set("development.enable", True)
    new_config.set("neuromod.curiosity.enable", True)
    new_config.set("plasticity.structural.enable", True)
    
    new_brain = pynlm.createBrain(new_config)
    new_brain.initialize()
    
    return new_brain

# Example 2: Performance benchmarking migration
def migrate_to_performance_analysis():
    """Migrate to performance analysis capabilities."""
    
    # OLD: Basic timing
    import time
    start = time.time()
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    for i in range(1000):
        brain.step(i)
    elapsed = time.time() - start
    
    # NEW: Comprehensive benchmarking
    benchmark_result = brain.benchmark(1000)
    
    return benchmark_result

# Example 3: Experiment management migration
def migrate_to_experimental():
    """Migrate to experimental management capabilities."""
    
    # OLD: Single experiment
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    # ... run single experiment ...
    
    # NEW: Multiple experiment comparison
    configs = {
        'basic': pynlm.createDefaultConfig(),
        'optimized': pynlm.createHighPerformanceConfig(),
        'learning': pynlm.createDevelopmentConfig()
    }
    
    results = {}
    for name, config in configs.items():
        brain = pynlm.createBrain(config)
        brain.initialize()
        results[name] = brain.benchmark(1000)
    
    return results
```

### 3. Backward Compatibility

The enhanced NLM API maintains full backward compatibility:

```python
# All old code continues to work
import pynlm

# Old API still works
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)

# New API available for advanced users
config = pynlm.createDevelopmentConfig()
brain = pynlm.createBrain(config)
brain.initialize()
brain.benchmark(100)  # New feature
```

## Conclusion

The enhanced NLM Python API provides comprehensive support for Phase 6 neural simulation with:

### Key Features

1. **Core Simulation**: Enhanced Brain, AgentBrain, and SimpleWorld classes
2. **Advanced Configuration**: Specialized config builders for different use cases
3. **Performance Monitoring**: Timer, PerformanceMetrics, and benchmarking tools
4. **Batch Processing**: SimulationBatch for running multiple simulations efficiently
5. **Experiment Management**: Protocol-based experimentation and comparison
6. **Visualization Tools**: Comprehensive plotting and network visualization
7. **Analysis Capabilities**: Feature extraction, network export, and reporting
8. **Experimental Tools**: Architecture search, protocol management, and simulation optimization

### Benefits

- **Accessibility**: Simple API for beginners with comprehensive documentation
- **Power User Features**: Advanced tools for researchers and developers
- **Performance Optimization**: Built-in profiling and optimization tools
- **Experimental Flexibility**: Support for diverse experimental paradigms
- **Analysis Capabilities**: Comprehensive analysis and reporting tools
- **Integration Support**: Database and web API integration examples
- **Backward Compatibility**: Existing code continues to work without changes

### Usage Examples

```python
# Basic usage (accessible)
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Advanced usage (power features)
import pynlm
config = pynlm.createDevelopmentConfig()
brain = pynlm.createBrain(config)
brain.initialize()
metrics = brain.benchmark(1000)
features = pynlm.extractFeatures(brain)
```

The NLM system is now ready for both educational use and advanced research applications in Phase 6 neural simulation! 🚀