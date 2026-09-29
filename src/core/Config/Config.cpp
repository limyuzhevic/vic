#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <limits>
#include <typeinfo>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// JSON Parser Implementation
bool JSONParser::parse(const std::string& jsonStr, std::unordered_map<std::string, ConfigValue>& config) {
    try {
        std::string cleanJson = trim(jsonStr);
        if (cleanJson.empty()) {
            return true;
        }
        
        // Simple JSON parsing for key-value pairs
        // This handles basic objects like {"key": "value", "number": 42}
        config.clear();
        
        // Remove outer braces if present
        std::string content = cleanJson;
        if (content.front() == '{' && content.back() == '}') {
            content = content.substr(1, content.size() - 2);
        }
        
        // Simple parsing - find key:value pairs
        std::regex pattern(R"((\w+)\s*:\s*([^,}\s]+))");
        auto begin = std::sregex_iterator(content.begin(), content.end(), pattern);
        auto end = std::sregex_iterator();
        
        for (std::sregex_iterator i = begin; i != end; ++i) {
            std::smatch match = *i;
            std::string key = trim(match[1].str());
            std::string valueStr = trim(match[2].str());
            
            ConfigValue value;
            if (parseValue(valueStr, value)) {
                config[key] = value;
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool JSONParser::isValidJSON(const std::string& str) {
    std::string clean = trim(str);
    if (clean.empty()) return true;
    
    if ((clean.front() == '{' && clean.back() == '}') ||
        (clean.front() == '[' && clean.back() == ']')) {
        return true;
    }
    
    // Try to parse as simple key:value pairs
    try {
        std::string content = clean;
        if (content.front() == '{' && content.back() == '}') {
            content = content.substr(1, content.size() - 2);
            std::regex pattern(R"((\w+)\s*:\s*([^,}\s]+))");
            auto begin = std::sregex_iterator(content.begin(), content.end(), pattern);
            auto end = std::sregex_iterator();
            
            for (std::sregex_iterator i = begin; i != end; ++i) {
                std::smatch match = *i;
                std::string key = trim(match[1].str());
                std::string valueStr = trim(match[2].str());
                
                ConfigValue value;
                if (!parseValue(valueStr, value)) {
                    return false;
                }
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

std::string JSONParser::escapeString(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '\"': oss << "\\\""; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

std::string JSONParser::unescapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[++i]) {
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                case '\\': result += '\\'; break;
                case '\"': result += '\"'; break;
                default: result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    
    return result;
}

std::string JSONParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> JSONParser::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(trim(item));
    }
    
    return result;
}

std::string JSONParser::serializeValue(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            // Format to avoid scientific notation for simple values
            if (std::floor(arg) == arg && std::abs(arg) < 1e9 && std::abs(arg) > 1e-6) {
                oss << std::fixed << std::setprecision(6) << arg;
            } else {
                oss << arg;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << escapeString(arg) << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                first = false;
                oss << item;
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                first = false;
                // Format double
                if (std::floor(item) == item && std::abs(item) < 1e9 && std::abs(item) > 1e-6) {
                    oss << std::fixed << std::setprecision(6) << item;
                } else {
                    oss << item;
                }
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            oss << "[";
            bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                first = false;
                oss << (item ? "true" : "false");
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                first = false;
                oss << "\"" << escapeString(item) << "\"";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, ConfigValue>>) {
            oss << "{\n";
            bool first = true;
            for (const auto& pair : arg) {
                if (!first) oss << ",\n";
                first = false;
                oss << "  \"" << pair.first << "\": " << serializeValue(pair.second);
            }
            oss << "\n}";
        } else if constexpr (std::is_same_v<T, std::vector<std::unordered_map<std::string, ConfigValue>>>) {
            oss << "[";
            bool first = true;
            for (const auto& obj : arg) {
                if (!first) oss << ",\n  ";
                first = false;
                oss << "{\n";
                bool firstItem = true;
                for (const auto& pair : obj) {
                    if (!firstItem) oss << ",\n";
                    firstItem = false;
                    oss << "    \"" << pair.first << "\": " << serializeValue(pair.second);
                }
                oss << "\n  }";
            }
            oss << "\n]";
        }
    }, value);
    
    return oss.str();
}

bool JSONParser::parseValue(const std::string& str, ConfigValue& value) {
    std::string clean = trim(str);
    
    // Remove quotes
    if (clean.size() >= 2 && clean.front() == '\"' && clean.back() == '\"') {
        value = unescapeString(clean.substr(1, clean.size() - 2));
        return true;
    }
    
    // Parse boolean
    if (clean == "true" || clean == "false") {
        value = (clean == "true");
        return true;
    }
    
    // Parse null
    if (clean == "null" || clean.empty()) {
        value = std::string("");
        return true;
    }
    
    // Parse numbers
    try {
        // Try double first
        size_t pos = 0;
        double doubleVal = std::stod(clean, &pos);
        if (pos == clean.size()) {
            // Check if it fits in integer range
            if (std::abs(doubleVal) < static_cast<double>(std::numeric_limits<int64_t>::max()) / 2) {
                value = static_cast<int64_t>(doubleVal);
            } else {
                value = doubleVal;
            }
            return true;
        }
    } catch (...) {}
    
    // Parse arrays
    if (clean.front() == '[' && clean.back() == ']') {
        std::string arrayContent = clean.substr(1, clean.size() - 2);
        std::vector<ConfigValue> array;
        
        // Simple array parsing - split by comma
        size_t pos = 0;
        while (pos < arrayContent.size()) {
            size_t commaPos = arrayContent.find(',', pos);
            if (commaPos == std::string::npos) {
                commaPos = arrayContent.size();
            }
            
            std::string itemStr = trim(arrayContent.substr(pos, commaPos - pos));
            ConfigValue item;
            if (parseValue(itemStr, item)) {
                array.push_back(item);
            } else {
                return false;
            }
            
            pos = commaPos + 1;
            while (pos < arrayContent.size() && arrayContent[pos] == ' ') pos++;
        }
        
        // Try to determine array type
        if (!array.empty()) {
            bool allInt = std::all_of(array.begin(), array.end(), [](const ConfigValue& v) { 
                return std::holds_alternative<int>(v) || std::holds_alternative<int64_t>(v); 
            });
            bool allDouble = std::all_of(array.begin(), array.end(), [](const ConfigValue& v) { 
                return std::holds_alternative<double>(v); 
            });
            bool allBool = std::all_of(array.begin(), array.end(), [](const ConfigValue& v) { 
                return std::holds_alternative<bool>(v); 
            });
            bool allString = std::all_of(array.begin(), array.end(), [](const ConfigValue& v) { 
                return std::holds_alternative<std::string>(v); 
            });
            bool allObjects = std::all_of(array.begin(), array.end(), [](const ConfigValue& v) { 
                return std::holds_alternative<std::unordered_map<std::string, ConfigValue>>(v); 
            });
            
            if (allInt) {
                std::vector<int> intArray;
                for (const auto& item : array) {
                    intArray.push_back(std::get<int>(item));
                }
                value = intArray;
            } else if (allDouble) {
                std::vector<double> doubleArray;
                for (const auto& item : array) {
                    doubleArray.push_back(std::get<double>(item));
                }
                value = doubleArray;
            } else if (allBool) {
                std::vector<bool> boolArray;
                for (const auto& item : array) {
                    boolArray.push_back(std::get<bool>(item));
                }
                value = boolArray;
            } else if (allString) {
                std::vector<std::string> stringArray;
                for (const auto& item : array) {
                    stringArray.push_back(std::get<std::string>(item));
                }
                value = stringArray;
            } else if (allObjects) {
                std::vector<std::unordered_map<std::string, ConfigValue>> objectArray;
                for (const auto& item : array) {
                    objectArray.push_back(std::get<std::unordered_map<std::string, ConfigValue>>(item));
                }
                value = objectArray;
            } else {
                value = array;
            }
        } else {
            value = std::vector<int>();
        }
        return true;
    }
    
    // Parse objects
    if (clean.front() == '{' && clean.back() == '}') {
        std::string objectContent = clean.substr(1, clean.size() - 2);
        std::unordered_map<std::string, ConfigValue> object;
        
        // Simple object parsing - find key:value pairs
        std::regex pattern(R"((\w+)\s*:\s*([^,}\s]+))");
        auto begin = std::sregex_iterator(objectContent.begin(), objectContent.end(), pattern);
        auto end = std::sregex_iterator();
        
        for (std::sregex_iterator i = begin; i != end; ++i) {
            std::smatch match = *i;
            std::string key = trim(match[1].str());
            std::string valueStr = trim(match[2].str());
            
            ConfigValue value;
            if (parseValue(valueStr, value)) {
                object[key] = value;
            }
        }
        
        value = object;
        return true;
    }
    
    // Try to parse as string (final fallback)
    value = clean;
    return true;
}

// YAML Parser Implementation
bool YAMLParser::parse(const std::string& yamlStr, std::unordered_map<std::string, ConfigValue>& config) {
    try {
        config.clear();
        std::vector<std::string> lines = splitLines(yamlStr);
        
        for (const std::string& line : lines) {
            std::string trimmed = trim(line);
            if (trimmed.empty() || trimmed.front() == '#') {
                continue;
            }
            
            // Parse key: value format (YAML uses : or =)
            size_t colonPos = trimmed.find(':');
            size_t equalPos = trimmed.find('=');
            
            if (colonPos != std::string::npos && (equalPos == std::string::npos || colonPos < equalPos)) {
                size_t keyEnd = colonPos;
                while (keyEnd > 0 && std::isspace(static_cast<unsigned char>(trimmed[keyEnd - 1]))) {
                    keyEnd--;
                }
                
                std::string key = trimmed.substr(0, keyEnd);
                std::string value = trim(trimmed.substr(colonPos + 1));
                
                ConfigValue configValue;
                if (parseValue(value, configValue)) {
                    config[key] = configValue;
                }
            } else if (equalPos != std::string::npos) {
                size_t keyEnd = equalPos;
                while (keyEnd > 0 && std::isspace(static_cast<unsigned char>(trimmed[keyEnd - 1]))) {
                    keyEnd--;
                }
                
                std::string key = trimmed.substr(0, keyEnd);
                std::string value = trim(trimmed.substr(equalPos + 1));
                
                ConfigValue configValue;
                if (parseValue(value, configValue)) {
                    config[key] = configValue;
                }
            } else {
                // Try to parse as key:value without separator (e.g., key: value)
                size_t colonInValue = trimmed.find(':');
                if (colonInValue != std::string::npos && colonInValue > 0) {
                    size_t keyEnd = colonInValue;
                    while (keyEnd > 0 && std::isspace(static_cast<unsigned char>(trimmed[keyEnd - 1]))) {
                        keyEnd--;
                    }
                    
                    std::string key = trimmed.substr(0, keyEnd);
                    std::string value = trim(trimmed.substr(colonInValue + 1));
                    
                    ConfigValue configValue;
                    if (parseValue(value, configValue)) {
                        config[key] = configValue;
                    }
                } else if (!trimmed.empty() && trimmed.find('=') == std::string::npos) {
                    // Assume boolean or integer
                    ConfigValue configValue;
                    if (parseValue(trimmed, configValue)) {
                        config[trimmed] = configValue;
                    }
                }
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool YAMLParser::isValidYAML(const std::string& str) {
    return !str.empty() && (JSONParser::isValidJSON(str) || YAMLParser::parse(str, std::unordered_map<std::string, ConfigValue>()));
}

std::string YAMLParser::serialize(const std::unordered_map<std::string, ConfigValue>& config, bool pretty) {
    std::ostringstream oss;
    
    bool first = true;
    for (const auto& pair : config) {
        if (!first) oss << "\n";
        first = false;
        
        oss << pair.first << ": " << serializeValue(pair.second);
    }
    
    return oss.str();
}

std::string YAMLParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> YAMLParser::splitLines(const std::string& str) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string line;
    
    while (std::getline(ss, line)) {
        result.push_back(line);
    }
    
    return result;
}

std::string YAMLParser::serializeValue(const ConfigValue& value) {
    return JSONParser::serializeValue(value);
}

bool YAMLParser::parseValue(const std::string& str, ConfigValue& value) {
    return JSONParser::parseValue(str, value);
}

std::string YAMLParser::yamlEscapeString(const std::string& str) {
    return JSONParser::escapeString(str);
}

// ConfigValidator Implementation
ValidationResult ConfigValidator::validate(const std::unordered_map<std::string, ConfigValue>& config,
                                          const std::vector<ConfigSchema>& schema) {
    for (const auto& entry : config) {
        auto it = std::find_if(schema.begin(), schema.end(), 
                              [&entry](const ConfigSchema& s) { return s.key == entry.first; });
        
        if (it != schema.end()) {
            const auto& schemaEntry = *it;
            auto validation = validateValue(schemaEntry.type, schemaEntry.elementType, entry.second, 
                                          schemaEntry.range, schemaEntry.minSize, schemaEntry.maxSize);
            if (!validation.isValid) {
                return ValidationResult(false, "Key \"" + entry.first + "\": " + validation.errorMessage);
            }
        }
    }
    
    return ValidationResult(true, "");
}

ValidationResult ConfigValidator::validateValue(const std::string& type, const std::string& elementType,
                                               const ConfigValue& value,
                                               const std::optional<std::pair<double, double>>& range,
                                               const std::optional<size_t>& minSize,
                                               const std::optional<size_t>& maxSize) {
    // Check type
    std::string actualType = getTypeName(value);
    if (type != "auto" && type != actualType) {
        return ValidationResult(false, "Expected type " + type + ", got " + actualType);
    }
    
    // Check element type for arrays
    if (isArrayType(type) && !elementType.empty()) {
        std::string actualElementType = getElementTypeName(value);
        if (elementType != "auto" && elementType != actualElementType) {
            return ValidationResult(false, "Expected array element type " + elementType + ", got " + actualElementType);
        }
    }
    
    // Check range if specified
    if (range.has_value()) {
        double min = range->first;
        double max = range->second;
        
        if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
            double val = static_cast<double>(std::get<int64_t>(value));
            if (val < min || val > max) {
                return ValidationResult(false, "Value " + std::to_string(val) + " outside range [" + 
                                          std::to_string(min) + ", " + std::to_string(max) + "]");
            }
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < min || val > max) {
                return ValidationResult(false, "Value " + std::to_string(val) + " outside range [" + 
                                          std::to_string(min) + ", " + std::to_string(max) + "]");
            }
        }
    }
    
    // Check array size constraints
    if (validateArraySize(value, minSize, maxSize)) {
        return ValidationResult(true, "");
    }
    
    return ValidationResult(true, "");
}

bool ConfigValidator::isNumericType(const std::string& type) {
    return type == "int" || type == "int64" || type == "double";
}

bool ConfigValidator::isArrayType(const std::string& type) {
    return type == "array" || type == "vector";
}

bool ConfigValidator::isObjectType(const std::string& type) {
    return type == "object";
}

std::string ConfigValidator::getTypeName(const ConfigValue& value) {
    if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) return "int";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "array";
    if (std::holds_alternative<std::vector<double>>(value)) return "array";
    if (std::holds_alternative<std::vector<bool>>(value)) return "array";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "array";
    if (std::holds_alternative<std::unordered_map<std::string, ConfigValue>>(value)) return "object";
    if (std::holds_alternative<std::vector<std::unordered_map<std::string, ConfigValue>>>(value)) return "array";
    return "unknown";
}

std::string ConfigValidator::getElementTypeName(const ConfigValue& value) {
    if (std::holds_alternative<std::vector<int>>(value)) return "int";
    if (std::holds_alternative<std::vector<double>>(value)) return "double";
    if (std::holds_alternative<std::vector<bool>>(value)) return "bool";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "string";
    if (std::holds_alternative<std::vector<std::unordered_map<std::string, ConfigValue>>>(value)) return "object";
    return "auto";
}

bool ConfigValidator::validateArraySize(const ConfigValue& value, const std::optional<size_t>& minSize,
                                        const std::optional<size_t>& maxSize) {
    if (minSize.has_value()) {
        if (std::holds_alternative<std::vector<int>>(value) && std::get<std::vector<int>>(value).size() < minSize.value()) {
            return false;
        }
        if (std::holds_alternative<std::vector<double>>(value) && std::get<std::vector<double>>(value).size() < minSize.value()) {
            return false;
        }
        if (std::holds_alternative<std::vector<bool>>(value) && std::get<std::vector<bool>>(value).size() < minSize.value()) {
            return false;
        }
        if (std::holds_alternative<std::vector<std::string>>(value) && std::get<std::vector<std::string>>(value).size() < minSize.value()) {
            return false;
        }
        if (std::holds_alternative<std::vector<std::unordered_map<std::string, ConfigValue>>>(value) && 
            std::get<std::vector<std::unordered_map<std::string, ConfigValue>>>(value).size() < minSize.value()) {
            return false;
        }
    }
    
    if (maxSize.has_value()) {
        if (std::holds_alternative<std::vector<int>>(value) && std::get<std::vector<int>>(value).size() > maxSize.value()) {
            return false;
        }
        if (std::holds_alternative<std::vector<double>>(value) && std::get<std::vector<double>>(value).size() > maxSize.value()) {
            return false;
        }
        if (std::holds_alternative<std::vector<bool>>(value) && std::get<std::vector<bool>>(value).size() > maxSize.value()) {
            return false;
        }
        if (std::holds_alternative<std::vector<std::string>>(value) && std::get<std::vector<std::string>>(value).size() > maxSize.value()) {
            return false;
        }
        if (std::holds_alternative<std::vector<std::unordered_map<std::string, ConfigValue>>>(value) && 
            std::get<std::vector<std::unordered_map<std::string, ConfigValue>>>(value).size() > maxSize.value()) {
            return false;
        }
    }
    
    return true;
}

// Config Implementation
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return loadFromString(content);
}

bool Config::loadFromString(const std::string& content) {
    clear();
    
    // Detect format from content
    std::string cleanContent = trim(content);
    std::string format;
    
    if (JSONParser::isValidJSON(cleanContent)) {
        format = "json";
    } else if (YAMLParser::isValidYAML(cleanContent)) {
        format = "yaml";
    } else {
        // Fall back to original key=value format
        format = "ini";
        // Parse original format
        std::istringstream stream(cleanContent);
        std::string line;
        
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
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
    
    // Parse with appropriate parser
    std::unordered_map<std::string, ConfigValue> parsedConfig;
    bool success = false;
    
    if (format == "json") {
        success = JSONParser::parse(cleanContent, parsedConfig);
    } else if (format == "yaml") {
        success = YAMLParser::parse(cleanContent, parsedConfig);
    }
    
    if (!success) {
        return false;
    }
    
    // Convert parsed config to Config
    for (const auto& pair : parsedConfig) {
        std::string description = "Loaded from " + format + " file";
        ConfigMetadata metadata;
        metadata.format = format;
        metadata.timestamp = std::chrono::system_clock::now();
        
        set(pair.first, pair.second, ConfigSource::File, description);
        setMetadata(pair.first, metadata);
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

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string detectedFormat = format;
    if (detectedFormat == "auto") {
        detectedFormat = getFileExtension(filepath);
    }
    
    std::unordered_map<std::string, ConfigValue> configMap;
    for (const auto& entry : pImpl->entries) {
        configMap[entry.key] = entry.value;
    }
    
    std::string content;
    bool success = false;
    
    if (detectedFormat == "json" || detectedFormat == ".json") {
        content = JSONParser::serialize(configMap, true);
        success = true;
    } else if (detectedFormat == "yaml" || detectedFormat == ".yaml" || 
               detectedFormat == ".yml") {
        content = YAMLParser::serialize(configMap, true);
        success = true;
    } else {
        // Fall back to original key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n\n";
        }
        return true;
    }
    
    if (success) {
        file << content;
    }
    
    return success;
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

template<typename T>
std::optional<std::vector<T>> Config::getArray(const std::string& key) const {
    auto value = get<std::vector<T>>(key);
    return value;
}

std::optional<NestedConfig> Config::getObject(const std::string& key) const {
    auto value = get<NestedConfig>(key);
    return value;
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

void Config::set(const std::string& key, const std::string& value, ConfigSource source, const std::string& description, const std::string& schemaName) {
    ConfigValue configValue(value);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigMetadata metadata;
    metadata.format = "string";
    metadata.timestamp = std::chrono::system_clock::now();
    metadata.schemaName = schemaName;
    
    ConfigEntry entry(key, configValue, source, description, metadata);
    
    if (it != pImpl->entries.end()) {
        *it = entry;
    } else {
        pImpl->entries.push_back(entry);
    }
}

void Config::set(const std::string& key, int value, ConfigSource source, const std::string& description, const std::string& schemaName) {
    set(key, ConfigValue(value), source, description, schemaName);
}

void Config::set(const std::string& key, double value, ConfigSource source, const std::string& description, const std::string& schemaName) {
    set(key, ConfigValue(value), source, description, schemaName);
}

void Config::set(const std::string& key, bool value, ConfigSource source, const std::string& description, const std::string& schemaName) {
    set(key, ConfigValue(value), source, description, schemaName);
}

void Config::setArray(const std::string& key, const std::vector<int>& value, ConfigSource source, const std::string& description, const std::string& schemaName) {
    ConfigValue configValue(value);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigMetadata metadata;
    metadata.format = "json";
    metadata.timestamp = std::chrono::system_clock::now();
    metadata.schemaName = schemaName;
    
    ConfigEntry entry(key, configValue, source, description, metadata);
    
    if (it != pImpl->entries.end()) {
        *it = entry;
    } else {
        pImpl->entries.push_back(entry);
    }
}

void Config::setArray(const std::string& key, const std::vector<double>& value, ConfigSource source, const std::string& description, const std::string& schemaName) {
    ConfigValue configValue(value);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigMetadata metadata;
    metadata.format = "json";
    metadata.timestamp = std::chrono::system_clock::now();
    metadata.schemaName = schemaName;
    
    ConfigEntry entry(key, configValue, source, description, metadata);
    
    if (it != pImpl->entries.end()) {
        *it = entry;
    } else {
        pImpl->entries.push_back(entry);
    }
}

void Config::setArray(const std::string& key, const std::vector<std::string>& value, ConfigSource source, const std::string& description, const std::string& schemaName) {
    ConfigValue configValue(value);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigMetadata metadata;
    metadata.format = "json";
    metadata.timestamp = std::chrono::system_clock::now();
    metadata.schemaName = schemaName;
    
    ConfigEntry entry(key, configValue, source, description, metadata);
    
    if (it != pImpl->entries.end()) {
        *it = entry;
    } else {
        pImpl->entries.push_back(entry);
    }
}

void Config::setObject(const std::string& key, const NestedConfig& value, ConfigSource source, const std::string& description, const std::string& schemaName) {
    ConfigValue configValue(value);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigMetadata metadata;
    metadata.format = "json";
    metadata.timestamp = std::chrono::system_clock::now();
    metadata.schemaName = schemaName;
    
    ConfigEntry entry(key, configValue, source, description, metadata);
    
    if (it != pImpl->entries.end()) {
        *it = entry;
    } else {
        pImpl->entries.push_back(entry);
    }
}

std::unordered_map<std::string, ConfigValue> Config::getNested(const std::string& prefix) const {
    std::unordered_map<std::string, ConfigValue> result;
    
    for (const auto& entry : pImpl->entries) {
        if (prefix.empty() || entry.key.find(prefix) == 0) {
            std::string key = entry.key;
            if (!prefix.empty() && entry.key.size() > prefix.size()) {
                key = entry.key.substr(prefix.size() + 1); // +1 for dot separator
            }
            result[key] = entry.value;
        }
    }
    
    return result;
}

std::optional<ConfigValue> Config::getNestedValue(const std::string& keyPath) const {
    auto entries = getNested();
    auto it = entries.find(keyPath);
    if (it != entries.end()) {
        return it->second;
    }
    return std::nullopt;
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

ValidationResult Config::validate(const std::vector<ConfigSchema>& schema) const {
    std::unordered_map<std::string, ConfigValue> configMap;
    for (const auto& entry : pImpl->entries) {
        configMap[entry.key] = entry.value;
    }
    return ConfigValidator::validate(configMap, schema);
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it == other.pImpl->entries.end() || it->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it != pImpl->entries.end()) {
            it->value = entry.value;
            it->source = source;
        } else {
            pImpl->entries.push_back(entry);
        }
    }
}

std::optional<ConfigMetadata> Config::getMetadata(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->metadata;
    }
    return std::nullopt;
}

void Config::setMetadata(const std::string& key, const ConfigMetadata& metadata) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        it->metadata = metadata;
    }
}

void Config::applyInheritance(const Config& parent, ConfigSource source) {
    for (const auto& entry : parent.pImpl->entries) {
        if (!has(entry.key)) {
            ConfigEntry newEntry = entry;
            newEntry.source = source;
            pImpl->entries.push_back(newEntry);
        }
    }
}

std::string Config::diff(const Config& other) const {
    std::ostringstream oss;
    
    // Find entries that are only in this config
    std::vector<std::string> onlyInThis;
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& otherEntry : other.pImpl->entries) {
            if (entry.key == otherEntry.key) {
                found = true;
                if (entry.value != otherEntry.value) {
                    oss << "\"" << entry.key << "\": changed from ";
                    std::visit([&oss](const auto& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            oss << "\"" << arg << "\"";
                        } else {
                            oss << arg;
                        }
                    }, otherEntry.value);
                    oss << " to ";
                    std::visit([&oss](const auto& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            oss << "\"" << arg << "\"";
                        } else {
                            oss << arg;
                        }
                    }, entry.value);
                    oss << "\n";
                }
                break;
            }
        }
        if (!found) {
            oss << "\"" << entry.key << "\": added\n";
        }
    }
    
    // Find entries that are only in other config
    for (const auto& entry : other.pImpl->entries) {
        bool found = false;
        for (const auto& thisEntry : pImpl->entries) {
            if (entry.key == thisEntry.key) {
                found = true;
                break;
            }
        }
        if (!found) {
            oss << "\"" << entry.key << "\": removed\n";
        }
    }
    
    if (oss.str().empty()) {
        return "Configurations are identical";
    }
    
    return oss.str();
}

