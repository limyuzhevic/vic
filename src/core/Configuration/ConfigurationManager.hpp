#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Constants.hpp"

namespace nlm {

namespace config {

// Configuration versioning
struct Version {
    int major;
    int minor;
    int patch;
    
    Version() : major(1), minor(0), patch(0) {}
    Version(int major, int minor, int patch) : major(major), minor(minor), patch(patch) {}
    
    std::string toString() const {
        return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    }
    
    bool operator==(const Version& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
    
    bool operator<(const Version& other) const {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        return patch < other.patch;
    }
};

// Configuration categories
enum class ConfigCategory {
    Basic,
    Simulation,
    Plasticity,
    Neuromodulation,
    Development,
    World,
    Agent,
    Advanced,
    Custom
};

// Configuration parameter metadata
struct ParameterMetadata {
    std::string name;
    std::string description;
    ConfigCategory category;
    nlm::ConfigSource defaultSource;
    bool required;
    std::string validationRegex;
    std::vector<std::string> allowedValues;
    std::string unit;
    
    // Type information
    enum class Type {
        Integer,
        Float,
        Boolean,
        String,
        Array,
        Object
    };
    
    Type type;
    
    // Default values
    nlm::ConfigValue defaultValue;
    
    ParameterMetadata() : category(ConfigCategory::Basic), defaultSource(nlm::ConfigSource::Default),
                          required(false), type(Type::String) {}
    
    ParameterMetadata(const std::string& name, const std::string& description,
                      Type type, const nlm::ConfigValue& defaultVal,
                      ConfigCategory category = ConfigCategory::Basic)
        : name(name), description(description), category(category),
          defaultSource(nlm::ConfigSource::Default), required(false), type(type),
          defaultValue(defaultVal) {}
};

// Configuration schema
class ConfigSchema {
private:
    std::map<std::string, ParameterMetadata> parameters_;
    std::map<ConfigCategory, std::string> categoryDescriptions_;
    
public:
    ConfigSchema() {
        initializeDefaultSchema();
    }
    
