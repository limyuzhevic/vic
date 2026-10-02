# NLM Project Documentation

## Overview

The NLM (Neural Learning Machine) project is a sophisticated neural simulation framework implementing real spiking neural computation with event-driven dynamics and integrated cognitive systems. This Phase 2 implementation focuses on real neural computation with Leaky Integrate-and-Fire (LIF) neurons, STDP and Hebbian plasticity, structural plasticity, and neuromodulation.

## Key Features

### Core Neural Computation
- **LIF Neuron Dynamics**: Real spiking neural dynamics with membrane potential integration
- **Event-Driven Spike Propagation**: Asynchronous spike transmission with synaptic delays
- **Plasticity Rules**: STDP, Hebbian learning, and structural plasticity
- **Neuromodulation**: Dopamine, curiosity, and novelty detection

### Integrated Systems
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Prediction Systems**: Forward and inverse models
- **Cognitive Systems**: Planning, concept formation, attentional selection
- **Development Systems**: Developmental stages and critical periods

### World Interaction
- **Sensory Processing**: Vision, touch, proprioception, and internal signals
- **Motor Control**: Action execution and reward-based learning
- **Physical Environment**: 2D world simulation with physics

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    NLM Main Framework                      │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────┐  ┌───────────────────────┐            │
│  │   Configuration │  │      Core Logger      │            │
│  └─────────────────┘  └───────────────────────┘            │
│  ┌─────────────────┐  ┌───────────────────────┐            │
│  │   Brain System  │  │   Spike System        │            │
│  └─────────────────┘  └───────────────────────┘            │
│  │                 │                                  │
│  │  ┌─────────────┐│  ┌─────────────────────────────┐      │
│  │  │  Neuron    ││  │  ┌─────────────────────────┐ │      │
│  │  │  Synapse   ││  │  │   Plasticity System    │ │      │
│  │  │  Region    ││  │  └─────────────────────────┘ │      │
│  │  │  Population ││  │  ┌─────────────────────────┐ │      │
│  │  └─────────────┘│  │  │     Agent System      │ │      │
│  └─────────────────┘  └─────────────────────────┘      │
│                                                         │
│  ┌─────────────────┐  ┌───────────────────────┐  ┌───────┐│
│  │   World System  │  │   Experiment System   │  │   IO  ││
│  └─────────────────┘  └───────────────────────┘  └───────┘│
└─────────────────────────────────────────────────────────────┘
```

## Quick Start Guide

### Installation
```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Build the project
mkdir build
cd build
cmake ..
make -j$(nproc)

# Run basic tests
./nlm

# Run with custom configuration
./nlm --neuron_count=5000 --region_count=3 --connection_probability=0.2

# View help
./nlm --help
```

### Configuration

The NLM uses a flexible configuration system with the following primary settings:

```ini
# configs/default.cfg
{
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "performance_monitor": true,
    "enable_optimization": true
}
```

### Command Line Interface

```
Options:
  --help, -h                    Show this help message
  --config=file                Load configuration from file
  --neuron_count=N             Set number of neurons (default: 1000)
  --region_count=N              Set number of brain regions (default: 1)
  --connection_probability=N    Set synaptic connection probability (default: 0.1)
  --random_seed=N               Set random seed for reproducibility (default: 42)
  --simulation_timestep=N       Set simulation timestep in seconds (default: 0.001)
  --stdp_ltp_weight=N           Set STDP LTP weight (default: 0.01)
  --stdp_ltd_weight=N            Set STDP LTD weight (default: 0.012)
  --stdp_tau=N                  Set STDP time constant (default: 20.0)
  --synaptogenesis_rate=N       Set synaptic creation rate (default: 0.0001)
  --pruning_rate=N              Set synaptic pruning rate (default: 0.00001)
  --log_level=LEVEL             Set log level: debug, info, warning, error, critical
```

### Example Runs

```bash
# Basic test with default configuration
./nlm

# Learning experiment with custom parameters
./nlm \
  --neuron_count=2000 \
  --region_count=3 \
  --connection_probability=0.15 \
  --random_seed=12345 \
  --stdp_ltp_weight=0.015 \
  --stdp_tau=30.0 \
  --synaptogenesis_rate=0.00005

# Performance benchmark
./nlm \
  --neuron_count=5000 \
  --performance_monitor=true \
  --log_level=info
```

## Performance Optimization

### STDP Optimization

The STDP (Spike-Timing-Dependent Plasticity) algorithm has been optimized from O(n²) to O(n log n) complexity:

```cpp
// Old implementation: O(n²) complexity
for (Timestamp preTime : preSpikes) {
    for (Timestamp postTime : postSpikes) {
        // Calculate weight changes
    }
}

