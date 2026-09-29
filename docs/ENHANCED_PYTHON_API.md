# Enhanced NLM Python Bindings - Usage Examples

This document provides comprehensive examples for using the enhanced NLM Python bindings.

## Overview

The enhanced NLM Python bindings provide a comprehensive API for working with the Neural Learning Machine framework. It includes:

- **Enhanced Configuration System**: Pythonic configuration builders and presets
- **Advanced Simulation Management**: High-level API for running simulations
- **Performance Monitoring**: Built-in profiling and statistics collection
- **Debug and Analysis Tools**: Visualization and debugging utilities
- **Interactive Command Line**: Advanced CLI for power users
- **Error Handling**: Custom exceptions with detailed error messages

## Installation

```bash
# Install from source (after building the C++ library)
pip install -e .

# Or install the Python bindings directly
pip install pynlm
```

## Basic Usage

### Creating a Simple Brain

```python
import pynlm

# Create a brain with minimal configuration
brain = pynlm.createBrain(pynlm.ConfigPresets.minimal())
brain.initialize()

# Run some simulation steps
for i in range(100):
    brain.step(i)

print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Creating a Research-Grade Brain

```python
import pynlm

# Create a research-grade brain with full configuration
brain = pynlm.createBrain(pynlm.ConfigPresets.research())
brain.initialize()

# Run simulation with progress reporting
result = brain.runSteps(1000, progress=lambda p: print(f"Progress: {p*100:.1f}%") )

print(f"Simulation completed: {result.stepsCompleted} steps")
print(f"Total spikes: {result.totalSpikes}")
print(f"Average firing rate: {result.averageFiringRate:.2f} Hz")
```

## Configuration System

### Using Configuration Presets

```python
import pynlm

# Research preset - for scientific experiments
research_config = pynlm.ConfigPresets.research()

# Agent preset - for reinforcement learning
agent_config = pynlm.ConfigPresets.agent()

# Minimal preset - for quick testing
minimal_config = pynlm.ConfigPresets.minimal()

# Experimental preset - for novel research
experimental_config = pynlm.ConfigPresets.experimental()

# Create brains with different configurations
brain1 = pynlm.createBrain(research_config)
brain2 = pynlm.createBrain(agent_config)
brain3 = pynlm.createBrain(minimal_config)
```

### Using Configuration Builder

```python
import pynlm

# Build custom configuration with method chaining
config = (pynlm.createConfigBuilder()
          .withNeuronCount(2000)
          .withRegionCount(4)
          .withConnectionProbability(0.1f)
          .withSTDP(0.015f, 0.018f, 25.0f)
          .withHebbianLearning(0.005f)
          .withDevelopment(true, 900.0)
          .withNeuromodulation(true, true, true)
          .withMemoryCapacity(1000, 2000)
          .withPrediction(true, 0.7f)
          .withCognition(true, true)
          .build())

brain = pynlm.createBrain(config)
```

## Advanced Simulation Management

### Using EnhancedBrain with Context Manager

```python
import pynlm

# Use EnhancedBrain with context manager for automatic cleanup
with pynlm.createBrain(pynlm.ConfigPresets.research()) as brain:
    brain.initialize()
    
    # Run simulation
    result = brain.runSteps(500)
    
    print(f"Steps completed: {result.stepsCompleted}")
    print(f"Total spikes: {result.totalSpikes}")
    print(f"Firing rate: {result.averageFiringRate:.2f} Hz")
```

### Running Agent Simulation

```python
import pynlm

# Create a complete simulation manager
sim = pynlm.createSimulationManager()
sim.createConfig("agent").createWorld(40, 40, 12, 12)
sim.initializeBrain()

# Run agent simulation
result = sim.runAgentSimulation(2000, enable_learning=True)

print(f"Simulation results:")
print(f"  Steps: {result.steps}")
print(f"  Total reward: {result.totalReward:.2f}")
print(f"  Final curiosity: {result.finalCuriosity:.3f}")
print(f"  Final novelty: {result.finalNovelty:.3f}")
print(f"  Average firing rate: {result.averageFiringRate:.2f} Hz")
print(f"  Trajectory points: {len(result.trajectory)}")
```

## Performance Monitoring

### Using Performance Monitor

```python
import pynlm

# Create performance monitor
monitor = pynlm.createPerformanceMonitor()

# Create brain with performance monitoring
brain = pynlm.createBrain(pynlm.ConfigPresets.research())
brain.initialize()

