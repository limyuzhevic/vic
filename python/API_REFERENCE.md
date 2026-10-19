# NLM Python API Reference - Enhanced Edition

## Overview

This document provides comprehensive documentation for the enhanced NLM Python API. Building upon the original bindings, this version includes extensive improvements for performance, usability, and advanced features.

## New Features Overview

### 1. Enhanced Core API
- **Advanced Configuration Management**: `createAdvancedConfig()` with Phase 6 defaults
- **Enhanced Brain Factory**: `createBrain()` with improved validation
- **World Management**: `createWorldWithFeatures()` for standard configurations
- **Agent Factory**: `createAgentWithFeatures()` with optimized settings

### 2. Batch Operations for Performance
- **Batch Parameter Updates**: `batchSetNeuronParameters()`
- **Batch State Queries**: `batchGetNeuronStates()`
- **Batch Learning**: `batchApplyLearning()`
- **Batch Neuron Creation**: `batchCreateNeurons()`

### 3. Advanced API for Power Users
- **Performance Monitoring**: `getBrainPerformanceStats()`
- **Memory Profiling**: `profileBrainMemory()`
- **Configuration Validation**: `validateBrainConfig()`
- **Experiment Management**: Serialization and import/export utilities
- **Visualization Pipeline**: Configuration for monitoring
- **Learning Optimization**: `optimizeBrainForTraining()`

### 4. Enhanced Error Handling and Validation
- Comprehensive input validation for all parameters
- Graceful error recovery mechanisms
- Detailed error messages with actionable feedback
- Type checking and parameter validation

### 5. Convenience Methods
- **Debug Utilities**: `debugBrainState()` for troubleshooting
- **World Validation**: `validateWorldState()`
- **Action Utilities**: `createActionSequence()`, `actionToString()`
- **Sensor Normalization**: `normalizeSensorData()`
- **Resource Management**: `spawnResources()`

### 6. Advanced Configuration Management
- **Configuration Merging**: `mergeConfigs()` for flexible configuration
- **Configuration Validation**: `validateBrainConfig()` with detailed results
- **Phase 6 Experiment Setup**: `createPhase6ExperimentSetup()`

## API Reference

### Core Classes

#### Brain Class
```python
# Enhanced brain creation with validation
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

# Debug and monitoring
brain_info = pynlm.debugBrainState(brain)
stats = pynlm.getBrainPerformanceStats(brain)
```

#### Config Class
```python
# Enhanced configuration
config = pynlm.createAdvancedConfig()
validation = pynlm.validateBrainConfig(config)

# Configuration merging
base_config = pynlm.createDefaultConfig()
override_config = pynlm.createAdvancedConfig()
merged_config = pynlm.mergeConfigs(base_config, override_config)
```

#### World Class
```python
# Enhanced world creation
world = pynlm.createWorldWithFeatures()
validation = pynlm.validateWorldState(world)

# Resource management
pynlm.spawnResources(world, count=10, resourceType=pynlm.WorldObjectType.Resource)
```

#### AgentBrain Class
```python
# Enhanced agent creation
agent = pynlm.createAgentWithFeatures(brain, world)
```

### Batch Operations

#### Neuron Operations
```python
# Batch parameter updates
neuron_ids = [pynlm.INVALID_NEURON_ID, ...]  # Get from existing brain
pynlm.batchSetNeuronParameters(
    brain,
    neuron_ids,
    membranePotential=0.0,
    threshold=-55.0,
    type=pynlm.NeuronType.Excitatory,
    refractoryPeriod=5
)

# Batch state queries
states = pynlm.batchGetNeuronStates(brain, neuron_ids)
```

#### Learning Operations
```python
# Batch learning application
pynlm.batchApplyLearning(
    brain,
    targetTypes=[pynlm.NeuronType.Sensory, pynlm.NeuronType.Internal],
    strength=30.0,
    steps=100
)
```

### Performance Monitoring

