# NLM Configuration System Analysis

## Summary of Findings

### 1. Configuration Parsing Format
- **Current Implementation**: Simple key=value format (Phase 1)
- **Expected Format**: JSON/YAML format (Phase 2, as indicated by TODO comments)
- **Evidence**:
  - Config.cpp line 22: "TODO PHASE 2: Implement proper JSON/YAML parser"
  - Config.cpp line 23: "PLACEHOLDER - Phase 1 uses a simple key=value format"
  - Config.hpp line 59: Comment says "Load from file (JSON format)"
  - Python bindings docstrings say "Load configuration from a JSON file" and "Save configuration to a JSON file"
  - ConfigValue variant supports complex types (vectors, strings, etc.) which are typical JSON/YAML types

### 2. Current Configuration Implementation

**Config.hpp** defines:
- **ConfigValue**: std::variant supporting 8 types:
  - Basic types: int, int64_t, double, bool, std::string
  - Collection types: std::vector<int>, std::vector<double>, std::vector<std::string>
- **ConfigSource**: Enum with 4 sources: Default, File, CommandLine, Runtime
- **ConfigEntry**: Struct with key, value, source, and description
- **Config class**: Singleton-like class with key-value storage and retrieval methods

**Config.cpp** implements:
- Simple key=value parser for file loading (Phase 1)
- Command-line argument parsing
- Basic get/set/remove operations
- Placeholder saveToFile method (saves "PLACEHOLDER_VALUE")

### 3. Configuration Usage Throughout Brain Initialization

**Critical configuration keys used in Brain.cpp**:
- **STDP Parameters**:
  - `stdp_ltp_weight` (float): Long-term potentiation weight
  - `stdp_ltd_weight` (float): Long-term depression weight
  - `stdp_tau` (float): Time constant for STDP
  - `synaptogenesis_rate` (float): Rate of synapse formation
  - `pruning_rate` (float): Rate of synapse elimination

- **Simulation Parameters**:
  - `simulation_timestep` (double): Time step for simulation
  - `replay_interval` (size_t): Memory replay interval
  - `consolidation_interval` (size_t): Memory consolidation interval

- **Brain Structure**:
  - `neuron_count` (size_t): Number of neurons
  - `region_count` (size_t): Number of brain regions
  - `connection_probability` (float): Probability of connections

- **Other Parameters**:
  - `random_seed` (int64_t): Random number generator seed
  - `checkpoint_dir` (std::string): Directory for saving checkpoints

### 4. Example Configuration (configs/default.cfg)

