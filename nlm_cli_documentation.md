# NLM Command Line Interface

## Overview

The NLM Command Line Interface (CLI) provides a comprehensive interface for running neural simulations, experiments, and demos. It supports multiple execution modes, configuration loading, and advanced features for both beginners and power users.

## Installation

### Build from Source

```bash
# Clone NLM repository
cd /path/to/nlm
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_CLI=ON -DENABLE_PYTHON_BINDINGS=ON
make -j4 nlm-cli
```

### Quick Install

```bash
# Install using the development environment script
source nlm_env.sh

# Test installation
nlm-cli --version
```

## Basic Usage

### Help and Version

```bash
# Show general help
nlm-cli --help

# Show version information
nlm-cli --version

# Show help for a specific command
nlm-cli help run
nlm-cli help demo
nlm-cli help explore
```

### Demo Programs

```bash
# Run Phase 2: Real Neural Computation demo
nlm-cli demo phase2

# Run Phase 6: Final Integration demo
nlm-cli demo phase6

# Run quick integration test
nlm-cli demo quick

# Run specific demo
nlm-cli demo integration
```

### Experiments

```bash
# Run learning experiment
nlm-cli run learning --steps 1000 --neuron-count 500

# Run plasticity experiment
nlm-cli run plasticity --steps 500 --neuron-count 100

# Run connectivity test
nlm-cli run connectivity --steps 200 --neuron-count 50

# Run full integration
nlm-cli run full --duration 3000 --neuron-count 1000
```

### Component Exploration

```bash
# Explore neuron dynamics
nlm-cli explore neurons --neuron-count 100 --neuron-type sensory

# Explore synaptic connections
nlm-cli explore synapses --synapse-count 200 --plasticity true

# Explore memory systems
nlm-cli explore memory --memory-type working --capacity 1000

# Explore plasticity rules
nlm-cli explore plasticity --rule stdp --learning-rate 0.01

# Explore cognitive components
nlm-cli explore cognition --module planning --depth 5
```

## Configuration Management

### Show Current Configuration

```bash
# Display current settings
nlm-cli config show
```

### Set Configuration Values

```bash
# Set individual configuration values
nlm-cli config set neuron_count 1000
nlm-cli config set region_count 3
nlm-cli config set connection_probability 0.05
nlm-cli config set enable_plasticity true
```

### Get Configuration Values

```bash
# Get specific configuration values
nlm-cli config get neuron_count
nlm-cli config get connection_probability
nlm-cli config get enable_plasticity
```

### Load Configuration Files

```bash
# Load configuration from file
nlm-cli config load experiments/learning.cfg
nlm-cli config load configs/large.cfg
```

### Save Configuration Files

```bash
# Save current configuration to file
nlm-cli config save my_experiment.cfg
nlm-cli config save --format yaml my_config.yaml
```

### Reset Configuration

```bash
# Reset to default configuration
nlm-cli config reset
```

## Configuration Files

### Configuration Formats

NLM supports multiple configuration file formats:

#### Basic Format (.cfg)
```ini
# NLM Configuration Example
simulation_timestep = 0.001
random_seed = 42
neuron_count = 500
region_count = 1
connection_probability = 0.15

enable_plasticity = true
enable_memory = true
enable_neuromodulation = true

stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
```

#### YAML Format (.yaml)
```yaml
# NLM Configuration in YAML
simulation_timestep: 0.001
random_seed: 42
neuron_count: 500
region_count: 1
connection_probability: 0.15

enable_plasticity: true
enable_memory: true
enable_neuromodulation: true

stdp:
  ltp_weight: 0.02
  ltd_weight: 0.015
  tau: 20.0
```

### Configuration Templates

The NLM repository includes several configuration templates:

- `configs/simple.cfg` - Minimal configuration for quick testing
- `configs/medium.cfg` - Medium-sized brain for learning experiments
- `configs/large.cfg` - Large brain for comprehensive experiments
- `configs/performance.cfg` - Configuration optimized for benchmarks

## Advanced Features

### Python API Integration

The NLM CLI can be easily integrated with Python scripts:

```python
import subprocess
import json
import sys

def run_experiment(experiment_type, **kwargs):
    """Run an NLM experiment via CLI"""
    cmd = ['nlm-cli', 'run', experiment_type]
    
    # Add keyword arguments as command line options
    for key, value in kwargs.items():
        cmd.extend([f'--{key}', str(value)])
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        return {
            'success': True,
            'stdout': result.stdout,
            'stderr': result.stderr
        }
    except subprocess.CalledProcessError as e:
        return {
            'success': False,
            'stdout': e.stdout,
            'stderr': e.stderr,
            'returncode': e.returncode
        }

# Example usage
result = run_experiment('learning', steps=1000, neuron_count=500)
if result['success']:
    print("Experiment completed successfully!")
else:
    print(f"Experiment failed: {result['stderr']}")
```

### Batch Processing

Run multiple experiments in batch mode:

```bash
# Create batch file
create_batch.py << EOF
phase2 learning --steps 500 --neuron-count 500
phase3 learning --steps 1000 --neuron-count 1000
phase6 integration --duration 2000
EOF

# Run batch processing
nlm-cli batch run create_batch.py
```

### Web Dashboard

