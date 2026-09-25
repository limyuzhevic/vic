# NLM CLI Usage Examples

This document provides examples of how to use the NLM CLI interface for various tasks.

## Basic Usage

The NLM CLI provides a comprehensive interface for brain simulation and analysis. All commands follow the pattern:

```bash
nlm <command> [options]
```

### Help and Version

Get help for a specific command:
```bash
nlm run --help
nlm inspect --help
nlm benchmark --help
```

Get general help:
```bash
nlm --help
```

Get version information:
```bash
nlm --version
```

## Command Examples

### 1. Running Simulations

#### Basic Connectivity Simulation
```bash
# Run a basic simulation with 1000 neurons for 1000 steps
nlm run --experiment basic --steps 1000 --neuron-count 1000

# Save the brain state after simulation
nlm run --experiment basic --steps 1000 --save-file simulation_results.bin

# Run with custom configuration
nlm run -c my_config.cfg --experiment plasticity
```

#### Plasticity Learning Simulation
```bash
# Run a plasticity experiment
nlm run --experiment plasticity --steps 2000 --neuron-count 500

# Enable verbose logging
nlm run --experiment plasticity -v

# Run with specific timestep
nlm run --experiment plasticity --timestep 0.0005
```

### 2. Inspecting Brain State

#### Basic Inspection
```bash
# Inspect current brain state
nlm inspect

# Show detailed information
nlm inspect --detailed

# Export to JSON format
nlm inspect --export json --export-file brain_state.json

# Export to CSV format
nlm inspect --export csv --export-file statistics.csv
```

#### Load and Inspect
```bash
# Load a saved brain state and inspect it
nlm inspect --load saved_brain.bin

# Show configuration details
nlm inspect --detailed --config-only
```

### 3. Configuration Management

#### Listing Configuration
```bash
# List current configuration
nlm config list

# Show default values
nlm config list --show-defaults
```

#### Setting Configuration
```bash
# Set integer value
nlm config set neuron_count int:2000

# Set floating point value
nlm config set connection_probability float:0.15

# Set string value
nlm config set environment_name string:GridWorld

# Set boolean value
nlm config set visualization_enabled bool:true
```

#### Save and Load
```bash
# Save current configuration
nlm config save my_config.cfg

# Load configuration from file
nlm config load my_config.cfg

# Validate configuration
nlm config validate
```

### 4. Performance Benchmarking

#### Basic Benchmarking
```bash
# Run benchmarks with default configurations
nlm benchmark

# Specify neuron counts
nlm benchmark --neuron-counts 100,500,1000,2000

# Set custom steps
nlm benchmark --steps 500
```

#### Save Benchmark Results
```bash
# Save results to CSV file
nlm benchmark --output bench_results.csv --format csv

# Save results to JSON file
nlm benchmark --output bench_results.json --format json
```

### 5. Replay and Checkpointing

#### Replay Simulation
```bash
# Replay a saved brain state
nlm replay --load saved_state.bin

# Replay with custom parameters
nlm replay --load saved_state.bin --steps 500 --speed 2.0
```

#### Checkpoint Management
```bash
# Save current state as checkpoint
nlm checkpoint save

# List available checkpoints
nlm checkpoint list

# Restore checkpoint
nlm checkpoint restore 0

# Delete checkpoint
nlm checkpoint delete 1
```

### 6. Debugging

#### Enable Debugging
```bash
# Enable verbose logging
nlm run -v

# Enable debug logging
nlm run -vv

# Suppress output
nlm run -q
```

#### Debugging Tools
```bash
# Enable tracing
nlm debug --trace debug.log

# Show performance statistics
nlm debug --perf

# Show brain diagnostics
nlm debug --diagnostics
```

### 7. Specific Simulations

#### Available Scenarios
```bash
# List available simulation scenarios
nlm simulate --help

# Run basic scenario
nlm simulate basic --iterations 500

# Run plasticity scenario
nlm simulate plasticity --iterations 1000

# Run STDP scenario
nlm simulate stdp --iterations 300
```

### 8. Exploring Brain Structure

