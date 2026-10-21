#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <unordered_map>
#include <stack>
#include <regex>

// JSON/YAML Parser implementation
namespace {

// JSON value types
enum class JsonValueType {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
};

struct JsonValue {
    JsonValueType type;
    union {
        bool boolVal;
        double numberVal;
        std::string* stringVal;
        std::vector<JsonValue>* arrayVal;
        std::unordered_map<std::string, JsonValue>* objectVal;
    } value;
    
    JsonValue() : type(JsonValueType::JSON_NULL) {}
    ~JsonValue() {
        switch (type) {
            case JsonValueType::JSON_STRING:
                delete value.stringVal;
                break;
            case JsonValueType::JSON_ARRAY:
                delete value.arrayVal;
                break;
            case JsonValueType::JSON_OBJECT:
                delete value.objectVal;
                break;
            default:
                break;
        }
    }
};

// YAML value types
enum class YamlValueType {
    YAML_NULL,
    YAML_BOOL,
    YAML_NUMBER,
    YAML_STRING,
    YAML_SEQUENCE,
    YAML_MAPPING
};

struct YamlValue {
    YamlValueType type;
    union {
        bool boolVal;
        double numberVal;
        std::string* stringVal;
        std::vector<YamlValue>* sequenceVal;
        std::unordered_map<std::string, YamlValue>* mappingVal;
    } value;
    
    YamlValue() : type(YamlValueType::YAML_NULL) {}
    ~YamlValue() {
        switch (type) {
            case YamlValueType::YAML_STRING:
                delete value.stringVal;
                break;
            case YamlValueType::YAML_SEQUENCE:
                delete value.sequenceVal;
                break;
            case YamlValueType::YAML_MAPPING:
                delete value.mappingVal;
                break;
            default:
                break;
        }
    }
};

// Helper functions for JSON parsing
JsonValue parseJsonValue(const std::string& str, size_t& pos);
std::string parseJsonString(const std::string& str, size_t& pos);
size_t skipWhitespace(const std::string& str, size_t pos);
bool parseJsonBool(const std::string& str, size_t& pos);
double parseJsonNumber(const std::string& str, size_t& pos);
std::vector<JsonValue> parseJsonArray(const std::string& str, size_t& pos);
std::unordered_map<std::string, JsonValue> parseJsonObject(const std::string& str, size_t& pos);

// Helper functions for YAML parsing
YamlValue parseYamlValue(const std::string& str, size_t& pos, bool& expectingKey);
std::string parseYamlString(const std::string& str, size_t& pos);
double parseYamlNumber(const std::string& str, size_t& pos);
std::vector<YamlValue> parseYamlSequence(const std::string& str, size_t& pos, bool& expectingKey);
std::unordered_map<std::string, YamlValue> parseYamlMapping(const std::string& str, size_t& pos, bool& expectingKey);

// Convert JsonValue to ConfigValue
ConfigValue jsonValueToConfigValue(const JsonValue& jsonVal);
ConfigValue yamlValueToConfigValue(const YamlValue& yamlVal);

// Main parsing functions
JsonValue parseJson(const std::string& content);
YamlValue parseYaml(const std::string& content);

} // namespace nlm

// Helper conversion functions for JSON/YAML to Config
void convertJsonToConfig(const JsonValue& jsonVal, ConfigSource source, std::vector<ConfigEntry>& entries);
void convertYamlToConfig(const YamlValue& yamlVal, ConfigSource source, std::vector<ConfigEntry>& entries);

