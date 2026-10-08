#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <set>
#include <ctime>
#include <iomanip>
#include <regex>
#include <functional>
#include <utility>

namespace nlm {

// Global constants for JSON/YAML extensions
const std::vector<std::string> ConfigParser::JSON_EXTENSIONS = {".json", ".cfg"};
const std::vector<std::string> ConfigParser::YAML_EXTENSIONS = {".yaml", ".yml"};

// JSON Parser Implementation
namespace {
    bool JsonParser::parse(const std::string& content, std::unordered_map<std::string, ConfigValue>& config) {
        size_t pos = 0;
        skipWhitespace(content, pos);
        
        if (!parseObject(content, pos, config)) {
            return false;
        }
        
        skipWhitespace(content, pos);
        if (pos != content.size()) {
            return false; // Extra content after JSON object
        }
        
        return true;
    }
    
    bool JsonParser::parseFile(const std::string& filepath, std::unordered_map<std::string, ConfigValue>& config) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        std::string content = buffer.str();
        return parse(content, config);
    }
    
    void JsonParser::skipWhitespace(const std::string& content, size_t& pos) {
        while (pos < content.size() && std::isspace(content[pos])) {
            pos++;
        }
    }
    
    bool JsonParser::parseValue(const std::string& content, size_t& pos, ConfigValue& value) {
        skipWhitespace(content, pos);
        
        if (pos >= content.size()) {
            return false;
        }
        
        switch (content[pos]) {
            case '"':
                return parseString(content, pos, std::get<std::string>(value));
            case '{':
                return parseObject(content, pos, std::get<std::unordered_map<std::string, ConfigValue>>(value));
            case '[':
                return parseArray(content, pos, value);
            case 't': case 'f':
                return parseBoolean(content, pos, value);
            default:
                return parseNumber(content, pos, value);
        }
    }
    
    bool JsonParser::parseString(const std::string& content, size_t& pos, std::string& value) {
        if (content[pos] != '"') {
            return false;
        }
        
        pos++; // Skip opening quote
        value.clear();
        
        while (pos < content.size()) {
            if (content[pos] == '\\') {
                pos++;
                if (pos >= content.size()) {
                    return false;
                }
                
                switch (content[pos]) {
                    case '"': value += '"'; break;
                    case '\\': value += '\\'; break;
                    case '/': value += '/'; break;
                    case 'b': value += '\b'; break;
                    case 'f': value += '\f'; break;
                    case 'n': value += '\n'; break;
                    case 'r': value += '\r'; break;
                    case 't': value += '\t'; break;
                    case 'u': {
                        // Unicode escape sequence
                        if (pos + 4 >= content.size()) {
                            return false;
                        }
                        std::string hex = content.substr(pos + 1, 4);
                        try {
                            char32_t unicode = static_cast<char32_t>(std::stoul(hex, nullptr, 16));
                            value += static_cast<char>(unicode);
                        } catch (...) {
                            return false;
                        }
                        pos += 4;
                        break;
                    }
                    default:
                        return false; // Invalid escape sequence
                }
            } else if (content[pos] == '"') {
                pos++; // Skip closing quote
                return true;
            } else {
                value += content[pos];
            }
            pos++;
        }
        
        return false; // Unterminated string
    }
    
    bool JsonParser::parseNumber(const std::string& content, size_t& pos, ConfigValue& value) {
        size_t start = pos;
        bool hasDecimal = false;
        bool hasExponent = false;
        
        // Parse integer part
        if (content[pos] == '-') {
            pos++;
            if (pos >= content.size() || !std::isdigit(content[pos])) {
                return false;
            }
        }
        
        while (pos < content.size() && std::isdigit(content[pos])) {
            pos++;
        }
        
        // Parse decimal part
        if (pos < content.size() && content[pos] == '.') {
            hasDecimal = true;
            pos++;
            if (pos >= content.size() || !std::isdigit(content[pos])) {
                return false;
            }
            while (pos < content.size() && std::isdigit(content[pos])) {
                pos++;
            }
        }
        
        // Parse exponent
        if (pos < content.size() && (content[pos] == 'e' || content[pos] == 'E')) {
            hasExponent = true;
            pos++;
            if (pos < content.size() && (content[pos] == '+' || content[pos] == '-')) {
                pos++;
            }
            if (pos >= content.size() || !std::isdigit(content[pos])) {
                return false;
            }
            while (pos < content.size() && std::isdigit(content[pos])) {
                pos++;
            }
        }
        
        if (pos == start) {
            return false; // No digits were parsed
        }
        
        std::string numStr = content.substr(start, pos - start);
        
        // Try to parse as int first
        try {
            if (numStr.find('.') == std::string::npos && numStr.find('e') == std::string::npos && 
                numStr.find('E') == std::string::npos) {
                // Integer
                size_t endPos;
                long long intVal = std::stoll(numStr, &endPos);
                if (endPos == numStr.size()) {
                    value = static_cast<int64_t>(intVal);
                    return true;
                }
            }
            
            // Double
            size_t endPos;
            double doubleVal = std::stod(numStr, &endPos);
            if (endPos == numStr.size()) {
                value = doubleVal;
                return true;
            }
        } catch (...) {
            return false;
        }
        
        return false;
    }
    
    bool JsonParser::parseBoolean(const std::string& content, size_t& pos, ConfigValue& value) {
        if (content.compare(pos, 4, "true") == 0) {
            pos += 4;
            value = true;
            return true;
        } else if (content.compare(pos, 5, "false") == 0) {
            pos += 5;
            value = false;
            return true;
        }
        return false;
    }
    
