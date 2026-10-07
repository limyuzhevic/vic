# Debug and Visualization System

## Overview

This document describes the enhanced debugging and visualization tools for NLM (Neural Learning Machine). The system provides comprehensive debugging utilities, real-time visualization, and monitoring capabilities to help researchers understand and analyze neural network behavior.

## Components

### 1. BrainStateDebugger
A comprehensive debugger that provides detailed information about brain state, neuron properties, and network dynamics.

### 2. Real-time Visualization System
- Network topology visualization
- Activity monitoring and heatmaps
- Spike raster plots
- Weight matrix visualization
- Memory system state monitoring

### 3. Performance Profiler
- Time profiling of different brain operations
- Memory usage tracking
- Event system monitoring
- Plasticity rate analysis

### 4. Debug Logging System
- Enhanced logging with levels and filtering
- Debug mode for detailed output
- Performance timing utilities

## Features

### Brain State Debugging
- Neuron state inspection (membrane potential, firing state, etc.)
- Synapse connectivity analysis
- Network statistics and metrics
- Regional activity patterns
- Spike history and timing analysis

### Real-time Visualization
- Web-based visualization interface
- Console-based text output
- Export capabilities for data analysis
- Interactive controls (pause, step, reset)

### Performance Monitoring
- Operation timing
- Memory allocation tracking
- Event queue monitoring
- Plasticity rate analysis

### Debug Utilities
- State serialization/deserialization for debugging
- Network reset utilities
- Configuration debugging
- Input/output tracing

## Usage Examples

### Basic Debugging
```python
import pynlm

# Create brain and enable debugging
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get debugger instance
debugger = brain.getDebugInterface()

# Print detailed brain state
debugger.printBrainState()

# Visualize network topology
debugger.visualizeNetworkTopology()

# Monitor activity for 100 steps
for i in range(100):
    brain.step(i)
    debugger.update()

# Analyze spike patterns
debugger.analyzeSpikePatterns()
```

### Advanced Visualization
```python
import pynlm

# Create visualization interface
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize visualization
viz = brain.getVisualizationInterface()
viz.initialize()

# Configure visualization options
viz.setMode("real_time")
viz.setUpdateRate(30.0)
viz.enableActivityHeatmap(True)
viz.enableSpikeRaster(True)
viz.enableWeightMatrix(True)

# Run simulation with visualization
for i in range(1000):
    brain.step(i)
    viz.update()
    
    # Save snapshots periodically
    if i % 100 == 0:
        viz.saveSnapshot(f"visualization_step_{i}.png")

viz.close()
```

### Performance Profiling
```python
import pynlm
import time

# Create brain with profiling enabled
config = pynlm.createDefaultConfig()
config.set("enable_profiling", True)
brain = pynlm.createBrain(config)
brain.initialize()

# Get profiler
profiler = brain.getProfiler()

# Run simulation with profiling
start_time = time.time()
for i in range(1000):
    brain.step(i)
end_time = time.time()

# Print profiling results
profiler.printReport()
profiler.exportResults("profile_data.json")
```

### Debug Mode
```python
import pynlm

# Enable debug mode
config = pynlm.createDefaultConfig()
config.set("debug_level", "verbose")
config.set("enable_debug_logging", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Get debug interface
debug = brain.getDebugInterface()

# Set up detailed logging
debug.enableNeuronDebugLogging(True)
debug.enableSynapseDebugLogging(True)
debug.enablePlasticityDebugLogging(True)

# Run with debug output
for i in range(100):
    # Inject specific inputs for debugging
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
    
    brain.step(i)
    debug.logStepDetails(i)

# Generate debug report
debug.generateDebugReport("debug_output.txt")
```

## Configuration Options

### Visualization Configuration
```python
viz_config = {
    "mode": "web",  # web, console, file
    "update_rate": 30.0,
    "activity_heatmap": True,
    "spike_raster": True,
    "weight_matrix": True,
    "network_topology": True,
    "export_formats": ["png", "json", "csv"]
}
```

### Debug Configuration
```python
debug_config = {
    "level": "info",  # debug, info, warning, error
    "neuron_logging": True,
    "synapse_logging": True,
    "plasticity_logging": True,
    "event_logging": True,
    "timing_details": True,
    "memory_tracking": True
}
```

