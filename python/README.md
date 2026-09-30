# NLM Enhanced Python Bindings

## Overview

This module provides an enhanced, Pythonic API for the NLM (Neural Learning Machine) neural simulation framework while maintaining full backward compatibility with the existing bindings.

## Key Features

### 1. Intuitive Pythonic Naming
- `setup_brain()` instead of `createBrain()`
- `create_world()` instead of `createSimpleWorld()`
- `create_agent()` instead of `createAgentBrain()`
- `setup_reward_modulation()` instead of `enableRewardModulation()`
- `setup_curiosity()` instead of `enableCuriosity()`
- `setup_development()` instead of `enableDevelopment()`
- `run_step()` instead of `step()`
- `decode_action()` instead of `decodeMotorCommand()`

### 2. Configuration Management
- Schema validation with type checking
- Default value management
- Value range validation
- Configuration inheritance
- Validation error reporting

### 3. Batch Processing
- Sequential batch execution
- Batch result aggregation
- Batch progress monitoring
- Resume/cancel capabilities

### 4. Enhanced Monitoring
- Real-time statistics collection
- Time series tracking
- Performance metrics
- Custom metric collection

### 5. Learning Control
- Granular control over plasticity rules
- Neuromodulation parameter configuration
- Development system control
- Learning rate adjustments

### 6. Brain Region Access
- Direct access to specific brain regions
- Advanced neuron filtering and querying
- Population statistics
- Batch operations on neuron populations

### 7. Checkpoint Management
- Automatic versioning
- Incremental and full checkpoint support
- Checkpoint validation and verification
- Backup and restore capabilities

### 8. Development Tracking
- Developmental stage monitoring
- Plasticity changes tracking
- Learning milestone management
- Growth metrics

### 9. Error Handling
- Custom exception types
- Comprehensive error messages
- Validation error reporting
- Graceful error recovery

### 10. Type Safety
- Complete type hints
- Runtime type validation
- Schema validation
- Documentation generation support

## API Reference

### Core Classes

#### `EnhancedNLMBindings`
Main bindings class providing the complete Python API.

**Methods:**
- `setup_brain(config=None)`: Setup brain with enhanced configuration
- `create_world(config=None)`: Create world with optional configuration
- `create_agent(brain)`: Create agent brain interface
- `run_step(brain, world, action=None)`: Run a single simulation step
- `batch_run(steps, brain, world, initial_action=None)`: Run multiple steps in batch
- `get_simulation_stats()`: Get comprehensive simulation statistics
- `get_development_summary()`: Get development tracking information

#### `NLMConfigManager`
Enhanced configuration management with validation.

**Methods:**
- `update(config_dict)`: Update configuration with validation
- `get(key, default=None)`: Get configuration value
- `get_all()`: Get all configuration values
- `validate()`: Validate configuration and return errors
- `is_valid()`: Check if configuration is valid

#### `NLMStatisticsCollector`
Real-time statistics collection and analysis.

**Methods:**
- `record_metric(name, value, timestamp=None)`: Record a metric value
- `get_metric(name, default=None)`: Get latest metric value
- `get_time_series(name, start_time=None)`: Get time series data
- `take_snapshot(name="default")`: Take a snapshot of current metrics
- `get_summary_stats()`: Get summary statistics

#### `NLMBatchProcessor`
Batch processing for simulations.

**Methods:**
- `create_batch(steps=100, batch_name="unnamed", callbacks=None)`: Create a new batch
- `run_batch(batch_id, config=None)`: Run a batch
- `get_batch_results(batch_id)`: Get results for a specific batch
- `get_all_batch_results()`: Get results for all batches

#### `NMLearningController`
Enhanced control over learning subsystems.

**Methods:**
- `set_plasticity_rule(rule_type, learning_rate, weight_decay, eligibility_window)`: Configure plasticity rule
- `get_plasticity_rules()`: Get all configured plasticity rules
- `configure_neuromodulation(dopamine, serotonin, acetylcholine, noradrenaline, curiosity_threshold)`: Configure neuromodulation
- `get_neuromodulation_params()`: Get current neuromodulation parameters
- `configure_development(developmental_stage, plasticity_rate_modifier, structural_plasticity_enabled, critical_period_enabled)`: Configure development
- `get_development_params()`: Get current development parameters

#### `NLMCheckpointManager`
Advanced checkpoint management with automatic versioning.

**Methods:**
- `save(brain, name=None, description=None, compress=False)`: Save a checkpoint
- `load(name, validate=True)`: Load a checkpoint
- `list_checkpoints()`: List all available checkpoints
- `delete_checkpoint(name)`: Delete a checkpoint

#### `NLMBrainRegionAccessor`
Direct access to brain regions and populations.

**Methods:**
- `get_region(region_id)`: Get a brain region by ID
- `find_neurons(region_id=None, neuron_type=None, min_activity=None, max_activity=None, limit=None)`: Find neurons by criteria
- `get_population_stats(region_id=None, population_id=None)`: Get population statistics

#### `NLMDevelopmentTracker`
Tools for monitoring developmental stages and progress.

**Methods:**
- `get_current_stage()`: Get current developmental stage information
- `get_development_history(start_time=None, end_time=None)`: Get developmental history
- `get_milestones()`: Get developmental milestones
- `add_milestone(name, description, target_stage, target_metric, target_value)`: Add a developmental milestone
- `update_milestone(milestone_name, current_value)`: Update milestone progress

### Convenience Functions

