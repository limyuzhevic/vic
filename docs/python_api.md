# NLM Python API Documentation

## Overview

The NLM Python API provides comprehensive access to NLM's advanced features from Python. It enables programmatic control of neural network simulations, configuration management, and advanced analysis capabilities.

## Installation

### Prerequisites

- Python 3.8 or higher
- CMake 3.16 or higher
- pybind11 development headers
- C++20 compatible compiler

### Installation Steps

```bash
# Clone the repository
cd /workspace/app

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DPYTHON_EXECUTABLE=/usr/bin/python3

# Build the project
cmake --build .

# Install the Python module
cd ..
pip install -e python
```

### Alternative Installation (Conda)

```bash
conda create -n nlm python=3.8
conda activate nlm
pip install -e python
```

## Basic Usage

### Import and Initialization

```python
import pynlm

# Import main classes
from pynlm import (
    createAdvancedBrainConfig,
    createBrain,
    createAgentBrain,
    createAdvancedAgent,
    MonitoringSystem,
    AdvancedNeuralOperations,
    IntegrationSystem,
    AdvancedNeuroscience,
    PerformanceOptimizer
)

# Verify installation
print(pynlm.__version__)
```

### Creating Advanced Brain Configuration

```python
import pynlm

# Create advanced brain configuration
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 2000,
    'simulation_timestep': 0.001,
    'enable_plasticity': True,
    'enable_neuromodulation': True,
    'enable_curiosity': True,
    'enable_novelty': True,
    'enable_prediction_error': True,
    'enable_structural_plasticity': True,
    'enable_development': True,
    'learning_rate': 0.001,
    'discount_factor': 0.99
})

print(f"Created brain configuration with {brain_config.get('neuron_count')} neurons")
```

## Core API Classes

### AdvancedBrainConfiguration

```python
import pynlm

# Create advanced brain configuration
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 2000,
    'simulation_timestep': 0.001,
    'enable_plasticity': True,
    'enable_neuromodulation': True,
    'enable_curiosity': True,
    'enable_novelty': True,
    'enable_prediction_error': True
})

# Access configuration values
neuron_count = brain_config.get('neuron_count')
simulation_timestep = brain_config.get('simulation_timestep')
enable_plasticity = brain_config.get('enable_plasticity')

# Update configuration
brain_config.set('neuron_count', 3000)
brain_config.set('learning_rate', 0.01)

# Validate configuration
schema = pynlm.python.AdvancedBrainConfiguration.getDefaultBrainSchema()
is_valid = pynlm.python.AdvancedBrainConfiguration.validateConfig(brain_config, schema)

# Apply preset
specialized_config = pynlm.python.AdvancedBrainConfiguration.applyPreset("research")

# Export configuration
pynlm.python.AdvancedBrainConfiguration.exportConfiguration("brain_config.json")

# Import configuration
imported_config = pynlm.python.AdvancedBrainConfiguration.importConfiguration("brain_config.json")
```

### AdvancedAgent

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(brain_config)
agent_brain = pynlm.createAgentBrain(brain)

# Create advanced agent
advanced_agent = pynlm.createAdvancedAgent(agent_brain, {
    'algorithm': 'DEEP_Q_LEARNING',
    'learning_rate': 0.001,
    'discount_factor': 0.99,
    'replay_buffer_size': 10000,
    'batch_size': 32,
    'target_update_freq': 100
})

# Access agent properties
algorithm = advanced_agent.getAlgorithm()
learning_rate = advanced_agent.getLearningRate()
replay_buffer_size = advanced_agent.getReplayBufferSize()

# Control agent
advanced_agent.startLearning()
advanced_agent.stopLearning()

# Get learning statistics
learning_stats = advanced_agent.getLearningStatistics()
```

### MonitoringSystem

```python
import pynlm

# Initialize monitoring
pynlm.python.MonitoringSystem.initializeMonitoring({
    'enableMetricsStreaming': True,
    'enableRealTimeVisualization': True,
    'enablePerformanceProfiling': True,
    'enableActivityPatternAnalysis': True,
    'metricsInterval': 1.0,
    'visualizationPort': 8080
})

