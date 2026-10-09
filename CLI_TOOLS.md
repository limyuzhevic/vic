# NLM CLI Tools Documentation

This document provides comprehensive documentation for the NLM (Neural Learning Machine) CLI tools, which enable users to interact with neural simulations through command-line interfaces.

## Overview

The NLM CLI tools provide a unified command-line interface for:

1. **Running simulations** with various configurations and performance options
2. **Managing configurations** (create, modify, save, load)
3. **Monitoring performance** in real-time
4. **Profiling performance** to identify bottlenecks
5. **Managing checkpoints** for simulation state persistence
6. **Debugging** simulation issues

## Available CLI Tools

### 1. nlm-cli - Main Simulation CLI

**Purpose**: Run neural simulations with various configurations and performance optimizations.

**Usage**: `nlm-cli [command] [options]`

**Available Commands**:

- `run` - Run a simulation with specified configuration
- `config` - Configuration management (list, set, get, save)
- `monitor` - Performance monitoring (start, stop, report)
- `profile` - Performance profiling (generate profile data)
- `checkpoint` - Checkpoint management (save, load, list)
- `debug` - Debug utilities (memory, threads, validation)
- `help` - Show help message

**Examples**:

```bash
# Run a simulation with default configuration
nlm-cli run

# Run with custom configuration file
nlm-cli run --config configs/experiment.cfg

# Run with performance optimizations
nlm-cli run --enable-multithreading --enable-event-driven

# Run specific simulation phase
nlm-cli run --phase2 --steps 10000

# Run with verbose output
nlm-cli run --verbose --help
```

### 2. nlm-config - Configuration Management

**Purpose**: Manage NLM configuration files and settings.

**Usage**: `nlm-config <command> [options]`

**Available Commands**:

- `list` - List all configuration keys and values
- `set KEY VALUE` - Set a configuration key to a value
- `get KEY` - Get the value of a configuration key
- `save` - Save current configuration to file
- `load` - Load configuration from file
- `help` - Show help message

**Examples**:

```bash
# List all configuration settings
nlm-config list

# Set a configuration value
nlm-config set random_seed 12345
nlm-config set neuron_count 2000
nlm-config set connection_probability 0.15

# Get a configuration value
nlm-config get random_seed
nlm-config get neuron_count

# Save configuration to file
nlm-config save --file configs/my_experiment.cfg

# Load configuration from file
nlm-config load --file configs/my_experiment.cfg
```

### 3. nlm-monitor - Performance Monitoring

**Purpose**: Monitor NLM simulation performance in real-time.

**Usage**: `nlm-monitor <command> [options]`

**Available Commands**:

- `start` - Start performance monitoring
- `stop` - Stop performance monitoring
- `report` - Show current performance report
- `help` - Show help message

**Examples**:

```bash
# Start monitoring with default settings
nlm-monitor start

# Start monitoring with specific interval and duration
nlm-monitor start --interval 0.1 --duration 60

# Stop monitoring
nlm-monitor stop

# Show current performance report
nlm-monitor report
```

### 4. nlm-profile - Performance Profiling

**Purpose**: Generate and analyze performance profiles for different configurations.

**Usage**: `nlm-profile <command> [options]`

**Available Commands**:

- `generate` - Generate a new performance profile
- `compare` - Compare two performance profiles
- `analyze` - Analyze a profile file
- `help` - Show help message

**Examples**:

```bash
# Generate a profile with default settings
nlm-profile generate

# Generate a profile with custom settings
nlm-profile generate --config configs/fast.cfg --name fast_profile --steps 5000

# Compare two profiles
nlm-profile compare profile1.prof profile2.prof

# Analyze a profile file
nlm-profile analyze my_profile.prof
```

### 5. nlm-checkpoint - Checkpoint Management

**Purpose**: Manage simulation checkpoints for state persistence and recovery.

**Usage**: `nlm-checkpoint <command> [options]`

**Available Commands**:

- `save` - Save a checkpoint
- `load` - Load a checkpoint
- `list` - List all checkpoints
- `delete` - Delete a checkpoint
- `configure` - Configure checkpoint manager settings
- `help` - Show help message

**Examples**:

```bash
# Save a checkpoint
nlm-checkpoint save --name my_checkpoint --step 1000

# Load a checkpoint
nlm-checkpoint load --name my_checkpoint

# List all checkpoints
nlm-checkpoint list

# Delete a checkpoint
nlm-checkpoint delete --name old_checkpoint

# Configure checkpoint manager
nlm-checkpoint configure --dir my_checkpoints --max-checkpoints 20
```

### 6. nlm-debug - Debugging and Visualization

**Purpose**: Debug NLM simulation components and diagnose issues.

