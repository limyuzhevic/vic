# NLM Advanced Python API Guide

This guide provides comprehensive documentation for using the NLM (Neural Learning Machine) Python API, focusing on advanced features and usage patterns introduced in the recent improvements.

## Table of Contents

1. [Advanced Configuration Management](#advanced-configuration-management)
2. [Performance Monitoring & Debugging](#performance-monitoring--debugging)
3. [Checkpoint & Restart Capabilities](#checkpoint--restart-capabilities)
4. [AgentBrain Advanced Usage](#agentbrain-advanced-usage)
5. [Neuron & Synapse Management](#neuron--synapse-management)
6. [Best Practices & Tips](#best-practices--tips)
7. [Example Applications](#example-applications)

---

## Advanced Configuration Management

### Batch Operations

The improved Python API provides powerful batch configuration operations:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Batch set multiple values using dictionary
config.setBatch({
    "neuron_count": 2000,
    "region_count": 3,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "simulation_timestep": 0.0005,
    "replay_interval": 200,
    "consolidation_interval": 2000
})

# Validate required keys
required_keys = ["neuron_count", "region_count", "connection_probability"]
validation_result = config.validateKeys(required_keys)
if not validation_result["success"]:
    print(f"Missing keys: {validation_result['missing_keys']}")

# Get all configuration values
all_config = config.getAll()
print(f"Configuration summary: {config.summary()}")
```

### Configuration Source Management

```python
# Set values with specific sources (for tracking configuration origin)
config.setSource({
    "neuron_count": 1000,
    "region_count": 1
}, pynlm.ConfigSource::CommandLine)

config.setSource({
    "stdp_ltp_weight": 0.02
}, pynlm.ConfigSource::File)

# Compare with defaults
config.setBatch({"neuron_count": 2000})  # Override from default
compared = config.compareWithDefaults(pynlm.createDefaultConfig())
print(f"Overridden values: {compared}")
```

## Performance Monitoring & Debugging

### Brain Performance Benchmarking

```python
import pynlm
import time

# Create and configure brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable performance monitoring
brain.enablePerformanceMonitoring(True)
brain.enableDebugLogging(True, "info")

# Benchmark brain performance over different step counts
print("=== Performance Benchmarks ===")

# Quick benchmark
results = brain.benchmarkStep(1000, reset=True)
print(f"1000 steps: {results['duration_us']/1000:.2f} ms avg, "
      f"{results['spikes_per_step']:.2f} spikes/step")

# Medium benchmark
results = brain.benchmarkStep(10000, reset=True)
print(f"10000 steps: {results['duration_us']/1000000:.2f} seconds avg, "
      f"{results['firing_rate_hz']:.2f} Hz firing rate")

# Stress test
results = brain.benchmarkStep(50000, reset=True)
print(f"50000 steps: {results['avg_time_per_step_us']:.2f} μs/step, "
      f"{results['neurons_firing']} neurons firing")
```

### Performance Profile Analysis

```python
# Create detailed performance profile
profile = brain.createPerformanceProfile(10000, 100)
print(f"\n=== Performance Profile ===")
print(f"Average spikes per sample: {profile['avg_spikes_per_sample']:.2f}")
print(f"Firing rate variance: {profile['firing_rate_variance']:.4f}")
print(f"Spike rate variance: {profile['spike_rate_variance']:.4f}")

# Check for performance anomalies
if profile['firing_rate_variance'] > 100.0:
    print("WARNING: High firing rate variance detected")
if profile['spike_rate_variance'] > 50.0:
    print("WARNING: High spike rate variance detected")
```

### System Validation

```python
# Validate brain state consistency
validation = brain.validateBrainState()
if not validation["valid"]:
    print(f"VALIDATION FAILED: {validation['errors']}")

for warning in validation["warnings"]:
    print(f"WARNING: {warning}")
```

### Diagnostic Export

```python
# Export detailed diagnostics for analysis
brain.exportDiagnostics("brain_diagnostics.txt")
print("Diagnostics exported to brain_diagnostics.txt")

# Load and analyze diagnostics
with open("brain_diagnostics.txt", "r") as f:
    diagnostics = f.read()
    if "Basic Statistics:" in diagnostics:
        print("Diagnostics file contains complete information")
```

## Checkpoint & Restart Capabilities

### Basic Checkpoint Operations

```python
import pynlm
import time

# Create brain and run some simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run for a while
for step in range(100):
    brain.step(step)

# Create checkpoint with label
result = brain.createCheckpoint("pre_learning")
if result["success"]:
    print(f"Checkpoint created: {result['filepath']} (label: {result['label']})")
else:
    print("Failed to create checkpoint")

# Create another checkpoint
result = brain.createCheckpoint("post_learning")
if result["success"]:
    print(f"Checkpoint created: {result['filepath']}")
```

### Checkpoint Management

```python
# List available checkpoints
checkpoints = brain.listCheckpoints("./saves")
print(f"Available checkpoints: {len(checkpoints)}")
for cp in checkpoints:
    print(f"  {cp}")

# Get checkpoint information
info = brain.checkpointInfo("./saves/checkpoint_pre_learning_123456789.bin")
if info["readable"]:
    print(f"Checkpoint info:")
    print(f"  Size: {info['file_size_bytes']} bytes")
    print(f"  Neurons: {info['brain_size']}")
    print(f"  Synapses: {info['synapses']}")
    print(f"  Regions: {info['regions']}")
```

### Checkpoint Comparison & Analysis

```python
# Compare two checkpoints
comparison = brain.compareCheckpoints(
    "./saves/checkpoint_pre_learning_123456789.bin",
    "./saves/checkpoint_post_learning_987654321.bin"
)

if comparison["comparable"]:
    print(f"Checkpoint comparison:")
    print(f"  File 1 size: {comparison['file1_size']} bytes")
    print(f"  File 2 size: {comparison['file2_size']} bytes")
    print(f"  Size difference: {comparison['size_difference']} bytes")
    print(f"  Recommendation: {comparison['recommendation']}")
else:
    print(f"Comparison failed: {comparison.get('error', 'Unknown error')}")
```

### Automated Checkpoint Management

```python
# Cleanup old checkpoints (keep only 5 most recent)
result = brain.cleanupOldCheckpoints("./saves", maxFiles=5)
print(f"Cleanup result: {result['message']}")
print(f"Deleted: {result['deleted']}, Remaining: {result['remaining']}")

# Create automatic checkpoints at intervals
for step in range(5000):
    brain.step(step)
    
    # Create checkpoint every 1000 steps
    if step > 0 and step % 1000 == 0:
        result = brain.autoCheckpoint("./auto_saves", interval=1000)
        if result["success"]:
            print(f"Auto-checkpoint at step {step}: {result['filepath']}")
```

## AgentBrain Advanced Usage

### Enhanced Agent Configuration

```python
import pynlm

# Create brain with specific configuration
config = pynlm.createDefaultConfig()
config.setBatch({
    "neuron_count": 1500,
    "region_count": 2,
    "connection_probability": 0.12
})

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with the brain
agent = pynlm.createAgentBrain(brain)

# Configure agent features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Initialize with world
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
agent.initialize(world)
```

### Advanced Agent Control

```python
# Run simulation with detailed monitoring
for step in range(500):
    world.update(0.1)
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Decode action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation based on world state
    agent_body = world.getAgentBody()
    reward = calculate_reward(agent_body)
    predicted = agent.getPredictionError()
    agent.applyRewardModulation(reward, predicted)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Log performance every 50 steps
    if step % 50 == 0:
        stats = brain.getPerformanceStats()
        print(f"Step {step}: {stats['firing_neuron_count']} neurons "
              f"firing, {stats['average_firing_rate']:.2f} Hz rate")
```

### Development & Neuromodulation

```python
# Monitor development stages
print("=== Development Monitor ===")
for step in range(0, 3000, 500):
    stage = agent.getDevelopmentalStage()
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    dopamine = agent.getNeuromodulationLevel()
    
    print(f"Step {step}: Stage {stage.name}, "
          f"Curiosity: {curiosity:.3f}, Novelty: {novelty:.3f}, "
          f"Dopamine: {dopamine:.3f}")
```

## Neuron & Synapse Management

### Neuron Configuration

```python
# Create brain and access neurons for configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Note: Direct neuron access requires implementing custom methods
# This is a placeholder for advanced neuron manipulation
# In a real implementation, you might add:
# - Custom neuron creation methods
# - Synapse manipulation utilities
# - Population management functions

print("Advanced neuron management requires custom implementation")
```

## Best Practices & Tips

### 1. Configuration Management

- Use batch operations (`setBatch`) for multiple configuration changes
- Validate required keys before simulation
- Use specific sources (`ConfigSource::File`, `::CommandLine`) for tracking configuration origin
- Compare configurations to defaults to detect unexpected changes

### 2. Performance Optimization

- Use `benchmarkStep()` to measure and optimize performance
- Monitor firing rate variance to detect instabilities
- Implement automatic checkpoints for long-running simulations
- Use performance profiles to identify bottlenecks

### 3. Error Handling

- Always validate inputs before use
- Implement comprehensive error handling for file operations
- Use the validation system to catch configuration issues early
- Monitor system warnings and take corrective action

### 4. Memory Management

- Use `reset()` to clear state between simulations
- Monitor neuron and synapse counts for memory usage
- Implement proper checkpoint cleanup strategies
- Consider memory pools for long-running simulations

### 5. Debugging

- Enable debug logging for detailed information
- Use diagnostic export for offline analysis
- Implement custom validation for your specific use case
- Use performance profiling to identify issues

## Example Applications

### 1. Learning Simulation

```python
import pynlm
import time

# Create learning simulation environment
config = pynlm.createDefaultConfig()
config.setBatch({
    "neuron_count": 3000,
    "region_count": 2,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.015,
    "stdp_ltd_weight": 0.012
})

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(30, 30, 16, 16)
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run learning simulation
print("=== Learning Simulation ===")
start_time = time.time()

for step in range(10000):
    # Environment update
    world.update(0.05)
    
    # Agent perception and action
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learning feedback
    if step % 100 == 0:
        reward = 1.0 if world.getAgentBody().health > 0 else -1.0
        predicted = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted)
        agent.updateDevelopment(0.05)
        
        # Log progress
        if step % 1000 == 0:
            stats = brain.getPerformanceStats()
            stage = agent.getDevelopmentalStage()
            print(f"Step {step}: {stats['average_firing_rate']:.2f} Hz, "
                  f"Stage {stage.name}, Curiosity: {agent.getCuriosityLevel():.3f}")

end_time = time.time()
print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")

# Create final checkpoint
brain.createCheckpoint("learning_complete")
print("Final checkpoint created")
```

### 2. Parameter Experimentation

```python
import pynlm

# Experiment with different configurations
experiment_configs = [
    {"name": "Conservative", "neuron_count": 500, "connection_probability": 0.05},
    {"name": "Balanced", "neuron_count": 1000, "connection_probability": 0.1},
    {"name": "Exploratory", "neuron_count": 2000, "connection_probability": 0.15},
    {"name": "Aggressive", "neuron_count": 3000, "connection_probability": 0.2}
]

results = []

for exp_config in experiment_configs:
    print(f"\n=== Running experiment: {exp_config['name']} ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.setBatch({
        "neuron_count": exp_config["neuron_count"],
        "region_count": 2,
        "connection_probability": exp_config["connection_probability"]
    })
    
    # Run experiment
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Simple simulation
    benchmark = brain.benchmarkStep(1000, reset=True)
    
    # Record results
    results.append({
        "name": exp_config["name"],
        "neurons": exp_config["neuron_count"],
        "connection_prob": exp_config["connection_probability"],
        "duration_ms": benchmark["duration_us"] / 1000,
        "firing_rate": benchmark["firing_rate_hz"],
        "spikes_per_step": benchmark["spikes_per_step"]
    })
    
    print(f"Results: {results[-1]}")

# Analyze results
print("\n=== Experiment Analysis ===")
print("Configuration Performance:")
for result in results:
    print(f"  {result['name']}: {result['firing_rate']:.2f} Hz, "
          f"{result['duration_ms']:.2f} ms for 1000 steps")
```

### 3. Performance Testing Suite

```python
import pynlm
import time

def performance_test_suite():
    """Comprehensive performance testing suite"""
    
    test_results = {}
    
    # Test different brain sizes
    sizes = [500, 1000, 2000, 5000]
    
    for size in sizes:
        print(f"\nTesting brain size: {size}")
        
        config = pynlm.createDefaultConfig()
        config.setBatch({
            "neuron_count": size,
            "region_count": 2,
            "connection_probability": 0.1
        })
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Performance benchmark
        start = time.time()
        benchmark = brain.benchmarkStep(1000, reset=True)
        end = time.time()
        
        test_results[size] = {
            "time_ms": (end - start) * 1000,
            "steps_per_second": 1000 / ((end - start)),
            "firing_rate_hz": benchmark["firing_rate_hz"],
            "spikes_per_step": benchmark["spikes_per_step"],
            "neurons_firing": benchmark["neurons_firing"]
        }
        
        print(f"  Firing rate: {test_results[size]['firing_rate_hz']:.2f} Hz")
        print(f"  Throughput: {test_results[size]['steps_per_second']:.0f} steps/sec")
    
    return test_results

# Run performance tests
print("=== NLM Performance Test Suite ===")
results = performance_test_suite()

print("\n=== Performance Summary ===")
print(f"{'Size':<8} {'Time(ms)':<10} {'Steps/sec':<12} {'Firing Rate':<15} {'Spikes/step':<12}")
print("-" * 65)
for size, data in results.items():
    print(f"{size:<8} {data['time_ms']:<10.2f} {data['steps_per_second']:<12.0f} "
          f"{data['firing_rate_hz']:<15.2f} {data['spikes_per_step']:<12.2f}")

performance_test_suite()
```

## Conclusion

The enhanced NLM Python API provides powerful tools for advanced neural network simulation and experimentation. By leveraging these new capabilities, you can:

- **Configure complex neural networks** with batch operations and validation
- **Monitor and optimize performance** with comprehensive benchmarking tools
- **Manage simulations effectively** with checkpoint/restart capabilities
- **Debug and troubleshoot** with detailed diagnostic tools
- **Experiment systematically** with parameter variation utilities
- **Deploy production simulations** with robust error handling

These improvements make NLM more accessible to beginners while providing advanced features for experienced users and researchers.

---

**Note**: This guide covers advanced features. For basic usage, refer to the existing `easy_usage.md` and `HOW_TO_USE.md` documentation files.
