#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <iostream>

using json = nlohmann::json;

// Helper functions for JSON/YAML parsing

// Convert JSON value to ConfigValue
ConfigValue jsonToConfigValue(const json& value);

// Convert ConfigValue to JSON value
json configValueToJson(const ConfigValue& value);

// Convert JSON to flat key-value pairs (for backward compatibility)
std::unordered_map<std::string, std::string> flattenJson(const json& node);

// Convert flat key-value pairs to ConfigValue with support for nested paths
ConfigValue flattenToConfigValue(const std::unordered_map<std::string, std::string>& flatData);

// Helper to convert nested YAML to flat key-value pairs
std::unordered_map<std::string, std::string> flattenYaml(const YAML::Node& node);

// Helper to convert YAML value to ConfigValue
ConfigValue yamlToConfigValue(const YAML::Node& value);

// Check file extension to determine format
bool isJsonFile(const std::string& filepath);
bool isYamlFile(const std::string& filepath);

// Error handling
class ConfigParseError {
public:
    ConfigParseError(const std::string& message) : message(message) {}
    const std::string& getMessage() const { return message; }
    
private:
    std::string message;
};

void logConfigError(const std::string& filepath, const ConfigParseError& error);

// Implementation of the conversion functions

// Helper to trim whitespace
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper to convert to lowercase
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Convert JSON value to ConfigValue
ConfigValue jsonToConfigValue(const json& value) {
    if (value.is_string()) {
        return value.get<std::string>();
    } else if (value.is_number_integer()) {
        return value.get<int>();
    } else if (value.is_number_unsigned()) {
        return value.get<uint64_t>();
    } else if (value.is_number_float()) {
        return value.get<double>();
    } else if (value.is_boolean()) {
        return value.get<bool>();
    } else if (value.is_array()) {
        // Handle arrays based on element type
        std::vector<int> intArray;
        std::vector<double> doubleArray;
        std::vector<std::string> stringArray;
        
        bool allInt = true, allDouble = true, allString = true;
        
        for (const auto& item : value) {
            if (item.is_string()) {
                allInt = false;
                allDouble = false;
                stringArray.push_back(item.get<std::string>());
            } else if (item.is_number_integer()) {
                allString = false;
                allDouble = false;
                intArray.push_back(item.get<int>());
            } else if (item.is_number_float()) {
                allString = false;
                allInt = false;
                doubleArray.push_back(item.get<double>());
            } else if (item.is_boolean()) {
                allInt = false;
                allDouble = false;
                allString = false;
                // Convert bool to string for consistency
                stringArray.push_back(item.get<bool>() ? "true" : "false");
            } else {
                allInt = false;
                allDouble = false;
                allString = false;
                // Use string representation for unknown types
                stringArray.push_back(item.dump());
            }
        }
        
        if (allInt) return intArray;
        if (allDouble) return doubleArray;
        if (allString) return stringArray;
        
        // Mixed types - use string array
        std::vector<std::string> mixedArray;
        for (const auto& item : value) {
            mixedArray.push_back(item.dump());
        }
        return mixedArray;
    } else if (value.is_null()) {
        return std::string("null");
    } else {
        return value.dump();
    }
}

// Convert ConfigValue to JSON value
json configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) {
            return static_cast<int64_t>(arg);
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            return static_cast<double>(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return json(arg);
        } else {
            return json(arg);
        }
    }, value);
}

// Convert JSON to flat key-value pairs (for backward compatibility)
std::unordered_map<std::string, std::string> flattenJson(const json& node, const std::string& prefix = "") {
    std::unordered_map<std::string, std::string> result;
    
    if (node.is_object()) {
        for (const auto& item : node.items()) {
            std::string key = prefix + item.key();
            if (item.value().is_object()) {
                // Recursively flatten nested objects
                auto nested = flattenJson(item.value(), key + ".");
                result.insert(nested.begin(), nested.end());
            } else {
                result[key] = item.value().dump();
            }
        }
    }
    
    return result;
}

