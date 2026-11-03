# NLM Examples

This directory contains working examples demonstrating various aspects of the NLM (Neural Learning Machine) framework.

## Overview

These examples are designed to help users get started with NLM, understand configuration management, and explore advanced features. Each example focuses on a specific use case or feature.

## Examples

### 1. Basic Configuration Loading (`basic_config.cpp`)

**Description**: Demonstrates basic configuration loading, validation, and value access.

**Key Features**:
- Load configuration from file (key=value format)
- Validate configuration with detailed error reporting
- Access configuration values with type safety
- Save configuration to file
- Generate detailed configuration reports

**Building**:
```bash
mkdir build-examples
cd build-examples
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --parallel 4
```

**Running**:
```bash
./build-examples/basic_config
```

**Expected Output**:
- Loads configuration from `configs/default.cfg`
- Validates all configuration parameters
- Prints all configuration values
- Saves validated configuration to `output_basic_config.cfg`
- Prints a detailed configuration report

**Error Handling**:
- File I/O errors
- Configuration parsing errors
- Validation failures
- Type conversion errors

### 2. Advanced Configuration Management (`advanced_config.cpp`)

**Description**: Shows advanced configuration management with multiple sources and command-line arguments.

**Key Features**:
- Load from multiple configuration sources (file, JSON, YAML, command-line)
- Configuration source precedence management
- Runtime configuration modification
- Configuration merging and overriding
- Advanced validation with custom rules

### 3. STDP Configuration (`stdp_config.cpp`)

**Description**: Demonstrates STDP (Spike-Timing-Dependent Plasticity) configuration and parameter tuning.

**Key Features**:
- Configure STDP parameters (LTP/LTD weights, time constant)
- Create different STDP variants (standard, reward-modulated, triplet)
- Parameter validation for biological plausibility
- Configuration export for scientific reproducibility

### 4. Performance Configuration (`performance_config.cpp`)

**Description**: Shows how to configure NLM for optimal performance.

**Key Features**:
- Performance-oriented configuration
- Memory pool configuration
- SIMD optimization settings
- Parallel processing configuration
- Benchmark configuration

### 5. Visualization Configuration (`visualization_config.cpp`)

**Description**: Demonstrates visualization configuration and output management.

**Key Features**:
- Visualization enable/disable
- Output format configuration
- Update rate settings
- Data export configuration
- Visualization integration

## Building All Examples

### Standard Build
```bash
mkdir build-examples
cd build-examples
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --parallel 4
```

### Build with Tests
```bash
mkdir build-examples
cd build-examples
cmake -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_TESTS=ON ..
cmake --build . --parallel 4
./examples_test
```

### Build with All Examples
```bash
mkdir build-all-examples
cd build-all-examples
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_ALL_EXAMPLES=ON ..
cmake --build . --parallel 4
```

## Running Examples

All examples are installed to the `bin` directory:

```bash
# List all examples
ls bin/

# Run a specific example
./bin/basic_config

# Run all examples
for example in bin/*; do
    if [ -x "$example" ]; then
        echo "Running $example"
        "$example"
        echo "---"
    fi
done
```

## Configuration Files

Each example may require additional configuration files:

### Example Configuration Files

- `configs/default.cfg`: Basic configuration template
- `configs/high_perf.cfg`: High-performance configuration
- `configs/scientific_config.json`: Scientific configuration (JSON)
- `configs/scientific_config.yml`: Scientific configuration (YAML)

### Output Configuration Files

- `output_basic_config.cfg`: Basic configuration output
- `output_advanced_config.cfg`: Advanced configuration output
- `benchmark_results.txt`: Benchmark results
- `test_results.txt`: Test results

## Configuration Validation

Each example includes configuration validation:

```cpp
// Validate configuration
if (!config.validate()) {
    std::cerr << "Configuration validation failed!" << std::endl;
    std::cout << config.getConfigurationReport();
    return 1;
}
```

The validation includes:

1. **Type validation**: Parameters have correct data types
2. **Range validation**: Parameters are within valid ranges
3. **Required parameter validation**: All required parameters are present
4. **Logical consistency**: Parameters make sense together

