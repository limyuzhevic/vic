NLM Configuration System Improvements

This document outlines improvements to the NLM configuration system.

## Current Issues

1. **JSON/YAML Support**: Placeholder implementation in loadFromFile()
2. **File I/O Bug**: saveToFile() writes "PLACEHOLDER_VALUE" instead of actual values
3. **Lambda Capture Issue**: Incorrect capture in summary() lambda
4. **Limited Error Handling**: Basic file operations with minimal error reporting
5. **Limited Type Support**: Only basic types, no nested structures or arrays

## Proposed Improvements

### 1. Enhanced File Format Support
- Add JSON (.json) support with full schema validation
- Add YAML (.yaml, .yml) support
- Maintain backward compatibility with key=value format
- Add configuration file validation

### 2. Fixed saveToFile Implementation
- Save actual values instead of placeholders
- Support for all ConfigValue types
- Proper formatting for different data types
- Optional pretty-printing for human-readable output

### 3. Improved Error Handling
- Better error messages with context
- Graceful degradation (continue with defaults if file parsing fails)
- Validation warnings for unknown parameters
- File permission error handling

### 4. Enhanced Type Support
- Add support for vectors (arrays)
- Add support for nested configuration objects
- Add built-in parameter validation
- Add parameter categories and descriptions

### 5. Advanced Features
- Configuration inheritance
- Environment variable substitution
- Configuration validation against schema
- Default value system with source tracking

## Implementation Plan

### Phase 1: Core Fixes
- Fix lambda capture issue in summary()
- Fix saveToFile() implementation
- Add basic JSON/YAML parsing stubs

### Phase 2: Format Support
- Implement JSON parsing using existing libraries
- Implement YAML parsing
- Add format detection based on file extension

### Phase 3: Validation and Enhancement
- Add parameter validation system
- Add configuration schema
- Add environment variable support

## Files to Modify

1. `src/core/Config/Config.hpp` - Enhanced configuration system
2. `src/core/Config/Config.cpp` - Improved implementation
3. `configs/default.cfg` - Updated with new format options

## Testing

1. Existing functionality preservation
2. New format support testing
3. Error handling validation
4. Integration with existing Brain and other systems

## Benefits

1. **Better Developer Experience**: Easier configuration setup and validation
2. **Improved Portability**: JSON/YAML is more portable than key=value
3. **Enhanced Debugging**: Better error messages and source tracking
4. **Future-Proof**: Extensible architecture for new configuration features
5. **Production Ready**: Robust error handling and validation
