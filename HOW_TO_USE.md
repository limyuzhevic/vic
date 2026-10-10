# NLM (Neural Learning Machine) - ENHANCED HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. This enhanced version provides a comprehensive Python interface with advanced features for both beginners and power users.

This document covers:
1. Enhanced Python API usage
2. Building the C++ project from source
3. Installing the enhanced Python library
4. Using advanced NLM Python features

---

## Part 1: Enhanced Python API

### 1.1 Enhanced Class Bindings

The enhanced Python bindings now include:

#### Enhanced Brain Class
```python
import pynlm

# Create brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()

# Enhanced step methods with callbacks
brain.step(step_number)                    # Basic step
brain.step(step_number, current_time)       # Step with timestamp

# Performance monitoring
metrics = brain.getPerformanceMetrics()
print(f"Firing neurons: {metrics.firing_neuron_count}")
print(f"Average firing rate: {metrics.average_firing_rate} Hz")
print(f"Total spikes: {metrics.total_spike_count}")

# Advanced batch processing
steps_completed = brain.batchStep(range(1000))
```

#### Enhanced AgentBrain Class
```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent with world
agent.initialize(world)

# Enable all learning systems
agent.enableAllSystems(True)

# Enhanced sensory processing
agent.processSensoryInput(percept)           # Process single percept
agent.processSensoryBatch(percepts)         # Process multiple percepts

# Enhanced action selection
action = agent.decodeMotorCommand()
neuromod_state = agent.getFullNeuromodulationState()

# Advanced learning
agent.applyLearningBatch([0.5, 0.8, 0.3], [0.6, 0.7, 0.4])
```

#### Enhanced SimpleWorld Class
```python
import pynlm

# Create and configure enhanced world
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8,
    energyDecayRate=0.01,
    maxEnergy=100.0
)

# Enhanced world operations
world.reset()
percepts = world.batchUpdate([0.1, 0.2, 0.3, 0.4, 0.5])
world_stats = world.getWorldStats()
print(f"World dimensions: {world_stats.width}x{world_stats.height}")
print(f"Total objects: {world_stats.total_objects}")
```

#### Enhanced Configuration Class
```python
import pynlm

# Create enhanced configuration
config = pynlm.createDefaultConfig()

# Pythonic setter/getter methods
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)

# Get configuration values
neuron_count = config.get("brain.neuron_count")
stdp_enabled = config.get("plasticity.stdp.enable")

# Convert to dictionary for machine learning
config_dict = config.to_dict()
print(config_dict)
```

### 1.2 New Enhanced Features

#### 1.2.1 Context Managers for Resource Management

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Use context manager for automatic resource management
with pynlm.simulation_guard(brain, auto_save=True, backup_path="checkpoint") as guard:
    # Run simulation with automatic backup
    for step in range(1000):
        brain.step(step)
        
        # Access simulation statistics
        steps_completed = guard.getCurrentStep()
        spikes_generated = guard.getSpikeCountDelta()
    
    # Context manager automatically restores from backup
print("Simulation completed with automatic resource management")
```

#### 1.2.2 Batch Processor for Efficient Operations

```python
import pynlm

# Create multiple brains for batch processing
config = pynlm.createDefaultConfig()
num_brains = 5
brains = [pynlm.createBrain(config) for _ in range(num_brains)]

# Initialize all brains
for brain in brains:
    brain.initialize()

# Use batch processor for efficient processing
with pynlm.batch_processor(brains) as processor:
    # Process multiple steps efficiently
    steps = processor.batchStep(range(1000))
    print(f"Completed {len(steps)} batch steps efficiently")
```

#### 1.2.3 Performance Profiler

```python
import pynlm
import time

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Start performance profiling
profiler = pynlm.PerformanceProfiler()
profiler.start()

# Run simulation with profiling
for step in range(1000):
    start_time = time.time()
    brain.step(step)
    end_time = time.time()
    profiler.recordStep((end_time - start_time) * 1000)  # Convert to milliseconds

profiler.stop()

# Get performance metrics
metrics = profiler.getMetrics()
print(f"Steps per second: {metrics.steps_per_second:.0f}")
print(f"Average step time: {metrics.avg_step_time_ms:.2f} ms")
print(f"Total simulation time: {metrics.total_time_ms:.0f} ms")
```

#### 1.2.4 Feature Extraction for Machine Learning

```python
import pynlm

# Create complete agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Extract comprehensive features for ML
features = agent.extractFeatures()
print("Extracted features for machine learning:")
for key, value in features.items():
    print(f"  {key}: {value}")

# Get neuromodulation state for advanced analysis
neuromod_state = agent.getFullNeuromodulationState()
print(f"Neuromodulation state: {neuromod_state}")
```

#### 1.2.5 Enhanced Checkpoint Management

```python
import pynlm
import os

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create checkpoint directory
checkpoint_dir = "enhanced_checkpoints"
os.makedirs(checkpoint_dir, exist_ok=True)

# Run simulation with enhanced checkpointing
with pynlm.simulation_guard(brain, auto_save=True, backup_path=checkpoint_dir) as guard:
    # Run simulation
    for step in range(1000):
        brain.step(step)
        
        # Save checkpoint every 250 steps
        if step % 250 == 0:
            checkpoint_path = brain.saveCheckpoint(checkpoint_dir, step)
            print(f"Checkpoint saved: {checkpoint_path}")
    
    print(f"Simulation completed: {guard.getCurrentStep()} steps")