    void initializeDefaultSchema() {
        // Basic simulation parameters
        parameters_["brain.neuron_count"] = ParameterMetadata(
            "brain.neuron_count",
            "Total number of neurons in the brain",
            ParameterMetadata::Type::Integer,
            static_cast<int64_t>(nlm::constants::main_phase2::DEFAULT_NEURON_COUNT),
            ConfigCategory::Basic
        );
        
        parameters_["simulation_timestep"] = ParameterMetadata(
            "simulation_timestep",
            "Time step for simulation (in seconds)",
            ParameterMetadata::Type::Float,
            nlm::constants::main_phase2::DEFAULT_TIMESTEP,
            ConfigCategory::Simulation
        );
        
        parameters_["region_count"] = ParameterMetadata(
            "region_count",
            "Number of neural regions",
            ParameterMetadata::Type::Integer,
            static_cast<int64_t>(1),
            ConfigCategory::Basic
        );
        
        parameters_["connection_probability"] = ParameterMetadata(
            "connection_probability",
            "Probability of connection between neurons (0.0 to 1.0)",
            ParameterMetadata::Type::Float,
            nlm::constants::main_phase2::DEFAULT_CONNECTION_PROBABILITY,
            ConfigCategory::Basic
        );
        
        // Plasticity parameters
        parameters_["plasticity.stdp.enable"] = ParameterMetadata(
            "plasticity.stdp.enable",
            "Enable Spike-Timing-Dependent Plasticity",
            ParameterMetadata::Type::Boolean,
            true,
            ConfigCategory::Plasticity
        );
        
        parameters_["plasticity.stdp.learning_rate"] = ParameterMetadata(
            "plasticity.stdp.learning_rate",
            "STDP learning rate",
            ParameterMetadata::Type::Float,
            0.001f,
            ConfigCategory::Plasticity
        );
        
        parameters_["plasticity.stdp.tau_plus"] = ParameterMetadata(
            "plasticity.stdp.tau_plus",
            "STDP time constant for potentiation (ms)",
            ParameterMetadata::Type::Float,
            20.0f,
            ConfigCategory::Plasticity
        );
        
        parameters_["plasticity.stdp.tau_minus"] = ParameterMetadata(
            "plasticity.stdp.tau_minus",
            "STDP time constant for depression (ms)",
            ParameterMetadata::Type::Float,
            20.0f,
            ConfigCategory::Plasticity
        );
        
        parameters_["plasticity.hebbian.enable"] = ParameterMetadata(
            "plasticity.hebbian.enable",
            "Enable Hebbian learning",
            ParameterMetadata::Type::Boolean,
            true,
            ConfigCategory::Plasticity
        );
        
        parameters_["plasticity.structural.enable"] = ParameterMetadata(
            "plasticity.structural.enable",
            "Enable structural plasticity",
            ParameterMetadata::Type::Boolean,
            true,
            ConfigCategory::Plasticity
        );
        
        // Neuromodulation parameters
        parameters_["neuromod.dopamine.scale"] = ParameterMetadata(
            "neuromod.dopamine.scale",
            "Dopamine modulation scale",
            ParameterMetadata::Type::Float,
            1.0f,
            ConfigCategory::Neuromodulation
        );
        
        parameters_["neuromod.curiosity.enable"] = ParameterMetadata(
            "neuromod.curiosity.enable",
            "Enable curiosity-driven exploration",
            ParameterMetadata::Type::Boolean,
            true,
            ConfigCategory::Neuromodulation
        );
        
        parameters_["neuromod.novelty.enable"] = ParameterMetadata(
            "neuromod.novelty.enable",
            "Enable novelty detection",
            ParameterMetadata::Type::Boolean,
            true,
            ConfigCategory::Neuromodulation
        );
        
        // Development parameters
        parameters_["development.enabled"] = ParameterMetadata(
            "development.enabled",
            "Enable developmental stages",
            ParameterMetadata::Type::Boolean,
            true,
            ConfigCategory::Development
        );
        
        parameters_["development.initial_age"] = ParameterMetadata(
            "development.initial_age",
            "Initial developmental age (in simulation steps)",
            ParameterMetadata::Type::Float,
            0.0f,
            ConfigCategory::Development
        );
        
        // World parameters
        parameters_["world.width"] = ParameterMetadata(
            "world.width",
            "Width of the simulation world",
            ParameterMetadata::Type::Integer,
            static_cast<int64_t>(20),
            ConfigCategory::World
        );
        
        parameters_["world.height"] = ParameterMetadata(
            "world.height",
            "Height of the simulation world",
            ParameterMetadata::Type::Integer,
            static_cast<int64_t>(20),
            ConfigCategory::World
        );
        
        parameters_["world.vision_width"] = ParameterMetadata(
            "world.vision_width",
            "Width of agent's visual field",
            ParameterMetadata::Type::Integer,
            static_cast<int64_t>(8),
            ConfigCategory::World
        );
        
        parameters_["world.vision_height"] = ParameterMetadata(
            "world.vision_height",
            "Height of agent's visual field",
            ParameterMetadata::Type::Integer,
            static_cast<int64_t>(8),
            ConfigCategory::World
        );
        
        // Agent parameters
        parameters_["agent.reward_modulation.enabled"] = ParameterMetadata(
            "agent.reward_modulation.enabled",
            "Enable reward modulation for the agent",
            ParameterMetadata::Type::Boolean,
            true,
            ConfigCategory::Agent
        );
        
        parameters_["agent.curiosity.enabled"] = ParameterMetadata(
            "agent.curiosity.enabled",
            "Enable curiosity-based exploration",
            ParameterMetadata::Type::Boolean,
            true,
            ConfigCategory::Agent
        );
        
        parameters_["agent.development.enabled"] = ParameterMetadata(
            "agent.development.enabled",
            "Enable developmental stages for the agent",
            ParameterMetadata::Type::Boolean,
            true,
            ConfigCategory::Agent
        );
        
        // Advanced parameters
        parameters_["advanced.enable_logging"] = ParameterMetadata(
            "advanced.enable_logging",
            "Enable detailed logging",
            ParameterMetadata::Type::Boolean,
            false,
            ConfigCategory::Advanced
        );
        
        parameters_["advanced.log_level"] = ParameterMetadata(
            "advanced.log_level",
            "Log level (0=debug, 1=info, 2=warning, 3=error)",
            ParameterMetadata::Type::Integer,
            static_cast<int64_t>(1),
            ConfigCategory::Advanced
        );
        
        parameters_["advanced.performance_monitoring"] = ParameterMetadata(
            "advanced.performance_monitoring",
            "Enable performance monitoring",
            ParameterMetadata::Type::Boolean,
            false,
            ConfigCategory::Advanced
        );
        
        // Initialize category descriptions
        categoryDescriptions_[ConfigCategory::Basic] = "Basic simulation parameters";
        categoryDescriptions_[ConfigCategory::Simulation] = "Simulation control parameters";
        categoryDescriptions_[ConfigCategory::Plasticity] = "Plasticity and learning parameters";
        categoryDescriptions_[ConfigCategory::Neuromodulation] = "Neuromodulation parameters";
        categoryDescriptions_[ConfigCategory::Development] = "Development stage parameters";
        categoryDescriptions_[ConfigCategory::World] = "World simulation parameters";
        categoryDescriptions_[ConfigCategory::Agent] = "Agent behavior parameters";
        categoryDescriptions_[ConfigCategory::Advanced] = "Advanced configuration options";
    }
    