The NLM CLI includes a built-in web dashboard for monitoring:

```bash
# Start web dashboard on port 8080
nlm-cli monitor --port 8080 --format json

# Access dashboard at: http://localhost:8080
```

### Export Results

Export simulation results in various formats:

```bash
# Export to CSV
nlm-cli run learning --steps 1000 --output /tmp/experiment1.csv

# Export configuration to YAML
nlm-cli config export --format yaml /tmp/config.yaml

# Export metrics to JSON
nlm-cli monitor --format json --output /tmp/metrics.json
```

## Monitoring and Visualization

### Real-time Monitoring

```bash
# Start real-time monitoring
nlm-cli monitor --interval 1.0 --metrics "spikes,neurons,firing_rate"

# Monitor specific metrics
nlm-cli monitor --metrics "total_spikes,average_firing_rate,learning_achieved"
```

### Monitoring Options

```bash
# Monitoring command options
--port PORT              Web server port (default: 8080)
--format FORMAT         Output format: json, csv, text
--interval INTERVAL     Update interval in seconds
--metrics METRICS       Comma-separated list of metrics
--output FILE           Output file path
--verbose, -v           Enable verbose output
--debug                 Enable debug output
```

## Scripts and Automation

### Environment Setup

```bash
# Source the development environment script
source nlm_env.sh

# Set up a development workspace
nlm/scripts/dev_workspace.sh

# Run quick tests to verify installation
nlm/scripts/quick_test.sh
```

### CI/CD Integration

```yaml
# GitHub Actions example
deployment:
  steps:
    - name: Setup NLM Environment
      run: |
        source nlm_env.sh
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_CLI=ON
        make nlm-cli
    
    - name: Run NLM Tests
      run: |
        nlm-cli demo phase2
        nlm-cli run learning --steps 100
    
    - name: Run Integration Test
      run: |
        nlm-cli demo phase6
```

## Configuration Management Scripts

### Configuration Validation

```bash
# Validate configuration file
nlm-cli config validate --fix experiments/learning.cfg

# Check for configuration errors
nlm-cli config check --strict my_config.cfg
```

### Configuration Backup

```bash
# Backup configuration
nlm-cli config backup --directory backups --compress

# List backup files
nlm-cli config backup --list

# Restore configuration from backup
nlm-cli config backup --restore backups/config_2024_01_01.tar.gz
```

## Troubleshooting

### Common Issues

#### Configuration Errors
```bash
# Fix common configuration errors
nlm-cli config validate --fix errors.cfg
```

#### Performance Issues
```bash
# Run with performance optimizations
nlm-cli run --phase 6 --optimization-level high

# Limit memory usage
nlm-cli run --max-memory 4GB
```

#### Missing Dependencies
```bash
# Check if NLM is properly installed
nlm-cli --version

# Check Python bindings
python3 -c "import pynlm; print('Python bindings loaded successfully')"
```

### Getting Help

```bash
# Get comprehensive help
nlm-cli help --all

# Get help for specific commands
nlm-cli help run
nlm-cli help demo
nlm-cli help explore
nlm-cli help config
```

## Advanced Configuration

### Custom Configuration Scripts

Create custom configuration scripts:

```bash
#!/bin/bash

# Custom NLM configuration script
cat > custom_config.cfg << 'EOF'
# Custom NLM configuration
simulation_timestep = 0.001
random_seed = $(date +%s)
neuron_count = 1000
region_count = 3
connection_probability = 0.05

enable_plasticity = true
enable_memory = true
enable_neuromodulation = true

stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
EOF

echo "Custom configuration created: custom_config.cfg"
```

### Performance Tuning

```bash
# Performance tuning options
nlm-cli run --performance-level high --optimization-mode aggressive
nlm-cli run --cache-size 2GB --gpu-acceleration
```

## Development Environment

### Development Scripts

The NLM repository includes several development scripts:

- `scripts/dev_workspace.sh` - Create development workspace
- `scripts/quick_test.sh` - Quick installation test
- `scripts/benchmark.sh` - Performance benchmarks
- `scripts/validate.sh` - Configuration validation

### Build from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build with development options
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DBUILD_CLI=ON -DENABLE_PYTHON_BINDINGS=ON
make -j4

# Install
make install
```

### Running Tests

```bash
# Run unit tests
./build/nlm-cli run test --suite unit

# Run integration tests
./build/nlm-cli run test --suite integration

# Run all tests
./build/nlm-cli run test --suite all
```

## Conclusion

The NLM CLI provides a comprehensive interface for working with the Neural Learning Machine framework. It supports:

- **Easy Usage**: Simple commands for beginners
- **Advanced Features**: Powerful options for power users
- **Integration**: Seamless integration with Python and other tools
- **Monitoring**: Real-time monitoring and visualization
- **Automation**: Batch processing and scripting support
- **Configuration**: Comprehensive configuration management

The CLI makes it easy to run experiments, test components, and explore the NLM framework, whether you're just getting started or are an experienced researcher.

## See Also

- NLM Documentation: https://docs.nlm.ai
- NLM GitHub Repository: https://github.com/nlm-project/nlm
- NLM Python Bindings: https://pypi.org/project/pynlm
- NLM Discord Community: https://discord.gg/nlm
- NLM Discord Community: https://discord.gg/nlm
