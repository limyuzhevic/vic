# NLM (Neural Learning Machine) - Enhanced Version

## Overview

NLM is an experimental artificial developmental brain system implementing real spiking neural computation with:

- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)
- Integrated memory, prediction, cognition, and neuromodulation systems

## System Architecture

### Core Components

1. **Configuration System**
   - Advanced parameter validation and type checking
   - Multiple configuration presets (default, test, benchmark)
   - Command-line argument overrides
   - Save/load configuration to files

2. **Experiment Management**
   - Enhanced experiment runner with multiple modes
   - Result tracking and analysis
   - Automated reporting
   - Batch experiment execution

3. **User Interface**
   - Command-line interface for running experiments
   - Configuration presets for different use cases
   - Result saving and reporting
   - Progress monitoring

4. **Performance Tools**
   - Profiling support
   - Performance benchmarking
   - Memory usage monitoring
   - Progress tracking

## Installation and Setup

### Prerequisites

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j$(nproc)
```

### Running Experiments

#### Basic Usage

```bash
# Run all experiments with default configuration
./nlm --config configs/default.cfg

# Run specific experiment mode
./nlm --mode connectivity --config configs/default.cfg

# Use enhanced experiment runner
python3 run_experiments.py --mode plasticity --config configs/default.cfg
```

#### Using the Shell Script

```bash
# Run experiment with custom settings
cd /workspace/app
./run-experiment.sh --mode plasticity --config configs/example.cfg --save-results plasticity_results.csv

# View help
./run-experiment.sh --help
```

#### Configuration Presets

```bash
# Test mode (small, fast)
python3 run_experiments.py --preset test --mode all

# Benchmark mode (large, performance testing)
python3 run_experiments.py --preset benchmark --mode connectivity
```

## Experiment Modes

### Basic Mode (`--mode basic`)

- Runs basic connectivity test
- Tests neural spike generation
- Verifies network responsiveness
- Fast execution (50 steps)

### Connectivity Mode (`--mode connectivity`)

- Tests neural connectivity and spike propagation
- Measures network efficiency
- Validates synaptic connections
- Medium execution (100 steps)

### Plasticity Mode (`--mode plasticity`)

- Runs plasticity learning experiment
- Demonstrates synaptic weight changes
- Shows learning through experience
- Long execution (1000 steps)

### STDP Mode (`--mode stdp`)

- Verifies STDP learning rule
- Tests pre-post spike pairing
- Demonstrates potentiation/depression
- Medium execution (500 steps)

### All Modes (`--mode all`)

- Runs all experiment modes sequentially
- Comprehensive system validation
- Complete learning demonstration
- Long execution (all steps)

## Configuration Files

### Configuration Formats

#### configs/default.cfg (Default Configuration)

```ini
# Core simulation parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15

# Neuron dynamics
threshold = 15.0
membrane_capacitance = 1.0
time_constant = 20.0

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Neuromodulation
reward_gain = 1.0
curiosity_gain = 0.5

# Memory
working_memory_capacity = 100
episodic_memory_capacity = 1000

# Logging and monitoring
log_level = "INFO"
log_to_file = false
enable_profiling = false
```

#### configs/example.cfg (Example Configuration)

```ini
# Custom configuration for specific experiments
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2

# Enhanced neuromodulation
enable_bayesian_learning = false
enable_meta_learning = false

# Safety limits
max_neurons = 10000
max_synapses = 1000000

# Performance settings
enable_profiling = true
test_mode = false
benchmark_mode = false
```

### Configuration Features

1. **Parameter Validation**
   - Type checking (int, double, bool, string, arrays)
   - Range validation
   - Required parameter checking

2. **Presets**
   - `default`: Standard configuration for research
   - `test`: Small configuration for quick testing
   - `benchmark`: Large configuration for performance testing

3. **Command Line Override**
   ```bash
   ./nlm --config configs/default.cfg --neuron_count 2000 --region_count 3
   ```

## Experiment Results

### Result Formats

#### JSON Format

```json
{
  "mode": "plasticity",
  "config": {"neuron_count": 500, "region_count": 1},
  "steps_completed": 1000,
  "spikes_generated": 20000,
  "learning_detected": true,
  "weight_change": 0.15,
  "execution_time": 125.3
}
```

#### CSV Format

```csv
metric,value
mode,plasticity
steps_completed,1000
spikes_generated,20000
learning_detected,true
weight_change,0.15
execution_time,125.3
```

### Result Files

1. **Main Results Directory**
   ```
   /workspace/app/results/
   ├── experiment_report_20240101_120000.txt
   ├── plasticity_results.json
   ├── plasticity_results.csv
   └── connectivity_results.json
   ```

2. **Log Files**
   - `nlm.log`: Detailed execution log
   - Console output: Real-time progress monitoring

## Advanced Features

### 1. Configuration Validation

The enhanced configuration system validates:
- Parameter types (int, float, bool, string, arrays)
- Parameter ranges (min/max values)
- Required parameters
- Data consistency

### 2. Experiment Tracking

Each experiment run is tracked with:
- Start/end timestamps
- Execution time metrics
- Step-by-step progress
- Resource usage monitoring
- Automated error detection

### 3. Result Analysis

Generated reports include:
- Key performance metrics
- Configuration summary
- Learning efficiency calculations
- Comparative analysis
- Visual progress indicators

### 4. Batch Processing

Run multiple experiments efficiently:
```bash
# Run all experiment modes
python3 run_experiments.py --mode all --config configs/example.cfg

