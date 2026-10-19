# NLM Advanced Python Utilities

This module provides comprehensive expert-level Python utilities for NLM (Neural Learning Machine) neural simulations. It extends the basic NLM Python API with advanced features for complex research and production workloads.

## Overview

The NLM Advanced Utilities module is organized into logical submodules, each addressing specific needs of expert users:

- **Advanced Simulation Controls**: Custom simulation callbacks, step-by-step debugging, pause/resume functionality, multi-agent coordination
- **Enhanced Debugging and Visualization**: Real-time neural activity visualization, connectivity graph exploration, activity profiling, performance benchmarking
- **Performance Monitoring**: Resource usage monitoring, simulation progress tracking, custom metrics collection, auto-tuning suggestions
- **Batch Processing Utilities**: Simulation parameter sweeps, cross-validation tools, result aggregation, parallel execution
- **Data Export/Import Tools**: Multiple export formats, checkpoint management, recording and playback, result analysis
- **Advanced Agent Capabilities**: Custom agent architectures, social learning algorithms, hierarchical control systems
- **Robustness and Recovery**: Automatic checkpointing, error recovery, state inspection, safe interruption handling
- **Advanced Configuration Tools**: Configuration templates, parameter search, hyperparameter tuning, validation and auditing

## Module Structure

```
python/nlm_advanced/
├── __init__.py
├── sim/                    # Advanced Simulation Controls
│   └── __init__.py
├── debug/                  # Enhanced Debugging and Visualization
│   └── __init__.py
├── performance/            # Performance Monitoring
│   └── __init__.py
├── batch/                  # Batch Processing Utilities
│   └── __init__.py
├── data/                   # Data Export/Import Tools
│   └── __init__.py
├── agent/                  # Advanced Agent Capabilities
│   └── __init__.py
├── recovery/               # Robustness and Recovery
│   └── __init__.py
├── config/                 # Advanced Configuration Tools
│   └── __init__.py
```

## Installation and Setup

### Prerequisites

```bash
pip install numpy matplotlib networkx pandas h5py psutil memory_profiler
```

### Quick Start

```python
from python.nlm_advanced import *

# Create a brain with advanced simulation controls
brain = createBrain(config)
simulation_controller = SimulationController(brain)

# Start recording with visualization
neural_visualizer = NeuralVisualizer(brain)
neural_visualizer.setup_plots()

# Run simulation with advanced controls
for step in range(1000):
    time = step * 0.01
    dt = simulation_controller.step(step, time)
    brain.step(step, time)
    neural_visualizer.update_plots(step, time)

# Export results
data_exporter = DataExporter("./exports")
results_file = data_exporter.export_results({
    'final_metrics': {
        'final_firing_rate': brain.getAverageFiringRate(),
        'final_e_i_ratio': brain.getExcitationInhibitionRatio(),
        'total_spikes': brain.getTotalSpikeCount()
    },
    'step_results': [],
    'config': config
})
```

## Advanced Simulation Controls

### SimulationController
Provides comprehensive control over simulation execution with custom callbacks, pause/resume functionality, and advanced time-stepping strategies.

### StepByStepDebugger
Enables granular control over simulation steps with breakpoints, state recording, and detailed logging.

### PauseResumeManager
Implements safe pause and resume functionality for interactive analysis and debugging.

### CustomTimeStepper
Allows custom time-stepping strategies including adaptive time-stepping based on neural activity.

### MultiAgentCoordinator
Coordinates multiple agents with social interactions, communication channels, and hierarchical control.

## Enhanced Debugging and Visualization

### NeuralVisualizer
Real-time visualization of neural activity with firing rate plots, E/I balance charts, and activity heatmaps.

### ConnectivityExplorer
Analyzes and visualizes neural connectivity graphs with network metrics, centrality measures, and community detection.

### ActivityProfiler
Profiles neural activity over time, generating heatmaps, detecting bursts and oscillations, and calculating stability metrics.

### PerformanceBenchmarker
Comprehensive benchmarking tools for measuring simulation performance, identifying bottlenecks, and comparing configurations.

