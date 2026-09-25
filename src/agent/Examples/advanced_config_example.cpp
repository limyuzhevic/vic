#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>

// NLM headers
#include "src/agent/AdvancedConfig.hpp"

using namespace nlm;

// Advanced configuration example
class AdvancedConfigExample {
public:
    AdvancedConfigExample() {
        std::cout << "=== NLM Advanced Configuration Example ===" << std::endl;
    }
    
    void runExample() {
        std::cout << "Running advanced configuration example..." << std::endl;
        
        // Initialize advanced configuration system
        AdvancedConfig& advancedConfig = AdvancedConfig::getInstance();
        
        // Step 1: Create configuration templates
        createTemplates(advancedConfig);
        
        // Step 2: Load and use templates
        loadAndUseTemplates(advancedConfig);
        
        // Step 3: Configure inheritance
        configureInheritance(advancedConfig);
        
        // Step 4: Add validation rules
        addValidationRules(advancedConfig);
        
        // Step 5: Modify configuration at runtime
        modifyRuntimeConfiguration(advancedConfig);
        
        // Step 6: Save and load configuration
        saveAndLoadConfiguration(advancedConfig);
        
        // Step 7: Execute configuration scripts
        executeConfigurationScripts(advancedConfig);
        
        // Step 8: Export/import templates
        exportImportTemplates(advancedConfig);
        
        std::cout << "Advanced configuration example completed successfully!" << std::endl;
    }
    
private:
    void createTemplates(AdvancedConfig& config) {
        std::cout << "\n1. Creating configuration templates..." << std::endl;
        
        // Create high-performance template
        ConfigTemplate perfTemplate;
        perfTemplate.name = "high_performance";
        perfTemplate.description = "Configuration optimized for high-performance computing";
        perfTemplate.version = "2.0";
        perfTemplate.isAbstract = false;
        
        // Add schema entries
        std::vector<ConfigSchemaEntry> schema;
        
        // Neuron configuration
        ConfigSchemaEntry neuronCount;
        neuronCount.key = "brain.neuron_count";
        neuronCount.type = "int";
        neuronCount.defaultValue = 2000;
        neuronCount.minValue = 100;
        neuronCount.maxValue = 100000;
        neuronCount.required = false;
        neuronCount.description = "Total number of neurons in the brain";
        schema.push_back(neuronCount);
        
        // Synapse configuration
        ConfigSchemaEntry synapseDensity;
        synapseDensity.key = "brain.synapse_density";
        synapseDensity.type = "double";
        synapseDensity.defaultValue = 0.1;
        synapseDensity.minValue = 0.01;
        synapseDensity.maxValue = 1.0;
        synapseDensity.required = false;
        synapseDensity.description = "Synapse density (connectivity between neurons)";
        schema.push_back(synapseDensity);
        
        // Performance optimization
        ConfigSchemaEntry optimizationLevel;
        optimizationLevel.key = "performance.optimization_level";
        optimizationLevel.type = "int";
        optimizationLevel.defaultValue = 8;
        optimizationLevel.minValue = 1;
        optimizationLevel.maxValue = 10;
        optimizationLevel.required = false;
        optimizationLevel.description = "Performance optimization level (1-10)";
        schema.push_back(optimizationLevel);
        
        // Memory configuration
        ConfigSchemaEntry memoryPoolSize;
        memoryPoolSize.key = "memory.pool_size";
        memoryPoolSize.type = "int";
        memoryPoolSize.defaultValue = 2048;
        memoryPoolSize.minValue = 256;
        memoryPoolSize.maxValue = 65536;
        memoryPoolSize.required = false;
        memoryPoolSize.description = "Memory pool size in KB";
        schema.push_back(memoryPoolSize);
        
        // Neural network configuration
        ConfigSchemaEntry simultaneousUpdates;
        simultaneousUpdates.key = "neural_network.simultaneous_updates";
        simultaneousUpdates.type = "boolean";
        simultaneousUpdates.defaultValue = true;
        simultaneousUpdates.required = false;
        simultaneousUpdates.description = "Enable simultaneous neural network updates";
        schema.push_back(simultaneousUpdates);
        
        // Learning rate
        ConfigSchemaEntry learningRate;
        learningRate.key = "neural_network.learning_rate";
        learningRate.type = "double";
        learningRate.defaultValue = 0.001;
        learningRate.minValue = 0.0001;
        learningRate.maxValue = 0.1;
        learningRate.required = false;
        learningRate.description = "Learning rate for neural network updates";
        schema.push_back(learningRate);
        
        perfTemplate.schema = schema;
        
        // Add tags
        perfTemplate.tags = {"performance", "production", "high-speed"};
        
        // Create template
        if (config.createTemplate(perfTemplate)) {
            std::cout << "  Created 'high_performance' template successfully!" << std::endl;
        } else {
            std::cout << "  Failed to create 'high_performance' template!" << std::endl;
        }
        
        // Create memory-optimized template
        ConfigTemplate memoryTemplate;
        memoryTemplate.name = "memory_optimized";
        memoryTemplate.description = "Configuration optimized for memory usage";
        memoryTemplate.version = "1.0";
        memoryTemplate.isAbstract = false;
        
        std::vector<ConfigSchemaEntry> memorySchema;
        
        // Reduced memory configuration
        ConfigSchemaEntry reducedMemory;
        reducedMemory.key = "memory.pool_size";
        reducedMemory.type = "int";
        reducedMemory.defaultValue = 512;
        reducedMemory.minValue = 128;
        reducedMemory.maxValue = 4096;
        reducedMemory.required = false;
        reducedMemory.description = "Memory pool size in KB (reduced for memory efficiency)";
        memorySchema.push_back(reducedMemory);
        
        // Low synapse density
        ConfigSchemaEntry lowSynapseDensity;
        lowSynapseDensity.key = "brain.synapse_density";
        lowSynapseDensity.type = "double";
        lowSynapseDensity.defaultValue = 0.05;
        lowSynapseDensity.minValue = 0.01;
        lowSynapseDensity.maxValue = 0.2;
        lowSynapseDensity.required = false;
        lowSynapseDensity.description = "Reduced synapse density for memory efficiency";
        memorySchema.push_back(lowSynapseDensity);
        
        memoryTemplate.schema = memorySchema;
        memoryTemplate.tags = {"memory", "production", "efficient"};
        
        if (config.createTemplate(memoryTemplate)) {
            std::cout << "  Created 'memory_optimized' template successfully!" << std::endl;
        } else {
            std::cout << "  Failed to create 'memory_optimized' template!" << std::endl;
        }
        
        // Create debug template
        ConfigTemplate debugTemplate;
        debugTemplate.name = "debug";
        debugTemplate.description = "Configuration for debugging and testing";
        debugTemplate.version = "1.0";
        debugTemplate.isAbstract = false;
        
        std::vector<ConfigSchemaEntry> debugSchema;
        
        // Debug mode
        ConfigSchemaEntry debugMode;
        debugMode.key = "debug.enabled";
        debugMode.type = "boolean";
        debugMode.defaultValue = true;
        debugMode.required = false;
        debugMode.description = "Enable debug mode";
        debugSchema.push_back(debugMode);
        
        // Log level
        ConfigSchemaEntry logLevel;
        logLevel.key = "debug.log_level";
        logLevel.type = "string";
        logLevel.defaultValue = "debug";
        logLevel.required = false;
        logLevel.description = "Log level for debugging";
        debugSchema.push_back(logLevel);
        
        debugTemplate.schema = debugSchema;
        debugTemplate.tags = {"debug", "testing", "development"};
        
        if (config.createTemplate(debugTemplate)) {
            std::cout << "  Created 'debug' template successfully!" << std::endl;
        } else {
            std::cout << "  Failed to create 'debug' template!" << std::endl;
        }
    }
    
