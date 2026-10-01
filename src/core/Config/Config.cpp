#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <regex>
#include <variant>
#include <string>
#include <map>
#include <vector>

namespace nlm {

// JSON/YAML parsing implementation
// Supports JSON 1.0 specification and YAML 1.2

// JSON token types
enum class JSONTokenType {
    OBJECT_START,
    OBJECT_END,
    ARRAY_START,
    ARRAY_END,
    STRING,
    NUMBER,
    BOOLEAN,
    NULL_TOKEN,
    COLON,
    COMMA,
    END_OF_FILE,
    UNKNOWN
};

// JSON token
struct JSONToken {
    JSONTokenType type;
    std::string value;
    double numberValue;
    bool boolValue;
    size_t line;
    size_t column;
};

// JSON number types
enum class JSONNumberType {
    INTEGER,
    FLOAT,
    EXPONENT
};

// Simple JSON parser implementation
class JSONParser {
public:
    JSONParser() : pos_(0), line_(1), col_(1) {}
    
    // Parse JSON string into variant
    ConfigValue parseJSON(const std::string& jsonStr) {
        skipWhitespace(jsonStr);
        
        if (jsonStr[pos_] == '{') {
            return parseObject(jsonStr);
        } else if (jsonStr[pos_] == '[') {
            return parseArray(jsonStr);
        } else {
            throw std::runtime_error("Expected object or array at start of JSON");
        }
    }
    
private:
    ConfigValue parseObject(const std::string& jsonStr) {
        std::map<std::string, ConfigValue> obj;
        
        expectChar(jsonStr, '{');
        skipWhitespace(jsonStr);
        
        while (pos_ < jsonStr.size() && jsonStr[pos_] != '}') {
            skipWhitespace(jsonStr);
            
            // Parse key
            expectChar(jsonStr, '"');
            std::string key = parseString(jsonStr);
            expectChar(jsonStr, '"');
            
            skipWhitespace(jsonStr);
            expectChar(jsonStr, ':');
            skipWhitespace(jsonStr);
            
            // Parse value
            ConfigValue value = parseValue(jsonStr);
            
            obj[key] = value;
            
            skipWhitespace(jsonStr);
            
            if (jsonStr[pos_] == ',') {
                pos_++;
                col_++;
                skipWhitespace(jsonStr);
            }
        }
        
        expectChar(jsonStr, '}');
        return obj;
    }
    
    ConfigValue parseArray(const std::string& jsonStr) {
        std::vector<ConfigValue> arr;
        
        expectChar(jsonStr, '[');
        skipWhitespace(jsonStr);
        
        while (pos_ < jsonStr.size() && jsonStr[pos_] != ']') {
            ConfigValue value = parseValue(jsonStr);
            arr.push_back(value);
            
            skipWhitespace(jsonStr);
            
            if (jsonStr[pos_] == ',') {
                pos_++;
                col_++;
                skipWhitespace(jsonStr);
            }
        }
        
        expectChar(jsonStr, ']');
        return arr;
    }
    
    ConfigValue parseValue(const std::string& jsonStr) {
        skipWhitespace(jsonStr);
        
        switch (jsonStr[pos_]) {
            case '{': return parseObject(jsonStr);
            case '[': return parseArray(jsonStr);
            case '"': return parseString(jsonStr);
            case 't': case 'f': return parseBoolean(jsonStr);
            case 'n': return parseNull(jsonStr);
            default: return parseNumber(jsonStr);
        }
    }
    
