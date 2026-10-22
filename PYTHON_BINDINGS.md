# Python Bindings for NLM

This directory contains Python bindings for the NLM (Neural Learning Machine) framework, enabling seamless integration with the machine learning ecosystem.

## Overview

The Python bindings provide access to NLM's neural network simulation capabilities from within Python, making it easy to:

- Use NLM as a drop-in replacement for traditional neural networks
- Integrate NLM with existing Python machine learning libraries (TensorFlow, PyTorch, scikit-learn)
- Leverage Python's rich ecosystem for data analysis and visualization
- Create custom experiments and training loops
- Perform hyperparameter optimization
- Apply advanced machine learning techniques to neural network simulations

## Key Features

### 1. High-Level Python API
- Simple, intuitive Python interface
- Automatic memory management
- Comprehensive documentation and examples
- Type hints for better IDE support

### 2. Low-Level Python API
- Direct access to C++ functionality
- Fine-grained control over simulation parameters
- Performance-critical operations
- Advanced customization options

### 3. Integration with ML Libraries
- NumPy-compatible data structures
- Pandas DataFrame support for results
- Matplotlib integration for visualization
- Scikit-learn compatibility for evaluation

### 4. Advanced Features
- Real-time simulation monitoring
- Automatic data collection and analysis
- Custom event handling
- Parallel processing capabilities

## Installation

### Requirements
- Python 3.8 or higher
- NumPy
- SciPy
- Matplotlib (optional, for visualization)
- scikit-learn (optional, for analysis)

### Installation Methods

#### Using pip (Recommended)
```bash
pip install pynlm[all]
```

#### From source
```bash
pip install -e .
```

#### With optional dependencies
```bash
pip install pynlm[visualization]
pip install pynlm[analysis]
pip install pynlm[full]
```

## Basic Usage

### Import and Setup

```python
import nlm
import numpy as np
import matplotlib.pyplot as plt

# Create a simple neural network simulation
brain = nlm.Brain(
    neuron_count=100,
    region_count=2,
    connection_probability=0.15,
    random_seed=42
)

# Initialize the brain
brain.initialize()
```

### Running a Simulation

```python
# Run a simple simulation
print("Running simulation...")
for step in range(1000):
    brain.step(step, step * 0.001)
    
    # Collect data periodically
    if step % 100 == 0:
        spike_count = brain.get_total_spike_count()
        print(f"Step {step}: Total spikes = {spike_count}")
```

### Analyzing Results

```python
# Analyze simulation results
print("Simulation completed!")
print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Total neurons: {brain.get_total_neuron_count()}")
print(f"Total synapses: {brain.get_total_synapse_count()}")

# Get neuron statistics
neuron_stats = brain.get_neuron_statistics()
print(f"Average firing rate: {neuron_stats['average_firing_rate']:.2f} Hz")

# Visualize results
nlm.visualize_neural_network(brain, "simulation_results.html")
```

## Advanced Usage

### Using Configuration Files

```python
import json
import nlm

# Load configuration from JSON file
with open("configs/training_config.json", 'r') as f:
    config = json.load(f)

# Create brain from configuration
brain = nlm.Brain.from_config(config)
brain.initialize()

# Run simulation
brain.run(1000)
```

### Working with Different Brain Types

```python
import nlm

# Create different types of neural networks
standard_brain = nlm.Brain(
    neuron_count=500,
    region_count=2,
    connection_probability=0.15,
    brain_type="standard"
)

scale_free_brain = nlm.Brain(
    neuron_count=500,
    region_count=2,
    connection_probability=0.05,
    brain_type="scale_free"
)

small_world_brain = nlm.Brain(
    neuron_count=500,
    region_count=3,
    connection_probability=0.1,
    brain_type="small_world"
)
```

### Machine Learning Integration

```python
import nlm
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler

# Create training data
X = np.random.randn(100, 10)
y = np.random.randint(0, 2, 100)

# Split data
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2)

# Train NLM on training data
brain = nlm.Brain.from_training_data(X_train, y_train)
brain.train()

# Evaluate on test data
predictions = brain.predict(X_test)
accuracy = np.mean(predictions == y_test)
print(f"Accuracy: {accuracy:.2f}")
```

