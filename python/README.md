# NLM Enhanced Python Bindings
# Advanced Python interface for NLM (Neural Learning Machine)

This package provides enhanced Python bindings for NLM with advanced features beyond the original bindings.

## Overview

The enhanced Python bindings (`pynlm_enhanced`) provide comprehensive functionality for NLM simulations with:

- **Advanced Configuration**: Enhanced configuration management with validation, presets, and comparison
- **Performance Profiling**: Comprehensive performance monitoring, profiling, and analysis
- **Enhanced Debugging**: Advanced debugging and introspection capabilities
- **Visualization**: Visualization and monitoring interface with real-time capabilities
- **Scripting**: Scripting and automation support for custom workflows
- **Machine Learning**: Integration of ML models with neural simulation
- **Event System**: Advanced event system for real-time updates and notifications
- **Error Recovery**: Error recovery and handling system

For the original, lightweight bindings, use `pynlm` (built with the main CMakeLists.txt).

## Installation

### Install via pip (recommended)
```bash
pip install pynlm-enhanced
```

### Install from source
```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
pip install -e .
```

### Development installation with extras
```bash
pip install -e .[dev]
```

## Quick Start

### Basic Usage
```python
import pynlm_enhanced as nlm

# Create enhanced NLM system
brain = nlm.createDefaultConfig()
agent = nlm.initializeEnhancedNLM(brain)

# Access advanced features
debugger = agent.create_debug_console()
profiler = agent.create_performance_profiler()
visualizer = agent.create_visualization_interface()
script = agent.create_script_interpreter()
ml_interface = agent.create_machine_learning_interface()

# Run simulation
debugger.print_brain_info()
profiler.start_profiling()
agent.step(100)
profiler.stop_profiling()
debugger.print_performance_info()
```

### Configuration Management
```python
import pynlm_enhanced as nlm

# Create advanced config with validation
config = nlm.AdvancedConfigManager()
config.set("neuron_count", 5000)
config.set("region_count", 2)
config.set("enable_plasticity", True)

if config.validate():
    print("Configuration is valid!")
else:
    print("Configuration errors:", config.getValidationErrors())

# Save configuration to file
config.saveToJSONFile("my_config.json")

# Load configuration from file
config2 = nlm.AdvancedConfigManager()
config2.loadFromJSONFile("my_config.json")
```

### Performance Monitoring
```python
import pynlm_enhanced as nlm

# Create NLM system
brain = nlm.initializeEnhancedNLM()
agent = nlm.AgentBrain(brain)

# Create performance profiler
profiler = nlm.PerformanceProfiler(brain)

# Start profiling
profiler.startProfiling()
profiler.setSampleInterval(0.1)  # 100ms sampling

# Run simulation
for step in range(1000):
    agent.step(step)

# Stop profiling
profiler.stopProfiling()

# Get metrics
metrics = profiler.getMetrics()
print(f"Simulation time: {metrics.simulationTime}s")
print(f"Steps per second: {metrics.stepsPerSecond}")
print(f"Average firing rate: {metrics.averageFiringRate} Hz")

# Export metrics
profiler.exportToJSON("performance.json")
profiler.exportToCSV("performance.csv")
```

## Core Classes

### AdvancedConfigManager
Enhanced configuration manager with validation, presets, and comparison capabilities.

**Key Methods:**
- `fromDict(config_dict)` - Load configuration from Python dictionary
- `toDict()` - Convert configuration to Python dictionary
- `loadFromJSONFile(filepath)` - Load from JSON file
- `saveToJSONFile(filepath)` - Save to JSON file
- `setPreset(name)` - Set configuration to preset
- `savePreset(name)` - Save current configuration as preset
- `validate()` - Validate configuration
- `getValidationErrors()` - Get validation errors
- `diff(other)` - Compare with another configuration

### PerformanceProfiler
Comprehensive performance monitoring and analysis.

**Key Methods:**
- `startProfiling()` - Start performance profiling
- `stopProfiling()` - Stop performance profiling
- `getMetrics()` - Get current performance metrics
- `setSampleInterval(interval)` - Set sampling interval
- `enableComponentProfiling(enable)` - Enable component profiling
- `setPerformanceThreshold(metric, threshold)` - Set performance thresholds
- `exportToJSON(filepath)` - Export metrics to JSON file
- `exportToCSV(filepath)` - Export metrics to CSV file

### DebugConsole
Advanced debugging and introspection console.

**Key Methods:**
- `inspectBrainState()` - Inspect current brain state
- `inspectNeuronState(neuron_id)` - Inspect neuron state
- `inspectSynapseState(synapse_id)` - Inspect synapse state
- `inspectRegionState(region_id)` - Inspect region state
- `printBrainInfo()` - Print brain information
- `printMemoryInfo()` - Print memory information
- `printPerformanceInfo()` - Print performance information
- `traceStep(step)` - Trace simulation step
- `getDebugLog()` - Get debug log
- `exportState(filepath)` - Export state to file

### VisualizationInterface
Visualization and monitoring interface with real-time capabilities.

**Key Methods:**
- `setOptions(options)` - Set visualization options
- `getOptions()` - Get visualization options
- `startRealTimeVisualization()` - Start real-time visualization
- `collectVisualizationData()` - Collect visualization data
- `getNeuronVisualizationData()` - Get neuron visualization data
- `getSynapseVisualizationData()` - Get synapse visualization data
- `getNetworkVisualizationData()` - Get network visualization data
- `getMemoryVisualizationData()` - Get memory visualization data
- `update()` - Update visualization
- `generateReport(format, filepath)` - Generate report
- `visualizeNetworkTopology()` - Visualize network topology
- `visualizeActivityPatterns()` - Visualize activity patterns

