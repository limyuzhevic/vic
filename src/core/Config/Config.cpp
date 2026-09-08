#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

// YAML-cpp includes (optional)
// #include <yaml-cpp/yaml.h>

namespace nlm {

// Global error message for last error
static std::string lastError;

// JSON/YAML parsing function implementations
nlohmann::json loadJSONFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        lastError = "Failed to open JSON file: " + filepath;
        return nlohmann::json();
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        return jsonData;
    } catch (const std::exception& e) {
        lastError = "Failed to parse JSON file " + filepath + ": " + std::string(e.what());
        return nlohmann::json();
    }
}

nlohmann::json loadYAMLFromFile(const std::string& filepath) {
    // Placeholder implementation - would need yaml-cpp
    (void)filepath; // Suppress unused parameter warning
    lastError = "YAML parsing not yet implemented (requires yaml-cpp)";
    return nlohmann::json();
}

// Helper function to convert JSON value to ConfigValue
static ConfigValue jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        nlohmann::json::const_iterator it = j.begin();
        if (it == j.end()) {
            return std::vector<int>();
        }
        if ((*it).is_number_integer()) {
            std::vector<int> result;
            result.reserve(j.size());
            for (const auto& elem : j) {
                result.push_back(elem.get<int>());
            }
            return result;
        } else if ((*it).is_number_float()) {
            std::vector<double> result;
            result.reserve(j.size());
            for (const auto& elem : j) {
                result.push_back(elem.get<double>());
            }
            return result;
        } else if ((*it).is_string()) {
            std::vector<std::string> result;
            result.reserve(j.size());
            for (const auto& elem : j) {
                result.push_back(elem.get<std::string>());
            }
            return result;
        }
    }
    // Default fallback
    return 0;
}

// Helper function to convert ConfigValue to JSON value
static nlohmann::json configValueToJson(const ConfigValue& value) {
    nlohmann::json j;
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        }
    }, value);
    return j;
}

// Config implementation

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string errorMsg;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper to detect file format
bool Config::isJSONFile(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read first few characters
    char buffer[4];
    file.read(buffer, 4);
    file.close();
    
    // Check for JSON opening braces/brackets
    return (buffer[0] == '{' || buffer[0] == '[');
}

// Helper function to detect file format (non-member)
static bool isJSONFileFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read first few characters
    char buffer[4];
    file.read(buffer, 4);
    file.close();
    
    // Check for JSON opening braces/brackets
    return (buffer[0] == '{' || buffer[0] == '[');
}

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format and route appropriately
    if (isJSONFileFormat(filepath)) {
        return loadFromJSONFile(filepath);
    } else {
        // Try to detect YAML format (simple check for YAML-like content)
        // For backward compatibility, fall back to simple parser
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    clear();
    
    nlohmann::json jsonData = loadJSONFromFile(filepath);
    if (jsonData.empty() && !lastError.empty()) {
        return false;
    }
    
    // Process JSON object recursively
    std::function<void(const nlohmann::json&, const std::string&)> processJsonNode = 
        [&](const nlohmann::json& node, const std::string& prefix) {
            if (node.is_object()) {
                for (const auto& [key, value] : node.items()) {
                    std::string fullKey = prefix.empty() ? key : prefix + "." + key;
                    processJsonNode(value, fullKey);
                }
            } else if (node.is_array()) {
                for (size_t i = 0; i < node.size(); ++i) {
                    std::string fullKey = prefix.empty() ? std::to_string(i) : prefix + "." + std::to_string(i);
                    processJsonNode(node[i], fullKey);
                }
            } else {
                // Leaf node - convert to ConfigValue and set
                ConfigValue configValue = jsonToConfigValue(node);
                if (validateKey(prefix) && validateValue(configValue)) {
                    // Extract description from key if available
                    std::string description = "Loaded from JSON";
                    set(prefix, configValue, ConfigSource::File);
                }
            }
        };
    
    processJsonNode(jsonData, "");
    return true;
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    clear();
    lastError.clear();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        lastError = "Failed to open file: " + filepath;
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
            
            // Basic type inference for simple values
            ConfigValue configValue;
            if (value.find('.') != std::string::npos) {
                try {
                    configValue = std::stod(value);
                } catch (...) {
                    configValue = value;
                }
            } else if (value == "true" || value == "false") {
                configValue = (value == "true");
            } else {
                try {
                    configValue = std::stoll(value);
                } catch (...) {
                    configValue = value;
                }
            }
            
            if (validateKey(key) && validateValue(configValue)) {
                set(key, configValue, ConfigSource::File);
            }
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        lastError = "Failed to open file for writing: " + filepath;
        return false;
    }
    
    // Create JSON object
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        // Handle dot notation for nested keys
        size_t dotPos = key.find('.');
        if (dotPos != std::string::npos) {
            nlohmann::json* current = &jsonData;
            std::string remaining = key;
            while (dotPos != std::string::npos && remaining.find('.') != std::string::npos) {
                std::string segment = remaining.substr(0, dotPos);
                remaining = remaining.substr(dotPos + 1);
                
                if (!current->contains(segment)) {
                    current->emplace_json(segment, nlohmann::json::object());
                }
                current = &((*current)[segment]);
                dotPos = remaining.find('.');
            }
            if (!remaining.empty()) {
                (*current)[remaining] = configValueToJson(entry.value);
            }
        } else {
            jsonData[key] = configValueToJson(entry.value);
        }
        
        // Add description as a separate field
        std::string descKey = key + "_description";
        jsonData[descKey] = entry.description;
    }
    
    // Write JSON to file
    try {
        file << jsonData.dump(4); // Pretty-print with 4-space indentation
    } catch (const std::exception& e) {
        lastError = "Failed to write JSON to file " + filepath + ": " + std::string(e.what());
        return false;
    }
    
    return true;
}

