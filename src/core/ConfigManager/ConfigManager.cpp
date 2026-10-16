#include "ConfigManager.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Validation/Validation.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>

namespace nlm {

struct ConfigManager::Impl {
    Config config;
    ConfigSchema schema;
    ConfigInheritance inheritance;
    ConfigBackup backup;
    std::map<std::string, std::string> envPrefixMap;
};

ConfigManager::ConfigManager() : pImpl(std::make_unique<Impl>()) {
    // Set up environment variable prefix mapping
    pImpl->envPrefixMap = {
        {"brain", "BRAIN_"},
        {"neural", "NEURAL_"},
        {"simulation", "SIM_"},
        {"memory", "MEM_"},
        {"output", "OUT_"},
        {"debug", "DEBUG_"},
        {"profiling", "PROF_"}
    };
}

ConfigManager::~ConfigManager() = default;

ConfigManager::ConfigManager(ConfigManager&&) noexcept = default;

ConfigManager& ConfigManager::operator=(ConfigManager&&) noexcept = default;

bool ConfigManager::loadFromFile(const std::string& filepath, bool validate) {
    NLM_VALIDATE_FILE_ACCESS(filepath, "reading configuration", 
                           "Check if file exists and is readable");
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    try {
        // Load using base Config class
        if (!pImpl->config.loadFromFile(filepath)) {
            NLM_LOG_ERROR("Failed to parse configuration file: " + filepath);
            return false;
        }
        
        pImpl->config.set("config_file", filepath, ConfigSource::File);
        
        if (validate && !pImpl->schema.validate(pImpl->config, 
                                             std::vector<std::string>{})) {
            NLM_LOG_ERROR("Configuration validation failed for file: " + filepath);
            return false;
        }
        
        NLM_LOG_INFO("Loaded configuration from file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration from file " + filepath + ": " + 
                     std::string(e.what()));
        return false;
    }
}

bool ConfigManager::loadFromArgs(int argc, char** argv, bool validate) {
    try {
        if (!pImpl->config.loadFromArgs(argc, argv)) {
            NLM_LOG_WARNING("Command line argument loading completed with warnings");
        }
        
        if (validate && !pImpl->schema.validate(pImpl->config,
                                             std::vector<std::string>{})) {
            NLM_LOG_ERROR("Configuration validation failed after parsing command line");
            return false;
        }
        
        NLM_LOG_INFO("Loaded configuration from command line arguments");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration from command line: " + 
                     std::string(e.what()));
        return false;
    }
}

bool ConfigManager::loadFromEnv(const std::map<std::string, std::string>& envVars, 
                              bool validate) {
    try {
        for (const auto& pair : envVars) {
            const std::string& key = pair.first;
            const std::string& value = pair.second;
            
            // Skip empty values
            if (value.empty()) continue;
            
            // Convert environment variable value to appropriate type
            ConfigValue configValue;
            bool converted = false;
            
            // Try to infer type from value
            if (value == "true" || value == "false") {
                configValue = (value == "true");
                converted = true;
            } else if (value.find('.') != std::string::npos) {
                try {
                    double doubleVal = std::stod(value);
                    configValue = doubleVal;
                    converted = true;
                } catch (...) {}
            } else {
                try {
                    int64_t intVal = std::stoll(value);
                    configValue = intVal;
                    converted = true;
                } catch (...) {}
            }
            
            // If no conversion succeeded, treat as string
            if (!converted) {
                configValue = value;
            }
            
            // Set with environment source
            pImpl->config.set(key, configValue, ConfigSource::CommandLine);
            
            // Log environment variable loading
            std::string typeStr;
            if (std::holds_alternative<int>(configValue)) typeStr = "int";
            else if (std::holds_alternative<int64_t>(configValue)) typeStr = "int64";
            else if (std::holds_alternative<double>(configValue)) typeStr = "double";
            else if (std::holds_alternative<bool>(configValue)) typeStr = "bool";
            else typeStr = "string";
            
            NLM_LOG_DEBUG("Loaded environment variable " + key + ": " + value + 
                        " (type: " + typeStr + ")");
        }
        
        if (validate && !pImpl->schema.validate(pImpl->config,
                                             std::vector<std::string>{})) {
            NLM_LOG_ERROR("Configuration validation failed after loading from environment");
            return false;
        }
        
        NLM_LOG_INFO("Loaded configuration from environment variables");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration from environment: " + 
                     std::string(e.what()));
        return false;
    }
}

