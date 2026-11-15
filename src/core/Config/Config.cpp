#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <stdexcept>

// JSON support - try to include nlohmann::json
try {
    #include <nlohmann/json.hpp>
    #define HAS_NLOHMANN_JSON 1
} catch (...) {
    #define HAS_NLOHMANN_JSON 0
}

// YAML support - try to include yaml-cpp
try {
    #include <yaml-cpp/yaml.h>
    #define HAS_YAML_CPP 1
} catch (...) {
    #define HAS_YAML_CPP 0
}

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Detect file format by extension or content
    std::string ext = "." + std::string(std::filesystem::path(filepath).extension());
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    // Try to parse based on file extension
    if (ext == ".json" && HAS_NLOHMANN_JSON) {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml" && HAS_YAML_CPP) {
        return loadFromYAML(filepath);
    } else {
        // Try to detect format by content
        std::ifstream peekFile(filepath);
        std::string firstLine;
        if (std::getline(peekFile, firstLine)) {
            std::string trimmed = trim(firstLine);
            if (trimmed.empty()) {
                return loadFromSimpleFormat(filepath);
            } else if (trimmed.find('{') != std::string::npos && trimmed.find('}') != std::string::npos) {
                // Looks like JSON
                return loadFromJSON(filepath);
            } else if (trimmed.find("---") != std::string::npos || trimmed.find(": ") != std::string::npos) {
                // Looks like YAML
                return loadFromYAML(filepath);
            }
        }
        
        // Fallback to simple format
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format consistently
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                // Handle --key value format (legacy support)
                if (i + 1 < argc) {
                    std::string key = arg.substr(2);
                    std::string value = argv[++i];
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format (legacy support)
        else if (arg[0] == '-' && arg.size() > 1) {
            std::string key = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Determine output format based on extension
    std::string ext = "." + std::string(std::filesystem::path(filepath).extension());
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    // JSON output
    if (ext == ".json" && HAS_NLOHMANN_JSON) {
        return saveToJSON(filepath);
    } 
    // YAML output
    else if (ext == ".yaml" || ext == ".yml" && HAS_YAML_CPP) {
        return saveToYAML(filepath);
    } 
    // Simple format output (default)
    else {
        return saveToSimpleFormat(filepath);
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

// JSON parsing implementation
bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAS_NLOHMANN_JSON
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        clear();
        
        // Parse JSON object
        if (jsonData.is_object()) {
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                
                // Convert JSON value to ConfigValue
                ConfigValue configValue;
                try {
                    configValue = jsonToConfigValue(it.value());
                    set(key, configValue, ConfigSource::File);
                } catch (const std::exception& e) {
                    // Skip invalid values but log error
                    (void)e; // Ignore for now
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log error: JSON parsing failed
        return false;
    }
#else
    return false;
#endif
}

// YAML parsing implementation
bool Config::loadFromYAML(const std::string& filepath) {
#ifdef HAS_YAML_CPP
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        YAML::Node yamlData;
        file >> yamlData;
        
        // Clear existing entries
        clear();
        
        // Parse YAML node
        if (yamlData.IsMap()) {
            for (const auto& node : yamlData) {
                std::string key = node.first.as<std::string>();
                
                // Convert YAML value to ConfigValue
                ConfigValue configValue;
                try {
                    configValue = yamlToConfigValue(node.second);
                    set(key, configValue, ConfigSource::File);
                } catch (const std::exception& e) {
                    // Skip invalid values but log error
                    (void)e; // Ignore for now
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log error: YAML parsing failed
        return false;
    }
#else
    return false;
#endif
}

// Simple format parsing implementation (backward compatibility)
bool Config::loadFromSimpleFormat(const std::string& filepath) {
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
                 (value.front() == \'\' && value.back() == \'\'))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

// JSON to ConfigValue conversion
ConfigValue Config::jsonToConfigValue(const nlohmann::json& jsonValue) {
#ifdef HAS_NLOHMANN_JSON
    if (jsonValue.is_string()) {
        return jsonValue.get<std::string>();
    } else if (jsonValue.is_number_integer()) {
        return jsonValue.get<int64_t>();
    } else if (jsonValue.is_number_float()) {
        return jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>();
    } else if (jsonValue.is_array()) {
        // Convert array to appropriate vector type
        if (!jsonValue.empty()) {
            // Check array element types
            bool allInt = true, allDouble = true, allString = true;
            for (const auto& item : jsonValue) {
                if (!item.is_number_integer()) allInt = false;
                if (!item.is_number_float()) allDouble = false;
                if (!item.is_string()) allString = false;
            }
            
            if (allInt) {
                std::vector<int64_t> result;
                for (const auto& item : jsonValue) {
                    result.push_back(item.get<int64_t>());
                }
                return result;
            } else if (allDouble) {
                std::vector<double> result;
                for (const auto& item : jsonValue) {
                    result.push_back(item.get<double>());
                }
                return result;
            } else if (allString) {
                std::vector<std::string> result;
                for (const auto& item : jsonValue) {
                    result.push_back(item.get<std::string>());
                }
                return result;
            }
        }
        // Default to vector<double> for mixed numeric types
        std::vector<double> result;
        for (const auto& item : jsonValue) {
            result.push_back(item.get<double>());
        }
        return result;
    } else if (jsonValue.is_null()) {
        return std::string("");
    }
#endif
    throw std::runtime_error("Unsupported JSON value type");
}

// YAML to ConfigValue conversion
ConfigValue Config::yamlToConfigValue(const YAML::Node& yamlValue) {
#ifdef HAS_YAML_CPP
    if (yamlValue.IsScalar()) {
        const std::string& valueStr = yamlValue.as<std::string>();
        
        // Try to determine type from string
        if (valueStr == "true" || valueStr == "false") {
            return valueStr == "true";
        }
        
        // Try to parse as integer
        try {
            if (valueStr.find('.') == std::string::npos) {
                return std::stoll(valueStr);
            } else {
                return std::stod(valueStr);
            }
        } catch (...) {
            // Not a number, treat as string
        }
        
        return valueStr;
    } else if (yamlValue.IsSequence()) {
        // Convert YAML sequence to appropriate vector type
        if (!yamlValue.size()) {
            return std::vector<double>{};
        }
        
        // Check element types
        bool allInt = true, allDouble = true, allString = true;
        for (const auto& node : yamlValue) {
            if (!node.IsScalar()) {
                allInt = allDouble = allString = false;
                break;
            }
            const std::string& val = node.as<std::string>();
            if (val == "true" || val == "false") {
                allInt = allDouble = false;
            } else {
                try {
                    if (val.find('.') == std::string::npos) {
                        std::stoll(val);
                    } else {
                        std::stod(val);
                    }
                } catch (...) {
                    allInt = allDouble = false;
                }
            }
        }
        
        if (allInt) {
            std::vector<int64_t> result;
            for (const auto& node : yamlValue) {
                result.push_back(node.as<int64_t>());
            }
            return result;
        } else if (allDouble) {
            std::vector<double> result;
            for (const auto& node : yamlValue) {
                result.push_back(node.as<double>());
            }
            return result;
        } else if (allString) {
            std::vector<std::string> result;
            for (const auto& node : yamlValue) {
                result.push_back(node.as<std::string>());
            }
            return result;
        }
        
        // Default to vector<double>
        std::vector<double> result;
        for (const auto& node : yamlValue) {
            result.push_back(node.as<double>());
        }
        return result;
    } else if (yamlValue.IsNull()) {
        return std::string("");
    }
#endif
    throw std::runtime_error("Unsupported YAML value type");
}

// ConfigValue to JSON conversion
nlohmann::json Config::configValueToJSON(const ConfigValue& configValue) const {
#ifdef HAS_NLOHMANN_JSON
    return std::visit([](const auto& v) -> nlohmann::json {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return v;
        } else if constexpr (std::is_same_v<T, double>) {
            return v;
        } else if constexpr (std::is_same_v<T, bool>) {
            return v;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            return nlohmann::json(v);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return nlohmann::json(v);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return nlohmann::json(v);
        } else {
            throw std::runtime_error("Unsupported ConfigValue type for JSON serialization");
        }
    }, configValue);
#else
    throw std::runtime_error("nlohmann::json not available");
#endif
}

// ConfigValue to YAML conversion
YAML::Node Config::configValueToYAML(const ConfigValue& configValue) const {
#ifdef HAS_YAML_CPP
    YAML::Node node;
    std::visit([&](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            node = v;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            node = v;
        } else if constexpr (std::is_same_v<T, double>) {
            node = v;
        } else if constexpr (std::is_same_v<T, bool>) {
            node = v;
        }185} else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            for (const auto& item : v) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : v) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : v) {
                node.push_back(item);
            }
        } else {
            throw std::runtime_error("Unsupported ConfigValue type for YAML serialization");
        }
    }, configValue);
    return node;
#else
    throw std::runtime_error("yaml-cpp not available");
#endif
}

bool Config::saveToFile(const std::string& filepath) const {
    // Determine output format based on extension
    std::string ext = "." + std::string(std::filesystem::path(filepath).extension());
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    // JSON output
    if (ext == ".json" && HAS_NLOHMANN_JSON) {
        return saveToJSON(filepath);
    } 
    // YAML output
    else if (ext == ".yaml" || ext == ".yml" && HAS_YAML_CPP) {
        return saveToYAML(filepath);
    } 
    // Simple format output (default)
    else {
        return saveToSimpleFormat(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
#ifdef HAS_NLOHMANN_JSON
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        
        // Convert config entries to JSON
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = configValueToJSON(entry.value);
        }
        
        // Write JSON with pretty formatting
        file << std::setw(2) << jsonData << std::endl;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
#else
    return false;
#endif
}

bool Config::saveToYAML(const std::string& filepath) const {
#ifdef HAS_YAML_CPP
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        // Convert config entries to YAML
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            emitter << YAML::Value << configValueToYAML(entry.value);
        }
        
        emitter << YAML::EndMap;
        
        // Write YAML content
        file << emitter.c_str();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
#else
    return false;
#endif
}

bool Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Format value based on type
        std::visit([&file](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << v << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                file << "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << v[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << v[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << v[i] << "\"";
                }
                file << "]";
            } else {
                file << v;
            }
        }, entry.value);
        
        file << "\n";
    }
    
    return true;
}

// Helper methods for the Config class
ConfigValue Config::jsonToConfigValue(const nlohmann::json& jsonValue);
ConfigValue Config::yamlToConfigValue(const YAML::Node& yamlValue);
nlohmann::json Config::configValueToJSON(const ConfigValue& configValue) const;
YAML::Node Config::configValueToYAML(const ConfigValue& configValue) const;

} // namespace nlm
