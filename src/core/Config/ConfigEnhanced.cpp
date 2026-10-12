// NLM (熙然) - Neural Learning Machine
// Phase 8: Enhanced Configuration System Implementation
//
// This file implements the enhanced configuration system with:
// - Enhanced error handling and validation
// - Configuration schema support
// - Improved API consistency
// - Configuration merging and diffing capabilities
// - Export/import capabilities (simplified)

#include "ConfigEnhanced.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <regex>
#include <ctime>
#include <unordered_map>
#include <limits>

namespace nlm {

// Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigSource> keySources;
    std::unordered_map<std::string, bool> keyModified;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema_(getDefaultSchema()) {
    // Initialize with default values from schema
    for (const auto& [key, fieldInfo] : schema_.schema) {
        if (!fieldInfo.defaultValue.empty()) {
            set(key, fieldInfo.defaultValue, ConfigSource::Default, fieldInfo.description);
        }
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool validate) {
    std::string extension = std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    // Try JSON format first
    if (extension == ".json") {
        return loadFromJson(filepath, validate);
    }
    // Try YAML format
    else if (extension == ".yaml" || extension == ".yml") {
        return loadFromYaml(filepath, validate);
    } else {
        // Try simple key=value format (backward compatibility)
        return loadFromSimpleFile(filepath, validate);
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath, bool validate) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigFileException("Cannot open file: " + filepath);
        }
        
        return parseSimpleFormat(file, ConfigSource::File, "Loaded from file: " + filepath);
    } catch (const std::exception& e) {
        std::cerr << "Warning: Failed to load config file " << filepath 
                  << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromYaml(const std::string& filepath, bool validate) {
    try {
        // Simplified YAML parsing - just try to read and parse basic key: value pairs
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigFileException("Cannot open YAML file: " + filepath);
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), 
                           std::istreambuf_iterator<char>());
        
        return parseSimpleYaml(content, ConfigSource::File, "Loaded from YAML file: " + filepath);
    } catch (const std::exception& e) {
        std::cerr << "Warning: Failed to parse YAML file " << filepath 
                  << ": " << e.what() << ". Falling back to simple format." << std::endl;
        return loadFromSimpleFile(filepath, validate);
    }
}

