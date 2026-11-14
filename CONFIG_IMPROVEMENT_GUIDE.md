# Config System Issues and Improvements

## Current Status

The NLM config system (`src/core/Config/Config.cpp`) has been partially improved with JSON support, but still has several critical issues that need to be addressed:

### Critical Issues:

#### 1. **Placeholder Values in Save** (Lines 187-188)
```cpp
file << "# " << entry.description << "\n";
file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
```
**Issue**: saveToFile() writes "PLACEHOLDER_VALUE" instead of actual values.
**Impact**: Cannot save/load actual configuration state.

#### 2. **Old File Format Still in Use** (Lines 120-155)
```cpp
bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    // ... simple key=value parsing logic ...
}
```
**Issue**: Still using simple key=value format instead of JSON.
**Impact**: Limited configuration complexity support.

#### 3. **Missing JSON Implementation**
**Issue**: JSON helper functions exist (lines 19-110) but not used in main methods.
**Impact**: Cannot leverage JSON benefits.

### Required Immediate Fixes:

#### Fix 1: Update saveToFile() to Write Actual Values
**Current** (line 188):
```cpp
file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
```

**Required**:
```cpp
file << entry.key << " = " << configValueToString(entry.value) << "\n";
```

#### Fix 2: Add JSON Support to loadFromFile()
**Current**: Simple key=value parsing
**Required**: JSON parsing with fallback to simple format

#### Fix 3: Add Configuration Value to String Helper
**Required**: New function `configValueToString()` to convert ConfigValue to string.

### Implementation Strategy:

#### Phase 1: Basic Fixes (Immediate)
1. **Fix saveToFile()**: Write actual values instead of placeholders
2. **Add configValueToString()**: Helper to convert ConfigValue to string representation
3. **Add JSON parsing**: Implement basic JSON file support
4. **Add error handling**: Better error messages and validation

#### Phase 2: Enhanced Features (Next)
1. **Add nested config support**: Support for "section.key" format
2. **Add configuration validation**: Schema validation and type checking
3. **Add command line integration**: Better CLI argument parsing
4. **Add configuration inheritance**: Default + override merging

#### Phase 3: Advanced Features (Future)
1. **Add YAML support**: Alternative configuration format
2. **Add environment variables**: Support for env var overrides
3. **Add hot-reload**: Runtime configuration changes
4. **Add configuration monitoring**: Changes detection and notification

### Files to Modify:

1. **`src/core/Config/Config.cpp`** - Main implementation (primary focus)
2. **`src/core/Config/Config.hpp`** - Header file (minimal changes)
3. **`configs/test_config.json`** - Test configuration file (already created)

### Testing Strategy:

#### Test 1: Basic Config Operations
- Test setting/getting various value types
- Test save/load operations
- Test error handling

#### Test 2: JSON File Support
- Test loading JSON files
- Test saving to JSON format
- Test mixed format support

#### Test 3: Configuration Validation
- Test nested key paths
- Test default value handling
- Test override behavior

#### Test 4: Performance Testing
- Test config operations performance
- Test memory usage with large configs
- Test config operations during simulation

### Expected Improvements:

#### After Phase 1 Fixes:
1. **Proper Save/Load**: Can save and load actual configuration state
2. **JSON Support**: Can read/write JSON configuration files
3. **Better User Experience**: Intrusive configuration management
4. **Error Handling**: Better error messages and validation

#### After Phase 2 Features:
1. **Nested Configs**: Support for complex configuration structures
2. **Configuration Validation**: Schema validation and type checking
3. **CLI Integration**: Better command line argument support
4. **Configuration Inheritance**: Default + override merging

#### After Phase 3 Advanced Features:
1. **YAML Support**: Alternative configuration format
2. **Hot-Reload**: Runtime configuration changes
3. **Configuration Monitoring**: Real-time changes detection
4. **Advanced Features**: Environment variables, etc.

### Technical Implementation Details:

#### ConfigValue to String Conversion:
```cpp
std::string Config::configValueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int64_t> || 
                            std::is_same_v<T, int> ||
                            std::is_same_v<T, double>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                            std::is_same_v<T, std::vector<double>> ||
                            std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ", ";
                oss << configValueToString(arg[i]);
            }
            oss << "]";
        }
    }, value);
    return oss.str();
}
```

#### JSON Load Implementation:
```cpp
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Load from JSON
        loadFromJson(jsonConfig, ConfigSource::File);
        
        pImpl->jsonParsed = true;
        pImpl->jsonSource = filepath;
        
    } catch (const std::exception& e) {
        // Fallback to simple key=value format
        file.clear();
        file.seekg(0, std::ios::beg);
        return loadFromSimpleFormat(filepath);
    }
    
    return true;
}
```

### Risk Assessment:

#### Low Risk:
- JSON parsing using nlohmann/json is well-established
- Backward compatibility maintained with simple format fallback
- Well-tested configuration operations

#### Medium Risk:
- Complex value type conversions
- Performance impact of new operations
- Error handling complexity

#### High Risk:
- Memory usage with large configurations
- Thread safety in multi-threaded environments
- Integration with existing NLM components

### Timeline and Milestones:

#### Week 1: Phase 1 Implementation
- [x] Add configValueToString() helper
- [ ] Fix saveToFile() to write actual values
- [ ] Add JSON parsing to loadFromFile()
- [ ] Add error handling and validation

#### Week 2: Testing and Refinement
- [ ] Write unit tests for new functionality
- [ ] Test performance and memory usage
- [ ] Fix any bugs and edge cases
- [ ] Integrate with existing NLM components

#### Week 3: Documentation and Examples
- [ ] Update documentation with examples
- [ ] Add configuration file examples
- [ ] Update user guides
- [ ] Add API documentation

### Success Criteria:

#### After Implementation:
1. **Functional**: Can save and load configuration files
2. **Performance**: Config operations don't impact simulation performance
3. **Reliability**: Proper error handling and recovery
4. **Usability**: Clear user interface and documentation
5. **Compatibility**: Backward compatibility with existing configurations

### Next Steps:

1. **Implement configValueToString()**: Basic value to string conversion
2. **Update saveToFile()**: Write actual values instead of placeholders
3. **Add JSON parsing**: Implement JSON file support
4. **Add validation**: Validate configuration values
5. **Write tests**: Comprehensive test coverage
6. **Document changes**: Update documentation and examples

This plan provides a clear roadmap for improving the NLM config system while maintaining backward compatibility and ensuring reliability.