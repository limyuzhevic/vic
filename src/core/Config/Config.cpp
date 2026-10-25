#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

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
    if (loadFromJSON(filepath)) {
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    }
    
    // Try YAML second
    if (loadFromYAML(filepath)) {
        NLM_LOG_INFO("Loaded configuration from YAML file: " + filepath);
        return true;
    }
    
    // Fall back to simple text format
    return loadFromSimpleTextFormat(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Parse JSON object
        if (jsonData.is_object()) {
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                
                // Convert JSON value to ConfigValue
                ConfigValue configValue;
                try {
                    configValue = jsonToConfigValue(*it);
                    set(key, configValue, ConfigSource::File);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Failed to parse JSON value for key '" + key + "': " + std::string(e.what()));
                }
            }
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON file '" + filepath + "': " + std::string(e.what()));
    }
    
    return false;
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Try to parse as YAML using a simple approach
        // In a real implementation, you would use a YAML parser like yaml-cpp
        return loadFromSimpleYAML(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML file '" + filepath + "': " + std::string(e.what()));
    }
    
    return false;
}

bool Config::loadFromSimpleYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line.find("#") == 0) {
            continue;
        }
        
        // Parse YAML key: value format
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
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

bool Config::loadFromSimpleTextFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
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
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_WARNING("Empty key found in config file at line " + std::to_string(lineNumber));
                continue;
            }
            
            set(key, value, ConfigSource::File);
            NLM_LOG_DEBUG("Loaded config: " + key + " = " + value);
        } else {
            NLM_LOG_WARNING("Invalid config line format at line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + " configuration entries from " + filepath);
    return true;
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& json) {
    if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_number_integer()) {
        return json.get<int64_t>();
    } else if (json.is_number_unsigned()) {
        return json.get<uint64_t>();
    } else if (json.is_number_float()) {
        return json.get<double>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_array()) {
        // Handle arrays based on element types
        nlohmann::json::array_t arr = json.get<nlohmann::json::array_t>();
        if (!arr.empty()) {
            if (arr[0].is_number_integer()) {
                std::vector<int64_t> vec;
                for (const auto& item : arr) {
                    vec.push_back(item.get<int64_t>());
                }
                return vec;
            } else if (arr[0].is_number_float()) {
                std::vector<double> vec;
                for (const auto& item : arr) {
                    vec.push_back(item.get<double>());
                }
                return vec;
            } else if (arr[0].is_string()) {
                std::vector<std::string> vec;
                for (const auto& item : arr) {
                    vec.push_back(item.get<std::string>());
                }
                return vec;
            }
        }
        // Default to empty vector
        return std::vector<int64_t>{};
    }
    
    // Default to empty string
    return std::string();
}

std::string Config::configValueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        }
    }, value);
    return oss.str();
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
    
    // Write header comment
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM configuration system\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        
        // Format the value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Escape quotes and newlines
                std::string escaped = arg;
                size_t pos = 0;
                while ((pos = escaped.find('"', pos)) != std::string::npos) {
                    escaped.replace(pos, 1, "\\\"");
                    pos += 2;
                }
                file << entry.key << " = \"" << escaped << "\"\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]\n";
            } else {
                // Simple type (int, int64_t, double, bool)
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << "\n";
    }
    
    NLM_LOG_INFO("Saved " + std::to_string(pImpl->entries.size()) + " configuration entries to " + filepath);
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
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config with empty key");
        return;
    }
    
    // Check if key already exists
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Update existing entry
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated config key: " + key);
    } else {
        // Add new entry with empty description
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Added config key: " + key);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config with empty key");
        return;
    }
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config with empty key");
        return;
    }
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config with empty key");
        return;
    }
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config with empty key");
        return;
    }
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot check config with empty key");
        return false;
    }
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot remove config with empty key");
        return;
    }
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    NLM_LOG_DEBUG("Removed config key: " + key);
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
    if (!pImpl->entries.empty()) {
        NLM_LOG_INFO("Clearing all configuration entries (" + std::to_string(pImpl->entries.size()) + " entries)");
        pImpl->entries.clear();
    }
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    oss << "Source breakdown:\n";
    
    std::unordered_map<ConfigSource, int> sourceCounts;
    for (const auto& entry : pImpl->entries) {
        sourceCounts[entry.source]++;
    }
    
    for (const auto& [source, count] : sourceCounts) {
        oss << "  " << static_cast<int>(source) << ": " << count << " entries\n";
    }
    
    oss << "\nEntries:\n";
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
        oss << "] (source=" << static_cast<int>(entry.source) << ")";
        
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

// Validation methods
bool Config::validate() const {
    bool isValid = true;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            NLM_LOG_ERROR("Empty key found in configuration");
            isValid = false;
        }
        
        if (entry.key.find(' ') != std::string::npos) {
            NLM_LOG_WARNING("Key contains spaces: " + entry.key);
        }
        
        // Check for duplicate keys
        for (const auto& other : pImpl->entries) {
            if (&other != &entry && other.key == entry.key) {
                NLM_LOG_WARNING("Duplicate key found in configuration: " + entry.key);
                // This is not necessarily an error, but worth noting
            }
        }
    }
    
    return isValid;
}

bool Config::isValidKey(const std::string& key) {
    return !key.empty() && key.find(' ') == std::string::npos &&
           std::find_if(key.begin(), key.end(), ::ispunct) == key.end();
}

bool Config::isValidValue(const std::string& value) {
    // Check for invalid characters
    return !value.empty();
}

// Get configuration statistics
Config::ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    
    for (const auto& entry : pImpl->entries) {
        stats.entriesBySource[entry.source]++;
        
        std::visit([&stats](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                stats.stringEntries++;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                stats.intEntries++;
            } else if constexpr (std::is_same_v<T, double>) {
                stats.doubleEntries++;
            } else if constexpr (std::is_same_v<T, bool>) {
                stats.boolEntries++;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                stats.vectorEntries++;
            }
        }, entry.value);
    }
    
    return stats;
}

// Merge configurations
void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Use the specified source for all merged entries
        set(entry.key, entry.value, source);
        
        // Preserve description from original if available
        if (!entry.description.empty()) {
            setDescription(entry.key, entry.description);
        }
    }
}

// Find entries by source
std::vector<std::string> Config::findKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
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
