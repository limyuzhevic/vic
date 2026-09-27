# NLM Migration Guide
# ====================

Comprehensive migration guide for transitioning from older NLM versions to the current API,
including breaking changes, migration strategies, and upgrade procedures.

## Overview

The NLM Python API has undergone significant improvements across multiple versions, with substantial changes introduced in recent releases. This guide provides comprehensive migration strategies for users transitioning from older versions to the current API (version 1.0.0).

### Key Migration Changes

1. **API Restructuring**: From low-level C++ bindings to high-level class-based interfaces
2. **Convenience Functions**: Addition of high-level convenience functions for common operations
3. **Error Handling**: Improved exception hierarchy with custom exception classes
4. **Configuration System**: Enhanced configuration management with validation
5. **Training Module**: Complete overhaul of training infrastructure with new patterns and callbacks
6. **Performance**: Significant performance improvements and optimization

## Version Migration Paths

### From NLM v0.x to NLM v1.0.0

#### Low-Level API Migration

**Before (NLM v0.x)**:
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Get stats
spikes = brain.getTotalSpikeCount()
rate = brain.getAverageFiringRate()
```

**After (NLM v1.0.0)**:
```python
import pynlm as nlm

# Create brain (simplified)
brain = nlm.NLMBrain(neuron_count=1000, complexity=0.7)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Get stats (simplified interface)
spikes = brain.get_total_spikes()
rate = brain.get_average_firing_rate()
```

#### High-Level API Introduction

NLM v1.0.0 introduces high-level API classes that provide more intuitive interfaces:

```python
import pynlm as nlm

# High-level experiment setup
experiment = nlm.NLMExperiment.create_standard(world_size=(20, 20))

# Run experiment with comprehensive monitoring
results = experiment.run(num_steps=1000)

# Access high-level results
print(f"Final firing rate: {results.metrics['final_firing_rate']:.3f}")
print(f"Total spikes: {results.metrics['final_spikes']}")
```

## Migration Strategies

### 1. Incremental Migration

#### Step 1: Add High-Level API Support

```python
import pynlm
import pynlm as nlm  # Also imports high-level API

# Existing low-level code continues to work
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# New high-level code can be added gradually
brain_v2 = nlm.NLMBrain(neuron_count=1000, complexity=0.7)
brain_v2.initialize()
```

#### Step 2: Mixed Usage

```python
import pynlm as nlm

# Use high-level classes for new development
brain = nlm.NLMBrain(neuron_count=1000)

# Use low-level classes for specific operations
config = pynlm.createDefaultConfig()
# Configure low-level parameters
```

#### Step 3: Complete Migration

```python
import pynlm as nlm

# Migrate entire project to high-level API
# Replace all low-level pynlm calls with nlm equivalents
# Update code to use new class-based interfaces
```

### 2. Training Module Migration

#### Before (NLM v0.x Training)

**Old training approach**:
```python
import pynlm as pynlm

# Create trainer
trainer = pynlm.TrainingCore()

# Configure training
config = pynlm.TrainingConfig()
config.maxEpisodes = 100
config.learningRate = 0.001

# Run training
trainer.trainAgent(brain, world, agent, config)
```

#### After (NLM v1.0.0 Training)

**New training approach**:
```python
import pynlm as nlm
from nlm.training import TrainingCore, TrainingConfig, SimpleTrainingCallback

# Create trainer
trainer = TrainingCore()

# Configure training with advanced options
config = TrainingConfig()
config.maxEpisodes = 100
config.learningRate = 0.001
config.explorationRate = 0.1
config.rewardConfig.type = nlm.RewardConfig.Type.WEIGHTED

# Create callback
callback = SimpleTrainingCallback()

# Run training with monitoring
trainer.trainAgent(brain, world, agent, config, [callback])
```

### 3. Error Handling Migration

#### Before (NLM v0.x)

**Old error handling**:
```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except Exception as e:
    print(f"Error: {e}")
```

#### After (NLM v1.0.0)

**New error handling**:
```python
try:
    brain = nlm.NLMBrain(neuron_count=1000, complexity=0.7)
    brain.initialize()
except nlm.ValidationError as e:
    print(f"Validation error: {e}")
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.NLMError as e:
    print(f"NLM error: {e}")
```

### 4. Configuration Migration

#### Before (NLM v0.x)

**Old configuration**:
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
```

#### After (NLM v1.0.0)

**New configuration**:
```python
import pynlm as nlm

# Global configuration
nlm.configure(
    default_neuron_count=1000,
    default_complexity=0.5,
    default_world_size=(10, 10)
)

# Object configuration
brain = nlm.NLMBrain(
    neuron_count=1000,
    complexity=0.7,
    config={
        "brain": {
            "excitatory_neurons": 800,
            "inhibitory_neurons": 200
        }
    }
)
```

## Feature Migration

### 1. Brain Interface Migration

| Old Feature | New Feature | Migration Strategy |
|-------------|
| `brain.getTotalNeuronCount()` | `brain.get_neuron_count()` | Direct replacement |
| `brain.getTotalSynapseCount()` | `brain.get_total_synapses()` | Direct replacement |
| `brain.getFiringNeuronCount()` | `brain.get_firing_neurons()` | Direct replacement |
| `brain.getAverageFiringRate()` | `brain.get_average_firing_rate()` | Direct replacement |
| `brain.getTotalSpikeCount()` | `brain.get_total_spikes()` | Direct replacement |
| `brain.receiveSensoryInput(percept)` | `brain.receive_sensory_input(percept)` | Direct replacement |
| `brain.produceAction()` | `brain.produce_action()` | Direct replacement |

