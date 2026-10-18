# NLM Phase 6 Integration Guide

## Quick Start Examples

### Example 1: Basic Simulation

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrainWithDefaults()

# Initialize the brain
brain.initialize()

# Run a few simulation steps
for step in range(100):
    brain.step(step, step * 0.001)

print("Simulation completed!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
```

### Example 2: Custom Configuration

```python
import pynlm

# Create configuration
config = pynlm.Config()
config.set("neuron_count", 2000)
config.set("region_count", 3)
config.set("connection_probability", 0.15)
config.set("plasticity_learning_rate", 0.05)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for step in range(500):
    brain.step(step, step * 0.001)

print("Custom configuration simulation completed!")
```

### Example 3: Simple World Environment

```python
import pynlm

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)

# Place agent in world
world.setAgentStart(15, 15)

# Create agent brain
brain = pynlm.createBrain(pynlm.createBrainWithDefaults())
agent = pynlm.createAgentBrain(brain)

# Initialize world with agent
agent.initialize(world)

# Run simulation
for step in range(1000):
    # Get sensory input from world
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain step
    brain.step(step, step * 0.001)
    
    # Decode motor command
    motor_cmd = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(motor_cmd, step * 0.001)
    
    # Check if simulation should end
    if world.getSimulationTime() >= 10.0:  # 10 seconds
        break

print("World simulation completed!")
```

### Example 4: Integration Test

```python
import pynlm

# Create brain and run integration test
brain = pynlm.createBrainWithDefaults()
brain.initialize()

# Run integration test using the enhanced main function interface
print("=== Phase 6 Integration Test ===")
pynlm.runIntegrationDemo(brain)

print("Integration test completed!")
```

### Example 5: Configuration Management

```python
import pynlm

# Load from file
config = pynlm.Config()
if config.loadFromFile("myconfig.cfg"):
    print("Configuration loaded successfully!")
else:
    print("Using default configuration")

# Save configuration
config.saveToFile("backup_config.cfg")

# Check configuration
if config.has("neuron_count"):
    count = config.get("neuron_count")
    print(f"Neuron count: {count}")

# Get all configuration keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Display configuration summary
print(config.summary())
```

### Example 6: Working Memory and Episodic Memory

```python
import pynlm

# Create brain
brain = pynlm.createBrainWithDefaults()
brain.initialize()

# Note: Memory systems are currently disconnected in Phase 6
# But this shows how they would be used when integrated
working_memory = brain.getWorkingMemory()
if working_memory:
    print("Working memory available!")
    # working_memory.store(...)
else:
    print("Working memory not available in current Phase 6 implementation")

# Episodic memory (when implemented)
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    # episodic_memory.storeExperience(...)
    pass
```

### Example 7: Development Stages

```python
import pynlm

# Create brain
brain = pynlm.createBrainWithDefaults()
brain.initialize()

# Check developmental stage
current_stage = brain.getDevelopmentalStage()
print(f"Current developmental stage: {current_stage}")

# Set developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
print(f"New developmental stage: {brain.getDevelopmentalStage()}")

# Development affects plasticity rates during critical periods
if brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.CriticalPeriod:
    print("Critical period - high plasticity rates!")
```

### Example 8: Sensory Processing

```python
import pynlm

# Create vision input
vision = pynlm.Vision(10, 10, 3)  # 10x10 pixels, 3 color channels
vision.setData([0.1, 0.2, 0.3, ...])  # Set pixel data
vision.setTimestamp(1.5)

print(f"Vision width: {vision.getWidth()}")
print(f"Vision height: {vision.getHeight()}")
print(f"Vision dimensions: {vision.getDimensions()}")
print(f"Vision timestamp: {vision.getTimestamp()}")

# Audio input
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
audio.setData([0.0, 0.1, -0.2, ...])  # Audio samples
audio.setSampleRate(48000)  # Change sample rate

print(f"Audio sample rate: {audio.getSampleRate()}")
print(f"Audio num samples: {audio.getNumSamples()}")
```

### Example 9: Action and Motor Commands

```python
import pynlm

# Create actions
action1 = pynlm.Action(pynlm.ActionType.MoveForward)
action2 = pynlm.Action(pynlm.ActionType.Look, [45.0, 0.0])  # Look 45 degrees right

print(f"Action type: {action1.getType()}")
print(f"Action name: {action1.getName()}")

# Get parameters
print(f"Action parameters: {action1.getParameters()}")

# Clone action
cloned_action = action1.clone()
```

### Example 10: Checkpointing and Persistence

```python
import pynlm

# Create brain
brain = pynlm.createBrainWithDefaults()
brain.initialize()

# Run some simulation
for step in range(100):
    brain.step(step, step * 0.001)