    void loadAndUseTemplates(AdvancedConfig& config) {
        std::cout << "\n2. Loading and using templates..." << std::endl;
        
        // Load high-performance template
        Config perfConfig;
        if (config.loadTemplate("high_performance", perfConfig)) {
            std::cout << "  Loaded 'high_performance' template!" << std::endl;
            
            // Use template values
            int neuronCount = perfConfig.get<int>("brain.neuron_count");
            double synapseDensity = perfConfig.get<double>("brain.synapse_density");
            int optimizationLevel = perfConfig.get<int>("performance.optimization_level");
            int memoryPoolSize = perfConfig.get<int>("memory.pool_size");
            bool simultaneousUpdates = perfConfig.get<bool>("neural_network.simultaneous_updates");
            double learningRate = perfConfig.get<double>("neural_network.learning_rate");
            
            std::cout << "  Template values:" << std::endl;
            std::cout << "    Neurons: " << neuronCount << std::endl;
            std::cout << "    Synapse Density: " << synapseDensity << std::endl;
            std::cout << "    Optimization Level: " << optimizationLevel << std::endl;
            std::cout << "    Memory Pool Size: " << memoryPoolSize << " KB" << std::endl;
            std::cout << "    Simultaneous Updates: " << (simultaneousUpdates ? "Yes" : "No") << std::endl;
            std::cout << "    Learning Rate: " << learningRate << std::endl;
        } else {
            std::cout << "  Failed to load 'high_performance' template!" << std::endl;
        }
        
        // Load memory-optimized template
        Config memoryConfig;
        if (config.loadTemplate("memory_optimized", memoryConfig)) {
            std::cout << "  Loaded 'memory_optimized' template!" << std::endl;
            
            int memoryPoolSize = memoryConfig.get<int>("memory.pool_size");
            double synapseDensity = memoryConfig.get<double>("brain.synapse_density");
            
            std::cout << "  Memory-optimized values:" << std::endl;
            std::cout << "    Memory Pool Size: " << memoryPoolSize << " KB" << std::endl;
            std::cout << "    Synapse Density: " << synapseDensity << std::endl;
        } else {
            std::cout << "  Failed to load 'memory_optimized' template!" << std::endl;
        }
        
        // Load debug template
        Config debugConfig;
        if (config.loadTemplate("debug", debugConfig)) {
            std::cout << "  Loaded 'debug' template!" << std::endl;
            
            bool debugEnabled = debugConfig.get<bool>("debug.enabled");
            std::string logLevel = debugConfig.get<std::string>("debug.log_level");
            
            std::cout << "  Debug values:" << std::endl;
            std::cout << "    Debug Enabled: " << (debugEnabled ? "Yes" : "No") << std::endl;
            std::cout << "    Log Level: " << logLevel << std::endl;
        } else {
            std::cout << "  Failed to load 'debug' template!" << std::endl;
        }
    }
    