# Start metrics streaming
pynlm.python.MonitoringSystem.startMetricsStreaming(brain)

# Stop monitoring
pynlm.python.MonitoringSystem.stopMetricsStreaming(brain)

# Get performance profile
performance_profile = pynlm.python.MonitoringSystem.getPerformanceProfile(brain)

# Get activity patterns
activity_patterns = pynlm.python.MonitoringSystem.getActivityPatterns(brain)
```

### AdvancedNeuralOperations

```python
import pynlm

# Create neural populations
populations = pynlm.python.AdvancedNeuralOperations.createNeuralPopulation(
    brain, "default_region", nlm.NeuronType.Excitatory, 100
)

populations = pynlm.python.AdvancedNeuralOperations.createNeuralPopulation(
    brain, "cortical_region", nlm.NeuronType.Inhibitory, 50
)

# Manipulate synapses
synapses = pynlm.python.AdvancedNeuralOperations.getSynapses(
    brain, populations[0].id
)

for synapse in synapses:
    synapse.weight = 0.5  # Set weight
    synapse.delay = 2     # Set delay

# Analyze neural activity
activity_stats = pynlm.python.AdvancedNeuralOperations.analyzeNeuralActivity(
    brain, populations[0].id, window_size=1000
)

# Control plasticity
pynlm.python.AdvancedNeuralOperations.enablePlasticity(brain, True)
pynlm.python.AdvancedNeuralOperations.disablePlasticity(brain)

# Manage development stages
pynlm.python.AdvancedNeuralOperations.setDevelopmentStage(
    brain, "adolescent"
)
```

### IntegrationSystem

```python
import pynlm
import numpy as np

# Create NumPy arrays
numpy_array = pynlm.python.IntegrationSystem.createNumPyArray((50, 10))
scipy_matrix = pynlm.python.IntegrationSystem.createSciPyMatrix((100, 100))

# Convert to Python types
python_list = pynlm.python.IntegrationSystem.toPythonList(numpy_array)
python_dict = pynlm.python.IntegrationSystem.toPythonDict({
    'key1': np.array([1, 2, 3]),
    'key2': np.array([[1, 2], [3, 4]])
})

# Export data
pynlm.python.IntegrationSystem.exportData("data.npy", numpy_array)

# Import data
imported_data = pynlm.python.IntegrationSystem.importData("data.npy")

# Serialization
pynlm.python.IntegrationSystem.serialize("model.pkl", brain)
deserialized_brain = pynlm.python.IntegrationSystem.deserialize("model.pkl")
```

### AdvancedNeuroscience

```python
import pynlm

# Create custom neuromodulators
dopamine = pynlm.python.AdvancedNeuroscience.createNeuromodulator(
    "dopamine", strength=1.0, time_constant=10.0
)

curiosity = pynlm.python.AdvancedNeuroscience.createNeuromodulator(
    "curiosity", strength=0.5, time_constant=5.0
)

# Compute novelty and curiosity
novelty = pynlm.python.AdvancedNeuroscience.computeNovelty(
    brain, observation
)

curiosity = pynlm.python.AdvancedNeuroscience.computeCuriosity(
    brain, prediction_error
)

# Analyze prediction errors
prediction_error = pynlm.python.AdvancedNeuroscience.computePredictionError(
    brain, action, reward
)

# Memory consolidation and replay
pynlm.python.AdvancedNeuroscience.consolidateMemory(brain)
pynlm.python.AdvancedNeuroscience.replayMemory(brain)
```

### PerformanceOptimizer

```python
import pynlm

# Optimize performance
pynlm.python.PerformanceOptimizer.optimizeMemoryUsage(brain)

# Enable/disable optimizations
pynlm.python.PerformanceOptimizer.enableSIMD(brain)
pynlm.python.PerformanceOptimizer.disableSIMD(brain)

pynlm.python.PerformanceOptimizer.enableParallelism(brain)
pynlm.python.PerformanceOptimizer.disableParallelism(brain)

