# NLM Advanced CLI Parser Documentation

## Overview

The NLM (Neural Learning Machine) Advanced CLI Parser is a comprehensive command-line interface system designed for the NLM project. It provides expert-level features while maintaining backward compatibility with the existing `Config::loadFromArgs()` method.

## Key Features

### 1. Advanced Argument Types

#### Positional Arguments
- **Definition**: Arguments that appear in the command line without flags
- **Validation**: Can be required or optional
- **Usage**: `parser.addPositional("input_file", "Input file path", true);`

#### Flag Arguments
- **Definition**: Boolean flags that enable/disable features
- **Usage**: `parser.addFlag("verbose", "Enable verbose output");`

#### Option Arguments
- **Definition**: Key-value pairs with flags
- **Usage**: `parser.addOption("input", "Input file path");`

#### Enum Arguments
- **Definition**: Arguments with a limited set of allowed values
- **Usage**: `parser.addEnum("format", "Output format", {"json", "yaml", "toml"}, "json");`

#### Range Arguments
- **Definition**: Arguments with minimum and maximum bounds
- **Usage**: `parser.addRange("steps", "Number of steps", 100, 1000, 500);`

#### File Arguments
- **Definition**: Arguments that represent file paths
- **Usage**: `parser.addFile("weights", "Neural network weights file", false);`

#### Path Arguments
- **Definition**: Arguments that represent file or directory paths
- **Usage**: `parser.addPath("checkpoint", "Checkpoint file path", false, true);`

### 2. Value Types

#### String
- **Usage**: `parser.addOption("name", "Neural network name", "default_network");`

#### Integer
- **Usage**: `parser.addOption("neuron_count", "Number of neurons", "500");`

#### Float
- **Usage**: `parser.addOption("learning_rate", "Learning rate", "0.001");`

#### Boolean
- **Usage**: `parser.addFlag("use_gpu", "Use GPU acceleration");`

#### Vector
- **Usage**: `parser.addVector("learning_rates", "Learning rates (comma-separated)", "float");`

#### Neural Network Hyperparameters
- **Usage**: `parser.addOption("nn_type", "Neural network type (autoencoder|mlp|cnn)", "autoencoder");`

### 3. Expert Features

#### Environment Variable Fallback
- **Purpose**: Allow environment variables to override command-line arguments
- **Usage**: `parser.addEnvVariable("verbose", "NLM_VERBOSE");`
- **Priority**: Environment variables override command-line arguments

#### Configuration File Integration
- **Purpose**: Load configuration from JSON, YAML, or key-value files
- **Usage**: `parser.addConfigFile("config", "json");`
- **Priority**: Configuration files override defaults but are overridden by command-line arguments

#### Defaults File Support
- **Purpose**: Provide default values for arguments
- **Usage**: `parser.setDefaultsFile("configs/defaults.cfg");`
- **Priority**: Defaults are overridden by configuration files and command-line arguments

#### Profile Management
- **Purpose**: Switch between different configuration sets
- **Usage**: 
  ```cpp
  parser.addProfile("default");
  parser.addProfile("fast");
  parser.addProfile("high_quality");
  parser.setProfile("fast");
  ```

### 4. Command Structure

#### Subcommands
- **Purpose**: Organize related arguments into logical groups
- **Usage**: `parser.addSubcommand("run", "Run neural network simulation");`
- **Syntax**: `--run.nn_type autoencoder --run.learning_rate 0.001`

#### Nested Options
- **Purpose**: Group related arguments under subcommands
- **Usage**: `parser.addNestedOption("run", "nn_type", "Neural network type", "autoencoder");`
- **Syntax**: `--run.nn_type autoencoder`

#### Command Discovery and Help Generation
- **Purpose**: Provide users with information about available commands and options
- **Usage**: `parser.getHelpText(); parser.getCommandHelp("run");`

### 5. Integration

#### With Existing Config Class
- **Purpose**: Se seamlessly integrate with the existing configuration system
- **Usage**: `parser.applyToConfig(config);`
- **Benefits**: Backward compatibility, unified configuration management

#### Environment Variable Override System
- **Purpose**: Allow environment variables to override command-line arguments
- **Usage**: `parser.addEnvVariable("verbose", "NLM_VERBOSE");`