    void configureInheritance(AdvancedConfig& config) {
        std::cout << "\n3. Configuring inheritance..." << std::endl;
        
        // Create child template that inherits from high_performance
        ConfigTemplate childTemplate;
        childTemplate.name = "child_performance";
        childTemplate.description = "Child template that extends high_performance";
        childTemplate.version = "1.0";
        childTemplate.baseTemplate = "high_performance";
        childTemplate.isAbstract = false;
        
        std::vector<ConfigSchemaEntry> childSchema;
        
        // Override some values
        ConfigSchemaEntry customOptimization;
        customOptimization.key = "performance.optimization_level";
        customOptimization.type = "int";
        customOptimization.defaultValue = 9;
        customOptimization.minValue = 1;
        customOptimization.maxValue = 10;
        customOptimization.required = false;
        customOptimization.description = "Custom optimization level (overrides parent)";
        childSchema.push_back(customOptimization);
        
        // Add new value
        ConfigSchemaEntry customValue;
        customValue.key = "custom.parameter";
        customValue.type = "string";
        customValue.defaultValue = "custom_value";
        customValue.required = false;
        customValue.description = "Custom parameter specific to child template";
        childSchema.push_back(customValue);
        
        childTemplate.schema = childSchema;
        childTemplate.tags = {"performance", "child", "custom"};
        
        if (config.createTemplate(childTemplate)) {
            std::cout << "  Created 'child_performance' template!" << std::endl;
            
            // Apply inheritance
            if (config.applyInheritance("child_performance", "high_performance")) {
                std::cout << "  Applied inheritance from 'high_performance'!" << std::endl;
                
                // Resolve configuration
                Config resolvedConfig = config.resolveInheritance("child_performance");
                
                // Use resolved configuration
                int optimizationLevel = resolvedConfig.get<int>("performance.optimization_level");
                std::string customValue = resolvedConfig.get<std::string>("custom.parameter");
                
                std::cout << "  Resolved configuration:" << std::endl;
                std::cout << "    Optimization Level: " << optimizationLevel << std::endl;
                std::cout << "    Custom Parameter: " << customValue << std::endl;
            } else {
                std::cout << "  Failed to apply inheritance!" << std::endl;
            }
        } else {
            std::cout << "  Failed to create 'child_performance' template!" << std::endl;
        }
    }
    
