# NLM Framework - Advanced Features Implementation

This document describes the advanced features being added to the NLM (Neural Learning Machine) framework for power users.

## Overview

The NLM framework is a computational brain project that implements neural network dynamics with learning, development, and cognition capabilities. This implementation adds advanced features to support power users while maintaining backward compatibility.

## Features Added

### 1. Custom Neuron Types

#### 1.1 Custom Neuron Factory Functions

Users can now create custom neuron types using factory functions:

```python
# Example custom neuron factory
from nlm import NeuronFactory, Neuron

def create_ca1_hippocampal_neuron(neuron_id):
    neuron = NeuronFactory.create("custom_ca1", neuron_id)
    neuron.setMembranePotential(-65.0)
    neuron.setThreshold(-50.0)
    neuron.setRestingPotential(-65.0)
    neuron.setResetPotential(-65.0)
    neuron.setLeakConductance(5.0)
    return neuron

# Register custom neuron type
NeuronFactory.register(
    "custom_ca1",
    create_ca1_hippocampal_neuron,
    {"threshold": (-50.0, "mV"), "leak_conductance": (5.0, "nS")}
)
```

#### 1.2 Custom Integration Methods

The framework now supports multiple integration methods:

```python
from nlm import Brain, SimulationConfig

# Euler integration
config = SimulationConfig(integration_method="euler")
brain = Brain(config)

# Runge-Kutta 4 integration
config = SimulationConfig(integration_method="rk4")
brain = Brain(config)

# Exponential Euler (default)
config = SimulationConfig(integration_method="exponential_euler")
brain = Brain(config)
```

#### 1.3 Custom Plasticity Rules

Users can implement and register custom plasticity rules:

```python
from nlm import PlasticityRule, Synapse

class CustomPlasticityRule(PlasticityRule):
    def __init__(self, learning_rate=0.01):
        super().__init__()
        self.learning_rate = learning_rate
        self.rule_name = "CustomPlasticity"
    
    def update(self, synapse, pre_spikes, post_spikes, dt):
        # Custom plasticity implementation
        # Calculate weight change based on spike timing and other factors
        weight_change = self.calculate_weight_change(pre_spikes, post_spikes, dt)
        self.apply_weight_change(synapse, weight_change)
    
    def apply_weight_change(self, synapse, delta):
        new_weight = synapse.getWeight() + delta
        new_weight = max(-1.0, min(1.0, new_weight))  # Clamp
        synapse.setWeight(new_weight)
    
    def getName(self):
        return self.rule_name
    
    def setLearningRate(self, rate):
        self.learning_rate = max(0.0, min(1.0, rate))
    
    def getLearningRate(self):
        return self.learning_rate

# Register custom plasticity rule
from nlm import PlasticitySystem
PlasticitySystem.register_rule("custom", CustomPlasticityRule)
```

### 2. Advanced Learning Algorithms

#### 2.1 Reward-Modulated STDP (R-STDP)

```python
from nlm import RewardModulatedSTDP

# Create reward-modulated STDP rule
rstdp = RewardModulatedSTDP(learning_rate=0.01, reward_factor=0.1)

# Configure it
rstdp.configure(
    reward_gain=0.1,
    depression_gain=0.01,
    potentiation_gain=0.02,
    tau_pre=20.0,
    tau_post=20.0
)

# Use in brain
brain = Brain()
brain.setPlasticityRule(rstdp)
```

#### 2.2 Sequence Learning (BCM Theory)

```python
from nlm import BCMRule, Neuron

# Create BCM rule for sequence learning
bc_rule = BCMRule(
    theta=0.5,          # Threshold
    tau=100.0,         # Learning time constant
    beta=0.02          # Learning rate
)

# BCM updates based on post-synaptic activity history
# Implements "sliding threshold" theory of synaptic modification
```

#### 2.3 Homeostatic Plasticity Rules

```python
from nlm import HomeostaticPlasticity

# Create homeostatic plasticity rule
homeo = HomeostaticPlasticity(
    target_rate=10.0,      # Target firing rate (Hz)
    time_constant=100.0,   # Time constant (ms)
    max_rate_change=0.1    # Maximum rate change per timestep
)

# Maintains stability by scaling all synapses
# when activity deviates from target
```

#### 2.4 Predictive Coding Algorithms

```python
from nlm import PredictiveCoding

# Create predictive coding layer
pc = PredictiveCoding(
    prediction_error_gain=0.5,
    learning_rate=0.01,
    prediction_horizon=10
)

# Implements predictive coding algorithm where neurons
# represent both predictions and prediction errors
```