template<typename T>
T Config::convertValue(const ConfigValue& value) {
    if (std::holds_alternative<T>(value)) {
        return std::get<T>(value);
    }
    
    // Try to convert between types
    if constexpr (std::is_same_v<T, int> && std::is_same_v<ConfigValue, double>) {
        return static_cast<int>(std::get<double>(value));
    } else if constexpr (std::is_same_v<T, double> && std::is_same_v<ConfigValue, int>) {
        return static_cast<double>(std::get<int>(value));
    } else if constexpr (std::is_same_v<T, std::string> && std::is_same_v<ConfigValue, bool>) {
        return std::get<bool>(value) ? "true" : "false";
    } else if constexpr (std::is_same_v<T, std::vector<T>> && std::is_same_v<ConfigValue, std::vector<ConfigValue>>) {
        // Cannot automatically convert vector of ConfigValue to vector of T
        // This would need to be handled separately
        throw std::runtime_error("Cannot automatically convert vector of ConfigValue to specific vector type");
    }
    
    throw std::runtime_error("Cannot convert from ConfigValue to " + std::string(typeid(T).name()));
}

std::string Config::getDefaultSchemaName(const std::string& key) {
    return "schema_" + toLower(key);
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

std::string Config::detectFormat(const std::string& filepath) const {
    std::string extension = getFileExtension(filepath);
    if (extension == "json") return "json";
    if (extension == "yaml" || extension == "yml") return "yaml";
    return "auto";
}

std::string Config::getFileExtension(const std::string& filepath) const {
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        return filepath.substr(dotPos + 1);
    }
    return "";
}

