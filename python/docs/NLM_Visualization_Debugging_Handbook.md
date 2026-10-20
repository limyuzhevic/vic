# NLM Visualization and Debugging Tools Documentation

This document provides comprehensive information about the NLM (Neural Learning Machine)
visualization and debugging tools. These tools provide enhanced capabilities for
monitoring, visualizing, and debugging neural simulations.

## Overview

The NLM visualization and debugging tools are designed to provide comprehensive
insights into neural simulation behavior. They include:

1. **Visualization Tools**: Real-time visualization of brain activity, world simulation,
   and neural network structures
2. **Debugging Tools**: Live debugging interface with state inspection and error detection
3. **Advanced Monitoring**: Performance monitoring, memory tracking, and neural activity metrics
4. **Interactive Features**: Command-line interface and web-based debugging dashboard
5. **Python Integration**: Python bindings for easy integration and scripting

## Key Features

### 1. Visualization Interface

The `VisualizationInterface` class provides comprehensive visualization capabilities:

#### Brain Activity Visualization
- Real-time neuron spike visualization
- Synaptic weight visualization
- Population activity visualization
- 2D and 3D neural network representations

#### World Simulation Visualization
- Agent movement visualization
- Environment map visualization
- Resource distribution visualization
- Interaction visualization

#### Time-Series Visualization
- Simulation progress tracking
- Performance metrics visualization
- Neural activity trends
- Error and warning trends

#### Heat Maps and Activity Patterns
- Activity heat maps for neural populations
- World activity heat maps
- Custom color schemes for different data types
- Static and animated visualizations

#### Configuration
- Real-time updates
- Update rate control
- Visualization modes (Static, RealTime, Animated, TimeSeries, HeatMap, 3DNeural, WorldSimulation)
- Output formats (Text, JSON, CSV, Binary, WebSocket, NetworkStream)
- Interactive mode support

### 2. Debug Interface

The `DebugInterface` class provides comprehensive debugging capabilities:

#### Real-time Debugging
- Live state inspection of neurons, synapses, and system components
- Step-by-step simulation debugging
- Performance profiling (CPU, memory, I/O)
- Memory leak detection and profiling

#### Error Detection and Recovery
- Real-time error detection
- Error classification and severity analysis
- Recovery analysis
- Error logging and reporting

#### State Inspection
- Neuron state inspection (membrane potential, firing rate, etc.)
- Synapse state inspection (weight, eligibility trace, etc.)
- System state inspection (performance metrics, resource usage, etc.)

#### Configuration Validation
- Configuration validation and debugging
- Error detection in configuration files
- Configuration fix suggestions

### 3. Advanced Monitoring

The tools provide advanced monitoring capabilities:

#### Performance Monitoring
- CPU usage tracking
- Memory usage monitoring
- Disk usage monitoring
- Network I/O tracking
- System resource usage

#### Neural Activity Monitoring
- Firing rate monitoring
- Neuron activity patterns
- Population-level statistics
- Connectivity analysis

#### Resource Usage Tracking
- Memory pool usage
- Event queue size
- Checkpoint sizes
- Temperature monitoring

### 4. Interactive Features

The tools provide multiple interactive interfaces:

#### Command-line Debugging Interface
- Text-based command-line interface
- Scriptable debugging commands
- Interactive help system
- Command history and completion

#### Web-based Debugging Dashboard
- HTML/HTTP-based dashboard
- Real-time data updates
- Interactive visualizations
- Mobile-friendly interface

#### Scriptable Debugging Commands
- Python-based scripting
- Automated debugging scenarios
- Reproducible debugging workflows
- Custom command creation

### 5. Python Integration

The tools provide comprehensive Python bindings:

#### Python Native Implementation
- `VisualizationInterface`: Main visualization class
- `DebugInterface`: Main debugging class
- `NLMVisualizationTools`: Main entry point
- `Data structures`: Python wrappers for all data types

