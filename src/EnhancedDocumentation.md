# NLM Enhanced Documentation

## Overview

This document describes the enhancements made to the NLM (Neural Learning Machine) project to improve usability, configurability, and error handling. These improvements make the system easier to use for beginners while providing advanced capabilities for experienced users.

## Enhanced Command-Line Interface

### New CLI Features

The NLM Phase 2 demo now includes a comprehensive command-line interface that provides:

1. **Flexible Configuration Loading**
   - Multiple file formats: JSON, YAML, and plain text
   - Automatic file format detection based on extensions
   - Error handling for missing or malformed configuration files

2. **Test Mode Selection**
   - `--basic-test-only`: Run only basic connectivity test
   - `--plasticity-test-only`: Run only plasticity learning experiment
   - `--stdp-test-only`: Run only STDP verification
   - `--all-tests`: Run all tests (default)

3. **Output and Logging Options**
   - `--output-format=FORMAT`: Choose output format (text, json, csv)
   - `--verbose, -v`: Enable verbose logging for debugging
   - `--validate-only`: Validate configuration and exit
   - `--show-defaults`: Display default configuration values

4. **Enhanced Help System**
   - `--help, -h`: Display comprehensive help information
   - Examples and usage instructions
   - Configuration format specifications

### Usage Examples

```bash
# Run all tests with defaults
./nlm_phase2_demo

# Load custom configuration
./nlm_phase2_demo --config=myconfig.cfg

# Run only plasticity experiment
./nlm_phase2_demo --plasticity-test-only

# Run with verbose output for debugging
./nlm_phase2_demo --verbose

# Validate configuration and exit
./nlm_phase2_demo --validate-only

# Show default configuration values
./nlm_phase2_demo --show-defaults

# Run with JSON output for programmatic processing
./nlm_phase2_demo --output-format=json
```

## Enhanced Configuration System

### Configuration File Formats

The configuration system now supports multiple formats:

#### JSON (.json, .cfg.json)
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "simulation_timestep": 0.001,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001
}
```

#### YAML (.yaml, .yml)
```yaml
neuron_count: 1000
region_count: 2
simulation_timestep: 0.001
connection_probability: 0.1
stdp_ltp_weight: 0.02
stdp_ltd_weight: 0.015
stdp_tau: 20.0
synaptogenesis_rate: 0.0001
pruning_rate: 0.00001
```

#### Plain Text (key=value)
```
neuron_count = 1000
region_count = 2
simulation_timestep = 0.001
connection_probability = 0.1
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

### Configuration Command-Line Options

#### File Loading
```bash
--config=file.cfg        Load configuration from file (automatic format detection)
--config file.cfg        Alternative format for specifying config file
```

#### Configuration Overrides
```bash
--neuron_count=1000      Override configuration value
-n1000                    Short format for configuration override
--connection_probability=0.15
```

#### Configuration Validation
```bash
--validate-only          Validate configuration and exit without running simulation
--show-defaults          Display default configuration values
```

## Enhanced Test Results

### Comprehensive Logging

Each test now provides detailed logging including:

1. **Timing Information**
   - Total execution time for each test
   - Step-by-step progress logging
   - Performance metrics

2. **Detailed Analysis**
   - Spike count statistics
   - Firing neuron statistics
   - E/I ratio calculations
   - Membrane potential changes

3. **Error Handling**
   - Validation of input parameters
   - Clear error messages for failures
   - Graceful handling of edge cases

### Example Enhanced Output

```
=== Test 1: Enhanced Basic Neural Connectivity ===
  Initial state: 500 neurons, 0 spikes
  Injecting current into 10 neurons...
  Test completed in 45 ms
  Spikes generated: 12
  Neurons currently firing: 3
  ✓ Spikes propagate through network
  Spike activity analysis:
    Maximum firing rate observed: 2.4 spikes/step
  Membrane potential changes:
    Neuron 0: V=-55.2 (Δ=25.8)
    Neuron 1: V=-58.7 (Δ=22.3)
```

## Error Handling and Validation

### Enhanced Error Detection

The system now includes comprehensive error handling:

1. **Input Validation**
   - Validation of brain object parameters
   - Verification of required components (regions, neurons, synapses)
   - Configuration file validation

2. **Clear Error Messages**
   - Descriptive error messages for debugging
   - Multiple error levels (error, warning, info)
   - Context-specific error information

3. **Graceful Failure**
   - Clear exit codes for different failure scenarios
   - Detailed logging of failure causes
   - Recommendations for resolving issues