# Load checkpoint
latest_checkpoint = f"{checkpoint_dir}/checkpoint_step_750.bin"
if os.path.exists(latest_checkpoint):
    brain.loadCheckpoint(latest_checkpoint)
    print(f"Loaded checkpoint: {latest_checkpoint}")
```

### 1.3 NumPy Integration

```python
import pynlm
import numpy as np

# Convert Python lists to numpy arrays
python_list = [1.0, 2.0, 3.0, 4.0, 5.0]
numpy_array = pynlm.numpy_array(python_list)
print(f"NumPy array shape: {numpy_array.shape}")

# Convert numpy arrays to Python lists
converted_list = pynlm.vector_to_list(numpy_array)
print(f"Converted list: {converted_list}")
```

### 1.4 Visualization Utilities

```python
import pynlm

# Create brain for visualization
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for visualization
for step in range(100):
    brain.step(step)

# Visualize neural activity
pynlm.VisualizationUtils.plot_neural_activity(brain, 100)

# Visualize spike raster
pynlm.VisualizationUtils.plot_spike_raster(brain, 100)

# Visualize connectivity
pynlm.VisualizationUtils.plot_connectivity(brain)
```

---

## Part 2: Enhanced Building and Installation

### 2.1 Enhanced Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for enhanced Python bindings)
- **pybind11**: Version 2.11.0+ (for enhanced Python bindings)
- **NumPy**: Required for enhanced NumPy integration
- **Matplotlib**: Optional for enhanced visualization utilities

### 2.2 Enhanced Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Install Python dependencies for enhanced features
pip install numpy matplotlib scipy pandas scikit-learn

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (enhanced for performance)
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DPYTHON_EXECUTABLE=$(which python) \
         -DENHANCED_PYTHON_BINDINGS=ON \
         -DENABLE_VISUALIZATION_TOOLS=ON \
         -DENABLE_PERFORMANCE_PROFILING=ON

# Build all targets with enhanced features
make -j$(nproc)

# Run enhanced tests
ctest --output-on-failure
```

### 2.3 Enhanced Installation Options

#### 2.3.1 From Source (Recommended for Development)

```bash
# Install build dependencies for enhanced features
pip install pybind11 scikit-build-core pytest numpy matplotlib scipy pandas

# Build and install with enhanced features
pip install .

# Or install in development mode
pip install -e .
```

#### 2.3.2 Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### 2.4 Enhanced Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_enhanced` | Executable | Enhanced simulation with Python bindings |
| `pynlm` | Python module | Enhanced Python bindings |
| `nlm_test` | Executable | Unit test suite |
| `nlm_performance_profiler` | Library | Performance monitoring tools |
| `nlm_visualization` | Library | Visualization utilities |

### 2.5 Enhanced Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENHANCED_FEATURES=ON

# Enhanced build with Python bindings
cmake .. -DPYTHON_EXECUTABLE=g++ -DENHANCED_PYTHON_BINDINGS=ON \
         -DENABLE_PERFORMANCE_PROFILING=ON -DENABLE_VISUALIZATION_TOOLS=ON

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local -DENHANCED_INSTALL=ON
make install
```

---

## Part 3: Enhanced Configuration Options

### 3.1 Enhanced Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### 3.2 Enhanced Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### 3.3 Enhanced Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.prediction_error.scale` | float | 0.5 | Prediction error modulation scale |

### 3.4 Enhanced Performance Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `performance.memory_pool_size` | int | 1000000 | Memory pool size for optimization |
| `performance.event_queue_size` | int | 100000 | Event queue size |
| `performance.enable_profiling` | bool | true | Enable performance profiling |
| `performance.enable_parallel` | bool | false | Enable parallel processing |

---

## Part 4: Advanced Examples

### 4.1 Advanced Simulation with Context Manager

```python
import pynlm
import os

def run_advanced_simulation(config_file=None, backup_enabled=True):
    """Run an advanced simulation with context manager and checkpointing."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Load configuration from file if specified
    if config_file:
        config.loadFromFile(config_file)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced learning systems
    agent.enableAllSystems(True)
    
    # Create checkpoint directory
    if backup_enabled:
        os.makedirs("advanced_checkpoints", exist_ok=True)
    
    # Run simulation with enhanced context manager
    with pynlm.simulation_guard(brain, backup_enabled, "advanced_checkpoints") as guard:
        for step in range(2000):
            # Update world
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = agent.getNeuromodulationLevel()
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Save checkpoint every 500 steps
            if step % 500 == 0:
                checkpoint_path = brain.saveCheckpoint("advanced_checkpoints", step)
                print(f"Checkpoint saved at step {step}: {checkpoint_path}")
            
            # Print progress
            if step % 200 == 0:
                metrics = brain.getPerformanceMetrics()
                neuromod_state = agent.getFullNeuromodulationState()
                print(f"Step {step}:")
                print(f"  Firing neurons: {metrics.firing_neuron_count}")
                print(f"  Curiosity: {neuromod_state.curiosity:.3f}")
                print(f"  Novelty: {neuromod_state.novelty:.3f}")
                print(f"  Dev Stage: {neuromod_state.developmental_stage}")
    
    # Get final statistics
    final_metrics = brain.getPerformanceMetrics()
    final_neuromod = agent.getFullNeuromodulationState()
    
    return {
        "total_steps": 2000,
        "total_spikes": final_metrics.total_spike_count,
        "final_firing_rate": final_metrics.average_firing_rate,
        "final_curiosity": final_neuromod.curiosity,
        "final_novelty": final_neuromod.novelty,
        "checkpoint_dir": "advanced_checkpoints" if backup_enabled else None
    }

# Run advanced simulation
results = run_advanced_simulation("advanced_config.json", backup_enabled=True)
print("Advanced simulation completed!")
print(results)
```

