# Config System JSON Enhancement

## Summary

Enhanced the NLM Config system to support both JSON and simple key=value file formats with robust validation, error handling, and serialization capabilities.

## Changes Made

### 1. Core Configuration Files

#### `src/core/Config/Config.hpp`
- Added `nlohmann::json` forward declaration
- Added error exception classes:
  - `ConfigParseError`: For parsing errors
  - `ConfigFileError`: For file I/O errors
  - `ConfigValidationError`: For validation errors
- Enhanced `Config` class with JSON conversion helpers:
  - `jsonToConfigValue(const nlohmann::json& j)`
  - `configValueToJson(const ConfigValue& value) const`
  - `validateKey(const std::string& key)`
  - `validateValue(const ConfigValue& value)`
  - `saveToJSON(const std::string& filepath) const`
  - `saveToSimpleFormat(const std::string& filepath) const`
- Updated constructor/destructor and move semantics
- Enhanced `loadFromFile()` to support both JSON and simple formats
- Enhanced `saveToFile()` to determine format by file extension

#### `src/core/Config/Config.cpp`
- Added `#include <nlohmann/json.hpp>` for JSON parsing
- Added `#include <cmath>` and `#include <iomanip>` for numerical operations
- Implemented robust JSON parsing with error handling
- Added validation for configuration keys and values
- Implemented JSON serialization with pretty printing
- Maintained backward compatibility with simple key=value format
- Added `std::isnan()` checks for floating-point values

### 2. Build System

#### `CMakeLists.txt`
- Added `find_package(nlohmann_json 3.11.2 REQUIRED)` to locate nlohmann/json
- Added comment explaining nlohmann_json is header-only

### 3. Test Framework

#### `tests/test_config.cpp`
- Added comprehensive tests for new functionality:
  - `testConfigSimpleFormat()`: Tests loading/saving in simple key=value format
  - `testConfigJSONFormat()`: Tests loading/saving in JSON format
  - `testConfigKeyValidation()`: Tests key validation
  - `testConfigValueValidation()`: Tests value validation
- All tests pass for existing functionality

### 4. Python Bindings

#### `python/bindings.cpp`
- Updated `Config` class documentation to reflect support for both JSON and simple formats
- Updated method documentation to clarify format support

### 5. Example Files

#### `test_config.json`
- Example JSON configuration file demonstrating various data types
- Shows support for: integers, doubles, strings, booleans, and arrays

## Features Implemented

### 1. JSON Format Support
- Automatic detection of JSON files (extension-based)
- Robust JSON parsing with error handling
- Support for all `ConfigValue` types in JSON:
  - Numbers (integers and doubles)
  - Booleans
  - Strings
  - Arrays (of ints, doubles, strings, booleans)

### 2. Simple Format Support
- Maintains backward compatibility with existing key=value format
- Supports comments (lines starting with # or /)
- Supports quoted string values
- Same validation and error handling as JSON format

### 3. Validation
- **Key Validation**:
  - Keys cannot be empty
  - Keys cannot contain dots ('.')
  - Keys cannot contain spaces
- **Value Validation**:
  - Checks for NaN in floating-point values
  - Checks for NaN in array elements
  - Type-specific validation

### 4. Error Handling
- `ConfigParseError`: JSON parsing errors, invalid formats
- `ConfigFileError`: File I/O errors
- `ConfigValidationError`: Validation failures
- Graceful fallback from JSON to simple format on parse error

### 5. Serialization
- **JSON Output**: Pretty-printed with 2-space indentation
- **Simple Format Output**: Maintains original format style
- **Format Detection**: Automatically chooses format based on file extension

### 6. Backward Compatibility
- Existing code continues to work without changes
- Simple configuration files are still supported
- All existing API functions work as before

## Configuration File Examples

### Simple Format (`.cfg` files)
```
# Neural network configuration
neuron_count = 1000
connection_probability = 0.1
plasticity_learning_rate = 0.01
weight_matrix = [1.0, 2.0, 3.0]
```

### JSON Format (`.json` files)
```json
{
    "neuron_count": 1000,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01,
    "weight_matrix": [1.0, 2.0, 3.0],
    "description_neuron_count": "Number of neurons",
    "description_connection_probability": "Connection probability"
}
```

## Testing

Run all tests with:
```bash
./nlm_test
```

New tests added:
- `testConfigSimpleFormat()`: Tests simple format I/O
- `testConfigJSONFormat()`: Tests JSON format I/O
- `testConfigKeyValidation()`: Tests key validation
- `testConfigValueValidation()`: Tests value validation

## Benefits

1. **Flexibility**: Support for both human-readable simple format and machine-readable JSON
2. **Robustness**: Comprehensive error handling and validation
3. **Backward Compatibility**: Existing configurations continue to work
4. **Extensibility**: Easy to add new configuration types and validation rules
5. **Modern Features**: Leverages modern C++20 features and the popular nlohmann/json library

## Notes

1. The nlohmann/json library is now a required dependency
2. File format is determined by file extension (`.json` vs `.cfg`/no extension)
3. JSON files support pretty-printing for human readability
4. Both formats support the same set of configuration value types
