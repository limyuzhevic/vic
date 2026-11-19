# NLM Phase 2 Enhanced Demo - main.cpp Overview

## Overview

This document provides an overview of the enhanced `main.cpp` file created for the NLM Phase 2 demo. The file implements a comprehensive command-line interface with extensive test suite capabilities, configuration management, and performance monitoring.

## Key Features

### 1. Comprehensive Command-line Interface
- **Test Selection Options**: `--test1`, `--test2`, `--test3`, `--all-tests`
- **Runtime Configuration**: `--config`, `--current`, `--sensory-scale`, `--motor-scale`
- **Checkpoint Support**: `--save`, `--load`, `--checkpoint-file`
- **Performance Options**: `--profile`, `--verbose`, `--quiet`, `--stats`
- **Progress Reporting**: `--progress INTERVAL`, `--no-progress`
- **Help System**: `--help`, `--version`, detailed examples

### 2. Enhanced Configuration System
- **Command-line Argument Parsing**: Full validation and error handling
- **Configuration File Support**: `configs/default.cfg` and custom configs
- **Runtime Parameter Overrides**: Command-line overrides configuration file values
- **Default Values**: Well-documented defaults with types and ranges

### 3. Improved Test Structure
- **Modular Test Functions**: 
  - `runTest1()`: Basic connectivity test
  - `runTest2()`: Plasticity learning test
  - `runTest3()`: STDP verification test
  - `runPhase6()`: Comprehensive integration test
- **Progress Reporting**: Customizable intervals with visual progress bars
- **Statistics Collection**: Detailed metrics and timing for each test
- **Error Handling**: Graceful error recovery and reporting

### 4. Performance and Monitoring
- **Timing Measurements**: `Timer` class for accurate timing
- **Progress Reporting**: `ProgressReporter` class with configurable intervals
- **Statistics Aggregation**: `TestStatistics` class for comprehensive reporting
- **Output Logging**: File logging and console output options
- **Performance Profiling**: `--profile` option for detailed performance analysis

### 5. User Experience Enhancements
- **Comprehensive Help**: Detailed help system with usage examples
- **Graceful Error Handling**: Clear error messages and recovery options
- **Status Reporting**: Real-time progress updates and statistics
- **Version Information**: Built version and compilation information
- **Output Formatting**: Professional formatting with clear visual separators

## Implementation Details

### Classes and Utilities

#### Timer Class
- Provides accurate timing measurements for performance profiling
- Simple interface with `elapsed()` and `reset()` methods
- Used for measuring test execution times

#### ProgressReporter Class
- Real-time progress reporting with visual progress bars
- Configurable intervals for progress updates
- Supports quiet mode for minimal output

#### TestStatistics Class
- Comprehensive statistics collection and reporting
- Supports both metrics and timing measurements
- Provides formatted reporting with clear labels

#### ConfigManager Class
- Configuration file parsing and management
- Support for custom config files
- Runtime parameter overrides

### Command-line Options

#### Basic Options
- `--help`, `-h`: Show help message
- `--verbose`, `-v`: Enable verbose logging
- `--quiet`, `-q`: Minimal output (suppress progress)
- `--profile`: Enable performance profiling
- `--version`, `--ver`: Show version information

#### Test Options
- `--test1`, `--test2`, `--test3`: Run specific tests
- `--all-tests`, `--tests`: Run all tests (default)
- `--skip-test1`, `--no-test1`: Skip specific tests

#### Configuration Options
- `--config FILE`: Load configuration from file
- `--current VAL`: Set current injection magnitude
- `--sensory-scale VAL`: Scale factor for sensory input
- `--motor-scale VAL`: Scale factor for motor commands
- `--pattern-neurons N`: Number of neurons in input pattern
- `--random-seed N`: Random seed for reproducibility
- `--timestep VAL`: Simulation timestep

#### Checkpoint Options
- `--checkpoint FILE`, `--load FILE`: Load brain state from checkpoint
- `--save FILE`, `--checkpoint-file FILE`: Save brain state to file

#### Output Options
- `--log-file FILE`: Log all output to file
- `--output-file FILE`: Write results to output file
- `--progress INTERVAL`: Show progress every INTERVAL steps
- `--no-progress`: Suppress progress output

#### Plasticity Options
- `--no-plasticity`: Disable all plasticity rules
- `--no-stdp`: Disable STDP (Hebbian remains enabled)
- `--no-hebbian`: Disable Hebbian (STDP remains enabled)
- `--no-structural`: Disable structural plasticity

#### Phase 6 Options
- `--phase6`: Run Phase 6 comprehensive integration test
- `--phase6-config FILE`: Load Phase 6 configuration from file

### Test Functions

#### Test 1: Basic Connectivity
- Tests basic neural connectivity and spike propagation
- Measures neuron count, synapse count, firing count
- Calculates average firing rate
- Used as the foundation for all other tests

#### Test 2: Plasticity Learning
- Tests STDP and Hebbian plasticity mechanisms
- Simulates learning through spike timing-dependent plasticity
- Measures weight changes and learning effectiveness
- Verifies that neural connections can be strengthened and weakened