### 2. World Interface Migration

| Old Feature | New Feature | Migration Strategy |
|------------|-------------|-------------------|
| `world.update(0.1)` | `world.update(0.1)` | Direct replacement |
| `world.getSensoryPercept()` | `world.get_sensory_percept()` | Direct replacement |
| `world.setAgentStart(x, y)` | `world.set_agent_start(x, y)` | Direct replacement |
| `world.applyMotorCommand(action, time)` | `world.apply_motor_command(action, time)` | Direct replacement |

### 3. Agent Interface Migration

| Old Feature | New Feature | Migration Strategy |
|------------|-------------|-------------------|
| `agent.processSensoryInput(percept)` | `agent.process_sensory_input(percept)` | Direct replacement |
| `agent.decodeMotorCommand()` | `agent.decode_motor_command()` | Direct replacement |
| `agent.applyRewardModulation(reward, predicted)` | `agent.apply_reward_modulation(reward, predicted)` | Direct replacement |

## Migration Checklist

### Phase 1: Preparation

- [ ] Review existing code and identify all `pynlm` calls
- [ ] Create test projects for migration validation
- [ ] Set up development environment with both old and new APIs
- [ ] Document all existing configurations and parameters

### Phase 2: Initial Migration

- [ ] Replace `import pynlm` with `import pynlm as nlm`
- [ ] Add high-level API imports where needed
- [ ] Update class and method names
- [ ] Maintain compatibility with existing code

### Phase 3: Feature Migration

- [ ] Migrate training configurations
- [ ] Update error handling code
- [ ] Convert convenience function calls
- [ ] Update configuration management

### Phase 4: Validation

- [ ] Run comprehensive tests on migrated code
- [ ] Compare performance between old and new implementations
- [ ] Validate error handling and edge cases
- [ ] Update documentation and examples

### Phase 5: Completion

- [ ] Remove deprecated low-level API imports where appropriate
- [ ] Update project configuration files
- [ ] Document migration procedures for team members
- [ ] Create migration guide for future reference

## Common Migration Issues and Solutions

### Issue 1: Method Name Conflicts

**Problem**: Method name conflicts between old and new APIs.

**Solution**: Use explicit imports to avoid conflicts:
```python
import pynlm as pynlm_old  # Keep old API for reference
import pynlm as nlm  # New high-level API
```

### Issue 2: Configuration Migration

**Problem**: Configuration parameter names have changed.

**Solution**: Map old configuration names to new ones:
```python
# Old configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# New configuration
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
# Same parameter name, different interface
```

### Issue 3: Training Loop Migration

**Problem**: Training loop structure has changed significantly.

**Solution**: Use new training patterns:
```python
# Old approach
trainer = pynlm.TrainingCore()
trainer.trainAgent(brain, world, agent, config)

# New approach
from nlm.training import CommonTrainingPatterns
CommonTrainingPatterns.runReinforcementLearningPattern(
    trainer, brain, world, agent, config, callback
)
```

## Support and Resources

### Migration Support

1. **Documentation**: Comprehensive migration guides and API references
2. **Examples**: Updated code examples demonstrating migration patterns
3. **Testing**: Validation scripts for comparing old and new implementations
4. **Support**: Community forums and technical support for migration questions

### Resources

- **API Reference**: Complete documentation of all high-level and low-level APIs
- **Training Guide**: Comprehensive guide to training module features
- **Performance Guide**: Performance optimization and benchmarking
- **Examples**: Working examples of both old and new APIs
- **Migration Checklist**: Step-by-step migration procedures

## Future Compatibility

### Forward Compatibility

NLM v1.0.0 maintains backward compatibility through:

1. **Module-level aliasing**: `import pynlm as nlm` imports both old and new APIs
2. **Feature preservation**: Core functionality preserved with improved interfaces
3. **Gradual migration**: Users can migrate incrementally without breaking existing code

### Future Development

Future versions will continue to evolve the API while maintaining:

- **Backward compatibility** for critical features
- **Performance improvements** across all interfaces
- **Enhanced usability** through better abstractions
- **Expanded functionality** with new training patterns and features

## Conclusion

Migration from NLM v0.x to NLM v1.0.0 offers significant benefits:

1. **Improved Developer Experience**: Intuitive, high-level interfaces
2. **Enhanced Functionality**: Advanced training features and patterns
3. **Better Error Handling**: Comprehensive exception hierarchy
4. **Performance**: Optimized implementations and better resource management
5. **Future-Proof**: Extensible architecture for future enhancements

The migration process, while requiring some code changes, provides a clear path to more powerful and maintainable NLM applications.

---

**Migration Success Tips**:

1. **Start Small**: Migrate one module or feature at a time
2. **Test Thoroughly**: Validate functionality after each migration step
3. **Document Changes**: Keep track of all migration decisions
4. **Gradual Migration**: Maintain compatibility during transition period
5. **Training**: Ensure team members are trained on new APIs

**Need Help?**:

- Check the NLM documentation for detailed migration guides
- Review example code for best practices
- Contact support for specific migration challenges
- Participate in community forums for shared experiences

Migration to NLM v1.0.0 represents a significant improvement in developer experience and capabilities while maintaining the power and flexibility that made NLM v0.x successful.

---

**Version**: 1.0.0 **Last Updated**: 2026-09-27 **License**: MIT