#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <cctype>
#include <ctime>
#include <random>

namespace nlm {

// Helper function to detect file format
std::string detectFileFormat(const std::string& filepath) {
    if (filepath.empty()) return "auto";
    
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos == std::string::npos) return "auto";
    
    std::string extension = filepath.substr(dotPos + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json") return "json";
    if (extension == "yaml" || extension == "yml") return "yaml";
    if (extension == "cfg" || extension == "config") return "simple";
    
    return "auto";
}

// JSON utilities implementation (simplified)
class JSONUtils {
public:
    static bool parseJSONFile(const std::string& filepath, std::unordered_map<std::string, std::string>& result) {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        return parseJSONString(buffer.str(), result);
    }
    
    static bool parseJSONString(const std::string& jsonStr, std::unordered_map<std::string, std::string>& result) {
        try {
            // Simple JSON parsing - would use nlohmann::json in production
            if (jsonStr.empty() || jsonStr.find('{') == std::string::npos) return false;
            
            // Remove whitespace and newlines
            std::string clean = jsonStr;
            clean.erase(std::remove_if(clean.begin(), clean.end(), ::isspace), clean.end());
            
            // Basic key-value extraction
            size_t pos = 0;
            while ((pos = clean.find('"', pos)) != std::string::npos) {
                size_t keyStart = pos++;
                pos = clean.find('"', pos);
                if (pos == std::string::npos) break;
                
                std::string key = clean.substr(keyStart, pos - keyStart);
                
                // Find value
                size_t colonPos = clean.find(':', pos);
                if (colonPos == std::string::npos) break;
                
                pos = clean.find('"', colonPos + 1);
                if (pos == std::string::npos) break;
                
                size_t valueStart = pos++;
                pos = clean.find('"', pos);
                if (pos == std::string::npos) break;
                
                std::string value = clean.substr(valueStart, pos - valueStart);
                
                result[key] = value;
            }
            
            return true;
        } catch (...) {
            return false;
        }
    }
    
    static std::string serializeToJSON(const std::unordered_map<std::string, std::string>& data) {
        std::stringstream json;
        json << "{\n";
        
        bool first = true;
        for (const auto& pair : data) {
            if (!first) json << ",\n";
            first = false;
            
            json << "  \"" << pair.first << "\": \"" << escapeJSONString(pair.second) << "\"";
        }
        
        json << "\n}";
        return json.str();
    }
    
    static bool validateJSON(const std::string& jsonStr) {
        if (jsonStr.empty()) return false;
        
        // Basic validation - check for balanced braces
        int braceCount = 0;
        for (char c : jsonStr) {
            if (c == '{') braceCount++;
            else if (c == '}') braceCount--;
            if (braceCount < 0) return false;
        }
        
        return braceCount == 0;
    }
    
    static std::string escapeJSONString(const std::string& str) {
        std::stringstream ss;
        for (char c : str) {
            switch (c) {
                case '\\': ss << "\\\\"; break;
                case '"': ss << "\\\""; break;
                case '/': ss << "\\\\/"; break;
                case '\b': ss << "\\b"; break;
                case '\f': ss << "\\f"; break;
                case '\n': ss << "\\n"; break;
                case '\r': ss << "\\r"; break;
                case '\t': ss << "\\t"; break;
                default:
                    if (c < ' ' || c == 0x7f) ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)(unsigned char)c;
                    else ss << c;
                    break;
            }
        }
        return ss.str();
    }
    
