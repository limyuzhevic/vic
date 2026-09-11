#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <variant>
#include <string>
#include <map>
#include <stdexcept>
#include <iostream>
#include <cctype>
#include <unordered_set>
#include <optional>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <typeinfo>

namespace nlm {

// Exception definitions
ConfigException::ConfigException(const std::string& message) 
    : std::runtime_error(message) {}

FileParseException::FileParseException(const std::string& message, const std::string& filepath, size_t line, size_t column)
    : ConfigException(message), filepath_(filepath), line_(line), column_(column) {}

// Implementation of Config::Impl - now includes snapshot capabilities
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::unordered_map<std::string, ConfigValue>> nestedEntries;
    std::string currentSection;
    std::vector<ValidationError> validationErrors;
    std::unordered_map<std::string, std::vector<ValidationError>> validationErrorsByKey;
    std::vector<ConfigEntry> pendingChanges;
    bool isDirty = false;
    std::string lastFilepath;
};

// File parsing implementation with multiple format support
class FileParser {
public:
    enum class Format { AUTO, JSON, YAML, TOML, INI };
    
    static Format detectFormat(const std::string& content) {
        // Trim whitespace
        std::string trimmed = trim(content);
        
        // Check for JSON
        if ((trimmed.front() == '{' && trimmed.back() == '}') ||
            (trimmed.front() == '[' && trimmed.back() == ']')) {
            return Format::JSON;
        }
        
        // Check for YAML (simple indicators)
        if (content.find(": ") != std::string::npos && content.find('\n') != std::string::npos) {
            return Format::YAML;
        }
        
        // Check for TOML (key = value format)
        if (content.find(" = ") != std::string::npos) {
            return Format::TOML;
        }
        
        // Default to INI format
        return Format::INI;
    }
    
    static bool parseJSON(const std::string& content, std::unordered_map<std::string, ConfigValue>& result) {
        try {
            // Simple JSON parsing - in production would use a proper JSON library
            size_t pos = 0;
            while (pos < content.size()) {
                skipWhitespace(content, pos);
                if (pos >= content.size()) break;
                
                if (content[pos] == '}') {
                    pos++;
                    continue;
                }
                
                // Parse key
                size_t keyStart = pos;
                if (content[pos] == '"') {
                    pos++;
                    while (pos < content.size() && content[pos] != '"') {
                        if (content[pos] == '\\') pos++; // Skip escaped char
                        pos++;
                    }
                } else {
                    while (pos < content.size() && content[pos] != ':' && content[pos] != '}') {
                        pos++;
                    }
                }
                std::string key = content.substr(keyStart, pos - keyStart);
                
                skipWhitespace(content, pos);
                if (pos >= content.size() || content[pos] != ':') {
                    throw FileParseException("Expected ':' after key", "", 0, pos);
                }
                pos++;
                skipWhitespace(content, pos);
                
                // Parse value
                ConfigValue value;
                if (content[pos] == '"') {
                    // String value
                    pos++;
                    size_t valStart = pos;
                    while (pos < content.size() && content[pos] != '"') {
                        if (content[pos] == '\\') pos++;
                        pos++;
                    }
                    value = content.substr(valStart, pos - valStart);
                    pos++; // Skip closing quote
                } else if (content[pos] == 't' || content[pos] == 'f') {
                    // Boolean value
                    if (content.substr(pos, 4) == "true") {
                        value = true;
                        pos += 4;
                    } else if (content.substr(pos, 5) == "false") {
                        value = false;
                        pos += 5;
                    } else {
                        throw FileParseException("Expected boolean value", "", 0, pos);
                    }
                } else if (content[pos] == '-' || std::isdigit(content[pos])) {
                    // Number
                    size_t valStart = pos;
                    while (pos < content.size() && (std::isdigit(content[pos]) || content[pos] == '.' || content[pos] == '-' || content[pos] == 'e' || content[pos] == 'E')) {
                        pos++;
                    }
                    std::string numStr = content.substr(valStart, pos - valStart);
                    try {
                        size_t decPos = numStr.find('.');
                        if (decPos != std::string::npos && decPos < numStr.size() - 1) {
                            value = std::stod(numStr);
                        } else if (numStr.find('e') != std::string::npos || numStr.find('E') != std::string::npos) {
                            value = std::stod(numStr);
                        } else {
                            value = std::stoll(numStr);
                        }
                    } catch (...) {
                        throw FileParseException("Invalid number format: " + numStr, "", 0, valStart);
                    }
                } else if (content[pos] == '[') {
                    // Array
                    pos++;
                    std::vector<ConfigValue> array;
                    while (pos < content.size() && content[pos] != ']') {
                        skipWhitespace(content, pos);
                        if (content[pos] == ']') break;
                        
                        ConfigValue arrayItem;
                        parseValueSimple(content, pos, arrayItem);
                        array.push_back(arrayItem);
                        
                        skipWhitespace(content, pos);
                        if (pos < content.size() && content[pos] == ',') pos++;
                    }
                    if (pos >= content.size() || content[pos] != ']') {
                        throw FileParseException("Unclosed array", "", 0, pos);
                    }
                    pos++; // Skip closing bracket
                    value = array;
                } else {
                    throw FileParseException("Unexpected character in JSON: " + std::string(1, content[pos]), "", 0, pos);
                }
                
                result[key] = value;
                skipWhitespace(content, pos);
                if (pos < content.size() && content[pos] == ',') pos++;
            }
            return true;
        } catch (const FileParseException&) {
            throw;
        } catch (...) {
            throw FileParseException("Invalid JSON format", "", 0, 0);
        }
    }
    
    static bool parseYAML(const std::string& content, std::unordered_map<std::string, ConfigValue>& result) {
        // Simple YAML parsing - would use proper YAML library in production
        try {
            size_t pos = 0;
            std::string currentSection;
            
            while (pos < content.size()) {
                skipWhitespace(content, pos);
                if (pos >= content.size()) break;
                
                if (content[pos] == '#') {
                    // Comment
                    while (pos < content.size() && content[pos] != '\n') pos++;
                    continue;
                }
                
                if (content[pos] == '-') {
                    // List item - simplified
                    pos++;
                    skipWhitespace(content, pos);
                    // Parse the list item value
                    ConfigValue value;
                    parseValueSimple(content, pos, value);
                    // For now, just add to result with a generated key
                    static int listIndex = 0;
                    result["list_" + std::to_string(listIndex++)] = value;
                    continue;
                }
                
                // Parse key
                size_t keyStart = pos;
                while (pos < content.size() && content[pos] != ':' && content[pos] != '\n') {
                    pos++;
                }
                std::string key = trim(content.substr(keyStart, pos - keyStart));
                
                if (pos >= content.size() || content[pos] != ':') {
                    // Might be a section header
                    if (key.size() > 0 && (key.front() == '[' && key.back() == ']')) {
                        currentSection = key.substr(1, key.size() - 2);
                        pos++;
                        continue;
                    }
                    throw FileParseException("Expected ':' after key in YAML", "", 0, pos);
                }
                
                pos++; // Skip ':'
                skipWhitespace(content, pos);
                
                // Parse value
                ConfigValue value;
                parseValueYAML(content, pos, value);
                
                // Add to result with section prefix if any
                std::string fullKey = currentSection.empty() ? key : currentSection + "." + key;
                result[fullKey] = value;
                
                skipWhitespace(content, pos);
                if (pos < content.size() && content[pos] == '\n') pos++;
            }
            return true;
        } catch (const FileParseException&) {
            throw;
        } catch (...) {
            throw FileParseException("Invalid YAML format", "", 0, 0);
        }
    }
    
