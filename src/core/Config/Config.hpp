# NLM (熙然) - Neural Learning Machine
# Enhanced Configuration System
#
# This configuration system supports multiple formats:
# - Basic key=value format (backward compatible)
# - JSON format (standard)
# - YAML format (if available)
#
# Enhanced features:
# - Proper JSON/YAML parsing
# - Configuration validation
# - File format detection
# - Better error handling
# - Descriptive configuration entries
# - Type safety

#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <regex>

// Include nlohmann/json if available
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

namespace nlm {

class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) : std::runtime_error(message) {}
};

// Enhanced configuration entry with metadata
struct ConfigEntry {
    std::string key;
    std::string value;  // Stored as string for serialization
    ConfigSource source;
    std::string description;
    std::string type;  // "string", "int", "float", "bool", "array"
    bool required;
    std::string validationPattern;  // Regex pattern for validation
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), type("string"),
          required(false), validationPattern() {}
    
    ConfigEntry(const std::string& k, const std::string& v, ConfigSource s,
                const std::string& desc = "", const std::string& t = "string",
                bool req = false, const std::string& pattern = "")
        : key(k), value(v), source(s), description(desc), type(t),
          required(req), validationPattern(pattern) {}
    
    bool isValid() const {
        if (required && value.empty()) {
            return false;
        }
        if (!validationPattern.empty()) {
            std::regex pattern(validationPattern);
            if (!std::regex_match(value, pattern)) {
                return false;
            }
        }
        return true;
    }
};

// Enhanced configuration class with multi-format support
class Config {
public:
    Config() = default;
    ~Config() = default;
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept = default;
    Config& operator=(Config&&) noexcept = default;
    
    // Enhanced file loading with format detection
    bool loadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Detect file format
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.clear();
        file.seekg(0);
        
        std::string detectedFormat = detectFileFormat(filepath, content);
        
        try {
            if (detectedFormat == "json") {
                return loadFromJSON(content, filepath);
            } else if (detectedFormat == "yaml" || detectedFormat == "yml") {
                return loadFromYAML(content, filepath);
            } else {
                // Default to basic key=value format
                return loadFromBasicFormat(content, filepath);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to load configuration from " + filepath + ": " + e.what());
            return false;
        }
    }
    
    // Load from JSON (using nlohmann/json if available)
    bool loadFromJSON(const std::string& content, const std::string& filepath = "") {
        #ifdef HAVE_NLOHMANN_JSON
        try {
            auto json = nlohmann::json::parse(content);
            
            // Clear existing entries
            clear();
            
            for (auto it = json.begin(); it != json.end(); ++it) {
                std::string key = it.key();
                std::string value;
                std::string type = "unknown";
                std::string description = "";
                
                if (it->is_string()) {
                    value = it->get<std::string>();
                    type = "string";
                } else if (it->is_number_integer()) {
                    value = std::to_string(it->get<int64_t>());
                    type = "int";
                } else if (it->is_number_float()) {
                    value = std::to_string(it->get<double>());
                    type = "float";
                } else if (it->is_boolean()) {
                    value = it->get<bool>() ? "true" : "false";
                    type = "bool";
                } else if (it->is_array()) {
                    // Convert array to string representation
                    value = "[";
                    for (size_t i = 0; i < it->size(); ++i) {
                        if (i > 0) value += ", ";
                        if ((*it)[i].is_string()) {
                            value += "\"" + (*it)[i].get<std::string>() + "\"";
                        } else {
                            value += std::to_string((*it)[i].get<double>());
                        }
                    }
                    value += "]";
                    type = "array";
                }
                
                // Try to get description from a special field
                std::string descKey = "description_" + key;
                if (json.contains(descKey)) {
                    description = json[descKey].get<std::string>();
                }
                
                ConfigEntry entry(key, value, ConfigSource::File, description, type);
                if (!entry.isValid()) {
                    NLM_LOG_WARNING("Invalid configuration entry: " + key + " = " + value);
                }
                
                addEntry(std::move(entry));
            }
            
            NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
            return true;
            
        } catch (const nlohmann::json::exception& e) {
            throw ConfigError("JSON parsing error: " + std::string(e.what()));
        }
        #else
        throw ConfigError("JSON support requires nlohmann/json library");
        #endif
    }
    
    // Load from YAML (placeholder for future implementation)
    bool loadFromYAML(const std::string& content, const std::string& filepath = "") {
        // Placeholder for YAML support
        // In a real implementation, you would use a YAML library like YAML-cpp
        NLM_LOG_WARNING("YAML loading is not yet implemented - using basic format");
        return loadFromBasicFormat(content, filepath);
    }
    
