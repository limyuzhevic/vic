# Enhanced NLM Documentation
# Updated API and Examples

This document provides comprehensive information about the enhanced NLM API, including new convenience functions, presets, and examples.

## Overview of Enhancements

This documentation covers the improvements made to the NLM (Neural Learning Machine) framework, including:

1. **Enhanced Python Bindings** - New convenience functions and validation
2. **Factory Methods and Presets** - Quick setup for common use cases
3. **Error Handling** - Comprehensive validation and error recovery
4. **Advanced Examples** - Complex use cases and best practices
5. **Performance Monitoring** - Tools for tracking simulation metrics

## 1. Enhanced Python API

### 1.1 Quick Start with Enhanced Functions

The enhanced API provides several new convenience functions for rapid prototyping:

```python
import pynlm

# Create a quick-start brain with default configuration
brain = pynlm.createQuickBrain(
    neuronCount=1000,
    timestep=0.001,
    enablePlasticity=True,
    enableDevelopment=False,
    enableCuriosity=True
)

# Create a complete agent with all systems enabled
brain, world, agent = pynlm.createCompleteAgent(
    name="my_agent",
    neurons=2000,
    worldSize=30
)

# Run a validated simulation
results = pynlm.runValidatedSimulation(
    agent=agent,
    world=world,
    steps=500,
    worldTimestep=0.05,
    enableLogging=True
)
```

### 1.2 Validation and Error Handling

The enhanced API includes comprehensive validation:

```python
# Validate configuration
pynlm.validateConfig(config)

# Validate world dimensions
pynlm.validateWorldDimensions(20, 20, 8, 8)

# Validate simulation parameters
pynlm.validateSimulationParams(1000, 0.001)
```

All functions now throw informative exceptions:
- `pynlm.RuntimeError` for runtime errors
- `pynlm.ValueError` for invalid parameter values

## 2. Simulation Presets

The enhanced API provides predefined configurations for common use cases:

### 2.1 Available Presets

| Preset | Neurons | Timestep | Plasticity | Development | Use Case |
|--------|---------|----------|------------|-------------|----------|
| `minimal` | 100 | 0.001 | None | Disabled | Quick testing |
| `simple` | 500 | 0.001 | Basic | Disabled | Basic learning |
| `advanced` | 2000 | 0.0005 | Full | Enabled | Complex learning |

### 2.2 Using Presets

```python
# Create a simple preset brain
brain = pynlm.createSimulationPreset("simple")

# Create an advanced preset agent
brain, world, agent = pynlm.createCompleteAgent()
brain = pynlm.createSimulationPreset("advanced")
```

## 3. Advanced Examples

### 3.1 Complete Agent Example

This example demonstrates creating and running a complete agent with all systems enabled:

```python
import pynlm

# Create a complete agent with all features
brain, world, agent = pynlm.createCompleteAgent(
    name="advanced_agent",
    neurons=1500,
    worldSize=25
)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation with monitoring
results = pynlm.runValidatedSimulation(
    agent=agent,
    world=world,
    steps=800,
    worldTimestep=0.05,
    enableLogging=True,
    callback=lambda step, data: print(f"Step {step}: Health={data['agent_health']:.2f}")
)

if results["success"]:
    print("Simulation completed successfully!")
    print(f"Final stats: {results['brain_stats']}")
else:
    print(f"Simulation failed: {results['error']}")
```

### 3.2 Learning Experiment Example

This example demonstrates tracking learning progress through neuromodulation:

```python
def learning_monitor(step, data):
    """Custom monitoring callback for learning experiments"""
    if step % 100 == 0:
        print(f"Trial {step}:")
        print(f"  - Dopamine: {data['dopamine_level']:.3f}")
        print(f"  - Curiosity: {data['curiosity_level']:.3f}")
        print(f"  - Health: {data['agent_health']:.3f}")
        
        # Analyze learning
        if data['dopamine_level'] > 0.5:
            print("  -> High motivation detected!")
        if data['curiosity_level'] > 0.3:
            print("  -> High exploration detected!")

# Run learning experiment
results = pynlm.runValidatedSimulation(
    agent=agent,
    world=world,
    steps=1000,
    callback=learning_monitor
)
```

### 3.3 Preset Comparison Example

This example compares different simulation presets:

```python
import pynlm

presets = ["minimal", "simple", "advanced"]
results = {}

for preset in presets:
    print(f"\nTesting {preset} preset...")
    
    # Create agent with preset
    brain, world, agent = pynlm.createValidatedAgent(
        pynlm.createSimulationPreset(preset),
        worldWidth=15,
        worldHeight=15
    )
    
    # Run quick test
    test_results = pynlm.runValidatedSimulation(agent, world, 200, 0.1)
    
    if test_results["success"]:
        results[preset] = test_results["brain_stats"]
        print(f"  - {preset}: {results[preset]['total_spikes']} spikes, "
              f"{results[preset]['average_firing_rate']:.3f} avg firing rate")
```

## 4. Error Handling and Validation

### 4.1 Common Errors and Solutions

