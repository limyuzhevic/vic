# Enhanced Command-Line Interface for NLM

## Summary of Improvements

This document summarizes the enhancements made to the NLM (Neural Learning Machine) command-line interface in `src/main.cpp`.

## Previous Issues

1. **Limited command-line options** - Only had basic `--config` support
2. **No help/usage information** - No `-h` or `--help` option
3. **Basic error handling** - Limited validation and error messages
4. **Limited configuration flexibility** - Few options available

## Enhancements Implemented

### 1. Comprehensive Help/Usage
- **`-h, --help`** - Displays detailed help message with all available options
- **`-v, --version`** - Shows version information and build details
- Detailed description of all available options grouped by category
- Examples of usage for common scenarios

### 2. Enhanced Configuration Options
- **More configuration parameters** with sensible defaults:
  - Random seed control (`--random-seed`)
  - Neuron count configuration (`--neuron-count`)
  - Region count configuration (`--region-count`)
  - Simulation timestep control (`--timestep`)
  - Connection probability (`--connection-prob`)
  - STDP parameters (`--stdp-ltp-weight`, `--stdp-ltd-weight`, `--stdp-tau`)
  - Plasticity parameters (`--plasticity-learning-rate`)
  - Structural plasticity (`--synaptogenesis-rate`, `--pruning-rate`)
  - Environment configuration (`--env-name`, `--env-width`, `--env-height`)

- **Support for different test modes**:
  - `connectivity` - Basic neural connectivity test
  - `plasticity` - Plasticity learning experiment
  - `stdp` - STDP verification test
  - `all` - Run all tests (default)

- **Configuration file validation** - Validates numeric ranges and formats

### 3. Better Error Handling
- **Clear error messages** for invalid arguments
- **Graceful handling of missing files** with informative messages
- **Validation of configuration values** with bounds checking
- **Exception-safe test execution** with detailed error reporting

### 4. Extended Functionality
- **Selective test execution** (`--test MODE`) - Run only specific tests
- **Different verbosity levels**:
  - `--verbose, -V` - Increase verbosity (can be used multiple times)
  - `--quiet, -Q` - Reduce verbosity
  - `--log-level LEVEL` - Set logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
- **Output format options**:
  - `human` - Human-readable report (default)
  - `machine` - Machine-readable output
  - `json` - JSON-formatted report
  - `csv` - CSV-formatted report
- **Save/load configuration**:
  - `--save-config FILE` - Save current configuration to file
  - `--load-config FILE` - Load configuration from file

### 5. Additional Features
- **Random seed control** (`--random-seed`) for reproducible experiments
- **Neuron count configuration** (`--neuron-count`) for different simulation sizes
- **Simulation timestep control** (`--timestep`) for temporal resolution
- **Plasticity parameter configuration**:
  - STDP parameters (`--stdp-ltp-weight`, `--stdp-ltd-weight`, `--stdp-tau`)
  - Structural plasticity (`--synaptogenesis-rate`, `--pruning-rate`)

## Backward Compatibility

All existing functionality has been maintained:
- The `--config` option continues to work as before
- Existing configuration files are still supported
- Default configuration values are preserved
- Test execution behavior remains the same unless overridden by new options

## New Command-Line Options

### Basic Options
```bash
-h, --help                    Display help message
-v, --version                 Display version information
-c, --config FILE             Load configuration from file
-o, --output FILE             Save test results to file
```

### Test Execution
```bash
--test MODE                  Run specific test (connectivity, plasticity, stdp, all)
--run-tests-from FILE         Load test list from file
--save-config FILE           Save configuration
--load-config FILE           Load configuration
```

### Verbosity and Output
```bash
-l, --log-level LEVEL         Set logging level
--verbose, -V                 Increase verbosity
--quiet, -Q                   Reduce verbosity
--output-format FORMAT        Set output format (human, machine, json, csv)
-H, --human-readable          Human-readable report
-M, --machine-readable        Machine-readable output
```

### Configuration
```bash
--random-seed SEED            Set random seed
--neuron-count COUNT          Set neuron count
--region-count COUNT          Set region count
--timestep VALUE              Set simulation timestep
--connection-prob VALUE       Set connection probability
--stdp-ltp-weight VALUE       Set STDP LTP weight
--stdp-ltd-weight VALUE       Set STDP LTD weight
--stdp-tau VALUE              Set STDP time constant
--plasticity-learning-rate VALUE Set plasticity learning rate
--synaptogenesis-rate VALUE   Set synaptogenesis rate
--pruning-rate VALUE         Set pruning rate
--env-name NAME              Set environment name
--env-width WIDTH            Set environment width
--env-height HEIGHT           Set environment height
```

### Advanced
```bash
--max-steps VALUE            Set maximum simulation steps
--time-limit VALUE           Set time limit
--enable-visualization       Enable visualization
--visualization-rate RATE     Set visualization update rate
```

## Examples

### Basic Usage
```bash
# Run all tests with custom configuration
./nlm --config my_config.cfg --random-seed 123 --neuron-count 2000

# Run only connectivity test with machine-readable output
./nlm --test connectivity --output-format machine --output results.json

# Save current configuration and exit
./nlm --save-config current.cfg

# Run plasticity experiment with high verbosity
./nlm --test plasticity --verbose --verbose --random-seed 42
```

### Configuration Management
```bash
# Save current configuration
./nlm --save-config my_config.cfg

# Load configuration and run tests
./nlm --load-config my_config.cfg --test all
```

### Selective Test Execution
```bash
# Run only STDP test with human-readable output
./nlm --test stdp --output-format human

# Run connectivity test with machine-readable output
./nlm --test connectivity --output-format machine
```

## Technical Implementation Details

### Argument Parsing
- Uses vector-based argument parsing for flexibility
- Supports both `--key=value` and `--key value` formats
- Provides helper functions for argument validation

### Error Handling
- Comprehensive validation of numeric ranges
- Clear error messages with parameter names
- Graceful handling of file I/O errors
- Exception-safe test execution

### Output Formatting
- Multiple output formats supported
- JSON and CSV formats for machine processing
- Human-readable reports with detailed explanations
- Configurable verbosity levels

### Configuration Validation
- Type validation for all numeric parameters
- Range checking for probability values (0-1)
- Bounds checking for integer parameters
- Informative error messages for invalid values

## Build Information

- All new features are fully backward compatible
- No breaking changes to existing API
- Enhanced error handling improves robustness
- Configuration validation prevents invalid parameter combinations

The enhanced command-line interface provides comprehensive control over NLM simulations while maintaining simplicity for basic use cases and power for advanced users.