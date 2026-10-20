# NLM Enhanced Python Bindings - Documentation

## Overview

The NLM (Neural Learning Machine) Enhanced Python Bindings provide a comprehensive neuroscience research platform with advanced features for experimentation, analysis, and simulation.

## New Features

### 1. Advanced Plasticity Control

The enhanced plasticity control system allows for fine-grained configuration of different plasticity rules:

```python
import pynlm

# Create enhanced plasticity controller
plasticity = pynlm.create_enhanced_plasticity_controller()

# Enable/disable plasticity rules
plasticity.enable_stdp(True)
plasticity.enable_hebbian(True)
plasticity.enable_structural(True)

# Configure STDP parameters
plasticity.set_stdp_params(learning_rate=0.01, time_constant=20.0, weight_scale=1.0)

# Configure Hebbian parameters
plasticity.set_hebbian_params(learning_rate=0.005, weight_scale=0.5)

# Configure structural plasticity parameters
plasticity.set_structural_params(synaptogenesis_rate=0.0001, pruning_rate=0.00001, min_synapse_weight=0.1)

# Dynamic plasticity rule switching
plasticity.switch_rule("STDP")
print(f"Current rule: {plasticity.get_current_rule()}")

# Plasticity pattern control
plasticity.set_spatial_pattern("grid")
plasticity.set_temporal_pattern("exponential")
plasticity.set_input_specific_pattern("temporal")

# Plasticity statistics
print(f"Total weight change: {plasticity.get_total_weight_change()}")
print(f"Average plasticity strength: {plasticity.get_average_plasticity_strength()}")
plasticity.reset_plasticity_stats()
```

### 2. Neuromodulation System

The neuromodulation system provides fine-grained control over reward, curiosity, novelty, and motivation:

```python
import pynlm

# Create enhanced neuromodulation controller
neuromod = pynlm.create_enhanced_neuromodulation_controller()

# Enable neuromodulation systems
neuromod.enable_reward_modulation(True)
neuromod.enable_curiosity(True)
neuromod.enable_novelty_detection(True)
neuromod.enable_dopamine_system(True)

# Configure neuromodulation parameters
neuromod.set_curiosity_strength(0.5)
neuromod.set_novelty_threshold(0.7)
neuromod.set_reward_scale(1.0)

# Get neuromodulation levels
print(f"Curiosity level: {neuromod.get_curiosity_level()}")
print(f"Novelty level: {neuromod.get_novelty_level()}")
print(f"Dopamine level: {neuromod.get_dopamine_level()}")
print(f"Reward prediction error: {neuromod.get_reward_prediction_error()}")

# Update neuromodulators
neuromod.update_neuromodulators(0.1)
```

### 3. State Management

The state management system provides comprehensive checkpointing and state control:

```python
import pynlm

# Create enhanced state manager
state = pynlm.create_enhanced_state_manager()

# Save and load brain states
state.save_brain_state("brain_state.bin")
state.load_brain_state("brain_state.bin")

# Incremental checkpointing
state.save_incremental("checkpoint1.bin", "episode_1")
state.save_incremental("checkpoint2.bin", "episode_2")

# Checkpoint management
state.create_checkpoint("training_checkpoint")
state.restore_checkpoint("training_checkpoint")

print("Available checkpoints:", state.list_checkpoints())

# State validation
state.validate_state("expected_hash")

# State statistics
print(f"State size: {state.get_state_size()} bytes")
print(f"Compression ratio: {state.get_compression_ratio()}")
print(f"Save/load statistics: {state.get_save_load_statistics()}")
```

### 4. Development System

The development system provides precise control over developmental stages and aging:

```python
import pynlm

# Create enhanced development controller
development = pynlm.create_enhanced_development_controller()

# Set developmental stage
development.set_developmental_stage(pynlm.DevelopmentalStage.CriticalPeriod)
print(f"Current stage: {development.get_developmental_stage()}")
print(f"Stage name: {development.get_stage_name()}")

# Advance development
development.advance_stage()

# Enable critical periods
development.enable_critical_periods(True)

# Set critical period ending
development.set_critical_period_ending(pynlm.DevelopmentalStage.CriticalPeriod, 50.0)
print(f"Critical period progress: {development.get_critical_period_progress()}")
print(f"Critical period active: {development.is_critical_period_active()}")

# Plasticity age curves
development.set_plasticity_age_curve("exponential")
print(f"Plasticity rate: {development.get_plasticity_rate()}")

# Aging simulation
development.enable_aging_simulation(True)
development.set_aging_rate(0.001)
print(f"Aging level: {development.get_aging_level()}")
print(f"Remaining lifespan: {development.get_remaining_lifespan()} steps")

# Development statistics
stats = development.get_development_statistics()
for key, value in stats.items():
    print(f"{key}: {value}")
```

### 5. Advanced Configuration

The advanced configuration system provides hierarchical configuration with presets:

```python
import pynlm

# Create enhanced configuration manager
config = pynlm.create_enhanced_configuration_manager()

# Register presets
preset = pynlm.ConfigurationPreset("cognitive_learning", "Cognitive learning scenario")
preset.default_values = {"brain.neuron_count": "5000", "plasticity.stdp.enable": "true"}
config.register_preset(preset)

# Apply preset
config.apply_preset("cognitive_learning")

# Get available presets
print("Available presets:", config.get_available_presets())

# Set configuration values
config.set_value("brain.neuron_count", "8000", pynlm.ConfigSource.Runtime)
config.set_value("plasticity.stdp.learning_rate", "0.001", pynlm.ConfigSource.Runtime)

# Validate configuration
validation = config.validate_configuration()
if validation.valid:
    print("Configuration is valid")
else:
    print("Validation errors:", validation.errors)

# Compare configurations
other_config = pynlm.create_enhanced_configuration_manager()
differences = config.compare_with(other_config)
print(f"Differences: {differences}")

# Get configuration summary
summary = config.get_summary()
for key, value in summary.items():
    print(f"{key}: {value}")
```

### 6. Experiment Management

The experiment management system provides comprehensive experiment tracking and analysis:

```python
import pynlm

# Create enhanced experiment manager
experiment = pynlm.create_enhanced_experiment_manager()

# Create experiments
experiment.create_experiment("cognitive_learning", {"neuron_count": 5000, "learning_rate": 0.01})
experiment.create_experiment("motor_control", {"neuron_count": 3000, "action_selection": True})

# Set experiment parameters
experiment.add_experiment_parameter("cognitive_learning", "plasticity_rule", "STDP")
experiment.add_experiment_parameter("cognitive_learning", "reward_function", "sparse")

# Get experiment parameters
params = experiment.get_experiment_parameters("cognitive_learning")
print(f"Experiment parameters: {params}")

# Schedule experiments
experiment.schedule_experiment("cognitive_learning", priority=1)
experiment.schedule_experiment("motor_control", priority=2)

# Run experiments (placeholder)
# results = experiment.run_experiment("cognitive_learning", brain_template, world_template, max_steps=1000)

# Collect and export data
# experiment.collect_experiment_data("cognitive_learning")
# experiment.export_experiment_data("cognitive_learning", "results.csv")
```

### 7. Statistical Analysis

The statistical analysis system provides comprehensive analysis tools:

```python
import pynlm

# Create enhanced statistical analyzer
analyzer = pynlm.create_enhanced_statistical_analyzer()

# Time-series analysis
sample_data = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]
print("Time-series analysis results:", analyzer.analyze_time_series(sample_data, 0.1))

# Oscillations detection
print("Oscillation detection:", analyzer.detect_oscillations(sample_data, (0.5, 2.0)))

# Autocorrelation
print("Autocorrelation:", analyzer.compute_autocorrelation(sample_data, max_lag=5))

# Hypothesis testing
# Perform t-test, ANOVA, etc.
print("Statistical hypothesis testing available")

# Pattern recognition
matrix_data = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
print("Correlation matrix:", analyzer.compute_correlation_matrix(matrix_data))

# Principal Component Analysis
print("PCA results:", analyzer.perform_pca(matrix_data, n_components=2))
```

### 8. Visualization and Output

The visualization and output system provides scientific plotting and data export:

```python
import pynlm

# Create enhanced visualization exporter
viz = pynlm.create_enhanced_visualization_exporter()

# Scientific plotting
# viz.plot_neural_activity(brain, "neural_activity.png", "spike_raster")
# viz.plot_learning_curve("experiment_1", "learning_curve.png")
# viz.plot_spike_raster(spike_data, "spike_raster.png")
# viz.plot_weight_matrix(brain, "weight_matrix.png")

# Data export
# data = {"time": [0, 1, 2, 3], "value": [0.1, 0.2, 0.3, 0.4]}
# viz.export_to_csv(data, "results.csv")
# viz.export_to_json(data, "results.json")
# viz.export_to_hdf5(data, "results.h5")

# Interactive visualization
# viz.enable_interactive_plots(True)
# viz.create_interactive_dashboard(brain, world)

# Multi-dimensional analysis
# viz.analyze_multi_dimensional(data, "PCA")
# viz.generate_report(brain, world, "experiment_1")
```

### 9. Performance Monitoring

The performance monitoring system provides benchmarking and profiling:

```python
import pynlm

# Create enhanced performance monitor
perf = pynlm.create_enhanced_performance_monitor()

# Start timers
perf.start_timer("brain_simulation")
perf.start_timer("plasticity_updates")
perf.start_timer("neuromodulation")

# Run simulation
for step in range(100):
    brain.step(step)

# Stop timers and get results
perf.stop_timer("brain_simulation")
perf.stop_timer("plasticity_updates")
perf.stop_timer("neuromodulation")

results = perf.get_timing_results()
print(f"Timing results: {results}")

# Memory tracking
perf.track_memory_usage("brain_memory")
print(f"Memory statistics: {perf.get_memory_statistics()}")

# Performance profiling
perf.profile_neural_computation(brain)
perf.profile_plasticity_updates(plasticity_controller)

# Scalability analysis
neuron_counts = [1000, 5000, 10000, 50000]
results = perf.analyze_scalability(neuron_counts)
print(f"Scalability results: {results}")

# Resource monitoring
perf.monitor_cpu_usage(1.0)
perf.monitor_gpu_usage(1.0)
print(f"Resource usage: {perf.get_resource_usage()}")
```

