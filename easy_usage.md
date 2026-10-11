# NLM Advanced Features - Documentation

## Overview

This document provides comprehensive documentation for the advanced features implemented in the NLM (Neural Learning Machine) codebase, including:

1. **Visualization Enhancement**
2. **Performance Monitoring**
3. **Advanced Configuration**
4. **System Improvements**

## 1. Visualization Enhancement

### Introduction

The visualization system has been enhanced to provide real-time monitoring and debugging capabilities for neural simulations. The VisualizationInterface class offers multiple visualization modes for different aspects of the neural network.

### Classes

#### VisualizationInterface

**Purpose:** Main visualization interface for neural simulations

**Key Methods:**

- `initialize()`: Initialize visualization system
- `update()`: Update visualization data
- `render()`: Render current state
- `close()`: Close visualization
- `isActive()`: Check if visualization is active
- `setUpdateRate()`: Set update rate in Hz
- `getUpdateRate()`: Get update rate in Hz
- `visualizeNetwork()`: Visualize network structure
- `visualizeActivity()`: Visualize neural activity
- `visualizeSpikes()`: Visualize spike raster plot
- `visualizeWeights()`: Visualize weight matrix

**Usage Example:**

```python
import pynlm

# Create visualization interface
vis = pynlm.createVisualization()

# Initialize visualization
vis.initialize()
vis.setUpdateRate(30.0)  # 30 Hz update rate

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()

# Main simulation loop
while True:
    brain.step(1)
    world.update(1)
    
    # Update visualization
    vis.update()
    
    # Visualize network structure
    vis.visualizeNetwork(brain)
    
    # Visualize neural activity
    vis.visualizeActivity(brain)

# Close visualization
vis.close()
```

### Visualization Modes

1. **Network Visualization**: Shows the structural layout of the neural network, including regions, populations, and connections
2. **Activity Visualization**: Displays real-time neural activity patterns and firing rates
3. **Spike Raster Plot**: Visualizes spike timing across all neurons
4. **Weight Matrix Visualization**: Shows synaptic weight distributions and patterns

### Configuration

Visualization configuration is handled through the Config class:

```python
# Enable/disable visualization
config.set("visualization.enabled", True)

# Set visualization update rate
config.set("visualization.updateRate", 30.0)

# Visualization settings
config.set("visualization.network.enabled", True)
config.set("visualization.activity.enabled", True)
config.set("visualization.spikes.enabled", True)
config.set("visualization.weights.enabled", True)
```

## 2. Performance Monitoring

### Introduction

The performance monitoring system provides comprehensive real-time metrics and profiling capabilities for neural simulations. The PerformanceStats struct collects and reports detailed performance information.

### Classes

#### PerformanceStats

**Purpose:** Performance statistics container

**Attributes:**

- `neuronsPerSecond`: Number of neuron operations per second
- `spikesPerSecond`: Number of spike events per second
- `synapticEventsPerSecond`: Number of synaptic events per second
- `realTimeFactor`: Simulation speed relative to real time
- `activeNeurons`: Number of currently active neurons
- `firingNeurons`: Number of neurons firing in current step
- `pendingSpikes`: Number of pending spike events
- `pendingDelayedSpikes`: Number of pending delayed spike events
- `avgFiringRate`: Average firing rate across all neurons
- `memoryUsageMB`: Current memory usage in megabytes
- `stepTimeMs`: Time for current simulation step
- `updateTimeMs`: Time for data updates
- `renderTimeMs`: Time for visualization rendering
- `visualizationFrames`: Number of visualization frames rendered

**Usage Example:**

```python
import pynlm

# Create brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get performance statistics
stats = brain.getPerformanceStats()

print(f"Neurons per second: {stats.neuronsPerSecond}")
print(f"Spikes per second: {stats.spikesPerSecond}")
print(f"Active neurons: {stats.activeNeurons}")
print(f"Memory usage: {stats.memoryUsageMB} MB")
print(f"Step time: {stats.stepTimeMs} ms")

# Log performance report
brain.logPerformanceReport()
```

### Performance Monitoring Features

1. **Real-time Metrics**: Continuous monitoring of simulation performance
2. **Timing Measurements**: Detailed timing for different simulation phases
3. **Resource Usage**: Memory and CPU usage tracking
4. **Performance Profiling**: Detailed analysis of performance bottlenecks
5. **Progress Tracking**: Simulation progress and completion metrics

### Configuration

Performance monitoring configuration:

```python
# Enable/disable performance monitoring
config.set("performance.monitoring", True)

# Enable/disable detailed profiling
config.set("performance.profiling", True)

# Performance settings
config.set("performance.stats.updateInterval", 100)  # Update stats every 100 steps
config.set("performance.memory.track", True)
config.set("performance.cpu.track", True)
config.set("performance.timing.detailed", True)
```