# Run simulation with monitoring
monitor.startMeasurement()
result = brain.runSteps(500)
monitor.stopMeasurement()

# Collect performance metrics
report = monitor.generateReport()

print(f"Performance Report:")
print(f"  Average CPU time: {report.avgCPUTime:.3f}s")
print(f"  Average wall time: {report.avgWallTime:.3f}s")
print(f"  CPU efficiency: {report.cpuEfficiency:.2f}")
print(f"  Max memory usage: {report.maxMemoryUsage} bytes")
print(f"  Total spikes: {report.totalSpikes}")
print(f"  Average firing rate: {report.avgFiringRate:.2f} Hz")
```

### Custom Performance Monitoring

```python
import pynlm
import time

# Create custom performance monitoring
monitor = pynlm.PerformanceMonitor()
brain = pynlm.createBrain(pynlm.ConfigPresets.agent())
brain.initialize()

# Run multiple simulations and collect metrics
for i in range(5):
    monitor.startMeasurement()
    
    # Record spike count before simulation
    spikes_before = brain.getBrain().getTotalSpikeCount()
    
    result = brain.runSteps(200)
    
    monitor.stopMeasurement()
    
    # Record metrics
    monitor.recordCPUTime(result.elapsedTime)
    monitor.recordMemoryUsage(512 * 1024 * 1024)  # 512 MB (example)
    monitor.recordSpikeCount(result.totalSpikes)
    monitor.recordFiringRate(result.averageFiringRate)

# Generate final report
report = monitor.generateReport()
print(f"Batch performance report:")
print(f"  Measurements: {report.measurementCount}")
print(f"  Average CPU efficiency: {report.cpuEfficiency:.2f}")
```

## Debug and Analysis Tools

### Visualizing Brain Connectivity

```python
import pynlm

brain = pynlm.createBrain(pynlm.ConfigPresets.research())
brain.initialize()

# Generate connectivity visualization
viz = pynlm.visualizeBrainConnectivity(brain.getBrain())
print(viz)

# Save visualization to file
pynlm.saveBrainState(brain.getBrain(), "brain_state.json")
```

### Analyzing Brain State

```python
import pynlm

brain = pynlm.createBrain(pynlm.ConfigPresets.agent())
brain.initialize()

# Analyze brain state
analysis = pynlm.analyzeBrainState(brain)

print(f"Brain Analysis:")
print(f"  Total neurons: {analysis['brain_statistics']['total_neurons']}")
print(f"  Total synapses: {analysis['brain_statistics']['total_synapses']}")
print(f"  E/I ratio: {analysis['brain_statistics']['e_i_ratio']:.2f}")
print(f"  Working memory traces: {analysis['memory_systems']['working_memory']['traces']}")
```

### Exporting Brain State

```python
import pynlm

brain = pynlm.createBrain(pynlm.ConfigPresets.research())
brain.initialize()

# Export brain state for later use
pynlm.exportBrainState(brain, "research_brain.nbm")

# Import brain state later
brain2 = pynlm.createBrain(pynlm.ConfigPresets.research())
brain2.importBrainState("research_brain.nbm")
```

## Command Line Interface

### Quick Start with CLI

```bash
# Create and run a research simulation
python nlm_cli.py research 30 30 5000

# Create and run agent training
python nlm_cli.py agent 100 200

# Create custom simulation
python nlm_cli.py custom configs/research.json "40 40"

# Run with advanced monitoring
python nlm_cli.py run results.json true 1000

# Run agent training with checkpointing
python nlm_cli.py train 50 ./agent_runs true

# Enter interactive mode
python nlm_cli.py interactive
```

### CLI Examples

```bash
# Run a research simulation with output file
./nlm_cli.py research 30 30 5000 output.json

# Run agent training with custom output directory
./nlm_cli.py agent 200 300 ./my_agent_results

# Create and run experiments
./nlm_cli.py experiments plasticity development novelty ./exp_results

# Analyze current simulation
./nlm_cli.py analyze brain_analysis.json
```

## Advanced Features

### Custom Plasticity Rules

```python
import pynlm

# Create brain with custom configuration
config = pynlm.createConfigBuilder()
config.withSTDP(0.025f, 0.03f, 30.0f)  # Custom STDP parameters
config.withHebbianLearning(0.015f)     # Custom Hebbian learning rate
config.withSynaptogenesis(0.0002f)    # Custom synaptogenesis rate

brain = pynlm.createBrain(config.build())
brain.initialize()
```

### Developmental Stages

```python
import pynlm