### 4.2 Research Experiment with Batch Processing

```python
import pynlm
import numpy as np
import pandas as pd

def run_research_experiment(config, num_replicates=20, steps_per_replicate=1000):
    """Run multiple replicates of an experiment for statistical analysis."""
    
    print(f"Running research experiment with {num_replicates} replicates...")
    
    # Create batch processor for efficiency
    with pynlm.batch_processor([pynlm.createBrain(config) for _ in range(num_replicates)]) as processor:
        brains = processor.brains
        
        # Create common world template
        world_template = {
            "width": 20,
            "height": 20,
            "visionWidth": 8,
            "visionHeight": 8,
            "visionWidth": 8,
            "visionHeight": 8
        }
        
        # Run experiment for all replicates
        results = []
        for i in range(num_replicates):
            # Create fresh world for each replicate
            world = pynlm.createSimpleWorld()
            world.configure(**world_template)
            world.reset()
            
            # Create agent for this replicate
            brain = brains[i]
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable advanced learning
            agent.enableAllSystems(True)
            
            # Run simulation
            for step in range(steps_per_replicate):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply learning
                reward = agent.getNeuromodulationLevel()
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
            
            # Collect comprehensive results
            features = agent.extractFeatures()
            features["replicate"] = i
            features["config_id"] = id(config)
            
            # Add performance metrics
            perf_metrics = brain.getPerformanceMetrics()
            features["firing_rate"] = perf_metrics.average_firing_rate
            features["total_spikes"] = perf_metrics.total_spike_count
            features["ei_ratio"] = perf_metrics.excitation_inhibition_ratio
            
            # Add neuromodulation state
            neuromod_state = agent.getFullNeuromodulationState()
            features["final_curiosity"] = neuromod_state.curiosity
            features["final_novelty"] = neuromod_state.novelty
            features["final_prediction_error"] = neuromod_state.prediction_error
            
            results.append(features)
            
            if (i + 1) % 5 == 0:
                print(f"Completed {i + 1}/{num_replicates} replicates")
        
        # Convert to DataFrame for analysis
        df = pd.DataFrame(results)
        
        # Calculate summary statistics
        summary = {
            "mean_firing_rate": df["firing_rate"].mean(),
            "std_firing_rate": df["firing_rate"].std(),
            "mean_total_spikes": df["total_spikes"].mean(),
            "std_total_spikes": df["total_spikes"].std(),
            "mean_curiosity": df["final_curiosity"].mean(),
            "mean_novelty": df["final_novelty"].mean(),
            "n_replicates": len(df)
        }
        
        return df, summary

# Create research configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

# Run research experiment
df, summary = run_research_experiment(config, num_replicates=20, steps_per_replicate=1000)

print("Research Experiment Results:")
print(f"Number of replicates: {summary['n_replicates']}")
print(f"Mean firing rate: {summary['mean_firing_rate']:.2f} Hz")
print(f"Firing rate std: {summary['std_firing_rate']:.2f} Hz")
print(f"Mean total spikes: {summary['mean_total_spikes']:.0f}")
print(f"Total spikes std: {summary['std_total_spikes']:.0f}")
print(f"Mean curiosity: {summary['mean_curiosity']:.3f}")
print(f"Mean novelty: {summary['mean_novelty']:.3f}")

# Save results to CSV for further analysis
df.to_csv("research_results.csv", index=False)
print("Results saved to research_results.csv")
```

### 4.3 Performance Monitoring and Profiling

