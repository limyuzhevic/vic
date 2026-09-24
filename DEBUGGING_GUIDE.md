# NLM Debugging Module Documentation

## Overview

This document provides comprehensive documentation for the debugging and logging capabilities implemented in the NLM project.

## New Modules

### 1. pynlm.logging

**Purpose:** Advanced logging system for NLM project

**Features:**
- Multi-level logging (DEBUG, INFO, WARNING, ERROR, CRITICAL)
- Context-aware logging (brain regions, simulation steps, agent IDs)
- Log file rotation and management
- Log level configuration and filtering
- Structured logging with JSON format

**Key Classes:**
- `NLMLogManager`: Manages logging configuration and rotation
- `JSONFormatter`: Formats logs as JSON with context
- `LogContext`: Dataclass for structured log context

**Usage:**
```python
from pynlm.logging import setup_logging, get_brain_logger, log_simulation_step

# Setup logging
logger = setup_logging("simulation.log", "DEBUG")

# Context-aware logging
log_simulation_step("simulation", step=100, message="Completed time step")
```

### 2. pynlm.debug

**Purpose:** Debug utilities for NLM project

**Features:**
- Brain state inspection utilities
- Simulation step debugging
- Connection validation tools
- Memory usage monitoring
- Performance profiling tools

**Key Classes:**
- `BrainStateInspector`: Inspects brain state and neural components
- `SimulationDebugger`: Debugger for simulation steps and execution
- `ConnectionValidator`: Validates neural connections
- `MemoryMonitor`: Monitors memory usage and detects leaks
- `PerformanceProfiler`: Profiles performance of NLM components

**Usage:**
```python
from pynlm.debug import (
    get_brain_state_inspector, get_simulation_debugger,
    get_connection_validator, get_memory_monitor
)

# Inspect brain states
inspector = get_brain_state_inspector()
snapshot = inspector.create_snapshot(step=10, neurons=neuron_data)

# Debug simulation
debugger = get_simulation_debugger()
debugger.add_breakpoint(50)
```

### 3. pynlm.state_inspection

**Purpose:** Tools for inspecting brain state and simulation state

**Features:**
- Brain state snapshot capabilities
- Connection inspection tools
- Memory content inspection
- Neural activity visualization tools

**Key Classes:**
- `BrainStateSnapshotter`: Takes snapshots of brain state
- `ConnectionInspector`: Inspects neural connections and connectivity
- `MemoryContentInspector`: Inspects memory content and states
- `NeuralActivityVisualizer`: Tools for visualizing neural activity

**Usage:**
```python
from pynlm.state_inspection import (
    get_brain_state_snapshotter, get_connection_inspector,
    get_memory_content_inspector
)

# Snapshot brain state
snapshotter = get_brain_state_snapshotter()
snapshot = snapshotter.inspect_state()

# Inspect connections
inspector = get_connection_inspector()
analysis = inspector.inspect_connectivity(connectivity_data)
```

### 4. pynlm.visualization_debug

**Purpose:** Debugging tools for visualization

**Features:**
- Visualization state inspection
- Rendering debugging tools
- Data validation for visualization
- Interactive debugging interfaces

**Key Classes:**
- `VisualizationDebugger`: Debugger for visualization components
- `RenderingDebugger`: Tools for debugging rendering process
- `DataValidator`: Validator for visualization data

**Usage:**
```python
from pynlm.visualization_debug import (
    get_visualization_debugger, get_rendering_debugger, get_data_validator
)

# Debug visualization
viz_debugger = get_visualization_debugger()
viz_debugger.start_debug_session("session1", {"type": "heatmap"})
validation = viz_debugger.validate_visualization_data("session1", data)
```

### 5. pynlm.error_reporting

**Purpose:** Enhanced error reporting and diagnostics

**Features:**
- Detailed error messages with context
- Stack trace generation
- Error categorization and handling
- Error recovery suggestions

**Key Classes:**
- `ErrorReporter`: Enhanced error reporting and diagnostics
- `ErrorContext`: Context information for errors
- `ErrorDetails`: Detailed error information
- `ErrorSeverity`: Enumeration of error severity levels
- `ErrorCategory`: Enumeration of error categories

**Usage:**
```python
from pynlm.error_reporting import (
    get_error_reporter, report_brain_state_error,
    ErrorSeverity, ErrorCategory
)

# Report errors with context
error_reporter = get_error_reporter()
error = report_brain_state_error(
    ValueError("Neurons exceeded firing rate threshold"),
    brain_region="hippocampus",
    simulation_step=50,
    recovery_suggestion="Reduce neural excitability"
)
```

### 6. pynlm.export_debug

**Purpose:** Tools for exporting debug information

**Features:**
- Debug report generation
- State export for analysis
- Configuration export
- Performance report generation

**Key Classes:**
- `DebugReportGenerator`: Generate comprehensive debug reports
- `StateExporter`: Export simulation state for analysis
- `ConfigurationExporter`: Export configuration and settings
- `PerformanceReportGenerator`: Generate performance reports

