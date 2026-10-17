# Phase 3 Advanced Features for NLM Python Bindings

This document describes Phase 3 advanced features for expert users of the Neural Learning Machine (NLM) Python bindings.

## Overview

Phase 3 extends the basic Phase 2 functionality with advanced-level utilities and tools for:
- Fine-grained neural circuit manipulation
- Sophisticated performance analysis
- Advanced experiment management
- Complex data workflows
- Research-grade analysis capabilities

The Phase 3 bindings maintain Pythonic interfaces while providing expert-level control and flexibility.

## Quick Start

```python
from nlm import AdvancedBrainManipulator, createBrain, createDefaultConfig, PerformanceMonitor

# Create brain and manipulator
config = createDefaultConfig()
brain = createBrain(config)
manipulator = AdvancedBrainManipulator(brain)

# Create neurons
region_id = brain.addRegion("visual_cortex")
neuron1 = manipulator.createNeuron(region_id, NeuronType.Excitatory)
neuron2 = manipulator.createNeuron(region_id, NeuronType.Inhibitory)

# Create synapses
synapse = manipulator.createSynapse(neuron1, neuron2, weight=0.5)

# Monitor performance
monitor = PerformanceMonitor()
monitor.start()

for step in range(1000):
    brain.step(step)
    monitor.recordStep(step_time, spikes, active_neurons)

monitor.stop()
print(f"Performance: {monitor.getStepsPerSecond():.0f} steps/sec")
```

## 1. Advanced Brain Manipulation Utilities

### AdvancedBrainManipulator

Provides direct access to neural components for fine-grained manipulation:

#### Neuron Manipulation
- **createNeuron(regionId, type, initialState)**: Create neurons with specific types and initial states
- **deleteNeuron(neuronId)**: Remove neurons from the brain
- **modifyNeuronProperties(neuronId, newState)**: Update neuron state parameters
- **getNeuronState(neuronId)**: Get neuron state
- **getNeuronsInRegion(regionId)**: Get all neurons in a region
- **getNeuronsByType(type)**: Get all neurons of a specific type

#### Synapse Manipulation
- **createSynapse(source, target, type, weight, delay)**: Create synaptic connections
- **deleteSynapse(synapseId)**: Remove synapses
- **modifySynapseProperties(synapseId, newSynapse)**: Update synapse properties
- **getSynapseProperties(synapseId)**: Get synapse properties
- **getSynapsesFromNeuron(neuronId)**: Get outgoing synapses
- **getSynapsesToNeuron(neuronId)**: Get incoming synapses

#### Memory System Access
- **setWorkingMemoryEntry(key, value)**: Set working memory entries
- **getWorkingMemoryEntry(key)**: Get working memory entries
- **clearWorkingMemory()**: Clear all working memory
- **getFullWorkingMemory()**: Get all memory entries

#### Plasticity Rule Customization
- **setHebbianLearningRate(rate)**: Set Hebbian learning rate
- **setSTDPParameters(tauPre, tauPost, alpha)**: Configure STDP parameters
- **setRewardModulatedPlasticity(rate, traceDecay)**: Set reward-modulated plasticity
- **enablePlasticityForNeuron(neuronId, hebbian, stdp, rewardModulated)**: Configure neuron plasticity
- **enablePlasticityForSynapse(synapseId, hebbian, stdp, rewardModulated)**: Configure synapse plasticity

#### Development Stage Control
- **setDevelopmentalStage(stage)**: Set developmental stage
- **getDevelopmentalStage()**: Get current developmental stage
- **triggerDevelopmentStep()**: Trigger developmental step
- **getDevelopmentEvents()**: Get developmental events

#### Regional Connectivity Management
- **addInterRegionConnection(source, target, weight, delay)**: Add inter-region connections
- **removeInterRegionConnection(source, target)**: Remove inter-region connections
- **getInterRegionConnections()**: Get all inter-region connections
- **setRegionalConnectivityProbability(probability)**: Set connectivity probability
- **optimizeRegionalTopology()**: Optimize regional topology

## 2. Performance Monitoring Tools

### PerformanceMonitor

Real-time performance monitoring for neural simulations:

- **start()**: Start performance monitoring
- **stop()**: Stop performance monitoring
- **recordStep(stepTimeMs, spikes, activeNeurons)**: Record simulation step performance
- **reset()**: Reset performance counters

**Key Metrics:**
- **getTotalTimeSeconds()**: Total simulation time
- **getAvgStepTimeMs()**: Average step time in milliseconds
- **getStepsPerSecond()**: Simulation steps per second
- **getSpikesPerSecond()**: Spikes per second
- **getFiringRateHz()**: Firing rate in Hz
- **isStable(windowSize, threshold)**: Check performance stability