# Monitor performance
performance_stats = pynlm.python.PerformanceOptimizer.getPerformanceStats(brain)

# Resource management
pynlm.python.PerformanceOptimizer.setMemoryLimit(brain, "1GB")
pynlm.python.PerformanceOptimizer.setCPULimit(brain, "4")
```

## Python API Reference

### Function Reference

#### pynlm.createAdvancedBrainConfig(config_dict)
Creates an advanced brain configuration from a dictionary.

**Parameters**:
- `config_dict`: Dictionary containing brain configuration parameters

**Returns**:
- `AdvancedBrainConfiguration`: Configured brain configuration object

**Example**:
```python
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 1000,
    'enable_plasticity': True,
    'enable_neuromodulation': True
})
```

#### pynlm.createBrain(brain_config)
Creates a brain instance from a configuration.

**Parameters**:
- `brain_config`: Advanced brain configuration

**Returns**:
- `Brain`: Configured brain instance

#### pynlm.createAgentBrain(brain)
Creates an agent brain interface for the given brain.

**Parameters**:
- `brain`: Brain instance

**Returns**:
- `AgentBrain`: Agent brain interface

#### pynlm.createAdvancedAgent(agent_brain, rl_config)
Creates an advanced reinforcement learning agent.

**Parameters**:
- `agent_brain`: Agent brain interface
- `rl_config`: Dictionary containing RL agent configuration

**Returns**:
- `AdvancedAgent`: Configured advanced agent

#### pynlm.python.MonitoringSystem.initializeMonitoring(config)
Initializes the monitoring system with given configuration.

**Parameters**:
- `config`: Dictionary containing monitoring configuration

#### pynlm.python.MonitoringSystem.startMetricsStreaming(brain)
Starts real-time metrics streaming for the given brain.

**Parameters**:
- `brain`: Brain instance

#### pynlm.python.MonitoringSystem.getPerformanceProfile(brain)
Gets performance profile for the given brain.

**Parameters**:
- `brain`: Brain instance

**Returns**:
- `dict`: Performance profile information

### Class Reference

#### AdvancedBrainConfiguration
Manages brain configuration with schema validation and inheritance.

**Methods**:
- `get(key, default=None)`: Get configuration value
- `set(key, value)`: Set configuration value
- `validate(schema)`: Validate configuration against schema
- `applyPreset(preset_name)`: Apply predefined preset
- `exportConfiguration(filename)`: Export configuration to file
- `importConfiguration(filename)`: Import configuration from file

#### AdvancedAgent
Implements advanced reinforcement learning agent features.

**Methods**:
- `getAlgorithm()`: Get RL algorithm name
- `getLearningRate()`: Get learning rate
- `getDiscountFactor()`: Get discount factor
- `getReplayBufferSize()`: Get replay buffer size
- `startLearning()`: Start learning process
- `stopLearning()`: Stop learning process
- `selectAction(state)`: Select action for given state
- `computeCustomReward(state, action, next_state)`: Compute custom reward
- `addExperience(state, action, reward, next_state, done)`: Add experience to replay buffer
- `performRLStep()`: Perform RL learning step
- `getLearningStatistics()`: Get learning statistics

#### MonitoringSystem
Real-time monitoring and visualization system.

**Methods**:
- `initializeMonitoring(config)`: Initialize monitoring system
- `startMetricsStreaming(brain)`: Start metrics streaming
- `stopMetricsStreaming(brain)`: Stop metrics streaming
- `getPerformanceProfile(brain)`: Get performance profile
- `getActivityPatterns(brain)`: Get activity patterns
- `getLearningStatistics(brain)`: Get learning statistics

#### AdvancedNeuralOperations
Advanced neural network operations.

**Methods**:
- `createNeuralPopulation(brain, region, neuron_type, count)`: Create neural population
- `getSynapses(brain, population_id)`: Get synapses for population
- `analyzeNeuralActivity(brain, population_id, window_size)`: Analyze neural activity
- `enablePlasticity(brain, enable)`: Enable/disable plasticity
- `setDevelopmentStage(brain, stage)`: Set development stage

#### IntegrationSystem
Integration with Python ML libraries and data formats.

**Methods**:
- `createNumPyArray(shape)`: Create NumPy array
- `createSciPyMatrix(shape)`: Create SciPy matrix
- `toPythonList(array)`: Convert array to Python list
- `toPythonDict(dict_array)`: Convert dictionary array to Python dict
- `exportData(filename, data)`: Export data to file
- `importData(filename)`: Import data from file
- `serialize(filename, obj)`: Serialize object to file
- `deserialize(filename)`: Deserialize object from file

#### AdvancedNeuroscience
Advanced neuroscience features and neuromodulation.

**Methods**:
- `createNeuromodulator(name, strength, time_constant)`: Create neuromodulator
- `computeNovelty(brain, observation)`: Compute novelty score
- `computeCuriosity(brain, prediction_error)`: Compute curiosity score
- `computePredictionError(brain, action, reward)`: Compute prediction error
- `consolidateMemory(brain)`: Consolidate memory
- `replayMemory(brain)`: Replay memory

#### PerformanceOptimizer
Performance optimization and resource management.

**Methods**:
- `optimizeMemoryUsage(brain)`: Optimize memory usage
- `enableSIMD(brain)`: Enable SIMD optimizations
- `disableSIMD(brain)`: Disable SIMD optimizations
- `enableParallelism(brain)`: Enable parallel processing
- `disableParallelism(brain)`: Disable parallel processing
- `getPerformanceStats(brain)`: Get performance statistics
- `setMemoryLimit(brain, limit)`: Set memory limit
- `setCPULimit(brain, limit)`: Set CPU limit

## Examples

### Example 1: Basic Usage

```python
import pynlm

