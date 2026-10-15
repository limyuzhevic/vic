// Simple JSON parser implementation for NLM configuration
// Provides basic JSON parsing for configuration files

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <unordered_map>
#include <iomanip>

namespace nlm {

// Enhanced Config class with JSON support and better error handling
Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;
    std::vector<JSONValue> arrayValue_;
    std::vector<std::pair<std::string, JSONValue>> objectValue_;
};

// Simple JSON parser
class JSONParser {
public:
    static bool parse(const std::string& json, JSONValue& root) {
        if (json.empty()) {
            return false;
        }
        
        // Skip whitespace
        size_t pos = 0;
        skipWhitespace(json, pos);
        
        // Parse JSON
        if (!parseValue(json, pos, root)) {
            return false;
        }
        
        // Ensure we consumed the entire input
        skipWhitespace(json, pos);
        if (pos != json.size()) {
            return false;
        }
        
        return true;
    }
    
private:
    static void skipWhitespace(const std::string& json, size_t& pos) {
        while (pos < json.size() && std::isspace(json[pos])) {
            pos++;
        }
    }
    
    static bool parseValue(const std::string& json, size_t& pos, JSONValue& value) {
        skipWhitespace(json, pos);
        
        if (pos >= json.size()) {
            return false;
        }
        
        switch (json[pos]) {
            case '{':
                return parseObject(json, pos, value);
            case '[':
                return parseArray(json, pos, value);
            case '"':
                return parseString(json, pos, value);
            case 't':
            case 'f':
                return parseBoolean(json, pos, value);
            case 'n':
                return parseNull(json, pos, value);
            default:
                return parseNumber(json, pos, value);
        }
    }
    
    static bool parseObject(const std::string& json, size_t& pos, JSONValue& value) {
        if (json[pos] != '{') {
            return false;
        }
        
        pos++; // Skip '{'
        skipWhitespace(json, pos);
        
        std::vector<std::pair<std::string, JSONValue>> members;
        
        if (json[pos] == '}') {
            pos++; // Skip '}'
            value = JSONValue(members);
            return true;
        }
        
        while (true) {
            // Parse key
            std::string key;
            if (!parseString(json, pos, key)) {
                return false;
            }
            
            skipWhitespace(json, pos);
            
            if (pos >= json.size() || json[pos] != ':') {
                return false;
            }
            
            pos++; // Skip ':'
            skipWhitespace(json, pos);
            
            // Parse value
            JSONValue memberValue;
            if (!parseValue(json, pos, memberValue)) {
                return false;
            }
            
            members.push_back({key, memberValue});
            skipWhitespace(json, pos);
            
            if (pos >= json.size()) {
                return false;
            }
            
            if (json[pos] == '}') {
                pos++; // Skip '}'
                break;
            } else if (json[pos] == ',') {
                pos++; // Skip ','
                skipWhitespace(json, pos);
            } else {
                return false;
            }
        }
        
        value = JSONValue(members);
        return true;
    }
    
    static bool parseArray(const std::string& json, size_t& pos, JSONValue& value) {
        if (json[pos] != '[') {
            return false;
        }
        
        pos++; // Skip '['
        skipWhitespace(json, pos);
        
        std::vector<JSONValue> elements;
        
        if (json[pos] == ']') {
            pos++; // Skip ']'
            value = JSONValue(elements);
            return true;
        }
        
        while (true) {
            // Parse element
            JSONValue element;
            if (!parseValue(json, pos, element)) {
                return false;
            }
            
            elements.push_back(element);
            skipWhitespace(json, pos);
            
            if (pos >= json.size()) {
                return false;
            }
            
            if (json[pos] == ']') {
                pos++; // Skip ']'
                break;
            } else if (json[pos] == ',') {
                pos++; // Skip ','
                skipWhitespace(json, pos);
            } else {
                return false;
            }
        }
        
        value = JSONValue(elements);
        return true;
    }
    
    static bool parseString(const std::string& json, size_t& pos, std::string& value) {
        if (json[pos] != '"') {
            return false;
        }
        
        pos++; // Skip '"'
        size_t start = pos;
        
        while (pos < json.size() && json[pos] != '"') {
            if (json[pos] == '\\') {
                pos++; // Skip escape character
                if (pos >= json.size()) {
                    return false;
                }
            }
            pos++;
        }
        
        if (pos >= json.size() || json[pos] != '"') {
            return false;
        }
        
        value = json.substr(start, pos - start);
        pos++; // Skip '"'
        
        return true;
    }
    
