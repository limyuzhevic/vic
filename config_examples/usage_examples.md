#!/usr/bin/env bash
# Configuration file examples for NLM

# configs/basic.cfg - Basic configuration
cat > configs/basic.cfg << 'EOF'
# Basic NLM Configuration File
# This configuration provides minimal settings for neural simulations

# Simulation parameters
neuron_count = 500
max_steps = 1000
timestep = 0.001

# Network parameters
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015

# Output configuration
output_format = "text"
output_file = "results.txt"

# Logging configuration
log_level = "info"
log_file = "nlm.log"
enable_logging = true

# Performance settings
enable_profiling = false
enable_benchmarking = false
parallel_threads = 1
memory_limit = 0  # 0 = unlimited

# Validation settings
enable_validation = false

EOF

# configs/advanced.cfg - Advanced configuration
cat > configs/advanced.cfg << 'EOF'
# Advanced NLM Configuration File
# This configuration provides optimized settings for high-performance neural simulations

# Simulation parameters
neuron_count = 2000
max_steps = 2000
timestep = 0.0005

# Network parameters
connection_probability = 0.2
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
stdp_tau = 20.0

# Plasticity parameters
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002
enable_plasticity = true

# Output configuration
output_format = "json"
output_file = "advanced_results.json"

# Logging configuration
log_level = "debug"
log_file = "advanced_nlm.log"
enable_logging = true

# Performance settings
enable_profiling = true
enable_benchmarking = true
parallel_threads = 4
memory_limit = 2048  # MB

# Validation settings
enable_validation = true
validation_depth = "deep"

# Plugin configuration
plugin_paths = "./plugins,./custom_plugins"
load_plugins = true
enable_plugin_hot_swapping = true

EOF

# configs/benchmark.cfg - Benchmark configuration
cat > configs/benchmark.cfg << 'EOF'
# Benchmark NLM Configuration File
# This configuration is optimized for performance benchmarking

# Simulation parameters
neuron_count = 1000
max_steps = 500
timestep = 0.001

# Network parameters (sparse connections for faster benchmarking)
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Output configuration (CSV format for easy analysis)
output_format = "csv"
output_file = "benchmark_results.csv"

# Logging configuration (minimal for benchmarking)
log_level = "warning"
log_file = "benchmark_nlm.log"
enable_logging = false

# Performance settings (maximum performance)
enable_profiling = true
enable_benchmarking = true
parallel_threads = 8
memory_limit = 4096  # MB

# Validation settings (lightweight for benchmarking)
enable_validation = false

EOF

# configs/development.cfg - Development configuration
cat > configs/development.cfg << 'EOF'
# Development NLM Configuration File
# This configuration is optimized for development and testing

# Simulation parameters (smaller for faster development)
neuron_count = 100
max_steps = 100
timestep = 0.01

# Network parameters
connection_probability = 0.3
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.018

# Output configuration (detailed for development)
output_format = "text"
output_file = "development_results.txt"

# Logging configuration (detailed for development)
log_level = "debug"
log_file = "development_nlm.log"
enable_logging = true

# Performance settings (conservative for development)
enable_profiling = true
enable_benchmarking = false
parallel_threads = 2
memory_limit = 1024  # MB

# Validation settings (comprehensive for development)
enable_validation = true
validation_depth = "comprehensive"

# Plugin configuration (all plugins enabled)
plugin_paths = "./dev_plugins,./test_plugins"
load_plugins = true
enable_plugin_hot_swapping = false

EOF

# configs/production.cfg - Production configuration
cat > configs/production.cfg << 'EOF'
# Production NLM Configuration File
# This configuration is optimized for production environments

# Simulation parameters (balanced performance)
neuron_count = 1500
max_steps = 1000
timestep = 0.001

# Network parameters
connection_probability = 0.18
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.019

# Output configuration (optimized for production)
output_format = "json"
output_file = "production_results.json"

