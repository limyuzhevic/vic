# NLM Python Bindings - Advanced Features Documentation

This document provides detailed information about the advanced features added to the NLM Python bindings for power users and researchers.

## Context Managers

### BrainContextManager

```python
import pynlm as nlm

# Manual resource management
brain = nlm.createBrainWithDefaultConfig()
try:
    brain.initialize()
    # Use brain...
    for step in range(100):
        brain.step(step, step * 0.001)
finally:
    brain.reset()

# Preferred: Using context manager
with nlm.BrainContextManager(nlm.createBrainWithDefaultConfig()) as brain:
    # Brain is automatically initialized
    for step in range(100):
        brain.step(step, step * 0.001)
    # Brain is automatically reset when exiting the context
```

**Features:**
- Automatic initialization and cleanup
- Exception-safe resource management
- Context manager protocol support
- Automatic reset on exit

### WorldContextManager

```python
import pynlm as nlm

# Using context manager for world management
with nlm.WorldContextManager(nlm.createWorldWithSetup(width=300, height=300)) as world:
    agent = nlm.createCompleteAgentSetup(world)
    
    for episode in range(50):
        agent.reset()
        # Run episode...
        pass
        
    # World resources are automatically cleaned up
```

## Event System

### EventQueue Class

The event system provides efficient management of neural events and callbacks.

```python
import pynlm as nlm

# Create event queue
event_queue = nlm.EventQueue()

# Queue spikes
dopamine_neuron = nlm.NeuronId(1)
excitation_neuron = nlm.NeuronId(2)

# Queue immediate spikes
event_queue.queueSpike(dopamine_neuron, 1000.0)
event_queue.queueSpike(excitation_neuron, 1001.0)

# Queue delayed spikes (2 time units delay)
method_neuron = nlm.NeuronId(3)
event_queue.queueDelayedSpike(method_neuron, 1002.0, 2)

# Register event handlers
def on_spike(spike_event):
    print(f"Spike at neuron {spike_event.neuron_id} at time {spike_event.timestamp}")

event_queue.registerSpikeHandler(on_spike)

def on_delayed_spike(spike_event):
    print(f"Delayed spike at neuron {spike_event.neuron_id} with delay {spike_event.delay}")

event_queue.registerDelayedSpikeHandler(on_delayed_spike)

# Process events
current_time = 1000.0
event_queue.processSpikes(current_time)
event_queue.processDelayedSpikes()

# Check event history
history = event_queue.getEventHistory()
print(f"Processed {len(history)} events")

# Event statistics
stats = event_queue.getEventStatistics()
print(f"Statistics: {stats}")
```

**Event Types:**
- Spike events: Immediate neural spikes
- Delayed spike events: Spikes with propagation delays
- Each event contains neuron ID, timestamp, and additional metadata

## Performance Profiler

### Comprehensive Performance Monitoring

```python
import pynlm as nlm
from pynlm import PerformanceProfiler
import time

# Create performance profiler
profiler = PerformanceProfiler(
    name="brain_simulation",
    record_fps=True,
    record_memory=True,
    record_custom_metrics=["spike_count", "neuron_activity", "synaptic_efficiency"]
)

# Start profiling
profiler.start()

# Run simulation
brain = nlm.createBrainWithDefaultConfig()
brain.initialize()

for step in range(1000):
    brain.step(step, step * 0.001)
    
    # Record custom metrics
    profiler.recordMetric("spike_count", brain.getTotalSpikeCount())
    profiler.recordMetric("neuron_activity", brain.getActiveNeuronCount())
    profiler.recordMetric("synaptic_efficiency", brain.getExcitationInhibitionRatio())

# Stop profiling
profiler.stop()

# Get performance metrics
metrics = profiler.getMetrics()

print(f"Performance Summary:")
print(f"  Total frames: {metrics['frames']}")
print(f"  Average FPS: {metrics['average_fps']:.2f}")
print(f"  Total time: {metrics['total_time']:.3f}s")
print(f"  Average time per step: {metrics['average_step_time']*1000:.2f}ms")
print(f"  Memory usage: {metrics['memory_usage_mb']:.2f} MB")

# Get detailed metrics
detailed = profiler.getDetailedMetrics()
print(f"\nDetailed Metrics:")
for key, values in detailed.items():
    if values:
        print(f"  {key}: {len(values)} data points")

# Compare with baseline
baseline = profiler.getBaseline()
if baseline:
    print(f"\nComparison with baseline:")
    print(f"  FPS improvement: {metrics['average_fps'] / baseline['average_fps']:.2f}x")
    print(f"  Time reduction: {(baseline['total_time'] - metrics['total_time']) / baseline['total_time'] * 100:.1f}%")
```