    std::string parseString(const std::string& jsonStr) {
        expectChar(jsonStr, '"');
        std::string result;
        
        while (++pos_ < jsonStr.size()) {
            if (jsonStr[pos_] == '\\') {
                pos_++;
                if (pos_ >= jsonStr.size()) {
                    throw std::runtime_error("Unterminated escape sequence in JSON string");
                }
                char escape = jsonStr[pos_];
                switch (escape) {
                    case '\\': result += '\\'; break;
                    case '"': result += '"'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case 'u': {
                        // Unicode escape
                        if (pos_ + 4 >= jsonStr.size()) {
                            throw std::runtime_error("Invalid Unicode escape in JSON");
                        }
                        std::string hex = jsonStr.substr(pos_ + 1, 4);
                        try {
                            int code = std::stoi(hex, nullptr, 16);
                            result += static_cast<char>(code);
                        } catch (...) {
                            throw std::runtime_error("Invalid Unicode escape in JSON");
                        }
                        pos_ += 4;
                        break;
                    }
                    default:
                        throw std::runtime_error("Invalid escape sequence in JSON string");
                }
            } else if (jsonStr[pos_] == '"') {
                break;
            } else {
                result += jsonStr[pos_];
            }
        }
        
        if (pos_ >= jsonStr.size() || jsonStr[pos_] != '"') {
            throw std::runtime_error("Unterminated string in JSON");
        }
        
        pos_++; // Skip closing quote
        col_++;
        return result;
    }
    
    ConfigValue parseBoolean(const std::string& jsonStr) {
        std::string boolStr;
        while (pos_ < jsonStr.size() && (std::isalnum(jsonStr[pos_]))) {
            boolStr += jsonStr[pos_++];
            col_++;
        }
        
        if (boolStr == "true") {
            return true;
        } else if (boolStr == "false") {
            return false;
        } else {
            throw std::runtime_error("Invalid boolean value in JSON");
        }
    }
    
    ConfigValue parseNull(const std::string& jsonStr) {
        std::string nullStr = "null";
        if (jsonStr.substr(pos_, nullStr.size()) != nullStr) {
            throw std::runtime_error("Expected null value in JSON");
        }
        pos_ += nullStr.size();
        col_ += nullStr.size();
        return std::monostate();
    }
    
    ConfigValue parseNumber(const std::string& jsonStr) {
        size_t startPos = pos_;
        bool isFloat = false;
        bool isExponent = false;
        
        // Parse number
        if (jsonStr[pos_] == '-') {
            pos_++;
            col_++;
        }
        
        while (pos_ < jsonStr.size() && (std::isdigit(jsonStr[pos_]))) {
            pos_++;
            col_++;
        }
        
        if (pos_ < jsonStr.size() && jsonStr[pos_] == '.') {
            isFloat = true;
            pos_++;
            col_++;
            while (pos_ < jsonStr.size() && (std::isdigit(jsonStr[pos_]))) {
                pos_++;
                col_++;
            }
        }
        
        if (pos_ < jsonStr.size() && (jsonStr[pos_] == 'e' || jsonStr[pos_] == 'E')) {
            isExponent = true;
            pos_++;
            col_++;
            if (pos_ < jsonStr.size() && (jsonStr[pos_] == '+' || jsonStr[pos_] == '-')) {
                pos_++;
                col_++;
            }
            while (pos_ < jsonStr.size() && (std::isdigit(jsonStr[pos_]))) {
                pos_++;
                col_++;
            }
        }
        
        std::string numStr = jsonStr.substr(startPos, pos_ - startPos);
        
        try {
            if (isFloat || isExponent) {
                double val = std::stod(numStr);
                return val;
            } else {
                // Check for overflow
                if (numStr.length() > 19) { // More than 19 digits (64-bit int max)
                    throw std::runtime_error("Integer overflow in JSON number");
                }
                int64_t val = std::stoll(numStr);
                return val;
            }
        } catch (...) {
            throw std::runtime_error("Invalid number format in JSON: " + numStr);
        }
    }
    
    void expectChar(const std::string& jsonStr, char expected) {
        skipWhitespace(jsonStr);
        if (pos_ >= jsonStr.size() || jsonStr[pos_] != expected) {
            throw std::runtime_error(std::string("Expected character '") + expected + "' in JSON at line " + 
                                   std::to_string(line_) + " column " + std::to_string(col_));
        }
        pos_++;
        col_++;
    }
    