```python
import pynlm
import time
import matplotlib.pyplot as plt

def profile_simulation_performance(config, steps=1000):
    """Profile simulation performance with detailed metrics."""
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create profiler
    profiler = pynlm.PerformanceProfiler()
    profiler.start()
    
    # Run simulation with detailed timing
    step_times = []
    spike_counts = []
    
    for step in range(steps):
        step_start = time.time()
        
        # Record spike count before step
        spike_counts.append(brain.getTotalSpikeCount())
        
        # Run step
        brain.step(step)
        
        step_end = time.time()
        step_time = (step_end - step_start) * 1000  # Convert to milliseconds
        step_times.append(step_time)
        
        profiler.recordStep(step_time)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: {step_time:.2f} ms")
    
    profiler.stop()
    
    # Get profiler metrics
    perf_metrics = profiler.getMetrics()
    
    # Get brain metrics
    brain_metrics = brain.getPerformanceMetrics()
    
    # Create visualization
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 10))
    
    # Plot step times
    ax1.plot(step_times)
    ax1.set_title("Step Execution Times")
    ax1.set_xlabel("Step")
    ax1.set_ylabel("Time (ms)")
    ax1.grid(True)
    
    # Plot spike counts
    ax2.plot(spike_counts)
    ax2.set_title("Cumulative Spike Count")
    ax2.set_xlabel("Step")
    ax2.set_ylabel("Total Spikes")
    ax2.grid(True)
    
    # Performance metrics bar chart
    performance_data = {
        'Steps per second': perf_metrics.steps_per_second,
        'Average step time (ms)': perf_metrics.avg_step_time_ms,
        'Min step time (ms)': perf_metrics.min_step_time_ms,
        'Max step time (ms)': perf_metrics.max_step_time_ms
    }
    
    bars = ax3.bar(performance_data.keys(), performance_data.values())
    ax3.set_title("Performance Metrics")
    ax3.set_ylabel("Value")
    ax3.tick_params(axis='x', rotation=45)
    
    # Add value labels on bars
    for bar in bars:
        height = bar.get_height()
        ax3.text(bar.get_x() + bar.get_width()/2., height,
                f'{height:.1f}', ha='center', va='bottom')
    
    # Brain metrics
    brain_data = {
        'Neurons': brain_metrics.total_neuron_count,
        'Synapses': brain_metrics.total_synapse_count,
        'Firing': brain_metrics.firing_neuron_count,
        'Active': brain_metrics.active_neuron_count
    }
    
    bars2 = ax4.bar(brain_data.keys(), brain_data.values())
    ax4.set_title("Brain Configuration")
    ax4.set_ylabel("Count")
    ax4.tick_params(axis='x', rotation=45)
    
    # Add value labels on bars
    for bar in bars2:
        height = bar.get_height()
        ax4.text(bar.get_x() + bar.get_width()/2., height,
                f'{height}', ha='center', va='bottom')
    
    plt.tight_layout()
    plt.savefig('simulation_performance.png')
    print("Performance visualization saved as simulation_performance.png")
    
    return {
        "profiler_metrics": perf_metrics,
        "brain_metrics": brain_metrics,
        "step_times": step_times,
        "spike_counts": spike_counts,
        "performance_visualization": "simulation_performance.png"
    }

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Run performance profiling
results = profile_simulation_performance(config, steps=1000)
print("Performance profiling completed!")
```

---

## Part 5: Troubleshooting and Common Issues

### 5.1 Enhanced Troubleshooting Guide

#### "Performance is slow"

**Solutions:**
```python
# Use batch processing for better performance
brain.batchStep(range(1000))  # Much faster than loop

# Optimize configuration
config.set("brain.neuron_count", 500)  # Reduce neurons
config.set("performance.memory_pool_size", 2000000)  # Increase memory pool
```

#### "Memory usage is high"

**Solutions:**
```python
# Monitor memory usage
import psutil
process = psutil.Process()
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")

# Use context managers for automatic cleanup
with pynlm.simulation_guard(brain):
    # Simulation runs with automatic cleanup
```

#### "Simulation crashes or segfaults"

**Solutions:**
```python
# Run with minimal configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)  # Start with few neurons

# Use checkpoints for recovery
with pynlm.simulation_guard(brain, auto_save=True, backup_path="recovery"):
    try:
        # Run simulation
        for step in range(1000):
            brain.step(step)
    except Exception as e:
        print(f"Error occurred: {e}")
        # Context manager automatically restores from backup
```

### 5.2 Enhanced Error Messages and Exceptions

```python
import pynlm

# Examples of different exception types

try:
    # File not found error
    config = pynlm.createDefaultConfig()
    config.loadFromFile("nonexistent_file.json")
except pynlm.PythonIOError as e:
    print(f"IO Error: {e}")

try:
    # Invalid parameter error
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", -1)  # Invalid value
except pynlm.PythonValueError as e:
    print(f"Value Error: {e}")

try:
    # Runtime error
    brain = pynlm.createBrain(config)
    brain.initialize()
    # Try to step without initializing properly
    brain.step(0)
except pynlm.PythonRuntimeError as e:
    print(f"Runtime Error: {e}")
```

---

## Part 6: Advanced Features

### 6.1 Machine Learning Integration

The enhanced Python bindings provide comprehensive support for machine learning:

#### Feature Extraction for ML Models
```python
import pynlm
import numpy as np
from sklearn.preprocessing import StandardScaler

# Create agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Extract features for ML
features = agent.extractFeatures()
feature_names = list(features.keys())
feature_values = [features[name] for name in feature_names]

# Scale features for ML
feature_array = np.array(feature_values).reshape(1, -1)
scaler = StandardScaler()
scaled_features = scaler.fit_transform(feature_array)

print("ML-ready features:")
print(f"Raw features: {feature_values}")
print(f"Scaled features: {scaled_features}")
```

### 6.2 Advanced Visualization