// New implementation: O(n log n) complexity
if (preSpikes.size() <= postSpikes.size()) {
    for (Timestamp preTime : preSpikes) {
        auto postIt = std::upper_bound(postSpikes.begin(), postSpikes.end(), preTime);
        for (; postIt != postSpikes.end(); ++postIt) {
            // Calculate potentiation
        }
    }
    // Handle depression
} else {
    // Symmetric approach for larger preSpikes
}
```

### Configuration-Based Optimization

Performance can be tuned through configuration:

```ini
{
    "use_optimized_stdp": true,
    "enable_vectorization": true,
    "use_memory_pooling": true,
    "enable_async_processing": false,
    "max_memory_usage_mb": 1024,
    "target_fps": 60.0,
    "performance_log_interval": 100
}
```

## Error Handling

The NLM includes comprehensive error handling and validation:

### Error Categories

1. **Configuration Errors**: Invalid values, missing files, parameter parsing
2. **Memory Errors**: Allocation failures, null pointer dereferences
3. **Simulation Errors**: Invalid neural states, plasticity failures
4. **System Errors**: I/O failures, file access issues

### Error Handling Features

- **Exception Safety**: All operations are exception-safe
- **Graceful Degradation**: System continues running if possible
- **Detailed Logging**: Comprehensive error logging with context
- **Validation**: Input validation and parameter checking

### Error Recovery

The system implements several error recovery mechanisms:

1. **Retry Logic**: Automatic retry for transient failures
2. **Fallback Values**: Use safe defaults when invalid values are detected
3. **State Restoration**: Checkpoint and restore on errors
4. **Resource Cleanup**: Proper cleanup in error conditions

## Testing and Validation

### Unit Tests

The project includes comprehensive unit tests:

```bash
# Run unit tests
./tests/unit_tests

# Run performance tests
./tests/performance_tests

# Run integration tests
./tests/integration_tests
```

### Experiment Framework

The experiment system provides:

- **Reproducible Experiments**: Random seed management
- **Data Collection**: Automatic metric tracking
- **Result Analysis**: Statistical analysis and visualization
- **Batch Processing**: Multiple experiment runs

```cpp
// Example experiment configuration
ExperimentRunner runner;
auto experiment = runner.createExperiment("STDP Learning");
bool success = runner.runExperiment(
    experiment, brain, world, 1000
);
```

## Advanced Features

### Performance Monitoring

Real-time performance monitoring with:

- **Metrics Collection**: Steps/sec, spikes/sec, memory usage
- **Visualization**: Performance dashboards
- **Alerting**: Threshold-based notifications
- **Profiling**: Detailed performance analysis

### Modular Architecture

The system supports:

- **Plugin Architecture**: Custom neural components
- **Extension Points**: New plasticity rules, neuron types
- **Configuration APIs**: Runtime configuration changes
- **Interfacing**: External tool integration

### Scalability

Features for large-scale simulations:

- **Memory Management**: Efficient memory usage
- **Parallel Processing**: Multi-core optimization
- **Distributed Computing**: Grid and cloud support
- **Resource Management**: Dynamic resource allocation

## File Structure

```
NLM Project Structure
├── src/
│   ├── core/                    # Core framework
│   │   ├── Config/              # Configuration system
│   │   ├── Logger/              # Logging system
│   │   ├── Random/              # Random number generation
│   │   └── ...
│   ├── brain/                   # Neural computation
│   │   ├── Brain.hpp            # Main brain class
│   │   ├── Brain.cpp            # Brain implementation
│   │   ├── Neuron.hpp          # Neuron class
│   │   ├── Neuron.cpp          # Neuron implementation
│   │   ├── Synapse.hpp          # Synapse class
│   │   └── ...
│   ├── agent/                   # Agent system
│   │   ├── AgentBrain.hpp        # Agent brain
│   │   └── ...
│   ├── plasticity/              # Plasticity rules
│   │   ├── STDP.hpp             # STDP rule
│   │   ├── STDP.cpp             # STDP implementation
│   │   └── ...
│   ├── sensory/                 # Sensory systems
│   │   ├── Vision.hpp           # Vision system
│   │   └── ...
│   ├── motor/                   # Motor systems
│   │   ├── Action.hpp           # Action class
│   │   └── ...
│   ├── world/                   # World simulation
│   │   ├── SimpleWorld.hpp      # World class
│   │   └── ...
│   └── ...
│
├── experiments/                # Experiment examples
│   ├── Phase2Experiment.cpp     # Phase 2 experiments
│   ├── Phase3Demo.cpp           # Phase 3 demonstrations
│   └── ...
│
├── configs/                    # Configuration files
│   ├── default.cfg             # Default configuration
│   ├── performance.cfg         # Performance settings
│   └── learning.cfg            # Learning settings
│
├── docs/                       # Documentation
│   ├── README.md               # Main documentation
│   ├── API_REFERENCE.md         # API reference
│   └── TUTORIALS.md            # Tutorial guides
│
└── tests/                      # Test suite
    ├── unit_tests/             # Unit tests
    ├── integration_tests/      # Integration tests
    └── performance_tests/      # Performance tests