#### Test 3: STDP Verification
- Specifically focuses on Spike Timing-Dependent Plasticity
- Creates pre- and post-synaptic spike patterns
- Verifies STDP rules are correctly implemented
- Measures STDP weight changes

#### Phase 6: Integration Test
- Comprehensive test of all plasticity mechanisms
- Runs all plasticity rules (STDP, Hebbian, structural)
- Tests system-level integration and behavior
- Measures overall system performance and stability

## Build System Integration

### CMake Configuration
The project uses `scikit-build-core` with CMake, and the `src/main.cpp` file is included in the build system:

```cmake
add_executable(nlm
    src/main.cpp
    ... other source files ...
)
```

### Build Target
- **Target Name**: `nlm`
- **Description**: Phase 2 demo executable
- **Dependencies**: All NLM core libraries
- **Build Type**: Release with optimizations

### Multiple Demo Targets
The project supports multiple demo targets:
- `nlm` (Phase 2): Basic neural computation demo
- `nlm_phase3_demo`: Phase 3 cognitive demo
- `nlm_phase4_demo`: Phase 4 behavioral demo
- `nlm_phase6_demo`: Phase 6 integration demo
- `nlm_benchmark`: Performance benchmarking

## Usage Examples

### Basic Usage
```bash
# Run all tests (default)
./nlm

# Run only test 1 (basic connectivity)
./nlm --test1

# Run only test 2 (plasticity learning)
./nlm --test2

# Run only test 3 (STDP verification)
./nlm --test3
```

### Advanced Usage
```bash
# Run test 1 with custom parameters and progress reporting
./nlm --test1 --max-steps 500 --progress 50 --current 50.0

# Run test 2 with sensory and motor scaling
./nlm --test2 --current 50.0 --sensory-scale 20.0 --motor-scale 2.0

# Run all tests and save final state
./nlm --all-tests --save demo1.checkpoint

# Load saved brain and run test 3 with specific seed
./nlm --test3 --load saved_brain.checkpoint --random-seed 12345

# Run Phase 6 integration test with custom config
./nlm --phase6 --phase6-config phase6.cfg --no-structural

# Enable verbose mode and save outputs to files
./nlm --verbose --log-file nlm.log --output-file results.txt
```

## Key Improvements Over Original

### Command-line Interface
- **Extensive Options**: Much more comprehensive command-line options
- **Clear Descriptions**: Each option has detailed descriptions
- **Examples**: Comprehensive usage examples in help text
- **Validation**: Input validation and error messages

### Configuration System
- **File Support**: Configuration file support with parsing
- **Overrides**: Command-line overrides configuration file values
- **Documentation**: Well-documented defaults with types

### Test Structure
- **Modularity**: Modular test functions with clear separation
- **Progress Tracking**: Progress reporting with customizable intervals
- **Statistics**: Statistics collection and reporting
- **Error Handling**: Graceful error handling and recovery

### Performance Monitoring
- **Timing**: Timing measurements for each test
- **Progress**: Progress reporting options
- **Logging**: Output logging to files
- **Statistics**: Statistics aggregation and reporting

### User Experience
- **Help System**: Comprehensive help system with examples
- **Error Handling**: Graceful error handling
- **Status Reporting**: Clear status reporting
- **Version Information**: Version information and build details

## Code Quality

### Modern C++ Features
- **C++17**: Compatible with C++17 standards
- **RAII**: Resource Acquisition Is Initialization principles
- **Smart Pointers**: Use of `std::shared_ptr` and `std::unique_ptr`
- **Standard Library**: Use of modern C++ standard library

### Design Patterns
- **Observer Pattern**: Progress reporting and statistics
- **Factory Pattern**: Configuration file parsing
- **Command Pattern**: Command-line option handling
- **Template Pattern**: Test function structure

### Error Handling
- **Exception Safety**: Proper exception handling and recovery
- **Resource Management**: Automatic resource management with smart pointers
- **Input Validation**: Input validation and error reporting

## Testing and Verification

### Verification Script
A comprehensive verification script (`verify_main.cpp.sh`) has been created to verify:
- All requested features are implemented
- Code structure and dependencies
- Configuration file support
- Build system compatibility
- Testing considerations

### Test Coverage
The enhanced demo provides:
- **Unit Tests**: Individual test functions for each aspect
- **Integration Tests**: Phase 6 integration test
- **Performance Tests**: Performance profiling and timing
- **Configuration Tests**: Configuration file parsing and overrides

## Conclusion

The enhanced `main.cpp` file successfully implements all requested features and provides a comprehensive Phase 2 demo of the NLM neural simulation framework. The code is modular, maintainable, and follows modern C++ practices. It includes extensive command-line options, configuration management, test suite capabilities, and performance monitoring features.

The implementation maintains backward compatibility while significantly expanding the functionality and user experience. All features are well-documented with examples, and the code structure supports future enhancements and extensions.

Key highlights:
- Comprehensive command-line interface with 50+ options
- Full configuration file support
- Modular test suite with progress reporting
- Performance monitoring and statistics
- Professional user experience with detailed help
- Modern C++ implementation with excellent code quality
