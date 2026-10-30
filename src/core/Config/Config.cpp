#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cmath>
#include <cctype>
#include <unordered_map>
#include <optional>
#include <variant>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <cctype>
#include <algorithm>
#include <map>
#include <set>

namespace nlm {

// Parser Exception hierarchy
class ParserException : public std::runtime_error {
public:
    explicit ParserException(const std::string& message) 
        : std::runtime_error(message) {}
};

class ParserFormatException : public ParserException {
public:
    explicit ParserFormatException(const std::string& message) 
        : ParserException(message) {}
};

class ParserContentException : public ParserException {
public:
    explicit ParserContentException(const std::string& message) 
        : ParserException(message) {}
};

// Parser interface
class Parser {
public:
    virtual ~Parser() = default;
    virtual std::unordered_map<std::string, ConfigValue> parse(const std::string& content) = 0;
    virtual std::string getFormatName() const = 0;
};

// JSON Parser implementation
class JSONParser : public Parser {
private:
    std::unordered_map<std::string, ConfigValue> parseValue(const std::string& content, size_t& pos);
    std::string parseString(const std::string& content, size_t& pos);
    std::string parseNumber(const std::string& content, size_t& pos);
    bool parseBool(const std::string& content, size_t& pos);
    bool parseNull(const std::string& content, size_t& pos);
    void skipWhitespace(const std::string& content, size_t& pos);
    char peek(const std::string& content, size_t pos) const;
    void skipComment(const std::string& content, size_t& pos);
    
public:
    std::unordered_map<std::string, ConfigValue> parse(const std::string& content) override;
    std::string getFormatName() const override { return "JSON"; }
    