```

## Building and Running

### Build Instructions

```bash
# Configure with CMake
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Build with parallel jobs
cmake --build build --parallel $(nproc)

# Install
cmake --build build --target install

# Run installation verification
ctest --output-on-failure
```

### Environment Setup

```bash
# Create virtual environment (recommended)
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt

# Set environment variables
export NLM_CONFIG_PATH=./configs
export NLM_LOG_LEVEL=info
export NLM_PERFORMANCE_MONITOR=true

# Set dynamic library path (Linux)
export LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH

# Set dynamic library path (macOS)
export DYLD_LIBRARY_PATH=./lib:$DYLD_LIBRARY_PATH
```

### Running Examples

```bash
# Basic demonstration
./build/nlm --help

# Run with default configuration
./build/nlm

# Run with custom configuration
./build/nlm --config=./configs/learning.cfg

# Run with performance monitoring
./build/nlm --performance_monitor=true --log_level=info

# Run batch of experiments
./build/experiment_runner --batch --config=experiments/batch.cfg
```

## Advanced Configuration

### Performance Optimization Profile

```ini
// configs/performance.cfg
{
    "neuron_count": 5000,
    "region_count": 4,
    "connection_probability": 0.15,
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.018,
    "stdp_tau": 25.0,
    "synaptogenesis_rate": 0.0002,
    "pruning_rate": 0.00002,
    "use_optimized_stdp": true,
    "enable_vectorization": true,
    "use_memory_pooling": true,
    "enable_async_processing": true,
    "max_memory_usage_mb": 2048,
    "target_fps": 60.0,
    "performance_log_interval": 50,
    "enable_performance_profiling": true,
    "memory_usage_report": true,
    "cpu_usage_report": true
}
```

### Learning Optimization Profile

```ini
// configs/learning.cfg
{
    "neuron_count": 2000,
    "region_count": 2,
    "connection_probability": 0.12,
    "random_seed": 123,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.015,
    "stdp_ltd_weight": 0.013,
    "stdp_tau": 30.0,
    "synaptogenesis_rate": 0.00015,
    "pruning_rate": 0.000015,
    "enable_development": true,
    "enable_curiosity": true,
    "enable_reward_modulation": true,
    "development_stage": "critical",
    "plasticity_modifier": 1.5
}
```

## Monitoring and Maintenance

### Performance Monitoring

```bash
# Monitor system performance
./monitor.sh

# Check for memory leaks
./memory_profiler.py

# Analyze performance bottlenecks
./performance_analyzer.py

# Generate performance reports
./performance_reporter.py
```

### Log Analysis

```bash
# Filter logs by level
./build/nlm --log_level=debug | grep "ERROR"

# Extract performance metrics
./build/nlm --log_level=info | grep "Performance"

# Analyze experiment results
./build/experiment_runner --analyze --output=results.json
```

### Maintenance Tasks

1. **Regular Backups**: Backup experiment data and configurations
2. **Performance Tuning**: Monitor and adjust performance settings
3. **Memory Management**: Monitor memory usage and clean up
4. **Documentation Updates**: Keep documentation current
5. **Security Audits**: Regularly review security settings

## Contributing

### Code Contributions

1. **Fork the Repository**: Create your own copy
2. **Create Branch**: Work on your improvements
3. **Submit Pull Request**: Submit your changes
4. **Code Review**: Follow coding standards and best practices

### Documentation Contributions

1. **Update Documentation**: Keep documentation current
2. **Add Examples**: Include new examples and tutorials
3. **Improve Tutorials**: Enhance existing tutorials
4. **Bug Reports**: Report issues and help improve documentation

### Testing Contributions

1. **Add Tests**: Add unit and integration tests
2. **Performance Tests**: Add performance benchmarks
3. **Integration Tests**: Test system integration
4. **Regression Tests**: Ensure no regressions

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Contact

For questions, issues, or contributions:
- **GitHub**: https://github.com/nlmlab/nlm
- **Email**: contact@nlmlab.org
- **Documentation**: https://docs.nlmlab.org

---

This documentation is continuously updated. For the latest version, visit the documentation repository.

© 2024 NLM Project. All rights reserved.
