# NLM Advanced Configuration Guide

## Overview

The Advanced Configuration system provides NLM users with powerful configuration management capabilities beyond the basic configuration system. It includes template-based configuration, inheritance, validation, scripting, and runtime modification capabilities.

## Key Features

### 1. Configuration Templates

Template-based configuration management with:

- **Schema definition**: Validate configuration structure and types
- **Default values**: Provide sensible defaults for all configuration options
- **Inheritance**: Support for template inheritance and composition
- **Validation rules**: Complex validation logic for configuration values
- **Metadata**: Documentation, versioning, and categorization

### 2. Configuration Inheritance

Advanced inheritance mechanisms:

- **Template inheritance**: Derive templates from parent templates
- **Configuration inheritance**: Apply inheritance rules to configuration values
- **Override rules**: Control how child templates override parent values
- **Merge rules**: Combine values from multiple sources
- **Validation inheritance**: Inherit validation rules from templates

### 3. Runtime Configuration

Real-time configuration management:

- **Runtime modification**: Change configuration without restarting
- **Change tracking**: Track all configuration changes
- **Rollback support**: Revert to previous configuration states
- **Validation**: Validate configuration on modification
- **Persistence**: Save and load configuration from files

### 4. Configuration Scripting

Script-based configuration management:

- **Expression evaluation**: Simple expression language for configuration
- **Lua/Python support**: Full scripting language integration
- **Conditional logic**: If/then/else logic for configuration
- **Variable substitution**: Reference other configuration values
- **Trigger-based execution**: Execute scripts based on events

## Getting Started

### Basic Configuration Management

```cpp
#include "src/agent/AdvancedConfig.hpp"

// Access advanced configuration system
AdvancedConfig& config = AdvancedConfig::getInstance();

// Set basic configuration values
config.setValue("brain.neuron_count", 2000);
config.setValue("performance.optimization_level", 5);
config.setValue("memory.pool_size", 2048);

// Get configuration values
int neuronCount = config.getOr("brain.neuron_count", 1000);
std::string optimization = config.get<std::string>("performance.optimization_level");

// Check if configuration exists
if (config.has("brain.neuron_count")) {
    int count = config.get<int>("brain.neuron_count");
}
```

### Configuration Templates

```cpp
#include "src/agent/AdvancedConfig.hpp"

void createPerformanceTemplate() {
    AdvancedConfig& config = AdvancedConfig::getInstance();
    
    // Create a performance optimization template
    nlm::ConfigTemplate perfTemplate;
    perfTemplate.name = "high_performance";
    perfTemplate.description = "Configuration optimized for high-performance computing";
    perfTemplate.version = "2.0";
    
    // Add schema entries
    nlm::ConfigSchemaEntry entry1;
    entry1.key = "performance.optimization_level";
    entry1.type = "int";
    entry1.defaultValue = 5;
    entry1.minValue = 1;
    entry1.maxValue = 10;
    entry1.required = false;
    entry1.description = "Performance optimization level (1-10)";
    perfTemplate.schema.push_back(entry1);
    
    // Add more entries...
    
    // Create template
    config.createTemplate(perfTemplate);
}

void usePerformanceTemplate() {
    AdvancedConfig& config = AdvancedConfig::getInstance();
    
    // Load performance template
    nlm::Config myConfig;
    if (config.loadTemplate("high_performance", myConfig)) {
        std::cout << "Performance template loaded successfully!" << std::endl;
    }
}
```

### Configuration Inheritance

```cpp
#include "src/agent/AdvancedConfig.hpp"

void setupInheritanceChain() {
    AdvancedConfig& config = AdvancedConfig::getInstance();
    
    // Create base template
    nlm::ConfigTemplate baseTemplate;
    baseTemplate.name = "base_template";
    baseTemplate.description = "Base configuration template";
    baseTemplate.isAbstract = true;
    baseTemplate.addSchemaEntry("brain.neuron_count", nlm::ConfigType::INT, 1000, 100, 10000);
    
    config.createTemplate(baseTemplate);
    
    // Create derived template
    nlm::ConfigTemplate derivedTemplate;
    derivedTemplate.name = "derived_template";
    derivedTemplate.description = "Derived from base template";
    derivedTemplate.baseTemplate = "base_template";
    derivedTemplate.addSchemaEntry("performance.optimization_level", nlm::ConfigType::INT, 3, 1, 5);
    
    config.createTemplate(derivedTemplate);
    
    // Apply inheritance
    config.applyInheritance("derived_template", "base_template");
}

void resolveConfiguration() {
    AdvancedConfig& config = AdvancedConfig::getInstance();
    
    // Resolve complete configuration from template
    nlm::Config resolvedConfig = config.resolveInheritance("derived_template");
    
    // Use resolved configuration
    int neuronCount = resolvedConfig.get<int>("brain.neuron_count");
    int optimizationLevel = resolvedConfig.get<int>("performance.optimization_level");
}
```

