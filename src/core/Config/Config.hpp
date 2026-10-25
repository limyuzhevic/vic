#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cctype>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    PythonAPI
};

// JSON parsing error types
enum class JSONError {
    None,
    FileNotFound,
    InvalidJSON,
    MissingValue,
    TypeMismatch,
    UnknownKey
};

// Configuration validation rules
struct ValidationRule {
    std::string key;
    std::string type; // "int", "double", "string", "bool", "array"
    std::string description;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    bool required;
    std::string defaultValue;
    
    ValidationRule(const std::string& k, const std::string& t, const std::string& desc = "")
        : key(k), type(t), description(desc), required(false) {}
    
    ValidationRule(const std::string& k, const std::string& t, const std::string& defaultVal, 
                   const std::string& desc = "")
        : key(k), type(t), description(desc), required(true), defaultValue(defaultVal) {}
};

// JSON value structure for internal parsing
struct JSONValue {
    std::variant<int, int64_t, double, bool, std::string, std::vector<JSONValue>> value;
    
    JSONValue() = default;
    JSONValue(int val) : value(val) {}
    JSONValue(double val) : value(val) {}
    JSONValue(bool val) : value(val) {}
    JSONValue(const std::string& val) : value(val) {}
    JSONValue(const std::vector<JSONValue>& val) : value(val) {}
    
    bool isNumber() const { return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value) || std::holds_alternative<double>(value); }
    bool isString() const { return std::holds_alternative<std::string>(value); }
    bool isBool() const { return std::holds_alternative<bool>(value); }
    bool isArray() const { return std::holds_alternative<std::vector<JSONValue>>(value); }
    
    template<typename T>
    const T* get() const {
        if (std::holds_alternative<T>(value)) {
            return &std::get<T>(value);
        }
        return nullptr;
    }
};

// Simple JSON parser implementation
class SimpleJSONParser {
private:
    std::string filepath_;
    JSONError lastError_;
    std::string errorMessage_;
    
    // Skip whitespace and comments
    size_t skipWhitespaceAndComments(const std::string& str, size_t pos) {
        while (pos < str.size()) {
            char c = str[pos];
            if (std::isspace(static_cast<unsigned char>(c))) {
                pos++;
            } else if (c == '/' && pos + 1 < str.size() && str[pos + 1] == '/') {
                // Line comment
                pos = str.find('\n', pos + 2);
                if (pos == std::string::npos) pos = str.size();
            } else if (c == '/' && pos + 1 < str.size() && str[pos + 1] == '*') {
                // Block comment
                pos = str.find("*/", pos + 2);
                if (pos == std::string::npos) {
                    lastError_ = JSONError::InvalidJSON;
                    errorMessage_ = "Unterminated block comment";
                    return str.size();
                }
                pos += 2;
            } else {
                break;
            }
        }
        return pos;
    }
    
    // Parse string value
    std::string parseString(const std::string& str, size_t& pos) {
        if (str[pos] != '"') {
            lastError_ = JSONError::InvalidJSON;
            errorMessage_ = "Expected \" for string start";
            return "";
        }
        
        pos++;
        std::string result;
        
        while (pos < str.size()) {
            if (str[pos] == '\\') {
                pos++;
                if (pos >= str.size()) {
                    lastError_ = JSONError::InvalidJSON;
                    errorMessage_ = "Invalid escape sequence";
                    return result;
                }
                char escape = str[pos];
                switch (escape) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    default: result += escape; break;
                }
            } else if (str[pos] == '"') {
                pos++;
                return result;
            } else {
                result += str[pos];
            }
            pos++;
        }
        