// JSON parsing implementation
namespace {

// Helper functions for JSON parsing
size_t skipWhitespace(const std::string& str, size_t pos) {
    while (pos < str.size() && std::isspace(str[pos])) {
        pos++;
    }
    return pos;
}

std::string parseJsonString(const std::string& str, size_t& pos) {
    size_t start = ++pos; // Skip opening quote
    while (pos < str.size() && str[pos] != '\"') {
        if (str[pos] == '\\' && pos + 1 < str.size()) {
            pos += 2; // Skip escaped character
        }
        pos++;
    }
    std::string result = str.substr(start, pos - start);
    pos++; // Skip closing quote
    return result;
}

bool parseJsonBool(const std::string& str, size_t& pos) {
    std::string token = str.substr(pos, 4);
    if (token == "true") {
        pos += 4;
        return true;
    } else if (token == "false") {
        pos += 5;
        return false;
    }
    return false;
}

double parseJsonNumber(const std::string& str, size_t& pos) {
    size_t start = pos;
    while (pos < str.size() && (std::isdigit(str[pos]) || str[pos] == '.' || str[pos] == '-' || str[pos] == '+' || str[pos] == 'e' || str[pos] == 'E')) {
        pos++;
    }
    return std::stod(str.substr(start, pos - start));
}

JsonValue parseJsonArray(const std::string& str, size_t& pos);
std::unordered_map<std::string, JsonValue> parseJsonObject(const std::string& str, size_t& pos);

JsonValue parseJsonValue(const std::string& str, size_t& pos) {
    pos = skipWhitespace(str, pos);
    
    if (pos >= str.size()) {
        return JsonValue{};
    }
    
    char c = str[pos];
    if (c == 'n' && str.substr(pos, 4) == "null") {
        JsonValue val;
        val.type = JsonValueType::JSON_NULL;
        pos += 4;
        return val;
    } else if (c == 't' || c == 'f') {
        bool b = parseJsonBool(str, pos);
        JsonValue val;
        val.type = JsonValueType::JSON_BOOL;
        val.value.boolVal = b;
        return val;
    } else if (c == '\"') {
        std::string s = parseJsonString(str, pos);
        JsonValue val;
        val.type = JsonValueType::JSON_STRING;
        val.value.stringVal = new std::string(s);
        return val;
    } else if (c == '[') {
        return parseJsonArray(str, pos);
    } else if (c == '{') {
        return parseJsonObject(str, pos);
    } else {
        double n = parseJsonNumber(str, pos);
        JsonValue val;
        val.type = JsonValueType::JSON_NUMBER;
        val.value.numberVal = n;
        return val;
    }
}

std::vector<JsonValue> parseJsonArray(const std::string& str, size_t& pos) {
    std::vector<JsonValue> result;
    pos = skipWhitespace(str, pos + 1); // Skip '['
    
    if (str[pos] == ']') {
        pos++;
        return result;
    }
    
    while (pos < str.size()) {
        pos = skipWhitespace(str, pos);
        JsonValue val = parseJsonValue(str, pos);
        result.push_back(val);
        
        pos = skipWhitespace(str, pos);
        if (str[pos] == ']') {
            pos++;
            break;
        } else if (str[pos] == ',') {
            pos++;
        }
    }
    
    return result;
}

std::unordered_map<std::string, JsonValue> parseJsonObject(const std::string& str, size_t& pos) {
    std::unordered_map<std::string, JsonValue> result;
    pos = skipWhitespace(str, pos + 1); // Skip '{'
    
    if (str[pos] == '}') {
        pos++;
        return result;
    }
    
    while (pos < str.size()) {
        pos = skipWhitespace(str, pos);
        
        if (str[pos] == '\"') {
            std::string key = parseJsonString(str, pos);
            pos = skipWhitespace(str, pos);
            
            if (str[pos] != ':') {
                break;
            }
            
            pos = skipWhitespace(str, pos + 1); // Skip ':'
            JsonValue val = parseJsonValue(str, pos);
            result[key] = val;
            
            pos = skipWhitespace(str, pos);
            if (str[pos] == '}') {
                pos++;
                break;
            } else if (str[pos] == ',') {
                pos++;
            }
        } else {
            break;
        }
    }
    
    return result;
}

JsonValue parseJson(const std::string& content) {
    size_t pos = 0;
    pos = skipWhitespace(content, pos);
    return parseJsonValue(content, pos);
}

// YAML parsing implementation
double parseYamlNumber(const std::string& str, size_t& pos) {
    size_t start = pos;
    while (pos < str.size() && (std::isdigit(str[pos]) || str[pos] == '.' || str[pos] == '-' || str[pos] == '+' || str[pos] == 'e' || str[pos] == 'E')) {
        pos++;
    }
    return std::stod(str.substr(start, pos - start));
}

std::string parseYamlString(const std::string& str, size_t& pos) {
    size_t start = pos;
    while (pos < str.size() && str[pos] != '\'' && str[pos] != '\"') {
        pos++;
    }
    std::string result = str.substr(start, pos - start);
    pos++; // Skip closing quote
    return result;
}

YamlValue parseYamlSequence(const std::string& str, size_t& pos, bool& expectingKey);
std::unordered_map<std::string, YamlValue> parseYamlMapping(const std::string& str, size_t& pos, bool& expectingKey);

YamlValue parseYamlValue(const std::string& str, size_t& pos, bool& expectingKey) {
    pos = skipWhitespace(str, pos);
    
    if (pos >= str.size()) {
        YamlValue val;
        val.type = YamlValueType::YAML_NULL;
        return val;
    }
    
    char c = str[pos];
    if (c == '-' || c == '\'' || c == '\"' || c == '?' || c == '[' || c == '{') {
        // YAML sequence or mapping
        if (c == '[') {
            std::vector<YamlValue> seq = parseYamlSequence(str, pos, expectingKey);
            YamlValue val;
            val.type = YamlValueType::YAML_SEQUENCE;
            val.value.sequenceVal = new std::vector<YamlValue>(seq);
            return val;
        } else if (c == '{') {
            std::unordered_map<std::string, YamlValue> mapping = parseYamlMapping(str, pos, expectingKey);
            YamlValue val;
            val.type = YamlValueType::YAML_MAPPING;
            val.value.mappingVal = new std::unordered_map<std::string, YamlValue>(mapping);
            return val;
        }
    }
    
    // Parse simple value
    size_t start = pos;
    while (pos < str.size() && c != ':' && c != '\n' && c != ' ' && c != '\t') {
        pos++;
        if (pos < str.size()) {
            c = str[pos];
        }
    }
    
    std::string token = str.substr(start, pos - start);
    YamlValue val;
    
    if (token == "null" || token.empty()) {
        val.type = YamlValueType::YAML_NULL;
    } else if (token == "true" || token == "false") {
        val.type = YamlValueType::YAML_BOOL;
        val.value.boolVal = (token == "true");
    } else if (std::isdigit(token[0]) || (token[0] == '-' && token.size() > 1)) {
        val.type = YamlValueType::YAML_NUMBER;
        val.value.numberVal = parseYamlNumber(str, start);
    } else {
        val.type = YamlValueType::YAML_STRING;
        val.value.stringVal = new std::string(token);
    }
    
    return val;
}

std::vector<YamlValue> parseYamlSequence(const std::string& str, size_t& pos, bool& expectingKey) {
    std::vector<YamlValue> result;
    pos = skipWhitespace(str, pos + 1); // Skip '[' or '-'
    
    while (pos < str.size()) {
        pos = skipWhitespace(str, pos);
        
        if (str[pos] == ']') {
            pos++;
            break;
        }
        
        if (str[pos] == '-') {
            pos++;
        }
        
        YamlValue val = parseYamlValue(str, pos, expectingKey);
        result.push_back(val);
        
        pos = skipWhitespace(str, pos);
        if (str[pos] == ']') {
            pos++;
            break;
        } else if (str[pos] == ',') {
            pos++;
        }
    }
    
    return result;
}

std::unordered_map<std::string, YamlValue> parseYamlMapping(const std::string& str, size_t& pos, bool& expectingKey) {
    std::unordered_map<std::string, YamlValue> result;
    pos = skipWhitespace(str, pos + 1); // Skip '{'
    
    expectingKey = true;
    
    while (pos < str.size()) {
        pos = skipWhitespace(str, pos);
        
        if (str[pos] == '}') {
            pos++;
            break;
        }
        
        if (expectingKey) {
            // Parse key
            if (str[pos] == '\'' || str[pos] == '\"') {
                std::string key = parseYamlString(str, pos);
                pos = skipWhitespace(str, pos);
                
                if (str[pos] != ':') {
                    break;
                }
                
                pos = skipWhitespace(str, pos + 1); // Skip ':'
                expectingKey = false;
            } else {
                break;
            }
        } else {
            YamlValue val = parseYamlValue(str, pos, expectingKey);
            result["item"] = val; // For simple mappings, use "item" as key
            expectingKey = true;
            
            pos = skipWhitespace(str, pos);
            if (str[pos] == '}') {
                pos++;
                break;
            }
        }
    }
    
    return result;
}

YamlValue parseYaml(const std::string& content) {
    bool expectingKey = true;
    size_t pos = 0;
    pos = skipWhitespace(content, pos);
    return parseYamlValue(content, pos, expectingKey);
}

// Convert JsonValue to ConfigValue
ConfigValue jsonValueToConfigValue(const JsonValue& jsonVal) {
    switch (jsonVal.type) {
        case JsonValueType::JSON_NULL:
            return ConfigValue(std::string("null"));
        case JsonValueType::JSON_BOOL:
            return ConfigValue(jsonVal.value.boolVal);
        case JsonValueType::JSON_NUMBER:
            return ConfigValue(jsonVal.value.numberVal);
        case JsonValueType::JSON_STRING:
            return ConfigValue(*jsonVal.value.stringVal);
        case JsonValueType::JSON_ARRAY:
            // Convert array to string representation
            std::string arrayStr = "[";
            for (size_t i = 0; i < jsonVal.value.arrayVal->size(); ++i) {
                if (i > 0) arrayStr += ", ";
                arrayStr += std::to_string((*jsonVal.value.arrayVal)[i].value.numberVal);
            }
            arrayStr += "]";
            return ConfigValue(arrayStr);
        case JsonValueType::JSON_OBJECT:
            // Convert object to string representation
            std::string objStr = "{";
            bool first = true;
            for (const auto& pair : *jsonVal.value.objectVal) {
                if (!first) objStr += ", ";
                objStr += "\"" + pair.first + "\": " + std::to_string(pair.second.value.numberVal);
                first = false;
            }
            objStr += "}";
            return ConfigValue(objStr);
        default:
            return ConfigValue(std::string("unknown"));
    }
}

// Convert YamlValue to ConfigValue
ConfigValue yamlValueToConfigValue(const YamlValue& yamlVal) {
    switch (yamlVal.type) {
        case YamlValueType::YAML_NULL:
            return ConfigValue(std::string("null"));
        case YamlValueType::YAML_BOOL:
            return ConfigValue(yamlVal.value.boolVal);
        case YamlValueType::YAML_NUMBER:
            return ConfigValue(yamlVal.value.numberVal);
        case YamlValueType::YAML_STRING:
            return ConfigValue(*yamlVal.value.stringVal);
        case YamlValueType::YAML_SEQUENCE:
            // Convert sequence to string representation
            std::string seqStr = "[";
            for (size_t i = 0; i < yamlVal.value.sequenceVal->size(); ++i) {
                if (i > 0) seqStr += ", ";
                const YamlValue& item = (*yamlVal.value.sequenceVal)[i];
                if (item.type == YamlValueType::YAML_STRING) {
                    seqStr += "\"" + *item.value.stringVal + "\"";
                } else if (item.type == YamlValueType::YAML_NUMBER) {
                    seqStr += std::to_string(item.value.numberVal);
                } else {
                    seqStr += "null";
                }
            }
            seqStr += "]";
            return ConfigValue(seqStr);
        case YamlValueType::YAML_MAPPING:
            // Convert mapping to string representation
            std::string mapStr = "{";
            bool first = true;
            for (const auto& pair : *yamlVal.value.mappingVal) {
                if (!first) mapStr += ", ";
                mapStr += "\"" + pair.first + "\": \"" + *pair.second.value.stringVal + "\"";
                first = false;
            }
            mapStr += "}";
            return ConfigValue(mapStr);
        default:
            return ConfigValue(std::string("unknown"));
    }
}

// Convert JsonValue to Config entries
void convertJsonToConfig(const JsonValue& jsonVal, ConfigSource source, std::vector<ConfigEntry>& entries) {
    // For now, convert simple JSON values to a single key=value entry
    // This can be enhanced later for more complex JSON structures
    if (jsonVal.type == JsonValueType::JSON_OBJECT) {
        for (const auto& pair : *jsonVal.value.objectVal) {
            ConfigValue configVal = jsonValueToConfigValue(pair.second);
            entries.emplace_back(pair.first, configVal, source);
        }
    } else {
        entries.emplace_back("value", jsonValueToConfigValue(jsonVal), source);
    }
}

// Convert YamlValue to Config entries
void convertYamlToConfig(const YamlValue& yamlVal, ConfigSource source, std::vector<ConfigEntry>& entries) {
    // For now, convert simple YAML values to a single key=value entry
    // This can be enhanced later for more complex YAML structures
    if (yamlVal.type == YamlValueType::YAML_MAPPING) {
        for (const auto& pair : *yamlVal.value.mappingVal) {
            ConfigValue configVal = yamlValueToConfigValue(pair.second);
            entries.emplace_back(pair.first, configVal, source);
        }
    } else {
        entries.emplace_back("value", yamlValueToConfigValue(yamlVal), source);
    }
}

}} // anonymous namespace

