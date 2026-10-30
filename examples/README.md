# Simple NLM Python Example

This example demonstrates the basic usage of the NLM Python bindings.

## Basic Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()

# Run a few steps
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Enhanced Example

```python
import pynlm

# Create a simple sensory-motor loop
brain, world = pynlm.createSimpleSensoryMotorLoop(
    worldSize="20x20",
    neuronCount=500,
    enableDevelopment=True,
    enableMemory=True,
    enableNeuromodulation=True
)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run experiment
results = pynlm.runSensoryMotorExperiment(
    brain, world,
    numEpisodes=5,
    maxStepsPerEpisode=200
)

print(f"Total reward: {results.totalReward:.2f}")
print(f"Avg firing rate: {results.avgFiringRate:.2f} Hz")
print(f"Memory episodes: {results.memoryEpisodes}")
```

## Configuration Management

```python
import pynlm

# Create configurations using convenience functions
small_config = pynlm.createSmallConfig()
medium_config = pynlm.createMediumConfig()
large_config = pynlm.createLargeConfig()

# Validate configuration
errors = []
if pynlm.validateConfig(medium_config, errors):
    print("Configuration is valid")
else:
    print("Configuration errors:", errors)
```

## Best Practices

1. Always validate configurations before use
2. Use convenience functions for common setup scenarios
3. Monitor experiment results using the returned results objects
4. Check brain health before critical operations
5. Use appropriate configuration sizes for your use case

## Available Features

- **Configuration Management**: Quick setup with small/medium/large configurations
- **Validation**: Built-in configuration validation with error reporting
- **Sensory-Motor Loop**: Ready-to-use world interaction setup
- **Experiment Runner**: Complete experiment execution with results tracking
- **Brain Health Checks**: Methods to verify brain state and health
- **Development Control**: Enable/disable development, memory, neuromodulation
- **High-Level APIs**: Convenience methods for common operations

## Performance Tips

- Use `createSmallConfig()` for quick testing
- Use `createMediumConfig()` for development and validation
- Use `createLargeConfig()` for production and research
- Monitor memory usage when running large networks
- Use configuration validation to catch errors early