std::string Config::joinKeyPath(const std::vector<std::string>& parts) {
    if (parts.empty()) return "";
    std::string result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result += "." + parts[i];
    }
    return result;
}

std::vector<std::string> Config::splitKeyPath(const std::string& keyPath) {
    std::vector<std::string> parts;
    std::stringstream ss(keyPath);
    std::string part;
    
    while (std::getline(ss, part, '.')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    return parts;
}

// Explicit template instantiations for enhanced type support
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

// Enhanced array template instantiations
template std::optional<std::vector<int>> Config::getArray<int>(const std::string&) const;
template std::optional<std::vector<double>> Config::getArray<double>(const std::string&) const;
template std::optional<std::vector<bool>> Config::getArray<bool>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::getArray<std::string>(const std::string&) const;
template std::optional<std::vector<std::unordered_map<std::string, ConfigValue>>> Config::getArray<std::vector<std::unordered_map<std::string, ConfigValue>>>(const std::string&) const;

} // namespace nlm

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// JSON Parser Implementation
bool JSONParser::parse(const std::string& jsonStr, std::unordered_map<std::string, ConfigValue>& config) {
    try {
        std::string cleanJson = trim(jsonStr);
        if (cleanJson.empty()) {
            return true;
        }
        
        // Simple JSON parsing for key-value pairs
        // This handles basic objects like {"key": "value", "number": 42}
        config.clear();
        
        // Remove outer braces if present
        std::string content = cleanJson;
        if (content.front() == '{' && content.back() == '}') {
            content = content.substr(1, content.size() - 2);
        }
        
        // Simple parsing - find key:value pairs
        std::regex pattern(R"((\w+)\s*:\s*([^,}\s]+))");
        auto begin = std::sregex_iterator(content.begin(), content.end(), pattern);
        auto end = std::sregex_iterator();
        
        for (std::sregex_iterator i = begin; i != end; ++i) {
            std::smatch match = *i;
            std::string key = trim(match[1].str());
            std::string valueStr = trim(match[2].str());
            
            ConfigValue value;
            if (parseValue(valueStr, value)) {
                config[key] = value;
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool JSONParser::isValidJSON(const std::string& str) {
    std::string clean = trim(str);
    if (clean.empty()) return true;
    
    if ((clean.front() == '{' && clean.back() == '}') ||
        (clean.front() == '[' && clean.back() == ']')) {
        return true;
    }
    
    // Try to parse as simple key:value pairs
    try {
        std::string content = clean;
        if (content.front() == '{' && content.back() == '}') {
            content = content.substr(1, content.size() - 2);
            std::regex pattern(R"((\w+)\s*:\s*([^,}\s]+))");
            auto begin = std::sregex_iterator(content.begin(), content.end(), pattern);
            auto end = std::sregex_iterator();
            
            for (std::sregex_iterator i = begin; i != end; ++i) {
                std::smatch match = *i;
                std::string key = trim(match[1].str());
                std::string valueStr = trim(match[2].str());
                
                ConfigValue value;
                if (!parseValue(valueStr, value)) {
                    return false;
                }
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

std::string JSONParser::escapeString(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '\"': oss << "\\\""; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

std::string JSONParser::unescapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[++i]) {
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                case '\\': result += '\\'; break;
                case '\"': result += '\"'; break;
                default: result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    
    return result;
}

std::string JSONParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> JSONParser::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(trim(item));
    }
    
    return result;
}

std::string JSONParser::serializeValue(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            // Format to avoid scientific notation for simple values
            if (std::floor(arg) == arg && std::abs(arg) < 1e9 && std::abs(arg) > 1e-6) {
                oss << std::fixed << std::setprecision(6) << arg;
            } else {
                oss << arg;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << escapeString(arg) << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                           std::is_same_v<T, std::vector<double>> ||
                           std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                first = false;
                oss << serializeValue(item);
            }
            oss << "]";
        }
    }, value);
    
    return oss.str();
}

bool JSONParser::parseValue(const std::string& str, ConfigValue& value) {
    std::string clean = trim(str);
    
    // Remove quotes
    if (clean.size() >= 2 && clean.front() == '\"' && clean.back() == '\"') {
        value = unescapeString(clean.substr(1, clean.size() - 2));
        return true;
    }
    
    // Parse boolean
    if (clean == "true" || clean == "false") {
        value = (clean == "true");
        return true;
    }
    
    // Parse null
    if (clean == "null" || clean.empty()) {
        value = std::string("");
        return true;
    }
    
    // Parse numbers
    try {
        // Try double first
        size_t pos = 0;
        double doubleVal = std::stod(clean, &pos);
        if (pos == clean.size()) {
            // Check if it fits in integer range
            if (std::abs(doubleVal) < static_cast<double>(std::numeric_limits<int64_t>::max()) / 2) {
                value = static_cast<int64_t>(doubleVal);
            } else {
                value = doubleVal;
            }
            return true;
        }
    } catch (...) {}
    
    // Parse arrays
    if (clean.front() == '[' && clean.back() == ']') {
        std::string arrayContent = clean.substr(1, clean.size() - 2);
        std::vector<ConfigValue> array;
        
        // Simple array parsing - split by comma
        size_t pos = 0;
        while (pos < arrayContent.size()) {
            size_t commaPos = arrayContent.find(',', pos);
            if (commaPos == std::string::npos) {
                commaPos = arrayContent.size();
            }
            
            std::string itemStr = trim(arrayContent.substr(pos, commaPos - pos));
            ConfigValue item;
            if (parseValue(itemStr, item)) {
                array.push_back(item);
            } else {
                return false;
            }
            
            pos = commaPos + 1;
            while (pos < arrayContent.size() && arrayContent[pos] == ' ') pos++;
        }
        
        // Try to determine array type
        if (!array.empty()) {
            if (std::all_of(array.begin(), array.end(), [](const ConfigValue& v) { 
                return std::holds_alternative<int>(v) || std::holds_alternative<int64_t>(v); 
            })) {
                std::vector<int> intArray;
                for (const auto& item : array) {
                    intArray.push_back(std::get<int>(item));
                }
                value = intArray;
            } else if (std::all_of(array.begin(), array.end(), [](const ConfigValue& v) { 
                return std::holds_alternative<double>(v); 
            })) {
                std::vector<double> doubleArray;
                for (const auto& item : array) {
                    doubleArray.push_back(std::get<double>(item));
                }
                value = doubleArray;
            } else if (std::all_of(array.begin(), array.end(), [](const ConfigValue& v) { 
                return std::holds_alternative<std::string>(v); 
            })) {
                std::vector<std::string> stringArray;
                for (const auto& item : array) {
                    stringArray.push_back(std::get<std::string>(item));
                }
                value = stringArray;
            } else {
                value = array;
            }
        } else {
            value = std::vector<int>();
        }
        return true;
    }
    
    // Try to parse as string (final fallback)
    value = clean;
    return true;
}

std::string JSONParser::serialize(const std::unordered_map<std::string, ConfigValue>& config, bool pretty) {
    std::ostringstream oss;
    if (pretty) {
        oss << std::setw(2);
    }
    
    oss << "{\n";
    bool first = true;
    for (const auto& pair : config) {
        if (!first) oss << ",\n";
        first = false;
        
        oss << "  \"" << pair.first << "\": " << serializeValue(pair.second);
    }
    oss << "\n}";
    
    return oss.str();
}

std::string JSONParser::escapeString(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '\"': oss << "\\\""; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

std::string JSONParser::unescapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[++i]) {
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                case '\\': result += '\\'; break;
                case '\"': result += '\"'; break;
                default: result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    
    return result;
}

std::string JSONParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> JSONParser::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(trim(item));
    }
    
    return result;
}

std::string JSONParser::serializeValue(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            // Format to avoid scientific notation for simple values
            if (std::floor(arg) == arg && std::abs(arg) < 1e9 && std::abs(arg) > 1e-6) {
                oss << std::fixed << std::setprecision(6) << arg;
            } else {
                oss << arg;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << escapeString(arg) << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                           std::is_same_v<T, std::vector<double>> ||
                           std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                first = false;
                oss << serializeValue(item);
            }
            oss << "]";
        }
    }, value);
    
    return oss.str();
}

