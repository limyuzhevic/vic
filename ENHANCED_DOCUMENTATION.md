# NLM Enhanced Documentation - Phase 6 Advanced Features

## Overview

This document covers the enhanced features in NLM Phase 6, including:

1. Enhanced Command Line Interface (CLI)
2. Advanced Configuration Options
3. Performance Benchmarking
4. Debugging and Monitoring Tools
5. Integration with Advanced Python Bindings
6. Installation and Setup Guides

## Enhanced Command Line Interface

### New CLI Structure

The enhanced NLM CLI now provides:

- **Multiple execution modes**: demo, experiment, benchmark, test
- **Comprehensive help system**: detailed usage information
- **Advanced configuration options**
- **Performance monitoring and profiling**
- **Flexible logging system**

### Usage Examples

#### Basic Usage

```bash
# Run enhanced demo (default)
nlm demo

# Run with custom configuration
nlm demo --neurons=500 --steps=5000

# Run with verbose output
nlm demo --verbose

# Run benchmark mode
nlm benchmark --neurons=1000 --steps=10000

# Run integration experiment
nlm experiment --checkpoint=./checkpoints
```

#### Help and Information

```bash
nlm --help          # Show complete help
nlm --version        # Show version information
nlm demo --help      # Show mode-specific help
```

### Advanced CLI Features

#### Configuration Options

| Option | Description | Default |
|--------|-------------|---------|
| `--neurons=<N>` | Number of neurons | 1000 |
| `--steps=<N>` | Number of simulation steps | 1000 |
| `--timestep=<T>` | Simulation timestep | 0.001 |
| `--loglevel=<L>` | Log level (trace, debug, info, warn, error) | info |
| `--checkpoint=<path>` | Checkpoint file path | (auto-generated) |
| `--checkpointing` | Enable checkpointing | disabled |
| `--no-replay` | Disable replay system | enabled |
| `--no-development` | Disable development system | enabled |
| `--verbose` | Enable verbose output | disabled |
| `--quiet` | Suppress non-error output | disabled |

#### Mode-Specific Options

**Demo Mode (`demo`):**
- Runs the classic NLM demonstration with enhanced features
- Includes learning experiments and plasticity verification

**Experiment Mode (`experiment`):**
- Runs Phase 6 integration experiment
- Tests all brain systems integration
- Enables checkpointing, replay, and development

**Benchmark Mode (`benchmark`):**
- Runs performance benchmark with configurable parameters
- Measures steps per second and memory usage
- Includes warm-up and steady-state testing

**Test Mode (`test`):**
- Runs basic unit tests
- Verifies core system functionality
- Fast and minimal output mode

## Advanced Configuration

### Configuration File Formats

NLM supports multiple configuration file formats:

#### 1. Simple Key=Value Format (`.cfg`)

```
# Neuron configuration
neuron_count = 1000
region_count = 1
connection_probability = 0.15

# Plasticity configuration
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Development configuration
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002

# Neuromodulation configuration
novelty_threshold = 0.1
curiosity_decay = 0.001
```

#### 2. Command Line Arguments

```bash
nlm demo \
  --set=neuron_count=2000 \
  --set=stdp_ltp_weight=0.025 \
  --set=synaptogenesis_rate=0.0003
```

### Advanced Configuration Keys

#### Core Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `neuron_count` | int64_t | 1000 | Total number of neurons |
| `region_count` | int64_t | 1 | Number of neural regions |
| `connection_probability` | float | 0.1 | Probability of synaptic connections |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds |
| `random_seed` | int64_t | 42 | Random number generator seed |

#### Plasticity Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.01 | STDP LTP learning rate |
| `stdp_ltd_weight` | float | 0.012 | STDP LTD learning rate |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `hebbian_learning_rate` | float | 0.001 | Hebbian learning rate |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse removal |

#### Neuromodulation Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `novelty_threshold` | float | 0.1 | Threshold for novelty detection |
| `curiosity_decay` | float | 0.001 | Decay rate for curiosity |
| `dopamine_scale` | float | 1.0 | Scale factor for dopamine effects |
| `prediction_error_weight` | float | 1.0 | Weight for prediction error learning |