#### Usage Examples
```python
from nlm_visualization_tools import (
    VisualizationInterface,
    DebugInterface,
    VisualizationMode,
    DebugMode,
    NLMVisualizationTools
)

# Initialize tools
tools = NLMVisualizationTools()
tools.initialize()

# Get interfaces
viz = tools.get_visualization_interface()
debug = tools.get_debug_interface()

# Configure tools
viz.set_update_rate(30.0)
viz.set_visualization_mode(VisualizationMode.REAL_TIME)

debug.set_debug_mode(DebugMode.DETAILED)
debug.enable_memory_leak_detection(True)
debug.enable_performance_profiling(True)

# Run simulation
for step in range(100):
    viz.update()
    debug.update()
    
    # Add data
    viz.add_time_series_metric("step", float(step))
    viz.add_time_series_metric("firing_rate", 10.0 + (step * 0.1))
    
    # Debug
    debug.log_error("SIMULATION_ERROR", f"Error at step {step}")

# Generate report
report = debug.generate_debug_report()
print(report)

# Export data
tools.export_data("simulation_report.json")

# Cleanup
tools.close()
```

## Installation

### Prerequisites

- Python 3.8 or higher
- CMake 3.16 or higher
- C++ compiler with C++20 support

### Installation Steps

1. **Clone the repository**
```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

2. **Build the C++ library**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

3. **Install Python bindings**
```bash
cd python
pip install -e .
```

4. **Verify installation**
```bash
python -c "from nlm_visualization_tools import NLMVisualizationTools; print('NLM tools available')"
```

### Alternative Installation (Using pip)

```bash
pip install nlm-visualization-tools
```

## Usage

### Basic Usage

```python
from nlm_visualization_tools import NLMVisualizationTools

# Initialize tools
tools = NLMVisualizationTools()
tools.initialize()

# Get interfaces
viz = tools.get_visualization_interface()
debug = tools.get_debug_interface()

# Configure and run simulation
viz.set_update_rate(30.0)
debug.set_debug_mode(DebugMode.DETAILED)

# Run your simulation...
for step in range(1000):
    viz.update()
    debug.update()
    # Your simulation code here

# Generate report
report = debug.generate_debug_report()
print(report)

# Export data
tools.export_data("simulation_report.json")

# Cleanup
tools.close()
```

### Advanced Usage

#### Time Series Monitoring

```python
# Add time series data
viz.add_time_series_metric("cpu_usage", 50.0)
viz.add_time_series_metric("memory_usage", 60.0)
viz.add_time_series_metric("firing_rate", 12.5)

# Retrieve time series data
cpu_data = viz.get_time_series_data("cpu_usage")
firing_data = viz.get_time_series_data("firing_rate")
```

#### Debug Monitoring

```python
# Enable debug features
debug.enable_error_detection(True)
debug.enable_memory_leak_detection(True)
debug.enable_performance_profiling(True)

# Log errors
debug.log_error("MEMORY_LEAK", "Memory leak detected at address 0x12345678")
debug.log_error("PERFORMANCE", "High CPU usage detected", is_critical=True)

# Get debug info
recent_errors = debug.get_recent_errors()
critical_errors = debug.get_critical_errors()
debug_infos = viz.get_debug_info()
```

#### Monitoring Hooks

```python
# Define callback functions
def custom_callback(metrics):
    if metrics.cpu_usage > 80.0:
        print(f"High CPU usage: {metrics.cpu_usage}%")

# Create monitoring hook
from nlm_visualization_tools import MonitoringHook

hook = MonitoringHook(
    hook_name="performance_monitor",
    callback=custom_callback,
    last_execution=time.time(),
    execution_interval=1.0,
    is_active=True,
    hook_type="periodic"
)

# Add hook to visualization interface
viz.add_monitoring_hook(hook)
```

### Demo Scripts

The repository includes several demo scripts:

1. **main_demo.py**: Comprehensive demo showing all features
2. **demo_example.py**: Detailed example with explanations
3. **usage_example.py**: Basic usage examples
4. **test_nlm_tools.py**: Test suite for the tools

#### Running Demos

```bash
cd python
python main_demo.py
python demo_example.py
python usage_example.py
python test_nlm_tools.py
```

## Configuration

### Configuration File

The tools support configuration files for persistent settings:

```python
# Load configuration from file
viz.load_configuration("visualization_config.json")
debug.load_debug_configuration("debug_config.json")

