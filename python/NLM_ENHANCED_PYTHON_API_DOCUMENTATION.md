# NLM Enhanced Python API Documentation

## Overview

This document provides comprehensive documentation for the enhanced NLM Python API. The enhanced API transforms NLM from a basic educational tool into a comprehensive, production-ready research platform capable of supporting cutting-edge research in artificial intelligence and computational neuroscience.

## Key Improvements

### 1. Complete Subsystem Access
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Prediction System**: Sensory prediction and error computation
- **Cognition Systems**: Neural planner, concept formation, attentional selection
- **Development System**: Developmental stage management and plasticity modulation
- **Neuromodulation Systems**: Dopamine, curiosity, novelty detection
- **Plasticity Systems**: STDP, Hebbian learning, structural plasticity

### 2. Type-Safe Configuration Management
- Integer, float, boolean, and string type-safe getters
- Configuration validation with range checking
- Schema validation
- Preset configuration management
- Configuration cloning and merging

### 3. Enhanced Error Handling
- Python-specific exception hierarchy
- Configuration-specific exceptions
- Brain operation exceptions
- World operation exceptions
- Simulation exceptions

### 4. Advanced Agent Operations
- Enhanced AgentBrain with batch operations
- Complete episode management
- Learning system control
- Detailed statistics and monitoring

### 5. Performance Optimizations
- Batch step operations
- Batch current injection
- Batch configuration operations
- Enhanced statistics collection

## API Reference

### Brain Class

#### Enhanced Core Methods

| Method | Description | Pythonic Equivalent |
|--------|-------------|-------------------|
| `getWorkingMemory()` | Get working memory system | `brain.getWorkingMemory()` |
| `getEpisodicMemory()` | Get episodic memory system | `brain.getEpisodicMemory()` |
| `getAssociativeMemory()` | Get associative memory system | `brain.getAssociativeMemory()` |
| `getPredictionSystem()` | Get prediction system | `brain.getPredictionSystem()` |
| `getPlanner()` | Get neural planner | `brain.getPlanner()` |
| `getConceptFormation()` | Get concept formation system | `brain.getConceptFormation()` |
| `getAttention()` | Get attentional selection | `brain.getAttention()` |
| `getDevelopmentSystem()` | Get development system | `brain.getDevelopmentSystem()` |
| `getDopamine()` | Get dopamine system | `brain.getDopamine()` |
| `getCuriosity()` | Get curiosity system | `brain.getCuriosity()` |
| `getNovelty()` | Get novelty system | `brain.getNovelty()` |
| `getPredictionErrorSignal()` | Get prediction error signal | `brain.getPredictionErrorSignal()` |
| `getSTDP()` | Get STDP system | `brain.getSTDP()` |
| `getHebbian()` | Get Hebbian system | `brain.getHebbian()` |
| `getStructuralPlasticity()` | Get structural plasticity | `brain.getStructuralPlasticity()` |
| `getRandomGenerator()` | Get random generator | `brain.getRandomGenerator()` |

#### Enhanced Statistics

| Method | Description |
|--------|-------------|
| `getDetailedStats()` | Get comprehensive brain statistics |
| `getSystemStatus()` | Get detailed brain system status |
| `getFiringStats()` | Get firing-related statistics |
| `getDevelopmentStats()` | Get development system statistics |

#### Batch Operations

| Method | Description |
|--------|-------------|
| `step_range(start, end, start_time=0.0)` | Perform multiple steps efficiently |
| `inject_currents(neurons, current)` | Inject current into multiple neurons |
| `step_range(start, end, start_time=0.0)` | Convenience wrapper for batch steps |

#### State Management

| Method | Description |
|--------|-------------|
| `clear()` | Clear brain state (alias for reset) |
| `reset_and_initialize()` | Reset and reinitialize brain |
| `configure_neurons(count)` | Configure number of neurons |

#### Configuration Access

| Method | Description |
|--------|-------------|
| `get_int(key)` | Get configuration as integer |
| `get_float(key)` | Get configuration as float |
| `get_bool(key)` | Get configuration as boolean |
| `get_string(key)` | Get configuration as string |
| `get_int_or_default(key, default)` | Get with default value |
| `get_float_or_default(key, default)` | Get with default value |
| `get_bool_or_default(key, default)` | Get with default value |
| `get_string_or_default(key, default)` | Get with default value |
| `validate_range(key, min, max)` | Validate value is within range |
| `validate_positive(key)` | Validate value is positive |
| `validate_non_negative(key)` | Validate value is non-negative |

### AgentBrain Class

#### Enhanced AgentBrain Methods

