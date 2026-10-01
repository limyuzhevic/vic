# NLM Test Framework

The NLM Test Framework provides a modular, configurable testing infrastructure for the Neural Learning Machine (NLM) project. It replaces the monolithic `main.cpp` test runner with a clean, extensible architecture.

## Overview

The Test Framework addresses several key issues in the original implementation:

- **Code Organization**: Extracted the massive 449-line `main.cpp` into a clean, maintainable structure
- **Modularity**: Separated concerns into distinct components (TestFramework, LearningExperiment, logging)
- **Configurability**: Added comprehensive configuration options via command line and config files
- **Extensibility**: Easy to add new test types without modifying core framework
- **Error Handling**: Improved error detection and reporting
- **Documentation**: Comprehensive help and usage documentation

## Architecture

```
NLM Test Framework
├── include/framework/TestFramework.hpp
├── src/framework/TestFramework.cpp
├── experiments/Experiment.hpp
├── experiments/Experiment.cpp
├── configs/default.cfg
└── docs/TEST_FRAMEWORK.md (this file)
```

## Key Components

### 1. TestFramework Class
The central class that manages the entire testing process:

- **Configuration Management**: Load from files and command line
- **Brain Setup**: Initialize and configure the neural network
- **Test Execution**: Run individual tests or full test suites
- **Result Tracking**: Collect and report test results
- **Status Reporting**: Provide comprehensive status information

### 2. LearningExperiment Struct
Extracted from main.cpp for better organization:

- **Data Collection**: Track initial/final synapse weights and activity
- **Statistical Analysis**: Compute and report learning metrics
- **Logging**: Output experiment results in a structured format

### 3. Configuration System
- **Config Files**: `configs/default.cfg` with sensible defaults
- **Command Line**: Override config via command line arguments
- **Flexible Values**: Support for various data types (int, float, string, etc.)

## Command Line Interface

### Basic Usage
```bash
# Run all tests with default configuration
./nlm

# Load custom configuration file
./nlm --config custom.cfg

# Run specific test modes
./nlm --test-mode connectivity
./nlm --test-mode plasticity
./nlm --test-mode stdp

# Run all tests with custom settings
./nlm --test-mode all --seed 12345 --duration 2000 --intensity 2
```

### Available Options

| Option | Description | Example |
|--------|-------------|---------|
| `--config file.cfg` | Load configuration from file | `--config my_config.cfg` |
| `--test-mode mode` | Select test mode (connectivity, plasticity, stdp, all) | `--test-mode connectivity` |
| `--seed number` | Set random seed for reproducibility | `--seed 42` |
| `--duration steps` | Set test duration in simulation steps | `--duration 1000` |
| `--intensity level` | Set test intensity (0=light, 1=medium, 2=heavy) | `--intensity 2` |

### Configuration File Format

```ini
# NLM Test Framework Configuration
# Default configuration for NLM Phase 2 testing

# Random seed for deterministic testing
random_seed = 42

# Simulation timestep (in seconds)
simulation_timestep = 0.001

# Brain configuration
neuron_count = 500
region_count = 1
connection_probability = 0.15

# STDP plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

## Test Modes

### Connectivity Test
**Purpose**: Verify basic neural connectivity and spike propagation
- **Default Duration**: 50 simulation steps
- **Key Metrics**: Spikes generated, neuron membrane potentials
- **Validation**: Checks that spikes propagate through the network

### Plasticity Test
**Purpose**: Demonstrate learning through experience
- **Default Duration**: 1000 simulation steps
- **Key Metrics**: Synaptic weight changes, learning detection
- **Validation**: Detects significant weight changes indicating learning

### STDP Test
**Purpose**: Verify spike-timing-dependent plasticity mechanisms
- **Default Trials**: 50 correlated pre/post spikes
- **Key Metrics**: Weight changes, potentiation detection
- **Validation**: Confirms LTP (long-term potentiation) from correlated activity

### All Tests
**Purpose**: Run the complete test suite in sequence
- **Sequence**: Connectivity → Plasticity → STDP
- **Validation**: All individual tests must pass for overall success

## Test Results

The Test Framework provides comprehensive result reporting:

### Result Format
```
=== Test Framework Results ===
Test: Connectivity | Status: Completed | Duration: 0.123s | Success: YES
Test: Plasticity    | Status: Completed | Duration: 2.456s | Success: YES
Test: STDP          | Status: Completed | Duration: 1.789s | Success: YES