### Configuration Validation

```cpp
#include "src/agent/AdvancedConfig.hpp"

void setupValidationRules() {
    AdvancedConfig& config = AdvancedConfig::getInstance();
    
    // Create validation rule for neuron count
    nlm::AdvancedValidationRule neuronCountRule;
    neuronCountRule.ruleId = "neuron_count_validation";
    neuronCountRule.description = "Validates neuron count is within acceptable range";
    neuronCountRule.level = nlm::ConfigValidationLevel::Strict;
    neuronCountRule.isCrossValidation = false;
    neuronCountRule.severity = "error";
    
    // Set validator function
    neuronCountRule.validator = [](const nlm::Config& config, const std::string& key, 
                                   const nlm::ConfigValue& value) {
        if (key == "brain.neuron_count") {
            int count = std::get<int>(value);
            return count >= 100 && count <= 100000;
        }
        return true;
    };
    
    neuronCountRule.errorMessage = "Neuron count must be between 100 and 100,000";
    
    config.addValidationRule(neuronCountRule);
}

bool validateConfiguration(const nlm::Config& config) {
    AdvancedConfig& advancedConfig = AdvancedConfig::getInstance();
    
    return advancedConfig.validateConfig(config, nlm::ConfigValidationLevel::Strict);
}
```

### Runtime Configuration Modification

```cpp
#include "src/agent/AdvancedConfig.hpp"

void modifyRuntimeConfiguration() {
    AdvancedConfig& config = AdvancedConfig::getInstance();
    
    // Modify configuration at runtime
    config.setValue("brain.neuron_count", 3000, "User requested increase");
    config.setValue("performance.optimization_level", 7, "Enable higher optimization");
    
    // Get change history
    auto changes = config.getRecentChanges(10);
    
    for (const auto& change : changes) {
        std::cout << "Changed " << change.key << " from " << change.oldValue 
                  << " to " << change.newValue << std::endl;
    }
}

bool rollbackConfiguration(const nlm::AdvancedConfig::ConfigChange& change) {
    AdvancedConfig& config = AdvancedConfig::getInstance();
    
    return config.rollbackToChange(change);
}
```

### Configuration Scripting

```cpp
#include "src/agent/AdvancedConfig.hpp"

void setupConfigurationScripts() {
    AdvancedConfig& config = AdvancedConfig::getInstance();
    
    // Create configuration script
    nlm::ConfigScript script;
    script.name = "performance_optimizer";
    script.description = "Optimizes configuration for performance";
    script.language = "expression";
    script.code = "if (brain.neuron_count > 1000) { performance.optimization_level = 5; } else { performance.optimization_level = 2; }";
    script.enabled = true;
    script.triggers.push_back("brain.neuron_count");
    
    config.addScript(script);
}

void executeConfigurationScript() {
    AdvancedConfig& config = AdvancedConfig::getInstance();
    
    // Create base configuration
    nlm::Config baseConfig;
    baseConfig.setValue("brain.neuron_count", 2000);
    
    // Execute script
    nlm::Config finalConfig;
    if (config.executeScript("performance_optimizer", baseConfig, finalConfig)) {
        std::cout << "Script executed successfully!" << std::endl;
        
        // Get optimized values
        int optimizationLevel = finalConfig.get<int>("performance.optimization_level");
    }
}
```

### Configuration File I/O

```cpp
#include "src/agent/AdvancedConfig.hpp"
#include <fstream>

void saveConfigurationToFile() {
    AdvancedConfig& config = AdvancedConfig::getInstance();
    
    // Save current configuration
    std::string configFile = "current_config.json";
    if (config.saveToFile(configFile)) {
        std::cout << "Configuration saved to " << configFile << std::endl;
    }
}

void loadConfigurationFromFile() {
    AdvancedConfig& config = AdvancedConfig::getInstance();
    
    // Load configuration from file
    std::string configFile = "current_config.json";
    if (config.loadFromFile(configFile)) {
        std::cout << "Configuration loaded from " << configFile << std::endl;
        
        // Verify configuration
        if (config.validateConfig()) {
            std::cout << "Configuration is valid!" << std::endl;
        } else {
            std::cout << "Configuration validation failed!" << std::endl;
        }
    }
}

void loadConfigurationFromJson() {
    AdvancedConfig& config = AdvancedConfig::getInstance();
    
    // Load configuration from JSON string
    std::string jsonString = R"({"brain.neuron_count": 1500, "performance.optimization_level": 3})";
    
    if (config.loadFromJson(jsonString)) {
        std::cout << "Configuration loaded from JSON!" << std::endl;
    }
}
```