    // Helper for flattening nested objects
    static void flattenObject(const std::unordered_map<std::string, ConfigValue>& obj, 
                            std::unordered_map<std::string, ConfigValue>& flat, 
                            const std::string& prefix = "");
};

// YAML Parser implementation
class YAMLParser : public Parser {
private:
    std::unordered_map<std::string, ConfigValue> parse(const std::string& content) override;
    std::string getFormatName() const override { return "YAML"; }
    std::vector<ConfigValue> parseArray(const std::string& content, size_t& pos);
    ConfigValue parseValue(const std::string& content, size_t& pos);
    void skipWhitespace(const std::string& content, size_t& pos);
    char peek(const std::string& content, size_t pos) const;
    void skipComment(const std::string& content, size_t& pos);
};

// Format detection
static std::unique_ptr<Parser> createParser(const std::string& filepath);
static std::unique_ptr<Parser> detectParserByExtension(const std::string& filepath);
static std::unique_ptr<Parser> detectParserByContent(const std::string& content);

// String utilities
static std::string trim(const std::string& str);
static bool startsWith(const std::string& str, const std::string& prefix);
static bool endsWith(const std::string& str, const std::string& suffix);
static std::string toLower(const std::string& str);

// Type conversion utilities
static ConfigValue convertToConfigValue(const std::string& str);
static bool isNumber(const std::string& str);
static bool isInteger(const std::string& str);
static bool isBoolean(const std::string& str);
static bool isNull(const std::string& str);

// Enhanced backward compatibility for simple key=value format
static std::unordered_map<std::string, ConfigValue> parseSimpleFormat(const std::string& content);

// JSON Parser implementation
std::unordered_map<std::string, ConfigValue> JSONParser::parse(const std::string& content) {
    size_t pos = 0;
    skipWhitespace(content, pos);
    
    if (content.empty() || content[pos] != '{') {
        throw ParserFormatException("JSON must start with '{'");
    }
    
    pos++; // Skip opening brace
    skipWhitespace(content, pos);
    
    std::unordered_map<std::string, ConfigValue> result;
    
    if (content[pos] == '}') {
        pos++; // Skip closing brace
        return result;
    }
    
    while (true) {
        skipWhitespace(content, pos);
        
        // Parse key
        if (content[pos] != '"') {
            throw ParserFormatException("Expected key name (string) at position " + std::to_string(pos));
        }
        
        std::string key = parseString(content, pos);
        skipWhitespace(content, pos);
        
        if (content[pos] != ':') {
            throw ParserFormatException("Expected ':' after key " + key + " at position " + std::to_string(pos));
        }
        
        pos++; // Skip colon
        skipWhitespace(content, pos);
        
        // Parse value
        ConfigValue value = parseValue(content, pos);
        result[key] = value;
        
        skipWhitespace(content, pos);
        
        if (content[pos] == ',') {
            pos++; // Skip comma
            continue;
        } else if (content[pos] == '}') {
            pos++; // Skip closing brace
            break;
        } else {
            throw ParserFormatException("Expected ',' or '}' at position " + std::to_string(pos));
        }
    }
    
    return result;
}

std::string JSONParser::parseString(const std::string& content, size_t& pos) {
    if (content[pos] != '"') {
        throw ParserFormatException("Expected '\"' at position " + std::to_string(pos));
    }
    
    pos++; // Skip opening quote
    std::string result;
    
    while (pos < content.size()) {
        if (content[pos] == '\\') {
            pos++; // Skip escape character
            if (pos >= content.size()) {
                throw ParserFormatException("Unexpected end of string after escape at position " + std::to_string(pos - 1));
            }
            
            switch (content[pos]) {
                case '\"': result += '\"'; break;
                case '\\': result += '\\'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                case 'u': {
                    // Unicode escape
                    if (pos + 4 >= content.size()) {
                        throw ParserFormatException("Incomplete Unicode escape sequence at position " + std::to_string(pos));
                    }
                    // For now, just skip the escape sequence
                    pos += 4;
                    break;
                }
                default:
                    throw ParserFormatException("Unknown escape sequence '\\" + content[pos] + "' at position " + std::to_string(pos));
            }
        } else if (content[pos] == '"') {
            pos++; // Skip closing quote
            return result;
        } else {
            result += content[pos];
        }
        pos++;
    }
    
    throw ParserFormatException("Unterminated string literal at position " + std::to_string(pos));
}

std::string JSONParser::parseNumber(const std::string& content, size_t& pos) {
    size_t startPos = pos;
    
    if (content[pos] == '-') {
        pos++;
    }
    
    if (content[pos] == '0') {
        pos++;
        if (pos < content.size() && std::isdigit(content[pos])) {
            throw ParserFormatException("Invalid number: trailing digits after 0 at position " + std::to_string(pos));
        }
    } else {
        while (pos < content.size() && std::isdigit(content[pos])) {
            pos++;
        }
    }
    
    if (pos < content.size() && content[pos] == '.') {
        pos++; // Skip decimal point
        if (pos >= content.size() || !std::isdigit(content[pos])) {
            throw ParserFormatException("Invalid number: missing digits after decimal point at position " + std::to_string(pos - 1));
        }
        while (pos < content.size() && std::isdigit(content[pos])) {
            pos++;
        }
    }
    
    if (pos < content.size() && (content[pos] == 'e' || content[pos] == 'E')) {
        pos++; // Skip exponent
        if (pos < content.size() && (content[pos] == '+' || content[pos] == '-')) {
            pos++; // Skip exponent sign
        }
        if (pos >= content.size() || !std::isdigit(content[pos])) {
            throw ParserFormatException("Invalid number: missing digits in exponent at position " + std::to_string(pos - 1));
        }
        while (pos < content.size() && std::isdigit(content[pos])) {
            pos++;
        }
    }
    
    return content.substr(startPos, pos - startPos);
}

bool JSONParser::parseBool(const std::string& content, size_t& pos) {
    if (content.compare(pos, 4, "true") == 0) {
        pos += 4;
        return true;
    } else if (content.compare(pos, 5, "false") == 0) {
        pos += 5;
        return false;
    }
    return false;
}

bool JSONParser::parseNull(const std::string& content, size_t& pos) {
    if (content.compare(pos, 4, "null") == 0) {
        pos += 4;
        return true;
    }
    return false;
}

void JSONParser::skipWhitespace(const std::string& content, size_t& pos) {
    while (pos < content.size() && std::isspace(content[pos])) {
        pos++;
    }
}

char JSONParser::peek(const std::string& content, size_t pos) const {
    if (pos < content.size()) {
        return content[pos];
    }
    return '\0';
}

void JSONParser::skipComment(const std::string& content, size_t& pos) {
    if (content.compare(pos, 2, "//") == 0) {
        pos += 2;
        while (pos < content.size() && content[pos] != '\n') {
            pos++;
        }
    } else if (content.compare(pos, 2, "/*") == 0) {
        pos += 2;
        while (pos + 1 < content.size()) {
            if (content.compare(pos, 2, "*/") == 0) {
                pos += 2;
                return;
            }
            pos++;
        }
        throw ParserFormatException("Unterminated multi-line comment");
    }
}

ConfigValue JSONParser::parseValue(const std::string& content, size_t& pos) {
    skipWhitespace(content, pos);
    
    switch (content[pos]) {
        case '"': {
            std::string strValue = parseString(content, pos);
            return convertToConfigValue(strValue);
        }
        case '-': case '0': case '1': case '2': case '3': case '4': 
        case '5': case '6': case '7': case '8': case '9': {
            std::string numStr = parseNumber(content, pos);
            return convertToConfigValue(numStr);
        }
        case 't': case 'f': {
            bool boolValue = parseBool(content, pos);
            return boolValue;
        }
        case 'n': {
            if (parseNull(content, pos)) {
                return std::string("null");
            } else {
                throw ParserFormatException("Expected 'null' at position " + std::to_string(pos));
            }
        }
        case '[': {
            pos++; // Skip '['
            skipWhitespace(content, pos);
            std::vector<ConfigValue> array;
            
            if (content[pos] == ']') {
                pos++; // Skip ']'
                return array;
            }
            
            while (true) {
                ConfigValue value = parseValue(content, pos);
                array.push_back(value);
                
                skipWhitespace(content, pos);
                
                if (content[pos] == ',') {
                    pos++; // Skip comma
                    continue;
                } else if (content[pos] == ']') {
                    pos++; // Skip ']'
                    break;
                } else {
                    throw ParserFormatException("Expected ',' or ']' at position " + std::to_string(pos));
                }
            }
            
            return array;
        }
        case '{': {
            return parse(content, pos);
        }
        default:
            throw ParserFormatException("Unexpected character '" + content[pos] + "' at position " + std::to_string(pos));
    }
}

void JSONParser::flattenObject(const std::unordered_map<std::string, ConfigValue>& obj, 
                             std::unordered_map<std::string, ConfigValue>& flat, 
                             const std::string& prefix) {
    for (const auto& pair : obj) {
        std::string key = pair.first;
        ConfigValue value = pair.second;
        
        if (!prefix.empty()) {
            key = prefix + "." + key;
        }
        
        if (std::holds_alternative<std::unordered_map<std::string, ConfigValue>>(value)) {
            // Recursively flatten nested objects
            flattenObject(std::get<std::unordered_map<std::string, ConfigValue>>(value), flat, key);
        } else {
            flat[key] = value;
        }
    }
}

// YAML Parser implementation
std::unordered_map<std::string, ConfigValue> YAMLParser::parse(const std::string& content) {
    std::unordered_map<std::string, ConfigValue> result;
    std::string currentKey;
    bool isNested = false;
    
    size_t pos = 0;
    size_t lineStart = 0;
    size_t lineEnd;
    
    while ((lineEnd = content.find('\n', lineStart)) != std::string::npos) {
        std::string line = content.substr(lineStart, lineEnd - lineStart);
        lineStart = lineEnd + 1;
        
        // Skip comments and empty lines
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        
        // Check for document separator
        if (line == "---") {
            continue;
        }
        
        // Parse key: value pair
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            throw ParserFormatException("YAML line does not contain ':' separator: " + line);
        }
        
        std::string key = trim(line.substr(0, colonPos));
        std::string valueStr = trim(line.substr(colonPos + 1));
        
        // Handle nested objects (indented content)
        size_t indentLevel = std::count(line.begin(), line.begin() + colonPos, ' ');
        
        if (!key.empty() && !isNested) {
            currentKey = key;
            isNested = false;
        }
        
        // Parse value based on content
        ConfigValue value;
        
        if (valueStr.empty()) {
            // Check if this is a nested object
            size_t nextLineStart = content.find('\n', lineEnd);
            if (nextLineStart != std::string::npos) {
                std::string nextLine = content.substr(lineEnd + 1, nextLineStart - lineEnd - 1);
                nextLine = trim(nextLine);
                if (!nextLine.empty() && nextLine.find(':') != std::string::npos) {
                    // This is a nested object
                    isNested = true;
                    value = std::string("[nested]"); // Placeholder for nested object
                    result[currentKey] = value;
                    continue;
                }
            }
            
            // Empty value - treat as string
            value = valueStr;
        } else if (valueStr == "[]") {
            value = std::vector<ConfigValue>();
        } else if (valueStr.substr(0, 2) == "- ") {
            // Array item
            value = parseValue(content, lineEnd);
        } else {
            value = convertToConfigValue(valueStr);
        }
        
        if (!isNested) {
            result[currentKey] = value;
        }
    }
    
