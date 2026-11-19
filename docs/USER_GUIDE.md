# NLM User Guide

## Welcome to NLM (Neural Learning Machine)

NLM is an experimental artificial developmental brain project that simulates spiking neural computation with real LIF neuron dynamics, event-driven spike propagation, and multiple plasticity mechanisms.

## Quick Start

### What You'll Learn
- How to create and simulate virtual brains
- Basic brain-world-agent interactions
- Configuration management
- Advanced simulation features

### Prerequisites
- Python 3.8+
- Basic understanding of neural concepts

### Installation (if not already installed)

```bash
pip install pynlm
```

## Getting Started: Your First Brain

This guide will walk you through creating, initializing, and running a simple brain simulation.

### Step 1: Create a Brain

```python
import pynlm

# Method 1: Using default configuration (recommended for beginners)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Method 2: Using custom configuration
config = pynlm.createSimulationConfig(neuronCount=500, timestep=0.001, regions=2)
brain = pynlm.createBrain(config)
```

**What you're doing:**
- `createDefaultConfig()`: Creates a standard configuration for most use cases
- `createBrain()`: Instantiates a neural simulation brain with your configuration

### Step 2: Initialize the Brain

```python
brain.initialize()
print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
```

**What you're doing:**
- `initialize()`: Sets up the brain's internal systems and initializes neurons
- Always call `initialize()` before simulation

### Step 3: Run a Simulation

```python
import time

# Run a short simulation
steps = 1000
start_time = time.time()

for step in range(steps):
    brain.step(step)

end_time = time.time()
duration = end_time - start_time

print(f"Simulation completed in {duration:.2f} seconds")
print(f"Processed {steps} steps at {steps/duration:.0f} steps/second")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

**What you're doing:**
- `step(step)`: Performs one simulation step, allowing neurons to process and communicate
- Each step represents a small time interval in the brain's simulation time

## Complete Example: Agent in a World

Here's a complete example showing how to create a complete agent simulation.

```python
import pynlm

def run_agent_simulation(steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultBrainConfig()
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 7. Run simulation loop
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'total_spikes': brain.getTotalSpikeCount(),
        'final_firing_rate': brain.getAverageFiringRate(),
        'curiosity_level': agent.getCuriosityLevel()
    }

# Run the simulation
print("Starting agent simulation...")
result = run_agent_simulation(500)
print(f"\nSimulation completed!")
print(f"Total spikes: {result['total_spikes']}")
print(f"Final firing rate: {result['final_firing_rate']:.2f} Hz")
print(f"Final curiosity level: {result['curiosity_level']:.3f}")
```

## Advanced Features

### Configuration Management

NLM provides flexible configuration options for different use cases.

#### Preset Configurations

```python
# Create different sized brains for different needs
minimal_config = pynlm.createPresetConfig("minimal")  # Small, fast
standard_config = pynlm.createPresetConfig("standard")  # Balanced
complex_config = pynlm.createPresetConfig("complex")  # Large, detailed

# Create custom configuration
custom_config = pynlm.createSimulationConfig(
    neuronCount=2000,
    timestep=0.0005,
    regions=3
)
```

#### Configuration Validation

```python
# Validate your configuration
brain_config = pynlm.createDefaultBrainConfig()
is_valid, errors = pynlm.validateConfigForUseCase(brain_config, "brain_simulation")

if not is_valid:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
else:
    print("Configuration is valid!")
```

### Brain Analysis

```python
# Analyze brain state
brain_analysis = pynlm.analyzeBrainState(brain)
print("Brain Statistics:")
for key, value in brain_analysis.items():
    print(f"  {key}: {value}")

# Compute network metrics
metrics = pynlm.computeNetworkMetrics(brain)
print("\nNetwork Metrics:")
for metric, value in metrics.items():
    print(f"  {metric}: {value:.3f}")
```

### Performance Benchmarking

```python
# Benchmark your configuration
results = pynlm.benchmarkConfiguration(config, testSteps=2000)
print(f"Performance: {results['steps_per_second']:.0f} steps/second")
print(f"Average spikes per step: {results['spikes_per_step']:.3f}")
print(f"Neurons: {int(results['neurons'])}")
print(f"Synapses: {int(results['synapses'])}")
```

## Common Patterns and Best Practices

### Pattern 1: Standard Simulation Loop

```python
def run_simulation(brain, steps):
    """Standard simulation pattern used in most examples."""
    for step in range(steps):
        brain.step(step)
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    return brain.getTotalSpikeCount()
```

### Pattern 2: Monitoring and Logging

```python
def run_monitored_simulation(brain, steps, monitor_interval=50):
    """Run simulation with periodic monitoring."""
    import time
    
    start_time = time.time()
    spike_history = []
    firing_rate_history = []
    
    for step in range(steps):
        brain.step(step)
        
        if step % monitor_interval == 0:
            spike_history.append(brain.getTotalSpikeCount())
            firing_rate_history.append(brain.getAverageFiringRate())
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    duration = time.time() - start_time
    return {
        'spike_history': spike_history,
        'firing_rate_history': firing_rate_history,
        'duration': duration,
        'steps_per_second': steps / duration
    }