    static std::string configValueToString(const ConfigValue& value) {
        std::stringstream ss;
        std::visit([&ss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                ss << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                ss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                ss << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                ss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                ss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << arg[i];
                }
                ss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << arg[i];
                }
                ss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << "\"" << arg[i] << "\"";
                }
                ss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<float>>) {
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << arg[i];
                }
                ss << "]";
            }
        }, value);
        return ss.str();
    }
    
    static std::optional<ConfigValue> stringToConfigValue(const std::string& str, const std::string& typeHint) {
        // Remove surrounding quotes
        std::string value = str;
        if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                 (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Try different types based on type hint or auto-detection
        if (typeHint.empty() || typeHint == "auto") {
            // Auto-detect type
            if (value.empty()) return std::string();
            if (value == "true" || value == "false") {
                return value == "true";
            }
            
            // Check if it's a number
            bool isNumber = true;
            bool hasDecimal = false;
            for (char c : value) {
                if (c == '.') {
                    hasDecimal = true;
                    continue;
                }
                if (!std::isdigit(c)) {
                    isNumber = false;
                    break;
                }
            }
            
            if (isNumber) {
                if (hasDecimal) {
                    return std::stod(value);
                } else {
                    // Check if it fits in int64_t
                    try {
                        size_t pos;
                        int64_t intVal = std::stoll(value, &pos);
                        if (pos == value.size()) {
                            return intVal;
                        }
                    } catch (...) {
                    }
                    return std::stod(value);
                }
            }
            
            // Check if it's a vector
            if (value.front() == '[' && value.back() == ']') {
                std::string inner = value.substr(1, value.size() - 2);
                if (inner.empty()) {
                    return std::vector<std::string>();
                }
                
                // Try to parse as int vector
                std::vector<int> intVec;
                std::stringstream ss(inner);
                std::string token;
                bool allInts = true;
                while (std::getline(ss, token, ',')) {
                    token.erase(0, token.find_first_not_of(" \t"));
                    token.erase(token.find_last_not_of(" \t") + 1);
                    try {
                        intVec.push_back(std::stoi(token));
                    } catch (...) {
                        allInts = false;
                        break;
                    }
                }
                
                if (allInts && !intVec.empty()) {
                    return intVec;
                }
                
                // Try to parse as double vector
                std::vector<double> doubleVec;
                ss.clear();
                ss.str(inner);
                bool allDoubles = true;
                while (std::getline(ss, token, ',')) {
                    token.erase(0, token.find_first_not_of(" \t"));
                    token.erase(token.find_last_not_of(" \t") + 1);
                    try {
                        doubleVec.push_back(std::stod(token));
                    } catch (...) {
                        allDoubles = false;
                        break;
                    }
                }
                
                if (allDoubles && !doubleVec.empty()) {
                    return doubleVec;
                }
                
                // Try to parse as string vector
                std::vector<std::string> stringVec;
                ss.clear();
                ss.str(inner);
                bool allStrings = true;
                while (std::getline(ss, token, ',')) {
                    token.erase(0, token.find_first_not_of(" \t\"'"));
                    token.erase(token.find_last_not_of(" \t\"'")) + 1;
                    if (!token.empty()) {
                        stringVec.push_back(token);
                    }
                }
                
                if (!stringVec.empty()) {
                    return stringVec;
                }
            }
        } else {
            // Type hint provided
            if (typeHint == "int") {
                try { return std::stoi(value); } catch (...) {}
            } else if (typeHint == "int64") {
                try { return std::stoll(value); } catch (...) {}
            } else if (typeHint == "float" || typeHint == "double") {
                try { return std::stod(value); } catch (...) {}
            } else if (typeHint == "bool") {
                if (value == "true") return true;
                if (value == "false") return false;
            } else if (typeHint == "string") {
                return value;
            } else if (typeHint == "int_array") {
                // Parse int array
                std::vector<int> intVec;
                std::stringstream ss(value);
                std::string token;
                while (std::getline(ss, token, ',')) {
                    token.erase(0, token.find_first_not_of(" \t"));
                    token.erase(token.find_last_not_of(" \t") + 1);
                    try {
                        intVec.push_back(std::stoi(token));
                    } catch (...) {
                        break;
                    }
                }
                if (!intVec.empty()) return intVec;
            } else if (typeHint == "float_array") {
                // Parse float array
                std::vector<double> floatVec;
                std::stringstream ss(value);
                std::string token;
                while (std::getline(ss, token, ',')) {
                    token.erase(0, token.find_first_not_of(" \t"));
                    token.erase(token.find_last_not_of(" \t") + 1);
                    try {
                        floatVec.push_back(std::stod(token));
                    } catch (...) {
                        break;
                    }
                }
                if (!floatVec.empty()) return floatVec;
            } else if (typeHint == "string_array") {
                // Parse string array
                std::vector<std::string> stringVec;
                std::stringstream ss(value);
                std::string token;
                while (std::getline(ss, token, ',')) {
                    token.erase(0, token.find_first_not_of(" \t\"'"));
                    token.erase(token.find_last_not_of(" \t\"'")) + 1;
                    if (!token.empty()) {
                        stringVec.push_back(token);
                    }
                }
                if (!stringVec.empty()) return stringVec;
            }
        }
        
        return std::string(); // Default to string
    }
};

