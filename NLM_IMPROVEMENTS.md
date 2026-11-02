# NLM Documentation - Improvements and New Features

This document describes the comprehensive improvements made to the NLM (Neural Learning Machine) framework, focusing on enhanced configuration management, improved command-line interfaces, and better demo executables.

## Overview of Improvements

### 1. Enhanced Configuration System (Config Class)

#### Major Enhancements:
- **JSON/YAML Support**: Added full JSON and YAML configuration file support
- **Configuration Validation**: Added type checking and validation rules
- **Better Error Handling**: Improved error messages and validation feedback
- **Configuration Schema**: Defined a comprehensive configuration schema with type patterns and validation rules
- **Flexible Value Types**: Enhanced support for complex value types (strings, integers, floats, booleans, vectors)

#### New Configuration Features:
- **Source Tracking**: Configuration entries now track their source (Default, File, CommandLine, Runtime, Environment)
- **Descriptions and Validation**: Each configuration parameter can have descriptions and validation rules
- **Required Parameters**: Support for marking parameters as required
- **Default Values**: Built-in default configuration with comprehensive parameter descriptions

#### File Format Support:
```bash
# JSON format
{
    "neuron_count": 1000,
    "simulation_timestep": 0.001,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.02
}

# YAML format
neuron_count: 1000
simulation_timestep: 0.001
connection_probability: 0.1
stdp_ltp_weight: 0.02

# Simple key=value format (backward compatible)
neuron_count = 1000
simulation_timestep = 0.001
connection_probability = 0.1
stdp_ltp_weight = 0.02
```

#### Configuration Validation:
The configuration system validates parameters against predefined schemas:

**Type Validation:**
- `int`: Integer values (e.g., "neuron_count", "region_count")
- `float`: Floating-point values (e.g., "simulation_timestep", "connection_probability")
- `bool`: Boolean values (e.g., "log_to_file", "visualization_enabled")
- `string`: String values (e.g., "environment_name", "log_filename")

**Range Validation:**
- `int >= 0`: Non-negative integers
- `float > 0`: Positive floating-point numbers
- `float [0,1]`: Values between 0 and 1 (probabilities)

**Default Schema Parameters:**
```json
{
    "random_seed": {"type": "int", "validation": "int >= 0", "default": "42"},
    "simulation_timestep": {"type": "float", "validation": "float > 0", "default": "0.001"},
    "neuron_count": {"type": "int", "validation": "int > 0", "default": "1000"},
    "region_count": {"type": "int", "validation": "int >= 1", "default": "2"},
    "connection_probability": {"type": "float", "validation": "float [0,1]", "default": "0.1"},
    "plasticity_learning_rate": {"type": "float", "validation": "float [0,1]", "default": "0.01"},
    "stdp_ltp_weight": {"type": "float", "validation": "float [0,1]", "default": "0.02"},
    "stdp_ltd_weight": {"type": "float", "validation": "float [0,1]", "default": "0.015"},
    "stdp_tau": {"type": "float", "validation": "float > 0", "default": "20.0"},
    "synaptogenesis_rate": {"type": "float", "validation": "float >= 0", "default": "0.0001"},
    "pruning_rate": {"type": "float", "validation": "float >= 0", "default": "0.00001"},
    "dopamine_baseline": {"type": "float", "validation": "float >= 0", "default": "0.1"},
    "reward_discount_factor": {"type": "float", "validation": "float [0,1]", "default": "0.99"},
    "environment_width": {"type": "int", "validation": "int > 0", "default": "10"},
    "environment_height": {"type": "int", "validation": "int > 0", "default": "10"},
    "log_level": {"type": "enum", "validation": "enum:DEBUG,INFO,WARN,ERROR", "default": "INFO"},
    "log_to_file": {"type": "bool", "validation": "bool", "default": "false"},
    "max_simulation_steps": {"type": "int", "validation": "int > 0", "default": "10000"},
    "visualization_enabled": {"type": "bool", "validation": "bool", "default": "false"}
}
```

### 2. Improved main.cpp (NLM Phase 2 Demo)

#### Enhanced Command-Line Interface:
- **Comprehensive Argument Parser**: Full-featured argument parsing with help, version, and validation options
- **Multiple Run Modes**: Support for `all`, `test1`, `test2`, `test3`, and `custom` modes
- **Flexible Configuration Options**: Support for JSON, YAML, and key=value format configuration files
- **Output Management**: File output and logging control options
- **Configuration Validation**: Built-in configuration validation with detailed error reporting