**Profiler Features:**
- Frame rate monitoring
- Memory usage tracking
- Custom metric recording
- Baseline comparison
- Detailed time series data
- Statistical analysis

## Advanced Factory Methods

### Specialized Brain Creation

```python
import pynlm as nlm

# 1. Ultra-fast minimal brain (for testing)
def createMinimalBrain():
    brain = nlm.createBrainWithDefaultConfig()
    config = brain.getConfig()
    
    # Minimize for speed
    config.set("brain.region_count", 1)
    config.set("brain.population_size", 50)
    config.set("brain.connection_probability", 0.05)
    
    brain.reset()
    brain.initialize()
    return brain

# 2. High-precision simulation (for accuracy)
def createHighPrecisionBrain():
    brain = nlm.createBrainWithDefaultConfig()
    config = brain.getConfig()
    
    # Increase precision
    config.set("simulation.steps_per_second", 1000)
    config.set("brain.population_size", 500)
    config.set("brain.connection_probability", 0.2)
    config.set("brain.enable_structural_plasticity", True)
    config.set("brain.enable_reward_modulation", True)
    config.set("brain.enable_curiosity", True)
    config.set("brain.enable_novelty_detection", True)
    
    brain.reset()
    brain.initialize()
    return brain

# 3. Development-focused brain
def createDevelopmentBrain():
    brain = nlm.createBrainWithDefaultConfig()
    config = brain.getConfig()
    
    # Development settings
    config.set("brain.development.stage", "CriticalPeriod")
    config.set("brain.enable_structural_plasticity", True)
    config.set("brain.enable_development", True)
    config.set("brain.enable_curiosity", True)
    config.set("brain.enable_novelty_detection", True)
    
    brain.reset()
    brain.initialize()
    return brain

# 4. Memory-focused brain
def createMemoryBrain():
    brain = nlm.createBrainWithDefaultConfig()
    config = brain.getConfig()
    
    # Memory settings
    config.set("brain.enable_working_memory", True)
    config.set("brain.enable_episodic_memory", True)
    config.set("brain.enable_associative_memory", True)
    config.set("brain.memory.capacity", 10000)
    
    brain.reset()
    brain.initialize()
    return brain
```

### Sensory Pipeline Factory

