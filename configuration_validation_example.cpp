// Configuration Validation and Management Example
// This file demonstrates the enhanced configuration management and validation
system in NLM, showing how to use the new configuration features.

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <fstream>
#include <sstream>

// Include NLM configuration headers
#include "core/Config/Config.hpp"

namespace nlm {

// Configuration validation example
class ConfigurationValidationExample {
public:
    static void demonstrateBasicValidation() {
        std::cout << "=== Basic Configuration Validation ===" << std::endl;
        
        // Create configuration
        Config config;
        
        // Set valid values
        config.set("neuron_count", 1000);
        config.set("region_count", 2);
        config.set("learning_rate", 0.01f);
        config.set("simulation_timestep", 0.001);
        config.set("enable_learning", true);
        
        // Validate configuration
        bool is_valid = config.isValid();
        std::vector<std::string> errors = config.validate();
        
        std::cout << "Configuration is valid: " << (is_valid ? "Yes" : "No") << std::endl;
        if (!is_valid) {
            std::cout << "Validation errors:" << std::endl;
            for (const auto& error : errors) {
                std::cout << "  - " << error << std::endl;
            }
        }
        
        std::cout << std::endl;
    }
    
    static void demonstrateConfigurationMerging() {
        std::cout << "=== Configuration Merging ===" << std::endl;
        
        // Create base configuration
        Config base_config;
        base_config.set("neuron_count", 1000);
        base_config.set("region_count", 2);
        base_config.set("learning_rate", 0.01f);
        base_config.set("simulation_timestep", 0.001);
        
        // Create override configuration
        Config override_config;
        override_config.set("neuron_count", 2000);  // Override
        override_config.set("learning_rate", 0.1f);   // Override
        override_config.set("max_steps", 2000);       // New
        
        // Create environment configuration
        Config env_config;
        env_config.set("simulation_timestep", 0.01f);  // Override
        env_config.set("enable_learning", true);        // New
        
        // Merge configurations
        base_config.merge(override_config, ConfigSource::CommandLine);
        base_config.merge(env_config, ConfigSource::Runtime);
        
        std::cout << "Merged configuration:" << std::endl;
        std::cout << base_config.summary() << std::endl;
        
        std::cout << std::endl;
    }
    
    static void demonstrateConfigurationComparison() {
        std::cout << "=== Configuration Comparison ===" << std::endl;
        
        // Create two configurations
        Config config1;
        config1.set("neuron_count", 1000);
        config1.set("region_count", 2);
        config1.set("learning_rate", 0.01f);
        
        Config config2;
        config2.set("neuron_count", 1000);
        config2.set("region_count", 2);
        config2.set("learning_rate", 0.02f);  // Different
        
        // Compare configurations
        bool equal = configsEqual(config1, config2);
        double similarity = configSimilarity(config1, config2);
        
        std::cout << "Configurations are equal: " << (equal ? "Yes" : "No") << std::endl;
        std::cout << "Configuration similarity: " << similarity << std::endl;
        
        std::cout << std::endl;
    }
    
