# NLM Advanced Utilities - Summary of Created Utilities

## Overview

This document provides a comprehensive summary of all utilities created for advanced users of the NLM (Neural Learning Machine) neural simulation framework. The utilities are organized into five main categories as requested, each with specific features designed to help advanced users work with the complex neural computation system.

## 1. Performance Monitoring Utilities

### Core Components:
- **PerformanceMonitor Class** (`nlm_utilities/performance/performance_monitor.py`)
- **PerformanceMetrics Dataclass** - Container for performance data

### Key Features:
✅ **Real-time Performance Monitoring**
   - Real-time metrics collection with configurable update intervals (default: 0.1s)
   - System resource monitoring (CPU%, memory%, active threads, VRAM)
   - Neural simulation-specific metrics (neuron count, synapse count, spike counts, firing rates)

✅ **CPU Usage Analysis**
   - Process CPU percentage monitoring
   - Multithreading awareness (thread count tracking)
   - Bottleneck detection with configurable thresholds (default: 80% for CPU)

✅ **Memory Usage Tracking**
   - Process memory percentage monitoring
   - VRAM usage tracking (when GPU acceleration available)
   - Memory pool efficiency metrics

✅ **Bottleneck Identification**
   - Automatic detection based on configurable thresholds
   - Multi-dimensional bottleneck analysis (CPU, Memory, VRAM, Time)
   - Bottleneck type classification and reporting

✅ **Performance Metrics Storage**
   - Historical metrics storage (up to 1000 entries by default)
   - Configurable retention policies
   - Data export capabilities (JSON, CSV, Parquet)

✅ **Performance Report Generation**
   - Comprehensive performance report with statistics
   - Bottleneck analysis and recommendations
   - Time-series visualization capabilities

### Usage Example:
```python
from nlm_utilities.performance.performance_monitor import PerformanceMonitor

# Initialize with brain object
perf_monitor = PerformanceMonitor(brain=your_brain)
perf_monitor.start_monitoring()

# In simulation loop
perf_monitor.capture_metrics(step=current_step)

# Get statistics
stats = perf_monitor.get_statistics()
print(f"CPU Usage: {stats['avg_cpu_usage']:.1f}%")

# Generate report
report = perf_monitor.generate_report()
perf_monitor.stop_monitoring()
```

## 2. Advanced Configuration Utilities

### Core Components:
- **ConfigurationUtils Class** (`nlm_utilities/configuration/config_utils.py`)
- **ConfigDiff Dataclass** - Represents configuration differences

### Key Features:
✅ **Configuration Comparison and Diffing**
   - Recursive dictionary comparison with detailed change tracking
   - Support for added/removed/modified keys
   - Key path tracking for nested configurations
   - Hash-based integrity verification (SHA256)

✅ **Configuration Validation and Schema Checking**
   - JSON Schema Draft-07 validation support
   - Custom schema management with named schemas
   - Validation error and warning reporting
   - Required section validation (brain, network, simulation, plasticity)

✅ **Configuration Backup and Restore**
   - Incremental backup creation with versioning
   - Timestamp-based backup naming
   - Backup integrity verification with hash checking
   - Directory-based backup management

✅ **Configuration Migration Tools**
   - Version-based migration rules
   - Automatic schema migration between versions
   - Custom migration rule definitions
   - Backward and forward compatibility support

✅ **Additional Features**
   - Multi-format support (JSON, YAML)
   - Schema generation from sample configurations
   - Configuration summary and statistics
   - Copy/deep copy operations for safe editing

### Usage Example:
```python
from nlm_utilities.configuration.config_utils import ConfigurationUtils

config_utils = ConfigurationUtils()

# Load and save configurations
config = config_utils.load_config("config.json")
config_utils.save_config(config, "new_config.json")

# Compare configurations
diff = config_utils.compare_configs(config1, config2)
print(f"Identical: {diff['identical']}")
print(f"Difference hash: {diff['hash1']} vs {diff['hash2']}")

# Validate configuration
validation = config_utils.validate_config(config)
if not validation['valid']:
    print(f"Errors: {validation['errors']}")

# Create backup
backup_path = config_utils.create_backup(config, "experiment_1")

# Restore from backup
restored_config = config_utils.restore_backup(backup_path)
```

## 3. Debugging and Profiling Utilities

### Core Components:
- **Debugger Class** (`nlm_utilities/debugging/debugger.py`)
- **DebugData Dataclass** - Container for debug information
- **SpikeEvent Dataclass** - Represents neural spike events

### Key Features:
✅ **Neural Activity Visualization**
   - Spike raster plots for neural activity patterns
   - Membrane potential time series for selected neurons
   - Region activity heatmaps
   - Synaptic weight distribution histograms