```python
import pynlm as nlm

def createVisionPipeline(width=30, height=30, channels=3):
    """Create a vision-only sensory pipeline."""
    world = nlm.createWorldWithSetup(width=300, height=300)
    
    # Create specialized config for vision
    config = nlm.Config()
    config.set("sensory.vision.enabled", True)
    config.set("sensory.vision.width", width)
    config.set("sensory.vision.height", height)
    config.set("sensory.vision.channels", channels)
    
    config.set("sensory.audio.enabled", False)
    config.set("sensory.internal.enabled", False)
    
    brain = nlm.Brain(config)
    brain.initialize()
    
    agent = nlm.AgentBrain(brain)
    agent.initialize(world)
    
    return agent

def createAudioPipeline(sample_rate=44100, num_samples=2048):
    """Create an audio-only sensory pipeline."""
    world = nlm.createWorldWithSetup(width=200, height=200)
    
    # Create specialized config for audio
    config = nlm.Config()
    config.set("sensory.vision.enabled", False)
    config.set("sensory.audio.enabled", True)
    config.set("sensory.audio.sampleRate", sample_rate)
    config.set("sensory.audio.numSamples", num_samples)
    config.set("sensory.internal.enabled", False)
    
    brain = nlm.Brain(config)
    brain.initialize()
    
    agent = nlm.AgentBrain(brain)
    agent.initialize(world)
    
    return agent, world

def createMultisensoryPipeline():
    """Create a complete multisensory pipeline."""
    world = nlm.createWorldWithSetup(width=400, height=400)
    
    # High-performance multisensory config
    config = nlm.Config()
    config.set("sensory.vision.enabled", True)
    config.set("sensory.vision.width", 40)
    config.set("sensory.vision.height", 40)
    config.set("sensory.vision.channels", 3)
    
    config.set("sensory.audio.enabled", True)
    config.set("sensory.audio.sampleRate", 48000)
    config.set("sensory.audio.numSamples", 4096)
    
    config.set("sensory.internal.enabled", True)
    
    config.set("brain.enable_structural_plasticity", True)
    config.set("brain.enable_reward_modulation", True)
    config.set("brain.enable_curiosity", True)
    config.set("brain.enable_novelty_detection", True)
    config.set("brain.enable_prediction_system", True)
    config.set("brain.enable_cognitive_planning", True)
    
    brain = nlm.Brain(config)
    brain.initialize()
    
    agent = nlm.AgentBrain(brain)
    agent.initialize(world)
    
    return agent, world, config
```

## Neural Network Builder

### Custom Network Architecture

```python
import pynlm as nlm
from pynlm import NeuralNetworkBuilder

# Create network builder
builder = NeuralNetworkBuilder()

# 1. Layered Feed-Forward Network
layered_network = builder.createLayeredNetwork(
    input_size=10,
    hidden_sizes=[32, 64, 32],
    output_size=5,
    activation="relu",
    output_activation="softmax"
)
print("Created layered feed-forward network")
print(f"  Input: 10 -> Hidden: {layered_network.getLayerSizes()} -> Output: 5")

# 2. Autoencoder
autoencoder = builder.createAutoencoder(
    input_size=20,
    hidden_size=8,
    encoding_activation="relu",
    decoding_activation="sigmoid"
)
print("Created autoencoder network")
print(f"  Input/Output: 20 -> Encoding: 8 -> Output: 20")

# 3. Recurrent Neural Network
rnn = builder.createRNN(
    input_size=15,
    hidden_size=50,
    output_size=3,
    num_layers=2,
    sequence_length=10
)
print("Created RNN network")
print(f"  Input: 15 -> Hidden: 50 (x{2}) -> Output: 3")

# 4. Long Short-Term Memory Network
lstm = builder.createLSTM(
    input_size=12,
    hidden_size=64,
    output_size=4,
    num_layers=3,
    sequence_length=20
)
print("Created LSTM network")
print(f"  Input: 12 -> Hidden: 64 (x{3}) -> Output: 4")

# 5. Custom Network with Special Topology
def createSpecialTopology():
    # Create brain with special connectivity pattern
    brain = nlm.createBrainWithDefaultConfig()
    config = brain.getConfig()
    
    # Configure special topology
    config.set("brain.region_count", 3)
    config.set("brain.population_size", 100)
    
    # Create special connectivity patterns
    # (This would require custom initialization code)
    brain.reset()
    brain.initialize()
    return brain

special_network = createSpecialTopology()
print("Created special topology network")
print(f"  Regions: {special_network.getRegionCount()}")
print(f"  Total neurons: {special_network.getTotalNeuronCount()}")
```

## Batch Processing Utilities

### Efficient Simulation Batches