    return result;
}

std::vector<ConfigValue> YAMLParser::parseArray(const std::string& content, size_t& pos) {
    std::vector<ConfigValue> array;
    skipWhitespace(content, pos);
    
    if (content[pos] != '[') {
        throw ParserFormatException("Expected '[' at position " + std::to_string(pos));
    }
    
    pos++; // Skip '['
    skipWhitespace(content, pos);
    
    if (content[pos] == ']') {
        pos++; // Skip ']'
        return array;
    }
    
    while (true) {
        ConfigValue value = parseValue(content, pos);
        array.push_back(value);
        
        skipWhitespace(content, pos);
        
        if (content[pos] == ',') {
            pos++; // Skip comma
            continue;
        } else if (content[pos] == ']') {
            pos++; // Skip ']'
            break;
        } else {
            throw ParserFormatException("Expected ',' or ']' at position " + std::to_string(pos));
        }
    }
    
    return array;
}

ConfigValue YAMLParser::parseValue(const std::string& content, size_t& pos) {
    skipWhitespace(content, pos);
    
    if (content[pos] == '[') {
        return parseArray(content, pos);
    } else if (content[pos] == '"') {
        // Parse string (simplified)
        std::string strValue;
        pos++; // Skip opening quote
        while (pos < content.size() && content[pos] != '"') {
            strValue += content[pos];
            pos++;
        }
        if (pos >= content.size()) {
            throw ParserFormatException("Unterminated string literal at position " + std::to_string(pos));
        }
        pos++; // Skip closing quote
        return convertToConfigValue(strValue);
    } else if (content[pos] == '-') {
        // Parse array item
        pos++; // Skip '-'
        skipWhitespace(content, pos);
        return parseValue(content, pos);
    } else {
        // Parse scalar value
        size_t startPos = pos;
        while (pos < content.size() && content[pos] != '\n' && content[pos] != '\r') {
            pos++;
        }
        std::string scalar = content.substr(startPos, pos - startPos);
        return convertToConfigValue(scalar);
    }
}