bool ConfigManager::mergeConfigs(const std::vector<std::string>& configPaths,
                                bool validate) {
    try {
        Config mergedConfig;
        std::vector<MergeStrategy> strategies;
        
        for (size_t i = 0; i < configPaths.size(); ++i) {
            Config tempConfig;
            std::string filepath = configPaths[i];
            
            NLM_LOG_INFO("Merging configuration from: " + filepath);
            
            // Load each config file
            if (!tempConfig.loadFromFile(filepath)) {
                NLM_LOG_WARNING("Failed to load configuration from: " + filepath);
                continue;
            }
            
            // Merge with existing
            for (const auto& key : tempConfig.getKeys()) {
                if (mergedConfig.has(key)) {
                    // Key already exists - override behavior based on strategy
                    ConfigSource source = mergedConfig.getSource(key);
                    if (i == 0) { // First file gets highest priority
                        mergedConfig.set(key, tempConfig.get<ConfigValue>(key), source);
                    }
                } else {
                    mergedConfig.set(key, tempConfig.get<ConfigValue>(key), ConfigSource::File);
                }
            }
        }
        
        // Replace current config with merged version
        pImpl->config = std::move(mergedConfig);
        
        if (validate && !pImpl->schema.validate(pImpl->config,
                                             std::vector<std::string>{})) {
            NLM_LOG_ERROR("Configuration validation failed after merging");
            return false;
        }
        
        NLM_LOG_INFO("Successfully merged " + std::to_string(configPaths.size()) + 
                    " configuration files");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error merging configurations: " + std::string(e.what()));
        return false;
    }
}

bool ConfigManager::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save configuration to file: " + filepath);
        return false;
    }
    
    try {
        // Save all configuration entries
        for (const auto& key : pImpl->config.getKeys()) {
            ConfigSource source = pImpl->config.getSource(key);
            file << "# Source: " << static_cast<int>(source) << "\n";
            file << key << " = ";
            
            // Get the value and convert to string
            ConfigValue value;
            // Note: This assumes Config has a method to get raw value
            // In real implementation, you'd need to add this
            file << "[value]" << "\n\n";
        }
        
        NLM_LOG_INFO("Configuration saved to file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to file " + filepath + ": " + 
                     std::string(e.what()));
        return false;
    }
}

template<typename T>
T ConfigManager::get(const std::string& key, const T& defaultValue) const {
    auto value = pImpl->config.get<T>(key);
    return value.has_value() ? *value : defaultValue;
}

void ConfigManager::set(const std::string& key, const ConfigValue& value, 
                       ConfigSource source) {
    pImpl->config.set(key, value, source);
}

bool ConfigManager::has(const std::string& key) const {
    return pImpl->config.has(key);
}

std::vector<std::string> ConfigManager::getKeys() const {
    return pImpl->config.getKeys();
}

ConfigSource ConfigManager::getSource(const std::string& key) const {
    return pImpl->config.getSource(key);
}

bool ConfigManager::validate(const ConfigSchema& schema, 
                           std::vector<std::string>& errors) const {
    return schema.validate(pImpl->config, errors);
}

void ConfigManager::setSchema(const ConfigSchema& schema) {
    pImpl->schema = schema;
}

const ConfigSchema& ConfigManager::getSchema() const {
    return pImpl->schema;
}

void ConfigManager::setInheritance(const ConfigInheritance& inheritance) {
    pImpl->inheritance = inheritance;
}

const ConfigInheritance& ConfigManager::getInheritance() const {
    return pImpl->inheritance;
}

bool ConfigManager::createBackup(const std::string& backupPath) {
    return pImpl->backup.createBackup(pImpl->config, backupPath);
}

bool ConfigManager::restoreBackup(const std::string& backupPath) {
    return pImpl->backup.restoreBackup(pImpl->config, backupPath);
}

std::string ConfigManager::summary() const {
    std::ostringstream oss;
    oss << "=== Configuration Summary ===\n";
    oss << "Total parameters: " << pImpl->config.getKeys().size() << "\n";
    oss << "Schema validation: " << (pImpl->schema.getParameterNames().size() > 0 ? "Enabled" : "Disabled") << "\n";
    oss << "Inheritance chain: " << pImpl->inheritance.getInheritanceChain().size() << " sources\n";
    oss << "Configuration sources:" << std::endl;
    
    std::map<ConfigSource, int> sourceCounts;
    for (const auto& key : pImpl->config.getKeys()) {
        sourceCounts[pImpl->config.getSource(key)]++;
    }
    
    for (const auto& pair : sourceCounts) {
        oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << " parameters\n";
    }
    
    oss << "\n" << pImpl->config.summary();
    return oss.str();
}