### Example Error Handling

```bash
# Configuration validation errors
./nlm_phase2_demo --validate-only
ERROR: Configuration validation failed
  - Missing required parameter: neuron_count
  - Invalid value type for simulation_timestep

# Brain initialization errors
./nlm_phase2_demo --basic-test-only
ERROR: No brain regions found! Cannot run connectivity test.
ERROR: No neurons found! Cannot run connectivity test.

# Test execution errors
./nlm_phase2_demo --plasticity-test-only
ERROR: Cannot enable plasticity - no synapses available.
```

## Performance Monitoring

### Built-in Performance Tracking

The enhanced system includes performance monitoring:

1. **Timing Statistics**
   - Individual test execution times
   - Total simulation duration
   - Step-by-step timing information

2. **Activity Metrics**
   - Spike count and firing rates
   - E/I ratio calculations
   - Network connectivity statistics

3. **Resource Usage**
   - Memory usage tracking
   - CPU time measurement
   - Network performance metrics

## Advanced Features

### Configuration Merge Strategies

The configuration system supports advanced merge strategies:

1. **File and Command-Line Merging**
   - Command-line arguments override file settings
   - Configuration source tracking for priority management
   - Validation of merged configurations

2. **Configuration Validation**
   - Schema validation for configuration files
   - Type checking for configuration values
   - Range validation for numerical parameters

### Enhanced Test Scenarios

The enhanced CLI enables new testing scenarios:

1. **Targeted Testing**
   - Test individual components in isolation
   - Focus on specific neural mechanisms
   - Validate specific plasticity rules

2. **Comparative Analysis**
   - Run multiple configurations side-by-side
   - Compare performance across different parameters
   - Analyze learning curves and development trajectories

## Migration Guide

### From Legacy to Enhanced System

If you're migrating from the legacy version, here are the key changes:

#### CLI Interface

**Legacy:**
```bash
./nlm_phase2_demo --config=myconfig.cfg
```

**Enhanced:**
```bash
./nlm_phase2_demo --config=myconfig.cfg --verbose
```

#### Configuration Files

**Legacy:** Plain text only

**Enhanced:** JSON, YAML, and plain text support

#### Test Output

**Legacy:** Basic results

**Enhanced:** Comprehensive logging with timing, analysis, and error handling

## Troubleshooting

### Common Issues and Solutions

#### "No spikes detected"
**Problem:** Test fails to generate spikes
**Solution:**
- Check neuron parameters (threshold, leak rate)
- Verify network connectivity
- Ensure proper current injection

#### "Configuration file not found"
**Problem:** Cannot load configuration file
**Solution:**
- Verify file path and permissions
- Check file extension (.json, .yaml, .yml, or plain text)
- Use `--validate-only` to test configuration

#### "Insufficient synapses for STDP test"
**Problem:** Not enough synapses for plasticity testing
**Solution:**
- Increase neuron count in configuration
- Check network connectivity parameters
- Use `--basic-test-only` to test network structure first

#### "Configuration validation failed"
**Problem:** Configuration contains invalid values
**Solution:**
- Use `--show-defaults` to see valid values
- Check type compatibility for each parameter
- Validate parameter ranges

### Getting Help

For additional assistance:

1. **Command-line help:**
   ```bash
   ./nlm_phase2_demo --help
   ```

2. **Documentation:**
   - Visit https://nlm.readthedocs.io for detailed documentation
   - Check local documentation in the `docs/` directory

3. **Examples:**
   - Review the example configuration files in `configs/`
   - Examine enhanced usage examples in this document

## Future Enhancements

The enhanced system provides a foundation for future improvements:

1. **Additional Output Formats**
   - Real-time visualization support
   - Machine-readable output formats
   - Custom report generation

2. **Advanced Configuration**
   - Configuration templates
   - Parameter optimization
   - Automated experiment design

3. **Enhanced Testing**
   - Reproducible experiments
   - Statistical analysis tools
   - Performance benchmarking

## Summary

The enhanced NLM system provides:

- **Improved Usability:** Intuitive command-line interface with comprehensive help
- **Enhanced Configurability:** Multiple configuration file formats and validation
- **Better Error Handling:** Clear error messages and graceful failure handling
- **Comprehensive Logging:** Detailed timing and performance metrics
- **Production Readiness:** Robust error handling and validation for reliable operation

These enhancements make the NLM system more accessible to beginners while providing advanced capabilities for experienced users and researchers.