**Usage:**
```python
from pynlm.export_debug import (
    get_debug_report_generator, get_state_exporter,
    get_configuration_exporter
)

# Generate debug report
report_generator = get_debug_report_generator()
report = report_generator.generate_comprehensive_report("session1", data_sources)

# Export state
state_exporter = get_state_exporter()
exported_files = state_exporter.export_simulation_state(simulation_data)
```

## Usage Examples

### Example 1: Comprehensive Debugging Workflow

```python
from pynlm import Simulation
from pynlm.logging import setup_logging, get_simulation_logger
from pynlm.debug import get_simulation_debugger, get_performance_profiler
from pynlm.error_reporting import get_error_reporter

# Setup logging with JSON format
setup_logging("debug_simulation.log", "DEBUG")

# Get logger and debug tools
logger = get_simulation_logger()
error_reporter = get_error_reporter()
debugger = get_simulation_debugger()
profiler = get_performance_profiler()

# Add debugging features
debugger.add_breakpoint(100)
debugger.add_watch_var("error_rate")
profiler.start_profiling("simulation")

# Run simulation with error handling
try:
    simulation = Simulation()
    simulation.run(steps=200)
    
    # Record performance metrics
    performance = profiler.stop_profiling("simulation")
    
    # Generate debug report
    from pynlm.export_debug import get_debug_report_generator
    report_generator = get_debug_report_generator()
    report = report_generator.generate_comprehensive_report("simulation_1", {
        "simulation": simulation,
        "debugger": debugger,
        "profiler": profiler,
        "error_reporter": error_reporter
    })
    
except Exception as e:
    # Report error with context
    error = error_reporter.report_simulation_error(
        e,
        component="simulation",
        recovery_suggestion="Check simulation parameters and state"
    )
    
    # Generate error report
    from pynlm.export_debug import get_debug_report_generator
    report_generator = get_debug_report_generator()
    report = report_generator.generate_comprehensive_report("error_simulation_1", {
        "simulation": simulation,
        "debugger": debugger,
        "profiler": profiler,
        "error_reporter": error_reporter
    })
```

### Example 2: Brain State Analysis

```python
from pynlm.debug import get_brain_state_inspector
from pynlm.state_inspection import get_connection_inspector, get_memory_content_inspector

# Initialize inspectors
brain_inspector = get_brain_state_inspector()
connection_inspector = get_connection_inspector()
memory_inspector = get_memory_content_inspector()

# Capture brain state at different steps
snapshots = []
for step in [10, 50, 100, 150, 200]:
    # Get brain state (this would be provided by the simulation)
    brain_state = get_brain_state_at_step(step)
    
    # Create snapshot
    snapshot = brain_inspector.create_snapshot(
        step=step,
        neurons=extract_neurons(brain_state),
        synapses=extract_synapses(brain_state),
        memory_states=extract_memory_states(brain_state)
    )
    snapshots.append(snapshot)

# Compare snapshots
for i in range(len(snapshots) - 1):
    comparison = brain_inspector.compare_snapshots(i, i + 1)
    print(f"Comparison between steps {i} and {i + 1}:")
    print(f"  Time difference: {comparison['timestamp_difference']:.2f}s")
    print(f"  New regions: {comparison['differences'].get('brain_regions', {}).get('new_regions', set())}")

# Inspect connections
connectivity_data = get_connectivity_data()
connectivity_analysis = connection_inspector.inspect_connectivity(connectivity_data)
print(f"Total connections: {connectivity_analysis['total_connections']}")
print(f"Isolated neurons: {connectivity_analysis['isolated_neurons']}")

# Inspect memory
memory_data = get_memory_data()
memory_analysis = memory_inspector.inspect_memory(memory_data)
print(f"Total memories: {memory_analysis['total_memories']}")
print(f"Memory types: {list(memory_analysis['memory_types'].keys())}")
```

### Example 3: Performance Monitoring

```python
from pynlm.debug import get_performance_profiler
from pynlm.logging import setup_logging
import time

# Setup logging
setup_logging("performance_monitor.log", "INFO")

# Create performance profiler
profiler = get_performance_profiler()

# Start profiling multiple components
profiler.start_profiling("simulation", interval=0.1)
profiler.start_profiling("visualization", interval=0.5)
profiler.start_profiling("memory", interval=1.0)

# Run simulation
start_time = time.time()
run_simulation()
end_time = time.time()

# Stop profiling and get results
sim_results = profiler.stop_profiling("simulation")
viz_results = profiler.stop_profiling("visualization")
mem_results = profiler.stop_profiling("memory")

# Analyze bottlenecks
bottlenecks = profiler.analyze_bottlenecks()
print("Performance bottlenecks:")
for bottleneck in bottlenecks.get("bottlenecks", []):
    print(f"  - {bottleneck['component']}: {bottleneck['issue']} (severity: {bottleneck['severity']})")

# Generate performance report
from pynlm.export_debug import get_performance_report_generator
report_generator = get_performance_report_generator()
performance_report = report_generator.generate_performance_report({
    "simulation": sim_results,
    "visualization": viz_results,
    "memory": mem_results,
    "total_time": end_time - start_time
})
```

