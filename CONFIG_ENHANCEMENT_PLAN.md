# NLM Configuration System Enhancement

This document describes the planned improvements to the NLM configuration system, including JSON/YAML support, parameter naming consistency, and enhanced functionality.

## Current Issues

### 1.1 Basic Configuration System Problems

**Problem**: The current configuration system uses a simple key=value format with several limitations:

```cpp
// Current implementation in Config.cpp
bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    // ... simple parsing logic ...
}
```

**Issues**:
- Only supports simple key=value format
- No support for nested parameters (e.g., `brain.neuron_count`)
- saveToFile() uses placeholder values
- Limited data type support

### 1.2 Configuration File Inconsistencies

**Problems**:

**Parameter Naming Mismatches**:

| Source | Parameter | Description |
|--------|-----------|-------------|
| `default.cfg` | `development_synaptogenesis_rate` | Development synaptogenesis rate |
| `Brain.cpp` | `synaptogenesis_rate` | Synaptogenesis rate |
| `default.cfg` | `development_pruning_rate` | Development pruning rate |
| `Brain.cpp` | `pruning_rate` | Pruning rate |
| `default.cfg` | `plasticity_learning_rate` | Plasticity learning rate |
| `Brain.cpp` | `stdp_ltp_weight` | STDP LTP weight |

**Duplicate Configuration**:
```cfg
# Neuromodulation settings
dopamine_baseline = 0.0  # DUPLICATE - removed
dopamine_baseline = 0.1
```

### 1.3 Configuration Usage Patterns

**Inconsistent Patterns**:

```cpp
// Brain.cpp uses:
config->getOr<float>("synaptogenesis_rate", 0.0001f)

// Python API docs mention:
config.set("plasticity.structural.enable", true)

// Actual config file uses:
plasticity.structural.enable = true  # Different format!
```

## Enhancement Plan

### 2.1 JSON/YAML Configuration Support

**Approach**: Implement a simple JSON parser that supports:

- **Top-level objects**: Direct key-value pairs
- **Nested objects**: Support for `brain.neuron_count` format
- **Arrays**: Support for list parameters
- **Type conversion**: String to int/float/bool conversion
- **Backward compatibility**: Support existing key=value format

**Implementation Files**:
- `src/core/Config/Config.hpp` - Updated with JSON support
- `src/core/Config/Config.cpp` - Enhanced parser implementation

**Key Features**:

```cpp
// Enhanced loadFromFile() - supports both JSON and simple format
bool Config::loadFromFile(const std::string& filepath) {
    // Auto-detect format based on file extension
    if (filepath.ends_with(".json") || filepath.ends_with(".yaml") || 
        filepath.ends_with(".yml")) {
        return loadFromJSON(filepath);
    } else {
        return loadFromSimpleFormat(filepath);
    }
}
```

### 2.2 Parameter Naming Consistency

**Standardization Plan**:

| Python API | C++ API | Config File | Description |
|------------|---------|-------------|-------------|
| `brain.neuron_count` | N/A | `neuron_count` | Total neuron count |
| `plasticity.stdp.learning_rate` | N/A | `stdp_ltp_weight` | STDP LTP weight |
| `plasticity.structural.enable` | N/A | `structural_enable` | Structural plasticity enable |

**Mapping Rules**:
1. Replace dots with underscores for config files
2. Use snake_case for all parameters
3. Provide backward compatibility for existing code

### 2.3 Enhanced Configuration Operations

**New Methods**:

```cpp
// JSON serialization
bool Config::saveToJSON(const std::string& filepath) const;

// Enhanced saveToFile - save actual values
bool Config::saveToFile(const std::string& filepath) const {
    // Support both simple and JSON formats based on extension
    if (filepath.ends_with(".json")) {
        return saveToJSON(filepath);
    }
    return saveToSimpleFormat(filepath);
}
```

**Parameter Validation**:

```cpp
// Validate configuration parameters
bool Config::validate() const {
    // Check required parameters
    // Validate parameter ranges
    // Ensure consistency between related parameters
}
```

### 2.4 Configuration Templates

**Create standard configuration templates**:

1. **Minimal Configuration** (`minimal.cfg`):
   ```yaml
   brain:
     neuron_count: 100
     region_count: 1
   simulation:
     timestep: 0.001
   ```

2. **Development Configuration** (`development.cfg`):
   ```yaml
   brain:
     neuron_count: 1000
   plasticity:
     stdp:
       learning_rate: 0.001
       tau: 20.0
   ```

3. **Production Configuration** (`production.cfg`):
   ```yaml
   brain:
     neuron_count: 10000
   performance:
     enable_optimization: true
   ```

## Implementation Details

### 3.1 JSON Parser Implementation

**File**: `src/core/Config/ConfigJSONParser.cpp` (new file)

**Key Functions**:

```cpp
// Parse JSON string
nlohmann::json parseJSON(const std::string& jsonString);

// Convert JSON to Config
bool jsonToConfig(const nlohmann::json& json, Config& config);

// Convert Config to JSON
nlohmann::json configToJSON(const Config& config);
```

**Parser Features**:
- String to type conversion
- Array handling
- Object nesting support
- Error handling with meaningful messages

### 3.2 Backward Compatibility

**Support Both Formats**:

```cpp
// Auto-detect format
Format detectFormat(const std::string& filepath) {
    if (filepath.ends_with(".json") || filepath.ends_with(".yaml")) {
        return Format::JSON;
    } else if (filepath.ends_with(".cfg")) {
        return Format::Simple;
    }
    // Try to auto-detect
    // For now, default to simple format
    return Format::Simple;
}
```

**Migration Path**:
- Existing code continues to work
- Gradual migration to JSON format
- Clear error messages for invalid formats

### 3.3 Error Handling

**Improved Error Messages**:

```cpp
// Before:
// "Failed to load config"  (unhelpful)

// After:
// "Failed to load config from 'config.json': Syntax error at line 5: Expected string"
// "Failed to load config: Missing required parameter 'brain.neuron_count'"
// "Failed to load config: Invalid value for parameter 'neuron_count': 'not_a_number'"
```

### 3.4 Configuration Schema

**Define parameter schema**:

```cpp
struct ConfigParameter {
    std::string name;
    std::string type;  // "int", "float", "bool", "string", "array"
    std::string description;
    bool required;
    std::variant<int, float, bool, std::string> default_value;
    std::string validation_pattern;
};

// Predefined parameters
const std::vector<ConfigParameter> NLM_CONFIG_SCHEMA = {
    {"neuron_count", "int", "Total number of neurons", true, 1000},
    {"region_count", "int", "Number of brain regions", false, 1},
    {"simulation_timestep", "float", "Simulation timestep", false, 0.001},
    // ... more parameters
};
```

## Testing and Validation

### 4.1 Configuration Test Suite

**New test file**: `tests/test_config.cpp`

**Test Cases**:
- Load JSON configuration
- Load simple format configuration
- Save and reload configuration
- Parameter validation
- Error handling
- Type conversion

### 4.2 Integration Tests

**Test configuration system integration**:

```cpp
void testConfigIntegration() {
    Config config;
    config.loadFromFile("configs/default.cfg");
    
    // Verify configuration
    ASSERT_EQ(config.getOr<int>("neuron_count", 0), 1000);
    ASSERT_EQ(config.getOr<float>("simulation_timestep", 0.0f), 0.001f);
    
    // Test JSON export/import
    config.saveToFile("test_config.json");
    Config config2;
    config2.loadFromFile("test_config.json");
    // Verify they match
}
```

## Performance Considerations

### 5.1 Memory Efficiency

**Optimization strategies**:
- Use string views for parsing
- Implement incremental parsing for large files
- Cache parsed JSON for repeated access

### 5.2 Speed Considerations