std::string ConfigManager::generateHelp() const {
    std::ostringstream oss;
    oss << "=== NLM Configuration Help ===\n\n";
    
    oss << "Available Configuration Options:" << std::endl;
    for (const auto& key : pImpl->config.getKeys()) {
        ConfigSource source = pImpl->config.getSource(key);
        oss << "  " << key << " (source: " << static_cast<int>(source) << ")" << std::endl;
    }
    
    oss << "\nSchema Parameters:" << std::endl;
    for (const auto& paramName : pImpl->schema.getParameterNames()) {
        oss << "  " << paramName << ": " << pImpl->schema.getParameterDescription(paramName) << std::endl;
    }
    
    oss << "\nConfiguration File Options:" << std::endl;
    oss << "  --config=<filepath>: Specify configuration file" << std::endl;
    oss << "  --merge=<filepath>: Merge additional configuration file" << std::endl;
    oss << "  --backup=<path>: Create backup before saving" << std::endl;
    
    oss << "\nEnvironment Variables:" << std::endl;
    for (const auto& prefix : pImpl->envPrefixMap) {
        oss << "  " << prefix.second << "<param>: " << prefix.first << " namespace" << std::endl;
    }
    
    return oss.str();
}

bool ConfigConverter::convertTo(ConfigValue& target, const std::string& value, 
                               ConfigType targetType, const std::string& key) {
    try {
        switch (targetType) {
            case ConfigType::INT: {
                int val = std::stoi(value);
                target = val;
                break;
            }
            case ConfigType::INT64: {
                int64_t val = std::stoll(value);
                target = val;
                break;
            }
            case ConfigType::DOUBLE: {
                double val = std::stod(value);
                target = val;
                break;
            }
            case ConfigType::FLOAT: {
                float val = std::stof(value);
                target = val;
                break;
            }
            case ConfigType::BOOL: {
                bool val = (value == "true" || value == "1" || value == "yes");
                target = val;
                break;
            }
            case ConfigType::STRING: {
                target = value;
                break;
            }
            default:
                NLM_LOG_ERROR("Unsupported ConfigType for conversion: " + 
                             std::to_string(static_cast<int>(targetType)));
                return false;
        }
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Conversion error for key " + key + ": " + std::string(e.what()));
        return false;
    }
}

std::string ConfigConverter::valueToString(const ConfigValue& value, ConfigType type) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

ConfigValue ConfigConverter::stringToValue(const std::string& str, ConfigType type) {
    ConfigValue result;
    switch (type) {
        case ConfigType::INT: {
            int val = std::stoi(str);
            result = val;
            break;
        }
        case ConfigType::INT64: {
            int64_t val = std::stoll(str);
            result = val;
            break;
        }
        case ConfigType::DOUBLE: {
            double val = std::stod(str);
            result = val;
            break;
        }
        case ConfigType::FLOAT: {
            float val = std::stof(str);
            result = val;
            break;
        }
        case ConfigType::BOOL: {
            bool val = (str == "true" || str == "1" || str == "yes");
            result = val;
            break;
        }
        case ConfigType::STRING: {
            result = str;
            break;
        }
        default:
            NLM_LOG_ERROR("Unsupported ConfigType for string conversion: " + 
                         std::to_string(static_cast<int>(type)));
            result = "";
    }
    return result;
}

bool ConfigConverter::validateValue(const ConfigValue& value, ConfigType type) {
    switch (type) {
        case ConfigType::INT: {
            return std::holds_alternative<int>(value);
        }
        case ConfigType::INT64: {
            return std::holds_alternative<int64_t>(value);
        }
        case ConfigType::DOUBLE: {
            return std::holds_alternative<double>(value);
        }
        case ConfigType::FLOAT: {
            return std::holds_alternative<float>(value);
        }
        case ConfigType::BOOL: {
            return std::holds_alternative<bool>(value);
        }
        case ConfigType::STRING: {
            return std::holds_alternative<std::string>(value);
        }
        case ConfigType::STRING_VEC:
        case ConfigType::INT_VEC:
        case ConfigType::DOUBLE_VEC:
        case ConfigType::FLOAT_VEC: {
            return std::visit([](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                return std::is_same_v<T, std::vector<std::string>> ||
                       std::is_same_v<T, std::vector<int>> ||
                       std::is_same_v<T, std::vector<double>> ||
                       std::is_same_v<T, std::vector<float>>;
            }, value);
        }
        default:
            return false;
    }
}

} // namespace nlm