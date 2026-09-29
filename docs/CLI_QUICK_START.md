# NLM CLI Quick Start Guide

## Installation

### Prerequisites

- C++ compiler with C++17 support
- CMake 3.10 or higher
- pthread library (for multithreading)

### Building from Source

1. Clone the NLM repository:
```bash
git clone <repository-url>
cd nlm
```

2. Build the NLM CLI:
```bash
mkdir build
cd build
cmake ..
make
```

3. Install:
```bash
sudo make install
```

4. Verify installation:
```bash
nlm --version
```

## Getting Started

### 1. Running a Quick Simulation

The fastest way to get started is with a quick simulation using the built-in template:

```bash
# Generate configuration from quick simulation template
nlm config generate quick_sim quick_config.yaml

# Run the basic simulation
nlm run basic quick_config.yaml
```

### 2. Exploring Available Commands

Get help for specific commands:

```bash
nlm run --help
nlm config --help
nlm experiment --help
nlm benchmark --help
nlm visualize --help
nlm analyze --help
```

### 3. Creating and Running an Experiment

```bash
# Create a new experiment
nlm experiment create my_first_experiment --config config/experiment.yaml

# List available experiments
nlm experiment list

# Run the experiment
nlm experiment run my_first_experiment
```

### 4. Performance Benchmarking

```bash
# Run basic performance benchmark
nlm benchmark basic config/experiment.yaml

# Run scalability benchmark
nlm benchmark scalability config/experiment.yaml

# Generate performance report
nlm benchmark report config/experiment.yaml
```

### 5. Analyzing Results

After running simulations, analyze your results:

```bash
# Analyze simulation results
nlm analyze results ./simulation_results

# Compare two result directories
nlm analyze compare ./results_run1 ./results_run2

# Generate analysis report
nlm analyze report ./simulation_results --format markdown
```

### 6. Visualizing Data

Create visualizations of your results:

```bash
# Visualize performance metrics
nlm visualize metrics ./simulation_results

# Visualize learning curves
nlm visualize learning ./simulation_results

# Visualize memory usage patterns
nlm visualize memory ./simulation_results
```

### 7. Configuration Management

Generate custom configurations:

```bash
# Generate configuration from large scale template
nlm config generate large_scale large_config.yaml

# Validate your configuration
nlm config validate my_config.yaml

# Merge two configuration files
nlm config merge config1.yaml config2.yaml -o merged_config.yaml
```

### 8. Automation with Script Generation

Generate automation scripts for running experiments:

```bash
# Generate bash script
nlm generate script --template bash_template --output run_experiment.sh

# Generate Docker file
nml generate docker --config config.yaml --output Dockerfile

# Generate SLURM job script
nlm generate slurm --config config.yaml --output job.slurm

# Generate Kubernetes manifest
nlm generate kubernetes --config config.yaml --output k8s.yaml
```

## Common Workflows

### Batch Processing

```bash
# Run multiple simulations in batch
nlm run batch config/batch_config.yaml

# Process results from multiple runs
for dir in results/*; do
    nlm analyze results "$dir"
done
```

### Experiment Management

```bash
# Create experiment with specific parameters
nlm experiment create exp1 --config config/exp1.yaml
nlm experiment create exp2 --config config/exp2.yaml

# Run all experiments
for exp in exp1 exp2; do
    nlm experiment run "$exp"
done

# Analyze all experiments
for exp in exp1 exp2; do
    nlm experiment analyze "$exp"
done
```

### Performance Testing

```bash
# Run comprehensive performance benchmarks
nlm benchmark basic config.yaml
nlm benchmark scalability config.yaml
nlm benchmark memory config.yaml
nlm benchmark profile config.yaml

# Generate comprehensive report
nlm benchmark report config.yaml
```

### Data Analysis Pipeline

```bash
# Complete analysis pipeline
nlm run basic config.yaml
nlm analyze results ./output
nlm visualize metrics ./output
nlm visualize learning ./output
nlm analyze report ./output --format markdown
```

## Configuration File Examples

### Quick Simulation Config (`quick_config.yaml`)
```yaml
neuron_count: 1000
max_steps: 10000
enable_development: true
enable_curiosity: true
learning_rate: 0.01
window_size: 100
train_on_gpu: false
```

### Experiment Config (`experiment.yaml`)
```yaml
experiment_name: my_experiment
neuron_count: 5000
max_steps: 5000
enable_curiosity: true
enable_development: true
learning_rate: 0.01
window_size: 500
random_seed: 42
output_dir: "./results"
```

## Troubleshooting

### Common Issues

**Command not found**
- Ensure NLM CLI is installed and in your PATH
- Run `nlm --version` to verify installation

**Configuration file not found**
- Use absolute paths or relative paths from current directory
- Use `nlm config generate quick_sim <filename>` to create example config

**Permission denied**
- Check file permissions
- For script generation, ensure scripts are executable (`chmod +x script.sh`)

**Memory errors**
- Use `--thread-count 1` for single-threaded execution
- Use `nlm config generate large_scale` for optimized large-scale configuration

### Getting Help

For detailed information:
- `nlm --help` - Global help
- `nlm <command> --help` - Command-specific help
- Visit the NLM documentation at <documentation-url>

## Tips and Best Practices

1. **Use templates**: Always generate configurations from templates for consistency
2. **Monitor progress**: Use `--verbose` flag to see detailed output
3. **Manage output**: Use `--output-dir` to organize results
4. **Batch processing**: Use batch commands for running multiple simulations
5. **Automation**: Generate scripts for repetitive tasks
6. **Version control**: Keep configuration files in version control
7. **Backup**: Regularly backup important experiment results

## Next Steps

Once you're comfortable with the basic commands:

1. Explore advanced configuration options
2. Implement custom analysis scripts
3. Set up automated experiment pipelines
4. Integrate with distributed computing systems
5. Contribute to the NLM community

The NLM CLI is a powerful tool that can be customized and extended for your specific research needs.