bool JSONParser::parseValue(const std::string& str, ConfigValue& value) {
    std::string clean = trim(str);
    
    // Remove quotes
    if (clean.size() >= 2 && clean.front() == '\"' && clean.back() == '\"') {
        value = unescapeString(clean.substr(1, clean.size() - 2));
        return true;
    }
    
    // Parse boolean
    if (clean == "true" || clean == "false") {
        value = (clean == "true");
        return true;
    }
    
    // Parse null
    if (clean == "null" || clean.empty()) {
        value = std::string("");
        return true;
    }
    
    // Parse numbers
    try {
        // Try double first
        size_t pos = 0;
        double doubleVal = std::stod(clean, &pos);
        if (pos == clean.size()) {
            // Check if it fits in integer range
            if (std::abs(doubleVal) < static_cast<double>(std::numeric_limits<int64_t>::max()) / 2) {
                value = static_cast<int64_t>(doubleVal);
            } else {
                value = doubleVal;
            }
            return true;
        }
    } catch (...) {}
    
    // Parse arrays
    if (clean.front() == '[' && clean.back() == ']') {
        std::string arrayContent = clean.substr(1, clean.size() - 2);
        std::vector<ConfigValue> array;
        
        // Simple array parsing - split by comma
        size_t pos = 0;
        while (pos < arrayContent.size()) {
            size_t commaPos = arrayContent.find(',', pos);
            if (commaPos == std::string::npos) {
                commaPos = arrayContent.size();
            }
            
            std::string itemStr = trim(arrayContent.substr(pos, commaPos - pos));
            ConfigValue item;
            if (parseValue(itemStr, item)) {
                array.push_back(item);
            } else {
                return false;
            }
            
            pos = commaPos + 1;
            while (pos < arrayContent.size() && arrayContent[pos] == ' ') pos++;
        }
        
        // Try to determine array type
        if (!array.empty()) {
            if (std::all_of(array.begin(), array.end(), [](const ConfigValue& v) { 
                return std::holds_alternative<int>(v) || std::holds_alternative<int64_t>(v); 
            })) {
                std::vector<int> intArray;
                for (const auto& item : array) {
                    intArray.push_back(std::get<int>(item));
                }
                value = intArray;
            } else if (std::all_of(array.begin(), array.end(), [](const ConfigValue& v) { 
                return std::holds_alternative<double>(v); 
            })) {
                std::vector<double> doubleArray;
                for (const auto& item : array) {
                    doubleArray.push_back(std::get<double>(item));
                }
                value = doubleArray;
            } else if (std::all_of(array.begin(), array.end(), [](const ConfigValue& v) { 
                return std::holds_alternative<std::string>(v); 
            })) {
                std::vector<std::string> stringArray;
                for (const auto& item : array) {
                    stringArray.push_back(std::get<std::string>(item));
                }
                value = stringArray;
            } else {
                value = array;
            }
        } else {
            value = std::vector<int>();
        }
        return true;
    }
    
    // Try to parse as string (final fallback)
    value = clean;
    return true;
}