## Error Handling

Each example demonstrates proper error handling:

```cpp
try {
    // Load and validate configuration
    config.loadFromFile("configs/default.cfg");
    config.validate();
    
    // Perform operations
    processConfiguration(config);
    
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
```

## Testing Examples

Run the examples test suite:

```bash
# Build with tests
mkdir build-examples-test
cd build-examples-test
cmake -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_TESTS=ON ..
cmake --build . --parallel 4

# Run tests
./examples_test --verbose
```

## Example Extensions

### Adding New Examples

To add a new example:

1. Create a new `.cpp` file in the `examples/` directory
2. Add the example to `examples/CMakeLists.txt`
3. Document the example in `examples/README.md`
4. Add configuration files if needed

### Modifying Examples

Examples can be modified by:

1. Changing configuration file paths
2. Adding new configuration parameters
3. Modifying validation logic
4. Adding new features

## Troubleshooting

### Common Issues

1. **Configuration not loading**:
   - Check file paths
   - Ensure files have correct permissions
   - Verify file format matches expected type

2. **Validation failures**:
   - Check configuration report for specific errors
   - Ensure all required parameters are present
   - Verify parameter ranges are valid

3. **Build errors**:
   - Ensure NLM is properly installed
   - Check CMake configuration
   - Verify compiler supports C++20

### Debug Configuration

```bash
# Build with debug symbols
mkdir build-examples-debug
cd build-examples-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --parallel 4

# Run with debug output
./basic_config --debug
```

## Best Practices

### Configuration Management

1. **Use templates**: Create configuration templates for common use cases
2. **Validate early**: Validate configuration as early as possible
3. **Document configuration**: Keep configuration documentation in sync
4. **Use sources appropriately**: Use different configuration sources for different needs

### Error Handling

1. **Catch exceptions**: Use try-catch blocks for error handling
2. **Provide detailed messages**: Include specific error details
3. **Exit with non-zero code**: Return non-zero on error
4. **Clean up resources**: Ensure proper cleanup

### Code Quality

1. **Follow coding standards**: Use consistent coding style
2. **Include comments**: Document complex logic
3. **Use type safety**: Prefer strongly-typed operations
4. **Test thoroughly**: Write unit tests for new examples

## Migration Guide

### From Examples to Production

When moving from examples to production:

1. **Add proper error handling**: Ensure robust error handling
2. **Add configuration validation**: Validate configuration in production
3. **Add logging**: Add appropriate logging for debugging
4. **Add performance monitoring**: Monitor performance in production
5. **Add configuration management**: Use proper configuration management

### Example Configuration Files

Configuration files should be maintained in:

- `configs/`: Default configuration templates
- `examples/configs/`: Example-specific configurations
- `docs/configs/`: Configuration documentation

## License

This project is licensed under the MIT License.

See `LICENSE` for more information.

## Authors

Original Authors: NLM Development Team
Maintainers: NLM Contributors

For more information, visit the NLM GitHub repository.

## Acknowledgements

This project is made possible by the contributions of the NLM community and the open-source ecosystem.

## Support

For support, visit the NLM GitHub Issues page or contact the NLM team.

## Future Improvements

### Planned Features

1. **More configuration templates**: Additional configuration templates for different use cases
2. **Configuration versioning**: Support for configuration versioning
3. **Advanced validation**: More sophisticated configuration validation rules
4. **Configuration documentation**: Automatic configuration documentation generation
5. **Configuration comparison**: Compare different configuration versions

### Example Improvements

1. **More examples**: Add examples for new NLM features
2. **Performance examples**: Add examples for performance optimization
3. **Integration examples**: Add examples for system integration
4. **Advanced examples**: Add examples for advanced features
5. **Tutorial examples**: Add tutorial-style examples

## Conclusion

The NLM examples provide a comprehensive starting point for users to get started with the NLM framework. Each example demonstrates specific features and use cases, making it easier for users to understand and use NLM effectively.

By providing working examples, documentation, and testing, the NLM project aims to be accessible to users at all experience levels, from beginners to advanced users.

For more information, visit the NLM GitHub repository and explore the comprehensive documentation available online.