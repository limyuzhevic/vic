# NLM Configuration Guide

## Overview
This comprehensive configuration guide covers the enhanced NLM (熙然) configuration system, including JSON/YAML support, schema validation, templates, and advanced configuration management features.

## Configuration System Architecture

The NLM configuration system is built around a modular architecture with the following components:

```
Configuration System Architecture:
├── Config.hpp (Enhanced header)
│   ├── ConfigEntry (Configuration entry structure)
│   ├── ConfigSchema (Schema definition)
│   ├── ConfigHistoryEntry (History tracking)
│   └── Config (Main configuration class)
├── Config.cpp (Enhanced implementation)
│   ├── JSON/YAML parsing
│   ├── Schema validation
│   ├── Inheritance handling
│   ├── History and backup
│   │   └── Change tracking
├── ConfigCLI.hpp (CLI interface)
├── ConfigCLI.cpp (CLI implementation)
└── main.cpp (Application entry point)
```

## Configuration File Formats

### JSON Configuration

JSON is the primary configuration format for NLM. It supports:

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "log_level": "INFO",
    "log_to_file": false,
    "visualization_enabled": false,
    "performance": {
        "parallel_processing": true,
        "thread_count": 4,
        "memory_pool_size": 1024
    },
    "neuromodulation": {
        "dopamine_sensitivity": 1.0,
        "curiosity_factor": 0.5,
        "novelty_threshold": 0.3
    },
    "memory_system": {
        "working_memory_size": 100,
        "episodic_capacity": 1000,
        "associative_strength": 0.8
    }
}
```

### YAML Configuration

YAML provides a more human-readable configuration format:

```yaml
# Neural Network Configuration Template
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
region_count: 2
connection_probability: 0.1

stdp:
    ltp_weight: 0.01
    ltd_weight: 0.012
    time_constant: 20.0

logging:
    level: INFO
    to_file: false
    filename: nlm.log
    file_rotation: true
    max_file_size: 10485760  # 10MB

visualization:
    enabled: false
    update_rate: 30.0
    window_size: 800
    show_neurons: true
    show_synapses: false

performance:
    parallel_processing: true
    thread_count: 4
    vectorization: true
    memory_pool_size: 1024
    event_queue_size: 10000

neuromodulation:
    dopamine_sensitivity: 1.0
    curiosity_factor: 0.5
    novelty_threshold: 0.3
    neuromodulator_reset_time: 10.0

memory_system:
    working_memory_size: 100
    episodic_capacity: 1000
    associative_strength: 0.8
    replay_enabled: true
    consolidation_threshold: 0.8

development:
    stage_duration: 100.0
    plasticity_rate_initial: 1.0
    plasticity_rate_final: 0.1
    structural_plasticity: true
```

### Legacy Configuration Format

The original key-value configuration format is still supported for backward compatibility:

```
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
log_level = INFO
log_to_file = false
visualization_enabled = false
performance_parallel_processing = true
performance_thread_count = 4
memory_working_memory_size = 100
```

## Configuration Schema Definition

### Schema Structure

The configuration system uses JSON Schema for validation. Here's an example schema:

```json
{
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "NLM Configuration Schema",
    "description": "Configuration schema for NLM neural simulation",
    "type": "object",
    "properties": {
        "random_seed": {
            "type": "integer",
            "minimum": 0,
            "maximum": 2147483647,
            "description": "Random seed for reproducible simulations"
        },
        "simulation_timestep": {
            "type": "number",
            "minimum": 0.0001,
            "maximum": 1.0,
            "description": "Simulation timestep in seconds"
        },
        "neuron_count": {
            "type": "integer",
            "minimum": 1,
            "maximum": 1000000,
            "description": "Total number of neurons in the network"
        },
        "region_count": {
            "type": "integer",
            "minimum": 1,
            "maximum": 100,
            "description": "Number of brain regions"
        },
        "connection_probability": {
            "type": "number",
            "minimum": 0.0,
            "maximum": 1.0,
            "description": "Probability of synaptic connections"
        }
    },
    "required": ["random_seed", "simulation_timestep", "neuron_count", "region_count"],
    "additionalProperties": true
}
```

### Creating Custom Schemas

```cpp
#include "core/Config/Config.hpp"