# Logging configuration (optimized for production)
log_level = "warning"
log_file = "production_nlm.log"
enable_logging = true

# Performance settings (optimized for production)
enable_profiling = true
enable_benchmarking = false
parallel_threads = 6
memory_limit = 3072  # MB

# Validation settings (production-grade)
enable_validation = true
validation_depth = "production"

# Plugin configuration (stable plugins only)
plugin_paths = "./production_plugins"
load_plugins = true
enable_plugin_hot_swapping = false

EOF

# configs/custom_neural.cfg - Custom neural network configuration
cat > configs/custom_neural.cfg << 'EOF'
# Custom Neural Network Configuration
# This configuration demonstrates custom neural network parameters

# Simulation parameters
neuron_count = 3000
max_steps = 1500
timestep = 0.0002

# Network parameters (custom connection pattern)
connection_probability = 0.25
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.025

# Custom neural network model
neuron_model = "LIF_with_reinforcement"
plasticity_rule = "STDP_with_eligibility_traces"

# Output configuration
output_format = "json"
output_file = "custom_neural_results.json"

# Logging configuration
log_level = "info"
log_file = "custom_neural_nlm.log"
enable_logging = true

# Performance settings
enable_profiling = true
enable_benchmarking = true
parallel_threads = 8
memory_limit = 6144  # MB

# Validation settings
enable_validation = true
validation_custom = "neural_network_specific"

EOF

# configs/memory_intensive.cfg - Memory-intensive configuration
cat > configs/memory_intensive.cfg << 'EOF'
# Memory-Intensive NLM Configuration
# This configuration is optimized for memory-intensive simulations

# Simulation parameters (many neurons)
neuron_count = 5000
max_steps = 2000
timestep = 0.0001

# Network parameters (dense connections)
connection_probability = 0.4
stdp_ltp_weight = 0.035
stdp_ltd_weight = 0.032

# Memory optimization
enable_spike_compression = true
enable_synaptic_pruning = true
enable_memory_efficient_neural = true

# Output configuration (compact format)
output_format = "csv"
output_file = "memory_intensive_results.csv"

# Logging configuration (minimal to save memory)
log_level = "error"
log_file = "memory_intensive_nlm.log"
enable_logging = false

# Performance settings (memory-optimized)
enable_profiling = true
enable_benchmarking = true
parallel_threads = 4
memory_limit = 8192  # MB

# Validation settings
enable_validation = true
validation_focus = "memory_usage"

EOF

# configs/real_time.cfg - Real-time configuration
cat > configs/real_time.cfg << 'EOF'
# Real-Time NLM Configuration
# This configuration is optimized for real-time neural simulations

# Simulation parameters (lower latency)
neuron_count = 800
max_steps = 500
timestep = 0.0005

# Network parameters
connection_probability = 0.2
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.019

# Real-time optimization
enable_event_driven_processing = true
enable_low_latency_spikes = true
enable_real_time_validation = true

# Output configuration (real-time friendly)
output_format = "text"
output_file = "real_time_results.txt"

# Logging configuration (real-time friendly)
log_level = "info"
log_file = "real_time_nlm.log"
enable_logging = true

# Performance settings (real-time optimized)
enable_profiling = true
enable_benchmarking = true
parallel_threads = 4
memory_limit = 2048  # MB
enable_soft_realtime = true

# Validation settings
enable_validation = true
validation_real_time = true

EOF

# configs/plugin_develop.cfg - Plugin development configuration
cat > configs/plugin_develop.cfg << 'EOF'
# Plugin Development NLM Configuration
# This configuration is optimized for plugin development

# Simulation parameters (smaller for faster development)
neuron_count = 200
max_steps = 100
timestep = 0.01

# Network parameters
connection_probability = 0.15
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Plugin development settings
plugin_paths = "./dev_plugins,./test_plugins"
load_plugins = true
enable_plugin_hot_swapping = true
plugin_validation = true
plugin_debug_mode = true

