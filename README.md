# NLM Phase 2: Advanced Neural Computation

## Overview

This document provides comprehensive information about NLM Phase 2: Real Neural Computation, an experimental artificial developmental brain system implementing real spiking neural computation.

## Key Features

### Core Neural Components
- **Leaky Integrate-and-Fire (LIF) neurons** with realistic biophysical dynamics
- **Event-driven spike propagation** with synaptic delays
- **STDP and Hebbian plasticity** rules for learning
- **Structural plasticity** (synaptogenesis/pruning) for network development
- **Neuromodulation** systems (dopamine, acetylcholine, serotonin)
- **Memory systems** (working, episodic, semantic, procedural, associative)
- **Cognition modules** (concept formation, neural planning)

### Advanced User Features
- **Comprehensive CLI interface** with 20+ command-line options
- **Multiple experiment modes** for different research scenarios
- **Performance profiling and optimization** tools
- **Advanced configuration management** for custom network architectures
- **Batch processing capabilities** for large-scale experiments
- **Debugging and visualization tools** for analysis

### Technical Specifications
- **C++20** implementation with full optimization support
- **Python bindings** via pybind11 for integration with scientific computing ecosystem
- **SIMD vectorization** and parallel processing for performance
- **Memory pool management** and sparse connectivity
- **Checkpointing and replay** for reproducible experiments

## Command-Line Interface

### Basic Usage
```bash
nlm                    # Run basic test suite (default)
nlm --help            # Show help information
nlm --version         # Show version information
nlm -v                # Increase verbosity
nlm --debug           # Enable debug mode
```

### Experiment Modes
```bash
nlm -e basic          # Run basic connectivity and plasticity tests
nlm -e stdp           # Run STDP-specific verification
nlm -e learning        # Run advanced learning experiment
nlm -e advanced       # Run comprehensive advanced testing suite
nlm -e custom         # Run custom experiment sequence
nlm -e phase3         # Run Phase 3 cognitive experiments
nlm -e phase4         # Run Phase 4 developmental experiments
nlm -e phase6         # Run Phase 6 integration experiment
```

### Configuration and Output
```bash
nlm -c myconfig.cfg           # Load configuration from file
nlm -o results.json           # Save output to JSON file
nlm --set neuron_count=1000   # Set custom configuration value
nlm --set connection_probability=0.2
nlm --profile                 # Enable performance profiling
nlm --steps 5000              # Set maximum simulation steps
nlm --timestep 0.0005         # Set simulation timestep
nlm --threads 8               # Set thread count for parallel processing
```

### Advanced Usage
```bash
# Learning benchmark with profiling
nlm -e learning -s 3000 --profile --debug

# Advanced testing with custom configuration
nlm -e advanced --set neuron_count=2000 --set connection_probability=0.3

# Batch processing for large experiments
nlm -e custom --set batch_size=100 --set num_batches=50

# Phase 6 integration experiment
nlm -e phase6 --set enable_checkpointing=true --set enable_replay=true
```

## Configuration Files

### Default Configuration
The system uses `configs/default.cfg` as the default configuration file with the following settings:

```ini
# Core simulation parameters
random_seed=42
simulation_timestep=0.001
neuron_count=500
region_count=1
connection_probability=0.15

# STDP parameters
stdp_ltp_weight=0.02
stdp_ltd_weight=0.015
stdp_tau=20.0

# Structural plasticity parameters
synaptogenesis_rate=0.0001
pruning_rate=0.00001
```

### Custom Configuration
Users can create custom configuration files with additional parameters:

```ini
# Advanced network parameters
layer_count=3
neurons_per_layer=500,300,200
use_recurrent_connections=true

# Learning parameters
learning_rate=0.01
momentum=0.9
weight_decay=0.0001
dropout_rate=0.1
use_batch_norm=true

# Neuromodulation
enable_dopamine=true
enable_acetylcholine=true
enable_serotonin=true
reward_learning_rate=0.05
prediction_error_rate=0.03
```

## Testing and Validation

### Built-in Test Suite
The system includes comprehensive tests covering:

1. **Neuron Tests** (`tests/test_neuron.cpp`)
   - Neuron creation and initialization
   - Membrane potential dynamics
   - Firing state transitions
   - Refractory period behavior

2. **Synapse Tests** (`tests/test_synapse.cpp`)
   - Synapse creation and weight management
   - STDP rule implementation
   - Short-term plasticity
   - Eligibility traces

3. **System Integration Tests**
   - Brain initialization and configuration
   - Plasticity learning experiments
   - STDP verification
   - Memory integration tests

