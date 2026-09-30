#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <regex>
#include <iomanip>
#include <ctime>
#include <memory>
#include <cmath>
#include <numeric>

// Optional includes for JSON/YAML support
#ifdef NLOHMANN_JSON_HPP
#include "nlohmann/json.hpp"
#endif

#ifdef YAML_CPP_HPP
#include "yaml-cpp/yaml.h"
#endif

namespace nlm {

// Helper function to get file extension
static std::string getFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of(".");
    if (pos == std::string::npos) return "";
    return filename.substr(pos + 1);
}

// Helper function to check if string ends with suffix
static bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// Helper function to trim whitespace
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper function to convert to lowercase
static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// ConfigTypeUtils implementation
std::string ConfigTypeUtils::valueToString(const ConfigValue& value) {
    std::stringstream ss;
    std::visit([&ss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << '"' << arg << '"';
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << std::fixed << std::setprecision(6) << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << '"' << arg[i] << '"';
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<ConfigValue>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << valueToString(arg[i]);
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::map<std::string, ConfigValue>>) {
            ss << "{";
            for (auto it = arg.begin(); it != arg.end(); ++it) {
                if (it != arg.begin()) ss << ", ";
                ss << '"' << it->first << "": valueToString(it->second);
            }
            ss << "}";
        } else {
            ss << arg;
        }
    }, value);
    return ss.str();
}

bool ConfigTypeUtils::tryConvertTo(ConfigValue& value, const std::string& typeName) {
    // Try to convert value to specified type
    std::string type = toLower(typeName);
    
    if (type == "auto") return true;  // No conversion needed
    
    // Helper to check and convert type
    auto tryConvert = [&](auto& result, const std::function<bool(const ConfigValue&)>& predicate) {
        if (predicate(result)) return true;
        return false;
    };
    
    try {
        if (type == "int" || type == "integer") {
            if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value) || std::holds_alternative<double>(value)) {
                // Convert to int
                if (std::holds_alternative<double>(value)) {
                    double val = std::get<double>(value);
                    value = static_cast<int>(val);
                } else if (std::holds_alternative<int64_t>(value)) {
                    int64_t val = std::get<int64_t>(value);
                    value = static_cast<int>(val);
                }
                return true;
            }
        } else if (type == "double" || type == "float") {
            if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value) || std::holds_alternative<double>(value)) {
                if (std::holds_alternative<int>(value)) {
                    int val = std::get<int>(value);
                    value = static_cast<double>(val);
                } else if (std::holds_alternative<int64_t>(value)) {
                    int64_t val = std::get<int64_t>(value);
                    value = static_cast<double>(val);
                }
                return true;
            }
        } else if (type == "bool" || type == "boolean") {
            if (std::holds_alternative<bool>(value) || std::holds_alternative<std::string>(value)) {
                if (std::holds_alternative<std::string>(value)) {
                    std::string str = std::get<std::string>(value);
                    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
                    if (str == "true" || str == "yes" || str == "1") {
                        value = true;
                        return true;
                    } else if (str == "false" || str == "no" || str == "0") {
                        value = false;
                        return true;
                    }
                }
                return true;
            }
        } else if (type == "string") {
            if (!std::holds_alternative<std::string>(value)) {
                value = valueToString(value);
                return true;
            }
            return true;
        }
    } catch (...) {
        return false;
    }
    
    return false;
}

bool ConfigTypeUtils::isNumericType(const ConfigValue& value) {
    return std::holds_alternative<int>(value) ||
           std::holds_alternative<int64_t>(value) ||
           std::holds_alternative<double>(value);
}

bool ConfigTypeUtils::isContainerType(const ConfigValue& value) {
    return std::holds_alternative<std::vector<int>>(value) ||
           std::holds_alternative<std::vector<double>>(value) ||
           std::holds_alternative<std::vector<std::string>>(value) ||
           std::holds_alternative<std::vector<ConfigValue>>(value) ||
           std::holds_alternative<std::map<std::string, ConfigValue>>(value) ||
           std::holds_alternative<std::vector<std::map<std::string, ConfigValue>>>(value);
}