**Usage**: `nlm-debug <command> [options]`

**Available Commands**:

- `memory` - Debug memory usage
- `threads` - Debug thread pool
- `validation` - Validate brain state
- `brain` - Debug brain state
- `help` - Show help message

**Examples**:

```bash
# Debug memory usage
nlm-debug memory
nlm-debug memory --detailed --histogram

# Debug thread pool
nlm-debug threads --status
nlm-debug threads --list

# Validate brain state
nlm-debug validation --type all --verbose

# Debug brain state
nlm-debug brain --action status
nlm-debug brain --action neurons --output neuron_debug.txt
```

## Configuration File Format

NLM configuration files use a simple key-value format:

```
# Configuration file example
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
dopamine_baseline = 0.1
reward_discount_factor = 0.99
environment_name = GridWorld
environment_width = 10
environment_height = 10
log_level = INFO
log_to_file = false
log_filename = nlm.log
max_simulation_steps = 10000
simulation_time_limit = 0.0
visualization_enabled = false
visualization_update_rate = 30.0
```

## Performance Optimization Options

All simulation runs support the following performance optimization options:

### 1. OptimizedBrain Features

- **Event-driven processing**: Only active neurons consume computation
- **Structure-of-Arrays (SoA)**: Better cache utilization and SIMD optimization
- **Sparse connectivity**: Efficient storage and lookup for large networks
- **Memory pools**: Pre-allocated neuron and synapse pools

### 2. Multi-threading Options

- **Parallel population updates**: Multiple threads process different neuron populations
- **Work-stealing load balancing**: Efficient task distribution
- **Thread-local state accumulation**: Reduced contention

### 3. SIMD Vectorization

- **AVX-512**: 16 floats per vector instruction
- **AVX2**: 8 floats per vector instruction
- **SSE**: 4 floats per vector instruction
- **Scalar fallback**: Works on all platforms

### 4. Checkpointing

- **Incremental saves**: Only changed data is saved
- **Compression**: Configurable compression levels
- **Validation**: Integrity checking and version compatibility
- **Streaming**: Efficient handling of large checkpoints

## Performance Monitoring Metrics

The performance monitoring system tracks the following metrics:

### Core Performance Metrics

| Metric | Description | Importance | Measurement Method |
|--------|-------------|------------|-------------------|
| Simulation Time | Time per simulation step | Overall speed | `time.time()` measurements |
| Firing Rate | Average neuron firing rate | Neural activity level | `brain.getAverageFiringRate()` |
| Memory Usage | Memory consumption | Resource efficiency | `psutil` or `memory_profiler` |
| CPU Usage | Processor utilization | Computational efficiency | `psutil` or `time` measurements |
| Spike Count | Total number of neural spikes | Learning activity | `brain.getTotalSpikeCount()` |
| Network Latency | Time between sensor input and motor output | Real-time responsiveness | Timestamp measurements |

### Monitoring Commands

**Real-time Monitoring**:
- `nlm-monitor start` - Start continuous performance monitoring
- `--interval SECONDS` - Sampling interval (default: 0.1s)
- `--duration SECONDS` - Monitoring duration (default: infinite)

**Performance Reporting**:
- `nlm-monitor report` - Show current performance metrics
- Shows step time, firing rate, CPU usage, memory usage

**Live Metrics**:
- Real-time updates at specified intervals
- Shows current performance and trends

## Profiling Options

### Profile Generation

```bash
nlm-profile generate \
  --config configs/experiment.cfg \
  --name my_profile \
  --steps 5000 \
  --enable-multithreading \
  --disable-simd \
  --disable-event-driven
```

### Profile Comparison

```bash
nlm-profile compare profile1.prof profile2.prof
```

Shows correlation coefficients and performance differences between profiles.

## Configuration Management

### Setting Configuration Values

Configuration values can be set using different data types:

```bash
# Integer values
nlm-config set random_seed 12345
nlm-config set neuron_count 2000

# Floating-point values
nlm-config set connection_probability 0.15
nlm-config set simulation_timestep 0.001

# String values
nlm-config set environment_name GridWorld
nlm-config set log_filename nlm.log
```

### Configuration Templates

Predefined configuration templates for common use cases:

1. **Speed-optimized**: Fast performance for real-time applications
2. **Balanced**: General-purpose configuration
3. **Quality-optimized**: High fidelity for research
4. **Memory-efficient**: Minimal memory footprint

```bash
# Create optimized configurations
nlm-config set --brain-size medium --performance-mode speed
nlm-config set --brain-size medium --performance-mode balanced
nlm-config set --brain-size large --performance-mode quality
nlm-config set --brain-size small --performance-mode memory
```

## Debugging Options

### Memory Debugging