# Output configuration
output_format = "text"
output_file = "plugin_dev_results.txt"

# Logging configuration (detailed for plugin development)
log_level = "debug"
log_file = "plugin_dev_nlm.log"
enable_logging = true

# Performance settings
enable_profiling = true
enable_benchmarking = false
parallel_threads = 2
memory_limit = 512  # MB

# Validation settings
enable_validation = true
validation_plugin_compatibility = true

EOF

# Create example plugin directory structure
cat > create_plugin_structure.sh << 'EOF'
#!/bin/bash

# Create basic plugin directory structure
mkdir -p plugins/neural_plugin/plugins/memory_plugin/plugins/visualization_plugin

# Create neural plugin example
cat > plugins/neural_plugin/README.md << 'INNER_EOF'
# Neural Plugin Example

This is an example neural plugin for NLM.

## Plugin Structure

- `include/neural_plugin/` - Plugin headers
- `src/` - Plugin source code
- `tests/` - Plugin tests
- `docs/` - Plugin documentation

## Example Implementation

See `plugins/neural_plugin/src/` for example implementations.
INNER_EOF

# Create memory plugin example
cat > plugins/memory_plugin/README.md << 'INNER_EOF'
# Memory Plugin Example

This is an example memory plugin for NLM.

## Plugin Features

- Memory-efficient neural representations
- Spike compression and decompression
- Memory management for large networks

## Example Implementation

See `plugins/memory_plugin/src/` for example implementations.
INNER_EOF

# Create visualization plugin example
cat > plugins/visualization_plugin/README.md << 'INNER_EOF'
# Visualization Plugin Example

This is an example visualization plugin for NLM.

## Plugin Features

- Neural network visualization
- Spike train visualization
- Network topology visualization

## Example Implementation

See `plugins/visualization_plugin/src/` for example implementations.
INNER_EOF

echo "Plugin structure created successfully!"
echo "See each plugin directory for examples and documentation."
EOF

chmod +x create_plugin_structure.sh

# Create example configuration summary
cat > config_examples_summary.md << 'EOF'
# NLM Configuration Examples Summary

This document summarizes the available configuration examples in the NLM project.

## Available Configuration Files

### Basic Configurations

1. **`configs/basic.cfg`** - Basic configuration for simple simulations
   - Neuron count: 500
   - Max steps: 1000
   - Format: Text
   - Logging: Info level

2. **`configs/development.cfg`** - Development configuration for testing
   - Neuron count: 100
   - Max steps: 100
   - Format: Text
   - Logging: Debug level

### Performance Configurations

3. **`configs/benchmark.cfg`** - Benchmark configuration for performance testing
   - Neuron count: 1000
   - Max steps: 500
   - Format: CSV
   - Logging: Warning level

4. **`configs/production.cfg`** - Production configuration for production environments
   - Neuron count: 1500
   - Max steps: 1000
   - Format: JSON
   - Logging: Warning level

### Advanced Configurations

5. **`configs/advanced.cfg`** - Advanced configuration for high-performance simulations
   - Neuron count: 2000
   - Max steps: 2000
   - Format: JSON
   - Logging: Debug level
   - Plugins: Enabled

6. **`configs/custom_neural.cfg`** - Custom neural network configuration
   - Neuron count: 3000
   - Max steps: 1500
   - Format: JSON
   - Custom neural models

7. **`configs/memory_intensive.cfg`** - Memory-intensive configuration
   - Neuron count: 5000
   - Max steps: 2000
   - Format: CSV
   - Memory optimization enabled

8. **`configs/real_time.cfg`** - Real-time configuration
   - Neuron count: 800
   - Max steps: 500
   - Format: Text
   - Real-time processing enabled

9. **`configs/plugin_develop.cfg`** - Plugin development configuration
   - Neuron count: 200
   - Max steps: 100
   - Format: Text
   - Plugin development tools enabled

