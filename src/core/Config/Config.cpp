#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

// nlohmann/json for JSON parsing
#include <nlohmann/json.hpp>

namespace nlm {

// YAML parsing (optional - can use yaml-cpp if available)
#ifdef HAVE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to convert string to appropriate type
template<typename T>
T stringToValue(const std::string& str) {
    std::istringstream iss(str);
    T value;
    if (!(iss >> value)) {
        throw std::runtime_error("Failed to convert string to value");
    }
    return value;
}

// Helper function to flatten nested JSON object to dot notation
void flattenJson(const nlohmann::json& json, const std::string& prefix,
                std::vector<std::pair<std::string, nlohmann::json>>& flatItems) {
    for (const auto& item : json.items()) {
        std::string key = item.key();
        if (!prefix.empty()) {
            key = prefix + "." + key;
        }
        
        if (item.is_object()) {
            // Recursively flatten nested objects
            flattenJson(item, key, flatItems);
        } else if (item.is_array()) {
            // Convert arrays to string representation
            std::string arrayStr = "[";
            for (size_t i = 0; i < item.size(); ++i) {
                if (i > 0) arrayStr += ",";
                arrayStr += item[i].dump();
            }
            arrayStr += "]";
            flatItems.emplace_back(key, arrayStr);
        } else {
            flatItems.emplace_back(key, item);
        }
    }
}

// Helper function to convert nlohmann::json to ConfigValue
ConfigValue jsonToConfigValue(const nlohmann::json& json) {
    if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_number_integer()) {
        return json.get<int>();
    } else if (json.is_number_unsigned()) {
        return json.get<uint64_t>();
    } else if (json.is_number()) {
        // Check if it's an integer or double
        if (json.is_number_integer()) {
            return json.get<int>();
        } else {
            return json.get<double>();
        }
    } else if (json.is_array()) {
        // Convert array to appropriate vector type
        if (!json.empty()) {
            const auto& firstElement = json[0];
            if (firstElement.is_string()) {
                std::vector<std::string> strVec;
                for (const auto& element : json) {
                    strVec.push_back(element.get<std::string>());
                }
                return strVec;
            } else if (firstElement.is_number_integer()) {
                std::vector<int> intVec;
                for (const auto& element : json) {
                    intVec.push_back(element.get<int>());
                }
                return intVec;
            } else if (firstElement.is_number()) {
                std::vector<double> doubleVec;
                for (const auto& element : json) {
                    doubleVec.push_back(element.get<double>());
                }
                return doubleVec;
            }
        }
        // Fallback to empty int vector
        return std::vector<int>{};
    }
    throw std::runtime_error("Unsupported JSON type");
}

