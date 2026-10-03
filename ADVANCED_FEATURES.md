# NLM (Neural Learning Machine) - Advanced Features

This document describes the advanced features and improvements made to the NLM (Neural Learning Machine) framework.

## Overview

NLM is an experimental artificial developmental brain project that has reached Phase 6 (Final Integration). This document covers the advanced features, improvements, and new capabilities added to the framework.

## Key Improvements

### 1. Enhanced Configuration System

The configuration system now includes:

- **Advanced validation**: Validates parameter ranges and provides detailed error messages
- **Configuration presets**: Quick Test, Performance, Research, and Visualization presets
- **Configuration merging**: Combine multiple configuration sources
- **Parameter range validation**: Validates neuron counts, connection probabilities, learning rates
- **Missing parameter detection**: Identifies required parameters that are missing

#### New Configuration Methods

```python
# Create preset configurations
config = pynlm.createDefaultConfig()  # Uses Quick Test preset
config = pynlm.createDefaultConfig(type="performance")  # Performance preset
config = pynlm.createDefaultConfig(type="research")     # Research preset
config = pynlm.createDefaultConfig(type="visualization") # Visualization preset

# Validate configuration
if config.validate():
    print("Configuration is valid")
else:
    print("Validation errors:", config.validateAndGetErrors())

# Check for missing parameters
missing = config.getMissingParameters()
if missing:
    print("Missing parameters:", missing)
```

### 2. Advanced Learning Protocols

The framework now supports sophisticated learning algorithms:

- **Policy Gradient Learning**: Enables policy-based decision making
- **Reinforcement Learning**: Q-learning and SARSA algorithms
- **Curriculum Learning**: Progressive learning through increasing complexity
- **Meta Learning**: Learning to learn algorithms
- **Transfer Learning**: Knowledge transfer between tasks

#### Advanced Experiment Class

```python
import pynlm

# Create advanced experiment
experiment = pynlm.AdvancedExperiment()

# Configure for advanced learning
config = experiment.AdvancedConfig()
config.usePolicyGradient = True
config.useReinforcementLearning = True
config.useCurriculumLearning = True
config.enableMetaLearning = True

# Run experiment
result = experiment.run(config)

# Access results
print(f"Policy gradient score: {result.policyGradientScore}")
print(f"Reinforcement return: {result.reinforcementLearningReturn}")
print(f"Curriculum progress: {result.curriculumLearningProgress}")
print(f"System stable: {result.systemStable}")
```

### 3. Enhanced Error Handling and Validation

Improved error handling with comprehensive validation:

- **Parameter validation**: Validates ranges for all configuration parameters
- **File handling**: Graceful handling of missing or corrupt configuration files
- **Memory management**: Monitoring and prevention of memory overflows
- **System stability checks**: Detection of potential system instabilities
- **Detailed error messages**: Comprehensive error reporting with suggestions

#### Validation Example

```python
# Validate configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)

if not config.validate():
    errors = config.validateAndGetErrors()
    print("Validation errors:", errors)
    # Fix configuration and try again
```

### 4. Advanced Simulation Modes

Multiple advanced simulation modes for different research scenarios:

- **Research Mode**: Large-scale simulations with advanced learning
- **Performance Mode**: Optimized for speed and efficiency
- **Visualization Mode**: Configured for real-time visualization and monitoring
- **Development Mode**: Focused on developmental processes and maturation

#### Mode Configuration

```python
# Performance mode (fast simulation)
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("simulation_timestep", 0.01)

# Research mode (comprehensive study)
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)
config.set("enable_curiosity", True)
config.set("enable_novelty", True)

# Visualization mode (monitoring and analysis)
config = pynlm.createDefaultConfig()
config.set("neuron_count", 200)
config.set("enable_checkpointing", True)
config.set("enable_replay", True)
```

### 5. Comprehensive Monitoring and Debugging

Advanced monitoring tools for simulation debugging and optimization:

- **Performance monitoring**: Track simulation speed, memory usage, CPU usage
- **System stability checks**: Detect and report potential issues
- **Detailed logging**: Comprehensive logging for debugging
- **Memory management**: Monitor and optimize memory usage
- **Error recovery**: Graceful handling of errors and exceptions

#### Monitoring Example

```python
# Enable comprehensive monitoring
config = pynlm.createDefaultConfig()
config.set("log_level", "debug")
config.set("enable_detailed_logging", True)
config.set("enable_performance_monitoring", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with monitoring
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        # Get performance metrics
        firing_rate = brain.getAverageFiringRate()
        memory_usage = brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0
        print(f"Step {step}: Rate={firing_rate:.3f}, Memory={memory_usage}")
```

### 6. Enhanced Python API

The Python bindings have been enhanced with:

- **Convenience methods**: Simplified API for common operations
- **Type safety**: Better type checking and conversion
- **Error handling**: Comprehensive exception handling
- **Documentation**: Improved docstrings and examples
- **Advanced features**: Support for all new advanced functionality

