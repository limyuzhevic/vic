# NLM Python Bindings - Advanced Usage Guide

## Overview

The NLM (Neural Learning Machine) Python bindings provide a comprehensive interface to the C++ neural simulation framework. This guide covers advanced usage patterns, convenience functions, and powerful features for both beginners and experienced users.

## Quick Start

```python
import pynlm

# Create a brain with advanced configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run a simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: Reward={agent.getNeuromodulationLevel():.3f}, "
              f"Curiosity={agent.getCuriosityLevel():.3f}, "
              f"Development={brain.getDevelopmentalStage()}")
```

## Advanced Brain Configuration

### Reinforcement Learning Brain

```python
brain = pynlm.createRLBrain(2000)  # Optimized for reinforcement learning
```

### Sensory Processing Brain

```python
brain = pynlm.createSensoryBrain(visionNeurons=500, sensoryNeurons=200)
```

### Standard Brain

```python
brain = pynlm.createStandardBrain(
    neuron_count=1000,
    region_count=4,
    connection_probability=0.1
)
```

## Advanced World Configuration

### Exploration World

```python
world = pynlm.createExplorationWorld()
```

### Training World

```python
world = pynlm.createTrainingWorld()
```

### Standard World

```python
world = pynlm.createStandardWorld(
    width=20,
    height=20,
    vision_width=8,
    vision_height=8
)
```

## Agent Configuration

### Learning Agent

```python
agent = pynlm.createLearningAgent(brain)
```

### Standard Agent

```python
agent = pynlm.createStandardAgent(brain)
```

### Minimal Agent

```python
agent = pynlm.createMinimalAgent(brain)
```

## Advanced Analysis and Monitoring

### Brain Network Metrics

```python
metrics = brain.getNetworkMetrics()
print(f"Total neurons: {metrics.totalNeurons}")
print(f"Total synapses: {metrics.totalSynapses}")
print(f"Average firing rate: {metrics.avgFiringRate:.3f} Hz")
print(f"E/I ratio: {metrics.excitationInhibitionRatio:.3f}")
print(f"Total spikes: {metrics.totalSpikes}")
```

### Region Statistics

```python
region_stats = brain.getAllRegionStats()
for stats in region_stats:
    print(f"Region {stats['id']}: {stats['neuron_count']} neurons, "
          f"{stats['firing_rate']:.3f} Hz firing rate")
```

### Region Connectivity

```python
connectivity = brain.getRegionConnectivity(region_id)
print(f"Synapse count: {connectivity['synapse_count']}")
print(f"Neurons: {connectivity['neuron_count']}")
print(f"Synaptic density: {connectivity['synaptic_density']:.3f}")
print(f"Average weight: {connectivity['avg_synaptic_weight']:.3f}")
```

### Activity Analysis

```python
# Analyze activity patterns over time
pattern = brain.analyzeActivityPattern(steps=1000, timestep=0.001)

# Get spike raster data
raster = brain.getSpikeRaster(steps=500)
print(f"Total spikes in raster: {len(raster)}")

# Get activity heatmap for a region
heatmap = brain.getActivityHeatmap(region_id, steps=200)
```

## Training and Learning Utilities

### Training Episode

```python
def train_agent(brain, agent, world, num_episodes=10, steps_per_episode=100):
    for episode in range(num_episodes):
        results = agent.runTrainingEpisode(world, steps_per_episode)
        print(f"Episode {episode + 1}: Reward={results['total_reward']:.2f}, "
              f"Energy={results['final_energy']:.2f}")
        
        # Save best checkpoint
        if episode % 5 == 0:
            agent.saveWithTimestamp("./checkpoints", f"episode_{episode}")

train_agent(brain, agent, world)
```

### Task Optimization

```python
# Optimize brain for specific task
task_type = "exploration"
optimized_params = brain.optimizeForTask(task_type)
print(f"Optimized parameters for {task_type}: {optimized_params}")
```

### Performance Monitoring

```python
# Measure simulation performance
performance = brain.measurePerformance(steps=1000, timestep=0.001)
print(f"Simulation took {performance['total_time_seconds']:.2f} seconds")
print(f"Speed: {performance['steps_per_second']:.0f} steps/second")
print(f"Time per step: {performance['time_per_step_ms']:.2f} ms")
```

