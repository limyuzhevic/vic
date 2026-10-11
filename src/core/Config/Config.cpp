#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::unordered_map<std::string, ConfigEntry> entriesByKey;
    std::vector<std::string> keys;  // Maintain insertion order for getKeys()
    
    // Cache for frequently accessed keys to reduce string comparisons
    mutable std::unordered_map<std::string, size_t> keyToIndex;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper functions
namespace {
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    // Convert YAML to nlohmann::json
    nlohmann::json yamlToJson(const YAML::Node& node) {
        if (node.IsScalar()) {
            const std::string& value = node.Scalar();
            try {
                return std::stoi(value);
            } catch (...) {}
            try {
                return std::stod(value);
            } catch (...) {}
            if (value == "true" || value == "false") {
                return value == "true";
            }
            return value;
        } else if (node.IsSequence()) {
            nlohmann::json json = nlohmann::json::array();
            for (const auto& item : node) {
                json.push_back(yamlToJson(item));
            }
            return json;
        } else if (node.IsMap()) {
            nlohmann::json json = nlohmann::json::object();
            for (const auto& item : node) {
                json[item.first.Scalar()] = yamlToJson(item.second);
            }
            return json;
        }
        return nullptr;
    }
    
    // Convert nlohmann::json to ConfigValue
    ConfigValue jsonToConfigValue(const nlohmann::json& json) {
        if (json.is_number_integer()) {
            return json.get<int64_t>();
        } else if (json.is_number_float()) {
            return json.get<double>();
        } else if (json.is_boolean()) {
            return json.get<bool>();
        } else if (json.is_string()) {
            return json.get<std::string>();
        } else if (json.is_array()) {
            if (json.empty()) {
                return std::vector<ConfigValue>{};
            }
            
            bool allNumbers = std::all_of(json.begin(), json.end(),
                [](const auto& item) { return item.is_number(); });
            if (allNumbers) {
                bool allIntegers = std::all_of(json.begin(), json.end(),
                    [](const auto& item) { return item.is_number_integer(); });
                if (allIntegers) {
                    std::vector<int64_t> result;
                    result.reserve(json.size());
                    for (const auto& item : json) {
                        result.push_back(item.get<int64_t>());
                    }
                    return result;
                } else {
                    std::vector<double> result;
                    result.reserve(json.size());
                    for (const auto& item : json) {
                        result.push_back(item.get<double>());
                    }
                    return result;
                }
            } else if (std::all_of(json.begin(), json.end(),
                [](const auto& item) { return item.is_string(); })) {
                std::vector<std::string> result;
                result.reserve(json.size());
                for (const auto& item : json) {
                    result.push_back(item.get<std::string>());
                }
                return result;
            } else if (std::all_of(json.begin(), json.end(),
                [](const auto& item) { return item.is_boolean(); })) {
                std::vector<bool> result;
                result.reserve(json.size());
                for (const auto& item : json) {
                    result.push_back(item.get<bool>());
                }
                return result;
            } else {
                // For now, return a generic vector<ConfigValue> as fallback
                // This maintains backward compatibility
                std::vector<ConfigValue> result;
                result.reserve(json.size());
                for (const auto& item : json) {
                    result.push_back(jsonToConfigValue(item));
                }
                return result;
            }
        } else if (json.is_object()) {
            // For now, convert objects to string representation
            // This maintains backward compatibility
            return json.dump(4);
        }
        throw std::runtime_error("Unsupported JSON type");
    }
    
    // Convert ConfigValue to nlohmann::json
    nlohmann::json configValueToJson(const ConfigValue& value) {
        if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int64_t>>(value)) {
            return std::get<std::vector<int64_t>>(value);
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return std::get<std::vector<double>>(value);
        } else if (std::holds_alternative<std::vector<bool>>(value)) {
            return std::get<std::vector<bool>>(value);
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            return std::get<std::vector<std::string>>(value);
        }
        return nullptr;
    }
} // namespace

// File loading implementations
bool Config::loadFromFile(const std::string& filepath) {
    // Backward compatible key=value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
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

bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData = nlohmann::json::parse(file);
        return loadFromJSON(jsonData.dump());
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        nlohmann::json jsonData = yamlToJson(yamlData);
        return loadFromJSON(jsonData.dump());
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromJSON(const std::string& jsonString) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        
        // Clear existing entries
        pImpl->entriesByKey.clear();
        pImpl->keys.clear();
        pImpl->keyToIndex.clear();
        
        // Process JSON data
        for (const auto& item : jsonData.items()) {
            ConfigValue value = jsonToConfigValue(item.value());
            pImpl->entriesByKey[item.key()] = ConfigEntry(item.key(), value, ConfigSource::JSONFile);
            pImpl->keys.push_back(item.key());
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlString) {
    try {
        YAML::Node yamlData = YAML::Load(yamlString);
        nlohmann::json jsonData = yamlToJson(yamlData);
        return loadFromJSON(jsonData.dump());
    } catch (const std::exception& e) {
        return false;
    }
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

// Save to file implementations
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entriesByKey) {
        file << entry.second.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

bool Config::saveToJSONFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json jsonData;
    for (const auto& entry : pImpl->entriesByKey) {
        jsonData[entry.second.key] = configValueToJson(entry.second.value);
    }
    
    file << jsonData.dump(4); // Pretty print with 4 spaces
    return true;
}

bool Config::saveToYAMLFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json jsonData = nlohmann::json::parse(toJSON());
    std::string yamlString = jsonData.dump(4);
    file << yamlString;
    
    return true;
}