| Error | Cause | Solution |
|-------|-------|----------|
| `RuntimeError: Missing required configuration key` | Missing config parameter | Check documentation for required keys |
| `ValueError: World dimensions too large` | Excessive world size | Reduce dimensions or use smaller preset |
| `RuntimeError: Invalid preset name` | Unknown preset | Use "minimal", "simple", or "advanced" |

### 4.2 Error Recovery

The enhanced API includes error recovery mechanisms:

```python
# Run simulation with error handling
try:
    results = pynlm.runValidatedSimulation(agent, world, 1000)
    if results["success"]:
        print("Simulation completed successfully")
    else:
        print(f"Simulation failed: {results['error']}")
        
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
    
except pynlm.ValueError as e:
    print(f"Value error: {e}")
```

## 5. Performance Monitoring

### 5.1 Brain Statistics

The enhanced API provides comprehensive brain statistics:

```python
brain_stats = pynlm.getBrainStats(brain)

print(f"Total neurons: {brain_stats['total_neurons']}")
print(f"Total synapses: {brain_stats['total_synapses']}")
print(f"Total spikes: {brain_stats['total_spikes']}")
print(f"Average firing rate: {brain_stats['average_firing_rate']:.3f}")
print(f"E/I ratio: {brain_stats['excitatory_inhibition_ratio']:.3f}")

# Memory system statistics
print(f"Working memory traces: {brain_stats['working_memory_traces']}")
print(f"Episodic memory episodes: {brain_stats['episodic_memory_episodes']}")

# Neuromodulation statistics
print(f"Dopamine level: {brain_stats['dopamine_level']:.3f}")
print(f"Curiosity level: {brain_stats['curiosity_level']:.3f}")
```

### 5.2 Region Monitoring

Monitor individual brain regions:

```python
regions = brain_stats['regions']
for region in regions:
    print(f"Region {region['id']} ({region['name']}):")
    print(f"  - Neurons: {region['neuron_count']}")
    print(f"  - Synapses: {region['synapse_count']}")
    print(f"  - Average weight: {region['average_weight']:.3f}")
```

## 6. Migration Guide

### 6.1 From Old API to New API

If you're using the old NLM API, here's how to migrate:

**Old code:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)
```

**New code:**
```python
import pynlm

# Quick start with validation
brain = pynlm.createQuickBrain(neuronCount=1000)
```

**Old code:**
```python
# Manual world setup
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

**New code:**
```python
# Quick agent creation
brain, world, agent = pynlm.createCompleteAgent(worldSize=25)
```

### 6.2 Backward Compatibility

The enhanced API maintains full backward compatibility:
- All original functions work exactly as before
- New functions are additive
- Original class interfaces unchanged
- No breaking changes to existing code

## 7. API Reference

### 7.1 New Convenience Functions

| Function | Description | Parameters |
|----------|-------------|------------|
| `createQuickBrain()` | Create brain with default config | `neuronCount`, `timestep`, `enablePlasticity`, `enableDevelopment`, `enableCuriosity` |
| `createCompleteAgent()` | Create complete agent with all systems | `name`, `neurons`, `worldSize` |
| `createSimulationPreset()` | Create brain using preset | `presetName` |
| `runValidatedSimulation()` | Run simulation with validation | `agent`, `world`, `steps`, `worldTimestep`, `enableLogging`, `callback` |
| `getBrainStats()` | Get comprehensive brain statistics | `brain` |

### 7.2 Validation Functions

| Function | Description | Parameters |
|----------|-------------|------------|
| `validateConfig()` | Validate configuration parameters | `config` |
| `validateWorldDimensions()` | Validate world dimensions | `width`, `height`, `visionWidth`, `visionHeight` |
| `validateSimulationParams()` | Validate simulation parameters | `steps`, `timestep` |

### 7.3 Preset Names

- `"minimal"`: Quick testing with minimal features
- `"simple"`: Basic learning with essential plasticity
- `"advanced"`: Full-featured simulation with development and curiosity

## 8. Performance Tips

### 8.1 Simulation Optimization

1. **Choose appropriate presets** based on your use case
2. **Use validation** to catch errors early
3. **Monitor performance** with comprehensive statistics
4. **Use callbacks** for custom monitoring

### 8.2 Memory Management

The enhanced API includes automatic memory management:
- Smart pointers prevent memory leaks
- Validation prevents invalid object creation
- Error handling ensures clean resource cleanup

## 9. Getting Help

### 9.1 Documentation Resources

- **API Reference**: This document
- **Examples**: `examples/` directory
- **Source Code**: `src/` directory with detailed comments
- **Community**: GitHub repository with issues and discussions

### 9.2 Troubleshooting

If you encounter issues:

1. **Check parameter validation**: Use the validation functions
2. **Use appropriate presets**: Start with "simple" for debugging
3. **Review error messages**: They provide detailed information
4. **Consult examples**: They demonstrate common use cases

## 10. Future Enhancements

The NLM framework is actively evolving. Future enhancements may include:

- Additional simulation presets
- More sophisticated monitoring tools
- Performance optimization features
- Integration with external systems
- Advanced visualization capabilities

---

*This documentation is continuously updated to reflect the latest enhancements to the NLM framework.*
*For the most recent updates, consult the project repository and release notes.*

## License

MIT License - See LICENSE file for details

---

*Last updated: 2026-10-03*
*NLM Framework Team*