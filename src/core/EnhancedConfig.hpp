// Enhanced Configuration Support for NLM
// Provides JSON and YAML file format support while maintaining backward compatibility
// Integrates seamlessly with the existing Config class

#ifndef NLM_ENHANCED_CONFIG_HPP
#define NLM_ENHANCED_CONFIG_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <fstream>

namespace nlm {

// Forward declarations
class EnhancedConfig;

// Configuration format detector
enum class ConfigFormat {
    Unknown,
    Text,    // Original key=value format
    JSON,
    YAML,
    XML
};

// Configuration value converter
class ConfigValueConverter {
public:
    // Convert string to appropriate type based on value
    static ConfigValue stringToConfigValue(const std::string& str);
    
    // Convert any type to string representation
    template<typename T>
    static std::string toString(const T& value);
    
    // Parse YAML scalar
    static bool parseYAMLScalar(const std::string& scalar, std::string& result, bool& isQuoted);
    
    // Parse JSON value
    static bool parseJSONValue(const std::string& json, size_t& pos, std::string& result);
    
    // Escape special characters for safe output
    static std::string escapeString(const std::string& str);
    
    // Unescape string
    static std::string unescapeString(const std::string& str);
};

// Simple JSON Parser (minimal implementation for configuration)
class SimpleJsonParser {
public:
    struct JsonValue {
        enum class Type {
            Null,
            Bool,
            Number,
            String,
            Array,
            Object
        };
        
        bool asBool() const { return type == Type::Bool && boolValue; }
        double asNumber() const { return type == Type::Number ? numberValue : 0.0; }
        const std::string& asString() const { return type == Type::String ? stringValue : emptyString; }
        const std::vector<JsonValue>& asArray() const { return type == Type::Array ? arrayValue : emptyArray; }
        const std::unordered_map<std::string, JsonValue>& asObject() const { return type == Type::Object ? objectValue : emptyObject; }
        Type type() const { return type; }
        
        JsonValue() : type(Type::Null) {}
        JsonValue(bool value) : type(Type::Bool) { boolValue = value; }
        JsonValue(double value) : type(Type::Number) { numberValue = value; }
        JsonValue(const std::string& value) : type(Type::String) { stringValue = value; }
        JsonValue(const std::vector<JsonValue>& value) : type(Type::Array) { arrayValue = value; }
        JsonValue(const std::unordered_map<std::string, JsonValue>& value) : type(Type::Object) { objectValue = value; }
        
    private:
        Type type;
        union {
            bool boolValue;
            double numberValue;
            std::string stringValue;
            std::vector<JsonValue> arrayValue;
            std::unordered_map<std::string, JsonValue> objectValue;
        };
        
        friend class SimpleJsonParser;
    };
    
    static bool parse(const std::string& json, JsonValue& result);
    static std::string serialize(const JsonValue& value);
    static std::string toString(const JsonValue& value);
    
private:
    static bool parseValue(const std::string& json, size_t& pos, JsonValue& result);
    static bool parseObject(const std::string& json, size_t& pos, std::unordered_map<std::string, JsonValue>& result);
    static bool parseArray(const std::string& json, size_t& pos, std::vector<JsonValue>& result);
    static bool parseString(const std::string& json, size_t& pos, std::string& result);
    static bool parseNumber(const std::string& json, size_t& pos, double& result);
    static bool parseBool(const std::string& json, size_t& pos, bool& result);
    static bool parseNull(const std::string& json, size_t& pos);
    static void skipWhitespace(const std::string& json, size_t& pos);
    static bool expectChar(const std::string& json, size_t& pos, char c);
    
    static const std::string emptyString;
    static const std::vector<JsonValue> emptyArray;
    static const std::unordered_map<std::string, JsonValue> emptyObject;
};

// Enhanced Configuration Class
class EnhancedConfig {
public:
    EnhancedConfig() : format(ConfigFormat::Unknown) {}
    