    bool JsonParser::parseArray(const std::string& content, size_t& pos, ConfigValue& value) {
        if (content[pos] != '[') {
            return false;
        }
        
        pos++; // Skip '['
        skipWhitespace(content, pos);
        
        std::vector<ConfigValue> array;
        
        if (content[pos] == ']') {
            pos++; // Skip ']'
            value = array;
            return true;
        }
        
        while (true) {
            ConfigValue item;
            if (!parseValue(content, pos, item)) {
                return false;
            }
            
            array.push_back(std::move(item));
            
            skipWhitespace(content, pos);
            
            if (pos >= content.size()) {
                return false;
            }
            
            if (content[pos] == ']') {
                pos++; // Skip ']'
                value = array;
                return true;
            } else if (content[pos] == ',') {
                pos++; // Skip ','
                skipWhitespace(content, pos);
                continue;
            } else {
                return false; // Expected ',' or ']'
            }
        }
    }
    
    bool JsonParser::parseObject(const std::string& content, size_t& pos, std::unordered_map<std::string, ConfigValue>& obj) {
        if (content[pos] != '{') {
            return false;
        }
        
        pos++; // Skip '{'
        skipWhitespace(content, pos);
        
        if (content[pos] == '}') {
            pos++; // Skip '}'
            return true;
        }
        
        while (true) {
            // Parse key
            std::string key;
            if (!parseString(content, pos, key)) {
                return false;
            }
            
            skipWhitespace(content, pos);
            if (pos >= content.size() || content[pos] != ':') {
                return false;
            }
            pos++; // Skip ':'
            skipWhitespace(content, pos);
            
            // Parse value
            ConfigValue value;
            if (!parseValue(content, pos, value)) {
                return false;
            }
            
            obj[key] = std::move(value);
            
            skipWhitespace(content, pos);
            
            if (pos >= content.size()) {
                return false;
            }
            
            if (content[pos] == '}') {
                pos++; // Skip '}'
                return true;
            } else if (content[pos] == ',') {
                pos++; // Skip ','
                skipWhitespace(content, pos);
                continue;
            } else {
                return false; // Expected ',' or '}'
            }
        }
    }
    
    std::string JsonParser::unescapeString(const std::string& str) {
        std::string result;
        result.reserve(str.size());
        
        for (size_t i = 0; i < str.size(); ++i) {
            if (str[i] == '\\' && i + 1 < str.size()) {
                switch (str[i + 1]) {
                    case '\"': result += '\"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    default: result += str[i + 1]; break; // Unknown escape, keep as-is
                }
                i++; // Skip escaped character
            } else {
                result += str[i];
            }
        }
        
        return result;
    }
}

// YAML Parser Implementation
namespace {
    bool YamlParser::parse(const std::string& content, std::unordered_map<std::string, ConfigValue>& config) {
        return parseYamlDocument(content, config);
    }
    