### Configuration Export/Import

```cpp
#include "src/agent/AdvancedConfig.hpp"

void exportTemplate() {
    AdvancedConfig& config = AdvancedConfig::getInstance();
    
    // Export current configuration as template
    std::string templateFile = "my_template.json";
    if (config.createTemplateFromConfig("my_template", config.getCurrentConfig(), 
                                        "My custom configuration template")) {
        std::cout << "Template exported to " << templateFile << std::endl;
    }
}

void importTemplate() {
    AdvancedConfig& config = AdvancedConfig::getInstance();
    
    // Import configuration as template
    nlm::Config importedConfig;
    importedConfig.setValue("brain.neuron_count", 2000);
    importedConfig.setValue("performance.optimization_level", 5);
    
    if (config.importTemplate("imported_template", importedConfig)) {
        std::cout << "Template imported successfully!" << std::endl;
    }
}
```

## Configuration Templates

### Built-in Templates

The system includes several built-in templates:

1. **default** - Basic configuration with all features enabled
2. **high_performance** - Optimized for maximum performance
3. **memory_optimized** - Optimized for memory usage
4. **cpu_intensive** - Optimized for CPU-bound workloads
5. **balanced** - Balanced configuration for general use
6. **quick_test** - Minimal configuration for testing
7. **production** - Production-ready configuration
8. **debug** - Debug-focused configuration

### Template Structure

Each template includes:

- **name**: Template identifier
- **description**: Human-readable description
- **schema**: Configuration schema with validation rules
- **defaultValues**: Default configuration values
- **inheritanceRules**: Template inheritance rules
- **isAbstract**: Whether template is abstract
- **tags**: Template categorization tags
- **version**: Template version

### Template Examples

#### High Performance Template

```json
{
    "name": "high_performance",
    "description": "Configuration optimized for maximum performance",
    "version": "2.0",
    "schema": [
        {
            "key": "performance.optimization_level",
            "type": "integer",
            "default": 8,
            "min": 1,
            "max": 10,
            "required": false,
            "description": "Performance optimization level"
        },
        {
            "key": "neural_network.simultaneous_updates",
            "type": "boolean",
            "default": true,
            "required": false,
            "description": "Enable simultaneous neural network updates"
        }
    ],
    "inheritance_rules": [],
    "tags": ["performance", "production"],
    "isAbstract": false
}
```

#### Memory Optimized Template

```json
{
    "name": "memory_optimized",
    "description": "Configuration optimized for memory usage",
    "version": "1.0",
    "schema": [
        {
            "key": "memory.pool_size",
            "type": "integer",
            "default": 512,
            "min": 256,
            "max": 2048,
            "required": false,
            "description": "Memory pool size in KB"
        },
        {
            "key": "brain.synapse_density",
            "type": "double",
            "default": 0.05,
            "min": 0.01,
            "max": 0.2,
            "required": false,
            "description": "Synapse density (lower = less memory)"
        }
    ],
    "inheritance_rules": [],
    "tags": ["memory", "production"],
    "isAbstract": false
}
```

## Configuration Validation

### Validation Rules

Configuration validation includes:

1. **Type validation**: Ensure values match expected types
2. **Range validation**: Check values are within acceptable ranges
3. **Dependency validation**: Validate relationships between values
4. **Cross-validation**: Validate rules that depend on multiple values
5. **Custom validation**: User-defined validation functions

### Validation Levels

- **None**: No validation
- **Basic**: Basic type and range validation
- **Strict**: Comprehensive validation including dependencies
- **Comprehensive**: All validation rules including custom validation

### Validation Examples

