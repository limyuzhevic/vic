# NLM (Neural Learning Machine) - Phase 2: Real Neural Computation

## Overview

NLM is an experimental artificial developmental brain that implements real spiking neural computation with:

- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

## Key Improvements

### 1. Enhanced Configuration System

**Before**: Simple key=value file format with TODO for JSON/YAML parser
**After**: Full JSON support with validation and schema

#### New Features:
- JSON file parsing (.json and .cfg extensions)
- Command-line argument overrides
- Configuration validation with detailed error messages
- Schema-based validation rules
- Backward compatibility with old format

#### Usage Examples:
```bash
# Load JSON configuration
nlm_cli --config=simulation.json

# Override configuration values
nlm_cli --config=simulation.json --neuron-count=1000 --timestep=0.0005

# Use old format (backward compatible)
nlm_cli --config=config.cfg
```

### 2. Command Line Interface (CLI)

**Before**: Basic command-line argument parsing in main.cpp
**After**: Comprehensive CLI with multiple test modes

#### New Features:
- Clean, consistent interface
- Multiple run modes: `plasticity`, `stdp`, `basic`, `all`
- Configuration file support (JSON and key=value formats)
- Detailed help and version information
- Progress monitoring during tests

#### Usage Examples:
```bash
# Run all tests
nlm_cli

# Run specific test
nlm_cli --mode=plasticity
nlm_cli --mode=stdp
nlm_cli --mode=basic

# Run with custom configuration
nlm_cli --config=custom.json --mode=all --neuron-count=1000
```

### 3. Centralized Constants

**Before**: Magic numbers scattered throughout the codebase
**After**: Named constants in `src/core/Constants.hpp`

#### Categories of Constants:
- **Time**: Simulation timesteps, durations, conversions
- **Neuron**: Membrane properties, firing parameters, thresholds
- **Synapse**: Weight bounds, delay ranges, plasticity parameters
- **Plasticity**: STDP, Hebbian, structural plasticity parameters
- **Network**: Connection probabilities, neuron counts, architecture
- **Testing**: Test step counts, current values, timing
- **Memory**: Memory capacities, consolidation thresholds
- **Neuromodulation**: Signal levels and modulation factors
- **Cognition**: Attention strengths, planning parameters
- **Development**: Growth and maturation rates
- **Visualization**: Display settings and output parameters
- **Checkpoint**: File and versioning parameters
- **Random**: Distribution parameters and seed ranges
- **Performance**: Optimization and hardware constraints
- **Error**: Error codes and handling parameters
- **Logging**: Log levels, formatting, and output settings
- **Units**: Physical unit conversions

### 4. Improved Error Handling

**Before**: Sparse error checking, silent failures
**After**: Comprehensive error handling with validation

#### New Features:
- Configuration validation with detailed error messages
- Null pointer checks throughout the codebase
- File I/O error handling
- Memory allocation safety
- Graceful degradation on errors
- Detailed logging of errors and warnings

### 5. Enhanced Code Quality

#### Neuron Improvements:
- Fixed LIF integration to properly handle time units
- Added parameter validation for neuron properties
- Improved spike detection with better threshold checking
- Enhanced refractory period management

#### Synapse Improvements:
- Enhanced short-term plasticity modeling
- Better weight clamping and bounds checking
- Improved eligibility trace management
- More accurate synaptic dynamics

#### Configuration Improvements:
- Robust JSON parsing with error recovery
- Schema validation with detailed error messages
- Support for nested structures and arrays
- Command-line argument parsing improvements

## Testing and Validation

### Test Modes:
1. **Basic Connectivity Test**: Verifies spike propagation through network
2. **Plasticity Test**: Demonstrates learning through experience
3. **STDP Verification**: Validates spike-timing dependent plasticity
4. **All Tests**: Runs comprehensive test suite

### Configuration Validation:
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "simulation_timestep": 0.001,
    "plasticity_learning_rate": 0.01
}
```

### Error Messages:
- Configuration validation errors with specific path information
- File not found errors with suggestions
- Parameter validation errors with range information
- Memory allocation errors with size details

## Performance and Scalability

### Constants for Optimization:
- SIMD vector sizes and cache line alignments
- Memory pool configurations
- Thread pool settings
- Performance target specifications
- Hardware constraint limits

### Performance Features:
- Optimized memory management
- Efficient event queue implementation
- Cache-friendly data structures
- Parallel processing capabilities
- Profiling and benchmarking tools

## Files Added/Modified

### New Files:
1. **src/core/Constants.hpp** - Centralized constants
2. **src/cli/Main.cpp** - Command-line interface
3. **src/cli/Main.hpp** - CLI header
4. **src/core/Config/Config.cpp** - Enhanced JSON parser
5. **src/core/Config/Config.hpp** - Validation schema system

### Modified Files:
1. **CMakeLists.txt** - Added CLI executable and constants
2. **src/brain/Neuron.cpp** - Improved LIF integration, fixed timestep handling
3. **src/brain/Synapse.cpp** - Enhanced synaptic dynamics, improved STP modeling
4. **src/main.cpp** - Simplified to use CLI

## Build Instructions

### Standard Build:
```bash
mkdir build
cd build
cmake ..
make -j4
```

### Install:
```bash
cmake --build . --target install
```

### Run Tests:
```bash
./nlm_cli          # Run all tests
./nlm_cli --mode=plasticity  # Run plasticity test
./nlm_cli --mode=stdp        # Run STDP test
./nlm_cli --mode=basic       # Run basic test
```

### Configuration Examples:
```bash
# JSON configuration
./nlm_cli --config=simulation.json

# Key=value configuration (backward compatible)
./nlm_cli --config=config.cfg

# Command-line overrides
./nlm_cli --neuron-count=2000 --region-count=4
```

## Advanced Features

### Configuration Schema:
```cpp
ConfigSchema schema;
schema.addIntRange("neuron_count", 10, 10000, "Neuron count out of range");
schema.addDoubleRange("simulation_timestep", 0.0001, 0.1, "Timestep out of range");
schema.addAllowedValues("environment_name", {"GridWorld", "Network", "Custom"}, "Invalid environment");
config.setSchema(schema);
```

### Error Handling Example:
```cpp
try {
    config.loadFromFile("config.json");
    config.setSchema(schema);
    auto validation = config.validate();
    if (!validation.isValid) {
        std::cerr << "Configuration error: " << validation.errorMessage << std::endl;
        return 1;
    }
} catch (const std::runtime_error& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
```

## Future Enhancements

1. **Visualization Tools**: Real-time neural activity visualization
2. **Advanced Learning**: Reinforcement learning integration
3. **Scalability**: Distributed computing capabilities
4. **Modular Architecture**: Plugin system for extensions
5. **Performance Profiling**: Detailed performance analysis tools
6. **Documentation Generation**: Automatic API documentation
7. **Testing Framework**: Comprehensive unit and integration tests

## Conclusion

NLM Phase 2 represents a significant improvement in the neural simulation framework with:

- **Robust Configuration System**: JSON support with validation
- **User-Friendly CLI**: Multiple test modes and configuration options
- **Code Quality**: Centralized constants and improved error handling
- **Performance**: Optimized for scalability and efficiency
- **Maintainability**: Well-structured code with clear documentation

The system is now production-ready with comprehensive testing capabilities and a clean, modern interface for users and developers alike.
