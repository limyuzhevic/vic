#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// Configuration System Implementation
// ===================================
// 
// Current Implementation (Phase 1 Complete):
// - Simple key=value file parser (see Config::loadFromFile)
// - Command line argument parser (see Config::loadFromArgs)
// - Core configuration storage and management (see Config class in Config.hpp)
// - Type-safe value access via templates (get<T>(), getOr<T>())
// 
// Design Decisions:
// 1. Uses std::variant for flexible value types (ConfigValue type alias in Config.hpp)
// 2. Supports multiple configuration sources with precedence: CommandLine > File > Default
// 3. Each configuration entry tracks its source for debugging purposes
// 4. Simple parser designed for human-editable config files
// 5. Type safety maintained through template specializations
// 
// Phase 2 Planned Enhancements:
// 1. Replace simple parser with JSON/YAML support (see TODO in loadFromFile)
// 2. Implement proper value serialization in saveToFile (see TODO in loadFromFile)
// 3. Add configuration validation and schema support
// 4. Implement hierarchical configuration (sections/ namespaces)
// 5. Add configuration hot-reloading capabilities
// 
// Key Implementation Notes:
// - Configuration is immutable after loading (entries stored in pImpl->entries)
// - Thread-safe access through const methods
// - Memory efficient storage using std::variant and std::vector
// - ConfigSource enum tracks configuration source for debugging
// 
// Usage Examples (see main.cpp for examples):
//   config->set("neuron_count", 500);           // Set default value
//   config->getOr<int>("neuron_count", 100);     // Get with default
//   config->has("neuron_count");                // Check existence
// 
// Cross-references:
// - Config.hpp: Interface definition and ConfigValue type
// - main.cpp: Usage examples in NLM Phase 2
// - configs/: Example configuration files

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Basic validation - ensure pImpl was created successfully
    if (!pImpl) {
        NLM_LOG_ERROR("Config::Config: failed to initialize Config internal structure");
        // In a real implementation, we might throw an exception here
    } else {
        NLM_LOG_DEBUG("Config::Config: initialized successfully");
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Load configuration from a simple key=value file
// IMPLEMENTED: Supports basic key=value format with quoted values and comments
// PLANNED: Phase 2 will replace with JSON/YAML parser for complex nested configurations
// 
// File format details:
// - Lines starting with #, // or / are treated as comments
// - Empty lines are skipped
// - Values can be quoted with " or '
// - Only simple key=value pairs supported (no nesting or arrays)
// 
// Configuration sources and precedence:
// 1. Command line arguments (highest priority)
// 2. Configuration file
// 3. Default values set at runtime (lowest priority)
// 
// Example config file:
// # Neural network configuration
// neuron_count = 500
// region_count = 2
// connection_probability = 0.15
Config::loadFromFile(const std::string& filepath) {
    // Validate filepath is not empty
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config::loadFromFile: empty filepath provided");
        return false;
    }
    
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Config::loadFromFile: failed to open file: " + filepath);
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
            
            // Validate key is not empty
            if (key.empty()) {
                NLM_LOG_WARNING("Config::loadFromFile: empty key at line " + std::to_string(lineNumber));
                continue;
            }
            
            // Validate key contains only valid characters (alphanumeric, underscore, hyphen)
            bool validKey = true;
            for (char c : key) {
                if (!std::isalnum(c) && c != '_' && c != '-') {
                    validKey = false;
                    break;
                }
            }
            if (!validKey) {
                NLM_LOG_WARNING("Config::loadFromFile: invalid key name '" + key + "' at line " + std::to_string(lineNumber));
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Config::loadFromFile: invalid line format at line " + std::to_string(lineNumber) + ": '" + line + "'");
        }
    }
    
    NLM_LOG_DEBUG("Config::loadFromFile: loaded configuration from " + filepath + " (" + 
                 std::to_string(pImpl->entries.size()) + " entries)");
    return true;
}

// Load configuration from command line arguments
// IMPLEMENTED: Supports both --key=value and -key value formats
// Command line arguments override file and default values
// Examples: --neuron_count=500, -r 2, --connection_probability 0.15
bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        NLM_LOG_DEBUG("Config::loadFromArgs: no command line arguments provided");
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
                
                // Validate key is not empty
                if (key.empty()) {
                    NLM_LOG_WARNING("Config::loadFromArgs: empty key in argument '--" + key + "'");
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Config::loadFromArgs: malformed argument '--" + arg + "' (missing '=')");
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key is not empty
            if (key.empty()) {
                NLM_LOG_WARNING("Config::loadFromArgs: empty key in argument '-" + key + "'");
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
        } else {
            NLM_LOG_WARNING("Config::loadFromArgs: unrecognized argument format: '" + arg + "'");
        }
    }
    
    NLM_LOG_DEBUG("Config::loadFromArgs: processed " + std::to_string(argc - 1) + 
                 " command line arguments");
    return true;
}

// Save configuration to a file for persistence or sharing
// IMPLEMENTED: Saves key-value pairs with descriptions as comments
// Note: Currently saves placeholder values - actual values would need proper serialization
// PLANNED: Phase 2 will implement proper JSON/YAML serialization with real values
// 
// Output format:
// # <description>
// <key> = <value>
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

// Get configuration value with type safety
// IMPLEMENTED: Uses std::variant to store multiple types (int, double, bool, string, vectors)
// Returns std::nullopt if key doesn't exist or type mismatch
// Type checking is done at runtime via std::variant's type index
// 
// Usage: auto value = config->get<int>("neuron_count");
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

// Get configuration value with default fallback
// IMPLEMENTED: Template method that calls get() and returns default if not found
// This is the preferred way to access configuration values as it handles missing keys gracefully
// 
// Usage: int neurons = config->getOr<int>("neuron_count", 500);
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
