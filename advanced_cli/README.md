# NLM - Enhanced Command Line Interface

This directory contains the enhanced command-line interface for NLM with advanced features for expert users.

## Overview

The enhanced CLI provides comprehensive control over NLM experiments, including:
- Advanced experiment configuration
- Real-time monitoring and analysis
- Data export for scientific research
- Performance benchmarking
- Automated experiment management

## Installation

```bash
# Make sure to install required dependencies first
system dependencies:
  - Python 3.8+
  - CMake 3.16+
  - C++ compiler

python dependencies:
  - pyyaml
  - numpy (for analysis)
  - h5py (for HDF5 export)

# Install Python dependencies
pip install pyyaml numpy h5py
```

## Usage

### Basic Commands

```bash
# Run an experiment with custom configuration
./advanced_cli.py run configs/experiment.yaml --duration 120 --checkpoint 30

# Create custom configuration from base
./advanced_cli.py create "My Experiment" configs/base_config.yaml

# Analyze experiment results
./advanced_cli.py analyze results/experiment_1.json

# Export data in various formats
./advanced_cli.py export data/results.json json output/scientific_data.json

# Run performance benchmarks
./advanced_cli.py benchmark --duration 30

# Show help information
./advanced_cli.py help
```

### Advanced Usage

```bash
# Run multiple experiments with different configurations
for config in configs/*.yaml; do
    ./advanced_cli.py run "$config" --duration 60 --checkpoint 10
    ./advanced_cli.py analyze results/experiment_1.json
    ./advanced_cli.py export results/experiment_1.json json results/experiment_1_data.json
    mv results/experiment_1.json results/experiment_$(date +%Y%m%d_%H%M%S).json
done

# Create complex experiment configurations
./advanced_cli.py create "Research Experiment" \
    --brain.neuron_count=5000 \
    --brain.simulation_steps=100000 \
    --world.width=200.0 \
    --world.height=200.0 \
    --agent.energy_decay_rate=0.0001
```

## Configuration Files

### Available Configurations

The `configs/` directory contains pre-configured experiment setups:

- `development.yaml` - Detailed debugging and logging
- `production.yaml` - Optimized for performance
- `research.yaml` - Maximum flexibility and control
- `debug.yaml` - Intensive debugging mode
- `optimization.yaml` - Performance-focused settings

### Configuration Format

Configurations are stored in YAML format and support the following sections:

```yaml
brain:
  neuron_count: 1000
  simulation_steps: 10000
  timestep: 0.01
  enable_neuromodulation: true
  enable_development: true
  enable_curiosity: true
  neuromodulation_preset: "balanced"
  plasticity_preset: "adaptive"

world:
  width: 100.0
  height: 100.0
  vision_width: 16
  vision_height: 16
  enable_vision: true
  enable_audio: false

agent:
  energy_decay_rate: 0.001
  health_threshold: 0.1
  enable_structural_plasticity: true
  memory_capacity: 100.0
  curiosity_threshold: 0.3

experiments:
  duration: 60.0
  repetitions: 5
  checkpoint_interval: 10.0
  save_checkpoints: true
```

## Analysis and Results

### Generated Outputs

When running experiments, the CLI generates the following outputs:

1. **Checkpoints**: Regular saved states of the simulation
   - Format: JSON with brain state and metrics
   - Location: `results/<experiment_name>/checkpoints/`

2. **Performance Metrics**: Real-time monitoring data
   - Learning curves
   - Neuromodulation activity
   - Novelty and curiosity levels
   - Prediction errors

3. **Analysis Reports**: Automated experiment analysis
   - Statistical summaries
   - Trend analysis
   - Anomaly detection
   - Recommendations

### Supported Export Formats

The CLI supports exporting data in multiple formats:

- **JSON**: Standard format for scientific data
- **CSV**: Spreadsheet-compatible format
- **HDF5**: High-performance format for large datasets
- **Pickle**: Python object serialization

### Example Analysis Commands

```bash
# Analyze experiment results and generate report
./advanced_cli.py analyze results/experiment_1.json --output report.json

# Compare multiple experiment runs
./advanced_cli.py compare results/experiment_1.json results/experiment_2.json

# Generate visualization scripts
./advanced_cli.py visualize results/experiment_1.json --format plots

# Export results for statistical analysis
./advanced_cli.py export results/experiment_1.json hdf5 scientific_data.h5
```