## Integration with Existing NLM Components

The debugging modules integrate seamlessly with existing NLM components:

### Integration Points:

1. **Simulation Integration:**
   - `Simulation` class can automatically enable debugging
   - `EpisodeController` can capture state at each episode
   - `WorldManager` can log world state changes

2. **Brain System Integration:**
   - `Brain` class can expose state for inspection
   - `Neuron` classes can log firing activity
   - `Synapse` classes can log connection changes

3. **Memory System Integration:**
   - `Memory` classes can expose state for inspection
   - `WorkingMemory` can log access patterns
   - `EpisodicMemory` can log encoding/retrieval events

4. **Visualization Integration:**
   - `VisualSystem` can expose state for debugging
   - `ActivityPlotter` can log rendering information
   - `SimulationVisualizer` can expose visualization state

## Configuration

### Environment Variables:

- `NLM_LOG_LEVEL`: Default logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
- `NLM_LOG_FILE`: Default log file path
- `NLM_LOG_FORMAT`: Log format (standard, json)
- `NLM_DEBUG_MODE`: Enable debug mode (true, false)

### Configuration Files:

Configuration can be set via YAML/JSON files in the `configs/` directory:

```yaml
logging:
  level: DEBUG
  file: "nlm.log"
  format: "json"
  rotation:
    max_size_mb: 100
    backup_count: 5

debugging:
  brain_state_inspection: true
  simulation_debugging: true
  connection_validation: true
  memory_monitoring: true
  performance_profiling: true

visualization:
  debug_mode: true
  capture_frames: true
  validation_enabled: true
```

## Command Line Interface

The debugging modules provide command-line interfaces:

### Basic Commands:

```bash
# Generate debug report
nlm-debug-report --session-id session1 --output-dir ./reports

# Export simulation state
nlm-state-export --input simulation.json --output ./exports --format json

# Generate performance report
nlm-performance-report --input performance.json --output ./reports

# Analyze log files
nlm-log-analyzer --input nlm.log --output analysis.json
```

### Advanced Options:

```bash
# Comprehensive debug report with all data sources
nlm-debug-report --session-id session1 --type comprehensive --output-dir ./reports

# State export with snapshots
nlm-state-export --input simulation.json --output ./exports --format json --include-snapshots

# Performance report with analysis
nlm-performance-report --input performance.json --output ./reports --type comprehensive

# Log analysis with filtering
nlm-log-analyzer --input nlm.log --output analysis.json --level ERROR --time-range 24h
```

## Dependencies

### Required Dependencies:

- Python 3.8+
- logging (standard library)
- json (standard library)
- datetime (standard library)
- threading (standard library)
- numpy (for numerical operations)
- psutil (for system monitoring)

### Optional Dependencies:

- matplotlib (for visualization debugging)
- pandas (for data analysis)
- yaml (for YAML configuration)

## Installation

### From Source:

```bash
pip install -e .
```

### With All Debugging Capabilities:

```bash
pip install -e .[debug]
```

## Testing

The debugging modules include comprehensive tests:

```bash
# Run debugging module tests
pytest pynlm/logging/
pytest pynlm/debug/
pytest pynlm/state_inspection/
pytest pynlm/visualization_debug/
pytest pynlm/error_reporting/
pytest pynlm/export_debug/
```

## Performance Considerations

The debugging modules are designed to be minimally invasive:

1. **Logging Overhead:** JSON formatting adds ~10-20% overhead
2. **Memory Usage:** Debug data structures can be memory-intensive
3. **I/O Operations:** Frequent file writes can impact performance
4. **Threading:** Some operations use background threads

**Recommendations:**
- Disable debug features in production
- Use appropriate log levels
- Configure log rotation to prevent disk space issues
- Monitor memory usage in long-running simulations

## Security Considerations

1. **Log Files:** May contain sensitive data (neuron states, agent IDs)
2. **Configuration Files:** Should be protected with appropriate permissions
3. **Network Access:** Debug interfaces may expose network services
4. **Data Sanitization:** Ensure sensitive data is not logged

**Best Practices:**
- Use secure file permissions
- Sanitize sensitive data before logging
- Disable debug interfaces in production
- Regular security audits

## Future Enhancements

The debugging capabilities can be extended with:

1. **Real-time Visualization:** Live monitoring of simulation state
2. **Machine Learning:** Anomaly detection in simulation data
3. **Distributed Debugging:** Debugging across multiple machines
4. **API Integration:** REST API for remote debugging
5. **Custom Debugging:** Plugin system for custom debugging tools

## References

1. Python Logging Tutorial: https://docs.python.org/3/library/logging.html
2. JSON Logging: https://betterprogramming.pub/structured-logging-with-python-b64381d69d97
3. Performance Profiling: https://realpython.com/python-profiling/
4. Memory Leak Detection: https://www.geeksforgeeks.org/python-memory-profiler/
5. Visualization Debugging: https://matplotlib.org/stable/gallery/