### MetricRecorder

Flexible metric recording with statistical analysis:

- **record(name, value)**: Record a metric value
- **recordTimeSeries(name, values, timestamp)**: Record time-series data
- **getMetric(name)**: Get current metric value
- **getValues(name)**: Get all values for a metric
- **getMean(name)**: Get mean of metric values
- **getStdDev(name)**: Get standard deviation
- **getMin(name)**: Get minimum value
- **getMax(name)**: Get maximum value
- **getMedian(name)**: Get median value

### TimelineRecorder

Event timeline recording and analysis:

- **startRecording()**: Start recording events
- **stopRecording()**: Stop recording events
- **recordEvent(eventName, description)**: Record an event
- **recordStep(eventName, timestamp, metrics)**: Record a simulation step
- **getEventsAfter(timestamp)**: Get events after timestamp
- **getEventsInRange(start, end)**: Get events within time range
- **getTimelineReport()**: Generate timeline report

### Profiler

Function and performance profiling for bottleneck identification:

- **startProfile(functionName)**: Start profiling a function
- **stopProfile(functionName)**: Stop profiling a function
- **recordMemoryUsage(context, bytes)**: Record memory usage
- **getProfileReport()**: Generate profile report
- **identifyBottlenecks()**: Identify performance bottlenecks

### MemoryMonitor

Memory usage monitoring for neural simulations:

- **startMonitoring()**: Start memory monitoring
- **stopMonitoring()**: Stop memory monitoring
- **recordSample()**: Record a memory sample
- **getPeakMemoryMB()**: Get peak memory usage
- **getCurrentMemoryMB()**: Get current memory usage
- **getMemoryGrowthRate()**: Get memory growth rate
- **getMemoryReport()**: Generate memory report

## 3. Enhanced Configuration Management

### ConfigManager

Advanced configuration management with templates and validation:

#### Template Management
- **addTemplate(name, description, config, isDefault)**: Add configuration template
- **removeTemplate(name)**: Remove configuration template
- **getTemplate(name)**: Get configuration template
- **getTemplateNames()**: Get all template names
- **setActiveTemplate(name)**: Set active template
- **getActiveTemplateName()**: Get active template name

#### Configuration Creation
- **createFromTemplate(templateName)**: Create config from template
- **createFromDefaults()**: Create default configuration
- **createFromInheritance(templateNames)**: Create config from inheritance

#### Configuration Validation
- **validateConfig(config)**: Validate configuration
- **validateConfigWithDetails(config)**: Validate with detailed errors
- **isConfigComplete(config)**: Check if configuration is complete

#### Config Inheritance and Merging
- **mergeConfigs(configs)**: Merge multiple configurations
- **mergeConfigWithTemplate(config, templateName)**: Merge config with template

#### Parameter Sensitivity Analysis
- **analyzeSensitivity(config)**: Analyze parameter sensitivity
- **getSensitivityReport(config)**: Get sensitivity report

#### Configuration History
- **saveConfig(config, description)**: Save configuration to history
- **loadConfig(description)**: Load configuration from history
- **getConfigHistory()**: Get configuration history

## 4. Advanced Serialization and Persistence

### SerializationManager

Comprehensive serialization capabilities:

#### HDF5 Support
- **saveHDF5(filepath, config, datasets, attributes)**: Save to HDF5 format
- **loadHDF5(filepath)**: Load from HDF5 format

#### Pickle-Compatible Serialization
- **savePickle(filepath, config, brain)**: Save to pickle format
- **loadPickle(filepath)**: Load from pickle format

#### Incremental Save/Restore
- **saveIncremental(filepath, config, data)**: Save incrementally
- **loadIncremental(filepath)**: Load incrementally

#### Checkpoint Management
- **createCheckpoint(name, config, brain)**: Create a checkpoint
- **restoreCheckpoint(name)**: Restore from checkpoint
- **getCheckpoints()**: Get list of checkpoints

#### Compressed Storage
- **saveCompressed(filepath, config, brain)**: Save with compression
- **loadCompressed(filepath)**: Load from compressed file

## 5. Visualization and Monitoring

### VisualizationTools

Comprehensive visualization capabilities:

#### Neural Network Visualization
- **visualizeNetwork(filepath, brain, format)**: Visualize neural network
- **visualizeConnections(filepath, brain, format)**: Visualize neural connections
- **visualizeActivity(filepath, brain, metric, threshold, format)**: Visualize neural activity

#### Activity Monitoring
- **createActivityMonitor(filepath, brain, metrics)**: Create activity monitor
- **createRealTimeMonitor(filepath, brain, updateIntervalMs)**: Create real-time monitor

