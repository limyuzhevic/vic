# NLM Enhanced Python API Documentation

## Overview

NLM (Neural Learning Machine) is now available with an enhanced Python API that provides:

- **Better error handling** with custom exception types
- **More intuitive configuration** through the `PythonConfig` class
- **Enhanced brain and agent interfaces** with improved methods
- **Advanced simulation utilities** for batch processing and analysis
- **Comprehensive documentation** with examples for all use cases

## Installation

The enhanced Python API is installed automatically with the main package. You can upgrade to the latest version:

```bash
pip install --upgrade pynlm
```

## Quick Start with Enhanced API

### Example 1: Enhanced Configuration

```python
import pynlm

# Create an enhanced configuration
config = pynlm.createDefaultConfig()

# Use the enhanced configuration with better API
config.set("brain.neuron_count", 500, "runtime")
config.set("brain.synapse_density", 0.15, "runtime")
config.set("plasticity.stdp.enable", True, "default")

print(f"Config summary: {config.getSummary()}")
```

### Example 2: Using Enhanced Brain Interface

```python
import pynlm

# Create a brain with enhanced configuration
config = pynlm.createEnhancedConfig()  # New convenience function
brain = pynlm.createBrain(config)

# Initialize with error handling
try:
    if not brain.initialize():
        raise pynlm.SimulationError("Failed to initialize brain")
    print("Brain initialized successfully!")
except pynlm.SimulationError as e:
    print(f"Brain initialization failed: {e}")

# Run enhanced simulation
brain.step(0)
brain.step(1, 0.1)  # With timestamp

# Get enhanced statistics
print(f"Brain stats: {brain.getStats()}")
```

### Example 3: Complete Agent with Enhanced Utilities

```python
import pynlm

# Create a complete agent using enhanced utilities
def run_advanced_simulation(episode_count=10, steps_per_episode=100):
    print(f"Running {episode_count} episodes with {steps_per_episode} steps each")
    
    # Use the enhanced batch processing utility
    pynlm.runMultipleEpisodes(
        numEpisodes=episode_count,
        config=pynlm.createHighPerformanceConfig(),
        stepsPerEpisode=steps_per_episode,
        enableLearning=True,
        verbose=True
    )
    
    print("Simulation completed!")

# Run the simulation
run_advanced_simulation()
```

## Enhanced API Reference

### Configuration Classes

#### PythonConfig
The enhanced configuration class provides a more intuitive API:

```python
# Create enhanced configuration
config = pynlm.PythonConfig()

# Set values with explicit source control
config.set("brain.neuron_count", 1000, "default")  # Default configuration
config.set("brain.neuron_count", 500, "runtime")   # Runtime override
config.set("brain.neuron_count", 200, "experiment")  # Experiment-specific

# Get values with fallback
neuron_count = config.getOr("brain.neuron_count", 1000)

# Check existence and get keys
if config.has("brain.neuron_count"):
    available_keys = config.getKeys()

# File operations with error handling
config.loadFromFile("config.json")
config.saveToFile("enhanced_config.json")

# String representation
print(config.getSummary())
```

### Enhanced Brain Interface

#### PythonBrain
The enhanced brain class provides improved functionality:

```python
import pynlm

# Create brain with enhanced configuration
config = pynlm.createEnhancedConfig()
brain = pynlm.PythonBrain(config)

# Enhanced initialization with error checking
if brain.initialize():
    print("Brain initialized successfully!")
else:
    raise pynlm.SimulationError("Brain initialization failed")

# Enhanced simulation methods
brain.step(0)           # Basic step
brain.step(1, 0.1)      # Step with timestamp

# Enhanced statistics and monitoring
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Enhanced developmental control
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
print(f"Development stage: {brain.getDevelopmentalStage()}")

# Enhanced region management
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)

# Enhanced statistics
print(f"Brain statistics:\n{brain.getStats()}")
```

### Enhanced Agent Interface

#### PythonAgentBrain
The enhanced agent class provides comprehensive functionality:

```python
import pynlm

# Create enhanced agent
config = pynlm.createDevelopmentConfig()
brain = pynlm.PythonBrain(config)
agent = pynlm.PythonAgentBrain(brain)

# Create and configure world
world = pynlm.PythonWorld()
world.configure(20, 20, 8, 8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Initialize agent with enhanced error handling
agent.initialize(world)

# Enable all subsystems with enhanced API
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Enhanced sensory processing
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Enhanced motor decoding
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())

# Enhanced learning
reward = 1.0  # Calculate reward from environment
agent.applyRewardModulation(reward, 0.0)
agent.updateDevelopment(0.1)

# Enhanced statistics
print(f"Agent stats:")
print(f"  Developmental stage: {agent.getDevelopmentalStage()}")
print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"  Prediction error: {agent.getPredictionError():.3f}")

# Agent utilities
print(f"Agent status: {agent.getStatus()}")
```

### Enhanced World Interface

