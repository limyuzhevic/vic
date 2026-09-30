#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <cctype>
#include <cstring>
#include <cstdint>

// Try to include nlohmann/json, but provide fallback if not available
#if defined(__has_include)
#if __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
#define HAS_NLOHMANN_JSON 1
#else
#define HAS_NLOHMANN_JSON 0
#endif
#else
#define HAS_NLOHMANN_JSON 0
#endif

#if HAS_NLOHMANN_JSON
using json = nlohmann::json;
#else
// Fallback implementation for basic JSON parsing (simplified)
struct json {
    static json parse(const std::string& s) {
        // Basic JSON parsing implementation (placeholder)
        // This should be replaced with proper nlohmann/json when available
        json result;
        return result;
    }
    
    void to_file(const std::string& filepath) const {
        // Basic JSON writing implementation
    }
    
    std::string to_string() const {
        return "{}";
    }
};
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigValue> defaults;
    std::map<std::string, std::string> envVariables;
    std::vector<std::string> inheritanceChain;
    mutable bool dirty = true;
    mutable std::unordered_map<std::string, std::optional<ConfigValue>> cachedValues;
    
    // Schema validation
    struct SchemaEntry {
        std::string key;
        std::string type; // "int", "double", "string", "bool", "array"
        bool required = false;
        std::string defaultValue;
        std::string description;
    };
    std::vector<SchemaEntry> schema;
    
    // Configuration sources
    enum class SourceType {
        DEFAULT,
        ENVIRONMENT,
        FILE,
        COMMAND_LINE,
        RUNTIME
    };
    
    struct SourceInfo {
        std::string key;
        ConfigValue value;
        SourceType type;
        std::string sourceName;
        std::chrono::system_clock::time_point timestamp;
    };
    
    std::vector<SourceInfo> sourceHistory;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default sources
    pImpl->inheritanceChain = {"Default"};
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper functions for environment variable handling
static std::string getEnvVar(const std::string& name) {
    const char* value = std::getenv(name.c_str());
    return value ? value : "";
}

static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

static bool stringToBool(const std::string& str) {
    std::string lower = toLower(trim(str));
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
}

static std::string interpolateEnvVars(const std::string& input) {
    std::string result = input;
    size_t pos = 0;
    while ((pos = result.find("${", pos)) != std::string::npos) {
        size_t endPos = result.find("}", pos + 2);
        if (endPos == std::string::npos) break;
        
        std::string varName = result.substr(pos + 2, endPos - pos - 2);
        std::string varValue = getEnvVar(varName);
        
        result.replace(pos, endPos - pos + 1, varValue);
        pos += varValue.length();
    }
    return result;
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Check if file exists
        if (!std::filesystem::exists(filepath)) {
            return false;
        }
        
        // Try to parse as JSON first
        if (HAS_NLOHMANN_JSON) {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            try {
                json j;
                file >> j;
                
                // Recursively parse JSON and set values
                for (auto it = j.begin(); it != j.end(); ++it) {
                    std::string key = it.key();
                    std::string envKey = interpolateEnvVars(key);
                    
                    // Try to convert to appropriate type
                    if (it->is_number_integer()) {
                        set(envKey, it->get<int64_t>(), ConfigSource::File);
                    } else if (it->is_number_unsigned()) {
                        set(envKey, static_cast<int64_t>(it->get<uint64_t>()), ConfigSource::File);
                    } else if (it->is_number_float()) {
                        set(envKey, it->get<double>(), ConfigSource::File);
                    } else if (it->is_boolean()) {
                        set(envKey, it->get<bool>(), ConfigSource::File);
                    } else if (it->is_string()) {
                        std::string value = it->get<std::string>();
                        // Apply environment variable interpolation
                        value = interpolateEnvVars(value);
                        set(envKey, value, ConfigSource::File);
                    } else if (it->is_array()) {
                        // Handle array types (for future enhancement)
                        set(envKey, std::vector<std::string>(), ConfigSource::File);
                    }
                }
                
                return true;
            } catch (const json::parse_error& e) {
                // Fallback to key=value format
                return loadFromFileLegacy(filepath);
            }
        } else {
            // Use legacy parsing if nlohmann/json is not available
            return loadFromFileLegacy(filepath);
        }
    } catch (const std::exception& e) {
        // Log error but don't crash
        return false;
    }
}