    // Get parameter metadata
    const ParameterMetadata* getParameter(const std::string& key) const {
        auto it = parameters_.find(key);
        return (it != parameters_.end()) ? &it->second : nullptr;
    }
    
    // Check if parameter exists
    bool hasParameter(const std::string& key) const {
        return parameters_.find(key) != parameters_.end();
    }
    
    // Get all parameter names
    std::vector<std::string> getParameterNames() const {
        std::vector<std::string> names;
        names.reserve(parameters_.size());
        for (const auto& pair : parameters_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get parameters by category
    std::vector<std::string> getParametersByCategory(ConfigCategory category) const {
        std::vector<std::string> names;
        for (const auto& pair : parameters_) {
            if (pair.second.category == category) {
                names.push_back(pair.first);
            }
        }
        return names;
    }
    
    // Get all categories
    std::vector<ConfigCategory> getCategories() const {
        std::vector<ConfigCategory> categories;
        for (const auto& pair : categoryDescriptions_) {
            categories.push_back(pair.first);
        }
        return categories;
    }
    
    // Get category description
    std::string getCategoryDescription(ConfigCategory category) const {
        auto it = categoryDescriptions_.find(category);
        return (it != categoryDescriptions_.end()) ? it->second : "";
    }
    
    // Validate parameter value
    bool validateParameter(const std::string& key, const nlm::ConfigValue& value) const {
        const ParameterMetadata* metadata = getParameter(key);
        if (!metadata) {
            return false; // Unknown parameter
        }
        
        // Basic type validation
        if (!validateType(value, metadata->type)) {
            return false;
        }
        
        // Range validation for numeric types
        if (metadata->type == ParameterMetadata::Type::Integer) {
            if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<double>(value)) {
                return false;
            }
            double intValue = std::holds_alternative<int64_t>(value) ? 
                static_cast<double>(std::get<int64_t>(value)) : std::get<double>(value);
            
            // Additional range validation based on parameter
            if (key == "brain.neuron_count") {
                return intValue >= 1 && intValue <= 1000000;
            } else if (key == "simulation_timestep") {
                return intValue > 0 && intValue <= 0.1;
            } else if (key == "region_count") {
                return intValue >= 1 && intValue <= 100;
            } else if (key == "connection_probability") {
                return intValue >= 0.0 && intValue <= 1.0;
            }
        } else if (metadata->type == ParameterMetadata::Type::Float) {
            if (!std::holds_alternative<double>(value)) {
                return false;
            }
            double floatValue = std::get<double>(value);
            
            // Additional range validation based on parameter
            if (key == "plasticity.stdp.learning_rate") {
                return floatValue >= 0.0 && floatValue <= 1.0;
            } else if (key == "plasticity.stdp.tau_plus") {
                return floatValue >= 1.0 && floatValue <= 100.0;
            } else if (key == "neuromod.dopamine.scale") {
                return floatValue >= 0.0 && floatValue <= 10.0;
            }
        } else if (metadata->type == ParameterMetadata::Type::Boolean) {
            if (!std::holds_alternative<bool>(value)) {
                return false;
            }
        }
        
        // Allowed values validation
        if (!metadata->allowedValues.empty()) {
            if (metadata->type == ParameterMetadata::Type::String) {
                if (!std::holds_alternative<std::string>(value)) {
                    return false;
                }
                const std::string& strValue = std::get<std::string>(value);
                return std::find(metadata->allowedValues.begin(), metadata->allowedValues.end(), strValue) != metadata->allowedValues.end();
            }
        }
        
        return true;
    }
    
    // Get default value for parameter
    nlm::ConfigValue getDefaultValue(const std::string& key) const {
        const ParameterMetadata* metadata = getParameter(key);
        return (metadata) ? metadata->defaultValue : nlm::ConfigValue();
    }
    
    // Get description for parameter
    std::string getDescription(const std::string& key) const {
        const ParameterMetadata* metadata = getParameter(key);
        return (metadata) ? metadata->description : "";
    }
    
    // Get unit for parameter
    std::string getUnit(const std::string& key) const {
        const ParameterMetadata* metadata = getParameter(key);
        return (metadata) ? metadata->unit : "";
    }
    
private:
    bool validateType(const nlm::ConfigValue& value, ParameterMetadata::Type type) const {
        switch (type) {
            case ParameterMetadata::Type::Integer:
                return std::holds_alternative<int64_t>(value) || std::holds_alternative<double>(value);
            case ParameterMetadata::Type::Float:
                return std::holds_alternative<double>(value);
            case ParameterMetadata::Type::Boolean:
                return std::holds_alternative<bool>(value);
            case ParameterMetadata::Type::String:
                return std::holds_alternative<std::string>(value);
            case ParameterMetadata::Type::Array:
                return std::holds_alternative<std::vector<double>>(value) || std::holds_alternative<std::vector<int>>(value);
            case ParameterMetadata::Type::Object:
                return false; // Not implemented
            default:
                return false;
        }
    }
};

// Configuration manager
class ConfigurationManager {
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<ConfigSchema> schema_;
    Version currentVersion_;
    
