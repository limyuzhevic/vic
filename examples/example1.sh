#!/usr/bin/env bash

# Example 1: Basic Configuration Loading
# Load and validate a basic NLM configuration

echo "=== Example 1: Basic Configuration Loading ==="

echo "Creating basic_config.cpp..."
cat > basic_config.cpp << 'EOF'
#include <nlm.h>
#include <iostream>
#include <fstream>

int main() {
    try {
        std::cout << "Loading NLM configuration..." << std::endl;
        
        // Create configuration
        nlm::Config config;
        
        // Load from file
        if (!config.loadFromFile("configs/default.cfg")) {
            std::cerr << "Failed to load configuration from configs/default.cfg" << std::endl;
            return 1;
        }
        
        std::cout << "Configuration loaded successfully!" << std::endl;
        
        // Validate configuration
        if (!config.validate()) {
            std::cerr << "Configuration validation failed!" << std::endl;
            std::cout << "Configuration summary:" << std::endl;
            std::cout << config.summary();
            return 1;
        }
        
        std::cout << "Configuration validated successfully!" << std::endl;
        
        // Access configuration values
        std::cout << "\n=== Configuration Values ===" << std::endl;
        
        // Required parameters
        double timestep = config.getDouble("simulation_timestep");
        int neuronCount = config.getInt("neuron_count");
        int regionCount = config.getInt("region_count");
        double connectionProb = config.getDouble("connection_probability");
        
        std::cout << "Simulation timestep: " << timestep << " s" << std::endl;
        std::cout << "Neuron count: " << neuronCount << std::endl;
        std::cout << "Region count: " << regionCount << std::endl;
        std::cout << "Connection probability: " << connectionProb << std::endl;
        
        // Optional parameters with defaults
        int randomSeed = config.getOr("random_seed", 42);
        std::string envName = config.getOr("environment_name", "Unknown");
        bool logToFile = config.getOr("log_to_file", false);
        
        std::cout << "Random seed: " << randomSeed << std::endl;
        std::cout << "Environment name: " << envName << std::endl;
        std::cout << "Log to file: " << (logToFile ? "Yes" : "No") << std::endl;
        
        // Save configuration
        config.saveToFile("output_basic_config.cfg");
        std::cout << "\nConfiguration saved to output_basic_config.cfg" << std::endl;
        
        // Get detailed configuration report
        std::cout << "\n=== Configuration Report ===" << std::endl;
        std::cout << config.getConfigurationReport();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
EOF

echo "Creating CMakeLists.txt for example 1..."
cat > example1/CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.16)
project(NLM_Example1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find NLM library
find_package(nlm REQUIRED)

# Create executable
add_executable(basic_config basic_config.cpp)
target_link_libraries(basic_config PRIVATE nlm::nlm_core)

# Install executable
install(TARGETS basic_config DESTINATION bin)
EOF

echo "Creating README.md for example 1..."
cat > example1/README.md << 'EOF'
# Example 1: Basic Configuration Loading

This example demonstrates basic configuration loading and validation in the NLM framework.

## Description

This example shows how to:
1. Load configuration from a file (key=value format)
2. Validate the configuration
3. Access configuration values
4. Save configuration to file
5. Generate detailed configuration reports

## Files

- `basic_config.cpp`: Main example code
- `CMakeLists.txt`: Build configuration
- `README.md`: This file

## Building

```bash
mkdir build-example1
cd build-example1
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/path/to/nlm/installation ..
cmake --build . --parallel 4
```

## Running

```bash
./build-example1/basic_config
```

## Expected Output

The program should:
1. Load configuration from `configs/default.cfg`
2. Validate all configuration parameters
3. Print all configuration values
4. Save validated configuration to `output_basic_config.cfg`
5. Print a detailed configuration report

## Configuration Validation

The configuration validation checks for:

1. **Required parameters**: All required configuration keys must be present
2. **Parameter ranges**: Values must be within valid ranges
3. **Type safety**: Parameters must have correct data types
4. **Logical consistency**: Parameters should make sense together

## Error Handling

The program catches exceptions and provides detailed error messages for:

- File I/O errors
- Configuration parsing errors
- Validation failures
- Type conversion errors

## Extensions

1. **Command-line arguments**: Add `configureFromCommandLine()` calls
2. **Multiple configuration sources**: Load from JSON/YAML files
3. **Runtime configuration**: Modify configuration at runtime
4. **Configuration templates**: Create predefined configuration templates
EOF

echo "Example 1 created successfully!"