```cpp
// Basic validation
config.addValidationRule({
    "rule_id": "neuron_count_basic",
    "description": "Validates neuron count is positive",
    "level": ConfigValidationLevel::Basic,
    "validator": [](const Config& config, const std::string& key, const ConfigValue& value) {
        if (key == "brain.neuron_count") {
            return std::get<int>(value) > 0;
        }
        return true;
    }
});

// Strict validation with cross-validation
config.addValidationRule({
    "rule_id": "neuron_memory_ratio",
    "description": "Validates neuron count to memory ratio",
    "level": ConfigValidationLevel::Strict,
    "isCrossValidation": true,
    "dependentKeys": ["brain.neuron_count", "memory.pool_size"],
    "validator": [](const Config& config, const std::string& key, const ConfigValue& value) {
        int neuronCount = config.get<int>("brain.neuron_count");
        int poolSize = config.get<int>("memory.pool_size");
        
        // Ensure ratio is reasonable (1 neuron per 10 memory units)
        return (double)neuronCount / poolSize < 10.0;
    }
});
```

## Configuration Inheritance

### Inheritance Types

Configuration inheritance supports:

1. **Override**: Child completely overrides parent value
2. **Merge**: Child merges with parent values
3. **Extend**: Child extends parent with additional values
4. **Substitute**: Child completely replaces parent value

### Inheritance Examples

```cpp
// Template inheritance
config.applyInheritance("derived_template", "base_template");
config.applyInheritanceChain("final_template", {"base1", "base2", "base3"});

// Configuration inheritance
nlm::Config childConfig;
childConfig.setValue("brain.neuron_count", 2000);

nlm::Config parentConfig;
parentConfig.setValue("brain.neuron_count", 1000);
parentConfig.setValue("performance.optimization_level", 3);

// Resolve inheritance
nlm::Config resolved = config.resolveInheritance("child_template");
```

## Configuration Scripting

### Script Languages

Configuration scripting supports:

1. **Expression**: Simple expression language
2. **Lua**: Full Lua scripting
3. **Python**: Full Python scripting
4. **JavaScript**: Full JavaScript scripting

### Script Examples

#### Expression Language

```cpp
// Expression script for automatic optimization
nlm::ConfigScript script;
script.name = "auto_optimize";
script.language = "expression";
script.code = "if (brain.neuron_count > 5000) { performance.optimization_level = 5; } else { performance.optimization_level = 2; }";
script.triggers.push_back("brain.neuron_count");
```

#### Lua Script

```cpp
// Lua script for complex configuration logic
nlm::ConfigScript script;
script.name = "complex_config";
script.language = "lua";
script.code = R"(
    if config.brain.neuron_count > 10000 then
        config.performance.optimization_level = 8
        config.memory.pool_size = 4096
    elseif config.brain.neuron_count > 5000 then
        config.performance.optimization_level = 5
        config.memory.pool_size = 2048
    else
        config.performance.optimization_level = 2
        config.memory.pool_size = 1024
    end
    
    config.neural_network.simultaneous_updates = true
    config.neural_network.learning_rate = 0.001
)";
script.triggers.push_back("brain.neuron_count");
```

## Configuration Commands

### Command Line Interface

The configuration system includes a command-line interface:

```bash
# List available templates
./advanced_config_management.sh --list-templates

# Create new template
./advanced_config_management.sh --create-template my_template

# Load template to file
./advanced_config_management.sh --load-template my_template config.json

# Export configuration as template
./advanced_config_management.sh --export-template config.json my_template.json

# Validate configuration
./advanced_config_management.sh --validate-config production_config.json

# Monitor configuration changes
./advanced_config_management.sh --monitor-changes

# List recent changes
./advanced_config_management.sh --list-changes

# Execute configuration script
./advanced_config_management.sh --script-execute config_script.py
```

## Configuration API Reference

### Class: AdvancedConfig

| Method | Description |
|--------|-------------|
| `createTemplate(template)` | Create new configuration template |
| `updateTemplate(template)` | Update existing template |
| `deleteTemplate(name)` | Delete template |
| `loadTemplate(name, outConfig)` | Load configuration from template |
| `getAllTemplates()` | Get all templates |
| `getTemplate(name)` | Get template by name |
| `applyInheritance(target, source)` | Apply template inheritance |
| `resolveInheritance(name)` | Resolve configuration from template |
| `addValidationRule(rule)` | Add validation rule |
| `removeValidationRule(id)` | Remove validation rule |
| `updateValidationRule(rule)` | Update validation rule |
| `getValidationRules()` | Get all validation rules |
| `validateConfig(config, level)` | Validate configuration |
| `setValue(key, value, description)` | Set configuration value |
| `get(key)` | Get configuration value |
| `has(key)` | Check if key exists |
| `getKeys()` | Get all keys |
| `loadFromFile(filepath, templateName)` | Load configuration from file |
| `saveToFile(filepath)` | Save configuration to file |
| `loadFromJson(jsonString, templateName)` | Load configuration from JSON |
| `toJson()` | Convert configuration to JSON |
| `getSection(prefix)` | Get configuration section |
| `rollbackToChange(change)` | Rollback to specific change |
| `getRecentChanges(maxChanges)` | Get recent configuration changes |
| `startSession(sessionId)` | Start new configuration session |
| `endSession(sessionId)` | End configuration session |