    static bool parseBoolean(const std::string& json, size_t& pos, JSONValue& value) {
        if (json.compare(pos, 4, "true") == 0) {
            value = JSONValue(true);
            pos += 4;
            return true;
        } else if (json.compare(pos, 5, "false") == 0) {
            value = JSONValue(false);
            pos += 5;
            return true;
        }
        return false;
    }
    
    static bool parseNull(const std::string& json, size_t& pos, JSONValue& value) {
        if (json.compare(pos, 4, "null") == 0) {
            value = JSONValue();
            pos += 4;
            return true;
        }
        return false;
    }
    
    static bool parseNumber(const std::string& json, size_t& pos, JSONValue& value) {
        size_t start = pos;
        bool hasDecimal = false;
        bool hasExponent = false;
        
        while (pos < json.size() && (std::isdigit(json[pos]) || 
                                    json[pos] == '.' || 
                                    json[pos] == '-' || 
                                    json[pos] == '+' ||
                                    json[pos] == 'e' ||
                                    json[pos] == 'E')) {
            if (json[pos] == '.') {
                if (hasDecimal || hasExponent) {
                    return false;
                }
                hasDecimal = true;
            } else if (json[pos] == 'e' || json[pos] == 'E') {
                if (hasExponent) {
                    return false;
                }
                hasExponent = true;
            }
            pos++;
        }
        
        std::string numStr = json.substr(start, pos - start);
        
        try {
            // Try to parse as double first
            size_t pos2;
            double val = std::stod(numStr, pos2);
            if (pos2 == numStr.size()) {
                value = JSONValue(val);
                return true;
            }
            
            // Try to parse as integer
            size_t pos3;
            long long intVal = std::stoll(numStr, pos3);
            if (pos3 == numStr.size()) {
                value = JSONValue(intVal);
                return true;
            }
            
            return false;
        } catch (...) {
            return false;
        }
    }
};

// Enhanced JSON parsing implementation for configuration
bool parseJSONConfig(const std::string& filepath, Config& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();
    
    // Parse JSON
    JSONValue root;
    if (!JSONParser::parse(jsonContent, root)) {
        return false;
    }
    
    // Convert JSON to config entries
    if (root.getType() == JSONValue::Type::Object) {
        const auto& members = root.asObject();
        for (const auto& member : members) {
            const std::string& key = member.first;
            const JSONValue& jsonValue = member.second;
            
            ConfigValue configValue;
            
            switch (jsonValue.getType()) {
                case JSONValue::Type::String:
                    configValue = ConfigValue(jsonValue.asString());
                    break;
                case JSONValue::Type::Number:
                    // Try to determine if it's an integer
                    double numVal = jsonValue.asNumber();
                    if (std::floor(numVal) == numVal && std::abs(numVal) < 1e15) {
                        configValue = ConfigValue(static_cast<int64_t>(numVal));
                    } else {
                        configValue = ConfigValue(numVal);
                    }
                    break;
                case JSONValue::Type::Boolean:
                    configValue = ConfigValue(jsonValue.asBool());
                    break;
                case JSONValue::Type::Array:
                    {
                        const auto& array = jsonValue.asArray();
                        std::vector<double> doubleArray;
                        bool allNumbers = true;
                        
                        for (const auto& item : array) {
                            if (item.getType() == JSONValue::Type::Number) {
                                doubleArray.push_back(item.asNumber());
                            } else if (item.getType() == JSONValue::Type::String) {
                                // Try to parse string as number
                                try {
                                    size_t pos;
                                    double val = std::stod(item.asString(), pos);
                                    doubleArray.push_back(val);
                                } catch (...) {
                                    allNumbers = false;
                                }
                            } else {
                                allNumbers = false;
                            }
                        }
                        
                        if (allNumbers) {
                            configValue = ConfigValue(doubleArray);
                        } else {
                            // Create string representation of array
                            std::string arrayStr = "[";
                            for (size_t i = 0; i < array.size(); ++i) {
                                if (i > 0) arrayStr += ", ";
                                switch (array[i].getType()) {
                                    case JSONValue::Type::String:
                                        arrayStr += "\"" + array[i].asString() + "\"";
                                        break;
                                    case JSONValue::Type::Number:
                                        arrayStr += std::to_string(array[i].asNumber());
                                        break;
                                    case JSONValue::Type::Boolean:
                                        arrayStr += array[i].asBool() ? "true" : "false";
                                        break;
                                    case JSONValue::Type::Null:
                                        arrayStr += "null";
                                        break;
                                    case JSONValue::Type::Array:
                                    case JSONValue::Type::Object:
                                        arrayStr += "[...]";
                                        break;
                                }
                            }
                            arrayStr += "]";
                            configValue = ConfigValue(arrayStr);
                        }
                    }
                    break;
                default:
                    // Skip null or unknown types
                    continue;
            }
            
            config.set(key, configValue, ConfigSource::File);
        }
        
        return true;
    }
    
    return false;
}

