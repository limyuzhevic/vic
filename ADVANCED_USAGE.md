# Advanced NLM Usage Guide

Advanced NLM provides sophisticated tools for creating and running complex neural simulations with advanced learning protocols, performance optimization, and comprehensive monitoring.

## Overview

This guide covers advanced usage patterns, performance optimization, debugging, and integration techniques for the NLM framework.

## Advanced Configuration Presets

NLM now includes several predefined configuration presets to help you get started with different use cases:

### Quick Test Preset

```python
import pynlm

# Create a quick test configuration
config = pynlm.createDefaultConfig()
# The createDefaultConfig() automatically uses the quick test preset
brain = pynlm.createBrain(config)
brain.initialize()
```

### Performance Preset

```python
import pynlm

# Performance-optimized configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("simulation_timestep", 0.01)
config.set("working_memory_capacity", 50)
config.set("episodic_memory_limit", 500)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Research Preset

```python
import pynlm

# Research-grade configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)
config.set("region_count", 2)
config.set("connection_probability", 0.05)
config.set("enable_curiosity", True)
config.set("enable_novelty", True)
config.set("enable_prediction", True)
config.set("enable_planning", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Visualization Preset

```python
import pynlm

# Visualization-optimized configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 200)
config.set("enable_checkpointing", True)
config.set("enable_replay", True)
config.set("checkpoint_path", "./checkpoints")
config.set("replay_interval", 100)

brain = pynlm.createBrain(config)
brain.initialize()
```

## Advanced Learning Protocols

### Policy Gradient Learning

The advanced experiment framework supports policy gradient learning for more sophisticated decision-making:

```python
import pynlm

# Create advanced configuration for policy gradient
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1500)
config.set("enable_advanced_learning", True)
config.set("use_policy_gradient", True)
config.set("learning_rate", 0.01)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with policy gradient
for step in range(2000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Reinforcement Learning

Integrated Q-learning and SARSA algorithms for optimal decision-making:

```python
# Enable reinforcement learning
agent.enableRewardModulation(True)
# The agent automatically uses reinforcement learning
```

### Curriculum Learning

Progressive learning through increasing task complexity:

```python
# Curriculum learning is automatically enabled in advanced experiments
# The brain progresses through learning phases:
# Phase 1: Simple navigation
# Phase 2: Medium complexity tasks
# Phase 3: Complex challenges
# Phase 4: Expert-level performance
```

## Performance Optimization

### Memory Management

Monitor and optimize memory usage during long simulations:

```python
# Check memory system efficiency
if brain.getWorkingMemory():
    efficiency = brain.getWorkingMemory().getEfficiency()
    print(f"Working memory efficiency: {efficiency:.3f}")

if brain.getEpisodicMemory():
    episode_count = brain.getEpisodicMemory().getEpisodeCount()
    capacity = brain.getEpisodicMemory().getMaxEpisodes()
    print(f"Episodic memory: {episode_count}/{capacity} episodes")
```

### Simulation Speed Monitoring

Track simulation performance and optimize settings:

```python
# Monitor simulation speed
import time
start_time = time.time()

# Run simulation
for step in range(1000):
    brain.step(step)

end_time = time.time()
elapsed = end_time - start_time
speed = 1000 / elapsed
print(f"Simulation speed: {speed:.1f} steps/second")
```

### System Stability Checks

Ensure your simulation remains stable during long runs:

```python
# Perform system stability checks
if not brain.getAverageFiringRate() < 0 or > 100:
    print("Warning: Firing rate outside normal range")

if brain.getTotalNeuronCount() == 0:
    print("Error: No neurons in brain")

# Check memory overflow
if brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces() > brain.getWorkingMemory().getCapacity() * 2:
    print("Warning: Working memory may be overloaded")
```

## Advanced Experiment Examples

### Complete Advanced Experiment

Here's a complete example of running an advanced experiment with all learning protocols:

```python
import pynlm

def run_advanced_experiment():
    # Create advanced configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1500)
    config.set("enable_advanced_learning", True)
    config.set("use_policy_gradient", True)
    config.set("use_reinforcement_learning", True)
    config.set("use_curriculum_learning", True)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run experiment
    for step in range(2000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply advanced learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    return brain

# Run the experiment
brain = run_advanced_experiment()
print(f"Advanced experiment completed with {brain.getTotalSpikeCount()} total spikes")
```

### Learning Metrics and Analytics

Track and analyze learning progress:

```python
# Collect learning metrics
total_reward = 0
learning_episodes = 0
curiosity_peaks = []

for step in range(2000):
    # ... simulation code ...
    
    # Track metrics
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    total_reward += reward
    
    curiosity = agent.getCuriosityLevel()
    if curiosity > 0.7:
        curiosity_peaks.append((step, curiosity))
    
    if step % 500 == 0:
        avg_reward = total_reward / (step + 1)
        print(f"Step {step}: Avg reward={avg_reward:.4f}, "
              f"Curiosity={curiosity:.3f}, "
              f"Firing={brain.getFiringNeuronCount()}")

# Analyze results
avg_reward = total_reward / 2000
peak_curiosity_count = len(curiosity_peaks)
print(f"Learning results: Avg reward={avg_reward:.4f}, "
      f"High curiosity episodes={peak_curiosity_count}")
```

## Configuration Validation and Debugging

### Validate Configuration

Ensure your configuration is valid before running experiments:

```python
def validate_configuration(config):
    """Validate configuration parameters."""
    errors = []
    
    # Check required parameters
    if not config.has("neuron_count"):
        errors.append("Missing neuron_count")
    elif not pynlm.Config.validateNeuronCount(config.get("neuron_count")):
        errors.append("Invalid neuron_count")
    
    if not config.has("region_count"):
        errors.append("Missing region_count")
    
    if not config.has("connection_probability"):
        errors.append("Missing connection_probability")
    elif not pynlm.Config.validateSynapseDensity(config.get("connection_probability")):
        errors.append("Invalid connection_probability")
    
    if not config.has("simulation_timestep"):
        errors.append("Missing simulation_timestep")
    
    return errors

# Usage
errors = validate_configuration(config)
if errors:
    print("Configuration errors:", errors)
else:
    print("Configuration is valid")
```

### Debug Mode

Enable debug logging and detailed output:

```python
# Enable debug logging
config.set("log_level", "debug")
config.set("enable_detailed_logging", True)

# Set up monitoring
config.set("enable_performance_monitoring", True)
config.set("log_interval", 100)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with debug output
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        # Debug output
        print(f"Step {step}: "
              f"Firing={brain.getFiringNeuronCount()}, "
              f"Memory={brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0}")
```

## Integration with External Systems

### Save/Load Advanced Experiments

Save and load experiment configurations and results:

```python
# Save experiment state
brain.save("advanced_experiment_checkpoint.bin")

# Load experiment state
brain2 = pynlm.createBrain(config)
brain2.initialize()
brain2.load("advanced_experiment_checkpoint.bin")
```

### Multi-Experiment Analysis

Run multiple experiments with different configurations and compare results:

```python
def run_multiple_experiments(configs):
    """Run multiple experiments with different configurations."""
    results = []
    
    for i, config in enumerate(configs):
        print(f"Running experiment {i + 1}/{len(configs)}")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run experiment
        # ... simulation code ...
        
        # Collect results
        result = {
            "experiment_id": i,
            "total_spikes": brain.getTotalSpikeCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "final_reward": total_reward,
            "learning_progress": learning_curve
        }
        
        results.append(result)
        
        # Save experiment
        brain.save(f"experiment_{i}.bin")
    
    return results

# Compare experiment results
configs = [
    {"neuron_count": 500, "learning_rate": 0.01},
    {"neuron_count": 1000, "learning_rate": 0.005},
    {"neuron_count": 2000, "learning_rate": 0.001},
]

results = run_multiple_experiments(configs)
for result in results:
    print(f"Experiment {result['experiment_id']}: "
          f"Spikes={result['total_spikes']}, "
          f"Rate={result['avg_firing_rate']:.3f}, "
          f"Reward={result['final_reward']:.4f}")
```

## Troubleshooting

### Common Issues and Solutions

**Issue: Brain not initializing**
```python
# Solution:
if not brain.initialize():
    print("Failed to initialize brain")
    # Check configuration
    print("Configuration:", config.summary())
```

**Issue: Memory usage growing too large**
```python
# Solution: Monitor memory usage
if brain.getWorkingMemory():
    active = brain.getWorkingMemory().getActiveTraces()
    capacity = brain.getWorkingMemory().getCapacity()
    if active > capacity * 0.8:
        print("Warning: Working memory approaching capacity")
        # Clear unused traces
        brain.getWorkingMemory().clear()
```

**Issue: Simulation too slow**
```python
# Solution: Optimize configuration
config.set("simulation_timestep", 0.01)  # Larger timestep
config.set("neuron_count", 500)  # Fewer neurons
config.set("region_count", 1)  # Fewer regions
```

### Debug Mode Commands

```python
# Enable comprehensive debugging
config.set("log_level", "debug")
config.set("enable_detailed_logging", True)
config.set("enable_performance_monitoring", True)
config.set("enable_system_checks", True)

# Run with debug output
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        # Debug information
        print(f"DEBUG Step {step}:")
        print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  Memory traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0}")
        print(f"  Dopamine: {agent.getNeuromodulationLevel():.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
```

## Performance Tips

### Simulation Optimization

1. **Use appropriate neuron counts**: Start with 100-500 neurons for testing, increase for production
2. **Optimize timestep**: 0.001-0.01 is typical for biological simulations
3. **Balance complexity**: More regions and neurons increase realism but reduce speed
4. **Use checkpoints**: Save progress periodically to avoid losing work

### Memory Optimization

1. **Monitor memory usage**: Check working memory efficiency regularly
2. **Clear unused memory**: Use `brain.getWorkingMemory().clear()` when needed
3. **Adjust memory limits**: Set appropriate capacities based on your use case
4. **Use efficient data structures**: The framework includes optimized memory pools

### Learning Efficiency

1. **Tune learning rates**: Start with 0.01 and adjust based on results
2. **Enable curiosity**: Let the agent explore naturally
3. **Use curriculum learning**: Progressive complexity improves learning
4. **Monitor convergence**: Track metrics to detect when learning plateaus

## Best Practices

### Configuration Management

- Always validate configuration before running experiments
- Use presets as starting points and customize as needed
- Document your experimental setup for reproducibility
- Keep track of random seeds for reproducible results

### Experiment Management

- Save checkpoints regularly during long experiments
- Use different configuration names for different experiments
- Document hyperparameter settings for each experiment
- Maintain a results database for analysis

### Performance Monitoring

- Monitor simulation speed throughout experiments
- Set up alerts for unusual performance metrics
- Regularily check memory usage and system stability
- Profile your code to identify bottlenecks

## Further Reading

For more advanced topics, refer to:

- **ARCHITECTURE.md**: Deep dive into NLM system architecture
- **SCIENCE.md**: Scientific background and theory
- **EXPERIMENTS.md**: Detailed experiment descriptions
- **Phase 6 integration documentation**: Understanding the final integrated system

## Getting Help

If you encounter issues or need help:

1. Check the comprehensive error messages in your code
2. Review the example scripts in the `examples/` directory
3. Consult the `easy_usage.md` for basic usage patterns
4. Use the advanced example in `examples/advanced_experiment.py` for complex scenarios

The advanced features in NLM provide a powerful framework for creating sophisticated neural simulations with comprehensive learning capabilities. Use these tools to build more realistic and intelligent artificial systems.