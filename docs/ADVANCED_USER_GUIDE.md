# NLM Advanced User Guide

This guide provides comprehensive documentation for advanced users of the NLM (Neural Learning Machine) framework, covering enhanced features, debugging capabilities, and advanced configuration options.

## Table of Contents

1. [Advanced Command-Line Interface](#advanced-command-line-interface)
2. [AgentBrain Advanced Features](#agentbrain-advanced-features)
3. [Configuration Validation and Schema](#configuration-validation-and-schema)
4. [Performance Profiling and Optimization](#performance-profiling-and-optimization)
5. [Debugging and Monitoring](#debugging-and-monitoring)
6. [Plugin System and Extensibility](#plugin-system-and-extensibility)
7. [Troubleshooting Common Issues](#troubleshooting-common-issues)
8. [API Reference](#api-reference)

---

## Advanced Command-Line Interface

The NLM framework now provides a comprehensive command-line interface with extensive options for advanced users.

### Basic Usage

```bash
# Standard simulation
./nlm

# With custom configuration
./nlm --config my_config.json

# With custom parameters
./nlm --key=neuron_count=10000 --key=connection_probability=0.05 --verbose

# Help and documentation
./nlm --help
```

### Advanced Options

#### Simulation Control

| Option | Description | Example |
|--------|-------------|---------|
| `--steps <N>` | Number of simulation steps to run | `--steps 5000` |
| `--benchmark <N>` | Run performance benchmark with N neurons | `--benchmark 2000` |
| `--test-mode <mode>` | Specific test mode: basic, plasticity, stdp, learning | `--test-mode learning` |
| `--run-all-tests` | Run all test suites | `--run-all-tests` |

#### Configuration Management

| Option | Description | Example |
|--------|-------------|---------|
| `--config <file>` | Load configuration from file | `--config configs/phase6_config.json` |
| `--key=<value>` | Set configuration value | `--key=neuron_count=5000` |
| `--import-state <file>` | Load brain state from checkpoint | `--import-state checkpoint.bin` |
| `--export-state <file>` | Save brain state to checkpoint | `--export-state new_checkpoint.bin` |

#### Debugging and Logging

| Option | Description | Example |
|--------|-------------|---------|
| `--verbose, -v` | Enable verbose logging | `--verbose` |
| `--debug` | Enable debug mode with detailed diagnostics | `--debug` |
| `--profile` | Enable performance profiling | `--profile` |
| `--neuron-log <file>` | Log all neuron activity | `--neuron-log neurons.log` |
| `--synapse-log <file>` | Log synapse modifications | `--synapse-log synapses.log` |

#### Advanced Features

| Option | Description | Example |
|--------|-------------|---------|
| `--plasticity-enable` | Enable all plasticity rules | `--plasticity-enable` |
| `--plasticity-disable` | Disable all plasticity rules | `--plasticity-disable` |
| `--neuromodulation-enable` | Enable all neuromodulation systems | `--neuromodulation-enable` |
| `--neuromodulation-disable` | Disable all neuromodulation systems | `--neuromodulation-disable` |
| `--connectivity <N>` | Test with N random connections | `--connectivity 100` |

### Command-Line Examples

#### Performance Benchmark

```bash
# Run a comprehensive performance benchmark
./nlm --benchmark 5000 --profile --verbose

# Benchmark with specific network configuration
./nlm --benchmark 10000 --key=neuron_count=10000 --key=connection_probability=0.1
```

#### Advanced Testing

```bash
# Run specific learning experiment
./nlm --test-mode learning --steps 2000 --debug

# Run plasticity experiments with logging
./nlm --test-mode plasticity --neuron-log plasticity.log --synapse-log synapses.log

# Run all tests with detailed output
./nlm --run-all-tests --verbose
```

#### State Management

```bash
# Import checkpoint and continue from saved state
./nlm --import-state saved_brain.bin --steps 1000 --export-state new_brain.bin

# Export brain state for later analysis
./nlm --export-state brain_checkpoint.bin --steps 500
```

#### Development and Research

```bash
# Enable maximum plasticity for development research
./nlm --plasticity-enable --neuromodulation-enable --development --curiosity

# Run detailed debugging session
./nlm --debug --neuron-log debug_neurons.log --synapse-log debug_synapses.log
```

## AgentBrain Advanced Features

The AgentBrain class now provides extensive runtime control and monitoring capabilities.

### Runtime Configuration

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Advanced runtime configuration
agent.setConfig("learning_rate", 0.01)
agent.setConfig("exploration_rate", 0.3)
agent.setConfig("curiosity_threshold", 0.5)

# Development control
agent.setDevelopmentalStage(pynlm.DevelopmentalStage::CriticalPeriod)
agent.accelerateDevelopment(2.0)  # 2x faster development
agent.pauseDevelopment(True)      # Pause development

# Neuromodulation control
agent.setNeuromodulationTarget("dopamine", 0.8)
agent.setNeuromodulationTarget("curiosity", 0.6)
agent.enableNeuromodulation("novelty", True)

# Memory system control
agent.enableMemoryConsolidation(True)
agent.setReplayFrequency(100)
```

### Debugging and Monitoring

```python
# Get comprehensive debug information
agent = pynlm.createAgentBrain(brain)
debug_info = agent.getDebugInfo()

print(f"Active neurons: {debug_info.activeNeuronCount}")
print(f"Dopamine level: {debug_info.dopamineLevel}")
print(f"Curiosity level: {debug_info.curiosityLevel}")
print(f"Developmental stage: {debug_info.developmentalStage}")
print(f"Average firing rate: {debug_info.averageFiringRate}")

# Performance metrics
metrics = agent.getPerformanceMetrics()
print(f"Neurons per second: {metrics.neuronsPerSecond}")
print(f"Last step time: {metrics.lastStepTime}ms")

# Anomaly detection
if agent.detectAnomalies():
    anomalies = agent.getAnomalies()
    for anomaly in anomalies:
        print(f"Anomaly detected: {anomaly}")
```

### Plugin System

```python
# Create custom plugin
class CustomPlugin(pynlm.AgentBrain.Plugin):
    def __init__(self):
        self.agent = None
    
    def initialize(self, agent):
        self.agent = agent
        print("Custom plugin initialized")
    
    def update(self, timestep):
        # Custom logic here
        if hasattr(self.agent, 'getDebugInfo'):
            debug = self.agent.getDebugInfo()
            if debug.dopamineLevel > 0.5:
                print(f"High dopamine detected at step {timestep}")
    
    def shutdown(self):
        print("Custom plugin shutdown")
    
    def getName(self):
        return "CustomAnalysisPlugin"

# Register plugin
plugin = CustomPlugin()
agent.registerPlugin(pynlm.AgentBrain.PluginPtr(plugin))
```

### Event Callbacks

```python
# State change listener
def on_state_change(key, value):
    print(f"State changed: {key} = {value}")

agent.addStateChangeListener("learning_rate", on_state_change)

# Performance callback
def on_performance_update(debug_info):
    if debug_info.curiosityLevel > 0.7:
        print("High curiosity detected!")

agent.addPerformanceListener(on_performance_update)

# Debug event callback
def on_debug_event(event, message):
    print(f"Debug event: {event} - {message}")

agent.addDebugEventListener(on_debug_event)
```

## Configuration Validation and Schema

The enhanced configuration system provides comprehensive validation and schema documentation.

### Configuration Validation

```python
import pynlm

# Create configuration with validation
config = pynlm.createDefaultConfig()

# Set values with automatic validation
config.set("neuron_count", 5000)  # Validation checks this is positive
config.set("connection_probability", 0.1)  # Validates range [0,1]
config.set("stdp_ltp_weight", 0.02)  # Validates type and range

# Get schema information
schema = config.getSchema()
for field in schema:
    print(f"{field.key}: {field.type} - {field.description}")
    print(f"  Default: {field.defaultValue}")
    print(f"  Required: {field.required}")
    print(f"  Valid values: {field.validValues}")
```

### Configuration Categories

The configuration system is organized into logical categories:

| Category | Description | Example Fields |
|----------|-------------|----------------|
| `brain` | Core brain parameters | `neuron_count`, `connection_probability` |
| `plasticity` | Plasticity system parameters | `stdp_ltp_weight`, `hebbian_learning_rate` |
| `neuromod` | Neuromodulation parameters | `dopamine_scale`, `curiosity_threshold` |
| `development` | Development system parameters | `plasticity_modifier_start`, `developmental_stages` |
| `memory` | Memory system parameters | `working_memory_size`, `replay_frequency` |

### JSON and YAML Support

```bash
# Save configuration to JSON
./nlm --export-config brain_config.json

# Load configuration from JSON
./nlm --config brain_config.json

# Load configuration from YAML
./nlm --config brain_config.yaml

# Generate configuration schema
./nlm --generate-schema config_schema.json
```

## Performance Profiling and Optimization

The framework includes comprehensive performance profiling and optimization tools.

### Basic Performance Profiling

```python
import pynlm
import time

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Start performance monitoring
start_time = time.time()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Performance metrics every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
```

### Advanced Performance Analysis

```python
# Performance metrics collection
performance = agent.getPerformanceMetrics()
print(f"Neurons per second: {performance.neuronsPerSecond}")
print(f"Average step time: {performance.lastStepTime}ms")

# Performance history
performance_history = agent.getPerformanceHistory(50)
for i, metrics in enumerate(performance_history):
    print(f"Step {i}: {metrics.neuronsPerSecond} neurons/sec")

# Optimization recommendations
def get_optimization_recommendations(agent):
    recommendations = []
    
    if agent.getPerformanceMetrics().neuronsPerSecond < 1000:
        recommendations.append("Consider increasing network efficiency")
    
    if agent.getDebugInfo().dopamineLevel > 0.8:
        recommendations.append("Dopamine levels are high - consider adjusting neuromodulation")
    
    if agent.getDebugInfo().curiosityLevel < 0.1:
        recommendations.append("Low curiosity - consider increasing exploration")
    
    return recommendations

recommendations = get_optimization_recommendations(agent)
for rec in recommendations:
    print(f"Recommendation: {rec}")
```

## Debugging and Monitoring

Comprehensive debugging and monitoring capabilities for research and development.

### Debug Targets

Enable specific debug targets for focused monitoring:

```python
# Enable various debug targets
agent.setDebugTarget("neuron_activity", True)
agent.setDebugTarget("synapse_weights", True)
agent.setDebugTarget("neuromodulation", True)
agent.setDebugTarget("development", True)
agent.setDebugTarget("memory_systems", True)

# Get enabled debug targets
enabled_targets = agent.getEnabledDebugTargets()
print(f"Debug targets: {enabled_targets}")
```

### Anomaly Detection

Automatic anomaly detection and alerting:

```python
# Check for anomalies
if agent.detectAnomalies():
    anomalies = agent.getAnomalies()
    for anomaly in anomalies:
        print(f"⚠️  ANOMALY: {anomaly}")
        # Trigger alert or logging
        agent.emitDebugEvent("anomaly", anomaly)
```

### Performance Monitoring

Continuous performance monitoring with historical tracking:

```python
# Get comprehensive performance metrics
performance = agent.getPerformanceMetrics()
print(f"Step time: {performance.lastStepTime}ms")
print(f"Neurons/sec: {performance.neuronsPerSecond}")
print(f"Firing rate: {performance.spikesPerStep}")

# Performance history analysis
performance_history = agent.getPerformanceHistory(100)
latest = performance_history[-1]
average = sum(m.neuronsPerSecond for m in performance_history) / len(performance_history)

print(f"Average neurons/sec: {average}")
print(f"Performance variation: {((latest.neuronsPerSecond - average) / average * 100):.1f}%")
```

## Plugin System and Extensibility

The plugin system allows for advanced extensions and custom functionality.

### Plugin Development

```python
import pynlm

# Create custom plugin
class AnalysisPlugin(pynlm.AgentBrain.Plugin):
    def __init__(self):
        self.agent = None
        self.data_collection = []
    
    def initialize(self, agent):
        self.agent = agent
        print(f"Plugin '{self.getName()}' initialized")
        # Register for performance updates
        agent.addPerformanceListener(self.on_performance_update)
    
    def update(self, timestep):
        # Custom analysis logic
        if hasattr(self.agent, 'getDebugInfo'):
            debug = self.agent.getDebugInfo()
            self.data_collection.append({
                'timestamp': timestep,
                'dopamine': debug.dopamineLevel,
                'curiosity': debug.curiosityLevel,
                'firing_rate': debug.averageFiringRate
            })
    
    def shutdown(self):
        print(f"Plugin '{self.getName()}' shutdown")
        # Clean up resources
        self.data_collection.clear()
    
    def getName(self):
        return "BehaviorAnalysisPlugin"
    
    def getCollectedData(self):
        return self.data_collection
    
    def on_performance_update(self, debug_info):
        # Custom analysis on performance updates
        if debug_info.dopamineLevel > 0.6:
            print(f"[Plugin] High dopamine detected: {debug_info.dopamineLevel}")

# Register and use plugin
plugin = AnalysisPlugin()
agent.registerPlugin(pynlm.AgentBrain.PluginPtr(plugin))

# Later, access plugin data
if "BehaviorAnalysisPlugin" in agent.getRegisteredPlugins():
    plugin = agent.getPlugin("BehaviorAnalysisPlugin")
    data = plugin.getCollectedData()
    print(f"Collected {len(data)} data points")
```

### Custom Modules

Add custom modules for specialized functionality:

```python
# Define custom module
def learning_rate_adjuster(agent, timestep):
    """Custom learning rate adjustment based on performance"""
    debug = agent.getDebugInfo()
    
    if debug.averageFiringRate > 10.0:
        agent.setConfig("learning_rate", 0.005)  # Reduce if firing too high
    elif debug.averageFiringRate < 1.0:
        agent.setConfig("learning_rate", 0.02)   # Increase if firing too low

# Register custom module
agent.addCustomModule("learning_rate_adjuster", learning_rate_adjuster)

# The module will be called every simulation step
```

## Troubleshooting Common Issues

### Common Problems and Solutions

#### Problem: Brain Initialization Fails

**Symptoms:**
- "Failed to initialize brain!" error
- Segmentation fault during initialization

**Solutions:**
```python
# Check configuration
config = pynlm.createDefaultConfig()
if not config.has("neuron_count"):
    config.set("neuron_count", 1000)

# Validate configuration
if not config.validate():
    print("Configuration validation failed")
    # Fix configuration issues

# Try with simpler configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)
```

#### Problem: Simulation Runs Slowly

**Symptoms:**
- Very slow simulation performance
- High memory usage
- Frequent performance degradation

**Solutions:**
```python
# Check performance metrics
metrics = agent.getPerformanceMetrics()
if metrics.neuronsPerSecond < 1000:
    print(f"Low performance: {metrics.neuronsPerSecond} neurons/sec")

# Enable performance profiling
agent.addPerformanceListener(lambda debug: print(f"Step {debug.lastUpdate}: {debug.averageFiringRate} Hz"))

# Check for memory issues
debug_info = agent.getDebugInfo()
if debug_info.workingMemorySize > 10000:
    print("High memory usage - consider reducing memory capacity")
```

#### Problem: Learning is Not Occurring

**Symptoms:**
- Synaptic weights remain unchanged
- Low firing rates
- No observable behavior changes

**Solutions:**
```python
# Check plasticity settings
agent.enableStructuralPlasticity(True)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Check neuromodulation levels
debug_info = agent.getDebugInfo()
print(f"Dopamine: {debug_info.dopamineLevel}")
print(f"Curiosity: {debug_info.curiosityLevel}")

# Adjust parameters if needed
if debug_info.dopamineLevel < 0.1:
    print("Low dopamine - may need reward signal")

if debug_info.curiosityLevel < 0.2:
    print("Low curiosity - may need exploration rewards")
```

#### Problem: Memory Issues

**Symptoms:**
- High memory usage
- Performance degradation
- System instability

**Solutions:**
```python
# Check memory usage
debug_info = agent.getDebugInfo()
print(f"Working memory: {debug_info.workingMemorySize}")
print(f"Episodic memory: {debug_info.episodicMemorySize}")

# Optimize memory usage
agent.optimizeMemoryUsage()

# Clear anomalies
agent.clearAnomalies()

# Check for memory leaks
if agent.hasMemoryLeaks():
    agent.garbageCollect()
```

### Debugging Tools

```python
# Enable comprehensive debugging
agent.setDebugTarget("all", True)

# Get detailed debug information
debug_info = agent.getDebugInfo()

# Log debug information
import json
debug_json = json.dumps(debug_info.__dict__, indent=2)
with open("debug_output.json", "w") as f:
    f.write(debug_json)

# Check for specific issues
if agent.detectAnomalies():
    anomalies = agent.getAnomalies()
    for anomaly in anomalies:
        print(f"🔍 ANOMALY: {anomaly}")
        # Get detailed information about anomaly
        agent.emitDebugEvent("anomaly_detected", anomaly)
```

## API Reference

### Core Classes

#### AgentBrain

**Constructor:**
```python
AgentBrain(brain)
```

**Methods:**
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward modulation
- `updateDevelopment(timestep)` - Update development system

**Advanced Methods:**
- `setConfig(key, value)` - Set runtime configuration
- `getConfig(key)` - Get configuration value
- `getDebugInfo()` - Get comprehensive debug information
- `getPerformanceMetrics()` - Get performance metrics
- `detectAnomalies()` - Detect anomalies
- `addStateChangeListener(key, callback)` - Add state change listener
- `registerPlugin(plugin)` - Register plugin
- `addCustomModule(name, module)` - Add custom module

#### Config

**Constructor:**
```python
Config()
```

**Methods:**
- `loadFromFile(filepath)` - Load from file
- `loadFromJSON(filepath)` - Load from JSON
- `loadFromYAML(filepath)` - Load from YAML
- `saveToJSON(filepath)` - Save to JSON
- `saveToYAML(filepath)` - Save to YAML
- `validate()` - Validate configuration
- `addValidationRule(rule)` - Add validation rule
- `getSchema()` - Get configuration schema

### Configuration Value Types

The configuration system supports the following types:

- `int` - Integer values
- `int64_t` - 64-bit integer values
- `double` - Floating-point values
- `bool` - Boolean values
- `string` - String values
- `vector<int>` - Integer arrays
- `vector<double>` - Floating-point arrays
- `vector<string>` - String arrays

### Error Handling

The enhanced error handling system provides detailed error information:

**Error Types:**
- `ConfigFileNotFound` - Configuration file not found
- `InvalidConfigValue` - Invalid configuration value
- `MissingRequiredConfig` - Required configuration missing
- `ConfigValidationFailed` - Configuration validation failed
- `TypeConversionFailed` - Type conversion failed
- `OutOfRange` - Value out of range

**Usage:**
```python
try:
    config.set("learning_rate", 0.5)
except pynlm.ConfigError as e:
    print(f"Error: {e}")
    print(f"Code: {e.getCode()}")
    print(f"Context: {e.getContext()}")
```

## Conclusion

The enhanced NLM framework provides comprehensive tools for advanced users, including:

1. **Advanced Command-Line Interface** - Extensive CLI options for simulation control
2. **Runtime Configuration** - Dynamic configuration modification during runtime
3. **Comprehensive Debugging** - Detailed debugging and monitoring capabilities
4. **Performance Profiling** - Advanced performance analysis and optimization
5. **Plugin System** - Extensible plugin architecture for custom functionality
6. **Configuration Validation** - Robust configuration validation and schema support
7. **Error Handling** - Enhanced error handling with detailed diagnostics

These features enable researchers and developers to conduct sophisticated experiments, optimize performance, and extend the framework with custom functionality for their specific research needs.

---

*This documentation is continuously updated. For the latest information, refer to the official NLM documentation repository.*
