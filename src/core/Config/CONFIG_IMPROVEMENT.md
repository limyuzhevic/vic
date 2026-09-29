Simple JSON Parser Implementation for NLM Configuration System

This file provides a minimal but functional JSON parser for NLM configuration.
It implements the basic functionality needed for the NLM project without
requiring external dependencies.

## Implementation Strategy

The implementation provides:
1. Basic JSON parsing for configuration files
2. Backward compatibility with simple key=value format
3. Parameter validation and error handling
4. File format auto-detection
5. Support for common JSON types

## Key Features

### 1. JSON Parsing
- Parse JSON objects with nested properties
- Support for primitive types (string, number, boolean)
- Array support
- Automatic type conversion

### 2. Simple Format Support
- Continue supporting existing key=value format
- Graceful handling of malformed files
- Auto-detection of file format

### 3. Error Handling
- Detailed error messages with context
- Validation of configuration parameters
- Graceful degradation

### 4. Configuration Management
- Load configuration from JSON/YAML/Simple format
- Save configuration to JSON/YAML/Simple format
- Parameter validation
- Configuration merging

## Implementation Details

### JSON Parser Implementation
The JSON parser is implemented as a standalone class that can:
- Parse JSON strings
- Convert to ConfigValue variants
- Handle nested objects
- Validate types

### Simple Format Parser
The simple format parser continues to support:
- key=value pairs
- Comments with # or /
- Basic type conversion
- No quotes for values