### Performance Configuration
```python
perf_config = {
    "enable_timing": True,
    "time_all_operations": True,
    "memory_tracking": True,
    "event_queue_monitoring": True,
    "export_interval": 100,
    "report_format": "json"
}
```

## API Reference

### BrainStateDebugger Class
```python
class BrainStateDebugger:
    def print_brain_state(self)
    def print_neuron_details(self, neuron_id)
    def print_synapse_details(self, synapse_id)
    def visualize_network_topology(self)
    def analyze_spike_patterns(self, neuron_id=None)
    def analyze_plasticity_activity(self)
    def analyze_memory_activity(self)
    def get_brain_statistics(self)
    def save_state_snapshot(self, filepath)
    def compare_states(self, brain1, brain2)
```

### VisualizationInterface Class
```python
class VisualizationInterface:
    def initialize(self, config=None)
    def update(self)
    def render(self)
    def close(self)
    def is_active(self)
    def set_mode(self, mode)
    def set_update_rate(self, hz)
    def enable_activity_heatmap(self, enable)
    def enable_spike_raster(self, enable)
    def enable_weight_matrix(self, enable)
    def enable_network_topology(self, enable)
    def save_snapshot(self, filepath, format="png")
    def export_data(self, filepath, data_type="all")
    def connect_to_viewer(self, host, port)
```

### PerformanceProfiler Class
```python
class PerformanceProfiler:
    def start_timing(self, operation_name)
    def stop_timing(self, operation_name)
    def get_timing_report(self)
    def export_results(self, filepath, format="json")
    def print_report(self)
    def reset(self)
    def set_sampling_rate(self, rate)
    def enable_memory_tracking(self, enable)
```

## Files Created/Modified

### Core Files
- `src/debugging/BrainStateDebugger.hpp` - Main debugging interface
- `src/debugging/BrainStateDebugger.cpp` - Implementation
- `src/visualization/RealTimeVisualization.hpp` - Enhanced visualization interface
- `src/visualization/RealTimeVisualization.cpp` - Implementation
- `src/performance/Profiler.hpp` - Performance profiling interface
- `src/performance/Profiler.cpp` - Implementation

### Python Bindings
- `python/bindings.cpp` - Enhanced Python bindings for debugging tools
- `python/debugging.py` - High-level Python debugging utilities

### Configuration
- `config/debug_config.json` - Default debug configuration
- `config/visualization_config.json` - Default visualization configuration
- `config/performance_config.json` - Default performance configuration

### Documentation
- `docs/debugging_guide.md` - Comprehensive debugging guide
- `docs/visualization_guide.md` - Visualization documentation
- `docs/performance_guide.md` - Performance monitoring documentation

## Installation and Setup

### Building with Debugging Support
```bash
cmake .. -DENABLE_DEBUGGING=ON -DENABLE_VISUALIZATION=ON -DENABLE_PROFILING=ON
make -j4
```

### Running with Debugging
```bash
./nlm_debug --debug-level=verbose --enable-visualization
```

### Python Usage
```bash
# Install with debugging support
pip install .[debug]

# Import debugging modules
import pynlm.debugging as nlm_debug
import pynlm.visualization as nlm_viz
```

## Troubleshooting

### Common Issues
1. **Visualization not showing**: Check if display is available or use file export mode
2. **Performance slowdown**: Reduce visualization update rate
3. **Memory usage high**: Increase garbage collection frequency
4. **Debug logging too verbose**: Increase log level to warning or error

### Debugging Tips
1. Use debug mode for detailed output
2. Export data for offline analysis
3. Use snapshots for comparing different states
4. Profile specific operations for bottlenecks

## Future Enhancements

1. **Web-based visualization**: Real-time web interface for remote monitoring
2. **Machine learning integration**: Use ML to identify patterns in neural activity
3. **Automated debugging**: AI-powered debugging suggestions
4. **Multi-platform support**: Export for different analysis tools
5. **Plugin system**: Allow third-party debug and visualization plugins

## Acknowledgments

This debugging and visualization system builds upon the existing NLM architecture and enhances it with comprehensive debugging and visualization capabilities for research and development purposes.