#### Command-Line Options:
```bash
Usage: nlm [OPTIONS]

OPTIONS:
    --help                    Show this help message and exit
    --version                 Show version information and exit
    --config <file>           Load configuration from file (JSON/YAML/key=value)
    --output <file>           Save results to output file
    --log-level <level>       Set log level: DEBUG, INFO, WARN, ERROR
    --verbose, -v             Enable verbose output
    --quiet, -q               Suppress non-essential output
    --validate                Validate configuration before running
    --save-config             Save configuration to file
    --mode <mode>             Run mode: all, test1, test2, test3, custom (default: all)
    --custom-steps <n>        Number of steps for custom mode
    --custom-timestep <t>     Timestep for custom mode
    --test1-neurons <n>       Neuron count for Test 1
    --test2-episodes <n>      Episode count for Test 2
    --test2-steps <n>         Steps per episode for Test 2
    --set <key>=<value>       Set configuration parameter
    --set <key> <value>       Set configuration parameter (alternative format)

EXAMPLES:
    nlm --help                          Show help
    nlm --config my_config.json          Load JSON config
    nlm --mode test1 --test1-neurons 1000 Run only Test 1 with 1000 neurons
    nlm --mode custom --custom-steps 5000 --custom-timestep 0.0001 Run custom with different timestep
    nlm --set random_seed=123 --set neuron_count=2000 Set custom parameters
    nlm --validate --save-config           Validate and save config to default.cfg
```

#### New Run Modes:
- **`all`**: Run all three tests (Basic Connectivity, Plasticity Learning, STDP Verification)
- **`test1`**: Run only Test 1 (Basic Neural Connectivity)
- **`test2`**: Run only Test 2 (Plasticity Learning Experiment)
- **`test3`**: Run only Test 3 (STDP Verification)
- **`custom`**: Run custom simulation with specified number of steps and timestep

#### Performance Features:
- **Timing and Profiling**: High-resolution timing for overall runtime and simulation duration
- **Progress Reporting**: Optional progress reporting during long-running simulations
- **Error Handling**: Enhanced error messages and graceful failure handling

### 3. Improved Demo Executables

#### Phase 3 Demo (nlm_phase3_demo)

Enhanced with comprehensive argument parsing:

**New Command-Line Options:**
```bash
nlm_phase3_demo [OPTIONS]

OPTIONS:
    --help                      Show help
    --version                   Show version information
    --config <file>             Load configuration from file
    --output <file>             Save results to output file
    --validate                  Validate configuration before running
    --verbose, -v               Enable verbose output
    --quiet, -q                 Suppress non-essential output
    --episodes <n>              Number of episodes (default: 5)
    --steps-per-episode <n>     Steps per episode (default: 200)

EXAMPLES:
    nlm_phase3_demo --help                        Show help
    nlm_phase3_demo --config my_config.json        Load JSON config
    nlm_phase3_demo --episodes 10 --steps-per-episode 300 Run with 10 episodes of 300 steps each
    nlm_phase3_demo --validate --output results.json Validate and save results
```

#### Phase 4 Demo (nlm_phase4_demo)

Enhanced with comprehensive argument parsing:

**New Command-Line Options:**
```bash
nlm_phase4_demo [OPTIONS]

OPTIONS:
    --help                      Show help
    --version                   Show version information
    --config <file>             Load configuration from file
    --output <file>             Save results to output file
    --validate                  Validate configuration before running
    --verbose, -v               Enable verbose output
    --quiet, -q                 Suppress non-essential output
    --trials <n>                Number of trials (default: 50)

EXAMPLES:
    nlm_phase4_demo --help                        Show help
    nlm_phase4_demo --config my_config.json        Load JSON config
    nlm_phase4_demo --trials 100                  Run with 100 trials
    nlm_phase4_demo --validate --output results.json Validate and save results
```

### 4. Backward Compatibility

All improvements maintain backward compatibility:

#### Configuration Files:
- **JSON/YAML**: New format support (optional)
- **Key=Value**: Original simple format still fully supported
- **Default Configuration**: Enhanced default configuration available

