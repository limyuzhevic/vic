#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

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
    
    try {
        // Parse JSON configuration file
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        // Convert JSON to key-value pairs
        std::vector<std::pair<std::string, std::string>> entries;
        
        for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
            std::string key = it.key();
            std::string value;
            
            // Handle different value types
            if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_number_integer()) {
                value = std::to_string(it->get<int64_t>());
            } else if (it->is_number_unsigned()) {
                value = std::to_string(it->get<uint64_t>());
            } else if (it->is_number_float()) {
                value = std::to_string(it->get<double>());
            } else if (it->is_boolean()) {
                value = it->get<bool>() ? "true" : "false";
            } else if (it->is_array()) {
                // Convert array to JSON string representation
                value = it->dump();
            } else {
                // Skip null values
                continue;
            }
            
            entries.emplace_back(key, value);
        }
        
        // Add entries to config
        for (const auto& entry : entries) {
            set(entry.first, entry.second, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Failed to parse command line argument --" + key + "=" + value + ": " + std::string(e.what()));
                    success = false;
                }
            } else {
                NLM_LOG_WARNING("Invalid command line argument format: " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to parse command line argument -" + key + " " + value + ": " + std::string(e.what()));
                success = false;
            }
        }
        // Handle --key value (without =)
        else if (arg.substr(0, 2) == "--") {
            // Check if next argument is a value (not another --option)
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string key = arg.substr(2);
                std::string value = argv[++i];
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Failed to parse command line argument --" + key + " " + value + ": " + std::string(e.what()));
                    success = false;
                }
            } else {
                NLM_LOG_WARNING("Missing value for command line argument: " + arg);
                success = false;
            }
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonConfig;
        
        for (const auto& entry : pImpl->entries) {
            std::string key = entry.key;
            
            // Convert ConfigValue to JSON
            std::visit([&jsonConfig, &key](auto&& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonConfig[key] = value;
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    jsonConfig[key] = static_cast<int64_t>(value);
                } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                    jsonConfig[key] = static_cast<double>(value);
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonConfig[key] = value;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    jsonConfig[key] = value;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonConfig[key] = value;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonConfig[key] = value;
                }
            }, entry.value);
        }
        
        // Write formatted JSON to file
        file << std::setw(2) << jsonConfig << std::endl;
        
        NLM_LOG_INFO("Configuration saved to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save config file " + filepath + ": " + std::string(e.what()));
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
        oss << "] (source=" << static_cast<int>(entry.source) << ")\n";
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

// Define configuration-specific exceptions
namespace nlm {
    
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigFileException : public ConfigException {
public:
    ConfigFileException(const std::string& filepath, const std::string& reason)
        : ConfigException("Config file error: " + filepath + " - " + reason) {}
};

class ConfigParseException : public ConfigException {
public:
    ConfigParseException(const std::string& filepath, const std::string& reason)
        : ConfigException("Config parse error: " + filepath + " - " + reason) {}
};

class ConfigArgumentException : public ConfigException {
public:
    ConfigArgumentException(const std::string& argument, const std::string& reason)
        : ConfigException("Config argument error: --" + argument + " - " + reason) {}
};

// Helper function to validate configuration entries
bool Config::validateEntry(const std::string& key, const ConfigValue& value) const {
    // Basic validation - can be extended with type-specific validation
    if (key.empty()) {
        return false;
    }
    
    // Check for invalid key characters
    if (key.find('\n') != std::string::npos || key.find('\r') != std::string::npos ||
        key.find('\t') != std::string::npos) {
        return false;
    }
    
    return true;
}

// Enhanced set method with validation and exception support
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!validateEntry(key, value)) {
        throw ConfigException("Invalid configuration entry: key or value validation failed");
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Update existing entry
        it->value = value;
        it->source = source;
        NLM_LOG_INFO("Updated configuration entry: " + key + " = " + getValueString(value));
    } else {
        // Add new entry
        pImpl->entries.emplace_back(key, value, source, "Configuration entry");
        NLM_LOG_INFO("Added configuration entry: " + key + " = " + getValueString(value));
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

// Helper function to convert ConfigValue to string for logging
std::string Config::getValueString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << v << "\"";
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << v;
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            oss << v;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (v ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                             std::is_same_v<T, std::vector<double>> ||
                             std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i) oss << ", ";
                oss << v[i];
            }
            oss << "]";
        } else {
            oss << "<unknown>";
        }
    }, value);
    return oss.str();
}

