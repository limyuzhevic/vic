# NLM Batch Processing and Scripting System - Quick Start

## Overview

This document provides a quick start guide for the NLM batch processing and scripting system. The system provides powerful capabilities for automated experiment execution, parameter sweeps, and result management.

## What Was Implemented

### Core Components

1. **Script Processing Module**
   - `ScriptProcessor`: Core script execution engine
   - `ScriptParser`: Handle JSON, YAML, and simple script formats
   - `ScriptConfig`: Configuration management for script execution
   - `ParameterTemplate`: Template engine for parameter substitution

2. **Batch Execution Module**
   - `BatchExecutor`: Main batch execution engine with parallel/sequential modes
   - `ParameterSweeper`: Generate parameter combinations for sweeps and optimization
   - `ExperimentChainer`: Manage experiment sequencing and dependencies

3. **Result Management Module**
   - `ResultCollector`: Aggregate and analyze experiment results
   - `ResultExporter`: Export results in multiple formats (JSON, CSV, YAML, text)

4. **CLI Integration**
   - Enhanced CLI framework with `script` command
   - Advanced script options and modes

## Key Features

### 1. Multi-Format Script Support
- **JSON**: Standard JSON configuration
- **YAML**: Human-readable YAML format
- **Simple**: Key-value pair format for simple scripts

### 2. Advanced Parameter Generation
- **Linear Progression**: Uniform spacing between values
- **Geometric Progression**: Exponential spacing between values
- **Random Sampling**: Stochastic sampling with configurable seeds
- **Fixed Values**: Use predefined values

### 3. Multiple Execution Modes
- **Sequential**: Execute experiments one after another
- **Parallel**: Execute experiments concurrently
- **Mixed**: Combine sequential and parallel execution

### 4. Comprehensive Result Management
- **Aggregation**: Statistical analysis and aggregation
- **Export**: Multi-format export (JSON, CSV, YAML, text)
- **Comparison**: Compare results between experiments
- **Statistics**: Compute mean, stddev, min, max, and other statistics

## Installation and Setup

### Prerequisites
- **Compiler**: C++17 compatible compiler (g++, clang++)
- **Build System**: CMake 3.16 or higher

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake ..

# Build the project
make -j4
```

## Usage Examples

### 1. Basic Script Execution

```bash
# Run a script file
./nml script --script-file experiment_suite.json

# Run with verbose output
./nml script --script-file experiment_suite.json -v

# Run with output to file
./nml script --script-file experiment_suite.json --script-output results.json
```

### 2. Parameter Sweeps

```bash
# Linear parameter sweep
./nml script --sweep-parameter "neuron_count 500,1000,2000" --experiment phase6

# Grid search
./nml script --grid-search "random_seed 42,123,456" "neuron_count 1000,2000" --experiment phase6

# Random parameter sampling
./nml script --script-mode sweep --script-parameter-type random --script-seed demo
```

### 3. Sequential Execution

```bash
# Execute multiple scripts sequentially
./nml script --sequential experiment1.json experiment2.json experiment3.json
```

### 4. Parallel Execution

```bash
# Execute script in parallel
./nml script --parallel --script-file batch.json

# Configure thread count
./nml script --parallel --script-max-threads 8
```

### 5. Advanced Script Configuration

```bash
# Linear progression sweep with custom parameters
./nml script --script-mode sweep \
  --script-parameter-type linear \
  --script-start 500 \
  --script-end 2000 \
  --script-steps 10 \
  --script-output-format json \
  --script-output results.json \
  --script-file phase6_experiment.json