### Real-time Simulation

```python
import nlm
import time
import matplotlib.pyplot as plt

# Create brain for real-time simulation
brain = nlm.Brain(
    neuron_count=200,
    region_count=2,
    connection_probability=0.15
)
brain.initialize()

# Set up real-time plotting
plt.ion()
fig, ax = plt.subplots()

# Run real-time simulation
for step in range(500):
    brain.step(step, step * 0.001)
    
    # Update plot
    spike_count = brain.get_total_spike_count()
    ax.clear()
    ax.plot(step, spike_count, 'ro')
    ax.set_xlim(0, 500)
    ax.set_ylim(0, max(brain.get_total_spike_count(), 100))
    ax.set_xlabel('Step')
    ax.set_ylabel('Total Spikes')
    plt.pause(0.01)

plt.ioff()
plt.show()
```

### Batch Processing

```python
import nlm
import time
from concurrent.futures import ProcessPoolExecutor
import os

# Create multiple simulations
brains = []
for i in range(10):
    brain = nlm.Brain(
        neuron_count=100,
        region_count=2,
        connection_probability=0.15,
        random_seed=i
    )
    brains.append(brain)

# Initialize all brains
for brain in brains:
    brain.initialize()

# Run simulations in parallel
start_time = time.time()
with ProcessPoolExecutor() as executor:
    results = list(executor.map(lambda brain: brain.run(100), brains))

end_time = time.time()
print(f"Parallel execution time: {end_time - start_time:.2f} seconds")
```

### Custom Configuration Templates

```python
import nlm

# Create custom configuration template
config_template = {
    "name": "Custom Neural Network",
    "description": "A custom neural network configuration",
    "simulation": {
        "timestep": 0.001,
        "duration": 10.0,
        "neuron_count": 500,
        "region_count": 2,
        "connection_probability": 0.15
    },
    "plasticity": {
        "hebbian": True,
        "stdp": True,
        "reward_modulated": False,
        "learning_rate": 0.001
    },
    "output": {
        "format": "json",
        "compress": True
    }
}

# Create brain from template
brain = nlm.Brain.from_template(config_template)
brain.initialize()
```

## API Reference

### Core Classes

#### Brain
The main neural network simulation class.

**Methods:**
- `Brain(neuron_count, region_count, connection_probability, random_seed)` - Create a new brain
- `Brain.from_config(config)` - Create brain from configuration dictionary
- `Brain.from_training_data(X, y)` - Create brain from training data
- `Brain.from_template(template)` - Create brain from template
- `initialize()` - Initialize the brain
- `run(steps)` - Run simulation for specified number of steps
- `step(step, time)` - Run a single simulation step
- `get_total_spike_count()` - Get total spike count
- `get_total_neuron_count()` - Get total neuron count
- `get_total_synapse_count()` - Get total synapse count
- `get_neuron_statistics()` - Get neuron statistics
- `get_synapse_statistics()` - Get synapse statistics

**Properties:**
- `neuron_count` - Number of neurons
- `region_count` - Number of regions
- `connection_probability` - Connection probability
- `random_seed` - Random seed

#### Configuration
Configuration management utilities.

**Methods:**
- `Configuration.load_from_file(filename)` - Load configuration from file
- `Configuration.load_from_dict(config_dict)` - Load configuration from dictionary
- `Configuration.save_to_file(filename)` - Save configuration to file
- `Configuration.get(key, default=None)` - Get configuration value
- `Configuration.set(key, value, source='runtime')` - Set configuration value

#### Visualization
Visualization utilities.

**Functions:**
- `visualize_neural_network(brain, filename)` - Generate HTML visualization
- `visualize_spike_activity(brain, filename)` - Generate spike activity visualization
- `visualize_network_topology(brain, filename)` - Generate network topology visualization
- `create_interactive_dashboard(brain, filename)` - Create interactive dashboard

### Utility Functions

#### Simulation Utilities
- `run_simulation(config, steps)` - Run simulation with configuration
- `analyze_simulation_results(brain)` - Analyze simulation results
- `compare_simulations(brain1, brain2)` - Compare two simulations