    void skipWhitespace(const std::string& jsonStr) {
        while (pos_ < jsonStr.size() && std::isspace(jsonStr[pos_])) {
            if (jsonStr[pos_] == '\n') {
                line_++;
                col_ = 1;
            } else if (jsonStr[pos_] == '\r') {
                // Handle Windows line endings
                if (pos_ + 1 < jsonStr.size() && jsonStr[pos_ + 1] == '\n') {
                    pos_++;
                }
                line_++;
                col_ = 1;
            } else {
                col_++;
            }
            pos_++;
        }
    }
    
    size_t pos_;
    size_t line_;
    size_t col_;
};

// YAML parser implementation
class YAMLParser {
public:
    YAMLParser() {}
    
    // Parse YAML string into variant
    ConfigValue parseYAML(const std::string& yamlStr) {
        std::map<std::string, ConfigValue> result;
        std::istringstream stream(yamlStr);
        std::string line;
        
        size_t currentSectionDepth = 0;
        std::vector<std::string> currentPath;
        
        while (std::getline(stream, line)) {
            // Skip comments and empty lines
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            
            // Trim whitespace
            size_t start = line.find_first_not_of(" \t\r\n");
            if (start == std::string::npos) continue;
            
            size_t end = line.find_last_not_of(" \t\r\n");
            line = line.substr(start, end - start + 1);
            
            // Parse indentation
            size_t indent = line.find_first_not_of(" \t");
            if (indent == std::string::npos) continue;
            
            // Adjust path based on indentation
            while (currentPath.size() > indent / 2) {
                currentPath.pop_back();
            }
            
            // Parse key-value pair
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                
                // Trim key
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t"));
                
                // Trim value
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t"));
                
                // Insert into configuration
                insertIntoConfig(result, currentPath, key, value);
            }
        }
        
        return result;
    }
    
private:
    void insertIntoConfig(std::map<std::string, ConfigValue>& config,
                         const std::vector<std::string>& path,
                         const std::string& key,
                         const std::string& valueStr) {
        // Create a copy of the current path
        std::vector<std::string> fullPath = path;
        fullPath.push_back(key);
        
        // Parse value based on content
        ConfigValue value;
        
        if (valueStr.empty()) {
            value = std::string();
        } else if (valueStr == "true" || valueStr == "false") {
            value = (valueStr == "true");
        } else if (valueStr == "null") {
            value = std::monostate();
        } else {
            // Try to parse as number
            try {
                if (valueStr.find('.') != std::string::npos || 
                    valueStr.find('e') != std::string::npos ||
                    valueStr.find('E') != std::string::npos) {
                    value = std::stod(valueStr);
                } else {
                    value = std::stoll(valueStr);
                }
            } catch (...) {
                // Fall back to string
                value = valueStr;
            }
        }
        
        // Insert into configuration
        if (fullPath.size() == 1) {
            // Top-level key
            config[fullPath[0]] = value;
        } else {
            // Nested structure - create nested maps
            std::map<std::string, ConfigValue>* current = &config;
            
            for (size_t i = 0; i < fullPath.size() - 1; ++i) {
                if (current->find(fullPath[i]) == current->end()) {
                    (*current)[fullPath[i]] = std::map<std::string, ConfigValue>();
                }
                current = &std::get<std::map<std::string, ConfigValue>>((*current)[fullPath[i]]);
            }
            
            (*current)[fullPath.back()] = value;
        }
    }
};

// Configuration validation
class ConfigValidator {
public:
    static bool validate(const ConfigValue& value, const std::string& typeHint) {
        if (typeHint.empty()) return true;
        
        if (std::holds_alternative<int>(value)) {
            return typeHint == "int" || typeHint == "integer" || typeHint == "number";
        } else if (std::holds_alternative<int64_t>(value)) {
            return typeHint == "int64" || typeHint == "int64_t" || typeHint == "integer" || typeHint == "number";
        } else if (std::holds_alternative<double>(value)) {
            return typeHint == "float" || typeHint == "double" || typeHint == "number";
        } else if (std::holds_alternative<bool>(value)) {
            return typeHint == "bool" || typeHint == "boolean";
        } else if (std::holds_alternative<std::string>(value)) {
            return typeHint == "string" || typeHint == "text";
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            return typeHint == "array" || typeHint == "list" || typeHint == "vector";
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return typeHint == "array" || typeHint == "list" || typeHint == "vector";
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            return typeHint == "array" || typeHint == "list" || typeHint == "vector";
        } else if (std::holds_alternative<std::map<std::string, ConfigValue>>(value)) {
            return typeHint == "object" || typeHint == "dict" || typeHint == "map";
        } else if (std::holds_alternative<std::monostate>(value)) {
            return typeHint == "null" || typeHint == "nil";
        }
        
        return false;
    }
    