### ScriptInterpreter
Scripting and automation interpreter for custom workflows.

**Key Methods:**
- `executeScript(script)` - Execute Python script
- `executeScriptFile(filepath)` - Execute script from file
- `registerPythonFunction(name, func)` - Register Python function
- `registerGlobalVariable(name, value)` - Register global variable
- `pushContext()` - Push execution context
- `popContext()` - Pop execution context
- `setErrorHandler(handler)` - Set error handler

### MachineLearningInterface
Machine learning model integration with neural simulation.

**Key Methods:**
- `registerModel(name, type, predict_func, train_func)` - Register ML model
- `removeModel(name)` - Remove ML model
- `getModel(name)` - Get ML model
- `trainModel(name, training_data)` - Train ML model
- `predict(name, input)` - Make prediction
- `integrateModelWithPrediction(name)` - Integrate with prediction
- `saveModel(name, filepath)` - Save model
- `loadModel(name, filepath)` - Load model

### EventSystem
Advanced event system for real-time updates and notifications.

**Key Methods:**
- `subscribe(event_type, callback)` - Subscribe to event
- `unsubscribe(subscription_id)` - Unsubscribe from event
- `publish(event_type, data, source)` - Publish event
- `getEventHistory(event_type, max_events)` - Get event history
- `setupBrainEvents()` - Setup brain events
- `setupMemoryEvents()` - Setup memory events

### ErrorRecoverySystem
Error recovery and handling system.

**Key Methods:**
- `addRecoveryAction(action)` - Add recovery action
- `removeRecoveryAction(name)` - Remove recovery action
- `monitorForErrors()` - Monitor for errors
- `checkRecoveryConditions()` - Check recovery conditions
- `executeRecoveryActions()` - Execute recovery actions
- `logError(error_type, message, context)` - Log error
- `testRecoveryActions()` - Test recovery actions

## Usage Examples

### Complete Simulation with Profiling
```python
import pynlm_enhanced as nlm
import time

# Initialize enhanced NLM system
brain = nlm.initializeEnhancedNLM()
agent = nlm.AgentBrain(brain)

# Setup debugging and profiling
debugger = agent.create_debug_console()
profiler = nlm.PerformanceProfiler(brain)
visualizer = nlm.VisualizationInterface(brain)

# Configure visualization
options = visualizer.getOptions()
options.enableVisualization = True
options.fps = 30.0
options.showNeuronStates = True
options.showSynapseWeights = True
visualizer.setOptions(options)

# Start profiling and visualization
profiler.startProfiling()
visualizer.startRealTimeVisualization()

# Run simulation
start_time = time.time()
for step in range(1000):
    agent.step(step)
    
    # Update visualization periodically
    if step % 100 == 0:
        visualizer.update()

end_time = time.time()

# Stop profiling and visualization
profiler.stopProfiling()
visualizer.stopRealTimeVisualization()

# Generate reports
visualizer.generateReport("json", "simulation_report.json")
profiler.exportToCSV("performance_metrics.csv")

# Display results
debugger.printBrainInfo()
debugger.printPerformanceInfo()

print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Average steps per second: {profiler.getMetrics().stepsPerSecond}")
```

## API Reference

### Main Classes

| Class | Purpose |
|-------|---------|
| AdvancedConfigManager | Configuration management with validation |
| PerformanceProfiler | Performance monitoring and analysis |
| DebugConsole | Advanced debugging and introspection |
| VisualizationInterface | Visualization and monitoring |
| ScriptInterpreter | Scripting and automation |
| MachineLearningInterface | ML model integration |
| EventSystem | Event-driven system |
| ErrorRecoverySystem | Error recovery and handling |

### Factory Functions

| Function | Description |
|----------|-------------|
| createDefaultConfig() | Create default configuration |
| initializeEnhancedNLM(config) | Initialize enhanced NLM system |
| createPerformanceProfiler(brain) | Create performance profiler |
| createDebugConsole(brain, agent) | Create debug console |
| createVisualizationInterface(brain) | Create visualization interface |
| createScriptInterpreter(brain, agent) | Create script interpreter |
| createMachineLearningInterface(brain) | Create ML interface |
| createEventSystem(brain) | Create event system |
| createErrorRecoverySystem(brain) | Create error recovery system |

### Utility Functions

| Function | Description |
|----------|-------------|
| fromDict(config_dict) | Load configuration from dictionary |
| toDict() | Convert object to dictionary |
| validateBrainConfig(config) | Validate brain configuration |
| setupPythonLogging(level) | Setup Python logging |
| runQuickTest(brain) | Run quick test |
| validateInstallation() | Validate installation |
| getSystemInfo() | Get system information |

## Development

### Building from Source
```bash
# Clone repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build with CMake
mkdir build
cd build
cmake ..
make

# Install Python package
pip install -e .
```

### Running Tests
```bash
# Run all tests
pytest tests/

# Run specific test modules
pytest tests/test_config.py
pytest tests/test_profiling.py
pytest tests/test_debug.py
```

### Code Style
The project follows these guidelines:
- Use black for code formatting (`black python/ src/ tests/`)
- Use isort for import sorting (`isort python/ src/ tests/`)
- Use mypy for type checking (`mypy python/ src/`)
- Use pytest for testing

## License

MIT License. See LICENSE file for details.

## Copyright

Copyright (c) 2026 NLM Project Authors. All rights reserved.
