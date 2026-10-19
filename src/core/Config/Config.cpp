#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// JSON parsing helpers
bool parseJSON(const std::string& content, ConfigSource source);

// Internal JSON value parsing
ConfigValue parseJSONValue(const std::string& str);
std::string parseJSONString(const std::string& str, size_t& pos);
double parseJSONNumber(const std::string& str, size_t& pos);
bool parseJSONBoolean(const std::string& str, size_t& pos);
std::vector<double> parseJSONArray(const std::string& str, size_t& pos);
std::vector<std::string> parseJSONStringArray(const std::string& str, size_t& pos);

// JSON utility functions
size_t skipWhitespace(const std::string& str, size_t pos);
bool isDigit(char c);
bool isAlpha(char c);
bool startsWith(const std::string& str, size_t pos, const std::string& prefix);

// JSON escape handling
std::string unescapeString(const std::string& str);

// JSON validation
bool validateKey(const std::string& key);
bool validateValue(const ConfigValue& value);

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Parse JSON
    return parseJSON(content, ConfigSource::File);
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
    
    // Write JSON content
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double> || 
                               std::is_same_v<T, int> || 
                               std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << arg[j];
                    if (j < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << arg[j];
                    if (j < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << "\"" << arg[j] << "\"";
                    if (j < arg.size() - 1) file << ", ";
                }
                file << "]";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) file << ",\n";
    }
    file << "}\n";
    
    return true;
}

// ============================================================================
// JSON parsing implementation
// ============================================================================