## Configuration Usage

### Basic Usage

```bash
# Use basic configuration
nlm simulation --config-file configs/basic.cfg

# Use development configuration
nlm simulation --config-file configs/development.cfg
```

### Advanced Usage

```bash
# Use advanced configuration with custom settings
nlm simulation \
  --config-file configs/advanced.cfg \
  --output-file advanced_results.json \
  --enable-profiling \
  --enable-validation

# Use benchmark configuration for performance testing
nlm benchmark --config-file configs/benchmark.cfg --output-file benchmark_results.csv
```

### Plugin Configuration

```bash
# Use plugin development configuration
nlm simulation --config-file configs/plugin_develop.cfg --plugin-path ./dev_plugins

# List all plugins with development configuration
nlm plugin --list --config-file configs/plugin_develop.cfg
```

## Configuration Management

### Creating Custom Configurations

To create a custom configuration, copy one of the examples and modify it:

```bash
cp configs/basic.cfg configs/custom.cfg
# Edit custom.cfg with your desired settings
nlm simulation --config-file configs/custom.cfg
```

### Configuration Validation

Validate your configuration before use:

```bash
nlm config --config-file configs/custom.cfg --validate
```

### Configuration Examples Script

Run the configuration examples script to see all examples:

```bash
cat create_plugin_structure.sh
chmod +x create_plugin_structure.sh
./create_plugin_structure.sh
```

## Configuration File Format

All configuration files are in INI format:

```ini
# Configuration parameter
parameter_name = value

# Example
neuron_count = 500
max_steps = 1000
output_format = "json"
```

## Configuration Search Path

NLM searches for configuration files in this order:

1. Command line arguments
2. Environment variables
3. Current directory
4. `configs/` directory

## Environment Variables

You can also set configuration values using environment variables:

```bash
export NLM_NEURON_COUNT=1000
export NLM_MAX_STEPS=500
export NLM_OUTPUT_FORMAT=json
nlm simulation
```

## Configuration Examples Summary

This document provides an overview of all available configuration examples. Each configuration file is optimized for specific use cases:

- **Basic**: Simple, fast simulations
- **Development**: Testing and development
- **Benchmark**: Performance testing
- **Production**: Production environments
- **Advanced**: High-performance simulations
- **Custom Neural**: Custom neural network models
- **Memory Intensive**: Memory-optimized simulations
- **Real-Time**: Real-time processing
- **Plugin Development**: Plugin development and testing

Each configuration file includes appropriate settings for logging, performance, validation, and plugins.
EOF

# Create usage examples
cat > usage_examples.md << 'EOF'
# NLM Usage Examples

This document provides examples of how to use NLM with different configuration files.

## Basic Usage Examples

### Basic Configuration

```bash
# Run a simulation with basic configuration
nlm simulation --config-file configs/basic.cfg

# Run a simulation with basic configuration and custom output
nlm simulation --config-file configs/basic.cfg --output-file my_results.txt

# Run a simulation with basic configuration and JSON output
nlm simulation --config-file configs/basic.cfg --output-format json --output-file results.json
```

### Development Configuration

```bash
# Run a simulation with development configuration
nlm simulation --config-file configs/development.cfg

# Run a simulation with development configuration and profiling
nlm simulation --config-file configs/development.cfg --enable-profiling

# Run a simulation with development configuration and validation
nlm simulation --config-file configs/development.cfg --enable-validation
```

### Benchmark Configuration

```bash
# Run a benchmark with benchmark configuration
nlm benchmark --config-file configs/benchmark.cfg

# Run a benchmark with benchmark configuration and detailed output
nlm benchmark --config-file configs/benchmark.cfg --output-file detailed_benchmark.csv

# Run a benchmark with benchmark configuration and profiling
nlm benchmark --config-file configs/benchmark.cfg --enable-profiling
```

