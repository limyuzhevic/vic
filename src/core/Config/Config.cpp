#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <variant>
#include <vector>
#include <unordered_map>

using json = nlohmann::json;
using namespace nlm;

// Helper functions for Config
namespace {

// Convert string to lowercase for comparison
// Used for case-insensitive key comparison and file extension detection
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Trim whitespace from both ends
// Removes leading and trailing whitespace, tabs, and newlines
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Convert string to specific type with validation
// Attempts to parse string as target type, throws on failure or invalid format
template<typename T>
T stringToValue(const std::string& str) {
    std::istringstream iss(str);
    T value;
    if (iss >> value) {
        // Check if there are remaining characters (except whitespace)
        char remaining;
        if (iss >> remaining) {
            throw std::invalid_argument("Extra characters after value: '" + str + "'");
        }
        return value;
    }
    throw std::invalid_argument("Cannot convert string to " + std::string(typeid(T).name()) + ": '" + str + "'");
}

// Convert JSON value to ConfigValue
// Supports conversion from JSON types to variant ConfigValue types
ConfigValue jsonToConfigValue(const json& j) {
    if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        // Handle arrays - attempt to determine most specific type
        std::vector<int64_t> intArray;
        std::vector<double> doubleArray;
        std::vector<std::string> stringArray;
        
        bool hasInt = true, hasDouble = true, hasString = true;
        
        for (const auto& item : j) {
            if (hasInt) {
                try {
                    intArray.push_back(item.get<int64_t>());
                } catch (...) {
                    hasInt = false;
                }
            }
            if (hasDouble) {
                try {
                    doubleArray.push_back(item.get<double>());
                } catch (...) {
                    hasDouble = false;
                }
            }
            if (hasString) {
                try {
                    stringArray.push_back(item.get<std::string>());
                } catch (...) {
                    hasString = false;
                }
            }
            if (!hasInt && !hasDouble && !hasString) break;
        }
        
        // Prefer most specific type: int > double > string
        if (hasInt && !intArray.empty()) return intArray;
        if (hasDouble && !doubleArray.empty()) return doubleArray;
        if (hasString && !stringArray.empty()) return stringArray;
        
        // Fallback to string array if all other type conversions fail
        std::vector<std::string> fallbackArray;
        for (const auto& item : j) {
            fallbackArray.push_back(item.get<std::string>());
        }
        return fallbackArray;
    } else {
        throw std::invalid_argument("Unsupported JSON type for ConfigValue conversion");
    }
}

// Convert ConfigValue to JSON value
// Converts variant ConfigValue to JSON for serialization
json configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return arg;
        } else {
            throw std::invalid_argument("Unsupported ConfigValue type for JSON serialization");
        }
    }, value);
}

// Convert YAML node to ConfigValue
// Converts YAML node to variant ConfigValue for deserialization
ConfigValue yamlToConfigValue(const YAML::Node& node) {
    if (node.IsScalar()) {
        std::string value = node.Scalar();
        
        // Try to convert to specific types with validation
        try {
            return stringToValue<int64_t>(value);
        } catch (...) {}
        try {
            return stringToValue<double>(value);
        } catch (...) {}
        try {
            return stringToValue<bool>(value);
        } catch (...) {}
        
        return value;
    } else if (node.IsSequence()) {
        // Handle sequences/arrays - attempt to determine most specific type
        std::vector<int64_t> intArray;
        std::vector<double> doubleArray;
        std::vector<std::string> stringArray;
        
        bool hasInt = true, hasDouble = true, hasString = true;
        
        for (const auto& item : node) {
            std::string strValue = item.Scalar();
            if (hasInt) {
                try {
                    intArray.push_back(stringToValue<int64_t>(strValue));
                } catch (...) {
                    hasInt = false;
                }
            }
            if (hasDouble) {
                try {
                    doubleArray.push_back(stringToValue<double>(strValue));
                } catch (...) {
                    hasDouble = false;
                }
            }
            if (hasString) {
                stringArray.push_back(strValue);
            }
            if (!hasInt && !hasDouble && !hasString) break;
        }
        
        // Prefer most specific type: int > double > string
        if (hasInt && !intArray.empty()) return intArray;
        if (hasDouble && !doubleArray.empty()) return doubleArray;
        if (!stringArray.empty()) return stringArray;
        
        return stringArray;
    } else if (node.IsMap()) {
        // Convert YAML map to JSON object first, then to ConfigValue
        json j = json::object();
        for (const auto& it : node) {
            j[it.first.Scalar()] = yamlToConfigValue(it.second);
        }
        return jsonToConfigValue(j);
    }
    
    // Return empty string for unsupported node types
    return "";
}

// Convert ConfigValue to YAML node
// Converts variant ConfigValue to YAML node for serialization
YAML::Node configValueToYaml(const ConfigValue& value) {
    YAML::Node node;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else {
            throw std::invalid_argument("Unsupported ConfigValue type for YAML serialization");
        }
    }, value);
    return node;
}