    bool YamlParser::parseFile(const std::string& filepath, std::unordered_map<std::string, ConfigValue>& config) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        std::string content = buffer.str();
        return parse(content, config);
    }
    
    bool YamlParser::parseYamlDocument(const std::string& content, std::unordered_map<std::string, ConfigValue>& config) {
        size_t pos = 0;
        skipWhitespace(content, pos);
        
        ConfigValue doc;
        if (!parseValue(content, pos, doc)) {
            return false;
        }
        
        // For YAML documents, we convert the parsed value to a ConfigValue
        // and extract key-value pairs if it's a mapping
        if (std::holds_alternative<std::unordered_map<std::string, ConfigValue>>(doc)) {
            config = std::get<std::unordered_map<std::string, ConfigValue>>(doc);
        } else {
            // Single value document
            config["value"] = std::move(doc);
        }
        
        skipWhitespace(content, pos);
        if (pos != content.size()) {
            return false; // Extra content after YAML document
        }
        
        return true;
    }
    
    bool YamlParser::parseScalar(const std::string& content, size_t& pos, ConfigValue& value) {
        if (pos >= content.size()) {
            return false;
        }
        
        if (content[pos] == '"') {
            return parseDoubleQuotedString(content, pos, std::get<std::string>(value));
        } else if (content[pos] == \"'\") {
            return parseSingleQuotedString(content, pos, std::get<std::string>(value));
        } else if (content[pos] == '|') {
            return parseFoldedString(content, pos, std::get<std::string>(value));
        } else {
            return parsePlainScalar(content, pos, std::get<std::string>(value));
        }
    }
    
    bool YamlParser::parseSequence(const std::string& content, size_t& pos, ConfigValue& value) {
        if (content[pos] != '[' && content[pos] != '-') {
            return false;
        }
        
        std::vector<ConfigValue> sequence;
        
        // Handle flow style []
        if (content[pos] == '[') {
            pos++; // Skip '['
            skipWhitespace(content, pos);
            
            if (pos < content.size() && content[pos] == ']') {
                pos++; // Skip ']'
                value = sequence;
                return true;
            }
            
            while (true) {
                ConfigValue item;
                if (!parseValue(content, pos, item)) {
                    return false;
                }
                
                sequence.push_back(std::move(item));
                
                skipWhitespace(content, pos);
                
                if (pos >= content.size()) {
                    return false;
                }
                
                if (content[pos] == ']') {
                    pos++; // Skip ']'
                    break;
                } else if (content[pos] == ',') {
                    pos++; // Skip ','
                    skipWhitespace(content, pos);
                    continue;
                } else {
                    return false;
                }
            }
        } else {
            // Block style sequence
            pos++; // Skip '-'
            while (true) {
                ConfigValue item;
                if (!parseValue(content, pos, item)) {
                    return false;
                }
                
                sequence.push_back(std::move(item));
                
                skipWhitespace(content, pos);
                
                if (pos >= content.size() || content[pos] == '\n') {
                    // New line indicates end of sequence if followed by indentation
                    // This is a simplified implementation
                    break;
                }
            }
        }
        
        value = sequence;
        return true;
    }
    
    bool YamlParser::parseMapping(const std::string& content, size_t& pos, ConfigValue& value) {
        if (content[pos] != '{' && content[pos] != '%') {
            return false;
        }
        
        std::unordered_map<std::string, ConfigValue> mapping;
        
        // Handle flow style {}
        if (content[pos] == '{') {
            pos++; // Skip '{'
            skipWhitespace(content, pos);
            
            if (pos < content.size() && content[pos] == '}') {
                pos++; // Skip '}'
                value = mapping;
                return true;
            }
            
            while (true) {
                // Parse key
                std::string key;
                if (!parseScalar(content, pos, std::get<std::string>(value))) {
                    return false;
                }
                key = std::get<std::string>(value);
                
                skipWhitespace(content, pos);
                
                if (pos >= content.size() || (content[pos] != ':' && content[pos] != '%')) {
                    return false;
                }
                pos++; // Skip ':' or '%'
                skipWhitespace(content, pos);
                
                // Parse value
                ConfigValue val;
                if (!parseValue(content, pos, val)) {
                    return false;
                }
                
                mapping[key] = std::move(val);
                
                skipWhitespace(content, pos);
                
                if (pos >= content.size()) {
                    return false;
                }
                
                if (content[pos] == '}') {
                    pos++; // Skip '}'
                    break;
                } else if (content[pos] == ',') {
                    pos++; // Skip ','
                    skipWhitespace(content, pos);
                    continue;
                } else {
                    return false;
                }
            }
        } else {
            // Block style mapping - simplified
            while (true) {
                skipWhitespace(content, pos);
                
                if (pos >= content.size()) {
                    break;
                }
                
                // Parse key
                std::string key;
                if (!parsePlainScalar(content, pos, key)) {
                    break; // No more keys
                }
                
                skipWhitespace(content, pos);
                if (pos >= content.size() || content[pos] != ':') {
                    return false;
                }
                pos++; // Skip ':'
                skipWhitespace(content, pos);
                
                // Parse value
                ConfigValue val;
                if (!parseValue(content, pos, val)) {
                    return false;
                }
                
                mapping[key] = std::move(val);
                
                skipWhitespace(content, pos);
                if (pos < content.size() && content[pos] == '\n') {
                    // New line indicates next key or end
                    pos++;
                }
            }
        }
        
        value = mapping;
        return true;
    }
    
    bool YamlParser::parseValue(const std::string& content, size_t& pos, ConfigValue& value) {
        skipWhitespace(content, pos);
        
        if (pos >= content.size()) {
            return false;
        }
        
        // Handle aliases
        if (content[pos] == '*') {
            return parseAlias(content, pos, value);
        }
        
        switch (content[pos]) {
            case '"': case \"'\": {
                std::string str;
                if (!parseDoubleQuotedString(content, pos, str)) {
                    if (!parseSingleQuotedString(content, pos, str)) {
                        return false;
                    }
                }
                value = str;
                return true;
            }
            case '|': case '>': {
                std::string str;
                if (!parseFoldedString(content, pos, str)) {
                    return false;
                }
                value = str;
                return true;
            }
            case '{': case '[': case '-': case '%': {
                if (content[pos] == '[' || content[pos] == '-') {
                    return parseSequence(content, pos, value);
                } else {
                    return parseMapping(content, pos, value);
                }
            }
            default: {
                // Try to parse as plain scalar or number/bool
                std::string plain;
                if (!parsePlainScalar(content, pos, plain)) {
                    return false;
                }
                
                // Try to convert to appropriate type
                if (parseBoolean(content, pos, value)) {
                    return true;
                }
                
                size_t tempPos = pos;
                ConfigValue numVal;
                if (parseNumber(content, tempPos, numVal)) {
                    value = numVal;
                    pos = tempPos;
                    return true;
                }
                
                // Keep as string
                value = plain;
                return true;
            }
        }
    }
    
    bool YamlParser::parseAlias(const std::string& content, size_t& pos, ConfigValue& value) {
        pos++; // Skip '*'
        skipWhitespace(content, pos);
        
        // Find anchor name
        size_t start = pos;
        while (pos < content.size() && std::isalnum(content[pos])) {
            pos++;
        }
        
        if (pos == start) {
            return false;
        }
        
        std::string anchor = content.substr(start, pos - start);
        
        // For simplicity, we'll just return the anchor name as a string
        // In a full implementation, we'd maintain an anchor map
        value = "*" + anchor;
        return true;
    }
    
    bool YamlParser::parseDoubleQuotedString(const std::string& content, size_t& pos, std::string& value) {
        char quote = content[pos];
        pos++; // Skip opening quote
        
        value.clear();
        
        while (pos < content.size()) {
            if (content[pos] == '\\') {
                pos++;
                if (pos >= content.size()) {
                    return false;
                }
                
                switch (content[pos]) {
                    case '\"': value += '\"'; break;
                    case '\\': value += '\\'; break;
                    case '/': value += '/'; break;
                    case 'b': value += '\b'; break;
                    case 'f': value += '\f'; break;
                    case 'n': value += '\n'; break;
                    case 'r': value += '\r'; break;
                    case 't': value += '\t'; break;
                    default: value += content[pos]; break; // Unknown escape
                }
            } else if (content[pos] == quote) {
                pos++; // Skip closing quote
                return true;
            } else {
                value += content[pos];
            }
            pos++;
        }
        
        return false; // Unterminated string
    }
    
    bool YamlParser::parseSingleQuotedString(const std::string& content, size_t& pos, std::string& value) {
        char quote = content[pos];
        pos++; // Skip opening quote
        
        value.clear();
        
        while (pos < content.size()) {
            if (content[pos] == quote) {
                pos++; // Skip closing quote
                return true;
            } else {
                value += content[pos];n            }
            pos++;
        }
        
        return false; // Unterminated string
    }
    
    bool YamlParser::parseFoldedString(const std::string& content, size_t& pos, std::string& value) {
        char indicator = content[pos];
        pos++; // Skip indicator
        
        // Skip any scalar indicator (>, |, >-, |-, >>, etc.)
        while (pos < content.size() && (content[pos] == '-' || content[pos] == '+' || content[pos] == '<')) {
            pos++;
        }
        
        skipWhitespace(content, pos);
        
        if (pos >= content.size()) {
            return false;
        }
        
        // Read until we encounter a line that starts at the same indentation level
        // This is a simplified implementation
        size_t startLine = pos;
        while (pos < content.size()) {
            if (content[pos] == '\n') {
                // Check if this is the end of the folded string
                size_t nextPos = pos + 1;
                while (nextPos < content.size() && (content[nextPos] == ' ' || content[nextPos] == '\t')) {
                    nextPos++;
                }
                if (nextPos >= content.size() || content[nextPos] == '\n') {
                    // End of folded string
                    break;
                }
            }
            pos++;
        }
        
        value = content.substr(startLine, pos - startLine);
        
        // Replace folded newlines with actual newlines and spaces
        std::string result;
        for (size_t i = 0; i < value.size(); ++i) {
            if (value[i] == '\n') {
                // Skip leading spaces and tabs
                size_t next = i + 1;
                while (next < value.size() && (value[next] == ' ' || value[next] == '\t')) {
                    next++;
                }
                result += '\n';
                // Add two spaces after newline (YAML folded style)
                result += ' ';
                result += ' ';
                i = next - 1;
            } else {
                result += value[i];
            }
        }
        
        value = result;
        return true;
    }
    
    bool YamlParser::parsePlainScalar(const std::string& content, size_t& pos, std::string& value) {
        size_t start = pos;
        
        while (pos < content.size()) {
            char c = content[pos];
            
            // YAML plain scalar rules
            if (c == '\n' || c == ' ' || c == '\t') {
                break;
            }
            
            if (c == ':' && (pos + 1 >= content.size() || content[pos + 1] == ' ' || 
                             content[pos + 1] == '\t' || content[pos + 1] == '\n')) {
                // This is a mapping key separator
                break;
            }
            
            if (c == '[' || c == ']' || c == '{' || c == '}' || 
                c == ',' || c == '#' || c == '&' || c == '*') {
                break;
            }
            
            pos++;
        }
        
        if (pos == start) {
            return false;
        }
        
        value = content.substr(start, pos - start);
        return true;
    }
    
    void YamlParser::skipWhitespace(const std::string& content, size_t& pos) {
        while (pos < content.size() && std::isspace(content[pos])) {
            pos++;
        }
    }
}