ConfigSchema createCustomSchema() {
    ConfigSchema schema;
    
    // Define custom properties
    schema.addProperty("custom_parameter_1", 
                     ConfigSchema::PropertyType::INTEGER,
                     "Custom integer parameter",
                     0, 100, true);
    
    schema.addProperty("custom_parameter_2",
                     ConfigSchema::PropertyType::STRING,
                     "Custom string parameter",
                     "default_value",
                     false);
    
    schema.addProperty("custom_parameter_3",
                     ConfigSchema::PropertyType::NUMBER,
                     "Custom float parameter",
                     0.0, 1.0, true);
    
    return schema;
}
```

## Configuration Loading and Validation

### Loading Configuration from Files

```cpp
#include "core/Config/Config.hpp"
#include "core/utilities/ErrorHandler.hpp"

class ConfigurationManager {
public:
    bool loadConfiguration(const std::string& configFile, 
                          const std::string& schemaFile = "") {
        ErrorHandler errorHandler;
        Config config;
        
        try {
            // Load configuration from file
            ConfigFormat format = detectFileFormat(configFile);
            if (!config.loadFromFile(configFile, format)) {
                errorHandler.report(ErrorHandler::Severity::ERROR,
                                  "Failed to load configuration from " + configFile);
                return false;
            }
            
            // Load and apply schema if provided
            if (!schemaFile.empty()) {
                ConfigSchema schema;
                if (!schema.loadFromFile(schemaFile)) {
                    errorHandler.report(ErrorHandler::Severity::ERROR,
                                      "Failed to load schema from " + schemaFile);
                    return false;
                }
                
                if (!config.validate(schema)) {
                    errorHandler.report(ErrorHandler::Severity::ERROR,
                                      "Configuration validation failed");
                    return false;
                }
            }
            
            // Validate against default schema
            if (!config.validate()) {
                errorHandler.report(ErrorHandler::Severity::ERROR,
                                  "Configuration validation against default schema failed");
                return false;
            }
            
            // Apply configuration to current instance
            applyConfiguration(config);
            
            return true;
            
        } catch (const std::exception& e) {
            errorHandler.report(ErrorHandler::Severity::ERROR,
                              "Exception during configuration loading: " + std::string(e.what()));
            return false;
        }
    }
    
    ConfigFormat detectFileFormat(const std::string& filename) {
        std::string extension = filename.substr(filename.find_last_of(".") + 1);
        
        if (extension == "json") return ConfigFormat::JSON;
        if (extension == "yaml" || extension == "yml") return ConfigFormat::YAML;
        if (extension == "cfg" || extension == "config") return ConfigFormat::LEGACY;
        
        // Auto-detect based on content
        return autoDetectFormat(filename);
    }
    
private:
    void applyConfiguration(const Config& config) {
        // Apply configuration to global settings
        // This is where you would apply the loaded configuration
        // to your application components
    }
};
```

### Runtime Configuration Validation

```cpp
#include "core/Config/Config.hpp"
#include "core/utilities/ErrorHandler.hpp"

class RuntimeConfigurationValidator {
public:
    bool validateConfiguration(const Config& config) {
        ErrorHandler errorHandler;
        errorHandler.setLogLevel(ErrorHandler::Severity::WARNING);
        
        std::vector<std::string> validationErrors;
        
        // Check required parameters
        validationErrors = checkRequiredParameters(config);
        if (!validationErrors.empty()) {
            for (const auto& error : validationErrors) {
                errorHandler.report(ErrorHandler::Severity::ERROR, error);
            }
            return false;
        }
        
        // Check parameter consistency
        validationErrors = checkParameterConsistency(config);
        if (!validationErrors.empty()) {
            for (const auto& error : validationErrors) {
                errorHandler.report(ErrorHandler::Severity::WARNING, error);
            }
        }
        
        // Check performance implications
        validationErrors = checkPerformanceImplications(config);
        if (!validationErrors.empty()) {
            for (const auto& error : validationErrors) {
                errorHandler.report(ErrorHandler::Severity::INFO, error);
            }
        }
        
        return validationErrors.empty();
    }
    