bool Config::loadFromJson(const std::string& filepath, bool validate) {
    try {
        // Simplified JSON parsing - just try to read and parse basic key-value pairs
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigFileException("Cannot open JSON file: " + filepath);
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), 
                           std::istreambuf_iterator<char>());
        
        return parseSimpleJson(content, ConfigSource::File, "Loaded from JSON file: " + filepath);
    } catch (const std::exception& e) {
        std::cerr << "Warning: Failed to parse JSON file " << filepath 
                  << ": " << e.what() << ". Falling back to simple format." << std::endl;
        return loadFromSimpleFile(filepath, validate);
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        bool hasArgs = false;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine, "Command line argument");
                    hasArgs = true;
                } else if (arg == "--help" || arg == "-h") {
                    std::cout << "NLM Configuration Options:" << std::endl;
                    std::cout << "  --key=value    Set configuration key to value" << std::endl;
                    std::cout << "  -key value     Set configuration key to value" << std::endl;
                    std::cout << "  --help, -h     Show this help message" << std::endl;
                    std::cout << "  --config=file  Load configuration from file" << std::endl;
                    hasArgs = true;
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine, "Command line argument");
                hasArgs = true;
            }
        }
        
        return hasArgs;
    } catch (const std::exception& e) {
        std::cerr << "Error loading command line arguments: " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromEnv(const std::unordered_map<std::string, std::string>& envVars) {
    try {
        // Get environment variables
        std::unordered_map<std::string, std::string> env;
        if (envVars.empty()) {
            const char* path = std::getenv("PATH");
            if (path) env["PATH"] = path;
            
            const char* home = std::getenv("HOME");
            if (home) env["HOME"] = home;
            
            const char* user = std::getenv("USER");
            if (user) env["USER"] = user;
            
            const char* shell = std::getenv("SHELL");
            if (shell) env["SHELL"] = shell;
            
            const char* pwd = std::getenv("PWD");
            if (pwd) env["PWD"] = pwd;
        } else {
            env = envVars;
        }
        
        bool hasEnvVars = false;
        for (const auto& pair : env) {
            std::string key = "env_" + pair.first;
            set(key, pair.second, ConfigSource::Environment, "Environment variable: " + pair.first);
            hasEnvVars = true;
        }
        
        return hasEnvVars;
    } catch (const std::exception& e) {
        std::cerr << "Error loading environment variables: " << e.what() << std::endl;
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::string extension = std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    // Try JSON format
    if (extension == ".json") {
        return saveToJson(filepath);
    }
    // Try YAML format
    else if (extension == ".yaml" || extension == ".yml") {
        return saveToYaml(filepath);
    } else {
        // Default to simple format
        return saveToSimpleFile(filepath);
    }
}

bool Config::saveToSimpleFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigFileException("Cannot open file for writing: " + filepath);
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << valueToString(entry.value) << "\n\n";
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving to file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::saveToYaml(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigFileException("Cannot open YAML file for writing: " + filepath);
        }
        
        file << "# NLM Configuration (YAML format)\n";
        file << "# Generated by NLM Configuration System\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << entry.key << ": " << yamlValueToString(entry.value) << "\n";
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving YAML to file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::saveToJson(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigFileException("Cannot open JSON file for writing: " + filepath);
        }
        
        file << "// NLM Configuration (JSON format)\n";
        file << "// Generated by NLM Configuration System\n\n";
        
        file << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": " << jsonValueToString(entry.value);
            if (i < pImpl->entries.size() - 1) {
                file << ",";
            }
            file << "\n";
        }
        file << "}\n";
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving JSON to file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entryIt == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(entryIt->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source, const std::string& description) {
    // Validate against schema
    if (!validateEntry({key, value, source, description})) {
        throw ConfigValidationException("Configuration validation failed for key: " + key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->description = description.empty() ? it->description : description;
        pImpl->keyModified[key] = true;
    } else {
        ConfigEntry newEntry;
        newEntry.key = key;
        newEntry.value = value;
        newEntry.source = source;
        newEntry.description = description;
        
        // Determine type from value
        newEntry.type = getTypeName(value);
        
        // Check if schema defines this field
        auto schemaIt = schema_.schema.find(key);
        if (schemaIt != schema_.schema.end()) {
            newEntry.type = schemaIt->second.type;
            newEntry.required = schemaIt->second.required;
            newEntry.validationRegex = schemaIt->second.validationRegex;
            newEntry.defaultValue = schemaIt->second.defaultValue;
        }
        
        pImpl->entries.push_back(std::move(newEntry));
        pImpl->keyModified[key] = true;
    }
}

void Config::set(const std::string& key, const std::string& value,
                 ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, int value,
                 ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, double value,
                 ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, bool value,
                 ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
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
    pImpl->keyModified.erase(key);
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
    pImpl->keyModified.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [" << valueToString(entry.value) << "] (";
        oss << static_cast<int>(entry.source) << ")";
        
        if (!entry.type.empty()) {
            oss << " [type: " << entry.type << "]";
        }
        
        if (entry.required) {
            oss << " [required]";
        }
        
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        
        oss << "\n";
    }
    
    oss << "\nSchema fields: " << schema_.schema.size() << " defined";
    oss << "\nModified keys: " << pImpl->keyModified.size();
    return oss.str();
}

bool Config::validate() const {
    // Validate all entries against schema
    for (const auto& entry : pImpl->entries) {
        if (!validateEntry(entry)) {
            return false;
        }
    }
    
    // Check for missing required fields
    for (const auto& [key, fieldInfo] : schema_.schema) {
        if (fieldInfo.required && !has(key)) {
            std::cerr << "Validation error: Missing required field: '" << key << "'" << std::endl;
            return false;
        }
    }
    
    return true;
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    
    return ConfigSource::Default;
}

bool Config::isModified(const std::string& key) const {
    auto it = pImpl->keyModified.find(key);
    return it != pImpl->keyModified.end() && it->second;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Mark as modified if different from current value
        auto currentIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        bool isDifferent = currentIt == pImpl->entries.end() ||
                          currentIt->value != entry.value;
        
        set(entry.key, entry.value, source, entry.description);
        if (isDifferent) {
            pImpl->keyModified[entry.key] = true;
        }
    }
}

std::vector<std::string> Config::diff(const Config& other) const {
    std::vector<std::string> diffs;
    
    // Check for keys in this config not in other
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (otherIt == other.pImpl->entries.end()) {
            diffs.push_back("Added: " + entry.key + " = " + valueToString(entry.value));
        } else if (otherIt->value != entry.value) {
            diffs.push_back("Changed: " + entry.key + " from " + 
                          valueToString(otherIt->value) + " to " + valueToString(entry.value));
        }
    }
    
    // Check for keys in other not in this
    for (const auto& entry : other.pImpl->entries) {
        auto currentIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (currentIt == pImpl->entries.end()) {
            diffs.push_back("Removed: " + entry.key + " = " + valueToString(entry.value));
        }
    }
    
    return diffs;
}

