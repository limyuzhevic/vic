# Configuration Examples

This file demonstrates various aspects of the enhanced NLM configuration system.

## 1. Basic Configuration Usage

```cpp
#include "core/Config/Config.hpp"

int main() {
    // Create configuration from default
    nlm::Config config = nlm::Config::fromEnvironment("development");
    
    // Set basic values
    config.set("brain.neuron_count", 1024);
    config.set("brain.synapse_density", 0.15);
    config.set("brain.threshold", -50.0);
    config.set("simulation.enabled", true);
    config.set("simulation.max_steps", 1000);
    
    // Get values with type safety
    if (auto count = config.get<int>("brain.neuron_count")) {
        std::cout << "Neuron count: " << *count << std::endl;
    }
    
    // Get with default value
    int steps = config.getOr("simulation.max_steps", 1000);
    std::cout << "Max steps: " << steps << std::endl;
    
    // Save to file
    config.saveToFile("config_development.json");
    
    return 0;
}
```

## 2. Nested Configuration

```cpp
#include "core/Config/Config.hpp"

void demonstrateNestedConfig() {
    nlm::Config config;
    
    // Set nested configuration
    config.set({"brain", "neuron_count"}, 2000);
    config.set({"brain", "synapse", "density"}, 0.2);
    config.set({"brain", "plasticity", "stdp", "enable"}, true);
    config.set({"brain", "plasticity", "stdp", "learning_rate"}, 0.001);
    config.set({"brain", "neuromodulation", "dopamine", "scale"}, 1.5);
    config.set({"brain", "neuromodulation", "curiosity", "enable"}, true);
    config.set({"brain", "development", "enable"}, true);
    
    // Retrieve nested values
    if (auto stdpEnable = config.get<bool>({"brain", "plasticity", "stdp", "enable"})) {
        std::cout << "STDP enabled: " << (*stdpEnable ? "yes" : "no") << std::endl;
    }
    
    if (auto dopamineScale = config.get<double>({"brain", "neuromodulation", "dopamine", "scale"})) {
        std::cout << "Dopamine scale: " << *dopamineScale << std::endl;
    }
    
    // Get all keys
    auto keys = config.getAllKeys();
    std::cout << "All configuration keys:" << std::endl;
    for (const auto& key : keys) {
        std::cout << "  " << key << std::endl;
    }
}
```

## 3. Validation Examples

```cpp
#include "core/Config/Config.hpp"

void demonstrateValidation() {
    nlm::Config config;
    
    // Set up validation rules
    config.addValidator("brain.neuron_count", 
        [](const std::string& key, const nlm::ConfigValue& value) {
            if (auto val = std::get_if<int>(&value)) {
                if (*val < 100 || *val > 10000) {
                    return nlm::ValidationResult::error(
                        "Neuron count must be between 100 and 10000", key);
                }
            }
            return nlm::ValidationResult::success();
        }
    );
    
    config.addValidator("brain.synapse_density", 
        [](const std::string& key, const nlm::ConfigValue& value) {
            if (auto val = std::get_if<double>(&value)) {
                if (*val < 0.0 || *val > 1.0) {
                    return nlm::ValidationResult::error(
                        "Synapse density must be between 0.0 and 1.0", key);
                }
            }
            return nlm::ValidationResult::success();
        }
    );
    
    config.addValidator("learning_rate", 
        [](const std::string& key, const nlm::ConfigValue& value) {
            if (auto val = std::get_if<double>(&value)) {
                if (*val <= 0.0) {
                    return nlm::ValidationResult::error(
                        "Learning rate must be positive", key);
                }
            }
            return nlm::ValidationResult::success();
        }
    );
    
    // Set valid values
    config.set("brain.neuron_count", 1000);
    config.set("brain.synapse_density", 0.15);
    config.set("learning_rate", 0.001);
    
    // Set invalid value
    config.set("brain.neuron_count", 50);
    
    // Validate configuration
    auto result = config.validate();
    if (result.valid) {
        std::cout << "Configuration is valid!" << std::endl;
    } else {
        std::cout << "Validation error: " << result.error << std::endl;
    }
}
```

## 4. Schema Validation Examples

