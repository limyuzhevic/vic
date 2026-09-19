# NLM CLI User Guide

## Overview

The Neural Learning Machine (NLM) provides a comprehensive command-line interface for neural network simulations, experiments, and analysis. This guide documents all available commands and their usage.

## Installation

NLM is installed as part of the project. The main executable is `nlm`.

## Basic Usage

```bash
# Show general help
nlm help

# Run a simulation
nlm run --config my_config.cfg --steps 10000 --experiment phase6

# Show configuration
nlm config show

# Set configuration value
nlm config set neuron_count 500

# Run an experiment
nlm experiment run --experiment phase3 --steps 10000 --output results.json

# Monitor system performance
nlm monitor --interval 0.1 --duration 3600

# Run performance profiling
nlm profile memory --duration 30000

# Advanced debugging
nlm debug neural --output neural_activity.json

# Parallel execution
nlm parallel execute --num-processes 4 --experiment phase4

# Custom analysis
nlm analysis statistical --input metrics.json --analysis comprehensive
```

## Command Categories

### 1. Core Commands (`nlm <command>`)

- `run` - Run a simulation or experiment
- `config` - Configuration management
- `experiment` - Experiment management
- `monitor` - Real-time monitoring
- `profile` - Performance profiling
- `debug` - Advanced debugging
- `parallel` - Parallel execution
- `analysis` - Custom analysis
- `help` - Show help information
- `version` - Show version information

### 2. Configuration Commands (`nlm config <subcommand>`)

- `show` - Display current configuration
- `set <key> <value>` - Set configuration value
- `load <file>` - Load configuration from file
- `save <file>` - Save configuration to file
- `validate` - Validate configuration against schema
- `export <format>` - Export configuration in specified format
- `import` - Import configuration from stdin
- `list` - List all configuration keys and sources

### 3. Experiment Commands (`nlm experiment <subcommand>`)

- `run` - Run an experiment
- `list` - List registered experiments
- `info <name>` - Show experiment information

### 4. Profile Commands (`nlm profile <subcommand>`)

- `memory` - Memory profiling
- `cpu` - CPU and execution time profiling
- `network` - Network/data access profiling
- `gpu` - GPU acceleration support
- `leak` - Memory leak detection
- `timeline` - Timeline profiling
- `start` - Start profile capture
- `stop` - Stop profile capture
- `report` - Generate profile report
- `export` - Export profile data

### 5. Debug Commands (`nlm debug <subcommand>`)

- `neural` - Neural activity debugging
- `spikes` - Spike train debugging
- `synapses` - Synapse weight evolution tracking
- `memory` - Memory trace analysis
- `trace` - Debug trace collection
- `visualize` - Neural activity visualization
- `errors` - Error and exception debugging
- `breakpoints` - Breakpoint debugging
- `watch` - Watch variable debugging
- `inspect` - Inspect brain state
- `log` - Debug log management

### 6. Parallel Commands (`nlm parallel <subcommand>`)

- `execute` - Run parallel experiments
- `gpu` - Run GPU-accelerated experiments
- `distributed` - Run distributed experiments
- `resources` - Show resource information
- `monitor` - Monitor system resources
- `aggregate` - Aggregate experiment results
- `schedule` - Create workload schedule
- `workload` - Create workload specification
- `profile` - Profile parallel execution

### 7. Analysis Commands (`nlm analysis <subcommand>`)

- `metrics` - Custom metric collection and management
- `statistical` - Statistical data analysis
- `machine-learning` - Machine learning model training
- `features` - Feature engineering and selection
- `optimization` - Experiment optimization
- `forecast` - Time series forecasting
- `anomaly` - Anomaly detection
- `correlation` - Correlation analysis
- `clustering` - Cluster analysis

## Configuration File Format

Configuration files are JSON format by default, but NLM supports multiple formats:

### JSON Configuration
```json
{
  "random_seed": 42,
  "neuron_count": 500,
  "region_count": 1,
  "learning_rate": 0.01,
  "enable_stdp": true,
  "simulation_timestep": 0.001,
  "max_steps": 10000,
  "checkpoint_interval": 1000
}
```

