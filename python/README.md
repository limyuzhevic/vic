# Advanced NLM Python API

## Overview

This directory contains the advanced Python API for NLM (Neural Learning Machine) with enhanced features for brain configuration, agent learning, monitoring, and neuroscience operations.

## Files Created

### Core Headers
- **advanced_api.hpp** - Main advanced API header with all feature classes
- **advanced_api.cpp** - Implementation of the advanced API
- **advanced_brain.hpp** - Advanced brain configuration with presets and validation
- **advanced_agent.hpp** - Advanced agent features with reinforcement learning
- **monitoring.hpp** - Real-time monitoring and visualization system
- **neural_ops.hpp** - Advanced neural network operations and analysis
- **integration.hpp** - Integration with Python ML libraries and data export/import
- **advanced_neuroscience.hpp** - Advanced neuroscience features and neuromodulation
- **performance.hpp** - Performance optimization and monitoring

### Python Examples
- **examples/advanced_examples.py** - Comprehensive examples demonstrating all advanced features

### Build System
- **CMakeLists.txt** - CMake build configuration for compiling advanced API

### Configuration
- **pyproject.toml** - Project configuration and dependencies

## Key Features

### 1. Advanced Brain Configuration
- Programmatic configuration creation with schema validation
- Configuration presets and templates
- Runtime configuration modification
- Configuration inheritance and merging

### 2. Advanced Agent Features
- Reinforcement learning integration (Q-learning, SARSA, Deep Q-learning, etc.)
- Experience replay and learning
- Custom reward functions
- Advanced perception processing

### 3. Real-time Monitoring and Visualization
- Live metrics monitoring and streaming
- Real-time brain state visualization
- Performance profiling
- Activity patterns analysis

### 4. Advanced Neural Network Operations
- Custom neural population creation
- Synapse manipulation and analysis
- Plasticity control and tuning
- Developmental stage management

### 5. Integration and Interoperability
- Integration with Python ML libraries (NumPy, SciPy)
- Data import/export capabilities
- Serialization formats (JSON, pickle, custom)
- Cross-language interoperability

### 6. Advanced Neuroscience Features
- Custom neuromodulator implementations
- Novelty and curiosity computation
- Prediction error analysis
- Memory consolidation and replay

### 7. Performance and Optimization
- Batch operations for large-scale simulations
- Parallel execution support
- Memory management and optimization
- Resource monitoring and management

## Installation and Building

### Prerequisites
- CMake 3.16 or higher
- Python 3.8 or higher
- pybind11 development headers
- C++20 compatible compiler

### Building

1. **Clone the repository**
```bash
git clone <repository-url>
cd advanced-nlm-api
```

2. **Create build directory**
```bash
mkdir build
cd build
```

3. **Run CMake**
```bash
cmake ..
```

4. **Build the project**
```bash
cmake --build .
```

5. **Install**
```bash
cmake --install .
```

### Build Options

The build supports several options:

- `-DENABLE_PYTHON_BINDINGS=ON/OFF` - Enable Python bindings
- `-DENABLE_ADVANCED_API=ON/OFF` - Enable advanced API features
- `-DCMAKE_BUILD_TYPE=Release/Debug` - Build type

## Python Usage Examples

### Basic Usage

```python
import pynlm
import numpy as np

# Create advanced brain configuration
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 2000,
    'simulation_timestep': 0.001,
    'enable_plasticity': True,
    'enable_neuromodulation': True,
    'enable_curiosity': True,
    'enable_novelty': True
})

# Create brain
brain = pynlm.createBrain(brain_config)

# Create advanced agent
agent_brain = pynlm.createAgentBrain(brain)
advanced_agent = pynlm.createAdvancedAgent(agent_brain, {
    'algorithm': 'DEEP_Q_LEARNING',
    'learning_rate': 0.001,
    'discount_factor': 0.99,
    'replay_buffer_size': 10000
})

# Start learning
advanced_agent.startLearning()

# Simulate experience
state = np.random.rand(20).astype(np.float32)
action = advanced_agent.selectAction(state)
next_state = np.random.rand(20).astype(np.float32)
reward = advanced_agent.computeCustomReward(state, action, next_state)

advanced_agent.addExperience(state, action, reward, next_state, False)
advanced_agent.performRLStep()
```

