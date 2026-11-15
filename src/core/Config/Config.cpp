#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <stdexcept>
#include <numeric>
#include <set>

// Include JSON and YAML libraries if available
#ifdef __has_include
    #if __has_include(<nlohmann/json.hpp>)
        #include <nlohmann/json.hpp>
        #define NLM_HAS_JSON 1
    #endif
    #if __has_include(<yaml-cpp/yaml.h>)
        #include <yaml-cpp/yaml.h>
        #define NLM_HAS_YAML 1
    #endif
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Static helper methods
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

ConfigFormat Config::detectFormatFromExtension(const std::string& filepath) {
    std::string lower = toLower(filepath);
    
    if (lower.empty()) return ConfigFormat::Unknown;
    
    // Check file extension
    if (lower.ends_with(".cfg")) return ConfigFormat::KeyValue;
    if (lower.ends_with(".json")) return ConfigFormat::JSON;
    if (lower.ends_with(".jsonc") || lower.ends_with(".jsonl")) return ConfigFormat::JSONC;
    if (lower.ends_with(".yaml") || lower.ends_with(".yml")) return ConfigFormat::YAML;
    
    return ConfigFormat::Unknown;
}

ConfigFormat Config::detectFormat(const std::string& filepath) {
    ConfigFormat format = detectFormatFromExtension(filepath);
    
    if (format == ConfigFormat::Unknown) {
        // Try to detect from file content
        std::ifstream file(filepath);
        if (!file.is_open()) return ConfigFormat::Unknown;
        
        std::string firstLine;
        std::getline(file, firstLine);
        
        format = detectFormatFromContent(firstLine);
    }
    
    return format;
}

ConfigFormat Config::detectFormatFromContent(const std::string& content) {
    if (content.empty()) return ConfigFormat::Unknown;
    
    std::string trimmed = trim(content);
    
    // Check for JSON indicators
    if (trimmed.starts_with("{") && trimmed.ends_with("}")) {
        return ConfigFormat::JSON;
    }
    if (trimmed.starts_with("[") && trimmed.ends_with("]")) {
        return ConfigFormat::JSONC;
    }
    
    // Check for YAML indicators
    if (trimmed.find(':') != std::string::npos && 
        (trimmed.find('"') != std::string::npos || trimmed.find("'") != std::string::npos ||
         trimmed.find("\n  ") != std::string::npos || trimmed.find("\n\t") != std::string::npos)) {
        return ConfigFormat::YAML;
    }
    
    // Check for key=value format
    if (trimmed.find('=') != std::string::npos && (trimmed.find('=') < trimmed.find('#') || trimmed.find('#') == std::string::npos)) {
        return ConfigFormat::KeyValue;
    }
    
    return ConfigFormat::Unknown;
}

bool Config::loadFromFile(const std::string& filepath) {
    clear(); // Clear existing configuration
    
    ConfigFormat format = detectFormat(filepath);
    
    switch (format) {
        case ConfigFormat::KeyValue:
            return loadFromKeyValue(filepath);
        case ConfigFormat::JSON:
        case ConfigFormat::JSONC:
            return loadFromJSONFile(filepath);
        case ConfigFormat::YAML:
            return loadFromYAMLFile(filepath);
        default:
            // Try key=value format as fallback
            return loadFromKeyValue(filepath);
    }
}