## Performance Monitoring

### ResourceMonitor
Tracks CPU, memory, disk I/O, and network usage during simulations.

### SimulationProgressTracker
Monitors simulation progress with estimated completion times and progress reporting.

### MetricsCollector
Collects custom metrics from simulations with real-time monitoring and analysis.

### AutoTuner
Suggests performance optimizations based on monitoring data, including parameter tuning recommendations.

### LoadBalancer
Distributes simulation load across multiple workers for parallel execution.

## Batch Processing Utilities

### SimulationSweeper
Performs parameter sweeps across simulation parameters with comprehensive analysis of results.

### CrossValidator
Implements cross-validation tools for simulation validation and robustness testing.

### ResultAggregator
Aggregates results from multiple simulations with statistical analysis and visualization support.

### ParallelExecutor
Executes simulations in parallel across multiple CPU cores or workers.

### ExperimentGenerator
Generates automated experiment configurations based on templates and parameter ranges.

## Data Export/Import Tools

### DataExporter
Exports simulation results in multiple formats: HDF5, JSON, CSV, and Pickle.

### DataImporter
Imports results from various file formats with automatic format detection.

### CheckpointManager
Manages compressed checkpoints for simulation state preservation and recovery.

### RecordingPlayer
Records and plays back simulation sessions with precise timing and state restoration.

### ResultAnalyzer
Analyzes simulation results with statistical tests, pattern detection, and anomaly identification.

### ReportGenerator
Generates comprehensive reports with automated analysis and visualization.

## Advanced Agent Capabilities

### CustomArchitect
Creates custom agent architectures with tailored neural network designs and connectivity patterns.

### SocialLearner
Implements social learning algorithms with knowledge sharing, bonding, and collaborative learning.

### HierarchicalController
Implements hierarchical control systems for multi-level agent coordination and decision-making.

### AdaptiveBehaviorModifier
Adapts agent behavior based on environmental feedback with machine learning algorithms.

### CollaborativeTrainer
Creates collaborative training sessions with shared objectives and knowledge transfer.

## Robustness and Recovery

### AutoCheckpoint
Implements automatic checkpointing with configurable intervals and cleanup strategies.

### ErrorRecovery
Provides error recovery mechanisms with recovery points and error handling strategies.

### StateInspector
Inspects and analyzes simulation state with comprehensive monitoring and anomaly detection.

### SafeInterruptHandler
Handles interrupt signals safely with state preservation and recovery options.

### MemoryManager
Monitors and optimizes memory usage with garbage collection and memory analysis.

## Advanced Configuration Tools

### ConfigTemplate
Manages configuration templates for simulation setups with inheritance and customization.

### ParameterSearcher
Searches parameter spaces using grid search, random search, Bayesian optimization, and genetic algorithms.

### HyperparameterTuner
Automated hyperparameter tuning for optimal simulation performance.

### ConfigValidator
Validates configurations against rules and provides detailed error reporting.

### ConfigAuditor
Audits configurations for compliance and generates audit reports.

## Integration and Usage

### Integration with Basic NLM API

The advanced utilities integrate seamlessly with the basic NLM Python API:

```python
import pynlm as nlm
from python.nlm_advanced import *

# Create basic brain
brain = nlm.createBrain(config)

# Enhance with advanced features
brain = AdvancedBrainWrapper(brain)  # Optional wrapper for additional functionality

# Use advanced simulation controls
simulation_controller = SimulationController(brain)

# Start performance monitoring
resource_monitor = ResourceMonitor()
resource_monitor.start_monitoring()

# Run enhanced simulation
for step in range(1000):
    time = step * 0.01
    
    # Advanced step control
    simulation_controller.step(step, time)
    
    # Perform simulation step
    brain.step(step, time)
    
    # Record performance metrics
    resource_monitor.collect_sample(step, time)
    
    # Check for errors
    if simulation_controller.should_pause():
        simulation_controller.pause()
        # Perform analysis
        analysis = StateInspector(brain).analyze_state_history()
        simulation_controller.resume()

# Export results with analysis
data_exporter = DataExporter("./exports")
results = {
    'final_metrics': {
        'final_firing_rate': brain.getAverageFiringRate(),
        'final_e_i_ratio': brain.getExcitationInhibitionRatio(),
        'total_spikes': brain.getTotalSpikeCount()
    },
    'performance_metrics': resource_monitor.get_resource_stats(),
    'step_results': [],
    'config': config
}

results_file = data_exporter.export_results(results)
```

