# Advanced NLM Python Interface

This file provides comprehensive Python bindings and examples for using the Advanced NLM framework.

## Overview

The Advanced NLM Python interface provides access to all the advanced features of the NLM framework,
including custom neuron types, advanced learning algorithms, visualization tools, simulation control,
export/import capabilities, advanced configuration, and memory management.

## Installation

```bash
pip install nlm-advanced
# or
conda install -c conda-forge nlm-advanced
```

## Basic Usage

### Creating an Advanced NLM Instance

```python
import nlm

# Create default advanced NLM
advanced_nlm = nlm.AdvancedNLM()
advanced_nlm.initialize()

# Create with custom configuration
config = {
    "neuron_count": 5000,
    "plasticity.stdp.enabled": "true",
    "plasticity.stdp.learning_rate": "0.02",
    "plasticity.hebbian.enabled": "true",
    "plasticity.reward_modulated.enabled": "true",
    "plasticity.reward_modulated.gain": "0.1",
    "visualization.enabled": "true",
    "visualization.real_time": "true",
    "export.format": "json",
    "memory.gc.enabled": "true",
    "memory.pools.enabled": "true"
}

advanced_nlm = nlm.AdvancedNLM(config)
```

### Basic Simulation

```python
import nlm

# Create brain
brain = nlm.AdvancedNLM()
brain.initialize()

# Run simulation
brain.run(1000, 0.001)  # 1000 steps, 1ms timestep

# Get metrics
metrics = brain.getCurrentMetrics()
print(f"Average firing rate: {metrics['average_firing_rate']:.2f} Hz")
print(f"Total spikes: {metrics['total_spikes']}")

# Save visualization
brain.saveVisualization("simulation_results.png")
```

## Custom Neuron Types

### Creating Custom Neurons

```python
import nlm

# Register custom neuron types
from nlm import NeuronFactory

# Create a custom hippocampal neuron factory
def create_hippocampal_neuron(neuron_id):
    neuron = nlm.NeuronFactory.create("custom_ca1", neuron_id)
    neuron.setMembranePotential(-65.0)
    neuron.setThreshold(-50.0)
    neuron.setRestingPotential(-65.0)
    neuron.setResetPotential(-65.0)
    neuron.setLeakConductance(5.0)
    neuron.setRefractoryPeriod(5)
    neuron.enablePlasticity(true, true, true)
    return neuron

# Register the custom neuron type
NeuronFactory.register("custom_ca1", create_hippocampal_neuron)

# Create custom neuron configuration
custom_config = {
    "name": "Hippocampal CA1 Neuron",
    "type": "Pyramidal",
    "threshold": -50.0,
    "resting_potential": -65.0,
    "reset_potential": -65.0,
    "leak_conductance": 5.0,
    "refractory_period": 5,
    "enable_plasticity": true,
    "enable_development": true,
    "enable_neuromodulation": true,
    "integration_method": "exponential_euler",
    "plasticity_rule": "stdp",
    "learning_rate": 0.01
}

# Configure neuron for specific ID
brain = nlm.AdvancedNLM()
brain.configureNeuron(0, custom_config)
```

### Available Neuron Types

```python
import nlm

neuron_registry = nlm.AdvancedNLM.getNeuronRegistry()
available_types = neuron_registry.getNeuronTypes()

print("Available neuron types:")
for neuron_type in available_types:
    print(f"  - {neuron_type}")
```

## Advanced Learning Algorithms

### Reward-Modulated STDP

```python
import nlm

brain = nlm.AdvancedNLM()

# Configure reward-modulated STDP
brain.setAdvancedLearningAlgorithm("reward_modulated_stdp")
brain.configureAdvancedLearning({
    "reward_gain": 0.1,
    "learning_rate": 0.01,
    "time_constant": 20.0
})

# The R-STDP rule combines spike timing with reward signals
# to modulate synaptic plasticity based on behavioral outcomes
```

### BCM Learning

