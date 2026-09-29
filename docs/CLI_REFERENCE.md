# NLM CLI Reference

## Overview

The NLM (Neural Language Model) Neural Network Simulator provides a comprehensive command-line interface for running simulations, configuring experiments, and analyzing results.

## Core Commands

### run
Run a simulation or experiment.

**Subcommands:**
- `basic [config]` - Run basic simulation
- `quick [config]` - Run quick test simulation
- `long [config]` - Run long-duration simulation
- `batch [config]` - Run batch of simulations
- `checkpoint [config]` - Run with checkpointing

### config
Configuration management.

**Subcommands:**
- `generate [template]` - Generate configuration file from template
- `validate [file]` - Validate configuration file
- `merge [file1] [file2] -o [output]` - Merge configuration files
- `export [file] --format [format]` - Export config in different formats
- `import [file] --format [format]` - Import config from different formats
- `show [key]` - Show configuration values

### experiment
Experiment management and analysis.

**Subcommands:**
- `create [name] --config [file]` - Create new experiment
- `run [name] [options]` - Run experiment with advanced options
- `list` - List available experiments
- `status [name]` - Show experiment status
- `analyze [name]` - Analyze experiment results
- `export [name] --format [format]` - Export experiment data

### benchmark
Performance benchmarking.

**Subcommands:**
- `basic [config]` - Basic performance benchmarking
- `scalability [config]` - Scalability testing
- `memory [config]` - Memory usage benchmarking
- `profile [config]` - Detailed profiling
- `report [config]` - Generate performance report

### visualize
Visualization and analysis.

**Subcommands:**
- `metrics [directory]` - Visualize performance metrics
- `memory [directory]` - Visualize memory usage patterns
- `learning [directory]` - Visualize learning curves
- `network [directory]` - Visualize network structure
- `results [directory] --type [type]` - Generate visualizations

### analyze
Data analysis and reporting.

**Subcommands:**
- `results [directory]` - Analyze simulation results
- `patterns [directory]` - Pattern analysis
- `trends [directory]` - Trend analysis
- `compare [dir1] [dir2]` - Compare results between runs
- `report [directory] --format [format]` - Generate analysis report

### help
Show help information.

## Global Options

- `--version` - Show version information
- `--help` - Show global help
- `--verbose, -v` - Enable verbose output
- `--quiet, -q` - Suppress non-essential output
- `--config FILE, -c FILE` - Configuration file path
- `--output-dir DIR, -o DIR` - Output directory for results
- `--log-level LEVEL` - Set logging level (debug, info, warning, error)
- `--thread-count N` - Set number of worker threads

## Global Arguments

- `SIMULATION_FILE` - Path to simulation configuration file
- `EXPERIMENT_CONFIG` - Path to experiment configuration

## Configuration Templates

Available templates:

### quick_sim.yaml
```yaml
template: quick_simulation
neuron_count: 1000
max_steps: 10000
enable_development: true
enable_curiosity: true
learning_rate: 0.01
window_size: 100
train_on_gpu: false
```

### large_scale.yaml
```yaml
template: large_scale
neuron_count: 100000
max_steps: 1000000
enable_checkpointing: true
enable_multithreading: true
enable_vectorization: true
learning_rate: 0.001
window_size: 1000
train_on_gpu: true
gpu_count: 4
distributed_backend: "mpi"
```

### experiment.yaml
```yaml
template: experiment
experiment_name: test_experiment
neuron_count: 5000
max_steps: 5000
enable_curiosity: true
enable_development: true
learning_rate: 0.01
window_size: 500
random_seed: 42
output_dir: "./results"
```

## Examples

### Running a basic simulation
```bash
nlm run basic config.yaml
```

### Generating configuration from template
```bash
nlm config generate quick_sim output_config.yaml
```

### Validating configuration
```bash
nlm config validate my_config.yaml
```

### Running an experiment
```bash
nlm experiment create my_experiment --config exp_config.yaml
nlm experiment run my_experiment
```

### Benchmarking performance
```bash
nlm benchmark basic config.yaml
```

### Analyzing results
```bash
nlm analyze results ./simulation_results
```

### Visualizing metrics
```bash
nlm visualize metrics ./results
```

## Output Formats

The CLI supports various output formats:

- **JSON** - Machine-readable format
- **YAML** - Human-readable format
- **CSV** - Tabular data format
- **SQLite** - Structured database format
- **HDF5** - Scientific dataset format

## Visualization Formats

- **PNG** - Static raster images
- **SVG** - Scalable vector graphics
- **PDF** - Portable document format
- **HTML** - Interactive dashboards
- **Plotly.js** - Web-based visualizations

## Report Formats

- **Markdown** - Plain text with formatting
- **HTML** - Web pages with embedded plots
- **LaTeX** - Academic paper format
- **JSON** - Machine-readable reports

## Script Generation

The CLI can generate automation scripts:

- `nlm generate script --template bash_template --output script.sh`
- `nlm generate docker --config config.yaml --output Dockerfile`
- `nlm generate slurm --config config.yaml --output job.slurm`
- `nlm generate kubernetes --config config.yaml --output k8s.yaml`

## Automation Features

- Automated experiment execution with dependencies
- Parallel execution of multiple simulations
- Result aggregation and reporting
- Automated parameter sweeps and optimization

## Configuration System Integration

- Integration with enhanced JSON/YAML configuration
- Command-line argument overrides
- Environment variable support
- Configuration validation and error handling

## Logging System Integration

- Log level configuration
- Output format customization
- Log file management
- Real-time log viewing

## Performance Monitoring Integration

- Real-time performance metrics
- Memory usage tracking
- CPU utilization monitoring
- Automated performance regression detection