# Create advanced brain configuration
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 2000,
    'simulation_timestep': 0.001,
    'enable_plasticity': True,
    'enable_neuromodulation': True,
    'enable_curiosity': True,
    'enable_novelty': True,
    'enable_prediction_error': True
})

# Create brain and agent
brain = pynlm.createBrain(brain_config)
agent_brain = pynlm.createAgentBrain(brain)

# Create advanced agent
advanced_agent = pynlm.createAdvancedAgent(agent_brain, {
    'algorithm': 'Q_LEARNING',
    'learning_rate': 0.01,
    'discount_factor': 0.95,
    'replay_buffer_size': 1000
})

# Start learning
advanced_agent.startLearning()

# Run simulation
for episode in range(10):
    for step in range(100):
        state = [float(i) for i in range(20)]  # Example state
        action = advanced_agent.selectAction(state)
        reward = advanced_agent.computeCustomReward(state, action, state)
        advanced_agent.addExperience(state, action, reward, state, False)
        advanced_agent.performRLStep()
```

### Example 2: Advanced Configuration

```python
import pynlm

# Use configuration preset
specialized_config = pynlm.python.AdvancedBrainConfiguration.applyPreset("research")

# Validate configuration
schema = pynlm.python.AdvancedBrainConfiguration.getDefaultBrainSchema()
is_valid = pynlm.python.AdvancedBrainConfiguration.validateConfig(brain.getConfig(), schema)

# Export configuration
pynlm.python.AdvancedBrainConfiguration.exportConfiguration("brain_config.json")

# Import configuration
imported_config = pynlm.python.AdvancedBrainConfiguration.importConfiguration("brain_config.json")
```

### Example 3: Complete Example

```python
import pynlm
import numpy as np

# Create advanced brain configuration
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 3000,
    'enable_plasticity': True,
    'enable_neuromodulation': True,
    'enable_curiosity': True,
    'enable_novelty': True,
    'enable_prediction_error': True
})

# Create brain and agent
brain = pynlm.createBrain(brain_config)
agent_brain = pynlm.createAgentBrain(brain)
advanced_agent = pynlm.createAdvancedAgent(agent_brain, {
    'algorithm': 'DEEP_Q_LEARNING',
    'replay_buffer_size': 5000
})