    static bool parseTOML(const std::string& content, std::unordered_map<std::string, ConfigValue>& result) {
        // Simple TOML parsing - would use proper TOML library in production
        try {
            size_t pos = 0;
            std::string currentSection;
            
            while (pos < content.size()) {
                skipWhitespace(content, pos);
                if (pos >= content.size()) break;
                
                if (content[pos] == '#') {
                    // Comment
                    while (pos < content.size() && content[pos] != '\n') pos++;
                    continue;
                }
                
                if (content[pos] == '[') {
                    // Section header
                    size_t endPos = content.find(']', pos);
                    if (endPos == std::string::npos) {
                        throw FileParseException("Unclosed section header", "", 0, pos);
                    }
                    currentSection = trim(content.substr(pos + 1, endPos - pos - 1));
                    pos = endPos + 1;
                    continue;
                }
                
                // Parse key (can be quoted or unquoted)
                size_t keyStart = pos;
                while (pos < content.size() && content[pos] != '=' && content[pos] != '\n') {
                    pos++;
                }
                std::string key = trim(content.substr(keyStart, pos - keyStart));
                
                if (pos >= content.size() || content[pos] != '=') {
                    throw FileParseException("Expected '=' after key in TOML", "", 0, pos);
                }
                
                pos++; // Skip '='
                skipWhitespace(content, pos);
                
                // Parse value
                ConfigValue value;
                parseValueSimple(content, pos, value);
                
                // Add to result with section prefix if any
                std::string fullKey = currentSection.empty() ? key : currentSection + "." + key;
                result[fullKey] = value;
                
                skipWhitespace(content, pos);
                if (pos < content.size() && content[pos] == '\n') pos++;
            }
            return true;
        } catch (const FileParseException&) {
            throw;
        } catch (...) {
            throw FileParseException("Invalid TOML format", "", 0, 0);
        }
    }
    
    static bool parseINI(const std::string& content, std::unordered_map<std::string, ConfigValue>& result) {
        try {
            size_t pos = 0;
            std::string currentSection;
            
            while (pos < content.size()) {
                skipWhitespace(content, pos);
                if (pos >= content.size()) break;
                
                if (content[pos] == ';' || content[pos] == '#') {
                    // Comment
                    while (pos < content.size() && content[pos] != '\n') pos++;
                    continue;
                }
                
                if (content[pos] == '[') {
                    // Section header
                    size_t endPos = content.find(']', pos);
                    if (endPos == std::string::npos) {
                        throw FileParseException("Unclosed section header", "", 0, pos);
                    }
                    currentSection = trim(content.substr(pos + 1, endPos - pos - 1));
                    pos = endPos + 1;
                    continue;
                }
                
                // Parse key
                size_t keyStart = pos;
                while (pos < content.size() && content[pos] != '=' && content[pos] != '\n') {
                    pos++;
                }
                std::string key = trim(content.substr(keyStart, pos - keyStart));
                
                if (pos >= content.size() || content[pos] != '=') {
                    throw FileParseException("Expected '=' after key in INI", "", 0, pos);
                }
                
                pos++; // Skip '='
                skipWhitespace(content, pos);
                
                // Parse value
                ConfigValue value;
                parseValueSimple(content, pos, value);
                
                // Add to result with section prefix if any
                std::string fullKey = currentSection.empty() ? key : currentSection + "." + key;
                result[fullKey] = value;
                
                skipWhitespace(content, pos);
                if (pos < content.size() && content[pos] == '\n') pos++;
            }
            return true;
        } catch (const FileParseException&) {
            throw;
        } catch (...) {
            throw FileParseException("Invalid INI format", "", 0, 0);
        }
    }
    
    static void skipWhitespace(const std::string& content, size_t& pos) {
        while (pos < content.size() && std::isspace(content[pos])) pos++;
    }
    
    static void parseValueSimple(const std::string& content, size_t& pos, ConfigValue& value) {
        skipWhitespace(content, pos);
        if (pos >= content.size()) {
            throw FileParseException("Unexpected end of input", "", 0, pos);
        }
        
        if (content[pos] == '"') {
            // String
            pos++;
            size_t start = pos;
            while (pos < content.size() && content[pos] != '"') {
                if (content[pos] == '\\') pos++;
                pos++;
            }
            value = content.substr(start, pos - start);
            pos++; // Skip closing quote
        } else if (content[pos] == 't' || content[pos] == 'f') {
            // Boolean
            if (content.substr(pos, 4) == "true") {
                value = true;
                pos += 4;
            } else if (content.substr(pos, 5) == "false") {
                value = false;
                pos += 5;
            } else {
                throw FileParseException("Expected boolean value", "", 0, pos);
            }
        } else if (content[pos] == '-' || std::isdigit(content[pos])) {
            // Number
            size_t start = pos;
            while (pos < content.size() && (std::isdigit(content[pos]) || content[pos] == '.' || content[pos] == '-' || content[pos] == 'e' || content[pos] == 'E')) {
                pos++;
            }
            std::string numStr = content.substr(start, pos - start);
            try {
                size_t decPos = numStr.find('.');
                if (decPos != std::string::npos && decPos < numStr.size() - 1) {
                    value = std::stod(numStr);
                } else if (numStr.find('e') != std::string::npos || numStr.find('E') != std::string::npos) {
                    value = std::stod(numStr);
                } else {
                    value = std::stoll(numStr);
                }
            } catch (...) {
                throw FileParseException("Invalid number format: " + numStr, "", 0, start);
            }
        } else if (content[pos] == '[') {
            // Array
            pos++;
            std::vector<ConfigValue> array;
            while (pos < content.size() && content[pos] != ']') {
                skipWhitespace(content, pos);
                if (content[pos] == ']') break;
                
                ConfigValue arrayItem;
                parseValueSimple(content, pos, arrayItem);
                array.push_back(arrayItem);
                
                skipWhitespace(content, pos);
                if (pos < content.size() && content[pos] == ',') pos++;
            }
            if (pos >= content.size() || content[pos] != ']') {
                throw FileParseException("Unclosed array", "", 0, pos);
            }
            pos++; // Skip closing bracket
            value = array;
        } else if (content[pos] == '{') {
            // Object/dictionary - convert to map<string, ConfigValue>
            pos++;
            std::unordered_map<std::string, ConfigValue> dict;
            while (pos < content.size() && content[pos] != '}') {
                skipWhitespace(content, pos);
                if (content[pos] == '}') break;
                
                // Parse key
                skipWhitespace(content, pos);
                if (content[pos] != '"') {
                    throw FileParseException("Expected string key in object", "", 0, pos);
                }
                pos++;
                size_t keyStart = pos;
                while (pos < content.size() && content[pos] != '"') {
                    if (content[pos] == '\\') pos++;
                    pos++;
                }
                std::string key = content.substr(keyStart, pos - keyStart);
                pos++; // Skip closing quote
                
                skipWhitespace(content, pos);
                if (pos >= content.size() || content[pos] != ':') {
                    throw FileParseException("Expected ':' after key in object", "", 0, pos);
                }
                pos++; // Skip ':'
                skipWhitespace(content, pos);
                
                // Parse value
                ConfigValue objValue;
                parseValueSimple(content, pos, objValue);
                dict[key] = objValue;
                
                skipWhitespace(content, pos);
                if (pos < content.size() && content[pos] == ',') pos++;
            }
            if (pos >= content.size() || content[pos] != '}') {
                throw FileParseException("Unclosed object", "", 0, pos);
            }
            pos++; // Skip closing brace
            value = dict;
        } else {
            // String (unquoted)
            size_t start = pos;
            while (pos < content.size() && content[pos] != ' ' && content[pos] != '\n' && content[pos] != '=' && content[pos] != ',' && content[pos] != ']') {
                pos++;
            }
            value = content.substr(start, pos - start);
        }
    }
    