bool Config::loadFromKeyValue(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
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
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::loadFromEnv(const std::string& prefix, bool skipEmpty) {
    for (const auto& env : std::filesystem::environment()) {
        std::string key = env.first;
        std::string value = env.second;
        
        if (prefix.empty() || key.rfind(prefix, 0) == 0) {
            std::string configKey = key;
            if (!prefix.empty()) {
                configKey = key.substr(prefix.length());
            }
            
            if (skipEmpty && value.empty()) {
                continue;
            }
            
            set(configKey, value, ConfigSource::Environment);
        }
    }
    return true;
}

// JSON parsing helper methods
namespace {
    template<typename T>
    bool convertJSONValue(const nlohmann::json& jsonVal, ConfigValue& configVal) {
        try {
            if constexpr (std::is_same_v<T, int>) {
                configVal = jsonVal.get<int>();
            } else if constexpr (std::is_same_v<T, int64_t>) {
                configVal = jsonVal.get<int64_t>();
            } else if constexpr (std::is_same_v<T, double>) {
                configVal = jsonVal.get<double>();
            } else if constexpr (std::is_same_v<T, bool>) {
                configVal = jsonVal.get<bool>();
            } else if constexpr (std::is_same_v<T, std::string>) {
                configVal = jsonVal.get<std::string>();
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                std::vector<int> vec = jsonVal.get<std::vector<int>>();
                configVal = vec;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                std::vector<double> vec = jsonVal.get<std::vector<double>>();
                configVal = vec;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                std::vector<std::string> vec = jsonVal.get<std::vector<std::string>>();
                configVal = vec;
            } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, ConfigValue>>) {
                // Parse nested object
                std::unordered_map<std::string, ConfigValue> map;
                for (auto it = jsonVal.begin(); it != jsonVal.end(); ++it) {
                    ConfigValue nestedVal;
                    convertJSONValue<ConfigValue>(*it, nestedVal);
                    map[it.key()] = nestedVal;
                }
                configVal = map;
            } else {
                return false;
            }
            return true;
        } catch (...) {
            return false;
        }
    }
    
    std::string jsonValueToString(const nlohmann::json& jsonVal) {
        if (jsonVal.is_string()) return jsonVal.get<std::string>();
        if (jsonVal.is_number_integer()) return std::to_string(jsonVal.get<int64_t>());
        if (jsonVal.is_number_float()) return std::to_string(jsonVal.get<double>());
        if (jsonVal.is_boolean()) return jsonVal.get<bool>() ? "true" : "false";
        if (jsonVal.is_array()) {
            std::string result = "[";
            for (size_t i = 0; i < jsonVal.size(); ++i) {
                if (i > 0) result += ", ";
                result += jsonValueToString(jsonVal[i]);
            }
            result += "]";
            return result;
        }
        if (jsonVal.is_object()) {
            std::string result = "{";
            bool first = true;
            for (auto it = jsonVal.begin(); it != jsonVal.end(); ++it) {
                if (!first) result += ", ";
                first = false;
                result += "\"" + it.key() + "\": " + jsonValueToString(*it);
            }
            result += "}";
            return result;
        }
        return "";
    }
}

bool Config::loadFromJSONFile(const std::string& filepath) {
#ifdef NLM_HAS_JSON
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        return loadFromJSONInternal(jsonData.dump(), filepath);
    } catch (...) {
        return false;
    }
#else
    // JSON support not available
    return false;
#endif
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
#ifdef NLM_HAS_YAML
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        YAML::Node yamlData = YAML::Load(file);
        
        // Convert YAML to JSON-like format for easier processing
        std::ostringstream jsonStream;
        jsonStream << yamlData;
        
        return loadFromJSONInternal(jsonStream.str(), filepath);
    } catch (...) {
        return false;
    }
#else
    // YAML support not available
    return false;
#endif
}

bool Config::loadFromJSON(const std::string& jsonStr, const std::string& sourceInfo) {
    return loadFromJSONInternal(jsonStr, sourceInfo);
}

bool Config::loadFromYAML(const std::string& yamlStr, const std::string& sourceInfo) {
    return loadFromJSONInternal(yamlStr, sourceInfo);
}