```python
import pynlm as nlm
import time

brain = nlm.createBrainWithDefaultConfig()
brain.initialize()

print("Testing batch processing performance...")

# Method 1: Individual steps
start_time = time.time()
for step in range(1000):
    brain.step(step, step * 0.001)
individual_time = time.time() - start_time
print(f"Individual steps: {individual_time:.3f}s ({individual_time/1000*1000:.2f}ms per step)")

# Reset brain
brain.reset()
brain.initialize()

# Method 2: Batch steps
start_time = time.time()
brain.batchStep(1000)  # Process 1000 steps at once
batch_time = time.time() - start_time
print(f"Batch steps: {batch_time:.3f}s ({batch_time/1000*1000:.2f}ms per step)")
print(f"Speedup: {individual_time/batch_time:.2f}x")

# Method 3: Vectorized activity processing
brain.reset()
brain.initialize()

start_time = time.time()
for step in range(500):
    # Process in batches of 50
    if step % 50 == 0:
        brain.vectorizeNeuronActivity()
    brain.step(step, step * 0.001)
vectorized_time = time.time() - start_time
print(f"Vectorized processing: {vectorized_time:.3f}s ({vectorized_time/500*1000:.2f}ms per step)")
print(f"Speedup vs individual: {individual_time/vectorized_time:.2f}x")
```

### Neural Network Optimization

```python
import pynlm as nlm

brain = nlm.createBrainWithDefaultConfig()
brain.initialize()

print("Optimizing neural network...")

# Get initial statistics
initial_stats = brain.computeNeuralStats()
print(f"Initial stats: {initial_stats}")

# Apply optimization techniques
brain.optimizeConnectivity()
brain.normalizeWeights()
brain.computeNeuralStats()  # Recompute after optimization

# Analyze optimization effects
print("Network optimization completed")
print(f"Optimization type: Connectivity + Weight normalization")
```

## Experiment Tracking

### Comprehensive Experiment Management

```python
import pynlm as nlm
from pynlm import ExperimentTracker
import json
import time
from datetime import datetime

# Create experiment tracker
experiment = ExperimentTracker(
    name="neural_learning_experiment",
    description="Testing NLM learning capabilities",
    tags=["learning", "neuroscience", "ai"],
    metadata={
        "researcher": "Dr. Smith",
        "institution": "Neural Research Institute",
        "version": "1.0"
    }
)

# Start experiment
experiment.start()
experiment.recordMetric("configuration", {
    "neuron_count": 100,
    "region_count": 1,
    "population_size": 100
})

# Run experimental phases
brain = nlm.createBrainWithDefaultConfig()
brain.initialize()

# Phase 1: Baseline
print("Phase 1: Baseline simulation")
for step in range(100):
    brain.step(step, step * 0.001)
    experiment.recordStepData({
        "phase": "baseline",
        "step": step,
        "spike_count": brain.getTotalSpikeCount(),
        "active_neurons": brain.getActiveNeuronCount(),
        "firing_rate": brain.getAverageFiringRate()
    })

# Phase 2: Learning
print("Phase 2: Learning simulation")
for step in range(200):
    # Inject learning signals
    for i in range(10):
        brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 10.0)
    
    brain.step(step, step * 0.001)
    experiment.recordStepData({
        "phase": "learning",
        "step": step,
        "spike_count": brain.getTotalSpikeCount(),
        "active_neurons": brain.getActiveNeuronCount(),
        "firing_rate": brain.getAverageFiringRate(),
        "plasticity_enabled": True
    })

# Phase 3: Consolidation
print("Phase 3: Consolidation simulation")
for step in range(100):
    # Use memory systems
    if hasattr(brain, 'getWorkingMemory'):
        working_memory = brain.getWorkingMemory()
        working_memory.store(f"step_{step}", {
            "spike_count": brain.getTotalSpikeCount(),
            "firing_rate": brain.getAverageFiringRate()
        })
    
    brain.step(step, step * 0.001)
    experiment.recordStepData({
        "phase": "consolidation",
        "step": step,
        "spike_count": brain.getTotalSpikeCount(),
        "active_neurons": brain.getActiveNeuronCount(),
        "memory_usage": len(working_memory.getAllData()) if hasattr(brain, 'getWorkingMemory') else 0
    })

# End experiment
results = experiment.end()

# Save results
with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
    json.dump(results, f, indent=2, default=str)
    results_file = f.name

print(f"\nExperiment completed!")
print(f"Experiment name: {results['experiment_name']}")
print(f"Duration: {results['duration_seconds']:.2f} seconds")
print(f"Total steps: {results['total_steps']}")
print(f"Metrics recorded: {len(results['metrics'])}")
print(f"Step data points: {len(results['step_data'])}")
print(f"Results saved to: {results_file}")

# Analyze results
print(f"\nAnalysis:")
print(f"  Average spike rate: {results['analysis']['average_spike_rate']:.2f} Hz")
print(f"  Peak activity: {results['analysis']['peak_activity']:.2f}")
print(f"  Learning trend: {results['analysis']['learning_trend']}")
print(f"  Stability score: {results['analysis']['stability_score']:.3f}")

# Clean up
os.unlink(results_file)
```

