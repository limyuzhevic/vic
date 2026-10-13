# NLM - Neural Learning Machine

## Quick Start

The NLM (Neural Learning Machine) is an experimental artificial developmental brain system that implements real spiking neural computation. This enhanced version includes comprehensive improvements for research and development.

## Installation

The project uses CMake for building. Ensure you have a C++20 compatible compiler.

### Basic Build
```bash
cd /path/to/nlm
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### Build with All Features
```bash
cd /path/to/nlm
mkdir build
cd build
cmake -D NLM_ENABLE_VISUALIZATION=ON \
      -D NLM_ENABLE_PROFILING=ON \
      -D NLM_ENABLE_MEMORY_ANALYSIS=ON \
      -D NLM_BUILD_TESTS=ON
make -j$(nproc)
```

## Usage

### Basic Commands

```bash
# Run basic neural simulation (1000 steps)
./nlm --mode simulation --steps 1000

# Run learning experiment with visualization
./nlm --mode learning --enable-visualization --steps 5000

# Run performance benchmark
./nlm --mode benchmark --steps 500

# Run analysis with profiling and memory tracking
./nlm --mode analysis --enable-profiling --enable-memory-analysis
```

### Configuration

Create a configuration file (e.g., `configs/my_config.cfg`):

```
# Network configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.15

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
simulation_timestep = 0.001

# Random seed for reproducibility
random_seed = 42

# Enable advanced features
enable_visualization = true
enable_profiling = true
enable_memory_analysis = true
```

Run with configuration:
```bash
./nlm --mode simulation --config configs/my_config.cfg
```

### Advanced Usage

```bash
# Hyperparameter optimization with cross-validation
./nlm --mode benchmark \
      --enable-hyperparameter-optimization \
      --enable-cross-validation --folds 5 \
      --enable-profiling --enable-memory-analysis

# Batch processing with real-time monitoring
./nlm --mode batch \
      --experiment-name research_study \
      --save-checkpoint --load-checkpoint \
      --export-csv results.csv --export-json results.json

# Run experiment with specific subsystems
./nlm --mode learning \
      --enable-subsystem plasticity \
      --enable-subsystem neuromodulation \
      --disable-subsystem development
```

## Features

### 1. Enhanced Command Line Interface
- Multiple operation modes (simulation, learning, analysis, benchmark, batch)
- Comprehensive configuration loading (file, JSON, params)
- Runtime parameter modification
- Progress reporting and status monitoring
- Checkpoint management

### 2. Advanced Visualization
- Console-based real-time visualization
- Multiple display modes (basic, activity, network, learning, detailed)
- Configurable refresh rates and display options
- Activity heatmaps and network graphs

### 3. Performance Monitoring
- Multi-level profiling (basic, detailed, call stack, memory)
- Frame time tracking and statistics
- Memory usage monitoring
- Benchmarking and comparative analysis

### 4. Memory Analysis
- Real-time memory allocation tracking
- Automatic memory leak detection
- Allocation site analysis
- Integration with profiling system

### 5. Enhanced Experiment Management
- Template-based experiment creation
- Parameter sweep for hyperparameter optimization
- Real-time monitoring and adaptive learning
- Cross-validation and ablation study support
- Data export (CSV, JSON formats)

### 6. Code Quality
- Comprehensive inline documentation
- Modern C++20 implementation
- Consistent error handling
- Backward compatibility maintained

## Examples

### Example 1: Basic Simulation
```bash
# Run 1000 steps of neural simulation
./nlm --mode simulation --steps 1000 --timestep 0.001
```

### Example 2: Learning Experiment
```bash
# Run learning experiment with visualization
./nlm --mode learning --enable-visualization --steps 5000 --timestep 0.0001
```

### Example 3: Performance Analysis
```bash
# Run detailed analysis with monitoring
./nlm --mode analysis --enable-profiling --enable-memory-analysis
```

### Example 4: Hyperparameter Optimization
```bash
# Run parameter sweep for model optimization
./nlm --mode benchmark --enable-hyperparameter-optimization --enable-cross-validation
```

## Testing

### Run Integration Tests
```bash
# Run integration test (verifies all components work together)
./nlm_test
```

### Run Phase 3 Demo
```bash
# Run existing Phase 3 demonstration
./nlm_phase3_demo
```

### Run Phase 4 Demo
```bash
# Run existing Phase 4 demonstration
./nlm_phase4_demo
```

## Development

### Adding New Experiments
1. Create a new experiment class inheriting from `Experiment`
2. Register the experiment in `AdvancedExperimentRunner`
3. Add configuration options in `AdvancedExperimentConfig`
4. Implement experiment-specific logic

### Extending CLI
1. Add new command line arguments to `CLIOptions`
2. Implement parsing logic in `CLI::parseArguments`
3. Add execution logic in `CLI::execute` methods

### Adding New Visualization Modes
1. Add new enum value to `ConsoleVisualizationMode`
2. Implement corresponding rendering method in `ConsoleVisualizer`
3. Add mode selection logic in `CLI`

## Configuration Reference

### Common Configuration Options
- `neuron_count`: Number of neurons in the network
- `region_count`: Number of neural regions
- `connection_probability`: Probability of synaptic connections
- `simulation_timestep`: Time step for simulation (seconds)
- `random_seed`: Random seed for reproducibility

### Advanced Configuration Options
- `enable_visualization`: Enable console visualization
- `enable_profiling`: Enable performance profiling
- `enable_memory_analysis`: Enable memory tracking
- `enable_real_time_monitoring`: Enable real-time monitoring
- `enable_adaptive_learning`: Enable adaptive parameter adjustment

## Troubleshooting

### Common Issues

1. **Build Failures**
   - Ensure C++20 compiler support
   - Check CMake version (minimum 3.16)
   - Verify required libraries are installed

2. **Runtime Errors**
   - Check configuration file syntax
   - Verify command line arguments are valid
   - Ensure output directories have write permissions

3. **Performance Issues**
   - Reduce number of samples in profiler
   - Disable visualization for better performance
   - Increase memory allocation limit if needed

### Getting Help

- **Documentation**: `NLM_ENHANCEMENT_SUMMARY.md` provides detailed information
- **Examples**: `easy_usage.md` and `HOW_TO_USE.md` provide quick start guides
- **Code**: All source files include comprehensive inline documentation

## Support

For support, questions, or feedback:
1. Check the documentation for usage examples
2. Review the code for implementation details
3. Report issues at the project's issue tracker

## License

This project is part of the NLM (Neural Learning Machine) research initiative.
See LICENSE file for details.

---

**Version**: 2.0
**Last Updated**: 2026-10-13
**Author**: NLM Development Team