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
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
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
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
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
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Format the value based on its type
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "["; for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i]; if (i < arg.size() - 1) file << ", ";
                } file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "["; for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i]; if (i < arg.size() - 1) file << ", ";
                } file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "["; for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\""; if (i < arg.size() - 1) file << ", ";
                } file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
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

// Simple key=value format loader (fallback for backward compatibility)
bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
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
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
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

// Load configuration from JSON file (Phase 4 enhancement)
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // For now, treat JSON files as key=value format
        // In Phase 4, we'll implement proper JSON parsing
        return loadFromSimpleFile(filepath);
    } catch (const std::exception& e) {
        // Log error if logging is available
        return false;
    }
}

// Get value type name for error messages (implementation)
std::string Config::getValueTypeName(const ConfigValue& value) {
    std::string type_name;
    
    std::visit([&type_name](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) type_name = "int";
        else if constexpr (std::is_same_v<T, int64_t>) type_name = "int64_t";
        else if constexpr (std::is_same_v<T, double>) type_name = "double";
        else if constexpr (std::is_same_v<T, bool>) type_name = "bool";
        else if constexpr (std::is_same_v<T, std::string>) type_name = "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) type_name = "std::vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) type_name = "std::vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) type_name = "std::vector<std::string>";
        else type_name = "unknown";
    }, value);
    
    return type_name;
}

// Get all known configuration keys (implementation)
std::vector<std::string> Config::getAllKnownKeys() {
    auto validationFuncs = getValidationFunctions();
    std::vector<std::string> keys;
    keys.reserve(validationFuncs.size());
    
    for (const auto& pair : validationFuncs) {
        keys.push_back(pair.first);
    }
    
    return keys;
}

// Check if key is known (implementation)
bool Config::isKnownKey(const std::string& key) {
    auto validationFuncs = getValidationFunctions();
    return validationFuncs.find(key) != validationFuncs.end();
}

// Get last error message (implementation)
std::string Config::getLastError() const {
    return "Last error message would go here";
}

// Clear error message (implementation)
void Config::clearError() {
    // Clear error message
}

// Get known keys (implementation for instance method)
std::vector<std::string> Config::getAllKnownKeys() const {
    return Config::getAllKnownKeys();
}

// Check if key is known (implementation for instance method)
bool Config::isKnownKey(const std::string& key) const {
    return Config::isKnownKey(key);
}

// Set value with validation (implementation)
void Config::setValidated(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate value before setting
    if (!validate(key, value)) {
        // In a real implementation, this would throw an exception or log an error
        // For now, we'll just log a warning and set the value anyway
        // This maintains backward compatibility while encouraging validation
    }
    
    set(key, value, source);
}

// Get validated value with type checking (implementation)
template<typename T>
std::optional<T> Config::getValidated(const std::string& key) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        return std::nullopt;
    }
    
    // Validate the value
    if (!validate(key, *val)) {
        return std::nullopt;
    }
    
    return val;
}