bool ConfigTypeUtils::isObjectType(const ConfigValue& value) {
    return std::holds_alternative<std::map<std::string, ConfigValue>>(value) ||
           std::holds_alternative<std::vector<std::map<std::string, ConfigValue>>>(value);
}

std::string ConfigTypeUtils::getTypeName(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "std::vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "std::vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "std::vector<std::string>";
    if (std::holds_alternative<std::vector<ConfigValue>>(value)) return "std::vector<ConfigValue>";
    if (std::holds_alternative<std::map<std::string, ConfigValue>>(value)) return "std::map<std::string, ConfigValue>";
    if (std::holds_alternative<std::vector<std::map<std::string, ConfigValue>>>(value)) return "std::vector<std::map<std::string, ConfigValue>>";
    return "unknown";
}

std::string ConfigTypeUtils::getTypeName(const ConfigSchemaEntry& entry) {
    return entry.type;
}

std::vector<std::string> ConfigTypeUtils::getAllTypeNames() {
    return {"int", "int64_t", "double", "bool", "string", "std::vector<int>", "std::vector<double>", "std::vector<std::string>", "std::vector<ConfigValue>", "std::map<std::string, ConfigValue>", "std::vector<std::map<std::string, ConfigValue>>"};
}

std::vector<std::string> ConfigTypeUtils::getNumericTypeNames() {
    return {"int", "int64_t", "double"};
}

std::vector<std::string> ConfigTypeUtils::getContainerTypeNames() {
    return {"std::vector<int>", "std::vector<double>", "std::vector<std::string>", "std::vector<ConfigValue>", "std::map<std::string, ConfigValue>", "std::vector<std::map<std::string, ConfigValue>>"};
}

std::vector<std::string> ConfigTypeUtils::getObjectTypeNames() {
    return {"std::map<std::string, ConfigValue>", "std::vector<std::map<std::string, ConfigValue>>"};
}

// ConfigComparisonUtils implementation
bool ConfigComparisonUtils::valuesEqual(const ConfigValue& a, const ConfigValue& b) {
    if (std::holds_alternative<double>(a) && std::holds_alternative<double>(b)) {
        double val1 = std::get<double>(a);
        double val2 = std::get<double>(b);
        return std::abs(val1 - val2) < 1e-9;
    }
    return a == b;
}

double ConfigComparisonUtils::valuesDiff(const ConfigValue& a, const ConfigValue& b) {
    if (std::holds_alternative<double>(a) && std::holds_alternative<double>(b)) {
        double val1 = std::get<double>(a);
        double val2 = std::get<double>(b);
        return std::abs(val1 - val2);
    }
    return 0.0;
}

std::map<std::string, double> ConfigComparisonUtils::compareConfigs(const Config& a, const Config& b) {
    std::map<std::string, double> differences;
    
    // Compare keys from first config
    auto keysA = a.getKeys();
    auto keysB = b.getKeys();
    
    for (const auto& key : keysA) {
        if (b.has(key)) {
            // Get values and compare
            auto valueA = a.get(key);
            auto valueB = b.get(key);
            
            if (valueA.has_value() && valueB.has_value()) {
                differences[key] = valuesDiff(*valueA, *valueB);
            }
        }
    }
    
    // Find keys only in second config
    for (const auto& key : keysB) {
        if (!a.has(key)) {
            differences[key] = 1.0;  // New key
        }
    }
    
    return differences;
}