## Visualization Advanced Features

### Network Visualization Options

```python
import pynlm as nlm
from pynlm import VisualizationInterface

# Create visualization interface
vis = VisualizationInterface(
    backend="matplotlib",
    dpi=150,
    figsize=(12, 8),
    style="neural",
    interactive=True
)

vis.initialize()

# Create and initialize brain
brain = nlm.createBrainWithDefaultConfig()
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step, step * 0.001)

# Generate comprehensive visualization set
print("Generating comprehensive visualization set...")

# 1. Network architecture visualization
network_viz = vis.visualizeNetwork(brain, style="circuit")

# 2. Real-time activity monitoring
activity_viz = vis.visualizeActivity(brain, duration=10.0)

# 3. Spike raster analysis
spike_viz = vis.visualizeSpikes(brain, time_window=5.0)

# 4. Synaptic weight visualization
weight_viz = vis.visualizeWeights(brain, threshold=0.5)

# 5. Development stage visualization
development_viz = vis.visualizeDevelopment(brain, stages=True)

# 6. Neuromodulation levels
 neuromod_viz = vis.visualizeNeuromodulation(brain, show_levels=True)

# Save all visualizations
output_dir = "visualization_results"
vis.saveAll(output_dir, format="png", dpi=150)

print(f"Visualizations saved to: {output_dir}/")
print("Files generated:")
print("  - network_circuit.png")
print("  - activity_monitor.png")
print("  - spike_raster.png")
print("  - weight_matrix.png")
print("  - development_stages.png")
print("  - neuromodulation_levels.png")

vis.close()
```

## Error Handling and Recovery

### Robust Error Management

```python
import pynlm as nlm

def safe_simulation(brain_config, simulation_steps=100):
    """Run simulation with comprehensive error handling."""
    try:
        # Create brain
        brain = nlm.createBrainWithDefaultConfig()
        
        # Apply custom config
        config = brain.getConfig()
        for key, value in brain_config.items():
            config.set(key, value)
        
        # Initialize with error handling
        if not brain.initialize():
            raise RuntimeError("Brain initialization failed")
        
        # Run simulation
        results = []
        for step in range(simulation_steps):
            try:
                brain.step(step, step * 0.001)
                
                # Record metrics with error checking
                results.append({
                    "step": step,
                    "spike_count": brain.getTotalSpikeCount(),
                    "active_neurons": brain.getActiveNeuronCount(),
                    "firing_rate": brain.getAverageFiringRate(),
                    "timestamp": time.time()
                })
                
            except Exception as e:
                print(f"Error at step {step}: {e}")
                # Try to recover
                brain.reset()
                brain.initialize()
                continue
        
        # Save results
        return {
            "success": True,
            "results": results,
            "final_spike_count": brain.getTotalSpikeCount(),
            "final_active_neurons": brain.getActiveNeuronCount()
        }
        
    except Exception as e:
        print(f"Critical error: {e}")
        return {
            "success": False,
            "error": str(e),
            "partial_results": results if 'results' in locals() else []
        }

# Example usage with error handling
def run_experiments():
    """Run multiple experiments with error handling."""
    experiments = [
        {"brain.region_count": 1, "brain.population_size": 50},
        {"brain.region_count": 2, "brain.population_size": 100},
        {"brain.region_count": 3, "brain.population_size": 150},
    ]
    
    all_results = []
    
    for i, config in enumerate(experiments):
        print(f"\nRunning experiment {i+1}: {config}")
        result = safe_simulation(config, simulation_steps=50)
        all_results.append(result)
        
        if result["success"]:
            print(f"  ✓ Experiment {i+1} completed successfully")
            print(f"    Final spike count: {result['final_spike_count']}")
        else:
            print(f"  ✗ Experiment {i+1} failed: {result['error']}")
    
    # Summary
    successful = sum(1 for r in all_results if r["success"])
    print(f"\nExperiment Summary: {successful}/{len(experiments)} successful")
    
    return all_results

# Run experiments
results = run_experiments()
```

