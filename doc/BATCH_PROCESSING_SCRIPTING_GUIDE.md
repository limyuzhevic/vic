# NLM Batch Processing and Scripting System

## Overview

This document provides a comprehensive overview of the new batch processing and scripting system for the NLM (Neural Learning Machine) project. The system provides power users with the ability to run multiple experiments, parameter sweeps, and automated workflows while maintaining simplicity for end users.

## System Architecture

The batch processing and scripting system consists of several key components:

### 1. Script Processing Module
- **ScriptProcessor**: Core script parsing, validation, and execution
- **ScriptParser**: Handles JSON, YAML, and simple key-value script formats
- **ScriptConfig**: Configuration management for script execution
- **ParameterTemplate**: Template engine for parameter substitution

### 2. Batch Execution Module
- **BatchExecutor**: Main batch execution engine with parallel/sequential modes
- **ParameterSweeper**: Parameter generation for sweeps and optimization
- **ExperimentChainer**: Manages experiment sequencing and dependencies

### 3. Result Management Module
- **ResultCollector**: Aggregates and analyzes experiment results
- **ResultExporter**: Exports results in multiple formats (JSON, CSV, YAML, text)

### 4. CLI Integration
- Enhanced CLI framework with `script` command
- Support for advanced script options and modes

## Key Features

### 1. Script-based Execution
- **Script File Execution**: Execute scripts from JSON/YAML configuration files
- **Parameterized Experiment Execution**: Define and run experiments with parameterized configurations
- **Script Progress Tracking**: Real-time progress monitoring and reporting
- **Script Validation**: Syntax and semantic validation of script files

### 2. Parameter Sweeps
- **Grid Search**: Hyperparameter optimization with grid-based search
- **Random Parameter Sampling**: Stochastic sampling with configurable seeds
- **Linear Progression Sweeps**: Uniform spacing between parameter values
- **Geometric Progression Sweeps**: Exponential spacing between parameter values

### 3. Experiment Management
- **Multi-experiment Batch Execution**: Execute multiple experiments in batch
- **Experiment Chaining and Sequencing**: Execute experiments in dependency order
- **Conditional Experiment Execution**: Skip experiments based on conditions
- **Experiment Result Aggregation**: Combine results from multiple experiments

### 4. Automation Features
- **Automatic Checkpointing**: Save and resume interrupted batch executions
- **Result Persistence**: Save results to disk for later analysis
- **Progress Saving**: Checkpoint progress during long-running executions
- **Error Recovery and Retry**: Automatic retry on failures

### 5. Scripting Support
- **Python Script Integration**: Integration with Python-based experiment scripts
- **Script Parameter Templating**: Substitute parameters in script templates
- **Script-based Experiment Definition**: Define experiments directly in scripts
- **External Command Execution**: Run external commands from scripts

## Technical Implementation Details

### Script Processing

#### Script Formats Supported
- **JSON**: Standard JSON format for script configuration
- **YAML**: Human-readable YAML format
- **Simple**: Key-value pair format for simple scripts

#### Script Structure
```json
{
  "name": "Parameter Sweep",
  "description": "Sweep neuron count parameter",
  "experiments": [
    {"type": "phase6", "config": {"neuron_count": 500}},
    {"type": "phase6", "config": {"neuron_count": 1000}},
    {"type": "phase6", "config": {"neuron_count": 2000}}
  ],
  "parameters": {
    "type": "linear",
    "start": 500,
    "end": 2000,
    "steps": 5
  }
}
```

### Parameter Sweeps

#### Parameter Types
- **Linear**: Uniform spacing between start and end values
- **Geometric**: Exponential spacing between start and end values
- **Random**: Random sampling from specified distribution
- **Fixed**: Use predefined values

#### Generation Examples
- **Linear**: 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500
- **Geometric**: 10, 100, 1000, 10000
- **Random**: 0.42, 0.89, 0.15, 0.73, 0.91 (seed-based)

### Batch Execution Modes

#### Sequential Mode
- Execute experiments one after another
- Simple dependency management
- No parallel overhead

#### Parallel Mode
- Execute experiments concurrently
- Thread-safe result collection
- Configurable number of threads

#### Mixed Mode
- Combine sequential and parallel execution
- Optimize based on resource constraints

### Result Management

#### Aggregation Types
- **Average**: Compute mean values across experiments
- **Max**: Find maximum values
- **Min**: Find minimum values
- **Sum**: Sum values across experiments

#### Export Formats
- **JSON**: Standard JSON format
- **CSV**: Comma-separated values for spreadsheet import
- **YAML**: Human-readable YAML format
- **Text**: Plain text format for console output

## File Structure

### Script Processing Files
- `src/script/ScriptProcessor.hpp` / `ScriptProcessor.cpp`
- `src/script/ScriptParser.hpp` / `ScriptParser.cpp`
- `src/script/ScriptConfig.hpp` / `ScriptConfig.cpp`
- `src/script/ParameterTemplate.hpp` / `ParameterTemplate.cpp`