// ConfigParser Implementation
namespace {
    ConfigParser::ParseResult ConfigParser::parseFile(const std::string& filepath) {
        ParseResult result;
        result.error = ParseError::Success;
        
        // Check file extension
        std::string ext = "." + std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        bool isJson = std::find(JSON_EXTENSIONS.begin(), JSON_EXTENSIONS.end(), ext) != JSON_EXTENSIONS.end();
        bool isYaml = std::find(YAML_EXTENSIONS.begin(), YAML_EXTENSIONS.end(), ext) != YAML_EXTENSIONS.end();
        
        if (!isJson && !isYaml) {
            result.error = ParseError::InvalidFormat;
            result.message = "Unsupported file extension: " + ext;
            return result;
        }
        
        // Parse based on file type
        if (isJson) {
            if (!JsonParser::parseFile(filepath, result.config)) {
                result.error = ParseError::ParseError;
                result.message = "Failed to parse JSON file: " + filepath;
            }
        } else {
            if (!YamlParser::parseFile(filepath, result.config)) {
                result.error = ParseError::ParseError;
                result.message = "Failed to parse YAML file: " + filepath;
            }
        }
        
        return result;
    }
    
    ConfigParser::ParseResult ConfigParser::parseContent(const std::string& content, const std::string& extension) {
        ParseResult result;
        result.error = ParseError::Success;
        
        std::string ext = extension;
        if (ext.empty()) {
            ext = ".json"; // Default to JSON
        } else if (ext[0] != '.') {
            ext = "." + ext;
        }
        
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        bool isJson = std::find(JSON_EXTENSIONS.begin(), JSON_EXTENSIONS.end(), ext) != JSON_EXTENSIONS.end();
        bool isYaml = std::find(YAML_EXTENSIONS.begin(), YAML_EXTENSIONS.end(), ext) != YAML_EXTENSIONS.end();
        
        if (!isJson && !isYaml) {
            result.error = ParseError::InvalidFormat;
            result.message = "Unsupported file extension: " + ext;
            return result;
        }
        
        // Parse based on content type
        if (isJson) {
            if (!JsonParser::parse(content, result.config)) {
                result.error = ParseError::ParseError;
                result.message = "Failed to parse JSON content";
            }
        } else {
            if (!YamlParser::parse(content, result.config)) {
                result.error = ParseError::ParseError;
                result.message = "Failed to parse YAML content";
            }
        }
        
        return result;
    }
    
