# NLM Configuration System Enhancement Proposal

## Executive Summary

The current NLM configuration system uses a simple key=value format with limited error handling and no JSON/YAML support. This proposal outlines a comprehensive enhancement to support JSON/YAML formats while maintaining backward compatibility and improving validation/error handling.

## Current System Analysis

### 1.1 Current Implementation Details

**Files Modified:**
- `/workspace/app/src/core/Config/Config.cpp` - Core configuration loading/parsing logic
- `/workspace/app/src/core/Config/Config.hpp` - Configuration data structures and interface

**Current Format (key=value):**
```ini
# Examples from configs/default.cfg
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
connection_probability = 0.1
stdp_ltp_weight = 0.01
```

**Current Capabilities:**
- Load from file (key=value format only)
- Load from command line arguments
- Set/get configuration values with type conversion
- Support for: int, int64_t, double, bool, std::string, vectors
- Multiple configuration sources: Default, File, CommandLine, Runtime
- Basic error handling (file open failures only)

**Current Limitations:**
- No JSON/YAML support (marked as TODO PHASE 2)
- Minimal error handling for malformed lines
- No schema validation
- Type conversion errors not gracefully handled
- Save format uses placeholder values

### 1.2 Configuration Usage Throughout Codebase

**Configuration Keys Currently Used:**
- `random_seed` - Random number generator seed
- `simulation_timestep` - Time step for simulation
- `neuron_count` - Number of neurons in brain
- `region_count` - Number of brain regions
- `connection_probability` - Random connectivity probability
- `stdp_ltp_weight` - STDP long-term potentiation weight
- `stdp_ltd_weight` - STDP long-term depression weight
- `stdp_tau` - STDP time constant
- `synaptogenesis_rate` - Synaptogenesis rate
- `pruning_rate` - Pruning rate
- `replay_interval` - Replay interval
- `consolidation_interval` - Consolidation interval
- `checkpoint_dir` - Checkpoint directory

