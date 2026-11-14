# NLM - Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Key Improvements in This Version

### 1. Unified Command-Line Interface (NEW)
- **Consistent argument parsing** across all demo executables
- **Help system** with detailed usage information
- **Phase selection** for easy switching between different experimental phases
- **Configuration file support** for saving and loading settings

### 2. Enhanced CLI Architecture
- **CLIParser class**: Robust command-line argument parsing with validation
- **PhaseSelector class**: Unified interface for selecting and executing different phases
- **Comprehensive help system**: Detailed documentation for all options
- **Type-safe argument handling**: Support for flags, values, choices, and ranges

### 3. Improved Code Organization
- **Modular design**: Separation of CLI logic into dedicated classes
- **Extensible architecture**: Easy to add new phases and features
- **Better error handling**: Comprehensive error checking and recovery
- **Performance monitoring**: Built-in profiling and statistics

### 4. Advanced Features
- **Multi-phase execution**: Support for Phase 1 through Phase 6
- **Performance metrics**: Detailed tracking of simulation statistics
- **Debugging tools**: Advanced debugging and visualization capabilities
- **Python bindings integration**: Enhanced Python API for external access

### 5. Better Documentation
- **Comprehensive examples**: Detailed usage examples for all features
- **Command-line reference**: Complete documentation of all options
- **Phase guides**: Specific documentation for each experimental phase

## Building

### Prerequisites
- C++20 compatible compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)
- pybind11 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
mkdir build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4

# Run the main demo (Phase 2)
./nlm

# Run specific phase demos
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase5_demo
./nlm_phase6_demo

# Run benchmark
./nlm_benchmark
```

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# Build with Python bindings (requires pybind11)
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make
```

## Usage

### Basic Usage

```bash
# Run the main Phase 2 demo (default)
nlm

# Run Phase 3 - World Interaction
nlm --phase=3

# Run Phase 4 - Emerging Cognition
nlm --phase=4

# Run Phase 6 - Integration Test
nlm --phase=6

# Show help information
nlm --help

# Show version information
nlm --version

# Enable verbose output
nlm --verbose

# Enable debug mode
nlm --debug
```

### Phase-Specific Arguments

#### Phase 3: World Interaction

```bash
# Run with custom episode and step counts
nlm --phase=3 10 500  # 10 episodes, 500 steps each
nlm --phase=3 --episodes=20 --steps-per-episode=300
```

#### Phase 4: Emerging Cognition

```bash
# Run with custom trial count
nlm --phase=4 100  # 100 trials
nlm --phase=4 --trials=200
```

#### Configuration Files

Create a configuration file `my_config.cfg`:

```
# NLM Configuration File
neuron_count = 1000
region_count = 2
connection_probability = 0.1
random_seed = 42
simulation_timestep = 0.001
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
```

Load the configuration:

```bash
nlm --config=my_config.cfg
```

### Command-Line Reference

```bash
# Global options
-h, --help              Show help message
-v, --version           Show version information
-V, --verbose           Enable verbose output
-d, --debug             Enable debug mode
-p, --phase <phase>     Select phase (1, 2, 3, 4, 5, 6, benchmark, all)
-c, --config <file>     Load configuration from file
-l, --log-level <level> Set log level (debug, info, warn, error)

# Phase 3 specific
<episodes>             Number of episodes (default: 5)
<steps>                Number of steps per episode (default: 200)

# Phase 4 specific
<trials>                Number of trials (default: 50)

# Custom options (defined by specific phases)
--neuron-count <n>     Number of neurons
--region-count <n>     Number of regions
--connection-prob <p>  Connection probability (0.0-1.0)
```

## Phase Summary

### Phase 1 (Complete) - Project Skeleton
- Core types and configuration
- Neural interfaces
- Basic building blocks

### Phase 2 (Complete) - Real Neural Computation
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

### Phase 3 (Complete) - World Interaction
- World simulation with agents
- Sensory processing and motor output
- Reward prediction error
- Developmental stages

### Phase 4 (Complete) - Emerging Cognition
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete) - Performance Optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system
- Brain-personality sandbox

### Phase 6 (Complete) - Final Integration
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional

## Examples

### Simple Brain Simulation

```bash
nlm --phase=2 --verbose
```

### World Interaction Demo

```bash
nlm --phase=3 10 500  # 10 episodes, 500 steps each
```

### Cognition Experiments

```bash
nlm --phase=4 100  # 100 trials
```

### Integration Test

```bash
nlm --phase=6 --verbose --debug
```

### Performance Benchmark

```bash
nlm_benchmark --trials=1000 --neuron-count=5000
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |

## Performance Monitoring

The enhanced NLM includes built-in performance monitoring:

- **Real-time statistics**: Neuron firing rates, synaptic activity, memory usage
- **Memory tracking**: Working memory traces, episodic memory storage
- **Timing information**: Wall-clock time, simulation time, CPU usage
- **Debug output**: Detailed logging when `--debug` is enabled

## Python Bindings

The project includes Python bindings for easy integration:

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete! Neurons: {brain.getTotalNeuronCount()}")
```

## Advanced Features

### 1. Performance Monitoring

The CLI system includes comprehensive performance monitoring:

```bash
nlm --verbose --debug
```

### 2. Configuration Management

Save and load configurations:

```bash
nlm --config=my_config.cfg
# ... run simulation ...
nlm --config=saved_config.cfg --load
```

### 3. Phase Comparison

Compare performance across different phases:

```bash
# Run Phase 2 for comparison
nlm --phase=2 --verbose > phase2_results.txt

# Run Phase 3 for comparison
nlm --phase=3 5 200 > phase3_results.txt
```

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

## Further Reading

- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Final Phase 6 audit report
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PERFORMANCE.md](docs/PERFORMANCE.md) - Performance analysis

## License

MIT

## Authors

Research project — See docs for scientific background.