// Convert ConfigValue to JSON value
json flattenToConfigValue(const std::unordered_map<std::string, std::string>& flatData) {
    json result = json::object();
    
    for (const auto& item : flatData) {
        // Parse the value string into appropriate type
        std::string key = item.first;
        std::string value = item.second;
        
        // Remove quotes from string values
        if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                 (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Try to determine the type from the value
        if (value == "true" || value == "false") {
            result[key] = (value == "true");
        } else {
            try {
                // Try to parse as integer
                size_t pos;
                int intValue = std::stoi(value, &pos);
                if (pos == value.size()) {
                    result[key] = intValue;
                    continue;
                }
            } catch (...) {}
            
            try {
                // Try to parse as floating point
                size_t pos;
                double doubleValue = std::stod(value, &pos);
                if (pos == value.size()) {
                    result[key] = doubleValue;
                    continue;
                }
            } catch (...) {}
            
            // Treat as string (or array if contains brackets)
            if (value.find('[') != std::string::npos && value.find(']') != std::string::npos) {
                // Parse as array - simple implementation
                try {
                    json array = json::parse(value);
                    result[key] = array;
                } catch (...) {
                    result[key] = value;
                }
            } else {
                result[key] = value;
            }
        }
    }
    
    return result;
}

// Helper to convert nested YAML to flat key-value pairs
std::unordered_map<std::string, std::string> flattenYaml(const YAML::Node& node, const std::string& prefix = "") {
    std::unordered_map<std::string, std::string> result;
    
    if (node.IsMap()) {
        for (const auto& item : node) {
            std::string key = prefix + item.first.as<std::string>();
            if (item.second.IsMap()) {
                // Recursively flatten nested objects
                auto nested = flattenYaml(item.second, key + ".");
                result.insert(nested.begin(), nested.end());
            } else {
                result[key] = item.second.as<std::string>();
            }
        }
    }
    
    return result;
}

// Helper to convert YAML value to ConfigValue
ConfigValue yamlToConfigValue(const YAML::Node& value) {
    if (value.IsScalar()) {
        std::string str = value.as<std::string>();
        
        // Try to convert to boolean
        if (str == "true") return true;
        if (str == "false") return false;
        
        // Try to convert to integer
        try {
            size_t pos;
            int intVal = std::stoi(str, &pos);
            if (pos == str.size()) return intVal;
        } catch (...) {}
        
        // Try to convert to double
        try {
            size_t pos;
            double doubleVal = std::stod(str, &pos);
            if (pos == str.size()) return doubleVal;
        } catch (...) {}
        
        return str;
    } else if (value.IsSequence()) {
        // Convert YAML sequence to ConfigValue
        std::vector<std::string> array;
        for (const auto& item : value) {
            array.push_back(yamlToConfigValue(item).as<std::string>());
        }
        return array;
    } else if (value.IsMap()) {
        // For nested objects, return as string representation
        std::string result = "{";
        for (const auto& item : value) {
            result += item.first.as<std::string>() + ":" + 
                     std::visit([](const auto& v) { return std::to_string(v); }, yamlToConfigValue(item.second)) + ",";
        }
        if (!result.empty()) result.back() = '}';
        return result;
    } else {
        return "null";
    }
}

// Check file extension to determine format
bool isJsonFile(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".json";
}

bool isYamlFile(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".yaml" || ext == ".yml";
}

// Error handling
void logConfigError(const std::string& filepath, const ConfigParseError& error) {
    std::cerr << "Config parsing error in file \"" << filepath << "\": " << error.getMessage() << std::endl;
}

// Implementation of Config class methods

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first
    if (isJsonFile(filepath)) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            json data = json::parse(file);
            
            // Convert to flat key-value pairs for backward compatibility
            auto flatData = flattenJson(data);
            
            // Restore the flat key=value format for backward compatibility
            clear();
            for (const auto& item : flatData) {
                set(item.first, item.second, ConfigSource::File);
            }
            
            NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
            return true;
            
        } catch (const json::parse_error& e) {
            NLM_LOG_ERROR("JSON parse error in file " + filepath + ": " + std::string(e.what()));
            return false;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error loading JSON config from " + filepath + ": " + std::string(e.what()));
            return false;
        }
    } 
    // Try YAML
    else if (isYamlFile(filepath)) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            YAML::Node data = YAML::LoadFile(filepath);
            
            // Convert to flat key-value pairs for backward compatibility
            auto flatData = flattenYaml(data);
            
            // Restore the flat key=value format for backward compatibility
            clear();
            for (const auto& item : flatData) {
                set(item.first, item.second, ConfigSource::File);
            }
            
            NLM_LOG_INFO("Loaded configuration from YAML file: " + filepath);
            return true;
            
        } catch (const YAML::Exception& e) {
            NLM_LOG_ERROR("YAML parse error in file " + filepath + ": " + std::string(e.what()));
            return false;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error loading YAML config from " + filepath + ": " + std::string(e.what()));
            return false;
        }
    }
    
    // Try simple key=value format for backward compatibility
    else {
        NLM_LOG_WARNING("Unknown config file format, trying simple key=value format for " + filepath);
        // Use original implementation for backward compatibility
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
        
        NLM_LOG_INFO("Loaded configuration from simple file: " + filepath);
        return true;
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Create JSON structure
    json data = json::object();
    
    for (const auto& entry : pImpl->entries) {
        // Add to JSON with description as comment
        data[entry.key] = configValueToJson(entry.value);
    }
    
    try {
        file << data.dump(2); // Pretty print with 2-space indentation
        NLM_LOG_INFO("Saved configuration to JSON file: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving JSON config to " + filepath + ": " + std::string(e.what()));
        return false;
    }
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
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::optional<int> Config::get<int>(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Implementation details omitted for brevity
    return std::nullopt;
}

// Other template specializations for different types

} // namespace nlm