#include "Config.hpp"
#include "../Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

/**
 * @brief Default constructor - creates empty configuration
 * 
 * @note Memory is managed through pImpl idiom for efficient copying and moving.
 *       Internal storage is opaque to users for encapsulation.
 */
Config::Config() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Config: Created empty configuration");
}

/**
 * @brief Destroy configuration and clean up implementation
 */
Config::~Config() = default;

/**
 * @brief Move constructor (deleted)
 * 
 * @note Default implementation enables efficient moving while preventing copying.
 *       Prevents shallow copy of internal configuration entries.
 */
Config::Config(Config&&) noexcept = default;

/**
 * @brief Move assignment operator (deleted)
 * 
 * @note Default implementation enables efficient moving while preventing copying.
 */
Config& Config::operator=(Config&&) noexcept = default;

/**
 * @brief Load configuration from file with simple key=value format
 * 
 * @param filepath Path to configuration file
 * @return true if file loaded successfully, false otherwise
 * 
 * @note Supports:
 *       - Simple key=value pairs
 *       - Comments starting with # or /
 *       - Quoted values (supports spaces in values)
 *       - Case-sensitive keys
 * 
 * @warning Currently uses simple text parser. Consider implementing
 *         full JSON/YAML parser for complex configurations.
 * 
 * @see ConfigEntry, ConfigSource
 */
bool Config::loadFromFile(const std::string& filepath) {
    NLM_LOG_INFO("Config: Loading configuration from " + filepath);
    
    // Validate file path
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config: Empty filepath provided");
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Config: Failed to open file: " + filepath);
        return false;
    }
    
    size_t lineCount = 0;
    size_t parsedCount = 0;
    
    std::string line;
    while (std::getline(file, line)) {
        ++lineCount;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Config: Invalid line format at " + filepath + ":" + 
                          std::to_string(lineCount) + " - " + line);
            continue;
        }
        
        try {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (key.empty()) {
                NLM_LOG_WARNING("Config: Empty key at line " + std::to_string(lineCount));
                continue;
            }
            
            // Remove quotes if present (support both " and ' quoting)
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
            ++parsedCount;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Config: Error parsing line " + std::to_string(lineCount) + 
                        " in " + filepath + ": " + e.what());
            continue;
        }
    }
    
    NLM_LOG_INFO("Config: Loaded " + std::to_string(parsedCount) + " entries from " + 
                std::to_string(lineCount) + " lines in " + filepath);
    
    return true;
}

/**
 * @brief Load configuration from command-line arguments
 * 
 * @param argc Number of command-line arguments
 * @param argv Array of command-line arguments
 * @return true if arguments processed successfully, false otherwise
 * 
 * @note Supports two formats:
 *       1. --key=value (long options)
 *       2. -key value (short options)
 *       
 * @note Command-line arguments override configuration file settings.
 *       Last value for a key wins.
 */
bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        NLM_LOG_INFO("Config: No command-line arguments provided");
        return true;
    }
    
    NLM_LOG_INFO("Config: Processing " + std::to_string(argc - 1) + " command-line arguments");
    
    size_t parsedCount = 0;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format (long options)
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                try {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    if (key.empty()) {
                        NLM_LOG_WARNING("Config: Empty key in argument " + arg);
                        continue;
                    }
                    
                    set(key, value, ConfigSource::CommandLine);
                    ++parsedCount;
                    
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Config: Error parsing argument " + arg + ": " + e.what());
                }
            } else {
                NLM_LOG_WARNING("Config: Malformed --key=value argument: " + arg);
            }
        }
        // Handle -key value format (short options)
        else if (arg[0] == '-' && i + 1 < argc) {
            try {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                if (key.empty()) {
                    NLM_LOG_WARNING("Config: Empty key in argument " + arg);
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
                ++parsedCount;
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Config: Error parsing argument " + arg + ": " + e.what());
            }
        } else {
            NLM_LOG_WARNING("Config: Unrecognized argument format: " + arg);
        }
    }
    
    NLM_LOG_INFO("Config: Processed " + std::to_string(parsedCount) + " configuration arguments");
    return true;
}

/**
 * @brief Save configuration to file
 * 
 * @param filepath Path to save configuration file
 * @return true if save successful, false otherwise
 * 
 * @note Currently saves all entries with placeholder values.
 *       TODO: Implement proper value serialization for all types.
 */
bool Config::saveToFile(const std::string& filepath) const {
    NLM_LOG_INFO("Config: Saving configuration to " + filepath);
    
    // Validate file path
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config: Empty filepath provided for saving");
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Config: Failed to open file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Config System\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        
        // Try to get type-appropriate value representation
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, int>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else {
                file << entry.key << " = " << "[unsupported_type]\n";
            }
        }, entry.value);
        
        file << "\n";
    }
    
    NLM_LOG_INFO("Config: Saved " + std::to_string(pImpl->entries.size()) + " entries to " + filepath);
    
    return true;
}