    std::vector<std::string> checkRequiredParameters(const Config& config) {
        std::vector<std::string> errors;
        
        // Check for required parameters
        if (!config.hasValue("random_seed")) {
            errors.push_back("Missing required parameter: random_seed");
        }
        
        if (!config.hasValue("simulation_timestep")) {
            errors.push_back("Missing required parameter: simulation_timestep");
        }
        
        if (!config.hasValue("neuron_count")) {
            errors.push_back("Missing required parameter: neuron_count");
        }
        
        // Check consistency between parameters
        int neuronCount = config.getIntOr("neuron_count", 0);
        int regionCount = config.getIntOr("region_count", 0);
        
        if (neuronCount < regionCount) {
            errors.push_back("neuron_count must be >= region_count");
        }
        
        return errors;
    }
    
    std::vector<std::string> checkParameterConsistency(const Config& config) {
        std::vector<std::string> warnings;
        
        // Check for inconsistent parameters
        double timestep = config.getDoubleOr("simulation_timestep", 0.001);
        double neuronCount = config.getIntOr("neuron_count", 1000);
        
        if (timestep < 0.0001) {
            warnings.push_back("simulation_timestep is very small, may impact performance");
        }
        
        if (timestep > 0.1) {
            warnings.push_back("simulation_timestep is large, may impact accuracy");
        }
        
        if (neuronCount > 100000) {
            warnings.push_back("neuron_count is very large, may require significant memory");
        }
        
        return warnings;
    }
    
    std::vector<std::string> checkPerformanceImplications(const Config& config) {
        std::vector<std::string> infos;
        
        // Check performance-related parameters
        bool parallelEnabled = config.getBoolOr("performance_parallel_processing", false);
        int threadCount = config.getIntOr("performance_thread_count", 1);
        bool vectorizationEnabled = config.getBoolOr("performance_vectorization", false);
        
        if (parallelEnabled && threadCount > 8) {
            infos.push_back("High thread count may not provide significant benefits due to overhead");
        }
        
        if (vectorizationEnabled && !hasSIMDSupport()) {
            infos.push_back("Vectorization enabled but SIMD hardware not available");
        }
        
        return infos;
    }
    
    bool hasSIMDSupport() {
        // Check for SIMD support at runtime
        // This would typically check CPU capabilities
        return true; // Placeholder
    }
};
```

## Configuration Management

### Configuration Backup and Restore

```cpp
#include "core/Config/Config.hpp"
#include "core/utilities/ErrorHandler.hpp"

class ConfigurationBackupManager {
public:
    bool createBackup(const std::string& configFile, const std::string& backupName = "") {
        ErrorHandler errorHandler;
        
        try {
            Config config;
            if (!config.loadFromFile(configFile)) {
                errorHandler.report(ErrorHandler::Severity::ERROR,
                                  "Failed to load configuration for backup");
                return false;
            }
            
            // Generate backup name if not provided
            std::string backupPath = backupName;
            if (backupPath.empty()) {
                backupPath = generateBackupName(configFile);
            }
            
            // Create backup directory if it doesn't exist
            std::string backupDir = "backups/" + getTimestamp();
            std::filesystem::create_directories(backupDir);
            
            // Save configuration to backup file
            std::string backupFile = backupDir + "/config.json";
            if (!config.saveToFile(backupFile)) {
                errorHandler.report(ErrorHandler::Severity::ERROR,
                                  "Failed to save configuration backup");
                return false;
            }
            
            // Create metadata file
            std::string metadataFile = backupDir + "/metadata.json";
            saveBackupMetadata(backupDir, configFile, config);
            
            // Update backup index
            updateBackupIndex(backupDir);
            
            // Log backup creation
            errorHandler.report(ErrorHandler::Severity::INFO,
                              "Configuration backup created: " + backupFile);
            
            return true;
            
        } catch (const std::exception& e) {
            errorHandler.report(ErrorHandler::Severity::ERROR,
                              "Exception during backup creation: " + std::string(e.what()));
            return false;
        }
    }
    
