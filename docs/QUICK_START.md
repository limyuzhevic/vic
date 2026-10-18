# NLM Phase 6 Quick Start Guide

This guide provides step-by-step instructions for getting started with NLM Phase 6, from basic setup to advanced experimentation.

## Quick Start

### Step 1: Installation

#### Prerequisites
- C++ compiler supporting C++20
- CMake 3.16 or higher
- Python 3.8 or higher (for configuration tools)

#### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4

# Build specific targets (for testing)
make nlm_phase6_demo
make nlm_phase3_demo
```

### Step 2: Run Basic Demo

#### Basic Demo (Legacy)

```bash
# Run the basic Phase 6 demo (backward compatible)
./nlm_phase6_demo
```

#### Basic Demo (Advanced)

```bash
# Run with basic advanced options
./nlm_phase6_demo --preset benchmark --verbose
```

### Step 3: Run Advanced Experiments

#### Research Experiment

```bash
# Run comprehensive research experiment
./nlm_phase6_demo \
  --preset research \
  --output-file research_results.csv \
  --detailed-log research.log
```

#### Production Deployment

```bash
# Run production-ready experiment
./nlm_phase6_demo \
  --preset production \
  --output-file production_results.csv \
  --detailed-log production.log \
  --performance-report production_perf.html
```

#### Performance Benchmark

```bash
# Run comprehensive benchmark
./nlm_phase6_demo \
  --preset benchmark \
  --benchmark \
  --output-file benchmark_results.csv \
  --detailed-log benchmark.log
```

### Step 4: Analyze Results

#### Result Analysis

```bash
# Analyze generated results
python3 analyze_results.py research_results.csv
```

#### Performance Review

```bash
# Review performance reports
open production_perf.html
```

#### Visualization

```bash
# Generate visualizations
python3 generate_visualizations.py neural_data.csv --output-dir visualizations/
```

## Configuration Management

### Configuration File Examples

#### Basic Configuration

```yaml
# config.yaml
name: basic_experiment

# Core parameters
neuron_count: 1000
max_steps: 10000
region_count: 1
connection_probability: 0.1

# Feature flags
enable_checkpointing: true
enable_replay: true
enable_development: true

# Learning parameters
learning_rate: 0.01
exploration_rate: 10
```

#### Advanced Configuration

```yaml
# advanced_config.yaml
name: advanced_experiment

# Core parameters
neuron_count: 3000
max_steps: 50000
region_count: 2
connection_probability: 0.08

# Feature flags
enable_checkpointing: true
enable_replay: true
enable_development: true
enable_neuromodulation: true
enable_prediction: true
enable_cognition: true

# Performance optimization
parallel_processing: true
vectorization: true
memory_pooling: true

# Learning parameters
learning_rate: 0.008
exploration_rate: 12
reward_decay: 0.95
novelty_threshold: 0.5
curiosity_threshold: 0.3