    ConfigParser::ParseResult ConfigParser::parseCommandLine(int argc, char** argv) {
        ParseResult result;
        result.error = ParseError::Success;
        
        std::unordered_map<std::string, ConfigValue> config;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    config[key] = value;
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                config[key] = value;
            }
        }
        
        result.config = std::move(config);
        return result;
    }
    
    ConfigParser::ParseResult ConfigParser::parseEnvironment(const std::unordered_map<std::string, std::string>& envVars) {
        ParseResult result;
        result.error = ParseError::Success;
        
        std::unordered_map<std::string, ConfigValue> config;
        
        for (const auto& [key, value] : envVars) {
            // Check if this is a NLM-related environment variable
            if (key.find("NLM_") == 0 || key.find("config_") == 0) {
                // Convert string value to appropriate type
                ConfigValue configValue;
                
                // Try to parse as boolean
                if (value == "true" || value == "false") {
                    configValue = (value == "true");
                } else {
                    // Try to parse as number
                    try {
                        size_t endpos;
                        double numVal = std::stod(value, &endpos);
                        if (endpos == value.size()) {
                            // Check if it's an integer
                            size_t intEndpos;
                            long long intVal = std::stoll(value, &intEndpos);
                            if (intEndpos == value.size()) {
                                configValue = intVal;
                            } else {
                                configValue = numVal;
                            }
                        } else {
                            configValue = numVal;
                        }
                    } catch (...) {
                        // Keep as string
                        configValue = value;
                    }
                }
                
                config[key] = std::move(configValue);
            }
        }
        
        result.config = std::move(config);
        return result;
    }
    
    std::vector<ConfigEntry> ConfigParser::convertToConfigEntries(const std::unordered_map<std::string, ConfigValue>& config,
                                                                 ConfigSource source) {
        std::vector<ConfigEntry> entries;
        
        for (const auto& [key, value] : config) {
            entries.emplace_back(key, value, source);
        }
        
        return entries;
    }
    
    std::unordered_map<std::string, ConfigValue> ConfigParser::mergeConfigs(const std::vector<std::unordered_map<std::string, ConfigValue>>& configs) {
        std::unordered_map<std::string, ConfigValue> merged;
        
        for (const auto& config : configs) {
            for (const auto& [key, value] : config) {
                // Later configs override earlier ones
                merged[key] = value;
            }
        }
        
        return merged;
    }
    
    template<typename T>
    bool ConfigParser::convertToType(const ConfigValue& value, T& result) {
        return ConfigValueConverter::tryConvert(value, result);
    }
    
    template<typename T>
    ConfigValue ConfigParser::convertFromType(const T& value) {
        return ConfigValueConverter::convert(value);
    }
}

// ConfigValueConverter Implementation
namespace {
    template<typename T>
    T ConfigValueConverter::convert(const ConfigValue& value) {
        T result;
        if (!tryConvert(value, result)) {
            throw std::runtime_error("Cannot convert ConfigValue to type " + getTypeName(value));
        }
        return result;
    }
    
    template<typename T>
    bool ConfigValueConverter::tryConvert(const ConfigValue& value, T& result) {
        if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(value)) {
                result = std::get<std::string>(value);
                return true;
            }
        } else if constexpr (std::is_same_v<T, int>) {
            if (std::holds_alternative<int>(value)) {
                result = std::get<int>(value);
                return true;
            } else if (std::holds_alternative<int64_t>(value)) {
                result = static_cast<int>(std::get<int64_t>(value));
                return true;
            } else if (std::holds_alternative<double>(value)) {
                result = static_cast<int>(std::get<double>(value));
                return true;
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<int64_t>(value)) {
                result = std::get<int64_t>(value);
                return true;
            } else if (std::holds_alternative<int>(value)) {
                result = static_cast<int64_t>(std::get<int>(value));
                return true;
            } else if (std::holds_alternative<double>(value)) {
                result = static_cast<int64_t>(std::get<double>(value));
                return true;
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(value)) {
                result = std::get<double>(value);
                return true;
            } else if (std::holds_alternative<int>(value)) {
                result = static_cast<double>(std::get<int>(value));
                return true;
            } else if (std::holds_alternative<int64_t>(value)) {
                result = static_cast<double>(std::get<int64_t>(value));
                return true;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(value)) {
                result = std::get<bool>(value);
                return true;
            } else if (std::holds_alternative<std::string>(value)) {
                const std::string& str = std::get<std::string>(value);
                if (str == "true" || str == "True" || str == "1") {
                    result = true;
                    return true;
                } else if (str == "false" || str == "False" || str == "0") {
                    result = false;
                    return true;
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return variantToVector(value, result);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return variantToVector(value, result);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return variantToVector(value, result);
        }
        
        return false;
    }
    
    template<typename T>
    bool ConfigValueConverter::isConvertible(const ConfigValue& value) {
        T dummy;
        return tryConvert(value, dummy);
    }
    
    std::string ConfigValueConverter::getTypeName(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64_t";
        if (std::holds_alternative<double>(value)) return "double";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
        if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
        if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
        return "unknown";
    }
    
    template<typename T>
    bool ConfigValueConverter::vectorToVariant(const std::vector<T>& vec, ConfigValue& result) {
        if constexpr (std::is_same_v<T, int>) {
            result = vec;
            return true;
        } else if constexpr (std::is_same_v<T, double>) {
            result = vec;
            return true;
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = vec;
            return true;
        }
        return false;
    }
    
    template<typename T>
    bool ConfigValueConverter::variantToVector(const ConfigValue& value, std::vector<T>& result) {
        if constexpr (std::is_same_v<T, int>) {
            if (std::holds_alternative<std::vector<int>>(value)) {
                result = std::get<std::vector<int>>(value);
                return true;
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                const auto& vec = std::get<std::vector<double>>(value);
                result.clear();
                for (double val : vec) {
                    result.push_back(static_cast<int>(val));
                }
                return true;
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                return false; // Cannot convert string vector to int vector
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<std::vector<double>>(value)) {
                result = std::get<std::vector<double>>(value);
                return true;
            } else if (std::holds_alternative<std::vector<int>>(value)) {
                const auto& vec = std::get<std::vector<int>>(value);
                result.clear();
                for (int val : vec) {
                    result.push_back(static_cast<double>(val));
                }
                return true;
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                return false; // Cannot convert string vector to double vector
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::vector<std::string>>(value)) {
                result = std::get<std::vector<std::string>>(value);
                return true;
            }
        }
        return false;
    }
}

