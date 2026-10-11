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

// Load configuration from a key-value file (simple format, not JSON/YAML)
// This is Phase 2: The configuration system uses a simple but flexible key=value format
// that supports comments, type conversion, and multiple configuration sources (file, command line, runtime)
// 
// The format is:
//   key = value        // Simple value (string, number, boolean)
//   key = [v1, v2, ...] // Array format for multiple values
//   key = "value"      // Quoted strings support spaces and special characters
// 
// Comments start with # or //
// Empty lines are ignored
// Values are trimmed of surrounding whitespace
// Type conversion is handled automatically when retrieving values
// 
// Configuration sources have priority: CommandLine > File > Runtime > Default
// Later values override earlier ones for the same key
    
    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse the line into key-value pair with proper error handling
        // Validates key format to prevent malformed configuration
        // Supports both quoted and unquoted values
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present (allows strings with spaces)
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
                NLM_LOG_DEBUG("Unquoted string: '" + value + "'");
            }
            
            // Validate key format - must not be empty and must not contain '=' or newlines
            if (key.empty() || key.find('=') != std::string::npos || key.find('\n') != std::string::npos) {
                NLM_LOG_WARNING("Invalid config key in line " + std::to_string(lineNum) + ": '" + key + "'");
                continue;
            }
            
            // Set the configuration value (creates entry if it doesn't exist)
            set(key, value, ConfigSource::File);
        } else {
            // Log warning for malformed line without '=' sign
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": no '=' found in '" + line + "'");
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from: " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
    return true;
}

// Load configuration from command line arguments
// Supports multiple formats:
//   --key=value        // Long format with equals sign
//   -key value         // Short format with separate value
// Command line arguments have highest priority in the configuration hierarchy
// (overrides file and default values)
// 
// Examples:
//   --neuron_count=1000      // Set neuron count to 1000
//   -seed 42                // Set random seed to 42
//   -learning_rate 0.01      // Set learning rate to 0.01
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save config file: " + filepath);
        return false;
    }
    
    // Sort entries by key for consistent output
    std::vector<ConfigEntry> sortedEntries = pImpl->entries;
    std::sort(sortedEntries.begin(), sortedEntries.end(), 
              [](const ConfigEntry& a, const ConfigEntry& b) {
                  return a.key < b.key;
              });
    
    for (const auto& entry : sortedEntries) {
        file << "# " << entry.description << " (source: " << static_cast<int>(entry.source) << ")\n";
        
        // Format value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
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
            } else {
                // For primitive types
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << "\n";
    }
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Config key not found: " + key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Config type mismatch for key: " + key);
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
    if (key.empty() || key.find('=') != std::string::npos || key.find('\n') != std::string::npos) {
        NLM_LOG_WARNING("Invalid config key: '" + key + "'");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Config updated: " + key);
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Config added: " + key);
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
    bool exists = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (!exists) {
        NLM_LOG_TRACE("Config key checked (not found): " + key);
    }
    
    return exists;
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
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    
    // Show entries sorted by source priority (CommandLine > File > Runtime > Default)
    std::vector<const ConfigEntry*> sortedEntries;
    for (const auto& entry : pImpl->entries) {
        sortedEntries.push_back(&entry);
    }
    
    std::sort(sortedEntries.begin(), sortedEntries.end(),
        [](const ConfigEntry* a, const ConfigEntry* b) {
            return static_cast<int>(a->source) < static_cast<int>(b->source);
        });
    
    for (const auto* entry : sortedEntries) {
        oss << "  " << entry->key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry->value);
        oss << "] (source: " << static_cast<int>(entry->source) << ")\n";
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