#### Configuration File Loading
- **Purpose**: Load configuration from files with format detection
- **Usage**: `--config=file.json`

#### Output Formatting
- **Purpose**: Support multiple output formats
- **Usage**: `--output=json` or `--output=console`

### 6. Error Handling

#### Comprehensive Error Messages with Suggestions
- **Purpose**: Provide helpful error messages to users
- **Usage**: `parser.addErrorMessage("invalid_field", "Field must be a valid integer");`

#### Interactive Help and Examples
- **Purpose**: Guide users with examples and explanations
- **Usage**: `parser.getHelpText();`

#### Configuration Validation
- **Purpose**: Validate arguments with custom functions
- **Usage**: `parser.addValidator("learning_rate", [](const ConfigValue& v) { ... });`

#### Command Completion Support
- **Purpose**: Support tab completion for command-line interfaces
- **Usage**: `parser.getCommandCompletions("run"); parser.getOptionCompletions("--learn");`

## Usage Examples

### Basic Usage
```cpp
#include "core/CLI/AdvancedCLI.h"

int main(int argc, char** argv) {
    CLIParser parser;

    // Add arguments
    parser.addFlag("verbose", "Enable verbose output");
    parser.addOption("input", "Input file path");
    parser.addOption("output", "Output file path");
    parser.addEnum("format", "Output format", {"json", "yaml", "toml"}, "json");

    // Parse command line
    if (!parser.parse(argc, argv)) {
        std::cout << "Error: " << parser.getErrorReport() << std::endl;
        std::cout << "\nUsage:" << std::endl;
        std::cout << parser.getHelpText() << std::endl;
        return 1;
    }

    // Apply to config
    auto config = std::make_shared<Config>();
    if (!parser.applyToConfig(config)) {
        std::cout << "Error applying arguments: " << parser.getErrorReport() << std::endl;
        return 1;
    }

    return 0;
}
```

### Advanced Usage
```cpp
#include "core/CLI/AdvancedCLI.h"

int main(int argc, char** argv) {
    CLIParser parser;

    // Set up expert features
    parser.addEnvVariable("verbose", "NLM_VERBOSE");
    parser.addConfigFile("config", "json");
    parser.setDefaultsFile("configs/defaults.cfg");
    parser.addProfile("default");
    parser.addProfile("fast");
    parser.setProfile("fast");

    // Add subcommands
    parser.addSubcommand("run", "Run neural network simulation");
    parser.addSubcommand("train", "Train neural network model");
    parser.addNestedOption("run", "nn_type", "Neural network type", "autoencoder");
    parser.addNestedOption("run", "learning_rate", "Learning rate", "0.001");

    // Parse and execute
    if (parser.parse(argc, argv)) {
        auto config = std::make_shared<Config>();
        parser.applyToConfig(config);

        // Execute the requested command
        std::string command = "run";
        if (parser.hasArgument("command")) {
            command = std::get<std::string>(parser.getArgumentValue("command"));
        }

        executeCommand(command, config);
    }

    return 0;
}
```

### Neural Network Simulation Example
```bash
# Basic simulation
./nlm_simulation --input=data.json --output=result.json --format=json

# With environment variable override
export NLM_STEPS=2000
./nlm_simulation --input=data.json --output=result.json --steps=1000

# With nested options (run subcommand)
./nlm_simulation run --run.nn_type=cnn --run.learning_rate=0.01 --run.batch_size=64

# With config file
./nlm_simulation --config=experiment.json --output=result.json

# With profile
./nlm_simulation --profile=high_quality --output=result.json
```

## Command Line Syntax

### Basic Options
```
--verbose                 Enable verbose output
--quiet                   Suppress non-essential output
--debug                   Enable debug mode
--help                    Show help information
--version                 Show version information
```

### Value Options
```
--input=<file>            Input file path
--output=<file>           Output file path
--config=<file>           Configuration file path
--format=<json|yaml|toml> Output format (default: json)
--steps=<int>             Number of simulation steps (range: 100..10000, default: 1000)
```

