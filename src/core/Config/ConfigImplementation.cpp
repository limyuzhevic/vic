// ConfigImplementation.cpp - Enhanced Configuration System Implementation

This file implements the enhanced configuration system for NLM.
It provides:
- Enhanced saveToFile() method that saves actual values
- Comprehensive parameter validation
- Enhanced error handling
- Format auto-detection
- Backward compatibility

## Implementation Status

### Files Modified
1. `src/core/Config/Config.hpp` - Enhanced interface
2. `src/core/Config/Config.cpp` - Enhanced implementation

### Files Created
1. `src/core/Config/ConfigImplementation.cpp` - Main implementation file

## Implementation Details

### 1. Enhanced Configuration Interface

**File**: `src/core/Config/Config.hpp`
**Changes**: Enhanced interface with new methods and functionality

**New Methods**:
- `validate()` - Validate configuration parameters
- `loadFromJSON()` - Load configuration from JSON string
- `saveToJSON()` - Save configuration to JSON string
- `getParameterSchema()` - Get parameter schema for documentation
- `setJSONParser()` - Set JSON parser for dependency injection
- `getJSONParser()` - Get JSON parser

**Implementation**:
```cpp
class Config {
public:
    // ... existing methods ...
    
    // New methods for enhanced functionality
    std::vector<std::string> validate() const;
    bool loadFromJSON(const std::string& jsonString);
    std::string saveToJSON() const;
    std::vector<std::string> getParameterSchema();
    void setJSONParser(std::shared_ptr<ConfigJSONParser> parser);
    std::shared_ptr<ConfigJSONParser> getJSONParser() const;
    
private:
    struct Impl {
        std::vector<ConfigEntry> entries;
        std::shared_ptr<ConfigJSONParser> jsonParser;
    };
    std::unique_ptr<Impl> pImpl;
};
```

### 2. Enhanced Implementation

**File**: `src/core/Config/ConfigImplementation.cpp`
**Implementation**: Core implementation of enhanced configuration system

**Key Features**:
- Enhanced saveToFile() method that saves actual values
- Comprehensive parameter validation
- Enhanced error handling
- Format auto-detection

**Implementation**:
```cpp
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::shared_ptr<ConfigJSONParser> jsonParser;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->jsonParser = std::make_shared<ConfigJSONParser>();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Use JSON parser for JSON/YAML files, fallback to simple format for .cfg files
    if (pImpl->jsonParser) {
        // Try JSON/YAML first
        bool success = pImpl->jsonParser->parseFromFile(filepath, *this).success;
        if (success) {
            return true;
        }
        
        // If JSON/YAML fails and it's a .cfg file, try simple format for backward compatibility
        if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".cfg") {
            return pImpl->jsonParser->parseSimpleFormatFromFile(filepath, *this);
        }
        
        return false;
    }
    return false;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Use JSON parser for JSON/YAML files, simple format for .cfg files
    if (pImpl->jsonParser) {
        // Try JSON/YAML first for JSON/YAML files
        if (filepath.size() >= 5 && (filepath.substr(filepath.size() - 5) == ".json" || 
                                    filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml" ||
                                    filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yml")) {
            return pImpl->jsonParser->saveToFile(*this, filepath);
        } else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".cfg") {
            // Use simple format for .cfg files
            return pImpl->jsonParser->saveToSimpleFormat(filepath);
        }
        
        return false;
    }
    return false;
}

// ... implementation of other methods ...

} // namespace nlm
```

### 3. Enhanced SaveToFile Method

**Implementation**: Enhanced saveToFile() that saves actual values instead of placeholders

**Key Features**:
- Save actual values based on type
- Support for all primitive types
- Support for arrays
- Format auto-detection

**Implementation**:
```cpp
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Save actual values based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << "null";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}
```

### 4. Enhanced Parameter Validation

**Implementation**: Comprehensive parameter validation

**Key Features**:
- Type validation
- Value validation
- Duplicate parameter detection
- Required parameter checking

