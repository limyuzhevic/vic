# Configuration Validation Framework

## Overview

The Configuration Validation Framework is a comprehensive system that builds on the existing schema validation in Config.hpp/Config.cpp to provide advanced configuration validation capabilities. It includes:

1. **Advanced Validation Features**
   - Cross-parameter validation
   - Conditional validation
   - Range validation with dynamic bounds
   - Dependency validation
   - Conflict detection

2. **Validation Rules Engine**
   - Rule-based validation system
   - Validation rule definitions with actions
   - Rule priorities and execution order
   - Validation result tracking with severity levels

3. **Integration Points**
   - Integration with the existing ConfigSchema system
   - Integration with the main initialization flow
   - Validation during configuration loading
   - Validation during runtime configuration changes
   - Validation reporting with detailed error messages

4. **Configuration for Validation**
   - Validation configuration file support
   - Command-line validation options
   - Runtime validation controls
   - Validation result persistence

5. **Validation Results and Reporting**
   - Detailed validation result objects with context
   - Validation result aggregation and summary reporting
   - Validation result export/import for reporting
   - Validation result filtering and grouping

## Directory Structure

The validation framework is organized into the following directories:

- `/workspace/app/src/core/Validation/` - Core validation framework files
  - `ValidationFramework.hpp` - Main validation framework header
  - `ValidationFramework.cpp` - Main validation framework implementation
  - `ValidationUtilities.hpp` - Validation utility functions
  - `ValidationUtilities.cpp` - Validation utility implementation
  - `ValidationIntegration.hpp` - Integration examples and utilities
  - `ValidationIntegration.cpp` - Integration implementation
  - `ValidationModule.hpp` - Validation module header
  - `ValidationModule.cpp` - Validation module implementation

- `/workspace/app/configs/` - Configuration files
  - `validation.cfg` - Default validation configuration file

- `/workspace/app/examples/` - Example applications
  - `ValidationExample.cpp` - Basic validation example
  - `ValidationIntegrationExample.cpp` - Comprehensive integration example
  - `ValidationTest.cpp` - Complete validation test

- `/workspace/app/tests/` - Test files
  - `test_validation_framework.cpp` - Framework validation tests
  - `test_basic_validation.cpp` - Basic validation tests
  - `test_config.cpp` - Configuration tests

## Key Components

### 1. ValidationFramework.hpp

This file contains the main validation framework class with:

- `ValidationResultDetail` - Detailed validation results with context
- `ValidationRule` - Definition of validation rules
- `ValidationRulesEngine` - Engine for executing validation rules
- `ValidationConfig` - Configuration options for validation
- `ValidationFramework` - Main validation framework class

Key features:
- Advanced validation with cross-parameter support
- Rule-based validation system
- Flexible configuration options
- Detailed result tracking
- Support for multiple output formats

### 2. ValidationUtilities.hpp

This file provides utility functions for:

- Parsing validation configuration files
- Exporting validation results to different formats (JSON, YAML, CSV, text)
- Loading validation rules from files
- Filtering and grouping validation results
- Integrating with existing configuration system

### 3. ValidationIntegration.hpp

This file provides integration examples showing how to:

- Initialize validation framework with configuration
- Validate configuration using schema or framework
- Generate validation reports
- Handle command-line validation options
- Work with custom validation rules

### 4. ValidationModule.hpp

This file provides a module interface for the validation framework with:

- Global validation framework instance
- Validation result management
- Configuration file loading and saving
- Statistics and reporting

### 5. validation.cfg

The default validation configuration file includes:

```ini
[validation_options]
enable_cross_parameter_validation = true
enable_conditional_validation = true
enable_range_validation = true
enable_dependency_validation = true
enable_conflict_detection = true
max_memory_usage = 100.0
max_validation_time = 10
report_all_warnings = false
validate_on_load = true
validate_on_set = true
validate_on_change = true

[command_line_options]
--validate-only             Enable validation-only mode (don't load config)
--validate-file FILE         Specify validation rules file
--skip-validation           Skip validation (use defaults)
--validation-level LEVEL    Set validation level (Info, Warning, Error, Critical)
--validation-report FILE     Save validation report to file
--validation-stats           Print validation statistics
```

## Usage Examples

### Example 1: Basic Validation