    // Load from file (automatic format detection)
    bool loadFromFile(const std::string& filepath) {
        format = detectFormat(filepath);
        
        switch (format) {
            case ConfigFormat::JSON:
                return loadFromJson(filepath);
            case ConfigFormat::YAML:
                return loadFromYaml(filepath);
            case ConfigFormat::Text:
                // Use original Config class
                return originalConfig.loadFromFile(filepath);
            default:
                return false;
        }
    }
    
    // Load from JSON
    bool loadFromJson(const std::string& json) {
        SimpleJsonParser::JsonValue root;
        if (!SimpleJsonParser::parse(json, root)) {
            return false;
        }
        
        if (root.type() != SimpleJsonParser::JsonValue::Type::Object) {
            return false;
        }
        
        const auto& obj = root.asObject();
        for (const auto& [key, value] : obj) {
            setFromJson(key, value);
        }
        
        return true;
    }
    
    // Load from JSON string
    bool loadFromJsonString(const std::string& json) {
        format = ConfigFormat::JSON;
        return loadFromJson(json);
    }
    
    // Load from JSON file
    bool loadFromJsonFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return loadFromJsonString(buffer.str());
    }
    
    // Load from YAML
    bool loadFromYaml(const std::string& yaml) {
        // For now, simplified YAML support
        // TODO: Implement full YAML parser
        if (isSimpleYaml(yaml)) {
            parseSimpleYaml(yaml);
            return true;
        }
        return false;
    }
    
    // Load from YAML string
    bool loadFromYamlString(const std::string& yaml) {
        format = ConfigFormat::YAML;
        return loadFromYaml(yaml);
    }
    
    // Load from YAML file
    bool loadFromYamlFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return loadFromYamlString(buffer.str());
    }
    
    // Save to JSON
    std::string saveToJson() const {
        SimpleJsonParser::JsonValue root;
        root.type(SimpleJsonParser::JsonValue::Type::Object);
        root.asObject() = getJsonObject();
        return SimpleJsonParser::serialize(root);
    }
    
    // Save to YAML
    std::string saveToYaml() const {
        return convertToYaml();
    }
    
    // Get configuration format
    ConfigFormat getFormat() const { return format; }
    const std::string& getFormatString() const {
        static const std::unordered_map<ConfigFormat, std::string> formatStrings = {
            {ConfigFormat::Unknown, "Unknown"},
            {ConfigFormat::Text, "Text"},
            {ConfigFormat::JSON, "JSON"},
            {ConfigFormat::YAML, "YAML"},
            {ConfigFormat::XML, "XML"}
        };
        return formatStrings.at(format);
    }
    
    // Enhanced summary with format information
    std::string getEnhancedSummary() const {
        std::ostringstream oss;
        oss << "Enhanced Configuration\n";
        oss << "=====================\n";
        oss << "Format: " << getFormatString() << "\n";
        oss << "Entries: " << getKeys().size() << "\n\n";
        
        oss << "Key-Value Pairs:\n";
        for (const auto& key : getKeys()) {
            oss << "  " << key << " = [";
            auto value = getValue(key);
            oss << convertToString(value) << "]\n";
        }
        
        return oss.str();
    }
    
    // Integration with original Config class
    const class Config& getOriginalConfig() const { return originalConfig; }
    class Config& getOriginalConfig() { return originalConfig; }
    
    // Delegate common operations to original Config
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        return originalConfig.get<T>(key);
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        return originalConfig.getOr<T>(key, defaultValue);
    }
    
    bool has(const std::string& key) const {
        return originalConfig.has(key);
    }
    
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) {
        originalConfig.set(key, value, source);
    }
    
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
        originalConfig.set(key, value, source);
    }
    
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
        originalConfig.set(key, value, source);
    }
    
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
        originalConfig.set(key, value, source);
    }
    
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
        originalConfig.set(key, value, source);
    }
    
    bool remove(const std::string& key) {
        return originalConfig.remove(key);
    }
    
    std::vector<std::string> getKeys() const {
        return originalConfig.getKeys();
    }
    
    void clear() {
        originalConfig.clear();
    }
    
    std::string summary() const {
        return originalConfig.summary();
    }
    
