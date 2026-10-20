# Enhanced NLM Python API Documentation

This document provides comprehensive documentation for the enhanced NLM Python API.
It covers new features, improvements, and usage examples.

## Overview

The enhanced NLM Python API extends the basic `pynlm` bindings with advanced features including:

- **Performance Monitoring**: Real-time performance tracking and profiling
- **Enhanced Configuration Management**: Intuitive configuration presets and validation
- **Advanced Agent Capabilities**: Curiosity-driven exploration and sophisticated learning
- **Learning Experiment Utilities**: Automated experiment recording and analysis
- **Improved API Design**: More intuitive interfaces and better error handling

## Installation

### Basic Installation

```bash
pip install pynlm
```

### Enhanced Features Installation

```bash
pip install pynlm-enhanced
```

Or build from source:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install pynlm_enhanced
```

## Key Improvements

### 1. Performance Monitoring

The enhanced API includes comprehensive performance monitoring with:

- **Real-time metrics collection**: Timestep performance, spike rates, neuron activity
- **Performance scoring**: Automated performance evaluation
- **Resource usage tracking**: Memory and CPU utilization
- **Benchmarking capabilities**: Comparison between different configurations

```python
import pynlm_enhanced as nlm

config = nlm.createEnhancedConfig()
config.setupDefaultBrain(neuronCount=1000)

brain = nlm.createEnhancedBrain(config)
if brain.initialize():
    brain.batchProcess(100, timestep=0.001)
    print(brain.getPerformanceReport())
```

### 2. Enhanced Configuration Management

New configuration methods for easy setup:

```python
import pynlm_enhanced as nlm

# Create enhanced configuration
config = nlm.createEnhancedConfig()

# Quick setup methods
config.setupDefaultBrain(neuronCount=500, regionCount=2)
config.setupDevelopmentMode()  # Enables developmental plasticity
config.setupLearningMode()     # Enables reward modulation and curiosity
config.setupAgentMode()        # Enables agent-specific features
config.setupHighPerformance()  # Optimizes for speed

# Get configuration summary
config.printConfig()

# Load from file with format detection
config.loadConfig("my_config.json")
```

### 3. Advanced Agent Features

Enhanced agent with curiosity-driven exploration:

```python
import pynlm_enhanced as nlm

# Create enhanced agent
agent = nlm.createEnhancedAgent(brain)
agent.initialize(world, enableExploration=True, enableLearning=True, enableDevelopment=True)

# Execute actions with learning and curiosity
action = agent.executeAction(world, reward=0.5, predictedReward=0.3)

# Curiosity-driven exploration
exploration_action = agent.exploreWithCuriosity(world, threshold=0.5)

# Get detailed agent status
print(agent.getStatus())
```

### 4. Learning Experiment Utilities

Automated learning experiment recording and analysis:

```python
import pynlm_enhanced as nlm

# Create learning experiment
experiment = nlm.createLearningExperiment(brain)

# Run experiment with automatic analysis
experiment.runLearningExperiment(duration=10.0, verbose=True)
```

### 5. Backward Compatibility

The enhanced API maintains full backward compatibility with the original `pynlm`:

```python
# Original API still works
import pynlm as nlm_basic

brain = nlm_basic.createBrain(nlm_basic.createDefaultConfig())
brain.initialize()
brain.step(0)

# Enhanced API available separately
import pynlm_enhanced as nlm_enhanced
```

## API Reference

### Core Classes

#### EnhancedConfig
Manages configuration with enhanced features:

**Methods:**
- `createEnhancedConfig()` - Create enhanced configuration
- `loadConfig(filepath)` - Load configuration from file
- `setupDefaultBrain(neuronCount, regionCount)` - Setup default brain configuration
- `setupDevelopmentMode()` - Setup development configuration
- `setupLearningMode()` - Setup learning configuration
- `setupAgentMode()` - Setup agent configuration
- `setupHighPerformance(threads)` - Setup high-performance configuration
- `printConfig()` - Print configuration summary
- `getAllKeys()` - Get all configuration keys
- `getValueOr(key, defaultValue)` - Get value with default

#### EnhancedBrain
Brain with performance monitoring:

**Methods:**
- `createEnhancedBrain(config)` - Create enhanced brain
- `initialize(enableMonitoring)` - Initialize with optional monitoring
- `step(currentStep, timestep, recordMetrics)` - Simulation step
- `batchProcess(numSteps, timestep)` - Process multiple steps
- `getMetrics()` - Get performance metrics
- `getPerformanceReport()` - Get detailed performance report
- `enableLogging(enable, logFile)` - Enable logging
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state

#### EnhancedAgent
Agent with advanced capabilities:

**Methods:**
- `createEnhancedAgent(brain)` - Create enhanced agent
- `initialize(world, enableExploration, enableLearning, enableDevelopment)` - Initialize agent
- `executeAction(world, reward, predictedReward, enableExploration)` - Execute action
- `exploreWithCuriosity(world, threshold)` - Curiosity-driven exploration
- `getStatus()` - Get detailed agent status
- `enableFeature(feature, enable)` - Enable/disable features

#### LearningExperiment
Automated learning experiment utilities:

**Methods:**
- `createLearningExperiment(brain)` - Create learning experiment
- `runLearningExperiment(duration, verbose)` - Run learning experiment

### Factory Functions

```python
# Factory functions for convenience
nlm.createEnhancedConfig()
nlm.createEnhancedBrain(config)
nlm.createEnhancedAgent(brain)
nlm.createLearningExperiment(brain)
```

### Constants and Examples

```python
# Available configuration templates
examples = nlm.getExampleConfigs()