bool Config::loadFromJSONInternal(const std::string& jsonStr, const std::string& sourceInfo) {
#ifdef NLM_HAS_JSON
    try {
        // Parse JSON string
        auto jsonData = nlohmann::json::parse(jsonStr);
        
        // Clear existing configuration
        clear();
        
        // Process JSON object
        if (jsonData.is_object()) {
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                ConfigValue configVal;
                
                if (it.value().is_string()) {
                    configVal = it.value().get<std::string>();
                } else if (it.value().is_number_integer()) {
                    configVal = it.value().get<int64_t>();
                } else if (it.value().is_number_float()) {
                    configVal = it.value().get<double>();
                } else if (it.value().is_boolean()) {
                    configVal = it.value().get<bool>();
                } else if (it.value().is_array()) {
                    // Handle arrays based on element type
                    if (!it.value().empty()) {
                        auto first = it.value()[0];
                        if (first.is_string()) {
                            std::vector<std::string> vec;
                            for (const auto& item : it.value()) {
                                vec.push_back(item.get<std::string>());
                            }
                            configVal = vec;
                        } else if (first.is_number_integer()) {
                            std::vector<int64_t> vec;
                            for (const auto& item : it.value()) {
                                vec.push_back(item.get<int64_t>());
                            }
                            configVal = vec;
                        } else if (first.is_number_float()) {
                            std::vector<double> vec;
                            for (const auto& item : it.value()) {
                                vec.push_back(item.get<double>());
                            }
                            configVal = vec;
                        }
                    }
                } else if (it.value().is_object()) {
                    // Handle nested objects
                    std::unordered_map<std::string, ConfigValue> nestedMap;
                    for (auto nestedIt = it.value().begin(); nestedIt != it.value().end(); ++nestedIt) {
                        ConfigValue nestedVal;
                        if (nestedIt.value().is_string()) {
                            nestedVal = nestedIt.value().get<std::string>();
                        } else if (nestedIt.value().is_number_integer()) {
                            nestedVal = nestedIt.value().get<int64_t>();
                        } else if (nestedIt.value().is_number_float()) {
                            nestedVal = nestedIt.value().get<double>();
                        } else if (nestedIt.value().is_boolean()) {
                            nestedVal = nestedIt.value().get<bool>();
                        }
                        nestedMap[nestedIt.key()] = nestedVal;
                    }
                    configVal = nestedMap;
                }
                
                set(key, configVal, ConfigSource::File);
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
#else
    // JSON support not available
    return false;
#endif
}

bool Config::loadFromYAMLInternal(const std::string& yamlStr, const std::string& sourceInfo) {
    return loadFromJSONInternal(yamlStr, sourceInfo);
}

std::string Config::exportToJSON() const {
#ifdef NLM_HAS_JSON
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, ConfigValue>>) {
                // Handle nested objects
                nlohmann::json nestedJson;
                for (const auto& kv : arg) {
                    nestedJson[kv.first] = kv.second;
                }
                jsonData[entry.key] = nestedJson;
            }
        }, entry.value);
    }
    
    return jsonData.dump(2);
#else
    return "";
#endif
}

std::string Config::exportToYAML() const {
#ifdef NLM_HAS_YAML
    // For now, return JSON format as a placeholder
    // In a full implementation, this would convert to YAML format
    return exportToJSON();
#else
    return "";
#endif
}

