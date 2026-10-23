# NLM Advanced User Guide
# Expert-Level Documentation for NLM Version 0.1.0-Advanced

## Overview

This guide provides comprehensive documentation for advanced users of the Neural Learning Machine (NLM). It covers expert-level usage, configuration, debugging, performance optimization, and advanced features that go beyond the basic user guide.

**Prerequisites**: This guide assumes familiarity with the basic NLM concepts and Python programming.

## Table of Contents

1. [Advanced Configuration](#advanced-configuration)
2. [Expert-Level Brain Control](#expert-level-brain-control)
3. [Performance Optimization](#performance-optimization)
4. [Advanced Testing and Validation](#advanced-testing-and-validation)
5. [Debugging and Troubleshooting](#debugging-and-troubleshooting)
6. [Integration and Deployment](#integration-and-deployment)
7. [Advanced Python API Reference](#advanced-python-api-reference)
8. [Configuration Reference](#configuration-reference)

## 1. Advanced Configuration

### 1.1 Configuration Validation

The advanced configuration system includes comprehensive validation:

```python
import pynlm

# Create configuration with validation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10000)  # Large-scale configuration
config.set("brain.synapse_density", 0.05)
config.set("random_seed", 42)
config.set("simulation_timestep", 0.001)

# Validate configuration (raises ConfigurationError if invalid)
try:
    is_valid = config.validate()
    print("Configuration is valid")
except pynlm.ConfigurationError as e:
    print(f"Invalid configuration: {e}")
```

### 1.2 Configuration Merging and Comparison

```python
# Merge configurations
base_config = pynlm.createDefaultConfig()
base_config.set("brain.neuron_count", 1000)

expert_config = pynlm.createDefaultConfig()
expert_config.set("brain.neuron_count", 10000)
expert_config.set("plasticity.stdp.enable", False)

# Merge expert configuration into base
base_config.merge(expert_config)

# Compare configurations
config_diff = base_config.diff(expert_config)
print("Differences:", config_diff)
```

### 1.3 Expert Configuration Templates

```python
# Large-scale configuration for research
research_config = pynlm.createLargeScaleConfig(neurons=100000)
research_config.set("brain.synapse_density", 0.01)

# Small-scale configuration for development
dev_config = pynlm.createSmallScaleConfig(neurons=100)

# Benchmark configuration
benchmark_config = pynlm.createBenchmarkConfig(neurons=10000)
```

## 2. Expert-Level Brain Control

### 2.1 Advanced Brain Statistics

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get comprehensive statistics
stats = brain.getStats()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"Firing rate: {stats['average_firing_rate']:.3f} Hz")
print(f"E/I ratio: {stats['excitation_inhibition_ratio']:.3f}")

# Get detailed region information
region_stats = brain.getRegionStats()
for region_id, region_data in region_stats.items():
    print(f"Region {region_id}: {region_data['neuron_count']} neurons, "
          f"{region_data['population_count']} populations")
```

### 2.2 Advanced Simulation Control

```python
# Run long-duration simulation with progress reporting
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print("Running long simulation...")
total_spikes = brain.runSimulation(steps=100000, progressInterval=10000)
print(f"Simulation complete. Total spikes: {total_spikes}")

# Run simulation with custom time control
brain.reset()
for step in range(0, 1000, 10):
    time = step * 0.001  # Custom time scaling
    brain.step(step, time)
```

### 2.3 Advanced Neuromodulation

```python
# Apply complex neuromodulation patterns
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create dopamine signal with custom profile
from pynlm import Dopamine
dopamine = Dopamine()
dopamine.setLevel(0.5)  # Moderate dopamine
brain.applyNeuromodulation(dopamine)

# Apply reward prediction error
brain.applyRewardModulation(reward=1.0, predicted_reward=0.8)
```

### 2.4 Advanced Memory System Access

```python
# Access and manipulate memory systems
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Working memory operations
working_memory = brain.getWorkingMemory()
print(f"Active working memory traces: {working_memory.getActiveTraces()}")

# Episodic memory operations
episodic_memory = brain.getEpisodicMemory()
print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")

# Store a custom experience
experience = nlm.EpisodicMemoryItem()
experience.timestamp = brain.getTotalSpikeCount()
experience.reward = 0.7
# Add neural activity pattern...
episodic_memory.storeEpisode(experience)
```

### 2.5 Advanced Plasticity Control

```python
# Get detailed plasticity statistics
plasticity_stats = brain.getPlasticityStats()
print(f"STDP LTP weight: {plasticity_stats['stdp']['ltp_weight']}")
print(f"STDP LTD weight: {plasticity_stats['stdp']['ltd_weight']}")
print(f"STDP time constant: {plasticity_stats['stdp']['time_constant']}")

# Modify plasticity parameters dynamically
brain.getSTDP().setLTPWeight(0.02)  # Increase LTP weight
brain.getSTDP().setLTDWeight(0.01)  # Decrease LTD weight
brain.getSTDP().setTimeConstant(25.0)  # Change time constant
```

## 3. Performance Optimization

### 3.1 Performance Benchmarking

```python
# Built-in performance benchmarking
import pynlm

# Run performance benchmark
results = pynlm.benchmarkBrain(
    brain=None,  # Auto-create
    neurons=10000,
    steps=1000
)

print(f"Performance Results:")
print(f"  Neurons/second: {results['neurons_per_second']:,.0f}")
print(f"  Total steps: {results['total_steps']}")
print(f"  Wall time (ms): {results['wall_time_ms']}")
print(f"  Spikes per step: {results['spikes_per_step']:.2f}")
```

### 3.2 Configuration for Different Scales

```python
# Micro-scale (testing and development)
micro_config = pynlm.createSmallScaleConfig(neurons=1000)

# Small-scale (research experiments)
small_config = pynlm.createSmallScaleConfig(neurons=10000)

# Medium-scale (production testing)
medium_config = pynlm.createBenchmarkConfig(neurons=50000)

# Large-scale (research systems)
large_config = pynlm.createLargeScaleConfig(neurons=500000)

# Extra-large (HPC systems)
xl_config = pynlm.createLargeScaleConfig(neurons=5000000)
```

### 3.3 Performance Monitoring

```python
# Monitor brain performance during simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(1000):
    brain.step(step)
    
    # Periodic performance monitoring
    if step % 100 == 0:
        analysis = pynlm.analyzeBrainData(brain)
        print(f"Step {step}: {analysis['firing_rate']:.3f} Hz, "
              f"density: {analysis['connection_density']:.6f}")
```

## 4. Advanced Testing and Validation

### 4.1 Integration Testing

```python
# Test complete brain system integration
def test_phase6_integration():
    # Create brain with all subsystems
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test memory system integration
    assert brain.getWorkingMemory() is not None
    assert brain.getEpisodicMemory() is not None
    assert brain.getAssociativeMemory() is not None
    
    # Test neuromodulation integration
    assert brain.getDopamine() is not None
    assert brain.getCuriosity() is not None
    assert brain.getNovelty() is not None
    
    # Test cognition system integration
    assert brain.getPlanner() is not None
    assert brain.getConceptFormation() is not None
    assert brain.getAttention() is not None
    
    # Test prediction system integration
    assert brain.getPredictionSystem() is not None
    
    # Test simulation
    for step in range(100):
        brain.step(step)
        action = brain.produceAction()
        assert action is not None
    
    print("Phase 6 integration test passed")

test_phase6_integration()
```

### 4.2 Advanced Error Handling

```python
import pynlm

# Test error handling with custom exceptions
def test_advanced_error_handling():
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 0)  # Invalid configuration
    
    try:
        config.validate()
        print("ERROR: Should have raised ConfigurationError")
    except pynlm.ConfigurationError as e:
        print(f"Caught expected ConfigurationError: {e}")
    
    # Test simulation error handling
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # This should work despite invalid configuration
    for step in range(10):
        brain.step(step)
    
    print("Error handling test passed")

test_advanced_error_handling()
```

## 5. Debugging and Troubleshooting

### 5.1 Debugging Tools

```python
# Use advanced debugging tools
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable logging
import logging
logging.basicConfig(level=logging.DEBUG)

# Create snapshot for debugging
debug_snapshot = brain.createSnapshot("before_error")
print(f"Created debug snapshot: {debug_snapshot}")

# Inspect brain state
brain.logStatus()  # Enhanced logging
stats = brain.getStats()
print(f"Current state: {stats}")
```

### 5.2 Common Issues and Solutions

#### Issue: High Memory Usage

```python
# Solution: Use more memory-efficient configuration
config = pynlm.createSmallScaleConfig(neurons=1000)
config.set("brain.synapse_density", 0.01)  # Lower connectivity
config.set("brain.region_count", 1)  # Single region for efficiency
```

#### Issue: Slow Simulation

```python
# Solution: Optimize configuration
config = pynlm.createLargeScaleConfig(neurons=10000)
config.set("simulation_timestep", 0.001)  # Smaller timestep for stability
config.set("random_seed", 42)  # Reproducible results
```

#### Issue: Configuration Validation Errors

```python
# Solution: Use appropriate validation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

if not config.validate():
    print("Fix configuration errors")
    # Check config.getKeys() for missing required keys
```

## 6. Integration and Deployment

### 6.1 Integration with External Systems

```python
# Integrate with world simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run complete agent simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 6.2 Deployment in Production

```python
# Production deployment with monitoring
import pynlm
import time
import psutil

# Create production configuration
config = pynlm.createLargeScaleConfig(neurons=50000)
config.set("checkpoint_frequency", 10000)

brain = pynlm.createBrain(config)
brain.initialize()

# Monitor system resources
start_time = time.time()
start_memory = psutil.virtual_memory().used

# Run production simulation
for step in range(50000):
    brain.step(step)
    
    # Periodic checkpoint
    if step % 10000 == 0:
        brain.save(f"checkpoint_{step}.bin")

end_time = time.time()
end_memory = psutil.virtual_memory().used

# Report performance
print(f"Production simulation completed in {end_time - start_time:.2f} seconds")
print(f"Memory usage: {(end_memory - start_memory) / (1024*1024):.2f} MB")
print(f"Performance: {brain.getTotalSpikeCount() / (end_time - start_time):,.0f} spikes/second")
```

## 7. Advanced Python API Reference

### 7.1 AdvancedBrain Class

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `getStats()` | Get comprehensive statistics | None | dict |
| `getRegionStats()` | Get detailed region information | None | dict |
| `runSimulation()` | Run extended simulation | steps, progressInterval | int |
| `createSnapshot()` | Create state snapshot | label | str |

### 7.2 AdvancedConfig Class

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `validate()` | Validate configuration | None | bool |
| `merge()` | Merge another configuration | other | Config |
| `diff()` | Compare with another config | other | List[str] |

### 7.3 Exception Classes

| Exception | Description |
|-----------|-------------|
| `NLMException` | Base exception for all NLM errors |
| `ConfigurationError` | Configuration-related errors |
| `SimulationError` | Simulation-related errors |
| `MemoryError` | Memory-related errors |

## 8. Configuration Reference

### 8.1 Core Configuration Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential |
| `brain.tau_mem` | float | 20.0 | Membrane time constant |
| `brain.tau_ref` | float | 2.0 | Refractory period |

### 8.2 Performance Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `simulation_timestep` | float | 0.001 | Time step duration |
| `replay_interval` | int | 100 | Memory replay interval |
| `consolidation_interval` | int | 1000 | Memory consolidation interval |
| `checkpoint_frequency` | int | 10000 | Checkpoint saving frequency |

### 8.3 Plasticity Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

## Quick Reference

### Basic Usage

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
stats = brain.getStats()
print(f"Neurons: {stats['total_neurons']}, "
      f"Spikes: {stats['total_spikes']}")
```

### Advanced Usage

```python
# Advanced configuration with validation
config = pynlm.createLargeScaleConfig(neurons=10000)
if not config.validate():
    raise ValueError("Invalid configuration")

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run extended simulation with monitoring
results = brain.runSimulation(steps=10000, progressInterval=1000)
print(f"Completed with {results} spikes")

# Get detailed statistics
analysis = pynlm.analyzeBrainData(brain)
print(f"Performance: {analysis['firing_rate']:.3f} Hz")
```

### Error Handling

```python
import pynlm

try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(0)  # Do work
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.MemoryError as e:
    print(f"Memory error: {e}")
```

## Conclusion

This advanced user guide provides comprehensive coverage of NLM's expert-level features. Users should start with the basic guides and progress through these concepts as they become comfortable with the system.

Key takeaways:
1. Use appropriate configuration templates for different scales
2. Leverage advanced error handling and validation
3. Monitor performance during simulation
4. Use comprehensive statistics for analysis
5. Integrate with external systems for complete applications

For the latest documentation and examples, visit the NLM documentation website.
