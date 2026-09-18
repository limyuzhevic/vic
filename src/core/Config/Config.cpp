#include "Config.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <cctype>
#include <filesystem>

namespace nlm {

namespace {
    // Helper function to trim string
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    // Helper function to convert string to lowercase
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    // Helper function to determine if a string ends with specific extension
    bool hasExtension(const std::string& filename, const std::string& ext) {
        if (filename.length() < ext.length()) return false;
        return toLower(filename.substr(filename.length() - ext.length())) == toLower(ext);
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file type based on extension
    std::string extension = std::filesystem::path(filepath).extension().string();
    
    if (extension.empty()) {
        // No extension, try legacy parser
        return parseLegacyConfig(filepath, ConfigSource::File);
    }
    
    // Try appropriate parser based on extension
    if (hasExtension(filepath, ".json")) {
        // Parse JSON file
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            nlohmann::json jsonData;
            file >> jsonData;
            parseJsonConfig(jsonData, ConfigSource::File);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    } else if (hasExtension(filepath, ".yaml") || hasExtension(filepath, ".yml")) {
        // Parse YAML file
        try {
            YAML::Node yamlData = YAML::LoadFile(filepath);
            parseYamlConfig(yamlData, "", ConfigSource::File);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    } else {
        // Try legacy parser for backward compatibility
        return parseLegacyConfig(filepath, ConfigSource::File);
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Modern command line parsing
    // Handle --key=value format
    // Handle -key value format
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Try to parse as JSON
                try {
                    nlohmann::json jsonValue = nlohmann::json::parse(value);
                    setFromJsonValue(key, jsonValue, ConfigSource::CommandLine);
                } catch (...) {
                    // Fall back to string
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Try to parse as JSON
            try {
                nlohmann::json jsonValue = nlohmann::json::parse(value);
                setFromJsonValue(key, jsonValue, ConfigSource::CommandLine);
            } catch (...) {
                // Fall back to string
                set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save all entries in key=value format for backward compatibility
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
            } else if constexpr (std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t> || 
                                std::is_same_v<T, double> || 
                                std::is_same_v<T, bool>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                oss << "[vector]";
            } else {
                oss << "unknown";
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

void Config::setFromJsonValue(const std::string& key, const nlohmann::json& value, ConfigSource source) {
    if (value.is_string()) {
        set(key, value.get<std::string>(), source);
    } else if (value.is_number_integer()) {
        set(key, value.get<int>(), source);
    } else if (value.is_number_unsigned()) {
        set(key, static_cast<int64_t>(value.get<uint64_t>()), source);
    } else if (value.is_number_float()) {
        set(key, value.get<double>(), source);
    } else if (value.is_boolean()) {
        set(key, value.get<bool>(), source);
    } else if (value.is_null()) {
        set(key, std::string("null"), source);
    } else if (value.is_array()) {
        // For arrays, convert to string representation for now
        std::string arrayStr = "[";
        for (size_t i = 0; i < value.size(); ++i) {
            if (i > 0) arrayStr += ",";
            if (value[i].is_string()) {
                arrayStr += "\"" + value[i].get<std::string>() + "\"";
            } else {
                arrayStr += std::to_string(value[i].get<double>());
            }
        }
        arrayStr += "]";
        set(key, arrayStr, source);
    } else if (!value.is_object()) {
        // For unknown types, convert to string representation
        set(key, value.dump(), source);
    }
}

void Config::parseJsonConfig(const nlohmann::json& jsonData, ConfigSource source) {
    if (jsonData.is_object()) {
        for (const auto& [key, value] : jsonData.items()) {
            if (value.is_object()) {
                // Parse nested object recursively
                nlohmann::json nestedValue = value;
                parseJsonConfig(nestedValue, source);
            } else {
                setFromJsonValue(key, value, source);
            }
        }
    }
}

void Config::parseYamlConfig(const YAML::Node& node, const std::string& prefix, ConfigSource source) {
    if (node.IsMap()) {
        for (const auto& kv : node) {
            std::string key = kv.first.as<std::string>();
            std::string fullKey = prefix.empty() ? key : (prefix + "." + key);
            parseYamlConfig(kv.second, fullKey, source);
        }
    } else if (!prefix.empty() && node.IsScalar()) {
        // Leaf node - try to convert to appropriate type
        const std::string& value = node.as<std::string>();
        
        // Try to detect and convert to appropriate type
        // Check for boolean
        if (value == "true" || value == "false") {
            set(prefix, value == "true", source);
            return;
        }
        
        // Check for integer
        try {
            size_t pos;
            long long_val = std::stoll(value, &pos);
            if (pos == value.size()) {
                set(prefix, static_cast<int>(long_val), source);
                return;
            }
        } catch (...) {}
        
        // Check for unsigned integer
        try {
            size_t pos;
            unsigned long ulong_val = std::stoul(value, &pos);
            if (pos == value.size()) {
                set(prefix, static_cast<int64_t>(ulong_val), source);
                return;
            }
        } catch (...) {}
        
        // Check for floating point
        try {
            size_t pos;
            double double_val = std::stod(value, &pos);
            if (pos == value.size()) {
                set(prefix, double_val, source);
                return;
            }
        } catch (...) {}
        
        // Check for null
        if (value == "null" || value == "NULL" || value == "" || value == "None") {
            set(prefix, std::string("null"), source);
            return;
        }
        
        // Default to string
        set(prefix, value, source);
    }
}

bool Config::parseLegacyConfig(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool success = true;
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
            
            set(key, value, source);
        } else {
            success = false;
        }
    }
    return success;
}

} // namespace nlm