    // Environment variables
    static const std::string ENVIRONMENT_PREFIX;
    
public:
    ConfigurationManager() : config_(std::make_shared<Config>()), 
                           schema_(std::make_shared<ConfigSchema>()),
                           currentVersion_(1, 0, 0) {
        initializeDefaultConfig();
    }
    
    ConfigurationManager(const std::string& filepath) : config_(std::make_shared<Config>()),
                                                       schema_(std::make_shared<ConfigSchema>()),
                                                       currentVersion_(1, 0, 0) {
        loadFromFile(filepath);
    }
    
    // Get configuration manager instance (singleton)
    static ConfigurationManager& getInstance() {
        static ConfigurationManager instance;
        return instance;
    }
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath) {
        try {
            if (config_->loadFromFile(filepath)) {
                // Upgrade configuration if needed
                upgradeConfiguration(currentVersion_);
                return true;
            }
            return false;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to load configuration from file: ") + e.what());
            return false;
        }
    }
    
    // Load configuration from environment variables
    bool loadFromEnvironment() {
        bool success = true;
        
        for (const auto& pair : schema_->getParameterNames()) {
            const ParameterMetadata* metadata = schema_->getParameter(pair);
            if (!metadata) continue;
            
            std::string envKey = ENVIRONMENT_PREFIX + pair;
            std::string envValue;
            
            // Try to get environment variable
            const char* envVar = std::getenv(envKey.c_str());
            if (envVar) {
                envValue = envVar;
                
                try {
                    // Convert environment value to config value
                    nlm::ConfigValue configValue = stringToConfigValue(envValue, metadata->type);
                    config_->set(pair, configValue, nlm::ConfigSource::Runtime);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Invalid environment value for parameter ") + pair + ": " + e.what());
                    success = false;
                }
            }
        }
        
        return success;
    }
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    // Get configuration
    std::shared_ptr<Config> getConfig() const {
        return config_;
    }
    