// YAML Parser Implementation
bool YAMLParser::parse(const std::string& yamlStr, std::unordered_map<std::string, ConfigValue>& config) {
    try {
        config.clear();
        std::vector<std::string> lines = splitLines(yamlStr);
        
        for (const std::string& line : lines) {
            std::string trimmed = trim(line);
            if (trimmed.empty() || trimmed.front() == '#') {
                continue;
            }
            
            // Parse key: value format (YAML uses : or =)
            size_t colonPos = trimmed.find(':');
            size_t equalPos = trimmed.find('=');
            
            if (colonPos != std::string::npos && (equalPos == std::string::npos || colonPos < equalPos)) {
                size_t keyEnd = colonPos;
                while (keyEnd > 0 && std::isspace(static_cast<unsigned char>(trimmed[keyEnd - 1]))) {
                    keyEnd--;
                }
                
                std::string key = trimmed.substr(0, keyEnd);
                std::string value = trim(trimmed.substr(colonPos + 1));
                
                ConfigValue configValue;
                if (parseValue(value, configValue)) {
                    config[key] = configValue;
                }
            } else if (equalPos != std::string::npos) {
                size_t keyEnd = equalPos;
                while (keyEnd > 0 && std::isspace(static_cast<unsigned char>(trimmed[keyEnd - 1]))) {
                    keyEnd--;
                }
                
                std::string key = trimmed.substr(0, keyEnd);
                std::string value = trim(trimmed.substr(equalPos + 1));
                
                ConfigValue configValue;
                if (parseValue(value, configValue)) {
                    config[key] = configValue;
                }
            } else {
                // Try to parse as key:value without separator (e.g., key: value)
                size_t colonInValue = trimmed.find(':');
                if (colonInValue != std::string::npos && colonInValue > 0) {
                    size_t keyEnd = colonInValue;
                    while (keyEnd > 0 && std::isspace(static_cast<unsigned char>(trimmed[keyEnd - 1]))) {
                        keyEnd--;
                    }
                    
                    std::string key = trimmed.substr(0, keyEnd);
                    std::string value = trim(trimmed.substr(colonInValue + 1));
                    
                    ConfigValue configValue;
                    if (parseValue(value, configValue)) {
                        config[key] = configValue;
                    }
                } else if (!trimmed.empty() && trimmed.find('=') == std::string::npos) {
                    // Assume boolean or integer
                    ConfigValue configValue;
                    if (parseValue(trimmed, configValue)) {
                        config[trimmed] = configValue;
                    }
                }
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool YAMLParser::isValidYAML(const std::string& str) {
    return !str.empty() && (isValidJSON(str) || YAMLParser::parse(str, std::unordered_map<std::string, ConfigValue>()));
}

std::string YAMLParser::serialize(const std::unordered_map<std::string, ConfigValue>& config, bool pretty) {
    std::ostringstream oss;
    
    bool first = true;
    for (const auto& pair : config) {
        if (!first) oss << "\n";
        first = false;
        
        oss << pair.first << ": " << serializeValue(pair.second);
    }
    
    return oss.str();
}

std::string YAMLParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> YAMLParser::splitLines(const std::string& str) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string line;
    
    while (std::getline(ss, line)) {
        result.push_back(line);
    }
    
    return result;
}

std::string YAMLParser::serializeValue(const ConfigValue& value) {
    return JSONParser::serializeValue(value);
}

// ConfigValidator Implementation
ValidationResult ConfigValidator::validate(const std::unordered_map<std::string, ConfigValue>& config,
                                          const std::vector<ConfigSchema>& schema) {
    for (const auto& entry : config) {
        auto it = std::find_if(schema.begin(), schema.end(), 
                              [&entry](const ConfigSchema& s) { return s.key == entry.first; });
        
        if (it != schema.end()) {
            const auto& schemaEntry = *it;
            auto validation = validateValue(schemaEntry.type, entry.second, schemaEntry.range);
            if (!validation.isValid) {
                return ValidationResult(false, "Key \"" + entry.first + "\": " + validation.errorMessage);
            }
        }
    }
    
    return ValidationResult(true, "");
}

ValidationResult ConfigValidator::validateValue(const std::string& type, const ConfigValue& value,
                                               const std::optional<std::pair<double, double>>& range) {
    // Check type
    std::string actualType = getTypeName(value);
    if (type != "auto" && type != actualType) {
        return ValidationResult(false, "Expected type " + type + ", got " + actualType);
    }
    
    // Check range if specified
    if (range.has_value()) {
        double min = range->first;
        double max = range->second;
        
        if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
            double val = static_cast<double>(std::get<int64_t>(value));
            if (val < min || val > max) {
                return ValidationResult(false, "Value " + std::to_string(val) + " outside range [" + 
                                          std::to_string(min) + ", " + std::to_string(max) + "]");
            }
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < min || val > max) {
                return ValidationResult(false, "Value " + std::to_string(val) + " outside range [" + 
                                          std::to_string(min) + ", " + std::to_string(max) + "]");
            }
        }
    }
    
    return ValidationResult(true, "");
}

bool ConfigValidator::isNumericType(const std::string& type) {
    return type == "int" || type == "double";
}

std::string ConfigValidator::getTypeName(const ConfigValue& value) {
    if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) return "int";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value) || 
        std::holds_alternative<std::vector<double>>(value) || 
        std::holds_alternative<std::vector<std::string>>(value)) return "array";
    return "unknown";
}

