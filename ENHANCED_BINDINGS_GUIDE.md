# NLM Enhanced Python Bindings - Comprehensive Guide

## Overview

This document provides a comprehensive guide to the enhanced Python bindings for the NLM (Neural Learning Machine) framework. The enhanced bindings provide a more Pythonic, feature-rich interface for working with neural simulations, with improved usability, numpy integration, and advanced features for both beginners and expert users.

## Key Improvements

### 1. **Enhanced API Design**
- **Pythonic Method Names**: More intuitive and readable method names
- **Convenience Methods**: Shortcuts for common operations
- **Preset Configurations**: Quick setup for different use cases
- **Enhanced Error Handling**: Better exception types and context

### 2. **NumPy Integration**
- **Direct Array Support**: Process numpy arrays directly without copying
- **Automatic Conversions**: Seamless C++ ↔ Python data conversion
- **Visualization Helpers**: Functions to prepare data for plotting

### 3. **Advanced Features**
- **Progress Tracking**: Built-in progress monitoring during simulations
- **Benchmarking Tools**: Performance measurement utilities
- **Mathematical Utilities**: Common mathematical functions
- **Configuration Presets**: Quick setup for research, performance, or testing

### 4. **Enhanced Documentation**
- **Comprehensive Examples**: Multiple examples for different use cases
- **Interactive Documentation**: Rich docstrings with usage examples
- **Tutorial System**: Step-by-step guides

## Quick Start Guide

### Installation and Setup

First, ensure the enhanced bindings are compiled:

```bash
# Create build directory and compile
mkdir build
cd build
cmake ..
make -j4

# Install the enhanced module
pip install -e .
```

### Basic Usage

```python
import nlm_enhanced as nlm
import numpy as np

# Create configuration with research settings
config = nlm.create_config()
config.use_research_settings()

# Create brain
brain = nlm.Brain(config.get_raw_config())
brain.initialize()

# Run simulation
brain.step_batch(1000, verbose=True)

# Get statistics
summary = brain.get_summary()
print(f"Average firing rate: {summary['avg_firing_rate']:.4f}")
```

## API Reference

### Configuration System

```python
# Create configuration
config = nlm.create_config()

# Use preset configurations
config.use_research_settings()      # For research experiments
config.use_high_performance_settings()  # For maximum performance
config.use_quick_test_settings()     # For fast testing

# Set individual parameters
config.set("neuron_count", 5000)
config.set("simulation_timestep", 0.001)

# Check configuration
if config.has("neuron_count"):
    neurons = config.get("neuron_count", 1000)

# Convert to dictionary for serialization
config_dict = config.to_dict()
```

### Brain Interface

```python
# Initialize brain
brain = nlm.Brain(config.get_raw_config())
brain.initialize()

# Run simulation with progress tracking
brain.step_batch(1000, verbose=True)

# Get comprehensive statistics
summary = brain.get_summary()
print(f"Total neurons: {summary['total_neurons']}")
print(f"Average firing rate: {summary['avg_firing_rate']:.4f}")
print(f"Total spikes: {summary['total_spikes']}")

# Get neural activity data for plotting
plot_data = brain.get_neural_activity_plot_data()
neuron_activities = plot_data['neuron_activities']
synapse_weights = plot_data['synapse_weights']

# Configure brain with standard settings
brain.configure_standard(neuron_count=1000, region_count=1)

# Fast-forward development to adult stage
brain.develop_to_adult()

# Enable all forms of plasticity
brain.enable_all_plasticity()
```

### AgentBrain Interface

```python
# Create agent brain interface
agent_brain = nlm.AgentBrain(brain.get_raw_brain())

# Process vision data from numpy array
vision_data = np.random.rand(256)  # 16x16 vision
agent_brain.process_vision(vision_data, width=16, height=16)

# Run learning experiment
results = agent_brain.run_learning_experiment(steps=1000)
print(f"Weight change: {results['weight_change']:.4f}")

# Get comprehensive summary
stats = agent_brain.get_summary()
print(f"Curiosity level: {stats['curiosity_level']:.4f}")
print(f"Novelty level: {stats['novelty_level']:.4f}")

# Enable all systems
agent_brain.enable_all_systems()
```

### World Interface

```python
# Create world
world = nlm.create_world()

# Set up a structured test world
world.setup_town(size=32)

# Get visualization data for rendering
viz_data = world.get_visualization_data()
print(f"Map size: {viz_data['map_size']}")
print(f"Agent position: {viz_data['agent_position']}")
```

