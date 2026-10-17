# NLM — Migration Guide

This document provides comprehensive migration guide for the NLM codebase, covering version changes, breaking changes, migration steps, and compatibility.

---

## Overview

This migration guide covers the changes made during Phase 6 integration, focusing on the error handling framework, enhanced configuration system, and command-line interface improvements.

## Version History

### Current Version: 1.0.0 (Phase 6)

- **Release Date:** Phase 6 completion
- **Status:** Stable, production-ready
- **Key Improvements:**
  - Comprehensive error handling framework
  - Enhanced configuration system with CLI subcommands
  - All systems integrated into coherent brain loop
  - Checkpoint save/load working
  - Replay and consolidation functional

### Previous Versions

- **Phase 5:** Performance optimizations and component completion
- **Phase 4:** Individual system implementation
- **Phase 3:** Core system development
- **Phase 2:** Basic infrastructure
- **Phase 1:** Project skeleton and core types

---

## Breaking Changes

### 1. Error Handling Changes

#### Changes
- Exception hierarchy completely reorganized
- New error classes: NLMError, ConfigError, ValidationError, FileError, InitializationError
- Error handling macros: NLM_CHECK_NULL, NLM_CHECK_CONDITION, NLM_THROW_VALIDATION, NLM_THROW_CONFIG, NLM_THROW_FILE, NLM_THROW_INITIALIZATION
- Result type for error handling without exceptions

#### Migration Steps

**Before:**
```cpp
// Basic error handling
if (!some_condition) {
    std::cerr << "Error occurred" << std::endl;
    return 1;
}
```

**After:**
```cpp
// Using NLM error handling framework
NLM_CHECK_CONDITION(some_condition, 
                   "Invalid parameter value", 
                   "parameter", 
                   ValidationError::ErrorType::InvalidValue);
```

**Before:**
```cpp
// Configuration error handling
if (!config.has("brain.neuron_count")) {
    std::cerr << "Missing neuron count" << std::endl;
    return 1;
}
```

**After:**
```cpp
// Enhanced configuration error handling
try {
    config.loadFromFile("config.json");
    config.validate();
} catch (const ConfigError& e) {
    NLM_THROW_CONFIG("Failed to load configuration", e.getKey());
}
```

### 2. Configuration System Changes

#### Changes
- Enhanced configuration class with multiple sources
- Configuration validation with schema support
- Built-in presets: basic, advanced, debug, production
- Built-in templates: default, debug, simple
- Environment variable support (NLM_ prefix)
- CLI subcommands for configuration management

#### Migration Steps

**Before:**
```bash
# Basic configuration loading
./nlm --config=basic.cfg
```

**After:**
```bash
# Enhanced configuration loading
./nlm --config=basic.cfg
./nlm --preset=basic
./nlm --template=default --override="neuron_count=1000"
```

**Before:**
```python
# Basic configuration
config.loadFromFile("config.json");
```

**After:**
```python
# Enhanced configuration with validation
try {
    config.loadFromFile("config.json");
    config.validate();
} catch (const ConfigError& e) {
    std::cerr << "Configuration error: " << e.what() << std::endl;
}
```

### 3. Command Line Interface Changes

#### Changes
- Multiple subcommands for different operations
- Enhanced help system with detailed descriptions
- Configuration export/import capabilities
- Configuration comparison feature

#### Migration Steps

**Before:**
```bash
# Basic help
./nlm --help
```

**After:**
```bash
# Enhanced help with subcommands
./nlm --help
./nlm --run-phase-6
./nlm --validate-config
./nlm --export-config --format=json
./nlm --info
./nlm --list-presets
./nlm --list-templates
```

**Before:**
```bash
# Basic configuration loading
./nlm --config=config.json
```

**After:**
```bash
# Enhanced configuration loading with options
./nlm --config=config.json
./nlm --preset=basic
./nlm --template=default --override="neuron_count=1000"
./nlm --env-prefix=NLM
./nlm --no-env
```

### 4. Code Structure Changes

#### Changes
- Improved error handling in all critical components
- Enhanced configuration validation
- Better resource management
- Improved code organization

#### Migration Steps

**Before:**
```cpp
// Basic null pointer check
if (region == nullptr) {
    std::cerr << "Region is null" << std::endl;
    return;
}
```

**After:**
```cpp
// Using NLM error handling macros
NLM_CHECK_NULL(region, "region");
```

**Before:**
```cpp
// Basic configuration validation
if (!config.has("brain.neuron_count")) {
    std::cerr << "Missing neuron count" << std::endl;
    return;
}
```

