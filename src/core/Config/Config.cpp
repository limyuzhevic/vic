#include "nlohmann/json.hpp"
#include "yaml-cpp/yaml.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

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
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Try to parse as JSON first (if nlohmann/json is available)
    try {
        #ifdef HAVE_NLOHMANN_JSON
        return parseJSON(content, filepath);
        #else
        // Fall back to simple parser if JSON library not available
        return parseSimple(content, filepath);
        #endif
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("JSON parsing failed, falling back to simple parser: " + std::string(e.what());
        return parseSimple(content, filepath);
    }
}

// Helper function to parse JSON from string
bool Config::parseJSON(const std::string& content, const std::string& filepath) {
    auto json = nlohmann::json::parse(content);
    
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        std::string description = "Configuration parameter loaded from JSON";
        
        try {
            // Handle different JSON value types
            if (it->is_string()) {
                set(key, it->get<std::string>(), ConfigSource::File);
            } else if (it->is_number_integer()) {
                set(key, it->get<int64_t>(), ConfigSource::File);
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), ConfigSource::File);
            } else if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::File);
            } else if (it->is_array()) {
                // Convert array to appropriate vector type based on content
                if (!it->empty()) {
                    const auto& first = it->front();
                    if (first.is_string()) {
                        std::vector<std::string> strArray;
                        for (const auto& item : *it) {
                            strArray.push_back(item.get<std::string>());
                        }
                        set(key, strArray, ConfigSource::File);
                    } else if (first.is_number_integer()) {
                        std::vector<int64_t> intArray;
                        for (const auto& item : *it) {
                            intArray.push_back(item.get<int64_t>());
                        }
                        set(key, intArray, ConfigSource::File);
                    } else if (first.is_number_float()) {
                        std::vector<double> doubleArray;
                        for (const auto& item : *it) {
                            doubleArray.push_back(item.get<double>());
                        }
                        set(key, doubleArray, ConfigSource::File);
                    }
                }
            } else {
                NLM_LOG_WARNING("Unsupported JSON type for key '" + key + "', skipping");
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to parse JSON value for key '" + key + "': " + std::string(e.what()));
        }
    }
    
    return true;
}

// Helper function to parse YAML from string  
bool Config::parseYAML(const std::string& content, const std::string& filepath) {
    auto yaml = YAML::Load(content);
    
    for (const auto& node : yaml) {
        std::string key = node.first.as<std::string>();
        std::string description = "Configuration parameter loaded from YAML";
        
        try {
            // Handle different YAML value types
            if (node.second.IsScalar()) {
                std::string value = node.second.as<std::string>();
                // Try to detect if it's actually a number or boolean
                if (value.find('.') != std::string::npos || value.find('e') != std::string::npos || 
                    value.find('E') != std::string::npos) {
                    try {
                        double numValue = std::stod(value);
                        set(key, numValue, ConfigSource::File);
                    } catch (...) {
                        set(key, value, ConfigSource::File);
                    }
                } else if (value == "true" || value == "false") {
                    bool boolValue = (value == "true");
                    set(key, boolValue, ConfigSource::File);
                } else {
                    set(key, value, ConfigSource::File);
                }
            } else if (node.second.IsSequence()) {
                // Convert YAML sequence to appropriate vector type
                if (!node.second.IsNull()) {
                    YAML::Node seq = node.second;
                    if (!seq.empty()) {
                        // Check type of first element
                        YAML::Node first = seq[0];
                        if (first.IsScalar()) {
                            std::string firstValue = first.as<std::string>();
                            if (firstValue.find('.') != std::string::npos || firstValue.find('e') != std::string::npos || 
                                firstValue.find('E') != std::string::npos) {
                                std::vector<double> doubleArray;
                                for (const auto& item : seq) {
                                    doubleArray.push_back(item.as<double>());
                                }
                                set(key, doubleArray, ConfigSource::File);
                            } else {
                                std::vector<std::string> strArray;
                                for (const auto& item : seq) {
                                    strArray.push_back(item.as<std::string>());
                                }
                                set(key, strArray, ConfigSource::File);
                            }
                        } else if (first.IsInteger()) {
                            std::vector<int64_t> intArray;
                            for (const auto& item : seq) {
                                intArray.push_back(item.as<int64_t>());
                            }
                            set(key, intArray, ConfigSource::File);
                        }
                    }
                }
            } else {
                NLM_LOG_WARNING("Unsupported YAML type for key '" + key + "', skipping");
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to parse YAML value for key '" + key + "': " + std::string(e.what()));
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
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Write the key
        file << entry.key << " = ";
        
        // Format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int> ||
                                 std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, double> ||
                                 std::is_same_v<T, float>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "\"" << arg[i] << "\"";
                    } else {
                        file << arg[i];
                    }
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n";
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config value: key cannot be empty");
        return;
    }
    
    // Check for reserved keys that shouldn't be changed at runtime
    std::string lowerKey = toLower(key);
    if (lowerKey == "random_seed" || lowerKey == "neuron_count" || lowerKey == "region_count" || lowerKey == "connection_probability") {
        NLM_LOG_WARNING("Runtime modification of key '" + key + "' may have undefined behavior");
    }
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    pImpl->entries.emplace_back(key, value, source);
    
    // Log the setting for debugging
    if (source == ConfigSource::Runtime) {
        NLM_LOG_INFO("Config set: " + key + " = " + valueToString(value));
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config value: key cannot be empty");
        return;
    }
    
    if (!std::all_of(key.begin(), key.end(), [](char c) {
        return std::isalnum(c) || c == '_' || c == '-';
    })) {
        NLM_LOG_WARNING("Config key '" + key + "' contains non-alphanumeric characters");
    }
    
    // Validate value length to prevent memory issues
    if (value.length() > 1024) {
        NLM_LOG_WARNING("Config value for key '" + key + "' is very large (>1KB)");
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config value: key cannot be empty");
        return;
    }
    
    // Validate value ranges if needed
    if (value < -1000000 || value > 1000000) {
        NLM_LOG_WARNING("Config value for key '" + key + "' is outside typical range: " + std::to_string(value));
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config value: key cannot be empty");
        return;
    }
    
    // Validate value ranges if needed
    if (value < -1000000000 || value > 1000000000) {
        NLM_LOG_WARNING("Config value for key '" + key + "' is outside typical range: " + std::to_string(value));
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config value: key cannot be empty");
        return;
    }
    
    // Validate value ranges if needed
    if (std::isnan(value) || std::isinf(value)) {
        NLM_LOG_ERROR("Invalid double value for config key '" + key + "': " + std::to_string(value));
        return;
    }
    
    if (value < -1e9 || value > 1e9) {
        NLM_LOG_WARNING("Config value for key '" + key + "' is outside typical range: " + std::to_string(value));
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config value: key cannot be empty");
        return;
    }
    
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

bool Config::isValidKey(const std::string& key) {
    if (key.empty()) {
        return false;
    }
    
    // Keys must start with alphanumeric character
    if (!std::isalnum(key[事業)) {
        return false;
    }
    
    // Keys can contain alphanumeric characters, underscores, and hyphens
    return std::all_of(key.begin(), key.end(), [](char c) {
        return std::isalnum(c) || c == '_' || c == '-';
    });
}

std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, int64_t> ||
                             std::is_same_v<T, int> ||
                             std::is_same_v<T, double> ||
                             std::is_same_v<T, float>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
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
            oss << "[unknown type]";
        }
    }, value);
    return oss.str();
}

bool Config::parseSimple(const std::string& content, const std::string& filepath) {
    std::stringstream ss(content);
    std::string line;
    
    while (std::getline(ss, line)) {
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
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
