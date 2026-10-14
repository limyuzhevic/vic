# NLM Configuration and CLI Improvement Plan

## Overview
This plan addresses the current limitations of the NLM project's command line interface and configuration system, implementing comprehensive improvements for better user experience and enhanced functionality.

## Current State Analysis

### CLI Issues
- Basic command line interface with limited options (--config only)
- No help system or command discovery
- Limited validation
- Poor error messages
- No completion hints

### Configuration Issues
- Simple key=value format (no JSON/YAML support)
- Limited advanced configuration options
- No configuration validation
- No configuration templates
- Basic error handling

## Improvement Plan

### 1. Enhanced CLI with Help, Completion, and Validation

#### Implementation Details:

**Help System**
- Add `--help` flag for general help
- Add `--version` flag for version information
- Add `--commands` flag for command discovery
- Add `--validate` flag for configuration validation
- Add `--templates` flag for available templates

**Completion Hints**
- Tab completion for configuration keys
- Auto-completion for known command-line arguments
- Context-aware help (show relevant options based on context)

**Validation**
- Type validation (int, double, bool, string)
- Range validation (min/max values)
- Format validation (email, paths, URLs)
- Dependency validation (validate config relationships)

#### Code Changes in `src/main.cpp`:

```cpp
// Enhanced argument parsing with help and validation
int main(int argc, char** argv) {
    printBanner();
    
    // Parse arguments with enhanced help system
    auto args = parseArguments(argc, argv);
    
    if (args.helpRequested) {
        printHelpMenu(args.subcommand);
        return 0;
    }
    
    if (args.versionRequested) {
        printVersionInfo();
        return 0;
    }
    
    // Validate configuration
    if (args.validateRequested) {
        if (!validateConfiguration(args)) {
            return 1;
        }
    }
    
    // Load configuration with validation
    auto config = loadEnhancedConfiguration(args);
    
    // ... rest of main logic
}
```

### 2. JSON Configuration File Support

#### Implementation Details:

**JSON Parser Integration**
- Add JSON parsing using nlohmann/json library (or similar)
- Support nested configuration structures
- Support different data types (arrays, objects)
- Maintain backward compatibility with existing key=value format

**Enhanced Configuration Format**
```json
{
    "nlm": {
        "version": "2.0",
        "brain": {
            "neuron_count": 1000,
            "connection_probability": 0.1,
            "regions": [
                {"type": "cortex", "size": 500},
                {"type": "hippocampus", "size": 500}
            ]
        },
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.01,
                "time_constant": 20.0
            },
            "hebbian": {
                "enable": true,
                "learning_rate": 0.001
            }
        },
        "neuromodulation": {
            "dopamine": {"baseline": 0.1, "scale": 1.0},
            "curiosity": {"enable": true, "threshold": 0.5}
        }
    }
}
```

#### Code Changes in `src/core/Config/Config.hpp`:

```cpp
// Enhanced configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<std::vector<double>>,  // For nested arrays
    std::unordered_map<std::string, ConfigValue>  // For nested objects
>;

// Enhanced ConfigEntry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;              // "int", "double", "string", "bool", "array", "object"
    std::string validation;        // Regex pattern, range, etc.
    std::string category;          // "brain", "plasticity", "neuromodulation", etc.
    double minValue;               // For numeric validation
    double maxValue;               // For numeric validation
    bool required;                 // Whether this setting is required
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   type(), validation(), category(), minValue(), maxValue(), required(false) {}
};
```

#### Code Changes in `src/core/Config/Config.cpp`:

```cpp
// Enhanced file loading with JSON support
bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON parser first
    if (tryLoadFromJSON(filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format
    return loadFromSimpleFormat(filepath);
}

// JSON loading implementation
bool Config::tryLoadFromJSON(const std::string& filepath) {
    // Implementation would use nlohmann/json or similar
    // Parse nested structures
    // Convert JSON keys to dot notation for backward compatibility
    // Validate each value
    return false;  // Placeholder
}

// Configuration validation
bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateEntry(entry)) {
            return false;
        }
    }
    return true;
}

bool Config::validateEntry(const ConfigEntry& entry) const {
    // Type validation
    if (!validateType(entry)) return false;
    
    // Range validation
    if (!validateRange(entry)) return false;
    
    // Format validation
    if (!validateFormat(entry)) return false;
    
    return true;
}
```

### 3. Advanced Configuration Options

#### Implementation Details:

**New Configuration Categories**

1. **Brain Configuration**
   ```
   brain.neuron_count = 1000
   brain.region_count = 2
   brain.connection_probability = 0.1
   brain.initial_weight_mean = 0.5
   brain.initial_weight_std = 0.1
   brain.v_thresh = -50.0
   brain.v_rest = -70.0
   brain.tau_mem = 20.0
   brain.tau_ref = 2.0
   ```

2. **Plasticity Configuration**
   ```
   plasticity.stdp.enable = true
   plasticity.stdp.learning_rate = 0.01
   plasticity.stdp.tau_plus = 20.0
   plasticity.stdp.tau_minus = 20.0
   plasticity.hebbian.enable = true
   plasticity.hebbian.learning_rate = 0.001
   plasticity.structural.enable = true
   plasticity.structural.synaptogenesis_rate = 0.001
   plasticity.structural.pruning_rate = 0.0001
   ```