    bool restoreBackup(const std::string& backupPath, const std::string& configFile = "") {
        ErrorHandler errorHandler;
        
        try {
            // Load configuration from backup
            Config backupConfig;
            std::string configFileInBackup = backupPath + "/config.json";
            
            if (!backupConfig.loadFromFile(configFileInBackup)) {
                errorHandler.report(ErrorHandler::Severity::ERROR,
                                  "Failed to load configuration from backup");
                return false;
            }
            
            // Validate backup configuration
            if (!backupConfig.validate()) {
                errorHandler.report(ErrorHandler::Severity::ERROR,
                                  "Backup configuration validation failed");
                return false;
            }
            
            // Determine target configuration file
            std::string targetFile = configFile.empty() ? "current_config.json" : configFile;
            
            // Save restored configuration
            if (!backupConfig.saveToFile(targetFile)) {
                errorHandler.report(ErrorHandler::Severity::ERROR,
                                  "Failed to save restored configuration");
                return false;
            }
            
            // Log restoration
            errorHandler.report(ErrorHandler::Severity", "Configuration restored from backup: " + backupPath);
            
            return true;
            
        } catch (const std::exception& e) {
            errorHandler.report(ErrorHandler::Severity::ERROR,
                              "Exception during backup restoration: " + std::string(e.what()));
            return false;
        }
    }
    
    std::vector<std::string> listBackups() {
        std::vector<std::string> backups;
        
        if (!std::filesystem::exists("backups")) {
            return backups;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator("backups")) {
            if (entry.is_directory()) {
                backups.push_back(entry.path().string());
            }
        }
        
        // Sort backups by creation time (newest first)
        std::sort(backups.begin(), backups.end(),
                 [this](const std::string& a, const std::string& b) {
                     return getBackupTimestamp(a) > getBackupTimestamp(b);
                 });
        
        return backups;
    }
    
    bool deleteBackup(const std::string& backupPath) {
        try {
            if (std::filesystem::exists(backupPath)) {
                std::filesystem::remove_all(backupPath);
                return true;
            }
            return false;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    std::string generateBackupName(const std::string& originalFile) {
        std::string timestamp = getTimestamp();
        std::string filename = originalFile.substr(originalFile.find_last_of("/\\") + 1);
        return "backup_" + filename + "_" + timestamp;
    }
    
    std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        return ss.str();
    }
    
    double getBackupTimestamp(const std::string& backupPath) {
        auto time = std::filesystem::last_write_time(backupPath);
        auto time_t = std::chrono::file_clock::to_time_t(time);
        return std::chrono::time_point_cast<std::chrono::seconds>(time_t).time_since_epoch().count();
    }
    
    void saveBackupMetadata(const std::string& backupDir, const std::string& originalFile, const Config& config) {
        nlohmann::json metadata;
        metadata["backup_dir"] = backupDir;
        metadata["original_file"] = originalFile;
        metadata["timestamp"] = getBackupTimestamp(backupDir);
        metadata["config_size"] = std::filesystem::file_size(originalFile);
        
        std::string metadataFile = backupDir + "/metadata.json";
        std::ofstream metadataStream(metadataFile);
        metadataStream << metadata.dump(4);
    }
    
    void updateBackupIndex(const std::string& backupDir) {
        // Update backup index file
        nlohmann::json index;
        
        std::string indexFile = "backups/index.json";
        if (std::filesystem::exists(indexFile)) {
            std::ifstream indexStream(indexFile);
            index = nlohmann::json::parse(indexStream);
        }
        
        index.push_back(backupDir);
        std::ofstream indexStream(indexFile);
        indexStream << index.dump(4);
    }
};
```

### Configuration Diff and Comparison

```cpp
#include "core/Config/Config.hpp"

class ConfigurationDiff {
public:
    std::vector<std::string> diffConfigurations(const Config& config1, const Config& config2) {
        std::vector<std::string> differences;
        
        // Get all keys from both configurations
        auto allKeys = getAllKeys(config1, config2);
        
        for (const auto& key : allKeys) {
            std::optional<ConfigEntry> value1 = config1.getEntry(key);
            std::optional<ConfigEntry> value2 = config2.getEntry(key);
            
            if (value1 && !value2) {
                differences.push_back("Added: " + key + " = " + value1->toString());
            } else if (!value1 && value2) {
                differences.push_back("Removed: " + key + " = " + value2->toString());
            } else if (value1 && value2 && *value1 != *value2) {
                differences.push_back("Changed: " + key + " from " + value1->toString() + 
                                    " to " + value2->toString());
            }
        }
        
        return differences;
    }
    
    std::vector<std::string> getAllKeys(const Config& config1, const Config& config2) {
        std::vector<std::string> keys;
        
        // Get keys from first configuration
        auto config1Keys = config1.getAllKeys();
        keys.insert(keys.end(), config1Keys.begin(), config1Keys.end());
        
        // Add keys from second configuration
        auto config2Keys = config2.getAllKeys();
        for (const auto& key : config2Keys) {
            if (std::find(keys.begin(), keys.end(), key) == keys.end()) {
                keys.push_back(key);
            }
        }
        
        return keys;
    }
    
    std::string generateDiffReport(const std::vector<std::string>& differences) {
        std::stringstream report;
        report << "Configuration Differences:" << std::endl;
        report << std::string(50, '=') << std::endl;
        
        if (differences.empty()) {
            report << "No differences found." << std::endl;
        } else {
            for (size_t i = 0; i < differences.size(); ++i) {
                report << (i + 1) << ". " << differences[i] << std::endl;
            }
        }
        
        report << std::string(50, '=') << std::endl;
        report << "Total differences: " << differences.size() << std::endl;
        
        return report.str();
    }
};
```

## Configuration Templates

### Built-in Templates

The NLM configuration system includes several built-in templates:

#### Basic Template

```bash
nlm-config --template basic
```

**Applied configuration:**
```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "log_level": "INFO",
    "log_to_file": false
}
```

#### CPU Performance Template

```bash
nlm-config --template cpu_performance
```

**Applied configuration:**
```json
{
    "performance": {
        "parallel_processing": true,
        "thread_count": 4,
        "thread_pool_size": 8,
        "vectorization": true,
        "simd_vectorization": true
    },
    "memory_system": {
        "memory_pool_size": 2048,
        "event_queue_size": 20000,
        "cache_enabled": true,
        "compression_enabled": true
    },
    "logging": {
        "level": "WARNING"
    }
}
```

#### Memory-Intensive Template

```bash
nlm-config --template memory_intensive
```

**Applied configuration:**
```json
{
    "performance": {
        "parallel_processing": true,
        "thread_count": 8,
        "vectorization": true,
        "memory_pool_size": 4096,
        "event_queue_size": 50000
    },
    "memory_system": {
        "working_memory_size": 500,
        "episodic_capacity": 5000,
        "memory_pool_size": 8192,
        "compression_enabled": true,
        "cache_enabled": true
    },
    "logging": {
        "level": "ERROR",
        "to_file": true,
        "filename": "debug.log"
    }
}
```

#### Production Template

```bash
nlm-config --template production
```

**Applied configuration:**
```json
{
    "performance": {
        "parallel_processing": true,
        "thread_count": 4,
        "vectorization": true
    },
    "memory_system": {
        "memory_pool_size": 2048,
        "event_queue_size": 20000
    },
    "logging": {
        "level": "WARNING",
        "to_file": true,
        "filename": "production.log",
        "rotation": true,
        "max_size": 10485760,
        "backup_count": 5
    },
    "monitoring": {
        "enabled": true,
        "health_check_interval": 60.0,
        "performance_metrics": true,
        "resource_usage_monitoring": true
    }
}
```

### Creating Custom Templates

```cpp
#include "core/Config/Config.hpp"

class CustomTemplateManager {
public:
    bool saveTemplate(const std::string& name, const Config& templateConfig) {
        try {
            // Validate template configuration
            if (!templateConfig.validate()) {
                return false;
            }
            
            // Save template to file
            std::string templateFile = "templates/" + name + ".json";
            std::filesystem::create_directories("templates");
            
            if (!templateConfig.saveToFile(templateFile)) {
                return false;
            }
            
            // Update template index
            updateTemplateIndex(name, templateFile, templateConfig);
            
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    bool loadTemplate(const std::string& name, Config& templateConfig) {
        try {
            std::string templateFile = "templates/" + name + ".json";
            
            if (!templateConfig.loadFromFile(templateFile)) {
                return false;
            }
            
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    std::vector<std::string> listTemplates() {
        std::vector<std::string> templates;
        
        if (!std::filesystem::exists("templates")) {
            return templates;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator("templates")) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string filename = entry.path().filename().string();
                std::string name = filename.substr(0, filename.find_last_of("."));
                templates.push_back(name);
            }
        }
        
        return templates;
    }
    
    void updateTemplateIndex(const std::string& name, const std::string& filePath, const Config& config) {
        nlohmann::json index;
        
        std::string indexFile = "templates/index.json";
        if (std::filesystem::exists(indexFile)) {
            std::ifstream indexStream(indexFile);
            index = nlohmann::json::parse(indexStream);
        }
        
        nlohmann::json templateInfo;
        templateInfo["name"] = name;
        templateInfo["file_path"] = filePath;
        templateInfo["created_time"] = getCurrentTimestamp();
        templateInfo["description"] = getTemplateDescription(config);
        
        index.push_back(templateInfo);
        std::ofstream indexStream(indexFile);
        indexStream << index.dump(4);
    }
    
    std::string getTemplateDescription(const Config& config) {
        // Extract description from configuration
        // This could be customized based on template content
        return "Custom configuration template for NLM simulation";
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};
```

## Configuration CLI Usage

### Command Line Interface

The NLM configuration system provides a comprehensive CLI:

#### Basic Commands

```bash
# Show help information
nlm-config --help

# Show available templates
nlm-config --list-templates

# Show configuration examples
nlm-config --show-examples
```

#### Configuration File Operations

```bash
# Load configuration from file
nlm-config --config config.json

# Export configuration to file
nlm-config --export --format json > output.json
nlm-config --export --format yaml > output.yaml

# Validate configuration
nlm-config --validate --config user.cfg

# Merge configurations
nlm-config --merge --config1.json --config2.json --output merged.json
```

#### Template Operations

```bash
# Apply template
nlm-config --template basic
nlm-config --template cpu_performance
nlm-config --template memory_intensive
nlm-config --template production

# Apply template with custom values
nlm-config --template basic --set neuron_count=2000 --set simulation_timestep=0.0005

# Show template details
nlm-config --show-template basic
```

#### Backup and History Operations

```bash
# Create backup
nlm-config --backup

# Show backup list
nlm-config --list-backups

# Restore from backup
nlm-config --restore --backup backups/20240101_120000/config.json

# Show configuration history
nlm-config --show-history

# Show changes since backup
nlm-config --diff --backup backups/20240101_120000/config.json
```

### Configuration CLI Implementation

```cpp
#include "core/CLI/ConfigCLI.hpp"

class ConfigCLI {
public:
    ConfigCLI() {
        registerCommands();
    }
    
    void registerCommands() {
        // Register configuration commands
        registerCommand("config", "Load configuration from file", &ConfigCLI::cmdLoadConfig);
        registerCommand("export", "Export configuration to file", &ConfigCLI::cmdExportConfig);
        registerCommand("validate", "Validate configuration", &ConfigCLI::cmdValidateConfig);
        registerCommand("merge", "Merge multiple configuration files", &ConfigCLI::cmdMergeConfig);
        
        // Register template commands
        registerCommand("template", "Apply configuration template", &ConfigCLI::cmdApplyTemplate);
        registerCommand("list-templates", "List available templates", &ConfigCLI::cmdListTemplates);
        registerCommand("show-template", "Show template details", &ConfigCLI::cmdShowTemplate);
        
        // Register backup commands
        registerCommand("backup", "Create backup", &ConfigCLI::cmdCreateBackup);
        registerCommand("restore", "Restore from backup", &ConfigCLI::cmdRestoreBackup);
        registerCommand("list-backups", "List backups", &ConfigCLI::cmdListBackups);
        registerCommand("show-history", "Show configuration history", &ConfigCLI::cmdShowHistory);
        registerCommand("diff", "Show configuration differences", &ConfigCLI::cmdShowDiff);
    }
    
    int cmdLoadConfig(const std::vector<std::string>& args) {
        if (args.size() < 1) {
            std::cout << "Error: Configuration file required" << std::endl;
            return 1;
        }
        
        std::string configFile = args[0];
        bool validate = false;
        
        // Parse additional arguments
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] == "--validate") {
                validate = true;
            }
        }
        
        if (!loadConfiguration(configFile)) {
            return 1;
        }
        
        if (validate && !validateConfiguration()) {
            return 1;
        }
        
        return 0;
    }
    