```cpp
#include "core/Config/Config.hpp"
#include "core/Validation/ValidationFramework.hpp"

int main() {
    // Create configuration
    nlm::Config config;
    config.set("neuron_count", 1000);
    config.set("simulation_timestep", 0.001);
    
    // Create schema
    nlm::ConfigSchema schema;
    schema.addParameter(nlm::ConfigParameter("neuron_count", "int64_t", 1000,
                                            "Number of neurons", "Core", false));
    
    // Initialize validation framework
    nlm::ValidationFramework framework;
    framework.initialize();
    
    // Validate configuration
    auto results = framework.validate(config, schema);
    
    // Display results
    for (const auto& result : results) {
        std::cout << result.toString() << std::endl;
    }
    
    return 0;
}
```

### Example 2: Integration with Existing Code

```cpp
#include "core/Config/Config.hpp"
#include "core/Validation/ValidationIntegration.hpp"

int main() {
    // Load configuration from file
    auto config = std::make_shared<nlm::Config>();
    config->loadFromFile("configs/default.cfg");
    
    // Override with command line arguments
    config->loadFromArgs(argc, argv);
    
    // Initialize validation framework
    auto framework = nlm::ValidationIntegration::initializeFramework("configs/validation.cfg");
    
    // Create schema
    nlm::ConfigSchema schema;
    // Add schema parameters...
    
    // Validate configuration
    auto results = nlm::ValidationIntegration::validateConfiguration(*config, schema);
    
    // Generate report
    std::string report = nlm::ValidationIntegration::validateAndGenerateReport(*config, schema);
    std::cout << report << std::endl;
    
    return 0;
}
```

### Example 3: Command-Line Validation

```bash
# Run with validation options
./program --config=debug.cfg --validate-only --validation-level=Error --validation-report=validation_report.txt

# Run without validation
./program --config=release.cfg --skip-validation

# Run with custom validation rules
./program --config=custom.cfg --validate-file=custom_validation.cfg --validation-stats
```

### Example 4: Validation Configuration File

```yaml
# validation_rules.yaml - Custom validation rules
rules:
  - id: "NEURON_COUNT_VALIDATION"
    name: "Neuron Count Validation"
    description: "Validates that neuron count is within reasonable limits"
    level: "Warning"
    priority: 100
    validator_type: "range"
    condition: "neuron_count > 1000000"
    message: "Neuron count exceeds maximum limit of 1,000,000"
    action: "Error"

  - id: "MEMORY_VALIDATION"
    name: "Memory Validation"
    description: "Validates memory constraints and available resources"
    level: "Error"
    priority: 90
    validator_type: "cross_parameter"
    condition: "estimated_memory > 100.0"
    message: "Estimated memory usage exceeds 100 MB limit"
    action: "Error"

  - id: "PLASTICITY_VALIDATION"
    name: "Plasticity Validation"
    description: "Validates plasticity parameters and their combinations"
    level: "Warning"
    priority: 80
    validator_type: "conditional"
    condition: "enable_plasticity == true"
    message: "Plasticity is enabled but no plasticity rules are configured"
    action: "Error"
```

## Testing

Run the validation framework tests:

```bash
# Run all validation tests
cd /workspace/app
./build/tests test_validation_framework

# Run basic validation tests
./build/tests test_basic_validation

# Run validation example
./build/examples/ValidationExample

# Run validation integration example
./build/examples/ValidationIntegrationExample

# Run validation test
./build/examples/ValidationTest
```

## Building

The validation framework is built as part of the main project. To build all validation framework components:

```bash
cd /workspace/app
mkdir -p build
cd build
cmake ..
make
```

## Dependencies

The validation framework depends on:

- C++11 or later
- Standard C++ libraries
- No external dependencies beyond the standard library

## License

This validation framework is part of the NLM (Neural Learning Machine) project and is licensed under the same terms as the main project.

## Future Enhancements

Potential future enhancements for the validation framework:

1. **Plugin System**: Allow external validation plugins to be loaded dynamically
2. **Distributed Validation**: Support for distributed validation across multiple processes
3. **Machine Learning**: Integration with machine learning models for dynamic validation rules
4. **Real-time Validation**: Continuous validation during system operation
5. **Web Interface**: Web-based validation dashboard and reporting
6. **REST API**: REST API for programmatic access to validation functionality
7. **Docker Support**: Docker containers for validation services
8. **CI/CD Integration**: Integration with continuous integration and deployment pipelines

## Support

For issues, questions, or feature requests:

- File issues in the project repository
- Consult documentation in the source code
- Check example applications for usage patterns
- Review test files for unit testing approaches

## Conclusion

The Configuration Validation Framework provides a robust, flexible, and extensible solution for validating configuration parameters in the NLM project. It builds on the existing schema validation while adding advanced features for cross-parameter validation, conditional validation, and conflict detection. The framework integrates seamlessly with the existing configuration system and provides comprehensive reporting and export capabilities.