// YAML utilities implementation (simplified)
class YAMLUtils {
public:
    static bool parseYAMLFile(const std::string& filepath, std::unordered_map<std::string, std::string>& result) {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        return parseYAMLString(buffer.str(), result);
    }
    
    static bool parseYAMLString(const std::string& yamlStr, std::unordered_map<std::string, std::string>& result) {
        try {
            // Simple YAML parsing - would use yaml-cpp in production
            if (yamlStr.empty() || yamlStr.find(':') == std::string::npos) return false;
            
            // Remove comments
            std::string clean = yamlStr;
            size_t commentPos = clean.find('#');
            while (commentPos != std::string::npos) {
                size_t nextLine = clean.find('\n', commentPos);
                if (nextLine == std::string::npos) {
                    clean.erase(commentPos);
                    break;
                }
                clean.erase(commentPos, nextLine - commentPos);
                commentPos = clean.find('#', commentPos);
            }
            
            // Parse key-value pairs
            size_t pos = 0;
            while (pos < clean.size()) {
                // Find next line with content
                size_t lineEnd = clean.find('\n', pos);
                if (lineEnd == std::string::npos) lineEnd = clean.size();
                
                std::string line = clean.substr(pos, lineEnd - pos);
                pos = lineEnd + 1;
                
                // Trim whitespace
                line.erase(0, line.find_first_not_of(" \t"));
                line.erase(line.find_last_not_of(" \t") + 1);
                
                if (line.empty()) continue;
                
                // Find first colon
                size_t colonPos = line.find(':');
                if (colonPos == std::string::npos) continue;
                
                // Extract key (before colon)
                std::string key = line.substr(0, colonPos);
                key.erase(key.find_last_not_of(" \t") + 1);
                key.erase(0, key.find_first_not_of(" \t"));
                
                // Extract value (after colon)
                std::string value = line.substr(colonPos + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                
                // Remove quotes if present
                if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                         (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                result[key] = value;
            }
            
            return true;
        } catch (...) {
            return false;
        }
    }
    
    static std::string serializeToYAML(const std::unordered_map<std::string, std::string>& data) {
        std::stringstream yaml;
        yaml << "# YAML Configuration\n";
        
        for (const auto& pair : data) {
            yaml << pair.first << ": " << pair.second << "\n";
        }
        
        return yaml.str();
    }
    
    static bool validateYAML(const std::string& yamlStr) {
        if (yamlStr.empty()) return false;
        
        // Basic validation - check for key-value format
        size_t colonCount = 0;
        for (char c : yamlStr) {
            if (c == ':') colonCount++;
        }
        
        return colonCount > 0;
    }
};

// ConfigValidator implementation
ValidationResult ConfigValidator::validate(const Config& config, const std::vector<ConfigSchema>& schema) {
    ValidationResult result;
    
    // Check for required keys
    for (const auto& entry : config.getKeys()) {
        bool foundInSchema = false;
        for (const auto& schemaEntry : schema) {
            if (schemaEntry.key == entry) {
                foundInSchema = true;
                
                // Validate value type
                auto value = config.get<std::string>(entry);
                if (value.has_value()) {
                    if (!validateValue(value.value(), "string")) {  // Simplified
                        result.addError("Invalid value for key: " + entry);
                    }
                }
                break;
            }
        }
        
        if (!foundInSchema) {
            result.addWarning("Key not defined in schema: " + entry);
        }
    }
    
    // Check for missing required keys
    for (const auto& schemaEntry : schema) {
        if (schemaEntry.isRequired && !config.has(schemaEntry.key)) {
            result.addError("Required key missing: " + schemaEntry.key);
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validate(const std::vector<std::string>& keys, 
                                          const std::vector<ConfigSchema>& schema,
                                          const std::string& operation) {
    ValidationResult result;
    
    for (const auto& schemaEntry : schema) {
        if (schemaEntry.isRequired) {
            bool found = false;
            for (const auto& key : keys) {
                if (key == schemaEntry.key) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                result.addError("Required key missing for " + operation + ": " + schemaEntry.key);
            }
        }
    }
    
    return result;
}

bool ConfigValidator::validateValue(const ConfigValue& value, const std::string& typeName) {
    if (typeName == "int") {
        return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    } else if (typeName == "float" || typeName == "double") {
        return std::holds_alternative<double>(value);
    } else if (typeName == "bool") {
        return std::holds_alternative<bool>(value);
    } else if (typeName == "string") {
        return std::holds_alternative<std::string>(value);
    } else if (typeName == "int_array") {
        return std::holds_alternative<std::vector<int>>(value);
    } else if (typeName == "float_array" || typeName == "double_array") {
        return std::holds_alternative<std::vector<double>>(value);
    } else if (typeName == "string_array") {
        return std::holds_alternative<std::vector<std::string>>(value);
    } else if (typeName == "float32_array") {
        return std::holds_alternative<std::vector<float>>(value);
    }
    
    return false;
}

std::string ConfigValidator::getTypeName(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "int_array";
    if (std::holds_alternative<std::vector<double>>(value)) return "double_array";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "string_array";
    if (std::holds_alternative<std::vector<float>>(value)) return "float32_array";
    return "unknown";
}

// Main Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schemas;
    bool changeTrackingEnabled;
    std::vector<std::pair<std::string, ConfigSource>> changeHistory;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->changeTrackingEnabled = false;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::string format = detectFileFormat(filepath);
    return loadFromString(filepath, format);
}

bool Config::loadFromString(const std::string& content, const std::string& format) {
    std::unordered_map<std::string, std::string> parsedData;
    bool success = false;
    
    if (format == "json") {
        success = JSONUtils::parseJSONString(content, parsedData);
    } else if (format == "yaml" || format == "yml") {
        success = YAMLUtils::parseYAMLString(content, parsedData);
    } else {
        // Treat as simple key=value format
        std::stringstream ss(content);
        std::string line;
        while (std::getline(ss, line)) {
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
                
                parsedData[key] = value;
            }
        }
        success = true;
    }
    
    if (success) {
        // Convert parsed data to Config entries
        for (const auto& pair : parsedData) {
            // Try to determine type from value
            std::string value = pair.second;
            std::optional<ConfigValue> typedValue = JSONUtils::stringToConfigValue(value, "auto");
            
            if (typedValue.has_value()) {
                addEntry(pair.first, typedValue.value(), ConfigSource::File);
            } else {
                // Fallback to string
                addEntry(pair.first, ConfigValue(value), ConfigSource::File);
            }
        }
        return true;
    }
    
    return false;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
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
    
    if (format == "json") {
        std::unordered_map<std::string, std::string> jsonData;
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = JSONUtils::configValueToString(entry.value);
        }
        file << JSONUtils::serializeToJSON(jsonData);
    } else if (format == "yaml" || format == "yml") {
        std::unordered_map<std::string, std::string> yamlData;
        for (const auto& entry : pImpl->entries) {
            yamlData[entry.key] = JSONUtils::configValueToString(entry.value);
        }
        file << YAMLUtils::serializeToYAML(yamlData);
    } else {
        // Simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << JSONUtils::configValueToString(entry.value) << "\n";
        }
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
    bool changed = true;
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Check if value actually changed
        if (it->value == value && it->source == source) {
            changed = false;
        }
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
        changed = true;
    }
    
    if (changed && pImpl->changeTrackingEnabled) {
        pImpl->changeHistory.emplace_back(key, source);
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<float>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::setValidated(const std::string& key, const ConfigValue& value, ConfigSource source,
                         const std::string& pattern, const std::string& message) {
    // Check if key has a schema definition
    bool schemaValid = true;
    for (const auto& schema : pImpl->schemas) {
        if (schema.key == key) {
            schemaValid = validateAgainstSchema(schema);
            if (!schemaValid) {
                if (message.empty()) {
                    message = "Value does not match schema for key: " + key;
                }
                break;
            }
        }
    }
    
    // Validate pattern if provided
    if (schemaValid && !pattern.empty() && std::holds_alternative<std::string>(value)) {
        std::string strValue = std::get<std::string>(value);
        std::regex regex(pattern);
        if (!std::regex_match(strValue, regex)) {
            if (message.empty()) {
                message = "Value does not match validation pattern for key: " + key;
            }
            schemaValid = false;
        }
    }
    
    if (schemaValid) {
        set(key, value, source);
        return true;
    }
    
    return false;
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
    pImpl->schemas.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries, " << pImpl->schemas.size() << " schemas):\n";
    oss << "Source Distribution: ";
    int defaultCount = 0, fileCount = 0, cliCount = 0, runtimeCount = 0;
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default: defaultCount++; break;
            case ConfigSource::File: fileCount++; break;
            case ConfigSource::CommandLine: cliCount++; break;
            case ConfigSource::Runtime: runtimeCount++; break;
        }
    }
    oss << "Default: " << defaultCount << ", File: " << fileCount << ", CLI: " << cliCount << ", Runtime: " << runtimeCount << "\n";
    oss << "\nConfiguration Entries:";
    