| Method | Description |
|--------|-------------|
| `enable_all_learning(enable)` | Enable/disable all learning systems |
| `get_learning_status()` | Get complete learning system status |
| `run_episode(world, steps)` | Run complete agent episode in world |
| `get_detailed_stats()` | Get detailed agent statistics |

#### Learning System Control

| Method | Description |
|--------|-------------|
| `isRewardModulationEnabled()` | Check if reward modulation is enabled |
| `isStructuralPlasticityEnabled()` | Check if structural plasticity is enabled |
| `isDevelopmentEnabled()` | Check if development is enabled |
| `isCuriosityEnabled()` | Check if curiosity is enabled |

### SimpleWorld Class

#### Enhanced SimpleWorld Methods

| Method | Description |
|--------|-------------|
| `configure_advanced(width, height, visionWidth, visionHeight, maxEnergy, energyDecayRate, randomSeed)` | Advanced configuration |
| `configure_from_dict(config)` | Configure from dictionary |
| `add_resource(x, y, value=1.0)` | Add resource object |
| `add_hazard(x, y, damage=10.0)` | Add hazard object |
| `add_wall(x, y, width=1.0, height=1.0)` | Add wall object |
| `get_objects_by_type(type)` | Get objects by type |
| `set_agent_velocity(vx, vy)` | Set agent velocity |
| `set_agent_orientation(orientation)` | Set agent orientation |
| `get_world_state()` | Get complete world state |
| `get_environment_stats()` | Get environment statistics |
| `run_simulation(total_time, timestep)` | Run simulation for specified duration |
| `step_until_time(target_time)` | Advance simulation to specific time |
| `update_multiple(timesteps)` | Update world with multiple timesteps |
| `add_multiple_objects(objects)` | Add multiple objects at once |
| `create_sample_environment()` | Create sample environment with various objects |
| `clear_environment()` | Clear environment and reset |
| `get_environment_summary()` | Get environment summary |

### Configuration Class

#### Type-Safe Configuration Access

| Method | Description |
|--------|-------------|
| `get_int(key)` | Get configuration as integer |
| `get_int64(key)` | Get configuration as 64-bit integer |
| `get_float(key)` | Get configuration as float |
| `get_bool(key)` | Get configuration as boolean |
| `get_string(key)` | Get configuration as string |
| `get_int_list(key)` | Get configuration as integer list |
| `get_float_list(key)` | Get configuration as float list |
| `get_string_list(key)` | Get configuration as string list |
| `get_int_or_default(key, default)` | Get with default value |
| `get_float_or_default(key, default)` | Get with default value |
| `get_bool_or_default(key, default)` | Get with default value |
| `get_string_or_default(key, default)` | Get with default value |

#### Configuration Validation

| Method | Description |
|--------|-------------|
| `validate_range(key, min, max)` | Validate value is within range |
| `validate_positive(key)` | Validate value is positive |
| `validate_non_negative(key)` | Validate value is non-negative |
| `validate_schema(schema)` | Validate configuration against schema |

#### Configuration Management

| Method | Description |
|--------|-------------|
| `set_int(key, value)` | Set configuration as integer |
| `set_float(key, value)` | Set configuration as float |
| `set_bool(key, value)` | Set configuration as boolean |
| `set_string(key, value)` | Set configuration as string |
| `merge_from_dict(dict)` | Merge configuration from dictionary |
| `to_dict()` | Convert configuration to dictionary |
| `clone()` | Create copy of configuration |
| `apply_preset(preset_name)` | Apply predefined configuration preset |
| `get_preset_list()` | Get list of available presets |

#### Preset Configurations

| Preset | Description |
|--------|-------------|
| `default` | Default brain configuration |
| `minimal` | Minimal configuration |
| `learning` | Learning-focused configuration |
| `agent` | Agent-focused configuration |
| `development` | Development-focused configuration |

### Exception Classes

#### NLM Exception Hierarchy

| Class | Parent | Description |
|-------|--------|-------------|
| `NLMException` | `RuntimeError` | Base NLM exception |
| `NLMConfigurationError` | `NLMException` | Configuration-related error |
| `NLMBrainError` | `NLMException` | Brain-related error |
| `NLMWorldError` | `NLMException` | World-related error |
| `NLMArgumentError` | `NLMException` | Argument-related error |
| `NLMSimulationError` | `NLMException` | Simulation-related error |

### Utility Functions