    // Set configuration
    void setConfig(const std::shared_ptr<Config>& config) {
        config_ = config;
    }
    
    // Get configuration schema
    std::shared_ptr<ConfigSchema> getSchema() const {
        return schema_;
    }
    
    // Get current version
    Version getCurrentVersion() const {
        return currentVersion_;
    }
    
    // Set current version
    void setCurrentVersion(const Version& version) {
        currentVersion_ = version;
        upgradeConfiguration(version);
    }
    
    // Validate configuration
    bool validate() const {
        return schema_->validateParameter(config_);
    }
    
    // Get validation errors
    std::string getValidationErrors() const {
        return schema_->validateParameter(config_);
    }
    
    // Reset to defaults
    void resetToDefaults() {
        config_->clear();
        initializeDefaultConfig();
    }
    
    // Merge with another configuration
    void merge(const ConfigurationManager& other) {
        for (const auto& key : other.config_->getKeys()) {
            config_->set(key, other.config_->get(nlm::ConfigValue{}, key), nlm::ConfigSource::Runtime);
        }
    }
    
    // Get parameter metadata
    const ParameterMetadata* getParameterMetadata(const std::string& key) const {
        return schema_->getParameter(key);
    }
    
    // Get all parameter names
    std::vector<std::string> getParameterNames() const {
        return schema_->getParameterNames();
    }
    
    // Get parameters by category
    std::vector<std::string> getParametersByCategory(ConfigCategory category) const {
        return schema_->getParametersByCategory(category);
    }
    
    // Get category descriptions
    std::vector<std::pair<ConfigCategory, std::string>> getCategoryDescriptions() const {
        std::vector<std::pair<ConfigCategory, std::string>> descriptions;
        for (const auto& pair : schema_->getCategories()) {
            descriptions.push_back({pair, schema_->getCategoryDescription(pair)});
        }
        return descriptions;
    }
    
    // Get parameter value as string
    std::string getParameterAsString(const std::string& key) const {
        const ParameterMetadata* metadata = schema_->getParameter(key);
        if (!metadata) {
            return "";
        }
        
        auto value = config_->get(metadata->type == ParameterMetadata::Type::String ? 
                                  std::get<std::string>(metadata->defaultValue) : 
                                  "");
        return value.has_value() ? configValueToString(*value) : "";
    }
    
    // Set parameter from string
    void setParameterFromString(const std::string& key, const std::string& valueStr) {
        const ParameterMetadata* metadata = schema_->getParameter(key);
        if (!metadata) {
            return;
        }
        
        nlm::ConfigValue configValue = stringToConfigValue(valueStr, metadata->type);
        config_->set(key, configValue, nlm::ConfigSource::Runtime);
    }
    
    // Export configuration to JSON
    std::string exportToJson() const {
        nlohmann::json jsonConfig;
        
        for (const auto& key : config_->getKeys()) {
            auto value = config_->get(key);
            if (value) {
                jsonConfig[key] = configValueToJson(*value);
            }
        }
        
        return jsonConfig.dump(2);
    }
    