## Performance Optimization Guidelines

### Best Practices for High-Performance NLM Applications

```python
import pynlm as nlm
from pynlm import PerformanceProfiler

def optimize_for_speed(brain):
    """Optimize brain for maximum speed."""
    config = brain.getConfig()
    
    # Speed optimizations
    config.set("simulation.steps_per_second", 50)  # Lower timestep
    config.set("brain.population_size", 100)       # Smaller network
    config.set("brain.region_count", 1)            # Single region
    config.set("brain.connection_probability", 0.05)  # Sparse connections
    
    brain.reset()
    brain.initialize()
    
    return brain

def optimize_for_accuracy(brain):
    """Optimize brain for maximum accuracy."""
    config = brain.getConfig()
    
    # Accuracy optimizations
    config.set("simulation.steps_per_second", 1000)  # Higher timestep
    config.set("brain.population_size", 500)           # Larger network
    config.set("brain.region_count", 3)                # Multiple regions
    config.set("brain.connection_probability", 0.2)    # Dense connections
    config.set("brain.enable_structural_plasticity", True)
    config.set("brain.enable_reward_modulation", True)
    config.set("brain.enable_curiosity", True)
    config.set("brain.enable_novelty_detection", True)
    
    brain.reset()
    brain.initialize()
    
    return brain

# Performance comparison
def compare_performance():
    """Compare different optimization strategies."""
    strategies = [
        ("Default", lambda b: b),
        ("Speed", optimize_for_speed),
        ("Accuracy", optimize_for_accuracy),
    ]
    
    print("Performance Comparison:")
    print("=" * 50)
    
    for name, optimizer in strategies:
        brain = nlm.createBrainWithDefaultConfig()
        optimized_brain = optimizer(brain)
        
        profiler = PerformanceProfiler(f"{name}_comparison")
        profiler.start()
        
        # Run simulation
        for step in range(200):
            optimized_brain.step(step, step * 0.001)
        
        profiler.stop()
        
        metrics = profiler.getMetrics()
        print(f"{name:10s}: {metrics['average_fps']:8.1f} FPS, "
              f"{metrics['total_time']:6.3f}s total, "
              f"{metrics['average_step_time']*1000:6.2f}ms per step")

# Memory usage optimization
def optimize_memory_usage():
    """Optimize for minimal memory usage."""
    config = nlm.Config()
    
    # Memory-efficient configuration
    config.set("brain.population_size", 50)
    config.set("brain.region_count", 1)
    config.set("brain.connection_probability", 0.02)
    
    # Use compact data types where possible
    # (This would depend on NLM's memory management)
    
    brain = nlm.Brain(config)
    brain.initialize()
    
    return brain

# Batch processing optimization
def optimize_batch_processing():
    """Optimize for batch processing performance."""
    brain = nlm.createBrainWithDefaultConfig()
    brain.initialize()
    
    # Pre-compile frequently used functions
    # (This would depend on NLM's internal optimizations)
    
    # Use batch operations when possible
    brain.batchStep(1000)  # Process multiple steps at once
    brain.vectorizeNeuronActivity()  # Vectorized operations
    
    return brain
```

## Integration Examples

### Complete Application Pattern