// Get value implementations
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->entriesByKey.find(key);
    if (it == pImpl->entriesByKey.end()) {
        return std::nullopt;
    }
    
    try {
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return std::get<int64_t>(it->second.value);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::get<double>(it->second.value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return std::get<bool>(it->second.value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return std::get<std::string>(it->second.value);
        } else {
            try {
                return std::get<T>(it->second.value);
            } catch (const std::bad_variant_access&) {
                return std::nullopt;
            }
        }
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Set value implementations
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    auto it = pImpl->entriesByKey.find(key);
    
    if (it != pImpl->entriesByKey.end()) {
        it->second.value = value;
        it->second.source = source;
    } else {
        pImpl->entriesByKey[key] = ConfigEntry(key, value, source);
        pImpl->keys.push_back(key);
    }
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, std::to_string(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    std::stringstream ss;
    ss << value;
    set(key, ss.str(), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, value ? "true" : "false", source);
}

// Check existence
bool Config::has(const std::string& key) const {
    return pImpl->entriesByKey.find(key) != pImpl->entriesByKey.end();
}

// Remove key
void Config::remove(const std::string& key) {
    auto it = pImpl->entriesByKey.find(key);
    if (it != pImpl->entriesByKey.end()) {
        // Also remove from keys vector to maintain order
        for (auto vecIt = pImpl->keys.begin(); vecIt != pImpl->keys.end(); ++vecIt) {
            if (*vecIt == key) {
                pImpl->keys.erase(vecIt);
                break;
            }
        }
        pImpl->entriesByKey.erase(it);
        pImpl->keyToIndex.erase(key);
    }
}

// Get all keys
std::vector<std::string> Config::getKeys() const {
    return pImpl->keys;
}

// Clear all
void Config::clear() {
    pImpl->entriesByKey.clear();
    pImpl->keys.clear();
    pImpl->keyToIndex.clear();
}

// Configuration summary
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entriesByKey.size() << " entries):\n";
    for (const auto& entry : pImpl->entriesByKey) {
        oss << "  " << entry.second.key << " = [";
        if (std::holds_alternative<int64_t>(entry.second.value)) {
            oss << std::get<int64_t>(entry.second.value);
        } else if (std::holds_alternative<double>(entry.second.value)) {
            oss << std::get<double>(entry.second.value);
        } else if (std::holds_alternative<bool>(entry.second.value)) {
            oss << (std::get<bool>(entry.second.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.second.value)) {
            oss << "\"" << std::get<std::string>(entry.second.value) << "\"";
        } else if (std::holds_alternative<std::vector<int64_t>>(entry.second.value)) {
            bool first = true;
            oss << "[";
            for (const auto& item : std::get<std::vector<int64_t>>(entry.second.value)) {
                if (!first) oss << ", ";
                oss << item;
                first = false;
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<double>>(entry.second.value)) {
            bool first = true;
            oss << "[";
            for (const auto& item : std::get<std::vector<double>>(entry.second.value)) {
                if (!first) oss << ", ";
                oss << item;
                first = false;
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<bool>>(entry.second.value)) {
            bool first = true;
            oss << "[";
            for (const auto& item : std::get<std::vector<bool>>(entry.second.value)) {
                if (!first) oss << ", ";
                oss << (item ? "true" : "false");
                first = false;
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.second.value)) {
            bool first = true;
            oss << "[";
            for (const auto& item : std::get<std::vector<std::string>>(entry.second.value)) {
                if (!first) oss << ", ";
                oss << "\"" << item << "\"";
                first = false;
            }
            oss << "]";
        } else {
            oss << "[]";
        }
        oss << "] (" << static_cast<int>(entry.second.source) << ")\n";
    }
    return oss.str();
}

std::string Config::toJSON() const {
    nlohmann::json jsonData;
    for (const auto& entry : pImpl->entriesByKey) {
        jsonData[entry.second.key] = configValueToJson(entry.second.value);
    }
    return jsonData.dump(4);
}

// Configuration info
ConfigSource Config::getSource(const std::string& key) const {
    auto it = pImpl->entriesByKey.find(key);
    return (it != pImpl->entriesByKey.end()) ? it->second.source : ConfigSource::Default;
}

// Static utilities
Config Config::createFromJSONFile(const std::string& filepath) {
    Config config;
    config.loadFromJSONFile(filepath);
    return config;
}

Config Config::createFromYAMLFile(const std::string& filepath) {
    Config config;
    config.loadFromYAMLFile(filepath);
    return config;
}

} // namespace nlm