        lastError_ = JSONError::InvalidJSON;
        errorMessage_ = "Unterminated string literal";
        return result;
    }
    
    // Parse number value
    JSONValue parseNumber(const std::string& str, size_t& pos) {
        size_t start = pos;
        bool isFloat = false;
        
        if (str[pos] == '-') {
            pos++;
        }
        
        while (pos < str.size() && (std::isdigit(static_cast<unsigned char>(str[pos])) || str[pos] == '.')) {
            if (str[pos] == '.') {
                isFloat = true;
            }
            pos++;
        }
        
        std::string numStr = str.substr(start, pos - start);
        
        try {
            if (isFloat) {
                double val = std::stod(numStr);
                return JSONValue(val);
            } else {
                // Try to determine if it's int64_t or int
                try {
                    int64_t val = std::stoll(numStr);
                    return JSONValue(val);
                } catch (...) {
                    int val = std::stoi(numStr);
                    return JSONValue(val);
                }
            }
        } catch (...) {
            lastError_ = JSONError::InvalidJSON;
            errorMessage_ = "Invalid number format: " + numStr;
            return JSONValue();
        }
    }
    
    // Parse array value
    std::vector<JSONValue> parseArray(const std::string& str, size_t& pos) {
        if (str[pos] != '[') {
            lastError_ = JSONError::InvalidJSON;
            errorMessage_ = "Expected [ for array start";
            return {};
        }
        
        pos++;
        std::vector<JSONValue> result;
        size_t arrayPos = skipWhitespaceAndComments(str, pos);
        
        if (arrayPos < str.size() && str[arrayPos] == ']') {
            pos = arrayPos + 1;
            return result;
        }
        
        while (arrayPos < str.size()) {
            JSONValue item;
            if (str[arrayPos] == '"') {
                item = parseString(str, arrayPos);
            } else if (str[arrayPos] == '[') {
                item = JSONValue(parseArray(str, arrayPos));
            } else if (std::isdigit(static_cast<unsigned char>(str[arrayPos])) || 
                       str[arrayPos] == '-' || str[arrayPos] == '.') {
                item = parseNumber(str, arrayPos);
            } else if (str[arrayPos] == 't' || str[arrayPos] == 'f') {
                // Parse boolean
                std::string boolStr;
                while (arrayPos < str.size() && (std::isalpha(static_cast<unsigned char>(str[arrayPos])))) {
                    boolStr += str[arrayPos];
                    arrayPos++;
                }
                if (boolStr == "true") {
                    item = JSONValue(true);
                } else if (boolStr == "false") {
                    item = JSONValue(false);
                } else {
                    lastError_ = JSONError::InvalidJSON;
                    errorMessage_ = "Invalid boolean value: " + boolStr;
                    return result;
                }
            } else {
                lastError_ = JSONError::InvalidJSON;
                errorMessage_ = "Unexpected character in array: " + std::string(1, str[arrayPos]);
                return result;
            }
            
            result.push_back(item);
            
            // Skip whitespace
            arrayPos = skipWhitespaceAndComments(str, arrayPos);
            
            if (arrayPos < str.size() && str[arrayPos] == ',') {
                arrayPos++;
                arrayPos = skipWhitespaceAndComments(str, arrayPos);
            } else if (arrayPos < str.size() && str[arrayPos] == ']') {
                pos = arrayPos + 1;
                return result;
            } else {
                lastError_ = JSONError::InvalidJSON;
                errorMessage_ = "Expected , or ] in array";
                return result;
            }
        }
        
        lastError_ = JSONError::InvalidJSON;
        errorMessage_ = "Unterminated array";
        return result;
    }
    
    // Parse object value
    std::map<std::string, JSONValue> parseObject(const std::string& str, size_t& pos) {
        if (str[pos] != '{') {
            lastError_ = JSONError::InvalidJSON;
            errorMessage_ = "Expected { for object start";
            return {};
        }
        
        pos++;
        std::map<std::string, JSONValue> result;
        size_t objPos = skipWhitespaceAndComments(str, pos);
        
        if (objPos < str.size() && str[objPos] == '}') {
            pos = objPos + 1;
            return result;
        }
        
        while (objPos < str.size()) {
            // Parse key
            objPos = skipWhitespaceAndComments(str, objPos);
            if (str[objPos] != '"') {
                lastError_ = JSONError::InvalidJSON;
                errorMessage_ = "Expected \" for key start";
                return result;
            }
            
            std::string key = parseString(str, objPos);
            objPos = skipWhitespaceAndComments(str, objPos);
            
            if (objPos >= str.size() || str[objPos] != ':') {
                lastError_ = JSONError::InvalidJSON;
                errorMessage_ = "Expected : after key";
                return result;
            }
            
            objPos++;
            objPos = skipWhitespaceAndComments(str, objPos);
            
            // Parse value
            JSONValue value;
            if (str[objPos] == '"') {
                value = parseString(str, objPos);
            } else if (str[objPos] == '[') {
                value = JSONValue(parseArray(str, objPos));
            } else if (str[objPos] == '{') {
                value = JSONValue(parseObject(str, objPos));
            } else if (std::isdigit(static_cast<unsigned char>(str[objPos])) || 
                       str[objPos] == '-' || str[objPos] == '.') {
                value = parseNumber(str, objPos);
            } else if (str[objPos] == 't' || str[objPos] == 'f') {
                // Parse boolean
                std::string boolStr;
                while (objPos < str.size() && (std::isalpha(static_cast<unsigned char>(str[objPos])))) {
                    boolStr += str[objPos];
                    objPos++;
                }
                if (boolStr == "true") {
                    value = JSONValue(true);
                } else if (boolStr == "false") {
                    value = JSONValue(false);
                } else {
                    lastError_ = JSONError::InvalidJSON;
                    errorMessage_ = "Invalid boolean value: " + boolStr;
                    return result;
                }
            } else {
                lastError_ = JSONError::InvalidJSON;
                errorMessage_ = "Unexpected character in object: " + std::string(1, str[objPos]);
                return result;
            }
            
            result[key] = value;
            
            // Skip whitespace
            objPos = skipWhitespaceAndComments(str, objPos);
            
            if (objPos < str.size() && str[objPos] == ',') {
                objPos++;
                objPos = skipWhitespaceAndComments(str, objPos);
            } else if (objPos < str.size() && str[objPos] == '}') {
                pos = objPos + 1;
                return result;
            } else {
                lastError_ = JSONError::InvalidJSON;
                errorMessage_ = "Expected , or } in object";
                return result;
            }
        }
        
        lastError_ = JSONError::InvalidJSON;
        errorMessage_ = "Unterminated object";
        return result;
    }
    
    // Convert JSON value to ConfigValue
    ConfigValue jsonToConfigValue(const JSONValue& jsonVal, JSONError& error, std::string& errorMsg) {
        if (jsonVal.isNumber()) {
            const auto* numPtr = jsonVal.get<double>();
            if (numPtr) {
                double val = *numPtr;
                // Try to determine best integer type
                if (val == static_cast<int64_t>(val)) {
                    return static_cast<int64_t>(val);
                }
                return val;
            }
        } else if (jsonVal.isString()) {
            return *jsonVal.get<std::string>();
        } else if (jsonVal.isBool()) {
            return *jsonVal.get<bool>();
        } else if (jsonVal.isArray()) {
            const auto* arrPtr = jsonVal.get<std::vector<JSONValue>>();
            if (arrPtr) {
                // Convert JSON array to appropriate ConfigValue type
                std::vector<double> doubleArray;
                std::vector<std::string> stringArray;
                std::vector<int> intArray;
                
                for (const auto& item : *arrPtr) {
                    if (item.isNumber()) {
                        const auto* numPtr = item.get<double>();
                        if (numPtr) {
                            doubleArray.push_back(*numPtr);
                        }
                    } else if (item.isString()) {
                        const auto* strPtr = item.get<std::string>();
                        if (strPtr) {
                            stringArray.push_back(*strPtr);
                        }
                    }
                }
                
                // Try to determine best array type
                if (!stringArray.empty()) {
                    return stringArray;
                } else if (!doubleArray.empty()) {
                    // Check if all are integers
                    bool allIntegers = true;
                    for (double d : doubleArray) {
                        if (d != static_cast<int64_t>(d)) {
                            allIntegers = false;
                            break;
                        }
                    }
                    if (allIntegers) {
                        std::vector<int64_t> intArray;
                        for (double d : doubleArray) {
                            intArray.push_back(static_cast<int64_t>(d));
                        }
                        return intArray;
                    }
                    return doubleArray;
                }
            }
        }
        
        error = JSONError::TypeMismatch;
        errorMsg = "Cannot convert JSON value to ConfigValue";
        return 0; // Default to int
    }
    
    // Parse JSON from string
    std::map<std::string, JSONValue> parseJSONFromString(const std::string& jsonStr) {
        // Find the outermost object
        size_t pos = 0;
        size_t objStart = jsonStr.find('{');
        if (objStart == std::string::npos) {
            lastError_ = JSONError::InvalidJSON;
            errorMessage_ = "No JSON object found";
            return {};
        }
        
        return parseObject(jsonStr, objStart);
    }
    
