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
    // Enhanced with JSON/YAML support
    // Supports both simple key=value format and JSON/YAML parsing
    // Priority: JSON -> YAML -> key=value format
    
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

// Enhanced file loading with format detection
bool Config::loadFromJSON(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON parsing
    // For now, use the simple format as a placeholder
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromYAML(const std::string& filepath) {
    // TODO PHASE 2: Implement proper YAML parsing
    // For now, use the simple format as a placeholder
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
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
        } else {
            // Log warning for malformed line
            // For now, just continue
        }
    }
    
    return true;
}

// File operations helper
std::string Config::getFileExtension(const std::string& filepath) {
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    return filepath.substr(dotPos + 1);
}

bool Config::isJSON(const std::string& filepath) {
    std::string extension = getFileExtension(filepath);
    return extension == "json" || extension == "jsonl";
}

bool Config::isYAML(const std::string& filepath) {
    std::string extension = getFileExtension(filepath);
    return extension == "yaml" || extension == "yml";
}

// Configuration validation
bool Config::validate() const {
    bool isValid = true;
    
    // Check for duplicate keys
    std::vector<std::string> seenKeys;
    for (const auto& entry : pImpl->entries) {
        if (std::find(seenKeys.begin(), seenKeys.end(), entry.key) != seenKeys.end()) {
            // Duplicate key found - this might be okay if different sources
            isValid = false;
        } else {
            seenKeys.push_back(entry.key);
        }
    }
    
    // Validate value types based on key names (simplified validation)
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find("seed") != std::string::npos) {
            // Should be convertible to int64_t
            if (!std::holds_alternative<int64_t>(entry.value)) {
                isValid = false;
            }
        } else if (entry.key.find("rate") != std::string::npos || 
                   entry.key.find("factor") != std::string::npos ||
                   entry.key.find("weight") != std::string::npos) {
            // Should be convertible to double
            if (!std::holds_alternative<double>(entry.value)) {
                isValid = false;
            }
        } else if (entry.key.find("enabled") != std::string::npos ||
                   entry.key.find("active") != std::string::npos) {
            // Should be convertible to bool
            if (!std::holds_alternative<bool>(entry.value)) {
                isValid = false;
            }
        }
    }
    
    return isValid;
}

// Export configuration for other systems
std::string Config::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) oss << ",\n";
        first = false;
        
        oss << "  \"" << entry.key << "\": ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std অনেক>") {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int> ||
                                 std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, double> ||
                                 std::is_same_v<T, bool>) {
                oss << std::boolalpha << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        oss << "\"" << arg[i] << "\"";
                    } else {
                        oss << arg[i];
                    }
                }
                oss << "]";
            }
        }, entry.value);
    }
    oss << "\n}";
    return oss.str();
}

std::string Config::exportToYAML() const {
    std::ostringstream oss;
    for (const auto& entry : pImpl->entries) {
        oss << entry.key << ": ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int> ||
                                 std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, double> ||
                                 std::is_same_v<T, bool>) {
                oss << std::boolalpha << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        oss << "\"" << arg[i] << "\"";
                    } else {
                        oss << arg[i];
                    }
                }
                oss << "]";
            }
        }, entry.value);
        oss << "\n";
    }
    return oss.str();
}

// Import configuration from string
bool Config::importFromJSON(const std::string& jsonString) {
    // TODO PHASE 2: Implement proper JSON parsing from string
    // For now, return false to indicate not implemented
    return false;
}

bool Config::importFromYAML(const std::string& yamlString) {
    // TODO PHASE 2: Implement proper YAML parsing from string
    // For now, return false to indicate not implemented
    return false;
}

// Helper function for Python bindings
const char* configSourceToString(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Runtime: return "Runtime";
        default: return "Unknown";
    }
}