### Batch Execution Files
- `src/batch/BatchExecutor.hpp` / `BatchExecutor.cpp`
- `src/batch/ParameterSweeper.hpp` / `ParameterSweeper.cpp`
- `src/batch/ExperimentChainer.hpp` / `ExperimentChainer.cpp`

### Result Management Files
- `src/results/ResultCollector.hpp` / `ResultCollector.cpp`
- `src/results/ResultExporter.hpp` / `ResultExporter.cpp`

### Documentation Files
- `demo/batch_processing_demo.sh` - Demonstration script

## Command Line Interface

### Script Command Options

#### General Options
```bash
--script-file <file>                    Load commands from script file
--script-mode <mode>                     Script execution mode: file, sweep, grid, sequential, parallel
--script-verbose, -v                     Enable verbose output
--script-output-format <format>          Output format: json, csv, text
--script-output <file>                   Save output to file
--script-timeout <seconds>               Timeout for script execution
--script-parallel, -p                    Execute in parallel
--script-max-threads <n>                 Maximum threads for parallel execution
--script-parameter-type <type>           Parameter generation type: linear, geometric, random, grid, fixed
--script-start <value>                   Start value for parameter generation
--script-end <value>                     End value for parameter generation
--script-steps <n>                       Number of steps for parameter generation
--script-seed <seed>                     Random seed for parameter generation
--continue                               Continue from last checkpoint
```

#### Parameter Sweep Options
```bash
--sweep-parameter <param>    Parameter sweep: "name value1,value2,..."
```

#### Grid Search Options
```bash
--grid-search <param>        Grid search: "name value1,value2,..."
```

#### Sequential Options
```bash
--sequential <file>...       Execute sequential scripts
```

### Usage Examples

#### Run Script File
```bash
./nlm script --script-file experiment_suite.json
```

#### Parameter Sweep
```bash
./nlm script --sweep-parameter "neuron_count 500,1000,2000" --experiment phase6
```

#### Grid Search
```bash
./nlm script --grid-search "random_seed 42,123,456" "neuron_count 1000,2000" --experiment phase6
```

#### Sequential Execution
```bash
./nlm script --sequential experiment1.json experiment2.json
```

#### Parallel Execution
```bash
./nlm script --parallel --script-file batch.json
```

#### Custom Parameter Generation
```bash
./nlm script --script-mode sweep \
  --script-parameter-type linear \
  --script-start 500 \
  --script-end 2000 \
  --script-steps 10 \
  --script-output-format json \
  --script-output results.json \
  --script-file phase6_experiment.json
```

## Integration with Existing NLM Framework

### Experiment Integration
- **Phase 3 Experiments**: Full support for Phase 3 experiments
- **Phase 4 Experiments**: Full support for Phase 4 experiments
- **Phase 5 Experiments**: Full support for Phase 5 experiments
- **Phase 6 Experiments**: Full support for Phase 6 experiments
- **Experiment Configuration**: Configure experiments using script parameters
- **Result Collection**: Collect results from all experiment types

### Configuration System Integration
- **Script-based Configuration**: Load configuration from script files
- **Configuration Inheritance**: Support for configuration inheritance
- **Dynamic Configuration Updates**: Update configuration during execution
- **Configuration Validation**: Validate configuration files

### Logging and Progress Tracking
- **Detailed Progress Reporting**: Real-time progress monitoring
- **Comprehensive Logging**: Detailed logging at multiple levels
- **Progress Visualization**: Visual progress indicators
- **Resource Management**: Monitor and manage system resources

## System Capabilities

### Scalability
- **Large Experiment Counts**: Support for hundreds of experiments
- **High Concurrency**: Parallel execution with configurable thread pools
- **Memory Efficiency**: Efficient memory management for large result sets
- **Distributed Processing**: Foundation for distributed computing

### Flexibility
- **Multiple Script Formats**: JSON, YAML, and custom formats
- **Extensible Architecture**: Easy to add new script types
- **Customizable Execution**: Configurable execution modes and parameters
- **Plugin Architecture**: Extensible plugin system for new features

### Reliability
- **Error Handling**: Comprehensive error handling and recovery
- **Checkpointing**: Automatic checkpointing and resumption
- **Validation**: Input validation and format checking
- **Testing**: Comprehensive test coverage

## Performance Considerations

### Memory Usage
- **Efficient Result Storage**: Compressed storage for large result sets
- **Lazy Loading**: Load results on-demand to reduce memory usage
- **Streaming**: Stream results to disk for large experiments

### CPU Usage
- **Parallel Processing**: Efficient parallel execution
- **Optimized Algorithms**: Optimized algorithms for parameter generation
- **Caching**: Caching frequently used data