// Configuration loading with enhanced error handling and validation
bool Config::loadFromFile(const std::string& filepath) {
    // First, try JSON parsing
    if (parseJSONConfig(filepath, *this)) {
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
    NLM_LOG_WARNING("Failed to parse as JSON, falling back to simple format for: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool hasValidConfig = false;
    
    while (std::getline(file, line)) {
        lineNum++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with better error handling
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_WARNING("Configuration file " + filepath + ": line " + std::to_string(lineNum) +
                               ": empty key, skipping");
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                set(key, value, ConfigSource::File);
                hasValidConfig = true;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error parsing configuration key=value pair in " + filepath +
                             ": line " + std::to_string(lineNum) +
                             ": " + e.what());
            }
        } else {
            NLM_LOG_WARNING("Configuration file " + filepath + ": line " + std::to_string(lineNum) +
                           ": invalid format (no '=' found), skipping");
        }
    }
    
    file.close();
    
    if (hasValidConfig) {
        NLM_LOG_INFO("Loaded configuration from simple file: " + filepath);
        return true;
    } else {
        NLM_LOG_WARNING("Configuration file " + filepath + ": no valid key=value pairs found");
        return false;
    }
}

// Enhanced saveToFile with better error handling
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create configuration file: " + filepath);
        return false;
    }
    
    try {
        file << "# NLM Configuration File" << std::endl;
        file << "# Generated by NLM (熙然) Neural Learning Machine" << std::endl;
        file << "# Format: key = value (JSON format for complex values)" << std::endl;
        file << std::endl;
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << "# Source: " << static_cast<int>(entry.source) << "\n";
            
            // Determine the best format based on value type
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                
                if constexpr (std::is_same_v<T, std::string>) {
                    // Quote strings to preserve spaces and special characters
                    file << entry.key << " = \"" << escapeString(arg) << "\"\n";
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, double>) {
                    file << entry.key << " = " << std::fixed << std::setprecision(6) << arg << "\n";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]\n";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << std::fixed << std::setprecision(6) << arg[i];
                    }
                    file << "]\n";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << escapeString(arg[i]) << "\"";
                    }
                    file << "]\n";
                } else {
                    // Default format for unknown types
                    file << entry.key << " = \"unknown\"\n";
                }
            }, entry.value);
            
            file << std::endl;
        }
        
        file << "# End of configuration" << std::endl;
        
        NLM_LOG_INFO("Configuration saved successfully to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving configuration: ") + e.what());
        return false;
    }
}

// Helper function to escape special characters in strings
std::string Config::escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (char c : str) {
        switch (c) {
            case '\\':
                result += "\\\\";
                break;
            case '"':
                result += "\\\"";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            case '\b':
                result += "\\b";
                break;
            case '\f':
                result += "\\f";
                break;
            default:
                result += c;
                break;
        }
    }
    
    return result;
}

// Enhanced summary with more detailed information
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    // Count entries by source
    std::unordered_map<ConfigSource, int> sourceCount;
    for (const auto& entry : pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    oss << "Sources:";
    for (const auto& pair : sourceCount) {
        oss << " " << static_cast<int>(pair.first) << ":" << pair.second;
    }
    oss << "\n\n";
    
    // Detailed entries
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        
        // Format the value nicely
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                if (arg.find(' ') != std::string::npos || arg.find('"') != std::string::npos) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
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
                    oss << std::fixed << std::setprecision(6) << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << escapeString(arg[i]) << "\"";
                }
                oss << "]";
            } else {
                oss << "unknown";
            }
        }, entry.value);
        
        oss << "] (source: " << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

} // namespace nlm