bool Config::loadFromFileLegacy(const std::string& filepath) {
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
        
        // Parse key=value pairs
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
            
            // Apply environment variable interpolation
            key = interpolateEnvVars(key);
            value = interpolateEnvVars(value);
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
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
                
                // Apply environment variable interpolation
                key = interpolateEnvVars(key);
                value = interpolateEnvVars(value);
                
                // Try to infer type from value
                if (value.find('.') != std::string::npos) {
                    try {
                        double doubleVal = std::stod(value);
                        set(key, doubleVal, ConfigSource::CommandLine);
                    } catch (...) {
                        set(key, value, ConfigSource::CommandLine);
                    }
                } else {
                    if (stringToBool(value)) {
                        set(key, stringToBool(value), ConfigSource::CommandLine);
                    } else {
                        try {
                            int intVal = std::stoi(value);
                            set(key, intVal, ConfigSource::CommandLine);
                        } catch (...) {
                            set(key, value, ConfigSource::CommandLine);
                        }
                    }
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Apply environment variable interpolation
            key = interpolateEnvVars(key);
            value = interpolateEnvVars(value);
            
            // Try to infer type from value
            if (value.find('.') != std::string::npos) {
                try {
                    double doubleVal = std::stod(value);
                    set(key, doubleVal, ConfigSource::CommandLine);
                } catch (...) {
                    set(key, value, ConfigSource::CommandLine);
                }
            } else {
                if (stringToBool(value)) {
                    set(key, stringToBool(value), ConfigSource::CommandLine);
                } else {
                    try {
                        int intVal = std::stoi(value);
                        set(key, intVal, ConfigSource::CommandLine);
                    } catch (...) {
                        set(key, value, ConfigSource::CommandLine);
                    }
                }
            }
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write header
        file << "# NLM Configuration File\n";
        file << "# Generated by enhanced configuration system\n\n";
        
        // Write entries in JSON format for better compatibility
        if (HAS_NLOHMANN_JSON) {
            json j;
            for (const auto& entry : pImpl->entries) {
                // Convert ConfigValue to JSON
                j[entry.key] = entry.value;
            }
            
            file << j.dump(2);
        } else {
            // Fallback to key=value format
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                file << entry.key << " = ";
                
                // Write value with proper formatting
                std::visit([&file](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        file << "\"" << arg << "\"";
                    } else if constexpr (std::is_same_v<T, bool>) {
                        file << (arg ? "true" : "false");
                    } else {
                        file << arg;
                    }
                }, entry.value);
                file << "\n\n";
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log error but don't crash
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
        // Check cache first
        auto cacheIt = pImpl->cachedValues.find(key);
        if (cacheIt != pImpl->cachedValues.end()) {
            return cacheIt->second;
        }
        
        // Find the entry
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            return std::nullopt;
        }
        
        // Try to convert to requested type
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            // Try to convert from string representation
            std::string temp;
            std::visit([&temp](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    temp = arg;
                } else {
                    temp = std::to_string(arg);
                }
            }, it->value);
            
            // Attempt conversion based on requested type
            if constexpr (std::is_same_v<T, int>) {
                try {
                    return std::stoi(temp);
                } catch (...) {
                    return std::nullopt;
                }
            } else if constexpr (std::is_same_v<T, int64_t>) {
                try {
                    return std::stoll(temp);
                } catch (...) {
                    return std::nullopt;
                }
            } else if constexpr (std::is_same_v<T, double>) {
                try {
                    return std::stod(temp);
                } catch (...) {
                    return std::nullopt;
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                return stringToBool(temp);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return temp;
            } else {
                return std::nullopt;
            }
        }
    } catch (const std::exception& e) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Apply environment variable interpolation
    std::string processedKey = interpolateEnvVars(key);
    ConfigValue processedValue = value;
    
    std::visit([&processedValue](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            // Apply interpolation to string values
            auto* strPtr = std::get_if<std::string>(&processedValue);
            if (strPtr) {
                *strPtr = interpolateEnvVars(*strPtr);
            }
        }
    }, processedValue);
    
    // Find existing entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&processedKey](const ConfigEntry& e) { return e.key == processedKey; });
    
    if (it != pImpl->entries.end()) {
        it->value = processedValue;
        it->source = source;
        pImpl->dirty = true;
        pImpl->cachedValues.erase(processedKey);
    } else {
        pImpl->entries.emplace_back(processedKey, processedValue, source);
        pImpl->dirty = true;
        pImpl->cachedValues.erase(processedKey);
    }
    
    // Record source history
    pImpl->sourceHistory.push_back({
        processedKey,
        processedValue,
        static_cast<Impl::SourceType>(source),
        "Config::set",
        std::chrono::system_clock::now()
    });
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
    std::string processedKey = interpolateEnvVars(key);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&processedKey](const ConfigEntry& e) { return e.key == processedKey; });
}