✅ **Connection Graph Debugging**
   - NetworkX-based graph representation
   - Interactive graph visualization
   - Node coloring by neuron type (Excitatory, Inhibitory, Sensory, Motor, etc.)
   - Edge weighting by synaptic strength
   - Highlight capabilities for specific neurons or regions

✅ **Spike Tracing and Analysis**
   - Individual neuron spike tracing with time windows
   - Inter-spike interval calculation
   - Firing rate analysis
   - Top spiking neuron identification
   - Pattern recognition in spike trains

✅ **Memory State Inspection**
   - Working memory state monitoring
   - Episodic memory inspection
   - Associative memory analysis
   - Plasticity state tracking
   - Neuromodulation level visualization

✅ **Additional Debugging Features**
   - Real-time debug data recording with configurable intervals
   - Motor command tracking and analysis
   - Neuromodulation level monitoring
   - Export/import capabilities for debug data
   - Comprehensive debug report generation

### Usage Example:
```python
from nlm_utilities.debugging.debugger import Debugger

debugger = Debugger(brain=your_brain)
debugger.start_recording()

# In simulation loop
for step in range(total_steps):
    brain.step(step)
    debugger.capture_debug_data(step)

debugger.stop_recording()

# Analyze spike patterns
spikes = debugger.trace_spikes(neuron_id=123)
analysis = debugger.analyze_spike_patterns()

# Visualize
debugger.visualize_activity("activity.png", neurons_subset=[1,2,3])
debugger.visualize_connection_graph("connectivity.png")
```

## 4. Export/Import Utilities

### Core Components:
- **ExportUtils Class** (`nlm_utilities/export/export_utils.py`)
- **SimulationRecording Class** - Manages simulation recording
- **BrainState Dataclass** - Complete brain state representation

### Key Features:
✅ **Neural Network Export for Analysis**
   - Complete neural topology export
   - Neuron and synapse properties export
   - Connectivity matrix export
   - Region and population information
   - Multiple export formats (JSON, HDF5, CSV)

✅ **Brain State Export/Import**
   - Complete brain state preservation
   - Neuron state serialization
   - Synapse state export/import
   - Plasticity and neuromodulation states
   - Memory system states

✅ **Simulation Recording and Playback**
   - Frame-by-frame simulation capture
   - Recording with configurable intervals
   - Multiple export formats for recordings
   - Playback capabilities with state restoration
   - Recording metadata and versioning

✅ **Data Analysis Export Formats**
   - JSON: Human-readable, easy parsing
   - HDF5: Efficient for large datasets
   - CSV: Spreadsheet-compatible, simple analysis
   - Format optimization for different analysis tools

✅ **Export Management**
   - Automatic file naming with timestamps
   - Output directory management
   - Format conversion utilities
   - Data filtering and transformation options

### Usage Example:
```python
from nlm_utilities.export.export_utils import ExportUtils

export_utils = ExportUtils(brain=your_brain)

# Start recording
export_utils.start_recording("simulation_001")

# In simulation loop
for step in range(total_steps):
    export_utils.capture_frame(step, simulation_time)

export_utils.stop_recording()

# Export in multiple formats
export_utils.export_recording("results.json")
export_utils.export_recording("results.h5")
export_utils.export_recording("results.csv")

# Export network structure
network_path = export_utils.export_network("network.json")

# Export brain state
brain_state_path = export_utils.export_brain_state("brain_state.json")
```

## 5. Advanced Analysis Tools

### Core Components:
- **AnalysisTools Class** (`nlm_utilities/analysis/analysis_tools.py`)
- **NetworkTopologyAnalyzer Class**
- **PlasticityAnalyzer Class**
- **LearningCurveAnalyzer Class**
- **BehavioralPatternAnalyzer Class**

### Key Features:
✅ **Network Topology Analysis**
   - Connectivity matrix properties analysis
   - Graph theory metrics (density, clustering, diameter)
   - Region-specific topology analysis
   - Degree distribution analysis
   - Neuron type distribution analysis
   - Connection graph visualization

✅ **Plasticity Rule Effectiveness Analysis**
   - STDP (Spike-Timing-Dependent Plasticity) dynamics
   - Hebbian plasticity effectiveness
   - Structural plasticity analysis
   - Weight change patterns
   - Plasticity rule effectiveness metrics

✅ **Learning Curve Analysis**
   - Performance trend analysis over time
   - Learning curve fitting (exponential decay models)
   - Convergence point detection
   - Learning stage identification (exploration, exploitation, consolidation)
   - Statistical trend analysis

✅ **Behavioral Pattern Analysis**
   - Action sequence analysis
   - Behavioral pattern recognition
   - Transition matrix construction
   - Regression detection and stability analysis
   - Sequence pattern identification
   - Temporal pattern analysis

