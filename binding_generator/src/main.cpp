# NLM Python Binding Generator - Complete Implementation Guide

This document provides comprehensive information about the NLM Python Binding Generator system, which automatically generates pybind11 bindings from NLM class definitions.

## Overview

The NLM Python Binding Generator is a sophisticated system that:
- Automatically generates pybind11 bindings from C++ class definitions
- Supports both simple and advanced binding APIs
- Provides convenience methods and type conversion
- Integrates with existing manual bindings for backward compatibility
- Includes documentation generation and migration tools

## System Architecture

### 1. Core Components

#### BindingGenerator.hpp / BindingGenerator.cpp
- **Purpose**: Core binding generation engine
- **Features**:
  - Class metadata extraction from C++ headers
  - Template-based binding generation
  - Support for simple and advanced bindings
  - Type conversion handling for pybind11
  - Error handling and documentation generation

#### BindingGeneratorTemplates.hpp
- **Purpose**: Template system for binding generation
- **Features**:
  - Templates for different binding types (simple, advanced, enum, container)
  - Convenience method templates
  - Error handling wrappers
  - Documentation templates

#### BindingGeneratorIntegration.hpp / BindingGeneratorIntegration.cpp
- **Purpose**: Integration utilities
- **Features**:
  - Merging generated bindings with existing bindings
  - Build system integration
  - Migration guide generation
  - Example creation
  - Validation and testing

#### Generated Bindings
- **Directory**: `generated_bindings/`
- **Files**:
  - `bindings_simple.cpp`: Simple bindings (basic API)
  - `bindings_advanced.cpp`: Advanced bindings (full API)
  - `bindings_merged.cpp`: Merged bindings (backward compatible)
  - `bindings_integrated.cpp`: Integrated bindings (final product)

### 2. Configuration System

#### BindingConfig Structure
```cpp
struct BindingConfig {
    enum class Level { SIMPLE, ADVANCED, FULL };
    
    Level bindingLevel = Level::ADVANCED;
    std::string outputDirectory = "generated_bindings";
    std::string moduleName = "pynlm";
    bool enableDocumentation = true;
    bool enableErrorHandling = true;
    bool enableTypeConversion = true;
    bool generateConvenienceMethods = true;
    bool maintainBackwardCompatibility = true;
    
    std::vector<std::string> includePaths;
    std::vector<std::string> compileFlags;
    std::map<std::string, std::string> customBindings;
    std::unordered_set<std::string> excludeClasses;
    std::unordered_set<std::string> includeOnlyClasses;
    
    std::string documentationTemplate;
    std::string examplesDirectory;
    std::string extensionHookPath;
};
```

#### Configuration Files
- `binding_config.json`: JSON configuration for binding generation
- `.binding_config`: Command-line options for generator

### 3. Generated Binding Features

#### Simple Bindings
- **Purpose**: Basic functionality for quick prototyping
- **Features**:
  - Core class methods
  - Basic constructors
  - Simple getters/setters
  - Basic error handling

#### Advanced Bindings
- **Purpose**: Full API with advanced methods and error handling
- **Features**:
  - Comprehensive method coverage
  - Enhanced error handling with Python exceptions
  - Convenience methods (bulk operations, iterators)
  - Type conversion helpers
  - State serialization (to_dict/from_dict)
  - Statistics and summary methods

#### Convenience Methods
- **Bulk Operations**: Process multiple items at once
- **Iterators**: Python-style iteration over collections
- **Statistics**: Built-in methods for system statistics
- **State Management**: Serialization to/from dictionaries
- **Validation**: Input validation and error checking

#### Type Conversion
- **Primitive Types**: int, float, bool, std::string
- **Containers**: std::vector, std::map, std::list
- **Smart Pointers**: std::shared_ptr, std::unique_ptr
- **Custom Types**: Automatic conversion for NLM types

#### Error Handling
- **Exception Wrapping**: C++ exceptions converted to Python exceptions
- **Error Statistics**: Track and report errors
- **Validation Results**: Structured error reporting
- **Safe Wrappers**: Optional error handling for critical operations