```python
import pynlm
import matplotlib.pyplot as plt
import seaborn as sns

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(500):
    brain.step(step)

# Create advanced visualizations
fig, axes = plt.subplots(2, 3, figsize=(18, 12))

# Spike raster plot
ax = axes[0, 0]
# Implementation would show spike raster
ax.plot([0, 1, 2, 3, 4], [1, 2, 3, 4, 5], 'o-')
ax.set_title("Spike Raster Plot")

# Neural activity over time
ax = axes[0, 1]
# Implementation would show neural activity
ax.plot([0, 100, 200, 300, 400], [0.1, 0.5, 0.8, 0.6, 0.3], 'o-')
ax.set_title("Neural Activity Over Time")

# Synaptic weight distribution
ax = axes[0, 2]
# Implementation would show synaptic weight distribution
import numpy as np
weights = np.random.normal(0.5, 0.2, 1000)
ax.hist(weights, bins=30, alpha=0.7)
ax.set_title("Synaptic Weight Distribution")

# Firing rate histogram
ax = axes[1, 0]
# Implementation would show firing rate distribution
ax.hist(np.random.normal(5, 2, 500), bins=20, alpha=0.7)
ax.set_title("Firing Rate Distribution")

# Attention weights (if available)
ax = axes[1, 1]
# Implementation would show attention weights
ax.plot([0, 1, 2, 3, 4], [0.8, 0.6, 0.9, 0.7, 0.85], 'o-')
ax.set_title("Attention Weights")

# Development stages over time
ax = axes[1, 2]
# Implementation would show development stages
stages = ['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging']
ax.plot([0, 1, 2, 3, 4], [0, 0.3, 0.6, 0.9, 1.0], 'o-')
ax.set_xticks([0, 1, 2, 3, 4])
ax.set_xticklabels(stages)
ax.set_title("Development Stage Progression")

plt.tight_layout()
plt.savefig('advanced_visualization.png')
print("Advanced visualization saved as advanced_visualization.png")
```

### 6.3 Performance Optimization

```python
import pynlm
import time
import cProfile
import pstats

def profile_function(func, *args, **kwargs):
    """Profile a function and return results."""
    profiler = cProfile.Profile()
    profiler.enable()
    
    start_time = time.time()
    result = func(*args, **kwargs)
    end_time = time.time()
    
    profiler.disable()
    
    # Print profiling results
    stats = pstats.Stats(profiler)
    stats.sort_stats('cumulative')
    stats.print_stats(10)  # Top 10 functions
    
    return {
        "result": result,
        "execution_time": end_time - start_time,
        "profiler_stats": stats
    }

def run_optimized_simulation(config, steps=1000):
    """Run optimized simulation with performance monitoring."""
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Profile batch step function
    def batch_simulation():
        return brain.batchStep(range(steps))
    
    # Run optimized simulation
    results = profile_function(batch_simulation)
    
    print(f"Optimized simulation completed in {results['execution_time']:.2f} seconds")
    print(f"Steps per second: {steps / results['execution_time']:.0f}")
    
    return results

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)

# Run optimized simulation
results = run_optimized_simulation(config, steps=1000)
```

---

## Part 7: Developer Guide

### 7.1 Extending the Enhanced API

#### Creating Custom Brain Extensions
```python
import pynlm

class CustomBrain(pynlm.Brain):
    """Custom brain extension with additional functionality."""
    
    def __init__(self, config):
        super().__init__(config)
        self.custom_neurons = []
        self.custom_plasticity_rules = []
    
    def add_custom_neuron(self, neuron_type, params):
        """Add custom neuron type."""
        self.custom_neurons.append({
            'type': neuron_type,
            'params': params,
            'id': len(self.custom_neurons)
        })
    
    def enable_custom_plasticity(self, rule_name, params):
        """Enable custom plasticity rule."""
        self.custom_plasticity_rules.append({
            'name': rule_name,
            'params': params
        })
    
    def get_custom_metrics(self):
        """Get custom metrics for the brain."""
        return {
            'custom_neuron_count': len(self.custom_neurons),
            'plasticity_rules': len(self.custom_plasticity_rules),
            'enhanced_neuron_activity': self.getTotalSpikeCount() * 1.5  # Example calculation
        }

# Use custom brain
config = pynlm.createDefaultConfig()
custom_brain = CustomBrain(config)
custom_brain.initialize()
custom_brain.add_custom_neuron('EnhancedLIF', {'threshold': -40, 'tau': 25})
custom_brain.enable_custom_plasticity('AdaptiveSTDP', {'rate': 0.002})
```

#### Extending Visualization Utilities
```python
import pynlm
import matplotlib.pyplot as plt

class CustomVisualizationUtils(pynlm.VisualizationUtils):
    """Custom visualization utilities."""
    
    @staticmethod
    def plot_custom_analysis(brain, steps):
        """Plot custom analysis of brain activity."""
        fig, axes = plt.subplots(2, 2, figsize=(15, 12))
        
        # Custom plot 1: Detailed spike analysis
        ax = axes[0, 0]
        spikes = np.random.poisson(brain.getAverageFiringRate(), steps)
        ax.bar(range(steps), spikes, alpha=0.7)
        ax.set_title("Detailed Spike Analysis")
        ax.set_xlabel("Step")
        ax.set_ylabel("Spikes per step")
        
        # Custom plot 2: Network connectivity visualization
        ax = axes[0, 1]
        # Create a simple network visualization
        np.random.seed(42)
        adjacency = np.random.rand(10, 10)
        sns.heatmap(adjacency, ax=ax, annot=True, cmap='viridis')
        ax.set_title("Network Connectivity Heatmap")
        
        # Custom plot 3: Learning dynamics
        ax = axes[1, 0]
        learning_curve = np.cumsum(np.random.rand(steps) * 0.1)
        ax.plot(learning_curve)
        ax.set_title("Learning Dynamics")
        ax.set_xlabel("Step")
        ax.set_ylabel("Learning progress")
        
        # Custom plot 4: Neuromodulation effects
        ax = axes[1, 1]
        neuromodulation = np.random.rand(steps) * 2.0
        ax.plot(neuromodulation)
        ax.set_title("Neuromodulation Effects")
        ax.set_xlabel("Step")
        ax.set_ylabel("Modulation level")
        
        plt.tight_layout()
        plt.savefig('custom_visualization.png')
        print("Custom visualization saved as custom_visualization.png")

# Use custom visualization
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)

CustomVisualizationUtils.plot_custom_analysis(brain, 100)
```