// ConfigValidator Implementation
namespace {
    void ConfigValidator::addRule(const std::string& key, const std::string& type, bool required) {
        ValidationRule rule;
        rule.key = key;
        rule.type = type;
        rule.required = required;
        rules.push_back(rule);
    }
    
    void ConfigValidator::addRangeRule(const std::string& key, double min, double max) {
        ValidationRule rule;
        rule.key = key;
        rule.type = "number"; // Generic number type
        rule.min = min;
        rule.max = max;
        rule.required = false;
        rules.push_back(rule);
    }
    
    void ConfigValidator::addPatternRule(const std::string& key, const std::string& pattern) {
        ValidationRule rule;
        rule.key = key;
        rule.type = "string";
        rule.pattern = pattern;
        rule.required = false;
        rules.push_back(rule);
    }
    
    bool ConfigValidator::validate(const std::vector<ConfigEntry>& entries) const {
        errors.clear();
        
        for (const auto& rule : rules) {
            bool found = false;
            for (const auto& entry : entries) {
                if (entry.key == rule.key) {
                    found = true;
                    
                    // Check type
                    bool typeValid = false;
                    if (rule.type == "int") {
                        typeValid = std::holds_alternative<int>(entry.value) ||
                                   std::holds_alternative<int64_t>(entry.value) ||
                                   std::holds_alternative<double>(entry.value);
                    } else if (rule.type == "double") {
                        typeValid = std::holds_alternative<double>(entry.value) ||
                                   std::holds_alternative<int>(entry.value) ||
                                   std::holds_alternative<int64_t>(entry.value);
                    } else if (rule.type == "string") {
                        typeValid = std::holds_alternative<std::string>(entry.value);
                    } else if (rule.type == "bool") {
                        typeValid = std::holds_alternative<bool>(entry.value);
                    } else if (rule.type == "number") {
                        typeValid = std::holds_alternative<int>(entry.value) ||
                                   std::holds_alternative<int64_t>(entry.value) ||
                                   std::holds_alternative<double>(entry.value);
                    } else if (rule.type == "array") {
                        typeValid = std::holds_alternative<std::vector<int>>(entry.value) ||
                                   std::holds_alternative<std::vector<double>>(entry.value) ||
                                   std::holds_alternative<std::vector<std::string>>(entry.value);
                    }
                    
                    if (!typeValid) {
                        errors.push_back("Value for key \"" + rule.key + "\" has wrong type. Expected " + rule.type);
                    }
                    
                    // Check range
                    if (rule.min.has_value() || rule.max.has_value()) {
                        double value = 0.0;
                        if (std::holds_alternative<int>(entry.value)) {
                            value = std::get<int>(entry.value);
                        } else if (std::holds_alternative<int64_t>(entry.value)) {
                            value = static_cast<double>(std::get<int64_t>(entry.value));
                        } else if (std::holds_alternative<double>(entry.value)) {
                            value = std::get<double>(entry.value);
                        }
                        
                        if (rule.min.has_value() && value < rule.min.value()) {
                            errors.push_back("Value for key \"" + rule.key + "\" is below minimum " + std::to_string(rule.min.value()));
                        }
                        
                        if (rule.max.has_value() && value > rule.max.value()) {
                            errors.push_back("Value for key \"" + rule.key + "\" is above maximum " + std::to_string(rule.max.value()));
                        }
                    }
                    
                    // Check pattern
                    if (rule.pattern.has_value() && std::holds_alternative<std::string>(entry.value)) {
                        const std::string& str = std::get<std::string>(entry.value);
                        std::regex patternRegex(rule.pattern.value());
                        if (!std::regex_match(str, patternRegex)) {
                            errors.push_back("Value for key \"" + rule.key + "\" does not match pattern \"" + rule.pattern.value() + "\"");
                        }
                    }
                    
                    break;
                }
            }
            
            if (!found && rule.required) {
                errors.push_back("Required key \"" + rule.key + "\" is missing");
            }
        }
        
        return errors.empty();
    }
    
    std::vector<std::string> ConfigValidator::getErrors() const {
        return errors;
    }
}

// Config Implementation
namespace {
    struct Config::Impl {
        std::vector<ConfigEntry> entries;
        std::unordered_map<std::string, std::string> aliases;
    };
    
    Config::Config() : pImpl(std::make_unique<Impl>()), loggingEnabled(true) {}
    
    Config::~Config() = default;
    
    Config::Config(Config&&) noexcept = default;
    
    Config& Config::operator=(Config&&) noexcept = default;
    