std::map<std::string, std::string> ConfigComparisonUtils::generateDiffReport(const Config& oldConfig, const Config& newConfig) {
    std::map<std::string, std::string> diffReport;
    
    auto keysOld = oldConfig.getKeys();
    auto keysNew = newConfig.getKeys();
    
    for (const auto& key : keysOld) {
        if (newConfig.has(key)) {
            auto valOld = oldConfig.get(key);
            auto valNew = newConfig.get(key);
            
            if (valOld && valNew) {
                if (!valuesEqual(*valOld, *valNew)) {
                    diffReport[key] = "CHANGED: " + ConfigTypeUtils::valueToString(*valOld) + " -> " + ConfigTypeUtils::valueToString(*valNew);
                }
            }
        } else {
            diffReport[key] = "REMOVED: " + ConfigTypeUtils::valueToString(*valOld);
        }
    }
    
    for (const auto& key : keysNew) {
        if (!oldConfig.has(key)) {
            auto valNew = newConfig.get(key);
            if (valNew) {
                diffReport[key] = "ADDED: " + ConfigTypeUtils::valueToString(*valNew);
            }
        }
    }
    
    return diffReport;
}

// ConfigFactoryUtils implementation
std::shared_ptr<Config> ConfigFactoryUtils::createFromDictionary(const std::map<std::string, std::string>& dict) {
    auto config = std::make_shared<Config>();
    for (const auto& pair : dict) {
        config->setWithType(pair.first, pair.second, "auto");
    }
    return config;
}

std::shared_ptr<Config> ConfigFactoryUtils::createFromNestedDictionary(const std::map<std::string, std::map<std::string, std::string>>& nestedDict) {
    auto config = std::make_shared<Config>();
    for (const auto& outerPair : nestedDict) {
        for (const auto& innerPair : outerPair.second) {
            std::string key = outerPair.first + "." + innerPair.first;
            config->setWithType(key, innerPair.second, "auto");
        }
    }
    return config;
}

std::shared_ptr<Config> ConfigFactoryUtils::createDefaultConfig(const std::string& context) {
    auto config = std::make_shared<Config>();
    config->updateMetadata("0.1.0", context);
    config->set("brain.neuron_count", 100);
    config->set("brain.region_count", 1);
    config->set("brain.synapse_density", 0.1);
    config->set("brain.connection_probability", 0.2);
    config->set("brain.membrane_potential_rest", -65.0);
    config->set("brain.membrane_potential_threshold", -50.0);
    config->set("brain.refractory_period", 5);
    config->set("brain.max_firing_rate", 200.0);
    config->set("brain.synaptic_weight", 0.1);
    config->set("brain.enable_learning", true);
    config->set("brain.enable_development", false);
    config->set("brain.enable_curiosity", true);
    return config;
}

std::shared_ptr<Config> ConfigFactoryUtils::createConfigFromFile(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    config->loadFromFile(filepath);
    return config;
}

std::shared_ptr<Config> ConfigFactoryUtils::createConfigFromArgs(int argc, char** argv) {
    auto config = std::make_shared<Config>();
    config->loadFromArgs(argc, argv);
    return config;
}