    for (const auto& entry : pImpl->entries) {
        oss << "\n  " << entry.key << " = [";
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
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
    }
    
    if (!pImpl->schemas.empty()) {
        oss << "\n\nConfiguration Schemas:";
        for (const auto& schema : pImpl->schemas) {
            oss << "\n  " << schema.key << " (required: " << (schema.isRequired ? "yes" : "no") << ")";
            if (!schema.description.empty()) {
                oss << " - " << schema.description;
            }
        }
    }
    
    return oss.str();
}

ValidationResult Config::validate(const std::vector<ConfigSchema>& schema) const {
    return ConfigValidator::validate(*this, schema);
}

bool Config::validateValue(const std::string& key, const std::string& typeName) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });

    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return ConfigValidator::validateValue(it->value, typeName);
}

void Config::addSchema(const ConfigSchema& schema) {
    pImpl->schemas.push_back(schema);
}

void Config::removeSchema(const std::string& key) {
    pImpl->schemas.erase(
        std::remove_if(pImpl->schemas.begin(), pImpl->schemas.end(),
            [&key](const ConfigSchema& s) { return s.key == key; }),
        pImpl->schemas.end()
    );
}

std::vector<ConfigSchema> Config::getSchemas() const {
    return pImpl->schemas;
}

void Config::clearSchemas() {
    pImpl->schemas.clear();
}

