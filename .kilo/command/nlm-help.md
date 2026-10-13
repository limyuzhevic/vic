---
description: Navigate and understand the enhanced NLM Neural Learning Machine codebase with comprehensive help and documentation.
agent: nlm_improve
subtask: false
---

# Kilo Command: /nlm-help

Navigate and understand the enhanced NLM Neural Learning Machine codebase.

## Description

Get comprehensive help and documentation for working with the improved NLM (熙然) Neural Learning Machine codebase. This command provides access to all the enhancements made through Phase 6 integration.

## Usage

```bash
/nlm-help
```

## Key Features

### Enhanced CLI Interface Help

The NLM project now features a comprehensive command-line interface:

**Available experiments:**
- `./nlm learning` - Plasticity and STDP experiments
- `./nlm plasticity` - Plasticity-focused experiments  
- `./nlm stdp` - STDP verification
- `./nlm connectivity` - Neural connectivity tests
- `./nlm integration` - Full Phase 6 integration

**Key command-line options:**
- `--help, -h` - Show comprehensive help
- `--version, -v` - Show version information
- `--config=FILE` - Configuration file
- `--output-file=FILE` - Save results to file
- `--log-file=FILE` - Log output to file
- `--log-level=LEVEL` - Logging level (DEBUG, INFO, WARN, ERROR, CRITICAL)
- `--seed=NUMBER` - Random seed
- `--timestep=SECONDS` - Simulation timestep
- `--max-steps=NUMBER` - Maximum simulation steps
- `--verbose` - Enable verbose output
- `--quiet` - Quiet mode (minimal output)
- `--debug` - Enable debug mode
- `--profile` - Enable performance profiling

### Configuration Management

**Multi-level configuration system:**
- Default values (Phase 2 settings)
- Configuration file values  
- Command-line parameter overrides
- Custom parameters (key=value)

**Configuration files:**
- `configs/default.cfg` - Default configuration
- Custom `.cfg` files for different scenarios

### Performance Monitoring

**Real-time performance tracking:**
- Step timing analysis
- Spike rate calculations
- Benchmark metrics
- Statistical analysis (mean, stddev, range)

**Example performance output:**
```
=== PERFORMANCE REPORT ===
Total simulation time: 2.34s
Steps completed: 10000
Average step time: 0.234ms
Total spikes generated: 15678
Average spikes per step: 1.57
Step time range: 0.123 - 0.456ms
Step time stddev: 0.089ms

Benchmark metrics:
  Steps per second: 4271.79
  Spikes per second: 6698.12
```

### Debug & Diagnostic Tools

**Brain state analysis:**
- Regional neuron counts and states
- Synaptic connectivity analysis
- Plasticity mechanism status
- Membrane potential statistics

**System health checking:**
- Brain initialization validation
- Numerical stability checks
- Resource usage monitoring
- Error detection and reporting

### New Features

#### Enhanced CLI Interface
- 26+ command-line options
- Detailed help documentation with examples
- Version information display
- Experiment selection and filtering
- Custom parameter support (key=value)
- Multiple output formats

#### Advanced Configuration Management
- Multi-level configuration system with source tracking
- Command-line parameter overrides
- Configuration validation and error handling
- Enhanced configuration file support

#### Performance Monitoring & Statistics
- Real-time step timing
- Spike rate calculations
- Benchmark mode with detailed metrics
- Performance comparison capabilities

#### Debug & Diagnostic Tools
- Brain state analysis and regional diagnostics
- System health checking with comprehensive validation
- Error detection and numerical stability monitoring

#### Production-Ready Features
- Batch processing capabilities
- Plugin system for experiment templates
- Python scripting interface for automation
- Enhanced logging and output formatting

## Getting Started Examples

### Basic Usage
```bash
# Show help information
/nlm-help

# Get version information  
/nlm-help --version

# Show experiment options
/nlm-help --experiments
```

### Configuration Examples
```bash
# Run learning experiment with custom settings
/nlm learning --seed=123 --timestep=0.0001 --verbose

# Run integration test with full debugging
/nlm integration --config=production.cfg --debug --profile

# Save results to file with logging
/nlm learning --output-file=results.csv --log-file=nlm.log --log-level=DEBUG
```

### Performance Analysis
```bash
# Run with benchmarking enabled
/nlm --benchmark learning plasticity stdp

# Get detailed performance metrics
/nlm --profile --log-level=DEBUG integration
```

## Quick Reference

| Feature | Command | Description |
|---------|---------|-------------|
| **Help** | `/nlm-help` | Show comprehensive help |
| **Version** | `/nlm-help --version` | Get version information |
| **Experiments** | `/nlm-help --experiments` | List available experiments |
| **Configuration** | `/nlm-help --config` | Show configuration options |
| **Performance** | `/nlm-help --performance` | Show performance features |
| **Debug** | `/nlm-help --debug` | Show debugging tools |

## Technical Details

### Files Modified

- `src/main.cpp` - Enhanced CLI interface and configuration system
- `configs/default.cfg` - Default configuration settings
- `pyproject.toml` - Python bindings configuration
- `README.md` - Comprehensive documentation
- `HOW_TO_USE.md` - Usage guide
- `easy_usage.md` - Beginner-friendly guide

### Improvements Summary

- **CLI Interface**: 26+ new command-line options
- **Configuration**: Multi-tier source-based configuration system
- **Performance**: Real-time monitoring and benchmarking
- **Debugging**: Comprehensive brain state analysis
- **Logging**: Enhanced logging with multiple output formats
- **Extensibility**: Plugin architecture and automation capabilities

### Key Capabilities

1. **Production-Ready Architecture**: Comprehensive error handling and validation
2. **Advanced Configuration**: Multi-level source tracking and overrides
3. **Real-time Performance**: Event-driven performance monitoring
4. **Extensive Diagnostics**: Detailed brain state analysis
5. **User-Friendly**: Intuitive interface with comprehensive documentation
6. **Developer-Friendly**: Extensible plugin system and automation

## Support

For additional assistance with NLM improvements, contact the development team or refer to the comprehensive documentation in this repository.