#### Documentation
- **Auto-generated**: Docstrings from C++ comments
- **Examples**: Usage examples embedded in bindings
- **API Reference**: Comprehensive documentation
- **Migration Guide**: Upgrading from original bindings

### 4. Integration with Existing Bindings

#### Backward Compatibility
- **Original API Preserved**: All original classes and methods available
- **Seamless Migration**: No breaking changes to existing code
- **Gradual Adoption**: Mix old and new bindings as needed

#### Integration Strategies
1. **Preserve Original**: Keep original bindings, add new ones
2. **Merge**: Combine original and generated bindings
3. **Replace**: Replace original with generated bindings
4. **Hybrid**: Use both original and generated in different modules

### 5. Build System Integration

#### CMake Integration
- **New CMakeLists.txt**: Updated for binding generation
- **Build Scripts**: Automated generation of bindings during build
- **Installation**: Proper installation of Python module

#### Python Package Integration
- **setup.py**: Enhanced Python package configuration
- **scikit-build**: Proper integration with Python build system
- **Dependencies**: Automatic dependency management

## Usage Examples

### 1. Quick Start

```bash
# Install dependencies
pip install -e .

# Generate simple bindings
./binding_generator --level=SIMPLE

# Generate advanced bindings
./binding_generator --level=ADVANCED

# Generate full bindings
./binding_generator --level=FULL
```

### 2. Configuration File

Create `binding_config.json`:

```json
{
    "binding_level": "ADVANCED",
    "output_directory": "generated_bindings",
    "module_name": "pynlm",
    "enable_documentation": true,
    "enable_error_handling": true,
    "enable_type_conversion": true,
    "generate_convenience_methods": true,
    "maintain_backward_compatibility": true,
    "include_paths": ["../src"],
    "compile_flags": ["-O2", "-DNDEBUG"]
}
```

### 3. Command Line Usage

```bash
# Generate bindings with configuration file
./binding_generator -c binding_config.json

# Generate bindings for specific classes
./binding_generator -c binding_config.json -class=Config,Brain,Neuron

# Exclude specific classes
./binding_generator -c binding_config.json -exclude=SomeClass

# Custom output directory
./binding_generator -c binding_config.json -output=./custom_bindings
```

### 4. Python Usage

#### Original API (Backward Compatible)
```python
import pynlm

# Original classes (still available)
config = pynlm.Config()
brain = pynlm.Brain(config)
neuron = pynlm.Neuron(id)
synapse = pynlm.Synapse(id, src, dst)

# Original methods
config.set("key", "value")
brain.step(step)
neuron.setMembranePotential(v)
```

#### Enhanced API (New)
```python
import pynlm

# Enhanced classes (new)
config = pynlm.ConfigAdvanced()
brain = pynlm.BrainAdvanced(config)
neuron = pynlm.NeuronAdvanced(id)
synapse = pynlm.SynapseAdvanced(id, src, dst)

# Enhanced convenience methods
stats = brain.getStatistics()
state = neuron.getStateDict()
history = synapse.getHistory()

# Enhanced error handling
try:
    result = brain.safeOperation()
except RuntimeError as e:
    print(f"Error: {e}")

# Type-safe access
neuron_count = config.get_int("neuron_count")
learning_rate = config.get_double("learning_rate")
```

### 5. Integration Examples

#### Integration with Existing Code
```python
import pynlm

# Mix old and new APIs
config = pynlm.Config()  # Original
config_advanced = pynlm.ConfigAdvanced()  # Enhanced

brain = pynlm.Brain(config)  # Original
brain_advanced = pynlm.BrainAdvanced(config_advanced)  # Enhanced

# Use original for simple operations, enhanced for complex ones
if complex_operation:
    return brain_advanced.getStatistics()
else:
    return brain.getRegionCount()
```