#### Data Handling
- `save_results(brain, filename)` - Save simulation results
- `load_results(filename)` - Load simulation results
- `export_to_numpy(brain)` - Export results to NumPy arrays

#### Performance
- `profile_performance(brain, steps)` - Profile simulation performance
- `benchmark_neural_networks(configurations)` - Benchmark multiple configurations
- `measure_memory_usage(brain)` - Measure memory usage

## Examples

### Example 1: Basic Simulation

```python
import nlm

# Create and run a simple simulation
brain = nlm.Brain(neuron_count=100, region_count=2, connection_probability=0.15)
brain.initialize()

print("Running simulation...")
for step in range(500):
    brain.step(step, step * 0.001)

print(f"Simulation completed!")
print(f"Total spikes: {brain.get_total_spike_count()}")
```

### Example 2: Machine Learning Integration

```python
import nlm
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler

# Generate synthetic data
np.random.seed(42)
X = np.random.randn(200, 5)
y = (X[:, 0] + X[:, 1] > 0).astype(int)

# Split data
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2)

# Create and train brain
brain = nlm.Brain.from_training_data(X_train, y_train)
brain.train()

# Evaluate
predictions = brain.predict(X_test)
accuracy = np.mean(predictions == y_test)
print(f"Accuracy: {accuracy:.2f}")
```

### Example 3: Advanced Configuration

```python
import nlm
import json

# Create advanced configuration
config = {
    "simulation": {
        "timestep": 0.001,
        "duration": 10.0,
        "neuron_count": 500,
        "region_count": 2,
        "connection_probability": 0.15
    },
    "plasticity": {
        "hebbian": True,
        "stdp": True,
        "reward_modulated": True,
        "learning_rate": 0.001
    },
    "neuromodulation": {
        "dopamine_enabled": True,
        "curiosity_enabled": True
    },
    "output": {
        "format": "json",
        "compress": True,
        "include_visualizations": True
    }
}

# Create and run brain
brain = nlm.Brain.from_config(config)
brain.initialize()
brain.run(1000)

# Save results
brain.save_results("advanced_simulation_results.json")
```

## Installation and Setup

### Prerequisites
- Python 3.8 or higher
- NumPy
- SciPy

### Installation Steps

1. **Install NLM Python package:**
```bash
pip install pynlm[all]
```

2. **Install optional dependencies:**
```bash
pip install matplotlib
pip install scikit-learn
```

3. **Verify installation:**
```python
import nlm
print("NLM version:", nlm.__version__)
```

### Development Installation

For development, install with the development extras:
```bash
pip install -e .[dev]
```

## Troubleshooting

### Common Issues

1. **Import Error:**
```python
ImportError: cannot import name 'Brain' from 'nlm'
```
**Solution:** Ensure the Python package is properly installed. Try reinstalling:
```bash
pip install --upgrade pynlm
```

2. **Simulation Errors:**
**Solution:** Check configuration parameters and ensure they are valid.

3. **Memory Issues:**
**Solution:** Reduce simulation size or increase available memory.

### Getting Help

- **Documentation:** Check the NLM documentation for detailed API reference
- **Examples:** Look at the examples directory for sample usage
- **Issues:** Report issues on the NLM GitHub repository
- **Community:** Join the NLM community for support

## Future Enhancements

We plan to add:

1. **Additional Neural Network Types:** More architecture options
2. **Advanced Learning Algorithms:** Reinforcement learning, continual learning
3. **Real-time Data Integration:** Live data streaming capabilities
4. **Distributed Computing:** Support for distributed simulations
5. **Cloud Integration:** AWS, GCP, Azure integration
6. **Model Export:** Export models to standard formats (ONNX, TensorFlow)

## Conclusion

The Python bindings for NLM provide a powerful interface for neural network simulation and machine learning integration. Whether you're a researcher, practitioner, or developer, the Python bindings make it easy to:

- Leverage NLM's advanced neural network capabilities
- Integrate with existing Python machine learning workflows
- Perform complex simulations and experiments
- Analyze and visualize neural network behavior
- Build custom applications and tools

The Python bindings bridge the gap between NLM's C++ performance and Python's rich ecosystem, making advanced neural network simulation accessible to a wide range of users.