    bool Config::loadFromFile(const std::string& filepath) {
        if (loggingEnabled) {
            logInfo("Loading configuration from file: " + filepath);
        }
        
        auto result = parser.parseFile(filepath);
        
        if (result.error != ConfigParser::ParseError::Success) {
            addError(result.message);
            return false;
        }
        
        // Convert to ConfigEntry format
        auto entries = ConfigParser::convertToConfigEntries(result.config, ConfigSource::File);
        pImpl->entries.insert(pImpl->entries.end(), entries.begin(), entries.end());
        
        return true;
    }
    
    bool Config::loadFromArgs(int argc, char** argv) {
        if (loggingEnabled) {
            logInfo("Loading configuration from command line arguments");
        }
        
        auto result = parser.parseCommandLine(argc, argv);
        
        if (result.error != ConfigParser::ParseError::Success) {
            addError(result.message);
            return false;
        }
        
        // Convert to ConfigEntry format
        auto entries = ConfigParser::convertToConfigEntries(result.config, ConfigSource::CommandLine);
        pImpl->entries.insert(pImpl->entries.end(), entries.begin(), entries.end());
        
        return true;
    }
    
    bool Config::loadFromEnvironment() {
        if (loggingEnabled) {
            logInfo("Loading configuration from environment variables");
        }
        
        // Get environment variables
        std::unordered_map<std::string, std::string> envVars;
        for (const auto& env : std::vector<std::string>{
            "NLM_*", "CONFIG_*", "BRAIN_*", "MEMORY_*", "NEUROMODULATION_*"
        }) {
            // In a real implementation, we would iterate over actual environment variables
            // For this simplified version, we'll just use some example variables
        }
        
        // Use example environment variables
        envVars["NLM_BRAIN_NEURON_COUNT"] = "1000";
        envVars["NLM_MEMORY_CAPACITY"] = "5000";
        envVars["NLM_PREDICTION_CONFIDENCE"] = "0.9";
        
        auto result = parser.parseEnvironment(envVars);
        
        if (result.error != ConfigParser::ParseError::Success) {
            addError(result.message);
            return false;
        }
        
        // Convert to ConfigEntry format
        auto entries = ConfigParser::convertToConfigEntries(result.config, ConfigSource::Environment);
        pImpl->entries.insert(pImpl->entries.end(), entries.begin(), entries.end());
        
        return true;
    }
    
    bool Config::loadFromDefaults() {
        if (loggingEnabled) {
            logInfo("Loading default configuration values");
        }
        
        // Define default configuration values
        std::unordered_map<std::string, ConfigValue> defaults = {
            {"brain.neuron_count", 100},
            {"brain.simulation_time_step", 0.1},
            {"memory.capacity", 1000},
            {"neuromodulation.reward_scale", 1.0},
            {"prediction.confidence_threshold", 0.7},
            {"cognition.planning_depth", 3},
            {"development.plasticity_rate", 0.1}
        };
        
        auto entries = ConfigParser::convertToConfigEntries(defaults, ConfigSource::Default);
        pImpl->entries.insert(pImpl->entries.end(), entries.begin(), entries.end());
        
        return true;
    }
    
    bool Config::loadWithInheritance(const std::vector<std::string>& filePaths) {
        if (loggingEnabled) {
            logInfo("Loading configuration with inheritance from " + std::to_string(filePaths.size()) + " files");
        }
        
        std::vector<std::unordered_map<std::string, ConfigValue>> configs;
        
        for (const auto& filepath : filePaths) {
            auto result = parser.parseFile(filepath);
            if (result.error != ConfigParser::ParseError::Success) {
                addError("Failed to load configuration file " + filepath + ": " + result.message);
                continue;
            }
            
            configs.push_back(std::move(result.config));
        }
        
        // Merge configurations (later files override earlier ones)
        auto merged = parser.mergeConfigs(configs);
        
        // Convert to ConfigEntry format
        auto entries = ConfigParser::convertToConfigEntries(merged, ConfigSource::File);
        pImpl->entries.insert(pImpl->entries.end(), entries.begin(), entries.end());
        
        return true;
    }
    