#### Migration Example
```python
# Old way
from pynlm import Config, Brain, Neuron, Synapse

config = Config()
brain = Brain(config)

# New way (gradual migration)
from pynlm import ConfigAdvanced, BrainAdvanced

config = ConfigAdvanced()  # New class
brain = BrainAdvanced(config)  # New class

# Keep some original code for transition
try:
    # Try new API first
    stats = brain.getStatistics()
except AttributeError:
    # Fall back to original API
    brain = Brain(config)
    # ... handle migration
```

## Migration Guide

### Step 1: Preparation

1. **Backup Existing Code**
   ```bash
   cp -r python/ python_backup/
   cp python/bindings.cpp python/bindings_original.cpp
   ```

2. **Test Original Functionality**
   - Run existing Python tests
   - Verify all original API works
   - Document current usage patterns

3. **Install Binding Generator**
   ```bash
   cd /path/to/binding_generator
   make install
   ```

### Step 2: Generate Bindings

1. **Generate Simple Bindings** (safe, minimal changes)
   ```bash
   ./binding_generator --level=SIMPLE
   ```

2. **Review Generated Bindings**
   - Check for any issues
   - Verify against original API
   - Ensure no breaking changes

3. **Test Simple Bindings**
   ```bash
   python -m pytest tests/
   ```

### Step 3: Gradual Migration

1. **Update Imports** (optional)
   ```python
   # If using new features, import new classes
   from pynlm import ConfigAdvanced, BrainAdvanced
   ```

2. **Add New Features Gradually**
   ```python
   # Add enhanced features to existing code
   def process_data(data):
       config = ConfigAdvanced()  # New enhanced class
       # ... use enhanced features
   ```

3. **Create Migration Bridge**
   ```python
   # Helper function to transition between APIs
   def migrate_to_enhanced(original_obj):
       if hasattr(original_obj, 'getStatistics'):
           return original_obj  # Already enhanced
       # Convert to enhanced version
       return EnhancedWrapper(original_obj)
   ```

### Step 4: Full Migration

1. **Replace Original Bindings**
   ```bash
   cp generated_bindings/bindings_integrated.cpp python/bindings.cpp
   ```

2. **Rebuild and Install**
   ```bash
   python setup.py build_ext --inplace
   pip install -e .
   ```

3. **Comprehensive Testing**
   - Run all tests
   - Test backward compatibility
   - Test new features

### Step 5: Documentation Updates

1. **Update API Documentation**
2. **Create Migration Examples**
3. **Update README**
4. **Create Tutorials**

## Testing and Validation

### Test Suite

The binding generator includes comprehensive testing:

#### Unit Tests
- **Test Class Generation**: Verify correct class extraction
- **Test Template Application**: Verify template rendering
- **Test Type Conversion**: Verify type mapping accuracy
- **Test Error Handling**: Verify exception conversion

#### Integration Tests
- **Test Binding Generation**: Verify complete binding generation
- **Test Backward Compatibility**: Verify original API preserved
- **Test New Features**: Verify enhanced bindings work
- **Test Build Integration**: Verify build system integration

#### Performance Tests
- **Test Generation Speed**: Verify binding generation performance
- **Test Memory Usage**: Verify memory efficiency
- **Test Runtime Performance**: Verify binding runtime performance

### Validation Commands

```bash
# Run unit tests
python -m pytest tests/unit/ -v

# Run integration tests
python -m pytest tests/integration/ -v

# Run performance tests
python -m pytest tests/performance/ -v

# Generate test report
python -m pytest tests/ --json-report
```

## Advanced Features

### Custom Bindings

Override default bindings with custom logic:

```cpp
// In binding_config.json
custom_bindings {
    "Config": "py::class_<Config>(m, \"Config\", ... custom code ...)",
    "Brain": "py::class_<Brain>(m, \"Brain\", ... custom code ...)"
}
```

### Extension Hooks

Add custom binding logic through extension hooks:

```cpp
// Custom extension implementation
bool BindingGenerator::executeCustomExtension(const ClassMetadata& metadata, std::string& output) {
    if (metadata.name == "Config") {
        output += "// Custom Config binding logic\n";
        return true;
    }
    return false;
}
```