# Configure monitoring
pynlm.python.MonitoringSystem.initializeMonitoring({
    'enableMetricsStreaming': True,
    'enableRealTimeVisualization': True,
    'enablePerformanceProfiling': True,
    'enableActivityPatternAnalysis': True
})

pynlm.python.MonitoringSystem.startMetricsStreaming(brain)

# Run simulation
for episode in range(10):
    for step in range(100):
        state = [np.random.rand() for _ in range(20)]
        action = advanced_agent.selectAction(state)
        reward = advanced_agent.computeCustomReward(state, action, state)
        advanced_agent.addExperience(state, action, reward, state, False)
        advanced_agent.performRLStep()

# Generate reports
learning_stats = advanced_agent.getLearningStatistics()
performance_profile = pynlm.python.MonitoringSystem.getPerformanceProfile(brain)
```

### Example 4: Neural Operations

```python
import pynlm

# Create neural populations
populations = pynlm.python.AdvancedNeuralOperations.createNeuralPopulation(
    brain, "default_region", nlm.NeuronType.Excitatory, 100
)

# Manipulate synapses
synapses = pynlm.python.AdvancedNeuralOperations.getSynapses(
    brain, populations[0].id
)

for synapse in synapses:
    synapse.weight = 0.5
    synapse.delay = 2

# Analyze activity
activity_stats = pynlm.python.AdvancedNeuralOperations.analyzeNeuralActivity(
    brain, populations[0].id, window_size=1000
)
```

### Example 5: Integration and Data Handling

```python
import pynlm
import numpy as np

# Create NumPy arrays
numpy_array = pynlm.python.IntegrationSystem.createNumPyArray((50, 10))
scipy_matrix = pynlm.python.IntegrationSystem.createSciPyMatrix((100, 100))

# Convert to Python types
python_list = pynlm.python.IntegrationSystem.toPythonList(numpy_array)

# Export and import data
pynlm.python.IntegrationSystem.exportData("data.npy", numpy_array)
imported_data = pynlm.python.IntegrationSystem.importData("data.npy")
```

## Python API Errors

### Common Errors

#### Error: Module Not Found
```
ModuleNotFoundError: No module named 'pynlm'
```

**Solution**: Install the Python module:
```bash
pip install -e python
```

#### Error: Invalid Configuration
```
ValueError: Invalid configuration: neuron_count must be positive
```

**Solution**: Provide valid configuration values:
```python
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 1000,  # Valid positive integer
    'simulation_timestep': 0.001  # Valid positive float
})
```

#### Error: Invalid Brain Type
```
ValueError: Invalid brain type: 'invalid_type'
Available types: standard, cortical, subcortical, hybrid
```

**Solution**: Use valid brain type:
```python
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 1000,
    'brain_type': 'standard'  # Valid type
})
```

#### Error: Missing Required Dependencies
```
ImportError: Cannot import 'numpy'
```

**Solution**: Install required dependencies:
```bash
pip install numpy scipy
```

### Error Handling

```python
import pynlm

try:
    # Create advanced brain configuration
    brain_config = pynlm.createAdvancedBrainConfig({
        'neuron_count': 1000,
        'enable_plasticity': True,
        'enable_neuromodulation': True
    })
    
    # Create brain
    brain = pynlm.createBrain(brain_config)
    
    # Create agent
    agent_brain = pynlm.createAgentBrain(brain)
    advanced_agent = pynlm.createAdvancedAgent(agent_brain, {
        'algorithm': 'Q_LEARNING',
        'learning_rate': 0.01
    })
    
    # Start simulation
    advanced_agent.startLearning()
    
    for episode in range(10):
        for step in range(100):
            state = [float(i) for i in range(20)]
            action = advanced_agent.selectAction(state)
            reward = advanced_agent.computeCustomReward(state, action, state)
            advanced_agent.addExperience(state, action, reward, state, False)
            advanced_agent.performRLStep()
    
    print("Simulation completed successfully!")
    
