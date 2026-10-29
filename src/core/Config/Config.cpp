#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <stdexcept>

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
    
    // Try to parse as JSON first
    try {
        nlohmann::json j;
        file >> j;
        
        clear();
        
        // Process JSON object
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Handle nested configuration (e.g., "brain.neuron_count")
            size_t dotPos = key.find('.');
            if (dotPos != std::string::npos) {
                std::string parentKey = key.substr(0, dotPos);
                std::string childKey = key.substr(dotPos + 1);
                
                // Create or get nested config
                auto parentIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                             [&parentKey](const ConfigEntry& e) { return e.key == parentKey; });
                
                if (parentIt == pImpl->entries.end()) {
                    // Create nested config for parent
                    NestedConfig nested;
                    nested.values[childKey] = json_utils::fromJson(*it);
                    set(parentKey, nested, ConfigSource::File);
                } else {
                    // Update existing nested config
                    auto& nested = std::get<NestedConfig>(parentIt->value);
                    nested.values[childKey] = json_utils::fromJson(*it);
                }
            } else {
                // Top-level key
                set(key, json_utils::fromJson(*it), ConfigSource::File);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        // Fall back to simple key=value format for backward compatibility
        file.clear();
        file.seekg(0);
        
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
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
        // Handle --nested.key=value format for nested configurations
        else if (arg.substr(0, 2) == "--" && arg.find('=') != std::string::npos) {
            size_t keyPos = arg.find('=');
            std::string fullKey = arg.substr(2, keyPos - 2);
            std::string value = arg.substr(keyPos + 1);
            
            // Handle nested key with dot notation
            set(fullKey, value, ConfigSource::CommandLine);
        }
        // Handle boolean flags
        else if (arg == "--help" || arg == "--version" || arg == "--verbose") {
            set(arg.substr(2), true, ConfigSource::CommandLine);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json j;
    
    for (const auto& entry : pImpl->entries) {
        if (std::holds_alternative<NestedConfig>(entry.value)) {
            const auto& nested = std::get<NestedConfig>(entry.value);
            for (const auto& [childKey, childValue] : nested.values) {
                j[entry.key + '.' + childKey] = json_utils::toJson(childValue);
            }
        } else {
            j[entry.key] = json_utils::toJson(entry.value);
        }
    }
    
    file << std::setw(2) << j << std::endl;
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
        // Try to get nested config value
        if (std::holds_alternative<NestedConfig>(it->value)) {
            const auto& nested = std::get<NestedConfig>(it->value);
            // Simple implementation - assumes key is a nested key without dot
            if (nested.values.count(key)) {
                return std::get<T>(nested.values.at(key));
            }
        }
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the key
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    // Check if key contains only valid characters
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '.') {
            throw std::invalid_argument("Configuration key contains invalid characters: " + key);
        }
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
            } else if constexpr (std::is_same_v<T, NestedConfig>) {
                oss << "{" << arg.values.size() << " nested values}";
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

// NestedConfig implementation
void NestedConfig::merge(const NestedConfig& other) {
    for (const auto& [key, value] : other.values) {
        if (values.count(key)) {
            // Override with new value
            values[key] = value;
        } else {
            values[key] = value;
        }
    }
}

nlohmann::json NestedConfig::toJson() const {
    nlohmann::json j;
    for (const auto& [key, value] : values) {
        j[key] = json_utils::toJson(value);
    }
    return j;
}

NestedConfig NestedConfig::fromJson(const nlohmann::json& j) {
    NestedConfig nested;
    for (auto it = j.begin(); it != j.end(); ++it) {
        nested.values[it.key()] = json_utils::fromJson(*it);
    }
    return nested;
}

std::string NestedConfig::getString(const std::string& key) const {
    return getValue<std::string>(key, "string");
}

int NestedConfig::getInt(const std::string& key) const {
    return getValue<int>(key, "int");
}

double NestedConfig::getDouble(const std::string& key) const {
    return getValue<double>(key, "double");
}

bool NestedConfig::getBool(const std::string& key) const {
    return getValue<bool>(key, "bool");
}

std::vector<std::string> NestedConfig::getStringArray(const std::string& key) const {
    return getValue<std::vector<std::string>>(key, "string array");
}

std::vector<int> NestedConfig::getIntArray(const std::string& key) const {
    return getValue<std::vector<int>>(key, "int array");
}

std::vector<double> NestedConfig::getDoubleArray(const std::string& key) const {
    return getValue<std::vector<double>>(key, "double array");
}

template<typename T>
T NestedConfig::getValue(const std::string& key, const std::string& typeName) const {
    auto it = values.find(key);
    if (it == values.end()) {
        throw std::runtime_error("Key not found in nested config: " + key);
    }
    
    try {
        return std::get<T>(it->second);
    } catch (const std::bad_variant_access&) {
        throw std::runtime_error("Value for key " + key + " is not a " + typeName);
    }
}

// JSON utilities implementation
namespace json_utils {
    nlohmann::json toJson(const ConfigValue& value) {
        nlohmann::json j;
        std::visit([&j](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                j = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                j = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                j = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                j = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j = arg;
            } else if constexpr (std::is_same_v<T, NestedConfig>) {
                j = arg.toJson();
            }
        }, value);
        return j;
    }
    
    ConfigValue fromJson(const nlohmann::json& j) {
        if (j.is_string()) {
            return j.get<std::string>();
        } else if (j.is_number_integer()) {
            return j.get<int>();
        } else if (j.is_number_float()) {
            return j.get<double>();
        } else if (j.is_boolean()) {
            return j.get<bool>();
        } else if (j.is_array()) {
            // Try to infer array type
            if (!j.empty()) {
                if (j[0].is_string()) {
                    return j.get<std::vector<std::string>>();
                } else if (j[0].is_number_integer()) {
                    return j.get<std::vector<int>>();
                } else if (j[0].is_number_float()) {
                    return j.get<std::vector<double>>();
                }
            }
            // Fallback to vector<int> for empty arrays or unknown types
            return j.get<std::vector<int>>();
        } else if (j.is_object()) {
            return NestedConfig::fromJson(j);
        } else {
            throw std::runtime_error("Unsupported JSON type in configuration");
        }
    }
    
    std::string valueToString(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
} // namespace json_utils

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

// Note: The following files need to be created:
// 1. python/CMakeLists.txt - to add nlohmann::json dependency
// 2. test_refactored_config.cpp - to test the enhanced Config system