3. **Neuromodulation Configuration**
   ```
   neuromod.dopamine.scale = 1.0
   neuromod.curiosity.enable = true
   neuromod.novelty.enable = true
   neuromod.reward.scale = 1.0
   neuromod.reward.discount_factor = 0.99
   ```

4. **Development Configuration**
   ```
   development.enable = true
   development.critical_period_length = 100.0
   development.maturation_rate = 0.01
   development.aging_rate = 0.001
   ```

5. **Agent Configuration**
   ```
   agent.enable_reward_modulation = true
   agent.enable_structural_plasticity = true
   agent.enable_development = true
   agent.enable_curiosity = true
   agent.exploration_rate = 0.1
   ```

6. **Environment Configuration**
   ```
   environment.name = "GridWorld"
   environment.width = 10
   environment.height = 10
   environment.max_energy = 100.0
   environment.vision_width = 8
   environment.vision_height = 8
   ```

#### Code Changes:

Add comprehensive validation rules and descriptions for all new configuration options.

### 4. Better Error Handling and User Feedback

#### Implementation Details:

**Enhanced Error Messages**
- Clear, actionable error messages
- Context-aware suggestions
- Color-coded output (if supported)
- Error codes for programmatic handling

**User Feedback**
- Progress indicators for long operations
- Success/failure confirmation messages
- Detailed logs for debugging
- Summary reports

#### Code Changes in `src/main.cpp`:

```cpp
// Enhanced error handling and feedback
int main(int argc, char** argv) {
    try {
        // Parse arguments
        auto args = parseArguments(argc, argv);
        
        // Load configuration with progress feedback
        NLM_LOG_INFO("Loading configuration...");
        auto config = loadEnhancedConfiguration(args);
        
        // Validate configuration with detailed feedback
        if (!validateConfiguration(args, config)) {
            printValidationErrors();
            return 1;
        }
        
        // Run simulation with progress tracking
        runSimulationWithFeedback(config);
        
        // Generate comprehensive report
        generateSimulationReport(config);
        
    } catch (const std::exception& e) {
        handleError(e);
        return 1;
    }
    
    return 0;
}
```

### 5. Configuration Templates

#### Implementation Details:

**Template Categories**

1. **Beginner's Template**
   - Minimal configuration
   - Reasonable defaults for first-time users
   - Simplified key=value format

2. **Research Template**
   - Extensive configuration options
   - Advanced plasticity settings
   - Detailed neuromodulation

3. **Performance Template**
   - Optimized for speed
   - Reduced logging
   - Simplified connectivity

4. **Learning Template**
   - Enhanced plasticity
   - Strong learning signals
   - Curiosity-driven exploration

**Template Files**

Create template files in `configs/templates/`:
- `beginner.json`
- `research.json`
- `performance.json`
- `learning.json`

#### Implementation:

```cpp
namespace nlm {

class ConfigTemplateManager {
public:
    static std::string getTemplateContent(const std::string& templateName);
    static bool applyTemplate(const std::string& templateName, Config& config);
    static std::vector<std::string> getAvailableTemplates();
    
private:
    static std::unordered_map<std::string, std::string> templates;
};

} // namespace nlm
```

## Implementation Schedule

### Phase 1: Core Infrastructure (Weeks 1-2)
1. Implement JSON parsing support
2. Add enhanced argument parsing with help system
3. Add basic validation framework
4. Update existing tests

### Phase 2: Advanced Features (Weeks 3-4)
1. Implement advanced configuration options
2. Add configuration templates
3. Enhance error handling and user feedback
4. Add command discovery and completion hints

### Phase 3: Testing and Refinement (Weeks 5-6)
1. Comprehensive testing of new features
2. Performance optimization
3. Documentation updates
4. User acceptance testing

## Files to Modify

### Modified Files
1. `src/main.cpp` - Enhanced CLI with help, validation, and improved error handling
2. `src/core/Config/Config.hpp` - Enhanced configuration system with validation
3. `src/core/Config/Config.cpp` - JSON support and validation implementation

### New Files
1. `src/core/Config/JSONParser.hpp` - JSON parsing implementation
2. `src/core/Config/Validator.hpp` - Configuration validation framework
3. `src/core/Config/TemplateManager.hpp` - Configuration template management
4. `configs/templates/beginner.json` - Beginner's template
5. `configs/templates/research.json` - Research template
6. `configs/templates/performance.json` - Performance template
7. `configs/templates/learning.json` - Learning template
8. `docs/CLI_REFERENCE.md` - CLI documentation
9. `docs/CONFIG_REFERENCE.md` - Configuration reference

## Testing Strategy

### Unit Tests
- Existing config tests (test_config.cpp)
- New JSON parsing tests
- Validation tests
- Template application tests
- CLI argument parsing tests

### Integration Tests
- Full CLI workflow tests
- Configuration file loading tests
- Template integration tests
- Cross-platform compatibility

## Benefits

1. **User-Friendly**: Comprehensive help system and intuitive interface
2. **Flexible**: Multiple configuration formats (JSON, key=value)
3. **Robust**: Comprehensive validation and error handling
4. **Extensive**: Advanced configuration options for all use cases
5. **Template-Based**: Pre-configured templates for common scenarios
6. **Developer-Friendly**: Extensive documentation and examples
7. **Maintainable**: Clean separation of concerns and modular design

## Conclusion

This improvement plan addresses all the requirements specified in the task, providing a comprehensive upgrade to the NLM project's command line interface and configuration system. The implementation focuses on usability, flexibility, and robustness, ensuring that both beginners and advanced users can effectively use the system.