// ConfigValidationUtils implementation
bool ConfigValidationUtils::validateValue(const std::string& key, const ConfigValue& value, const ConfigMetadata& metadata) {
    auto schemaIt = metadata.schema.find(key);
    if (schemaIt == metadata.schema.end()) {
        return true;  // No schema for this key, skip validation
    }
    
    const ConfigSchemaEntry& schema = schemaIt->second;
    
    // Check required field
    if (schema.required && key.empty()) {
        return false;
    }
    
    // Check type
    std::string actualType = ConfigTypeUtils::getTypeName(value);
    if (schema.type != "any" && actualType != schema.type) {
        // Try type conversion
        ConfigValue temp = value;
        if (!ConfigTypeUtils::tryConvertTo(temp, schema.type)) {
            return false;
        }
    }
    
    // Check min/max for numeric types
    if (ConfigTypeUtils::isNumericType(value)) {
        double val = 0.0;
        if (std::holds_alternative<int>(value)) val = std::get<int>(value);
        else if (std::holds_alternative<int64_t>(value)) val = std::get<int64_t>(value);
        else if (std::holds_alternative<double>(value)) val = std::get<double>(value);
        
        if (schema.min.has_value() && val < schema.min.value()) return false;
        if (schema.max.has_value() && val > schema.max.value()) return false;
    }
    
    // Check pattern for string types
    if (std::holds_alternative<std::string>(value) && schema.pattern.has_value()) {
        std::regex pattern(schema.pattern.value());
        if (!std::regex_match(std::get<std::string>(value), pattern)) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ConfigValidationUtils::validateConfig(const Config& config, const ConfigMetadata& metadata) {
    std::vector<std::string> errors;
    
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        auto value = config.get(key);
        if (!validateValue(key, *value, metadata)) {
            errors.push_back("Invalid value for key '" + key + "': " + ConfigTypeUtils::valueToString(*value));
        }
    }
    
    // Check required fields
    for (const auto& schemaPair : metadata.schema) {
        if (schemaPair.second.required && !config.has(schemaPair.first)) {
            errors.push_back("Required key missing: '" + schemaPair.first + "'");
        }
    }
    
    return errors;
}

std::vector<std::string> ConfigValidationUtils::validateAllConfigs(const std::vector<std::shared_ptr<Config>>& configs) {
    std::vector<std::string> allErrors;
    for (const auto& config : configs) {
        if (!config) {
            allErrors.push_back("Config is null");
            continue;
        }
        
        // Get metadata from first config
        static ConfigMetadata metadata; // TODO: Implement proper metadata retrieval
        auto errors = validateConfig(*config, metadata);
        for (const auto& error : errors) {
            allErrors.push_back(error);
        }
    }
    return allErrors;
}

bool ConfigValidationUtils::updateMetadata(const Config& config, ConfigMetadata& metadata) {
    try {
        // Analyze config to create/update schema
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            auto value = config.get(key);
            if (value) {
                std::string type = ConfigTypeUtils::getTypeName(*value);
                ConfigSchemaEntry entry;
                entry.type = type;
                entry.description = "Configuration value for " + key;
                entry.required = false;
                
                metadata.schema[key] = entry;
            }
        }
        
        metadata.version = "1.0";
        metadata.timestamp = std::chrono::system_clock::now();
        return true;
    } catch (...) {
        return false;
    }
}

std::string ConfigValidationUtils::generateValidationReport(const std::vector<std::string>& errors) {
    std::stringstream ss;
    ss << "Validation Report (" << errors.size() << " errors):\n";
    for (size_t i = 0; i < errors.size(); ++i) {
        ss << "  " << (i + 1) << ". " << errors[i] << "\n";
    }
    return ss.str();
}

// ConfigTimeUtils implementation
std::string ConfigTimeUtils::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::chrono::system_clock::time_point ConfigTimeUtils::parseTimestamp(const std::string& timestamp) {
    std::tm tm = {};
    std::istringstream ss(timestamp);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    
    auto timepoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    return timepoint;
}