## Performance Benchmarking

### Running Benchmarks

```bash
# Basic performance benchmark
./advanced_cli.py benchmark --duration 30 --iterations 10

# Detailed benchmark with custom settings
./advanced_cli.py benchmark \
    --duration 60 \
    --iterations 5 \
    --memory_limit 2048 \
    --cpu_cores 4
```

### Benchmark Metrics

The benchmarking system measures:
- Simulation performance (steps per second)
- Memory usage patterns
- CPU utilization
- I/O performance
- Network latency (for distributed systems)
- Resource efficiency

### Benchmark Configuration

Benchmarks can be configured with custom parameters:

```yaml
benchmark:
  duration: 60.0
  iterations: 10
  warmup_time: 10.0
  memory_limit: 2048
  cpu_cores: 4
  metrics:
    - simulation_speed
    - memory_usage
    - cpu_utilization
    - iops
    - network_latency
```

## Advanced Features

### 1. Real-time Monitoring

The CLI provides real-time monitoring capabilities:

```bash
# Start monitoring with detailed output
./advanced_cli.py run config.yaml --monitor --verbose

# Export monitoring data for analysis
./advanced_cli.py export monitoring_data.json json real_time_metrics.json

# Generate monitoring dashboards
./advanced_cli.py dashboard --update-interval 1.0
```

### 2. Automated Experiment Management

The CLI includes automated experiment management:

```bash
# Run parameter sweep
./advanced_cli.py sweep configs/base_config.yaml \
    --parameters "brain.neuron_count=[1000,2000,5000]" \
    --parameters "world.width=[50.0,100.0,200.0]" \
    --repetitions 3

# Run hyperparameter optimization
./advanced_cli.py optimize configs/base_config.yaml \
    --objective "final_performance" \
    --algorithm "bayesian"
```

### 3. Reproducibility and Validation

The CLI includes tools for ensuring reproducibility:

```bash
# Generate experiment signature for reproducibility
./advanced_cli.py signature config.yaml --seed 12345 --hash algorithm sha256

# Validate experiment reproducibility
./advanced_cli.py validate results/experiment_1.json results/experiment_2.json

# Generate experiment metadata
./advanced_cli.py metadata config.yaml --format json > experiment_metadata.json
```

## Troubleshooting

### Common Issues

1. **Missing Dependencies**
```bash
pip install pyyaml numpy h5py
```

2. **Permission Errors**
```bash
chmod +x advanced_cli.py
```

3. **Memory Issues**
```bash
# Increase memory limits for large experiments
./advanced_cli.py run config.yaml --memory-limit 8192
```

4. **Configuration Errors**
```bash
# Validate configuration before running
./advanced_cli.py validate config.yaml
```

### Getting Help

- **Usage**: `./advanced_cli.py help`
- **Command-specific help**: `./advanced_cli.py <command> help`
- **Examples**: Check the examples directory for usage patterns
- **Documentation**: See `ADVANCED_IMPROVEMENTS.md` for detailed information

## Examples Directory

The `examples/` directory contains practical examples of using the advanced CLI:

- `basic_usage.yaml` - Basic experiment setup
- `advanced_config.yaml` - Complex configuration
- `parameter_sweep.yaml` - Parameter optimization setup
- `monitoring_config.yaml` - Real-time monitoring setup
- `comparison_example.py` - Experiment comparison script
- `visualization_example.py` - Data visualization examples

## Contributing

### Adding New Features

To add new features to the advanced CLI:

1. Create a new command in the `commands/` directory
2. Implement the command logic in `advanced_cli.py`
3. Add configuration support in `config/`
4. Update documentation in this file
5. Add tests in `tests/`

### Bug Reports

For bug reports, please:
1. Use the issue template in the repository
2. Include configuration details
3. Provide reproduction steps
4. Include any relevant log files

## License

This enhanced CLI is part of the NLM (Neural Learning Machine) project and is licensed under the MIT License.

See `LICENSE` file for more details.