| Function | Description |
|----------|-------------|
| `createDefaultConfig()` | Create default configuration |
| `createBrain(config)` | Create brain with configuration |
| `createSimpleWorld()` | Create simple world |
| `createAgentBrain(brain)` | Create agent brain interface |
| `createExampleBrain()` | Create configured example brain |
| `createExampleAgentSimulation()` | Create complete example simulation |
| `createSimpleLearningExample()` | Create learning example |
| `version()` | Get NLM version information |
| `getExampleCode()` | Get example Python code |

## Usage Examples

### Basic Enhanced Brain Usage

```python
import pynlm

# Create enhanced brain
config = pynlm.createDefaultConfig()
config.set_int("brain.neuron_count", 2000)
config.set_float("brain.synapse_density", 0.15)

brain = pynlm.createBrain(config)
brain.initialize()

# Access subsystems
working_memory = brain.getWorkingMemory()
prediction = brain.getPredictionSystem()
development = brain.getDevelopmentSystem()

# Get statistics
stats = brain.getDetailedStats()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Firing rate: {stats['average_firing_rate']:.3f}")

# Run enhanced simulation
brain.step_range(0, 100)
```

### Complete Agent Simulation

```python
import pynlm

# Create complete agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure_advanced(20, 20, 8, 8, 100.0, 0.02, 42)
world.reset()

agent.initialize(world)
agent.enable_all_learning(True)

# Run episode
steps = agent.run_episode(world, 200)

# Get statistics
stats = agent.get_detailed_stats()
print(f"Curiosity: {stats['curiosity_level']:.3f}")
print(f"Novelty: {stats['novelty_level']:.3f}")
```

### Configuration Management

```python
import pynlm

# Type-safe configuration
config = pynlm.createDefaultConfig()
config.set_int("brain.neuron_count", 1000)
config.set_float("brain.synapse_density", 0.1)
config.set_bool("plasticity.stdp.enable", True)

# Type-safe access
neuron_count = config.get_int("brain.neuron_count")
synapse_density = config.get_float("brain.synapse_density")
stdp_enabled = config.get_bool("plasticity.stdp.enable")

# Configuration validation
config.validate_range("brain.neuron_count", 100, 5000)
config.validate_positive("brain.synapse_density")

# Preset configuration
learning_config = pynlm.createDefaultConfig()
# Automatically applies learning optimizations
```

### Error Handling

```python
import pynlm

try:
    # Create brain and perform operations
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Access subsystems (may raise exceptions)
    working_memory = brain.getWorkingMemory()
    prediction = brain.getPredictionSystem()
    
except pynlm.NLMConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.NLMBrainError as e:
    print(f"Brain error: {e}")
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
```

### Batch Operations

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Batch step operations
brain.step_range(0, 1000)  # Steps 0-1000

# Batch current injection
neurons = [0, 1, 2, 3, 4]  # Neuron IDs
brain.inject_currents(neurons, 50.0)

# Batch statistics
for batch in range(0, 100, 20):
    brain.step_range(batch, batch + 19)
    stats = brain.getDetailedStats()
    print(f"Batch {batch}: {stats['total_spikes']} spikes")
```

### Enhanced World Simulation

```python
import pynlm

# Create enhanced world
world = pynlm.createSimpleWorld()
world.configure_advanced(30, 30, 10, 10, 200.0, 0.015, 12345)
world.create_sample_environment()

# Create agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enable_all_learning(True)

# Run enhanced simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply neuromodulation
    curiosity = agent.getCuriosityLevel()
    agent.applyRewardModulation(curiosity, 0.0)
    
    agent.updateDevelopment(0.1)
```

## Performance Considerations

### Batch Operations

1. **Batch Step Operations**: Use `step_range()` instead of individual `step()` calls for multiple steps
2. **Batch Current Injection**: Use `inject_currents()` to inject current into multiple neurons simultaneously
3. **Batch Statistics**: Collect statistics after batch operations instead of individual calls

### Memory Management

1. **Configuration Cloning**: Use `clone()` for configuration copies instead of creating new instances
2. **Subsystem Access**: Subsystems are cached and can be accessed multiple times without performance penalty

### Error Handling

1. **Exception Types**: Catch specific exception types for better error handling
2. **Configuration Validation**: Validate configuration early to avoid runtime errors
3. **Graceful Degradation**: Handle missing subsystems gracefully

## Migration Guide

### From Basic to Enhanced API

#### Basic API (Old)

```python
import pynlm

# Basic usage
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Limited subsystem access
for step in range(100):
    brain.step(step)
    
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

#### Enhanced API (New)