bool Config::saveToFile(const std::string& filepath, ConfigFormat format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    switch (format) {
        case ConfigFormat::JSON:
            file << exportToJSON();
            break;
        case ConfigFormat::YAML:
            file << exportToYAML();
            break;
        case ConfigFormat::KeyValue:
            // Save in key=value format
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                file << entry.key << " = ";
                std::visit([&](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        file << "\"" << arg << "\"";
                    } else if constexpr (std::is_same_v<T, int> || 
                                        std::is_same_v<T, int64_t>) {
                        file << arg;
                    } else if constexpr (std::is_same_v<T, double>) {
                        file << arg;
                    } else if constexpr (std::is_same_v<T, bool>) {
                        file << (arg ? "true" : "false");
                    } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                        std::is_same_v<T, std::vector<int64_t>>) {
                        file << "[";
                        for (size_t i = 0; i < arg.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << arg[i];
                        }
                        file << "]";
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        file << "[";
                        for (size_t i = 0; i < arg.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << arg[i];
                        }
                        file << "]";
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "[";
                        for (size_t i = 0; i < arg.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << "\"" << arg[i] << "\"";
                        }
                        file << "]";
                    } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, ConfigValue>>) {
                        file << "{";
                        for (auto it = arg.begin(); it != arg.end(); ++it) {
                            if (it != arg.begin()) file << ", ";
                            file << "\"" << it->first << "\": ";
                            std::visit([&](auto&& val) {
                                using U = std::decay_t<decltype(val)>;
                                if constexpr (std::is_same_v<U, std::string>) {
                                    file << "\"" << val << "\"";
                                } else if constexpr (std::is_same_v<U, int> || 
                                                    std::is_same_v<U, int64_t>) {
                                    file << val;
                                } else if constexpr (std::is_same_v<U, double>) {
                                    file << val;
                                } else if constexpr (std::is_same_v<U, bool>) {
                                    file << (val ? "true" : "false");
                                }
                            }, it->second);
                        }
                        file << "}";
                    }
                }, entry.value);
                file << "\n\n";
            }
            break;
        default:
            // Default to JSON format
            file << exportToJSON();
            break;
    }
    
    return true;
}

std::vector<ConfigErrorInfo> Config::validate() const {
    std::vector<ConfigErrorInfo> errors;
    
    for (const auto& entry : pImpl->entries) {
        // Check for validation errors stored in the entry
        if (!entry.validationError.empty()) {
            ConfigErrorInfo error;
            error.error = ConfigError::ValidationError;
            error.message = entry.validationError;
            error.key = entry.key;
            error.filePath = entry.filePath;
            error.lineNumber = entry.lineNumber;
            errors.push_back(error);
        }
        
        // Validate critical parameters
        if (entry.key == "neuron_count") {
            std::string error;
            if (!validateParameter(entry.key, error)) {
                ConfigErrorInfo validationError;
                validationError.error = ConfigError::ValidationError;
                validationError.message = error;
                validationError.key = entry.key;
                validationError.filePath = entry.filePath;
                validationError.lineNumber = entry.lineNumber;
                errors.push_back(validationError);
            }
        }
        
        if (entry.key == "simulation_timestep" && entry.value.index() == typeid(double)) {
            double timestep = std::get<double>(entry.value);
            if (timestep <= 0.0) {
                ConfigErrorInfo error;
                error.error = ConfigError::ValidationError;
                error.message = "simulation_timestep must be positive";
                error.key = entry.key;
                error.filePath = entry.filePath;
                error.lineNumber = entry.lineNumber;
                errors.push_back(error);
            }
        }
    }
    
    return errors;
}

bool Config::validateParameter(const std::string& key, std::string& error) const {
    error = "";
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return true; // Key doesn't exist, not an error
    }
    
    // Validate based on parameter type
    if (key == "neuron_count") {
        if (it->value.index() == typeid(int)) {
            int count = std::get<int>(it->value);
            if (count <= 0) {
                error = "neuron_count must be positive";
                return false;
            }
        } else if (it->value.index() == typeid(int64_t)) {
            int64_t count = std::get<int64_t>(it->value);
            if (count <= 0) {
                error = "neuron_count must be positive";
                return false;
            }
        }
    }
    
    if (key == "connection_probability") {
        if (it->value.index() == typeid(double)) {
            double prob = std::get<double>(it->value);
            if (prob < 0.0 || prob > 1.0) {
                error = "connection_probability must be between 0.0 and 1.0";
                return false;
            }
        }
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, ConfigValue>>) {
                oss << "{";
                bool first = true;
                for (const auto& kv : arg) {
                    if (!first) oss << ", ";
                    first = false;
                    oss << "\"" << kv.first << "\": ...";
                }
                oss << "}";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, entry.source);
    }
}

void Config::mergeFromFile(const std::string& filepath) {
    Config tempConfig;
    if (tempConfig.loadFromFile(filepath)) {
        merge(tempConfig);
    }
}

// Explicit template instantiations
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