    static void parseValueYAML(const std::string& content, size_t& pos, ConfigValue& value) {
        skipWhitespace(content, pos);
        if (pos >= content.size()) {
            throw FileParseException("Unexpected end of input in YAML", "", 0, pos);
        }
        
        if (content[pos] == '"') {
            // Quoted string
            pos++;
            size_t start = pos;
            while (pos < content.size() && content[pos] != '"') {
                if (content[pos] == '\\') pos++;
                pos++;
            }
            value = content.substr(start, pos - start);
            pos++; // Skip closing quote
        } else if (content[pos] == 't' || content[pos] == 'f') {
            // Boolean
            if (content.substr(pos, 4) == "true") {
                value = true;
                pos += 4;
            } else if (content.substr(pos, 5) == "false") {
                value = false;
                pos += 5;
            } else {
                throw FileParseException("Expected boolean value in YAML", "", 0, pos);
            }
        } else if (content[pos] == '-' || std::isdigit(content[pos]) || content[pos] == '.') {
            // Number
            size_t start = pos;
            while (pos < content.size() && (std::isdigit(content[pos]) || content[pos] == '.' || content[pos] == '-' || content[pos] == 'e' || content[pos] == 'E')) {
                pos++;
            }
            std::string numStr = content.substr(start, pos - start);
            try {
                size_t decPos = numStr.find('.');
                if (decPos != std::string::npos && decPos < numStr.size() - 1) {
                    value = std::stod(numStr);
                } else if (numStr.find('e') != std::string::npos || numStr.find('E') != std::string::npos) {
                    value = std::stod(numStr);
                } else {
                    value = std::stoll(numStr);
                }
            } catch (...) {
                throw FileParseException("Invalid number format: " + numStr, "", 0, start);
            }
        } else if (content[pos] == '[') {
            // Array
            pos++;
            std::vector<ConfigValue> array;
            while (pos < content.size() && content[pos] != ']') {
                skipWhitespace(content, pos);
                if (content[pos] == ']') break;
                
                ConfigValue arrayItem;
                parseValueYAML(content, pos, arrayItem);
                array.push_back(arrayItem);
                
                skipWhitespace(content, pos);
                if (pos < content.size() && content[pos] == ',') pos++;
            }
            if (pos >= content.size() || content[pos] != ']') {
                throw FileParseException("Unclosed array in YAML", "", 0, pos);
            }
            pos++; // Skip closing bracket
            value = array;
        } else if (content[pos] == '{') {
            // Object - simplified
            pos++;
            std::unordered_map<std::string, ConfigValue> dict;
            while (pos < content.size() && content[pos] != '}') {
                skipWhitespace(content, pos);
                if (content[pos] == '}') break;
                
                // Parse key
                skipWhitespace(content, pos);
                if (content[pos] != '"') {
                    throw FileParseException("Expected string key in YAML object", "", 0, pos);
                }
                pos++;
                size_t keyStart = pos;
                while (pos < content.size() && content[pos] != '"') {
                    if (content[pos] == '\\') pos++;
                    pos++;
                }
                std::string key = content.substr(keyStart, pos - keyStart);
                pos++; // Skip closing quote
                
                skipWhitespace(content, pos);
                if (pos >= content.size() || content[pos] != ':') {
                    throw FileParseException("Expected ':' after key in YAML object", "", 0, pos);
                }
                pos++; // Skip ':'
                skipWhitespace(content, pos);
                
                // Parse value
                ConfigValue objValue;
                parseValueYAML(content, pos, objValue);
                dict[key] = objValue;
                
                skipWhitespace(content, pos);
                if (pos < content.size() && (content[pos] == ',' || content[pos] == '\n')) {
                    pos++;
                }
            }
            if (pos >= content.size() || content[pos] != '}') {
                throw FileParseException("Unclosed object in YAML", "", 0, pos);
            }
            pos++; // Skip closing brace
            value = dict;
        } else {
            // Unquoted string
            size_t start = pos;
            while (pos < content.size() && content[pos] != ' ' && content[pos] != '\n' && content[pos] != ',' && content[pos] != ']') {
                pos++;
            }
            value = content.substr(start, pos - start);
        }
    }
    
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }
}; // End of FileParser

// Configuration schema definition with advanced features
class ConfigSchema {
public:
    struct SchemaEntry {
        std::string name;
        ConfigType type;
        std::string description;
        std::any defaultValue;
        bool required = false;
        std::optional<Range> range;
        std::vector<ValidationCallback> validators;
        std::string group;
        int precedence = 0;
        bool inherited = false;
        
        SchemaEntry(const std::string& name, ConfigType type, const std::string& desc = "")
            : name(name), type(type), description(desc), required(false), precedence(0), inherited(false) {}
    };
    
    struct Group {
        std::string name;
        std::string description;
        std::vector<std::string> parameterNames;
        int precedence = 0;
        bool inherited = false;
        
        Group(const std::string& name, const std::string& desc = "")
            : name(name), description(desc), precedence(0), inherited(false) {}
    };
    
public:
    ConfigSchema() = default;
    
    // Parameter management
    void addParameter(const ConfigParameter& param) {
        SchemaEntry entry(param.name, param.type, param.description);
        entry.required = param.required;
        entry.range = param.range;
        entry.validators = param.validators;
        entry.defaultValue = param.defaultValue;
        
        parameters_[param.name] = entry;
        if (!entry.group.empty()) {
            addToGroup(entry.group, param.name);
        }
    }
    
    void addGroup(const ConfigGroup& group) {
        Group grp(group.name, group.description);
        grp.parameterNames = group.parameterNames;
        
        groups_[group.name] = grp;
    }
    
    void addValidationRule(const std::string& key, const ValidationCallback& validator) {
        customValidators_[key].push_back(validator);
        
        if (parameters_.find(key) != parameters_.end()) {
            parameters_[key].validators.push_back(validator);
        }
    }
    