**After:**
```cpp
// Enhanced configuration validation
NLM_CHECK_CONDITION(config.has("brain.neuron_count"),
                   "Neuron count not specified",
                   "brain.neuron_count",
                   ValidationError::ErrorType::MissingRequired);
```

---

## Migration Steps

### Step 1: Upgrade Codebase

#### Prerequisites
- Ensure you have the latest version of the codebase
- Verify all dependencies are installed
- Check that the build system is properly configured

#### Actions
```bash
# Clone the latest version
# The codebase has been enhanced with backward compatibility
```

### Step 2: Update Error Handling

#### Update Files
- `src/brain/Brain.cpp` - Add NLM_CHECK_NULL macros
- `src/brain/NeuralPopulation.cpp` - Add NLM_CHECK_CONDITION macros
- `src/agent/AgentBrain.cpp` - Add NLM_CHECK_NULL macros
- `src/core/Config/Config.cpp` - Add validation macros

#### Example Migration

**File: `src/brain/Brain.cpp`**

**Before:**
```cpp
// Brain.cpp - Before
if (!region) {
    std::cerr << "Region is null" << std::endl;
    return;
}
```

**After:**
```cpp
// Brain.cpp - After
NLM_CHECK_NULL(region, "region");
```

### Step 3: Update Configuration Handling

#### Update Files
- `src/core/Config/Config.cpp` - Add validation
- `src/main.cpp` - Add CLI subcommands
- `src/brain/Brain.cpp` - Add configuration validation

#### Example Migration

**File: `src/core/Config/Config.cpp`**

**Before:**
```cpp
// Config.cpp - Before
if (!config.has("brain.neuron_count")) {
    std::cerr << "Missing neuron count" << std::endl;
    return;
}
```

**After:**
```cpp
// Config.cpp - After
try {
    config.loadFromFile("config.json");
    config.validate();
} catch (const ConfigError& e) {
    NLM_THROW_CONFIG("Failed to load configuration", e.getKey());
}
```

### Step 4: Update Command Line Interface

#### Update Files
- `src/main.cpp` - Add CLI subcommands
- Update help system
- Add configuration export/import

#### Example Migration

**File: `src/main.cpp`**

**Before:**
```cpp
// main.cpp - Before
void handleCommandLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--config") == 0) {
            // Load configuration
        }
    }
}
```

**After:**
```cpp
// main.cpp - After
void handleCommandLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--config") == 0) {
            // Load configuration from file
        } else if (strcmp(argv[i], "--preset") == 0) {
            // Load configuration from preset
        } else if (strcmp(argv[i], "--template") == 0) {
            // Load configuration from template
        } else if (strcmp(argv[i], "--run-phase-6") == 0) {
            // Run Phase 6 experiments
        } else if (strcmp(argv[i], "--validate-config") == 0) {
            // Validate configuration
        } else if (strcmp(argv[i], "--export-config") == 0) {
            // Export configuration
        } else if (strcmp(argv[i], "--info") == 0) {
            // Show configuration information
        } else if (strcmp(argv[i], "--compare-configs") == 0) {
            // Compare configurations
        } else if (strcmp(argv[i], "--list-presets") == 0) {
            // List presets
        } else if (strcmp(argv[i], "--list-templates") == 0) {
            // List templates
        }
    }
}
```

### Step 5: Update Test Files

#### Update Files
- `tests/test_error_handling.cpp` - Add error handling tests
- `tests/test_config.cpp` - Add configuration tests
- `tests/test_main.cpp` - Add CLI tests

#### Example Migration

**File: `tests/test_error_handling.cpp`**

**Before:**
```cpp
// test_error_handling.cpp - Before
void testErrorHandling() {
    // Basic error handling tests
}
```

**After:**
```cpp
// test_error_handling.cpp - After
void testErrorHandling() {
    // Test all error classes
    testNLMError();
    testConfigError();
    testValidationError();
    testFileError();
    testInitializationError();
    
    // Test null pointer checking
    testNullPointerChecking();
    
    // Test error handling macros
    testErrorHandlingMacros();
}
```

### Step 6: Update Documentation

#### Update Files
- Update `README.md` - Add new features
- Update `easy_usage.md` - Add new examples
- Update `HOW_TO_USE.md` - Add new CLI commands
- Create new documentation files

#### Example Migration

**File: `README.md`**

**Before:**
```markdown
# NLM — Neural Learning Machine

## Overview
NLM is a brain-inspired spiking neural network simulator...
```