### 3. Enhanced Visualization

#### 3.1 Real-time Neuron Activity Plotting

```python
from nlm import NeuronActivityPlotter

# Create real-time plotter
plotter = NeuronActivityPlotter(
    update_interval=0.1,  # Update every 100ms
    history_length=1000   # Store 1000 recent time steps
)

# Start plotting
plotter.start(brain)

# Get plots
plotter.update(brain)
plotter.save("neuron_activity.png")
```

#### 3.2 Network Topology Visualization

```python
from nlm import NetworkTopologyVisualizer

# Create topology visualization
visualizer = NetworkTopologyVisualizer(
    layout="spring_embedded",
    node_size=5.0,
    edge_width=1.0,
    highlight_activity=True
)

# Generate network graph
network_graph = visualizer.generate_graph(brain)
visualizer.plot(network_graph, "network_topology.png")
```

#### 3.3 Learning Curve Analysis

```python
from nlm import LearningCurveAnalyzer

# Create learning curve analyzer
analyzer = LearningCurveAnalyzer(
    metrics=["accuracy", "loss", "firing_rate"],
    smoothing_window=50
)

# Record learning progress
for step in range(simulation_steps):
    metrics = brain.getCurrentMetrics()
    analyzer.record(step, metrics)

# Generate learning curves
analyzer.generate_curves("learning_curves.png")
```

#### 3.4 Statistical Visualization

```python
from nlm import StatisticalVisualizer

# Create statistical visualization
sv = StatisticalVisualizer(
    statistics=["mean", "std", "percentile_25", "percentile_75"],
    confidence_interval=0.95
)

# Analyze neuron statistics
statistics = sv.analyze_neuron_populations(brain)
sv.plot_distributions(statistics, "neuron_distributions.png")
```

### 4. Simulation Control

#### 4.1 Custom Timestep Scheduling

```python
from nlm import Brain, TimestepScheduler

# Create custom timestep scheduler
class AdaptiveTimestepScheduler(TimestepScheduler):
    def getTimestep(self, brain, current_step):
        # Dynamically adjust timestep based on system state
        firing_rate = brain.getAverageFiringRate()
        if firing_rate > 50.0:
            return 0.001  # Smaller timestep for high activity
        elif firing_rate < 5.0:
            return 0.01   # Larger timestep for low activity
        else:
            return 0.005   # Medium timestep otherwise

# Use custom scheduler
scheduler = AdaptiveTimestepScheduler()
brain = Brain(timestep_scheduler=scheduler)
```

#### 4.2 Event-Driven Simulation Control

```python
from nlm import Brain, EventDrivenSimulator

# Create event-driven simulator
class SpikeDrivenSimulator(EventDrivenSimulator):
    def getNextEvent(self, brain):
        # Get next spike event
        spike_events = brain.getPendingSpikeEvents()
        if spike_events:
            return min(spike_events, key=lambda e: e.timestamp)
        return None

# Use event-driven simulation
simulator = SpikeDrivenSimulator()
brain = Brain(simulator=simulator)
```

#### 4.3 Parallel Processing Options

```python
from nlm import Brain, ParallelProcessor

# Configure parallel processing
parallel_config = {
    "neuron_update": True,      # Update neurons in parallel
    "synapse_update": False,    # Synaptic updates remain sequential
    "spike_propagation": True,  # Spike propagation in parallel
    "plasticity_update": False, # Plasticity updates sequentially
    "num_threads": 8           # Number of worker threads
}

brain = Brain(parallel_config=parallel_config)
```

#### 4.4 Memory Management Controls

```python
from nlm import Brain, MemoryManager

# Configure memory management
memory_config = {
    "neuron_pool_size": 10000,
    "spike_queue_size": 1000000,
    "checkpoint_interval": 1000,
    "max_memory_gb": 16,
    "gc_threshold": 0.8,
    "compression_level": "balanced"
}

brain = Brain(memory_config=memory_config)
```

### 5. Export/Import Capabilities

#### 5.1 Multiple Format Support

```python
from nlm import BrainExporter

# Export in multiple formats
exporter = BrainExporter(brain)

# JSON format (human readable)
exporter.export("brain_state.json", format="json")

# Binary format (compact)
exporter.export("brain_state.bin", format="binary")

# Compressed format (space efficient)
exporter.export("brain_state.gz", format="compressed", compression="gzip")

# Differential format (changes only)
exporter.export("brain_state_diff", format="differential", base_file="brain_state.json")
```