void YAMLParser::skipWhitespace(const std::string& content, size_t& pos) {
    while (pos < content.size() && std::isspace(content[pos])) {
        pos++;
    }
}

char YAMLParser::peek(const std::string& content, size_t pos) const {
    if (pos < content.size()) {
        return content[pos];
    }
    return '\0';
}

void YAMLParser::skipComment(const std::string& content, size_t& pos) {
    if (content.compare(pos, 2, "#") == 0) {
        pos += 2;
        while (pos < content.size() && content[pos] != '\n') {
            pos++;
        }
    }
}

// Format detection
std::unique_ptr<Parser> createParser(const std::string& filepath) {
    auto parserByExtension = detectParserByExtension(filepath);
    if (parserByExtension) {
        return parserByExtension;
    }
    
    // If extension detection failed, try content detection
    // Note: In a real implementation, we might read the file content here
    // For now, we can't detect without reading the file, so we'll return nullptr
    // The caller should handle this case
    return nullptr;
}

static std::unique_ptr<Parser> detectParserByExtension(const std::string& filepath) {
    if (filepath.empty()) {
        return nullptr;
    }
    
    std::string lowerExt = toLower(filepath);
    
    if (endsWith(lowerExt, ".json")) {
        return std::make_unique<JSONParser>();
    } else if (endsWith(lowerExt, ".yaml") || endsWith(lowerExt, ".yml")) {
        return std::make_unique<YAMLParser>();
    }
    
    return nullptr;
}