```python
import pynlm

# Enhanced usage
config = pynlm.createDefaultConfig()
config.set_int("brain.neuron_count", 2000)

brain = pynlm.createBrain(config)
brain.initialize()

# Complete subsystem access
working_memory = brain.getWorkingMemory()
prediction = brain.getPredictionSystem()
development = brain.getDevelopmentSystem()

# Enhanced simulation
brain.step_range(0, 100)

# Type-safe statistics
stats = brain.getDetailedStats()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Firing rate: {stats['average_firing_rate']:.3f}")
print(f"System status: {brain.getSystemStatus()}")
```

### Configuration Migration

#### Basic Configuration (Old)

```python
config = pynlm.createDefaultConfig()
# Manual parameter setting
config.loadFromFile("config.json")
```

#### Enhanced Configuration (New)

```python
config = pynlm.createDefaultConfig()

# Type-safe setting
config.set_int("brain.neuron_count", 1000)
config.set_float("brain.synapse_density", 0.1)
config.set_bool("plasticity.stdp.enable", True)

# Presets
learning_config = pynlm.createDefaultConfig()  # Automatically configures for learning
```

## Best Practices

### 1. Configuration Management

- Always validate configuration values before use
- Use type-safe getters to avoid type conversion errors
- Use preset configurations for common use cases
- Validate schema before runtime operations

### 2. Error Handling

- Catch specific exception types for targeted error handling
- Validate input parameters before operations
- Use try-catch blocks for critical operations
- Provide meaningful error messages to users

### 3. Performance Optimization

- Use batch operations for multiple steps
- Cache frequently accessed subsystems
- Collect statistics in batches
- Avoid redundant configuration lookups

### 4. Memory Management

- Use configuration cloning for copies
- Access subsystems once and reuse
- Monitor memory usage during long simulations
- Clean up resources when done

### 5. Simulation Best Practices

- Initialize brain before any operations
- Use proper timestep values for stability
- Monitor simulation progress regularly
- Save checkpoints for long-running simulations

## Troubleshooting

### Common Issues and Solutions

#### Issue: Brain not initializing properly

**Symptoms**: Brain methods fail or return incorrect values

**Solutions**:
1. Check configuration values
2. Validate required parameters
3. Ensure proper initialization order
4. Check for missing dependencies

#### Issue: Performance degradation

**Symptoms**: Simulation slows down over time

**Solutions**:
1. Use batch operations
2. Monitor memory usage
3. Clean up unused resources
4. Optimize configuration access

#### Issue: Configuration validation failures

**Symptoms**: Configuration errors during validation

**Solutions**:
1. Check parameter types
2. Validate range constraints
3. Use appropriate presets
4. Check configuration schema

#### Issue: Agent behavior unexpected

**Symptoms**: Agent produces unexpected actions

**Solutions**:
1. Check neuromodulation settings
2. Validate learning parameters
3. Monitor agent statistics
4. Use detailed logging

### Error Messages

| Error Message | Cause | Solution |
|---------------|-------|----------|
| "Key 'xxx' does not exist" | Missing configuration key | Add required key or use default |
| "Value for key 'xxx' is out of range" | Invalid configuration value | Set value within valid range |
| "Configuration validation failed" | Schema validation error | Check configuration schema |
| "Brain not initialized" | Brain not initialized | Call brain.initialize() before operations |
| "Invalid neuron ID" | Invalid neuron identifier | Use valid neuron IDs from brain |

## Future Enhancements

### Planned Improvements

1. **Enhanced Visualization**: Integration with plotting libraries for simulation visualization
2. **Parallel Processing**: Multi-threading and GPU acceleration for large-scale simulations
3. **Advanced Learning**: Integration with modern machine learning frameworks
4. **Cloud Integration**: Cloud-based simulation and collaboration tools
5. **API Documentation**: Interactive API documentation and examples

### Research Integration

1. ** Neuroscience Models**: Integration with detailed neuroscience models
2. **Cognitive Architectures**: Support for various cognitive architectures
3. **Developmental Psychology**: Advanced developmental modeling
4. **Social Learning**: Multi-agent social learning simulations

## Conclusion

The enhanced NLM Python API provides a comprehensive, production-ready platform for neural simulation research. With complete subsystem access, type-safe configuration management, enhanced error handling, and performance optimizations, the enhanced API supports a wide range of research applications from basic neural dynamics to advanced cognitive modeling.

The enhanced API maintains backward compatibility with the basic API while providing powerful new features for advanced users. Migration from the basic to enhanced API is straightforward and provides immediate performance and functionality improvements.

## References

1. NLM Project Documentation
2. Python Bindings Best Practices
3. Neural Network Simulation Frameworks
4. Machine Learning Research Papers

---

*This documentation is continuously updated to reflect new features and improvements in the NLM Python API.*