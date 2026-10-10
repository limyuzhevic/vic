# NLM Command-Line Interface (CLI) Documentation

## Overview

The NLM Command-Line Interface (CLI) provides a comprehensive suite of tools for managing neural simulations, debugging, profiling, and system administration. The CLI is designed to be both powerful and user-friendly, offering granular control over simulation parameters while maintaining an intuitive interface for both beginners and advanced users.

## Command Structure

The NLM CLI follows a hierarchical structure with the following main command groups:

| Command Group | Description | Key Commands |
|---------------|-------------|--------------|
| `nlm` | Core simulation commands | `simulation`, `run`, `execute` |
| `nlm config` | Configuration management | `list`, `set`, `get`, `validate` |
| `nlm debug` | Debugging tools | `attach`, `profile`, `validate` |
| `nlm plugin` | Plugin management | `list`, `load`, `unload`, `info` |
| `nlm benchmark` | Performance benchmarking | `run`, `compare`, `analyze` |
| `nlm profile` | Performance profiling | `start`, `stop`, `report` |
| `nlm analyze` | System analysis | `performance`, `memory`, `network` |
| `nlm help` | Command help and documentation | `help`, `--help`, `--list` |

## Core Simulation Commands

### `nlm simulation`

**Description**: Run neural simulations with various configurations and options.

**Usage**:
```bash
nlm simulation [OPTIONS] [--] [ARGS...]
```

**Options**:

| Option | Short | Type | Default | Description |
|--------|--------|------|---------|-------------|
| `--neuron-count` | `-n` | int | 1000 | Number of neurons in simulation |
| `--max-steps` | `-s` | int | 1000 | Maximum simulation steps |
| `--timestep` | `-t` | float | 0.001 | Simulation time step (ms) |
| `--config` | `-c` | string | - | Configuration file path |
| `--output-file` | `-o` | string | - | Output file path |
| `--output-format` | `-f` | enum | text | Output format (text, json, csv, binary) |
| `--enable-profiling` | - | flag | false | Enable performance profiling |
| `--enable-validation` | - | flag | false | Enable system validation |
| `--parallel` | `-p` | int | 1 | Number of parallel threads |
| `--random-seed` | `-r` | int | -1 | Random seed for reproducibility |
| `--log-level` | - | enum | info | Logging level (trace, debug, info, warn, error) |

**Examples**:

```bash
# Basic simulation
nlm simulation --neuron-count 500 --max-steps 200

# Simulation with JSON output
nlm simulation -n 1000 -s 500 -o results.json -f json

# Advanced simulation with profiling
nlm simulation --neuron-count 2000 --max-steps 1000 \
    --enable-profiling --enable-validation --parallel 4

# Simulation with custom configuration
nlm simulation --config myconfig.cfg --neuron-count 1500
```

### `nlm run`

**Description**: Run a specific simulation from configuration file.

**Usage**:
```bash
nlm run CONFIG_FILE [OPTIONS]
```

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--step` | int | 0 | Starting step |
| `--resume` | flag | false | Resume from saved state |
| `--validate` | flag | false | Validate configuration before running |

**Examples**:

```bash
# Run simulation from configuration
nlm run simulation.cfg --step 0

# Resume from saved state
nlm run simulation.cfg --resume

# Run with validation
nlm run simulation.cfg --validate
```

### `nlm execute`

**Description**: Execute multiple simulations with different parameters.

**Usage**:
```bash
nlm execute PARAMS_FILE [OPTIONS]
```

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--jobs` | int | 1 | Number of parallel jobs |
| `--output-dir` | string | results/ | Output directory for results |
| `--continue-on-error` | flag | false | Continue if a simulation fails |

**Examples**:

```bash
# Run parameter sweep
nlm execute param_sweep.json --jobs 4

# Run batch of simulations
nlm execute batch.cfg --output-dir batch_results --continue-on-error
```

## Configuration Management Commands

### `nlm config`

**Description**: Manage NLM configuration files and settings.

**Usage**:
```bash
nlm config <COMMAND> [OPTIONS]
```

#### `nlm config list`

**Description**: List all configuration parameters.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--all` | flag | false | Show all parameters including defaults |
| `--category` | string | - | Filter by category (brain, plasticity, neuromod) |

**Examples**:

```bash
# List all configuration parameters
nlm config list

# List only brain parameters
nlm config list --category brain