The current configs/default.cfg contains:
```
# Basic configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.0
dopamine_baseline = 0.1  # DUPLICATE - should be fixed

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

### 5. JSON Parser Options

**Recommended**: **nlohmann/json** (single-header, modern C++, no dependencies)

**Alternatives**:
- **yaml-cpp**: Native YAML support, but requires compilation
- **JSON for Modern C++**: Another header-only option

The nlohmann/json library is the most suitable because:
- Header-only: Easy to integrate
- Modern C++ interface: Intuitive and type-safe
- Comprehensive: Supports all primitive types and complex structures
- Well-integrated with STL: Compatible with existing ConfigValue variant

### 6. Issues with Current Implementation

**Critical Problems**:
1. **Incomplete JSON Support**: Placeholder parser doesn't support JSON format
2. **Broken saveToFile**: Saves "PLACEHOLDER_VALUE" instead of actual values
3. **Duplicate Configuration**: configs/default.cfg has duplicate `dopamine_baseline` entries
4. **Limited Error Handling**: Poor error messages and recovery
5. **Missing Validation**: No validation of configuration values
6. **Incomplete Type Support**: Missing vector support in implementation

**Specific Code Issues**:
- Config.cpp line 89: `file << entry.key << " = " << "PLACEHOLDER_VALUE\n";`
- Config.cpp: No JSON parsing implementation despite TODO comment
- ConfigValue variant supports vectors but implementation may not handle them correctly

## Recommendations

### 1. Implement Proper JSON Parsing

**Files to modify**:
- `src/core/Config/Config.cpp` - Replace placeholder parser with nlohmann/json integration
- `src/core/Config/Config.hpp` - Add JSON parsing helper methods

**Implementation details**:
```cpp
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// JSON loading
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    try {
        json j;
        file >> j;
        
        for (auto& [key, value] : j.items()) {
            // Convert JSON value to ConfigValue
            set(key, convertJsonToConfigValue(value));
        }
        return true;
    } catch (const json::parse_error& e) {
        // Log error and return false
        return false;
    }
}
```

### 2. Fix saveToFile Method

**Replace placeholder implementation** with actual value serialization:
```cpp
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    
    json j;
    
    for (const auto& entry : pImpl->entries) {
        // Add description as comment
        file << "# " << entry.description << "\n";
        
        // Add key-value pair to JSON
        j[entry.key] = convertConfigValueToJson(entry.value);
    }
    
    // Write JSON to file
    file << j.dump(4); // 4-space indentation
    return true;
}
```

### 3. Add Support for Complex Data Types

**Enhance ConfigValue variant** (already exists but needs implementation):
- Ensure std::vector types are properly serialized
- Add support for nested structures (optional for Phase 2)
- Implement type checking and conversion methods

**JSON conversion methods**:
```cpp
ConfigValue Config::convertJsonToConfigValue(const json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        // Handle vector types based on array content
        return convertJsonArrayToVector(j);
    }
    // Default to string
    return j.get<std::string>();
}
```

### 4. Improve Error Handling

**Add comprehensive error handling**:
- JSON parse errors with descriptive messages
- File I/O error handling
- Type conversion error handling
- Graceful degradation when encountering unsupported types

**Error handling patterns**:
```cpp
try {
    json j = json::parse(file);
} catch (const json::parse_error& e) {
    NLM_LOG_ERROR("JSON parse error in " + filepath + ": " + std::string(e.what()));
    return false;
} catch (const std::exception& e) {
    NLM_LOG_ERROR("Error loading config from " + filepath + ": " + std::string(e.what()));
    return false;
}
```

### 5. Add Validation

**Add configuration validation**:
- Range validation for numeric parameters
- Type validation for configuration values
- Required parameter validation
- Custom validation rules for specific parameters

**Validation examples**:
```cpp
bool Config::validate() const {
    bool valid = true;
    
    // Validate required parameters
    if (!has("random_seed")) {
        NLM_LOG_ERROR("Missing required parameter: random_seed");
        valid = false;
    }
    
    // Validate ranges
    if (has("neuron_count")) {
        int64_t neuronCount = getOr<int64_t>("neuron_count", 0);
        if (neuronCount < 1 || neuronCount > 1000000) {
            NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount));
            valid = false;
        }
    }
    
    // Validate timestep
    if (has("simulation_timestep")) {
        double timestep = getOr<double>("simulation_timestep", 0.0);
        if (timestep <= 0.0 || timestep > 1.0) {
            NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(timestep));
            valid = false;
        }
    }
    
    return valid;
}
```

## Implementation Priority

**Phase 2 Implementation**:
1. **Immediate** (Week 1):
   - Add nlohmann/json include
   - Replace placeholder loadFromFile with JSON parsing
   - Fix saveToFile to save actual values

2. **Short-term** (Week 2):
   - Implement comprehensive error handling
   - Add configuration validation
   - Fix duplicate entries in configs/default.cfg

3. **Medium-term** (Week 3):
   - Enhance type conversion support
   - Add vector type handling
   - Add documentation and examples

**Backward Compatibility**: Ensure the new JSON parser can still parse the existing key=value format for config files that haven't been updated.

## Files to Modify

1. **Primary**
   - `src/core/Config/Config.cpp` - Main implementation
   - `src/core/Config/Config.hpp` - Interface

2. **Secondary**
   - `configs/default.cfg` - Fix duplicate entries, add examples
   - `src/main.cpp` - Update configuration loading
   - `src/brain/Brain.cpp` - Update error handling

3. **Test**
   - `tests/test_config.cpp` - Add JSON parsing tests

## Dependencies

**Required**:
- nlohmann/json (header-only, no build dependencies)

**Optional**:
- yaml-cpp (for YAML support)
- Python bindings (already exist)

The nlohmann/json library can be added as a single-header dependency without modifying CMakeLists.txt (if not already present).