# Version information
print(f"NLM Version: {nlm.VERSION}")
print(f"Build Type: {nlm.BUILD_TYPE}")
```

## Examples

### Example 1: Basic Enhanced Brain

```python
import pynlm_enhanced as nlm

# Create and setup brain
config = nlm.createEnhancedConfig()
config.setupDefaultBrain(neuronCount=1000)
config.setupLearningMode()

brain = nlm.createEnhancedBrain(config)
if brain.initialize():
    brain.batchProcess(100, timestep=0.001)
    print(brain.getPerformanceReport())
```

### Example 2: Advanced Agent Simulation

```python
import pynlm_enhanced as nlm

# Setup brain and agent
config = nlm.createEnhancedConfig()
config.setupAgentMode()
config.setupLearningMode()

brain = nlm.createEnhancedBrain(config)
agent = nlm.createEnhancedAgent(brain)

world = nlm.createSimpleWorld()
world.configure(width=20, height=20)
agent.initialize(world)

# Run simulation
for step in range(50):
    action = agent.executeAction(world)
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Example 3: Learning Experiment

```python
import pynlm_enhanced as nlm

# Create experiment
config = nlm.createEnhancedConfig()
config.setupLearningMode()
brain = nlm.createEnhancedBrain(config)
experiment = nlm.createLearningExperiment(brain)

# Run experiment
experiment.runLearningExperiment(duration=10.0, verbose=True)
```

### Example 4: Performance Monitoring

```python
import pynlm_enhanced as nlm

# Compare different configurations
configs = {
    "basic": lambda c: c.setupDefaultBrain(neuronCount=500),
    "high_performance": lambda c: c.setupHighPerformance(),
    "learning": lambda c: c.setupLearningMode(),
}

results = {}
for name, config_func in configs.items():
    config = nlm.createEnhancedConfig()
    config_func(config)
    
    brain = nlm.createEnhancedBrain(config)
    if brain.initialize():
        brain.batchProcess(50)
        results[name] = brain.getPerformanceReport()
```

## Performance Tips

### 1. Configuration Optimization

```python
# Use preset configurations for optimal performance
config.setupHighPerformance(threads=8)
config.setupDefaultBrain(neuronCount=1000, regionCount=2)
```

### 2. Batch Processing

```python
# Process multiple steps efficiently
brain.batchProcess(1000, timestep=0.001)
```

### 3. Memory Management

```python
# Save and load brain states
brain.save("checkpoint.nlm")
# ... later ...
brain.load("checkpoint.nlm")
```

### 4. Monitoring and Debugging

```python
# Enable detailed logging
brain.enableLogging(True, "debug.log")

# Get performance metrics
metrics = brain.getMetrics()
```

## Troubleshooting

### Common Issues

1. **Import Error**
   ```
   # Ensure you're importing the correct module
   import pynlm_enhanced as nlm  # For enhanced features
   import pynlm as nlm_basic     # For basic features
   ```

2. **Configuration Loading**
   ```
   # Check file format and location
   config.loadConfig("config.json")  # Supports .cfg, .json, .yaml
   ```

3. **Performance Issues**
   ```
   # Use high-performance configuration
   config.setupHighPerformance()
   ```

4. **Memory Issues**
   ```
   # Save frequently during long simulations
   brain.save("checkpoint.nlm")
   ```

### Debugging Tips

```python
# Enable detailed logging
import pynlm_enhanced as nlm
config = nlm.createEnhancedConfig()
brain = nlm.createEnhancedBrain(config)
brain.enableLogging(True, "debug.log")

# Get performance metrics for debugging
print(brain.getPerformanceReport())
```

## Migration Guide

### From Basic to Enhanced API

**Basic API:**
```python
import pynlm as nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()
brain.step(0)
```

**Enhanced API:**
```python
import pynlm_enhanced as nlm

config = nlm.createEnhancedConfig()
config.setupDefaultBrain()
brain = nlm.createEnhancedBrain(config)
if brain.initialize():
    brain.step(0, 0.001, True)
```

**Benefits of Enhanced API:**
- Performance monitoring
- Better configuration management
- Advanced agent features
- Learning experiment utilities
- Enhanced logging and debugging

## Technical Specifications

### Performance Monitoring Metrics

| Metric | Description | Units |
|--------|-------------|-------|
| Average Timestep | Average simulation timestep | seconds |
| Performance Score | Performance evaluation metric | normalized |
| Total Spikes | Total number of neural spikes | count |
| Active Neurons | Number of active neurons | count |
| Firing Rate | Average firing rate | Hz |
| E/I Ratio | Excitatory/Inhibitory balance | ratio |

### Configuration Presets

| Preset | Description |
|--------|-------------|
| Default | Basic brain configuration |
| Development | Developmental plasticity enabled |
| Learning | Reward and curiosity enabled |
| Agent | Agent-specific features enabled |
| High Performance | Optimized for speed and efficiency |

### File Formats

Supported configuration file formats:
- `.cfg` - Basic key-value format
- `.json` - JSON configuration
- `.yaml` / `.yml` - YAML configuration

## License

This enhanced API is part of the NLM project and is licensed under the MIT License.

## Copyright

© 2026 NLM Authors

## Acknowledgments

This enhanced API builds upon the foundational NLM project and extends its capabilities with advanced features for improved performance monitoring, configuration management, and agent capabilities.