### 7.2 Configuration Extension

```python
import pynlm

class EnhancedConfig(pynlm.Config):
    """Enhanced configuration with additional features."""
    
    def __init__(self):
        super().__init__()
        self.metadata = {}
        self.validation_rules = {}
        self.logging_enabled = False
    
    def set_with_validation(self, key, value, validation_func=None):
        """Set value with validation."""
        if validation_func and not validation_func(value):
            raise ValueError(f"Invalid value for {key}: {value}")
        
        self.set(key, value)
        self.metadata[key] = {
            'value': value,
            'timestamp': time.time(),
            'valid': True
        }
    
    def get_with_history(self, key):
        """Get value with validation."""
        if key not in self.metadata:
            return self.get(key)
        
        return self.metadata[key]['value']
    
    def validate_configuration(self):
        """Validate entire configuration."""
        errors = []
        
        for key in self.validation_rules:
            try:
                if not self.validation_rules[key](key):
                    errors.append(f"Validation failed for {key}")
            except Exception as e:
                errors.append(f"Error validating {key}: {e}")
        
        if errors:
            raise ValueError(f"Configuration validation failed: {errors}")
        
        return True
    
    def export_to_json(self, filepath):
        """Export configuration to JSON with metadata."""
        config_data = {
            'settings': self.to_dict(),
            'metadata': self.metadata
        }
        
        import json
        with open(filepath, 'w') as f:
            json.dump(config_data, f, indent=2)
    
    def import_from_json(self, filepath):
        """Import configuration from JSON with metadata."""
        import json
        with open(filepath, 'r') as f:
            config_data = json.load(f)
        
        # Clear existing configuration
        self.clear()
        
        # Import settings
        for key, value in config_data['settings'].items():
            self.set(key, value)
        
        # Import metadata
        self.metadata = config_data.get('metadata', {})

# Use enhanced configuration
config = EnhancedConfig()
config.set_with_validation("brain.neuron_count", 1000, lambda x: x > 0)
config.set_with_validation("plasticity.stdp.learning_rate", 0.001, lambda x: 0 <= x <= 1)
config.validate_configuration()

# Export and import
config.export_to_json("enhanced_config.json")
config2 = EnhancedConfig()
config2.import_from_json("enhanced_config.json")
```

---

## Part 8: Performance and Scalability

### 8.1 Performance Optimization Strategies

#### 8.1.1 Batch Processing Optimization
```python
import pynlm
import time
import numpy as np

def benchmark_batch_processing(config, batch_sizes):
    """Benchmark different batch processing sizes."""
    
    results = {}
    
    for batch_size in batch_sizes:
        # Create fresh brain for each test
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Generate steps
        steps = list(range(batch_size))
        
        # Time batch processing
        start_time = time.time()
        completed_steps = brain.batchStep(steps)
        end_time = time.time()
        
        execution_time = end_time - start_time
        steps_per_second = batch_size / execution_time
        
        results[batch_size] = {
            'execution_time': execution_time,
            'steps_per_second': steps_per_second,
            'success': len(completed_steps) == batch_size
        }
        
        print(f"Batch size {batch_size}: {steps_per_second:.0f} steps/second")
    
    return results

# Benchmark different batch sizes
config = pynlm.createDefaultConfig()
batch_sizes = [100, 500, 1000, 5000, 10000]
results = benchmark_batch_processing(config, batch_sizes)

# Find optimal batch size
optimal_batch = max(results.keys(), key=lambda k: results[k]['steps_per_second'])
print(f"Optimal batch size: {optimal_batch}")
```

#### 8.1.2 Memory Usage Optimization
```python
import pynlm
import psutil
import os

def monitor_memory_usage(config, steps=1000):
    """Monitor memory usage during simulation."""
    
    # Get initial memory usage
    process = psutil.Process(os.getpid())
    initial_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation while monitoring memory
    memory_usage = []
    step_times = []
    
    for step in range(steps):
        step_start = time.time()
        
        # Record memory usage
        current_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_usage.append(current_memory)
        
        # Run step
        brain.step(step)
        
        step_end = time.time()
        step_times.append((step_end - step_start) * 1000)  # ms
    
    # Get final memory usage
    final_memory = process.memory_info().rss / 1024 / 1024  # MB
    memory_increase = final_memory - initial_memory
    
    # Memory statistics
    max_memory = max(memory_usage)
    avg_memory = sum(memory_usage) / len(memory_usage)
    
    return {
        'initial_memory_mb': initial_memory,
        'final_memory_mb': final_memory,
        'memory_increase_mb': memory_increase,
        'max_memory_mb': max_memory,
        'avg_memory_mb': avg_memory,
        'step_times': step_times
    }

# Monitor memory usage
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

results = monitor_memory_usage(config, steps=1000)
print(f"Memory usage increased by {results['memory_increase_mb']:.2f} MB")
print(f"Peak memory usage: {results['max_memory_mb']:.2f} MB")
```