## 3. Advanced Configuration

### Configuration System

The configuration system provides a flexible, type-safe configuration management system using JSON files and command-line arguments.

### Configuration Classes

#### Config

**Purpose:** Main configuration class for NLM system

**Key Methods:**

- `loadFromFile()`: Load configuration from JSON file
- `loadFromArgs()`: Load configuration from command line arguments
- `saveToFile()`: Save configuration to JSON file
- `get()`: Get configuration value by key
- `getOr()`: Get configuration value with default
- `set()`: Set configuration value
- `has()`: Check if configuration key exists
- `getKeys()`: Get all configuration keys
- `clear()`: Clear all configuration
- `summary()`: Get configuration summary

### Configuration Options

#### Visualization Configuration

```json
{
    "visualization": {
        "enabled": true,
        "updateRate": 30.0,
        "network": {
            "enabled": true,
            "showConnections": true,
            "showPopulations": true
        },
        "activity": {
            "enabled": true,
            "showFiringRates": true,
            "showVoltage": false
        },
        "spikes": {
            "enabled": true,
            "showRaster": true,
            "showTimestamps": true
        },
        "weights": {
            "enabled": true,
            "showMatrix": true,
            "showDistribution": true
        }
    }
}
```

#### Performance Configuration

```json
{
    "performance": {
        "monitoring": {
            "enabled": true,
            "updateInterval": 100,
            "trackMemory": true,
            "trackCPU": true,
            "timingDetailed": true
        },
        "profiling": {
            "enabled": false,
            "outputFile": "profile.json",
            "samplingInterval": 1000
        },
        "optimization": {
            "enableSIMD": true,
            "enableMultithreading": true,
            "enableEventDriven": true,
            "enableSparseConnectivity": true,
            "threadCount": 4
        }
    }
}
```

#### System Performance Tuning

```json
{
    "system": {
        "performance": {
            "simulationTimestep": 0.001,
            "maxNeurons": 100000,
            "memoryPoolSize": 1048576,
            "enableOptimizations": true,
            "compilerFlags": "-O3 -march=native"
        }
    }
}
```

### Configuration Loading

**JSON File Loading:**

```python
# Load configuration from file
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
```

**Command Line Loading:**

```python
# Load configuration from command line arguments
import sys
config = pynlm.createDefaultConfig()
config.loadFromArgs(len(sys.argv), sys.argv)
```

**Saving Configuration:**

```python
# Save configuration to file
config.saveToFile("config_output.json")
```

## 4. System Improvements

### Build System Enhancements

#### CMake Configuration

The CMake build system has been enhanced with the following improvements:

1. **Performance Optimization Options**: Support for SIMD, multithreading, event-driven processing
2. **Visualization Build Options**: Conditional compilation of visualization modules
3. **Compiler Optimization Flags**: Automatic selection of optimization flags based on compiler
4. **Parallel Processing**: Support for multi-core processing
5. **Memory Management**: Efficient memory allocation strategies

#### Build Options

```bash
# Configure with CMake
cmake -B build \
    -DNLM_PERFORMANCE_ENABLED=ON \
    -DNLM_VISUALIZATION_ENABLED=ON \
    -DNLM_ENABLE_PROFILING=OFF \
    -DNLM_THREAD_COUNT=4

# Build
cmake --build build

# Run
./build/nlm
```

### Performance Optimization Features

1. **SIMD Vectorization**: Automatic SIMD instruction generation for neural computations
2. **Multithreading**: Parallel processing of neural populations
3. **Event-Driven Processing**: Efficient handling of spike events
4. **Memory Pools**: Pre-allocated memory for improved performance
5. **Structure-of-Arrays (SoA)**: Better cache utilization for neural data
6. **Sparse Connectivity**: Efficient storage of neural connections

### Installation

**Binary Installation:**

```bash
# Install dependencies
sudo apt-get install build-essential cmake python3-dev

# Clone and build
mkdir build
cd nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install
sudo make install
```

**Python Package Installation:**

```bash
# Install Python dependencies
pip install pybind11 numpy matplotlib

# Build and install Python bindings
cd python
mkdir build
cd build
cmake .. -DPYTHON_EXECUTABLE=$(which python3)
make
pip install .
```

### Usage Examples

#### Basic Neural Simulation

```python
import pynlm

# Create simulation components
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)

brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent_brain = pynlm.createAgentBrain(brain)

# Initialize
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    world.update(step)
    
    # Get performance stats
    stats = brain.getPerformanceStats()
    
    # Update visualization if enabled
    # (visualization code would go here)
    
    if step % 100 == 0:
        brain.logPerformanceReport()
```

