# Enhanced NLM Python Bindings - User Guide

**An Experimental Artificial Developmental Brain with Enhanced Python API**

## Introduction

This is the enhanced version of the NLM (Neural Learning Machine) Python API. It provides:

- **Pythonic APIs**: Fluent interface design with method chaining
- **Context Managers**: Automatic resource management for simulation episodes
- **Batch Processing**: Efficient running of multiple experiments
- **Enhanced Configuration**: Better configuration management with presets
- **Debug & Profiling**: Built-in performance monitoring and analysis tools
- **Experiment Management**: Utilities for running and comparing experiments

## Quick Start (Copy & Paste)

### Enhanced Quick Start 1: Simplest Brain with Context Manager

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Use context manager for automatic cleanup
with pynlm.SimulationEpisode(brain, world, agent) as episode:
    # Configure and run in one line
    episode.configureWorld(10, 10, 8, 8).setAgentStart(5.0, 5.0)
    episode.enableFeatures(True, True, True, True)
    episode.runSteps(100)

print("Simulation complete!")
print("Neurons:", brain.getTotalNeuronCount())
```

### Enhanced Quick Start 2: Batch Experiment Management

```python
import pynlm
import time

# Create batch with multiple configurations
batch = (pynlm.SimulationBatch()
         .addEpisode("Learning", pynlm.createConfigWithPreset("research"))
         .addEpisode("Development", pynlm.createConfigWithPreset("small"))
         .addEpisode("Exploration", pynlm.createConfigWithPreset("medium")))

# Set consistent parameters for all episodes
batch.setSteps(500).setMaxTime(50.0)

print("Running batch experiment...")
start_time = time.time()

# Run all episodes with progress monitoring
batch.setVerbose(True)
batch.setProgressCallback(lambda msg: print(f"[PROGRESS] {msg}"))

results = batch.run()

print(f"Batch completed in {time.time() - start_time:.2f} seconds")

# Analyze results
batch_summary = batch.analyzeResults()
print(f"Summary: {batch_summary.totalSteps} steps, {batch_summary.totalNeurons} neurons")
print(f"Average firing rate: {batch_summary.averageFiringRate:.2f} Hz")

# Get best episodes
best_episodes = batch.getBestEpisodes(2)
print(f"Best episodes: {best_episodes}")

# Save results for later analysis
batch.saveResults("batch_results.json")
```

### Enhanced Quick Start 3: Experiment Factory Functions

```python
import pynlm

# Run learning experiment with one line
episode = pynlm.runLearningExperiment("My Learning", 1000)
print(f"Learning episode: {episode.getTotalSpikes()} spikes")

# Run development experiment
episode2 = pynlm.runDevelopmentalExperiment("Growth", 500, True)
print(f"Developmental stage: {episode2.getDevelopmentalStage()}")

# Run batch of experiments with different configs
step_counts = [500, 1000, 1500]
configs = ["small", "medium", "large"]
all_episodes = pynlm.runBatchExperiment("Comparison", step_counts, configs)
for i, ep in enumerate(all_episodes):
    print(f"Episode {i}: {ep.getTotalSpikes()} spikes, {ep.getFiringNeurons()} firing neurons")