## Advanced Features

### 1. NumPy Integration

The enhanced bindings provide seamless integration with NumPy arrays:

```python
import numpy as np

# Process numpy arrays directly
vision = np.random.rand(16, 16)
agent_brain.process_vision(vision)

# Get neural activity as numpy arrays
plot_data = brain.get_neural_activity_plot_data()
neuron_activities = plot_data['neuron_activities']  # numpy array
synapse_weights = plot_data['synapse_weights']      # numpy array

# Use NumPy for analysis
avg_activity = np.mean(neuron_activities)
std_activity = np.std(neuron_activities)
```

### 2. Mathematical Utilities

```python
# Mathematical constants
print(f"PI = {nlm.PI}")
print(f"E = {nlm.E}")

# Common mathematical operations
clamped_value = nlm.clamp(1.5, 0, 1)  # Returns 1.0
normalized_value = nlm.normalize(0.5, 0, 1)  # Returns 0.5

# Vector operations (when using numpy)
vectors = np.array([[1, 2, 3], [4, 5, 6]])
```

### 3. Benchmarking and Performance Monitoring

```python
# Benchmark function execution
def setup_standard_brain():
    config = nlm.create_config()
    config.use_quick_test_settings()
    brain = nlm.Brain(config.get_raw_config())
    brain.initialize()
    return brain

# Run benchmark
results = nlm.benchmark(setup_standard_brain, "Standard brain setup")
print(f"Setup time: {results['duration']:.4f}s")
print(f"Milliseconds: {results['milliseconds']:.2f}ms")
```

### 4. Error Handling

```python
try:
    # This will raise a ConfigurationError
    brain.process_vision(small_vision_array)
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except Exception as e:
    print(f"Unexpected error: {type(e).__name__}: {e}")
```

## Learning Experiment Examples

### 1. Simple Learning Experiment

```python
def run_simple_learning():
    # Setup
    config = nlm.create_config()
    config.use_research_settings()
    
    brain = nlm.Brain(config.get_raw_config())
    agent_brain = nlm.AgentBrain(brain.get_raw_brain())
    world = nlm.create_world()
    world.setup_town(32)
    
    # Enable learning systems
    agent_brain.enable_all_systems()
    
    # Run experiment
    for step in range(1000):
        # Get observation
        percept = world.observe(brain.getRegions()[0].get())
        
        # Process sensory input
        agent_brain.processSensoryInput(percept)
        
        # Brain step
        brain.step(step, step * 0.001)
        
        # Get action
        cmd = agent_brain.decodeMotorCommand()
        
        # Apply to world
        world.applyAction(brain.getRegions()[0].get(), cmd)
        
        # Get reward
        reward = world.computeReward(brain.getRegions()[0].get())
        agent_brain.applyRewardModulation(reward, 0.0f)
    
    return brain, agent_brain
```

### 2. Memory Integration Experiment

```python
def test_memory_integration():
    # Setup
    config = nlm.create_config()
    config.use_quick_test_settings()
    
    brain = nlm.Brain(config.get_raw_config())
    brain.initialize()
    
    # Test memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    # Run simulation to generate activity
    for step in range(500):
        brain.step(step, step * 0.001)
        
        # Store some activity in working memory
        if working_memory:
            # Inject some current to generate activity
            brain.injectCurrentToNeurons(NeuronType::Sensory, 10.0f)
    
    # Check results
    print(f"Working memory traces: {working_memory.getActiveTraces()}")
    print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
    
    return working_memory, episodic_memory
```

## Visualization and Plotting

### 1. Preparing Data for Visualization