namespace nlm {

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON/YAML first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Configuration file not found: " + filepath);
        NLM_LOG_ERROR("  Error: Cannot open file - file may not exist or permission denied");
        NLM_LOG_ERROR("  Suggestion: Check file path and permissions, or run with --config for default config");
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (content.empty()) {
        NLM_LOG_ERROR("Configuration file is empty: " + filepath);
        NLM_LOG_ERROR("  Error: File contains no data");
        NLM_LOG_ERROR("  Suggestion: Add key=value pairs or create a JSON/YAML file with configuration");
        return false;
    }
    
    // Try JSON parsing first
    try {
        JsonValue jsonRoot = parseJson(content);
        if (jsonRoot.type != JsonValueType::JSON_NULL) {
            // Convert JSON to Config entries
            convertJsonToConfig(jsonRoot, ConfigSource::File, pImpl->entries);
            NLM_LOG_INFO("Successfully parsed JSON configuration from: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Invalid JSON format in file: " + filepath);
            NLM_LOG_ERROR("  Error: JSON is empty or malformed");
            NLM_LOG_ERROR("  Suggestion: Check JSON syntax, ensure proper quoting and structure");
            return false;
        }
    } catch (const std::exception& e) {
        // JSON parsing failed, try YAML
        try {
            YamlValue yamlRoot = parseYaml(content);
            if (yamlRoot.type != YamlValueType::YAML_NULL) {
                // Convert YAML to Config entries
                convertYamlToConfig(yamlRoot, ConfigSource::File, pImpl->entries);
                NLM_LOG_INFO("Successfully parsed YAML configuration from: " + filepath);
                return true;
            } else {
                NLM_LOG_ERROR("Invalid YAML format in file: " + filepath);
                NLM_LOG_ERROR("  Error: YAML is empty or malformed");
                NLM_LOG_ERROR("  Suggestion: Check YAML syntax, use proper indentation and quotes");
                return false;
            }
        } catch (const std::exception& e) {
            // YAML parsing failed, fall back to simple key=value format
            NLM_LOG_WARNING("Neither JSON nor YAML parsing succeeded, falling back to key=value format");
            NLM_LOG_WARNING("  Parsing error: " + std::string(e.what());
        }
    }
    
    // Fall back to simple key=value format for backward compatibility
    file.clear();
    file.seekg(0, std::ios::beg);
    
    std::string line;
    int lineNumber = 0;
    bool hasValidEntries = false;
    
    while (std::getline(file, line)) {
        lineNumber++;
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
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_ERROR("Invalid configuration at line " + std::to_string(lineNumber) + ": empty key");
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
            hasValidEntries = true;
        } else {
            NLM_LOG_WARNING("Skipping invalid configuration line " + std::to_string(lineNumber) + ": " + line);
            NLM_LOG_WARNING("  Expected format: key = value or key=value");
        }
    }
    
    if (!hasValidEntries) {
        NLM_LOG_ERROR("No valid configuration entries found in: " + filepath);
        NLM_LOG_ERROR("  Error: File contains only comments or empty lines");
        NLM_LOG_ERROR("  Suggestion: Add key=value pairs or use JSON/YAML format");
        return false;
    }
    
    NLM_LOG_INFO("Successfully parsed key=value configuration from: " + filepath);
    NLM_LOG_INFO("  Loaded " + std::to_string(pImpl->entries.size()) + " configuration entries");
    return true;
}