#### Explore Brain
```bash
# Explore entire brain structure
nlm explore

# Explore specific region
nlm explore --region 1

# Explore specific neuron
nlm explore --neuron 5

# Explore specific synapse
nlm explore --synapse 10

# Show connection statistics
nlm explore --connections

# Export brain structure
nlm explore --export dot --export-file brain.dot
```

## Configuration File Format

Configuration files can be in JSON, YAML, or simple key-value format:

### JSON Format
```json
{
  "neuron_count": 1000,
  "region_count": 2,
  "connection_probability": 0.1,
  "simulation_timestep": 0.001,
  "random_seed": 42,
  "environment_name": "GridWorld",
  "plasticity_learning_rate": 0.01,
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.012
}
```

### Simple Format
```
# NLM Configuration File
neuron_count = 1000
region_count = 2
connection_probability = 0.1
simulation_timestep = 0.001
random_seed = 42
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
```

## Scripting Examples

### Shell Script for Running Experiments
```bash
#!/bin/bash

# Run a series of experiments
for neurons in 100 500 1000 2000; do
    echo "Running benchmark with $neurons neurons"
    nlm benchmark --neuron-counts $neurons
    sleep 2
    echo "Completed benchmark with $neurons neurons"
    echo "---"
done
```

### Python Script for Analysis
```python
#!/usr/bin/env python3

import subprocess
import json
import time

# Run simulation
print("Running simulation...")
result = subprocess.run(["nlm", "run", "--experiment", "basic", "--steps", "1000"], 
                       capture_output=True, text=True)
print(result.stdout)

# Inspect results
print("\nInspecting results...")
result = subprocess.run(["nlm", "inspect", "--export", "json", "--export-file", "results.json"], 
                       capture_output=True, text=True)
print(result.stdout)

# Load and analyze JSON
with open("results.json", "r") as f:
    data = json.load(f)
    print(f"Brain has {data['neurons']} neurons")
    print(f"Brain has {data['synapses']} synapses")
```

## Advanced Usage

### Combining Commands
```bash
# Run a complex workflow
nlm run --experiment plasticity --steps 1000 --save-file intermediate.bin && \
nlm inspect --load intermediate.bin --detailed --export json results.json && \
echo "Workflow completed successfully"
```

### Command History
```bash
# The CLI maintains command history
# Access with shell history (Ctrl+R)
# Save history with nlm history save
# Load history with nlm history load
```

## Troubleshooting

### Common Issues

1. **Command not found**
   ```bash
   # Check if nlm_cli executable is in your PATH
   which nlm
   # or
   ls /usr/local/bin/nlm*
   ```

2. **Configuration file errors**
   ```bash
   # Check syntax of config file
   nlm config validate
   ```

3. **Permission denied**
   ```bash
   # Check file permissions
   chmod +x nlm_cli
   ```

4. **Memory issues**
   ```bash
   # Reduce simulation size
   nlm run --neuron-count 500
   ```

## Performance Tips

1. **For large simulations**
   ```bash
   nlm run --neuron-count 100 --steps 100
   ```

2. **For debugging**
   ```bash
   nlm run --experiment basic -v
   ```

3. **For benchmarking**
   ```bash
   nlm benchmark --neuron-counts 100,500
   ```

## Automation

### Makefile Example
```makefile
.PHONY: all run-inspect benchmark

all: run-inspect benchmark

run-inspect:
	@echo "Running basic simulation..."
	nlm run --experiment basic --steps 500
	@echo "Inspecting results..."
	nlm inspect --detailed

benchmark:
	@echo "Running benchmarks..."
	nlm benchmark --neuron-counts 100,500,1000
```

### Docker Example
```dockerfile
FROM ubuntu:latest

RUN apt-get update && apt-get install -y cmake g++ make
RUN git clone https://github.com/your-org/nlm.git
WORKDIR /nlm
RUN mkdir build && cd build && cmake .. && make

ENTRYPOINT ["/nlm/build/nlm_cli"]
```

This comprehensive CLI interface provides everything needed to work with NLM simulations, from simple exploration to complex research workflows.