### Subcommands
```
run                       Run neural network simulation
  --run.nn_type=<autoencoder|mlp|cnn>    Neural network type (default: autoencoder)
  --run.learning_rate=<float>           Learning rate (default: 0.001)
  --run.batch_size=<int>                Batch size (default: 32)
  --run.steps=<int>                     Number of steps (default: 1000)

train                     Train neural network model
  --train.epochs=<int>                  Number of epochs (default: 100)
  --train.learning_rate=<float>         Learning rate (default: 0.01)
  --train.batch_size=<int>              Batch size (default: 64)

simulate                  Run simulation with current brain state
  --simulate.timestep=<float>           Simulation timestep (default: 0.001)
  --simulate.max_steps=<int>            Maximum simulation steps (default: 10000)
```

### Vector Arguments
```
--learning_rates=<float>,<float>,... Learning rates (comma-separated)
--batch_sizes=<int>,<int>,...        Batch sizes (comma-separated)
--hidden_sizes=<int>,<int>,...       Hidden layer sizes (comma-separated)
```

## Configuration Priority

When multiple sources provide values for the same argument, the following priority is used (highest priority first):

1. Command-line arguments
2. Configuration files
3. Environment variables
4. Defaults file
5. Hard-coded defaults

## Thread Safety

The Advanced CLI Parser is thread-safe by default. All operations are protected by mutex locks when `threadSafe` is enabled (default: true).

## Performance

The parser is optimized for performance:

- **Parsing Time**: O(n) where n is the number of arguments
- **Lookup Time**: O(1) for argument access
- **Memory Usage**: Minimal overhead with move semantics
- **Validation**: Lazy validation only when needed

## Error Codes

### Common Error Codes
- **0**: Success
- **1**: Command-line parsing error
- **2**: Configuration file error
- **3**: Argument validation error
- **4**: Configuration application error

### Error Message Format
```
Error: Argument <argument_name> is invalid
  Expected: <expected_format>
  Got: <actual_value>
  Suggestions: <suggestion1>, <suggestion2>, ...
```

## Files

### Core Files
- `AdvancedCLI.h` - Header file with all CLI classes and interfaces
- `AdvancedCLI.cpp` - Implementation of the CLI parser

### Example Files
- `AdvancedCLIExample.cpp` - Basic example of using the Advanced CLI Parser
- `AdvancedCLIDemo.cpp` - Comprehensive demonstration of all features

### Test Files
- `CLIAdvancedTests.cpp` - Comprehensive test suite for the Advanced CLI
- `TestAdvancedCLI.cpp` - Simple integration test
- `CLIIntegrationTest.cpp` - Integration test with existing Config class

### Documentation
- `AdvancedCLIDoc.md` - Complete documentation of the Advanced CLI Parser

## Build Instructions

### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Build with Tests
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_BUILD_TESTS=ON
make -j4
```

### Run Tests
```bash
cd build
./nlm_test
```

### Run Examples
```bash
cd build
./advanced_cli_example --help
./advanced_cli_demo --verbose --input=test.json
```

## Integration with Existing NLM Code

The Advanced CLI Parser is designed to integrate seamlessly with the existing NLM codebase:

### 1. Backward Compatibility
The parser maintains full backward compatibility with the existing `Config::loadFromArgs()` method:

```cpp
// Old style (still works)
config->loadFromArgs(argc, argv);

// New style (recommended)
CLIParser parser;
parser.addOption("input", "Input file path");
parser.addOption("output", "Output file path");
parser.parse(argc, argv);
parser.applyToConfig(config);
```

### 2. Configuration Integration
The parser can directly apply parsed arguments to the existing `Config` class:

```cpp
auto config = std::make_shared<Config>();
CLIParser parser;
parser.addOption("input", "Input file path");
parser.addOption("output", "Output file path");
parser.parse(argc, argv);
parser.applyToConfig(config);
```

### 3. Environment Variable Support
Environment variables can be used to override command-line arguments:

```cpp
CLIParser parser;
parser.addEnvVariable("verbose", "NLM_VERBOSE");
parser.addEnvVariable("output", "NLM_OUTPUT_FORMAT");
parser.addEnvVariable("config", "NLM_CONFIG_FILE");
```

### 4. Profile Management
Profiles allow switching between different configuration sets:

```cpp
CLIParser parser;
parser.addProfile("default");
parser.addProfile("fast");
parser.addProfile("high_quality");
parser.setProfile("fast");
```

### 5. Neural Network Integration
The parser supports neural network hyperparameters:

```cpp
CLIParser parser;
parser.addOption("nn_type", "Neural network type (autoencoder|mlp|cnn)", "autoencoder");
parser.addOption("activation", "Activation function (relu|tanh|sigmoid|elu)", "relu");
parser.addOption("optimizer", "Optimizer (adam|sgd|rmsprop)", "adam");
parser.addOption("loss_function", "Loss function (mse|crossentropy|bce)", "mse");
```

## Migration Guide

### From Basic Config::loadFromArgs() to Advanced CLI

#### Step 1: Replace Basic Parsing
```cpp
// Old
config->loadFromArgs(argc, argv);

