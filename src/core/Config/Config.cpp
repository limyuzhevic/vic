#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <ctime>
#include <iomanip>
#include <cstring>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string lastFilepath;
    bool jsonParserAvailable;
    bool yamlParserAvailable;
    
    Impl() : entries(), lastFilepath(), 
             jsonParserAvailable(true), yamlParserAvailable(true) {
        // Check if JSON parser is available
        #ifdef NLM_HAS_JSON
        jsonParserAvailable = true;
        #else
        jsonParserAvailable = false;
        #endif
        
        // Check if YAML parser is available
        #ifdef NLM_HAS_YAML
        yamlParserAvailable = true;
        #else
        yamlParserAvailable = false;
        #endif
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format by extension
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < filepath.length() - 1) {
        extension = filepath.substr(dotPos + 1);
    }
    
    bool success = false;
    
    // Try JSON format first
    if (extension == "json" || extension == "jsonc" || extension.empty()) {
        success = loadFromJSONFile(filepath);
        if (success) {
            pImpl->lastFilepath = filepath;
            return true;
        }
    }
    
    // Try YAML format
    if (!success && (extension == "yaml" || extension == "yml")) {
        success = loadFromYAMLFile(filepath);
        if (success) {
            pImpl->lastFilepath = filepath;
            return true;
        }
    }
    
    // Fallback to simple key=value format
    if (!success) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open config file: " + filepath);
        }
        
        std::string line;
        size_t lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            try {
                // Skip empty lines and comments
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                // Parse simple key=value pairs
                size_t pos = line.find('=');
                if (pos == std::string::npos) {
                    throw std::runtime_error("Invalid format at line " + std::to_string(lineNumber) + ": expected 'key=value'");
                }
                
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                throw std::runtime_error("Error parsing config file " + filepath + " at line " + 
                    std::to_string(lineNumber) + ": " + e.what());
            }
        }
        
        pImpl->lastFilepath = filepath;
        return true;
    }
    
    return false;
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open JSON file: " + filepath);
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Convert JSON to Config entries
        for (const auto& item : jsonData.items()) {
            std::string key = item.key();
            ConfigValue value;
            
            if (item.value().is_string()) {
                value = item.value().get<std::string>();
            } else if (item.value().is_number_integer()) {
                value = item.value().get<int64_t>();
            } else if (item.value().is_number_float()) {
                value = item.value().get<double>();
            } else if (item.value().is_boolean()) {
                value = item.value().get<bool>();
            } else if (item.value().is_array()) {
                // Convert array to appropriate type
                auto arr = item.value().get<nlohmann::json::array_t>();
                if (!arr.empty()) {
                    if (std::all_of(arr.begin(), arr.end(), 
                                   [](const auto& v) { return v.is_string(); })) {
                        value = arr.get<std::vector<std::string>>();
                    } else if (std::all_of(arr.begin(), arr.end(), 
                                          [](const auto& v) { return v.is_number_integer(); })) {
                        value = arr.get<std::vector<int>>();
                    } else if (std::all_of(arr.begin(), arr.end(), 
                                          [](const auto& v) { return v.is_number_float(); })) {
                        value = arr.get<std::vector<double>>();
                    } else {
                        // Mixed types - convert to strings
                        std::vector<std::string> strVec;
                        for (const auto& v : arr) {
                            strVec.push_back(v.dump());
                        }
                        value = strVec;
                    }
                } else {
                    value = std::vector<std::string>();
                }
            } else {
                // For other types (null, object), convert to string
                value = item.value().dump();
            }
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error parsing JSON file " + filepath + ": " + e.what());
    }
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    try {
        YAML::Loader loader;
        YAML::Node yamlData;
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open YAML file: " + filepath);
        }
        
        file >> yamlData;
        
        // Convert YAML to Config entries
        for (const auto& yamlItem : yamlData) {
            std::string key = yamlItem.first.as<std::string>();
            ConfigValue value;
            
            YAML::Node node = yamlItem.second;
            
            if (node.IsScalar()) {
                if (node.IsDouble()) {
                    value = node.as<double>();
                } else if (node.IsInt()) {
                    value = node.as<int64_t>();
                } else if (node.IsBool()) {
                    value = node.as<bool>();
                } else {
                    value = node.as<std::string>();
                }
            } else if (node.IsSequence()) {
                // Convert sequence to appropriate type
                std::vector<std::string> strVec;
                for (const auto& item : node) {
                    if (item.IsDouble()) {
                        strVec.push_back(std::to_string(item.as<double>()));
                    } else if (item.IsInt()) {
                        strVec.push_back(std::to_string(item.as<int64_t>()));
                    } else if (item.IsBool()) {
                        strVec.push_back(item.as<bool>() ? "true" : "false");
                    } else {
                        strVec.push_back(item.as<std::string>());
                    }
                }
                value = strVec;
            } else if (node.IsMap()) {
                // Convert nested mapping to JSON string
                std::stringstream ss;
                ss << node;
                value = ss.str();
            } else {
                value = node.as<std::string>();
            }
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error parsing YAML file " + filepath + ": " + e.what());
    }
}

// Helper functions for string processing
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

// Template instantiations for get<T> and getOr<T>
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    // Convert value to type T
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else if constexpr (std::is_same_v<V, std::string> && std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<V, std::vector<int>> && std::is_same_v<T, std::vector<int>>) {
            return v;
        } else if constexpr (std::is_same_v<V, std::vector<double>> && std::is_same_v<T, std::vector<double>>) {
            return v;
        } else if constexpr (std::is_same_v<V, std::vector<std::string>> && std::is_same_v<T, std::vector<std::string>>) {
            return v;
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
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << std::fixed << std::setprecision(6) << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
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

std::string Config::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ", ";
                oss << std::fixed << std::setprecision(6) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

std::string Config::getLastLoadedFile() const {
    return pImpl->lastFilepath;
}

bool Config::validate() const {
    // Basic validation: check for empty keys and null values
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            return false;
        }
        if (std::holds_alternative<std::monostate>(entry.value)) {
            return false;
        }
    }
    return true;
}

std::string Config::validateErrors() const {
    std::string errors;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            errors += "Empty configuration key\n";
        }
    }
    return errors;
}

// Explicit template instantiations for commonly used types
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

} // namespace nlm