### Production Configuration

```bash
# Run a simulation with production configuration
nlm simulation --config-file configs/production.cfg

# Run a simulation with production configuration and custom output
nlm simulation --config-file configs/production.cfg --output-file production_results.json

# Run a simulation with production configuration and plugins
nlm simulation --config-file configs/production.cfg --plugin-path ./production_plugins
```

### Advanced Configuration

```bash
# Run a simulation with advanced configuration
nlm simulation --config-file configs/advanced.cfg

# Run a simulation with advanced configuration and performance settings
nlm simulation --config-file configs/advanced.cfg --parallel 4 --memory-limit 2048

# Run a simulation with advanced configuration and plugins
nlm simulation --config-file configs/advanced.cfg --plugin-path ./plugins --enable-plugin-hot-swapping

# Run a simulation with advanced configuration and validation
nlm simulation --config-file configs/advanced.cfg --enable-validation --validation-depth deep
```

### Custom Neural Configuration

```bash
# Run a simulation with custom neural configuration
nlm simulation --config-file configs/custom_neural.cfg

# Run a simulation with custom neural configuration and performance
nlm simulation --config-file configs/custom_neural.cfg --enable-profiling --enable-benchmarking

# Run a simulation with custom neural configuration and validation
nlm simulation --config-file configs/custom_neural.cfg --enable-validation --validation-custom neural_network_specific
```

### Memory-Intensive Configuration

```bash
# Run a simulation with memory-intensive configuration
nlm simulation --config-file configs/memory_intensive.cfg

# Run a simulation with memory-intensive configuration and compression
nlm simulation --config-file configs/memory_intensive.cfg --enable-spike-compression

# Run a simulation with memory-intensive configuration and pruning
nlm simulation --config-file configs/memory_intensive.cfg --enable-synaptic-pruning
```

### Real-Time Configuration

```bash
# Run a simulation with real-time configuration
nlm simulation --config-file configs/real_time.cfg

# Run a simulation with real-time configuration and event-driven processing
nlm simulation --config-file configs/real_time.cfg --enable-event-driven-processing

# Run a simulation with real-time configuration and low latency
nlm simulation --config-file configs/real_time.cfg --enable-low-latency-spikes

# Run a simulation with real-time configuration and real-time validation
nlm simulation --config-file configs/real_time.cfg --enable-real-time-validation
```

### Plugin Development Configuration

```bash
# Run a simulation with plugin development configuration
nlm simulation --config-file configs/plugin_develop.cfg

# Run a simulation with plugin development configuration and plugins
nlm simulation --config-file configs/plugin_develop.cfg --plugin-path ./dev_plugins

# Run a simulation with plugin development configuration and validation
nlm simulation --config-file configs/plugin_develop.cfg --enable-validation --validation-plugin-compatibility
```

## Configuration Management Examples

### Configuration File Validation

```bash
# Validate basic configuration
nlm config --config-file configs/basic.cfg --validate

# Validate development configuration
nlm config --config-file configs/development.cfg --validate

# Validate benchmark configuration
nlm config --config-file configs/benchmark.cfg --validate

# Validate production configuration
nlm config --config-file configs/production.cfg --validate

# Validate advanced configuration
nlm config --config-file configs/advanced.cfg --validate
```

### Configuration File Export

```bash
# Export basic configuration
nlm config --config-file configs/basic.cfg --output-file exported_basic.cfg

# Export development configuration
nlm config --config-file configs/development.cfg --output-file exported_development.cfg

# Export benchmark configuration
nlm config --config-file configs/benchmark.cfg --output-file exported_benchmark.cfg
```

### Configuration File Listing

```bash
# List current configuration
nlm config --list

# List configuration with basic configuration
nlm config --config-file configs/basic.cfg --list

# List configuration with development configuration
nlm config --config-file configs/development.cfg --list
```

## Environment Variable Examples