    // Import configuration from JSON
    bool importFromJson(const std::string& jsonStr) {
        try {
            auto jsonConfig = nlohmann::json::parse(jsonStr);
            
            for (const auto& [key, value] : jsonConfig.items()) {
                config_->set(key, jsonToConfigValue(value));
            }
            
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to import configuration from JSON: ") + e.what());
            return false;
        }
    }
    
    // Get configuration summary
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Configuration Summary (Version " << currentVersion_.toString() << "):\n";
        oss << "Loaded " << config_->getKeys().size() << " parameters:\n";
        
        for (const auto& key : config_->getKeys()) {
            oss << "  " << key << ": ";
            auto value = config_->get(key);
            if (value) {
                oss << configValueToString(*value);
            }
            oss << "\n";
        }
        
        return oss.str();
    }
    
    // Get configuration statistics
    std::string getStatistics() const {
        std::ostringstream oss;
        oss << "Configuration Statistics:\n";
        oss << "  Total Parameters: " << config_->getKeys().size() << "\n";
        oss << "  Parameter Categories: " << schema_->getCategories().size() << "\n";
        oss << "  Validation: " << (validate() ? "Passed" : "Failed") << "\n";
        oss << "  Environment Variables: " << ENVIRONMENT_PREFIX << "*";
        return oss.str();
    }
    
    // Configuration upgrade
    bool upgradeConfiguration(const Version& targetVersion) {
        if (targetVersion <= currentVersion_) {
            return true; // No upgrade needed
        }
        
        std::vector<Version> versionsToUpgrade;
        Version current = currentVersion_;
        
        while (current < targetVersion) {
            versionsToUpgrade.push_back(current);
            // Increment version (simplified)
            current = Version(current.major, current.minor, current.patch + 1);
        }
        
        for (const auto& version : versionsToUpgrade) {
            if (!upgradeToVersion(version)) {
                return false;
            }
        }
        
        currentVersion_ = targetVersion;
        return true;
    }
    
    // Configuration backup
    bool backupConfiguration(const std::string& backupDir) const {
        try {
            std::filesystem::create_directories(backupDir);
            
            std::string backupFile = backupDir + "/nlm_config_" + 
                                    std::to_string(currentVersion_.major) + "_" +
                                    std::to_string(currentVersion_.minor) + "_" +
                                    std::to_string(currentVersion_.patch) + ".json";
            
            std::ofstream file(backupFile);
            if (!file.is_open()) {
                return false;
            }
            
            file << exportToJson();
            file.close();
            
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to backup configuration: ") + e.what());
            return false;
        }
    }
    
    // Restore configuration from backup
    bool restoreFromBackup(const std::string& backupFile) {
        try {
            std::ifstream file(backupFile);
            if (!file.is_open()) {
                return false;
            }
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            file.close();
            
            return importFromJson(buffer.str());
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to restore configuration from backup: ") + e.what());
            return false;
        }
    }
    
    // Get available backups
    std::vector<std::string> getAvailableBackups(const std::string& backupDir) const {
        std::vector<std::string> backups;
        
        try {
            if (!std::filesystem::exists(backupDir)) {
                return backups;
            }
            
            for (const auto& entry : std::filesystem::directory_iterator(backupDir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".json") {
                    backups.push_back(entry.path().string());
                }
            }
            
            std::sort(backups.begin(), backups.end(),
                     [](const std::string& a, const std::string& b) {
                         return std::filesystem::last_write_time(a) > std::filesystem::last_write_time(b);
                     });
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to get available backups: ") + e.what());
        }
        
        return backups;
    }
    