```

## Key Improvements Over Original API

### 1. Pythonic Fluent Interface

**Before (verbose):**
```python
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(10, 10, 8, 8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

**After (concise):**
```python
with pynlm.SimulationEpisode(brain, world, agent).configureWorld(10, 10, 8, 8).setAgentStart(5.0, 5.0).enableFeatures(True, True, True, True) as episode:
    episode.runSteps(100)
```

### 2. Context Management

```python
# Automatic cleanup even if exceptions occur
with pynlm.SimulationEpisode(brain, world, agent) as episode:
    episode.runSteps(100)
    # Episode automatically reset when exiting the context

# Can also use manually if preferred
episode = pynlm.SimulationEpisode(brain, world, agent)
episode.runSteps(100)
episode.reset()
```

### 3. Enhanced Configuration

**New configuration presets:**
- `"small"` - 100 neurons, fast testing
- `"medium"` - 1000 neurons, balanced
- `"large"` - 5000 neurons, detailed simulation
- `"research"` - 10000 neurons, full features enabled

```python
# Easy configuration creation
config = pynlm.createConfigWithPreset("research")
brain = pynlm.createBrain(config)

# Or create default and modify
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
```

### 4. Batch Experiment Management

```python
# Define batch with multiple experiments
batch = (pynlm.SimulationBatch()
         .addEpisode("QuickTest", pynlm.createConfigWithPreset("small"))
         .addEpisode("FullRun", pynlm.createConfigWithPreset("research"))
         .addEpisode("Development", pynlm.createConfigWithPreset("medium")))

# Apply parameters to all or specific episodes
batch.setSteps(200)  # All episodes
batch.setMaxTime(10.0)  # All episodes

# Run with progress monitoring
results = batch.run()

# Analyze and compare
best = batch.getBestEpisodes(1)
analysis = batch.analyzeResults()
```

### 5. Performance Monitoring

```python
# Get detailed performance statistics
stats = episode.getPerformanceStats()
print(f"Simulation: {stats.totalSteps} steps in {stats.totalSimulationTime:.2f}s")
print(f"Performance: {stats.steps_per_second:.1f} steps/second")
print(f"Memory: {stats.memoryUsageMB:.1f} MB")
print(f"Neurons: {stats.totalNeurons}, Firing: {stats.firingNeurons}")
```

### 6. Experiment Utilities

**Factory functions for common experiments:**
- `runLearningExperiment(name, steps, config)`
- `runExplorationExperiment(name, steps, explorationFactor, config)`
- `runDevelopmentalExperiment(name, steps, enableStructuralPlasticity, config)`
- `runBatchExperiment(baseName, stepCounts, configs)`

## New Classes and Features

### SimulationEpisode (Context Manager)

```python
with pynlm.SimulationEpisode(brain, world, agent) as episode:
    # Fluent API for setup
    episode.configureWorld(20, 20, 12, 12)  # Configure world dimensions
    episode.setAgentStart(10.0, 10.0)       # Set starting position
    episode.enableFeatures(True, True, True, True)  # Enable learning features
    
    # Run simulation with multiple strategies
    episode.runSteps(100)                    # Fixed number of steps
    episode.runUntil(5.0)                    # Run until time reaches 5.0
    episode.runWhile(10.0, lambda: curiosity > 0.5)  # Run while condition is true
    
    # Get statistics
    print(f"Neurons: {episode.getTotalNeurons()}")
    print(f"Spikes: {episode.getTotalSpikes()}")
    print(f"Curiosity: {episode.getCuriosityLevel():.3f}")
    
    # Save/load state
    episode.saveState("checkpoint.bin")
    # episode.loadState("checkpoint.bin")
```

### SimulationBatch (Batch Processing)

```python
batch = pynlm.SimulationBatch()

# Add episodes with different configurations
batch.addEpisode("Baseline", pynlm.createConfigWithPreset("small"))
    .addEpisode("Advanced", pynlm.createConfigWithPreset("research"))
    .addEpisode("Development", pynlm.createConfigWithPreset("medium"))

# Configure batch parameters
batch.setSteps(500)                    # 500 steps per episode
batch.setMaxTime(25.0)                 # Maximum 25 seconds per episode
batch.setVerbose(True)                 # Print progress

# Set up progress monitoring
batch.setProgressCallback(lambda msg: print(f"[BATCH] {msg}"))

# Run all experiments
results = batch.run()

# Analyze results
best_indices = batch.getBestEpisodes(2)  # Get 2 best episodes
analysis = batch.analyzeResults()

print(f"Batch completed: {len(results)} episodes")
print(f"Best episodes: {best_indices}")
print(f"Average firing rate: {analysis.averageFiringRate:.2f} Hz")
print(f"Total steps: {analysis.totalSteps}")

# Save batch results
batch.saveResults("experiment_results.json")
```

### Enhanced Configuration

```python
# Preset configurations
config_small = pynlm.createConfigWithPreset("small")      # 100 neurons
config_medium = pynlm.createConfigWithPreset("medium")    # 1000 neurons  
config_large = pynlm.createConfigWithPreset("large")      # 5000 neurons
config_research = pynlm.createConfigWithPreset("research") # 10000 neurons, all features

# Create default and customize
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)           # Custom neuron count
config.set("plasticity.stdp.enable", True)       # Enable STDP
config.set("neuromod.dopamine.scale", 0.5)       # Dopamine scale
config.set("neuromod.curiosity.enable", True)    # Curiosity enabled

# Save/load configurations
config.saveToFile("my_config.json")
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")
```

## Enhanced Error Handling

The enhanced API provides better error handling:

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.NLMRuntimeError as e:
    print(f"Runtime error: {e}")
except pynlm.NLMConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.NLMMemoryError as e:
    print(f"Memory error: {e}")
```

## Performance Monitoring and Profiling

### Get Performance Statistics

```python
stats = episode.getPerformanceStats()
print(f"Simulation Performance:")
print(f"  Steps: {stats.totalSteps}")
print(f"  Time: {stats.totalSimulationTime:.2f}s")
print(f"  Speed: {stats.steps_per_second:.1f} steps/sec")
print(f"  Neurons: {stats.totalNeurons}")
print(f"  Firing neurons: {stats.firingNeurons}")
print(f"  Total spikes: {stats.totalSpikes}")
print(f"  Memory usage: {stats.memoryUsageMB:.1f} MB")
print(f"  Avg firing rate: {stats.averageFiringRate:.2f} Hz")
```

### Perception Snapshots

```python
# Get sensory input snapshots for analysis
snapshot = episode.getPerceptionSnapshot()
print(f"Perception data: {snapshot.toString()}")
print(f"Vision: {len(snapshot.vision)} pixels")
print(f"Touch: {len(snapshot.touch)} contacts")
print(f"Internal: {len(snapshot.internal)} signals")
print(f"Proprioception: {len(snapshot.proprioception)} values")
```

## Experiment Management and Comparison

### Save and Load Results

```python
# Save experiment results for later analysis
batch.saveResults("experiment_batch.json")

# Load results (requires implementing load functionality)
# batch.loadResults("experiment_batch.json")
```

### Compare Episodes

```python
# Get best episodes based on multiple criteria
best_indices = batch.getBestEpisodes(3)  # Get top 3

# Compare specific metrics between episodes
for i in best_indices:
    episode = batch.getResults()[i]
    print(f"Episode {i}: {episode.getTotalSpikes()} spikes, "
          f"{episode.getAverageFiringRate():.2f} Hz, "
          f"curiosity {episode.getCuriosityLevel():.3f}")
```

## Advanced Usage Examples

### 1. Learning and Development Experiment

```python
import pynlm

# Create developmental experiment
config = pynlm.createConfigWithPreset("research")
config.set("development.stages", 5)  # 5 developmental stages

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(30, 30, 16, 16)
world.setAgentStart(15.0, 15.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all features for comprehensive experiment
agent.enableRewardModulation(True)
agent.enableCuriosity(True) 
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run developmental learning experiment
with pynlm.SimulationEpisode(brain, world, agent) as episode:
    episode.configureWorld(30, 30, 16, 16).setAgentStart(15.0, 15.0)
    episode.enableFeatures(True, True, True, True)
    
    # Run developmental phases
    episode.runSteps(1000)  # Main learning phase
    
    # Get developmental statistics
    print(f"Developmental stages reached: {episode.getDevelopmentalStage()}")
    print(f"Final curiosity: {episode.getCuriosityLevel():.3f}")
    print(f"Final novelty: {episode.getNoveltyLevel():.3f}")
    print(f"Final prediction error: {episode.getPredictionError():.3f}")
```

### 2. Comparative Experiment Batch

```python
import pynlm

# Define different experiment configurations
experiments = [
    ("Baseline", pynlm.createConfigWithPreset("small"), 500),
    ("Standard", pynlm.createConfigWithPreset("medium"), 1000),
    ("Advanced", pynlm.createConfigWithPreset("research"), 1500),
    ("Development", pynlm.createConfigWithPreset("large"), 2000),
]

# Create and run batch
batch = pynlm.SimulationBatch()
for name, config, steps in experiments:
    batch.addEpisode(name, config).setSteps(steps)

# Run with detailed monitoring
batch.setVerbose(True)
batch.setProgressCallback(lambda msg: print(f"Executing: {msg}"))

results = batch.run()

# Analyze comparative results
analysis = batch.analyzeResults()
best = batch.getBestEpisodes(2)

print("\n=== Comparative Analysis ===")
print(f"Best performing configurations: {[experiments[i][0] for i in best]}")
print(f"Average firing rate across all: {analysis.averageFiringRate:.2f} Hz")
print(f"Total simulation time: {analysis.totalSimulationTime:.2f}s")
print(f"Steps per second: {analysis.steps_per_second:.1f}")

# Save comparative results
batch.saveResults("comparative_results.json")
print("Results saved to comparative_results.json")
```

### 3. Real-time Monitoring and Control

```python
import pynlm
import time

# Create experiment with real-time monitoring
def realTimeMonitor(episode, interval=1.0):
    """Monitor episode progress in real-time"""
    start_time = episode.getSimulationTime()
    last_time = start_time
    
    while episode.getSimulationTime() - start_time < interval:
        time.sleep(0.1)
        current_time = episode.getSimulationTime()
        if current_time > last_time:
            print(f"Progress: {current_time:.2f}s, "
                  f"Neurons: {episode.getFiringNeurons()}, "
                  f"Curiosity: {episode.getCuriosityLevel():.3f}")
        last_time = current_time

# Run experiment with monitoring
episode = pynlm.runLearningExperiment("RealTimeMonitor", 2000)
realTimeMonitor(episode, 5.0)  # Monitor for 5 seconds

# Get final results
stats = episode.getPerformanceStats()
print(f"Experiment completed: {stats.totalSteps} steps in {stats.totalSimulationTime:.2f}s")
```

## Installation

### Install Enhanced Version

```bash
pip install pynlm-enhanced
```

### Build from Source

```bash
# Install build dependencies
pip install scikit-build-core pybind11

# Build and install
pip install -e .  # development mode

# Or build only
pip install build
python -m build
```

## Migration Guide

### From Original API to Enhanced API

**Original API (verbose):**
```python
# Original
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(10, 10, 8, 8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

**Enhanced API (concise):**
```python
# Enhanced - with context manager (recommended)
with pynlm.SimulationEpisode(brain, world, agent) as episode:
    episode.configureWorld(10, 10, 8, 8).setAgentStart(5.0, 5.0)
    episode.enableFeatures(True, True, True, True)
    episode.runSteps(100)

# Enhanced - manual approach
episode = pynlm.SimulationEpisode(brain, world, agent)
episode.configureWorld(10, 10, 8, 8).setAgentStart(5.0, 5.0)
episode.enableFeatures(True, True, True, True)
episode.runSteps(100)
episode.reset()
```

## Benefits of Enhanced API

1. **Simplified Code**: Much shorter and more readable
2. **Automatic Resource Management**: Context managers prevent resource leaks
3. **Batch Processing**: Efficient execution of multiple experiments
4. **Better Error Handling**: Specific exception types for different error conditions
5. **Performance Monitoring**: Built-in statistics and profiling tools
6. **Experiment Management**: Utilities for running and comparing experiments
7. **Fluent Interface**: Method chaining for more readable code
8. **Backward Compatibility**: Original API still supported in core functionality

## Troubleshooting

### Common Issues

**"ImportError: No module named 'pynlm_enhanced'"**
```bash
pip install pynlm-enhanced
```

**"ImportError: Cannot import name 'SimulationEpisode'"**
```bash
# The enhanced module may not be installed properly
pip install -e . --force-reinstall
```

**"RuntimeError: Brain not initialized""**
```python
# Always initialize before using
brain = pynlm.createBrain(config)
brain.initialize()  # Required!
```

**"Memory issues with large experiments""**
```python
# Use smaller configurations for memory-constrained environments
config = pynlm.createConfigWithPreset("small")
# or
config = pynlm.createConfigWithPreset("medium")
```

## Further Reading

- [Original NLM Documentation](https://nlm.readthedocs.io)
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

## License

MIT License

## Authors

Enhanced Python bindings by NLM development team
Original NLM framework by NLM authors

---

*Version 0.1.0-enhanced*
*Enhanced Python API for NLM (Neural Learning Machine)*