    // Validation
    bool validate(const ConfigValue& value, const std::string& key) const {
        const SchemaEntry* entry = getParameter(key);
        if (!entry) return true; // No schema for this parameter
        
        std::vector<ValidationError> errors;
        bool isValid = true;
        
        // Type validation
        if (!ConfigValidator::validateType(value, entry->type)) {
            errors.push_back(ConfigValidator::createTypeError(key, entry->type, 
                getConfigType(value)));
            isValid = false;
        }
        
        // Range validation
        if (entry->range && !ConfigValidator::validateRange(value, *entry->range)) {
            double numVal = extractNumber(value);
            errors.push_back(ConfigValidator::createRangeError(key, *entry->range, numVal));
            isValid = false;
        }
        
        // Required validation
        if (entry->required) {
            if (isNullValue(value)) {
                errors.push_back(ConfigValidator::createMissingError(key));
                isValid = false;
            }
        }
        
        // Custom validation
        auto it = customValidators_.find(key);
        if (it != customValidators_.end()) {
            for (const auto& validator : it->second) {
                if (!validator(value, key)) {
                    errors.push_back(ValidationError(ValidationError::ErrorCode::CustomValidation,
                        "Custom validation failed for key: " + key, key));
                    isValid = false;
                }
            }
        }
        
        // Apply entry-level validators
        for (const auto& validator : entry->validators) {
            if (!validator(value, key)) {
                errors.push_back(ValidationError(ValidationError::ErrorCode::CustomValidation,
                    "Entry validator failed for key: " + key, key));
                isValid = false;
            }
        }
        
        // Store errors for later retrieval
        if (!isValid) {
            schemaErrors_[key] = errors;
        } else {
            schemaErrors_.erase(key);
        }
        
        return isValid;
    }
    
    const std::vector<ValidationError> validateAll(const std::unordered_map<std::string, ConfigValue>& values) const {
        std::vector<ValidationError> allErrors;
        
        for (const auto& pair : values) {
            const SchemaEntry* entry = getParameter(pair.first);
            if (!entry) continue;
            
            // Type validation
            if (!ConfigValidator::validateType(pair.second, entry->type)) {
                allErrors.push_back(ConfigValidator::createTypeError(pair.first, entry->type,
                    getConfigType(pair.second)));
            }
            
            // Range validation
            if (entry->range && !ConfigValidator::validateRange(pair.second, *entry->range)) {
                double numVal = extractNumber(pair.second);
                allErrors.push_back(ConfigValidator::createRangeError(pair.first, *entry->range, numVal));
            }
            
            // Required validation
            if (entry->required && isNullValue(pair.second)) {
                allErrors.push_back(ConfigValidator::createMissingError(pair.first));
            }
            
            // Custom validation
            auto it = customValidators_.find(pair.first);
            if (it != customValidators_.end()) {
                for (const auto& validator : it->second) {
                    if (!validator(pair.second, pair.first)) {
                        allErrors.push_back(ValidationError(ValidationError::ErrorCode::CustomValidation,
                            "Custom validation failed for key: " + pair.first, pair.first));
                    }
                }
            }
        }
        
        return allErrors;
    }
    
    // Parameter retrieval
    ConfigParameter* getParameter(const std::string& key) {
        auto it = parameters_.find(key);
        if (it != parameters_.end()) {
            return &convertSchemaEntryToParameter(it->second);
        }
        return nullptr;
    }
    
    const ConfigParameter* getParameter(const std::string& key) const {
        auto it = parameters_.find(key);
        if (it != parameters_.end()) {
            return &convertSchemaEntryToParameter(it->second);
        }
        return nullptr;
    }
    
    const ConfigGroup* getGroup(const std::string& groupName) const {
        auto it = groups_.find(groupName);
        if (it != groups_.end()) {
            return &convertGroupToConfigGroup(it->second);
        }
        return nullptr;
    }
    
    std::vector<std::string> getAllParameterKeys() const {
        std::vector<std::string> keys;
        for (const auto& pair : parameters_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    std::vector<std::string> getGroups() const {
        std::vector<std::string> groupNames;
        for (const auto& pair : groups_) {
            groupNames.push_back(pair.first);
        }
        return groupNames;
    }
    
    // Schema management
    void clear() {
        parameters_.clear();
        groups_.clear();
        customValidators_.clear();
        schemaErrors_.clear();
    }
    
    // Get validation errors for a specific key
    const std::vector<ValidationError> getValidationErrors(const std::string& key) const {
        auto it = schemaErrors_.find(key);
        if (it != schemaErrors_.end()) {
            return it->second;
        }
        return std::vector<ValidationError>();
    }
    
    // Get all validation errors
    std::vector<ValidationError> getAllValidationErrors() const {
        std::vector<ValidationError> allErrors;
        for (const auto& pair : schemaErrors_) {
            allErrors.insert(allErrors.end(), pair.second.begin(), pair.second.end());
        }
        return allErrors;
    }
    
    // Set precedence for parameters and groups
    void setPrecedence(const std::string& key, int precedence) {
        if (parameters_.find(key) != parameters_.end()) {
            parameters_[key].precedence = precedence;
        }
    }
    
    void setInherited(const std::string& key, bool inherited) {
        if (parameters_.find(key) != parameters_.end()) {
            parameters_[key].inherited = inherited;
        }
    }
    
    void setGroupPrecedence(const std::string& groupName, int precedence) {
        if (groups_.find(groupName) != groups_.end()) {
            groups_[groupName].precedence = precedence;
        }
    }
    
    void setGroupInherited(const std::string& groupName, bool inherited) {
        if (groups_.find(groupName) != groups_.end()) {
            groups_[groupName].inherited = inherited;
        }
    }
    
    // Get schema statistics
    size_t getParameterCount() const { return parameters_.size(); }
    size_t getGroupCount() const { return groups_.size(); }
    
private:
    std::unordered_map<std::string, SchemaEntry> parameters_;
    std::unordered_map<std::string, Group> groups_;
    std::unordered_map<std::string, std::vector<ValidationCallback>> customValidators_;
    mutable std::unordered_map<std::string, std::vector<ValidationError>> schemaErrors_;
    
    void addToGroup(const std::string& groupName, const std::string& paramName) {
        auto it = groups_.find(groupName);
        if (it != groups_.end()) {
            it->second.parameterNames.push_back(paramName);
        }
    }
    
    ConfigParameter convertSchemaEntryToParameter(const SchemaEntry& entry) const {
        ConfigParameter param(entry.name, entry.type, entry.description);
        param.defaultValue = entry.defaultValue;
        param.required = entry.required;
        param.range = entry.range;
        param.validators = entry.validators;
        return param;
    }
    
    ConfigGroup convertGroupToConfigGroup(const Group& group) const {
        ConfigGroup cfgGroup(group.name, group.description);
        cfgGroup.parameterNames = group.parameterNames;
        return cfgGroup;
    }
    
    ConfigType getConfigType(const ConfigValue& value) const {
        return std::visit([](auto&& arg) -> ConfigType {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                return ConfigType::Integer;
            } else if constexpr (std::is_same_v<T, double>) {
                return ConfigType::Double;
            } else if constexpr (std::is_same_v<T, bool>) {
                return ConfigType::Boolean;
            } else if constexpr (std::is_same_v<T, std::string>) {
                return ConfigType::String;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                return ConfigType::IntArray;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                return ConfigType::DoubleArray;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                return ConfigType::StringArray;
            } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, ConfigValue>>) {
                return ConfigType::String; // Simplified
            }
            return ConfigType::String;
        }, value);
    }
    