### Basic Environment Variables

```bash
# Set basic configuration via environment variables
export NLM_NEURON_COUNT=500
export NLM_MAX_STEPS=1000
export NLM_OUTPUT_FORMAT=text
export NLM_LOG_LEVEL=info

# Run simulation with environment variables
nlm simulation
```

### Development Environment Variables

```bash
# Set development configuration via environment variables
export NLM_NEURON_COUNT=100
export NLM_MAX_STEPS=100
export NLM_OUTPUT_FORMAT=text
export NLM_LOG_LEVEL=debug
export NLM_ENABLE_PROFILING=true

# Run simulation with environment variables
nlm simulation --enable-validation
```

### Performance Environment Variables

```bash
# Set performance configuration via environment variables
export NLM_NEURON_COUNT=1000
export NLM_MAX_STEPS=500
export NLM_OUTPUT_FORMAT=csv
export NLM_LOG_LEVEL=warning
export NLM_ENABLE_PROFILING=true
export NLM_ENABLE_BENCHMARKING=true
export NLM_PARALLEL_THREADS=8
export NLM_MEMORY_LIMIT=4096

# Run benchmark with environment variables
nlm benchmark
```

## Batch Processing Examples

### Batch Processing with Basic Configuration

```bash
#!/bin/bash
# Batch processing with basic configuration

echo "Running batch processing with basic configuration"
echo "============================================"

# Run multiple simulations
for i in {1..5}; do
    echo "Running simulation $i"
    nlm simulation --config-file configs/basic.cfg --output-file results_${i}.txt
    echo "Simulation $i completed"
done

echo "Batch processing completed"
```

### Batch Processing with Advanced Configuration

```bash
#!/bin/bash
# Batch processing with advanced configuration

echo "Running batch processing with advanced configuration"
echo "=============================================="

# Run multiple simulations with different parameters
for neurons in 500 1000 1500; do
    for steps in 500 1000 1500; do
        echo "Running simulation with ${neurons} neurons and ${steps} steps"
        nlm simulation \
          --config-file configs/advanced.cfg \
          --neuron-count ${neurons} \
          --max-steps ${steps} \
          --output-file results_${neurons}_${steps}.json
        echo "Simulation completed"
    done
done

echo "Batch processing completed"
```

### Benchmark Suite Example

```bash
#!/bin/bash
# Benchmark suite example

echo "Running benchmark suite"
echo "======================="

# Run benchmark with different configurations
benchmarks=(
    "configs/benchmark.cfg --neuron-count 1000 --steps 500"
    "configs/benchmark.cfg --neuron-count 2000 --steps 1000"
    "configs/benchmark.cfg --neuron-count 5000 --steps 2000"
)

for benchmark in "${benchmarks[@]}"; do
    read config cmd <<<"$benchmark"
    echo "Running benchmark with ${cmd}"
    eval nlm benchmark ${cmd} --output-file "benchmark_$(echo ${cmd} | sed 's/.*--neuron-count //;s/ .*//').csv"
    echo "Benchmark completed"
done

echo "Benchmark suite completed"
```

## Performance Optimization Examples

### Performance Optimization with Parallel Processing

```bash
# Run simulation with parallel processing
nlm simulation \
  --config-file configs/production.cfg \
  --parallel 4 \
  --memory-limit 2048

# Run simulation with maximum performance
nlm simulation \
  --config-file configs/advanced.cfg \
  --parallel 8 \
  --memory-limit 4096 \
  --enable-profiling \
  --enable-validation
```

### Performance Optimization with Memory Management

```bash
# Run memory-intensive simulation with compression
nlm simulation \
  --config-file configs/memory_intensive.cfg \
  --enable-spike-compression \
  --enable-synaptic-pruning

# Run real-time simulation with memory optimization
nlm simulation \
  --config-file configs/real_time.cfg \
  --enable-low-latency-spikes \
  --enable-event-driven-processing
```

