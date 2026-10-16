"""
Enhanced NLM Command-Line Interface
===================================

This document describes the enhanced command-line interface (CLI) for the NLM
framework. The CLI has been significantly improved to provide better user
experience, more features, and easier access to simulation capabilities.

## Overview

The enhanced CLI provides:

1. **Comprehensive help system** with detailed usage information
2. **Multiple execution modes** for different use cases
3. **Advanced configuration options** 
4. **Real-time monitoring and statistics**
5. **Batch processing capabilities**
6. **Plugin system integration**
7. **Export/import functionality**
8. **Performance benchmarking**
9. **Visualization integration**
10. **Automated testing and validation**

## Command Structure

### Main Command
```bash
nlm [OPTIONS] [MODE] [CONFIG_FILE]
```

### Available Modes
- `basic` - Run basic demonstration
- `learning` - Run learning experiments
- `plasticity` - Test plasticity mechanisms
- `research` - Run comprehensive research simulations
- `benchmark` - Performance benchmarking
- `validate` - Configuration validation and testing
- `batch` - Batch processing of multiple configurations

### Key Options
- `--help` - Show help information
- `--version` - Show version information
- `--verbose` - Enable verbose output
- `--quiet` - Suppress non-essential output
- `--output` - Specify output file
- `--duration` - Simulation duration (seconds)
- `--steps` - Number of simulation steps
- `--config` - Load configuration file
- `--seed` - Random seed for reproducibility
- `--plugin` - Load plugin(s)
- `--export` - Export simulation results
- `--profile` - Enable performance profiling

## Usage Examples

### Basic Usage
```bash
# Run basic demonstration (default)
nlm basic

# Run with custom configuration
nlm basic --config my_config.cfg

# Run with verbose output
nlm basic --verbose

# Run with custom duration
nlm basic --duration 60.0
```

### Learning Experiments
```bash
# Run learning experiment with statistics
nlm learning --duration 120.0 --steps 5000 --output results.json

# Run learning with specific random seed
nlm learning --seed 12345 --verbose

# Run learning with plugin system
nlm learning --plugin synaptic_learning.so --plugin neuromodulation.so
```

### Plasticity Testing
```bash
# Test plasticity mechanisms
nlm plasticity --steps 2000 --export plasticity_results.json

# Test with custom parameters
nlm plasticity --neuron_count 1000 --connection_probability 0.2
```

### Research Mode
```bash
# Run comprehensive research simulation
nlm research --duration 300.0 --steps 100000

# Run research with batch processing
nlm research --batch --steps 50000 --output research_batch.json
```

### Benchmarking
```bash
# Run performance benchmarking
nlm benchmark --duration 60.0 --output benchmark_results.csv

# Compare different configurations
nlm benchmark --compare config1.cfg config2.cfg
```

### Validation Mode
```bash
# Validate configuration and test brain
nlm validate --config test.cfg

# Validate with additional plugins
nlm validate --config production.cfg --plugin memory_optimization.so
```

### Batch Processing
```bash
# Process multiple configurations
nlm batch --configs config_list.txt --output batch_results.json

# Run batch with parallel processing
nlm batch --configs configs/ --workers 4 --duration 30.0
```

## Configuration File Format

### Basic Configuration File
```ini
# NLM Configuration

[simulation]
random_seed = 42
simulation_timestep = 0.001
duration = 60.0
neuron_count = 1000
region_count = 2
connection_probability = 0.15

[plasticity]
stdp_enable = true
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
hebbian_enable = true
structural_enable = true

[neuromodulation]
dopamine_enable = true
dopamine_scale = 1.0
curiosity_enable = true
novelty_enable = true

[logging]
level = INFO
file = simulation.log
timestamp = true

[plugins]
# List of plugins to load (relative paths)
neuron_plugin = biophysical_neuron.so
plasticity_plugin = reward_stdp.so
memory_plugin = working_memory.so
```

### Command Line Overrides
```bash
# Override configuration values from command line
nlm basic --neuron_count 2000 --connection_probability 0.2

# Multiple overrides
nlm learning --duration 120.0 --neuron_count 1500 --seed 12345
```

## Output Formats

### JSON Output
```json
{
    "metadata": {
        "timestamp": "2026-10-16T10:41:45Z",
        "version": "0.1.0",
        "duration": 60.0,
        "steps": 5000,
        "neuron_count": 1000,
        "final_spikes": 3452
    },
    "statistics": {
        "learning_detected": true,
        "weight_changes": {
            "strengthened": 12,
            "weakened": 8,
            "unchanged": 45
        },
        "activity": {
            "total_spikes": 3452,
            "avg_firing_rate": 3.452,
            "most_active_neuron": 42
        }
    },
    "configuration": {
        "neuron_count": 1000,
        "connection_probability": 0.15,
        "stdp_enabled": true
    }
}
```

### CSV Output
```csv
date,time,step,neuron_count,total_spikes,firing_rate,learning_detected,weight_change
2026-10-16T10:41:45Z,0.000,0,1000,0,0.000,false,0.000
2026-10-16T10:41:45Z,0.001,100,1000,23,0.023,false,0.012
2026-10-16T10:41:45Z,0.002,200,1000,45,0.045,true,0.025
2026-10-16T10:41:45Z,0.003,300,1000,67,0.067,true,0.041
```

## Real-Time Monitoring

### Progress Indicators
```
NLM Basic Simulation
========================
Progress: [==================================================] 100%
Step: 1000 / 5000 (20.0%)
Neurons: 1000 firing: 42
Spikes: 3452 total
Time: 5.0s / 60.0s (8.3%)
FPS: 333.3
Eta: 45.0s

Learning Status: ✓ ACTIVE
Plasticity: ✓ ENABLED
Weights: +0.025 (↑)
Neuromodulation: ✓ ACTIVE
Curiosity: 0.456 (↑)
Novelty: 0.123 (↓)
```

### Interactive Controls (for supported terminals)
```
[>] Pause       [>] Resume     [>] Step       [>] Stop       [>] Export
[q] Quit        [>] Help       [>] Stats      [>] Config      [>] Plugins
```

## Plugin Integration

### Plugin Loading
```bash
# Load specific plugins
nlm basic --plugin biophysical_neuron.so --plugin reward_stdp.so

# Load all plugins from directory
nlm basic --plugins plugins/

# Plugin with configuration
nlm basic --plugin memory.so:capacity=1000:decay_rate=0.1
```

### Plugin Configuration
Plugins can be configured via command line arguments:

```bash
# Configure plugin parameters
nlm learning --plugin neuron.so:threshold=-50.0,conductance=120.0

# Multiple plugin configurations
nlm research --plugin stdp.so:learning_rate=0.01,time_constant=20.0
         --plugin hebbian.so:weight=0.5,growth_rate=0.001
```

## Advanced Features

### Performance Profiling
```bash
# Enable performance profiling
nlm basic --profile --output profile_report.html

# Performance comparison between runs
nlm benchmark --profile --compare run1.json run2.json
```

### Automated Testing
```bash
# Run automated test suite
nlm validate --test_suite="connectivity,plasticity,learning"

# Continuous integration friendly output
nlm validate --ci --output junit_results.xml
```

### Batch Processing
```bash
# Process multiple configurations
nlm batch --configs config_list.txt --parallel 4

# Export results in multiple formats
nlm batch --configs configs/ --output json,xml,csv
```

## Scripting Interface

### Python API for CLI Integration
```python
import subprocess
import json
import time

def run_nlm_simulation(config_file, mode="research"):
    """Run NLM simulation with Python API"""
    cmd = [
        "nlm", mode,
        "--config", config_file,
        "--output", "simulation_results.json",
        "--verbose"
    ]
    
    print(f"Running: {' '.join(cmd)}")
    
    start_time = time.time()
    result = subprocess.run(cmd, capture_output=True, text=True)
    end_time = time.time()
    
    if result.returncode != 0:
        print(f"Error: {result.stderr}")
        return None
    
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    
    # Load results
    try:
        with open("simulation_results.json", "r") as f:
            data = json.load(f)
        return data
    except FileNotFoundError:
        print("No results file generated")
        return None

# Example usage
if __name__ == "__main__":
    results = run_nlm_simulation("research_config.cfg")
    if results:
        print(f"Learning detected: {results['metadata']['learning_detected']}")
        print(f"Total spikes: {results['metadata']['final_spikes']}")
```

### Shell Script Interface
```bash
#!/bin/bash
# NLM Simulation Wrapper Script

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

run_simulation() {
    local config_file=$1
    local mode=${2:-"research"}
    local output_file="${3:-"${SCRIPT_DIR}/results.json"}"
    
    echo "Starting NLM simulation..."
    echo "Config: $config_file"
    echo "Mode: $mode"
    echo "Output: $output_file"
    
    # Run simulation
    nlm "$mode" --config "$config_file" --output "$output_file" --verbose
    
    if [ $? -eq 0 ]; then
        echo "Simulation completed successfully"
        echo "Results saved to: $output_file"
        
        # Extract key metrics
        if command -v jq &> /dev/null; then
            echo "Key metrics:"
            echo "  Learning detected: $(jq '.statistics.learning_detected' "$output_file")"
            echo "  Total spikes: $(jq '.statistics.activity.total_spikes' "$output_file")"
            echo "  Weight change: $(jq '.statistics.weight_changes.change' "$output_file")"
        fi
    else
        echo "Simulation failed!"
        exit 1
    fi
}

# Run simulation with provided config
if [ $# -ge 1 ]; then
    run_simulation "$1" "$2" "$3"
else
    echo "Usage: $0 <config_file> [mode] [output_file]"
    echo "Example: $0 research_config.cfg research results.json"
    exit 1
fi
```

## Error Handling and Diagnostics

### Common Error Messages
```
NLM Error: Configuration file not found
    Solution: Use --config to specify a valid configuration file
    Or ensure the file exists in the expected location

NLM Error: Invalid plugin specification
    Solution: Check plugin name and format
    Use --list-plugins to see available plugins

NLM Error: Memory allocation failed
    Solution: Reduce neuron count or increase system resources
    Try running with smaller configuration

NLM Error: Simulation timeout
    Solution: Reduce duration or increase simulation steps
    Check system resources and memory availability
```

### Diagnostic Options
```bash
# Enable debug mode
nlm basic --debug --verbose

# Check system resources
nlm basic --check-resources

# Validate configuration thoroughly
nlm basic --validate-full --strict

# Show plugin information
nlm basic --list-plugins
nlm basic --plugin-info neuron.so
```

## Installation and Setup

### Quick Installation
```bash
# Build NLM with enhanced CLI
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENHANCED_CLI=ON
make -j$(nproc)

# Install
sudo make install
```

### Development Installation
```bash
# Clone and build with CLI enhancements
git clone https://github.com/nlm-project/nlm.git
cd nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENHANCED_CLI=ON -DPLUGIN_SYSTEM=ON
make -j$(nproc)

# For development, use the Python API
pip install -e .
```

### Configuration File Generation
```bash
# Generate template configuration
nlm generate-config > template.cfg

# Validate configuration
nlm validate template.cfg --strict

# Test configuration
nlm test-config template.cfg
```

## Performance Optimization

### Command Line Optimization
```bash
# Use these options for best performance
nlm basic --neuron_count 500 --duration 30.0 --quiet

# Enable parallel processing (if supported)
nlm batch --parallel 4 --duration 60.0

# Use minimal logging for performance
nlm basic --quiet --no-logging
```

### Resource Monitoring
```bash
# Monitor resource usage during simulation
nlm basic --profile --monitor-resources --output resources.json

# Set resource limits
nlm basic --memory-limit 4GB --time-limit 300.0
```

## Conclusion

The enhanced NLM command-line interface provides a comprehensive set of tools for:

1. **Easy simulation control** with intuitive commands and options
2. **Advanced configuration** with flexible configuration files
3. **Comprehensive monitoring** with real-time statistics and progress indicators
4. **Plugin integration** for extending functionality
5. **Batch processing** for systematic experimentation
6. **Performance optimization** for large-scale simulations
7. **Error handling** with clear diagnostic messages
8. **Scripting support** for automated workflows
9. **Research reproducibility** with export/import capabilities
10. **Continuous integration** with CI/CD friendly output formats

The enhanced CLI makes NLM accessible to users with different skill levels, from beginners who want simple simulations to researchers who need sophisticated experimental setups and analysis tools.

For more information, refer to the specific mode documentation in the NLM manual.