    double extractNumber(const ConfigValue& value) const {
        double result = 0.0;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_arithmetic_v<T>) {
                result = static_cast<double>(arg);
            }
        }, value);
        return result;
    }
    
    bool isNullValue(const ConfigValue& value) const {
        return std::visit([](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return arg.empty();
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                return arg.empty();
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                return arg.empty();
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                return arg.empty();
            } else {
                return false; // Non-optional types are never null
            }
        }, value);
    }
}; // End of ConfigSchema

class ConfigValidator {
public:
    static bool validateType(const ConfigValue& value, ConfigType type) {
        return std::visit([type](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (type == ConfigType::Integer) {
                return std::is_same_v<T, int> || std::is_same_v<T, int64_t>;
            } else if constexpr (type == ConfigType::Double) {
                return std::is_same_v<T, double> || std::is_same_v<T, int> || std::is_same_v<T, int64_t>;
            } else if constexpr (type == ConfigType::Boolean) {
                return std::is_same_v<T, bool>;
            } else if constexpr (type == ConfigType::String) {
                return std::is_same_v<T, std::string> || std::is_same_v<T, std::unordered_map<std::string, ConfigValue>>;
            } else if constexpr (type == ConfigType::IntArray) {
                return std::is_same_v<T, std::vector<int>>;
            } else if constexpr (type == ConfigType::DoubleArray) {
                return std::is_same_v<T, std::vector<double>>;
            } else if constexpr (type == ConfigType::StringArray) {
                return std::is_same_v<T, std::vector<std::string>>;
            }
            return false;
        }, value);
    }
    
    static bool validateRange(const ConfigValue& value, const Range& range) {
        return std::visit([&](auto&& arg) {
            if constexpr (std::is_arithmetic_v<T>) {
                double val = static_cast<double>(arg);
                return range.contains(val);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                for (int val : arg) {
                    if (!range.contains(static_cast<double>(val))) return false;
                }
                return true;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                for (double val : arg) {
                    if (!range.contains(val)) return false;
                }
                return true;
            }
            return false;
        }, value);
    }
    
    static bool validateRequired(const bool isSet) {
        return isSet;
    }
    
    static ValidationError createTypeError(const std::string& key, ConfigType expected, ConfigType actual) {
        return ValidationError(ValidationError::ErrorCode::InvalidType,
            "Configuration type mismatch for key '" + key + "'. Expected " +
            std::to_string(static_cast<int>(expected)) + ", got " +
            std::to_string(static_cast<int>(actual)), key);
    }
    
    static ValidationError createRangeError(const std::string& key, const Range& range, double value) {
        return ValidationError(ValidationError::ErrorCode::OutOfRange,
            "Value " + std::to_string(value) + " for key '" + key + "' is out of range. "
            "Expected min: " + std::to_string(range.min) + ", max: " + std::to_string(range.max), key);
    }
    
    static ValidationError createMissingError(const std::string& key) {
        return ValidationError(ValidationError::ErrorCode::MissingRequired,
            "Required configuration key '" + key + "' is missing", key);
    }
    
    // Advanced validation methods
    static std::vector<ValidationError> validateAgainstSchema(
        const std::unordered_map<std::string, ConfigValue>& values,
        const ConfigSchema& schema) {
        std::vector<ValidationError> errors;
        
        for (const auto& pair : values) {
            const ConfigParameter* param = schema.getParameter(pair.first);
            if (!param) continue;
            
            if (!validateType(pair.second, param->type)) {
                errors.push_back(createTypeError(pair.first, param->type, 
                    getConfigTypeFromValue(pair.second)));
            }
            
            if (param->range && !validateRange(pair.second, *param->range)) {
                double numVal = extractNumber(pair.second);
                errors.push_back(createRangeError(pair.first, *param->range, numVal));
            }
            
            if (param->required && isNullValue(pair.second)) {
                errors.push_back(createMissingError(pair.first));
            }
        }
        
        return errors;
    }
    
    static bool runCustomValidators(
        const std::unordered_map<std::string, ConfigValue>& values,
        const std::unordered_map<std::string, std::vector<ValidationCallback>>& validators) {
        for (const auto& pair : values) {
            auto it = validators.find(pair.first);
            if (it != validators.end()) {
                for (const auto& validator : it->second) {
                    if (!validator(pair.second, pair.first)) {
                        return false;
                    }
                }
            }
        }
        return true;
    }
    
    static std::vector<ValidationError> batchValidate(
        const std::vector<std::pair<std::string, ConfigValue>>& entries,
        const ConfigSchema& schema) {
        std::vector<ValidationError> errors;
        
        for (const auto& pair : entries) {
            const ConfigParameter* param = schema.getParameter(pair.first);
            if (!param) continue;
            
            if (!validateType(pair.second, param->type)) {
                errors.push_back(createTypeError(pair.first, param->type,
                    getConfigTypeFromValue(pair.second)));
            }
        }
        
        return errors;
    }
    
    static std::future<std::vector<ValidationError>> asyncValidate(
        const std::vector<std::pair<std::string, ConfigValue>>& entries,
        const ConfigSchema& schema) {
        return std::async(std::launch::async, [&]() {
            return batchValidate(entries, schema);
        });
    }
    
    static std::vector<std::string> suggestCorrections(
        const std::string& invalidKey,
        const ConfigSchema& schema) {
        std::vector<std::string> suggestions;
        
        // Check for similar keys
        for (const auto& pair : schema.getAllParameterKeys()) {
            if (pair != invalidKey && similarStrings(pair, invalidKey)) {
                suggestions.push_back("Did you mean: '" + pair + "'?");
            }
        }
        
        // Check for missing keys
        if (schema.getParameter(invalidKey) == nullptr) {
            suggestions.push_back("Key does not exist in schema. Available keys:");
            for (const auto& key : schema.getAllParameterKeys()) {
                if (suggestions.size() < 5) {
                    suggestions.push_back("  - " + key);
                }
            }
        }
        
        return suggestions;
    }
    
    static bool isAlphanumeric(const std::string& str) {
        return std::all_of(str.begin(), str.end(), [](char c) {
            return std::isalnum(c) || c == '_' || c == '-';
        });
    }
    
private:
    static ConfigType getConfigTypeFromValue(const ConfigValue& value) {
        return std::visit([](auto&& arg) -> ConfigType {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                return ConfigType::Integer;
            } else if constexpr (std::is_same_v<T, double>) {
                return ConfigType::Double;
            } else if constexpr (std::is_same_v<T, bool>) {
                return ConfigType::Boolean;
            } else if constexpr (std::is_same_v<T, std::string>) {
                return ConfigType::String;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                return ConfigType::IntArray;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                return ConfigType::DoubleArray;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                return ConfigType::StringArray;
            }
            return ConfigType::String;
        }, value);
    }
    
    static double extractNumber(const ConfigValue& value) {
        double result = 0.0;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_arithmetic_v<T>) {
                result = static_cast<double>(arg);
            }
        }, value);
        return result;
    }
    
    static bool isNullValue(const ConfigValue& value) {
        return std::visit([](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return arg.empty();
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                return arg.empty();
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                return arg.empty();
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                return arg.empty();
            } else {
                return false;
            }
        }, value);
    }
    
    static bool similarStrings(const std::string& a, const std::string& b) {
        if (a == b) return false;
        
        // Simple similarity check based on Levenshtein distance
        size_t maxLen = std::max(a.size(), b.size());
        if (maxLen == 0) return true;
        
        int editDist = std::abs(static_cast<int>(a.size()) - static_cast<int>(b.size()));
        for (size_t i = 0; i < std::min(a.size(), b.size()); ++i) {
            if (a[i] != b[i]) editDist++;
        }
        
        return (static_cast<double>(editDist) / static_cast<double>(maxLen)) < 0.3;
    }
}; // End of ConfigValidator