```

### Pattern 3: Safe Simulation Operations

```python
def safe_brain_operation(brain, steps):
    """Run brain simulation with error handling."""
    try:
        result = pynlm.runSimulation(brain, steps)
        print(f"Simulation completed successfully: {result} spikes")
        return result
    except nlm.RuntimeError as e:
        print(f"Simulation failed: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None
```

### Pattern 4: Configuration Management

```python
def get_experiment_config(experiment_type):
    """Get configuration for different experiment types."""
    if experiment_type == "exploration":
        return pynlm.createPresetConfig("standard")
    elif experiment_type == "learning":
        config = pynlm.createPresetConfig("complex")
        # Enable learning subsystems
        config.set("plasticity.stdp.enable", True)
        config.set("neuromodulation.curiosity.enable", True)
        return config
    elif experiment_type == "development":
        config = pynlm.createPresetConfig("complex")
        # Enable development
        config.set("development.enabled", True)
        return config
    else:
        return pynlm.createPresetConfig("standard")
```

## Advanced Usage

### Multiple Brains and Batch Processing

```python
# Run multiple simulations with different configurations
configs = [
    pynlm.createPresetConfig("minimal"),
    pynlm.createPresetConfig("standard"),
    pynlm.createPresetConfig("complex")
]

results = pynlm.batchSimulations(configs, stepsPerConfig=500)

for i, result in enumerate(results):
    print(f"Config {i+1}: {result['spike_count']} spikes, "
          f"{result['firing_rate']:.2f} Hz average rate")
```

### Configuration Comparison

```python
# Compare two configurations
base_config = pynlm.createPresetConfig("standard")
modified_config = pynlm.createPresetConfig("complex")
modified_config.set("brain.synapse_density", 0.2)  # Slightly higher density

differences = pynlm.diffConfigs(base_config, modified_config)
print(f"Found {len(differences)} differences:")
for diff in differences:
    print(f"  - {diff}")
```

### State Management

```python
# Save and load brain states
brain.save("my_brain_checkpoint.bin")
print("Brain state saved to checkpoint")

# Later, load the saved state
loaded_brain = pynlm.createBrain(config)
loaded_brain.load("my_brain_checkpoint.bin")
print("Brain state loaded from checkpoint")
```

## Troubleshooting

### Common Issues and Solutions

**Issue: Brain doesn't produce activity**

*Cause:* Brain not initialized or insufficient input

*Solution:*
```python
brain.initialize()  # Always call initialize first
brain.step(0)       # Run at least one step
# Add sensory input for activity
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0f)
```

**Issue: Configuration validation fails**

*Cause:* Missing required parameters or invalid values

*Solution:*
```python
config = pynlm.createDefaultConfig()
is_valid, errors = pynlm.validateConfigForUseCase(config, "brain_simulation")
if not is_valid:
    for error in errors:
        print(f"Fix: {error}")
```

**Issue: Memory usage is high**

*Cause:* Too many neurons or synapses

*Solution:*
```python
# Use smaller configuration
config = pynlm.createPresetConfig("minimal")
# or
config = pynlm.createSimulationConfig(neuronCount=100, timestep=0.01)
```

### Getting Help

1. **Documentation:** This guide and the project's documentation/
2. **Examples:** Look at the test files in tests/ for usage examples
3. **Community:** For advanced questions, refer to the project's documentation

## Performance Tips

1. **Use smaller configurations for testing:** `createPresetConfig("minimal")`
2. **Monitor your simulations:** Use the analysis functions regularly
3. **Enable only needed subsystems:** Turn off unused learning mechanisms
4. **Benchmark regularly:** Use `benchmarkConfiguration` to track performance

## Next Steps

After mastering the basics, consider:

1. **Experiment with different configurations:** Try various neuron counts and connectivity patterns
2. **Explore advanced features:** Learn about memory systems, prediction, and cognition
3. **Build complex agents:** Create agents with multiple sensory modalities
4. **Study learning dynamics:** Track how brains learn and change over time
5. **Contribute to the project:** Add new features or fix bugs

## Conclusion

You now have a solid foundation for working with NLM. The framework provides flexible configuration options, comprehensive testing tools, and advanced analysis features that make it easy to experiment with neural computation.

The NLM project continues to evolve with new features and improvements. Check the project documentation for the latest updates and advanced usage patterns.

---
*Version: 0.1.0*  
*Phase: 6 (Final Integration)*  
*Last Updated: 2026-10-19*