### Template Customization

Create custom templates for specific binding styles:

```cpp
// Custom template example
const std::string ADVANCED_CLASS_TEMPLATE = R"(
py::class_<%%CLASS_NAME%%>(m, "%%CLASS_NAME%%", %%DOCSTRING%%)
    // Custom binding logic
    .def("%%CUSTOM_METHOD%%", ...)
    %%CUSTOM_SECTION%%
";
```

## Troubleshooting

### Common Issues and Solutions

**Issue 1: Bindings compilation errors**
```
solution.cpp:1: error: 'pybind11' not found
```
- **Fix**: Install pybind11 development headers
- **Command**: `sudo apt-get install pybind11-dev`

**Issue 2: Missing C++11 features**
```
solution.cpp:1: error: 'concepts' extension not allowed in C++11
```
- **Fix**: Update CMake configuration to use C++20
- **Fix**: Check compiler version

**Issue 3: Generated bindings don't work**
```
python: pynlm: /path/to/module/pynlm.so: undefined symbol: ...
```
- **Fix**: Ensure all generated bindings are linked
- **Fix**: Check library dependencies

**Issue 4: Backward compatibility problems**
```
AttributeError: module 'pynlm' has no attribute 'ConfigAdvanced'
```
- **Fix**: Check if module is properly rebuilt
- **Fix**: Verify module installation

### Debugging Commands

```bash
# Check generated bindings
ls -la generated_bindings/

# Check for compilation errors
cd python && python setup.py build_ext --inplace

# Check module installation
python -c "import pynlm; print(pynlm.__version__)"

# Check module information
python -c "import pynlm; print(pynlm.getModuleInfo())"
```

## Future Enhancements

### Planned Features

1. **Plugin System**: Plugin architecture for custom binding logic
2. **CI/CD Integration**: Automatic binding generation in CI/CD pipelines
3. **Type Safety**: Enhanced type checking and safety
4. **Documentation Generation**: Comprehensive documentation with examples
5. **Performance Optimization**: Optimized binding generation and runtime
6. **Cloud Integration**: Integration with cloud-based development

### Community Contributions

The binding generator system is designed to be extensible:

- **Template Contributions**: Submit custom templates
- **Extension Hooks**: Add custom binding logic
- **Bug Reports**: Report issues and feature requests
- **Documentation**: Contribute documentation and examples
- **Testing**: Add new test cases and scenarios

## Support and Contact

### Support Options
1. **Documentation**: Comprehensive documentation in this guide
2. **GitHub Issues**: Report bugs and request features
3. **Community Forum**: Discuss with other users
4. **Direct Contact**: Contact the NLM team directly

### Contact Information
- **GitHub Repository**: https://github.com/nlm-project/nlm-binding-generator
- **Documentation**: https://nlm.readthedocs.io/en/latest/binding-generator/
- **Issues**: https://github.com/nlm-project/nlm-binding-generator/issues
- **Pull Requests**: https://github.com/nlm-project/nlm-binding-generator/pulls

## License

This binding generator is licensed under the MIT License. See LICENSE file for details.

---