```bash
# Basic memory debugging
nlm-debug memory

# Detailed memory reporting
nlm-debug memory --detailed

# Memory usage histogram
nlm-debug memory --histogram

# Save debug report
nlm-debug memory --output memory_debug.txt
```

### Thread Debugging

```bash
# List all threads
nlm-debug threads --list

# Show thread pool status
nlm-debug threads --status

# Combined debugging
nlm-debug threads --list --status
```

### Brain Validation

```bash
# Validate all components
nlm-debug validation --type all --verbose

# Validate specific components
nlm-debug validation --type neurons --config configs/experiment.cfg

# Fix issues automatically
nlm-debug validation --type all --fix
```

## Performance Optimization Best Practices

### 1. Configuration Optimization

- Use appropriate brain size for your use case
- Balance between simulation speed and fidelity
- Configure plasticity parameters for your learning goals

### 2. Runtime Optimization

- Enable multithreading for large-scale simulations
- Use SIMD optimizations for neural computations
- Enable event-driven processing for sparse activity
- Use sparse connectivity for large networks

### 3. Monitoring and Profiling

- Use `nlm-monitor` for real-time performance monitoring
- Use `nlm-profile` for performance profiling
- Use `nlm-checkpoint` for simulation state management
- Use `nlm-debug` for troubleshooting

### 4. Performance Tuning

- Start with default settings
- Enable optimizations gradually
- Monitor performance metrics
- Profile bottlenecks
- Tune configuration based on results

## Common Issues and Solutions

### 1. High Memory Usage

**Solution**:
- Use `--disable-sparse-connectivity` if already using sparse representation
- Reduce `--memory-pool-size`
- Use `--performance-mode memory`

### 2. Slow Simulation Speed

**Solution**:
- Enable `--enable-multithreading`
- Enable `--enable-event-driven`
- Enable `--disable-simd` if SIMD is causing issues
- Use `--performance-mode speed`

### 3. Thread Pool Issues

**Solution**:
- Use `--thread-count N` to specify exact number of threads
- Use `--enable-multithreading` for automatic detection
- Monitor thread status with `nlm-debug threads --status`

### 4. Checkpoint Issues

**Solution**:
- Configure checkpoint directory with `nlm-checkpoint configure --dir /path/to/checkpoints`
- Adjust `--max-checkpoints` to control disk usage
- Use `--no-compress` if compression is causing issues

## Command Reference Summary

| Tool | Primary Purpose | Key Commands | Typical Use Case |
|------|-----------------|--------------|------------------|
| `nlm-cli` | Main simulation control | `run`, `config`, `monitor`, `profile`, `checkpoint`, `debug` | Complete simulation workflow |
| `nlm-config` | Configuration management | `list`, `set`, `get`, `save`, `load` | Configuration setup and management |
| `nlm-monitor` | Real-time monitoring | `start`, `stop`, `report` | Performance monitoring |
| `nlm-profile` | Performance profiling | `generate`, `compare`, `analyze` | Performance optimization |
| `nlm-checkpoint` | State management | `save`, `load`, `list`, `delete`, `configure` | Simulation persistence |
| `nlm-debug` | Troubleshooting | `memory`, `threads`, `validation`, `brain` | Issue diagnosis |

## Getting Started

For beginners, start with:

1. **Basic simulation**: `nlm-cli run`
2. **Configuration setup**: `nlm-config set random_seed 12345`
3. **Performance monitoring**: `nlm-monitor start`
4. **Checkpoint management**: `nlm-checkpoint save --name initial --step 0`

For advanced usage, explore:

1. **Performance optimization**: `nlm-cli run --enable-multithreading --enable-event-driven`
2. **Profiling**: `nlm-profile generate --name baseline --steps 1000`
3. **Debugging**: `nlm-debug validation --type all --verbose`
4. **Production**: Automated monitoring and checkpointing

## Support and Troubleshooting

For help with any CLI tool, use the `help` command:

```bash
nlm-cli help
nlm-config help
nlm-monitor help
nlm-profile help
nlm-checkpoint help
nlm-debug help
```

For specific issues, check the documentation for the specific tool and use the appropriate debugging commands.

## Version Information

This documentation corresponds to NLM version 0.1.0 with Phase 2 Real Neural Computation capabilities and performance optimization infrastructure.

The CLI tools are designed to work with the existing NLM Python bindings (`pynlm`) for Python-based workflows, while providing native C++ command-line interfaces for maximum performance.

---

**Note**: This documentation is a comprehensive guide. For specific help with any command or feature, use the `help` command with the appropriate tool, or consult the tool's specific documentation file.

The NLM CLI tools are designed to provide a unified interface for all aspects of neural simulation, from simple command-line execution to comprehensive performance optimization and monitoring.