#### Real-time Plotting
- **startRealTimePlotting(filepath, brain, metrics)**: Start real-time plotting
- **stopRealTimePlotting()**: Stop real-time plotting

#### Results Visualization
- **createResultsDashboard(filepath, brains, experimentNames)**: Create results dashboard
- **createComparisonPlot(filepath, brains, metric)**: Create comparison plot
- **createTrainingProgress(filepath, trainingData, title)**: Create training progress visualization

## Integration with Existing Framework

### Compatibility with Phase 2

Phase 3 maintains full compatibility with Phase 2 interfaces:

```python
# Phase 2 code continues to work
from nlm import SimpleExperiment, BrainTrainer, WorldBuilder, MetricsCollector

exp = SimpleExperiment()
exp.configure(max_steps=1000, timestep=0.01)
success = exp.run()

# Phase 3 code extends Phase 2
from nlm import AdvancedBrainManipulator

manipulator = AdvancedBrainManipulator(brain)
neuron = manipulator.createNeuron(region_id)
```

### Factory Functions

All advanced features provide factory functions for convenient instantiation:

```python
from nlm import (
    createAdvancedBrainManipulator,
    createPerformanceMonitor,
    createMetricRecorder,
    createTimelineRecorder,
    createProfiler,
    createMemoryMonitor,
    createConfigManager,
    createSerializationManager,
    createVisualizationTools,
)

# Use factory functions
manipulator = createAdvancedBrainManipulator(brain)
monitor = createPerformanceMonitor()
recorder = createMetricRecorder()
```

## Error Handling

The module provides comprehensive error handling:

- **ValidationError**: Configuration validation errors
- **BrainError**: Brain manipulation errors
- **ConfigError**: Configuration management errors
- **WorldError**: World simulation errors
- **NLMError**: Generic NLM errors

```python
try:
    manipulator = AdvancedBrainManipulator(brain)
    neuron = manipulator.createNeuron(region_id, NeuronType.Excitatory)
    
except ValidationError as e:
    print(f"Validation error: {e}")
except BrainError as e:
    print(f"Brain error: {e}")
except NLMError as e:
    print(f"NLM error: {e}")
```

## Performance Considerations

### Expert Use Cases
- **Maximum flexibility and control**: Use AdvancedBrainManipulator for fine-grained neural circuit manipulation
- **Performance optimization options**: Use PerformanceMonitor, Profiler, and MemoryMonitor for performance analysis
- **Advanced configuration patterns**: Use ConfigManager with template inheritance and sensitivity analysis
- **Comprehensive data management**: Use SerializationManager for large-scale data handling

### Best Practices
1. Use batch operations when creating multiple neurons/synapses
2. Monitor performance regularly using PerformanceMonitor
3. Use checkpointing for long-running simulations
4. Validate configurations before use
5. Analyze sensitivity for parameter tuning

## Example Workflows

### 1. Research Experiment Setup
```python
# Setup for research experiment
from nlm import createBrain, createDefaultConfig, createAdvancedBrainManipulator

config = createDefaultConfig()
config.set("brain.neuron_count", 10000)
config.set("brain.simulation_steps", 100000)

brain = createBrain(config)
manipulator = createAdvancedBrainManipulator(brain)

# Create complex neural architecture
for i in range(100):
    region_id = brain.addRegion(f"region_{i}")
    for j in range(100):
        neuron = manipulator.createNeuron(region_id, NeuronType.Excitatory)
```

### 2. Performance Analysis Workflow
```python
# Performance analysis workflow
from nlm import createBrain, createPerformanceMonitor, createProfiler, createMetricRecorder

brain = createBrain(createDefaultConfig())
monitor = createPerformanceMonitor()
profiler = createProfiler()
recorder = createMetricRecorder()

monitor.start()
profiler.startProfile("brain_step")

for step in range(10000):
    brain.step(step)
    monitor.recordStep(step_time, spikes, active_neurons)
    profiler.recordMemoryUsage("simulation", memory_usage)
    recorder.record("reward", calculate_reward(step))

profiler.stopProfile("brain_step")
monitor.stop()

# Analyze results
print(f"Performance report:\n{monitor.getPerformanceReport()}")
print(f"Profile report:\n{profiler.getProfileReport()}")
print(f"Metric summary:\n{recorder.summary()}")
```