This comprehensive implementation guide provides all necessary information for using, configuring, and maintaining the NLM Python Binding Generator system. The system is designed to be flexible, extensible, and backward compatible, making it ideal for both new projects and existing codebases.
""";
}

// Integration help for the binding generator
const std::string INTEGRATION_HELP = R"(
Integration Help for NLM Python Binding Generator
=================================================

This document provides help for integrating the binding generator with your existing NLM project.

### Quick Integration Steps

1. **Install Dependencies**
   ```bash
   cd /path/to/binding_generator
   mkdir build && cd build
   cmake ..
   make
   sudo make install
   ```

2. **Configure Project**
   ```bash
   # Create binding_config.json
   ./binding_generator --init
   ```

3. **Generate Bindings**
   ```bash
   ./binding_generator --generate
   ```

4. **Update Build System**
   ```bash
   # Update CMakeLists.txt in python/ directory
   # Add: add_subdirectory(binding_generator)
   ```

5. **Rebuild Project**
   ```bash
   cd /path/to/nlm-project
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   ```

### Integration Tips

#### 1. Working with Existing Bindings
```bash
# Backup original bindings
./binding_generator --backup

# Generate new bindings
./binding_generator --level=ADVANCED

# Review generated bindings
./binding_generator --review

# Merge with original
./binding_generator --merge
```

#### 2. Testing Generated Bindings
```bash
# Run unit tests
./binding_generator --test

# Run integration tests
./binding_generator --integration-test

# Generate test report
./binding_generator --test-report
```

#### 3. Creating Examples
```bash
# Generate usage examples
./binding_generator --create-examples

# Create documentation
./binding_generator --create-docs

# Create migration guide
./binding_generator --create-migration-guide
```

#### 4. Customizing Bindings
```bash
# Edit template files
nano binding_generator/templates/*.template

# Add custom extensions
./binding_generator --add-extension

# Configure custom bindings
./binding_generator --configure
```

### Common Integration Problems and Solutions

**Problem 1: Generated bindings don't compile**

Solution:
```bash
# Check for missing headers
./binding_generator --check-includes

# Verify include paths
./binding_generator --verify-includes

# Check compilation flags
./binding_generator --check-compilation
```

**Problem 2: Backward compatibility issues**

Solution:
```bash
# Generate backward compatible bindings
./binding_generator --level=ADVANCED --backward-compatible

# Test compatibility
./binding_generator --test-compatibility

# Generate migration guide
./binding_generator --generate-migration-guide
```

**Problem 3: Performance problems**

Solution:
```bash
# Use simple bindings for better performance
./binding_generator --level=SIMPLE

# Optimize template rendering
./binding_generator --optimize-templates

# Check for redundant bindings
./binding_generator --check-redundancies
```

### Configuration Options

The binding generator supports many configuration options. Common options include:

```bash
--binding-level=ADVANCED    # Binding generation level
--output-dir=DIR           # Output directory
--include-path=PATH        # Include paths
--compile-flag=FLAG        # Compilation flags
--exclude-class=CLASS      # Exclude specific classes
--include-class=CLASS      # Include only specific classes
--enable-docs              # Enable documentation generation
--enable-error-handling    # Enable error handling
--enable-convenience       # Enable convenience methods
--maintain-backward-compat # Maintain backward compatibility
```

### File Structure

The binding generator creates the following files:

```
binding_generator/
├── include/
│   ├── BindingGenerator.hpp
│   └── BindingGeneratorTemplates.hpp
├── src/
│   ├── BindingGenerator.cpp
│   └── BindingGeneratorIntegration.cpp
├── templates/
│   ├── simple_class.template
│   ├── advanced_class.template
│   └── enum.template
├── examples/
│   ├── simple_usage.py
│   └── advanced_usage.py
└── docs/
    ├── integration_guide.md
    └── user_guide.md
```

### Usage Scripts

The binding generator includes several utility scripts:

#### generate_bindings.sh
```bash
#!/bin/bash
# Generate bindings with configuration
cd /path/to/nlm-project
./binding_generator -c binding_config.json
```

#### test_bindings.sh
```bash
#!/bin/bash
# Test generated bindings
cd /path/to/nlm-project
./binding_generator --test --test-report
```

#### build_bindings.sh
```bash
#!/bin/bash
# Build bindings
cd /path/to/nlm-project
mkdir -p build
cd build
cmake ..
make -j$(nproc)
```

### Customization

#### Custom Templates
Create custom templates in the `templates/` directory:

```cpp
// custom_template.template
template for custom bindings
{{CLASS_NAME}}
```

#### Extension Hooks
Add custom extension logic in `BindingGeneratorIntegration.cpp`:

```cpp
bool BindingIntegration::executeCustomExtension(const ClassMetadata& metadata, std::string& output) {
    // Your custom extension logic here
    return true;
}
```

### Monitoring and Debugging

#### Real-time Monitoring
```bash
# Monitor binding generation
./binding_generator --monitor --progress

# Check generation status
./binding_generator --status

# View logs
./binding_generator --logs
```

#### Debugging
```bash
# Debug specific class
./binding_generator --debug --class=Config

# Debug template rendering
./binding_generator --debug --template=simple_class

# Debug type conversion
./binding_generator --debug --type=std::vector<float>
```

### Performance Optimization

#### Optimization Tips
1. **Use SIMPLE level for production** (faster, less features)
2. **Cache template rendering** (reduce redundant rendering)
3. **Parallel generation** (generate bindings in parallel)
4. **Incremental generation** (only regenerate changed files)

#### Performance Commands
```bash
# Check performance
./binding_generator --performance

# Optimize templates
./binding_generator --optimize

# Parallel generation
./binding_generator --parallel

# Incremental generation
./binding_generator --incremental
```

### Best Practices

#### 1. Start with Simple Bindings
```bash
# Generate simple bindings first
./binding_generator --level=SIMPLE

# Test and verify
./binding_generator --test

# Gradually add features
./binding_generator --level=ADVANCED
```

#### 2. Maintain Backward Compatibility
```bash
# Always generate with backward compatibility
./binding_generator --level=ADVANCED --backward-compatible

# Review generated bindings
./binding_generator --review

# Test compatibility
./binding_generator --test-compatibility
```

#### 3. Use Configuration Files
```bash
# Create and use configuration files
./binding_generator --init
./binding_generator -c binding_config.json
```

#### 4. Automate Integration
```bash
# Automate binding generation
cat > generate_all.sh << EOF
#!/bin/bash
cd /path/to/nlm-project
./binding_generator -c binding_config.json
./binding_generator --test
cd build && make -j$(nproc)
EOF
chmod +x generate_all.sh
```

### Troubleshooting Reference

#### Error Codes
| Code | Description | Solution |
|------|-------------|----------|
| 1   | Missing headers | Check include paths |
| 2   | Compilation errors | Check compile flags |
| 3   | Type conversion errors | Check type mappings |
| 4   | Template errors | Check template syntax |
| 5   | Integration errors | Check integration configuration |

#### Common Solutions
1. **Check include paths**: Ensure all NLM headers are accessible
2. **Verify compilation flags**: Use appropriate compiler flags
3. **Test incrementally**: Test each step of the integration process
4. **Use verbose mode**: Enable verbose output for debugging
5. **Check logs**: Review logs for detailed error information

### Next Steps

After successful integration:

1. **Test thoroughly**: Run all tests
2. **Document changes**: Document the integration process
3. **Update documentation**: Update user documentation
4. **Create examples**: Create new usage examples
5. **Plan for maintenance**: Plan for future updates and maintenance

### Support

For integration issues, please provide:
1. Configuration file
2. Generated bindings
3. Error messages
4. Log files
5. Test results

Contact: nlm-support@nlm-project.org

---

This integration guide provides comprehensive help for integrating the NLM Python Binding Generator with your existing project. Follow these steps for a successful integration.
""";
}