// Enhanced loadFromFile with better error handling
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    try {
        // Parse JSON configuration file
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        // Validate JSON structure
        if (!jsonConfig.is_object()) {
            throw ConfigParseException(filepath, "Root element must be a JSON object");
        }
        
        // Convert JSON to key-value pairs
        std::vector<std::pair<std::string, std::string>> entries;
        
        for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
            std::string key = it.key();
            std::string value;
            
            // Handle different value types
            if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_number_integer()) {
                value = std::to_string(it->get<int64_t>());
            } else if (it->is_number_unsigned()) {
                value = std::to_string(it->get<uint64_t>());
            } else if (it->is_number_float()) {
                value = std::to_string(it->get<double>());
            } else if (it->is_boolean()) {
                value = it->get<bool>() ? "true" : "false";
            } else if (it->is_array()) {
                // Convert array to JSON string representation
                value = it->dump();
            } else {
                // Skip null values
                NLM_LOG_WARNING("Skipping null value for key: " + key);
                continue;
            }
            
            if (validateEntry(key, ConfigValue(value))) {
                entries.emplace_back(key, value);
            } else {
                NLM_LOG_WARNING("Invalid configuration entry (key: " + key + ") - skipping");
            }
        }
        
        // Add entries to config
        for (const auto& entry : entries) {
            try {
                set(entry.first, entry.second, ConfigSource::File);
            } catch (const ConfigException& e) {
                NLM_LOG_ERROR("Failed to set configuration entry " + entry.first + ": " + e.what());
                // Continue processing other entries
            }
        }
        
        NLM_LOG_INFO("Successfully loaded " + std::to_string(entries.size()) + " configuration entries from: " + filepath);
        return true;
        
    } catch (const ConfigParseException& e) {
        NLM_LOG_ERROR(std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error parsing config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Enhanced saveToFile with better error handling
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonConfig;
        
        for (const auto& entry : pImpl->entries) {
            std::string key = entry.key;
            
            // Convert ConfigValue to JSON
            std::visit([&jsonConfig, &key](auto&& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonConfig[key] = value;
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    jsonConfig[key] = static_cast<int64_t>(value);
                } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                    jsonConfig[key] = static_cast<double>(value);
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonConfig[key] = value;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    jsonConfig[key] = value;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonConfig[key] = value;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonConfig[key] = value;
                }
            }, entry.value);
        }
        
        // Write formatted JSON to file
        file << std::setw(2) << jsonConfig << std::endl;
        
        NLM_LOG_INFO("Configuration saved to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Enhanced loadFromArgs with better argument validation
bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const ConfigException& e) {
                    NLM_LOG_WARNING("Failed to parse command line argument --" + key + "=" + value + ": " + e.what());
                    success = false;
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Failed to parse command line argument --" + key + "=" + value + ": " + std::string(e.what()));
                    success = false;
                }
            } else {
                NLM_LOG_WARNING("Invalid command line argument format: " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const ConfigException& e) {
                NLM_LOG_WARNING("Failed to parse command line argument -" + key + " " + value + ": " + e.what());
                success = false;
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to parse command line argument -" + key + " " + value + ": " + std::string(e.what()));
                success = false;
            }
        }
        // Handle --key value (without =)
        else if (arg.substr(0, 2) == "--" && i + 1 < argc && argv[i + 1][0] != '-') {
            std::string key = arg.substr(2);
            std::string value = argv[++i];
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const ConfigException& e) {
                NLM_LOG_WARNING("Failed to parse command line argument --" + key + " " + value + ": " + e.what());
                success = false;
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to parse command line argument --" + key + " " + value + ": " + std::string(e.what()));
                success = false;
            }
        } else {
            // Unknown argument
            NLM_LOG_WARNING("Unknown argument ignored: " + arg);
            success = false;
        }
    }
    
    return success;
}

// Clear all entries
void Config::clear() {
    if (!pImpl->entries.empty()) {
        NLM_LOG_INFO("Cleared " + std::to_string(pImpl->entries.size()) + " configuration entries");
    }
    pImpl->entries.clear();
}

// Enhanced summary with more details
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "  Entries by source:" << std::endl;
    
    // Count entries by source
    int defaultCount = 0, fileCount = 0, commandLineCount = 0, runtimeCount = 0;
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default: ++defaultCount; break;
            case ConfigSource::File: ++fileCount; break;
            case ConfigSource::CommandLine: ++commandLineCount; break;
            case ConfigSource::Runtime: ++runtimeCount; break;
        }
    }
    
    if (defaultCount > 0) oss << "    Default: " << defaultCount << std::endl;
    if (fileCount > 0) oss << "    File: " << fileCount << std::endl;
    if (commandLineCount > 0) oss << "    Command Line: " << commandLineCount << std::endl;
    if (runtimeCount > 0) oss << "    Runtime: " << runtimeCount << std::endl;
    
    oss << std::endl << "Detailed entries:" << std::endl;
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        oss << getValueString(entry.value) << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << std::endl;
    }
    
    return oss.str();
}

} // namespace nlm
