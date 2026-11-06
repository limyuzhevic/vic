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
    // Load configuration from file with proper error handling and validation
    NLM_LOG_INFO("Loading configuration from file: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
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
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid configuration line (missing '='): " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key format
        if (key.empty()) {
            NLM_LOG_WARNING("Empty configuration key in line: " + line);
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Validate key doesn't contain invalid characters
        if (key.find_first_of(" \\t\\r\\n") != std::string::npos) {
            NLM_LOG_WARNING("Configuration key contains whitespace: " + key);
        }
        
        set(key, value, ConfigSource::File);
    }
    
    return true;
}

// Add entry if it doesn't exist
        pImpl->entries.emplace_back(key, ConfigValue(value), source);
    }
    
    NLM_LOG_INFO("  Loaded configuration key: " + key);
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Loading configuration from command line arguments");
    
    if (argc <= 1) {
        NLM_LOG_INFO("  No command line arguments provided");
        return true;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key format
                if (key.empty()) {
                    NLM_LOG_WARNING("Empty configuration key in command line: " + arg);
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_INFO("  Loaded command line config: " + key + " = " + value);
            } else {
                NLM_LOG_WARNING("Invalid command line argument (missing '='): " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key format
            if (key.empty()) {
                NLM_LOG_WARNING("Empty configuration key in command line: -" + key);
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
            NLM_LOG_INFO("  Loaded command line config: " + key + " = " + value);
        }
        else {
            NLM_LOG_WARNING("Unrecognized command line argument: " + arg);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    NLM_LOG_INFO("Saving configuration to file: " + filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    // Write configuration entries
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Use value to_string conversion for different types
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_arithmetic_v<T>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        file << "\"" << arg[i] << "\"";
                    } else {
                        file << arg[i];
                    }
                }
                file << "]";
            } else {
                file << "UNKNOWN_TYPE";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("  Saved " + std::to_string(pImpl->entries.size()) + " configuration entries");
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key format
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    // Validate key format (no whitespace)
    if (key.find_first_of(" \\t\\r\\n") != std::string::npos) {
        throw std::invalid_argument("Configuration key cannot contain whitespace: '" + key + "'");
    }
    
    NLM_LOG_DEBUG("Setting configuration key: " + key + " (source: " + std::to_string(static_cast<int>(source)) + ")");
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        NLM_LOG_INFO("  Updating existing configuration key: " + key);
        it->value = value;
        it->source = source;
    } else {
        NLM_LOG_INFO("  Adding new configuration key: " + key);
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Set string value with validation and logging
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    // Validate key format (no whitespace)
    if (key.find_first_of(" \\t\\r\\n") != std::string::npos) {
        throw std::invalid_argument("Configuration key cannot contain whitespace: '" + key + "'");
    }
    
    NLM_LOG_DEBUG("Setting configuration string key: " + key + " = " + value);
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    // Set integer value with validation and logging
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    NLM_LOG_DEBUG("Setting configuration int key: " + key + " = " + std::to_string(value));
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    // Set int64 value with validation and logging
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    NLM_LOG_DEBUG("Setting configuration int64 key: " + key + " = " + std::to_string(value));
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Set double value with validation and logging
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    NLM_LOG_DEBUG("Setting configuration double key: " + key + " = " + std::to_string(value));
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    // Set boolean value with validation and logging
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    NLM_LOG_DEBUG("Setting configuration bool key: " + key + " = " + (value ? "true" : "false"));
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    // Check if configuration key exists with validation
    if (key.empty()) {
        NLM_LOG_ERROR("Configuration key cannot be empty when checking existence");
        return false;
    }
    
    bool exists = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (!exists) {
        NLM_LOG_DEBUG("Configuration key not found: " + key);
    }
    
    return exists;
}

void Config::remove(const std::string& key) {
    // Remove configuration key with validation
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty when removing");
    }
    
    NLM_LOG_INFO("Removing configuration key: " + key);
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    // Get all configuration keys with validation
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            NLM_LOG_WARNING("Found configuration entry with empty key, skipping");
            continue;
        }
        keys.push_back(entry.key);
    }
    
    NLM_LOG_DEBUG("Returning " + std::to_string(keys.size()) + " configuration keys");
    return keys;
}

void Config::clear() {
    // Clear all configuration entries with logging
    NLM_LOG_INFO("Clearing all configuration entries (" + std::to_string(pImpl->entries.size()) + " entries)");
    pImpl->entries.clear();
}

std::string Config::summary() const {
    // Generate configuration summary with validation
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        
        // Validate entry
        if (entry.key.empty()) {
            oss << "(INVALID_EMPTY_KEY)";
            continue;
        }
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        oss << "\"" << arg[i] << "\"";
                    } else {
                        oss << arg[i];
                    }
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
    // Trim whitespace from both ends of string with validation
    if (str.empty()) {
        NLM_LOG_DEBUG("Trimming empty string");
        return "";
    }
    
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        NLM_LOG_DEBUG("String contains only whitespace, returning empty string");
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t\r\n");
    std::string result = str.substr(start, end - start + 1);
    
    NLM_LOG_DEBUG("Trimmed string: \"" + result + "\"");
    return result;
}

std::string Config::toLower(const std::string& str) {
    // Convert string to lowercase with validation
    if (str.empty()) {
        NLM_LOG_DEBUG("Converting empty string to lowercase");
        return "";
    }
    
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    
    NLM_LOG_DEBUG("Converted to lowercase: \"" + result + "\"");
    return result;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Get configuration value with type safety and validation
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_WARNING("Configuration key not found: " + key);
        return std::nullopt;
    }
    
    NLM_LOG_DEBUG("Getting configuration value for key: " + key);
    
    // Try to get the value with type conversion
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            // Numeric conversion with bounds checking
            try {
                T result = static_cast<T>(v);
                return result;
            } catch (...) {
                NLM_LOG_ERROR("Failed to convert configuration value to type T for key: " + key);
                return std::nullopt;
            }
        } else {
            NLM_LOG_ERROR("Type mismatch for configuration key " + key + ": expected " + typeid(T).name() + " but got " + typeid(V).name());
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

} // namespace nlm