/**
 * @brief Get configuration value of specified type
 * 
 * @tparam T Type of value to retrieve (string, int, double, bool, etc.)
 * @param key Configuration key
 * @return Optional containing value if found, empty optional otherwise
 * 
 * @note Returns empty optional if:
 *       - Key not found in configuration
 *       - Key exists but value type doesn't match requested type
 *       - Type conversion fails
 *       
 * @warning Throws std::bad_variant_access if type mismatch occurs
 *       (caught internally and converted to empty optional).
 */
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        NLM_LOG_WARNING("Config: Empty key provided to get()");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Config: Key not found: " + key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARNING("Config: Type mismatch for key " + key + " (requested " + 
                      typeid(T).name() + ", stored " + typeid(it->value).name() + ")");
        return std::nullopt;
    }
}

/**
 * @brief Get configuration value with default fallback
 * 
 * @tparam T Type of value to retrieve
 * @param key Configuration key
 * @param defaultValue Value to return if key not found or type mismatch
 * @return Retrieved value or default value
 * 
 * @note More convenient than get() for cases where defaults are acceptable.
 *       No warnings or errors are logged for missing keys.
 */
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

/**
 * @brief Set configuration value
 * 
 * @param key Configuration key
 * @param value Configuration value of any supported type
 * @param source Source of this configuration entry
 * 
 * @note Creates new entry if key doesn't exist, updates if it does.
 *       Source tracking helps distinguish between file and command-line values.
 */
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_WARNING("Config: Empty key provided to set()");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    
    if (it != pImpl->entries.end()) {
        // Update existing entry
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Config: Updated key " + key + " from " + 
                     std::to_string(static_cast<int>(it->source)) + " to " +
                     std::to_string(static_cast<int>(source)));
    } else {
        // Create new entry
        ConfigEntry newEntry(key, value, source);
        pImpl->entries.push_back(std::move(newEntry));
        NLM_LOG_DEBUG("Config: Added new key " + key);
    }
}

/**
 * @brief Set configuration value (string overload)
 * 
 * @param key Configuration key
 * @param value String value
 * @param source Source of configuration entry
 */
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Set configuration value (int overload)
 * 
 * @param key Configuration key
 * @param value Integer value
 * @param source Source of configuration entry
 */
void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Set configuration value (double overload)
 * 
 * @param key Configuration key
 * @param value Double value
 * @param source Source of configuration entry
 */
void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Set configuration value (bool overload)
 * 
 * @param key Configuration key
 * @param value Boolean value
 * @param source Source of configuration entry
 */
void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Check if configuration contains a key
 * 
 * @param key Configuration key to check
 * @return true if key exists, false otherwise
 */
bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

/**
 * @brief Remove configuration entry by key
 * 
 * @param key Configuration key to remove
 * 
 * @note Does nothing if key doesn't exist.
 */
void Config::remove(const std::string& key) {
    if (key.empty()) {
        NLM_LOG_WARNING("Config: Empty key provided to remove()");
        return;
    }
    
    size_t beforeCount = pImpl->entries.size();
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    size_t afterCount = pImpl->entries.size();
    if (beforeCount > afterCount) {
        NLM_LOG_DEBUG("Config: Removed key " + key + " (" + 
                     std::to_string(beforeCount - afterCount) + " entries removed)");
    }
}

/**
 * @brief Get all configuration keys
 * 
 * @return Vector of all configuration keys
 * 
 * @note Keys are returned in the order they were added.
 */
std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

/**
 * @brief Clear all configuration entries
 * 
 * @note Removes all configuration entries, leaving empty config.
 *       Useful for resetting configuration to default state.
 */
void Config::clear() {
    size_t count = pImpl->entries.size();
    pImpl->entries.clear();
    if (count > 0) {
        NLM_LOG_INFO("Config: Cleared " + std::to_string(count) + " configuration entries");
    }
}

/**
 * @brief Generate human-readable configuration summary
 * 
 * @return String containing formatted configuration summary
 * 
 * @note Summary includes entry count, keys, values, and source information.
 *       Useful for debugging and configuration inspection.
 */
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "==============================================\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << entry.key << "\n";
        oss << "  Description: " << entry.description << "\n";
        oss << "  Source: " << static_cast<int>(entry.source) << "\n";
        oss << "  Value: [";
        
        // Format value based on type
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << "[unsupported_type]";
            }
        }, entry.value);
        
        oss << "]\n\n";
    }
    
    return oss.str();
}

/**
 * @brief Trim whitespace from string (both sides)
 * 
 * @param str Input string
 * @return String with leading/trailing whitespace removed
 * 
 * @note Removes spaces, tabs, carriage returns, and newlines.
 *       Preserves internal whitespace within the string.
 */
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

/**
 * @brief Convert string to lowercase
 * 
 * @param str Input string
 * @return Lowercase version of input string
 * 
 * @note Only affects ASCII letters A-Z to a-z.
 *       Non-ASCII characters are preserved unchanged.
 */
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Explicit template instantiations for supported types
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