### Advanced Configuration

```python
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

### Reinforcement Learning

```python
# Register custom reward function
def custom_reward(state, action, next_state):
    state_value = sum(state) / len(state)
    next_state_value = sum(next_state) / len(next_state)
    return next_state_value - state_value

advanced_agent.registerRewardFunction('custom_reward', custom_reward)

# Enable advanced perception
advanced_agent.enableAdvancedPerception(
    enable_predictions=True,
    enable_novelty=True,
    enable_curiosity=True,
    enable_attention=True
)

# Get learning statistics
stats = advanced_agent.getLearningStatistics()
print(f"Average reward: {stats.averageReward}")
print(f"Learning progress: {stats.learningProgress}")
```

### Monitoring and Visualization

```python
# Initialize monitoring
monitoring_config = {
    'enableMetricsStreaming': True,
    'metricsStreamIntervalMs': 100.0,
    'enableRealTimeVisualization': True,
    'enablePerformanceProfiling': True,
    'enableActivityPatternAnalysis': True
}

pynlm.python.MonitoringSystem.initializeMonitoring(monitoring_config)

# Start metrics streaming
pynlm.python.MonitoringSystem.startMetricsStreaming(brain, 100.0)

# Collect metrics
metrics = pynlm.python.MonitoringSystem.getCurrentMetrics(brain)
brain_state = pynlm.python.MonitoringSystem.getBrainStateVisualization(brain)
performance = pynlm.python.MonitoringSystem.getPerformanceProfile(brain)

# Stop monitoring
pynlm.python.MonitoringSystem.stopMetricsStreaming()
pynlm.python.MonitoringSystem.shutdownMonitoring()
```

### Integration with Python ML Libraries

```python
# NumPy integration
if pynlm.python.IntegrationSystem.isNumPyAvailable():
    numpy_array = pynlm.python.IntegrationSystem.createNumPyArray({100, 10})
    nlm_data = pynlm.python.IntegrationSystem.convertToNumPyFormat(numpy_array)
    converted_array = pynlm.python.IntegrationSystem.convertFromNumPyFormat(nlm_data)

# SciPy integration
if pynlm.python.IntegrationSystem.isSciPyAvailable():
    stats = pynlm.python.IntegrationSystem.computeStatisticalAnalysis(data_array)
    similarity_matrix = pynlm.python.IntegrationSystem.computeSimilarityMatrix(data)
    clustering = pynlm.python.IntegrationSystem.computeClusteringAnalysis(data)

# Data export/import
pynlm.python.IntegrationSystem.exportBrainToJSON(brain, "brain_data.json")
brain = pynlm.python.IntegrationSystem.importBrainFromJSON("brain_data.json")

pynlm.python.IntegrationSystem.exportDataToCSV("data.csv", some_data)
imported_data = pynlm.python.IntegrationSystem.importDataFromCSV("data.csv")
```

### Neuroscience Features

```python
# Neuromodulators
neuromod_config = {
    'name': 'creativity_modulator',
    'baseline_level': 0.5,
    'sensitivity': 2.0,
    'gain': 1.0
}

pynlm.python.AdvancedNeuroscience.registerCustomNeuromodulator(neurom_config)

# Novelty and curiosity
novelty_result = pynlm.python.AdvancedNeuroscience.computeNoveltyScore(brain, input_state)
curiosity_result = pynlm.python.AdvancedNeuroscience.computeCuriosityScore(brain, state)

# Memory consolidation
memory_trace = [0.1, 0.2, 0.3, 0.4, 0.5]
pynlm.python.AdvancedNeuroscience.recordMemory(brain, memory_trace, importance=0.8)

consolidation = pynlm.python.AdvancedNeuroscience.triggerMemoryConsolidation(brain)
```

### Performance Optimization

```python
# Enable performance features
pynlm.python.PerformanceOptimizer.enableBatchProcessing(brain, True)
pynlm.python.PerformanceOptimizer.enableParallelExecution(brain, True)
pynlm.python.PerformanceOptimizer.enableMemoryOptimization(brain, True)