private:
    ConfigFormat format;
    class Config originalConfig; // Original config for backward compatibility
    
    // Format detection
    ConfigFormat detectFormat(const std::string& filepath) {
        std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == "json") {
            return ConfigFormat::JSON;
        } else if (ext == "yaml" || ext == "yml") {
            return ConfigFormat::YAML;
        } else if (ext == "xml") {
            return ConfigFormat::XML;
        } else {
            return ConfigFormat::Text;
        }
    }
    
    // JSON conversion helpers
    void setFromJson(const std::string& key, const SimpleJsonParser::JsonValue& value) {
        originalConfig.set(key, convertFromJson(value), ConfigSource::File);
    }
    
    SimpleJsonParser::JsonValue convertToJson(const ConfigValue& value) const {
        return std::visit([](const auto& arg) -> SimpleJsonParser::JsonValue {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                return SimpleJsonParser::JsonValue(static_cast<double>(arg));
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return SimpleJsonParser::JsonValue(static_cast<double>(arg));
            } else if constexpr (std::is_same_v<T, double>) {
                return SimpleJsonParser::JsonValue(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                return SimpleJsonParser::JsonValue(arg);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return SimpleJsonParser::JsonValue(arg);
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                std::vector<SimpleJsonParser::JsonValue> array;
                array.reserve(arg.size());
                for (auto item : arg) {
                    array.push_back(SimpleJsonParser::JsonValue(static_cast<double>(item)));
                }
                return SimpleJsonParser::JsonValue(array);
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                std::vector<SimpleJsonParser::JsonValue> array;
                array.reserve(arg.size());
                for (auto item : arg) {
                    array.push_back(SimpleJsonParser::JsonValue(item));
                }
                return SimpleJsonParser::JsonValue(array);
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                std::vector<SimpleJsonParser::JsonValue> array;
                array.reserve(arg.size());
                for (auto item : arg) {
                    array.push_back(SimpleJsonParser::JsonValue(item));
                }
                return SimpleJsonParser::JsonValue(array);
            } else {
                return SimpleJsonParser::JsonValue();
            }
        }, value);
    }
    
    ConfigValue convertFromJson(const SimpleJsonParser::JsonValue& value) const {
        switch (value.type()) {
            case SimpleJsonParser::JsonValue::Type::Null:
                return std::string("null");
            case SimpleJsonParser::JsonValue::Type::Bool:
                return value.asBool();
            case SimpleJsonParser::JsonValue::Type::Number:
                double num = value.asNumber();
                if (num == static_cast<int64_t>(num)) {
                    return static_cast<int64_t>(num);
                }
                return num;
            case SimpleJsonParser::JsonValue::Type::String:
                return value.asString();
            case SimpleJsonParser::JsonValue::Type::Array:
                // Convert JSON array to vector (simplified)
                return std::vector<ConfigValue>(); // TODO: Full implementation
            case SimpleJsonParser::JsonValue::Type::Object:
                // For now, treat objects as strings
                return "{}";
            default:
                return std::string("");
        }
    }
    
    // Simple YAML parsing (basic key: value format)
    bool isSimpleYaml(const std::string& yaml) {
        // Check if it's a simple YAML format with key: value pairs
        size_t colonPos = yaml.find(':');
        return colonPos != std::string::npos && colonPos > 0;
    }
    
    void parseSimpleYaml(const std::string& yaml) {
        std::istringstream stream(yaml);
        std::string line;
        
        while (std::getline(stream, line)) {
            // Skip empty lines and comments
            if (line.empty() || line.find('#') == 0) {
                continue;
            }
            
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos && colonPos > 0) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Try to convert value to appropriate type
                ConfigValue convertedValue = ConfigValueConverter::stringToConfigValue(value);
                originalConfig.set(key, convertedValue, ConfigSource::File);
            }
        }
    }
    
    // Convert ConfigValue to string for YAML output
    std::string convertToYaml() const {
        std::ostringstream oss;
        oss << "# NLM Configuration (YAML format)\n";
        
        for (const auto& key : getKeys()) {
            oss << key << ": ";
            
            auto value = getValue(key);
            oss << convertToYamlValue(value) << "\n";
        }
        
        return oss.str();
    }
    
    // Convert ConfigValue to YAML representation
    std::string convertToYamlValue(const ConfigValue& value) const {
        return std::visit([](const auto& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                // Check if it's an integer
                if (arg == static_cast<int64_t>(arg)) {
                    return std::to_string(static_cast<int64_t>(arg));
                }
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::string>) {
                // Escape special characters
                std::string escaped = ConfigValueConverter::escapeString(arg);
                if (escaped.find(' ') != std::string::npos || 
                    escaped.find('\n') != std::string::npos ||
                    escaped.find(':') != std::string::npos) {
                    return "\"" + escaped + "\"";
                }
                return escaped;
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
                return oss.str();
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    // Check if it's an integer
                    if (arg[i] == static_cast<int64_t>(arg[i])) {
                        oss << static_cast<int64_t>(arg[i]);
                    } else {
                        oss << arg[i];
                    }
                }
                oss << "]";
                return oss.str();
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << ConfigValueConverter::escapeString(arg[i]) << "\"";
                }
                oss << "]";
                return oss.str();
            } else {
                return "null";
            }
        }, value);
    }
    
    // Get Json object representation
    std::unordered_map<std::string, SimpleJsonParser::JsonValue> getJsonObject() const {
        std::unordered_map<std::string, SimpleJsonParser::JsonValue> result;
        for (const auto& key : getKeys()) {
            result[key] = convertToJson(getValue(key));
        }
        return result;
    }
    
    // Get value from original Config
    ConfigValue getValue(const std::string& key) const {
        // This is a simplified version - actual implementation would need to handle the ConfigValue variant
        // For now, return a string representation
        return key + "=" + "<value>";
    }
    
    // Convert ConfigValue to string for display
    std::string convertToString(const ConfigValue& value) const {
        return std::visit([](const auto& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else {
                return "<complex>";
            }
        }, value);
    }
    
    // Get ConfigValue from original Config (simplified)
    ConfigValue getValueFromOriginal(const std::string& key) const {
        // This is a simplified version - actual implementation would need to access the original Config's entries
        // For now, return a placeholder
        return std::string("");
    }
};

// Convenience functions
inline EnhancedConfig loadEnhancedConfig(const std::string& filepath) {
    EnhancedConfig config;
    config.loadFromFile(filepath);
    return config;
}

inline EnhancedConfig loadJsonConfig(const std::string& json) {
    EnhancedConfig config;
    config.loadFromJsonString(json);
    return config;
}

inline EnhancedConfig loadYamlConfig(const std::string& yaml) {
    EnhancedConfig config;
    config.loadFromYamlString(yaml);
    return config;
}

} // namespace nlm

#endif // NLM_ENHANCED_CONFIG_HPP

// Example usage:
// #include "EnhancedConfig.hpp"
// #include <iostream>
// 
// int main() {
//     // Load from JSON file
//     auto config = nlm::loadEnhancedConfig("config.json");
//     
//     std::cout << config.getEnhancedSummary();
//     
//     // Save to YAML format
//     std::string yaml = config.saveToYaml();
//     std::cout << "\nYAML Output:\n" << yaml;
//     
//     // Access values as before
//     auto neuronCount = config.get<int64_t>("neuron_count");
//     if (neuronCount) {
//         std::cout << "Neuron count: " << *neuronCount << std::endl;
//     }
//     
//     return 0;
// }