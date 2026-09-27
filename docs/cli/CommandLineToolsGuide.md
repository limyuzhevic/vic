# Brain Command Line Tools Guide

## Overview

This document provides comprehensive documentation for the Brain Command Line Interface (CLI) tools for the NLM project. These tools provide extensive capabilities for brain manipulation, analysis, visualization, and performance benchmarking.

## Introduction

The Brain CLI tools are designed to provide:

1. **Brain Manipulation Tools**: Configuration editing, brain creation, backup/restore, comparison, optimization, and validation
2. **Brain Analysis Tools**: Structure analysis, activity pattern analysis, performance analysis, memory analysis, and dynamics analysis
3. **Advanced Command-Line Interface**: Interactive brain manipulation shell, batch processing, script-based operations, and configuration management
4. **Integration Tools**: Integration with existing NLM systems, plugin integration, and external tool support
5. **Visualization and Reporting Tools**: Brain visualization, performance visualization, report generation, and data export/import

## Components

### 1. BrainCommandLineInterface
- **Header File**: `include/nlm/cli/BrainCommandLineInterface.hpp`
- **Source File**: `src/cli/BrainCommandLineInterface.cpp`

**Description**: Main command-line interface class that provides the foundation for all brain operations.

**Key Features**:
- Command registration and execution
- Configuration management (--config, --output, --log-level)
- Help and version display
- Error handling and logging

**Usage**:
```bash
brain-cli [options] command [args]

Options:
  --help              Show help message
  --version           Show version information
  --config <path>     Specify configuration file
  --output <path>     Specify output directory
  --log-level <level> Set logging level (DEBUG, INFO, WARNING, ERROR)
```

### 2. BrainConfigTool
- **Header File**: `include/nlm/cli/BrainConfigTool.hpp`
- **Source File**: `src/cli/BrainConfigTool.cpp`

**Description**: Configuration manipulation tools for brain operations.

**Key Features**:
- Load, save, and validate brain configurations
- Backup and restore configuration
- Compare and optimize configurations
- Create and initialize brain configurations
- Export and import configuration data

**Usage**:
```bash
brain-cli config load <path>
brain-cli config save <path>
brain-cli config validate
brain-cli config backup <path>
brain-cli config restore <path>
brain-cli config compare <path1> <path2>
brain-cli config optimize
brain-cli config export <format> <path>
brain-cli config import <format> <path>
```

### 3. BrainAnalysisTool
- **Header File**: `include/nlm/cli/BrainAnalysisTool.hpp`
- **Source File**: `src/cli/BrainAnalysisTool.cpp`

**Description**: Brain analysis and reporting tools.

**Key Features**:
- Brain structure analysis
- Activity pattern analysis
- Performance analysis
- Memory analysis
- Dynamics analysis
- Report generation and export

**Usage**:
```bash
brain-cli analyze structure <path>
brain-cli analyze activity <path>
brain-cli analyze performance <path>
brain-cli analyze memory <path>
brain-cli analyze dynamics <path>
brain-cli analyze generate-report <output>
```

### 4. BrainVisualizationTool
- **Header File**: `include/nlm/cli/BrainVisualizationTool.hpp`
- **Source File**: `src/cli/BrainVisualizationTool.cpp`

**Description**: Brain visualization tools for graphical representation.

**Key Features**:
- Create brain visualizations
- Generate 3D and 2D views
- Create heatmaps and time series plots
- Export visualizations in multiple formats
- Configure visualization options

**Usage**:
```bash
brain-cli visualize brain <path> <output>
brain-cli visualize performance <data> <output>
brain-cli visualize structure <structure> <output>
brain-cli visualize activity <activity> <output>
brain-cli visualize dynamics <dynamics> <output>
brain-cli visualize export <format> <path>
```

### 5. BrainConversionTool
- **Header File**: `include/nlm/cli/BrainConversionTool.hpp`
- **Source File**: `src/cli/BrainConversionTool.cpp`

**Description**: Brain data conversion and processing tools.

**Key Features**:
- Convert between different brain data formats
- Process activity, structure, and performance data
- Batch processing capabilities
- Conversion profile management
- Validation and merging of converted data

**Usage**:
```bash
brain-cli convert brain <input> <output> <format>
brain-cli convert activity <input> <output>
brain-cli convert structure <input> <output>
brain-cli convert performance <input> <output>
brain-cli convert batch <inputs> <output-dir>
brain-cli convert profile load <path>
brain-cli convert profile save <path>
```

### 6. BrainValidationTool
- **Header File**: `include/nlm/cli/BrainValidationTool.hpp`
- **Source File**: `src/cli/BrainValidationTool.cpp`

**Description**: Brain validation and verification tools.

**Key Features**:
- Validate brain integrity
- Check brain consistency
- Validate brain format
- Validate brain performance
- Validate brain memory
- Validate brain dynamics
- Generate validation reports

**Usage**:
```bash
brain-cli validate integrity <path>
brain-cli validate consistency <path>
brain-cli validate format <path>
brain-cli validate performance <path>
brain-cli validate memory <path>
brain-cli validate dynamics <path>
brain-cli validate report <results> <output>
```

### 7. BrainPerformanceTool
- **Header File**: `include/nlm/cli/BrainPerformanceTool.hpp`
- **Source File**: `src/cli/BrainPerformanceTool.cpp`

**Description**: Performance analysis tools.

**Key Features**:
- Benchmark brain performance
- Monitor performance over time
- Profile brain activity
- Optimize brain performance
- Generate performance reports