```cpp
#include "core/Config/Config.hpp"

void demonstrateSchemaValidation() {
    nlm::Config config;
    
    // Create JSON schema
    nlohmann::json schema = {
        {"type", "object"},
        {"properties", {
            {"brain.neuron_count", {
                {"type", "integer"},
                {"minimum", 100},
                {"maximum", 10000},
                {"description", "Total number of neurons"}
            }},
            {"brain.synapse_density", {
                {"type", "number"},
                {"minimum", 0.0},
                {"maximum", 1.0},
                {"description", "Synapse connectivity density"}
            }},
            {"brain.threshold", {
                {"type", "number"},
                {"minimum", -100.0},
                {"maximum", 0.0},
                {"description", "Neuron firing threshold"}
            }},
            {"simulation.enabled", {
                {"type", "boolean"},
                {"description", "Enable simulation"}
            }},
            {"simulation.max_steps", {
                {"type", "integer"},
                {"minimum", 1},
                {"maximum", 100000},
                {"description", "Maximum simulation steps"}
            }}
        }},
        {"required", {"brain.neuron_count", "brain.synapse_density", "simulation.enabled"}},
        {"description", "Brain configuration schema"}
    };
    
    // Set schema
    config.setSchema(schema);
    
    // Set values
    config.set("brain.neuron_count", 2000);
    config.set("brain.synapse_density", 0.1);
    config.set("brain.threshold", -60.0);
    config.set("simulation.enabled", true);
    config.set("simulation.max_steps", 5000);
    
    // Validate schema
    auto result = config.validateSchema();
    if (result.valid) {
        std::cout << "Schema validation passed!" << std::endl;
    } else {
        std::cout << "Schema validation failed: " << result.error << std::endl;
    }
}
```

## 5. Environment-Specific Configuration

```cpp
#include "core/Config/Config.hpp"

void demonstrateEnvironmentConfig() {
    // Create environment configurations
    nlm::EnvironmentConfig devEnv;
    devEnv.name = "development";
    devEnv.file = "config_development.json";
    devEnv.isDevelopment = true;
    devEnv.overrides = {
        {"brain.neuron_count", 512},
        {"brain.synapse_density", 0.2},
        {"simulation.debug", true},
        {"logging.level", "debug"}
    };
    
    nlm::EnvironmentConfig prodEnv;
    prodEnv.name = "production";
    prodEnv.file = "config_production.json";
    prodEnv.isDevelopment = false;
    prodEnv.overrides = {
        {"brain.neuron_count", 10000},
        {"brain.synapse_density", 0.05},
        {"simulation.debug", false},
        {"logging.level", "info"},
        {"performance.optimization", true}
    };
    
    // Set up environment configs (in real application, this would be done once)
    nlm::Config::addEnvironmentConfig(devEnv);
    nlm::Config::addEnvironmentConfig(prodEnv);
    
    // Create configuration for production environment
    nlm::Config config = nlm::Config::fromEnvironment("production");
    
    // Or switch environments dynamically
    config.setEnvironment("development");
    std::cout << "Switched to development environment" << std::endl;
    
    // Check if it's a development environment
    if (config.isDevelopment()) {
        std::cout << "Running in development mode with debug features" << std::endl;
    }
}
```

## 6. Value Interpolation

```cpp
#include "core/Config/Config.hpp"

void demonstrateInterpolation() {
    nlm::Config config;
    
    // Enable interpolation
    config.enableInterpolation(true);
    
    // Set base values
    config.set("brain.base_neurons", 1000);
    config.set("brain.scale_factor", 1.5);
    
    // Add interpolation references
    config.addInterpolationReference("brain.neuron_count", "brain.base_neurons");
    config.addInterpolationReference("brain.effective_threshold", "brain.threshold");
    
    // Set threshold with interpolation
    config.set("brain.threshold", -50.0);
    
    // Set direct value
    config.set("brain.neuron_count", "${brain.base_neurons}");
    config.set("brain.effective_threshold", "${brain.threshold}");
    
    // Resolve interpolations
    bool success = config.resolveInterpolations();
    if (success) {
        std::cout << "Interpolation resolved successfully!" << std::endl;
        
        // Get interpolated values
        if (auto neurons = config.get<int>("brain.neuron_count")) {
            std::cout << "Neuron count: " << *neurons << std::endl;
        }
        
        if (auto threshold = config.get<double>("brain.effective_threshold")) {
            std::cout << "Effective threshold: " << *threshold << std::endl;
        }
    } else {
        std::cout << "Interpolation failed!" << std::endl;
    }
}
```