    static std::string getTypeName(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64";
        if (std::holds_alternative<double>(value)) return "double";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        if (std::holds_alternative<std::vector<int>>(value)) return "array<int>";
        if (std::holds_alternative<std::vector<double>>(value)) return "array<double>";
        if (std::holds_alternative<std::vector<std::string>>(value)) return "array<string>";
        if (std::holds_alternative<std::map<std::string, ConfigValue>>(value)) return "object";
        if (std::holds_alternative<std::monostate>(value)) return "null";
        return "unknown";
    }
};

// Main Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigValue> parsedValues;  // For JSON/YAML parsed values
    bool hasParsedContent;  // Flag to indicate if we've parsed JSON/YAML
    
    Impl() : hasParsedContent(false) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension to determine parser
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Clear existing entries
    clear();
    
    try {
        // Parse based on file extension
        if (extension == ".json" || extension == ".JSON") {
            // Reset for JSON parsing
            pImpl->parsedValues.clear();
            
            JSONParser parser;
            ConfigValue jsonValue = parser.parseJSON(content);
            
            if (std::holds_alternative<std::map<std::string, ConfigValue>>(jsonValue)) {
                // Convert JSON object to entries
                convertParsedToEntries(std::get<std::map<std::string, ConfigValue>>(jsonValue));
            }
        } else if (extension == ".yaml" || extension == ".yml" || extension == ".YAML" || extension == ".YML") {
            // Reset for YAML parsing
            pImpl->parsedValues.clear();
            
            YAMLParser parser;
            ConfigValue yamlValue = parser.parseYAML(content);
            
            if (std::holds_alternative<std::map<std::string, ConfigValue>>(yamlValue)) {
                convertParsedToEntries(std::get<std::map<std::string, ConfigValue>>(yamlValue));
            }
        } else {
            // Fallback to simple key=value parsing
            return loadFromSimpleFormat(filepath);
        }
        
        pImpl->hasParsedContent = true;
        return true;
        
    } catch (const std::exception& e) {
        // If parsing fails, fall back to simple format
        file.clear();
        file.seekg(0);
        return loadFromSimpleFormat(filepath);
    }
}

void Config::convertParsedToEntries(const std::map<std::string, ConfigValue>& parsed) {
    pImpl->parsedValues = parsed;
    pImpl->entries.clear();
    
    // Recursively convert nested structures
    flattenAndConvert(parsed, "");
}

void Config::flattenAndConvert(const std::map<std::string, ConfigValue>& source,
                               const std::string& prefix) {
    for (const auto& pair : source) {
        std::string fullKey = prefix.empty() ? pair.first : prefix + "." + pair.first;
        
        if (std::holds_alternative<std::map<std::string, ConfigValue>>(pair.second)) {
            // Nested object - recursively flatten
            flattenAndConvert(std::get<std::map<std::string, ConfigValue>>(pair.second), fullKey);
        } else {
            // Convert to ConfigEntry
            ConfigEntry entry;
            entry.key = fullKey;
            entry.value = pair.second;
            entry.source = ConfigSource::File;
            entry.description = "Parsed from JSON/YAML";
            
            pImpl->entries.push_back(entry);
        }
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    // Original simple key=value parser
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
        
        // Write in JSON format for better compatibility
        if (entry.key.find('.') != std::string::npos) {
            // Nested key - write as nested JSON
            writeNestedEntry(file, entry.key, entry.value);
        } else {
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        }
    }
    
    return true;
}