#### 5.2 Incremental Checkpointing

```python
from nlm import Brain, CheckpointManager

# Configure incremental checkpointing
checkpoint_config = {
    "directory": "./checkpoints",
    "interval": 100,           # Save every 100 steps
    "keep": 10,                # Keep last 10 checkpoints
    "compress": True,
    "format": "binary"
}

brain = Brain(checkpoint_config=checkpoint_config)
# Checkpoints are automatically saved
```

#### 5.3 Version Compatibility

```python
from nlm import BrainExporter, CheckpointImporter

# Export with version information
exporter = BrainExporter(brain)
exporter.export("brain_state.json", format="json", version="2.0.0")

# Import with version checking
importer = CheckpointImporter("brain_state.json")
if importer.validate():
    # Check version compatibility
    if importer.isCompatible("1.5.0"):
        # Compatible - load state
        pass
    else:
        # Handle version mismatch
        pass
```

#### 5.4 Delta Compression

```python
from nlm import DeltaCompressor

# Create delta compressor
delta_compressor = DeltaCompressor(
    base_checkpoint="brain_state_v1.json",
    compression="lz4"
)

# Generate delta
brain = Brain()
checkpoint1 = brain.export_state()
checkpoint2 = brain.export_state()
delta = delta_compressor.create_delta(checkpoint1, checkpoint2)
delta_compressor.save("delta_checkpoint", delta)
```

### 6. Advanced Configuration

#### 6.1 Configuration Templates

```python
from nlm import Brain, ConfigurationTemplates

# Get preset templates
templates = ConfigurationTemplates()

# Fast learning template (high plasticity, low stability)
fast_learning_config = templates.get("fast_learning")
brain = Brain(fast_learning_config)

# Stable learning template (low plasticity, high stability)
stable_learning_config = templates.get("stable_learning")
brain = Brain(stable_learning_config)

# Balanced template (moderate values)
balanced_config = templates.get("balanced")
brain = Brain(balanced_config)

# Create custom template
custom_config = {
    "neuron_count": 5000,
    "plasticity": {
        "stdp": {"enabled": True, "rate": 0.02},
        "hebbian": {"enabled": True, "rate": 0.01},
        "reward_modulated": {"enabled": True, "gain": 0.5}
    },
    "development": {
        "critical_period": 100,
        "maturation_rate": 0.01
    }
}
templates.save("my_custom_template", custom_config)
```

#### 6.2 Parameter Optimization

```python
from nlm import Brain, ParameterOptimizer

# Configure random search optimizer
random_search = ParameterOptimizer(
    optimization_method="random_search",
    parameter_space={
        "plasticity.stdp.learning_rate": [0.001, 0.01, 0.1],
        "development.critical_period": [50, 100, 200],
        "neuron.leak_conductance": [5.0, 10.0, 20.0]
    },
    n_trials=50,
    parallel=True
)

# Run optimization
best_params = random_search.optimize(brain)
brain.apply_configuration(best_params)
```

#### 6.3 Bayesian Optimization Support

```python
from nlm import Brain, BayesianOptimizer

# Configure Bayesian optimization
bayesian_opt = BayesianOptimizer(
    optimization_method="bayesian",
    acquisition_function="ei",  # Expected Improvement
    n_initial_points=10,
    n_iterations=30,
    model="random_forest"
)

# Define objective function
def objective_function(params):
    brain = Brain()
    brain.apply_configuration(params)
    return evaluate_performance(brain)

# Run optimization
best_params, best_score = bayesian_opt.optimize(objective_function)
```

### 7. Memory Management

#### 7.1 Memory Pool Management

```python
from nlm import Brain, MemoryPool

# Configure memory pools
pool_config = {
    "neuron_pool": {
        "size": 10000,
        "alignment": 64,
        "type": "numa_aware"
    },
    "spike_pool": {
        "size": 1000000,
        "burst_support": True
    },
    "synapse_pool": {
        "size": 50000,
        "sparse": True
    }
}

brain = Brain(memory_pools=pool_config)
```

#### 7.2 Garbage Collection Controls

```python
from nlm import Brain, GarbageCollector

# Configure garbage collection
gc_config = {
    "trigger_threshold": 0.8,        # Trigger when 80% memory used
    "collection_frequency": 100,      # Collect every 100 steps
    "max_promotions": 3,             # Max promotion rounds
    "pause_duration": 10,            # Pause duration (ms)
    "aggressive_mode": False         # Gentle or aggressive GC
}

brain = Brain(gc_config=gc_config)
```