std::string Config::exportToJSON() const {
    std::unordered_map<std::string, std::string> jsonData;
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = JSONUtils::configValueToString(entry.value);
    }
    return JSONUtils::serializeToJSON(jsonData);
}

std::string Config::exportToYAML() const {
    std::unordered_map<std::string, std::string> yamlData;
    for (const auto& entry : pImpl->entries) {
        yamlData[entry.key] = JSONUtils::configValueToString(entry.value);
    }
    return YAMLUtils::serializeToYAML(yamlData);
}

bool Config::importFromJSON(const std::string& jsonStr) {
    std::unordered_map<std::string, std::string> parsedData;
    if (JSONUtils::parseJSONString(jsonStr, parsedData)) {
        // Clear existing entries and import new ones
        clear();
        for (const auto& pair : parsedData) {
            std::optional<ConfigValue> typedValue = JSONUtils::stringToConfigValue(pair.second, "auto");
            if (typedValue.has_value()) {
                addEntry(pair.first, typedValue.value(), ConfigSource::File);
            }
        }
        return true;
    }
    return false;
}

bool Config::importFromYAML(const std::string& yamlStr) {
    std::unordered_map<std::string, std::string> parsedData;
    if (YAMLUtils::parseYAMLString(yamlStr, parsedData)) {
        // Clear existing entries and import new ones
        clear();
        for (const auto& pair : parsedData) {
            std::optional<ConfigValue> typedValue = JSONUtils::stringToConfigValue(pair.second, "auto");
            if (typedValue.has_value()) {
                addEntry(pair.first, typedValue.value(), ConfigSource::File);
            }
        }
        return true;
    }
    return false;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });

        if (it != pImpl->entries.end()) {
            // Key exists, update only if source is higher priority
            if (source == ConfigSource::Runtime || 
                (source == ConfigSource::File && it->source == ConfigSource::Default) ||
                (source == ConfigSource::CommandLine && it->source != ConfigSource::File)) {
                it->value = entry.value;
                it->source = entry.source;
            }
        } else {
            // Key doesn't exist, add it
            addEntry(entry.key, entry.value, entry.source);
        }
    }
}