    bool Config::saveToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            // Format based on value type
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                } else if constexpr (std::is_same_v<T, double>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        file << arg[i];
                        if (i < arg.size() - 1) file << ", ";
                    }
                    file << "]\n";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        file << arg[i];
                        if (i < arg.size() - 1) file << ", ";
                    }
                    file << "]\n";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << entry.key << " = [\"";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        file << arg[i];
                        if (i < arg.size() - 1) file << "\", \";
                    }
                    file << "\"]\n";
                } else {
                    file << entry.key << " = " << arg << "\n";
                }
            }, entry.value);
            
            file << "\n";
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
            return ConfigValueConverter::convert<T>(it->value);
        } catch (const std::runtime_error& e) {
            addError("Failed to convert value for key \"" + key + "\": " + e.what());
            return std::nullopt;
        }
    }
    
    template<typename T>
    T Config::getOr(const std::string& key, const T& defaultValue) const {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
    
    template<typename T>
    std::optional<T> Config::getNested(const std::string& keyPath) const {
        // Parse dot notation
        std::vector<std::string> pathComponents;
        std::string current;
        
        for (char c : keyPath) {
            if (c == '.') {
                if (!current.empty()) {
                    pathComponents.push_back(current);
                    current.clear();
                }
            } else {
                current += c;
            }
        }
        
        if (!current.empty()) {
            pathComponents.push_back(current);
        }
        
        // Navigate through the path
        std::unordered_map<std::string, ConfigValue>* currentMap = nullptr;
        
        // Start with the full entries
        std::unordered_map<std::string, ConfigValue> tempMap;
        for (const auto& entry : pImpl->entries) {
            // For simplicity, we treat all entries as flat keys
            tempMap[entry.key] = entry.value;
        }
        
        currentMap = &tempMap;
        
        for (size_t i = 0; i < pathComponents.size(); ++i) {
            const std::string& component = pathComponents[i];
            
            auto it = currentMap->find(component);
            if (it == currentMap->end()) {
                return std::nullopt;
            }
            
            if (i == pathComponents.size() - 1) {
                // Final component - convert to type T
                try {
                    return ConfigValueConverter::convert<T>(it->second);
                } catch (const std::runtime_error& e) {
                    addError("Failed to convert nested value for key path \"" + keyPath + "\": " + e.what());
                    return std::nullopt;
                }
            }
            
            // Not the final component - check if it's a mapping
            if (std::holds_alternative<std::unordered_map<std::string, ConfigValue>>(it->second)) {
                currentMap = &std::get<std::unordered_map<std::string, ConfigValue>>(it->second);
            } else {
                return std::nullopt; // Not a nested object
            }
        }
        
        return std::nullopt;
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
    
    bool Config::hasNested(const std::string& keyPath) const {
        return getNested<std::string>(keyPath).has_value();
    }
    
    void Config::remove(const std::string& key) {
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            pImpl->entries.end()
        );
    }
    
    void Config::removeNested(const std::string& keyPath) {
        // For now, just remove the exact key path
        // A more sophisticated implementation could handle nested structures
        remove(keyPath);
    }
    
    std::vector<std::string> Config::getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(pImpl->entries.size());
        for (const auto& entry : pImpl->entries) {
            keys.push_back(entry.key);
        }
        return keys;
    }
    
    std::vector<std::string> Config::getAllKeys() const {
        std::vector<std::string> keys;
        
        for (const auto& entry : pImpl->entries) {
            // Extract the base key (before first dot if present)
            size_t dotPos = entry.key.find('.');
            std::string baseKey = (dotPos != std::string::npos) ? entry.key.substr(0, dotPos) : entry.key;
            
            if (std::find(keys.begin(), keys.end(), baseKey) == keys.end()) {
                keys.push_back(baseKey);
            }
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
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "] (" << static_cast<int>(entry.source) << ")\n";
        }
        return oss.str();
    }
    
    Config::ConfigStats Config::getStats() const {
        ConfigStats stats;
        stats.totalEntries = pImpl->entries.size();
        
        for (const auto& entry : pImpl->entries) {
            switch (entry.source) {
                case ConfigSource::File: stats.fileEntries++; break;
                case ConfigSource::CommandLine: stats.cliEntries++; break;
                case ConfigSource::Environment: stats.envEntries++; break;
                case ConfigSource::Runtime: stats.runtimeEntries++; break;
                case ConfigSource::Default: stats.runtimeEntries++; break;
            }
            
            stats.typeCounts[ConfigValueConverter::getTypeName(entry.value)]++;
        }
        
        return stats;
    }
    
    bool Config::validate(const ConfigValidator::ValidationRules& rules) const {
        // Temporarily set validator
        ConfigValidator tempValidator = validator;
        for (const auto& rule : rules) {
            tempValidator.addRule(rule.key, rule.type, rule.required);
            if (rule.min.has_value()) {
                tempValidator.addRangeRule(rule.key, rule.min.value(), rule.max.value_or(1e9));
            }
            if (rule.pattern.has_value()) {
                tempValidator.addPatternRule(rule.key, rule.pattern.value());
            }
        }
        
        // Convert to flat entries for validation
        std::vector<ConfigEntry> flatEntries;
        for (const auto& entry : pImpl->entries) {
            flatEntries.push_back(entry);
        }
        
        bool result = tempValidator.validate(flatEntries);
        
        // Clear temporary errors
        lastErrors.clear();
        
        return result;
    }
    
    void Config::setValidator(const ConfigValidator& validator) {
        this->validator = validator;
    }
    
    void Config::setLogging(bool enabled) {
        loggingEnabled = enabled;
    }
    
    bool Config::isLogging() const {
        return loggingEnabled;
    }
    
    std::vector<std::string> Config::getLastErrors() const {
        return lastErrors;
    }
    
    void Config::logInfo(const std::string& message) const {
        if (loggingEnabled) {
            std::cout << "[INFO] " << message << std::endl;
        }
    }
    
    void Config::logError(const std::string& message) const {
        if (loggingEnabled) {
            std::cerr << "[ERROR] " << message << std::endl;
        }
    }
    
    void Config::addError(const std::string& error) {
        lastErrors.push_back(error);
        logError(error);
    }
    
    std::optional<ConfigEntry> Config::findNestedEntry(const std::string& keyPath) const {
        // Parse dot notation
        std::vector<std::string> pathComponents;
        std::string current;
        
        for (char c : keyPath) {
            if (c == '.') {
                if (!current.empty()) {
                    pathComponents.push_back(current);
                    current.clear();
                }
            } else {
                current += c;
            }
        }
        
        if (!current.empty()) {
            pathComponents.push_back(current);
        }
        
        // Find the entry that matches this path
        for (const auto& entry : pImpl->entries) {
            size_t dotPos = entry.key.find('.');
            if (dotPos != std::string::npos) {
                std::string baseKey = entry.key.substr(0, dotPos);
                if (baseKey == keyPath) {
                    return entry;
                }
            }
        }
        
        return std::nullopt;
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

// Nested get instantiations
namespace {
template std::optional<int> Config::getNested<int>(const std::string&) const;
template std::optional<double> Config::getNested<double>(const std::string&) const;
template std::optional<bool> Config::getNested<bool>(const std::string&) const;
template std::optional<std::string> Config::getNested<std::string>(const std::string&) const;
}
}