// Config Implementation
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return loadFromString(content);
}

bool Config::loadFromString(const std::string& content) {
    clear();
    
    // Detect format from content
    std::string cleanContent = trim(content);
    std::string format;
    
    if (JSONParser::isValidJSON(cleanContent)) {
        format = "json";
    } else if (YAMLParser::isValidYAML(cleanContent)) {
        format = "yaml";
    } else {
        // Fall back to original key=value format
        format = "ini";
        // Parse original format
        std::istringstream stream(cleanContent);
        std::string line;
        
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
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
    
    // Parse with appropriate parser
    std::unordered_map<std::string, ConfigValue> parsedConfig;
    bool success = false;
    
    if (format == "json") {
        success = JSONParser::parse(cleanContent, parsedConfig);
    } else if (format == "yaml") {
        success = YAMLParser::parse(cleanContent, parsedConfig);
    }
    
    if (!success) {
        return false;
    }
    
    // Convert parsed config to Config
    for (const auto& pair : parsedConfig) {
        std::string description = "Loaded from " + format + " file";
        ConfigMetadata metadata;
        metadata.format = format;
        metadata.timestamp = std::chrono::system_clock::now();
        
        set(pair.first, pair.second, ConfigSource::File, description);
        setMetadata(pair.first, metadata);
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

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string detectedFormat = format;
    if (detectedFormat == "auto") {
        detectedFormat = getFileExtension(filepath);
    }
    
    std::unordered_map<std::string, ConfigValue> configMap;
    for (const auto& entry : pImpl->entries) {
        configMap[entry.key] = entry.value;
    }
    
    std::string content;
    bool success = false;
    
    if (detectedFormat == "json" || detectedFormat == ".json") {
        content = JSONParser::serialize(configMap, true);
        success = true;
    } else if (detectedFormat == "yaml" || detectedFormat == ".yaml" || 
               detectedFormat == ".yml") {
        content = YAMLParser::serialize(configMap, true);
        success = true;
    } else {
        // Fall back to original key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n\n";
        }
        return true;
    }
    
    if (success) {
        file << content;
    }
    
    return success;
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

void Config::set(const std::string& key, const std::string& value, ConfigSource source, const std::string& description) {
    ConfigValue configValue(value);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigMetadata metadata;
    metadata.format = "string";
    metadata.timestamp = std::chrono::system_clock::now();
    
    ConfigEntry entry(key, configValue, source, description, metadata);
    
    if (it != pImpl->entries.end()) {
        *it = entry;
    } else {
        pImpl->entries.push_back(entry);
    }
}

void Config::set(const std::string& key, int value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, double value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, bool value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

std::unordered_map<std::string, ConfigValue> Config::getNested(const std::string& prefix) const {
    std::unordered_map<std::string, ConfigValue> result;
    
    for (const auto& entry : pImpl->entries) {
        if (prefix.empty() || entry.key.find(prefix) == 0) {
            std::string key = entry.key;
            if (!prefix.empty() && entry.key.size() > prefix.size()) {
                key = entry.key.substr(prefix.size() + 1); // +1 for dot separator
            }
            result[key] = entry.value;
        }
    }
    
    return result;
}

std::optional<ConfigValue> Config::getNestedValue(const std::string& keyPath) const {
    auto entries = getNested();
    auto it = entries.find(keyPath);
    if (it != entries.end()) {
        return it->second;
    }
    return std::nullopt;
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

ValidationResult Config::validate(const std::vector<ConfigSchema>& schema) const {
    std::unordered_map<std::string, ConfigValue> configMap;
    for (const auto& entry : pImpl->entries) {
        configMap[entry.key] = entry.value;
    }
    return ConfigValidator::validate(configMap, schema);
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it == other.pImpl->entries.end() || it->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it != pImpl->entries.end()) {
            it->value = entry.value;
            it->source = source;
        } else {
            pImpl->entries.push_back(entry);
        }
    }
}

std::optional<ConfigMetadata> Config::getMetadata(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->metadata;
    }
    return std::nullopt;
}

void Config::setMetadata(const std::string& key, const ConfigMetadata& metadata) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        it->metadata = metadata;
    }
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

std::string Config::detectFormat(const std::string& filepath) const {
    std::string extension = getFileExtension(filepath);
    if (extension == "json") return "json";
    if (extension == "yaml" || extension == "yml") return "yaml";
    return "auto";
}

std::string Config::getFileExtension(const std::string& filepath) const {
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        return filepath.substr(dotPos + 1);
    }
    return "";
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