// New
CLIParser parser;
parser.addOption("input", "Input file path");
parser.addOption("output", "Output file path");
parser.parse(argc, argv);
parser.applyToConfig(config);
```

#### Step 2: Add Advanced Features
```cpp
CLIParser parser;
parser.addEnvVariable("verbose", "NLM_VERBOSE");  // Environment variable support
parser.addConfigFile("config", "json");            // Configuration file support
parser.addProfile("default");                      // Profile support
parser.addProfile("fast");
parser.setProfile("fast");
```

#### Step 3: Add Subcommands
```cpp
CLIParser parser;
parser.addSubcommand("run", "Run neural network simulation");
parser.addSubcommand("train", "Train neural network model");
parser.addSubcommand("simulate", "Run simulation with current brain state");

parser.addNestedOption("run", "nn_type", "Neural network type", "autoencoder");
parser.addNestedOption("run", "learning_rate", "Learning rate", "0.001");
```

#### Step 4: Add Validation
```cpp
CLIParser parser;
parser.addValidator("learning_rate", [](const ConfigValue& v) {
    if (std::holds_alternative<double>(v)) {
        double lr = std::get<double>(v);
        return lr >= 0.0001 && lr <= 0.1;
    }
    return false;
});
```

### Troubleshooting

#### Common Issues and Solutions

**Issue**: Command-line parsing fails
**Solution**: Check for syntax errors in command-line arguments

**Issue**: Configuration file not found
**Solution**: Ensure the configuration file exists and is readable

**Issue**: Environment variable not set
**Solution**: Check if the environment variable is set before running the program

**Issue**: Validation fails
**Solution**: Check the validation function and test values

**Issue**: Profile not found
**Solution**: Add the profile before setting it

#### Debugging Tips

1. Use verbose mode to see more information:
   ```bash
   ./program --verbose
   ```

2. Check for error messages:
   ```bash
   ./program --help
   ```

3. Test with simple command lines:
   ```bash
   ./program --input=test.txt --output=result.txt
   ```

4. Use the help system to understand available options:
   ```bash
   ./program --help
   ```

## Future Enhancements

### Planned Features

1. **Web-based CLI Interface**: Web-based command-line interface
2. **Automatic Completion**: Enhanced tab completion with context awareness
3. **Configuration Templates**: Pre-defined configuration templates
4. **Distributed Configuration**: Support for distributed configuration management
5. **Real-time Configuration Updates**: Live updates without restarting the program

### Performance Optimizations

1. **Parallel Parsing**: Parallel parsing for large command-line argument sets
2. **Caching**: Argument caching for frequently accessed values
3. **Lazy Evaluation**: Lazy evaluation of validation functions
4. **Memory Pools**: Memory pools for reduced allocation overhead

## Support

### Documentation
- Complete documentation in `AdvancedCLIDoc.md`
- API documentation in header files
- Usage examples in demo files

### Testing
- Comprehensive test suite in `CLIAdvancedTests.cpp`
- Integration tests in `TestAdvancedCLI.cpp`
- Unit tests for individual components

### Support
For support with the Advanced CLI Parser, please:
1. Check the documentation in `AdvancedCLIDoc.md`
2. Review the test files for examples
3. Check the demo files for usage patterns
4. Contact the development team if issues persist

## Conclusion

The NLM Advanced CLI Parser provides a comprehensive solution for command-line argument parsing with expert-level features while maintaining backward compatibility with the existing `Config::loadFromArgs()` method. It supports advanced argument types, expert features, command structures, and seamless integration with the existing NLM codebase.

The parser is designed for performance, thread safety, and ease of use, making it suitable for both simple and complex command-line interfaces in the NLM project.