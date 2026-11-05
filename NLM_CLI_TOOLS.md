# NLM Advanced Command-Line Tools Implementation Summary

## Overview

This project implements comprehensive command-line tools for the NLM (Neural Learning Machine) framework, providing advanced functionality beyond basic examples. The CLI tools follow modern design patterns and integrate seamlessly with the existing CMake/build system.

## Implemented CLI Tools

### 1. Brain Manager CLI (`nlm-brain`)

**Purpose**: Create, configure, save, load, and manage brain instances

**Key Features**:
- Create new brain instances with configurable parameters
- Initialize brain with custom neuron/region counts
- Save and load brain states for persistence
- Inspect brain properties and statistics
- Support for both interactive and JSON output modes

**Commands**:
- `create`: Create a new brain instance
- `initialize`: Initialize brain with configuration
- `save`: Save brain state to file
- `load`: Load brain state from file
- `inspect`: Inspect brain properties and statistics

**Example Usage**:
```bash
nlm-brain create --neurons 100 --regions 2
nlm-brain save --brain-file brain.nemo
nlm-brain load --brain-file brain.nemo
nlm-brain inspect --json
```

### 2. Experiment Runner CLI (`nlm-experiment`)

**Purpose**: Run different experiment types with various parameters

**Key Features**:
- Run simple agent experiments with world interaction
- Parameter sweep capabilities for systematic testing
- Built-in performance benchmarking
- Comprehensive result reporting with metrics
- JSON output for programmatic consumption

**Commands**:
- `run`: Execute an NLM experiment
- `sweep`: Run parameter sweep experiments
- `benchmark`: Run performance benchmarks

**Example Usage**:
```bash
nlm-experiment run --max-steps 1000 --start-step 0
nlm-experiment sweep --param-values "neuron_count=500" "region_count=3"
nlm-experiment benchmark --steps 100 --warmup 10
```

### 3. Performance Profiler CLI (`nlm-performance`)

**Purpose**: Benchmarking and profiling brain performance

**Key Features**:
- Brain creation and initialization performance benchmarking
- Memory usage tracking and profiling
- Detailed statistics collection over time
- Performance metrics (steps/sec, spikes/sec)
- Statistical analysis (mean, median, stddev, min/max)

**Commands**:
- `benchmark`: Benchmark brain creation and step performance
- `memory`: Profile memory usage patterns
- `stats`: Profile detailed brain statistics

**Example Usage**:
```bash
nlm-performance benchmark --num-brains 5 --benchmark-steps 100
nlm-performance memory --steps 1000 --runs 3
nlm-performance stats --steps 1000
```

### 4. Configuration Manager CLI (`nlm-config`)

**Purpose**: Manage configuration files and parameters

**Key Features**:
- Create new configuration files with custom parameters
- Load and inspect existing configurations
- Get/set individual configuration parameters
- Configuration validation
- Full CRUD operations for configuration management

**Commands**:
- `create`: Create a new configuration file
- `load`: Load configuration from file
- `inspect`: Inspect configuration properties
- `set`: Set a configuration parameter
- `get`: Get a configuration parameter value
- `remove`: Remove a configuration parameter
- `validate`: Validate configuration file

**Example Usage**:
```bash
nlm-config create --output-file config.json --params "neuron_count=500" "region_count=3"
nlm-config load --config-file config.json --json
nlm-config set --key plasticity.stdp.enable --value true --output-file config.json
nlm-config get --key neuron_count
nlm-config validate --config-file config.json
```

### 5. Visualization CLI (`nlm-visualize`)

**Purpose**: Generate basic visualizations of brain activity

**Key Features**:
- Activity visualization over time (spikes, firing rates, neuron counts)
- Summary statistics visualization
- Data export in multiple formats (JSON, CSV, TSV)
- Activity pattern analysis
- Developmental stage tracking
- Integration with matplotlib for plotting

**Commands**:
- `plot`: Create brain activity visualizations
- `export`: Export brain data for external analysis
- `analyze`: Analyze brain activity patterns and provide insights