# Debug and analysis
debug_mode: false
profiling_enabled: true
verbose_logging: false
```

### Configuration File Commands

#### Create Configuration File

```bash
# Create basic configuration
cat > config.yaml << 'EOF'
name: basic_experiment
neuron_count: 1000
max_steps: 10000
region_count: 1
connection_probability: 0.1
enable_checkpointing: true
enable_replay: true
enable_development: true
learning_rate: 0.01
exploration_rate: 10
EOF
```

#### Load Configuration File

```bash
# Load configuration from file
./nlm_phase6_demo --config-file config.yaml
```

#### Save Configuration

```bash
# Save current configuration
./nlm_phase6_demo --advanced-export --format yaml --output-file config.yaml
```

## Command Line Reference

### Core Commands

#### `--help`
Display help information

```bash
./nlm_phase6_demo --help
```

#### `--version`
Display version information

```bash
./nlm_phase6_demo --version
```

#### `--config-file <path>`
Load configuration from file

```bash
./nlm_phase6_demo --config-file my_config.yaml
```

### Preset Commands

#### `--preset <name>`
Apply configuration preset

```bash
# Available presets: research, production, benchmark, optimization, validation, debug, extreme
./nlm_phase6_demo --preset research
```

### Parameter Commands

#### `--neurons <count>`
Set number of neurons

```bash
./nlm_phase6_demo --neurons 2000
```

#### `--steps <count>`
Set maximum simulation steps

```bash
./nlm_phase6_demo --steps 50000
```

#### `--regions <count>`
Set number of brain regions

```bash
./nlm_phase6_demo --regions 2
```

#### `--conn-prob <value>`
Set connection probability

```bash
./nlm_phase6_demo --conn-prob 0.15
```

### Feature Commands

#### `--no-checkpointing`
Disable checkpoint save/load

```bash
./nlm_phase6_demo --no-checkpointing
```

#### `--no-replay`
Disable replay system

```bash
./nlm_phase6_demo --no-replay
```

#### `--no-development`
Disable developmental system

```bash
./nlm_phase6_demo --no-development
```

### Performance Commands

#### `--parallel`
Enable parallel processing

```bash
./nlm_phase6_demo --parallel
```

#### `--vectorize`
Enable vectorization optimizations

```bash
./nlm_phase6_demo --vectorize
```

#### `--memory-pool`
Enable memory pooling

```bash
./nlm_phase6_demo --memory-pool
```

### Debug Commands

#### `--debug`
Enable debug mode

```bash
./nlm_phase6_demo --debug
```

#### `--verbose`
Enable verbose logging

```bash
./nlm_phase6_demo --verbose
```

#### `--profile`
Enable performance profiling

```bash
./nlm_phase6_demo --profile
```

#### `--validate`
Run configuration validation

```bash
./nlm_phase6_demo --validate
```

### Output Commands

#### `--output-file <path>`
Save results to file

```bash
./nlm_phase6_demo --output-file results.csv
```

#### `--detailed-log <path>`
Save detailed logs

```bash
./nlm_phase6_demo --detailed-log experiment.log
```

#### `--performance-report <path>`
Generate performance report

```bash
./nlm_phase6_demo --performance-report perf.html
```

#### `--neural-activity <path>`
Save neural activity data

```bash
./nlm_phase6_demo --neural-activity neural_data.csv
```

### Advanced Commands

#### `--advanced-run`
Run advanced experiment with expert controls

```bash
./nlm_phase6_demo --advanced-run --preset research
```

#### `--advanced-preset <preset>`
Apply advanced configuration preset with overrides

```bash
./nlm_phase6_demo --advanced-preset research --neurons 3000 --learning-rate 0.01
```

#### `--advanced-optimize`
Optimize configuration for performance

```bash
./nlm_phase6_demo --advanced-optimize --target-throughput 1000
```

#### `--advanced-analysis`
Run advanced analysis tools

```bash
./nlm_phase6_demo --advanced-analysis --analysis-type neural
```

#### `--advanced-export`
Export results to external systems

```bash
./nlm_phase6_demo --advanced-export --format json
```

## Examples Directory

The examples directory contains practical scripts and configuration files:

### Example 1: Research Experiment

```bash
# Run research experiment
./examples/research_experiment.sh
```

**Script content:**
```bash
#!/bin/bash
./nlm_phase6_demo \
  --preset research \
  --output-file research_results.csv \
  --detailed-log research.log \
  --performance-report research_perf.html \
  --parallel \
  --vectorize \
  --memory-pool \
  --neural-activity neural_data.csv
```

### Example 2: Production Deployment

```bash
# Run production experiment
./examples/production_deployment.sh
```

**Script content:**
```bash
#!/bin/bash
./nlm_phase6_demo \
  --preset production \
  --output-file production_results.csv \
  --detailed-log production.log \
  --performance-report production_perf.html \
  --monitor-system \
  --generate-alerts \
  --save-checkpoints \
  --enable-replay \
  --neural-activity neural_data.csv
