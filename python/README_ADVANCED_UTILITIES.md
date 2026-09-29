# NLM Python Bindings - Advanced Neural Network Utilities

## Overview
This file contains advanced neural network utilities for the NLM (Neural Learning Machine) Python bindings, adding comprehensive functionality for brain management, network analysis, performance optimization, simulation control, data analysis, debugging, and experimental tools.

## Key Features Added

### 1. Advanced Brain Management Utilities
- **Batch Processing**: `batchStep()`, `batchInjectCurrent()`, `batchInjectByType()` for efficient parallel operations
- **Brain Cloning**: `clone()` method for creating brain copies
- **State Export/Import**: `exportState()`, `importState()` for serialization/deserialization
- **Configuration Templates**: `applyTemplate()`, `getTemplateList()` for preset configurations

### 2. Enhanced Neural Network Utilities
- **Network Analysis**: `analyzeConnectivity()`, `getNetworkDensity()`, `analyzeFiringPatterns()`
- **Pattern Matching**: `findSimilarNeurons()`, `calculateSimilarity()` for neuron comparison
- **Learning Control**: `enableAdvancedLearning()`, `setLearningRate()` for plasticity management
- **Neuromodulation**: `adjustNeuromodulation()`, `getNeuromodulationState()` for fine-tuning

### 3. Performance Optimization Utilities
- **Profiling**: `profilePerformance()`, `benchmarkStep()` for performance analysis
- **Memory Monitoring**: `monitorMemory()` for resource tracking
- **Threading**: `enableThreading()`, `setThreadCount()` for parallel processing
- **Parallel Processing**: `processInParallel()` for batch operations

### 4. Simulation Control Utilities
- **Advanced Control**: `pauseSimulation()`, `resumeSimulation()`, `isSimulationPaused()`
- **Event-Driven**: `enableEventDriven()`, `scheduleEvent()` for timed events
- **Checkpointing**: `createCheckpoint()`, `restoreCheckpoint()` for state management
- **Time Manipulation**: `setTimeScale()`, `getTimeScale()`, `advanceTime()` for temporal control

### 5. Data Analysis Utilities
- **Spike Analysis**: `analyzeSpikeTrain()`, `getFiringStatistics()` for spike train analysis
- **Population Activity**: `analyzePopulationActivity()`, `getPopulationStats()` for population-level analysis
- **Network Topology**: `analyzeNetworkTopology()` for graph analysis
- **Learning Curves**: `analyzeLearningCurve()`, `getLearningStats()` for progress tracking

### 6. Debugging and Development Tools
- **Visualization**: `generateVisualizationData()`, `exportNetworkGraph()` for data export
- **Inspection**: `inspectNeuron()`, `inspectSynapse()` for detailed neuron/synapse analysis
- **Debug Output**: `getDebugInfo()` for comprehensive debugging information
- **Validation**: `validateBrainState()`, `runDiagnostics()` for integrity checking

### 7. Experimental Tools
- **Phase Experiments**: `runPhaseExperiment()`, `getAvailablePhases()` for phase-based experimentation
- **Ablation Studies**: `ablateNeuronType()`, `ablateRegion()`, `getAblationStatus()` for lesion studies
- **Parameter Tuning**: `optimizeParameters()`, `tuneLearningParameters()` for hyperparameter optimization
- **Comparative Analysis**: `compareBrains()`, `benchmarkBrains()` for cross-brain analysis

## Usage Examples

```python
import pynlm as nlm

# Create a brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Use advanced brain management
brain.applyTemplate("advanced_learning")
clones = brain.clone()

# Run batch processing
steps = [1, 2, 3, 4, 5]
brain.batchStep(steps)

# Analyze network
network_stats = brain.analyzeConnectivity()
firing_patterns = brain.analyzeFiringPatterns()

# Performance profiling
profile = brain.profilePerformance(duration_steps=100)
memory_usage = brain.monitorMemory()

# Simulation control
brain.pauseSimulation()
brain.advanceTime(10.0)
brain.resumeSimulation()

# Debug and validation
debug_info = brain.getDebugInfo()
validation = brain.validateBrainState()

# Experimental tools
brain.runPhaseExperiment("phase3_plasticity", {"learning_rate": 0.01})
```

## Implementation Notes

- All new functions include comprehensive docstrings using pybind11's pbdoc format
- Functions follow Pythonic naming conventions and provide intuitive interfaces
- Error handling is built into core functionality with appropriate validation
- Type hints are used where possible for better IDE support
- The utilities are designed to be both accessible for beginners and powerful for advanced users
- Memory management follows RAII principles with proper cleanup

## Benefits

1. **Power User Features**: Advanced tools for researchers and developers
2. **Productivity**: Batch operations and automation capabilities
3. **Analysis**: Comprehensive analysis tools for network understanding
4. **Debugging**: Extensive debugging and validation utilities
5. **Experimentation**: Framework for systematic experimentation
6. **Performance**: Optimization tools for resource management
7. **Flexibility**: Configurable templates and parameter tuning