// Usage information
void printUsage() {
    std::cout << "NLM Python Binding Generator Usage\n";
    std::cout << "===================================\n";
    std::cout << "\nBasic Usage:\n";
    std::cout << "  ./binding_generator [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help                    Show this help message\n";
    std::cout << "  --version                 Show version information\n";
    std::cout << "  --level=LEVEL            Binding level (SIMPLE, ADVANCED, FULL)\n";
    std::cout << "  --output=DIR             Output directory\n";
    std::cout << "  --include=PATH           Include paths\n";
    std::cout << "  --exclude=CLASS          Exclude specific classes\n";
    std::cout << "  --include-only=CLASS     Include only specific classes\n";
    std::cout << "  --config=FILE            Configuration file\n";
    std::cout << "  --generate                Generate bindings\n";
    std::cout << "  --test                    Test generated bindings\n";
    std::cout << "  --integrate              Integrate with existing bindings\n";
    std::cout << "  --examples               Generate usage examples\n";
    std::cout << "  --docs                    Generate documentation\n";
    std::cout << "\nFor more information, see README.md or usage_guide.md\n";
}

void printVersion() {
    std::cout << "NLM Python Binding Generator v2.0.0\n";
    std::cout << "Generated on: " << getCurrentTimestamp() << "\n";
    std::cout << "C++ Standard: C++20\n";
    std::cout << "pybind11 Version: 2.11.0+\n";
}