# List with defaults
nlm config list --all
```

#### `nlm config set`

**Description**: Set configuration parameter.

**Usage**:
```bash
nlm config set KEY VALUE [OPTIONS]
```

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--file` | string | - | Configuration file path |
| `--type` | string | auto | Value type (int, float, string, bool) |

**Examples**:

```bash
# Set parameter in memory
nlm config set brain.neuron_count 2000

# Set parameter in file
nlm config set plasticity.stdp.learning_rate 0.002 --file myconfig.cfg

# Set boolean parameter
nlm config set neuromod.curiosity.enable true --type bool
```

#### `nlm config get`

**Description**: Get configuration parameter value.

**Usage**:
```bash
nlm config get KEY [OPTIONS]
```

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--file` | string | - | Configuration file path |

**Examples**:

```bash
# Get parameter from memory
nlm config get brain.neuron_count

# Get parameter from file
nlm config get brain.synapse_density --file myconfig.cfg
```

#### `nlm config validate`

**Description**: Validate configuration file.

**Usage**:
```bash
nlm config validate FILE [OPTIONS]
```

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--strict` | flag | false | Strict validation mode |

**Examples**:

```bash
# Validate configuration file
nlm config validate myconfig.cfg

# Strict validation
nlm config validate myconfig.cfg --strict
```

#### `nlm config export`

**Description**: Export configuration to different formats.

**Usage**:
```bash
nlm config export FILE [OPTIONS]
```

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--format` | enum | json | Output format (json, yaml, ini, xml) |
| `--include-defaults` | flag | false | Include default values |
| `--template` | flag | false | Output template only |

**Examples**:

```bash
# Export configuration as JSON
nlm config export myconfig.cfg --format json

# Export configuration as YAML with defaults
nlm config export myconfig.cfg --format yaml --include-defaults

# Generate configuration template
nlm config export template.cfg --template
```

## Debugging Commands

### `nlm debug`

**Description**: Debug neural simulation and system components.

**Usage**:
```bash
nlm debug <COMMAND> [OPTIONS]
```

#### `nlm debug attach`

**Description**: Attach debugger to running simulation.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--pid` | int | -1 | Process ID (auto-detect if not specified) |
| `--port` | int | 2345 | Debugger port |
| `--timeout` | int | 30 | Connection timeout (seconds) |

**Examples**:

```bash
# Attach debugger to running process
nlm debug attach

# Attach to specific process with custom port
nlm debug attach --pid 12345 --port 8080
```

#### `nlm debug profile`

**Description**: Profile simulation performance.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--output-file` | string | profile.json | Profile output file |
| `--format` | enum | json | Profile format (json, txt, html) |
| `--duration` | int | 60 | Profile duration (seconds) |
| `--sampling-rate` | int | 100 | Sampling rate (Hz) |

**Examples**:

```bash
# Start performance profiling
nlm debug profile

# Profile with custom settings
nlm debug profile --output-file perf_report.json \
    --format json --duration 120 --sampling-rate 50
```

#### `nlm debug validate`

**Description**: Validate system state and configuration.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--memory` | flag | false | Check memory usage |
| `--threads` | flag | false | Check thread usage |
| `--config` | flag | false | Validate configuration |
| `--plugins` | flag | false | Validate plugins |
| `--performance` | flag | false | Check performance metrics |

**Examples**:

```bash
# Validate all system components
nlm debug validate --memory --threads --config --plugins --performance

# Quick validation
nlm debug validate --config --plugins
```

#### `nlm debug analyze`

**Description**: Analyze debug information.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--type` | enum | all | Analysis type (performance, memory, network) |
| `--output-file` | string | analysis.txt | Output file path |

**Examples**:

```bash
# Analyze performance
nlm debug analyze --type performance

# Analyze memory usage
nlm debug analyze --type memory --output-file memory_analysis.txt
```

## Plugin Management Commands

### `nlm plugin`

**Description**: Manage NLM plugins for extending functionality.

**Usage**:
```bash
nlm plugin <COMMAND> [OPTIONS]
```

#### `nlm plugin list`

**Description**: List all installed plugins.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--category` | enum | all | Filter by category (neuron, synapse, memory, cognition, neuromodulation, visualization) |
| `--enabled-only` | flag | false | Show only enabled plugins |
| `--details` | flag | false | Show detailed plugin information |

**Examples**:

```bash
# List all plugins
nlm plugin list

# List only neuron plugins
nlm plugin list --category neuron

# List enabled plugins only
nlm plugin list --enabled-only

# List plugins with details
nlm plugin list --details
```

#### `nlm plugin load`

**Description**: Load plugin from file.

**Usage**:
```bash
nlm plugin load PLUGIN_PATH [OPTIONS]
```

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--name` | string | auto | Custom plugin name |
| `--category` | enum | auto | Plugin category |
| `--init-file` | string | - | Initialization file path |

**Examples**:

```bash
# Load plugin from file
nlm plugin load /path/to/plugin.so

# Load with custom name
nlm plugin load ./myplugin.so --name CustomPlugin

# Load with initialization file
nlm plugin load ./neural_plugin.so --init-file config.json
```

#### `nlm plugin unload`

**Description**: Unload plugin from memory.

**Usage**:
```bash
nlm plugin unload PLUGIN_NAME [OPTIONS]
```

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--force` | flag | false | Force unload even if plugin is in use |
| `--cleanup` | flag | false | Cleanup plugin resources after unloading |

**Examples**:

```bash
# Unload plugin
nlm plugin unload MyPlugin

# Force unload
nlm plugin unload MyPlugin --force

# Cleanup after unload
nlm plugin unload MyPlugin --cleanup
```

#### `nlm plugin info`

**Description**: Get detailed information about plugin.

**Usage**:
```bash
nlm plugin info PLUGIN_NAME [OPTIONS]
```

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--verbose` | flag | false | Show verbose information |
| `--dependencies` | flag | false | Show plugin dependencies |
| `--config-schema` | flag | false | Show configuration schema |

**Examples**:

```bash
# Get plugin information
nlm plugin info MyPlugin

# Get detailed plugin information
nlm plugin info MyPlugin --verbose

# Get dependencies
nlm plugin info MyPlugin --dependencies

# Get configuration schema
nlm plugin info MyPlugin --config-schema
```

#### `nlm plugin install`

**Description**: Install plugin from repository or package.

**Usage**:
```bash
nlm plugin install PACKAGE_SPEC [OPTIONS]
```

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--repository` | string | https://plugins.nlm-project.org | Repository URL |
| `--version` | string | latest | Plugin version |
| `--force` | flag | false | Force installation even if plugin exists |

**Examples**:

```bash
# Install from repository
nlm plugin install my-plugin

# Install specific version
nlm plugin install my-plugin --version 1.2.3

# Install from custom repository
nlm plugin install my-plugin --repository https://custom-plugins.org
```

## Benchmarking Commands

### `nlm benchmark`

**Description**: Run performance benchmarks for NLM systems.

**Usage**:
```bash
nlm benchmark <COMMAND> [OPTIONS]
```

#### `nlm benchmark run`

**Description**: Run benchmark suite.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--test` | string | all | Test category (all, neuron, synapse, memory, cognition, plasticity) |
| `--neuron-count` | int | 1000 | Number of neurons for benchmarks |
| `--steps` | int | 1000 | Number of simulation steps |
| `--output-file` | string | benchmark_results.csv | Output file path |
| `--format` | enum | csv | Output format (csv, json, yaml) |
| `--warmup` | int | 100 | Warmup iterations |
| `--iterations` | int | 10 | Benchmark iterations |

**Examples**:

```bash
# Run all benchmarks
nlm benchmark run

# Run neuron benchmarks
nlm benchmark run --test neuron --neuron-count 2000 --iterations 5

# Run synapse benchmarks
nlm benchmark run --test synapse --steps 500 --output-file synapse_benchmarks.json
```

#### `nlm benchmark compare`

**Description**: Compare benchmark results.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--baseline` | string | baseline.json | Baseline results file |
| `--current` | string | current.json | Current results file |
| `--metric` | string | all | Metric to compare (all, time, memory, accuracy) |
| `--threshold` | float | 0.05 | Performance improvement threshold |

**Examples**:

```bash
# Compare with baseline
nlm benchmark compare --baseline baseline.json --current current.json

# Compare specific metrics
nlm benchmark compare --baseline old.json --current new.json \
    --metric performance --threshold 0.1
```

#### `nlm benchmark analyze`

**Description**: Analyze benchmark results.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--file` | string | benchmark_results.csv | Results file path |
| `--metric` | string | all | Analysis metric (time, memory, cpu, accuracy) |
| `--aggregation` | string | mean | Aggregation method (mean, median, min, max) |
| `--visualization` | flag | false | Generate visualizations |

**Examples**:

```bash
# Analyze benchmark results
nlm benchmark analyze --file results.csv --metric performance