Config::Config() : pImpl(std::make_unique<Impl>()), schema_(std::make_unique<ConfigSchema>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::setSchema(const ConfigSchema& schema) {
    schema_ = std::make_unique<ConfigSchema>(schema);
    // Clear old validation errors when schema changes
    pImpl->validationErrors.clear();
    pImpl->validationErrorsByKey.clear();
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        if (!std::filesystem::exists(filepath)) {
            throw ConfigOperationException("File does not exist: " + filepath);
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigOperationException("Cannot open file: " + filepath);
        }
        
        // Read entire file
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Detect format and parse
        FileParser::Format format = FileParser::detectFormat(content);
        std::unordered_map<std::string, ConfigValue> parsed;
        
        switch (format) {
            case FileParser::Format::JSON:
                if (!FileParser::parseJSON(content, parsed)) {
                    throw FileParseException("Failed to parse JSON file", filepath, 0, 0);
                }
                break;
            case FileParser::Format::YAML:
                if (!FileParser::parseYAML(content, parsed)) {
                    throw FileParseException("Failed to parse YAML file", filepath, 0, 0);
                }
                break;
            case FileParser::Format::TOML:
                if (!FileParser::parseTOML(content, parsed)) {
                    throw FileParseException("Failed to parse TOML file", filepath, 0, 0);
                }
                break;
            case FileParser::Format::INI:
                if (!FileParser::parseINI(content, parsed)) {
                    throw FileParseException("Failed to parse INI file", filepath, 0, 0);
                }
                break;
        }
        
        // Clear existing entries
        clear();
        
        // Apply parsed values with File source
        pImpl->lastFilepath = filepath;
        for (const auto& pair : parsed) {
            set(pair.first, pair.second, ConfigSource::File);
        }
        
        return true;
    } catch (const ConfigException&) {
        throw;
    } catch (const std::exception& e) {
        throw FileParseException(std::string("Error loading config file: ") + e.what(), filepath, 0, 0);
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        if (argc < 1) {
            throw ConfigOperationException("Invalid argument count");
        }
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    ConfigValue configValue;
                    if (!parseValue(value, configValue)) {
                        throw ConfigOperationException("Failed to parse argument value: " + value);
                    }
                    
                    set(key, configValue, ConfigSource::CommandLine);
                } else {
                    throw ConfigOperationException("Invalid command line argument format: " + arg);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                ConfigValue configValue;
                if (!parseValue(value, configValue)) {
                    throw ConfigOperationException("Failed to parse argument value: " + value);
                }
                
                set(key, configValue, ConfigSource::CommandLine);
            } else {
                throw ConfigOperationException("Invalid command line argument: " + arg);
            }
        }
        
        return true;
    } catch (const ConfigException&) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigOperationException(std::string("Error loading from arguments: ") + e.what());
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        if (filepath.empty()) {
            throw ConfigOperationException("Invalid file path: empty");
        }
        
        // Create atomic write - write to temporary file first
        std::string tempPath = filepath + ".tmp";
        std::ofstream file(tempPath);
        if (!file.is_open()) {
            throw ConfigOperationException("Cannot open file for writing: " + filepath);
        }
        
        // Write configuration in JSON format (can be extended to support other formats)
        file << "{\n";
        
        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) file << ",\n";
            first = false;
            
            file << "  \"" << entry.key << "\": ";
            file << toJSONString(entry.value);
        }
        
        file << "\n}\n";
        
        file.close();
        
        // Atomic move to final destination
        if (std::filesystem::exists(filepath)) {
            std::filesystem::remove(filepath);
        }
        std::filesystem::rename(tempPath, filepath);
        
        return true;
    } catch (const ConfigException&) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigOperationException(std::string("Error saving config file: ") + e.what());
    }
}

// Enhanced parseValue with better error handling
bool Config::parseValue(const std::string& str, ConfigValue& value) {
    try {
        if (str.empty()) {
            value = std::string();
            return true;
        }
        
        // Try to parse as boolean
        if (str == "true") {
            value = true;
            return true;
        } else if (str == "false") {
            value = false;
            return true;
        }
        
        // Try to parse as integer
        try {
            size_t pos;
            long long intVal = std::stoll(str, &pos);
            if (pos == str.size()) {
                if (intVal >= std::numeric_limits<int>::min() && intVal <= std::numeric_limits<int>::max()) {
                    value = static_cast<int>(intVal);
                    return true;
                } else {
                    value = intVal;
                    return true;
                }
            }
        } catch (...) {}
        
        // Try to parse as double
        try {
            size_t pos;
            double doubleVal = std::stod(str, &pos);
            if (pos == str.size()) {
                value = doubleVal;
                return true;
            }
        } catch (...) {}
        
        // Check for array format
        if (str.front() == '[' && str.back() == ']') {
            std::vector<ConfigValue> mixedArray;
            
            std::string token;
            std::stringstream ss(str.substr(1, str.size() - 2));
            while (std::getline(ss, token, ',')) {
                token = trim(token);
                if (token.empty()) continue;
                
                ConfigValue arrayItem;
                if (!parseValueSimple(token, arrayItem)) {
                    throw ConfigOperationException("Failed to parse array element: " + token);
                }
                mixedArray.push_back(arrayItem);
            }
            
            if (!mixedArray.empty()) {
                value = mixedArray;
                return true;
            }
        }
        
        // Check for object/map format (JSON-like)
        if (str.front() == '{' && str.back() == '}') {
            std::unordered_map<std::string, ConfigValue> object;
            std::string inner = str.substr(1, str.size() - 2);
            
            size_t pos = 0;
            while (pos < inner.size()) {
                skipWhitespace(inner, pos);
                if (pos >= inner.size() || inner[pos] == '}') break;
                
                // Parse key
                if (inner[pos] != '"') {
                    throw ConfigOperationException("Expected quoted key in object");
                }
                pos++;
                size_t keyStart = pos;
                while (pos < inner.size() && inner[pos] != '"') {
                    if (inner[pos] == '\\') pos++;
                    pos++;
                }
                if (pos >= inner.size()) {
                    throw ConfigOperationException("Unclosed quoted key");
                }
                std::string key = inner.substr(keyStart, pos - keyStart);
                pos++; // Skip closing quote
                
                skipWhitespace(inner, pos);
                if (pos >= inner.size() || inner[pos] != ':') {
                    throw ConfigOperationException("Expected ':' after key");
                }
                pos++; // Skip ':'
                skipWhitespace(inner, pos);
                
                // Parse value
                ConfigValue objValue;
                if (!parseValueSimple(inner, pos, objValue)) {
                    throw ConfigOperationException("Failed to parse object value");
                }
                
                object[key] = objValue;
                
                skipWhitespace(inner, pos);
                if (pos < inner.size() && inner[pos] == ',') pos++;
            }
            
            if (!object.empty()) {
                value = object;
                return true;
            }
        }
        
        // Default to string
        value = str;
        return true;
    } catch (const ConfigException&) {
        throw;
    } catch (...) {
        return false;
    }
}