### Format Detection
The parser auto-detects file format based on:
- File extension (.json, .yaml, .yml, .cfg)
- File content (JSON starts with { or [, simple format uses =)

### Error Recovery
The parser includes:
- Graceful handling of malformed input
- Recovery from parse errors
- Meaningful error messages

## Files in This Implementation

### Modified Files
1. `src/core/Config/Config.hpp` - Updated with JSON parser interface
2. `src/core/Config/Config.cpp` - Enhanced configuration implementation

### New Files
1. `src/core/Config/SimpleConfigParser.cpp` - Simple format parser implementation
2. `src/core/Config/JSONConfigParser.cpp` - JSON parser implementation
3. `configs/minimal.json` - Minimal JSON configuration template
4. `configs/development.json` - Development JSON configuration template
5. `configs/production.json` - Production JSON configuration template

## Usage Examples

### Loading JSON Configuration
```cpp
Config config;
if (config.loadFromFile("brain.json")) {
    std::cout << "Configuration loaded successfully" << std::endl;
    std::cout << config.summary() << std::endl;
} else {
    std::cout << "Failed to load configuration" << std::endl;
}
```

### Loading Simple Format Configuration
```cpp
Config config;
if (config.loadFromFile("brain.cfg")) {
    std::cout << "Configuration loaded successfully" << std::endl;
    std::cout << config.summary() << std::endl;
} else {
    std::cout << "Failed to load configuration" << std::endl;
}
```

### Configuration Validation
```cpp
Config config;
config.loadFromFile("brain.json");

std::vector<std::string> errors = config.validate();
if (!errors.empty()) {
    std::cout << "Configuration validation errors:" << std::endl;
    for (const auto& error : errors) {
        std::cout << "  - " << error << std::endl;
    }
}
```

## Implementation Details

### JSON Parser
The JSON parser uses a simple recursive descent approach:
- Tokenize the input
- Parse objects, arrays, and values
- Convert to ConfigValue variants
- Handle nested structures

### Simple Format Parser
The simple format parser uses:
- Line-by-line parsing
- Key=value pattern matching
- Type inference based on value content
- Comment and empty line handling

### Error Handling
The error handling system provides:
- Specific error types for different failure modes
- Context about what went wrong
- Suggestions for fixing errors

## Files Created/Modified

### Modified Files
1. `src/core/Config/Config.hpp` - Added JSON/Simple format support
2. `src/core/Config/Config.cpp` - Enhanced implementation with parser integration

### New Files
1. `src/core/Config/SimpleConfigParser.cpp` - Simple format parser implementation
2. `src/core/Config/JSONConfigParser.cpp` - JSON parser implementation
3. `configs/minimal.json` - Minimal JSON configuration template
4. `configs/development.json` - Development JSON configuration template
5. `configs/production.json` - Production JSON configuration template

## Testing

The implementation includes:
- Unit tests for configuration loading/saving
- Integration tests with the main NLM application
- Error handling tests
- Performance tests for large configurations

## Migration Guide

### For Users

1. **Backup existing configurations**
   ```bash
   cp configs/default.cfg configs/default.cfg.backup
   ```

2. **Test existing functionality**
   ```bash
   ./build/nlm --config configs/default.cfg
   ```

3. **Create new JSON configuration**
   ```bash
   # Create brain.json config
   ```

4. **Test new configuration**
   ```bash
   ./build/nlm --config brain.json
   ```

5. **Gradually migrate**
   - Replace config files one by one
   - Test each migration
   - Document any changes

### For Developers

1. **Update configuration loading**
   - Check if JSON configs are used
   - Ensure backward compatibility

2. **Update documentation**
   - Document new JSON format
   - Update parameter names to match new schema

3. **Update tests**
   - Add JSON configuration tests
   - Update existing configuration tests

## Performance Considerations

### Memory Efficiency
- Use streaming where possible
- Implement incremental parsing
- Cache parsed configurations

### Speed Considerations
- Use efficient parsing algorithms
- Pre-compile configuration schemas
- Implement lazy loading for large configurations

## Risk Mitigation

### JSON Library Availability
**Risk**: Complex JSON parsing may have bugs
**Mitigation**: Simple implementation with thorough testing, fallback to simple format

### Configuration Breakage
**Risk**: Users' custom configurations may break
**Mitigation**: Maintain backward compatibility, provide migration guide, extensive testing

### Performance Impact
**Risk**: JSON parsing may be slower than simple format
**Mitigation**: Optimize parser, provide benchmarks, consider caching

## Expected Benefits

### Code Quality Improvements
- **Reduced bugs**: Better error handling and validation
- **Improved maintainability**: Clear parameter schema
- **Better documentation**: Comprehensive parameter descriptions
- **Enhanced usability**: JSON format is more human-readable

### Developer Experience
- **Intellisense support**: IDE can provide parameter hints
- **Better debugging**: Clear error messages
- **Easy configuration**: JSON format is widely supported
- **Migration path**: Gradual transition possible

### User Experience
- **Better error messages**: Users get clear feedback on configuration issues
- **More flexible configuration**: Support for multiple formats
- **Better documentation**: Easy to understand parameter descriptions
- **Easier debugging**: Better error context

## Implementation Timeline

| Phase | Duration | Key Deliverables |
|-------|----------|------------------|
| Phase 1: JSON Parser | 1 week | JSON parsing implementation |
| Phase 2: Simple Format | 1 week | Enhanced simple format parser |
| Phase 3: Integration | 1 week | Integration with main Config class |
| Phase 4: Validation | 1 week | Parameter validation and testing |
| Phase 5: Templates | 1 week | Configuration templates |
| Phase 6: Documentation | 1 week | Configuration documentation |

**Total**: 6 weeks

## Success Criteria

1. **Functional**: JSON configuration files load and save correctly
2. **Compatibility**: All existing configurations continue to work
3. **Performance**: Configuration loading within acceptable limits
4. **Testing**: Comprehensive test coverage
5. **Documentation**: Complete documentation
6. **User Experience**: Clear error messages and migration path

## Dependencies

### Required Libraries
- **Standard C++17**: For implementation

### Implementation Strategy
The implementation uses only standard C++ libraries:
- No external dependencies
- Simple but robust implementation
- Easy to maintain and extend

## Conclusion

The enhanced configuration system will:

1. **Fix critical bugs** (duplicate parameters, inconsistent naming)
2. **Add modern format support** (JSON)
3. **Improve error handling** (clear, actionable error messages)
4. **Enhance maintainability** (parameter schema, validation)
5. **Provide migration path** (backward compatibility)
6. **Improve developer and user experience** (better API, documentation)

This enhancement will make the NLM project more robust, maintainable, and easier to use while preserving all existing functionality.