### YAML Configuration
```yaml
random_seed: 42
neuron_count: 500
region_count: 1
learning_rate: 0.01
enable_stdp: true
simulation_timestep: 0.001
max_steps: 10000
checkpoint_interval: 1000
```

### INI Configuration
```ini
[simulation]
random_seed = 42
neuron_count = 500
region_count = 1

[plasticity]
learning_rate = 0.01
enable_stdp = true

[performance]
simulation_timestep = 0.001
max_steps = 10000
checkpoint_interval = 1000
```

## Experiment Types

NLM supports several predefined experiment types:

### Phase 3: Basic Learning
```bash
nlm experiment run --experiment phase3 --steps 10000
```

### Phase 4: Pattern Recognition
```bash
nlm experiment run --experiment phase4 --steps 20000
```

### Phase 5: Advanced Learning
```bash
nlm experiment run --experiment phase5 --steps 50000
```

### Phase 6: Integrated System
```bash
nlm experiment run --experiment phase6 --steps 100000
```

## Profiling Commands

### Memory Profiling
```bash
# Basic memory profiling
nlm profile memory --output memory_profile.json

# Continuous memory monitoring
nlm profile memory --duration 60000 --continuous

# Memory leak detection
nlm profile leak --duration 300000
```

### CPU Profiling
```bash
# CPU usage profiling
nlm profile cpu --output cpu_profile.json --duration 30000
```

### Timeline Profiling
```bash
# Timeline-based profiling
nlm profile timeline --output timeline_profile.json
```

## Debugging Commands

### Neural Activity Debugging
```bash
# Debug neural activity
nlm debug neural --output neural_activity.json --live

# Debug spike trains
nlm debug spikes --output spike_traces.json --neuron-type excitatory
```

### Synapse Evolution Tracking
```bash
# Track synapse weight evolution
nlm debug synapses --output synapse_evolution.json --track-weights
```

### Memory Trace Analysis
```bash
# Analyze memory usage
nlm debug memory --output memory_trace.json
```

## Parallel Execution

### Basic Parallel Execution
```bash
# Run 4 parallel processes
nlm parallel execute --num-processes 4 --experiment phase3 --output-dir parallel_results
```

### GPU-Accelerated Execution
```bash
# Run GPU-accelerated experiments
nlm parallel gpu --output-dir gpu_results
```

### Distributed Execution
```bash
# Run distributed experiments
nlm parallel distributed --steps 50000 --output-dir distributed_results
```

## Custom Analysis

### Metric Collection
```bash
# Define and collect custom metrics
nlm analysis metrics --metric-file my_metrics.json --output metrics_report.json
```

### Statistical Analysis
```bash
# Perform statistical analysis
nlm analysis statistical --input metrics.json --analysis comprehensive
```

### Machine Learning
```bash
# Train ML models on neural data
nlm analysis machine-learning --training data.json --model random_forest --output model.json
```

## Advanced Features

### Configuration Templates

NLM provides built-in configuration templates:

```bash
# Create standard template
nlm config template standard

# Create research template
nlm config template research

# Create performance template
nlm config template performance
```

### Configuration Comparison
```bash
# Compare two configuration files
nlm config export --file config1.json --format json | nlm config compare --file config2.json
```

### Result Aggregation
```bash
# Aggregate results from multiple experiments
nlm parallel aggregate --input results/ --output aggregated.json
```

## Error Handling

Common error messages and solutions:

1. **Configuration validation fails** - Check that all required keys are present
2. **Experiment not found** - Use `nlm experiment list` to see available experiments
3. **Memory profiling fails** - Ensure sufficient memory is available
4. **GPU not available** - Install GPU drivers and CUDA toolkit
5. **MPI not initialized** - Install OpenMPI or equivalent

## Troubleshooting

### Command Not Found
Ensure NLM is installed and in PATH:
```bash
which nlm
nlm --version
```

### Permission Denied
Check file permissions:
```bash
chmod +x nlm
```

### Memory Issues
Increase available memory or reduce experiment size:
```bash
nlm config set max_steps 5000
```

### GPU Issues
Check GPU availability:
```bash
nvidia-smi
```

### MPI Issues
Check MPI installation:
```bash
mpirun --version
```

## Scripts and Automation

NLM provides several example scripts for common workflows:

### Basic Usage Script (`examples/basic_usage.py`)
```python
from pynlm import BrainAPI
import json

# Create and run brain simulation
brain = BrainAPI.create_brain()
BrainAPI.initialize_brain(brain)
BrainAPI.run_simulation(brain, 10000)
BrainAPI.log_brain_status(brain)
```

### Analysis Script (`examples/advanced_analysis.py`)
```python
from pynlm import AnalysisAPI, BrainAPI
import numpy as np

# Create brain and collect data
brain = BrainAPI.create_brain()
BrainAPI.initialize_brain(brain)

# Perform various analyses
stats = AnalysisAPI.calculate_statistics([1, 2, 3, 4, 5])
metrics = AnalysisAPI.detect_anomalies([1.0, 1.1, 1.2, 1.3, 1.4, 10.0])
```

### Parallel Execution Script (`examples/parallel_execution.py`)
```python
from pynlm import BatchAPI
import multiprocessing as mp

# Configure and run parallel experiments
params = BatchAPI.create_batch_parameters(10)
for i in range(params.numExperiments):
    BatchAPI.add_experiment_to_batch(params, "phase3", 10000)

BatchAPI.execute_batch(params)
```

### Debugging Script (`examples/debugging.py`)
```python
from pynlm import DebugAPI, BrainAPI

# Debug neural activity
brain = BrainAPI.create_brain()
DebugAPI.debug_neural_activity(brain, "neural_debug.json")
```

### Performance Monitoring Script (`examples/performance_monitoring.py`)
```python
from pynlm import PerformanceAPI

# Monitor system performance
metrics = PerformanceAPI.get_performance_metrics()
print(f"CPU Usage: {metrics.cpuUsage}%")
print(f"Memory Usage: {metrics.memoryUsage} MB")
```

### Experiment Pipeline Script (`examples/experiment_pipeline.py`)
```python
from pynlm import PipelineAPI, BrainAPI, ConfigAPI

# Create and execute experiment pipeline
pipeline = []
PipelineAPI.add_pipeline_step(pipeline, "initialize", lambda config, brain, _: BrainAPI.initialize_brain(brain))
PipelineAPI.add_pipeline_step(pipeline, "run", lambda config, brain, _: BrainAPI.run_simulation(brain, 10000))
PipelineAPI.execute_pipeline(pipeline, ConfigAPI.createConfig(), BrainAPI.create_brain())
```

### TensorFlow Integration Script (`examples/tensorflow_integration.py`)
```python
import tensorflow as tf
from pynlm import AnalysisAPI

# Use neural data for TensorFlow training
# ... integration code ...
```

### PyTorch Integration Script (`examples/pytorch_integration.py`)
```python
import torch
from pynlm import AnalysisAPI

# Use neural data for PyTorch training
# ... integration code ...
```

### Custom Metrics Script (`examples/custom_metrics.py`)
```python
from pynlm import AnalysisAPI

def custom_firing_rate(brain, step):
    return brain.getTotalSpikeCount() / brain.getTotalNeuronCount()

# Register custom metric
AnalysisAPI.add_custom_metric("firing_rate", custom_firing_rate, "spikes/neuron")
```

## API Reference

For detailed API documentation, refer to:
- `docs/configuration_guide.md` - Configuration management
- `docs/experiment_protocol.md` - Experiment protocols
- Generated API documentation for Python bindings

## Examples Gallery

The `examples/` directory contains comprehensive examples demonstrating:

1. Basic NLM usage
2. Advanced analysis techniques
3. Parallel execution strategies
4. Performance monitoring
5. Debugging workflows
6. Machine learning integration
7. Experiment pipeline automation
8. Configuration management

## License

NLM is licensed under the MIT License. See LICENSE file for details.

## Contributing

Contributions are welcome! Please submit pull requests with appropriate documentation.

## Support

For support, visit the NLM GitHub repository or contact the development team.

## Future Enhancements

Planned features for future releases:
- Enhanced GPU support with CUDA 12
- Distributed computing with Dask support
- Real-time visualization dashboards
- Cloud integration (AWS, GCP, Azure)
- Automated hyperparameter optimization
- Multi-language bindings (Rust, Julia)
- Advanced neural architecture search