```

### Example 3: Performance Benchmarking

```bash
# Run comprehensive benchmark
./examples/performance_benchmark.sh
```

**Script content:**
```bash
#!/bin/bash
./nlm_phase6_demo \
  --preset benchmark \
  --benchmark \
  --output-file benchmark_results.csv \
  --detailed-log benchmark.log \
  --generate-performance-report \
  --analyze-scalability \
  --parallel \
  --vectorize
```

### Example 4: Debug and Validation

```bash
# Run debug experiment
./examples/debug_validation.sh
```

**Script content:**
```bash
#!/bin/bash
./nlm_phase6_demo \
  --preset debug \
  --validate \
  --detailed-log debug.log \
  --memory-analysis \
  --generate-debug-report \
  --enable-exception-handling \
  --verbose
```

## Troubleshooting

### Common Issues

#### Issue 1: "Command not found: nlm_phase6_demo"

**Solution**: Build the demo executable
```bash
cd build
make nlm_phase6_demo
```

#### Issue 2: "Unknown option: --preset"

**Solution**: Use a valid preset name
```bash
./nlm_phase6_demo --help
```

#### Issue 3: Memory usage too high

**Solution**: Reduce configuration size
```bash
./nlm_phase6_demo --preset benchmark --neurons 500
```

#### Issue 4: Simulation takes too long

**Solution**: Use faster preset or reduce steps
```bash
./nlm_phase6_demo --preset benchmark --steps 1000
```

### Error Messages

#### "Configuration validation failed"

**Solution**: Check configuration parameters in config.yaml

#### "Failed to save checkpoint"

**Solution**: Check file system permissions and disk space

#### "Memory usage approaching limit"

**Solution**: Reduce neuron count or enable memory pooling

#### "Numerical instability"

**Solution**: Reduce learning rate or enable numerical stability checks

## Configuration Reference

### Configuration File Format

NLM Phase 6 supports YAML configuration files:

```yaml
# Configuration file example
name: experiment_name

# Core simulation parameters
neuron_count: 1000
max_steps: 10000
region_count: 1
connection_probability: 0.1
timestep: 0.001

# Feature flags
enable_checkpointing: true
enable_replay: true
enable_development: true
enable_neuromodulation: true
enable_prediction: true
enable_cognition: true

# Learning parameters
learning_rate: 0.01
exploration_rate: 10
reward_decay: 0.95
novelty_threshold: 0.5
curiosity_threshold: 0.3

# Performance optimization
parallel_processing: true
vectorization: true
memory_pooling: true

# Debug and analysis
debug_mode: false
profiling_enabled: true
verbose_logging: false

# Validation and safety
max_neurons: 10000
max_steps_safe: 50000
min_connection_probability: 0.01
max_connection_probability: 0.5
```

### Environment Variables

#### `NLM_CONFIG_PATH`
Path to configuration file

#### `NLM_LOG_LEVEL`
Log level (DEBUG, INFO, WARN, ERROR)

#### `NLM_VERBOSE`
Enable verbose logging (1/0)

#### `NLM_PARALLEL`
Enable parallel processing (1/0)

## Frequently Asked Questions

### Q1: How do I get started with NLM Phase 6?

A1: Start with the basic demo `./nlm_phase6_demo --preset benchmark --verbose`. This provides a good starting point with minimal complexity.

### Q2: What's the difference between presets?

A2: Each preset is optimized for a specific use case:
- **research**: Long-term research with full features
- **production**: Balanced production deployment
- **benchmark**: Fast testing and validation
- **optimization**: Performance-focused experiments
- **validation**: Comprehensive testing
- **debug**: Maximum debug output
- **extreme**: Large-scale experiments

### Q3: How do I analyze results?

A3: Use the analysis scripts in the examples directory, or generate HTML reports with `--performance-report`.

### Q4: Can I run experiments remotely?

A4: Yes, use SSH tunneling or remote execution with `--advanced-export` to save results to remote systems.

### Q5: How do I monitor system performance?

A5: Enable profiling with `--profile` and `--verbose` flags. Use `--performance-report` to generate detailed performance reports.

## Support

For support, visit the NLM project documentation or create an issue in the GitHub repository.

---

*This guide is continuously updated with new features and improvements.*