```python
import nlm

brain = nlm.AdvancedNLM()

# Configure BCM (Bienenstock-Cooper-Munro) learning
brain.setAdvancedLearningAlgorithm("bcn")
brain.configureAdvancedLearning({
    "theta": 0.5,           # Threshold for potentiation/depression
    "tau": 100.0,          # Learning time constant
    "beta": 0.02           # Learning rate
})

# BCM implements a sliding threshold for synaptic modification
# based on postsynaptic activity history
```

### Homeostatic Plasticity

```python
import nlm

brain = nlm.AdvancedNLM()

# Configure homeostatic plasticity
brain.setAdvancedLearningAlgorithm("homeostatic")
brain.configureAdvancedLearning({
    "target_rate": 10.0,           # Target firing rate (Hz)
    "time_constant": 100.0,       # Time constant (ms)
    "max_rate_change": 0.1        # Maximum rate change per timestep
})

# Homeostatic plasticity maintains stability by scaling synapses
# when activity deviates from target
```

### Predictive Coding

```python
import nlm

brain = nlm.AdvancedNLM()

# Configure predictive coding
brain.setAdvancedLearningAlgorithm("predictive_coding")
brain.configureAdvancedLearning({
    "prediction_error_gain": 0.5,
    "learning_rate": 0.01,
    "prediction_horizon": 10
})

# Predictive coding implements hierarchical prediction and error minimization
# across multiple neural levels
```

## Integration Methods

### Available Integration Methods

```python
import nlm

from nlm import IntegrationMethodSelector

methods = [
    "euler",
    "rk4", 
    "exponential_euler",
    "adaptive",
    "crank_nicolson",
    "forward_euler",
    "backward_euler",
    "implicit_euler",
    "trapezoidal_rule",
    "leapfrog",
    "velocity_verlet",
    "stochastic_euler",
    "milstein"
]

print("Available integration methods:")
for method in methods:
    print(f"  - {method}")

# Set integration method
brain = nlm.AdvancedNLM()
brain.setIntegrationMethod("rk4")  # Runge-Kutta 4
```

### Custom Timestep Scheduler

```python
import nlm

# Create custom timestep scheduler
class AdaptiveTimestepScheduler(nlm.CustomTimestepScheduler):
    def getTimestep(self, brain, current_step):
        # Dynamically adjust timestep based on system state
        firing_rate = brain.getBrain().getAverageFiringRate()
        if firing_rate > 50.0:
            return 0.001  # Smaller timestep for high activity
        elif firing_rate < 5.0:
            return 0.01   # Larger timestep for low activity
        else:
            return 0.005   # Medium timestep otherwise

# Use custom scheduler
brain = nlm.AdvancedNLM()
scheduler = AdaptiveTimestepScheduler()
brain.setCustomTimestepScheduler(scheduler)
```

## Visualization Tools

### Neuron Activity Plotting

```python
import nlm

brain = nlm.AdvancedNLM()

# Create real-time neuron activity plotter
plotter = brain.getNeuronActivityPlotter()

# Start plotting
plotter.start(brain.getBrain())

# Run simulation with visualization
for step in range(1000):
    brain.step(step)
    plotter.update(brain.getBrain())
    if step % 100 == 0:
        plotter.save(f"plot_{step:06d}.png")

# Stop plotting
plotter.stop()
```

### Network Topology Visualization

```python
import nlm

brain = nlm.AdvancedNLM()

# Create network topology visualizer
visualizer = brain.getNetworkTopologyVisualizer()

# Generate network graph
visualizer.visualize(brain.getBrain(), "network_topology.png")

# Show in interactive window
visualizer.show()
```

### Learning Curve Analysis

```python
import nlm

brain = nlm.AdvancedNLM()

# Create learning curve analyzer
analyzer = brain.getLearningCurveAnalyzer()

# Record learning progress
for step in range(1000):
    metrics = brain.getCurrentMetrics()
    analyzer.record(step, metrics)

# Analyze and generate curves
analyzer.analyze()
analyzer.generateCurves("learning_curves.png")
```

### Statistical Visualization