# Save checkpoint
if brain.save("my_checkpoint.nlm"):
    print("Checkpoint saved successfully!")
else:
    print("Failed to save checkpoint")

# Load checkpoint
brain2 = pynlm.createBrainWithDefaults()
if brain2.load("my_checkpoint.nlm"):
    print("Checkpoint loaded successfully!")
else:
    print("Failed to load checkpoint")
```

## Advanced Usage Examples

### Advanced 1: Multi-Step Prediction

```python
import pynlm

# Create brain and run comprehensive simulation
brain = pynlm.createBrainWithDefaults()
brain.initialize()

# Run integration test
print("Running comprehensive integration test...")
for step in range(1000):
    brain.step(step, step * 0.001)
    
    # Check for interesting patterns
    if step % 100 == 0:
        spikes = brain.getTotalSpikeCount()
        active = brain.getActiveNeuronCount()
        print(f"Step {step}: {spikes} total spikes, {active} active neurons")

print("Advanced simulation completed!")
```

### Advanced 2: Performance Benchmarking

```python
import pynlm

# Create brain
brain = pynlm.createBrainWithDefaults()
brain.initialize()

# Run benchmark
print("Starting benchmark...")
pynlm.benchmarkBrain(brain, 1000)

# Apply performance optimizations
brain = pynlm.optimizeForSpeed(brain)
brain = pynlm.optimizeForAccuracy(brain)
```

### Advanced 3: Configuration Presets

```python
import pynlm

# Create development-focused configuration
dev_config = pynlm.createDevelopmentConfig()
print("Development configuration created!")

# Create reproduction-focused configuration
rep_config = pynlm.createReproductionConfig()
print("Reproduction configuration created!")

# Create world-specific configurations
grid_config = pynlm.createGridWorldConfig()
social_config = pynlm.createSocialWorldConfig()
```

### Advanced 4: Integration with Custom Worlds

```python
import pynlm

# Create world with custom parameters
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=15, visionHeight=15)

# Customize world properties
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.01)
world.setRandomSeed(12345)

# Create agent with custom brain
brain = pynlm.createBrainWithDefaults()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run custom simulation
for step in range(2000):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step, step * 0.001)
    motor_cmd = agent.decodeMotorCommand()
    world.applyMotorCommand(motor_cmd, step * 0.001)
```

## Getting Started Checklist

### Step 1: Basic Installation
- ✅ Install NLM Python bindings
- ✅ Set up development environment
- ✅ Build the C++ library

### Step 2: Create Your First Brain
```python
brain = pynlm.createBrainWithDefaults()
brain.initialize()
```

### Step 3: Run a Simple Simulation
```python
for step in range(100):
    brain.step(step, step * 0.001)
```

### Step 4: Explore Brain State
```python
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Step 5: Try Integration Demo
```python
pynlm.runIntegrationDemo()
```

## Troubleshooting Common Issues

### Issue 1: "Failed to initialize brain"
**Solution:** Check configuration values and ensure all required settings are present.

### Issue 2: Memory usage too high
**Solution:** Use development configuration for faster testing, or optimize for speed.

### Issue 3: Simulation too slow
**Solution:** Use Phase 6 configuration with optimized settings, or enable performance optimizations.

### Issue 4: Invalid configuration
**Solution:** Validate configuration before creating brain, use createDevelopmentConfig() for reliable settings.

### Issue 5: Python bindings not found
**Solution:** Build Python bindings with `python setup.py build_ext --inplace` or install with pip.

## Performance Tips

### For Large-Scale Simulations:
- Use development configuration
- Enable speed optimizations
- Monitor simulation time
- Use checkpointing for recovery

### For Research Experiments:
- Use custom configurations
- Validate configurations before use
- Benchmark performance
- Compare results across configurations

### For Development:
- Use reproduction configuration for stable results
- Test with small numbers first
- Validate configuration at startup
- Use logging for debugging

## Best Practices

1. **Always validate configuration** before creating brain
2. **Use development configuration** for testing
3. **Benchmark performance** for large simulations
4. **Use checkpointing** for long-running experiments
5. **Monitor system state** during simulation
6. **Use reproduction configuration** for stable results
7. **Log important events** for debugging
8. **Validate input data** before processing

## Next Steps

### Phase 6 Integration:
- Connect working memory to sensory processing
- Connect episodic memory for experience storage
- Connect prediction system for temporal learning
- Connect cognitive systems for planning and attention

### Future Enhancements:
- Add sleep/rest cycle with memory consolidation
- Implement replay mechanisms
- Enhance neuromodulation beyond dopamine
- Add self-model integration
- Implement social learning

This guide provides practical examples for getting started with NLM Phase 6 integration. The system is designed to be both powerful for research and accessible for development.