std::string Config::exportToJson() const {
    return jsonValueToString(pImpl->entries);
}

bool Config::importFromJson(const std::string& jsonStr) {
    try {
        return parseSimpleJson(jsonStr, ConfigSource::File, "Imported from JSON");
    } catch (const std::exception& e) {
        std::cerr << "Error importing from JSON: " << e.what() << std::endl;
        return false;
    }
}

// ConfigFileParser implementation
bool ConfigFileParser::parse(const std::string& content, std::vector<ConfigEntry>& entries,
                            ConfigSource source, const std::string& description) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = Config::trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = Config::trim(line.substr(0, pos));
            std::string value = Config::trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                     (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            ConfigEntry entry;
            entry.key = key;
            entry.value = ConfigValue(value);
            entry.source = source;
            entry.description = description;
            entry.type = Config::getTypeName(entry.value);
            
            entries.push_back(entry);
        }
    }
    
    return true;
}

std::string ConfigFileParser::serialize(const std::vector<ConfigEntry>& entries) {
    std::ostringstream oss;
    
    for (const auto& entry : entries) {
        oss << "# " << entry.description << "\n";
        oss << entry.key << " = " << Config::valueToString(entry.value) << "\n\n";
    }
    
    return oss.str();
}

// ConfigJsonParser implementation
bool ConfigJsonParser::parse(const std::string& json, std::vector<ConfigEntry>& entries,
                            ConfigSource source, const std::string& description) {
    try {
        // Simple JSON parsing - extract key-value pairs from "key": "value" format
        std::istringstream stream(json);
        std::string line;
        
        while (std::getline(stream, line)) {
            line = Config::trim(line);
            if (line.empty() || line[0] == '/' || line[0] == '"' || line.find('{') != std::string::npos || 
                line.find('}') != std::string::npos || line.find('[') != std::string::npos || 
                line.find(']') != std::string::npos) {
                continue;
            }
            
            // Handle "key": "value" format
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = Config::trim(line.substr(0, colonPos));
                std::string value = Config::trim(line.substr(colonPos + 1));
                
                // Remove trailing comma and quotes
                if (!value.empty() && value.back() == ',') {
                    value.pop_back();
                }
                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
                
                ConfigEntry entry;
                entry.key = key;
                entry.value = ConfigValue(value);
                entry.source = source;
                entry.description = description;
                entry.type = Config::getTypeName(entry.value);
                
                entries.push_back(entry);
            }
        }
        
        return !entries.empty();
    } catch (const std::exception&) {
        return false;
    }
}