    // Configuration comparison
    std::string compareConfigurations(const ConfigurationManager& other) const {
        std::ostringstream oss;
        oss << "Configuration Comparison:\n";
        
        // Find common parameters
        std::vector<std::string> commonParams;
        std::set_intersection(config_->getKeys().begin(), config_->getKeys().end(),
                             other.config_->getKeys().begin(), other.config_->getKeys().end(),
                             std::back_inserter(commonParams));
        
        oss << "  Common Parameters: " << commonParams.size() << "\n";
        
        for (const auto& param : commonParams) {
            auto value1 = config_->get(param);
            auto value2 = other.config_->get(param);
            
            if (value1 && value2) {
                if (*value1 != *value2) {
                    oss << "    " << param << ": DIFFERENT\n";
                    oss << "      Current: " << configValueToString(*value1) << "\n";
                    oss << "      Other:   " << configValueToString(*value2) << "\n";
                }
            } else if (value1 != value2) {
                oss << "    " << param << ": DIFFERENT (one missing)\n";
            }
        }
        
        // Find unique parameters
        std::vector<std::string> uniqueToThis;
        std::set_difference(config_->getKeys().begin(), config_->getKeys().end(),
                           other.config_->getKeys().begin(), other.config_->getKeys().end(),
                           std::back_inserter(uniqueToThis));
        
        oss << "  Unique to this config: " << uniqueToThis.size() << "\n";
        
        for (const auto& param : uniqueToThis) {
            oss << "    " << param << "\n";
        }
        
        return oss.str();
    }
    
    // Configuration reset to environment
    void resetToEnvironment() {
        // Clear current configuration
        config_->clear();
        
        // Load from environment
        loadFromEnvironment();
        
        // Apply defaults
        initializeDefaultConfig();
    }
    
    // Configuration validation with suggestions
    std::string validateWithSuggestions() const {
        std::ostringstream oss;
        bool hasErrors = false;
        bool hasWarnings = false;
        
        for (const auto& key : config_->getKeys()) {
            const ParameterMetadata* metadata = schema_->getParameter(key);
            if (!metadata) {
                oss << "WARNING: Unknown parameter: " << key << "\n";
                hasWarnings = true;
                continue;
            }
            
            if (!schema_->validateParameter(key, metadata->defaultValue)) {
                oss << "ERROR: Invalid default value for parameter " << key << "\n";
                hasErrors = true;
            }
            
            // Check if parameter has invalid value
            auto value = config_->get(key);
            if (value && !metadata->allowedValues.empty()) {
                if (metadata->type == ParameterMetadata::Type::String) {
                    const std::string& strValue = std::get<std::string>(*value);
                    if (std::find(metadata->allowedValues.begin(), metadata->allowedValues.end(), strValue) == metadata->allowedValues.end()) {
                        oss << "WARNING: Parameter " << key << " has unexpected value: " << strValue << "\n";
                        hasWarnings = true;
                    }
                }
            }
        }
        
        if (!hasErrors && !hasWarnings) {
            oss << "Configuration validation passed with no errors or warnings.\n";
        }
        
        return oss.str();
    }
    
protected:
    virtual void initializeDefaultConfig() {
        // Set default values for all parameters
        for (const auto& pair : schema_->getParameterNames()) {
            const ParameterMetadata* metadata = schema_->getParameter(pair);
            if (!metadata) continue;
            
            config_->set(pair, metadata->defaultValue, nlm::ConfigSource::Default);
        }
    }
    
    virtual bool upgradeToVersion(const Version& targetVersion) {
        // Implement version-specific upgrades
        // For now, just log the upgrade attempt
        NLM_LOG_INFO(std::string("Upgrading configuration to version ") + targetVersion.toString());
        return true;
    }
    