```

## Command Line Reference

### Script Command Options

#### General Options
- `--script-file <file>`: Load commands from script file
- `--script-mode <mode>`: Script execution mode (file, sweep, grid, sequential, parallel)
- `--script-verbose, -v`: Enable verbose output
- `--script-output-format <format>`: Output format (json, csv, text)
- `--script-output <file>`: Save output to file
- `--script-timeout <seconds>`: Timeout for script execution
- `--script-parallel, -p`: Execute in parallel
- `--script-max-threads <n>`: Maximum threads for parallel execution
- `--script-parameter-type <type>`: Parameter generation type (linear, geometric, random, grid, fixed)
- `--script-start <value>`: Start value for parameter generation
- `--script-end <value>`: End value for parameter generation
- `--script-steps <n>`: Number of steps for parameter generation
- `--script-seed <seed>`: Random seed for parameter generation
- `--continue`: Continue from last checkpoint

#### Parameter Sweep Options
- `--sweep-parameter <param>`: Parameter sweep (e.g., "neuron_count 500,1000,2000")

#### Grid Search Options
- `--grid-search <param>`: Grid search (e.g., "random_seed 42,123,456")

#### Sequential Options
- `--sequential <file>...`: Execute sequential scripts

## System Architecture

```
+-----------------------------------+
|           CLI Framework            |
+-----------------------------------+
|                                   |
|  +-------------------------------+ |
|  |       Batch Executor          | |
|  +-------------------------------+ |
|  |   Script Processing           | |
|  |   Parameter Sweeper           | |
|  +-------------------------------+ |
|  |   Experiment Chainer          | |
|  +-------------------------------+ |
|  |    Result Collection          | |
|  |     Result Export             | |
|  +-------------------------------+ |
+-----------------------------------+
```

## Files Created

### Source Files (18 files)
1. **Script Processing** (4 files):
   - `src/script/ScriptProcessor.hpp` / `.cpp`
   - `src/script/ScriptParser.hpp` / `.cpp`
   - `src/script/ScriptConfig.hpp` / `.cpp`
   - `src/script/ParameterTemplate.hpp` / `.cpp`

2. **Batch Execution** (3 files):
   - `src/batch/BatchExecutor.hpp` / `.cpp`
   - `src/batch/ParameterSweeper.hpp` / `.cpp`
   - `src/batch/ExperimentChainer.hpp` / `.cpp`

3. **Result Management** (2 files):
   - `src/results/ResultCollector.hpp` / `.cpp`
   - `src/results/ResultExporter.hpp` / `.cpp`

### Documentation Files (3 files)
1. `doc/BATCH_PROCESSING_SCRIPTING_GUIDE.md`: Comprehensive guide
2. `doc/BATCH_PROCESSING_SYSTEM_IMPLEMENTATION.md`: Implementation details
3. `demo/batch_processing_demo.sh`: Demonstration script

### Testing Files (2 files)
1. `tests/test_batch_processing.cpp`: Comprehensive tests
2. `tests/CMakeLists.txt`: Test configuration

## Documentation

### Available Documentation
- **BATCH_PROCESSING_SCRIPTING_GUIDE.md**: Quick start guide with examples
- **BATCH_PROCESSING_SYSTEM_IMPLEMENTATION.md**: Detailed implementation information
- **batch_processing_demo.sh**: Demonstration script showing system capabilities

### Getting Help
- **Quick Start**: Read `BATCH_PROCESSING_SCRIPTING_GUIDE.md` for basic usage
- **Advanced Usage**: Read `BATCH_PROCESSING_SYSTEM_IMPLEMENTATION.md` for advanced features
- **Examples**: Run the demonstration script `demo/batch_processing_demo.sh`
- **Testing**: Run the test suite to verify system functionality

## Testing

### Run Test Suite
```bash
./nml_test
```

### Test Components
```bash
./nml --help
./nml --version
./nml script --help
./nml script --version
```

## System Capabilities

### 1. Script-Based Execution
- Load and parse scripts from multiple formats
- Execute experiments with parameterized configurations
- Support for template substitution in scripts
- Progress tracking and monitoring

### 2. Parameter Sweeps
- Linear progression for hyperparameter optimization
- Geometric progression for exponential scaling
- Random sampling for exploration
- Fixed values for deterministic execution

### 3. Experiment Management
- Multi-experiment batch execution
- Experiment chaining with dependency management
- Conditional experiment execution
- Result aggregation and analysis

### 4. Automation Features
- Automatic checkpointing and resumption
- Progress tracking and monitoring
- Error recovery and retry mechanisms
- Resource management and optimization

### 5. Result Management
- Result collection and aggregation
- Multi-format export (JSON, CSV, YAML, text)
- Statistical analysis and comparison
- Custom formatting and column ordering

## Performance Considerations

### Memory Usage
- **Efficient Storage**: Compressed storage for large result sets
- **Lazy Loading**: Load results on-demand to reduce memory usage
- **Streaming**: Streaming support for large datasets

### CPU Usage
- **Parallel Execution**: Efficient parallel execution with thread pools
- **Optimized Algorithms**: Optimized parameter generation algorithms
- **Caching**: Caching frequently used data

### I/O Performance
- **Buffered I/O**: Buffered file operations for better performance
- **Compression**: Support for compressed storage
- **Asynchronous Operations**: Asynchronous I/O operations

## Future Enhancements

### Planned Features
1. **Distributed Computing**: Extend to support distributed computing
2. **Web Interface**: Web-based interface for script management
3. **Advanced Visualization**: Advanced visualization of results
4. **Machine Learning Integration**: Integration with machine learning frameworks
5. **Cloud Integration**: Integration with cloud computing platforms

### Extension Points
1. **New Script Types**: Support for new script formats (XML, TOML, etc.)
2. **New Parameter Types**: Support for custom parameter generation
3. **New Export Formats**: Support for new output formats (HDF5, Parquet, etc.)
4. **New Execution Modes**: Support for new execution modes
5. **New Result Types**: Support for new result types

## Conclusion

The NLM batch processing and scripting system provides a comprehensive solution for automated experiment execution, parameter optimization, and result analysis. The system is:

✅ **Powerful**: Supports complex batch processing and automation
✅ **Flexible**: Supports multiple script formats and execution modes
✅ **Reliable**: Includes comprehensive error handling and recovery
✅ **Performant**: Optimized for efficient execution of large experiment batches
✅ **Extensible**: Easy to extend with new features and capabilities
✅ **User-Friendly**: Provides intuitive interfaces and comprehensive documentation

The system integrates seamlessly with the existing NLM framework and provides a solid foundation for future enhancements. It is now ready for use by power users who need advanced batch processing capabilities while maintaining simplicity for end users.

## Getting Started

### 1. Build the Project
```bash
mkdir build
cd build
cmake ..
make -j4
```

### 2. Try Basic Commands
```bash
# Phase 6 demo (default)
./nml

# Phase 3 demo
./nml demo --demo-type phase3

# Phase 4 demo
./nml demo --demo-type phase4

# Phase 6 demo
./nml demo --demo-type phase6

# Advanced script execution
./nml script --script-file experiment_suite.json
```

### 3. Get Help
```bash
./nml --help
./nml script --help
```

---

*Document Version: 1.0*
*Date: October 17, 2026*
*Author: NLM Development Team*
*Status: ✅ READY FOR PRODUCTION*