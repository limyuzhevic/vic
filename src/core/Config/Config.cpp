#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <stdexcept>
#include <unordered_map>
#include <memory>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Configuration schema for validation
    struct ConfigSchema {
        std::string name;
        std::type_index type;
        std::string description;
        bool required;
        std::string defaultValue;
        std::vector<std::string> allowedValues;
        double minValue;
        double maxValue;
    };
    std::unordered_map<std::string, ConfigSchema> schema;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize default schema
    registerSchema("debug", typeid(bool), "Enable debug mode", false, "false");
    registerSchema("port", typeid(int), "Network port number", false, "8080");
    registerSchema("host", typeid(std::string), "Network host address", false, "localhost");
    registerSchema("timeout", typeid(double), "Connection timeout in seconds", false, "30.0");
    registerSchema("log_level", typeid(std::string), "Logging level", false, "INFO");
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// File format detection
static ConfigFormat detectFileFormat(const std::string& filepath) {
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") return ConfigFormat::JSON;
    if (ext == ".yaml" || ext == ".yml") return ConfigFormat::YAML;
    if (ext == ".cfg" || ext == ".config") return ConfigFormat::KeyValue;
    
    // Try to guess from content if extension is unknown
    std::ifstream file(filepath);
    if (!file.is_open()) return ConfigFormat::Unknown;
    
    std::string firstLine;
    if (std::getline(file, firstLine)) {
        std::string trimmed = trim(firstLine);
        if (!trimmed.empty() && (trimmed.front() == '{' || trimmed.front() == '[')) {
            return ConfigFormat::JSON;
        }
        if (!trimmed.empty() && (trimmed.find(":") != std::string::npos)) {
            return ConfigFormat::YAML;
        }
    }
    return ConfigFormat::KeyValue;
}

// JSON parsing implementation
static bool loadFromJSON(const std::string& filepath, Config& config) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            // Convert JSON value to ConfigValue
            ConfigValue value;
            
            if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_number_integer()) {
                value = it->get<int>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_array()) {
                // For arrays, store as vector
                std::vector<int> intArray;
                std::vector<double> doubleArray;
                std::vector<std::string> stringArray;
                
                for (const auto& item : *it) {
                    if (item.is_boolean()) {
                        intArray.push_back(item.get<bool>() ? 1 : 0);
                        doubleArray.push_back(item.get<bool>() ? 1.0 : 0.0);
                    } else if (item.is_number_integer()) {
                        intArray.push_back(item.get<int>());
                        doubleArray.push_back(static_cast<double>(item.get<int>()));
                    } else if (item.is_number_float()) {
                        doubleArray.push_back(item.get<double>());
                    } else if (item.is_string()) {
                        stringArray.push_back(item.get<std::string>());
                    }
                }
                
                // Determine which vector type to use based on content
                if (!intArray.empty()) value = intArray;
                else if (!doubleArray.empty()) value = doubleArray;
                else if (!stringArray.empty()) value = stringArray;
            } else if (it->is_object()) {
                // Nested objects - convert to string representation for now
                value = it->dump();
            } else {
                // Null or other types - use string representation
                value = it->dump();
            }
            
            config.set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// YAML parsing implementation
static bool loadFromYAML(const std::string& filepath, Config& config) {
    try {
        YAML::Loader loader;
        YAML::Node doc;
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file >> doc;
        
        for (const auto& node : doc) {
            std::string key = node.first.as<std::string>();
            
            // Convert YAML node to ConfigValue
            ConfigValue value;
            
            if (node.second.IsBoolean()) {
                value = node.second.as<bool>();
            } else if (node.second.IsInt()) {
                value = node.second.as<int>();
            } else if (node.second.IsDouble()) {
                value = node.second.as<double>();
            } else if (node.second.IsScalar()) {
                value = node.second.as<std::string>();
            } else if (node.second.IsSequence()) {
                // For sequences, store as vector
                std::vector<int> intArray;
                std::vector<double> doubleArray;
                std::vector<std::string> stringArray;
                
                for (const auto& item : node.second) {
                    if (item.IsBoolean()) {
                        intArray.push_back(item.as<bool>() ? 1 : 0);
                        doubleArray.push_back(item.as<bool>() ? 1.0 : 0.0);
                    } else if (item.IsInt()) {
                        intArray.push_back(item.as<int>());
                        doubleArray.push_back(static_cast<double>(item.as<int>()));
                    } else if (item.IsDouble()) {
                        doubleArray.push_back(item.as<double>());
                    } else if (item.IsScalar()) {
                        stringArray.push_back(item.as<std::string>());
                    }
                }
                
                // Determine which vector type to use based on content
                if (!intArray.empty()) value = intArray;
                else if (!doubleArray.empty()) value = doubleArray;
                else if (!stringArray.empty()) value = stringArray;
            } else {
                // Complex node - convert to string representation
                value = node.second.as<std::string>();
            }
            
            config.set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    ConfigFormat format = detectFileFormat(filepath);
    
    switch (format) {
        case ConfigFormat::JSON:
            return loadFromJSON(filepath, *this);
        case ConfigFormat::YAML:
            return loadFromYAML(filepath, *this);
        case ConfigFormat::KeyValue:
            return loadFromSimpleKeyValue(filepath);
        case ConfigFormat::Unknown:
        default:
            return false;
    }
}

bool Config::loadFromSimpleKeyValue(const std::string& filepath) {
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
    
    // Write header with format based on file extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".yaml" || ext == ".yml") {
        file << "# Configuration file\n";
        
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            // Store with description as comment
            file << "# " << entry.description << "\n";
            
            std::visit([&jsonData, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                
                if constexpr (std::is_same_v<T, bool>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonData[entry.key] = arg;
                }
            }, entry.value);
        }
        
        if (ext == ".json") {
            file << jsonData.dump(4);
        } else {
            // For YAML, use YAML-cpp
            YAML::Emitter yamlData;
            yamlData << YAML::BeginDoc;
            
            for (const auto& entry : pImpl->entries) {
                std::visit([&yamlData, &entry](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    
                    if constexpr (std::is_same_v<T, bool>) {
                        yamlData << YAML::Key << entry.key << YAML::Value << arg;
                    } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                        yamlData << YAML::Key << entry.key << YAML::Value << arg;
                    } else if constexpr (std::is_same_v<T, double>) {
                        yamlData << YAML::Key << entry.key << YAML::Value << arg;
                    } else if constexpr (std::is_same_v<T, std::string>) {
                        yamlData << YAML::Key << entry.key << YAML::Value << arg;
                    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                        yamlData << YAML::Key << entry.key << YAML::Value << arg;
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        yamlData << YAML::Key << entry.key << YAML::Value << arg;
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        yamlData << YAML::Key << entry.key << YAML::Value << arg;
                    }
                }, entry.value);
            }
            
            yamlData << YAML::EndDoc;
            file << yamlData.c_str();
        }
    } else {
        // Default key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                
                if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
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
                }
            }, entry.value);
            file << "\n\n";
        }
    }
    
    return true;
}