#### Integration System Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `replay_interval` | size_t | 100 | Steps between memory replay |
| `consolidation_interval` | size_t | 1000 | Steps between memory consolidation |
| `working_memory_capacity` | size_t | 100 | Capacity of working memory |
| `max_episodes` | size_t | 1000 | Maximum episodic memory episodes |

## Performance Benchmarking

### Benchmark Mode Features

The enhanced benchmark mode provides:

- **Warm-up phase**: Stabilizes system before measurements
- **Steady-state testing**: Measures sustained performance
- **Comprehensive metrics**: Steps per second, memory usage, spike rates
- **Configurable parameters**: Neuron count, step count, timestep

### Benchmark Examples

```bash
# Basic benchmark
nlm benchmark --neurons=500 --steps=10000

# High-performance benchmark
nlm benchmark --neurons=2000 --steps=20000 --timestep=0.0005

# With checkpointing
nlm benchmark --neurons=1000 --steps=5000 --checkpointing

# Verbose benchmark
nlm benchmark --neurons=1000 --steps=10000 --verbose
```

### Benchmark Output Format

```
=== NLM Performance Benchmark ===
Warm-up phase (100 steps)...
Benchmark phase (10000 steps)...

=== Benchmark Results ===
Benchmark time: 12.345s
Steps per second: 810.0
Total spikes: 125000
Average firing rate: 125.0
Neurons: 1000
Steps: 10000
Total execution time: 13.456s
```

## Debugging and Monitoring

### Logging System

The enhanced logging system supports multiple levels:

| Level | Description |
|-------|-------------|
| trace | Detailed debugging information |
| debug | Development debugging information |
| info | General information and status messages |
| warn | Warning conditions |
| error | Error conditions and failures |

### Log Level Examples

```bash
# Enable trace logging
nlm demo --loglevel=trace

# Enable debug logging
nlm demo --loglevel=debug

# Enable only error logging
nlm demo --loglevel=error --quiet
```

### Debug Mode Features

Debug mode provides:

- **Enhanced error messages**: Detailed stack traces and context
- **Memory profiling**: Track memory allocation and usage
- **Performance monitoring**: Real-time system metrics
- **Step-by-step output**: Detailed progress tracking

## Python Bindings and Advanced Usage

### Enhanced Python API

The enhanced Python bindings provide:

1. **Advanced brain configuration**: Direct parameter access
2. **Memory system monitoring**: Real-time memory statistics
3. **Neuromodulation control**: Fine-grained neuromodulator management
4. **Performance metrics**: Comprehensive brain performance data
5. **Custom experiment framework**: Built-in support for complex experiments

### Python Examples

#### Advanced Brain Configuration

```python
import pynlm
import json

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Set advanced parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.025)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.01)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Expected synapses: {int(brain.getTotalNeuronCount() * 0.15)}")
```

#### Memory System Monitoring

```python
import pynlm

# Create brain with monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation while monitoring memory
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        
        if working_memory:
            print(f"Step {step}: Working memory traces: {working_memory.getActiveTraces()}")
        
        if episodic_memory:
            print(f"Step {step}: Episodes stored: {episodic_memory.getEpisodeCount()}")
        
        # Monitor neuromodulation
        dopamine = brain.getDopamine()
        if dopamine:
            print(f"Step {step}: Dopamine level: {dopamine.getLevel():.3f}")
```

#### Performance Analysis