### 8.2 Scalability Considerations

#### 8.2.1 Scaling with Neuron Count
```python
import pynlm
import time

def scale_neuron_performance(neuron_counts):
    """Test performance scaling with different neuron counts."""
    
    results = {}
    
    for neuron_count in neuron_counts:
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time step execution
        start_time = time.time()
        steps = brain.batchStep(range(100))
        end_time = time.time()
        
        execution_time = end_time - start_time
        steps_per_second = 100 / execution_time
        neurons_per_step = neuron_count
        
        results[neuron_count] = {
            'execution_time': execution_time,
            'steps_per_second': steps_per_second,
            'neurons_per_step': neurons_per_step,
            'efficiency': steps_per_second / neuron_count  # Steps per neuron per second
        }
        
        print(f"Neurons: {neuron_count}, Steps/sec: {steps_per_second:.0f}, Efficiency: {results[neuron_count]['efficiency']:.6f}")
    
    return results

# Test scaling with different neuron counts
neuron_counts = [100, 500, 1000, 2000, 5000, 10000]
results = scale_neuron_performance(neuron_counts)

# Find optimal scale
max_efficiency = max(results.values(), key=lambda x: x['efficiency'])
print(f"\nOptimal configuration:")
print(f"Neurons: {max_efficiency['neurons_per_step']}")
print(f"Steps per second: {max_efficiency['steps_per_second']:.0f}")
print(f"Efficiency: {max_efficiency['efficiency']:.6f}")
```

#### 8.2.2 Parallel Processing
```python
import pynlm
import time
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor

def parallel_simulation(config, num_parallel, steps_per_simulation):
    """Run multiple simulations in parallel."""
    
    def run_simulation(sim_id):
        """Run a single simulation."""
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps_per_simulation):
            brain.step(step)
        
        return {
            'simulation_id': sim_id,
            'total_spikes': brain.getTotalSpikeCount(),
            'final_firing_rate': brain.getAverageFiringRate(),
            'firing_neurons': brain.getFiringNeuronCount()
        }
    
    # Run simulations in parallel
    with ThreadPoolExecutor(max_workers=num_parallel) as executor:
        futures = [executor.submit(run_simulation, i) for i in range(num_parallel)]
        results = [future.result() for future in futures]
    
    # Calculate statistics
    total_spikes = sum(r['total_spikes'] for r in results)
    avg_firing_rate = sum(r['final_firing_rate'] for r in results) / num_parallel
    avg_firing_neurons = sum(r['firing_neurons'] for r in results) / num_parallel
    
    return {
        'results': results,
        'total_simulations': num_parallel,
        'total_spikes': total_spikes,
        'average_firing_rate': avg_firing_rate,
        'average_firing_neurons': avg_firing_neurons
    }

# Run parallel simulations
config = pynlm.createDefaultConfig()
num_parallel = 4
steps_per_simulation = 500

results = parallel_simulation(config, num_parallel, steps_per_simulation)

print(f"Parallel simulation results:")
print(f"Total simulations: {results['total_simulations']}")
print(f"Total spikes: {results['total_spikes']}")
print(f"Average firing rate: {results['average_firing_rate']:.2f} Hz")
print(f"Average firing neurons: {results['average_firing_neurons']}")
```

---

## Part 9: Migration Guide

### 9.1 From Original to Enhanced API

#### 9.1.1 Configuration Migration

**Original Configuration:**
```python
# Original simple configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
```

**Enhanced Configuration:**
```python
# Enhanced configuration with validation
config = pynlm.createDefaultConfig()
config.set_with_validation("brain.neuron_count", 1000, lambda x: x > 0)
config.set_with_validation("plasticity.stdp.learning_rate", 0.001, lambda x: 0 <= x <= 1)
```

#### 9.1.2 Brain Migration

**Original Brain Usage:**
```python
# Original brain usage
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(1000):
    brain.step(step)
```

**Enhanced Brain Usage:**
```python
# Enhanced brain usage with context manager
brain = pynlm.createBrain(config)
brain.initialize()

with pynlm.simulation_guard(brain, auto_save=True) as guard:
    for step in range(1000):
        brain.step(step)
        
        # Access enhanced metrics
        metrics = brain.getPerformanceMetrics()
        if metrics.firing_neuron_count == 0:
            print("No firing neurons - check simulation parameters")
```

#### 9.1.3 Agent Migration

**Original Agent Usage:**
```python
# Original agent usage
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(1000):
    agent.processSensoryInput(world.getSensoryPercept())
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

**Enhanced Agent Usage:**
```python
# Enhanced agent usage with advanced features
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableAllSystems(True)  # Enable all learning systems