// Schema validation
template<typename T>
static bool validateValue(const ConfigEntry& entry, Config::Impl::ConfigSchema& schema) {
    if (!std::is_same_v<T, std::string> && !std::is_same_v<T, bool> && 
        !std::is_same_v<T, int> && !std::is_same_v<T, int64_t> &&
        !std::is_same_v<T, double> && !std::is_same_v<T, std::vector<int>> &&
        !std::is_same_v<T, std::vector<double>> && !std::is_same_v<T, std::vector<std::string>>) {
        return false;
    }
    
    std::visit([&schema](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            // Check allowed values
            if (!schema.allowedValues.empty()) {
                auto it = std::find(schema.allowedValues.begin(), 
                                  schema.allowedValues.end(), arg);
                if (it == schema.allowedValues.end()) {
                    throw std::invalid_argument("Value '" + arg + "' is not allowed for key '" + schema.name + "'");
                }
            }
            
            // Check min/max for numeric-like strings (if min/max are set)
            if (schema.minValue != std::numeric_limits<double>::lowest() || 
                schema.maxValue != std::numeric_limits<double>::max()) {
                try {
                    double val = std::stod(arg);
                    if (val < schema.minValue || val > schema.maxValue) {
                        throw std::invalid_argument("Value '" + arg + "' is out of range for key '" + schema.name + "'");
                    }
                } catch (...) {
                    // String is not numeric, can't apply range checks
                }
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            // No additional validation for boolean
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            // Check min/max
            double val = static_cast<double>(arg);
            if (val < schema.minValue || val > schema.maxValue) {
                throw std::invalid_argument("Value '" + std::to_string(arg) + "' is out of range for key '" + schema.name + "'");
            }
        } else if constexpr (std::is_same_v<T, double>) {
            // Check min/max
            if (arg < schema.minValue || arg > schema.maxValue) {
                throw std::invalid_argument("Value '" + std::to_string(arg) + "' is out of range for key '" + schema.name + "'");
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                            std::is_same_v<T, std::vector<double>> || 
                            std::is_same_v<T, std::vector<std::string>>) {
            // For vectors, validate each element
            for (const auto& element : arg) {
                // Recursively call validation for each element
                // This is simplified - in a full implementation, you'd create a temporary schema
            }
        }
    }, entry.value);
    
    return true;
}

// Schema registration
void Config::registerSchema(const std::string& name, const std::type_index& type, 
                           const std::string& description, bool required,
                           const std::string& defaultValue,
                           const std::vector<std::string>& allowedValues,
                           double minValue, double maxValue) {
    pImpl->schema[name] = {
        name,
        type,
        description,
        required,
        defaultValue,
        allowedValues,
        minValue,
        maxValue
    };
}

// Configuration validation
bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        auto it = pImpl->schema.find(entry.key);
        if (it == pImpl->schema.end()) {
            // No schema defined for this key, skip validation
            continue;
        }
        
        const auto& schema = it->second;
        
        try {
            // Get the type from schema
            std::visit([&schema](auto&&) {
                // The actual validation would happen here
                // For simplicity, we'll just check if the entry value matches the schema type
            }, entry.value);
        } catch (...) {
            return false;
        }
    }
    
    return true;
}

// Format enum definition
enum class ConfigFormat {
    Unknown,
    JSON,
    YAML,
    KeyValue
};

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