# Analyze with visualization
nlm benchmark analyze --file results.json --metric memory \
    --aggregation median --visualization
```

## Profiling Commands

### `nlm profile`

**Description**: Profile NLM performance and identify bottlenecks.

**Usage**:
```bash
nlm profile <COMMAND> [OPTIONS]
```

#### `nlm profile start`

**Description**: Start performance profiling.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--duration` | int | 300 | Profile duration (seconds) |
| `--output-file` | string | profile.pstats | Output file path |
| `--format` | enum | pstats | Profile format (pstats, json, txt) |
| `--sampling-rate` | int | 1000 | Sampling rate (Hz) |
| `--cpu-sampling` | flag | false | Enable CPU profiling |
| `--memory-sampling` | flag | false | Enable memory profiling |

**Examples**:

```bash
# Start profiling for 5 minutes
nlm profile start

# Profile with custom settings
nlm profile start --duration 120 --output-file perf_data.pstats \
    --format json --sampling-rate 500
```

#### `nlm profile stop`

**Description**: Stop active profiling session.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--analyze` | flag | false | Analyze profile immediately |
| `--export` | flag | false | Export profile to multiple formats |

**Examples**:

```bash
# Stop profiling
nlm profile stop

# Stop and analyze
nlm profile stop --analyze

# Stop and export
nlm profile stop --export
```

#### `nlm profile report`

**Description**: Generate profiling report.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--file` | string | profile.pstats | Profile file path |
| `--type` | enum | performance | Report type (performance, memory, cpu, gpu) |
| `--format` | enum | html | Report format (html, text, json) |
| `--threshold` | float | 0.01 | Time threshold for inclusion |
| `--top-n` | int | 20 | Number of top items to include |

**Examples**:

```bash
# Generate performance report
nlm profile report --file profile.pstats --type performance

# Generate memory report
nlm profile report --file memory_profile.pstats --type memory --format json
```

## Analysis Commands

### `nlm analyze`

**Description**: Analyze system performance and metrics.

**Usage**:
```bash
nlm analyze <TYPE> [OPTIONS]
```

#### `nlm analyze performance`

**Description**: Analyze performance metrics.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--output-file` | string | perf_analysis.txt | Output file path |
| `--format` | enum | text | Output format (text, json, yaml) |
| `--time-window` | int | 60 | Analysis time window (seconds) |
| `--aggregation` | string | mean | Aggregation method (mean, median, max) |
| `--metrics` | string | all | Metrics to analyze (all, cpu, memory, gpu, network) |

**Examples**:

```bash
# Analyze performance
nlm analyze performance --output-file perf_analysis.txt

# Analyze with JSON output
nlm analyze performance --format json --metrics cpu,memory,gpu
```

#### `nlm analyze memory`

**Description**: Analyze memory usage and patterns.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--output-file` | string | memory_analysis.csv | Output file path |
| `--format` | enum | csv | Output format (csv, json, txt) |
| `--allocation-size` | int | 1024 | Memory allocation size (bytes) |
| `--track-allocation` | flag | false | Track all memory allocations |

**Examples**:

```bash
# Analyze memory usage
nlm analyze memory --output-file memory_usage.csv

# Track all allocations
nlm analyze memory --track-allocation --format json
```

#### `nlm analyze network`

**Description**: Analyze network communication patterns.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--output-file` | string | network_analysis.txt | Output file path |
| `--protocol` | enum | tcp | Network protocol (tcp, udp, http) |
| `--port-range` | string | 1024-65535 | Port range for analysis |
| `--threshold` | float | 0.001 | Traffic threshold |

**Examples**:

```bash
# Analyze network traffic
nlm analyze network --output-file network_analysis.txt

# Analyze UDP traffic
nlm analyze network --protocol udp --port-range 8000-9000
```

## Help Commands

### `nlm help`

**Description**: Get help information about NLM commands and options.

**Usage**:
```bash
nlm help [COMMAND] [OPTIONS]
```

#### `nlm help --list`

**Description**: List all available commands and their categories.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--format` | enum | text | Output format (text, json, yaml) |
| `--detailed` | flag | false | Show detailed command information |

**Examples**:

```bash
# List all commands
nlm help --list

# List commands in JSON format
nlm help --list --format json

# List with detailed information
nlm help --list --detailed
```

#### `nlm help --search`

**Description**: Search for commands or topics.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--query` | string | - | Search query |
| `--category` | enum | all | Filter by category (simulation, config, debug, plugin, benchmark, profile, analyze) |
| `--format` | enum | text | Output format (text, json, yaml) |

**Examples**:

```bash
# Search for simulation commands
nlm help --search simulation --category simulation

# Search for debug commands
nlm help --search profile --category debug

# Search with JSON output
nlm help --search plugin --category plugin --format json
```

#### `nlm help --examples`

**Description**: Show usage examples for a command.

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--command` | string | - | Command name |
| `--count` | int | 5 | Number of examples to show |
| `--format` | enum | text | Output format (text, json) |

**Examples**:

```bash
# Show examples for simulation command
nlm help --examples --command simulation

# Show 3 examples for config command
nlm help --examples --command config --count 3

# Show examples in JSON format
nlm help --examples --command plugin --format json
```

## Interactive Help

### `nlm interactive-help`

**Description**: Start interactive help session with command completion and suggestions.

**Usage**:
```bash
nlm interactive-help [OPTIONS]
```

**Options**:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--mode` | enum | auto | Help mode (auto, command, tutorial, advanced) |
| `--completion` | flag | true | Enable command completion |
| `--suggestions` | flag | true | Enable command suggestions |
| `--history` | flag | true | Enable command history |

**Examples**:

```bash
# Start interactive help
nlm interactive-help

# Start with tutorial mode
nlm interactive-help --mode tutorial

# Start without suggestions
nlm interactive-help --suggestions false
```

## Command Completion

### `nlm completion`

**Description**: Generate shell completion scripts.

**Usage**:
```bash
nlm completion SHELL [OPTIONS]
```

**Supported Shells**:

- `bash` - Bash shell completion
- `zsh` - Z shell completion  
- `fish` - Fish shell completion
- `powershell` - PowerShell completion

**Examples**:

```bash
# Generate bash completion
nlm completion bash > nlm-completion.bash
source nlm-completion.bash

# Generate zsh completion
nlm completion zsh > ~/.zsh/completions/_nlm

# Generate fish completion
nlm completion fish > ~/.config/fish/completions/nlm.fish
```

## Command-line Examples

### Basic Simulation Commands

```bash
# Simple simulation
nlm simulation --neuron-count 1000 --max-steps 500

# Simulation with output
nlm simulation -n 2000 -s 1000 -o results.json -f json

# Advanced simulation with all options
nlm simulation --neuron-count 5000 --max-steps 2000 \
    --enable-profiling --enable-validation --parallel 4 \
    --random-seed 42 --log-level debug
```

### Configuration Management

```bash
# Set configuration parameter
nlm config set brain.neuron_count 3000
nlm config set plasticity.stdp.learning_rate 0.001

# Get configuration parameter
nlm config get brain.synapse_density

# Validate configuration
nlm config validate myconfig.cfg

# Export configuration
nlm config export myconfig.cfg --format yaml --include-defaults
```

### Plugin Management

```bash
# List plugins
nlm plugin list --category neuron --enabled-only

# Load plugin
nlm plugin load ./custom_neuron_plugin.so

# Get plugin information
nlm plugin info CustomPlugin --verbose --dependencies

# Uninstall plugin
nlm plugin unload CustomPlugin --cleanup
```

### Benchmarking

```bash
# Run benchmark suite
nlm benchmark run --test all --neuron-count 2000 --iterations 5

# Compare with baseline
nlm benchmark compare --baseline baseline.json --current current.json

# Analyze results
nlm benchmark analyze --file results.csv --metric performance --aggregation median
```

### Profiling

```bash
# Start profiling
nlm profile start --duration 120 --sampling-rate 500

# Stop and analyze
nlm profile stop --analyze

# Generate report
nlm profile report --file profile.pstats --type performance --format html
```

### Debugging

```bash
# Validate system
nlm debug validate --memory --threads --config --plugins

# Analyze system
nlm debug analyze --type performance --output-file analysis.txt

# Attach debugger
nlm debug attach --port 2345
```

### Help and Documentation

```bash
# List all commands
nlm help --list --detailed

# Search for commands
nlm help --search "simulation" --category simulation

# Get examples
nlm help --examples --command simulation --count 3