#### Enhanced Python API Examples

```python
import pynlm

# Create brain with advanced configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with learning capabilities
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run advanced simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Advanced Experiment Examples

The framework includes comprehensive examples of advanced experiments:

### Advanced Learning Experiment

```python
from examples.advanced_experiment import (
    basic_advanced_experiment,
    advanced_experiment_with_learning,
    advanced_experiment_with_curriculum,
    main as run_advanced_examples
)

# Run all advanced experiment examples
run_advanced_examples()
```

### Complete Advanced Experiment Script

The `examples/advanced_experiment.py` file contains:

- Basic advanced experiments with enhanced learning
- Experiments with policy gradients and reinforcement learning
- Curriculum learning experiments with progressive complexity
- Complete example demonstrating all advanced features

## Configuration Presets

### Quick Test Preset

```python
# Optimized for quick testing and development
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)
config.set("region_count", 1)
config.set("connection_probability", 0.15)
config.set("simulation_timestep", 0.001)
```

### Performance Preset

```python
# Optimized for speed and efficiency
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("simulation_timestep", 0.01)
config.set("working_memory_capacity", 50)
config.set("episodic_memory_limit", 500)
```

### Research Preset

```python
# Optimized for comprehensive research
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)
config.set("region_count", 2)
config.set("connection_probability", 0.05)
config.set("enable_curiosity", True)
config.set("enable_novelty", True)
config.set("enable_prediction", True)
config.set("enable_planning", True)
```

### Visualization Preset

```python
# Optimized for visualization and analysis
config = pynlm.createDefaultConfig()
config.set("neuron_count", 200)
config.set("enable_checkpointing", True)
config.set("enable_replay", True)
config.set("checkpoint_path", "./checkpoints")
config.set("replay_interval", 100)
```

## Testing and Validation

The framework includes comprehensive tests:

### Test Suite

Run the test suite to verify all advanced features:

```bash
python test_nlm_advanced.py
```

The test suite includes:

- Basic functionality tests
- Advanced configuration preset tests
- Configuration validation tests
- Advanced learning feature tests
- Debugging tools tests
- Error handling tests

### Running Tests

```bash
# Run the test suite
python test_nlm_advanced.py

# Run specific tests
python -c "import test_nlm_advanced; test_nlm_advanced.test_basic_functionality()"
```

## Performance Optimization

### Tips for Optimal Performance

1. **Use appropriate neuron counts**: Start with 100-500 neurons for testing
2. **Optimize timestep**: 0.001-0.01 is typical for biological simulations
3. **Balance complexity**: More regions/neurons increase realism but reduce speed
4. **Use checkpoints**: Save progress periodically to avoid data loss

### Memory Management

1. **Monitor memory usage**: Check working memory efficiency regularly
2. **Clear unused memory**: Use `brain.getWorkingMemory().clear()` when needed
3. **Adjust memory limits**: Set appropriate capacities based on your use case
4. **Use efficient data structures**: The framework includes optimized memory pools

### Learning Efficiency

1. **Tune learning rates**: Start with 0.01 and adjust based on results
2. **Enable curiosity**: Let the agent explore naturally
3. **Use curriculum learning**: Progressive complexity improves learning
4. **Monitor convergence**: Track metrics to detect when learning plateaus

## Advanced Features Summary

| Feature | Description | Use Case |
|---------|-------------|----------|
| **Advanced Configuration** | Preset configurations and validation | Quick setup and validation |
| **Learning Protocols** | Policy gradient, reinforcement, curriculum | Complex behavior learning |
| **Error Handling** | Comprehensive validation and recovery | Robust operation |
| **Monitoring** | Performance tracking and debugging | Optimization and troubleshooting |
| **Advanced API** | Enhanced Python bindings | Easy scripting and integration |

## Getting Started

1. **Install dependencies**: Follow the README for installation instructions
2. **Run basic examples**: Check `examples/` directory for starter scripts
3. **Explore advanced features**: Use `test_nlm_advanced.py` for comprehensive testing
4. **Custom experiments**: Create your own experiments using the advanced API
5. **Performance tuning**: Use monitoring tools to optimize your simulations

## Documentation

### Key Documents

- **README.md**: Project overview and basic setup instructions
- **HOW_TO_USE.md**: Detailed usage guide for common tasks
- **ADVANCED_USAGE.md**: Advanced usage patterns and techniques
- **examples/advanced_experiment.py**: Complete advanced experiment examples
- **test_nlm_advanced.py**: Comprehensive test suite

### API Reference

The Python API is self-documented with comprehensive docstrings. Use your IDE's documentation viewer or `help()` in Python for detailed API information.

## Acknowledgements

This work was inspired by the NLM project's commitment to creating a comprehensive artificial brain simulation framework. The advanced features build upon the foundation laid by the original Phase 6 integration, extending the framework's capabilities for research and experimentation.

## License

MIT License. See LICENSE file for details.

## Contact

For questions or issues, please refer to the project documentation or contact the NLM team.