void Config::override(const Config& other) {
    // Replace existing entries with those from other config
    clear();
    for (const auto& entry : other.pImpl->entries) {
        addEntry(entry.key, entry.value, entry.source);
    }
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCount(ConfigSource source) const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            count++;
        }
    }
    return count;
}

float Config::getSourceRatio(ConfigSource source) const {
    if (pImpl->entries.empty()) return 0.0f;
    return static_cast<float>(getEntryCount(source)) / static_cast<float>(pImpl->entries.size());
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });

        if (it == other.pImpl->entries.end()) {
            return false;
        }
        
        if (it->value != entry.value || it->source != entry.source) {
            return false;
        }
    }
    
    return true;
}

size_t Config::hash() const {
    size_t hashValue = pImpl->entries.size();
    for (const auto& entry : pImpl->entries) {
        hashValue ^= std::hash<std::string>{}(entry.key);
        hashValue ^= std::hash<ConfigValue>{}(entry.value);
        hashValue ^= std::hash<int>{}(static_cast<int>(entry.source));
    }
    return hashValue;
}

std::vector<std::pair<std::string, ConfigSource>> Config::getChangeHistory() const {
    return pImpl->changeHistory;
}

void Config::enableChangeTracking(bool enable) {
    pImpl->changeTrackingEnabled = enable;
    if (!enable) {
        pImpl->changeHistory.clear();
    }
}

bool Config::isChangeTrackingEnabled() const {
    return pImpl->changeTrackingEnabled;
}

void Config::addEntry(const std::string& key, const ConfigValue& value, ConfigSource source,
                     const std::string& description, const std::string& pattern) {
    pImpl->entries.emplace_back(key, value, source, description, pattern);
}

ConfigEntry* Config::findEntry(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });

    return it != pImpl->entries.end() ? &(*it) : nullptr;
}

const ConfigEntry* Config::findEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });

    return it != pImpl->entries.end() ? &(*it) : nullptr;
}

bool Config::validateAgainstSchema(const ConfigSchema& schema) const {
    // Check if key exists
    const ConfigEntry* entry = findEntry(schema.key);
    if (!entry) {
        return !schema.isRequired; // Missing key is OK if not required
    }
    
    // Validate value type
    if (schema.validTypes.empty()) {
        return true; // No type restrictions
    }
    
    std::string actualType = ConfigValidator::getTypeName(entry->value);
    bool typeValid = false;
    for (const auto& validType : schema.validTypes) {
        if (validType == actualType || validType == "auto") {
            typeValid = true;
            break;
        }
    }
    
    if (!typeValid) {
        return false;
    }
    
    // TODO: Add pattern validation if schema includes validation patterns
    return true;
}

bool Config::validateEntryAgainstSchema(const ConfigEntry& entry, const ConfigSchema& schema) const {
    if (entry.key != schema.key) {
        return false;
    }
    
    // Validate type
    std::string actualType = ConfigValidator::getTypeName(entry.value);
    bool typeValid = false;
    for (const auto& validType : schema.validTypes) {
        if (validType == actualType || validType == "auto") {
            typeValid = true;
            break;
        }
    }
    
    return typeValid;
}

} // namespace nlm

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