### 3. Large-Scale Simulation Workflow
```python
# Large-scale simulation with checkpointing
from nlm import createBrain, createPerformanceMonitor, createSerializationManager

brain = createBrain(createDefaultConfig())
monitor = createPerformanceMonitor()
serializer = createSerializationManager()

monitor.start()

for step in range(100000):
    brain.step(step)
    monitor.recordStep(step_time, spikes, active_neurons)
    
    # Create checkpoint periodically
    if step % 10000 == 0:
        serializer.createCheckpoint(f"step_{step}", brain.getConfig(), brain)

monitor.stop()

# Save final results
serializer.saveCompressed("final_results.pkl", brain.getConfig(), brain)
serializer.saveHDF5("final_results.h5", brain.getConfig(), get_datasets(brain))

print(f"Simulation completed. Peak memory: {monitor.getPeakMemoryMB()} MB")
```

### 4. Comparative Experiment Workflow
```python
# Comparative experiment workflow
from nlm import createBrain, createDefaultConfig, createVisualizationTools

# Create multiple brains with different configurations
configs = [
    createDefaultConfig().set("brain.neuron_count", 1000),
    createDefaultConfig().set("brain.neuron_count", 5000),
    createDefaultConfig().set("brain.neuron_count", 10000),
]

brains = [createBrain(config) for config in configs]
visualizer = createVisualizationTools()

# Run experiments
for i, brain in enumerate(brains):
    brain.step(1000)

# Create comparison visualization
visualizer.createResultsDashboard("comparison.html", brains, 
                                  ["1K neurons", "5K neurons", "10K neurons"])
visualizer.createComparisonPlot("performance.png", brains, "firing_rate")
```

## Migration Guide from Phase 2 to Phase 3

### What Changed

**Phase 2 (Basic Interface)**:
- SimpleExperiment: High-level simulation interface
- BrainTrainer: Training-focused interface
- WorldBuilder: Environment construction
- MetricsCollector: Data collection

**Phase 3 (Advanced Interface)**:
- AdvancedBrainManipulator: Low-level neural control
- PerformanceMonitor: Real-time performance tracking
- MetricRecorder: Custom metric recording
- TimelineRecorder: Event timeline management
- Profiler: Performance bottleneck identification
- MemoryMonitor: Memory usage tracking
- ConfigManager: Advanced configuration management
- SerializationManager: Comprehensive serialization
- VisualizationTools: Advanced visualization capabilities

### Migration Steps

1. **Keep using Phase 2 code**: Existing Phase 2 code continues to work unchanged
2. **Gradual adoption**: Add Phase 3 features for specific use cases requiring advanced control
3. **Performance monitoring**: Use PerformanceMonitor and Profiler for production simulations
4. **Configuration management**: Use ConfigManager for complex experiments
5. **Data persistence**: Use SerializationManager for large-scale results
6. **Visualization**: Use VisualizationTools for comprehensive results analysis

### When to Use Phase 3

Use Phase 3 features when you need:
- Fine-grained control over neural components
- Real-time performance monitoring
- Advanced configuration management
- Large-scale data serialization
- Comprehensive visualization capabilities
- Research-grade analysis features

## Technical Specifications

### Python Interface
- **Type hints**: Comprehensive Python type hints throughout
- **Documentation**: Detailed docstrings with examples
- **Error handling**: Comprehensive exception handling
- **Performance**: Optimized for expert use cases

### Memory Management
- **Efficient storage**: Uses existing C++ memory management
- **Monitoring**: MemoryMonitor tracks memory usage
- **Serialization**: Compressed storage for large datasets
- **Checkpoints**: Incremental save/restore capabilities

### Performance Monitoring
- **Real-time tracking**: PerformanceMonitor for live metrics
- **Bottleneck identification**: Profiler for performance analysis
- **Historical analysis**: TimelineRecorder for event tracking
- **Memory analysis**: MemoryMonitor for memory usage

### Configuration Management
- **Template system**: Template-based configuration creation
- **Inheritance**: Configuration inheritance and merging
- **Validation**: Comprehensive validation utilities
- **Sensitivity**: Parameter sensitivity analysis

### Serialization
- **HDF5 support**: For large datasets
- **Pickle compatibility**: For model persistence
- **Incremental operations**: For continuous simulations
- **Compression**: For storage efficiency
- **Checkpoints**: For simulation recovery

## Conclusion

Phase 3 advanced features provide expert-level control and comprehensive capabilities for neural simulation. The module maintains Pythonic interfaces while offering the flexibility and power required for research-grade analysis and complex experiment management.

Key advantages:
- **Maximum flexibility**: Direct access to neural components
- **Comprehensive monitoring**: Performance, memory, and event tracking
- **Advanced configuration**: Template-based system with validation
- **Robust serialization**: Multiple formats with compression
- **Rich visualization**: Comprehensive plotting capabilities
- **Expert control**: Fine-grained manipulation of neural circuits

These features enable researchers and developers to create sophisticated neural simulations with complete control over the simulation process, comprehensive monitoring, and powerful analysis capabilities.