#### Advanced Configuration

```python
import pynlm
import json

# Load advanced configuration
with open("advanced_config.json", "r") as f:
    advanced_config = json.load(f)

# Create and configure
config = pynlm.createDefaultConfig()
for key, value in advanced_config.items():
    if isinstance(value, dict):
        # Handle nested configuration
        for sub_key, sub_value in value.items():
            full_key = f"{key}.{sub_key}"
            config.set(full_key, sub_value)
    else:
        config.set(key, value)

# Create brain with advanced configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Enable performance monitoring
brain.resetPerformanceStats()
```

## 5. API Reference

### Python API Summary

#### Core Classes

- `Config`: Configuration management
- `Brain`: Neural simulation engine
- `AgentBrain`: Agent-brain interface
- `SimpleWorld`: 2D simulation world
- `VisualizationInterface`: Visualization system
- `PerformanceStats`: Performance metrics

#### Key Functions

- `createDefaultConfig()`: Create default configuration
- `createBrain()`: Create neural brain
- `createSimpleWorld()`: Create simulation world
- `createAgentBrain()`: Create agent brain interface
- `createVisualization()`: Create visualization interface

#### Constants

- `INVALID_NEURON_ID`: Invalid neuron ID
- `INVALID_SYNAPSE_ID`: Invalid synapse ID
- `INVALID_REGION_ID`: Invalid region ID
- `INVALID_POPULATION_ID`: Invalid population ID

### C++ API Summary

#### Core Classes

- `nlm::Config`: Configuration system
- `nlm::Brain`: Neural simulation engine
- `nlm::AgentBrain`: Agent-brain interface
- `nlm::SimpleWorld`: Simulation world
- `nlm::VisualizationInterface`: Visualization system
- `nlm::PerformanceStats`: Performance metrics

#### Methods

- `Brain::initialize()`: Initialize brain
- `Brain::step()`: Perform simulation step
- `Brain::receiveSensoryInput()`: Inject sensory input
- `Brain::produceAction()`: Generate motor output
- `Brain::getPerformanceStats()`: Get performance statistics
- `Brain::logPerformanceReport()`: Log performance report
- `Brain::resetPerformanceStats()`: Reset performance statistics
- `VisualizationInterface::initialize()`: Initialize visualization
- `VisualizationInterface::update()`: Update visualization
- `VisualizationInterface::render()`: Render visualization

## 6. Troubleshooting

### Common Issues

#### Visualization Not Working

**Cause:** Visualization not enabled in configuration

**Solution:**
```python
config.set("visualization.enabled", True)
brain.setVisualizationInterface(vis)
```

#### Performance Monitoring Slow

**Cause:** Performance monitoring disabled

**Solution:**
```python
config.set("performance.monitoring", True)
brain.resetPerformanceStats()
```

#### Configuration Loading Errors

**Cause:** Invalid JSON or missing file

**Solution:**
```python
try:
    config.loadFromFile("config.json")
except Exception as e:
    print(f"Error loading config: {e}")
```

### Performance Tips

1. **Enable Performance Optimizations:** Set `NLM_PERFORMANCE_ENABLED=ON` in CMake
2. **Use Appropriate Thread Count:** Set `NLM_THREAD_COUNT` based on available cores
3. **Optimize Configuration:** Use appropriate settings for your simulation scale
4. **Monitor Performance:** Use `logPerformanceReport()` to identify bottlenecks
5. **Profile Regularly:** Use performance profiling to optimize simulations

## 7. Future Enhancements

### Planned Features

1. **3D Visualization**: Enhanced visualization with 3D rendering
2. **Advanced Profiling**: More detailed performance analysis tools
3. **Distributed Computing**: Support for multi-node simulations
4. **Machine Learning Integration**: Integration with ML frameworks
5. **Real-time Data Streaming**: Support for real-time data visualization
6. **Export Capabilities**: Export of simulation results in multiple formats
7. **Plugin System**: Extensible plugin architecture for new features

### Research Directions

1. **Neuromorphic Computing**: Hardware-accelerated neural simulation
2. **Advanced Plasticity**: Learning mechanisms inspired by biological systems
3. **Cognitive Architectures**: Integration of cognitive functions
4. **Bio-inspired Models**: More biologically realistic neural models
5. **Energy-efficient Computing**: Low-power neural simulation

## 8. License

This documentation is provided under the terms of the NLM license. For more information, see the LICENSE file in the project root.

## 9. Acknowledgments

Special thanks to all contributors and researchers who have advanced the NLM framework. This work builds upon decades of research in computational neuroscience and artificial intelligence.

---

*Documentation generated automatically from NLM source code and implementation.*