void Config::remove(const std::string& key) {
    std::string processedKey = interpolateEnvVars(key);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&processedKey](const ConfigEntry& e) { return e.key == processedKey; }),
        pImpl->entries.end()
    );
    pImpl->dirty = true;
    pImpl->cachedValues.erase(processedKey);
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
    pImpl->dirty = true;
    pImpl->cachedValues.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "Sources: ";
    for (const auto& source : pImpl->inheritanceChain) {
        oss << source << " -> ";
    }
    oss << "End\n\n";
    
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
    
    if (pImpl->dirty) {
        oss << "Status: DIRTY (unsaved changes)\n";
    } else {
        oss << "Status: CLEAN\n";
    }
    
    return oss.str();
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

// Configuration inheritance and merging
void Config::addInheritanceSource(const std::string& sourceName) {
    if (std::find(pImpl->inheritanceChain.begin(), pImpl->inheritanceChain.end(), sourceName) == pImpl->inheritanceChain.end()) {
        pImpl->inheritanceChain.push_back(sourceName);
        pImpl->dirty = true;
    }
}

void Config::removeInheritanceSource(const std::string& sourceName) {
    auto it = std::find(pImpl->inheritanceChain.begin(), pImpl->inheritanceChain.end(), sourceName);
    if (it != pImpl->inheritanceChain.end()) {
        pImpl->inheritanceChain.erase(it);
        pImpl->dirty = true;
    }
}

void Config::clearInheritanceChain() {
    pImpl->inheritanceChain.clear();
    pImpl->dirty = true;
}

bool Config::mergeFrom(const Config& other) {
    bool success = true;
    for (const auto& entry : other.pImpl->entries) {
        try {
            set(entry.key, entry.value, entry.source);
        } catch (...) {
            success = false;
        }
    }
    if (success) {
        pImpl->dirty = true;
    }
    return success;
}

bool Config::mergeFromFile(const std::string& filepath) {
    Config tempConfig;
    bool success = tempConfig.loadFromFile(filepath);
    if (success) {
        mergeFrom(tempConfig);
    }
    return success;
}

// Schema validation and type checking
bool Config::validateSchema() const {
    // Basic schema validation implementation
    // This can be expanded with more sophisticated validation rules
    for (const auto& entry : pImpl->entries) {
        // Validate that string values can be parsed as their expected types
        if (std::get_if<std::string>(&entry.value) != nullptr) {
            std::string strValue = std::get<std::string>(entry.value);
            // Try to parse as boolean
            if (stringToBool(strValue)) {
                continue; // Valid
            }
            // Try to parse as integer
            try {
                size_t pos;
                int64_t val = std::stoll(strValue, &pos);
                if (pos == strValue.length()) {
                    continue; // Valid integer
                }
            } catch (...) {}
            
            // Try to parse as double
            try {
                size_t pos;
                double val = std::stod(strValue, &pos);
                if (pos == strValue.length()) {
                    continue; // Valid double
                }
            } catch (...) {}
        }
    }
    return true;
}

std::string Config::getValidationError(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "Key not found: " + key;
    }
    
    std::string valueStr;
    std::visit([&valueStr](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            valueStr = arg;
        } else {
            valueStr = std::to_string(arg);
        }
    }, it->value);
    
    // Validate that the value can be parsed as appropriate types
    if (stringToBool(valueStr)) {
        return ""; // Valid boolean
    }
    
    try {
        size_t pos;
        int64_t val = std::stoll(valueStr, &pos);
        if (pos == valueStr.length()) {
            return ""; // Valid integer
        }
    } catch (...) {}
    
    try {
        size_t pos;
        double val = std::stod(valueStr, &pos);
        if (pos == valueStr.length()) {
            return ""; // Valid double
        }
    } catch (...) {}
    
    return "Value cannot be parsed as valid type: " + valueStr;
}

// Convenience methods and utility functions
std::string Config::getString(const std::string& key) const {
    auto val = get<std::string>(key);
    return val.has_value() ? val.value() : "";
}

int Config::getInt(const std::string& key) const {
    auto val = get<int>(key);
    return val.has_value() ? val.value() : 0;
}

int64_t Config::getInt64(const std::string& key) const {
    auto val = get<int64_t>(key);
    return val.has_value() ? val.value() : 0;
}

double Config::getDouble(const std::string& key) const {
    auto val = get<double>(key);
    return val.has_value() ? val.value() : 0.0;
}