    nlm::ConfigValue stringToConfigValue(const std::string& str, ParameterMetadata::Type type) const {
        switch (type) {
            case ParameterMetadata::Type::Integer:
                try {
                    return std::stoll(str);
                } catch (const std::exception&) {
                    throw std::runtime_error("Cannot convert string to integer");
                }
            case ParameterMetadata::Type::Float:
                try {
                    return std::stod(str);
                } catch (const std::exception&) {
                    throw std::runtime_error("Cannot convert string to float");
                }
            case ParameterMetadata::Type::Boolean:
                if (str == "true" || str == "1") return true;
                if (str == "false" || str == "0") return false;
                throw std::runtime_error("Cannot convert string to boolean");
            case ParameterMetadata::Type::String:
                return str;
            case ParameterMetadata::Type::Array:
                // Parse array from string (simplified)
                if (str.empty() || str == "[]") return std::vector<double>{};
                if (str.front() == '[' && str.back() == ']') {
                    std::vector<double> result;
                    std::stringstream ss(str.substr(1, str.size() - 2));
                    std::string item;
                    while (std::getline(ss, item, ',')) {
                        item = trim(item);
                        if (!item.empty()) {
                            try {
                                result.push_back(std::stod(item));
                            } catch (...) {
                                throw std::runtime_error("Cannot parse array element: " + item);
                            }
                        }
                    }
                    return result;
                }
                throw std::runtime_error("Invalid array format");
            default:
                throw std::runtime_error("Unsupported parameter type");
        }
    }
    
    std::string configValueToString(const nlm::ConfigValue& value) const {
        if (std::holds_alternative<int64_t>(value)) {
            return std::to_string(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value) ? "true" : "false";
        } else if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            const auto& arr = std::get<std::vector<double>>(value);
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arr[i];
            }
            oss << "]";
            return oss.str();
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            const auto& arr = std::get<std::vector<int>>(value);
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arr[i];
            }
            oss << "]";
            return oss.str();
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            const auto& arr = std::get<std::vector<std::string>>(value);
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arr[i] << "\"";
            }
            oss << "]";
            return oss.str();
        } else {
            return "<unknown>";
        }
    }
    
    nlohmann::json configValueToJson(const nlm::ConfigValue& value) const {
        if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return std::get<std::vector<double>>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            return std::get<std::vector<int>>(value);
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            return std::get<std::vector<std::string>>(value);
        } else {
            throw std::runtime_error("Cannot serialize unknown config value type");
        }
    }
    
    nlm::ConfigValue jsonToConfigValue(const nlohmann::json& json) const {
        if (json.is_number_integer()) {
            return json.get<int64_t>();
        } else if (json.is_number_float()) {
            return json.get<double>();
        } else if (json.is_boolean()) {
            return json.get<bool>();
        } else if (json.is_string()) {
            return json.get<std::string>();
        } else if (json.is_array()) {
            // Check array element type
            if (!json.empty() && json[0].is_number_integer()) {
                std::vector<int> result;
                for (const auto& item : json) {
                    result.push_back(item.get<int>());
                }
                return result;
            } else {
                std::vector<double> result;
                for (const auto& item : json) {
                    result.push_back(item.get<double>());
                }
                return result;
            }
        } else {
            throw std::runtime_error("Cannot deserialize JSON value to config");
        }
    }
    
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
    
    // Schema validation
    bool schemaValidateParameter(const std::string& key, const nlm::ConfigValue& value) const {
        const ParameterMetadata* metadata = schema_->getParameter(key);
        if (!metadata) return false;
        return schema_->validateParameter(key, value);
    }
};

// Global configuration manager instance
ConfigurationManager& getConfigurationManager() {
    static ConfigurationManager instance;
    return instance;
}

// Convenience functions
inline bool loadConfigurationFromFile(const std::string& filepath) {
    return getConfigurationManager().loadFromFile(filepath);
}

inline bool saveConfigurationToFile(const std::string& filepath) {
    return getConfigurationManager().saveToFile(filepath);
}

inline bool validateConfiguration() {
    return getConfigurationManager().validate();
}

inline std::string getConfigurationSummary() {
    return getConfigurationManager().getSummary();
}

inline void resetConfigurationToDefaults() {
    getConfigurationManager().resetToDefaults();
}

inline void resetConfigurationToEnvironment() {
    getConfigurationManager().resetToEnvironment();
}

} // namespace config

} // namespace nlm