std::string ConfigTimeUtils::formatTimestamp(const std::chrono::system_clock::time_point& timepoint) {
    auto in_time_t = std::chrono::system_clock::to_time_t(timepoint);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool ConfigTimeUtils::isExpired(const std::chrono::system_clock::time_point& timestamp, const std::string& ttl) {
    // Parse TTL (e.g., "1d", "2h", "30m", "30s")
    // For now, implement a simple version
    return false; // TODO: Implement proper TTL parsing
}

std::string ConfigTimeUtils::getContextFromTimestamp(const std::chrono::system_clock::time_point& timestamp) {
    // Extract date components for context
    auto in_time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << "context_" << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

// Config::Impl implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ConfigMetadata metadata;
};

// Config constructor/destructor
Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Internal helper methods
void Config::addEntry(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description) {
    pImpl->entries.emplace_back(key, value, source, description);
}

bool Config::findEntry(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

const ConfigEntry* Config::getEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end() ? &*it : nullptr;
}

std::string Config::detectFileFormat(const std::string& filepath) const {
    std::string extension = getFileExtension(filepath);
    
    if (endsWith(filepath, ".json") || extension == "json") {
        return "json";
    } else if (endsWith(filepath, ".yaml") || endsWith(filepath, ".yml") || extension == "yaml" || extension == "yml") {
        return "yaml";
    } else if (endsWith(filepath, ".conf") || endsWith(filepath, ".cfg") || endsWith(filepath, ".ini") || extension == "conf" || extension == "cfg" || extension == "ini") {
        return "ini";
    }
    
    // Default to ini for backward compatibility
    return "ini";
}

bool Config::isJSONFile(const std::string& filepath) const {
    return detectFileFormat(filepath) == "json";
}

bool Config::isYAMLMFile(const std::string& filepath) const {
    return detectFileFormat(filepath) == "yaml" || detectFileFormat(filepath) == "yml";
}

ConfigValue Config::stringToConfigValue(const std::string& valueStr, const std::string& type) const {
    ConfigValue result;
    
    try {
        std::string normalizedType = type;
        if (normalizedType.empty() || normalizedType == "auto") {
            normalizedType = "string";
        }
        
        if (normalizedType == "int" || normalizedType == "integer") {
            size_t pos;
            int val = std::stoi(valueStr, &pos);
            if (pos == valueStr.size()) {
                result = val;
            } else {
                result = std::stod(valueStr); // Fallback to double
            }
        } else if (normalizedType == "double" || normalizedType == "float") {
            result = std::stod(valueStr);
        } else if (normalizedType == "bool" || normalizedType == "boolean") {
            std::string lower = toLower(valueStr);
            if (lower == "true" || lower == "yes" || lower == "1") {
                result = true;
            } else if (lower == "false" || lower == "no" || lower == "0") {
                result = false;
            } else {
                result = std::stod(valueStr); // Fallback to double
            }
        } else {
            result = valueStr;
        }
    } catch (...) {
        result = valueStr; // Fallback to string
    }
    
    return result;
}

std::string Config::configValueToString(const ConfigValue& value) const {
    return ConfigTypeUtils::valueToString(value);
}

// Enhanced file loading with format support
bool Config::loadFromFile(const std::string& filepath, const std::string& format) {
    try {
        std::string actualFormat = format;
        if (actualFormat.empty() || actualFormat == "auto") {
            actualFormat = detectFileFormat(filepath);
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigLoadError("Cannot open file: " + filepath);
        }
        
        // Clear existing entries
        clear();
        
        if (actualFormat == "json") {
            return loadFromJSONFile(filepath);
        } else if (actualFormat == "yaml" || actualFormat == "yml") {
            return loadFromYAMLFile(filepath);
        } else {
            return loadFromSimpleFile(filepath);  // Backward compatibility
        }
    } catch (const ConfigError& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigLoadError(std::string("Error loading file '") + filepath + "': " + e.what());
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFile(filepath, "auto");
}

bool Config::loadFromJSONFile(const std::string& filepath) {
#ifdef NLOHMANN_JSON_HPP
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigLoadError("Cannot open JSON file: " + filepath);
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        clear();
        
        // Convert JSON to Config entries
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            // Convert JSON value to ConfigValue
            convertJSONValue(*it, value);
            
            addEntry(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const nlohmann::json::exception& e) {
        throw ConfigParseError(std::string("JSON parsing error in '") + filepath + "': " + e.what());
    } catch (const ConfigError& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigLoadError(std::string("Error loading JSON file '") + filepath + "': " + e.what());
    }
#else
    throw ConfigLoadError("JSON support not compiled in. Please enable NLOHMANN_JSON_HPP");
#endif
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
#ifdef YAML_CPP_HPP
    try {
        YAML::Loader loader;
        YAML::Node yamlData;
        
        try {
            yamlData = YAML::LoadFile(filepath);
        } catch (const YAML::BadFile& e) {
            throw ConfigLoadError(std::string("Cannot open YAML file: ") + filepath);
        } catch (const YAML::ParserException& e) {
            throw ConfigParseError(std::string("YAML parsing error in '") + filepath + "': " + e.what());
        }
        
        // Clear existing entries
        clear();
        
        // Convert YAML to Config entries
        if (yamlData.IsMap()) {
            for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
                std::string key = it->first.as<std::string>();
                ConfigValue value;
                
                // Convert YAML value to ConfigValue
                convertYAMLValue(*it, value);
                
                addEntry(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const ConfigError& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigLoadError(std::string("Error loading YAML file '") + filepath + "': " + e.what());
    }
#else
    throw ConfigLoadError("YAML support not compiled in. Please enable YAML_CPP_HPP");
#endif
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Clear existing entries
        clear();
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

// Convert JSON value to ConfigValue
void Config::convertJSONValue(const nlohmann::json& jsonValue, ConfigValue& configValue) {
    if (jsonValue.is_number_integer()) {
        configValue = jsonValue.get<int>();
    } else if (jsonValue.is_number_float()) {
        configValue = jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        configValue = jsonValue.get<bool>();
    } else if (jsonValue.is_string()) {
        configValue = jsonValue.get<std::string>();
    } else if (jsonValue.is_array()) {
        std::vector<ConfigValue> array;
        for (const auto& item : jsonValue) {
            ConfigValue element;
            convertJSONValue(item, element);
            array.push_back(element);
        }
        configValue = array;
    } else if (jsonValue.is_object()) {
        std::map<std::string, ConfigValue> object;
        for (auto it = jsonValue.begin(); it != jsonValue.end(); ++it) {
            ConfigValue value;
            convertJSONValue(it.value(), value);
            object[it.key()] = value;
        }
        configValue = object;
    } else if (jsonValue.is_null()) {
        configValue = std::string("null");
    } else {
        configValue = jsonValue.get<std::string>();
    }
}

// Convert YAML value to ConfigValue
void Config::convertYAMLValue(const YAML::Node& yamlValue, ConfigValue& configValue) {
    if (yamlValue.IsScalar()) {
        if (yamlValue.IsInt()) {
            configValue = yamlValue.as<int>();
        } else if (yamlValue.IsFloat()) {
            configValue = yamlValue.as<double>();
        } else if (yamlValue.IsBool()) {
            configValue = yamlValue.as<bool>();
        } else {
            configValue = yamlValue.as<std::string>();
        }
    } else if (yamlValue.IsSequence()) {
        std::vector<ConfigValue> array;
        for (const auto& item : yamlValue) {
            ConfigValue element;
            convertYAMLValue(item, element);
            array.push_back(element);
        }
        configValue = array;
    } else if (yamlValue.IsMap()) {
        std::map<std::string, ConfigValue> object;
        for (auto it = yamlValue.begin(); it != yamlValue.end(); ++it) {
            ConfigValue value;
            convertYAMLValue(it->second, value);
            object[it->first.as<std::string>()] = value;
        }
        configValue = object;
    } else {
        configValue = yamlValue.as<std::string>();
    }
}

// Enhanced saveToFile with format support
bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    try {
        std::string actualFormat = format;
        if (actualFormat.empty() || actualFormat == "auto") {
            actualFormat = detectFileFormat(filepath);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigSaveError("Cannot open file for writing: " + filepath);
        }
        
        // Set appropriate indentation for different formats
        if (actualFormat == "json") {
            file << "{\n";
        } else if (actualFormat == "yaml" || actualFormat == "yml") {
            // YAML header
            file << "# Configuration file generated by NLM Config\n";
        }
        
        // Write entries
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            
            // Write comment for description
            if (!entry.description.empty()) {
                file << "# " << entry.description << "\n";
            }
            
            if (actualFormat == "json") {
                // JSON format
                file << "  \"" << entry.key << "\": " << ConfigTypeUtils::valueToString(entry.value);
                if (i < pImpl->entries.size() - 1) {
                    file << ",\n";
                } else {
                    file << "\n";
                }
            } else if (actualFormat == "yaml" || actualFormat == "yml") {
                // YAML format
                file << "" << entry.key << ": " << ConfigTypeUtils::valueToString(entry.value) << "\n";
            } else {
                // Default simple format
                file << entry.key << " = " << ConfigTypeUtils::valueToString(entry.value) << "\n";
            }
        }
        
        if (actualFormat == "json") {
            file << "}\n";
        }
        
        return true;
    } catch (const ConfigError& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigSaveError(std::string("Error saving file '") + filepath + "': " + e.what());
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    return saveToFile(filepath, "auto");
}

// Metadata methods
const ConfigMetadata& Config::getMetadata() const {
    return pImpl->metadata;
}

void Config::setMetadata(const ConfigMetadata& metadata) {
    pImpl->metadata = metadata;
}

void Config::updateMetadata(const std::string& version, const std::string& context) {
    if (!version.empty()) {
        pImpl->metadata.version = version;
    }
    if (!context.empty()) {
        pImpl->metadata.context = context;
    }
    pImpl->metadata.timestamp = std::chrono::system_clock::now();
}

// Type conversion methods
void Config::setWithType(const std::string& key, const std::string& valueStr, const std::string& type) {
    ConfigValue value = stringToConfigValue(valueStr, type);
    set(key, value, ConfigSource::Runtime);
}

// Validation methods
bool Config::validate(const ConfigMetadata& metadata) const {
    return ConfigValidationUtils::validateConfig(*this, metadata).empty();
}

std::vector<std::string> Config::getValidationErrors(const ConfigMetadata& metadata) const {
    return ConfigValidationUtils::validateConfig(*this, metadata);
}

// Comparison methods
double Config::compareWith(const Config& other) const {
    return ConfigComparisonUtils::valuesDiff(
        std::visit([](const auto& v) { return ConfigValue(v); }, this->getEntry("") ? this->getEntry("")->value : ConfigValue()),
        std::visit([](const auto& v) { return ConfigValue(v); }, other.getEntry("") ? other.getEntry("")->value : ConfigValue())
    );
}

std::string Config::generateDiffReport(const Config& other) const {
    return ConfigComparisonUtils::generateDiffReport(*this, other).empty() ? "No differences" : "Differences found";
}

std::shared_ptr<Config> Config::mergeWith(const Config& other, bool override) const {
    auto merged = std::make_shared<Config>(*this);
    auto otherKeys = other.getKeys();
    
    for (const auto& key : otherKeys) {
        if (override || !has(key)) {
            auto value = other.get(key);
            if (value) {
                addEntry(key, *value, other.getEntry(key)->source);
            }
        }
    }
    
    return merged;
}

// Utility methods
std::shared_ptr<Config> Config::clone() const {
    auto clone = std::make_shared<Config>();
    clone->pImpl->entries = pImpl->entries;
    clone->pImpl->metadata = pImpl->metadata;
    return clone;
}

std::map<std::string, std::string> Config::toDictionary() const {
    std::map<std::string, std::string> dict;
    for (const auto& entry : pImpl->entries) {
        dict[entry.key] = ConfigTypeUtils::valueToString(entry.value);
    }
    return dict;
}

std::map<std::string, std::map<std::string, std::string>> Config::toNestedDictionary() const {
    std::map<std::string, std::map<std::string, std::string>> nested;
    
    for (const auto& entry : pImpl->entries) {
        size_t dotPos = entry.key.find('.');
        if (dotPos != std::string::npos) {
            std::string outerKey = entry.key.substr(0, dotPos);
            std::string innerKey = entry.key.substr(dotPos + 1);
            nested[outerKey][innerKey] = ConfigTypeUtils::valueToString(entry.value);
        } else {
            nested[entry.key]["value"] = ConfigTypeUtils::valueToString(entry.value);
        }
    }
    
    return nested;
}

// Static factory methods
std::shared_ptr<Config> Config::createDefault(const std::string& context) {
    return ConfigFactoryUtils::createDefaultConfig(context);
}

std::shared_ptr<Config> Config::loadFromFile(const std::string& filepath, const std::string& format) {
    auto config = std::make_shared<Config>();
    config->loadFromFile(filepath, format);
    return config;
}

std::shared_ptr<Config> Config::loadFromArgs(int argc, char** argv) {
    auto config = std::make_shared<Config>();
    config->loadFromArgs(argc, argv);
    return config;
}

// Static comparison utilities
std::map<std::string, double> Config::compare(const Config& a, const Config& b) {
    return ConfigComparisonUtils::compareConfigs(a, b);
}

std::map<std::string, std::string> Config::generateDiff(const Config& a, const Config& b) {
    return ConfigComparisonUtils::generateDiffReport(a, b);
}

// Time-based methods
bool Config::isExpired(const std::string& ttl) const {
    return ConfigTimeUtils::isExpired(pImpl->metadata.timestamp, ttl);
}

void Config::setContext(const std::string& context) {
    pImpl->metadata.context = context;
}

std::string Config::getContext() const {
    return pImpl->metadata.context;
}

// Error handling utilities
std::string Config::getErrorMessage(const ConfigError& error) {
    return error.what();
}

std::string Config::formatError(const ConfigError& error) {
    std::stringstream ss;
    ss << "[ConfigError] " << error.what();
    return ss.str();
}

// Performance and optimization
size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

bool Config::hasChanged(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return true;
    }
    
    auto keys1 = getKeys();
    auto keys2 = other.getKeys();
    
    if (keys1 != keys2) {
        return true;
    }
    
    for (const auto& key : keys1) {
        auto value1 = get(key);
        auto value2 = other.get(key);
        if (value1 && value2) {
            if (!ConfigComparisonUtils::valuesEqual(*value1, *value2)) {
                return true;
            }
        } else if (value1 || value2) {
            return true;
        }
    }
    
    return false;
}

// Statistics and analysis
std::string Config::getStatistics() const {
    std::stringstream ss;
    ss << "Configuration Statistics:";
    ss << "  Total entries: " << pImpl->entries.size() << "\n";
    
    auto typeDist = computeTypeDistribution();
    ss << "  Type distribution:";
    for (const auto& pair : typeDist) {
        ss << "    " << pair.first << ": " << pair.second << "%\n";
    }
    
    auto valueStats = computeValueStatistics();
    ss << "  Value statistics:";
    for (const auto& pair : valueStats) {
        ss << "    " << pair.first << ": " << pair.second << "\n";
    }
    
    return ss.str();
}

std::map<std::string, std::string> Config::getTypeDistribution() const {
    return computeTypeDistribution();
}

std::map<std::string, double> Config::getValueStatistics() const {
    return computeValueStatistics();
}

std::map<std::string, std::string> Config::computeTypeDistribution() const {
    std::map<std::string, int> counts;
    for (const auto& entry : pImpl->entries) {
        std::string type = ConfigTypeUtils::getTypeName(entry.value);
        counts[type]++;
    }
    
    std::map<std::string, std::string> result;
    int total = pImpl->entries.size();
    for (const auto& pair : counts) {
        double percentage = total > 0 ? (pair.second * 100.0 / total) : 0.0;
        result[pair.first] = std::to_string(percentage);
    }
    
    return result;
}

std::map<std::string, double> Config::computeValueStatistics() const {
    std::map<std::string, double> stats;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        
        if (std::holds_alternative<double>(entry.value)) {
            double val = std::get<double>(entry.value);
            if (stats.count("sum_" + key) == 0) stats["sum_" + key] = 0.0;
            stats["sum_" + key] += val;
            if (stats.count("count_" + key) == 0) stats["count_" + key] = 0;
            stats["count_" + key]++;
            if (stats.count("min_" + key) == 0 || val < stats["min_" + key]) stats["min_" + key] = val;
            if (stats.count("max_" + key) == 0 || val > stats["max_" + key]) stats["max_" + key] = val;
        }
    }
    
    for (auto it = stats.begin(); it != stats.end(); ++it) {
        if (it->first.substr(0, 4) == "sum_") {
            std::string key = it->first.substr(4);
            if (stats.count("count_" + key) > 0) {
                int count = static_cast<int>(stats["count_" + key]);
                double sum = it->second;
                stats["avg_" + key] = count > 0 ? sum / count : 0.0;
            }
        }
    }
    
    return stats;
}

std::vector<std::string> Config::computeValidationErrors(const ConfigMetadata& metadata) const {
    return ConfigValidationUtils::validateConfig(*this, metadata);
}

// Template method implementations
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
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        addEntry(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
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
    return oss.str();
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