bool Config::getBool(const std::string& key) const {
    auto val = get<bool>(key);
    return val.has_value() ? val.value() : false;
}

// Set convenience methods
void Config::setString(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, value, source);
}

void Config::setInt(const std::string& key, int value, ConfigSource source) {
    set(key, value, source);
}

void Config::setInt64(const std::string& key, int64_t value, ConfigSource source) {
    set(key, value, source);
}

double Config::setDouble(const std::string& key, double value, ConfigSource source) {
    set(key, value, source);
    return value;
}

bool Config::setBool(const std::string& key, bool value, ConfigSource source) {
    set(key, value, source);
    return value;
}

// Environment variable utilities
bool Config::hasEnvVar(const std::string& name) const {
    return getEnvVar(name) != "";
}

std::string Config::getEnvVar(const std::string& name) const {
    return nlm::getEnvVar(name);
}

// Utility for checking configuration health
bool Config::isHealthy() const {
    return validateSchema() && !pImpl->dirty;
}

std::string Config::getHealthReport() const {
    std::ostringstream oss;
    oss << "Configuration Health Report:\n";
    oss << "Status: " << (isHealthy() ? "HEALTHY" : "UNHEALTHY") << "\n";
    oss << "Entries: " << pImpl->entries.size() << "\n";
    oss << "Sources: " << pImpl->inheritanceChain.size() << "\n";
    oss << "Cache size: " << pImpl->cachedValues.size() << "\n";
    
    if (!isHealthy()) {
        oss << "\nIssues found:\n";
        for (const auto& entry : pImpl->entries) {
            std::string error = getValidationError(entry.key);
            if (!error.empty()) {
                oss << "  - " << entry.key << ": " << error << "\n";
            }
        }
    }
    
    return oss.str();
}

// Statistics and monitoring
struct ConfigStats {
    size_t totalEntries;
    size_t entriesBySource[4]; // Default, File, CommandLine, Runtime
    double healthScore;
    std::chrono::system_clock::time_point lastModified;
};

ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    stats.lastModified = std::chrono::system_clock::now();
    
    // Initialize source counts
    for (int i = 0; i < 4; ++i) {
        stats.entriesBySource[i] = 0;
    }
    
    // Count entries by source
    for (const auto& entry : pImpl->entries) {
        int sourceIndex = static_cast<int>(entry.source);
        if (sourceIndex >= 0 && sourceIndex < 4) {
            stats.entriesBySource[sourceIndex]++;
        }
    }
    
    // Calculate health score
    stats.healthScore = validateSchema() ? 1.0 : 0.0;
    
    return stats;
}

// Batch operations
void Config::setMultiple(const std::map<std::string, ConfigValue>& values, ConfigSource source) {
    for (const auto& pair : values) {
        set(pair.first, pair.second, source);
    }
}

void Config::removeMultiple(const std::vector<std::string>& keys) {
    for (const auto& key : keys) {
        remove(key);
    }
}

// Serialization and deserialization
std::string Config::toJSON() const {
    if (HAS_NLOHMANN_JSON) {
        json j;
        for (const auto& entry : pImpl->entries) {
            // Convert ConfigValue to JSON
            j[entry.key] = entry.value;
        }
        return j.dump();
    }
    return "{}";
}

bool Config::fromJSON(const std::string& jsonStr) {
    if (!HAS_NLOHMANN_JSON) {
        return false;
    }
    
    try {
        json j = json::parse(jsonStr);
        // Clear existing entries
        clear();
        
        // Parse JSON and set values
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            if (it->is_number_integer()) {
                set(key, it->get<int64_t>(), ConfigSource::File);
            } else if (it->is_number_unsigned()) {
                set(key, static_cast<int64_t>(it->get<uint64_t>()), ConfigSource::File);
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), ConfigSource::File);
            } else if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::File);
            } else if (it->is_string()) {
                std::string value = it->get<std::string>();
                value = interpolateEnvVars(value);
                set(key, value, ConfigSource::File);
            } else if (it->is_array()) {
                // Handle array types
                set(key, std::vector<std::string>(), ConfigSource::File);
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

// Comparison operators
bool Config::operator==(const Config& other) const {
    return pImpl->entries == other.pImpl->entries;
}

bool Config::operator!=(const Config& other) const {
    return !(*this == other);
}

// Hash for use in unordered containers
size_t Config::hash() const {
    std::hash<std::string> stringHash;
    size_t seed = 0;
    for (const auto& entry : pImpl->entries) {
        seed ^= stringHash(entry.key) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

// Explicit template instantiations for additional types
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

} // namespace nlm