    void addValidationRules(AdvancedConfig& config) {
        std::cout << "\n4. Adding validation rules..." << std::endl;
        
        // Create validation rule for neuron count
        AdvancedValidationRule neuronCountRule;
        neuronCountRule.ruleId = "neuron_count_validation";
        neuronCountRule.description = "Validates that neuron count is within acceptable range";
        neuronCountRule.level = ConfigValidationLevel::Strict;
        neuronCountRule.isCrossValidation = false;
        neuronCountRule.severity = "error";
        neuronCountRule.errorMessage = "Neuron count must be between 100 and 100,000";
        
        neuronCountRule.validator = [](const Config& config, const std::string& key, const ConfigValue& value) {
            if (key == "brain.neuron_count") {
                int count = std::get<int>(value);
                return count >= 100 && count <= 100000;
            }
            return true;
        };
        
        if (config.addValidationRule(neuronCountRule)) {
            std::cout << "  Added neuron count validation rule!" << std::endl;
        } else {
            std::cout << "  Failed to add neuron count validation rule!" << std::endl;
        }
        
        // Create validation rule for synapse density
        AdvancedValidationRule synapseDensityRule;
        synapseDensityRule.ruleId = "synapse_density_validation";
        synapseDensityRule.description = "Validates that synapse density is within acceptable range";
        synapseDensityRule.level = ConfigValidationLevel::Basic;
        synapseDensityRule.isCrossValidation = false;
        synapseDensityRule.severity = "warning";
        synapseDensityRule.errorMessage = "Synapse density should be between 0.01 and 1.0";
        
        synapseDensityRule.validator = [](const Config& config, const std::string& key, const ConfigValue& value) {
            if (key == "brain.synapse_density") {
                double density = std::get<double>(value);
                return density >= 0.01 && density <= 1.0;
            }
            return true;
        };
        
        if (config.addValidationRule(synapseDensityRule)) {
            std::cout << "  Added synapse density validation rule!" << std::endl;
        } else {
            std::cout << "  Failed to add synapse density validation rule!" << std::endl;
        }
        
        // Create cross-validation rule
        AdvancedValidationRule crossValidationRule;
        crossValidationRule.ruleId = "neuron_memory_ratio_validation";
        crossValidationRule.description = "Validates neuron count to memory ratio";
        crossValidationRule.level = ConfigValidationLevel::Strict;
        crossValidationRule.isCrossValidation = true;
        crossValidationRule.dependentKeys = {"brain.neuron_count", "memory.pool_size"};
        crossValidationRule.severity = "error";
        crossValidationRule.errorMessage = "Neuron to memory ratio should be less than 10:1";
        
        crossValidationRule.validator = [](const Config& config, const std::string& key, const ConfigValue& value) {
            if (key == "brain.neuron_count") {
                int neuronCount = std::get<int>(value);
                auto poolSizeIt = config.find("memory.pool_size");
                if (poolSizeIt != config.end()) {
                    int poolSize = std::get<int>(poolSizeIt->second);
                    return (double)neuronCount / poolSize < 10.0;
                }
            }
            return true;
        };
        
        if (config.addValidationRule(crossValidationRule)) {
            std::cout << "  Added neuron memory ratio validation rule!" << std::endl;
        } else {
            std::cout << "  Failed to add neuron memory ratio validation rule!" << std::endl;
        }
        
        // Validate configuration
        Config testConfig;
        testConfig.setValue("brain.neuron_count", 2000);
        testConfig.setValue("brain.synapse_density", 0.1);
        testConfig.setValue("memory.pool_size", 2048);
        
        bool isValid = config.validateConfig(testConfig, ConfigValidationLevel::Strict);
        std::cout << "  Validation result for test configuration: " << (isValid ? "Valid" : "Invalid") << std::endl;
    }
    