for step in range(1000):
    agent.processSensoryBatch([world.getSensoryPercept()])  # Batch processing
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Enhanced learning
    neuromod_state = agent.getFullNeuromodulationState()
    if neuromod_state.curiosity < 0.1:
        # Increase curiosity for exploration
        agent.enableCuriosity(True)
```

#### 9.1.4 World Migration

**Original World Usage:**
```python
# Original world usage
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
```

**Enhanced World Usage:**
```python
# Enhanced world usage with batch processing
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8,
    energyDecayRate=0.01,
    maxEnergy=100.0
)

# Batch processing for efficiency
percepts = world.batchUpdate([0.1, 0.2, 0.3, 0.4, 0.5])
for percept in percepts:
    agent.processSensoryInput(percept)
```

### 9.2 Backward Compatibility

The enhanced Python bindings maintain full backward compatibility:

- All original classes and methods are preserved
- All original functionality is maintained
- All existing examples continue to work
- New features are available through enhanced methods

#### 9.2.1 Compatibility Examples

```python
# Original code continues to work exactly as before
import pynlm

# Original simple usage (still works)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# New enhanced features (available)
brain = pynlm.createBrain(config)
brain.initialize()

with pynlm.simulation_guard(brain) as guard:
    for step in range(100):
        brain.step(step)
        
    metrics = brain.getPerformanceMetrics()
    print(f"Enhanced metrics: {metrics}")
```

### 9.3 Migration Checklist

#### 9.3.1 Required Changes

- [ ] Add `simulation_guard` for automatic resource management
- [ ] Use `batchStep` for better performance
- [ ] Add `PerformanceProfiler` for monitoring
- [ ] Use `numpy_array` for data conversion
- [ ] Enable learning systems with `enableAllSystems`
- [ ] Use `extractFeatures` for machine learning
- [ ] Add checkpointing with `saveCheckpoint`
- [ ] Use `getFullNeuromodulationState` for analysis

#### 9.3.2 Optional Enhancements

- [ ] Add performance profiling with `PerformanceProfiler`
- [ ] Use `VisualizationUtils` for advanced visualization
- [ ] Implement `BatchProcessorGuard` for parallel processing
- [ ] Use `Context manager` for automatic cleanup
- [ ] Add `enhanced_config` with validation
- [ ] Use `custom_brain` for extensions
- [ ] Implement `advanced_visualization` utilities
- [ ] Add `research_experiment` capabilities

---

## Part 10: Summary

### 10.1 Key Improvements

The enhanced NLM Python bindings provide:

1. **Enhanced Error Handling**: Pythonic exceptions with detailed error messages
2. **NumPy Integration**: Efficient data conversion between C++ and NumPy
3. **Context Managers**: Automatic resource management and cleanup
4. **Batch Processing**: Efficient execution of multiple steps
5. **Performance Monitoring**: Comprehensive profiling and metrics
6. **Advanced Visualization**: Built-in plotting and analysis tools
7. **Configuration Validation**: Input validation and error checking
8. **Machine Learning Integration**: Feature extraction for ML applications
9. **Checkpoint Management**: Automatic backup and recovery
10. **Parallel Processing**: Efficient parallel execution
11. **Backward Compatibility**: All original functionality preserved
12. **Documentation**: Comprehensive guides and examples

### 10.2 New Features Summary

| Feature | Description | Use Case |
|---------|-------------|----------|
| `simulation_guard` | Context manager for automatic resource management | Production simulations |
| `batchStep` | Efficient batch processing of multiple steps | Research and performance-critical applications |
| `PerformanceProfiler` | Comprehensive performance monitoring | Performance optimization |
| `numpy_array` | NumPy integration for efficient data operations | Machine learning and data analysis |
| `extractFeatures` | Feature extraction for ML applications | Machine learning integration |
| `saveCheckpoint` / `loadCheckpoint` | Advanced checkpoint management | Long-running simulations and recovery |
| `getFullNeuromodulationState` | Complete neuromodulation state | Advanced analysis and research |
| `VisualizationUtils` | Built-in visualization and plotting | Visualization and presentation |
| `BatchProcessorGuard` | Parallel processing for multiple simulations | Statistical analysis and research |
| `enhanced_config` | Enhanced configuration with validation | Advanced configuration management |

### 10.3 Getting Started

1. **Install enhanced Python bindings**: `pip install pynlm`
2. **Read the enhanced documentation**: `easy_usage.md` and `HOW_TO_USE.md`
3. **Start with basic examples**: Review the provided examples
4. **Experiment with enhanced features**: Try the new context managers and batch processing
5. **Implement advanced features**: Add performance monitoring and visualization
6. **Scale up**: Use parallel processing for large-scale research

The enhanced NLM Python bindings provide a comprehensive, high-performance interface for the NLM neural simulation framework, suitable for both beginners and advanced users. All existing functionality is preserved while adding powerful new features for research, production, and machine learning applications.

---

## Version Information

- **Version**: 2.0.0-enhanced
- **API Compatibility**: Fully backward compatible with original NLM API
- **Performance**: Enhanced for improved performance and scalability
- **Features**: New enhanced features added for advanced users
- **Documentation**: Comprehensive documentation provided

For more information, visit the NLM documentation website or contact the development team.

---

## License

MIT License - Enhanced NLM Python bindings

Copyright (c) 2026 NLM Development Team

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.