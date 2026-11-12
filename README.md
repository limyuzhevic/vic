# Neural Learning Machine (NLM) - Documentation

Welcome to the Neural Learning Machine (NLM) documentation. This is a comprehensive artificial developmental brain simulation project implementing real spiking neural computation with all major brain systems integrated.

## Table of Contents

1. [Introduction](#introduction)
2. [Quick Start](#quick-start)
3. [Installation](#installation)
4. [Configuration](#configuration)
5. [Command Line Interface](#command-line-interface)
6. [Advanced Usage](#advanced-usage)
7. [Experiments](#experiments)
8. [Python Bindings](#python-bindings)
9. [API Reference](#api-reference)
10. [Troubleshooting](#troubleshooting)
11. [Performance Tuning](#performance-tuning)
12. [Contributing](#contributing)

## Introduction

The Neural Learning Machine (NLM) is an experimental artificial developmental brain that implements real spiking neural computation based on:

- **Leaky Integrate-and-Fire (LIF) neurons** with realistic dynamics
- **Event-driven spike propagation** with synaptic delays
- **Spike-timing dependent plasticity (STDP)** and Hebbian learning
- **Structural plasticity** (synaptogenesis and pruning)
- **Integrated memory systems** (working, episodic, and associative memory)
- **Prediction and cognition systems** (planning, concept formation, attention)
- **Neuromodulation systems** (dopamine, curiosity, novelty detection)
- **Development system** with multiple developmental stages

This project is currently at **Phase 6 - Integrated Artificial Brain**, implementing all major brain systems with functional integration.

## Quick Start

### System Requirements

- C++20 compatible compiler (GCC 9+, Clang 10+, or MSVC 12+)
- CMake 3.16+
- Python 3.7+ (for Python bindings)
- Optional: pybind11 for Python bindings

### Build Instructions

1. Clone the repository:
```bash
git clone https://github.com/yourusername/nlm.git
 cd nlm
```

2. Create build directory and configure:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

3. Build the project:
```bash
make -j$(nproc)
```

4. Run the demo (Phase 2 - basic neural computation):
```bash
./nlm
```

5. Run Phase 3 experiments (memory formation):
```bash
./nlm_phase3_demo
```

6. Run benchmarks:
```bash
./nlm_benchmark
```

### Web Interface

The NLM CLI provides comprehensive command-line interface for interacting with the brain:

```bash
# Show help information
nlm-cli help

# Run simulation with custom parameters
nlm-cli run --timestep 0.0001 --neurons 500

# Load configuration and run experiment
nlm-cli config my_config.cfg experiment phase2_demo

# Validate configuration
config-validator configs/default.cfg --semantic
```

## Installation

### Standard C++ Installation

See the build instructions above for standard installation.

### Python Bindings

The project includes Python bindings that can be used to interact with the NLM from Python:

1. Install build dependencies:
```bash
# On Ubuntu/Debian
apt-get update && apt-get install -y python3-dev python3-pip python3-venv

# On macOS
brew install python3 pybind11
```

2. Build with Python bindings enabled:
```bash
mkdir build
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j$(nproc)
```

3. Install the Python module:
```bash
cd build
pip install pynlm
```

4. Verify installation:
```python
import pynlm
print(pynlm.__version__)
```

### Environment Setup

Set up environment variables for optimal performance:

```bash
# Set optimization level for C++ compilation
export CXXFLAGS="-O3 -march=native -flto"

# Set Python path
export PYTHONPATH="$(pwd)/build:$PYTHONPATH"

# Set NLM configuration directory
export NLM_CONFIG_DIR="$(pwd)/configs"
```

## Configuration

### Configuration File Format

NLM uses a simple key=value configuration file format. The default configuration is available at `configs/default.cfg`:

```cfg
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

### Configuration Validation

Use the configuration validator to validate your configuration files:

```bash
# Basic validation
config-validator my_config.cfg

# Schema validation
config-validator --schema-validate my_config.cfg

# Semantic validation
config-validator --semantic my_config.cfg

# Full validation with output
config-validator --strict --semantic --output validation_report.json my_config.cfg
```

### Command Line Configuration

Configuration can also be overridden via command line arguments:

```bash
# Run with custom configuration
./nlm --config my_config.cfg

# Override specific parameters
./nlm --random-seed 123 --neuron-count 2000 --timestep 0.0005

# Enable logging to file
./nlm --log-level DEBUG --log-filename simulation.log
```

## Command Line Interface

The NLM CLI provides comprehensive command-line interface for interacting with the brain:

### Available Commands

#### `run`
Run the neural simulation with current configuration.

**Options:**
- `-t, --timestep DT`: Override simulation timestep
- `-n, --neurons N`: Override neuron count
- `-d, --duration STEPS`: Override simulation duration
- `--reset-beforerun`: Reset brain before running
- `--checkpoint FILE`: Save brain state at end
- `--profile`: Enable profiling during run
- `--dry-run`: Print configuration and exit

**Examples:**
```bash
nlm-cli run --timestep 0.0001 --neurons 500
nlm-cli run --duration 5000 --max-spikes 10000
nlm-cli run --checkpoint my_brain
```

#### `config`
Load and validate a configuration file.

**Usage:**
```bash
nlm-cli config <file> [command [options]]
```

**Options:**
- `-v, --validate`: Only validate, don't load
- `--backup FILE`: Save current config to backup

**Examples:**
```bash
nlm-cli config my_config.cfg
nlm-cli config --validate my_config.cfg
```

#### `status`
Display current brain status.

**Options:**
- `-s, --summary`: Show only summary information
- `--detailed`: Show detailed system information
- `--performance`: Include performance metrics

**Examples:**
```bash
nlm-cli status
nlm-cli status --detailed
```

#### `experiment`
Run specific neural experiments.

**Available Experiments:**
- `phase2_demo`: Classic Phase 2 demonstration
- `phase3_demo`: Phase 3 memory formation
- `phase4_demo`: Phase 4 plasticity experiments
- `phase5_demo`: Phase 5 advanced plasticity
- `phase6_demo`: Phase 6 integration experiment
- `learning_experiment`: Generic learning experiment
- `connectivity_test`: Network connectivity test
- `stdp_verification`: STDP verification
- `plasticity_study`: Plasticity dynamics study

**Examples:**
```bash
nlm-cli experiment phase2_demo --timesteps 5000
nlm-cli experiment learning_experiment --seed 42 --plot
```

#### `checkpoint`
Manage brain state checkpoints.

**Usage:**
```bash
nlm-cli checkpoint <action> <file>
Actions: save, load, list, delete, info
```

**Options:**
- `--compress LEVEL`: Compression level (0-9)
- `--verify`: Verify checkpoint integrity

**Examples:**
```bash
nlm-cli checkpoint save my_checkpoint
nlm-cli checkpoint load my_checkpoint
```

#### `benchmark`
Run performance benchmarks.

**Options:**
- `-o, --output FILE`: Output file for results (JSON format)
- `--phases PHASES`: Comma-separated phases to test
- `--repeats COUNT`: Number of benchmark runs

**Examples:**
```bash
nlm-cli benchmark --output benchmark_results.json
nlm-cli benchmark --phases phase3,phase4,phase6
```

#### `profile`
Run profiling session.

**Options:**
- `--cpu`: Enable CPU profiling
- `--memory`: Enable memory profiling
- `--timeline`: Enable timeline profiling
- `--duration SECONDS`: Duration of profiling session

**Examples:**
```bash
nlm-cli profile --cpu --memory --output profile_data.json
```

#### `help`
Show help information for commands.

**Examples:**
```bash
nlm-cli help
nlm-cli help run
nlm-cli help experiment
```

## Advanced Usage

### Running with Different Phases

The NLM includes multiple experimental phases. Each phase builds upon the previous one:

#### Phase 2: Real Neural Computation
```bash
./nlm_phase2_demo
```

#### Phase 3: Memory Formation
```bash
./nlm_phase3_demo
```

#### Phase 4: Plasticity Experiments
```bash
./nlm_phase4_demo
```

#### Phase 5: Advanced Plasticity
```bash
./nlm_phase5_demo
```

#### Phase 6: Integration Experiments
```bash
./nlm_phase6_demo
./nlm_phase6_text_demo
```

### Running Experiments with Python

Use Python to run experiments and analyze results:

```python
import pynlm
import json

# Initialize brain
brain = pynlm.Brain()
brain.load_config("configs/default.cfg")

# Run experiment
results = brain.run_experiment("phase2_demo", timesteps=5000)

# Analyze results
print(f"Total spikes: {results['total_spikes']}")
print(f"Learning achieved: {results['learning_occurred']}")

# Save results
with open("experiment_results.json", "w") as f:
    json.dump(results, f, indent=2)
```

### Checkpointing and Reproducibility

Save and load brain states for reproducible experiments:

```bash
# Save brain state
nlm-cli checkpoint save experiment_checkpoint_001

# Load brain state (continue from saved state)
nlm-cli checkpoint load experiment_checkpoint_001

# Verify checkpoint integrity
nlm-cli checkpoint info experiment_checkpoint_001
```

### Performance Tuning

Optimize performance for different use cases:

#### For Real-time Applications
```bash
# Use smaller timesteps for better temporal resolution
cmake .. -DSIMULATION_TIMESTEP=0.0001

# Enable event-driven processing
cmake .. -DEVENT_DRIVEN=ON
```

#### For Research and Analysis
```bash
# Use larger timesteps for faster execution
cmake .. -DSIMULATION_TIMESTEP=0.001

# Enable detailed logging
cmake .. -DLOG_LEVEL=DEBUG
```

#### Memory-Optimized Mode
```bash
# Reduce memory footprint
cmake .. -DMEMORY_OPTIMIZED=ON

# Limit simulation size
./nlm --neuron-count 500 --region-count 1
```

## Experiments

### Available Experiments

The NLM includes several pre-defined experiments:

#### Phase 2 Demo
The classic Phase 2 demonstration showing basic neural computation with:
- LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

#### Learning Experiment
Measures synaptic changes through experience to verify learning capabilities.

#### Connectivity Test
Tests network connectivity and information propagation.

#### STDP Verification
Verifies Spike-timing dependent plasticity mechanisms.

#### Plasticity Study
Studies dynamic changes in synaptic weights over time.

### Experiment Parameters

Each experiment can be customized with parameters:

```bash
nlm-cli experiment phase2_demo --timesteps 10000 --seed 42 --plot
nlm-cli experiment learning_experiment --duration 20000 --save-results learning_curve.json
```

### Experiment Results

Experiment results are available in structured format for post-analysis:

```json
{
    "experiment_name": "phase2_demo",
    "timesteps": 10000,
    "total_spikes": 15234,
    "learning_occurred": true,
    "final_weights_mean": 0.654,
    "neuron_counts": {
        "sensory": 125,
        "internal": 625,
        "motor": 125,
        "inhibitory": 125
    },
    " plasticity_stats": {
        "strengthened": 1234,
        "weakened": 567,
        "unchanged": 1234
    }
}
```

## Python Bindings

The NLM includes Python bindings that provide full access to the neural brain functionality:

### Basic Usage

```python
import pynlm

# Create brain with configuration
brain = pynlm.Brain()
brain.load_config("configs/default.cfg")

# Initialize brain
brain.initialize()

# Run simulation
brain.step(0, 0.0)

# Get statistics
print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Neuron count: {brain.get_total_neuron_count()}")
```

### Advanced Python Features

#### Configuration Management
```python
# Get configuration value
random_seed = brain.get_config().get_or("random_seed", 42)

# Set configuration value
brain.get_config().set("neuron_count", 2000, pynlm.ConfigSource.Runtime)
```

#### Memory Systems
```python
# Access memory systems
working_memory = brain.get_working_memory()
episodic_memory = brain.get_episodic_memory()
associative_memory = brain.get_associative_memory()

# Store experience
brain.store_experience({"sensory_input": [0.1, 0.2, 0.3], "reward": 0.5})
```

#### Prediction and Cognition
```python
# Access prediction system
prediction_system = brain.get_prediction_system()
prediction_error = prediction_system.get_prediction_error()

# Access cognitive systems
planner = brain.get_planner()
concept_formation = brain.get_concept_formation()
attention = brain.get_attention()
```

#### Neuromodulation
```python
# Access neuromodulation systems
dopamine = brain.get_dopamine()
curiosity = brain.get_curiosity()
novelty = brain.get_novelty()
```

#### Development
```python
# Access development system
development_system = brain.get_development_system()
development_stage = brain.get_developmental_stage()

# Apply developmental changes
brain.develop()
```

### Python API Reference

For detailed API documentation, refer to the Python module documentation generated during installation.

## Troubleshooting

### Common Issues

#### Build Errors
**Symptom:** CMake configuration fails or compilation errors.

**Solutions:**
1. Update CMake to version 3.16 or higher
2. Check compiler compatibility
3. Verify all dependencies are installed
4. Clean build directory and rebuild:
```bash
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Runtime Errors
**Symptom:** NLM executable crashes or produces unexpected results.

**Solutions:**
1. Check configuration file syntax
2. Validate configuration with config-validator
3. Run with smaller simulation parameters
4. Check system resources (memory, CPU)
5. Review log files for error details

#### Memory Issues
**Symptom:** Memory usage exceeds system limits or crashes.

**Solutions:**
1. Reduce simulation size (`--neuron-count`, `--region-count`)
2. Enable memory optimization mode
3. Use checkpointing for large simulations
4. Monitor memory usage during execution

#### Python Bindings Issues
**Symptom:** Python bindings fail to import or use.

**Solutions:**
1. Ensure pybind11 is properly installed
2. Rebuild with Python bindings enabled:
```bash
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
```
3. Check Python path and environment variables
4. Verify Python version compatibility

### Error Messages

#### Configuration Errors
- "Missing required field": Add the missing configuration key
- "Additional property not allowed": Remove the unknown configuration key
- "Field value below minimum": Increase the value above the minimum

#### Runtime Errors
- "Failed to initialize brain": Check configuration parameters
- "Checkpoint validation failed": Verify checkpoint file integrity
- "Memory allocation failed": Check system resources

### Debugging

Enable debug logging:

```bash
# Run with debug logging
./nlm --log-level DEBUG --log-filename debug.log

# Use valgrind for memory debugging
valgrind --leak-check=full ./nlm

# Use address sanitizer for runtime errors
export CXXFLAGS="-fsanitize=address"
make clean && make
./nlm
```

### Performance Optimization

#### Profiling
```bash
# Run profiling session
nlm-cli profile --cpu --memory --output profile_data.json

# Analyze profiling results
python analyze_profile.py profile_data.json
```

#### Benchmarking
```bash
# Run comprehensive benchmark
nlm-cli benchmark --phases phase2,phase3,phase4,phase5,phase6 --output benchmark_results.json

# Analyze benchmark results
python analyze_benchmark.py benchmark_results.json
```

## Performance Tuning

### Compilation Optimizations

#### Basic Optimizations
```bash
# Standard optimization
cmake .. -DCMAKE_BUILD_TYPE=Release

# Maximum optimization with user code compilation
cmake .. -DCMAKE_BUILD_TYPE=Fast

# Enable link-time optimization
cmake .. -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=TRUE
```

#### Platform-Specific Optimizations
```bash
# Linux-specific optimizations
cmake .. -DCMAKE_CXX_FLAGS="-march=native -flto -DNDEBUG"

# macOS-specific optimizations
cmake .. .. -DCMAKE_CXX_FLAGS="-arch arm64 -O3"

# Windows-specific optimizations
cmake .. -A x64 -DCMAKE_CXX_FLAGS="/O2 /fp:fast"
```

### Runtime Optimizations

#### Memory Management
```bash
# Use memory pool for allocation
./nlm --memory-pool-enable

# Limit memory usage
./nlm --memory-limit 4GB
```

#### Simulation Control
```bash
# Use event-driven processing for real-time applications
./nlm --event-driven

# Enable checkpointing for long simulations
./nlm --checkpoint-interval 1000
```

#### Parallel Processing
```bash
# Enable parallel processing for large networks
./nlm --parallel-processing

# Set number of threads
cmake .. -DTHREADS_PER_CORE=2
```

## Contributing

### How to Contribute

1. **Code Contributions**
   - Fork the repository
   - Create a feature branch
   - Implement new functionality
   - Write tests for new features
   - Submit pull request

2. **Documentation Contributions**
   - Improve existing documentation
   - Add new examples and tutorials
   - Fix typos and formatting issues

3. **Bug Fixes**
   - Identify and report bugs
   - Fix issues with comprehensive tests
   - Update related or dependent code

### Contribution Guidelines

#### Code Style
- Follow existing C++ coding standards
- Use consistent formatting and indentation
- Add comprehensive comments for new code
- Write unit tests for new functionality
- Maintain backward compatibility

#### Documentation
- Update documentation for all public APIs
- Add usage examples and tutorials
- Include performance and memory considerations
- Document error conditions and edge cases

#### Testing
- Write comprehensive unit tests
- Add integration tests for new functionality
- Test with different configuration options
- Verify performance characteristics
- Ensure cross-platform compatibility

### Getting Help

#### Support Channels
- **GitHub Issues**: Report bugs and request features
- **Documentation**: Browse comprehensive documentation
- **Community**: Join discussion forums

#### Contact
- **Primary**: GitHub repository issues
- **Secondary**: Community forums and Discord
- **Emergency**: GitHub security advisories

### License and Copyright

```
Copyright 2024 NLM Project Contributors

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```

## Index

- [Command Line Interface](#command-line-interface)
- [Configuration](#configuration)
- [Experiments](#experiments)
- [Python Bindings](#python-bindings)
- [Performance Tuning](#performance-tuning)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)

---

Generated by NLM Documentation System
Last updated: $(date)
Version: 1.0.0