```python
# Get comprehensive performance statistics
stats = pynlm.getBrainPerformanceStats(brain)
print(f"Firing Rate: {stats['firing_rate_percent']:.1f}%")
print(f"STDP Active: {stats['stdp_active']}")
print(f"Hebbian Active: {stats['hebbian_active']}")

# Memory profiling
profile = pynlm.profileBrainMemory(brain)
print(f"Estimated Memory: {profile['estimated_mb']:.2f} MB")
```

### Experiment Management

```python
# Export brain for experiment
pynlm.exportBrainForExperiment(brain, "experiment_save.pkl")

# Import brain from experiment
imported_brain = pynlm.importBrainForExperiment("experiment_save.pkl")
```

### Configuration Management

```python
# Create experiment configuration
exp_config = pynlm.createExperimentConfig()

# Create Phase 6 experiment setup
phase6_setup = pynlm.createPhase6ExperimentSetup()
```

## Best Practices

### 1. Configuration Management
```python
def createOptimizedConfig():
    """Create configuration optimized for Phase 6 experiments."""
    config = pynlm.createAdvancedConfig()
    
    # Customize for specific use case
    config.set("neuron_count", 1000, ConfigSource.User)
    config.set("enable_structural_plasticity", True, ConfigSource.User)
    config.set("enable_curiosity", True, ConfigSource.User)
    
    return config
```

### 2. Performance Optimization
```python
def runOptimizedSimulation(brain, world, agent, steps):
    """Run simulation with performance optimizations."""
    # Apply optimizations
    pynlm.optimizeBrainForTraining(brain)
    
    # Use batch operations for better performance
    for step in range(steps):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent (can be batched for multiple inputs)
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

### 3. Monitoring and Debugging
```python
def monitorSimulation(brain, world, agent, steps):
    """Monitor simulation with detailed statistics."""
    for step in range(steps):
        # Get performance stats periodically
        if step % 100 == 0:
            stats = pynlm.getBrainPerformanceStats(brain)
            print(f"Step {step}: Firing Rate={stats['firing_rate_percent']:.1f}%")
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

### 4. Error Handling
```python
def safeSimulation(brain, world, agent, steps):
    """Run simulation with comprehensive error handling."""
    try:
        for step in range(steps):
            # Validate world state
            world_validation = pynlm.validateWorldState(world)
            if not world_validation["valid"]:
                print(f"Invalid world state at step {step}")
                continue
            
            # Run simulation step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
    except Exception as e:
        print(f"Simulation error: {e}")
        # Recovery logic here
        raise
```

## Examples

### Example 1: Advanced Simulation with Monitoring
```python
import pynlm

def advanced_simulation(num_steps=1000):
    """Run advanced simulation with comprehensive monitoring."""
    
    # Create optimized configuration
    config = pynlm.createAdvancedConfig()
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent with features
    world = pynlm.createWorldWithFeatures()
    agent = pynlm.createAgentWithFeatures(brain, world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Starting advanced simulation...")
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input and normalize
        percept = world.getSensoryPercept()
        normalized_percept = pynlm.normalizeSensorData(percept)
        
        # Process with agent
        agent.processSensoryInput(normalized_percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Monitor periodically
        if step % 100 == 0:
            stats = pynlm.getBrainPerformanceStats(brain)
            print(f"Step {step}: Spikes={stats['total_spikes']}, "
                  f"Firing Rate={stats['firing_rate_percent']:.1f}%")
    
    print("Simulation complete!")
    return brain, world, agent
```

### Example 2: Batch Operations for Efficiency
```python
import pynlm

def batch_operations_demo():
    """Demonstrate batch operations for efficiency."""
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createAdvancedConfig())
    brain.initialize()
    
    # Get some neuron IDs for batch operations
    spike_system = brain.getSpikeSystem()
    neurons = spike_system.getAllNeurons()
    neuron_ids = [neuron.getId() for neuron in neurons[:10]]  # First 10 neurons
    
    print("Demonstrating batch operations...")
    
    # Batch set neuron parameters
    pynlm.batchSetNeuronParameters(
        brain,
        neuron_ids,
        threshold=-50.0,
        type=pynlm.NeuronType.Excitatory,
        refractoryPeriod=3
    )
    
    # Get batch states
    states = pynlm.batchGetNeuronStates(brain, neuron_ids)
    print(f"Batch states retrieved: {len(states)} neurons")
    
    # Batch learning
    pynlm.batchApplyLearning(
        brain,
        targetTypes=[pynlm.NeuronType.Sensory],
        strength=40.0,
        steps=50
    )
    
    print("Batch operations complete!")
```