    void modifyRuntimeConfiguration(AdvancedConfig& config) {
        std::cout << "\n5. Modifying runtime configuration..." << std::endl;
        
        // Create initial configuration
        Config initialConfig;
        initialConfig.setValue("brain.neuron_count", 1000);
        initialConfig.setValue("brain.synapse_density", 0.05);
        initialConfig.setValue("performance.optimization_level", 3);
        initialConfig.setValue("memory.pool_size", 1024);
        
        // Validate initial configuration
        bool isValid = config.validateConfig(initialConfig, ConfigValidationLevel::Basic);
        std::cout << "  Initial configuration valid: " << (isValid ? "Yes" : "No") << std::endl;
        
        // Modify configuration at runtime
        std::cout << "  Modifying configuration..." << std::endl;
        
        config.setValue("brain.neuron_count", 2000, "User requested increase");
        config.setValue("performance.optimization_level", 5, "Enable higher optimization");
        config.setValue("memory.pool_size", 2048, "Increase memory pool");
        
        // Get change history
        auto changes = config.getRecentChanges(5);
        std::cout << "  Configuration changes:" << std::endl;
        for (size_t i = 0; i < changes.size(); ++i) {
            const auto& change = changes[i];
            std::cout << "    " << (i + 1) << ". " << change.key << 
                      " from " << change.oldValue << " to " << change.newValue << 
                      " (" << configChangeTypeToString(change.type) << ")" << std::endl;
        }
        
        // Rollback last change
        if (!changes.empty()) {
            std::cout << "  Rolling back last change..." << std::endl;
            config.rollbackToChange(changes.back());
            std::cout << "  Configuration rolled back successfully!" << std::endl;
        }
    }
    
    void saveAndLoadConfiguration(AdvancedConfig& config) {
        std::cout << "\n6. Saving and loading configuration..." << std::endl;
        
        // Create configuration
        Config configToSave;
        configToSave.setValue("brain.neuron_count", 1500);
        configToSave.setValue("brain.synapse_density", 0.15);
        configToSave.setValue("performance.optimization_level", 6);
        configToSave.setValue("memory.pool_size", 1536);
        configToSave.setValue("neural_network.simultaneous_updates", true);
        
        // Save configuration to file
        std::string filename = "advanced_config.json";
        if (config.saveToFile(filename)) {
            std::cout << "  Configuration saved to " << filename << "!" << std::endl;
            
            // Create backup
            std::string backupDir = "backups";
            if (config.saveToFileWithHistory(backupDir + "/backup_" + filename, "Backup configuration")) {
                std::cout << "  Backup created in " << backupDir << "!" << std::endl;
            }
        } else {
            std::cout << "  Failed to save configuration!" << std::endl;
        }
        
        // Load configuration from file
        Config loadedConfig;
        if (config.loadFromFile(filename)) {
            std::cout << "  Configuration loaded from " << filename << "!" << std::endl;
            
            // Verify loaded configuration
            if (config.validateConfig(loadedConfig, ConfigValidationLevel::Basic)) {
                std::cout << "  Loaded configuration is valid!" << std::endl;
                
                // Use loaded configuration
                int neuronCount = loadedConfig.get<int>("brain.neuron_count");
                double synapseDensity = loadedConfig.get<double>("brain.synapse_density");
                std::cout << "  Loaded values:" << std::endl;
                std::cout << "    Neurons: " << neuronCount << std::endl;
                std::cout << "    Synapse Density: " << synapseDensity << std::endl;
            } else {
                std::cout << "  Loaded configuration is invalid!" << std::endl;
            }
        } else {
            std::cout << "  Failed to load configuration!" << std::endl;
        }
        
        // Load configuration from JSON string
        std::string jsonString = R"({"brain.neuron_count": 2000, "brain.synapse_density": 0.2})";
        if (config.loadFromJson(jsonString, "high_performance")) {
            std::cout << "  Configuration loaded from JSON!" << std::endl;
            
            int neuronCount = config.get<int>("brain.neuron_count");
            double synapseDensity = config.get<double>("brain.synapse_density");
            std::cout << "  JSON values:" << std::endl;
            std::cout << "    Neurons: " << neuronCount << std::endl;
            std::cout << "    Synapse Density: " << synapseDensity << std::endl;
        } else {
            std::cout << "  Failed to load configuration from JSON!" << std::endl;
        }
    }
    