### Example: Complete Advanced Simulation

```python
import time
import numpy as np
from python.nlm_advanced import *

# Configuration
config = {
    'network_size': 10000,
    'time_step_size': 0.01,
    'target_firing_rate': 5.0,
    'visualization_enabled': True
}

# Create brain
brain = createBrain(config)

# Initialize advanced tools
simulation_controller = SimulationController(brain)
neural_visualizer = NeuralVisualizer(brain)
performance_monitor = PerformanceMonitor(brain)
result_aggregator = ResultAggregator()

# Setup visualization
if config.get('visualization_enabled', False):
    neural_visualizer.setup_plots()

# Start monitoring
performance_monitor.start_monitoring()

# Configure batch processing for parameter sweep
batch_config = ExperimentConfig(
    name='firing_rate_sweep',
    parameter_ranges={'target_firing_rate': [1.0, 2.0, 5.0, 10.0, 20.0]},
    fixed_parameters=config,
    repetitions=3
)

# Run batch experiment
batch_results = []
for param_set in batch_config.generate_configurations():
    print(f"Running experiment with parameters: {param_set}")
    
    # Create brain with new parameters
    brain = createBrain(param_set)
    
    # Run simulation
    for step in range(1000):
        time_val = step * param_set['time_step_size']
        
        # Advanced step control with callbacks
        simulation_controller.step(step, time_val)
        
        # Perform simulation
        brain.step(step, time_val)
        
        # Update visualization
        if config.get('visualization_enabled', False):
            neural_visualizer.update_plots(step, time_val)
        
        # Record results
        step_result = {
            'step': step,
            'time': time_val,
            'firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'active_neurons': brain.getActiveNeuronCount()
        }
        
        result_aggregator.add_result(step_result, {'parameters': param_set})
    
    # Store batch result
    batch_results.append({
        'parameters': param_set,
        'final_metrics': {
            'final_firing_rate': brain.getAverageFiringRate(),
            'final_e_i_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount()
        }
    })

# Analyze batch results
analysis = result_aggregator.analyze_results()
best_config = analysis['optimal_configuration']

print(f"Optimal configuration: {best_config}")

# Export comprehensive report
report_generator = ReportGenerator()
report_file = report_generator.generate_report({
    'batch_results': batch_results,
    'analysis': analysis,
    'config': config
})

print(f"Report generated: {report_file}")
```

## Performance Considerations

### Memory Management
The advanced utilities include comprehensive memory management with:
- Automatic garbage collection
- Memory monitoring and optimization
- Efficient data structures and serialization

### Parallel Processing
For large-scale simulations, use parallel processing:
- `ParallelExecutor` for distributing simulation tasks
- Batch processing utilities for parameter sweeps
- Parallel recording for multiple simulations

### Data Storage
- HDF5 format for large numerical datasets
- Compression for storage efficiency
- Chunked storage for memory management

## Compatibility

The NLM Advanced Utilities module is compatible with:
- Python 3.8+
- NLM Python bindings (pynlm)
- Common scientific Python libraries (numpy, matplotlib, pandas)
- Various operating systems (Linux, macOS, Windows)

## License

MIT License

## Dependencies

Required dependencies:
- numpy
- matplotlib
- pandas
- networkx
- h5py
- psutil
- memory_profiler
- pyyaml

Optional dependencies for advanced features:
- scikit-learn (for advanced analysis)
- seaborn (for enhanced plotting)
- plotly (for interactive visualizations)

## Support and Feedback

For support, feature requests, or feedback:
- Check the documentation for troubleshooting
- Report issues on GitHub
- Join the community for discussions and examples