public:
    SimpleJSONParser() : lastError_(JSONError::None), filepath_("") {}
    
    // Parse JSON file
    std::map<std::string, JSONValue> parseFile(const std::string& filepath) {
        filepath_ = filepath;
        std::ifstream file(filepath);
        if (!file.is_open()) {
            lastError_ = JSONError::FileNotFound;
            errorMessage_ = "Cannot open file: " + filepath;
            return {};
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string jsonStr = buffer.str();
        
        return parseJSONFromString(jsonStr);
    }
    
    // Parse JSON from string
    std::map<std::string, JSONValue> parseString(const std::string& jsonStr) {
        filepath_ = ""; // Clear filepath for string parsing
        return parseJSONFromString(jsonStr);
    }
    
    // Get last error
    JSONError getLastError() const { return lastError_; }
    const std::string& getErrorMessage() const { return errorMessage_; }
    
    // Check if valid
    bool isValid() const { return lastError_ == JSONError::None; }
};

// Main configuration class with enhanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type conversion
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get values with type conversion and validation
    template<typename T>
    std::optional<T> getWithValidation(const std::string& key, 
                                       const ValidationRule& rule) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getOrWithValidation(const std::string& key, const T& defaultValue,
                         const ValidationRule& rule) const;
    
    // Set values with source tracking
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Set values with validation
    bool setWithValidation(const std::string& key, const ConfigValue& value,
                          ConfigSource source = ConfigSource::Runtime,
                          const ValidationRule& rule = ValidationRule("", ""));
    
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Array value setters
    void set(const std::string& key, const std::vector<int>& values, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& values, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& values, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Check existence and type
    bool has(const std::string& key) const;
    bool hasType(const std::string& key, const std::string& type) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by type
    std::vector<std::string> getKeysByType(const std::string& type) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with validation info
    std::string summary() const;
    
    // Get configuration statistics
    std::string getStatistics() const;
    
    // Validation methods
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    void addValidationRule(const ValidationRule& rule);
    void removeValidationRule(const std::string& key);
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::File);
    
    // Configuration snapshot and restore
    std::string createSnapshot() const;
    bool restoreFromSnapshot(const std::string& snapshot);
    
    // Python API helper methods
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
    std::vector<std::string> getStringArray(const std::string& key) const;
    std::vector<int> getIntArray(const std::string& key) const;
    std::vector<double> getDoubleArray(const std::string& key) const;
    
    // Configuration presets for NLM
    static Config createDefaultConfig(int neuronCount = 1000, int regionCount = 1,
                                      double timestep = 0.001);
    static Config createMinimalConfig();
    static Config createResearchConfig();
    static Config createHighPerformanceConfig();
    
    // Python integration helpers
    std::map<std::string, std::string> toPythonDict() const;
    static Config fromPythonDict(const std::map<std::string, std::string>& configDict);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isWhitespace(char c);
    static bool isAlpha(char c);
    static bool isDigit(char c);
    static bool isAlphanumeric(char c);
    
    // Type conversion helpers
    template<typename T>
    static std::string configValueToString(const ConfigValue& value) const;
    
    // Error handling
    static std::string jsonErrorToString(JSONError error);
};

} // namespace nlm