### Struct: ConfigTemplate

| Field | Type | Description |
|-------|------|-------------|
| `name` | std::string | Template name |
| `description` | std::string | Template description |
| `schema` | std::vector<ConfigSchemaEntry> | Configuration schema |
| `defaultValues` | Config | Default configuration values |
| `inheritanceRules` | Config | Inheritance rules |
| `baseTemplate` | std::string | Base template name |
| `isAbstract` | bool | Whether template is abstract |
| `tags` | std::vector<std::string> | Template tags |
| `version` | std::string | Template version |

### Struct: AdvancedValidationRule

| Field | Type | Description |
|-------|------|-------------|
| `ruleId` | std::string | Rule identifier |
| `description` | std::string | Rule description |
| `level` | ConfigValidationLevel | Validation level |
| `validator` | std::function | Validation function |
| `errorMessage` | std::string | Error message |
| `isCrossValidation` | bool | Whether cross-validation |
| `dependentKeys` | std::vector<std::string> | Dependent keys |
| `severity` | std::string | Error severity |

### Struct: ConfigScript

| Field | Type | Description |
|-------|------|-------------|
| `name` | std::string | Script name |
| `description` | std::string | Script description |
| `language` | std::string | Script language |
| `code` | std::string | Script code |
| `enabled` | bool | Whether script is enabled |
| `triggers` | std::vector<std::string> | Script triggers |
| `variables` | std::map<std::string, std::string> | Script variables |

## Configuration Performance

### Memory Usage

- **Configuration data**: ~512KB
- **Templates**: ~1MB per template
- **Validation rules**: ~64KB per rule
- **Change history**: ~100KB per session

### CPU Usage

- **Configuration access**: <0.1ms per operation
- **Template loading**: <1ms per template
- **Validation**: <10ms per configuration
- **Inheritance resolution**: <5ms per configuration

### Thread Safety

All configuration operations are thread-safe:

- **Template operations**: Use mutexes
- **Configuration access**: Use read-write locks
- **Validation**: Use mutexes
- **Change tracking**: Use mutexes

## Configuration Error Handling

### Error Categories

1. **Template errors**: Template creation, loading, updating
2. **Validation errors**: Configuration validation failures
3. **Inheritance errors**: Template inheritance issues
4. **Script errors**: Configuration script execution
5. **File I/O errors**: Configuration file read/write

### Error Messages

```cpp
AdvancedConfig& config = AdvancedConfig::getInstance();

if (!config.loadFromFile("config.json")) {
    std::string error = config.getLastError();
    std::cout << "Error: " << error << std::endl;
}
```

## Configuration Best Practices

### 1. Template Design

- Use descriptive names and descriptions
- Keep templates focused on specific use cases
- Use appropriate inheritance hierarchies
- Validate template schema
- Test templates thoroughly

### 2. Configuration Validation

- Implement comprehensive validation rules
- Use appropriate validation levels
- Validate dependencies between values
- Provide clear error messages
- Test validation rules

### 3. Runtime Configuration

- Use runtime configuration for user preferences
- Implement change tracking
- Provide rollback capability
- Validate runtime modifications
- Log configuration changes

### 4. Configuration Scripting

- Use appropriate script languages for tasks
- Implement error handling in scripts
- Test scripts thoroughly
- Use appropriate triggers
- Document script logic

### 5. Configuration Management

- Use templates for common configurations
- Implement inheritance for related configurations
- Validate all configuration changes
- Use change tracking for auditing
- Regularly back up configurations

## Conclusion

The Advanced Configuration system provides comprehensive configuration management capabilities for NLM. It enables:

1. **Template-based configuration** with schema validation
2. **Advanced inheritance** with multiple inheritance types
3. **Runtime configuration** with change tracking and rollback
4. **Configuration scripting** with multiple language support
5. **Comprehensive validation** with multiple validation levels
6. **Efficient file I/O** with JSON support
7. **Thread-safe operations** with proper synchronization
8. **Comprehensive error handling** with detailed error messages

This system enables NLM users to manage complex configurations efficiently while maintaining data integrity and providing flexibility for different use cases.