    int cmdExportConfig(const std::vector<std::string>& args) {
        if (args.size() < 1) {
            std::cout << "Error: Output format required" << std::endl;
            return 1;
        }
        
        std::string format = args[0];
        std::string outputFile = "output." + format;
        
        if (args.size() > 1) {
            outputFile = args[1];
        }
        
        if (!exportConfiguration(format, outputFile)) {
            return 1;
        }
        
        return 0;
    }
    
    // ... other command implementations
};```

## Error Handling

### Common Configuration Errors

This section documents common configuration errors and their solutions:

#### Error 1: Missing Required Parameters
**Error**: Configuration validation failed - missing required parameters.

**Solution**:
```cpp
// Check for missing required parameters
Config config;
config.loadFromFile("config.json");

std::vector<std::string> missingParams = config.getMissingRequiredKeys();
for (const auto& param : missingParams) {
    std::cout << "Missing required parameter: " << param << std::endl;
}
```

#### Error 2: Invalid Parameter Values
**Error**: Configuration validation failed - invalid parameter values.

**Solution**:
```cpp
// Validate parameter values
if (!config.validate()) {
    auto errors = config.getValidationErrors();
    for (const auto& error : errors) {
        std::cout << "Validation error: " << error << std::endl;
    }
}
```

#### Error 3: File Not Found
**Error**: Failed to load configuration from file.

**Solution**:
```cpp
// Check if file exists
if (!std::filesystem::exists("config.json")) {
    std::cout << "Configuration file not found" << std::endl;
}
```

#### Error 4: Invalid File Format
**Error**: Failed to parse configuration file.

**Solution**:
```cpp
// Try different formats
Config config;
if (!config.loadFromFile("config.json")) {
    if (!config.loadFromFile("config.yaml")) {
        if (!config.loadFromFile("config.cfg")) {
            std::cout << "Unable to parse configuration file" << std::endl;
        }
    }
}
```

## Performance Considerations

### Configuration Loading Performance

1. **Use Pre-loaded Configurations**: Load configurations once and reuse them
2. **Use Efficient File Formats**: JSON and YAML provide good balance of readability and performance
3. **Validate Early**: Validate configurations early to catch errors before use
4. **Cache Configurations**: Cache frequently used configurations

### Configuration Access Performance

1. **Use Appropriate Access Methods**: Use type-safe accessors for better performance
2. **Minimize Configuration Lookup**: Cache frequently accessed configuration values
3. **Use References**: Use references to avoid copying large configuration objects
4. **Parallel Loading**: Load multiple configuration files in parallel when possible

### Configuration Validation Performance

1. **Lazy Validation**: Validate configurations only when needed
2. **Batch Validation**: Validate multiple configuration files in batch
3. **Optimized Validation**: Use optimized validation algorithms
4. **Skip Unnecessary Validation**: Skip validation for trusted configuration sources

## Migration Guide

### Migrating from Legacy Configuration System

1. **Convert Configuration Files**: Convert existing configuration files to JSON or YAML format
2. **Update Configuration Access**: Update configuration access code to use new `Config` class
3. **Add Schema Definitions**: Add schema definitions for validation (optional)
4. **Test Configuration**: Thoroughly test configuration loading and validation

### Migration Steps

1. **Backup Current Configuration**: Backup existing configuration files
2. **Create New Configuration Files**: Create new JSON or YAML configuration files
3. **Update Application**: Update application to use enhanced Config system
4. **Test Configuration**: Test configuration loading and validation
5. **Deploy New Configuration**: Deploy new configuration system

### Example Migration

```cpp
// Legacy configuration access
std::string logLevel = getLegacyConfigValue("log_level", "INFO");
int neuronCount = getLegacyConfigValue("neuron_count", 1000);
double timestep = getLegacyConfigValue("simulation_timestep", 0.001);

// New configuration access
Config config;
config.loadFromFile("config.json");
std::string logLevel = config.getStringOr("log_level", "INFO");
int neuronCount = config.getIntOr("neuron_count", 1000);
double timestep = config.getDoubleOr("simulation_timestep", 0.001);
```

## Conclusion

The NLM configuration system provides a powerful, flexible, and maintainable framework for configuration management. By following the guidelines and best practices in this guide, you can effectively utilize the enhanced configuration system and ensure optimal performance of your NLM simulations.

The configuration system includes:
- Comprehensive support for JSON and YAML configuration formats
- Advanced validation and error reporting
- Template-based configuration management
- Backup and history tracking
- Command-line interface for configuration management
- Performance optimizations for large-scale simulations

For more detailed information, refer to the other documentation files, particularly the usage guide, advanced usage guide, and plugin development guide.