# Get performance metrics
metrics = pynlm.python.PerformanceOptimizer.getPerformanceMetrics(brain)
memory_stats = pynlm.python.PerformanceOptimizer.getMemoryStatistics(brain)
resource_usage = pynlm.python.PerformanceOptimizer.getResourceUsage(brain)

# Optimize configuration
optimization_recommendations = pynlm.python.PerformanceOptimizer.getOptimizationRecommendations(brain)
```

## Examples

The `examples/` directory contains comprehensive examples:

- **advanced_examples.py**: Complete demonstration of all advanced features

Run the examples to see all advanced features in action:

```bash
python examples/advanced_examples.py
```

## Testing

### Unit Tests

Run the unit tests to verify the advanced API functionality:

```bash
pytest tests/
```

### Integration Tests

Run integration tests to verify Python bindings:

```bash
python -m pytest tests/integration_test.py
```

## Documentation

### API Reference

For detailed API documentation, refer to the header files:

- `advanced_api.hpp` - Main advanced API
- `advanced_brain.hpp` - Brain configuration
- `advanced_agent.hpp` - Agent features
- `monitoring.hpp` - Monitoring system
- `neural_ops.hpp` - Neural operations
- `integration.hpp` - Integration system
- `advanced_neuroscience.hpp` - Neuroscience features
- `performance.hpp` - Performance optimizer

### Python API Documentation

For Python-specific documentation, see the examples and the code comments in the bindings.

## Troubleshooting

### Build Issues

If you encounter build errors:

1. Ensure all dependencies are installed
2. Check that pybind11 is available
3. Verify C++ compiler supports C++20
4. Ensure Python development headers are installed

### Python Import Errors

If you cannot import the module:

1. Ensure the module is built and installed
2. Check Python path
3. Verify pybind11 is properly linked

### Runtime Errors

For runtime errors:

1. Check input validation
2. Verify configuration parameters
3. Check for memory leaks or resource limits
4. Review error messages for specific issues

## Contributing

### Adding New Features

To add new advanced features:

1. Add new header file in the `python/` directory
2. Implement functionality in the corresponding `.cpp` file
3. Update the `bindings.cpp` file to expose the new classes/functions
4. Update `CMakeLists.txt` if necessary
5. Add examples to the `examples/` directory

### Modifying Existing Features

When modifying existing features:

1. Update the appropriate header and implementation files
2. Ensure backward compatibility
3. Update examples if necessary
4. Run all tests to verify changes

### Documentation

Always update documentation when adding or modifying features:

1. Update docstrings in the C++ code
2. Add examples to the `examples/` directory
3. Update this README file
4. Consider adding Doxygen comments for C++ API

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Future Enhancements

Potential future enhancements for the advanced API:

1. **New Reinforcement Learning Algorithms**: Add more RL algorithms like A2C, PPO, DDPG
2. **Advanced Visualization**: Add more sophisticated visualization backends
3. **Distributed Computing**: Enable distributed training and simulation
4. **Real-time Web Interface**: Add web-based real-time monitoring interface
5. **Plugin Architecture**: Allow third-party plugin development
6. **Cloud Integration**: Enable cloud-based computing and storage
7. **Hardware Acceleration**: Support for GPU acceleration
8. **Quantum Computing Integration**: Quantum neural network operations

## Support

For support or questions:

1. **Documentation**: Refer to the examples and header files
2. **Issues**: Report bugs at the project repository
3. **Examples**: Use the comprehensive examples in the `examples/` directory
4. **Community**: Join the NLM community for discussions and support

## Acknowledgments

This advanced API was developed as an extension of the NLM (Neural Learning Machine) framework, building upon the existing C++ core and Python bindings to provide enhanced functionality for complex neural simulations.

Special thanks to the NLM development team for their contributions to the core framework.

---

*This documentation was automatically generated based on the advanced API implementation.*