except ValueError as e:
    print(f"Configuration error: {e}")
    # Handle configuration error
except RuntimeError as e:
    print(f"Runtime error: {e}")
    # Handle runtime error
except ImportError as e:
    print(f"Import error: {e}")
    # Handle import error
except Exception as e:
    print(f"Unexpected error: {e}")
    # Handle unexpected error
```

## Python API Configuration

### pyproject.toml

```toml
[build-system]
requires = ["setuptools", "wheel", "pybind11>=2.6"]
build-backend = "setuptools.build_meta"

[project]
name = "pynlm"
version = "0.5.0"
description = "NLM Python API - Advanced interface to NLM neural network simulations"
authors = [
    {name = "NLM Team"}
]
license = {text = "MIT"}
readme = "python/README.md"
requires-python = ">=3.8"
classifiers = [
    "Programming Language :: Python :: 3",
    "Programming Language :: C++",
    "Operating System :: POSIX :: Linux",
    "Operating System :: MacOS :: MacOS X",
    "Operating System :: Microsoft :: Windows",
]

[project.dependencies]
numpy = ">=1.19"
scipy = ">=1.5"

[project.optional-dependencies]
dev = [
    "pytest",
    "black",
    "isort",
    "mypy"
]

[tool.setuptools]
packages = ["pynlm"]
include-package-data = true
```

### Python Version Requirements

- Python 3.8 or higher
- NumPy 1.19 or higher
- SciPy 1.5 or higher

### Required Imports

```python
import pynlm
import numpy as np
import scipy

# Check if all required modules are available
try:
    import pynlm
    print("✓ pynlm module is available")
except ImportError as e:
    print(f"✗ pynlm module not available: {e}")
    exit(1)

try:
    import numpy
    print("✓ numpy module is available")
except ImportError as e:
    print(f"✗ numpy module not available: {e}")
    exit(1)

try:
    import scipy
    print("✓ scipy module is available")
except ImportError as e:
    print(f"✗ scipy module not available: {e}")
    exit(1)
```

## Performance Considerations

### Memory Usage

```python
import pynlm
import psutil
import os

# Monitor memory usage
process = psutil.Process(os.getpid())
initial_memory = process.memory_info().rss / 1024 / 1024  # MB

# Create brain configuration
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 10000,  # Large network
    'enable_plasticity': True
})

# Check memory after configuration
post_config_memory = process.memory_info().rss / 1024 / 1024  # MB
memory_increase = post_config_memory - initial_memory

print(f"Memory increase after configuration: {memory_increase:.2f} MB")
```

### Parallel Processing

```python
import pynlm
import concurrent.futures

# Enable parallel processing
pynlm.python.PerformanceOptimizer.enableParallelism(brain)

# Process multiple simulations in parallel
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
    futures = []
    
    for i in range(10):
        config = {
            'neuron_count': 1000 + i * 100,
            'enable_plasticity': True,
            'enable_neuromodulation': True
        }
        
        future = executor.submit(run_simulation, config)
        futures.append(future)
    
    # Wait for all simulations to complete
    results = []
    for future in concurrent.futures.as_completed(futures):
        results.append(future.result())
```

## Conclusion

The NLM Python API provides a comprehensive, type-safe, and user-friendly interface to NLM's advanced neural network simulation capabilities. Key features include:

1. **Type Safety**: Python bindings with C++ type safety
2. **Comprehensive Coverage**: Access to all NLM features and capabilities
3. **Integration**: Seamless integration with NumPy, SciPy, and other Python libraries
4. **Validation**: Configuration validation with detailed error messages
5. **Performance**: Performance optimization and resource management
6. **Monitoring**: Real-time monitoring and visualization
7. **Extensibility**: Easy to extend with new features and functionality

The Python API is designed to support both simple interactive use and complex automation scenarios, providing flexibility for researchers, developers, and users at all skill levels.

For more information, refer to the specific sections of this documentation or consult the command line interface documentation for detailed command line options and usage.

Happy coding and neural network simulation with NLM's Python API! 🧠✨