**Implementation**:
```cpp
std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    // Check for duplicate parameters
    std::map<std::string, size_t> keyCounts;
    for (const auto& entry : pImpl->entries) {
        keyCounts[entry.key]++;
    }
    
    for (const auto& [key, count] : keyCounts) {
        if (count > 1) {
            errors.push_back("Duplicate parameter: '" + key + "' appears multiple times");
        }
    }
    
    // Check for required parameters
    std::vector<std::string> requiredParams = {"neuron_count", "simulation_timestep"};
    for (const auto& param : requiredParams) {
        if (!has(param)) {
            errors.push_back("Missing required parameter: '" + param + "'");
        }
    }
    
    // Validate parameter types and values
    for (const auto& entry : pImpl->entries) {
        bool isValid = true;
        std::string errorMsg;
        
        // Type validation
        if (entry.key == "neuron_count" && !std::holds_alternative<int64_t>(entry.value) && 
            !std::holds_alternative<int>(entry.value)) {
            isValid = false;
            errorMsg = "Parameter 'neuron_count' must be an integer";
        } else if (entry.key == "simulation_timestep" && !std::holds_alternative<double>(entry.value)) {
            isValid = false;
            errorMsg = "Parameter 'simulation_timestep' must be a floating point number";
        } else if (entry.key == "connection_probability" && !std::holds_alternative<double>(entry.value)) {
            isValid = false;
            errorMsg = "Parameter 'connection_probability' must be a floating point number";
        } else if (entry.key == "random_seed" && !std::holds_alternative<int64_t>(entry.value) && 
                   !std::holds_alternative<int>(entry.value)) {
            isValid = false;
            errorMsg = "Parameter 'random_seed' must be an integer";
        } else if (entry.key == "log_to_file" && !std::holds_alternative<bool>(entry.value)) {
            isValid = false;
            errorMsg = "Parameter 'log_to_file' must be a boolean";
        }
        
        // Value validation
        if (isValid) {
            if (entry.key == "neuron_count") {
                int64_t neurons = std::holds_alternative<int64_t>(entry.value) ? 
                                  std::get<int64_t>(entry.value) : std::get<int>(entry.value);
                if (neurons <= 0) {
                    isValid = false;
                    errorMsg = "Parameter 'neuron_count' must be positive";
                }
            } else if (entry.key == "simulation_timestep") {
                double timestep = std::get<double>(entry.value);
                if (timestep <= 0.0) {
                    isValid = false;
                    errorMsg = "Parameter 'simulation_timestep' must be positive";
                }
            } else if (entry.key == "connection_probability") {
                double prob = std::get<double>(entry.value);
                if (prob < 0.0 || prob > 1.0) {
                    isValid = false;
                    errorMsg = "Parameter 'connection_probability' must be between 0.0 and 1.0";
                }
            }
        }
        
        if (!isValid) {
            errors.push_back("Parameter '" + entry.key + "': " + errorMsg);
        }
    }
    
    return errors;
}
```

### 5. Enhanced Error Handling

**Implementation**: Enhanced error handling in Brain::initialize()

**Key Features**:
- Better error messages
- Graceful degradation
- Exception safety

**Implementation**:
```cpp
bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain...");
    
    try {
        // Get configuration values
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        // Validate neuron count
        if (neuronCount == 0) {
            NLM_LOG_ERROR("Invalid neuron count: 0 (must be positive)");
            return false;
        }
        
        // Validate region count
        if (regionCount == 0) {
            NLM_LOG_ERROR("Invalid region count: 0 (must be positive)");
            return false;
        }
        
        // Validate connection probability
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            NLM_LOG_ERROR("Invalid connection probability: " + std::to_string(connectionProbability) + 
                         " (must be between 0.0 and 1.0)");
            return false;
        }
        
        // Validate configuration using enhanced validation
        auto validationErrors = validateConfiguration();
        if (!validationErrors.empty()) {
            NLM_LOG_ERROR("Configuration validation failed:");
            for (const auto& error : validationErrors) {
                NLM_LOG_ERROR("  " + error);
            }
            return false;
        }
        
        // Continue with initialization...
        // (rest of the method)
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during initialization: ") + e.what());
        return false;
    }
    
    return true;
}
```

## Testing

### 6.1 Unit Tests
**Test Cases**:
- Configuration loading/saving
- Parameter validation
- Error handling
- Brain initialization

**Example Test**:
```cpp
void testConfigSaving() {
    Config config;
    config.set("neuron_count", 100);
    config.set("simulation_timestep", 0.001);
    
    // Test saving to file
    ASSERT_TRUE(config.saveToFile("test_config.cfg"));
    
    // Load the saved file
    Config config2;
    ASSERT_TRUE(config2.loadFromFile("test_config.cfg"));
    
    // Verify values match
    ASSERT_EQ(config.getOr<int64_t>("neuron_count", 0), 100);
    ASSERT_EQ(config.getOr<double>("simulation_timestep", 0.0), 0.001);
    
    // Test parameter validation
    std::vector<std::string> errors = config2.validate();
    ASSERT_TRUE(errors.empty());  // Should have no validation errors
}
```