### Performance Optimization with Plugins

```bash
# Run simulation with plugins for performance
nlm simulation \
  --config-file configs/advanced.cfg \
  --plugin-path ./plugins \
  --enable-plugin-hot-swapping

# Run benchmark with plugins
nlm benchmark \
  --config-file configs/benchmark.cfg \
  --plugin-path ./performance_plugins
```

## Error Handling Examples

### Configuration Error Handling

```bash
#!/bin/bash
# Error handling for configuration files

# Check if configuration file exists
if [ ! -f "configs/basic.cfg" ]; then
    echo "Error: Configuration file not found"
    exit 1
fi

# Validate configuration file
if ! nlm config --config-file configs/basic.cfg --validate; then
    echo "Error: Configuration file is invalid"
    exit 1
fi

# Run simulation with error handling
echo "Running simulation..."
if nlm simulation --config-file configs/basic.cfg; then
    echo "Simulation completed successfully"
else
    echo "Error: Simulation failed"
    exit 1
fi
```

### Command Error Handling

```bash
#!/bin/bash
# Error handling for commands

# Run command with error handling
echo "Running simulation..."
if nlm simulation --neuron-count 1000 --max-steps 500; then
    echo "Simulation completed successfully"
else
    echo "Error: Simulation failed"
    echo "Check logs for details"
    exit 1
fi

# Run command with custom error handling
echo "Running benchmark..."
if nlm benchmark --neuron-count 1000 --steps 500; then
    echo "Benchmark completed successfully"
else
    echo "Error: Benchmark failed"
    echo "Retrying with smaller configuration..."
    nlm benchmark --neuron-count 500 --steps 250
fi
```

## Best Practices Examples

### Best Practices for Configuration Files

```bash
# Use descriptive filenames for configuration files
cp configs/basic.cfg configs/simple_simulation.cfg
cp configs/development.cfg configs/debugging_simulation.cfg

# Create configuration templates
cat > config_template.cfg << 'EOF'
neuron_count = 500
max_steps = 1000
timestep = 0.001
output_format = "text"
log_level = "info"
EOF

# Use configuration templates
nlm simulation --config-file config_template.cfg
```

### Best Practices for Scripting

```bash
#!/bin/bash
# Script for running multiple simulations with error handling

# Create results directory
mkdir -p results

# Run simulations with error handling
for i in {1..10}; do
    echo "Running simulation $i"
    if nlm simulation --neuron-count 500 --max-steps 500 --output-file "results/simulation_${i}.txt"; then
        echo "Simulation $i completed successfully"
    else
        echo "Simulation $i failed"
        # Continue with next simulation
    fi
done

echo "Batch processing completed"
```

### Best Practices for Performance

```bash
#!/bin/bash
# Script for optimized performance

# Set up environment variables
export NLM_PARALLEL_THREADS=4
export NLM_MEMORY_LIMIT=2048
export NLM_OUTPUT_FORMAT=json

# Run simulation with optimized settings
nlm simulation --config-file configs/production.cfg --enable-profiling

# Monitor performance
nlm profile --benchmark --output-file perf.json

# Analyze results
nlm analyze --type performance --output-format json > analysis.json
```

## Conclusion

These examples demonstrate various ways to use NLM with different configuration files. By understanding and using these configurations, you can optimize your neural simulation workflows for different use cases:

- **Basic**: Simple, fast simulations
- **Development**: Testing and development
- **Benchmark**: Performance testing
- **Production**: Production environments
- **Advanced**: High-performance simulations
- **Custom Neural**: Custom neural network models
- **Memory Intensive**: Memory-optimized simulations
- **Real-Time**: Real-time processing
- **Plugin Development**: Plugin development and testing

Each configuration file is designed to meet specific requirements for performance, logging, validation, and plugins. Use the appropriate configuration for your use case to achieve optimal results.