```python
import nlm

brain = nlm.AdvancedNLM()

# Create statistical visualizer
sv = brain.getStatisticalVisualizer()

# Visualize neuron statistics
sv.visualize(brain.getBrain(), "neuron_statistics.png")

# Show in interactive window
sv.show()
```

## Simulation Control

### Event-Driven Simulation

```python
import nlm

brain = nlm.AdvancedNLM()

# Create event-driven simulator
# Use spike-driven simulation
brain.getEventDrivenSimulator().setSpikeDriven(True)
brain.getEventDrivenSimulator().start(brain.getBrain())

# Run event-driven simulation
brain.getEventDrivenSimulator().runToEvent(brain.getBrain())

# Stop simulator
brain.getEventDrivenSimulator().stop()
```

### Parallel Processing

```python
import nlm

# Configure parallel processing
from nlm import ParallelProcessor

config = ParallelProcessor.Config()
config.neuronUpdate = True
config.spikePropagation = True
config.numThreads = 8
config.enableLoadBalancing = True

brain = nlm.AdvancedNLM()
brain.getParallelProcessor().setConfig(config)

# Process neurons in parallel
brain.getParallelProcessor().processNeurons(brain.getBrain(), 0)
```

## Export/Import Capabilities

### Multiple Format Support

```python
import nlm

brain = nlm.AdvancedNLM()

# Export in multiple formats
brain.getBrainExporter().exportToFile("brain_state.json", "json")
brain.getBrainExporter().exportToFile("brain_state.bin", "binary")
brain.getBrainExporter().exportToFile("brain_state.gz", "compressed", {"compression": "gzip"})

# Export to specific components
brain.getBrainExporter().exportNeurons("neurons.json")
brain.getBrainExporter().exportSynapses("synapses.json")
brain.getBrainExporter().exportConnections("connections.json")
```

### Incremental Checkpointing

```python
import nlm

brain = nlm.AdvancedNLM()

# Save checkpoints incrementally
brain.saveCheckpoint("checkpoint_1000")
brain.step(1000)
brain.saveCheckpoint("checkpoint_2000")
brain.step(1000)
brain.saveCheckpoint("checkpoint_3000")

# Load specific checkpoint
brain.loadCheckpoint("checkpoint_2000")
```

### Delta Compression

```python
import nlm

brain = nlm.AdvancedNLM()

# Create delta compressor
compressor = brain.getDeltaCompressor()

# Save checkpoints
brain.saveCheckpoint("base_checkpoint")

# Generate delta
brain.step(100)
brain.saveCheckpoint("new_checkpoint")

# Get delta between checkpoints
delta = compressor.getDelta("base_checkpoint", "new_checkpoint")
compressor.save("delta.bin", delta)
```

## Advanced Configuration

### Configuration Templates

```python
import nlm

# Get configuration templates
templates = nlm.AdvancedNLM.getConfigurationTemplates()

# Fast learning template
fast_config = templates.getFastLearningTemplate()

# Stable learning template  
stable_config = templates.getStableLearningTemplate()

# Balanced template
balanced_config = templates.getBalancedTemplate()

# Create brain with template
brain = nlm.AdvancedNLM(balanced_config)
```

### Parameter Optimization

```python
import nlm
import random

brain = nlm.AdvancedNLM()

# Configure random search optimizer
from nlm import ParameterOptimizer

optimizer = ParameterOptimizer(
    method=ParameterOptimizer.Method.RandomSearch,
    parameter_space={
        "plasticity.stdp.learning_rate": [0.001, 0.01, 0.1],
        "neuron.leak_conductance": [5.0, 10.0, 20.0],
        "development.critical_period": [50, 100, 200]
    },
    n_trials=20,
    parallel=False
)

# Define objective function
def objective_function(params):
    brain = nlm.AdvancedNLM()
    brain.applyConfiguration(params)
    brain.initialize()
    brain.run(100, 0.001)
    metrics = brain.getCurrentMetrics()
    return -metrics.get("average_firing_rate", 0)  # Minimize negative firing rate

# Run optimization
best_params = optimizer.optimize(objective_function)
print("Best parameters:", best_params)

# Apply best parameters
brain.applyConfiguration(best_params)
```

