# NLM Command Line Interface
# Comprehensive CLI for NLM (Neural Learning Machine)

## Overview
The NLM CLI provides a unified interface for running neural simulations,
experiments, and demos. It supports multiple execution modes, configuration
loading, and advanced features for both beginners and power users.

## Usage

### Basic Usage
```bash
nlm-cli [options] <command> [args]

Commands:
  run        Run a simulation or experiment
  demo       Run a specific demo program
  explore    Explore and test individual components
  monitor    Monitor an ongoing simulation
  config     Configuration management
  help       Show help information

Options:
  -h, --help    Show this help message
  -v, --verbose  Enable verbose output
  -c, --config   Configuration file
  --phase       Phase to run (2,3,4,5,6)
  --log-level   Set log level (trace, debug, info, warn, error)
  --output      Output directory
  --timeout     Timeout in seconds
```

### Examples

#### Run Phase 6 Integration Demo
```bash
nlm-cli run --phase 6 --duration 3000
```

#### Run Learning Experiment
```bash
nlm-cli run experiment learning --config experiments/learning.cfg
```

#### Run Specific Test
```bash
nlm-cli run test plasticity --steps 100 --neuron-count 100
```

#### Monitor Current Simulation
```bash
nlm-cli monitor --port 8080 --format json
```

#### Configure and Save Settings
```bash
nlm-cli config set neuron_count 1000
nlm-cli config save my_config.cfg
```

## Command Details

### run
Run a simulation or experiment. Available experiment types:

- `learning` - Demonstrate synaptic changes through experience
- `plasticity` - Test plasticity rules (STDP, Hebbian)
- `connectivity` - Test basic neural connectivity
- `full` - Run comprehensive integration test
- `benchmark` - Run performance benchmarks
- `custom` - Run custom configuration

### demo
Run a specific demo program. Available demos:

- `phase2` - Phase 2: Real Neural Computation
- `phase3` - Phase 3: World Interaction
- `phase4` - Phase 4: Cognition Integration
- `phase5` - Phase 5: Performance Optimizations
- `phase6` - Phase 6: Final Integration
- `quick` - Quick integration test

### explore
Explore and test individual components:

- `neurons` - Test neuron dynamics
- `synapses` - Test synaptic connections
- `memory` - Test memory systems
- `plasticity` - Test plasticity rules
- `cognition` - Test cognitive components

### monitor
Monitor simulation progress and metrics:

- `--port` - Web server port for monitoring
- `--format` - Output format (json, csv, text)
- `--interval` - Monitoring update interval
- `--metrics` - Metrics to monitor

### config
Configuration management:

- `show` - Show current configuration
- `set key value` - Set configuration value
- `get key` - Get configuration value
- `load file` - Load configuration from file
- `save file` - Save configuration to file
- `reset` - Reset to default configuration

## Advanced Features

### Custom Configurations
Create custom configurations with Python-like syntax:

```bash
# Create custom config
nlm-cli config create custom_config.cfg <<EOF
neuron_count = 2000
region_count = 3
connection_probability = 0.05
enable_plasticity = true
enable_memory = true
enable_neuromodulation = true
EOF

# Run with custom config
nlm-cli run custom --config custom_config.cfg
```

### Batch Processing
Run multiple experiments in batch mode:

```bash
# Run multiple experiments
nlm-cli batch run <<EOF
phase2 learning --steps 500 --neuron-count 500
phase3 learning --steps 1000 --neuron-count 1000
phase6 integration --duration 2000
EOF

# Run experiments with different configurations
nlm-cli batch config <<EOF
configs/small.cfg
configs/medium.cfg
configs/large.cfg
EOF
```

### Python API Integration
The NLM CLI supports integration with Python scripts:

```python
import subprocess
import json

# Run experiment via CLI
result = subprocess.run([
    'nlm-cli', 'run', 'learning',
    '--steps', '1000',
    '--neuron-count', '500'
], capture_output=True, text=True)

# Parse results
if result.returncode == 0:
    data = json.loads(result.stdout)
    print(f"Learning achieved: {data['learning_achieved']}")
```

## Configuration Files

### Configuration Format
Configuration files use a simple key=value format:

```ini
# NLM Configuration
# Set simulation parameters
simulation_timestep = 0.001
random_seed = 42
neuron_count = 500
region_count = 1
connection_probability = 0.15

# Enable/disable systems
enable_plasticity = true
enable_memory = true
enable_neuromodulation = true

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Logging and output
log_level = info
checkpoint_enabled = true
checkpoint_interval = 1000
```

### Configuration Templates
Predefined configuration templates:

- `configs/simple.cfg` - Minimal configuration for quick testing
- `configs/medium.cfg` - Medium-sized brain for learning experiments
- `configs/large.cfg` - Large brain for comprehensive experiments
- `configs/performance.cfg` - Configuration optimized for benchmarks

## Installation

### Build Instructions
```bash
# Configure build with options
cmake .. -DCMAKE_BUILD_TYPE=Release \
          -DBUILD_CLI=ON \
          -DENABLE_PYTHON_BINDINGS=ON \
          -DENABLE_MONITORING=ON \

# Build
make -j4 nlm-cli

# Install (optional)
make install
```

### Python Integration
```bash
# Install Python bindings
pip install -e .

# Verify installation
python3 -c "import pynlm; print('NLM Python bindings loaded successfully')"
```

## Monitoring and Visualization

### Web Dashboard
The CLI includes a built-in web dashboard for monitoring:

- **System Status** - Real-time simulation state
- **Metrics** - Live performance metrics
- **Configuration** - Current settings
- **Logs** - Recent system logs

Access at: `http://localhost:8080` (default port)

### Export Formats
Results can be exported in multiple formats:

- **JSON** - Machine-readable format
- **CSV** - Tabular data for analysis
- **YAML** - Configuration format
- **SQLite** - Database for long-term storage

### Example Export
```bash
# Export simulation results to CSV
nlm-cli run learning --steps 1000 --output /tmp/experiment1.csv

# Export configuration to YAML
nlm-cli config export --format yaml /tmp/config.yaml
```

## Troubleshooting

### Common Issues

#### Configuration Errors
```bash
# Fix common config errors
nlm-cli config validate --fix errors.cfg
```

#### Performance Issues
```bash
# Run with performance optimizations
nlm-cli run --phase 6 --optimization-level high
```

#### Memory Issues
```bash
# Limit memory usage
nlm-cli run --max-memory 4GB
```

### Getting Help

```bash
# Get help for specific commands
nlm-cli help run
nlm-cli help demo
nlm-cli help explore

# Get full help
nlm-cli help --all
```

## Scripts and Automation

### Environment Setup Script
```bash
# Setup script for NLM development environment
source nlm-env.sh

# Create development workspace
nlm-dev workspace

# Run common development workflows
development-workflow learn "my_learning_config.cfg"
development-workflow benchmark
```

### CI/CD Integration
```yaml
# GitHub Actions example
deployment:
  steps:
    - name: Build NLM CLI
      run: |
        mkdir build && cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make nlm-cli
    
    - name: Run Tests
      run: |
        ./build/nlm-cli run test --suite all
```

## Conclusion

The NLM CLI provides a comprehensive interface for working with the Neural Learning Machine framework. It supports:

- **Easy Usage**: Simple commands for beginners
- **Advanced Features**: Powerful options for power users
- **Integration**: Seamless integration with Python and other tools
- **Monitoring**: Real-time monitoring and visualization
- **Automation**: Batch processing and scripting support

The CLI makes it easy to run experiments, test components, and explore the NLM framework, whether you're just getting started or are an experienced researcher.