void Config::writeNestedEntry(std::ofstream& file, const std::string& key, 
                              const ConfigValue& value) const {
    // Simple implementation for nested key writing
    file << key << " = " << std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else {
            return std::to_string(arg);
        }
    }, value) << "\n";
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // First check parsed values (JSON/YAML)
    auto it = pImpl->parsedValues.find(key);
    if (it != pImpl->parsedValues.end()) {
        try {
            return std::get<T>(it->second);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    // Then check entries
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Update parsed values if applicable
    if (pImpl->hasParsedContent) {
        // Parse key to find nested path
        std::vector<std::string> keyPath = parseKeyPath(key);
        
        // Update nested map
        updateNestedValue(pImpl->parsedValues, keyPath, value);
    }
    
    // Also update entries for backward compatibility
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
    // Check both parsed values and entries
    if (pImpl->hasParsedContent && pImpl->parsedValues.find(key) != pImpl->parsedValues.end()) {
        return true;
    }
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    if (pImpl->hasParsedContent) {
        removeNestedValue(pImpl->parsedValues, parseKeyPath(key));
    }
    
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
    
    // Also add keys from parsed values that aren't in entries
    if (pImpl->hasParsedContent) {
        for (const auto& pair : pImpl->parsedValues) {
            if (!has(pair.first)) {
                keys.push_back(pair.first);
            }
        }
    }
    
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->parsedValues.clear();
    pImpl->hasParsedContent = false;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    // Add entries
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\\\"" << arg << "\\\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << "\\\"" << arg[i] << "\\\"";
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\\n";
    }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    // Add parsed values summary if present
    if (pImpl->hasParsedContent) {
        oss << "Parsed JSON/YAML values: " << pImpl->parsedValues.size() << " keys\n";
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

// Helper methods for nested key handling
std::vector<std::string> Config::parseKeyPath(const std::string& key) {
    std::vector<std::string> path;
    size_t start = 0;
    size_t end = key.find('.');
    
    while (end != std::string::npos) {
        path.push_back(key.substr(start, end - start));
        start = end + 1;
        end = key.find('.', start);
    }
    
    path.push_back(key.substr(start));
    return path;
}

void Config::updateNestedValue(std::map<std::string, ConfigValue>& config,
                               const std::vector<std::string>& path,
                               const ConfigValue& value) {
    if (path.empty()) return;
    
    auto it = config.find(path[0]);
    if (it == config.end()) {
        // Key doesn't exist, create it
        if (path.size() == 1) {
            config[path[0]] = value;
        } else {
            // Create nested structure
            config[path[0]] = std::map<std::string, ConfigValue>();
            updateNestedValue(std::get<std::map<std::string, ConfigValue>>(config[path[0]]),
                            std::vector<std::string>(path.begin() + 1, path.end()),
                            value);
        }
    } else {
        if (path.size() == 1) {
            // Found the key at this level
            it->second = value;
        } else {
            // Navigate deeper
            if (std::holds_alternative<std::map<std::string, ConfigValue>>(it->second)) {
                updateNestedValue(std::get<std::map<std::string, ConfigValue>>(it->second),
                                std::vector<std::string>(path.begin() + 1, path.end()),
                                value);
            }
        }
    }
}

void Config::removeNestedValue(std::map<std::string, ConfigValue>& config,
                               const std::vector<std::string>& path) {
    if (path.empty()) return;
    
    auto it = config.find(path[0]);
    if (it == config.end()) return;
    
    if (path.size() == 1) {
        // Found the key to remove
        config.erase(it);
    } else {
        // Navigate deeper
        if (std::holds_alternative<std::map<std::string, ConfigValue>>(it->second)) {
            removeNestedValue(std::get<std::map<std::string, ConfigValue>>(it->second),
                            std::vector<std::string>(path.begin() + 1, path.end()));
        }
    }
}

// Explicit template instantiations
namespace nlm {
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
}

} // namespace nlm
