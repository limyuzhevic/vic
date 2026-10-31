#include "ConfigManager.hpp"
#include "Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <filesystem>
#include <unordered_map>

namespace nlm {

bool ConfigManager::loadConfig(const std::string& filepath, std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("ConfigManager: Cannot load config - config is null");
        return false;
    }
    
    // Try to detect file format by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("ConfigManager: Cannot open JSON file: " + filepath);
            return false;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return loadFromJSON(buffer.str(), config);
    } else if (ext == ".yaml" || ext == ".yml") {
        // Placeholder for YAML support
        NLM_LOG_ERROR("ConfigManager: YAML support not yet implemented");
        return false;
    } else {
        // Default to key=value format
        return config->loadFromFile(filepath);
    }
}

bool ConfigManager::loadFromJSON(const std::string& jsonString, std::shared_ptr<Config> config) {
    if (!config) {
        return false;
    }
    
    try {
        auto json = nlohmann::json::parse(jsonString);
        
        // Convert JSON to config entries
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            
            // Convert JSON value to ConfigValue
            ConfigValue value;
            if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_unsigned()) {
                value = it->get<uint64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_array()) {
                // Convert array to vector of strings (simplified)
                std::vector<std::string> strArray;
                for (const auto& item : *it) {
                    strArray.push_back(item.get<std::string>());
                }
                value = strArray;
            } else {
                // Skip complex objects for now
                continue;
            }
            
            config->set(key, value, ConfigSource::File);
        }
        
        NLM_LOG_INFO("ConfigManager: Loaded " + std::to_string(json.size()) + " entries from JSON");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConfigManager: Failed to parse JSON: " + std::string(e.what()));
        return false;
    }
}

bool ConfigManager::loadFromCommandLine(int argc, char** argv, std::shared_ptr<Config> config) {
    if (!config) {
        return false;
    }
    
    // Use the existing Config::loadFromArgs method
    return config->loadFromArgs(argc, argv);
}

ConfigManager::ValidationResult ConfigManager::validateConfig(const std::shared_ptr<Config>& config, const std::vector<ConfigSchema>& schema) {
    ValidationResult result;
    
    if (!config) {
        result.valid = false;
        result.errorMessage = "Config is null";
        return result;
    }
    
    // Check for missing required fields
    for (const auto& schemaEntry : schema) {
        if (schemaEntry.required && !config->has(schemaEntry.name)) {
            result.valid = false;
            result.errorMessage = "Missing required config field: " + schemaEntry.name;
            return result;
        }
    }
    
    // Validate against schema validators
    for (const auto& schemaEntry : schema) {
        if (config->has(schemaEntry.name) && !schemaEntry.validators.empty()) {
            auto valueOpt = config->get<ConfigValue>(schemaEntry.name);
            if (valueOpt) {
                for (const auto& validator : schemaEntry.validators) {
                    auto validationResult = validator(*valueOpt);
                    if (!validationResult.valid) {
                        result.valid = false;
                        result.errorMessage = "Validation failed for field '" + schemaEntry.name + "': " + validationResult.errorMessage;
                        return result;
                    }
                    // Collect warnings
                    result.warnings.insert(result.warnings.end(), validationResult.warnings.begin(), validationResult.warnings.end());
                }
            }
        }
    }
    
    return result;
}

std::shared_ptr<Config> ConfigManager::getDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Set default values from configs/default.cfg
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_time_constant", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.001f, ConfigSource::Default);
    config->set("pruning_rate", 0.0001f, ConfigSource::Default);
    config->set("log_level", std::string("INFO"), ConfigSource::Default);
    
    NLM_LOG_INFO("ConfigManager: Created default configuration");
    return config;
}

bool ConfigManager::createConfigTemplate(const std::string& filepath, const std::vector<ConfigSchema>& schema) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("ConfigManager: Cannot create config template: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration Template\n";
    file << "# Generated by ConfigManager\n\n";
    
    for (const auto& entry : schema) {
        file << "# " << entry.description << "\n";
        if (entry.required) {
            file << "# REQUIRED\n";
        }
        file << entry.name << " = PLACEHOLDER_VALUE\n\n";
    }
    
    file.close();
    NLM_LOG_INFO("ConfigManager: Created config template at: " + filepath);
    return true;
}

std::string ConfigManager::toString(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, float>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return "[int array]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return "[double array]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return "[string array]";
        } else {
            return "[unknown]";
        }
    }, value);
}

double ConfigManager::toDouble(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> double {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, float>) {
            return static_cast<double>(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return static_cast<double>(arg);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return static_cast<double>(arg);
        } else {
            return 0.0;
        }
    }, value);
}

int64_t ConfigManager::toInt64(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> int64_t {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return static_cast<int64_t>(arg);
        } else if constexpr (std::is_same_v<T, float>) {
            return static_cast<int64_t>(arg);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return static_cast<int64_t>(arg);
        } else {
            return 0;
        }
    }, value);
}

bool ConfigManager::toBool(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::string lower = arg;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            return lower == "true" || lower == "1" || lower == "yes";
        } else if constexpr (std::is_same_v<T, double>) {
            return arg != 0.0;
        } else {
            return false;
        }
    }, value);
}

std::string ConfigManager::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string ConfigManager::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool ConfigManager::parseKeyValue(const std::string& line, std::string& key, std::string& value) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return false;
    }
    
    key = trim(line.substr(0, pos));
    value = trim(line.substr(pos + 1));
    
    // Remove quotes
    if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                             (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    return !key.empty();
}

} // namespace nlm