# Start interactive help
nlm interactive-help
```

## Command-line Tips and Best Practices

### 1. Common Patterns

#### Multiple Commands in One Session

```bash
# Set up environment
nlm config set brain.neuron_count 2000
nlm config set plasticity.stdp.learning_rate 0.001

# Run simulation
nlm simulation --neuron-count 2000 --max-steps 1000 --enable-profiling

# Analyze results
nlm debug validate --memory --config

# Generate report
nlm profile report --file profile.pstats
```

#### Parameter Sweeps

```bash
# Create parameter file
nlm benchmark run --test neuron --neuron-count 500 --iterations 3
nlm benchmark run --test neuron --neuron-count 1000 --iterations 3
nlm benchmark run --test neuron --neuron-count 2000 --iterations 3

# Compare results
nlm benchmark compare --baseline results.csv --current results2.csv
```

### 2. Error Handling

#### Graceful Error Recovery

```bash
# Run simulation with error recovery
nlm simulation --neuron-count 1000 --max-steps 500 || \
    (echo "Simulation failed, retrying..." && \
     nlm simulation --neuron-count 1000 --max-steps 500 --enable-validation)

# Use temporary configurations
nlm simulation --config temp.cfg
nlm config set brain.neuron_count 2000 --file temp.cfg
```

### 3. Performance Optimization

#### Parallel Processing

```bash
# Use multiple threads for better performance
nlm simulation --neuron-count 5000 --max-steps 2000 --parallel 4

# Combine with profiling for optimization
nlm simulation --neuron-count 5000 --max-steps 2000 \
    --enable-profiling --enable-validation --parallel 8
```

### 4. Configuration Management

#### Configuration Templates

```bash
# Create configuration template
nlm config export template.cfg --template

# Modify template for specific use cases
nlm config set brain.neuron_count 1000 --file simulation.cfg
nlm config set plasticity.stdp.learning_rate 0.001 --file simulation.cfg

# Use configuration in scripts
cat simulation.cfg | while read line; do
    nlm config set $line
    nlm simulation --neuron-count 1000 --max-steps 500
    # Process results...
done
```

### 5. Scripting and Automation

#### Bash Script Example

```bash
#!/bin/bash

# NLM Simulation Script
# Runs a series of simulations with different parameters

set -e

# Configuration
NEURON_COUNTS=(500 1000 2000 5000)
STEPS_VALUES=(100 500 1000 2000)

# Create output directory
mkdir -p results

# Run parameter sweep
for neurons in "${NEURON_COUNTS[@]}"; do
    for steps in "${STEPS_VALUES[@]}"; do
        echo "Running simulation: ${neurons} neurons, ${steps} steps"
        
        # Create unique output file
        output_file="results/simulation_${neurons}_${steps}.json"
        
        # Run simulation
        nlm simulation --neuron-count "$neurons" --max-steps "$steps" \
            --enable-profiling --output-file "$output_file" --output-format json || \
            echo "Simulation failed for ${neurons} neurons, ${steps} steps"
        
        # Small delay between simulations
        sleep 2
    done
done

echo "Parameter sweep completed. Results in results/ directory"
```

#### Python Script Example

```python
#!/usr/bin/env python3

import subprocess
import json
import os
import time
from pathlib import Path

class NLMSimulationRunner:
    def __init__(self, config_file=None):
        self.config_file = config_file
        self.results_dir = Path("simulation_results")
        self.results_dir.mkdir(exist_ok=True)
    
    def run_simulation(self, neuron_count=1000, max_steps=1000, 
                      enable_profiling=True, output_format="json"):
        """Run a single NLM simulation."""
        
        cmd = [
            "nlm", "simulation",
            "--neuron-count", str(neuron_count),
            "--max-steps", str(max_steps),
            "--output-file", f"{self.results_dir}/sim_{neuron_count}_{max_steps}.json",
            "--output-format", output_format
        ]
        
        if enable_profiling:
            cmd.append("--enable-profiling")
        
        print(f"Running: {' '.join(cmd)}")
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.returncode != 0:
            print(f"Error: {result.stderr}")
            return None
        
        return result.stdout
    
    def run_parameter_sweep(self, param_sets):
        """Run multiple simulations with different parameters."""
        
        results = []
        
        for i, params in enumerate(param_sets):
            print(f"Running parameter set {i+1}/{len(param_sets)}")
            
            neuron_count = params.get('neuron_count', 1000)
            max_steps = params.get('max_steps', 1000)
            
            result = self.run_simulation(
                neuron_count=neuron_count,
                max_steps=max_steps,
                enable_profiling=params.get('enable_profiling', True),
                output_format=params.get('output_format', 'json')
            )
            
            if result:
                results.append({
                    'parameters': params,
                    'output': result,
                    'timestamp': time.time()
                })
            
            time.sleep(1)  # Prevent overwhelming system
        
        return results