    static void demonstrateConfigurationExportImport() {
        std::cout << "=== Configuration Export/Import ===" << std::endl;
        
        // Create configuration
        Config config;
        config.set("neuron_count", 1000);
        config.set("region_count", 2);
        config.set("learning_rate", 0.01f);
        config.set("simulation_timestep", 0.001);
        config.set("enable_learning", true);
        
        // Export to JSON
        std::string json = config.toJSON();
        std::cout << "Configuration as JSON:" << std::endl;
        std::cout << json << std::endl;
        
        // Import from JSON
        std::shared_ptr<Config> imported_config = std::make_shared<Config>();
        bool import_success = imported_config->loadFromString(json, ConfigSource::File);
        
        std::cout << "Import successful: " << (import_success ? "Yes" : "No") << std::endl;
        if (import_success) {
            std::cout << "Imported configuration:" << std::endl;
            std::cout << imported_config->summary() << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    static void demonstrateConfigurationSnapshots() {
        std::cout << "=== Configuration Snapshots ===" << std::endl;
        
        // Create configuration
        Config config;
        config.set("neuron_count", 1000);
        config.set("region_count", 2);
        
        // Create snapshots
        std::string snapshot1 = config.createSnapshot("initial");
        config.set("neuron_count", 2000);
        std::string snapshot2 = config.createSnapshot("modified");
        
        std::cout << "Available snapshots:" << std::endl;
        for (const auto& name : config.getSnapshotNames()) {
            std::cout << "  - " << name << std::endl;
        }
        
        // Restore snapshot
        bool restore_success = config.restoreSnapshot("initial");
        std::cout << "Restored snapshot 'initial': " << (restore_success ? "Yes" : "No") << std::endl;
        
        if (restore_success) {
            std::cout << "Configuration after restore:" << std::endl;
            std::cout << config.summary() << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    static void demonstrateConfigurationMonitoring() {
        std::cout << "=== Configuration Monitoring ===" << std::endl;
        
        // Create configuration
        Config config;
        config.set("neuron_count", 1000);
        config.set("region_count", 2);
        
        // Set change callback
        auto change_callback = [](const std::string& key, const ConfigValue& value) {
            std::cout << "Configuration changed: " << key << std::endl;
        };
        config.setChangeCallback(change_callback);
        
        // Modify configuration
        config.set("neuron_count", 2000);
        
        // Get configuration statistics
        ConfigStats stats = config.getStats();
        std::cout << "Configuration statistics:" << std::endl;
        std::cout << "  Total entries: " << stats.totalEntries << std::endl;
        std::cout << "  File entries: " << stats.fileEntries << std::endl;
        std::cout << "  Command line entries: " << stats.commandLineEntries << std::endl;
        std::cout << "  Runtime entries: " << stats.runtimeEntries << std::endl;
        std::cout << "  Unique keys: " << stats.uniqueKeys.size() << std::endl;
        
        std::cout << std::endl;
    }
};

// Configuration type conversion example
class ConfigurationTypeConversionExample {
public:
    static void demonstrateTypeConversion() {
        std::cout << "=== Configuration Type Conversion ===" << std::endl;
        
        // Create configuration with different types
        Config config;
        config.set("int_value", 42);
        config.set("float_value", 3.14f);
        config.set("string_value", std::string("hello"));
        config.set("bool_value", true);
        config.set("vector_int", std::vector<int>{1, 2, 3, 4, 5});
        
        // Convert between types
        auto int_val = config.getAsOr<int>("int_value", 0);
        auto float_val = config.getAsOr<double>("int_value", 0.0);
        auto string_val = config.getAsOr<std::string>("string_value", "default");
        auto bool_val = config.getAsOr<bool>("bool_value", false);
        
        std::cout << "Type conversion examples:" << std::endl;
        std::cout << "  int_value as int: " << int_val << std::endl;
        std::cout << "  int_value as float: " << float_val << std::endl;
        std::cout << "  string_value as string: " << string_val << std::endl;
        std::cout << "  bool_value as bool: " << (bool_val ? "true" : "false") << std::endl;
        
        std::cout << std::endl;
    }
};

// Configuration template example
class ConfigurationTemplateExample {
public:
    static void demonstrateTemplateUsage() {
        std::cout << "=== Configuration Templates ===" << std::endl;
        
        // Create configuration
        Config config;
        
        // Check if template exists
        bool has_template = config.hasTemplate("research");
        std::cout << "Has 'research' template: " << (has_template ? "Yes" : "No") << std::endl;
        
        if (has_template) {
            // Apply template
            bool apply_success = config.applyTemplate("research");
            std::cout << "Applied template 'research': " << (apply_success ? "Yes" : "No") << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    static void demonstrateAvailableTemplates() {
        std::cout << "=== Available Templates ===" << std::endl;
        
        std::vector<std::string> templates = getAvailableConfigTemplates();
        for (const auto& template_name : templates) {
            std::cout << "  - " << template_name << std::endl;
        }
        
        std::cout << std::endl;
    }
};

// Main demonstration function
void runConfigurationValidationExample() {
    std::cout << "=== NLM Configuration Validation and Management Example ===" << std::endl;
    std::cout << std::endl;
    
    ConfigurationValidationExample::demonstrateBasicValidation();
    ConfigurationValidationExample::demonstrateConfigurationMerging();
    ConfigurationValidationExample::demonstrateConfigurationComparison();
    ConfigurationValidationExample::demonstrateConfigurationExportImport();
    ConfigurationValidationExample::demonstrateConfigurationSnapshots();
    ConfigurationValidationExample::demonstrateConfigurationMonitoring();
    
    ConfigurationTypeConversionExample::demonstrateTypeConversion();
    
    ConfigurationTemplateExample::demonstrateTemplateUsage();
    ConfigurationTemplateExample::demonstrateAvailableTemplates();
    
    std::cout << "=== Configuration Validation Example Completed ===" << std::endl;
}

} // namespace nlm

// Global function for configuration validation example
void run_configuration_validation_example() {
    nlm::runConfigurationValidationExample();
}

// Entry point
int main() {
    run_configuration_validation_example();
    return 0;
}