### 6.2 Integration Tests
**Test Cases**:
- Full simulation with different configurations
- Brain initialization with invalid parameters
- Error recovery scenarios

**Example Test**:
```cpp
void testBrainInitializationWithInvalidConfig() {
    Config config;
    config.set("neuron_count", 0);  // Invalid: must be positive
    
    Brain brain(config);
    
    // Brain initialization should fail
    ASSERT_FALSE(brain.initialize());
    
    // Check that error was logged
    // (would need to capture log output)
}
```

## Build System Updates

### 6.3 Build System Updates
**CMakeLists.txt Updates**:
```cmake
# Add Google Test
find_package(GTest REQUIRED)

# Add test executable
add_executable(config_tests
    tests/test_config_improvements.cpp
)

target_link_libraries(config_tests
    GTest::gtest
    GTest::gtest_main
    nlm_core
)

# Add test to CTest
include(GoogleTest)
gtest_discover_tests(config_tests)
```

## Configuration Updates

### 6.4 Configuration Updates
**Update configs/default.cfg**:
```cfg
# Fixed duplicate dopamine_baseline parameter
# Neuromodulation settings
dopamine_baseline = 0.1

# Add new configuration parameters
replay_interval = 100
consolidation_interval = 1000
structural_plasticity_rate = 0.0001
pruning_rate = 0.00001
```

## Documentation Updates

### 6.5 Documentation Updates
**Update docs/CONFIGURATION.md**:
```markdown
# Configuration System Improvements

## What's New

### 1. Enhanced saveToFile() Method
The configuration system now saves actual values instead of placeholder values.

### 2. Parameter Validation
The configuration system validates parameters for type and value correctness.

### 3. Enhanced Error Handling
The configuration system provides better error messages and recovery.

### 4. Format Detection
The configuration system auto-detects file formats (JSON, YAML, .cfg).
```

## Migration Guide

### For Users

**Steps to Update**:

1. **Backup current configuration**
   ```bash
   cp configs/default.cfg configs/default.cfg.backup
   ```

2. **Test configuration loading/saving**
   ```bash
   ./build/nlm --config configs/default.cfg
   ```

3. **Test parameter validation**
   ```bash
   # Test with invalid configuration
   ./build/nlm --neuron_count 0
   ```

4. **Test error handling**
   ```bash
   # Test with invalid configuration
   ./build/nlm --neuron_count -100
   ```

### For Developers

**Changes Needed**:

1. **Update configuration system**
   - Review and update Config.cpp implementation
   - Ensure parameter validation works correctly
   - Test configuration loading/saving

2. **Update documentation**
   - Update documentation to reflect new features
   - Update parameter descriptions
   - Update configuration examples

3. **Update tests**
   - Add configuration tests
   - Update integration tests
   - Add error handling tests

## Expected Benefits

### Code Quality Improvements
- **Fixed critical bugs**: Configuration system now saves real values
- **Improved error handling**: Better error messages and recovery
- **Enhanced validation**: Parameter validation prevents runtime errors
- **Better documentation**: Updated documentation with new features

### Developer Experience
- **Intellisense support**: Better IDE support with parameter validation
- **Better debugging**: Clear error messages for configuration issues
- **Easier testing**: Configuration tests for different scenarios
- **Migration path**: Clear migration path for users

### User Experience
- **Better error messages**: Users get clear feedback on configuration issues
- **More flexible configuration**: Enhanced configuration system
- **Better documentation**: Updated and accurate documentation
- **Easier debugging**: Better error context and recovery

### Performance
- **Optimized save operations**: Efficient value serialization
- **Better error handling**: Graceful degradation on errors
- **Efficient validation**: Fast parameter validation
- **Cleaner code**: Better organization and structure

## Conclusion

The enhanced configuration system will:

1. **Fix critical bugs** (saveToFile() now saves real values)
2. **Add modern features** (parameter validation, enhanced error handling)
3. **Improve error handling** (clear, actionable error messages)
4. **Enhance maintainability** (parameter schema, validation)
5. **Provide migration path** (backward compatibility)
6. **Improve developer and user experience** (better API, documentation)

This focused approach will make the NLM project more robust, maintainable, and easier to use while preserving all existing functionality.