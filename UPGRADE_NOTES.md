# NLM Upgrade Notes
# ==================

Summary of breaking changes, migration requirements, and upgrade procedures for NLM version 1.0.0.

## Overview

This document provides concise upgrade notes for migrating from NLM versions prior to 1.0.0 to the current release. It focuses on the most impactful changes and provides clear guidance for a smooth transition.

## Breaking Changes Summary

### 1. API Restructuring (Most Significant)

**Change**: Complete overhaul of the Python API from low-level C++ bindings to high-level class-based interfaces.

**Impact**: 
- Method names changed from `snake_case` to `snake_case` (with underscores)
- Class interfaces redesigned with better encapsulation
- New high-level APIs with enhanced functionality

**Migration Examples**:

| Old Method | New Method | Notes |
|------------|------------|-------|
| `brain.getTotalNeuronCount()` | `brain.get_neuron_count()` | Direct replacement |
| `brain.getTotalSynapseCount()` | `brain.get_total_synapses()` | Direct replacement |
| `brain.getFiringNeuronCount()` | `brain.get_firing_neurons()` | Direct replacement |
| `brain.getAverageFiringRate()` | `brain.get_average_firing_rate()` | Direct replacement |
| `brain.getTotalSpikeCount()` | `brain.get_total_spikes()` | Direct replacement |
| `brain.receiveSensoryInput(percept)` | `brain.receive_sensory_input(percept)` | Direct replacement |
| `brain.produceAction()` | `brain.produce_action()` | Direct replacement |

### 2. Convenience Functions (New)

**Change**: Introduction of high-level convenience functions for common operations.

**New Functions**:
- `create_simple_brain()` - One-line brain creation
- `create_standard_experiment()` - Complete experiment setup
- `quick_test()` - Quick system validation
- `train_agent()` - High-level training loop
- `evaluate_performance()` - Performance assessment
- `optimize_hyperparameters()` - Automated parameter tuning

### 3. Training Module (Complete Overhaul)

**Change**: Complete redesign of the training module with new patterns and callbacks.

**Key Changes**:
- New `TrainingCore` class with enhanced training loop
- New `TrainingConfig` class with advanced configuration options
- New callback system for training monitoring
- New training patterns (RL, SL, US, Hybrid)
- New experiment tracking and persistence

### 4. Error Handling (Improved)

**Change**: Enhanced exception hierarchy with custom exception classes.

**New Exception Hierarchy**:
```python
class NLMError(Exception):
    Base exception for all NLM errors

class ValidationError(NLMError):
    Invalid input parameters

class ConfigurationError(NLMError):
    Invalid configuration values
```

### 5. Configuration System (Enhanced)

**Change**: Improved configuration management with validation and global settings.

**Key Features**:
- Global configuration management
- Parameter validation
- Per-object configuration options
- Better error messages

## Migration Strategies

### Strategy 1: Gradual Migration (Recommended)

**Approach**: Migrate incrementally to minimize disruption.

**Steps**:

1. **Phase 1**: Add high-level API support alongside existing code
   ```python
   import pynlm
   import pynlm as nlm  # Also imports high-level API
   
   # Existing low-level code continues to work
   # New high-level code can be added gradually
   ```

2. **Phase 2**: Update new development to use high-level API
   ```python
   # New projects use high-level API
   brain = nlm.NLMBrain(neuron_count=1000, complexity=0.7)
   ```

3. **Phase 3**: Complete migration
   ```python
   # Gradually migrate existing code
   # Update methods and interfaces
   # Validate functionality after each change
   ```

### Strategy 2: Big Bang Migration

**Approach**: Migrate all at once for maximum consistency.

**Steps**:

1. **Backup**: Ensure all code is backed up
2. **Prepare**: Update all method calls and interfaces
3. **Test**: Run comprehensive tests on migrated code
4. **Deploy**: Replace old API with new API
5. **Validate**: Ensure all functionality works correctly

## Upgrade Checklist

### Pre-Upgrade Tasks

- [ ] Review existing codebase for all `pynlm` calls
- [ ] Identify all configuration files and parameters
- [ ] Set up development environment with both APIs
- [ ] Create test projects for migration validation
- [ ] Document all existing training configurations

### Migration Tasks

- [ ] Replace `import pynlm` with `import pynlm as nlm`
- [ ] Update all method calls to new interface
- [ ] Convert convenience function calls to new functions
- [ ] Update error handling code
- [ ] Migrate training configurations
- [ ] Update configuration management

### Post-Upgrade Tasks

- [ ] Run comprehensive tests on migrated code
- [ ] Compare performance between old and new implementations
- [ ] Validate error handling and edge cases
- [ ] Update documentation and examples
- [ ] Train team members on new APIs
- [ ] Set up monitoring and alerting for production systems

## Upgrade Examples

### Example 1: Brain Interface Migration

**Before**:
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get statistics
spikes = brain.getTotalSpikeCount()
rate = brain.getAverageFiringRate()
```

**After**:
```python
import pynlm as nlm

# Create brain (simplified)
brain = nlm.NLMBrain(neuron_count=1000, complexity=0.7)
brain.initialize()

# Get statistics (simplified interface)
spikes = brain.get_total_spikes()
rate = brain.get_average_firing_rate()
```

### Example 2: Training Migration

**Before**:
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

**After**:
```python
import pynlm as nlm
from nlm.training import TrainingCore, TrainingConfig

# Create trainer
trainer = TrainingCore()

# Configure training with advanced options
config = TrainingConfig()
config.maxEpisodes = 100
config.learningRate = 0.001
config.explorationRate = 0.1
config.rewardConfig.type = nlm.RewardConfig.Type.WEIGHTED

# Create callback
callback = nlm.training.SimpleTrainingCallback()

# Run training with monitoring
trainer.trainAgent(brain, world, agent, config, [callback])
```

### Example 3: Configuration Migration

**Before**:
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
```

**After**:
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

## Common Issues and Solutions

### Issue 1: Method Name Conflicts

**Problem**: Method name conflicts between old and new APIs.

**Solution**: Use explicit imports:
```python
import pynlm as pynlm_old  # Keep old API for reference
import pynlm as nlm  # New high-level API
```

### Issue 2: Configuration Migration

**Problem**: Configuration parameter names changed.

**Solution**: Map old configuration names:
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

**Problem**: Training loop structure changed significantly.

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

### Getting Help

- **Documentation**: Comprehensive migration guides and API references
- **Examples**: Updated code examples demonstrating migration patterns
- **Testing**: Validation scripts for comparing old and new implementations
- **Community**: Forums and technical support for migration questions

### Additional Resources

- **API Reference**: Complete documentation of all high-level and low-level APIs
- **Training Guide**: Comprehensive guide to training module features
- **Performance Guide**: Performance optimization and benchmarking
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