bool Config::parseValueSimple(const std::string& str, ConfigValue& value) {
    try {
        if (str.empty()) {
            value = std::string();
            return true;
        }
        
        // Try to parse as boolean
        if (str == "true") {
            value = true;
            return true;
        } else if (str == "false") {
            value = false;
            return true;
        }
        
        // Try to parse as integer
        try {
            size_t pos;
            long long intVal = std::stoll(str, &pos);
            if (pos == str.size()) {
                if (intVal >= std::numeric_limits<int>::min() && intVal <= std::numeric_limits<int>::max()) {
                    value = static_cast<int>(intVal);
                    return true;
                } else {
                    value = intVal;
                    return true;
                }
            }
        } catch (...) {}
        
        // Try to parse as double
        try {
            size_t pos;
            double doubleVal = std::stod(str, &pos);
            if (pos == str.size()) {
                value = doubleVal;
                return true;
            }
        } catch (...) {}
        
        // Check for array format
        if (str.front() == '[' && str.back() == ']') {
            std::vector<ConfigValue> array;
            std::string inner = str.substr(1, str.size() - 2);
            
            size_t pos = 0;
            while (pos < inner.size()) {
                skipWhitespace(inner, pos);
                if (pos >= inner.size() || inner[pos] == ']') break;
                
                ConfigValue arrayItem;
                if (!parseValueSimple(inner, pos, arrayItem)) {
                    return false;
                }
                array.push_back(arrayItem);
                
                skipWhitespace(inner, pos);
                if (pos < inner.size() && inner[pos] == ',') pos++;
            }
            
            if (!array.empty()) {
                value = array;
                return true;
            }
        }
        
        // Default to string
        value = str;
        return true;
    } catch (...) {
        return false;
    }
}

// Helper functions for parsing
std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

void Config::skipWhitespace(const std::string& content, size_t& pos) {
    while (pos < content.size() && std::isspace(content[pos])) pos++;
}

std::string Config::toJSONString(const ConfigValue& value) const {
    std::string result;
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            // Escape special characters in JSON string
            std::string escaped = arg;
            replaceInPlace(escaped, "\\", "\\\\");
            replaceInPlace(escaped, "\"", "\\\"");
            replaceInPlace(escaped, "\b", "\\b");
            replaceInPlace(escaped, "\f", "\\f");
            replaceInPlace(escaped, "\n", "\\n");
            replaceInPlace(escaped, "\r", "\\r");
            replaceInPlace(escaped, "\t", "\\t");
            
            result = "\"" + escaped + "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                std::string escaped = arg[i];
                replaceInPlace(escaped, "\\", "\\\\");
                replaceInPlace(escaped, "\"", "\\\"");
                replaceInPlace(escaped, "\b", "\\b");
                replaceInPlace(escaped, "\f", "\\f");
                replaceInPlace(escaped, "\n", "\\n");
                replaceInPlace(escaped, "\r", "\\r");
                replaceInPlace(escaped, "\t", "\\t");
                result += "\"" + escaped + "\"";
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, ConfigValue>>) {
            result = "{";
            bool first = true;
            for (const auto& pair : arg) {
                if (!first) result += ", ";
                first = false;
                
                result += "\"" + pair.first + "\": ";
                result += toJSONString(pair.second);
            }
            result += "}";
        } else {
            result = std::to_string(arg);
        }
    }, value);
    return result;
}

void Config::replaceInPlace(std::string& str, const std::string& from, const std::string& to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
}

// Enhanced set method with validation and error handling
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    try {
        // Validate if schema is set
        if (schema_) {
            const ConfigParameter* param = schema_->getParameter(key);
            if (param) {
                if (!schema_->validate(value, key)) {
                    // Collect validation errors
                    std::vector<ValidationError> errors = schema_->getValidationErrors(key);
                    
                    // Add to entry validation errors
                    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                        [&key](const ConfigEntry& e) { return e.key == key; });
                    
                    if (entryIt != pImpl->entries.end()) {
                        entryIt->validationErrors.insert(entryIt->validationErrors.end(),
                            errors.begin(), errors.end());
                    } else {
                        pImpl->validationErrors[key].insert(pImpl->validationErrors[key].end(),
                            errors.begin(), errors.end());
                    }
                    
                    // Store pending changes for atomic rollback
                    pImpl->pendingChanges.push_back(ConfigEntry(key, value, source));
                    pImpl->isDirty = true;
                    
                    throw ValidationException("Configuration validation failed for key: " + key);
                }
            }
        }
        
        // Clear pending changes if validation passes
        pImpl->pendingChanges.clear();
        pImpl->isDirty = true;
        
        // Set the value
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
            it->validationErrors.clear(); // Clear validation errors on successful set
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
        
        // Mark as dirty for persistence
        pImpl->isDirty = true;
    } catch (const ConfigException&) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigOperationException(std::string("Error setting configuration key '") + 
            key + "': " + e.what());
    }
}

// Other set overloads
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue configValue(value);
    set(key, configValue, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    ConfigValue configValue(value);
    set(key, configValue, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    ConfigValue configValue(value);
    set(key, configValue, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    ConfigValue configValue(value);
    set(key, configValue, source);
}

// Enhanced get methods with validation
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

// Other get methods
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Enhanced validation
bool Config::validate() const {
    bool isValid = true;
    
    for (const auto& entry : pImpl->entries) {
        if (schema_) {
            const ConfigParameter* param = schema_->getParameter(entry.key);
            if (param) {
                if (!schema_->validate(entry.value, entry.key)) {
                    isValid = false;
                }
            }
        }
    }
    
    return isValid;
}

std::vector<ValidationError> Config::getValidationErrors() const {
    std::vector<ValidationError> errors;
    
    // Add schema validation errors
    if (schema_) {
        auto schemaErrors = schema_->getAllValidationErrors();
        errors.insert(errors.end(), schemaErrors.begin(), schemaErrors.end());
    }
    
    // Add entry validation errors
    for (const auto& entry : pImpl->entries) {
        if (!entry.validationErrors.empty()) {
            errors.insert(errors.end(), entry.validationErrors.begin(), entry.validationErrors.end());
        }
    }
    
    // Add validation errors by key
    for (const auto& keyErrors : pImpl->validationErrors) {
        errors.insert(errors.end(), keyErrors.second.begin(), keyErrors.second.end());
    }
    
    return errors;
}

// Enhanced configuration management
void Config::addDefaults() {
    if (schema_) {
        for (const auto& param : schema_->parameters) {
            if (param.second.defaultValue.has_value()) {
                std::string key = param.second.name;
                ConfigSource source = ConfigSource::Default;
                
                if (param.second.type == ConfigType::Integer) {
                    int val = std::any_cast<int>(param.second.defaultValue);
                    set(key, val, source);
                } else if (param.second.type == ConfigType::Double) {
                    double val = std::any_cast<double>(param.second.defaultValue);
                    set(key, val, source);
                } else if (param.second.type == ConfigType::String) {
                    std::string val = std::any_cast<std::string>(param.second.defaultValue);
                    set(key, val, source);
                } else if (param.second.type == ConfigType::Boolean) {
                    bool val = std::any_cast<bool>(param.second.defaultValue);
                    set(key, val, source);
                }
            }
        }
    }
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, entry.source);
    }
}