## 7. Template Configuration

```cpp
#include "core/Config/Config.hpp"

void demonstrateTemplates() {
    // Create a configuration template
    nlm::ConfigTemplate defaultTemplate;
    defaultTemplate.name = "default";
    defaultTemplate.source = nlm::ConfigSource::Default;
    defaultTemplate.required = true;
    defaultTemplate.defaults = {
        {"brain.neuron_count", 1000},
        {"brain.synapse_density", 0.1},
        {"brain.threshold", -55.0},
        {"brain.tau_mem", 20.0},
        {"brain.tau_ref", 2.0},
        {"plasticity.stdp.enable", true},
        {"plasticity.stdp.learning_rate", 0.001},
        {"neuromod.dopamine.scale", 1.0},
        {"simulation.enabled", true},
        {"simulation.max_steps", 1000}
    };
    
    // Create another template
    nlm::ConfigTemplate fastTemplate;
    fastTemplate.name = "fast";
    fastTemplate.source = nlm::ConfigSource::File;
    fastTemplate.required = false;
    fastTemplate.defaults = {
        {"brain.neuron_count", 500},
        {"brain.synapse_density", 0.05},
        {"brain.threshold", -60.0},
        {"plasticity.stdp.learning_rate", 0.01},
        {"simulation.max_steps", 100}
    };
    
    // Set up templates (in real application, this would be done once)
    nlm::Config::addTemplate(defaultTemplate);
    nlm::Config::addTemplate(fastTemplate);
    
    // Create configuration from template
    nlm::Config config = nlm::Config::fromTemplate("default");
    
    // Apply a different template
    config.applyTemplate("fast");
    
    std::cout << "Applied fast template" << std::endl;
    
    // Modify and save
    config.set("simulation.max_steps", 200);
    config.saveToFile("fast_config.json");
}
```

## 8. File I/O Examples

```cpp
#include "core/Config/Config.hpp"

void demonstrateFileIO() {
    // Create configuration
    nlm::Config config = nlm::Config::fromEnvironment("development");
    config.set("brain.neuron_count", 2000);
    config.set("brain.synapse_density", 0.15);
    config.set("simulation.enabled", true);
    
    // Save as JSON
    bool jsonSuccess = config.saveToFile("config.json");
    if (jsonSuccess) {
        std::cout << "Configuration saved to JSON" << std::endl;
    }
    
    // Load from JSON
    nlm::Config loadedConfig = nlm::Config::fromFile("config.json");
    
    // Save as YAML
    bool yamlSuccess = config.saveToFile("config.yaml");
    if (yamlSuccess) {
        std::cout << "Configuration saved to YAML" << std::endl;
    }
    
    // Load from YAML
    nlm::Config loadedYamlConfig = nlm::Config::fromFile("config.yaml");
    
    // Load from command line arguments
    int argc = 4;
    char* argv[] = {
        (char*)"program",
        (char*)"--brain.neuron_count=1024",
        (char*)"--brain.synapse_density=0.1",
        (char*)"--simulation.max_steps=500"
    };
    
    nlm::Config cliConfig = nlm::Config::fromArgs(argc, argv);
    
    // Check file format support
    std::string testFile = "test_config.json";
    bool supported = nlm::Config::validateFileFormat(testFile);
    std::cout << "File format supported: " << (supported ? "yes" : "no") << std::endl;
}
```

## 9. Complete Example: Simulation Configuration