#### PythonWorld
The enhanced world class provides comprehensive simulation control:

```python
import pynlm

# Create enhanced world
world = pynlm.PythonWorld()

# Configure with enhanced parameters
world.configure(
    width=30,
    height=30,
    visionWidth=12,
    visionHeight=12
)

# Enhanced world control
world.reset()
world.setAgentStart(15.0, 15.0)
world.setRandomSeed(42)

# Enhanced simulation loop
time_step = 0.1
steps = 1000
for step in range(steps):
    world.update(time_step)
    
    # Get enhanced percept
    percept = world.getSensoryPercept()
    
    # Apply action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress with enhanced stats
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Enhanced world statistics
print(f"World stats:\n{world.getWorldStats()}")

# Enhanced object management
obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0, 0.5)
world.addObject(obj)

# Enhanced position checking
if world.isValidPosition(5.0, 5.0):
    print("Position is valid")
```

### Advanced Simulation Utilities

#### Batch Processing

```python
import pynlm

# Run multiple episodes efficiently
print("Running 20 episodes with high-performance configuration...")
pynlm.runMultipleEpisodes(
    numEpisodes=20,
    config=pynlm.createHighPerformanceConfig(),
    stepsPerEpisode=500,
    enableLearning=True,
    verbose=True
)

# Run episodes with specific configuration
print("Running development-focused episodes...")
pynlm.runMultipleEpisodes(
    numEpisodes=5,
    config=pynlm.createDevelopmentConfig(),
    stepsPerEpisode=200,
    enableLearning=True,
    verbose=False
)
```

#### Analysis Utilities

```python
import pynlm

# Create simulation components
config = pynlm.createDefaultConfig()
brain = pynlm.PythonBrain(config)
agent = pynlm.PythonAgentBrain(brain)
world = pynlm.PythonWorld()
world.configure(20, 20, 8, 8)
world.reset()

# Initialize and run simulation
brain.initialize()
agent.initialize(world)

# Run some steps for analysis
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Get comprehensive analysis
print("=== COMPREHENSIVE ANALYSIS ===")
print(brain.getStats())
print(f"\nWorld statistics:\n{world.getWorldStats()}")
print(f"\nAgent statistics:\n{agent.getAgentStats()}")
```

#### Configuration Factory Functions

```python
import pynlm

# Create specialized configurations for different use cases

# Default configuration (good for general use)
default_config = pynlm.createDefaultConfig()

# Minimal configuration (fast startup)
minimal_config = pynlm.createMinimalConfig()

# High-performance configuration (max capabilities)
high_perf_config = pynlm.createHighPerformanceConfig()

# Development configuration (focus on learning)
dev_config = pynlm.createDevelopmentConfig()

# Use configurations
brain1 = pynlm.PythonBrain(default_config)
brain2 = pynlm.PythonBrain(minimal_config)
brain3 = pynlm.PythonBrain(high_perf_config)
brain4 = pynlm.PythonBrain(dev_config)
```

## Error Handling

The enhanced API provides comprehensive error handling:

```python
import pynlm

# Create components
config = pynlm.PythonConfig()
config.set("brain.neuron_count", 1000, "runtime")

brain = pynlm.PythonBrain(config)

# Example 1: Configuration errors
try:
    # This would raise an InvalidConfigException
    config.loadFromFile("nonexistent_file.json")
except pynlm.InvalidConfigException as e:
    print(f"Configuration error: {e}")
except Exception as e:
    print(f"Other error: {e}")

# Example 2: Simulation errors
try:
    if not brain.initialize():
        raise pynlm.SimulationError("Brain initialization failed")
    
    brain.step(0)  # This could also raise errors
    
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.WorldError as e:
    print(f"World error: {e}")
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Advanced Features

### Custom Neuron Injection

```python
import pynlm

config = pynlm.PythonConfig()
config.set("brain.neuron_count", 100, "runtime")

brain = pynlm.PythonBrain(config)
brain.initialize()

# Inject current into specific neurons
brain.injectCurrent(pynlm.NeuronId(0), 5.0)  # Inject 5.0 current into neuron 0
brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 2.0)  # Inject to all excitatory neurons
```

### Developmental Control

```python
import pynlm

brain = pynlm.PythonBrain(pynlm.createDefaultConfig())
brain.initialize()

# Developmental stage progression
stages = [
    pynlm.DevelopmentalStage.Initial,
    pynlm.DevelopmentalStage.CriticalPeriod,
    pynlm.DevelopmentalStage.Maturation,
    pynlm.DevelopmentalStage.Adult,
]

for stage in stages:
    brain.setDevelopmentalStage(stage)
    print(f"Developmental stage: {stage}")
    # Run simulation at this stage
    brain.step(0)
```

### Advanced Agent Features

```python
import pynlm

# Create agent with all features enabled
config = pynlm.createHighPerformanceConfig()
brain = pynlm.PythonBrain(config)
agent = pynlm.PythonAgentBrain(brain)

