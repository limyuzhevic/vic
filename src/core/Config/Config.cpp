#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), validationErrors() {
    loadDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        validationErrors.emplace_back(ConfigError::ErrorType::FileNotFound,
                                    "Could not open configuration file: " + filepath);
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Determine format
    bool isJSON = isJSON(content);
    bool isYAML = isYAML(content);
    bool isLegacy = !isJSON && !isYAML;
    
    if (isJSON) {
        return loadJSONFromString(content, filepath);
    } else if (isYAML) {
        return loadYAMLFromString(content, filepath);
    } else if (isLegacy) {
        return loadLegacyFromFile(filepath);
    }
    
    validationErrors.emplace_back(ConfigError::ErrorType::ParseError,
                                "Unknown configuration file format", "", filepath);
    return false;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (!setFromString(key, value, ConfigSource::CommandLine)) {
                    validationErrors.emplace_back(ConfigError::ErrorType::ParseError,
                                                "Failed to parse command line argument: " + arg,
                                                key);
                    return false;
                }
            } else {
                validationErrors.emplace_back(ConfigError::ErrorType::ParseError,
                                            "Invalid command line argument format: " + arg);
                return false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (!setFromString(key, value, ConfigSource::CommandLine)) {
                validationErrors.emplace_back(ConfigError::ErrorType::ParseError,
                                            "Failed to parse command line argument: " + arg,
                                            key);
                return false;
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        validationErrors.emplace_back(ConfigError::ErrorType::FileSaveError,
                                    "Could not save configuration to: " + filepath);
        return false;
    }
    
    // Write as JSON for consistency
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        ConfigSchemaHelper::ConfigParameterType paramType = ConfigSchemaHelper::getConfigParameterType(entry.value);
        const auto& paramDef = schema().getParameter(entry.key);
        
        if (paramDef.name.empty()) {
            // Unknown parameter, try to infer type
            std::visit([&](auto&& val) {
                if constexpr (std::is_same_v<decltype(val), int>) {
                    jsonData[entry.key] = val;
                } else if constexpr (std::is_same_v<decltype(val), int64_t>) {
                    jsonData[entry.key] = val;
                } else if constexpr (std::is_same_v<decltype(val), double>) {
                    jsonData[entry.key] = val;
                } else if constexpr (std::is_same_v<decltype(val), bool>) {
                    jsonData[entry.key] = val;
                } else if constexpr (std::is_same_v<decltype(val), std::string>) {
                    jsonData[entry.key] = val;
                } else if constexpr (std::is_same_v<decltype(val), std::vector<int>>) {
                    jsonData[entry.key] = val;
                } else if constexpr (std::is_same_v<decltype(val), std::vector<double>>) {
                    jsonData[entry.key] = val;
                } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
                    jsonData[entry.key] = val;
                }
            }, entry.value);
        } else {
            // Known parameter, use schema type
            std::string strValue = ConfigSchemaHelper::configValueToString(entry.value);
            jsonData[entry.key] = strValue;
        }
        
        // Add comment if description exists
        if (!entry.description.empty()) {
            jsonData[entry.key + "_description"] = entry.description;
        }
    }
    
    file << std::setw(4) << jsonData.dump(4);
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    set(key, value, source, true); // validate by default
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue configValue(value);
    set(key, configValue, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    ConfigValue configValue(value);
    set(key, configValue, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    ConfigValue configValue(value);
    set(key, configValue, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    ConfigValue configValue(value);
    set(key, configValue, source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    validationErrors.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    if (!validationErrors.empty()) {
        oss << "\nValidation Errors:\n";
        for (const auto& error : validationErrors) {
            oss << "  " << error.what() << "\n";
        }
    }
    
    return oss.str();
}

bool Config::loadJSONFromString(const std::string& jsonStr, const std::string& filepath) {
    try {
        auto jsonData = nlohmann::json::parse(jsonStr);
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            // Skip description fields
            if (key.find("_description") != std::string::npos) {
                continue;
            }
            
            std::string valueStr = it->dump();
            
            // Remove quotes from JSON string values
            if (valueStr.size() >= 2 && valueStr.front() == '"' && valueStr.back() == '"') {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            if (!setFromString(key, valueStr, ConfigSource::File)) {
                validationErrors.emplace_back(ConfigError::ErrorType::ParseError,
                                            "Failed to parse JSON value for parameter: " + key,
                                            key, filepath);
                return false;
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        validationErrors.emplace_back(ConfigError::ErrorType::ParseError,
                                    "JSON parse error: " + std::string(e.what()),
                                    "", filepath);
        return false;
    } catch (const std::exception& e) {
        validationErrors.emplace_back(ConfigError::ErrorType::ParseError,
                                    "Error parsing JSON: " + std::string(e.what()),
                                    "", filepath);
        return false;
    }
}

bool Config::loadYAMLFromString(const std::string& yamlStr, const std::string& filepath) {
    try {
        auto yamlData = YAML::Load(yamlStr);
        
        for (const auto& pair : yamlData) {
            std::string key = pair.first.as<std::string>();
            std::string valueStr;
            
            // Convert YAML value to string representation
            if (pair.second.IsScalar()) {
                valueStr = pair.second.as<std::string>();
            } else if (pair.second.IsSequence()) {
                // For sequences, serialize as JSON-like array
                std::stringstream ss;
                ss << "[";
                bool first = true;
                for (const auto& item : pair.second) {
                    if (!first) ss << ",";
                    ss << item.as<std::string>();
                    first = false;
                }
                ss << "]";
                valueStr = ss.str();
            } else {
                validationErrors.emplace_back(ConfigError::ErrorType::ParseError,
                                            "Unsupported YAML value type for parameter: " + key,
                                            key, filepath);
                return false;
            }
            
            if (!setFromString(key, valueStr, ConfigSource::File)) {
                validationErrors.emplace_back(ConfigError::ErrorType::ParseError,
                                            "Failed to parse YAML value for parameter: " + key,
                                            key, filepath);
                return false;
            }
        }
        
        return true;
    } catch (const YAML::Exception& e) {
        validationErrors.emplace_back(ConfigError::ErrorType::ParseError,
                                    "YAML parse error: " + std::string(e.what()),
                                    "", filepath);
        return false;
    } catch (const std::exception& e) {
        validationErrors.emplace_back(ConfigError::ErrorType::ParseError,
                                    "Error parsing YAML: " + std::string(e.what()),
                                    "", filepath);
        return false;
    }
}

bool Config::loadLegacyFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        validationErrors.emplace_back(ConfigError::ErrorType::FileNotFound,
                                    "Could not open configuration file (legacy): " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        std::string key, value;
        if (parseLegacyFormat(line, key, value)) {
            if (!setFromString(key, value, ConfigSource::File)) {
                validationErrors.emplace_back(ConfigError::ErrorType::ParseError,
                                            "Failed to parse legacy format at line " + 
                                            std::to_string(lineNum) + ": " + line,
                                            key, filepath);
                return false;
            }
        } else {
            validationErrors.emplace_back(ConfigError::ErrorType::ParseError,
                                        "Invalid legacy format at line " + 
                                        std::to_string(lineNum) + ": " + line,
                                        "", filepath);
            return false;
        }
    }
    
    return true;
}

bool Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Find or create parameter definition
    ConfigParameterDefinition paramDef = schema().getParameter(key);
    
    // Determine target type
    ConfigParameterType targetType;
    if (!paramDef.name.empty()) {
        targetType = paramDef.type;
    } else {
        // Try to infer type from value
        if (valueStr == "true" || valueStr == "false" || valueStr == "1" || valueStr == "0") {
            targetType = ConfigParameterType::Bool;
        } else if (valueStr.find('.') != std::string::npos) {
            try {
                std::stod(valueStr);
                targetType = ConfigParameterType::Double;
            } catch (...) {
                targetType = ConfigParameterType::String;
            }
        } else {
            try {
                std::stoll(valueStr);
                targetType = ConfigParameterType::Int64;
            } catch (...) {
                targetType = ConfigParameterType::String;
            }
        }
    }
    
    // Convert string to ConfigValue
    ConfigValue configValue;
    if (!ConfigSchemaHelper::stringToConfigValue(valueStr, targetType, configValue)) {
        return false;
    }
    
    // Validate against schema if parameter is defined
    if (paramDef.name.empty() || !ConfigSchemaHelper::validateParameterValue(key, configValue)) {
        return false;
    }
    
    // Check for duplicates
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Update existing entry
        it->value = configValue;
        it->source = source;
    } else {
        // Get description from schema if available
        std::string description = "";
        if (!paramDef.name.empty()) {
            description = paramDef.description;
        }
        
        // Add new entry
        pImpl->entries.emplace_back(key, configValue, source, description);
    }
    
    return true;
}

bool Config::validateFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return isJSON(content) || isYAML(content);
}

std::string Config::getValidationErrors() const {
    std::string result;
    for (const auto& error : validationErrors) {
        if (!result.empty()) {
            result += "\n";
        }
        result += error.what();
    }
    return result;
}

void Config::loadDefaultSchema() {
    // Load default schema from JSON file or initialize with built-in defaults
    const std::string defaultConfigJson = R"(
    {
        "random_seed": {"type": "int", "default": 42, "description": "Random seed for reproducible experiments"},
        "simulation_timestep": {"type": "double", "default": 0.001, "description": "Simulation timestep in seconds"},
        "neuron_count": {"type": "int", "default": 1000, "description": "Number of neurons in the brain"},
        "region_count": {"type": "int", "default": 2, "description": "Number of brain regions"},
        "connection_probability": {"type": "double", "default": 0.1, "description": "Connection probability for random connectivity"},
        "plasticity_learning_rate": {"type": "double", "default": 0.01, "description": "Plasticity learning rate"},
        "stdp_ltp_weight": {"type": "double", "default": 0.01, "description": "STDP LTP weight"},
        "stdp_ltd_weight": {"type": "double", "default": 0.012, "description": "STDP LTD weight"},
        "stdp_time_constant": {"type": "double", "default": 20.0, "description": "STDP time constant"},
        "development_synaptogenesis_rate": {"type": "double", "default": 0.001, "description": "Development synaptogenesis rate"},
        "development_pruning_rate": {"type": "double", "default": 0.0001, "description": "Development pruning rate"},
        "dopamine_baseline": {"type": "double", "default": 0.0, "description": "Dopamine baseline"},
        "reward_discount_factor": {"type": "double", "default": 0.99, "description": "Reward discount factor"},
        "environment_name": {"type": "string", "default": "GridWorld", "description": "Environment name"},
        "environment_width": {"type": "int", "default": 10, "description": "Environment width"},
        "environment_height": {"type": "int", "default": 10, "description": "Environment height"},
        "log_level": {"type": "string", "default": "INFO", "description": "Log level"},
        "log_to_file": {"type": "bool", "default": false, "description": "Log to file"},
        "log_filename": {"type": "string", "default": "nlm.log", "description": "Log filename"},
        "max_simulation_steps": {"type": "int", "default": 10000, "description": "Maximum simulation steps"},
        "simulation_time_limit": {"type": "double", "default": 0.0, "description": "Simulation time limit"},
        "visualization_enabled": {"type": "bool", "default": false, "description": "Visualization enabled"},
        "visualization_update_rate": {"type": "double", "default": 30.0, "description": "Visualization update rate"}
    }
    )";
    
    try {
        auto jsonData = nlohmann::json::parse(defaultConfigJson);
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            std::string valueStr = it->dump();
            
            // Remove quotes from string values
            if (valueStr.size() >= 2 && valueStr.front() == '"' && valueStr.back() == '"') {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            // Get type from JSON
            std::string typeStr = jsonData[key]["type"].get<std::string>();
            ConfigParameterType paramType;
            if (typeStr == "int") {
                paramType = ConfigParameterType::Int;
            } else if (typeStr == "int64") {
                paramType = ConfigParameterType::Int64;
            } else if (typeStr == "double") {
                paramType = ConfigParameterType::Double;
            } else if (typeStr == "bool") {
                paramType = ConfigParameterType::Bool;
            } else if (typeStr == "string") {
                paramType = ConfigParameterType::String;
            } else if (typeStr == "vector<int>") {
                paramType = ConfigParameterType::VectorInt;
            } else if (typeStr == "vector<double>") {
                paramType = ConfigParameterType::VectorDouble;
            } else if (typeStr == "vector<string>") {
                paramType = ConfigParameterType::VectorString;
            } else {
                paramType = ConfigParameterType::String;
            }
            
            // Get default value if present
            std::optional<std::string> defaultValue;
            if (jsonData[key].contains("default")) {
                defaultValue = jsonData[key]["default"].get<std::string>();
            }
            
            // Get description
            std::string description = "";
            if (jsonData[key].contains("description")) {
                description = jsonData[key]["description"].get<std::string>();
            }
            
            // Register parameter
            ConfigParameterDefinition paramDef(key, paramType, description, defaultValue);
            schema().registerParameter(paramDef);
        }
    } catch (...) {
        // If schema loading fails, continue with minimal schema
    }
}