// Validation and error handling
bool Config::validateKey(const std::string& key) const {
    if (key.empty()) {
        lastError = "Configuration key cannot be empty";
        return false;
    }
    
    // Check for invalid characters in key
    for (char c : key) {
        if (!(std::isalnum(c) || c == '_' || c == '-' || c == '.')) {
            lastError = "Invalid character in key '" + key + "'. Only alphanumeric, _, -, . are allowed.";
            return false;
        }
    }
    
    return true;
}

bool Config::validateValue(const ConfigValue& value) const {
    std::visit([this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            if (arg.empty()) {
                lastError = "String value cannot be empty";
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (arg.size() > 1000) {
                lastError = "Integer vector too large (max 1000 elements)";
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (arg.size() > 1000) {
                lastError = "Double vector too large (max 1000 elements)";
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (arg.size() > 1000) {
                lastError = "String vector too large (max 1000 elements)";
            }
            for (const auto& str : arg) {
                if (str.empty()) {
                    lastError = "String in vector cannot be empty";
                    return;
                }
            }
        }
    }, value);
    
    return lastError.empty();
}

std::string Config::getLastError() const {
    return lastError;
}

// Get values
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
    if (!validateKey(key)) {
        return;
    }
    if (!validateValue(value)) {
        return;
    }
    
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
    lastError.clear();
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

bool Config::loadFromJSONFile(const std::string& filepath) {
    clear();
    
    nlohmann::json jsonData = loadJSONFromFile(filepath);
    if (jsonData.empty() && !lastError.empty()) {
        return false;
    }
    
    // Process JSON object recursively
    std::function<void(const nlohmann::json&, const std::string&)> processJsonNode = 
        [&](const nlohmann::json& node, const std::string& prefix) {
            if (node.is_object()) {
                for (const auto& [key, value] : node.items()) {
                    std::string fullKey = prefix.empty() ? key : prefix + "." + key;
                    processJsonNode(value, fullKey);
                }
            } else if (node.is_array()) {
                for (size_t i = 0; i < node.size(); ++i) {
                    std::string fullKey = prefix.empty() ? std::to_string(i) : prefix + "." + std::to_string(i);
                    processJsonNode(node[i], fullKey);
                }
            } else {
                // Leaf node - convert to ConfigValue and set
                ConfigValue configValue = jsonToConfigValue(node);
                if (validateKey(prefix) && validateValue(configValue)) {
                    // Extract description from key if available
                    std::string description = "Loaded from JSON";
                    set(prefix, configValue, ConfigSource::File);
                }
            }
        };
    
    processJsonNode(jsonData, "");
    return true;
}

} // namespace nlm