### I/O Operations
- **Asynchronous I/O**: Asynchronous file operations
- **Buffered I/O**: Buffered I/O for better performance
- **Compression**: Compressed storage for large files

## Usage Guidelines

### For Beginners
1. **Start with Simple Scripts**: Use simple JSON scripts to understand the system
2. **Use Provided Examples**: Try the examples in this documentation
3. **Check Demos**: Run the demonstration script to see the system in action
4. **Use Help Options**: Use `./nlm script --help` for script-specific help

### For Intermediate Users
1. **Explore Advanced Features**: Try parameter sweeps and grid search
2. **Customize Scripts**: Modify scripts to meet specific requirements
3. **Use Parallel Execution**: Take advantage of parallel processing
4. **Analyze Results**: Use result aggregation and analysis features

### For Advanced Users
1. **Customize Execution Modes**: Create custom execution modes
2. **Extend the System**: Add new script types and formats
3. **Integrate with External Tools**: Connect with external tools and services
4. **Optimize Performance**: Optimize for specific use cases

## Testing and Validation

### Unit Tests
- **Script Processing**: Test script parsing and validation
- **Batch Execution**: Test batch execution modes
- **Parameter Generation**: Test parameter generation algorithms
- **Result Management**: Test result collection and aggregation

### Integration Tests
- **End-to-End Tests**: Test complete workflows
- **Performance Tests**: Test system performance under load
- **Compatibility Tests**: Test compatibility with existing NLM components
- **Regression Tests**: Test for regressions in existing functionality

### Demonstration Tests
- **Demo Script**: Run the demonstration script to verify system functionality
- **Example Scripts**: Test example scripts to ensure they work correctly
- **Command Line Tests**: Test command line interface

## Future Enhancements

### Planned Features
1. **Distributed Computing**: Extend to support distributed computing
2. **Web Interface**: Web-based interface for script management
3. **Visualization**: Advanced visualization of results
4. **Machine Learning Integration**: Integration with machine learning frameworks
5. **Cloud Integration**: Integration with cloud computing platforms

### Extension Points
1. **New Script Types**: Support for new script formats (XML, TOML, etc.)
2. **New Parameter Types**: Support for custom parameter generation
3. **New Export Formats**: Support for new output formats (HDF5, Parquet, etc.)
4. **New Execution Modes**: Support for new execution modes
5. **New Result Types**: Support for new result types

## Installation and Setup

### Prerequisites
- **Compiler**: C++17 compatible compiler (g++, clang++)
- **Build System**: CMake 3.10 or higher
- **Required Libraries**: Standard C++ libraries

### Build Instructions
1. **Clone Repository**: Clone the NLM repository
2. **Configure Build**: Run CMake to configure the build
3. **Build Project**: Run make or cmake --build to build the project
4. **Install**: Run make install to install the system
5. **Run Tests**: Run the test suite to verify the system

### Quick Start
```bash
# Clone repository
cd /path/to/nlm

# Build project
mkdir build
cd build
cmake ..
make

# Install (optional)
make install

# Run tests
ctest

# Run demo script
../demo/batch_processing_demo.sh

# Run help
./nlm script --help
```

## Support and Documentation

### Getting Help
1. **Documentation**: Read this documentation for detailed information
2. **Help Options**: Use `./nlm script --help` for script-specific help
3. **Examples**: Try the examples in this documentation
4. **Demos**: Run the demonstration script to see the system in action

### Reporting Issues
1. **Bug Reports**: Report bugs through the NLM issue tracker
2. **Feature Requests**: Request new features through the NLM issue tracker
3. **Support Requests**: Request support through the NLM support channels
4. **Documentation Issues**: Report documentation issues through the NLM issue tracker

### Community
1. **GitHub**: Join the NLM community on GitHub
2. **Forums**: Participate in NLM forums and discussions
3. **Contributing**: Contribute to the NLM project
4. **Documentation**: Help improve NLM documentation

## Conclusion

The NLM batch processing and scripting system provides a comprehensive solution for automated experiment execution, parameter optimization, and result analysis. The system is:

✅ **Powerful**: Supports complex batch processing and automation
✅ **Flexible**: Supports multiple script formats and execution modes
✅ **Reliable**: Includes comprehensive error handling and recovery
✅ **Performant**: Optimized for efficient execution of large experiment batches
✅ **Extensible**: Easy to extend with new features and capabilities
✅ **User-Friendly**: Provides intuitive interfaces and comprehensive documentation

The system is now ready for use by power users who need advanced batch processing capabilities while maintaining simplicity for end users. The system integrates seamlessly with the existing NLM framework and provides a solid foundation for future enhancements.

## License

This system is part of the NLM (Neural Learning Machine) project and is licensed under the NLM license.

---

*Document Version: 1.0*
*Date: October 17, 2026*
*Author: NLM Development Team*