std::string ConfigJsonParser::serialize(const std::vector<ConfigEntry>& entries) {
    std::ostringstream oss;
    oss << "{\n";
    
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& entry = entries[i];
        oss << "  \"" << entry.key << "\": " << Config::jsonValueToString(entry.value);
        if (i < entries.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "}\n";
    return oss.str();
}

// ConfigYamlParser implementation
bool ConfigYamlParser::parse(const std::string& yaml, std::vector<ConfigEntry>& entries,
                            ConfigSource source, const std::string& description) {
    try {
        // Simple YAML parsing - extract key: value pairs
        std::istringstream stream(yaml);
        std::string line;
        
        while (std::getline(stream, line)) {
            line = Config::trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Handle "key: value" format
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = Config::trim(line.substr(0, colonPos));
                std::string value = Config::trim(line.substr(colonPos + 1));
                
                ConfigEntry entry;
                entry.key = key;
                entry.value = ConfigValue(value);
                entry.source = source;
                entry.description = description;
                entry.type = Config::getTypeName(entry.value);
                
                entries.push_back(entry);
            }
        }
        
        return !entries.empty();
    } catch (const std::exception&) {
        return false;
    }
}

std::string ConfigYamlParser::serialize(const std::vector<ConfigEntry>& entries) {
    std::ostringstream oss;
    
    for (const auto& entry : entries) {
        oss << entry.key << ": " << Config::yamlValueToString(entry.value) << "\n";
    }
    
    return oss.str();
}

// Config implementation helpers
std::string Config::yamlValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
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

std::string Config::jsonValueToString(const ConfigValue& value) {
    return valueToString(value);
}

bool Config::validateEntry(const ConfigEntry& entry) const {
    // Check if entry has a type defined in schema
    auto schemaIt = schema_.schema.find(entry.key);
    if (schemaIt != schema_.schema.end()) {
        const auto& fieldInfo = schemaIt->second;
        
        // Type validation
        std::string expectedType = fieldInfo.type;
        std::string actualType = getTypeName(entry.value);
        
        if (expectedType != "auto" && expectedType != actualType) {
            std::cerr << "Validation error: Type mismatch for field '" << entry.key 
                      << "': expected " << expectedType << ", got " << actualType << std::endl;
            return false;
        }
        
        // Required validation
        if (fieldInfo.required) {
            // Entry exists, so required check passes
        }
        
        // Regex validation
        if (!fieldInfo.validationRegex.empty()) {
            if (!matchesRegex(valueToString(entry.value), fieldInfo.validationRegex)) {
                std::cerr << "Validation error: Value for field '" << entry.key 
                          << "' does not match pattern: '" << valueToString(entry.value) << "'" << std::endl;
                return false;
            }
        }
    }
    
    return true;
}

std::string Config::getTypeName(const ConfigValue& value) {
    std::string typeName;
    std::visit([&typeName](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) typeName = "int";
        else if constexpr (std::is_same_v<T, int64_t>) typeName = "int64_t";
        else if constexpr (std::is_same_v<T, double>) typeName = "double";
        else if constexpr (std::is_same_v<T, bool>) typeName = "bool";
        else if constexpr (std::is_same_v<T, std::string>) typeName = "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) typeName = "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) typeName = "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) typeName = "vector<string>";
        else typeName = "unknown";
    }, value);
    return typeName;
}

bool Config::parseSimpleFormat(std::istream& stream, ConfigSource source,
                               const std::string& description) {
    std::string line;
    bool success = false;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                     (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, source, description);
            success = true;
        }
    }
    
    return success;
}

bool Config::parseSimpleFormat(const std::string& content, ConfigSource source,
                               const std::string& description) {
    std::istringstream stream(content);
    return parseSimpleFormat(stream, source, description);
}