### Bayesian Optimization

```python
import nlm

brain = nlm.AdvancedNLM()

# Configure Bayesian optimization
from nlm import BayesianOptimizer

optimizer = BayesianOptimizer(
    nInitialPoints=5,
    nIterations=10,
    model="random_forest",
    acquisitionFunction="ei"
)

# Define parameter bounds
parameter_bounds = {
    "plasticity.stdp.learning_rate": (0.001, 0.1),
    "neuron.leak_conductance": (5.0, 20.0),
    "development.critical_period": (50, 200)
}

# Run optimization
def objective_function(params):
    brain = nlm.AdvancedNLM()
    brain.applyConfiguration(params)
    brain.initialize()
    brain.run(100, 0.001)
    metrics = brain.getCurrentMetrics()
    return metrics.get("average_firing_rate", 0)

best_params, best_score = optimizer.optimize(objective_function, parameter_bounds)
print(f"Best parameters: {best_params}")
print(f"Best score: {best_score}")
```

## Memory Management

### Memory Pool Management

```python
import nlm

brain = nlm.AdvancedNLM()

# Configure memory pools
brain.getAdvancedMemoryPool().enableBurstSupport(True)
brain.getAdvancedMemoryPool().setAllocationStrategy("numa_aware")

# Check memory usage
print(f"Memory usage: {brain.getAdvancedMemoryPool().getUsageRatio() * 100:.1f}%")
```

### Smart Garbage Collection

```python
import nlm

brain = nlm.AdvancedNLM()

# Configure garbage collector
gc = brain.getSmartGarbageCollector()
gc.setThreshold(0.8)  # Trigger at 80% memory usage
gc.setCollectionFrequency(100)  # Collect every 100 steps
gc.setMaxPromotions(3)  # Max promotion rounds
gc.setPauseDuration(10.0)  # 10ms pause duration
gc.setAggressiveMode(False)  # Gentle GC mode

# Trigger garbage collection manually
brain.triggerGC()

# Monitor memory usage
print(f"Current memory usage: {gc.getCurrentMemoryUsage() * 100:.1f}%")
print(f"GC count: {gc.getGCCount()}")
```

### Memory Monitoring

```python
import nlm

brain = nlm.AdvancedNLM()

# Configure memory monitor
monitor = brain.getMemoryMonitor()

# Set alerts
monitor.setAlert("memory_usage", 0.9, "warn")
monitor.setAlert("gc_frequency", 0.1, "optimize")

# Start monitoring
monitor.startMonitoring(brain.getBrain())

# Log to console and file
monitor.logToConsole(True)
monitor.logToFile("./memory.log", True)

# Check memory usage
print(f"Memory usage: {monitor.getMemoryUsage() * 100:.1f}%")
print(f"Growth rate: {monitor.getGrowthRate():.4f}")
print(f"Allocated neurons: {monitor.getAllocatedNeurons()}")

# Stop monitoring
monitor.stopMonitoring()
```

## Complete Example: Custom Network with All Advanced Features

```python
import nlm
from nlm import NeuronFactory, PlasticitySystem, IntegrationMethodSelector

# 1. Configure custom neuron types
NeuronFactory.register("pyramidal", lambda id: nlm.Neuron(id))
NeuronFactory.register("basket", lambda id: nlm.Neuron(id))

# 2. Configure advanced learning
brain = nlm.AdvancedNLM()
brain.setAdvancedLearningAlgorithm("reward_modulated_stdp")
brain.configureAdvancedLearning({
    "reward_gain": 0.1,
    "learning_rate": 0.01,
    "time_constant": 20.0
})

# 3. Configure integration method
brain.setIntegrationMethod("rk4")

# 4. Configure visualization
brain.startVisualization()

# 5. Configure checkpointing
brain.saveCheckpoint("initial_state")

# 6. Configure memory management
brain.triggerGC()

# 7. Run optimization
def objective_function(params):
    brain = nlm.AdvancedNLM()
    brain.applyConfiguration(params)
    brain.initialize()
    brain.run(100, 0.001)
    return brain.getCurrentMetrics().get("average_firing_rate", 0)

best_params = brain.optimizeParameters(objective_function)

# 8. Apply optimized parameters
brain.applyConfiguration(best_params)

# 9. Run simulation with all features
for step in range(1000):
    brain.step(step)
    
    # Update visualization
    if step % 100 == 0:
        brain.saveVisualization(f"frame_{step:06d}.png")
    
    # Save checkpoints
    if step % 500 == 0:
        brain.saveCheckpoint(f"checkpoint_{step}")

# 10. Final analysis
brain.stopVisualization()
metrics = brain.getCurrentMetrics()
print(f"Simulation completed with {metrics['total_spikes']} spikes")
print(f"Average firing rate: {metrics['average_firing_rate']:.2f} Hz")

# 11. Export results
brain.exportToFile("final_results.json")
```

