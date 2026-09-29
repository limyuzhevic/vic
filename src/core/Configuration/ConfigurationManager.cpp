#include "ConfigurationManager.hpp"
#include <sstream>
#include <iostream>

namespace nlm {

namespace config {

// Static member definitions
const std::string ConfigurationManager::ENVIRONMENT_PREFIX = "NLM_";

// ConfigurationManager implementation
ConfigurationManager::ConfigurationManager() : config_(std::make_shared<Config>()),
                                           schema_(std::make_shared<ConfigSchema>()),
                                           currentVersion_(1, 0, 0) {
    initializeDefaultConfig();
}

ConfigurationManager::ConfigurationManager(const std::string& filepath) : config_(std::make_shared<Config>()),
                                                                           schema_(std::make_shared<ConfigSchema>()),
                                                                           currentVersion_(1, 0, 0) {
    loadFromFile(filepath);
}

bool ConfigurationManager::loadFromFile(const std::string& filepath) {
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

bool ConfigurationManager::loadFromEnvironment() {
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

bool ConfigurationManager::saveToFile(const std::string& filepath) const {
    return config_->saveToFile(filepath);
}

void ConfigurationManager::initializeDefaultConfig() {
    // Set default values for all parameters
    for (const auto& pair : schema_->getParameterNames()) {
        const ParameterMetadata* metadata = schema_->getParameter(pair);
        if (!metadata) continue;
        
        config_->set(pair, metadata->defaultValue, nlm::ConfigSource::Default);
    }
}

bool ConfigurationManager::upgradeConfiguration(const Version& targetVersion) {
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

bool ConfigurationManager::upgradeToVersion(const Version& targetVersion) {
    // Implement version-specific upgrades
    // For now, just log the upgrade attempt
    NLM_LOG_INFO(std::string("Upgrading configuration to version ") + targetVersion.toString());
    return true;
}

nlm::ConfigValue ConfigurationManager::stringToConfigValue(const std::string& str, ParameterMetadata::Type type) const {
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

std::string ConfigurationManager::configValueToString(const nlm::ConfigValue& value) const {
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

std::string ConfigurationManager::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

} // namespace config

} // namespace nlm