✅ **Comprehensive Analysis Suite**
   - Complete analysis workflow integration
   - Error handling and reporting
   - Results export in multiple formats
   - Interactive visualization integration
   - Customizable analysis parameters

### Usage Example:
```python
from nlm_utilities.analysis.analysis_tools import AnalysisTools

analysis_tools = AnalysisTools(brain=your_brain)

# Run complete analysis on recording
results = analysis_tools.run_complete_analysis("simulation.json")

# Export analysis results
analysis_tools.export_analysis_results(results, "analysis_results.json")

# Individual analysis modules
topology = analysis_tools.topology_analyzer.analyze_connectivity_matrix()
plasticity = analysis_tools.plasticity_analyzer.analyze_plasticity_effectiveness()
learning = analysis_tools.learning_curve_analyzer.analyze_performance_learning("simulation.json")
behavioral = analysis_tools.behavioral_pattern_analyzer.analyze_action_patterns("simulation.json")
```

## Installation and Setup

### Prerequisites:
- Python 3.8 or higher
- CMake and build tools (for NLM framework compilation)

### Installation Steps:
1. Clone the NLM repository
2. Build the NLM framework (if not already built)
3. Install Python dependencies:
   ```bash
   pip install -r nlm_utilities/requirements.txt
   ```
4. Install the utilities package:
   ```bash
   pip install -e ./nlm_utilities
   ```

### Development Installation:
For development and testing:
```bash
pip install -e ./nlm_utilities[dev]
```

## Key Benefits for Advanced Users

### 1. Enhanced Debugging Capabilities
- Real-time monitoring of neural simulation performance
- Comprehensive visualization of neural activity and connectivity
- Detailed spike tracing and pattern analysis
- Memory and plasticity state inspection

### 2. Improved Configuration Management
- Robust configuration comparison and diffing
- Schema validation and migration tools
- Automated backup and restore capabilities
- Version compatibility management

### 3. Advanced Analysis and Insights
- Sophisticated network topology analysis
- Plasticity rule effectiveness measurement
- Learning curve analysis and prediction
- Behavioral pattern recognition and analysis

### 4. Flexible Data Export/Import
- Multiple format support for different analysis tools
- Complete brain state preservation
- Simulation recording and playback
- Data exchange capabilities

### 5. Integration with Existing NLM Framework
- Seamless integration with the C++ NLM framework
- Python binding compatibility
- Minimal performance overhead
- Extensible architecture for custom utilities

## Usage Examples

### Complete Workflow Example:
```python
from nlm_utilities import PerformanceMonitor, ConfigurationUtils, Debugger, ExportUtils, AnalysisTools

# Initialize all utilities with brain object
brain = create_brain_with_config("config.json")

perf_monitor = PerformanceMonitor(brain)
config_utils = ConfigurationUtils()
debugger = Debugger(brain)
export_utils = ExportUtils(brain)
analysis_tools = AnalysisTools(brain)

# Start monitoring and debugging
perf_monitor.start_monitoring()
debugger.start_recording()
export_utils.start_recording("experiment_001")

# Run simulation with full monitoring
for step in range(total_steps):
    # Run brain simulation step
    brain.step(step, simulation_time)
    
    # Capture data with all utilities
    perf_monitor.capture_metrics(step)
    debugger.capture_debug_data(step)
    export_utils.capture_frame(step, simulation_time)

# Stop all monitoring
perf_monitor.stop_monitoring()
debugger.stop_recording()
export_utils.stop_recording()

# Analyze results
results = analysis_tools.run_complete_analysis("simulation.json")
network_analysis = results['network_topology']
plasticity_analysis = results['plasticity_analysis']
learning_analysis = results['learning_curve']
behavioral_analysis = results['behavioral_patterns']

# Export comprehensive results
analysis_tools.export_analysis_results(results, "final_results.json")

# Generate reports and visualizations
perf_report = perf_monitor.generate_report()
debug_report = debugger.generate_debug_report()

# Save configuration and results
config_utils.save_config(brain.getConfig(), "final_config.json")
```

## Conclusion

The NLM Advanced Utilities package provides a comprehensive suite of tools for advanced users working with the NLM neural simulation framework. These utilities enhance the debugging, analysis, configuration management, and data export capabilities of the framework, making it easier to develop, test, and analyze complex neural simulations.

The utilities are designed to be:
- **Modular**: Each utility can be used independently or in combination
- **Extensible**: Easy to add new analysis tools and utilities
- **Compatible**: Seamlessly integrates with the existing NLM framework
- **User-friendly**: Comprehensive documentation and examples
- **Robust**: Comprehensive error handling and validation

These utilities significantly enhance the capabilities available to advanced NLM users, providing powerful tools for simulation analysis, debugging, and optimization.