```cpp
#include "core/Config/Config.hpp"
#include <iostream>

class SimulationConfig {
public:
    SimulationConfig() {
        // Load base configuration
        config_ = nlm::Config::fromEnvironment("simulation");
        
        // Set up validation
        setupValidation();
        
        // Set up interpolation
        setupInterpolation();
        
        // Load environment-specific overrides
        loadEnvironmentConfig();
        
        // Apply template
        applyTemplate();
    }
    
    void setupValidation() {
        // Add validation rules
        config_.addValidator("brain.neuron_count", 
            [](const std::string& key, const nlm::ConfigValue& value) {
                if (auto val = std::get_if<int>(&value)) {
                    if (*val < 100 || *val > 100000) {
                        return nlm::ValidationResult::error(
                            "Invalid neuron count", key);
                    }
                }
                return nlm::ValidationResult::success();
            }
        );
        
        config_.addValidator("brain.synapse_density", 
            [](const std::string& key, const nlm::ConfigValue& value) {
                if (auto val = std::get_if<double>(&value)) {
                    if (*val < 0.0 || *val > 1.0) {
                        return nlm::ValidationResult::error(
                            "Invalid synapse density", key);
                    }
                }
                return nlm::ValidationResult::success();
            }
        );
    }
    
    void setupInterpolation() {
        config_.enableInterpolation(true);
        config_.addInterpolationReference("brain.effective_neuron_count", "brain.neuron_count");
        config_.addInterpolationReference("brain.effective_threshold", "brain.threshold");
    }
    
    void loadEnvironmentConfig() {
        std::string env = config_.getEnvironment();
        if (env == "development") {
            config_.set("simulation.debug", true);
            config_.set("logging.level", "debug");
        } else if (env == "production") {
            config_.set("simulation.debug", false);
            config_.set("logging.level", "info");
        }
    }
    
    void applyTemplate() {
        // Create and add template
        nlm::ConfigTemplate simTemplate;
        simTemplate.name = "simulation_default";
        simTemplate.defaults = {
            {"brain.neuron_count", 5000},
            {"brain.synapse_density", 0.1},
            {"brain.threshold", -55.0},
            {"simulation.max_steps", 10000},
            {"simulation.dt", 0.1},
            {"plasticity.stdp.enable", true}
        };
        
        config_.addTemplate(simTemplate);
        config_.applyTemplate("simulation_default");
    }
    
    void validateConfiguration() {
        auto result = config_.validate();
        if (!result.valid) {
            std::cout << "Configuration validation failed: " << result.error << std::endl;
        } else {
            std::cout << "Configuration validation passed!" << std::endl;
        }
    }
    
    void runSimulation() {
        validateConfiguration();
        
        // Resolve interpolations
        if (config_.resolveInterpolations()) {
            std::cout << "Interpolation resolved successfully" << std::endl;
        }
        
        // Save configuration
        config_.saveToFile("simulation_config.json");
        
        // Print configuration summary
        std::cout << "Simulation Configuration:" << std::endl;
        std::cout << config_.summary() << std::endl;
    }
    
private:
    nlm::Config config_;
};

int main() {
    // Run simulation with enhanced configuration
    SimulationConfig simConfig;
    simConfig.runSimulation();
    
    return 0;
}
```

## 10. Best Practices

### Configuration Management

1. **Use Environment-Specific Configurations**: Always use `fromEnvironment()` to load environment-specific configurations
2. **Apply Validation Early**: Validate configuration as soon as it's loaded
3. **Use Templates for Common Configurations**: Create templates for frequently used configurations
4. **Enable Interpolation for Dynamic Values**: Use interpolation for values that depend on other configuration settings
5. **Log Configuration Changes**: Always log important configuration changes

### Type Safety

1. **Always Use Type-Safe Getters**: Use `get<T>()` instead of direct access
2. **Provide Default Values**: Use `getOr()` for values with sensible defaults
3. **Handle Type Mismatches Gracefully**: Always check return value of `get<T>()`

### File I/O

1. **Prefer JSON for Configuration Files**: Use JSON for structured configuration
2. **Use Environment Variables for Sensitive Data**: Store sensitive configuration in environment variables
3. **Validate File Formats**: Always validate file format before loading
4. **Backup Original Configurations**: Always backup configurations before making changes

### Error Handling

1. **Validate Configuration Early**: Validate configuration immediately after loading
2. **Provide Clear Error Messages**: Use descriptive error messages in validation errors
3. **Log All Configuration Issues**: Log all configuration problems for debugging
4. **Use Exception-Safe Operations**: Ensure configuration operations are exception-safe

This comprehensive example demonstrates the full power of the enhanced NLM configuration system, covering all major features and best practices.