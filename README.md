# NLM - Neural Learning Machine Documentation

**Version 2.0 - Enhanced AI Research Framework**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project designed to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Key Improvements in Version 2.0

1. **Enhanced Command-Line Interface**: Comprehensive CLI with argument parsing, help system, and configuration options
2. **Advanced Configuration System**: JSON/YAML/INI file support with validation and schema checking
3. **Performance Monitoring**: Real-time progress tracking, CPU/memory monitoring, and performance statistics
4. **Plugin Architecture**: Extensible framework for adding new components
5. **Export/Import Capabilities**: Save/load brain states with checkpointing
6. **Improved Logging**: Multi-level logging with file output
7. **Thread Safety**: Enhanced concurrent processing support
8. **Memory Optimization**: Cache-friendly data structures and SIMD optimizations

## System Architecture

### Core Components

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Environment   │───▶│   Sensory      │───▶│    Brain       │
│   Interface     │    │   Processing    │    │   (LIF Neurons) │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                              │                     │
                              ▼                     ▼
                    ┌─────────────────┐    ┌─────────────────┐
                    │    Memory       │    │    Plasticity   │
                    │    Systems      │    │    Systems      │
                    └─────────────────┘    └─────────────────┘
                              │                     │
                              ▼                     ▼
                    ┌─────────────────┐    ┌─────────────────┐
                    │   Prediction    │    │  Neuromodulation │
                    │   System        │    │  Systems        │
                    └─────────────────┘    └─────────────────┘
                              │                     │
                              ▼                     ▼
                    ┌─────────────────┐    ┌─────────────────┐
                    │   Cognition     │    │   Development   │
                    │   Systems       │    │   System        │
                    └─────────────────┘    └─────────────────┘
```

## Installation

### Prerequisites

- C++17 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings, optional)
- Optional: nlohmann/json (for JSON config support)
- Optional: yaml-cpp (for YAML config support)

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j$(nproc)

# Install (optional)
sudo make install
```

### Quick Build Commands

```bash
# For development builds
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# For production builds with optimizations
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native"
make -j$(nproc)
```

## Usage

### Basic Usage

#### Command Line Interface

The main executable `nlm` provides comprehensive functionality:

```bash
# Show help
nlm --help

# Run basic connectivity test
nlm --test-mode 1

# Run plasticity learning experiment
nlm --test-mode 2 --verbose

# Run STDP verification
nlm --test-mode 3

# Run comprehensive simulation
nlm --steps 500 --neurons 500 --plasticity both

# Load configuration from file
nlm --config myconfig.cfg --output brain.checkpoint

# Custom parameters
nlm --neurons 1000 --regions 2 --steps 2000 --seed 12345
```

#### Configuration Files

NLM supports multiple configuration file formats:

**JSON Configuration (`config.json`):**
```json
{
    "neuron_count": 1000,
    "region_count": 1,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    "plasticity_mode": "both",
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "connection_probability": 0.1
}
```

**YAML Configuration (`config.yaml`):**
```yaml
# NLM Configuration File
# Generated on $(date)

neuron_count: 1000
region_count: 1
simulation_timestep: 0.001
random_seed: 42
plasticity_mode: both
stdp:
  ltp_weight: 0.02
  ltd_weight: 0.015
  tau: 20.0
structural_plasticity:
  synaptogenesis_rate: 0.0001
  pruning_rate: 0.00001
synaptic:
  connection_probability: 0.1
```

**INI Configuration (`config.ini`):**
```ini
# NLM Configuration File
# Generated on $(date)

[General]
neuron_count = 1000
region_count = 1
simulation_timestep = 0.001
random_seed = 42
plasticity_mode = both
stdp.ltp_weight = 0.02
stdp.ltd_weight = 0.015
structural_plasticity.synaptogenesis_rate = 0.0001
structural_plasticity.pruning_rate = 0.00001
synaptic.connection_probability = 0.1
```

### Advanced Usage

#### Using Python Bindings

Install Python bindings for programmatic access:

```bash
# Build with Python support
cmake .. -DPYTHON_EXECUTABLE=/usr/bin/python3

# Install Python package
pip install ./build/pynlm*
```

**Example Python Usage:**
```python
import pynlm

# Create brain with configuration
config = {
    'neuron_count': 1000,
    'plasticity_mode': 'both',
    'simulation_timestep': 0.001
}

brain = pynlm.Brain(config)
brain.initialize()

# Run simulation
for step in range(100):
    # Inject sensory input
    input_pattern = [0.1, 0.2, 0.3, 0.4, 0.5]
    brain.receive_sensory_input(input_pattern)
    
    # Step simulation
    brain.step(step, step * 0.001)
    
    # Monitor performance
    if step % 10 == 0:
        print(f"Step {step}: {brain.get_total_spike_count()} spikes")

# Save brain state
brain.save("trained_brain.checkpoint")
```

#### Plugin Development

NLM supports plugin architecture for extending functionality:

**Plugin Interface:**
```cpp
class NLMPlugin {
public:
    virtual ~NLMPlugin() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize(std::shared_ptr<Brain> brain) = 0;
    virtual void shutdown() = 0;
    virtual void update(Timestep step, Timestamp time) = 0;
};
```

**Plugin Example:**
```cpp
class CustomPlugin : public NLMPlugin {
public:
    std::string getName() const override { return "CustomAnalytics"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    bool initialize(std::shared_ptr<Brain> brain) override {
        this->brain = brain;
        // Custom initialization
        return true;
    }
    
    void update(Timestep step, Timestamp time) override {
        // Custom update logic
        if (brain) {
            auto metrics = calculateMetrics();
            logPerformance(metrics);
        }
    }
    
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Brain> brain;
    
    PerformanceMetrics calculateMetrics() {
        PerformanceMetrics metrics;
        metrics.totalSpikeCount = brain->get_totalSpikeCount();
        metrics.activeNeuronCount = brain->get_activeNeuronCount();
        // ... more metrics
        return metrics;
    }
    
    void logPerformance(const PerformanceMetrics& metrics) {
        // Custom logging implementation
    }
};
```