#### 7.3 Checkpoint Management

```python
from nlm import Brain, CheckpointManager

# Configure checkpoint management
checkpoint_config = {
    "directory": "./checkpoints",
    "max_checkpoints": 20,
    "auto_save": True,
    "auto_load": True,
    "compression": "lz4",
    "validation": True,
    "retention_policy": "fifo"  # Or "lifo", "size_based"
}

brain = Brain(checkpoint_config=checkpoint_config)
```

#### 7.4 Memory Monitoring

```python
from nlm import Brain, MemoryMonitor

# Configure memory monitoring
monitor_config = {
    "sampling_interval": 1.0,        # Sample every 1 second
    "metrics": ["usage", "growth", "leaks"],
    "alerts": {
        "memory_usage": {"threshold": 0.9, "action": "warn"},
        "gc_frequency": {"threshold": 0.1, "action": "optimize"}
    },
    "reporting": {
        "console": True,
        "file": "./memory.log",
        "metrics_server": False
    }
}

brain = Brain(memory_monitor=monitor_config)

# Monitor memory usage
monitor = brain.get_memory_monitor()
while running:
    usage = monitor.get_memory_usage()
    growth = monitor.get_growth_rate()
    if usage > 0.9:
        monitor.trigger_gc()
```

## API Usage Examples

### Complete Example: Custom Network with Advanced Features

```python
from nlm import (
    Brain, NeuronFactory, PlasticitySystem, 
    RewardModulatedSTDP, BCMRule, HomeostaticPlasticity,
    NeuronActivityPlotter, NetworkTopologyVisualizer,
    ParameterOptimizer, ConfigurationTemplates
)

# 1. Configure custom neuron types
NeuronFactory.register("pyramidal", create_pyr_neuron, {
    "threshold": (-50.0, "mV"),
    "leak_conductance": (10.0, "nS")
})

# 2. Configure advanced learning
brain = Brain(
    neuron_count=5000,
    plasticity_rules=[
        RewardModulatedSTDP(learning_rate=0.01),
        BCMRule(theta=0.5, beta=0.02),
        HomeostaticPlasticity(target_rate=10.0)
    ]
)

# 3. Configure optimization
random_search = ParameterOptimizer(
    optimization_method="random_search",
    parameter_space={
        "plasticity.stdp.learning_rate": [0.001, 0.01, 0.1],
        "neuron.leak_conductance": [5.0, 10.0, 20.0]
    },
    n_trials=20
)

# 4. Configure visualization
plotter = NeuronActivityPlotter(
    update_interval=0.1,
    history_length=1000
)

# 5. Configure checkpointing
brain.configure_checkpointing(
    directory="./checkpoints",
    interval=500,
    compression="gzip"
)

# 6. Run optimization
best_params = random_search.optimize(brain)
brain.apply_configuration(best_params)

# 7. Start simulation
brain.initialize()
for step in range(10000):
    brain.step(step)
    plotter.update(brain)
    if step % 100 == 0:
        plotter.save(f"plot_{step:06d}.png")

print("Simulation completed successfully!")
```

## Backward Compatibility

All new features maintain backward compatibility:

- Existing Python code using the basic NLM API continues to work unchanged
- New features are opt-in through new classes and methods
- Default behavior remains the same
- Migration guide available in `MIGRATION_GUIDE.md`

## Error Handling

Comprehensive error handling is implemented:

- Validation of parameters and configurations
- Graceful degradation when features are unavailable
- Detailed error messages with context
- Exception hierarchy for different error types
- Recovery mechanisms for simulation failures

## Performance Considerations

- Advanced features add minimal overhead when not used
- Parallel processing options available for computationally intensive tasks
- Memory management prevents memory leaks
- Compression options reduce storage requirements
- Checkpointing supports incremental saves

## Testing

Comprehensive testing includes:

- Unit tests for all new classes and methods
- Integration tests for feature interactions
- Performance benchmarks
- Memory usage profiling
- Stress tests for large networks
- Regression tests for backward compatibility

## Conclusion

The NLM framework now provides powerful capabilities for advanced users while maintaining simplicity for beginners. The new features enable:

- Custom neural architectures
- Advanced learning algorithms
- Comprehensive visualization tools
- Fine-grained simulation control
- Robust export/import capabilities
- Intelligent configuration management
- Efficient memory management

These enhancements make the NLM framework suitable for research, education, and production use across a wide range of applications.