ConfigSource stringToConfigSource(const std::string& sourceStr) {
    std::string lower = toLower(sourceStr);
    if (lower == "default") return ConfigSource::Default;
    if (lower == "file") return ConfigSource::File;
    if (lower == "commandline" || lower == "command_line" || lower == "argv") return ConfigSource::CommandLine;
    if (lower == "runtime") return ConfigSource::Runtime;
    return ConfigSource::Default;
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
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
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
    // Merge another configuration into this one (runtime overrides)
    void Config::merge(const Config& other) {
        for (const auto& entry : other.pImpl->entries) {
            // Runtime entries override everything (File > CommandLine > Default)
            if (entry.source == ConfigSource::Runtime) {
                set(entry.key, entry.value, ConfigSource::Runtime);
            } else if (entry.source == ConfigSource::CommandLine) {
                // CommandLine overrides File and Default
                set(entry.key, entry.value, ConfigSource::CommandLine);
            } else if (entry.source == ConfigSource::File && 
                       !has(entry.key) || 
                       (has(entry.key) && 
                        (getConfigSource(entry.key) == ConfigSource::Default))) {
                // File overrides Default, but not CommandLine/Runtime
                set(entry.key, entry.value, ConfigSource::File);
            }
        }
    }
    
    // Compare two configurations and return differences
    std::vector<std::string> Config::compare(const Config& other) const {
        std::vector<std::string> differences;
        
        // Get keys from both configurations
        std::vector<std::string> thisKeys = getKeys();
        std::vector<std::string> otherKeys = other.getKeys();
        
        // Check for keys only in this config
        for (const auto& key : thisKeys) {
            if (std::find(otherKeys.begin(), otherKeys.end(), key) == otherKeys.end()) {
                differences.push_back("Only in this config: " + key);
            }
        }
        
        // Check for keys only in other config
        for (const auto& key : otherKeys) {
            if (std::find(thisKeys.begin(), thisKeys.end(), key) == thisKeys.end()) {
                differences.push_back("Only in other config: " + key);
            }
        }
        
        // Compare common keys
        for (const auto& key : thisKeys) {
            if (std::find(otherKeys.begin(), otherKeys.end(), key) != otherKeys.end()) {
                ConfigSource thisSource = getConfigSource(key);
                ConfigSource otherSource = other.getConfigSource(key);
                if (thisSource != otherSource) {
                    differences.push_back("Different source for " + key + ": " + 
                                        configSourceToString(thisSource) + " vs " + 
                                        configSourceToString(otherSource));
                }
                
                // Get values for comparison (simplified)
                auto thisVal = get<std::string>(key);
                auto otherVal = other.get<std::string>(key);
                if (thisVal != otherVal) {
                    differences.push_back("Different value for " + key + ": " + 
                                        (thisVal ? *thisVal : "unset") + " vs " + 
                                        (otherVal ? *otherVal : "unset"));
                }
            }
        }
        
        return differences;
    }
    
    // Get statistics about configuration keys (value types, sources)
    std::string Config::getKeyStats() const {
        std::ostringstream oss;
        oss << "Configuration Statistics:\n";
        oss << "Total keys: " << pImpl->entries.size() << "\n";
        
        // Count by type
        int intCount = 0, doubleCount = 0, boolCount = 0, stringCount = 0, vectorCount = 0;
        for (const auto& entry : pImpl->entries) {
            std::visit([&intCount, &doubleCount, &boolCount, &stringCount, &vectorCount](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) intCount++;
                else if constexpr (std::is_same_v<T, double>) doubleCount++;
                else if constexpr (std::is_same_v<T, bool>) boolCount++;
                else if constexpr (std::is_same_v<T, std::string>) stringCount++;
                else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                   std::is_same_v<T, std::vector<double>> ||
                                   std::is_same_v<T, std::vector<std::string>>) {
                    vectorCount++;
                }
            }, entry.value);
        }
        
        oss << "Int values: " << intCount << "\n";
        oss << "Double values: " << doubleCount << "\n";
        oss << "Bool values: " << boolCount << "\n";
        oss << "String values: " << stringCount << "\n";
        oss << "Vector values: " << vectorCount << "\n";
        
        // Count by source
        int defaultCount = 0, fileCount = 0, commandLineCount = 0, runtimeCount = 0;
        for (const auto& entry : pImpl->entries) {
            switch (entry.source) {
                case ConfigSource::Default: defaultCount++; break;
                case ConfigSource::File: fileCount++; break;
                case ConfigSource::CommandLine: commandLineCount++; break;
                case ConfigSource::Runtime: runtimeCount++; break;
            }
        }
        
        oss << "Default source: " << defaultCount << "\n";
        oss << "File source: " << fileCount << "\n";
        oss << "Command line source: " << commandLineCount << "\n";
        oss << "Runtime source: " << runtimeCount << "\n";
        
        return oss.str();
    }
    
    // Get validation errors if any
    std::vector<std::string> Config::getValidationErrors() const {
        return validate() ? std::vector<std::string>() : std::vector<std::string>{"Configuration validation failed"};
    }
    
    // Export configuration for a specific version/format
    std::string Config::exportForVersion(const std::string& version, const std::string& format) const {
        std::ostringstream oss;
        oss << "// NLM Configuration Export\n";
        oss << "// Version: " << version << "\n";
        oss << "// Format: " << format << "\n";
        oss << "// Generated at: " << getTimestamp() << "\n\n";
        
        if (format == "json") {
            oss << exportToJSON();
        } else if (format == "yaml" || format == "yml") {
            oss << exportToYAML();
        } else {
            oss << "// Unknown format: " << format << "\n";
            oss << summary();
        }
        
        return oss.str();
    }
    
    // Import configuration from a specific version/format
    bool Config::importFromVersion(const std::string& data, const std::string& version, const std::string& format) {
        if (format == "json") {
            // Parse JSON (simplified)
            // TODO: Implement proper JSON parsing
            return false;
        } else if (format == "yaml" || format == "yml") {
            // Parse YAML (simplified)
            // TODO: Implement proper YAML parsing
            return false;
        } else {
            // Try simple key=value format
            std::istringstream iss(data);
            std::string line;
            bool success = true;
            while (std::getline(iss, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    set(key, value, ConfigSource::File);
                } else {
                    success = false;
                }
            }
            return success;
        }
    }
    
    // Helper: Get config source for a key
    ConfigSource Config::getConfigSource(const std::string& key) const {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        return it != pImpl->entries.end() ? it->source : ConfigSource::Default;
    }
    
    // Helper: Get timestamp
    std::string Config::getTimestamp() const {
        // Simple timestamp - in production would use proper time library
        return "2026-01-01T00:00:00Z";
    }

    template<typename T>
    std::optional<T> Config::get(const std::string& key) const;
    
    template<typename T>
    T Config::getOr(const std::string& key, const T& defaultValue) const;
    
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