namespace nlm {

// ============================================================================
// Core JSON parsing function
// ============================================================================
bool parseJSON(const std::string& content, ConfigSource source) {
    try {
        size_t pos = 0;
        pos = skipWhitespace(content, pos);
        
        if (pos >= content.size() || content[pos] != '{') {
            return false; // Not valid JSON object
        }
        
        pos++; // Skip '{'
        pos = skipWhitespace(content, pos);
        
        // Parse JSON object
        bool firstEntry = true;
        while (pos < content.size() && content[pos] != '}') {
            if (!firstEntry && content[pos] == ',') {
                pos++;
            }
            
            pos = skipWhitespace(content, pos);
            
            // Parse key
            std::string key = parseJSONString(content, pos);
            if (key.empty()) {
                return false; // Failed to parse key
            }
            
            pos = skipWhitespace(content, pos);
            
            if (pos >= content.size() || content[pos] != ':') {
                return false; // Missing colon after key
            }
            
            pos++; // Skip ':'
            pos = skipWhitespace(content, pos);
            
            // Parse value
            ConfigValue value = parseJSONValue(content.substr(pos));
            
            // Validate and set the value
            if (validateKey(key) && validateValue(value)) {
                ConfigEntry entry;
                entry.key = key;
                entry.value = value;
                entry.source = source;
                entry.description = "";
                pImpl->entries.push_back(entry);
            } else {
                return false;
            }
            
            firstEntry = false;
            pos = skipWhitespace(content, pos);
            
            if (pos < content.size() && content[pos] == ',') {
                pos++;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// ============================================================================
// Internal JSON value parsing
// ============================================================================
ConfigValue parseJSONValue(const std::string& str) {
    size_t pos = 0;
    pos = skipWhitespace(str, pos);
    
    if (pos >= str.size()) {
        return ConfigValue(); // Empty string
    }
    
    switch (str[pos]) {
        case '\"':
            return parseJSONString(str, pos);
        case '[':
            return parseJSONStringArray(str, pos);
        case 't':
        case 'f':
            return parseJSONBoolean(str, pos);
        default:
            // Try to parse as number
            size_t numPos = pos;
            double num = parseJSONNumber(str, numPos);
            if (numPos > pos) {
                return num;
            }
            
            // If not a number, try to parse as string
            return parseJSONString(str, pos);
    }
}

std::string parseJSONString(const std::string& str, size_t& pos) {
    if (pos >= str.size() || str[pos] != '\"') {
        return ""; // Not a string
    }
    
    pos++; // Skip opening quote
    std::string result;
    
    while (pos < str.size() && str[pos] != '\"') {
        if (str[pos] == '\\') {
            pos++; // Skip escape character
            if (pos < str.size()) {
                switch (str[pos]) {
                    case 'n': result += '\n'; break;
                    case 't': result += '\t'; break;
                    case 'r': result += '\r'; break;
                    case '\\': result += '\\'; break;
                    case '\"': result += '\"'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    default: result += str[pos]; break; // Handle unknown escapes
                }
            }
        } else {
            result += str[pos];
        }
        pos++;
    }
    
    if (pos < str.size() && str[pos] == '\"') {
        pos++; // Skip closing quote
    }
    
    return result;
}

double parseJSONNumber(const std::string& str, size_t& pos) {
    if (pos >= str.size()) {
        return 0.0;
    }
    
    // Check for negative sign
    bool negative = false;
    if (str[pos] == '-') {
        negative = true;
        pos++;
    }
    
    // Parse integer part
    double result = 0.0;
    bool hasDecimal = false;
    double decimal = 0.0;
    int decimalPlaces = 0;
    
    while (pos < str.size() && (isdigit(str[pos]) || str[pos] == '.')) {
        if (str[pos] == '.') {
            hasDecimal = true;
        } else if (hasDecimal) {
            decimalPlaces++;
            decimal = decimal * 10 + (str[pos] - '0');
        } else {
            result = result * 10 + (str[pos] - '0');
        }
        pos++;
    }
    
    if (hasDecimal) {
        result += decimal / std::pow(10.0, static_cast<double>(decimalPlaces));
    }
    
    if (negative) {
        result = -result;
    }
    
    return result;
}

bool parseJSONBoolean(const std::string& str, size_t& pos) {
    if (pos >= str.size()) {
        return false;
    }
    
    // Check for "true"
    if (startsWith(str, pos, "true")) {
        pos += 4;
        return true;
    }
    
    // Check for "false"
    if (startsWith(str, pos, "false")) {
        pos += 5;
        return false;
    }
    
    return false;
}

std::vector<double> parseJSONArray(const std::string& str, size_t& pos) {
    std::vector<double> result;
    
    if (pos >= str.size() || str[pos] != '[') {
        return result; // Not an array
    }
    
    pos++; // Skip '['
    
    bool firstElement = true;
    while (pos < str.size() && str[pos] != ']') {
        if (!firstElement && str[pos] == ',') {
            pos++;
        }
        
        pos = skipWhitespace(str, pos);
        
        size_t numPos = pos;
        double value = parseJSONNumber(str, numPos);
        
        if (numPos > pos) {
            result.push_back(value);
            pos = numPos;
        } else {
            break; // Failed to parse element
        }
        
        firstElement = false;
        pos = skipWhitespace(str, pos);
    }
    
    if (pos < str.size() && str[pos] == ']') {
        pos++; // Skip ']'
    }
    
    return result;
}

std::vector<std::string> parseJSONStringArray(const std::string& str, size_t& pos) {
    std::vector<std::string> result;
    
    if (pos >= str.size() || str[pos] != '[') {
        return result; // Not an array
    }
    
    pos++; // Skip '['
    
    bool firstElement = true;
    while (pos < str.size() && str[pos] != ']') {
        if (!firstElement && str[pos] == ',') {
            pos++;
        }
        
        pos = skipWhitespace(str, pos);
        
        std::string value = parseJSONString(str, pos);
        
        if (!value.empty()) {
            result.push_back(value);
        }
        
        firstElement = false;
        pos = skipWhitespace(str, pos);
    }
    
    if (pos < str.size() && str[pos] == ']') {
        pos++; // Skip ']'
    }
    
    return result;
}

// ============================================================================
// JSON utility functions
// ============================================================================
size_t skipWhitespace(const std::string& str, size_t pos) {
    while (pos < str.size() && (str[pos] == ' ' || str[pos] == '\t' || str[pos] == '\n' || str[pos] == '\r')) {
        pos++;
    }
    return pos;
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool startsWith(const std::string& str, size_t pos, const std::string& prefix) {
    if (pos + prefix.size() > str.size()) {
        return false;
    }
    return str.substr(pos, prefix.size()) == prefix;
}

// JSON escape handling
std::string unescapeString(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case 'n': result += '\n'; i++; break;
                case 't': result += '\t'; i++; break;
                case 'r': result += '\r'; i++; break;
                case '\\': result += '\\'; i++; break;
                case '\"': result += '\"'; i++; break;
                case 'b': result += '\b'; i++; break;
                case 'f': result += '\f'; i++; break;
                default: result += str[i + 1]; i++; break;
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

// JSON validation
bool validateKey(const std::string& key) {
    if (key.empty()) {
        return false;
    }
    
    // Valid JSON key must be a quoted string
    if (key[0] != '\"' || key[key.size() - 1] != '\"') {
        return false;
    }
    
    // Check for valid characters in key
    for (char c : key) {
        if (c == '\"' || c == '\\') {
            return false; // Disallow escape characters in keys
        }
    }
    
    return true;
}

bool validateValue(const ConfigValue& value) {
    // All types in ConfigValue are valid
    return true;
}

} // namespace nlm