# Run experiments with different configurations
for config in configs/default.cfg configs/example.cfg; do
    python3 run_experiments.py --mode plasticity --config "$config"
done
```

### 5. Performance Monitoring

Enable profiling for performance analysis:
```bash
# Enable profiling
./nlm --enable_profiling=true

# Or use experiment runner
python3 run_experiments.py --preset benchmark --enable_profiling=true
```

## Development and Testing

### Running Tests

```bash
# Run all tests
./test.sh

# Run specific experiment mode tests
python3 run_experiments.py --mode basic --preset test
```

### Adding New Experiment Modes

1. Create a new method in `ExperimentManager` class
2. Add corresponding command-line option
3. Update the argument parser
4. Implement experiment logic

### Adding New Configuration Parameters

1. Define parameter in `ParameterDefinition` structure
2. Add validation rules
3. Include in presets
4. Update documentation

## Troubleshooting

### Common Issues

1. **Configuration File Not Found**
   ```bash
   # Check if config file exists
   ls -la configs/default.cfg
   
   # Use absolute path
   ./nlm --config /workspace/app/configs/default.cfg
   ```

2. **Build Failed**
   ```bash
   # Clean and rebuild
   rm -rf build
   mkdir build
   cd build
   cmake ..
   make -j$(nproc)
   ```

3. **Memory Issues**
   ```bash
   # Reduce configuration parameters
   ./nlm --neuron_count 100 --region_count 1
   
   # Or use test preset
   python3 run_experiments.py --preset test
   ```

4. **Experiment Failed**
   ```bash
   # Check log files
   tail -f results/experiment_report_*.txt
   
   # Run with verbose output
   ./nlm --verbose
   ```

## Performance Recommendations

### For Development and Testing

```bash
# Use test configuration
python3 run_experiments.py --preset test --mode all

# Run individual experiments for debugging
python3 run_experiments.py --mode basic --preset test
```

### For Production Research

```bash
# Use default configuration
python3 run_experiments.py --preset default --mode plasticity

# Save results for analysis
python3 run_experiments.py --mode all --save-results research_results_$(date +%Y%m%d).json
```

### For Performance Benchmarking

```bash
# Use benchmark configuration
python3 run_experiments.py --preset benchmark --mode connectivity --enable_profiling=true
```

## Migration Guide

### From Basic to Enhanced Version

1. **Configuration**
   - Old: Hardcoded parameters in main.cpp
   - New: Flexible config system with presets

2. **Experiment Management**
   - Old: Limited experiment modes
   - New: Comprehensive experiment runner with multiple modes

3. **Result Tracking**
   - Old: No automated reporting
   - New: Detailed results with JSON/CSV output

4. **User Interface**
   - Old: Basic command-line arguments
   - New: Enhanced interface with help documentation

### Configuration Migration Examples

#### Old Configuration (main.cpp)
```cpp
// Hardcoded values in main.cpp
config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
config->set("simulation_timestep", 0.001, ConfigSource::Default);
config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
```

#### New Configuration (configs/default.cfg)
```ini
# Core simulation parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
```

## License

This project is part of the Neural Learning Machine (NLM) research initiative.
See LICENSE file for detailed licensing information.

## Contact

For issues, questions, or contributions:
- Repository: [GitHub Repository]
- Documentation: This file (README.md)
- Support: [Contact Information]

## Future Enhancements

1. **Advanced Visualization**
   - Real-time neural network visualization
   - Interactive result analysis
   - 3D brain modeling

2. **Cloud Integration**
   - Distributed experiment execution
   - Cloud-based result storage
   - Collaborative research

3. **Machine Learning Integration**
   - Deep learning model integration
   - Transfer learning capabilities
   - Hybrid architecture support

4. **Hardware Acceleration**
   - GPU-accelerated neural dynamics
   - Specialized hardware support
   - Real-time inference

## Conclusion

The enhanced NLM system provides a comprehensive platform for neural network research with:

- **Improved Configuration Management**: Type-safe, validated configuration system
- **Enhanced Experimentation**: Multiple experiment modes with detailed tracking
- **Better User Experience**: Intuitive command-line interface with comprehensive documentation
- **Production Ready**: Robust error handling, performance monitoring, and result analysis

This version builds upon the foundational neural network simulation with advanced features that make it suitable for both research and educational purposes, while maintaining backward compatibility with the original implementation.