# Usage example
if __name__ == "__main__":
    runner = NLMSimulationRunner()
    
    param_sets = [
        {'neuron_count': 500, 'max_steps': 100},
        {'neuron_count': 1000, 'max_steps': 500},
        {'neuron_count': 2000, 'max_steps': 1000},
        {'neuron_count': 5000, 'max_steps': 2000, 'enable_profiling': False}
    ]
    
    results = runner.run_parameter_sweep(param_sets)
    print(f"Completed {len(results)} simulations")
```

## Command Line Completion and Auto-suggestions

### Auto-completion Script

```bash
# nlm-completion.bash
_nlm_completion() {
    local cur prev opts base
    COMPREPLY=[]
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    
    case "$prev" in
        nlm)
            opts="simulation config debug plugin benchmark profile analyze help interactive-help completion"
            COMPREPLY=( $(compgen -W "$opts" -- "$cur") )
            return 0
            ;;
        simulation)
            opts="--neuron-count --max-steps --timestep --config --output-file --output-format --enable-profiling --enable-validation --parallel --random-seed --log-level"
            COMPREPLY=( $(compgen -W "$opts" -- "$cur") )
            return 0
            ;;
        config)
            opts="list set get validate export"
            COMPREPLY=( $(compgen -W "$opts" -- "$cur") )
            return 0
            ;;
        debug)
            opts="attach profile validate analyze"
            COMPREPLY=( $(compgen -W "$opts" -- "$cur") )
            return 0
            ;;
        profile)
            opts="start stop report"
            COMPREPLY=( $(compgen -W "$opts" -- "$cur") )
            return 0
            ;;
        benchmark)
            opts="run compare analyze"
            COMPREPLY=( $(compgen -W "$opts" -- "$cur") )
            return 0
            ;;
    esac
}

complete -F _nlm_completion nlm
```

### Command Suggestions

The NLM CLI provides intelligent command suggestions based on:

- **Command History**: Suggests previously used commands
- **Context Awareness**: Provides relevant commands based on current context
- **Usage Patterns**: Recommends options based on common usage patterns
- **Error Recovery**: Suggests corrective actions for common errors

```bash
# Example interactive session
$ nlm simulation --ne
nlm simulation --neuron-count
$ nlm simulation --neuron-count 500 --max-steps
nlm simulation --max-steps 1000
$ nlm config set bra
nlm config set brain.neuron_count
$ nlm plugin load /pat
nlm plugin load /path/to/plugin.so
```

## Conclusion

The NLM Command-Line Interface provides a comprehensive and flexible tool for managing neural simulations, debugging, profiling, and system administration. Whether you're running simple simulations or complex parameter sweeps, the CLI offers the power and flexibility needed for both basic and advanced use cases.

Key benefits of the NLM CLI:

1. **Comprehensive Coverage**: From basic simulation setup to advanced system management
2. **Flexible Configuration**: Extensive options for customization and optimization
3. **Powerful Analysis Tools**: Built-in profiling, benchmarking, and debugging capabilities
4. **Extensible Plugin System**: Easy addition of new functionality
5. **Intelligent Help**: Auto-completion, suggestions, and comprehensive documentation
6. **Scripting Support**: Integration with shell and Python scripts
7. **Cross-platform**: Works on Linux, macOS, and Windows

For more information and additional examples, refer to:
- **USAGE.md** - Complete usage guide
- **INSTALLATION.md** - Installation and setup instructions
- **DEVELOPING.md** - Development and extension guide
- **ARCHITECTURE.md** - System architecture documentation
- **CLI.md** - Command-line interface reference (this document)
- **PROFILING.md** - Performance profiling documentation
- **PLUGINS.md** - Plugin development guide
- **TUTORIALS.md** - Step-by-step tutorials

The NLM CLI is designed to be both accessible for beginners and powerful for advanced users, providing the tools needed to effectively simulate, analyze, and understand neural systems.