**JSON parsing optimizations**:
- Use efficient JSON library (nlohmann/json)
- Pre-compile JSON schemas
- Implement lazy loading for large configurations

## Migration Guide

### 6.1 For Users

**Steps to upgrade**:

1. **Backup current config**:
   ```bash
   cp configs/default.cfg configs/default.cfg.backup
   ```

2. **Test current functionality**:
   ```bash
   ./build/nlm --config configs/default.cfg
   ```

3. **Create new JSON config**:
   ```bash
   # Create brain.json config
   ```

4. **Test new config**:
   ```bash
   ./build/nlm --config brain.json
   ```

5. **Gradually migrate**:
   - Replace config files one by one
   - Test each migration
   - Document any changes

### 6.2 For Developers

**Changes needed**:

1. **Update configuration loading**:
   - Check if JSON/YAML configs are used
   - Ensure backward compatibility

2. **Update documentation**:
   - Document new configuration format
   - Update parameter names to match new schema

3. **Update tests**:
   - Add JSON/YAML configuration tests
   - Update existing configuration tests

## Expected Benefits

### 7.1 Code Quality Improvements

- **Reduced bugs**: Better error handling and validation
- **Improved maintainability**: Clear parameter schema
- **Better documentation**: Comprehensive parameter descriptions
- **Enhanced usability**: JSON format is more human-readable

### 7.2 Developer Experience

- **Intellisense support**: IDE can provide parameter hints
- **Better debugging**: Clear error messages
- **Easy configuration**: JSON format is widely supported
- **Migration path**: Gradual transition possible

### 7.3 Performance

- **Faster loading**: Optimized JSON parsing
- **Better memory usage**: Efficient data structures
- **Reduced configuration errors**: Validation prevents runtime issues

## Conclusion

The NLM configuration system enhancement will:

1. **Fix critical bugs** (duplicate parameters, inconsistent naming)
2. **Add modern format support** (JSON/YAML)
3. **Improve error handling** (clear, actionable error messages)
4. **Enhance maintainability** (parameter schema, validation)
5. **Provide migration path** (backward compatibility)
6. **Improve developer experience** (better API, documentation)

This enhancement will make the NLM project more robust, maintainable, and easier to use while preserving all existing functionality.

## Implementation Timeline

| Task | Duration | Dependencies |
|------|----------|--------------|
| Fix duplicate parameters | 1 day | Config file access |
| Implement JSON parser | 3-4 days | JSON library available |
| Add YAML support | 2-3 days | YAML library available |
| Parameter standardization | 2 days | Schema definition |
| Save actual values | 1 day | Config access |
| Add validation | 2 days | Schema definition |
| Create templates | 1 day | Completed implementation |
| Write tests | 3-4 days | Complete implementation |

**Total**: 17-21 days

## Risk Mitigation

### 1. JSON/YAML Library Availability
**Risk**: JSON library may not be available
**Mitigation**: Implement simple parser if needed, provide fallback to simple format

### 2. Configuration Breakage
**Risk**: Users' custom configurations may break
**Mitigation**: Maintain backward compatibility, provide migration guide, extensive testing

### 3. Performance Impact
**Risk**: JSON parsing may be slower than simple format
**Mitigation**: Optimize parser, provide benchmarks, consider caching

## Success Criteria

1. **Functional**: JSON/YAML files load correctly and produce valid configurations
2. **Performance**: Configuration loading is within acceptable limits
3. **Compatibility**: All existing configurations continue to work
4. **Testing**: Comprehensive test coverage for configuration system
5. **Documentation**: Complete documentation for new format
6. **Migration**: Clear migration path for users

## Dependencies

### Required Libraries
- **nlohmann/json**: For JSON parsing (modern, header-only)
- **YAML-cpp**: For YAML parsing (if available)

### Implementation Strategy
If external JSON/YAML libraries are not available, implement a simple but robust parser that:
1. Supports basic JSON syntax
2. Handles common data types
3. Provides good error messages
4. Can be easily extended later