// ConfigSchema implementation
void ConfigSchema::addField(const std::string& key, const std::string& type,
                           bool required, const std::string& description,
                           const std::string& validationRegex,
                           const std::string& defaultValue) {
    FieldInfo field;
    field.type = type;
    field.required = required;
    field.description = description;
    field.validationRegex = validationRegex;
    field.defaultValue = defaultValue;
    
    schema[key] = field;
    fieldMetadata[key] = description + " (type: " + type + ")";
}

void ConfigSchema::addValidator(const std::string& key, Validator validator) {
    auto it = schema.find(key);
    if (it != schema.end()) {
        it->second.validators.push_back(validator);
    }
}

std::vector<std::string> ConfigSchema::validate(const std::vector<ConfigEntry>& entries) const {
    std::vector<std::string> errors;
    
    for (const auto& entry : entries) {
        auto fieldIt = schema.find(entry.key);
        if (fieldIt != schema.end()) {
            // Type validation
            std::string expectedType = fieldIt->second.type;
            std::string actualType = Config::getTypeName(entry.value);
            
            if (expectedType != "auto" && expectedType != actualType) {
                errors.push_back("Type mismatch for field '" + entry.key + "': expected " +
                               expectedType + ", got " + actualType);
            }
            
            // Regex validation
            if (!fieldIt->second.validationRegex.empty() && 
                !Config::matchesRegex(Config::valueToString(entry.value), fieldIt->second.validationRegex)) {
                errors.push_back("Value validation failed for field '" + entry.key + 
                               "': '" + Config::valueToString(entry.value) + "' does not match pattern");
            }
            
            // Run custom validators
            for (const auto& validator : fieldIt->second.validators) {
                if (!validator(entry.value)) {
                    errors.push_back("Custom validation failed for field '" + entry.key + "'");
                }
            }
        }
    }
    
    return errors;
}

std::string ConfigSchema::getFieldDescription(const std::string& key) const {
    auto it = schema.find(key);
    return it != schema.end() ? it->second.description : "";
}

std::string ConfigSchema::getFieldType(const std::string& key) const {
    auto it = schema.find(key);
    return it != schema.end() ? it->second.type : "unknown";
}

bool ConfigSchema::isRequired(const std::string& key) const {
    auto it = schema.find(key);
    return it != schema.end() ? it->second.required : false;
}

// Default schema for common configuration fields
ConfigSchema Config::getDefaultSchema() {
    ConfigSchema schema;
    
    // Add common configuration fields
    schema.addField("random_seed", "int", false, "Random seed for reproducible simulations",
                   "^[0-9]+$", "42");
    schema.addField("simulation_timestep", "double", true, "Simulation timestep in seconds",
                   "^[0-9]*\\.?[0-9]+$", "0.001");
    schema.addField("neuron_count", "int", true, "Number of neurons in the network",
                   "^[1-9]\\d*$", "100");
    schema.addField("region_count", "int", false, "Number of neural regions",
                   "^[0-9]+$", "1");
    schema.addField("connection_probability", "double", false, 
                   "Probability of synaptic connections", "^[0-9]*\\.?[0-9]+$");
    
    // Plasticity parameters
    schema.addField("stdp_ltp_weight", "double", false, "STDP long-term potentiation weight",
                   "^[0-9]*\\.?[0-9]+$", "0.02");
    schema.addField("stdp_ltd_weight", "double", false, "STDP long-term depression weight",
                   "^[0-9]*\\.?[0-9]+$", "0.015");
    schema.addField("stdp_tau", "double", false, "STDP time constant",
                   "^[0-9]*\\.?[0-9]+$", "20.0");
    
    // Add custom validators for some fields
    schema.addValidator("random_seed", [](const ConfigValue& v) {
        try {
            int val = std::get<int>(v);
            return val >= 0 && val <= 1000000;
        } catch (...) { return false; }
    });
    
    schema.addValidator("simulation_timestep", [](const ConfigValue& v) {
        try {
            double val = std::get<double>(v);
            return val > 0.0 && val < 1.0;
        } catch (...) { return false; }
    });
    
    return schema;
}

} // namespace nlm