**Usage**:
```bash
brain-cli perf benchmark <path>
brain-cli perf monitor <path> <duration>
brain-cli perf profile <path> <duration>
brain-cli perf optimize <path>
brain-cli perf report <results> <output>
```

### 8. InteractiveBrainShell
- **Header File**: `include/nlm/cli/InteractiveBrainShell.hpp`
- **Source File**: `src/cli/InteractiveBrainShell.cpp`

**Description**: Interactive command-line shell for brain manipulation.

**Key Features**:
- Interactive command-line interface
- Command history
- Dynamic command registration
- Help system
- Real-time command processing

**Usage**:
```bash
brain-shell
# Then use commands like:
brain analyze <path>
brain create <path>
brain visualize <path>
brain benchmark
config show
help
exit
```

### 9. BatchBrainProcessor
- **Header File**: `include/nlm/cli/BatchBrainProcessor.hpp`
- **Source File**: `src/cli/BatchBrainProcessor.cpp`

**Description**: Batch processing tools for multiple brain operations.

**Key Features**:
- Sequential and parallel processing
- Batch result tracking
- Batch reporting
- Data export and import
- Thread-safe operations

**Usage**:
```bash
brain-cli batch process <files> <output-dir>
brain-cli batch sequential <files> <output-dir>
brain-cli batch parallel <files> <output-dir> [threads]
brain-cli batch report <results> <output>
brain-cli batch export <output> <path>
brain-cli batch import <input>
```

### 10. BrainScriptingTool
- **Header File**: `include/nlm/cli/BrainScriptingTool.hpp`
- **Source File**: `src/cli/BrainScriptingTool.cpp`

**Description**: Script-based brain operations.

**Key Features**:
- Script loading and execution
- Function registration
- Variable management
- Script templates
- Argument passing

**Usage**:
```bash
brain-cli script load <path>
brain-cli script execute <path>
brain-cli script execute-string <script>
brain-cli script save <path> <content>
brain-cli script register <name> <function>
brain-cli script list
brain-cli script batch <scripts>
```

## Examples

### Brain Batch Processor
```bash
# Process all brain files in a directory
brain-cli batch process ./brains/ ./output/ --parallel --threads 8

# Process sequentially
brain-cli batch sequential ./brains/ ./output/

# Generate report
brain-cli batch report ./results/ batch_report.md
```

### Interactive Brain Analysis
```bash
# Start interactive shell
brain-shell

# Within shell:
brain analyze ./brains/neural_network.json
brain visualize ./brains/neural_network.json ./visualizations/
brain benchmark
config show
```

### Configuration Migration
```bash
# Migrate configuration from YAML to JSON
brain-cli convert brain ./config.yaml ./config.json json

# Create backup and migrate
brain-cli config backup ./config.bak
brain-cli config restore ./config.bak
```

### Performance Benchmarking
```bash
# Run comprehensive performance benchmark
brain-cli perf benchmark ./brains/neural_network.json --duration 120

# Monitor performance for 30 seconds
brain-cli perf monitor ./brains/neural_network.json 30

# Generate performance report
brain-cli perf report ./perf_data.json performance_report.html
```

## Integration

The Brain CLI tools integrate with existing NLM systems through:

1. **Plugin Architecture**: Support for custom plugins
2. **Python Integration**: Python bindings for advanced scripting
3. **External Tool Integration**: Support for external analysis tools
4. **Configuration Management**: Integration with NLM configuration system
5. **Logging and Monitoring**: Integration with NLM logging infrastructure

## Installation

### Prerequisites
- C++17 compatible compiler
- CMake 3.10 or higher
- Build tools (make, ninja)

### Building
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Installation
```bash
sudo cmake --install . --prefix /usr/local
```

## Configuration

The Brain CLI tools use a configuration file (`brain_config.json`) by default. Configuration can be set via:

1. **Command-line options**: `--config <path>`
2. **Environment variables**: `BRAIN_CLI_CONFIG`
3. **Default location**: `./brain_config.json`

Configuration options include:
- `log_level`: Logging level (DEBUG, INFO, WARNING, ERROR)
- `output_directory`: Default output directory
- `default_format`: Default data format
- `performance_thresholds`: Performance thresholds
- `validation_thresholds`: Validation thresholds

## Troubleshooting

### Common Issues

1. **File Not Found**
   - Check that the file path exists
   - Verify file permissions
   - Use absolute paths for remote files

2. **Memory Issues**
   - Reduce the number of concurrent threads
   - Use streaming for large files
   - Increase system memory

3. **Performance Issues**
   - Check system resources (CPU, memory)
   - Reduce benchmark duration
   - Optimize brain configurations

4. **Integration Issues**
   - Verify plugin compatibility
   - Check version compatibility
   - Use `--help` for command options

### Debugging
```bash
# Enable debug logging
brain-cli --log-level DEBUG command

# Run with valgrind (Linux)
valgrind --leak-check=full brain-cli command

# Use strace for system call tracing
strace -o trace.log brain-cli command
```

## Future Enhancements

1. **Web Interface**: Web-based GUI for brain manipulation
2. **Docker Integration**: Docker support for containerized deployments
3. **Cloud Integration**: Integration with cloud computing platforms
4. **Machine Learning**: Integration with ML frameworks for analysis
5. **Real-time Processing**: Real-time brain analysis and visualization

## License

This software is licensed under the Apache 2.0 License.