### 10. Error Handling and Validation

The error handling and validation system provides comprehensive error management:

```python
import pynlm

# Create enhanced error handler
error_handler = pynlm.create_enhanced_error_handler()

# Validate input
data = {"neuron_count": 1000, "learning_rate": 0.01}
validation_rules = {
    "neuron_count": lambda x: x > 0,
    "learning_rate": lambda x: 0.0 <= x <= 1.0
}
validation = error_handler.validate_input(data, validation_rules)
if validation.valid:
    print("Input is valid")
else:
    print(f"Validation errors: {validation.errors}")

# Validate configuration
config = pynlm.create_default_config()
validation = error_handler.validate_configuration(config)
if validation.valid:
    print("Configuration is valid")
else:
    print(f"Configuration errors: {validation.errors}")

# Validate brain parameters
validation = error_handler.validate_brain_parameters(brain)
if validation.valid:
    print("Brain parameters are valid")
else:
    print(f"Brain parameter errors: {validation.errors}")

# Create error objects
error = error_handler.create_error("RuntimeError", "Brain initialization failed")
error_handler.handle_error(error)

# Error statistics
print(f"Error statistics: {error_handler.get_error_statistics()}")

# Recovery actions
error_handler.recover_from_validation_error(error, {"neuron_count": 100, "learning_rate": 0.001})
```

## Backward Compatibility

All existing Python APIs remain unchanged. The enhanced bindings are available through the same module name `pynlm`. Existing code will continue to work without any modifications.

## Getting Started

1. Install the enhanced bindings:
   ```bash
   pip install pynlm
   ```

2. Import the enhanced module:
   ```python
   import pynlm
   ```

3. Create brains and worlds (same as before):
   ```python
   config = pynlm.create_default_config()
   brain = pynlm.createBrain(config)
   brain.initialize()
   
   world = pynlm.createSimpleWorld()
   world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
   ```

4. Create enhanced controllers (new):
   ```python
   # Enhanced features
   plasticity = pynlm.create_enhanced_plasticity_controller()
   neuromod = pynlm.create_enhanced_neuromodulation_controller()
   state = pynlm.create_enhanced_state_manager()
   development = pynlm.create_enhanced_development_controller()
   config = pynlm.create_enhanced_configuration_manager()
   ```

## Example: Complete Enhanced Simulation

Here's a complete example showing all enhanced features:

```python
import pynlm

def run_enhanced_simulation(num_steps=1000):
    # 1. Create enhanced configuration
    config = pynlm.create_advanced_config()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create enhanced controllers
    plasticity = pynlm.create_enhanced_plasticity_controller()
    neuromod = pynlm.create_enhanced_neuromodulation_controller()
    state = pynlm.create_enhanced_state_manager()
    development = pynlm.create_enhanced_development_controller()
    
    # 4. Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 5. Enable enhanced features
    plasticity.enable_stdp(True)
    plasticity.enable_hebbian(True)
    neuromod.enable_reward_modulation(True)
    neuromod.enable_curiosity(True)
    neuromod.enable_novelty_detection(True)
    development.enable_aging_simulation(True)
    
    # 6. Run enhanced simulation
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Update enhanced controllers
        neuromod.update_neuromodulators(0.1)
        development.set_maturation_age(development.get_maturation_progress() + 0.001)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = neuromod.get_reward_prediction_error()
        agent.applyRewardModulation(reward, 0.0)
        
        # Save checkpoint periodically
        if step % 100 == 0:
            state.create_checkpoint(f"checkpoint_{step}")
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {neuromod.getCuriosityLevel():.3f}")
            print(f"  Novelty: {neuromod.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    # 7. Save final state
    state.save_brain_state("final_brain_state.bin")
    
    print("Enhanced simulation completed!")

# Run the enhanced simulation
run_enhanced_simulation(1000)
```

## Migration Guide

### From Old to Enhanced API

1. **Plasticity Control**: Use `create_enhanced_plasticity_controller()` instead of direct brain configuration
2. **Neuromodulation**: Use `create_enhanced_neuromodulation_controller()` for fine-grained control
3. **State Management**: Use `create_enhanced_state_manager()` for advanced checkpointing
4. **Development**: Use `create_enhanced_development_controller()` for developmental stage control
5. **Configuration**: Use `create_enhanced_configuration_manager()` for hierarchical configuration

### Performance Considerations

- Enhanced controllers have minimal overhead
- All new features are optional and can be used independently
- Performance monitoring helps identify bottlenecks
- State management provides efficient checkpointing with compression

### Troubleshooting

If you encounter issues with the enhanced bindings:

1. **Import errors**: Ensure you're using the enhanced version
2. **Missing features**: Check if the feature is available in your NLM version
3. **Performance issues**: Use the performance monitor to identify bottlenecks
4. **State validation**: Use the error handler to validate configuration

## Version Information

- Version: 2.0.0
- API Version: 2.0.0
- Backward Compatible: true
- Installation: `pip install pynlm (>=2.0.0)`