### Example 3: Experiment Management
```python
import pynlm

def experiment_management_demo():
    """Demonstrate experiment management features."""
    
    # Create experiment configuration
    exp_config = pynlm.createExperimentConfig()
    exp_config["name"] = "NLM_Batch_Learning_Experiment"
    
    # Create brain with experiment settings
    brain = pynlm.createBrain(pynlm.createAdvancedConfig())
    brain.initialize()
    
    # Run simulation
    world = pynlm.createWorldWithFeatures()
    agent = pynlm.createAgentWithFeatures(brain, world)
    
    # Run experiment
    for step in range(500):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Export for analysis
    pynlm.exportBrainForExperiment(brain, "experiment_results.pkl")
    
    # Get performance statistics
    stats = pynlm.getBrainPerformanceStats(brain)
    profile = pynlm.profileBrainMemory(brain)
    
    print(f"Experiment complete!")
    print(f"Statistics: {stats}")
    print(f"Memory Profile: {profile}")
    
    return brain, exp_config
```

## Migration Guide

### From Basic to Advanced API

If you're currently using the basic API and want to upgrade to the advanced features:

```python
# Before (basic API)
from pynlm import createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain

brain = createBrain(createDefaultConfig())
brain.initialize()

world = createSimpleWorld()
agent = createAgentBrain(brain)

# After (advanced API)
from pynlm import createBrain, createAdvancedConfig, createWorldWithFeatures, createAgentWithFeatures

brain = createBrain(createAdvancedConfig())
brain.initialize()

world = createWorldWithFeatures()
agent = createAgentWithFeatures(brain, world)
```

### Key Differences

1. **Configuration**: Use `createAdvancedConfig()` instead of `createDefaultConfig()`
2. **World**: Use `createWorldWithFeatures()` instead of `createSimpleWorld()`
3. **Agent**: Use `createAgentWithFeatures()` instead of `createAgentBrain()`
4. **New Features**: Batch operations, performance monitoring, experiment management

## Performance Considerations

### 1. Batch Operations
- Use `batchSetNeuronParameters()` instead of individual parameter sets
- Use `batchGetNeuronStates()` for efficient state retrieval
- Use `batchApplyLearning()` for multiple learning steps

### 2. Memory Management
- Monitor memory usage with `profileBrainMemory()`
- Use `optimizeBrainForTraining()` for large simulations
- Consider using checkpointing for long-running experiments

### 3. Configuration
- Merge configurations efficiently with `mergeConfigs()`
- Validate configurations before use with `validateBrainConfig()`
- Use `createAdvancedConfig()` as a starting point for optimized configurations

## Troubleshooting

### Common Issues and Solutions

**Issue 1: Brain not responding to input**
```python
# Solution: Check brain initialization
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()  # Always call this!

# Solution: Validate configuration
config = pynlm.createAdvancedConfig()
validation = pynlm.validateBrainConfig(config)
if not validation["valid"]:
    print("Configuration invalid:", validation)
```

**Issue 2: Poor performance**
```python
# Solution: Apply performance optimizations
pynlm.optimizeBrainForTraining(brain)

# Solution: Use batch operations
pynlm.batchApplyLearning(brain, targetTypes=[...], strength=..., steps=...)
```

**Issue 3: Memory issues**
```python
# Solution: Monitor memory usage
profile = pynlm.profileBrainMemory(brain)
print(f"Estimated memory: {profile['estimated_mb']:.2f} MB")

# Solution: Optimize for training
pynlm.optimizeBrainForTraining(brain)
```

## References

- [NLM Documentation](https://nlm.readthedocs.io)
- [Phase 6 Integration Guide](docs/PHASE6_FINAL_AUDIT.md)
- [Performance Optimization](docs/ARCHITECTURE.md#performance)

## License

MIT