static std::unique_ptr<Parser> detectParserByContent(const std::string& content) {
    // Basic content-based detection
    if (content.empty()) {
        return nullptr;
    }
    
    // Remove comments for analysis
    std::string cleanedContent = content;
    size_t commentPos = cleanedContent.find("//");
    while (commentPos != std::string::npos) {
        size_t lineEnd = cleanedContent.find('\n', commentPos);
        if (lineEnd == std::string::npos) {
            cleanedContent.erase(commentPos);
            break;
        }
        cleanedContent.erase(commentPos, lineEnd - commentPos);
        commentPos = cleanedContent.find("//");
    }
    
    commentPos = cleanedContent.find("/*");
    while (commentPos != std::string::npos) {
        size_t endCommentPos = cleanedContent.find("*/", commentPos);
        if (endCommentPos == std::string::npos) {
            throw ParserFormatException("Unterminated multi-line comment");
        }
        cleanedContent.erase(commentPos, endCommentPos - commentPos + 2);
        commentPos = cleanedContent.find("/*");
    }
    
    // Remove YAML comments
    commentPos = cleanedContent.find('#');
    while (commentPos != std::string::npos) {
        size_t lineEnd = cleanedContent.find('\n', commentPos);
        if (lineEnd == std::string::npos) {
            cleanedContent.erase(commentPos);
            break;
        }
        cleanedContent.erase(commentPos, lineEnd - commentPos);
        commentPos = cleanedContent.find('#');
    }
    
    cleanedContent = trim(cleanedContent);
    
    // Try JSON detection
    if (!cleanedContent.empty() && cleanedContent[0] == '{') {
        return std::make_unique<JSONParser>();
    }
    
    // Try YAML detection
    if (!cleanedContent.empty() && (std::isalpha(cleanedContent[0]) || cleanedContent[0] == '-')) {
        return std::make_unique<YAMLParser>();
    }
    
    return nullptr;
}

// String utilities
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

static bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

static bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Type conversion utilities
static ConfigValue convertToConfigValue(const std::string& str) {
    // Check for null first
    if (str == "null" || str == "NULL") {
        return std::string("null");
    }
    
    // Check for boolean
    if (str == "true" || str == "True" || str == "TRUE") {
        return true;
    }
    if (str == "false" || str == "False" || str == "FALSE") {
        return false;
    }
    
    // Check for integer
    try {
        // Try 64-bit integer first
        size_t pos;
        int64_t intValue = std::stoll(str, &pos);
        if (pos == str.size()) {
            return intValue;
        }
    } catch (...) {}
    
    // Check for unsigned integer
    try {
        size_t pos;
        uint64_t uintValue = std::stoull(str, &pos);
        if (pos == str.size()) {
            return static_cast<int64_t>(uintValue);
        }
    } catch (...) {}
    
    // Check for floating point
    try {
        size_t pos;
        double doubleValue = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleValue;
        }
    } catch (...) {}
    
    // Default to string
    return str;
}

static bool isNumber(const std::string& str) {
    try {
        size_t pos;
        double value = std::stod(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

static bool isInteger(const std::string& str) {
    try {
        size_t pos;
        int64_t value = std::stoll(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

static bool isBoolean(const std::string& str) {
    return str == "true" || str == "false" || str == "True" || str == "False" || 
           str == "TRUE" || str == "FALSE" || str == "null" || str == "NULL";
}

static bool isNull(const std::string& str) {
    return str == "null" || str == "NULL";
}

// Enhanced backward compatibility for simple key=value format
static std::unordered_map<std::string, ConfigValue> parseSimpleFormat(const std::string& content) {
    std::unordered_map<std::string, ConfigValue> result;
    size_t lineStart = 0;
    size_t lineEnd;
    
    while ((lineEnd = content.find('\n', lineStart)) != std::string::npos) {
        std::string line = content.substr(lineStart, lineEnd - lineStart);
        lineStart = lineEnd + 1;
        
        // Skip comments and empty lines
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line = trim(line);
        if (line.empty()) {
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
            
            result[key] = convertToConfigValue(value);
        }
    }
    
    return result;
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        auto parser = createParser(filepath);
        if (!parser) {
            // Fallback to simple format for backward compatibility
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            
            auto parsed = parseSimpleFormat(content);
            
            // Clear existing entries and add parsed ones
            pImpl->entries.clear();
            for (const auto& pair : parsed) {
                set(pair.first, pair.second, ConfigSource::File);
            }
            
            return true;
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        auto parsed = parser->parse(content);
        
        // Clear existing entries and add parsed ones
        pImpl->entries.clear();
        for (const auto& pair : parsed) {
            set(pair.first, pair.second, ConfigSource::File);
        }
        
        return true;
    } catch (const ParserException& e) {
        // Log error and return false for parsing failures
        return false;
    } catch (const std::exception& e) {
        // Log error and return false for other exceptions
        return false;
    }
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