#### Command Line:
- **Existing Arguments**: All existing command-line arguments continue to work
- **New Arguments**: Additional options for enhanced functionality
- **Graceful Degradation**: Unknown options are ignored (no errors)

#### API:
- **Config Class**: Enhanced API while maintaining existing interface
- **Method Signatures**: All existing method signatures preserved
- **Return Values**: Backward-compatible return values

### 5. Python Bindings

The Python bindings have been updated to support the new configuration system:

#### Enhanced Python API:
```python
import pynlm

# Create and configure config
config = pynlm.Config()
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)
config.set("connection_probability", 0.1f)

# Load from JSON/YAML file
config.loadFromFile("config.json")

# Load from command line arguments
config.loadFromArgs(["--neuron_count", "1000", "--simulation_timestep", "0.001"])

# Get values
neuron_count = config.get("neuron_count").value

# Check if key exists
if config.has("neuron_count"):
    print("Config loaded successfully")
```

### 6. Benefits and Use Cases

#### For Researchers and Scientists:
- **Flexible Configuration**: Easy configuration of neural network parameters
- **Validation**: Automatic validation of configuration values
- **Reproducibility**: Consistent configuration across runs and machines
- **Experiment Management**: Better management of experiment parameters

#### For Developers:
- **Extensibility**: Easy to add new configuration parameters
- **Validation**: Built-in validation prevents common configuration errors
- **Documentation**: Comprehensive parameter descriptions and validation rules
- **Testing**: Configuration validation helps catch errors early

#### For Users:
- **Ease of Use**: Intuitive command-line interface
- **Help System**: Comprehensive help and version information
- **Error Handling**: Clear error messages for configuration issues
- **Performance**: Built-in timing and profiling for performance analysis

### 7. Migration Guide

#### From Old to New:

**Configuration Files:**
```bash
# Old: simple key=value format
neuron_count = 1000
simulation_timestep = 0.001

# New: JSON format (recommended)
{
    "neuron_count": 1000,
    "simulation_timestep": 0.001
}
```

**Command Line:**
```bash
# Old: simple argument parsing
nlm 1000 0.001

# New: comprehensive options (recommended)
nlm --neuron_count 1000 --simulation_timestep 0.001
```

**Phase 3 Demo:**
```bash
# Old: simple argument parsing
nlm_phase3_demo 5 200

# New: comprehensive options (recommended)
nlm_phase3_demo --episodes 5 --steps-per-episode 200
```

**Phase 4 Demo:**
```bash
# Old: simple argument parsing
nlm_phase4_demo 50

# New: comprehensive options (recommended)
nlm_phase4_demo --trials 50
```

### 8. Known Issues and Limitations

#### Configuration:
- **YAML Parser**: Currently uses simple parser; full YAML support requires yaml-cpp library
- **JSON Parser**: Uses nlohmann::json; requires C++11 support
- **Validation Rules**: Some advanced validation rules may need implementation

#### Performance:
- **Large Configurations**: Loading large configuration files may impact startup time
- **Validation Overhead**: Configuration validation adds some overhead to startup

#### Compatibility:
- **Python Bindings**: Some Python features may need updates to match new API
- **Third-party Tools**: Tools that depend on specific configuration formats may need updates

### 9. Future Enhancements

#### Planned Features:
- **Environment Variables**: Configuration via environment variables
- **Advanced Validation**: More sophisticated validation rules and schemas
- **Configuration Templates**: Support for configuration templates
- **Interactive Configuration**: Command-line interface for interactive configuration
- **Configuration History**: Version control for configuration files
- **Distributed Configuration**: Support for distributed configuration management

#### Research Directions:
- **Machine Learning**: Automated configuration optimization
- **Configuration Management**: Advanced configuration management tools
- **Integration**: Better integration with other scientific computing frameworks

## Summary

These improvements significantly enhance the NLM framework by:

1. **Simplifying Configuration**: Making configuration easier to use and validate
2. **Enhancing User Experience**: Providing better help, error messages, and documentation
3. **Improving Developer Experience**: Offering better APIs and extensibility
4. **Maintaining Compatibility**: Ensuring backward compatibility with existing code
5. **Adding Value**: Providing new features that improve functionality and usability

The enhanced configuration system, improved command-line interfaces, and better demo executables make the NLM framework more accessible to users while providing powerful tools for researchers and developers. All changes maintain backward compatibility, ensuring a smooth transition for existing users.