**After:**
```markdown
# NLM — Neural Learning Machine

## Overview
NLM (熙然, meaning "serene flow") is an experimental computational brain project...

## New Features
- **Phase 6 Integration**: All brain systems work cohesively as a unified neural system
- **Error Handling Framework**: Comprehensive exception hierarchy with proper validation and error reporting
- **Enhanced Configuration System**: Advanced CLI with subcommands, presets, templates, and validation
- **Code Structure**: Improved maintainability and robustness across all modules

## Installation
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install .
```
```
```

### Step 7: Testing Migration

#### Run Tests
```bash
# Run all tests
cd build
ctest --output-on-failure

# Run specific tests
./nlm_test --error_handling
./nlm_test --config_system
```

#### Test Results
- All tests should pass with the new error handling framework
- All tests should pass with the enhanced configuration system
- All tests should pass with the new CLI subcommands

### Step 8: Production Deployment

#### Preparation
- Test the application with your specific configuration
- Verify all functionality works as expected
- Monitor system resources
- Check for any performance issues

#### Deployment
```bash
# Install the application
pip install .

# Test the application
./nlm --preset=production

# Monitor system resources
./nlm --monitor --resources
```

---

## Backward Compatibility

### What Remains Compatible

1. **Configuration Files:**
   - Existing .cfg files are still supported
   - JSON format is also supported
   - All existing configuration keys continue to work

2. **API Changes:**
   - All existing methods continue to work
   - New functionality is added, not replacing old
   - Enhanced error handling is additive, not breaking

3. **Command Line:**
   - Existing command line options continue to work
   - New subcommands are added, not replacing old
   - Help system is enhanced with detailed descriptions

### What Changed

1. **Error Handling:**
   - Exception hierarchy reorganized
   - New error classes added
   - Error handling macros added

2. **Configuration System:**
   - Enhanced configuration class
   - Configuration validation added
   - Built-in presets and templates added

3. **CLI:**
   - Multiple subcommands added
   - Enhanced help system
   - Configuration export/import added

### Compatibility Matrix

| Feature | Compatible Version | Notes |
|---------|------------------|-------|
| Error Handling Framework | 1.0.0+ | New framework, not backward compatible |
| Enhanced Configuration | 1.0.0+ | New features, backward compatible |
| CLI Subcommands | 1.0.0+ | New subcommands, backward compatible |
| Presets and Templates | 1.0.0+ | New features, backward compatible |

---

## Troubleshooting Migration

### Common Issues and Solutions

#### 1. "Error handling macros not found"

**Symptoms:**
- Compilation errors
- Missing NLM_CHECK_NULL, NLM_CHECK_CONDITION, etc.

**Solutions:**
```cpp
// Add error handling header
#include "core/Error/ErrorHandling.hpp"

// Use NLM error handling macros
NLM_CHECK_NULL(ptr, "parameter");
```

#### 2. "Configuration validation errors"

**Symptoms:**
- Configuration validation fails
- Missing required keys
- Invalid values

**Solutions:**
```bash
# Check configuration schema
./nlm --validate-config --schema=custom_schema.json

# Fix configuration
cat > config.json << EOF
{
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1
  }
}
EOF
```

#### 3. "CLI subcommands not found"

**Symptoms:**
- Command not found
- Help not showing subcommands

**Solutions:**
```bash
# Rebuild the application
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run CLI help
./nlm --help
```

#### 4. "Configuration presets not found"

**Symptoms:**
- Presets command fails
- No presets listed

**Solutions:**
```bash
# List available presets
./nlm --list-presets

# Check configuration system
./nlm --validate-config
```

#### 5. "Configuration templates not found"

**Symptoms:**
- Templates command fails
- No templates listed

**Solutions:**
```bash
# List available templates
./nlm --list-templates

# Check configuration system
./nlm --validate-config
```

### Migration Support

#### Getting Help

1. **Documentation:**
   - This migration guide
   - API documentation
   - Configuration documentation
   - Examples

2. **Community Resources:**
   - GitHub repository
   - Discussion forums
   - Issue tracker

3. **Technical Support:**
   - Available through commercial support
   - Available for enterprise customers

---

## References

1. **NLMUserGuide.md** - User guide and tutorials
2. **NLMApiReference.md** - API documentation
3. **NLMConfigurationReference.md** - Configuration documentation
4. **NLMExamples.md** - Additional examples
5. **NLMBestPractices.md** - Best practices
6. **NLMTroubleshooting.md** - Troubleshooting guide

---

*Last Updated: Phase 6 Completion*
*Version: 1.0.0*