# Create world and initialize
world = pynlm.PythonWorld()
world.configure(20, 20, 8, 8)
world.reset()
agent.initialize(world)

# Enable all learning mechanisms
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run learning simulation
for step in range(500):
    world.update(0.1)
    
    # Get percept and process
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Get action and execute
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    reward = calculate_reward()  # Your reward function
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
```

## Performance Tips

### For Beginners

1. **Start with minimal configuration**:
   ```python
   config = pynlm.createMinimalConfig()
   ```

2. **Use simple worlds first**:
   ```python
   world = pynlm.PythonWorld()
   world.configure(10, 10, 4, 4)  # Small world for quick testing
   ```

3. **Enable learning gradually**:
   ```python
   agent.enableRewardModulation(True)
   # Start with basic learning before adding other features
   ```

### For Advanced Users

1. **Use specialized configurations**:
   ```python
   config = pynlm.createHighPerformanceConfig()
   ```

2. **Batch process episodes**:
   ```python
   pynlm.runMultipleEpisodes(
       numEpisodes=100,
       config=pynlm.createHighPerformanceConfig(),
       stepsPerEpisode=1000
   )
   ```

3. **Monitor performance**:
   ```python
   print(brain.getStats())
   print(world.getWorldStats())
   print(agent.getAgentStats())
   ```

## Troubleshooting

### Common Issues and Solutions

**Issue 1: Brain initialization fails**
```python
# Solution: Check configuration and initialization
config = pynlm.PythonConfig()
config.set("brain.neuron_count", 100, "runtime")

brain = pynlm.PythonBrain(config)
if not brain.initialize():
    print("Brain initialization failed")
    print("Check configuration parameters")
```

**Issue 2: Memory usage is high**
```python
# Solution: Use minimal configuration or high-performance configuration
config = pynlm.createMinimalConfig()  # Fewer neurons
# or
config = pynlm.createHighPerformanceConfig()  # Better memory management
```

**Issue 3: Simulation is slow**
```python
# Solution: Use smaller worlds and fewer steps
world = pynlm.PythonWorld()
world.configure(10, 10, 4, 4)  # Small world
pynlm.runMultipleEpisodes(
    numEpisodes=10,
    stepsPerEpisode=100
)
```

**Issue 4: Errors not caught properly**
```python
# Solution: Use comprehensive error handling
try:
    # Your simulation code
    pass
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Other error: {e}")
```

## Migration Guide

If you're upgrading from the original API, here are the key changes:

### Old API → New API

**Configuration**:
- Old: `config.set("key", value)`
- New: `config.set("key", value, "source")` with source control

**Brain Creation**:
- Old: `brain = pynlm.createBrain(config)`
- New: `brain = pynlm.PythonBrain(config)` with enhanced features

**Agent Creation**:
- Old: `agent = pynlm.createAgentBrain(brain)`
- New: `agent = pynlm.PythonAgentBrain(brain)` with enhanced methods

**World Creation**:
- Old: `world = pynlm.createSimpleWorld()`
- New: `world = pynlm.PythonWorld()` with enhanced features

**Error Handling**:
- Old: Basic Python exceptions
- New: Custom NLMException hierarchy with specific error types

**Utilities**:
- Old: Basic examples only
- New: Comprehensive utilities (batch processing, analysis, configuration factories)

### Migration Example

**Old code**:
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(10, 10, 4, 4)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

**New code**:
```python
import pynlm

# Use enhanced configuration
config = pynlm.createDevelopmentConfig()
brain = pynlm.PythonBrain(config)

if not brain.initialize():
    raise pynlm.SimulationError("Brain initialization failed")

world = pynlm.PythonWorld()
world.configure(10, 10, 4, 4)
world.reset()

agent = pynlm.PythonAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableDevelopment(True)

# Enhanced error handling
try:
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = calculate_reward()
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
except pynlm.NLMException as e:
    print(f"Simulation error: {e}")

# Show enhanced statistics
print(brain.getStats())
print(world.getWorldStats())
```

## Compatibility

The enhanced API maintains full backward compatibility:

- **Original classes** still available: `Brain`, `Config`, `AgentBrain`, `SimpleWorld`
- **Original methods** preserved with enhanced implementations
- **Original constructors** maintained for compatibility
- **Same behavior** for all existing code

The enhanced API provides additional features and improvements while maintaining the original interface for existing code.

## Conclusion

The enhanced NLM Python API provides:

1. **Better error handling** with custom exception types
2. **More intuitive configuration** through `PythonConfig`
3. **Enhanced brain and agent interfaces** with improved methods
4. **Advanced simulation utilities** for batch processing
5. **Comprehensive documentation** with examples
6. **Full backward compatibility** with original API

Start with the quick start examples for beginners, or use the advanced features for power users. The enhanced API makes it easier to create, simulate, and analyze neural networks while maintaining the simplicity of the original API.