**Example Usage**:
```bash
nlm-visualize plot --steps 100 --plot-type activity
nlm-visualize export --steps 200 --format json --output-file brain_data.json
nlm-visualize analyze --steps 500 --analysis-type patterns
```

## Unified CLI Entry Point

**Purpose**: `nlm` - Main entry point with common arguments and subcommands

**Key Features**:
- Common arguments: `--version`, `--verbose`, `--json`, `--pretty`
- Delegates to appropriate subcommands
- Consistent help system across all tools
- Integrated logging and error handling

**Usage**:
```bash
nlm --version
nlm --verbose --json brain create --neurons 100
nlm brain --help
nlm experiment run --max-steps 1000
nlm performance benchmark --num-brains 5
nlm config create --output-file config.json
nlm visualize plot --steps 100
```

## Technical Implementation Details

### API Integration
- All tools use the existing `pynlm` Python API
- Built on the pybind11 C++ bindings exposed by the NLM framework
- Compatible with current CMake/build system
- Follows project coding standards and best practices

### CLI Design Patterns
- Uses `argparse` for command-line parsing
- Supports both short and long options
- Provides comprehensive help text with examples
- Implements JSON output for programmatic consumption
- Includes proper error handling and validation

### Output Formats
- **JSON**: Machine-readable output for programmatic processing
- **Pretty JSON**: Formatted JSON with indentation
- **Human-readable**: Console output with clear formatting
- **CSV/TSV**: Export formats for data analysis

### Error Handling
- Comprehensive error handling and validation
- Clear error messages
- Graceful failure with informative output
- Exit codes for different error conditions

## Build System Integration

### pyproject.toml Updates
- Added `project.scripts` section with all CLI tool entry points
- Tools can be installed via `pip install .` as console scripts
- Maintains compatibility with existing scikit-build configuration

### Console Scripts
- `nlm`: Unified CLI entry point
- `nlm-brain`: Brain management tool
- `nlm-experiment`: Experiment runner tool
- `nlm-performance`: Performance profiler tool
- `nlm-config`: Configuration manager tool
- `nlm-visualize`: Visualization tool

## Testing and Verification

### Testing Approach
- Each CLI tool includes comprehensive error handling
- Input validation for all parameters
- JSON output validation for machine-readable consumption
- Examples in help text demonstrate proper usage

### Output Examples
```bash
# JSON Output Example
$ nlm experiment run --max-steps 100 --json
{
  "experiment_type": "simple_agent",
  "parameters": {
    "max_steps": 100,
    "start_step": 0,
    "world_dimensions": "20x20",
    "vision_dimensions": "8x8"
  },
  "results": {
    "total_reward": 15.75,
    "total_spikes": 1247,
    "final_curiosity": 0.342
  }
}

# Human-Readable Output Example
$ nlm brain create --neurons 100 --regions 2
Brain created successfully!
  - Neurons: 100
  - Regions: 2
  - Synapses: 247
  - Brain object: <Brain object at 0x7f8e9a1b2c80>
```

## Benefits

1. **Automation**: Enables automated experimentation and benchmarking
2. **Integration**: Tools can be integrated into existing workflows
3. **Analysis**: Comprehensive data export for analysis
4. **Configuration**: Flexible configuration management
5. **Visualization**: Visual insights into brain activity
6. **Performance**: Built-in performance profiling and benchmarking
7. **Compatibility**: Seamless integration with existing NLM codebase

## Future Enhancements

Potential future improvements:
- More sophisticated experiment protocols
- Advanced visualization options
- Machine learning integration for experiment optimization
- Web interface for visualization
- Parallel processing for large-scale experiments
- Integration with external data sources
- Real-time monitoring capabilities

## Conclusion

These NLM CLI tools provide a comprehensive, modern command-line interface for working with the NLM framework. They enable users to:

- Automate experimentation and benchmarking
- Manage complex configuration scenarios
- Visualize and analyze brain activity patterns
- Profile and optimize performance
- Export data for external analysis

The tools are designed to be both powerful and user-friendly, supporting both interactive use and integration into automated workflows.

---

**Version**: 0.1.0  
**License**: MIT  
**Author**: NLM Project  
**Date**: 2026-11-05
