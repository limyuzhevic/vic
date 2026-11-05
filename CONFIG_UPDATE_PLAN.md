# NLM Project Configuration System Update

## Overview
This document outlines the implementation of JSON/YAML configuration support for the NLM (Neural Learning Machine) project. The goal is to modernize the configuration system while maintaining full backward compatibility with the existing key=value format.

## Current Status
- Config.cpp: Has TODO on line 22-23 for JSON/YAML parser implementation
- Current implementation uses simple key=value format parser
- Configuration files are saved with placeholder values
- No modern configuration file format support
- Limited configuration file I/O capabilities

## Solution Overview

### 1. Library Selection
- **Primary**: nlohmann::json (modern C++ JSON library, header-only)
- **Secondary**: yaml-cpp for YAML support (optional dependency)
- **Rationale**: Both libraries are widely used, well-maintained, and compatible with CMake

### 2. Architecture
```
Config System Components:
├── Config.hpp (interface)
├── Config.cpp (implementation)
├── JSON Parser (nlohmann::json)
├── YAML Parser (yaml-cpp - optional)
├── Python Bindings (pybind11)
└── CLI Tool (nlm_cli/config)
```

### 3. Format Detection Strategy
- **File Extension**: .cfg/.ini → key=value, .json → JSON, .yaml/.yml → YAML
- **Content-based**: Detect JSON by checking `{` at start, YAML by checking `:` patterns
- **Fallback**: Always support key=value format for backward compatibility

### 4. New API Methods
- `loadFromJson(const std::string& filepath)` - Load JSON configuration
- `loadFromYaml(const std::string& filepath)` - Load YAML configuration
- `toJson() const` - Convert to JSON string
- `toYaml() const` - Convert to YAML string
- `loadFromFile(const std::string& filepath)` - Auto-detect format
- `saveToFile(const std::string& filepath, ConfigFormat format = ConfigFormat::KEY_VALUE)` - Choose output format

### 5. Implementation Details

#### JSON Support
- Use nlohmann::json library
- Convert between ConfigValue and JSON types
- Support for nested objects (future enhancement)
- Type preservation for all supported types

#### YAML Support
- Use yaml-cpp library
- Equivalent to JSON but with different syntax
- Optional dependency (compile-time option)

#### Backward Compatibility
- Existing key=value parser maintained
- No breaking changes to existing API
- `loadFromFile` auto-detects format
- `saveToFile` uses configurable output format

### 6. Python Bindings Updates
- Expose new JSON/YAML methods
- Ensure backward compatibility with existing Python API
- Add format detection in Python layer

### 7. CLI Updates
- Add `--format` option for save operations
- Auto-detect input format for load operations
- Maintain key=value format for CLI params

## Files to Modify

### Core Configuration Files
1. `/workspace/app/src/core/Config/Config.hpp` - Add new API methods
2. `/workspace/app/src/core/Config/Config.cpp` - Implement JSON/YAML parsing

### Build System Files
3. `/workspace/app/CMakeLists.txt` - Add nlohmann::json and yaml-cpp dependencies
4. `/workspace/app/python/CMakeLists.txt` - Ensure Python bindings work with new features

### Python Configuration
5. `/workspace/app/pyproject.toml` - Update scikit-build configuration
6. `/workspace/app/python/bindings.cpp` - Update Python bindings

### Configuration Files
7. `/workspace/app/configs/default.cfg` - Add JSON/YAML examples

### CLI Tool
8. `/workspace/app/nlm_cli/config/__main__.py` - Update CLI to handle JSON/YAML

### Documentation
9. `/workspace/app/HOW_TO_USE.md` - Update usage documentation
10. `/workspace/app/README.md` - Add configuration section

## Build Configuration

### CMake Options
```cmake
option(NLM_ENABLE_JSON "Enable JSON configuration support" ON)
option(NLM_ENABLE_YAML "Enable YAML configuration support" OFF)
option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings" OFF)
```

### Dependencies
```cmake
if(NLM_ENABLE_JSON)
    find_package(nlohmann_json REQUIRED)
endif()

if(NLM_ENABLE_YAML)
    find_package(yaml-cpp REQUIRED)
endif()
```

## Testing Strategy

### Unit Tests
1. Test backward compatibility with existing key=value format
2. Test JSON parsing and serialization
3. Test YAML parsing and serialization (if enabled)
4. Test format detection
5. Test error handling

### Integration Tests
1. Test Python bindings
2. Test CLI tool with JSON/YAML files
3. Test cross-format compatibility

## Implementation Steps

### Phase 1: Research and Setup (Week 1)
1. Research nlohmann::json and yaml-cpp libraries
2. Create test harness for configuration parsing
3. Set up CMake configuration for optional dependencies
4. Create baseline for performance testing

### Phase 2: Core Implementation (Week 2-3)
1. Update Config.hpp with new API
2. Implement JSON parsing using nlohmann::json
3. Implement backward-compatible key=value parser
4. Add format detection logic
5. Update saveToFile with format options

### Phase 3: YAML Support (Week 4)
1. Implement YAML parsing using yaml-cpp
2. Test YAML round-trip serialization
3. Add error handling for missing dependency

### Phase 4: Python Bindings (Week 5)
1. Update python/bindings.cpp
2. Add Python wrapper methods for new features
3. Test integration with existing Python API

### Phase 5: CLI Updates (Week 6)
1. Update nlm_cli/config/__main__.py
2. Add JSON/YAML support to CLI
3. Maintain backward compatibility with existing usage

### Phase 6: Documentation and Testing (Week 7-8)
1. Update documentation
2. Create comprehensive test suite
3. Run performance benchmarks
4. Final integration testing

## Performance Considerations

### Memory Usage
- JSON parser: Minimal overhead
- YAML parser: Slightly higher memory usage
- Backward compatibility: No additional overhead

### Speed Considerations
- JSON parsing: Fast, especially for small-to-medium files
- YAML parsing: Slightly slower than JSON
- key=value parser: Fastest but limited functionality

### Recommendations
- Use JSON for production configuration (best balance of features and performance)
- Use key=value for simple configuration (backward compatibility)
- Use YAML for human-readable configuration (optional)

## Migration Path

### Existing Users
- **Zero Impact**: Existing configuration files continue to work
- **Gradual Migration**: Users can adopt JSON/YAML at their own pace
- **Backward Compatible API**: No changes needed to existing code

### New Users
- **Default Behavior**: Auto-detection of format
- **Easy Migration**: Simple JSON configuration possible
- **Advanced Features**: Full JSON/YAML support available

## Future Enhancements

### Planned Features
1. Nested configuration objects
2. Configuration validation
3. Configuration templates
4. Configuration inheritance
5. Integration with version control

### Research Items
1. Configuration schema validation
2. Configuration compression support
3. Configuration signing/validation
4. Real-time configuration reloading

## Conclusion

This implementation modernizes the NLM configuration system while maintaining full backward compatibility. The solution provides:

- **Modern configuration formats**: JSON and YAML
- **Backward compatibility**: Existing key=value format continues to work
- **Auto-detection**: Smart format detection based on file content
- **Optional dependencies**: YAML support is optional
- **Clean API**: Well-designed interface for both C++ and Python
- **Comprehensive testing**: Thorough test coverage for all features

The implementation will enable users to choose between simple key=value format (for backward compatibility) and modern JSON/YAML format (for advanced features) based on their needs.