### Running Tests
```bash
# Build and run all tests
mkdir build
cd build
cmake ..
make -j$(nproc)
./nlm --test-all
```

## Performance Optimization

### Profiling Options
```bash
# Enable comprehensive performance profiling
nlm --profile --debug

# Profile specific experiment modes
nlm -e learning --profile
nlm -e advanced --profile
```

### Performance Monitoring
The system provides real-time performance monitoring including:
- Processing speed (steps per second)
- Memory usage tracking
- Spike generation rates
- Network activity metrics

### Optimization Features
- **SIMD vectorization** for mathematical operations
- **Parallel processing** for multi-threaded execution
- **Memory pool management** for efficient allocation
- **Sparse connectivity** for large-scale networks

## Debugging and Visualization

### Debug Mode
```bash
# Enable comprehensive debugging
nlm --debug --verbose

# Export debug logs to file
nlm --debug --output debug.json
```

### Visualization
```bash
# Enable network visualization
nlm -e advanced --set enable_visualization=true

# Save visualization data
nlm -e advanced --set visualization_output=network_data.csv
```

### Debug Features
- **Real-time neuron monitoring**
- **Synaptic activity tracking**
- **Network state snapshots**
- **Performance profiling**
- **Memory usage analysis**
- **Error and warning logging**

## Advanced User Guide

### Network Architecture Customization
Users can customize network architecture using command-line options:

```bash
# Create custom network topology
nlm --set layer_count=4 --set neurons_per_layer=1000,600,400,200
nlm --set use_recurrent_connections=true --set connection_probability=0.25
```

### Learning Experiment Configuration
```bash
# Configure learning experiments
nlm -e learning --set learning_rate=0.05 --set momentum=0.95
nlm -e learning --set enable_neuromodulation=true --set reward_weight=0.1
```

### Batch Processing
```bash
# Run batch experiments for statistical analysis
nlm -e custom --set batch_size=100 --set num_batches=50 --set output_file=batch_results.csv
```

### Simulation Control
```bash
# Fine-tune simulation parameters
nlm --timestep 0.0001 --steps 10000
nlm --threads 16 --set enable_checkpointing=true
```

## Integration and API

### Python Bindings
The system includes Python bindings via pybind11 for integration with scientific computing ecosystem:

```python
import pynlm

# Create NLM instance
nlm = pynlm.NLM()

# Configure network
nlm.set_neuron_count(1000)
nlm.set_connection_probability(0.15)

# Run simulation
nlm.run(steps=5000)

# Access results
print("Total spikes:", nlm.get_total_spike_count())
print("Final firing rate:", nlm.get_firing_rate())
```

### C++ API
For advanced users, the NLM system provides a C++ API for custom implementations:

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

// Create brain configuration
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 1000);
config->set("connection_probability", 0.15);

// Initialize brain
auto brain = std::make_shared<nlm::Brain>(config);
brain->initialize();

// Run simulation
for (size_t step = 0; step < 1000; ++step) {
    brain->step(step, 0.001);
}

// Access brain state
std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
```

## Troubleshooting

### Common Issues and Solutions

#### 1. Configuration File Not Found
```bash
# Solution: Create configuration file
nlm --set neuron_count=500 --set connection_probability=0.15
```

#### 2. Memory Usage Exceeds Limits
```bash
# Solution: Reduce network size
nlm --set neuron_count=200 --set region_count=1
```

#### 3. Performance Issues
```bash
# Solution: Enable optimizations
nlm --profile --set enable_vectorization=true
```

#### 4. Debug Information Missing
```bash
# Solution: Enable debug mode
nlm --debug --verbose
```

### Error Codes
- **Exit code 1**: Brain initialization failed
- **Exit code 2**: Configuration error
- **Exit code 3**: Test suite failed
- **Exit code 4**: Memory allocation failed
- **Exit code 5**: Network connectivity error

## References

1. **Documentation**: https://nlm.readthedocs.io
2. **Repository**: https://github.com/nlm-project/nlm
3. **API Reference**: https://nlm.readthedocs.io/api/
4. **Tutorials**: https://nlm.readthedocs.io/tutorials/
5. **Examples**: https://github.com/nlm-project/nlm/tree/main/examples

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Contact

For technical support and feature requests:
- Email: support@nlm-project.org
- GitHub: https://github.com/nlm-project/nlm/issues
- Discord: https://discord.gg/nlm-project

## Version History

- **Phase 1.0**: Initial neural computation framework
- **Phase 2.0**: Real spiking neural computation with advanced plasticity
- **Phase 2.1**: Advanced CLI and performance optimization
- **Phase 2.2**: Comprehensive debugging and visualization tools

Current version: 0.1.0