```python
import pynlm as nlm
from pynlm import EventQueue, PerformanceProfiler, ExperimentTracker
from pynlm import VisualizationInterface
import time

class NLMApplication:
    """Complete NLM application with all features."""
    
    def __init__(self, config_file=None):
        self.config_file = config_file
        self.setup_environment()
        self.initialize_components()
        self.setup_monitoring()
        
    def setup_environment(self):
        """Set up application environment."""
        print("Initializing NLM Application...")
        
        # Create world with optimal settings
        self.world = nlm.createWorldWithSetup(
            width=400, height=400,
            visionWidth=40, visionHeight=40
        )
        
        # Setup event queue for real-time updates
        self.event_queue = EventQueue()
        self.setup_event_handlers()
        
    def setup_event_handlers(self):
        """Setup event handlers for neural events."""
        def spike_handler(spike_event):
            # Handle spike events
            self.record_neural_activity(spike_event)
            
        def delayed_spike_handler(spike_event):
            # Handle delayed spike events
            self.update_synaptic_state(spike_event)
            
        self.event_queue.registerSpikeHandler(spike_handler)
        self.event_queue.registerDelayedSpikeHandler(delayed_spike_handler)
        
    def initialize_components(self):
        """Initialize all NLM components."""
        # Create brain with custom configuration
        config = nlm.Config()
        if self.config_file:
            config.loadFromFile(self.config_file)
        
        # Advanced configuration
        config.set("simulation.steps_per_second", 200)
        config.set("brain.region_count", 2)
        config.set("brain.population_size", 200)
        config.set("brain.enable_structural_plasticity", True)
        config.set("brain.enable_reward_modulation", True)
        config.set("brain.enable_curiosity", True)
        config.set("brain.enable_novelty_detection", True)
        
        self.brain = nlm.Brain(config)
        
        # Create agent brain interface
        self.agent_brain = nlm.AgentBrain(self.brain)
        self.agent_brain.initialize(self.world)
        
        # Create visualization interface
        self.visualizer = VisualizationInterface(
            backend="matplotlib",
            interactive=True,
            auto_save=True,
            save_path="application_visualizations"
        )
        self.visualizer.initialize()
        
    def setup_monitoring(self):
        """Setup performance monitoring and experiment tracking."""
        # Performance profiler
        self.profiler = PerformanceProfiler(
            name="nlm_application",
            record_fps=True,
            record_memory=True,
            record_custom_metrics=["spike_count", "active_neurons", "learning_rate"]
        )
        
        # Experiment tracker
        self.experiment_tracker = ExperimentTracker(
            name="nlm_application_experiment",
            description="Complete NLM application demonstration",
            tags=["demo", "integration", "complete"]
        )
        
    def run_simulation(self, duration_steps=1000):
        """Run the main simulation loop."""
        self.profiler.start()
        self.experiment_tracker.start()
        
        print(f"Starting simulation for {duration_steps} steps...")
        
        for step in range(duration_steps):
            # Record experiment start
            if step % 100 == 0:
                self.experiment_tracker.recordStepData({
                    "step": step,
                    "phase": "simulation",
                    "spike_count": self.brain.getTotalSpikeCount(),
                    "active_neurons": self.brain.getActiveNeuronCount()
                })
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent_brain.processSensoryInput(percept)
            
            # Record metrics
            self.profiler.recordMetric("spike_count", self.brain.getTotalSpikeCount())
            self.profiler.recordMetric("active_neurons", self.brain.getActiveNeuronCount())
            
            # Produce action
            motor_command = self.agent_brain.decodeMotorCommand()
            action = nlm.Action(motor_command, [])
            
            # Apply action in world
            result = self.world.applyMotorCommand(action.getType(), step * 0.001)
            
            # Apply neuromodulation
            if result:
                reward = result.reward
                predicted_reward = self.agent_brain.getPredictionError()
                self.agent_brain.applyRewardModulation(reward, predicted_reward)
            
            # Update development
            self.agent_brain.updateDevelopment(0.001)
            
            # Process events
            current_time = step * 0.001
            self.event_queue.processSpikes(current_time)
            self.event_queue.processDelayedSpikes()
            
            # Periodic visualization
            if step % 50 == 0:
                self.visualizer.visualizeNetwork(self.brain)
                self.visualizer.visualizeActivity(self.brain)
            
            # Periodic recording
            if step % 200 == 0:
                self.experiment_tracker.recordMetric("simulation_progress", step / duration_steps)
        
        # Stop monitoring
        self.profiler.stop()
        self.experiment_tracker.end()
        
        # Generate final visualizations
        self.generate_final_visualizations()
        
        print("Simulation completed!")
        
    def generate_final_visualizations(self):
        """Generate final visualizations and analysis."""
        # Network visualization
        self.visualizer.visualizeNetwork(self.brain)
        
        # Activity heatmap
        self.visualizer.visualizeActivity(self.brain)
        
        # Spike statistics
        self.visualizer.visualizeSpikes(self.brain)
        
        # Weight matrix
        self.visualizer.visualizeWeights(self.brain)
        
        # Development stages
        self.visualizer.visualizeDevelopment(self.brain)
        
        # Neuromodulation levels
        self.visualizer.visualizeNeuromodulation(self.brain)
        
        # Save all visualizations
        self.visualizer.saveAll(
            directory="final_results",
            prefix="simulation_",
            formats=["png", "pdf"],
            dpi=300
        )
        
        # Generate analysis report
        self.generate_analysis_report()
        
    def generate_analysis_report(self):
        """Generate comprehensive analysis report."""
        # Get performance metrics
        perf_metrics = self.profiler.getMetrics()
        
        # Get experiment results
        exp_results = self.experiment_tracker.getResults()
        
        # Create analysis report
        analysis = {
            "simulation_summary": {
                "total_steps": exp_results["total_steps"],
                "final_spike_count": self.brain.getTotalSpikeCount(),
                "final_active_neurons": self.brain.getActiveNeuronCount(),
                "average_firing_rate": self.brain.getAverageFiringRate(),
                "excitation_inhibition_ratio": self.brain.getExcitationInhibitionRatio()
            },
            "performance_analysis": {
                "average_fps": perf_metrics['average_fps'],
                "total_time": perf_metrics['total_time'],
                "memory_usage": perf_metrics['memory_usage_mb'],
                "efficiency_score": perf_metrics['efficiency_score']
            },
            "learning_analysis": {
                "learning_trend": exp_results['analysis']['learning_trend'],
                "stability_score": exp_results['analysis']['stability_score'],
                "peak_activity": exp_results['analysis']['peak_activity'],
                "convergence_time": exp_results['analysis']['convergence_time']
            },
            "neuromodulation_analysis": {
                "final_dopamine": self.agent_brain.getNeuromodulationLevel(),
                "final_curiosity": self.agent_brain.getCuriosityLevel(),
                "final_novelty": self.agent_brain.getNoveltyLevel(),
                "final_prediction_error": self.agent_brain.getPredictionError()
            }
        }
        
        # Save analysis report
        with open("analysis_report.json", 'w') as f:
            json.dump(analysis, f, indent=2, default=str)
        
        print(f"Analysis report saved to: analysis_report.json")
        
    def cleanup(self):
        """Clean up all resources."""
        print("Cleaning up resources...")
        
        if hasattr(self, 'visualizer'):
            self.visualizer.close()
            
        if hasattr(self, 'agent_brain'):
            self.agent_brain.reset()
            
        if hasattr(self, 'brain'):
            self.brain.reset()
            
        print("Cleanup completed!")

# Example usage
if __name__ == "__main__":
    # Create application
    app = NLMApplication()
    
    try:
        # Run simulation
        app.run_simulation(duration_steps=500)
        
    except KeyboardInterrupt:
        print("\nSimulation interrupted by user")
        
    except Exception as e:
        print(f"Simulation error: {e}")
        
    finally:
        # Clean up
        app.cleanup()
```

This document provides comprehensive information about all advanced features added to the NLM Python bindings, enabling developers to build sophisticated neural simulation applications with ease and efficiency.