#### Performance Monitoring

Track simulation performance with the monitoring system:

```bash
# Enable verbose monitoring
nlm --verbose --steps 1000

# Real-time monitoring with progress bar
nlm --steps 500 --plasticity both

# Save performance statistics to file
nlm --steps 1000 --output brain.checkpoint
# Checkpoints saved to: nlm_stats.csv
```

**Performance Metrics Available:**

- **Simulation Time**: Total elapsed time in seconds
- **CPU Usage**: Current CPU utilization percentage
- **Memory Usage**: Memory consumption in MB
- **Spike Statistics**: Total spikes and firing rate
- **Network Activity**: Active/firing neuron counts
- **Plasticty Metrics**: E/I ratio and average firing rates
- **Checkpoint Statistics**: Number of saved checkpoints

## Development

### Code Structure

```
NLM/
├── src/
│   ├── core/              # Core utilities
│   │   ├── Config/        # Configuration system
│   │   ├── Logger/        # Logging system
│   │   ├── Random/        # Random number generation
│   │   └── SimulationClock/ # Time management
│   ├── brain/             # Neural components (integrated)
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity rules
│   ├── memory/            # Memory systems (integrated)
│   ├── prediction/        # Prediction systems (integrated)
│   ├── cognition/         # Cognitive mechanisms (integrated)
│   ├── neuromodulation/    # Neuromodulators (integrated)
│   ├── development/       # Developmental system
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor system
│   ├── environment/       # Environment interface
│   ├── experiments/       # Experiment framework (Phase 6)
│   └── visualization/     # Visualization
│   └── main.cpp           # Main executable with CLI
├── include/              # Public headers
├── docs/                 # Documentation
├── configs/              # Default configuration files
├── examples/             # Example programs
├── tests/                # Test suite
└── CMakeLists.txt        # Build system
```

### Building Examples

Run the example programs to understand different aspects of the system:

```bash
# Example 1: Basic connectivity test
./examples/basic_test

# Example 2: Learning experiment
./examples/learning_demo

# Example 3: Visualization
./examples/visualization
```

### Testing

Run the test suite to ensure everything works correctly:

```bash
# Build and run tests
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON
make -j$(nproc)
ctest

# Or run with verbose output
./nlm --test-mode 2 --verbose
```

## Troubleshooting

### Common Issues

**1. Build Failures**

**Symptoms**: Compiler errors during build
**Solutions**:
- Ensure C++17 support: `g++ --version` should show C++17 support
- Install missing dependencies: `sudo apt install build-essential cmake`
- Check for conflicting packages with `ldd`

**2. Runtime Errors**

**Symptoms**: Segmentation faults or crashes
**Solutions**:
- Enable debug symbols: `cmake .. -DCMAKE_BUILD_TYPE=Debug`
- Check for memory leaks with `valgrind`
- Verify configuration file paths and permissions

**3. Performance Issues**

**Symptoms**: Slow simulation or high memory usage
**Solutions**:
- Reduce simulation size: `--neurons 100 --steps 100`
- Disable verbose output: `--quiet`
- Use optimized build: `cmake .. -DCMAKE_BUILD_TYPE=Release`

**4. Configuration File Issues**

**Symptoms**: Failed to load configuration
**Solutions**:
- Check file syntax and permissions
- Use supported formats: JSON, YAML, or INI
- Test with default config: `nlm --config configs/default.cfg`

### Getting Help

- **Documentation**: See the `docs/` directory for detailed documentation
- **Examples**: Check the `examples/` directory for sample programs
- **Issues**: Report bugs at [GitHub Repository]
- **Discussions**: Join the NLM research community

## Migration Guide

### From Version 1.0 to 2.0

#### Breaking Changes

1. **Configuration Format**: Changed from custom format to JSON/YAML/INI
2. **Command Line Interface**: Enhanced CLI with new options and help system
3. **File Structure**: Reorganized with plugin architecture support
4. **API Changes**: Improved memory management and thread safety

#### Migration Steps

1. Update configuration files to use supported formats
2. Update command line usage to new CLI options
3. Review deprecated API calls and update accordingly
4. Update build system to use new CMake configuration

#### Example Migration

**Old configuration (v1.0):**
```
# config.txt
neurons=1000
regions=1
timestep=0.001
plasticity=both
```

**New configuration (v2.0 - JSON):**
```json
{
    "neurons": 1000,
    "regions": 1,
    "timestep": 0.001,
    "plasticity": "both"
}
```

**Old command line (v1.0):**
```bash
./nlm -n 1000 -r 1 -t 0.001 -p both
```

**New command line (v2.0):**
```bash
nlm --neurons 1000 --regions 1 --timestep 0.001 --plasticity both
```

## License

MIT License

Copyright (c) 2024 NLM Research Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## Acknowledgments

The NLM project was developed as part of an experimental artificial
developmental brain research initiative. Key contributions include:

- Original neural computation framework design
- LIF neuron dynamics implementation
- Event-driven spike propagation with synaptic delays
- Multiple plasticity rules (STDP, Hebbian, structural)
- Integrated memory, prediction, cognition, and neuromodulation systems
- Developmental stages affecting plasticity rates
- Checkpoint save/load functionality

Special thanks to the open-source community for providing foundational
libraries and tools.