## Error Handling and Validation

### Error Handling

```python
import nlm

brain = nlm.AdvancedNLM()

# Safe step execution
try:
    brain.step(0)
    print("Step completed successfully")
except Exception as e:
    print(f"Step failed: {e}")
    # Recovery logic
    brain.reset()

# Configuration validation
try:
    config = {"invalid_parameter": "value"}
    brain = nlm.AdvancedNLM(config)
except ValueError as e:
    print(f"Invalid configuration: {e}")

# Memory management validation
if brain.getMemoryUsage() > 0.9:
    print("High memory usage detected")
    brain.triggerGC()
```

## Performance Considerations

### Memory Management

```python
import nlm

# Configure memory-efficient settings
config = {
    "neuron_count": 1000,  # Smaller network
    "memory.gc.enabled": "true",
    "memory.pools.enabled": "true",
    "memory.pools.type": "buddy_system",
    "parallel.processing.enabled": "true",
    "parallel.num_threads": 4
}

brain = nlm.AdvancedNLM(config)

# Monitor and optimize memory usage
monitor = brain.getMemoryMonitor()
while brain.isRunning():
    usage = monitor.getMemoryUsage()
    if usage > 0.8:
        print("Memory usage high, triggering GC")
        brain.triggerGC()
```

### Checkpointing Strategy

```python
import nlm

# Configure efficient checkpointing
config = {
    "export.format": "differential",
    "export.checkpoint_interval": 500,
    "export.compression_level": "balanced"
}

brain = nlm.AdvancedNLM(config)

# Save checkpoints strategically
for step in range(0, 5000, 500):
    brain.step(step)
    brain.saveCheckpoint(f"step_{step}")

# Load specific checkpoint
brain.loadCheckpoint("step_2500")
```

## Integration with Existing NLM

### Backward Compatibility

```python
import nlm

# Old code continues to work
brain = nlm.Brain()  # Existing NLM
brain.step(0)

# New code uses advanced features
advanced_brain = nlm.AdvancedNLM()  # New Advanced NLM
advanced_brain.initialize()

# Both can be used interchangeably where possible
```

### Migration Guide

```python
import nlm

# Migration from Basic NLM to Advanced NLM
def migrate_legacy_brain(legacy_brain):
    # Create advanced NLM
    advanced_brain = nlm.AdvancedNLM()
    
    # Copy configuration
    config = legacy_brain.getConfig()
    advanced_brain.initialize(config)
    
    # Copy state
    advanced_brain.getBrain().step(0)
    
    return advanced_brain
```

## Conclusion

The Advanced NLM framework provides powerful capabilities for:

- Custom neuron types with factory functions
- Advanced learning algorithms (R-STDP, BCM, Homeostatic, Predictive Coding)
- Real-time visualization tools (activity plotting, topology, learning curves, statistics)
- Fine-grained simulation control (custom timesteps, event-driven, parallel processing)
- Robust export/import capabilities (multiple formats, incremental checkpoints, delta compression)
- Intelligent configuration management (templates, parameter optimization, Bayesian optimization)
- Efficient memory management (pools, garbage collection, monitoring)

These enhancements make the NLM framework suitable for research, education, and production use while maintaining backward compatibility with existing code.
