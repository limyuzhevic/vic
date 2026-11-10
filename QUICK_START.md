# NLM Setup and Usage Guide

## Quick Start

This guide provides quick instructions for getting started with the NLM framework.

### 1. Environment Setup

```bash
# Set up the NLM development environment
source nlm_env.sh

# Test if NLM is working
nlm-cli --version
```

### 2. Running Demos

```bash
# Run Phase 2: Real Neural Computation demo
nlm-cli demo phase2

# Run Phase 6: Final Integration demo
nlm-cli demo phase6

# Run quick integration test
nlm-cli demo quick
```

### 3. Running Experiments

```bash
# Run learning experiment
nlm-cli run learning --steps 1000 --neuron-count 500

# Run plasticity experiment
nlm-cli run plasticity --steps 500 --neuron-count 100

# Run connectivity test
nlm-cli run connectivity --steps 200 --neuron-count 50
```

### 4. Exploring Components

```bash
# Explore neuron dynamics
nlm-cli explore neurons --neuron-count 100

# Explore synaptic connections
nlm-cli explore synapses --synapse-count 200

# Explore memory systems
nlm-cli explore memory --memory-type working
```

## Detailed Installation Guide

### Prerequisites

```bash
# Check system requirements
uname -a
gcc --version
python3 --version
pip3 --version
```

### Installation Options

#### Option 1: Development Environment Setup (Recommended)

```bash
# Clone NLM repository
cd /path/to/nlm

# Source development environment script
source nlm_env.sh

# Build NLM CLI (if not already built)
if [ ! -f "bin/nlm-cli" ]; then
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_CLI=ON -DENABLE_PYTHON_BINDINGS=ON
    make -j4 nlm-cli
    cd ..
fi

# Test installation
nlm-cli --version
```

#### Option 2: Using Package Manager

```bash
# Check if NLM is available via package manager
apt list | grep nlm
brew list | grep nlm
```

#### Option 3: Using Docker

```bash
# Build Docker image
FROM ubuntu:20.04
RUN apt-get update && apt-get install -y cmake build-essential git
RUN git clone https://github.com/nlm-project/nlm.git /nlm
WORKDIR /nlm
RUN mkdir -p build && cd build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_CLI=ON
RUN make -j4 nlm-cli

CMD ["/nlm/build/nlm-cli", "--help"]
```

## Configuration Files

### Configuration File Format

NLM uses configuration files in INI format:

```ini
# Example configuration file
[simulation]
simulation_timestep = 0.001
random_seed = 42
neuron_count = 500
region_count = 1
connection_probability = 0.15

[systems]
enable_plasticity = true
enable_memory = true
enable_neuromodulation = true

[plasticity]
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

[structural_plasticity]
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

[logging]
log_level = info
checkpoint_enabled = true
checkpoint_interval = 1000
```

### Configuration Templates

The NLM repository provides several configuration templates:

- `configs/simple.cfg` - Minimal configuration
- `configs/medium.cfg` - Medium-sized brain
- `configs/large.cfg` - Large brain
- `configs/performance.cfg` - Performance-optimized

### Configuration Variables

| Variable | Type | Default | Description |
|----------|------|---------|-------------|
| `simulation_timestep` | float | 0.001 | Simulation time step |
| `random_seed` | int64 | 42 | Random seed for initialization |
| `neuron_count` | int64 | 500 | Number of neurons |
| `region_count` | int64 | 1 | Number of brain regions |
| `connection_probability` | float | 0.15 | Probability of synapses |
| `enable_plasticity` | bool | true | Enable plasticity systems |
| `enable_memory` | bool | true | Enable memory systems |
| `enable_neuromodulation` | bool | true | Enable neuromodulation |

## Running Demos

### Phase 2 Demo: Real Neural Computation

The Phase 2 demo demonstrates:

- **LIF Neuron Dynamics**: Leaky Integrate-and-Fire neuron models
- **Spike Propagation**: Event-driven spike transmission with delays
- **Plasticity Rules**: STDP and Hebbian learning
- **Structural Plasticity**: Synaptogenesis and pruning

```bash
nlm-cli demo phase2
```

### Phase 6 Demo: Final Integration

The Phase 6 demo shows the complete integration of all systems:

- **Memory Systems**: Working, episodic, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction System**: Forward and inverse models
- **Cognition**: Planning, concept formation, attention
- **Development**: Developmental stages and maturation

```bash
nlm-cli demo phase6
```

## Running Experiments

### Learning Experiment

Demonstrates synaptic changes through experience:

```bash
nlm-cli run learning --steps 1000 --neuron-count 500
```

### Plasticity Experiment

Tests plasticity rules with correlated activity:

```bash
nlm-cli run plasticity --steps 500 --neuron-count 100 --stdp
```

### Connectivity Experiment

Tests basic neural connectivity:

```bash
nlm-cli run connectivity --steps 200 --neuron-count 50
```

### Full Integration Experiment

Runs comprehensive integration test:

```bash
nlm-cli run full --duration 3000 --neuron-count 1000
```

## Exploring Components

### Neuron Exploration

Test neuron dynamics with different parameters:

```bash
nlm-cli explore neurons --neuron-count 100 --neuron-type sensory
nlm-cli explore neurons --neuron-count 50 --neuron-type internal
nlm-cli explore neurons --neuron-count 25 --neuron-type motor
```

### Synapse Exploration

Test synaptic connections and plasticity:

```bash
nlm-cli explore synapses --synapse-count 200 --plasticity true
nlm-cli explore synapses --synapse-count 100 --plasticity false
nlm-cli explore synapses --synapse-count 50 --rule stdp
```

### Memory Exploration

Test memory system functionality:

```bash
nlm-cli explore memory --memory-type working --capacity 1000
nlm-cli explore memory --memory-type episodic --max-episodes 100
nlm-cli explore memory --memory-type associative --pattern-size 50
```

## Configuration Management

### Show Current Configuration

```bash
# Display all current settings
nlm-cli config show

# Display specific settings
nlm-cli config get neuron_count
nlm-cli config get connection_probability
```

### Set Configuration Values

```bash
# Set configuration values
nlm-cli config set neuron_count 1000
nlm-cli config set region_count 3
nlm-cli config set connection_probability 0.05
nlm-cli config set enable_plasticity true

# Set multiple values
nlm-cli config set simulation_timestep 0.001
nlm-cli config set random_seed 12345
nlm-cli config set enable_memory false
```

### Load Configuration Files

```bash
# Load configuration from file
nlm-cli config load experiments/learning.cfg
nlm-cli config load configs/large.cfg

# Load with validation
nlm-cli config load --validate experiments/learning.cfg
```

### Save Configuration Files

```bash
# Save current configuration
nlm-cli config save my_experiment.cfg
nlm-cli config save --format yaml my_config.yaml

# Save with backup
nlm-cli config save --backup my_experiment.cfg
```

### Reset Configuration

```bash
# Reset to default configuration
nlm-cli config reset

# Reset with validation
nlm-cli config reset --validate
```

## Advanced Features

### Python API Integration

```python
import subprocess
import json

def run_experiment(experiment_type, **kwargs):
    """Run an NLM experiment via CLI"""
    cmd = ['nlm-cli', 'run', experiment_type]
    
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

```bash
# Create batch file
nlm-cli batch create my_batch << EOF
phase2 learning --steps 500 --neuron-count 500
phase3 learning --steps 1000 --neuron-count 1000
phase6 integration --duration 2000
EOF

# Run batch
nlm-cli batch run my_batch
```

### Web Dashboard

```bash
# Start web dashboard
nlm-cli monitor --port 8080 --format json

# Access at: http://localhost:8080
```

## Development Environment

### Development Scripts

```bash
# Source the development environment script
source nlm_env.sh

# Create development workspace
nlm/scripts/dev_workspace.sh

# Run quick tests
nlm/scripts/quick_test.sh

# Run performance benchmarks
nlm/scripts/benchmark.sh

# Validate configuration
nlm/scripts/validate.sh
```

### Build from Source

```bash
# Clone repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Configure build
cmake .. -DCMAKE_BUILD_TYPE=Debug \
         -DBUILD_TESTS=ON \
         -DBUILD_CLI=ON \
         -DENABLE_PYTHON_BINDINGS=ON \
         -DENABLE_MONITORING=ON

# Build
make -j4

# Install (optional)
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

## Troubleshooting

### Common Issues

#### Configuration Errors
```bash
# Fix configuration errors
nlm-cli config validate --fix my_config.cfg
```

#### Performance Issues
```bash
# Run with performance optimizations
nlm-cli run --performance-level high --optimization-mode aggressive
```

#### Memory Issues
```bash
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

## System Requirements

### Minimum Requirements

| Component | Minimum |
|-----------|---------|
| CPU | 2 cores |
| RAM | 4 GB |
| Storage | 1 GB |
| OS | Linux, macOS, Windows |

### Recommended Requirements

| Component | Recommended |
|-----------|-------------|
| CPU | 8+ cores |
| RAM | 16+ GB |
| Storage | 50+ GB |
| OS | Linux (preferred) |

## Performance Considerations

### Simulation Time Steps

```bash
# Fine-grained simulation (high accuracy, slower)
nlm-cli config set simulation_timestep 0.001

# Coarse-grained simulation (faster, less accurate)
nlm-cli config set simulation_timestep 0.01
```

### Neuron Count Impact

```bash
# Small brain (quick testing)
nlm-cli config set neuron_count 100

# Medium brain (balanced)
nlm-cli config set neuron_count 1000

# Large brain (comprehensive testing)
nlm-cli config set neuron_count 10000
```

### Region Count Impact

```bash
# Single region (simpler, faster)
nlm-cli config set region_count 1

# Multiple regions (more realistic, slower)
nlm-cli config set region_count 5
```

## Compatibility

### Supported Platforms

- **Linux**: Ubuntu 18.04+, CentOS 7+, Debian 9+
- **macOS**: macOS 10.14+
- **Windows**: Windows 10+ (with WSL2)

### Python Compatibility

- **Python 3.8+**: Python bindings work with Python 3.8, 3.9, 3.10, 3.11, 3.12
- **Dependencies**: NumPy, SciPy (if needed)

### Compiler Requirements

- **C++17**: Modern C++ compiler with C++17 support
- **CMake**: Version 3.16 or higher

## License

The NLM framework is licensed under the MIT License. See the LICENSE file for details.

## Contact

For questions, issues, or contributions:

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://docs.nlm.ai
- **Discord Community**: https://discord.gg/nlm

## Contributors

The NLM project welcomes contributions from the community. See the CONTRIBUTING.md file for guidelines.

## Changelog

For detailed release notes and version history, see the CHANGELOG.md file.

## Acknowledgments

The NLM project is inspired by numerous computational neuroscience projects and aims to advance the field of artificial neural systems through open-source collaboration.