    // Enhanced basic format loader with better validation
    bool loadFromBasicFormat(const std::string& content, const std::string& filepath = "") {
        std::istringstream stream(content);
        std::string line;
        int lineNumber = 0;
        
        while (std::getline(stream, line)) {
            ++lineNumber;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Enhanced parsing for complex formats
            try {
                // Handle JSON-like entries in basic format: key = {"value": "val", "type": "string", "description": "desc"}
                if (line.find('{') != std::string::npos && line.find('}') != std::string::npos) {
                    parseAdvancedFormatLine(line, filepath, lineNumber);
                } else {
                    parseSimpleFormatLine(line, filepath, lineNumber);
                }
            } catch (const ConfigError& e) {
                NLM_LOG_ERROR("Configuration error in " + filepath + ":" + std::to_string(lineNumber) + ": " + e.what());
                continue;  // Skip invalid lines but continue processing
            }
        }
        
        return !entries.empty();
    }
    
    // Parse advanced format with metadata
    void parseAdvancedFormatLine(const std::string& line, const std::string& filepath, int lineNumber) {
        // This is a placeholder for advanced parsing - could use regex or simple JSON parsing
        // For now, fall back to basic format
        parseSimpleFormatLine(line, filepath, lineNumber);
    }
    
    // Parse simple key=value line
    void parseSimpleFormatLine(const std::string& line, const std::string& filepath, int lineNumber) {
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
            
            // Determine type based on value content
            std::string type = detectType(value);
            
            ConfigEntry entry(key, value, ConfigSource::File, 
                            "Loaded from " + filepath + ":" + std::to_string(lineNumber), type);
            
            if (!entry.isValid()) {
                throw ConfigError("Invalid configuration entry: " + key + " = " + value);
            }
            
            addEntry(std::move(entry));
        }
    }
    
    // Enhanced save to file with format selection
    bool saveToFile(const std::string& filepath, const std::string& format = "auto") const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            if (format == "auto" || format == "json") {
                return saveToJSON(file, format == "json");
            } else if (format == "yaml" || format == "yml") {
                NLM_LOG_WARNING("YAML saving is not yet implemented - using JSON format");
                return saveToJSON(file, true);
            } else {
                return saveToBasicFormat(file);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to save configuration to " + filepath + ": " + e.what());
            return false;
        }
    }
    
    // Save to JSON format
    bool saveToJSON(std::ostream& output, bool prettyPrint = false) const {
        #ifdef HAVE_NLOHMANN_JSON
        nlohmann::json json;
        
        for (const auto& entry : entries) {
            // Parse value back to appropriate type
            nlohmann::json jsonValue;
            if (entry.type == "string") {
                jsonValue = entry.value;
            } else if (entry.type == "int") {
                try {
                    jsonValue = std::stoll(entry.value);
                } catch (...) {
                    jsonValue = 0;
                }
            } else if (entry.type == "float") {
                try {
                    jsonValue = std::stod(entry.value);
                } catch (...) {
                    jsonValue = 0.0;
                }
            } else if (entry.type == "bool") {
                jsonValue = (entry.value == "true" || entry.value == "1" || entry.value == "yes");
            } else if (entry.type == "array") {
                // Parse array representation
                jsonValue = parseArrayValue(entry.value);
            } else {
                jsonValue = entry.value;
            }
            
            json[entry.key] = jsonValue;
            
            // Add description as metadata if present
            if (!entry.description.empty() && entry.description.find(filepath) != std::string::npos) {
                std::string descKey = "description_" + entry.key;
                json[descKey] = entry.description;
            }
        }
        
        if (prettyPrint) {
            output << json.dump(2);
        } else {
            output << json.dump();
        }
        
        NLM_LOG_INFO("Saved JSON configuration successfully");
        return true;
        #else
        NLM_LOG_ERROR("JSON saving requires nlohmann/json library");
        return false;
        #endif
    }
    
    // Save to basic key=value format
    bool saveToBasicFormat(std::ostream& output) const {
        for (const auto& entry : entries) {
            output << "# " << entry.description << "\n";
            output << entry.key << " = " << entry.value << "\n\n";
        }
        
        NLM_LOG_INFO("Saved basic format configuration successfully");
        return true;
    }
    
    // Parse array value from string representation
    nlohmann::json parseArrayValue(const std::string& arrayStr) const {
        nlohmann::json result = nlohmann::json::array();
        
        if (arrayStr.empty() || arrayStr == "[]") {
            return result;
        }
        
        std::string content = arrayStr.substr(1, arrayStr.size() - 2);  // Remove brackets
        std::stringstream ss(content);
        std::string item;
        
        while (std::getline(ss, item, ',')) {
            item = trim(item);
            if (item.empty()) continue;
            
            // Remove quotes if present
            if (item.size() >= 2 && ((item.front() == '"' && item.back() == '"') || 
                                    (item.front() == '\'' && item.back() == '\''))) {
                item = item.substr(1, item.size() - 2);
            }
            
            // Try to parse as number, fallback to string
            try {
                if (item.find('.') != std::string::npos) {
                    result.push_back(std::stod(item));
                } else {
                    result.push_back(std::stoll(item));
                }
            } catch (...) {
                result.push_back(item);
            }
        }
        
        return result;
    }
    
    // Detect file format based on extension and content
    std::string detectFileFormat(const std::string& filepath, const std::string& content) const {
        // Check file extension first
        std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
        if (ext == "json") return "json";
        if (ext == "yaml" || ext == "yml") return "yaml";
        
        // Detect from content
        if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
            // Could be JSON or advanced format
            try {
                auto json = nlohmann::json::parse(content);
                return "json";
            } catch (...) {
                // Not valid JSON, check for basic format
            }
        }
        
        if (content.find("key\":") != std::string::npos || content.find("'key':") != std::string::npos) {
            return "yaml";
        }
        
        return "basic";  // Default to basic format
    }
    
    // Detect type from string value
    std::string detectType(const std::string& value) const {
        if (value.empty()) return "string";
        
        // Check for boolean
        if (value == "true" || value == "false" || value == "1" || value == "0" || 
            value == "yes" || value == "no") {
            return "bool";
        }
        
        // Check for integer
        if (std::all_of(value.begin(), value.end(), ::isdigit) ||
            (value[0] == '-' && std::all_of(value.begin() + 1, value.end(), ::isdigit))) {
            return "int";
        }
        
        // Check for float
        try {
            size_t pos;
            double result = std::stod(value, &pos);
            if (pos == value.size()) {
                return "float";
            }
        } catch (...) {
        }
        
        // Check for array
        if ((value.front() == '[' && value.back() == ']') ||
            (value.front() == '{' && value.back() == '}')) {
            return "array";
        }
        
        // Default to string
        return "string";
    }
    
    // Enhanced set with type checking
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& type = "string", const std::string& description = "") {
        ConfigEntry entry(key, value, source, description, type);
        if (!entry.isValid()) {
            throw ConfigError("Invalid configuration: " + key + " = " + value);
        }
        addEntry(std::move(entry));
    }
    
    // Convenience overloads
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
        set(key, value, source, detectType(value), "");
    }
    
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
        set(key, std::to_string(value), source, "int", "");
    }
    
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
        set(key, std::to_string(value), source, "float", "");
    }
    
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
        set(key, value ? "true" : "false", source, "bool", "");
    }
    
    // Add entry with validation
    void addEntry(ConfigEntry&& entry) {
        if (entry.isValid()) {
            entries.push_back(std::move(entry));
        } else {
            throw ConfigError("Invalid configuration entry: " + entry.key + " = " + entry.value);
        }
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        for (const auto& entry : entries) {
            keys.push_back(entry.key);
        }
        return keys;
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return std::any_of(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    }
    
    // Remove key
    void remove(const std::string& key) {
        entries.erase(
            std::remove_if(entries.begin(), entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            entries.end()
        );
    }
    
    // Clear all
    void clear() {
        entries.clear();
    }
    
    // Get configuration summary
    std::string summary() const {
        std::ostringstream oss;
        oss << "Enhanced Configuration (" << entries.size() << " entries):\n";
        
        for (const auto& entry : entries) {
            oss << "  " << entry.key << " = " << entry.value;
            if (!entry.type.empty() && entry.type != "string") {
                oss << " (" << entry.type << ")";
            }
            oss << " (" << static_cast<int>(entry.source) << ")";
            if (!entry.description.empty()) {
                oss << " - " << entry.description;
            }
            oss << "\n";
        }
        
        return oss.str();
    }
    
    // Get description for a key
    std::string getDescription(const std::string& key) const {
        auto it = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        return it != entries.end() ? it->description : "";
    }
    
    // Get source for a key
    ConfigSource getSource(const std::string& key) const {
        auto it = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        return it != entries.end() ? it->source : ConfigSource::Default;
    }
    
    // Get all entries
    const std::vector<ConfigEntry>& getAllEntries() const {
        return entries;
    }
    
private:
    std::string trim(const std::string& str) const {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    std::vector<ConfigEntry> entries;
};