brain = pynlm.createBrain(pynlm.ConfigPresets.agent())
brain.initialize()

# Check developmental stage
print(f"Current developmental stage: {brain.getBrain().getDevelopmentalStage()}")

# Force developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
```

### Neuromodulation Control

```python
import pynlm

sim = pynlm.createSimulationManager()
sim.createConfig("agent").createWorld(30, 30)
sim.initializeBrain()

# Enable/disable neuromodulation features
sim.getAgent().enableFeature("reward_modulation", true)
sim.getAgent().enableFeature("curiosity", true)
sim.getAgent().enableFeature("novelty", true)

# Run simulation with neuromodulation
result = sim.runAgentSimulation(1000, enable_learning=True)

print(f"Neuromodulation levels:")
print(f"  Curiosity: {sim.getAgent().getCuriosityLevel():.3f}")
print(f"  Novelty: {sim.getAgent().getNoveltyLevel():.3f}")
```

## Error Handling

### Custom Exceptions

```python
import pynlm

try:
    # Create a configuration that might fail
    config = pynlm.createConfigBuilder()
    config.withNeuronCount(-1)  # Invalid neuron count
    brain = pynlm.createBrain(config.build())
    brain.initialize()
    
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.MemoryError as e:
    print(f"Memory error: {e}")
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
```

### Graceful Error Recovery

```python
import pynlm

def safe_simulation_run(config_preset="minimal"):
    try:
        brain = pynlm.createBrain(getattr(pynlm.ConfigPresets, config_preset)())
        brain.initialize()
        result = brain.runSteps(100)
        return result
        
    except Exception as e:
        print(f"Simulation failed: {e}")
        print("Attempting recovery...")
        
        try:
            # Try with minimal configuration
            brain = pynlm.createBrain(pynlm.ConfigPresets.minimal())
            brain.initialize()
            result = brain.runSteps(50)  # Reduced steps
            return result
            
        except Exception as e2:
            print(f"Recovery failed: {e2}")
            raise

# Run safe simulation
result = safe_simulation_run("research")
```

## Performance Tips

### Optimizing Simulations

1. **Use appropriate presets**: Choose presets based on your needs (research, agent, minimal)

2. **Configure memory usage**: Use `withMemoryCapacity()` to control memory usage

3. **Enable only necessary features**: Disable unused features to reduce overhead

4. **Batch processing**: Run multiple simulation steps in batch for better performance

5. **Monitor performance**: Use performance monitoring to identify bottlenecks

### Scaling Recommendations

- **Small simulations** (≤ 100 neurons): Use `ConfigPresets.minimal()`
- **Research simulations** (100-2000 neurons): Use `ConfigPresets.research()`
- **Agent simulations** (1000-5000 neurons): Use `ConfigPresets.agent()`
- **Large-scale simulations** (≥ 5000 neurons): Custom configuration with custom builder

## Troubleshooting

### Common Issues and Solutions

1. **Brain initialization fails**
   ```python
   # Solution: Check configuration validity
   config = pynlm.createConfigBuilder().withNeuronCount(100).build()
   brain = pynlm.createBrain(config)
   if not brain.initialize():
       print("Brain initialization failed - check configuration")
   ```

2. **Memory usage is too high**
   ```python
   # Solution: Reduce memory requirements
   config = pynlm.createConfigBuilder()
   config.withMemoryCapacity(100, 200)  # Reduce memory capacity
   config.withNeuronCount(500)  # Reduce neuron count
   ```

3. **Simulation is too slow**
   ```python
   # Solution: Reduce timestep and increase efficiency
   config = pynlm.createConfigBuilder().withTimestep(0.01)  # Larger timestep
   config.withConnectionProbability(0.05)  # Reduce connections
   ```

4. **Plasity is not working**
   ```python
   # Solution: Enable plasticity features
   brain = pynlm.createBrain(pynlm.ConfigPresets.research())
   brain.initialize()
   # Check if plasticity is enabled in configuration
   ```

## Further Reading

- [NLM Documentation](https://nlm.readthedocs.io)
- [C++ API Reference](https://github.com/nlm-project/nlm/blob/main/docs/API.md)
- [Research Papers](https://github.com/nlm-project/nlm/blob/main/docs/RESEARCH.md)
- [Phase 6 Integration](https://github.com/nlm-project/nlm/blob/main/docs/PHASE6_FINAL_AUDIT.md)

## License

This documentation is licensed under the MIT License.