bool Config::parseJSON(const std::string& jsonStr, std::unordered_map<std::string, std::string>& outValues) {
    try {
        auto jsonData = nlohmann::json::parse(jsonStr);
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            std::string valueStr = it->dump();
            
            // Remove quotes from string values
            if (valueStr.size() >= 2 && valueStr.front() == '"' && valueStr.back() == '"') {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            outValues[key] = valueStr;
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::parseYAML(const std::string& yamlStr, std::unordered_map<std::string, std::string>& outValues) {
    try {
        auto yamlData = YAML::Load(yamlStr);
        
        for (const auto& pair : yamlData) {
            std::string key = pair.first.as<std::string>();
            std::string valueStr;
            
            if (pair.second.IsScalar()) {
                valueStr = pair.second.as<std::string>();
            } else if (pair.second.IsSequence()) {
                std::stringstream ss;
                ss << "[";
                bool first = true;
                for (const auto& item : pair.second) {
                    if (!first) ss << ",";
                    ss << item.as<std::string>();
                    first = false;
                }
                ss << "]";
                valueStr = ss.str();
            }
            
            outValues[key] = valueStr;
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::parseLegacyFormat(const std::string& line, std::string& key, std::string& value) {
    size_t pos = line.find('=');
    if (pos != std::string::npos) {
        key = trim(line.substr(0, pos));
        value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        return !key.empty();
    }
    return false;
}

bool Config::isJSON(const std::string& content) {
    if (content.empty()) return false;
    
    // Check for JSON array or object
    auto trimmed = trim(content);
    if (trimmed.front() == '{' && trimmed.back() == '}') return true;
    if (trimmed.front() == '[' && trimmed.back() == ']') return true;
    
    // Try to parse as JSON to be sure
    try {
        nlohmann::json::parse(content);
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::isYAML(const std::string& content) {
    if (content.empty()) return false;
    
    auto trimmed = trim(content);
    
    // Check for YAML indicators
    return trimmed.find(':') != std::string::npos || 
           trimmed.find('-') != std::string::npos ||
           trimmed.find('[') != std::string::npos ||
           trimmed.find('{') != std::string::npos;
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Helper function implementations
namespace ConfigSchemaHelper {
    bool stringToConfigValue(const std::string& valueStr, ConfigParameterType type, ConfigValue& outValue) {
        return schema().convertValue(valueStr, type, outValue);
    }
    
    std::string configValueToString(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ",";
                    oss << item;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ",";
                    oss << std::setprecision(6) << item;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ",";
                    oss << "\"" << item << "\"";
                    first = false;
                }
                oss << "]";
            }
        }, value);
        return oss.str();
    }
    
    ConfigParameterType getConfigParameterType(const ConfigValue& value) {
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                // Return Int type
            } else if constexpr (std::is_same_v<T, int64_t>) {
                // Return Int64 type
            } else if constexpr (std::is_same_v<T, double>) {
                // Return Double type
            } else if constexpr (std::is_same_v<T, bool>) {
                // Return Bool type
            } else if constexpr (std::is_same_v<T, std::string>) {
                // Return String type
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                // Return VectorInt type
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                // Return VectorDouble type
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                // Return VectorString type
            }
        }, value);
        // Return String as default
        return ConfigParameterType::String;
    }
    
    bool validateParameterValue(const std::string& name, const ConfigValue& value) {
        // This would need access to the schema - implementation would go here
        return true;
    }
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