Summary: 3 tests completed 3/3 passed
```

### Result Tracking
- **Individual Test Results**: Each test is tracked separately
- **Performance Metrics**: Duration and success/failure status
- **Comprehensive Reporting**: Summary and detailed results

## Configuration Constants

### Magic Numbers (Now Defined as Constants)

The following magic numbers from the original `main.cpp` have been replaced with named constants:

| Constant | Value | Description |
|----------|-------|-------------|
| `connectivityTestSteps_` | 50 | Number of steps in connectivity test |
| `plasticityTestSteps_` | 1000 | Number of steps in plasticity test |
| `stdpTestTrials_` | 50 | Number of trials in STDP test |
| `defaultTimestep_` | 0.001 | Default simulation timestep |
| `defaultNeuronCount_` | 500 | Default number of neurons |
| `defaultRegionCount_` | 1 | Default number of regions |
| `defaultConnectionProbability_` | 0.15 | Default connection probability |

### STDP Parameters
```cpp
// Configuration keys
const std::string keyRandomSeed_ = "random_seed";
const std::string keyTimestep_ = "simulation_timestep";
const std::string keyNeuronCount_ = "neuron_count";
const std::string keyRegionCount_ = "region_count";
const std::string keyConnectionProbability_ = "connection_probability";
```

## Error Handling

### Improved Error Detection

1. **Brain Initialization**: Checks and reports brain setup failures
2. **Region Access**: Validates access to brain regions
3. **Test Execution**: Reports test failures with descriptive messages
4. **Configuration Loading**: Graceful fallback to defaults on config errors

### Error Reporting Examples
```
NLM_LOG_ERROR("Failed to initialize brain");
NLM_LOG_ERROR("Could not access brain region for STDP test");
NLM_LOG_ERROR("Some tests failed");
```

## Logging and Output

### Log Levels
The framework uses the existing NLM logging system with these levels:

- **DEBUG**: Detailed debugging information
- **INFO**: General information and progress updates
- **WARNING**: Non-critical issues and fallbacks
- **ERROR**: Test failures and critical issues
- **CRITICAL**: Severe errors that prevent operation

### Output Formatting
- **Timestamps**: All log entries include timestamps
- **Structured Output**: Consistent formatting across all test outputs
- **Color Support**: Console colors for better readability (when supported)

## Adding New Test Types

The TestFramework makes it easy to add new test types:

```cpp
// In TestFramework.hpp
enum class TestMode {
    Connectivity,
    Plasticity,
    STDP,
    All,
    NewTest  // Example new test mode
};

// Add to TestFramework::runTest()
case TestMode::NewTest:
    success = runNewTest();
    break;
```

## Performance Considerations

### Test Configuration Options

The TestFramework supports various configuration options to balance performance and thoroughness:

1. **Test Duration**: Control how long each test runs
2. **Test Intensity**: Light/Medium/Heavy settings affect test thoroughness
3. **Neuron Count**: Configurable number of neurons for faster/slower tests
4. **Test Selection**: Run only specific tests instead of full suite

### Recommended Settings for Different Use Cases

| Use Case | Test Mode | Intensity | Duration | Neurons |
|----------|-----------|-----------|----------|---------|
| Development | All | Light | 100 | 100 |
| CI/CD | Connectivity | Light | 50 | 100 |
| Validation | All | Heavy | 2000 | 500 |
| Performance | Connectivity | Light | 50 | 200 |

## Backward Compatibility

The refactored codebase maintains backward compatibility:

- **Same Tests**: All original tests (connectivity, plasticity, STDP) are preserved
- **Same Behavior**: Test logic and results are identical to original implementation
- **Same Output**: Log format and content are consistent with original
- **Same Configuration**: Default values match original main.cpp

## Migration Guide

### From Original main.cpp

The original monolithic `main.cpp` has been replaced with the TestFramework:

**Old Structure**:
```cpp
// 449 lines in main.cpp containing:
- LearningExperiment struct (embedded)
- Three test functions (connectivity, plasticity, stdp)
- Basic command line parsing
- No error handling
- No configuration options
```

**New Structure**:
```cpp
// 183 lines in main.cpp using:
- TestFramework for test management
- LearningExperiment struct (separate files)
- Comprehensive command line parsing
- Robust error handling
- Extensive configuration options
```

### Migration Steps

1. **Update Build System**: Ensure TestFramework is included in CMakeLists.txt
2. **Test Configuration**: Update `configs/default.cfg` if needed
3. **Command Line**: Update any custom command line scripts
4. **Test Results**: Verify that test outputs are equivalent

## Troubleshooting

### Common Issues and Solutions

1. **Test Fails with "No neurons found"**
   - **Cause**: Configuration sets neuron_count too low
   - **Solution**: Increase `neuron_count` in config file or command line

2. **Test Fails with "Not enough synapses for STDP test"**
   - **Cause**: Connection probability too low or neuron count too small
   - **Solution**: Increase `connection_probability` or `neuron_count`

3. **Test Takes Too Long**
   - **Cause**: Heavy test intensity with large network
   - **Solution**: Reduce `--intensity` or `--duration` parameters

4. **Configuration File Not Loaded**
   - **Cause**: Incorrect file path
   - **Solution**: Verify file exists and use absolute paths

### Getting Help

```bash
# Build with help target
make help

# View configuration file
./nlm --help

# Check documentation
./nlm --docs
```

## Future Enhancements

### Planned Improvements

1. **Plugin Architecture**: Allow third-party test plugins
2. **Parallel Testing**: Run independent tests in parallel
3. **Continuous Integration**: Better CI/CD integration
4. **Performance Profiling**: Built-in performance monitoring
5. **Visualization**: Real-time test progress visualization
6. **Result Export**: Export results to various formats (JSON, CSV, etc.)

### Contribution Guidelines

1. **Test Types**: Add new test modes by extending `TestMode` enum
2. **Configuration**: Add new configuration options to `configs/default.cfg`
3. **Documentation**: Update this file with new features
4. **Code Quality**: Follow existing code style and conventions

## References

- [Original main.cpp](src/main.cpp) - Reference for original implementation
- [NLM Project Documentation](docs/) - General NLM project documentation
- [Configuration System](core/Config/) - Core configuration implementation
- [Logging System](core/Logger/) - Core logging implementation

## License

MIT License - See LICENSE file for details

---

*Document generated by NLM Test Framework refactoring*
*Version 1.0 - Phase 2 Refactoring*
*Last updated: $(date)*