    void executeConfigurationScripts(AdvancedConfig& config) {
        std::cout << "\n7. Executing configuration scripts..." << std::endl;
        
        // Create configuration script
        ConfigScript script;
        script.name = "auto_optimizer";
        script.description = "Automatically optimizes configuration based on usage patterns";
        script.language = "expression";
        script.code = R"(
            if (brain.neuron_count > 5000) {
                performance.optimization_level = 5
                memory.pool_size = 4096
            } else if (brain.neuron_count > 2000) {
                performance.optimization_level = 3
                memory.pool_size = 2048
            } else {
                performance.optimization_level = 1
                memory.pool_size = 1024
            }
            neural_network.simultaneous_updates = true
            neural_network.learning_rate = 0.001
        )";
        script.enabled = true;
        script.triggers.push_back("brain.neuron_count");
        script.triggers.push_back("performance.optimization_level");
        
        if (config.addScript(script)) {
            std::cout << "  Created configuration script!" << std::endl;
            
            // Execute script
            Config baseConfig;
            baseConfig.setValue("brain.neuron_count", 3000);
            baseConfig.setValue("brain.synapse_density", 0.1);
            
            Config optimizedConfig;
            if (config.executeScript("auto_optimizer", baseConfig, optimizedConfig)) {
                std::cout << "  Script executed successfully!" << std::endl;
                
                int optimizationLevel = optimizedConfig.get<int>("performance.optimization_level");
                int memoryPoolSize = optimizedConfig.get<int>("memory.pool_size");
                std::cout << "  Optimized values:" << std::endl;
                std::cout << "    Optimization Level: " << optimizationLevel << std::endl;
                std::cout << "    Memory Pool Size: " << memoryPoolSize << " KB" << std::endl;
            } else {
                std::cout << "  Failed to execute script!" << std::endl;
            }
            
            // Execute all scripts
            Config allScriptsConfig = config.executeAllScripts(baseConfig);
            std::cout << "  All scripts executed successfully!" << std::endl;
        } else {
            std::cout << "  Failed to create configuration script!" << std::endl;
        }
    }
    
    void exportImportTemplates(AdvancedConfig& config) {
        std::cout << "\n8. Exporting and importing templates..." << std::endl;
        
        // Export current configuration as template
        Config currentConfig;
        currentConfig.setValue("brain.neuron_count", 1000);
        currentConfig.setValue("brain.synapse_density", 0.05);
        currentConfig.setValue("performance.optimization_level", 2);
        currentConfig.setValue("memory.pool_size", 512);
        currentConfig.setValue("neural_network.simultaneous_updates", false);
        
        std::string templateFile = "custom_template.json";
        if (config.createTemplateFromConfig("custom_template", currentConfig, "Custom configuration template")) {
            std::cout << "  Exported configuration as template to " << templateFile << "!" << std::endl;
        } else {
            std::cout << "  Failed to export configuration as template!" << std::endl;
        }
        
        // Import template
        Config importedConfig;
        importedConfig.setValue("brain.neuron_count", 2000);
        importedConfig.setValue("brain.synapse_density", 0.1);
        importedConfig.setValue("performance.optimization_level", 4);
        importedConfig.setValue("memory.pool_size", 1024);
        
        if (config.importTemplate("imported_template", importedConfig)) {
            std::cout << "  Imported template successfully!" << std::endl;
            
            Config resolvedConfig = config.resolveInheritance("imported_template");
            std::cout << "  Imported template resolved successfully!" << std::endl;
        } else {
            std::cout << "  Failed to import template!" << std::endl;
        }
        
        // Export template
        Config exportConfig;
        exportConfig.setValue("brain.neuron_count", 500);
        exportConfig.setValue("brain.synapse_density", 0.02);
        exportConfig.setValue("performance.optimization_level", 1);
        exportConfig.setValue("memory.pool_size", 256);
        
        if (config.exportTemplate("exported_template", exportConfig)) {
            std::cout << "  Template exported successfully!" << std::endl;
        } else {
            std::cout << "  Failed to export template!" << std::endl;
        }
    }
    
    std::string configChangeTypeToString(ConfigChangeType type) const {
        switch (type) {
            case ConfigChangeType::Added: return "Added";
            case ConfigChangeType::Modified: return "Modified";
            case ConfigChangeType::Removed: return "Removed";
            case ConfigChangeType::Reordered: return "Reordered";
            case ConfigChangeType::Validated: return "Validated";
            default: return "Unknown";
        }
    }
};

int main() {
    std::cout << "=== NLM Advanced Configuration Example ===" << std::endl;
    
    // Create example instance
    AdvancedConfigExample example;
    
    // Run example
    example.runExample();
    
    std::cout << "\n=== Example Complete ===" << std::endl;
    return 0;
}