```python
import matplotlib.pyplot as plt
import seaborn as sns

def create_neural_activity_plots(brain):
    # Get neural activity data
    plot_data = brain.get_neural_activity_plot_data()
    
    # Create figure with subplots
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Plot 1: Neuron activities histogram
    axes[0, 0].hist(plot_data['neuron_activities'], bins=50, alpha=0.7)
    axes[0, 0].set_title('Neuron Activity Distribution')
    axes[0, 0].set_xlabel('Activity Level')
    axes[0, 0].set_ylabel('Frequency')
    
    # Plot 2: Synapse weights histogram
    axes[0, 1].hist(plot_data['synapse_weights'], bins=50, alpha=0.7, color='orange')
    axes[0, 1].set_title('Synaptic Weight Distribution')
    axes[0, 1].set_xlabel('Weight')
    axes[0, 1].set_ylabel('Frequency')
    
    # Plot 3: Activity over time (if available)
    # This would require time-series data
    
    # Plot 4: Network statistics
    summary = brain.get_summary()
    stats_data = {
        'Total Neurons': summary['total_neurons'],
        'Total Synapses': summary['total_synapses'],
        'Active Neurons': summary['active_neurons'],
        'Firing Rate': summary['avg_firing_rate']
    }
    
    axes[1, 0].bar(stats_data.keys(), stats_data.values())
    axes[1, 0].set_title('Network Statistics')
    axes[1, 0].set_ylabel('Count / Rate')
    axes[1, 0].tick_params(axis='x', rotation=45)
    
    # Plot 5: Excitatory/Inhibitory ratio
    axes[1, 1].pie([summary['e_i_ratio'], 1 - summary['e_i_ratio']],
                  labels=['Excitatory', 'Inhibitory'],
                  autopct='%1.1f%%')
    axes[1, 1].set_title('E/I Balance')
    
    plt.tight_layout()
    return fig
```

### 2. Saving Results

```python
def save_experiment_results(brain, agent_brain, world, filename):
    """Save experiment results to file."""
    import pickle
    import json
    
    # Create results dictionary
    results = {
        'brain_summary': brain.get_summary(),
        'agent_summary': agent_brain.get_summary(),
        'world_data': world.get_visualization_data(),
        'metadata': {
            'timestamp': time.time(),
            'version': 'enhanced-nlm-1.0',
            'experiment_type': 'neural_learning'
        }
    }
    
    # Save as pickle for full data preservation
    with open(f"{filename}_full.pkl", 'wb') as f:
        pickle.dump(results, f)
    
    # Save as JSON for compatibility
    with open(f"{filename}_results.json", 'w') as f:
        # Convert numpy arrays to lists for JSON
        json_results = {
            'brain_summary': results['brain_summary'],
            'agent_summary': results['agent_summary'],
            'world_data': results['world_data'],
            'metadata': results['metadata']
        }
        json.dump(json_results, f, indent=2, default=str)
    
    return results
```

## Performance Optimization

### 1. Memory Management

```python
# Use numpy arrays directly to reduce memory overhead
vision_data = np.random.rand(16, 16).astype(np.float32)

# Clear references when done to free memory
if 'large_data' in locals():
    del large_data

# Force garbage collection if needed
import gc
gc.collect()
```

### 2. Batch Processing

```python
def batch_simulation(brain, num_batches, steps_per_batch):
    """Run simulation in batches for better memory management."""
    all_results = []
    
    for batch in range(num_batches):
        print(f"Processing batch {batch + 1}/{num_batches}")
        
        # Run batch
        start_time = time.time()
        brain.step_batch(steps_per_batch, verbose=True)
        end_time = time.time()
        
        # Collect results
        batch_results = {
            'batch': batch + 1,
            'duration': end_time - start_time,
            'steps': steps_per_batch,
            'summary': brain.get_summary()
        }
        
        all_results.append(batch_results)
        
        # Clear large data if needed
        if batch < num_batches - 1:
            gc.collect()
    
    return all_results
```

## Troubleshooting

### 1. Import Errors

```bash
# If you get import errors
python -c "import nlm_enhanced"

# Check if the module is installed correctly
pip list | grep nlm
```

### 2. Compilation Errors

```bash
# Recompile if there are issues
cd build
cmake --build . --clean-first
```

### 3. Performance Issues

```python
# Profile your code
import cProfile
import pstats

cProfile.run('run_simulation(brain)', 'profile_stats')

# View profiling results
pstats.Stats('profile_stats').sort_stats('cumulative').print_stats()
```

## Future Enhancements

### 1. Plugin System

- Plugin architecture for extending NLM functionality
- Custom neural dynamics and plasticity rules
- External world implementations

### 2. Advanced Visualization

- Real-time plotting capabilities
- 3D neural network visualization
- Interactive Jupyter notebook support

### 3. Machine Learning Integration

- TensorFlow/PyTorch interoperability
- Custom layer implementations
- Gradient-based learning

### 4. Distributed Computing

- Multi-GPU acceleration
- Distributed simulation across multiple machines
- Shared memory optimizations

## License

The enhanced Python bindings are part of the NLM project and are licensed under the MIT License. See the main NLM documentation for license details.

## Acknowledgments

Special thanks to the NLM research team for their contributions to this project. The enhanced bindings build upon the foundation of the original NLM framework and aim to make it more accessible and powerful for Python users.
