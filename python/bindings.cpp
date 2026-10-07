# Advanced NLM Python Bindings Documentation

# Overview

This document provides comprehensive documentation for the enhanced NLM Python bindings,
which add advanced features for expert users while maintaining backward compatibility
with the existing pynlm API.

## Table of Contents

1. [Introduction](#introduction)
2. [Basic Usage (Backward Compatibility)](#basic-usage-backward-compatibility)
3. [AdvancedBrain Class](#advancedbrain-class)
4. [SimulationController Class](#simulationcontroller-class)
5. [PerformanceProfiler Class](#performanceprofiler-class)
6. [PluginSystem Class](#pluginsystem-class)
7. [ErrorHandler Class](#errorhandler-class)
8. [ConfigBuilder Class](#configbuilder-class)
9. [CheckpointManager Class](#checkpointmanager-class)
10. [Context Managers](#context-managers)
11. [AdvancedNLM Framework](#advancednlm-framework)
12. [Integration Methods](#integration-methods)
13. [Plasticity Rules](#plasticity-rules)
14. [Visualization Tools](#visualization-tools)
15. [Export/Import Capabilities](#exportimport-capabilities)
16. [Performance Optimization](#performance-optimization)
17. [Memory Management](#memory-management)
18. [Plugin Architecture](#plugin-architecture)
19. [Examples](#examples)
20. [API Reference](#api-reference)
21. [Troubleshooting](#troubleshooting)

# Introduction

The NLM (Neural Learning Machine) framework provides powerful neural simulation capabilities
through both C++ and Python interfaces. This documentation covers the enhanced Python bindings
that add advanced features for expert users.

## Key Features

- **Backward Compatibility**: All existing pynlm API remains unchanged
- **AdvancedBrain**: Extends Brain with custom neurons, advanced plasticity, and integration methods
- **SimulationController**: Manages batch processing and event-driven simulation
- **PerformanceProfiler**: Comprehensive performance monitoring and metrics
- **PluginSystem**: Extensibility framework for custom components
- **ErrorHandler**: Advanced error handling with logging and recovery
- **ConfigBuilder**: Advanced configuration management with templates
- **CheckpointManager**: Enhanced checkpointing with metadata and incremental backups
- **Context Managers**: Safe execution with automatic rollback on error
- **AdvancedNLM**: Complete framework with all features enabled

# Basic Usage (Backward Compatibility)

The enhanced bindings maintain full backward compatibility with the original pynlm API.

## Basic Example

```python
import pynlm

# Create basic brain
config = pynlm.create_default_config()
brain = pynlm.createBrain(config)

# Initialize
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Save state
brain.save("brain_state.json")
```

## Working with Existing Features

The existing API continues to work exactly as before:

```python
import pynlm

# Basic NLM operations
brain = pynlm.createBrain()
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# All existing methods are available
brain.initialize()
brain.step(0)
brain.logStatus()
```

# AdvancedBrain Class

The `AdvancedBrain` class extends the basic `Brain` class with enhanced features.

## Initialization

```python
import pynlm
from pynlm_advanced import AdvancedBrain, NeuronConfig, AdvancedNeuronType, CustomNeuronRegistry

# Create custom neuron registry
registry = CustomNeuronRegistry()

# Create configuration
config = pynlm.create_default_config()

# Create advanced brain
advanced_brain = AdvancedBrain(config, registry)
```

## Custom Neuron Types

```python
# Define neuron configuration
pyramidal_config = NeuronConfig(
    name="Pyramidal Neuron",
    neuron_type=AdvancedNeuronType.PYRAMIDAL,
    threshold=-50.0,
    resting_potential=-65.0,
    leak_conductance=10.0,
    custom_params={"population_size": 100}
)

# Add custom neuron type
advanced_brain.add_neuron_type("pyramidal", AdvancedNeuronType.PYRAMIDAL, pyramidal_config)

# Create neuron population
neuron_ids = advanced_brain.create_neuron_population("pyramidal", 50)
print(f"Created {len(neuron_ids)} pyramidal neurons")
```

## Integration Methods

```python
# Configure integration method
advanced_brain.configure_integration("exponential_euler")
print(f"Integration method: {advanced_brain.get_integration_method()}")

# Available methods: "euler", "rk4", "exponential_euler", "adaptive"
```

## Plasticity Rules

```python
# Set custom plasticity rule
rule_config = {
    'learning_rate': 0.01,
    'time_constant': 20.0,
    'weight_range': [-1.0, 1.0]
}

advanced_brain.set_plasticity_rule("custom_stdp", rule_config)
```

## Advanced Learning

```python
# Enable advanced learning algorithm
advanced_brain.enable_advanced_learning("bayesian_optimization")

# Configure algorithm parameters
advanced_brain.configure_advanced_learning({
    'n_iterations': 100,
    'model': 'random_forest',
    'acquisition_function': 'ei'
})
```

## Monitoring and Metrics

```python
# Enable monitoring
advanced_brain.enable_monitoring(True)

# Perform simulation with monitoring
for step in range(100):
    metrics = advanced_brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {advanced_brain.get_performance_metrics()}")
```

# SimulationController Class

The `SimulationController` class manages batch processing and event-driven simulation.

## Basic Usage

```python
import pynlm

# Create simulation controller
controller = pynlm.createSimulationController()

# Add brains
brain1 = pynlm.createBrain()
brain2 = pynlm.createBrain()
controller.add_brain(brain1)
controller.add_brain(brain2)

# Run batch simulation
controller.run_batch(steps=1000, timestep=0.001, mode="sequential")

print(f"Simulation completed: {controller.get_metrics()}")
```

## Event-Driven Simulation

```python
# Create controller with event-driven mode
controller = pynlm.createSimulationController()
controller.set_mode("event_driven")

# Configure event-driven behavior
controller.set_spike_driven(True)
controller.set_plasticity_driven(True)

# Run event-driven simulation
controller.run_batch(steps=500, timestep=0.001, mode="event_driven")
```

## Parallel Processing

```python
# Create controller with parallel mode
controller = pynlm.createSimulationController()
controller.set_mode("parallel")

# Run parallel simulation
controller.run_batch(steps=1000, timestep=0.001, mode="parallel")

print(f"Parallel simulation completed in {controller.get_metrics()['elapsed_time']:.2f}s")
```

## Pause and Resume

```python
controller = pynlm.createSimulationController()

# Start simulation in background
import threading
def run_simulation():
    controller.run_batch(steps=1000, timestep=0.001)

simulation_thread = threading.Thread(target=run_simulation)
simulation_thread.start()

# Pause after 100 steps
for i in range(100):
    pass

controller.pause()
print("Simulation paused")

# Resume
controller.resume()
print("Simulation resumed")

# Wait for completion
simulation_thread.join()
```

# PerformanceProfiler Class

The `PerformanceProfiler` class provides comprehensive performance monitoring.

## Basic Usage

```python
import pynlm
from pynlm_advanced import PerformanceProfiler

# Create performance profiler
profiler = PerformanceProfiler()

# Start monitoring
profiler.start_monitoring("simulation_1")

# Run simulation with monitoring
for step in range(1000):
    # ... simulation code ...
    profiler.get_monitor("simulation_1").add_step()

# Stop monitoring
profiler.stop_monitoring("simulation_1")

# Get metrics
metrics = profiler.get_all_metrics()
for name, monitor in metrics.items():
    print(f"{name}: {monitor}")
```

## Memory Monitoring

```python
import pynlm

profiler = pynlm.createPerformanceProfiler()

# Monitor memory usage
for step in range(1000):
    # ... simulation code ...
    memory_mb = profiler.get_memory_usage()
    print(f"Memory usage: {memory_mb:.1f} MB")
    
    growth_rate = profiler.get_growth_rate()
    print(f"Growth rate: {growth_rate:.4f}")
```

## Recording Custom Metrics

```python
profiler = pynlm.createPerformanceProfiler()

# Record custom metrics
for step in range(1000):
    # ... simulation code ...
    
    # Record various metrics
    profiler.record_metric("spike_rate", 0.05)
    profiler.record_metric("firing_rate", 12.5, "Hz")
    profiler.record_metric("energy_consumption", 0.75, "J")
```

## Alerts and Notifications

```python
profiler = pynlm.createPerformanceProfiler()

# Set up alerts
profiler.set_alert("memory_usage", 1000.0, "shutdown")
profiler.set_alert("growth_rate", 0.5, "log_warning")
profiler.set_alert("spike_rate", 100.0, "alert_admin")

# Monitor metrics
for step in range(1000):
    metrics = profiler.get_all_metrics()
    alerts = profiler.get_alerts()
    
    if alerts:
        for metric, alert_info in alerts.items():
            if alert_info.get('triggered', False):
                print(f"ALERT: {metric} threshold exceeded!")
```

# PluginSystem Class

The `PluginSystem` class provides extensibility through custom components.

## Basic Usage

```python
import pynlm

# Create plugin system
plugin_system = pynlm.createPluginSystem()

# Define a custom plasticity rule
def custom_plasticity_factory():
    # Create and return custom plasticity rule instance
    pass

# Register the plugin
plugin_system.register_plugin("my_custom_rule", custom_plasticity_factory)

# Create plugin instance
rule = plugin_system.create_plugin("my_custom_rule")
```

## Plugin Management

```python
# Check if plugin exists
if plugin_system.has_plugin("my_custom_rule"):
    print("Plugin is registered")

# Get all plugin names
plugin_names = plugin_system.get_plugin_names()
print(f"Registered plugins: {plugin_names}")

# Enable/disable plugin
plugin_system.enable_plugin("my_custom_rule", True)
print(f"Plugin enabled: {plugin_system.is_plugin_enabled('my_custom_rule')}")

# Unregister plugin
plugin_system.unregister_plugin("my_custom_rule")
```

# ErrorHandler Class

The `ErrorHandler` class provides advanced error handling and logging.

## Basic Usage

```python
import pynlm
from pynlm_advanced import ErrorHandler

# Create error handler
error_handler = ErrorHandler()

# Set up error handlers
error_handler.add_handler(lambda error: print(f"ERROR: {error['message']}"))

# Log errors with categories
error_handler.log_error("NEURON", "Neuron not found", 101)
error_handler.log_error("MEMORY", "Out of memory", 102)
error_handler.log_error("SIMULATION", "Invalid timestep", 103)

# Get error statistics
stats = error_handler.get_statistics()
print(f"Total errors: {stats['total_errors']}")
print(f"Error by level: {stats['level_counts']}")
print(f"Error by category: {stats['categories']}")
```

## Error Levels

```python
error_handler = pynlm.createErrorHandler()

# Log different levels of messages
error_handler.debug("Debug message for troubleshooting")
error_handler.info("Informational message")
error_handler.warning("Warning message")
error_handler.error("Error message")
error_handler.critical("Critical error message")
```

## Error Export/Import

```python
error_handler = pynlm.createErrorHandler()

# Export errors to file
error_handler.export_errors("error_log.json")

# Clear errors
error_handler.clear_errors()
```

# ConfigBuilder Class

The `ConfigBuilder` class provides advanced configuration management with templates.

## Basic Usage

```python
import pynlm

# Create config builder
config_builder = pynlm.createConfigBuilder()

# Get built-in templates
fast_config = config_builder.get_fast_learning_template()
stable_config = config_builder.get_stable_learning_template()
balanced_config = config_builder.get_balanced_template()

# Apply template to configuration
import pynlm.core as pynlm_core
config = pynlm_core.Config()
config_builder.apply_template(config, "fast_learning")
```

## Creating Custom Templates

```python
custom_template = {
    'neuron_count': 2000,
    'plasticity': {
        'stdp': {'enable': True, 'learning_rate': 0.02},
        'hebbian': {'enable': True},
        'reward_modulated': {'enable': True, 'gain': 0.5}
    },
    'development': {'critical_period': 50, 'maturation_rate': 0.02},
    'simulation': {'timestep': 0.001},
    'visualization': {'enabled': True, 'real_time': True},
    'export': {'format': 'json', 'checkpoint_interval': 100}
}

config_builder.create_template('my_template', custom_template)

# Use the template
config_builder.apply_template(config, 'my_template')
```

## Template Validation

```python
# Validate templates
if config_builder.validate_template('fast_learning'):
    print("Fast learning template is valid")

if config_builder.validate_template('my_template'):
    print("Custom template is valid")
```

# CheckpointManager Class

The `CheckpointManager` class provides enhanced checkpointing with metadata.

## Basic Usage

```python
import pynlm

# Create checkpoint manager
checkpoint_manager = pynlm.createCheckpointManager()

# Create a brain for checkpointing
brain = pynlm.createBrain()

# Create checkpoint
if checkpoint_manager.create_checkpoint("simulation_1", brain):
    print("Checkpoint created successfully")

# List checkpoints
checkpoints = checkpoint_manager.list_checkpoints()
for checkpoint in checkpoints:
    print(f"Checkpoint: {checkpoint['name']} (Neurons: {checkpoint['neuron_count']})")
```

## Checkpoint Operations

```python
# Load checkpoint
if checkpoint_manager.load_checkpoint("simulation_1", brain):
    print("Checkpoint loaded successfully")

# Get checkpoint info
checkpoint_info = checkpoint_manager.get_checkpoint_info("simulation_1")
print(f"Checkpoint details: {checkpoint_info}")

# Get delta between checkpoints
delta = checkpoint_manager.get_checkpoint_delta("simulation_1", "simulation_2")
print(f"Delta size: {delta.get('delta_size', 'unknown')}")
```

## Checkpoint Cleanup

```python
# Clean up old checkpoints (older than 24 hours)
checkpoint_manager.cleanup_old_checkpoints(max_age_hours=24.0)

print("Checkpoint cleanup completed")
```

# Context Managers

Context managers provide safe execution with automatic rollback.

## Basic Usage

```python
import pynlm
from pynlm_advanced import simulation_context

brain = pynlm.createBrain()

# Use context manager for safe execution
with pynlm.createSimulationContext(brain, config) as (brain_ctx, config_ctx):
    # Safe execution block
    for step in range(100):
        brain_ctx.step(step)
        # If an error occurs here, the context manager will restore the original config
```

## Nested Context Managers

```python
brain = pynlm.createBrain()
config1 = pynlm.create_default_config()
config2 = pynlm.create_default_config()

with pynlm.createSimulationContext(brain, config1) as (brain1, config1):
    # First context
    for step in range(50):
        brain1.step(step)
    
    with pynlm.createSimulationContext(brain, config2) as (brain2, config2):
        # Nested context
        for step in range(50, 100):
            brain2.step(step)
    
    # After nested context, config1 is restored
```

# AdvancedNLM Framework

The `AdvancedNLM` class provides a complete framework with all features enabled.

## Basic Usage

```python
import pynlm

# Create advanced NLM framework
advanced_nlm = pynlm.createAdvancedNLM()

# Initialize with configuration
advanced_nlm.initialize({
    'neuron_count': 5000,
    'plasticity': {'stdp': {'enable': True}},
    'visualization': {'enabled': True}
})

# Run simulation
advanced_nlm.run(steps=1000, timestep=0.001)

# Get metrics
metrics = advanced_nlm.get_current_metrics()
print(f"Performance metrics: {metrics}")
```

## Subsystems Access

```python
advanced_nlm = pynlm.createAdvancedNLM()

# Access various subsystems
brain = advanced_nlm.get_brain()
registry = advanced_nlm.get_neuron_registry()
templates = advanced_nlm.get_configuration_templates()
monitor = advanced_nlm.get_memory_monitor()
manager = advanced_nlm.get_memory_manager()
```

## Advanced Features

```python
# Enable visualization
advanced_nlm.start_visualization()

# Save checkpoint
if advanced_nlm.save_checkpoint("simulation_1"):
    print("Checkpoint saved")

# Export to file
advanced_nlm.export_to_file("results.json")

# Get status summary
status = advanced_nlm.get_status_summary()
print(f"Status: {status}")
```

# Integration Methods

Integration methods define how neuron dynamics are computed.

## Available Methods

```python
import pynlm

# Available integration methods
methods = [
    "euler", "rk4", "exponential_euler", "adaptive",
    "crank_nicolson", "forward_euler", "backward_euler",
    "implicit_euler", "trapezoidal_rule", "leapfrog",
    "velocity_verlet", "stochastic_euler", "milstein",
    "strang_splitting", "operator_splitting", "spectral_method"
]

# Set integration method
advanced_brain.configure_integration("exponential_euler")
```

# Plasticity Rules

Plasticity rules define how synaptic weights change over time.

## Built-in Plasticity Rules

```python
import pynlm

# Available plasticity rules
plasticity_rules = [
    "stdp", "hebbian", "reward_modulated_stdp",
    "bcn", "homeostatic", "predictive_coding"
]

# Set plasticity rule
advanced_brain.set_plasticity_rule("stdp", {
    'learning_rate': 0.01,
    'tau_plus': 20.0,
    'tau_minus': 20.0
})
```

# Visualization Tools

Visualization tools provide various ways to view and analyze neural data.

## Visualization Types

```python
import pynlm

# Neuron activity plotting
advanced_nlm.start_visualization()

# Network topology visualization
advanced_nlm.get_network_topology_visualizer().show()

# Learning curve analysis
advanced_nlm.get_learning_curve_analyzer().show()

# Statistical visualization
advanced_nlm.get_statistical_visualizer().show()

# Save visualizations
advanced_nlm.save_visualization("results.png")
```

# Export/Import Capabilities

Export/import capabilities allow saving and loading neural data in various formats.

## Export Formats

```python
advanced_nlm = pynlm.createAdvancedNLM()

# Export to JSON format
if advanced_nlm.export_to_file("brain.json", "json"):
    print("Exported to JSON")

# Export to binary format
if advanced_nlm.export_to_file("brain.bin", "binary"):
    print("Exported to binary")

# Export to compressed format
if advanced_nlm.export_to_file("brain.lz4", "lz4"):
    print("Exported to LZ4")

# Export specific data types
# advanced_nlm.export_neurons("neurons.json")
# advanced_nlm.export_synapses("synapses.json")
# advanced_nlm.export_plasticity("plasticity.json")
```

# Performance Optimization

Performance optimization techniques improve simulation efficiency.

## Parallel Processing

```python
# Enable parallel processing
advanced_nlm.get_parallel_processor().set_config({
    'neuron_update': True,
    'synapse_update': True,
    'spike_propagation': True,
    'plasticity_update': True,
    'num_threads': 8,
    'enable_load_balancing': True,
    'enable_work_stealing': True
})
```

## Memory Management

```python
# Configure memory management
advanced_nlm.get_memory_manager().set_config({
    'neuron_pool_size': 10000,
    'spike_queue_size': 1000000,
    'checkpoint_interval': 1000,
    'max_memory_gb': 16.0,
    'gc_threshold': 0.8,
    'compression_level': 'balanced'
})
```

# Memory Management

Memory management tools optimize memory usage.

## Memory Monitoring

```python
# Monitor memory usage
monitor = advanced_nlm.get_memory_monitor()
monitor.start_monitoring(advanced_nlm.get_brain())

while monitor.is_monitoring():
    memory_usage = monitor.get_memory_usage()
    print(f"Memory usage: {memory_usage:.1f} MB")
    time.sleep(1)
```

## Garbage Collection

```python
# Trigger garbage collection
advanced_nlm.trigger_gc()

# Configure garbage collection
advanced_nlm.get_memory_manager().enableGarbageCollection(True)
```

# Plugin Architecture

The plugin architecture allows extending NLM with custom components.

## Plugin Development

```python
# Define a plugin factory function
def my_plugin_factory():
    class MyPlugin:
        def __init__(self):
            self.name = "My Custom Plugin"
            self.version = "1.0"
        
        def process(self, data):
            # Custom processing logic
            return data
    
    return MyPlugin()

# Register the plugin
plugin_system = pynlm.createPluginSystem()
plugin_system.register_plugin("my_plugin", my_plugin_factory)

# Create plugin instance
plugin = plugin_system.create_plugin("my_plugin")
```

# Examples

This section provides comprehensive examples of using the advanced features.

## Example 1: Advanced Brain with Custom Neurons

```python
import pynlm
from pynlm_advanced import AdvancedBrain, NeuronConfig, AdvancedNeuronType, CustomNeuronRegistry

# Create custom neuron registry
registry = CustomNeuronRegistry()

# Create advanced brain with custom neuron types
config = pynlm.create_default_config()
advanced_brain = AdvancedBrain(config, registry)

# Define and add custom neuron types
for neuron_type_name in ['pyramidal', 'basket', 'sensory', 'motor']:
    neuron_config = NeuronConfig(
        name=f"{neuron_type_name.title()} Neuron",
        neuron_type=getattr(AdvancedNeuronType, neuron_type_name.upper()),
        threshold=-50.0,
        resting_potential=-65.0,
        custom_params={"population_size": 100}
    )
    advanced_brain.add_neuron_type(neuron_type_name, neuron_config)

# Create simulation with custom neurons
for step in range(100):
    metrics = advanced_brain.step(step)
    print(f"Step {step}: {advanced_brain.get_performance_metrics()}")
```

## Example 2: Batch Simulation with Performance Monitoring

```python
import pynlm
import time

# Create simulation controller
controller = pynlm.createSimulationController()

# Create brains
brains = [pynlm.createBrain() for _ in range(3)]
for i, brain in enumerate(brains):
    controller.add_brain(brain)

# Create performance profiler
profiler = pynlm.createPerformanceProfiler()

# Start monitoring
for i, brain in enumerate(brains):
    profiler.start_monitoring(f"brain_{i}")

# Run batch simulation
start_time = time.time()
controller.run_batch(steps=500, timestep=0.001, mode="parallel")
elapsed_time = time.time() - start_time

# Stop monitoring
for i in range(len(brains)):
    profiler.stop_monitoring(f"brain_{i}")

# Print results
print(f"Simulation completed in {elapsed_time:.2f}s")
print(f"Controller metrics: {controller.get_metrics()}")
print(f"Performance metrics: {profiler.get_all_metrics()}")
```

## Example 3: AdvancedNLM with All Features

```python
import pynlm

# Create advanced NLM framework
advanced_nlm = pynlm.createAdvancedNLM()

# Initialize with comprehensive configuration
advanced_nlm.initialize({
    'neuron_count': 10000,
    'plasticity': {
        'stdp': {'enable': True, 'learning_rate': 0.02},
        'hebbian': {'enable': True},
        'reward_modulated': {'enable': True, 'gain': 0.5},
        'homeostatic': {'enable': True}
    },
    'development': {'critical_period': 100, 'maturation_rate': 0.01},
    'simulation': {'timestep': 0.001},
    'visualization': {'enabled': True, 'real_time': True},
    'memory': {'gc': {'enabled': True}, 'pools': {'enabled': True}},
    'parallel': {'processing': {'enabled': True}}
})

# Create custom neurons and populations
neuron_ids = advanced_nlm.create_neurons('pyramidal', 1000)
print(f"Created {len(neuron_ids)} pyramidal neurons")

# Run simulation
advanced_nlm.run(steps=1000, timestep=0.001)

# Save checkpoint and export results
advanced_nlm.save_checkpoint("simulation_1")
advanced_nlm.export_to_file("results.json")

# Get comprehensive metrics
metrics = advanced_nlm.get_current_metrics()
status = advanced_nlm.get_status_summary()

print(f"Simulation metrics: {metrics}")
print(f"Status summary: {status}")
```

## Example 4: Plugin System Usage

```python
import pynlm

# Create plugin system
plugin_system = pynlm.createPluginSystem()

# Define custom plasticity rule plugin
def reward_modulated_stdp_factory():
    class RewardModulatedSTDP:
        def __init__(self):
            self.learning_rate = 0.01
            self.reward_gain = 0.1
            self.time_constant = 20.0
        
        def update(self, synapse, pre_spikes, post_spikes, dt):
            # Custom STDP implementation with reward modulation
            pass
        
        def apply_weight_change(self, synapse, delta):
            # Apply weight change
            pass
    
    return RewardModulatedSTDP()

# Register the plugin
plugin_system.register_plugin("reward_modulated_stdp", reward_modulated_stdp_factory)

# Create plugin instance
rule = plugin_system.create_plugin("reward_modulated_stdp")

# Use the plugin
rule.update(synapse, pre_spikes, post_spikes, 0.001)
```

## Example 5: Error Handling and Recovery

```python
import pynlm
from pynlm_advanced import simulation_context

# Create brain and config
brain = pynlm.createBrain()
config = pynlm.create_default_config()

# Create error handler
error_handler = pynlm.createErrorHandler()

# Set up error handling
error_handler.add_handler(lambda error: print(f"ERROR: {error['message']}"))

# Use context manager for safe execution
with pynlm.createSimulationContext(brain, config) as (brain_ctx, config_ctx):
    try:
        for step in range(100):
            # Safe simulation code
            brain_ctx.step(step)
            
            # Simulate an error condition
            if step == 50:
                error_handler.log_error("SIMULATION", "Simulated error condition", 500)
                raise RuntimeError("Simulated runtime error")
                
    except Exception as e:
        print(f"Simulation error caught: {e}")
        # Context manager automatically restores original config

# After context manager, brain should be back to original state
print("Simulation completed with error handling")
```

# API Reference

This section provides the API reference for all classes and functions.

## Class API Reference

### AdvancedBrain
- `__init__(config=None, neuron_registry=None)`
- `add_neuron_type(name, neuron_type, config)`
- `create_neuron_population(neuron_type, count, params=None)`
- `configure_integration(method)`
- `set_plasticity_rule(name, rule_config)`
- `enable_advanced_learning(algorithm, params=None)`
- `get_integration_method()`
- `get_available_learning_algorithms()`
- `get_neuron_registry()`
- `get_neuron_populations()`
- `get_plasticity_rules()`
- `enable_monitoring(enable=True)`
- `is_monitoring_enabled()`
- `get_performance_metrics()`
- `step(step, time=0.0)`

### SimulationController
- `__init__()`
- `add_brain(brain)`
- `remove_brain(brain)`
- `run_batch(steps, timestep=0.001, mode="sequential")`
- `pause()`
- `resume()`
- `stop()`
- `is_running()`
- `is_paused()`
- `set_mode(mode)`
- `get_mode()`
- `set_spike_driven(enabled)`
- `set_synapse_driven(enabled)`
- `set_plasticity_driven(enabled)`
- `get_metrics()`
- `reset_metrics()`

### PerformanceProfiler
- `__init__()`
- `start_monitoring(name, brain=None)`
- `stop_monitoring(name)`
- `is_monitoring(name)`
- `get_memory_usage()`
- `get_growth_rate()`
- `get_leak_rate()`
- `get_allocated_neurons()`
- `get_allocated_spikes()`
- `record_metric(name, value, unit="")`
- `get_metric(name)`
- `get_all_metrics()`
- `get_metrics_summary()`
- `set_alert(metric, threshold, action)`
- `remove_alert(metric)`
- `get_alerts()`
- `enable_console_logging(enable=True)`
- `log_to_file(filepath, enable=True)`
- `enable_metrics_server(enable=True, port=8080)`
- `get_monitor_names()`
- `get_monitor(name)`

### PluginSystem
- `__init__()`
- `register_plugin(name, factory, config=None)`
- `create_plugin(name)`
- `has_plugin(name)`
- `get_plugin_names()`
- `unregister_plugin(name)`
- `set_dependencies(plugin_name, dependencies)`
- `get_dependencies(plugin_name)`
- `enable_plugin(name, enable=True)`
- `is_plugin_enabled(name)`
- `validate_plugin(name)`
- `get_plugin_count()`

### ErrorHandler
- `__init__()`
- `log(level, message)`
- `info(message)`
- `warning(message)`
- `error(message)`
- `critical(message)`
- `debug(message)`
- `set_level(level)`
- `get_level()`
- `add_handler(handler)`
- `remove_handler(handler)`
- `clear_handlers()`
- `has_handlers()`
- `log_error(category, message, error_code=0)`
- `get_error_count()`
- `get_errors()`
- `clear_errors()`
- `export_errors(filepath)`
- `get_statistics()`

### ConfigBuilder
- `__init__()`
- `create_template(name, config)`
- `get_template(name)`
- `has_template(name)`
- `remove_template(name)`
- `get_template_names()`
- `get_fast_learning_template()`
- `get_stable_learning_template()`
- `get_balanced_template()`
- `get_exploratory_template()`
- `get_exploitative_template()`
- `apply_template(config, template_name)`
- `validate_template(name)`
- `save_template(name, config)`
- `get_template_count()`

### CheckpointManager
- `__init__()`
- `create_checkpoint(name, brain)`
- `load_checkpoint(name, brain)`
- `has_checkpoint(name)`
- `get_checkpoint_names()`
- `delete_checkpoint(name)`
- `get_checkpoint_info(name)`
- `get_checkpoint_delta(from_name, to_name)`
- `set_base_checkpoint(name)`
- `get_base_checkpoint()`
- `list_checkpoints()`
- `cleanup_old_checkpoints(max_age_hours=24.0)`
- `get_checkpoint_count()`

### SimulationContext
- `__enter__()`
- `__exit__(exc_type, exc_val, exc_tb)`
- `execute(func)`
- `get_brain()`
- `get_config()`

### AdvancedNLM
- `__init__()`
- `initialize(config=None)`
- `reset()`
- `finalize()`
- `step(step, time=0.0)`
- `run(steps, timestep=0.001)`
- `set_neuron_count(count)`
- `set_region_count(count)`
- `set_connection_probability(probability)`
- `set_random_seed(seed)`
- `set_simulation_timestep(timestep)`
- `set_checkpoint_directory(directory)`
- `create_neurons(neuron_type, count)`
- `configure_neuron(neuron_id, config)`
- `get_neuron_config(neuron_id)`
- `set_plasticity_rule(name, rule_config)`
- `enable_plasticity(name, enable)`
- `is_plasticity_enabled(name)`
- `set_integration_method(method)`
- `get_integration_method()`
- `set_custom_timestep_scheduler(scheduler)`
- `set_advanced_learning_algorithm(name)`
- `configure_advanced_learning(params)`
- `start_visualization()`
- `stop_visualization()`
- `save_visualization(filepath)`
- `save_checkpoint(name)`
- `load_checkpoint(name)`
- `export_to_file(filepath, format="json")`
- `optimize_parameters(objective_function)`
- `update_memory_manager(step)`
- `trigger_gc()`
- `get_memory_usage()`
- `get_current_metrics()`
- `get_status_summary()`
- `get_brain()`
- `get_neuron_registry()`
- `get_plasticity_rule_registry()`
- `get_configuration_templates()`
- `get_memory_monitor()`
- `get_memory_manager()`
- `get_parallel_processor()`
- `get_event_driven_simulator()`
- `get_neuron_activity_plotter()`
- `get_network_topology_visualizer()`
- `get_learning_curve_analyzer()`
- `get_statistical_visualizer()`
- `get_incremental_checkpoint_manager()`
- `get_brain_exporter()`
- `get_delta_compressor()`
- `get_bayesian_optimizer()`

## Factory Functions

- `create_advanced_brain(config=None)`
- `create_advanced_nlm()`
- `create_plugin_system()`
- `create_performance_profiler()`
- `create_simulation_controller()`
- `create_error_handler()`
- `create_config_builder()`
- `create_checkpoint_manager()`
- `create_simulation_context(brain, config=None)`

# Troubleshooting

This section addresses common issues and their solutions.

## Issue: "Unknown neuron type" Error

**Symptom:** Error when creating neuron populations.

**Solution:** Ensure the neuron type is registered with the neuron registry.

```python
# Register neuron type first
advanced_brain.add_neuron_type("my_neuron", neuron_type, config)

# Then create population
neuron_ids = advanced_brain.create_neuron_population("my_neuron", 100)
```

## Issue: Performance Degradation

**Symptom:** Simulation performance decreases over time.

**Solution:** Use performance profiling to identify bottlenecks.

```python
# Monitor performance
profiler = pynlm.createPerformanceProfiler()
profiler.start_monitoring("simulation")

# Run simulation
for step in range(1000):
    brain.step(step)
    profiler.get_monitor("simulation").add_step()

profiler.stop_monitoring("simulation")

# Analyze metrics
metrics = profiler.get_all_metrics()
print(f"Performance: {metrics}")
```

## Issue: Memory Usage

**Symptom:** High memory usage during simulation.

**Solution:** Configure memory management and garbage collection.

```python
# Configure memory management
advanced_nlm.get_memory_manager().set_config({
    'max_memory_gb': 16.0,
    'gc_threshold': 0.8,
    'enable_memory_pools': True
})

# Monitor memory usage
memory_usage = advanced_nlm.get_memory_usage()
print(f"Memory usage: {memory_usage:.1f} MB")
```

## Issue: Plugin Registration

**Symptom:** Plugin not found or not working.

**Solution:** Ensure plugin is properly registered and configured.

```python
# Register plugin with proper factory function
def my_plugin_factory():
    # Return properly initialized plugin instance
    pass

plugin_system.register_plugin("my_plugin", my_plugin_factory)

# Validate plugin
if plugin_system.validate_plugin("my_plugin"):
    print("Plugin is valid")
```

## Issue: Simulation Context

**Symptom:** Context manager not restoring original state.

**Solution:** Ensure proper exception handling in context manager.

```python
# Use context manager properly
with pynlm.createSimulationContext(brain, config) as (brain_ctx, config_ctx):
    try:
        # Simulation code
        brain_ctx.step(step)
    except Exception as e:
        print(f"Error caught: {e}")
        # Context manager restores config automatically
```

# Version Information

## Version

- **Version:** 2.0.0 (Enhanced Bindings)
- **Compatibility:** Backward compatible with pynlm v1.x
- **Requirements:** Python 3.7+, pybind11, NLM C++ framework

## Changes

### Version 2.0.0
- Added comprehensive advanced features
- Created new Python wrapper classes
- Enhanced error handling and logging
- Added performance monitoring and metrics
- Implemented plugin system for extensibility
- Added advanced configuration management
- Enhanced checkpointing capabilities
- Added context managers for safe execution

### Backward Compatibility

All existing pynlm API remains unchanged:
- `createBrain()`
- `createSimpleWorld()`
- `createAgentBrain()`
- `createDefaultConfig()`
- All existing methods and properties

# Conclusion

The enhanced NLM Python bindings provide a comprehensive set of advanced features for expert users
while maintaining full backward compatibility with the existing pynlm API. The new classes and
frameworks enable:

1. **Custom neuron types and populations** with advanced configurations
2. **Event-driven and parallel simulation** with flexible processing modes
3. **Comprehensive performance monitoring** with real-time metrics
4. **Extensible plugin system** for custom components
5. **Robust error handling** with logging and recovery
6. **Advanced configuration management** with template support
7. **Enhanced checkpointing** with metadata and incremental backups
8. **Safe execution** with context managers and automatic rollback
9. **Complete framework** with AdvancedNLM integrating all features

These enhancements make the NLM framework suitable for:

- Research applications with complex neural architectures
- Production systems requiring high-performance simulation
- Custom plugins and extensions for specialized applications
- Large-scale simulations with advanced learning algorithms
- Systems requiring comprehensive monitoring and debugging
- Applications needing advanced configuration and checkpointing

The enhanced bindings maintain the simplicity of the original API while providing powerful
features for advanced users and expert applications.

The enhanced NLM Python bindings provide a comprehensive set of advanced features for expert users
while maintaining full backward compatibility with the existing pynlm API. The new classes and
frameworks enable:

1. **Custom neuron types and populations** with advanced configurations
2. **Event-driven and parallel simulation** with flexible processing modes
3. **Comprehensive performance monitoring** with real-time metrics
4. **Extensible plugin system** for custom components
5. **Robust error handling** with logging and recovery
6. **Advanced configuration management** with template support
7. **Enhanced checkpointing** with metadata and incremental backups
8. **Safe execution** with context managers and automatic rollback
9. **Complete framework** with AdvancedNLM integrating all features

These enhancements make the NLM framework suitable for:

- Research applications with complex neural architectures
- Production systems requiring high-performance simulation
- Custom plugins and extensions for specialized applications
- Large-scale simulations with advanced learning algorithms
- Systems requiring comprehensive monitoring and debugging
- Applications needing advanced configuration and checkpointing

The enhanced bindings maintain the simplicity of the original API while providing powerful
features for advanced users and expert applications.