// Helper function to convert ConfigValue to nlohmann::json
nlohmann::json configValueToJson(const ConfigValue& value) {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json = arg;
        }
    }, value);
    return json;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first
    std::ifstream jsonFile(filepath);
    if (jsonFile.good()) {
        try {
            // Parse JSON file
            nlohmann::json jsonData;
            jsonFile >> jsonData;
            
            // Flatten JSON to dot notation for backward compatibility
            std::vector<std::pair<std::string, nlohmann::json>> flatItems;
            flattenJson(jsonData, "", flatItems);
            
            // Clear existing entries
            pImpl->entries.clear();
            
            // Load flattened JSON items
            for (const auto& pair : flatItems) {
                try {
                    ConfigValue configValue = jsonToConfigValue(pair.second);
                    set(pair.first, configValue, ConfigSource::File);
                } catch (const std::exception& e) {
                    NLM_LOG_WARN("Warning: Failed to parse JSON key '" + pair.first + "': " + e.what());
                }
            }
            
            NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
            return true;
        } catch (const nlohmann::json::parse_error& e) {
            NLM_LOG_WARN("JSON parse error in " + filepath + ": " + e.what());
            // Fall back to simple key=value parser
        } catch (const std::exception& e) {
            NLM_LOG_WARN("Error parsing JSON file " + filepath + ": " + e.what());
            // Fall back to simple key=value parser
        }
    }
    
    // If JSON parsing fails, try simple key=value format for backward compatibility
    std::ifstream simpleFile(filepath);
    if (!simpleFile.is_open()) {
        NLM_LOG_WARN("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    while (std::getline(simpleFile, line)) {
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
            
            // Try to infer type from value string
            try {
                if (value.find('.') != std::string::npos) {
                    double doubleValue = std::stod(value);
                    set(key, doubleValue, ConfigSource::File);
                } else if (value == "true" || value == "false") {
                    bool boolValue = (value == "true");
                    set(key, boolValue, ConfigSource::File);
                } else {
                    try {
                        int intValue = std::stoi(value);
                        set(key, intValue, ConfigSource::File);
                    } catch (...) {
                        set(key, value, ConfigSource::File);
                    }
                }
            } catch (...) {
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from simple key=value file: " + filepath);
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
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
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
Config& Config::operator=(Config&&) noexcept = default;

// Helper function to convert string to appropriate type
template<typename T>
T stringToValue(const std::string& str) {
    std::istringstream iss(str);
    T value;
    if (!(iss >> value)) {
        throw std::runtime_error("Failed to convert string to value");
    }
    return value;
}

// Helper function to flatten nested JSON object to dot notation
void flattenJson(const nlohmann::json& json, const std::string& prefix,
                std::vector<std::pair<std::string, nlohmann::json>>& flatItems) {
    for (const auto& item : json.items()) {
        std::string key = item.key();
        if (!prefix.empty()) {
            key = prefix + "." + key;
        }
        
        if (item.is_object()) {
            // Recursively flatten nested objects
            flattenJson(item, key, flatItems);
        } else if (item.is_array()) {
            // Convert arrays to string representation
            std::string arrayStr = "[";
            for (size_t i = 0; i < item.size(); ++i) {
                if (i > 0) arrayStr += ",";
                arrayStr += item[i].dump();
            }
            arrayStr += "]";
            flatItems.emplace_back(key, arrayStr);
        } else {
            flatItems.emplace_back(key, item);
        }
    }
}

// Helper function to convert nlohmann::json to ConfigValue
ConfigValue jsonToConfigValue(const nlohmann::json& json) {
    if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_number_integer()) {
        return json.get<int>();
    } else if (json.is_number_unsigned()) {
        return json.get<uint64_t>();
    } else if (json.is_number()) {
        // Check if it's an integer or double
        if (json.is_number_integer()) {
            return json.get<int>();
        } else {
            return json.get<double>();
        }
    } else if (json.is_array()) {
        // Convert array to appropriate vector type
        if (!json.empty()) {
            const auto& firstElement = json[0];
            if (firstElement.is_string()) {
                std::vector<std::string> strVec;
                for (const auto& element : json) {
                    strVec.push_back(element.get<std::string>());
                }
                return strVec;
            } else if (firstElement.is_number_integer()) {
                std::vector<int> intVec;
                for (const auto& element : json) {
                    intVec.push_back(element.get<int>());
                }
                return intVec;
            } else if (firstElement.is_number()) {
                std::vector<double> doubleVec;
                for (const auto& element : json) {
                    doubleVec.push_back(element.get<double>());
                }
                return doubleVec;
            }
        }
        // Fallback to empty int vector
        return std::vector<int>{};
    }
    throw std::runtime_error("Unsupported JSON type");
}

// Helper function to convert ConfigValue to nlohmann::json
nlohmann::json configValueToJson(const ConfigValue& value) {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json = arg;
        }
    }, value);
    return json;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first
    std::ifstream jsonFile(filepath);
    if (jsonFile.good()) {
        try {
            // Parse JSON file
            nlohmann::json jsonData;
            jsonFile >> jsonData;
            
            // Flatten JSON to dot notation for backward compatibility
            std::vector<std::pair<std::string, nlohmann::json>> flatItems;
            flattenJson(jsonData, "", flatItems);
            
            // Clear existing entries
            pImpl->entries.clear();
            
            // Load flattened JSON items
            for (const auto& pair : flatItems) {
                try {
                    ConfigValue configValue = jsonToConfigValue(pair.second);
                    set(pair.first, configValue, ConfigSource::File);
                } catch (const std::exception& e) {
                    NLM_LOG_WARN("Warning: Failed to parse JSON key '" + pair.first + "': " + e.what());
                }
            }
            
            NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
            return true;
        } catch (const nlohmann::json::parse_error& e) {
            NLM_LOG_WARN("JSON parse error in " + filepath + ": " + e.what());
            // Fall back to simple key=value parser
        } catch (const std::exception& e) {
            NLM_LOG_WARN("Error parsing JSON file " + filepath + ": " + e.what());
            // Fall back to simple key=value parser
        }
    }
    
    // If JSON parsing fails, try simple key=value format for backward compatibility
    std::ifstream simpleFile(filepath);
    if (!simpleFile.is_open()) {
        NLM_LOG_WARN("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    while (std::getline(simpleFile, line)) {
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
            
            // Try to infer type from value string
            try {
                if (value.find('.') != std::string::npos) {
                    double doubleValue = std::stod(value);
                    set(key, doubleValue, ConfigSource::File);
                } else if (value == "true" || value == "false") {
                    bool boolValue = (value == "true");
                    set(key, boolValue, ConfigSource::File);
                } else {
                    try {
                        int intValue = std::stoi(value);
                        set(key, intValue, ConfigSource::File);
                    } catch (...) {
                        set(key, value, ConfigSource::File);
                    }
                }
            } catch (...) {
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from simple key=value file: " + filepath);
    return true;
}