## Debugging and Validation

### Brain State Validation

```python
validation = brain.validateBrainState()
if validation['valid']:
    print("✓ Brain state is valid")
else:
    print("✗ Brain state has issues:")
    for error in validation['errors']:
        print(f"  - {error}")
    for warning in validation['warnings']:
        print(f"  - {warning}")
```

### Checkpoint Management

```python
# Save with automatic timestamp
brain_file, agent_file = agent.saveWithTimestamp("./checkpoints")
print(f"Saved: {brain_file}, {agent_file}")

# Load latest checkpoint
brain = agent.loadLatestCheckpoint("./checkpoints")
```

## Serialization and Persistence

### Save/Load States

```python
# Save brain state
brain.save("brain_checkpoint.bin")
brain.load("brain_checkpoint.bin")

# Save agent state
agent.save("agent_checkpoint.bin")
```

### Serialization with Timestamps

```python
# Save with timestamp
brain_file, agent_file = agent.saveWithTimestamp("./checkpoints")
# Files will be named like: brain_1234567890.bin, agent_1234567890.bin
```

## Utility Functions

### Random Seed Management

```python
world.setRandomSeed(42)
# Ensures reproducible simulations
```

### Configuration Management

```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.saveToFile("custom_config.json")
```

## Error Handling

### Exception Handling

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    # ... use the brain
except std::runtime_error as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Tips

1. **Reuse Brains**: Create brains once and reset them for multiple simulations
2. **Batch Operations**: Process multiple sensory inputs in batches when possible
3. **Memory Management**: Explicitly call reset() when done with simulations
4. **Configuration Caching**: Cache frequently used configurations
5. **Threading**: Use multiple threads for independent simulations

## Best Practices

1. **Always Initialize**: Call `initialize()` before using any component
2. **Reset Between Episodes**: Use `reset()` for fresh simulations
3. **Validate Configuration**: Check configuration settings before simulation
4. **Monitor Performance**: Use performance metrics to optimize parameters
5. **Save Checkpoints**: Regularly save progress to prevent data loss

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"
```python
# Solution: Call initialize()
brain = pynlm.createBrain(config)
brain.initialize()  # Essential!
brain.step(0)      # Now it will work
```

**"The agent isn't moving"
```python
# Solution: Update world before processing
world.update(0.1)
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
brain.step(step)
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

**"Everything is 0"
```python
# Solution: Run more steps for warm-up
for step in range(100):
    brain.step(step)
# The brain needs time to stabilize
```

**"ImportError: No module named 'pynlm'"
```bash
pip install --force-reinstall .
```

**"Segmentation fault"
```python
# Solution: Check initialization order
brain = pynlm.createBrain(config)
brain.initialize()  # Must be called first
brain.step(0)       # Then step
```

## Migration Guide

### From Easy Usage Guide to Advanced Guide

The advanced guide builds on the easy usage guide with additional functionality:

**Easy Usage → Advanced Usage**
- `pynlm.createBrain(config)` → `pynlm.createRLBrain()` or `pynlm.createSensoryBrain()`
- Manual configuration → `createStandardBrain()` helper
- Basic simulation → `runTrainingEpisode()`, `measurePerformance()`
- Simple stats → `getNetworkMetrics()`, `getRegionConnectivity()`

## Future Enhancements

Consider these improvements for future versions:

1. **Distributed Computing**: Multi-GPU and multi-node support
2. **Real-time Visualization**: Live plotting of neural activity
3. **Model Zoo**: Pre-trained models and architectures
4. **Gradient-based Learning**: Integration with deep learning frameworks
5. **Cloud Deployment**: Serverless execution environments
6. **Advanced Analytics**: Automated analysis of learning patterns

## Support and Resources

- **Documentation**: Comprehensive docstrings in bindings.cpp
- **Examples**: Advanced usage examples in this guide
- **Testing**: Unit tests in `tests/` directory
- **Community**: GitHub repository for issues and contributions

## License

MIT License - See project documentation for details.