// Parse simple key=value format for backward compatibility
// Converts legacy config files (key=value format) to ConfigValue entries
bool parseSimpleFormat(const std::string& line, std::vector<ConfigEntry>& entries) {
    // Parse simple key=value pairs
    size_t pos = line.find('=');
    if (pos != std::string::npos) {
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present (support "value" or 'value' syntax)
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Determine value type with validation
        try {
            bool boolVal = stringToValue<bool>(value);
            entries.emplace_back(key, ConfigValue(boolVal), ConfigSource::File);
        } catch (...) {
            try {
                int64_t intVal = stringToValue<int64_t>(value);
                entries.emplace_back(key, ConfigValue(intVal), ConfigSource::File);
            } catch (...) {
                try {
                    double doubleVal = stringToValue<double>(value);
                    entries.emplace_back(key, ConfigValue(doubleVal), ConfigSource::File);
                } catch (...) {
                    // String value as fallback
                    entries.emplace_back(key, ConfigValue(value), ConfigSource::File);
                }
            }
        }
        return true;
    }
    return false;
}

} // namespace

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigSource> keySources;
    bool isFileLoaded;
    std::string loadedFilePath;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->isFileLoaded = false;
    pImpl->loadedFilePath = "";
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Check file extension for format detection
        std::string ext = "." + std::string(std::filesystem::path(filepath).extension().string());
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        pImpl->entries.clear();
        pImpl->keySources.clear();
        pImpl->isFileLoaded = false;
        pImpl->loadedFilePath = "";
        
        // Clear existing entries
        clear();
        
        if (ext == ".json") {
            // Parse JSON format
            std::ifstream file(filepath);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file: '" + filepath + "'");
            }
            
            json j;
            file >> j;
            
            for (auto it = j.begin(); it != j.end(); ++it) {
                try {
                    ConfigValue value = jsonToConfigValue(it.value());
                    set(it.key(), value, ConfigSource::File);
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Failed to parse JSON key '" << it.key() 
                              << "': " << e.what() << std::endl;
                }
            }
            
            pImpl->isFileLoaded = true;
            pImpl->loadedFilePath = filepath;
            
        } else if (ext == ".yaml" || ext == ".yml") {
            // Parse YAML format
            try {
                YAML::Node doc = YAML::LoadFile(filepath);
                
                if (doc.IsMap()) {
                    for (const auto& it : doc) {
                        try {
                            ConfigValue value = yamlToConfigValue(it.second);
                            set(it.first.as<std::string>(), value, ConfigSource::File);
                        } catch (const std::exception& e) {
                            std::cerr << "Warning: Failed to parse YAML key '" 
                                      << it.first.as<std::string>() << "': " << e.what() << std::endl;
                        }
                    }
                    
                    pImpl->isFileLoaded = true;
                    pImpl->loadedFilePath = filepath;
                }
            } catch (const YAML::Exception& e) {
                throw std::runtime_error("YAML parsing error: '" + std::string(e.what()) + "'");
            }
            
        } else {
            // Try backward compatibility: parse as simple key=value format
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            std::string line;
            while (std::getline(file, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                // Try to parse as key=value
                if (parseSimpleFormat(line, pImpl->entries)) {
                    // Update key sources
                    for (auto& entry : pImpl->entries) {
                        pImpl->keySources[entry.key] = ConfigSource::File;
                    }
                    
                    pImpl->isFileLoaded = true;
                    pImpl->loadedFilePath = filepath;
                    break; // For backward compatibility, only parse first line
                }
            }
            
            if (pImpl->entries.empty()) {
                return false;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading config from file '" << filepath 
                  << "': " << e.what() << std::endl;
        clear();
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        clear();
        
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
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading config from arguments: " << e.what() << std::endl;
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Determine format based on file extension
        std::string ext = "." + std::string(std::filesystem::path(filepath).extension().string());
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: '" + filepath + "'");
        }
        
        if (ext == ".json") {
            // Save as JSON
            json j = json::object();
            
            for (const auto& entry : pImpl->entries) {
                try {
                    j[entry.key] = configValueToJson(entry.value);
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Failed to serialize key '" << entry.key 
                              << "': " << e.what() << std::endl;
                }
            }
            
            file << std::setw(4) << j;
            
        } else if (ext == ".yaml" || ext == ".yml") {
            // Save as YAML
            YAML::Emitter emitter;
            emitter << YAML::BeginMap;
            
            for (const auto& entry : pImpl->entries) {
                emitter << YAML::Key << entry.key;
                try {
                    YAML::Node node = configValueToYaml(entry.value);
                    emitter << node;
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Failed to serialize key '" << entry.key 
                              << "': " << e.what() << std::endl;
                    emitter << YAML::Null;
                }
            }
            
            emitter << YAML::EndMap;
            file << emitter.c_str();
            
        } else {
            // Save as simple key=value format (backward compatibility)
            for (const auto& entry : pImpl->entries) {
                // Add comment if available
                if (!entry.description.empty()) {
                    file << "# " << entry.description << "\n";
                }
                
                // Convert value to string
                std::visit([&file](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        file << entry.key << " = \"" << arg << "\"\n";
                    } else if constexpr (std::is_same_v<T, bool>) {
                        file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                    } else {
                        file << entry.key << " = " << arg << "\n";
                    }
                }, entry.value);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving config to file '" << filepath 
                  << "': " << e.what() << std::endl;
        return false;
    }
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
    pImpl->keySources.erase(key);
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
    pImpl->keySources.clear();
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
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>> ||
                               std::is_same_v<T, std::vector<double>> ||
                               std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        oss << "\"" << item << "\"";
                    } else {
                        oss << item;
                    }
                    first = false;
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