# Save configuration
viz.save_configuration("visualization_config.json")
debug.save_debug_configuration("debug_config.json")
```

### Sample Configuration Files

**visualization_config.json**:
```json
{
    "update_rate": 30.0,
    "visualization_mode": "real_time",
    "output_format": "text",
    "debug_overlay_enabled": true,
    "real_time_updates": true,
    "frame_rate_limit": 60.0
}
```

**debug_config.json**:
```json
{
    "debug_mode": "detailed",
    "monitoring_mode": "active",
    "real_time_debugging_enabled": true,
    "memory_leak_detection_enabled": true,
    "performance_profiling_enabled": true,
    "error_detection_enabled": true,
    "step_by_step_debugging_enabled": false,
    "command_line_interface_enabled": true,
    "debug_output_enabled": true,
    "output_level": 2
}
```

## Data Structures

The tools include comprehensive data structures for all types of data:

### NeuronActivityData
- `neuron_id`: Unique neuron identifier
- `membrane_potential`: Current membrane potential
- `firing_rate`: Current firing rate in Hz
- `inhibition_level`: Inhibition level
- `last_spike_time`: Time of last spike
- `is_active`: Whether neuron is currently active

### SynapseActivityData
- `synapse_id`: Unique synapse identifier
- `source_neuron_id`: Source neuron identifier
- `target_neuron_id`: Target neuron identifier
- `weight`: Current synaptic weight
- `eligibility_trace`: Eligibility trace for plasticity
- `last_update_time`: Time of last update
- `is_plastic`: Whether synapse is plastic

### TimeSeriesDataPoint
- `timestamp`: Time of data point
- `metric_name`: Name of the metric
- `value`: Metric value
- `metadata`: Additional metadata

### DebugInfo
- `component_name`: Name of the component
- `current_state`: Current state
- `warnings`: List of warnings
- `errors`: List of errors
- `last_update_time`: Time of last update
- `is_healthy`: Health status

### PerformanceMetrics
- `cpu_usage`: CPU usage percentage
- `memory_usage`: Memory usage percentage
- `total_memory`: Total system memory
- `used_memory`: Used memory
- `peak_memory`: Peak memory usage
- `network_bytes_sent`: Network bytes sent
- `network_bytes_received`: Network bytes received
- `disk_usage`: Disk usage percentage
- `timestamp`: Timestamp

### NeuralMetrics
- `total_neuron_count`: Total number of neurons
- `active_neuron_count`: Number of active neurons
- `spiking_neuron_count`: Number of spiking neurons
- `average_firing_rate`: Average firing rate
- `excitation_inhibition_ratio`: E/I ratio
- `mean_membrane_potential`: Mean membrane potential
- `mean_synaptic_weight`: Mean synaptic weight
- `connectivity_density`: Connectivity density
- `timestamp`: Timestamp

### SimulationMetrics
- `current_step`: Current simulation step
- `simulation_time`: Simulation time
- `wall_clock_time`: Wall clock time
- `steps_per_second`: Steps per second
- `efficiency`: Simulation efficiency
- `spike_count`: Total spike count
- `event_count`: Total event count
- `memory_pool_allocation_count`: Memory pool allocations
- `memory_pool_deallocation_count`: Memory pool deallocations
- `timestamp`: Timestamp

### ResourceUsage
- `memory_pool_used`: Used memory pool
- `memory_pool_allocated`: Allocated memory pool
- `event_queue_size`: Event queue size
- `pending_spikes`: Pending spike count
- `checkpoint_size`: Checkpoint size
- `cpu_temperature`: CPU temperature
- `memory_temperature`: Memory temperature
- `timestamp`: Timestamp

### MemoryAllocation
- `address`: Memory address
- `size`: Allocation size
- `allocation_time`: Allocation time
- `allocation_site`: Allocation site
- `thread_id`: Thread ID
- `is_leaked`: Whether allocation is leaked

### ErrorInfo
- `timestamp`: Error timestamp
- `error_type`: Error type
- `error_code`: Error code
- `error_message`: Error message
- `stack_trace`: Stack trace
- `context`: Error context
- `is_recovered`: Recovery status
- `is_critical`: Critical status

### DebugStepInfo
- `step`: Debug step
- `timestamp`: Timestamp
- `neuron_states`: Neuron states
- `synapse_states`: Synapse states
- `system_states`: System states
- `warnings`: List of warnings
- `errors`: List of errors

### ValidationResult
- `is_valid`: Validation result
- `errors`: List of errors
- `warnings`: List of warnings
- `corrected_values`: Corrected values
- `validation_time`: Validation time

## Error Handling

The tools provide comprehensive error handling:

### Error Types

- **RuntimeError**: Runtime errors in the NLM tools
- **ConfigurationError**: Configuration-related errors
- **ImportError**: Import-related errors
- **ValidationError**: Validation errors

### Error Recovery

The tools provide error recovery mechanisms:

1. **Automatic Recovery**: Automatic recovery for non-critical errors
2. **User Intervention**: User intervention for critical errors
3. **Fallback Mechanisms**: Fallback mechanisms for component failures

### Error Logging

```python
# Log an error
debug.log_error("MEMORY_LEAK", "Memory leak detected at address 0x12345678")