```python
import pynlm
import time

def benchmark_brain(neuron_counts, steps):
    """Benchmark brain performance across different neuron counts"""
    results = []
    
    for neuron_count in neuron_counts:
        print(f"Benchmarking {neuron_count} neurons...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        
        start_time = time.time()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
        
        end_time = time.time()
        duration = end_time - start_time
        
        avg_firing_rate = brain.getAverageFiringRate()
        total_spikes = brain.getTotalSpikeCount()
        
        results.append({
            'neurons': neuron_count,
            'time': duration,
            'steps_per_second': steps / duration,
            'avg_firing_rate': avg_firing_rate,
            'total_spikes': total_spikes
        })
        
        print(f"  Time: {duration:.2f}s")
        print(f"  Steps/sec: {steps/duration:.0f}")
        print(f"  Avg firing rate: {avg_firing_rate:.2f}")
    
    return results

# Run benchmark
neuron_counts = [500, 1000, 2000, 5000]
steps = 1000
results = benchmark_brain(neuron_counts, steps)
```

## Installation and Setup

### Prerequisites

- C++ compiler with C++20 support (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)
- pybind11 2.11.0+ (for Python bindings)

### Building from Source

#### Standard Build

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Build with Python Bindings

```bash
# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

#### Build Options

```bash
# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Custom compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

### Running Tests

#### Unit Tests

```bash
# Run unit tests
ctest --output-on-failure

# Run specific tests
ctest -R test_brain
ctest -R test_plasticity
```

#### Integration Tests

```bash
# Run Phase 6 demo
./nlm_phase6_demo

# Run enhanced demo
./nlm demo

# Run benchmark
./nlm benchmark --neurons=1000
```

### Configuration Files

#### Default Configuration

Create `configs/default.cfg`:

```
# NLM Default Configuration
# ===================================

# Core brain parameters
neuron_count = 1000
region_count = 1
connection_probability = 0.15
simulation_timestep = 0.001
random_seed = 42

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
hebbian_learning_rate = 0.001
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002

# Neuromodulation parameters
novelty_threshold = 0.1
curiosity_decay = 0.001
dopamine_scale = 1.0
prediction_error_weight = 1.0

# Integration system parameters
replay_interval = 50
consolidation_interval = 500
working_memory_capacity = 100
max_episodes = 1000
```

### Troubleshooting

#### Common Issues

**Issue: Build fails with compiler errors**

```bash
# Check compiler version
g++ --version

# Install required dependencies
apt-get install build-essential cmake
```

**Issue: Python bindings not found**

```bash
# Install build dependencies
pip install pybind11 scikit-build-core

# Build with Python support
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Issue: Segmentation fault**

```bash
# Enable debug mode
nlm demo --debug

# Check configuration
cat configs/default.cfg
```

**Issue: No output or silent operation**

```bash
# Disable quiet mode
nlm demo --verbose

# Check log level
nlm demo --loglevel=debug
```

## Advanced Features for Power Users

### Custom Experiment Framework

The NLM framework provides:

1. **Custom experiment builder**: Create complex multi-stage experiments
2. **Parameter sweeps**: Systematically vary parameters to study effects
3. **Machine learning integration**: Connect NLM with traditional ML algorithms
4. **Visualization tools**: Real-time monitoring and plotting

### Performance Optimization

Advanced optimization techniques:

1. **Memory pool management**: Reduce memory fragmentation
2. **SIMD vectorization**: Use single instruction multiple data instructions
3. **Parallel processing**: Multi-threading for compute-intensive operations
4. **Event-driven simulation**: Minimize unnecessary computations

### Extension Points

1. **Plugin system**: Add custom neural dynamics
2. **Custom plasticity rules**: Implement novel learning mechanisms
3. **External data integration**: Feed real-world data into the brain
4. **Hardware acceleration**: GPU support for large-scale simulations

## References

1. **Phase 6 Integration Documentation**: Complete details on Phase 6 implementation
2. **Python Bindings API**: Comprehensive Python API documentation
3. **Performance Tuning Guide**: Advanced performance optimization techniques
4. **Troubleshooting Guide**: Common issues and solutions

## Contact and Support

For questions and support:

- **Documentation**: https://docs.nlm-project.org
- **Source Code**: https://github.com/nlm-project
- **Issue Tracker**: https://github.com/nlm-project/issues
- **Discussion Forum**: https://github.com/nlm-project/discussions

---

*Last updated: $(date +'%Y-%m-%d')*
*Version: 0.1.0 (Phase 6 Enhanced Integration)*