void Config::applyInheritance(const Config& baseConfig) {
    for (const auto& entry : baseConfig.pImpl->entries) {
        if (!has(entry.key)) {
            set(entry.key, entry.value, ConfigSource::Inherited);
        }
    }
}

// Other public methods (unchanged but included for completeness)
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
    pImpl->isDirty = true;
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeysInGroup(const std::string& groupName) const {
    std::vector<std::string> keys;
    if (schema_) {
        const ConfigGroup* group = schema_->getGroup(groupName);
        if (group) {
            keys = group->parameterNames;
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->validationErrors.clear();
    pImpl->validationErrorsByKey.clear();
    pImpl->pendingChanges.clear();
    pImpl->isDirty = false;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    
    // Show grouped configuration
    if (!pImpl->nestedEntries.empty()) {
        oss << "Nested Configuration:\n";
        for (const auto& section : pImpl->nestedEntries) {
            oss << "  [" << section.first << "]\n";
            for (const auto& entry : section.second) {
                oss << "    " << entry.first << " = [";
                std::visit([&oss](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        oss << "\"" << arg << "\"";
                    } else {
                        oss << arg;
                    }
                }, entry.second);
                oss << "]\n";
            }
        }
    }
    
    // Show global configuration
    if (!pImpl->entries.empty()) {
        oss << "Global Configuration:\n";
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
            oss << "] (Source: " << static_cast<int>(entry.source) << ")\n";
        }
    }
    
    return oss.str();
}

// ConfigSnapshot struct (added for snapshot functionality)
struct Config::ConfigSnapshot {
    std::string name;
    std::string timestamp;
    std::vector<ConfigEntry> entries;
};

// Snapshot methods
void Config::saveSnapshot(const std::string& name) {
    ConfigSnapshot snapshot(name);
    snapshot.timestamp = std::to_string(std::time(nullptr));
    snapshot.entries = pImpl->entries;
    pImpl->snapshots.push_back(snapshot);
}

bool Config::restoreSnapshot(const std::string& name) {
    for (const auto& snapshot : pImpl->snapshots) {
        if (snapshot.name == name) {
            pImpl->entries = snapshot.entries;
            return true;
        }
    }
    return false;
}

// Additional utility methods
std::vector<std::string> Config::getDirtyKeys() const {
    std::vector<std::string> dirtyKeys;
    for (const auto& entry : pImpl->entries) {
        dirtyKeys.push_back(entry.key);
    }
    return dirtyKeys;
}

bool Config::isDirty() const {
    return pImpl->isDirty;
}

void Config::setDirty(bool dirty) {
    pImpl->isDirty = dirty;
}

const std::string& Config::getLastFilepath() const {
    return pImpl->lastFilepath;
}

// ConfigSchema::SchemaEntry helper methods
std::string ConfigSchema::getParameterGroup(const std::string& key) const {
    auto it = parameters_.find(key);
    if (it != parameters_.end()) {
        return it->second.group;
    }
    return "";
}

int ConfigSchema::getParameterPrecedence(const std::string& key) const {
    auto it = parameters_.find(key);
    if (it != parameters_.end()) {
        return it->second.precedence;
    }
    return 0;
}

bool ConfigSchema::isParameterInherited(const std::string& key) const {
    auto it = parameters_.find(key);
    if (it != parameters_.end()) {
        return it->second.inherited;
    }
    return false;
}

int ConfigSchema::getGroupPrecedence(const std::string& groupName) const {
    auto it = groups_.find(groupName);
    if (it != groups_.end()) {
        return it->second.precedence;
    }
    return 0;
}

bool ConfigSchema::isGroupInherited(const std::string& groupName) const {
    auto it = groups_.find(groupName);
    if (it != groups_.end()) {
        return it->second.inherited;
    }
    return false;
}

// Batch validation methods
std::vector<ValidationError> ConfigValidator::validateBatch(
    const std::vector<std::pair<std::string, ConfigValue>>& entries,
    const ConfigSchema& schema) {
    std::vector<ValidationError> errors;
    
    for (const auto& pair : entries) {
        const ConfigParameter* param = schema.getParameter(pair.first);
        if (!param) continue;
        
        if (!validateType(pair.second, param->type)) {
            errors.push_back(createTypeError(pair.first, param->type,
                getConfigTypeFromValue(pair.second)));
        }
        
        if (param->range && !validateRange(pair.second, *param->range)) {
            double numVal = extractNumber(pair.second);
            errors.push_back(createRangeError(pair.first, *param->range, numVal));
        }
    }
    
    return errors;
}

std::future<std::vector<ValidationError>> ConfigValidator::asyncValidateBatch(
    const std::vector<std::pair<std::string, ConfigValue>>& entries,
    const ConfigSchema& schema) {
    return std::async(std::launch::async, [&]() {
        return validateBatch(entries, schema);
    });
}

// ConfigValidator helper methods
ConfigType ConfigValidator::getConfigTypeFromValue(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> ConfigType {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return ConfigType::Integer;
        } else if constexpr (std::is_same_v<T, double>) {
            return ConfigType::Double;
        } else if constexpr (std::is_same_v<T, bool>) {
            return ConfigType::Boolean;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return ConfigType::String;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return ConfigType::IntArray;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return ConfigType::DoubleArray;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return ConfigType::StringArray;
        }
        return ConfigType::String;
    }, value);
}

double ConfigValidator::extractNumber(const ConfigValue& value) {
    double result = 0.0;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_arithmetic_v<T>) {
            result = static_cast<double>(arg);
        }
    }, value);
    return result;
}

bool ConfigValidator::isNullValue(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg.empty();
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return arg.empty();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return arg.empty();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return arg.empty();
        } else {
            return false;
        }
    }, value);
}

bool ConfigValidator::similarStrings(const std::string& a, const std::string& b) {
    if (a == b) return false;
    
    // Simple similarity check based on Levenshtein distance
    size_t maxLen = std::max(a.size(), b.size());
    if (maxLen == 0) return true;
    
    int editDist = std::abs(static_cast<int>(a.size()) - static_cast<int>(b.size()));
    for (size_t i = 0; i < std::min(a.size(), b.size()); ++i) {
        if (a[i] != b[i]) editDist++;
    }
    
    return (static_cast<double>(editDist) / static_cast<double>(maxLen)) < 0.3;
}

} // namespace nlm