#### Enhanced API
- `setup_brain(config=None)`: Setup brain with enhanced configuration
- `create_world(config=None)`: Create world with optional configuration
- `create_agent(brain)`: Create agent brain interface
- `run_batch(steps, brain, world, initial_action=None)`: Run batch simulation
- `get_simulation_stats()`: Get simulation statistics

#### Backward Compatibility
- `createBrain(config=None)`: Alias for `setup_brain()`
- `createSimpleWorld()`: Alias for `create_world()`
- `createAgentBrain(brain)`: Alias for `create_agent(brain)`
- `enableRewardModulation(enable=True)`: Alias for `setup_reward_modulation(enable)`
- `enableCuriosity(enable=True)`: Alias for `setup_curiosity(enable)`
- `enableDevelopment(enable=True)`: Alias for `setup_development(enable)`
- `step(brain, world, action=None)`: Alias for `run_step(brain, world, action)`
- `decodeMotorCommand(agent_brain)`: Alias for `decode_action(agent_brain)`

### Exceptions

- `NLMBindingsError`: Base exception for enhanced bindings errors
- `NLMValidationError`: Raised when configuration validation fails
- `NLMCheckpointError`: Raised when checkpoint operations fail
- `NLMStatisticsError`: Raised when statistics collection fails

## Usage Examples

### Beginner Example
```python
from nlm import enhanced_bindings as nlm

# Simple setup
brain = nlm.setup_brain({
    "region_count": 3,
    "neuron_count_per_region": 500,
    "learning_rate": 0.05
})

world = nlm.create_world({
    "width": 50,
    "height": 50,
    "max_energy": 200
})

agent = nlm.create_agent(brain)
nlm.run_batch(100, brain, world)
```

### Advanced Example
```python
from nlm import enhanced_bindings as nlm

# Configure advanced learning
brain = nlm.setup_brain({
    "region_count": 5,
    "neuron_count_per_region": 1000,
    "plasticity_rule": "BCM",
    "learning_rate": 0.02
})

# Configure learning controller
brain._learning_controller.set_plasticity_rule(
    rule_type="STDP",
    learning_rate=0.05,
    weight_decay=0.001
)

brain._learning_controller.configure_neuromodulation(
    dopamine=1.5,
    serotonin=1.2,
    curiosity_threshold=0.7
)

# Explore brain regions
neurons = brain.find_neurons(neuron_type="Excitatory", min_activity=0.5, limit=10)
print(f"Found {len(neurons)} high-activity neurons")

# Monitoring and checkpoints
stats = nlm.get_simulation_stats()
brain.save_checkpoint("my_checkpoint", "Advanced simulation state")
```

### Batch Processing Example
```python
# Run multiple simulations with different configurations
for i in range(3):
    config = {
        "region_count": i + 1,
        "neuron_count_per_region": 1000 * (i + 1),
        "learning_rate": 0.01 * (i + 1)
    }
    
    brain = nlm.setup_brain(config)
    world = nlm.create_world({"width": 100, "height": 100})
    
    results = nlm.run_batch(50, brain, world)
    print(f"Batch {i+1} completed with {len(results)} steps")
```

### Development Tracking Example
```python
# Monitor developmental progress
brain = nlm.setup_brain({"development": True})
world = nlm.create_world()
agent = nlm.create_agent(brain)

# Add milestones
agent.add_milestone(
    name="high_plasticity",
    description="Achieve plasticity rate > 1.0",
    target_stage="Maturation",
    target_metric="plasticity_rate",
    target_value=1.0
)

# Run simulation
nlm.run_batch(100, brain, world)

# Check milestone
milestone_achieved = agent.update_milestone("high_plasticity", 1.2)
if milestone_achieved:
    print("✓ High plasticity milestone achieved!")
```

## Installation

The enhanced bindings are part of the NLM Python package. To use them:

```bash
pip install pynlm
```

## Building from Source

If you need to build the enhanced bindings from source:

```bash
cd /path/to/nlm/python
python setup.py build_ext --inplace
```

## Testing

Run the examples to see the enhanced bindings in action:

```bash
python -m examples.nlm_enhanced_examples
```

## Documentation

For more detailed documentation, see:
- `README.md` in the root directory
- `HOW_TO_USE.md` for usage instructions
- `easy_usage.md` for quick start guide

## Migration Guide

### From Original Bindings

The enhanced bindings maintain full backward compatibility. If you have existing code using the original API, it will continue to work with deprecation warnings:

```python
# Original API (still works with warnings)
from nlm import createBrain, createSimpleWorld, enableRewardModulation

brain = createBrain()  # Now calls setup_brain() with deprecation warning
world = createSimpleWorld()  # Now calls create_world() with deprecation warning
enableRewardModulation(True)  # Now calls setup_reward_modulation(True) with deprecation warning

# New API (recommended)
from nlm import enhanced_bindings as nlm

brain = nlm.setup_brain()
world = nlm.create_world()
nlm.setup_reward_modulation(True)
```

## Contributing

### Adding New Features

To add new features to the enhanced bindings:

1. Extend the `EnhancedNLMBindings` class with new methods
2. Add new classes for specialized functionality
3. Update the `__all__` list in the module
4. Add examples to `examples/nlm_enhanced_examples.py`
5. Update documentation

### Modifying Existing Features

When modifying existing features:

1. Maintain backward compatibility
2. Issue deprecation warnings for changed methods
3. Add comprehensive tests
4. Update documentation

## License

This project is licensed under the MIT License.

## Copyright

© 2024 NLM Project Authors

## Contact

For questions, issues, or contributions:
- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Documentation: https://nlm.readthedocs.io