**Files Using Configuration:**
- `src/main.cpp` - Main configuration loading and default values
- `src/brain/Brain.cpp` - Brain initialization and parameter configuration
- `src/experiments/*.cpp` - Experiment-specific configuration
- `src/benchmark.cpp` - Benchmark configuration
- `python/bindings.cpp` - Python bindings (claims JSON support but currently doesn't deliver)

### 1.3 Stubbed JSON/YAML Support

**Evidence of Stubs:**
1. `Config.hpp` line 59: "// Load from file (JSON format)"
2. `Config.cpp` line 22: "// TODO PHASE 2: Implement proper JSON/YAML parser"
3. `Config.cpp` line 23: "// PLACEHOLDER - Phase 1 uses a simple key=value format"
4. `python/bindings.cpp` lines 147-160: Claims to load/save JSON files but currently doesn't implement

## Proposed New Architecture

### 2.1 High-Level Architecture

```
+---------------------+      +---------------------+      +--------------------+
|   Config Manager    | ---> |   Format Detector   | ---> |  Parser (JSON/YAML)|
+---------------------+      +---------------------+      +--------------------+
         |                             |                            |
         v                             v                            v
+---------------------+      +---------------------+      +--------------------+
|  Backward Compatible | ---> |   File Format        | ---> |  Key=Value Parser  |
|    Extension        |      |   Detection          |      | (for .cfg files)    |
+---------------------+      +---------------------+      +--------------------+
```

### 2.2 Configuration Loading Strategy

**File Format Detection:**
- `.cfg` files: Key=value format (current behavior)
- `.json`, `.jsonc`, `.jsonl` files: JSON format
- `.yaml`, `.yml` files: YAML format
- File content detection as fallback

**Configuration Sources Hierarchy:**
1. Command line arguments (highest priority)
2. Environment variables
3. Configuration files (JSON/YAML/CFG)
4. Default values (lowest priority)

### 2.3 Enhanced Data Model

**Extended ConfigValue Types:**
```cpp
typedef std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::unordered_map<std::string, ConfigValue>  // For JSON objects
> ConfigValue;
```

**Enhanced ConfigEntry:**
```cpp
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string filePath;      // Track which file this came from
    size_t lineNumber;         // For error reporting
    std::string validationError; // Store validation errors
};
```

### 2.4 Validation and Error Handling

**Error Categories:**
1. **Syntax Errors**: Malformed JSON/YAML, invalid key=value format
2. **Type Errors**: Invalid type conversions, schema mismatches
3. **Semantic Errors**: Invalid value ranges, conflicting configurations
4. **File System Errors**: Missing files, permission issues

**Error Handling Strategy:**
- **Non-blocking**: Log warnings but continue (for non-critical errors)
- **Blocking**: Fail fast for critical errors (malformed syntax)
- **Graceful degradation**: Use defaults when possible

**Validation Framework:**
- **Configuration Schema**: Define valid keys and expected types
- **Range Validation**: Check value ranges (e.g., neuron_count > 0)
- **Dependency Validation**: Ensure related configurations are consistent
- **Cross-file Validation**: Validate consistency across multiple config files

### 2.5 Backward Compatibility Migration

**Phase 1: Format Detection**
```python
# Detect file format by extension and content
if filename.endswith('.cfg'):
    format = 'key=value'
elif filename.endswith(('.json', '.jsonc', '.jsonl')):
    format = 'json'
elif filename.endswith(('.yaml', '.yml')):
    format = 'yaml'
else:
    # Try to detect by content
    first_line = read_first_line()
    if '=' in first_line and '#' in first_line:
        format = 'key=value'
    else:
        format = 'json'  # Default to JSON for new files
```

**Phase 2: Gradual Migration**
1. **Existing .cfg files**: Continue to work unchanged
2. **New files**: Default to JSON format
3. **Conversion tools**: Scripts to convert existing .cfg to JSON
4. **Deprecation path**: Announce key=value end-of-life with migration timeline

## Files Requiring Modification

### 3.1 Core Configuration System

**Required Changes:**

1. **`/workspace/app/src/core/Config/Config.hpp`**
   - Add JSON/YAML format support comments
   - Update docstrings for new loading methods
   - Add format detection utilities

2. **`/workspace/app/src/core/Config/Config.cpp`**
   - Implement `loadFromJSON()` method
   - Implement `loadFromYAML()` method
   - Implement format detection logic
   - Add comprehensive error handling
   - Add validation framework
   - Update `loadFromFile()` to detect and dispatch to appropriate parser
   - Update `saveToFile()` to support multiple output formats

3. **`/workspace/app/src/core/Config/Config.cpp`** (Additional methods)
   - `loadFromJSONFile()` - Load JSON configuration
   - `loadFromYAMLFile()` - Load YAML configuration
   - `exportToJSON()` - Export configuration to JSON format
   - `exportToYAML()` - Export configuration to YAML format
   - `validate()` - Validate configuration against schema
   - `merge()` - Merge multiple configuration sources

### 3.2 Files Using Configuration

**Files That May Need Updates:**

1. **`/workspace/app/src/main.cpp`**
   - Update configuration loading logic to handle multiple formats
   - Add format detection for default config file

2. **`/workspace/app/src/brain/Brain.cpp`**
   - Add validation for configuration values
   - Update error handling for missing/invalid configurations

3. **`/workspace/app/python/bindings.cpp`**
   - Update Python documentation to reflect actual capabilities
   - Add proper JSON/YAML loading/saving bindings

4. **`/workspace/app/src/experiments/*.cpp`**
   - Update experiment configuration loading
   - Add validation for experiment-specific parameters

### 3.3 Additional Files

**Supporting Files:**

1. **`/workspace/app/configs/default.cfg`**
   - Convert to JSON format (maintaining same values)
   - Or keep as-is for backward compatibility

2. **`/workspace/app/src/experiments/Phase6Config.hpp`**
   - Update to support JSON/YAML configuration
   - Add validation for experiment-specific settings

3. **Build system files**
   - Update CMakeLists.txt to link against JSON/YAML libraries

## Migration Strategy

### 4.1 Immediate Actions (Phase 1)

**Week 1-2: Core Implementation**
- Implement JSON parser integration (using nlohmann/json or similar)
- Implement YAML parser integration (using YAML-CPP or similar)
- Update format detection logic
- Maintain backward compatibility with existing .cfg files

**Week 3-4: Validation Framework**
- Implement basic configuration schema
- Add range validation for key parameters
- Implement error handling and reporting
- Add logging for configuration loading

### 4.2 Medium-term Actions (Phase 2)

**Week 5-6: Enhanced Features**
- Implement nested object support for JSON/YAML
- Add configuration inheritance/merging
- Implement validation against full schema
- Add configuration export functionality

**Week 7-8: Integration and Testing**
- Test backward compatibility with existing projects
- Add comprehensive test coverage for new functionality
- Update documentation and examples
- Performance testing with large configuration files

### 4.3 Long-term Actions (Phase 3)

**Week 9-10: Advanced Features**
- Implement configuration versioning
- Add schema evolution support
- Implement configuration monitoring and hot-reload
- Add advanced validation rules

**Week 11-12: Production Readiness**
- Complete documentation
- Add migration tools for existing users
- Performance optimization
- Security hardening

## Technology Selection

### 5.1 JSON Library Options

**nlohmann/json (Recommended):**
- Header-only library
- Excellent performance
- Full C++17 support
- Widely used and maintained
- Easy integration

**RapidJSON:**
- Faster parsing
- Memory efficient
- Smaller footprint
- Requires more configuration

**PicoJSON:**
- Minimal dependencies
- Good for embedded systems
- Less feature complete

### 5.2 YAML Library Options

**YAML-CPP:**
- Mature library
- Good performance
- Extensive features
- Multiple parsers available

**libyaml:**
- High performance
- Industry standard
- Multiple language bindings
- Lower-level API

**PyYAML (if using Python bindings):**
- Well maintained
- Python integration
- Mature ecosystem
- Good for configuration management

## Performance Considerations

### 6.1 Memory Usage
- JSON/YAML parsers typically use more memory than simple key=value parsing
- Consider streaming parsers for large configuration files
- Implement lazy loading for optional sections

### 6.2 Parsing Performance
- JSON parsing is generally faster than YAML
- Consider caching parsed configurations
- Implement incremental parsing for large files

### 6.3 Validation Overhead
- Schema validation adds overhead
- Implement selective validation (only for critical parameters)
- Consider validation caching

## Testing Strategy

### 7.1 Test Coverage

**Unit Tests:**
- Format detection tests
- JSON/YAML parser tests
- Type conversion tests
- Error handling tests
- Validation tests

**Integration Tests:**
- Full configuration loading tests
- Backward compatibility tests
- Cross-format migration tests
- Performance tests

**End-to-End Tests:**
- Complete application startup with new configuration
- Real-world configuration file testing
- Configuration validation in production

## Conclusion

The proposed configuration system enhancement will:

1. **Maintain backward compatibility** with existing .cfg files
2. **Add JSON/YAML support** for modern configuration management
3. **Improve validation and error handling** for robust operation
4. **Provide a clear migration path** for existing users
5. **Enable future extensibility** for configuration system enhancements

The implementation should be delivered in phases, with each phase building on the previous one, ensuring that existing functionality is never broken while new capabilities are gradually introduced.