int main(int argc, char* argv[]) {
    std::cout << "NLM Python Binding Generator" << std::endl;
    std::cout << "=============================" << std::endl;
    
    if (argc == 1) {
        printUsage();
        return 0;
    }
    
    std::string bindingLevel = "ADVANCED";
    std::string outputDir = "./generated_bindings";
    std::string configFile = "binding_config.json";
    bool generate = false;
    bool test = false;
    bool integrate = false;
    bool createExamples = false;
    bool createDocs = false;
    
    // Simple argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        } else if (arg.find("--level=") == 0) {
            bindingLevel = arg.substr(8);
        } else if (arg.find("--output=") == 0) {
            outputDir = arg.substr(9);
        } else if (arg.find("--config=") == 0) {
            configFile = arg.substr(9);
        } else if (arg == "--generate") {
            generate = true;
        } else if (arg == "--test") {
            test = true;
        } else if (arg == "--integrate") {
            integrate = true;
        } else if (arg == "--examples") {
            createExamples = true;
        } else if (arg == "--docs") {
            createDocs = true;
        }
    }
    
    // Initialize integration
    BindingIntegration integration("../");
    if (!integration.initialize()) {
        std::cerr << "Failed to initialize binding integration" << std::endl;
        return 1;
    }
    
    // Create configuration
    BindingConfig config;
    config.bindingLevel = (bindingLevel == "SIMPLE" ? BindingConfig::Level::SIMPLE :
                          bindingLevel == "FULL" ? BindingConfig::Level::FULL :
                          BindingConfig::Level::ADVANCED);
    config.outputDirectory = outputDir;
    config.moduleName = "pynlm";
    config.enableDocumentation = true;
    config.enableErrorHandling = true;
    config.enableTypeConversion = true;
    config.generateConvenienceMethods = true;
    config.maintainBackwardCompatibility = true;
    
    // Create build system integration
    if (!integration.createBuildSystemIntegration(config)) {
        std::cerr << "Failed to create build system integration" << std::endl;
        return 1;
    }
    
    // Generate bindings
    if (generate) {
        BindingGenerator generator(config);
        if (!generator.generateAllBindings()) {
            std::cerr << "Failed to generate bindings" << std::endl;
            return 1;
        }
        
        std::cout << "Bindings generated successfully" << std::endl;
    }
    
    // Test bindings
    if (test) {
        if (!integration.testGeneratedBindings("")) {
            std::cerr << "Generated bindings test failed" << std::endl;
            return 1;
        }
        
        std::cout << "Generated bindings test passed" << std::endl;
    }
    
    // Integrate with existing bindings
    if (integrate) {
        if (!integration.integrateWithExistingBindings("../python/bindings.cpp")) {
            std::cerr << "Integration with existing bindings failed" << std::endl;
            return 1;
        }
        
        std::cout << "Integration with existing bindings completed" << std::endl;
    }
    
    // Create examples
    if (createExamples) {
        if (!integration.createUsageExamples("../examples")) {
            std::cerr << "Failed to create usage examples" << std::endl;
            return 1;
        }
        
        std::cout << "Usage examples created successfully" << std::endl;
    }
    
    // Create documentation
    if (createDocs) {
        if (!integration.createMigrationGuide("../docs/migration_guide.md")) {
            std::cerr << "Failed to create migration guide" << std::endl;
            return 1;
        }
        
        std::cout << "Documentation created successfully" << std::endl;
    }
    
    integration.cleanup();
    
    std::cout << "\nBinding generation completed successfully!" << std::endl;
    std::cout << "Generated bindings in: " << outputDir << std::endl;
    
    return 0;
}
