// Enhanced Configuration Manager Implementation
// Provides advanced configuration features for power users

#include "AdvancedConfigurationManager.hpp"
#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <iomanip>

namespace nlm {

struct ConfigurationManager::Impl {
    std::shared_ptr<Config> config;
    std::unordered_map<std::string, std::string> parameterDescriptions;
    std::unordered_map<std::string, ConfigValue> defaultValues;
    std::vector<ValidationRule> validationRules;
    
    Impl() : config(std::make_shared<Config>()) {
        // Initialize with default parameter descriptions
        parameterDescriptions = {
            {"neuron_count", "Number of neurons in the brain"},
            {"region_count", "Number of brain regions"},
            {"connection_probability", "Connection probability for random connectivity"},
            {"simulation_timestep", "Simulation timestep in seconds"},
            {"plasticity_learning_rate", "Plasticity learning rate"},
            {"random_seed", "Random seed for reproducible experiments"},
            {"max_simulation_steps", "Maximum simulation steps"},
            {"checkpoint_path", "Path for checkpoint files"},
            {"log_level", "Logging level (DEBUG, INFO, WARN, ERROR)"},
            {"visualization_enabled", "Enable visualization"},
            {"environment_name", "Name of the simulation environment"}
        };
        
        // Initialize default values
        defaultValues = {
            {"neuron_count", 1000},
            {"region_count", 2},
            {"connection_probability", 0.1},
            {"simulation_timestep", 0.001},
            {"plasticity_learning_rate", 0.01},
            {"random_seed", 42},
            {"max_simulation_steps", 10000},
            {"checkpoint_path", "./checkpoint.bin"},
            {"log_level", "INFO"},
            {"visualization_enabled", false},
            {"environment_name", "GridWorld"}
        };
    }
};

ConfigurationManager::ConfigurationManager() : pImpl(std::make_unique<Impl>()) {}

ConfigurationManager::~ConfigurationManager() = default;

bool ConfigurationManager::loadFromJSON(const std::string& filepath) {
    // For now, fallback to simple key=value format
    // TODO: Implement full JSON parsing
    return pImpl->config->loadFromFile(filepath);
}

bool ConfigurationManager::loadFromYAML(const std::string& filepath) {
    // For now, fallback to simple key=value format
    // TODO: Implement full YAML parsing
    return pImpl->config->loadFromFile(filepath);
}

bool ConfigurationManager::exportToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->config->getKeys()) {
        if (!first) file << ",\n";
        first = false;
        
        file << "  \"" << entry << "\": ";
        
        auto value = pImpl->config->get<ConfigValue>(entry);
        if (value.has_value()) {
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> ||
                                     std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << std::setprecision(6) << arg;
                }
            }, *value);
        }
    }
    
    file << "\n}\n";
    return true;
}

bool ConfigurationManager::exportToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& key : pImpl->config->getKeys()) {
        file << key << ": ";
        
        auto value = pImpl->config->get<ConfigValue>(key);
        if (value.has_value()) {
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> ||
                                     std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << std::setprecision(6) << arg;
                }
            }, *value);
        }
        file << "\n";
    }
    
    return true;
}

bool ConfigurationManager::validateConfiguration(std::vector<std::string>& errors,
                                                 std::vector<std::string>& warnings) const {
    bool success = true;
    
    // Check validation rules
    for (const auto& rule : pImpl->validationRules) {
        auto value = pImpl->config->get<ConfigValue>(rule.parameter);
        if (value.has_value()) {
            if (!rule.validator(*value)) {
                if (rule.warningOnly) {
                    warnings.push_back("Warning: " + rule.errorMessage);
                } else {
                    errors.push_back("Error: " + rule.errorMessage);
                    success = false;
                }
            }
        }
    }
    
    return success;
}

void ConfigurationManager::addValidationRule(const std::string& param,
                           const std::function<bool(const ConfigValue&)>& validator,
                           const std::string& errorMsg,
                           bool warningOnly) {
    pImpl->validationRules.push_back({param, validator, errorMsg, warningOnly});
}

std::string ConfigurationManager::getValidationReport() const {
    std::vector<std::string> errors, warnings;
    validateConfiguration(errors, warnings);
    
    std::ostringstream oss;
    oss << "=== Configuration Validation Report ===\n\n";
    
    if (!warnings.empty()) {
        oss << "Warnings:";
        for (const auto& warning : warnings) {
            oss << "\n  - " << warning;
        }
        oss << "\n\n";
    }
    
    if (!errors.empty()) {
        oss << "Errors:";
        for (const auto& error : errors) {
            oss << "\n  - " << error;
        }
        oss << "\n\n";
    }
    
    if (warnings.empty() && errors.empty()) {
        oss << "All checks passed. Configuration is valid.\n";
    }
    
    return oss.str();
}