# Log a critical error
debug.log_error("SYSTEM_FAILURE", "System failure detected", is_critical=True)

# Get recent errors
recent_errors = debug.get_recent_errors()

# Get critical errors
critical_errors = debug.get_critical_errors()
```

## Performance Considerations

### Memory Usage

- The tools use efficient memory management
- Memory allocations are tracked and can be freed
- Memory leak detection helps identify memory issues

### CPU Usage

- The tools are designed to have minimal CPU overhead
- Monitoring hooks can be configured to run at different intervals
- Performance profiling can be disabled to reduce overhead

### I/O Operations

- File I/O operations are buffered
- Network I/O can be configured
- Export operations can be scheduled

## Threading and Concurrency

The tools support multiple threading models:

1. **Single-threaded**: All operations run in a single thread
2. **Multi-threaded**: Operations can run in multiple threads
3. **Event-driven**: Event-driven architecture for real-time updates

### Thread Safety

- Most operations are thread-safe
- Configuration updates require synchronization
- Data access requires proper locking

## Extensibility

The tools are designed to be extensible:

### Adding Custom Components

- Add custom visualization components
- Create custom debug components
- Extend existing components with new functionality

### Adding New Data Types

- Define new data structures
- Add serialization/deserialization support
- Integrate with existing visualization system

### Adding New Features

- Add new visualization modes
- Create new debugging capabilities
- Extend monitoring hooks

## Testing

The tools include comprehensive test suites:

### Unit Tests

- Test individual data structures
- Test visualization interface methods
- Test debug interface methods
- Test configuration validation

### Integration Tests

- Test integration with NLM core
- Test cross-component communication
- Test performance under load
- Test error handling

### System Tests

- Test complete workflows
- Test end-to-end functionality
- Test real-time operations
- Test export/import functionality

### Running Tests

```bash
cd python
python -m pytest test_nlm_tools.py -v
python -m pytest tests/ -v
```

## Troubleshooting

### Common Issues

1. **Import Errors**
   ```python
   # Check if NLM tools are available
   from nlm_visualization_tools import NLMVisualizationTools
   ```

2. **Initialization Errors**
   ```python
   # Check if initialization works
   tools = NLMVisualizationTools()
   if not tools.initialize():
       print("Initialization failed")
   ```

3. **File Not Found Errors**
   ```python
   # Check if files exist before using them
   import os
   if os.path.exists("config.json"):
       # Use the file
   else:
       print("Config file not found")
   ```

4. **Memory Issues**
   ```python
   # Monitor memory usage
   perf_metrics = debug.get_performance_metrics()
   print(f"Memory usage: {perf_metrics.memory_usage}%")
   ```

### Getting Help

1. **Documentation**: Read this documentation file
2. **Examples**: Run the demo scripts
3. **Tests**: Run the test suite
4. **GitHub**: Check the repository for issues and discussions

## License

This project is licensed under the MIT License.

## Acknowledgements

The NLM visualization and debugging tools are part of the NLM project, which is
an experimental artificial developmental brain project. This project would not
be possible without the contributions of the NLM team and the open-source community.

## Future Improvements

The tools are constantly being improved. Future development includes:

1. **Enhanced Visualization**: More advanced visualization capabilities
2. **Improved Performance**: Better performance and efficiency
3. **More Features**: Additional debugging and monitoring features
4. **Better Integration**: Better integration with NLM core
5. **Documentation**: More comprehensive documentation
6. **Testing**: More comprehensive test coverage

## Contact

For issues, questions, or suggestions, please visit the NLM GitHub repository
at https://github.com/nlm-project/nlm.

---

This documentation provides comprehensive information about the NLM visualization
and debugging tools. The tools are designed to provide powerful capabilities for
monitoring, visualizing, and debugging neural simulations while maintaining
flexibility and extensibility.