bool ConfigurationManager::optimizeConfiguration() {
    // Simple optimization: remove unused parameters
    auto keys = pImpl->config->getKeys();
    
    // For now, just log optimization opportunities
    // TODO: Implement actual optimization
    std::cout << "Optimization opportunities:\n";
    std::cout << "- Consider reducing neuron_count if simulation is slow";
    std::cout << "\n- Consider reducing max_simulation_steps if experiments complete early";
    
    return true;
}

bool ConfigurationManager::compressConfiguration() {
    // For now, placeholder implementation
    std::cout << "Configuration compression started...";
    std::cout << "(This would compress configuration data in a real implementation)";
    return true;
}

bool ConfigurationManager::createBackup(const std::string& backupPath) const {
    if (backupPath.empty()) {
        std::filesystem::path backupFile = "configs/backup_";
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        backupFile += std::to_string(time) + ".cfg";
        
        return pImpl->config->saveToFile(backupFile.string());
    } else {
        return pImpl->config->saveToFile(backupPath);
    }
}

bool ConfigurationManager::compareConfigurations(const ConfigurationManager& other,
                                                 std::vector<std::string>& differences) const {
    auto keys1 = pImpl->config->getKeys();
    auto keys2 = other.pImpl->config->getKeys();
    
    // Check keys in config1 but not in config2
    for (const auto& key : keys1) {
        if (!other.pImpl->config->has(key)) {
            differences.push_back("Missing in other config: " + key);
        } else {
            // Compare values
            auto val1 = pImpl->config->get<ConfigValue>(key);
            auto val2 = other.pImpl->config->get<ConfigValue>(key);
            if (val1.has_value() && val2.has_value()) {
                // Need to implement value comparison
                // For now, just note if keys differ
                if (std::visit([](auto&& a, auto&& b) { return a == b; }, *val1, *val2)) {
                    differences.push_back("Value difference: " + key);
                }
            }
        }
    }
    
    // Check keys in config2 but not in config1
    for (const auto& key : keys2) {
        if (!pImpl->config->has(key)) {
            differences.push_back("Only in other config: " + key);
        }
    }
    
    return differences.empty();
}

std::string ConfigurationManager::getConfigurationStats() const {
    std::ostringstream oss;
    auto keys = pImpl->config->getKeys();
    
    oss << "=== Configuration Statistics ===\n";
    oss << "Total parameters: " << keys.size() << "\n";
    
    // Count parameter types
    int intCount = 0, doubleCount = 0, stringCount = 0, boolCount = 0;
    
    for (const auto& key : keys) {
        auto value = pImpl->config->get<ConfigValue>(key);
        if (value.has_value()) {
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    intCount++;
                } else if constexpr (std::is_same_v<T, double>) {
                    doubleCount++;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    stringCount++;
                } else if constexpr (std::is_same_v<T, bool>) {
                    boolCount++;
                }
            }, *value);
        }
    }
    
    oss << "Integer parameters: " << intCount << "\n";
    oss << "Float parameters: " << doubleCount << "\n";
    oss << "String parameters: " << stringCount << "\n";
    oss << "Boolean parameters: " << boolCount << "\n";
    
    return oss.str();
}

std::unordered_map<std::string, std::string> ConfigurationManager::getAllParameterDescriptions() const {
    return pImpl->parameterDescriptions;
}

void ConfigurationManager::setParameterDescription(const std::string& key,
                                 const std::string& description) {
    pImpl->parameterDescriptions[key] = description;
}

std::optional<ConfigValue> ConfigurationManager::getDefaultValue(const std::string& key) const {
    auto it = pImpl->defaultValues.find(key);
    if (it != pImpl->defaultValues.end()) {
        return it->second;
    }
    return std::nullopt;
}

void ConfigurationManager::addDefaultValue(const std::string& key, const ConfigValue& value,
                         const std::string& description) {
    pImpl->defaultValues[key] = value;
    if (!description.empty()) {
        pImpl->parameterDescriptions[key] = description;
    }
}

void ConfigurationManager::resetToDefaults() {
    pImpl->config->clear();
    
    for (const auto& pair : pImpl->defaultValues) {
        pImpl->config->set(pair.first, pair.second, ConfigSource::Default);
    }
}

bool ConfigurationManager::mergeConfiguration(const ConfigurationManager& other) {
    auto otherKeys = other.pImpl->config->getKeys();
    
    for (const auto& key : otherKeys) {
        auto value = other.pImpl->config->get<ConfigValue>(key);
        if (value.has_value()) {
            pImpl->config->set(key, *value, ConfigSource::Runtime);
        }
    }
    
    return true;
}

bool ConfigurationManager::validateSchema(const json& schema) const {
    // For now, placeholder implementation
    std::cout << "Schema validation would be implemented here...";
    return true;
}

} // namespace nlm