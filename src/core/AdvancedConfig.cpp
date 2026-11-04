#include "AdvancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <cmath>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <yaml-cpp/yaml.h>

namespace nlm {

bool JSONParser::parse(const std::string& content, std::vector<ConfigEntry>& entries) {
    try {
        rapidjson::Document doc;
        doc.Parse(content.c_str());
        
        if (doc.HasParseError()) {
            return false;
        }
        
        if (!doc.IsObject()) {
            return false;
        }
        
        for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
            std::string key = it->name.GetString();
            ConfigValue value;
            
            if (it->value.IsString()) {
                value = it->value.GetString();
            } else if (it->value.IsInt()) {
                value = it->value.GetInt();
            } else if (it->value.IsInt64()) {
                value = it->value.GetInt64();
            } else if (it->value.IsUint()) {
                value = it->value.GetUint();
            } else if (it->value.IsInt64()) {
                value = it->value.GetInt64();
            } else if (it->value.IsDouble()) {
                value = it->value.GetDouble();
            } else if (it->value.IsFloat()) {
                value = it->value.GetFloat();
            } else if (it->value.IsBool()) {
                value = it->value.GetBool();
            } else if (it->value.IsArray()) {
                std::vector<double> array;
                for (const auto& element : it->value.GetArray()) {
                    if (element.IsInt()) {
                        array.push_back(element.GetInt());
                    } else if (element.IsDouble()) {
                        array.push_back(element.GetDouble());
                    } else if (element.IsString()) {
                        array.push_back(std::stod(element.GetString()));
                    }
                }
                value = array;
            }
            
            entries.emplace_back(key, value, ConfigSource::File, "");
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string JSONParser::serialize(const std::vector<ConfigEntry>& entries) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    
    writer.StartObject();
    for (const auto& entry : entries) {
        writer.Key(entry.key.c_str());
        
        std::visit([&writer](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                writer.Int(arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                writer.Int64(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                writer.Double(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                writer.Bool(arg);
            } else if constexpr (std::is_same_v<T, std::string>) {
                writer.String(arg.c_str());
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                writer.StartArray();
                for (const auto& item : arg) {
                    writer.Int(item);
                }
                writer.EndArray();
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                writer.StartArray();
                for (const auto& item : arg) {
                    writer.Double(item);
                }
                writer.EndArray();
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                writer.StartArray();
                for (const auto& item : arg) {
                    writer.String(item.c_str());
                }
                writer.EndArray();
            }
        }, entry.value);
    }
    writer.EndObject();
    
    return buffer.GetString();
}

bool JSONParser::parseValue(const std::string& content, size_t& pos, ConfigValue& value) {
    skipWhitespace(content, pos);
    
    if (pos >= content.size()) return false;
    
    char c = content[pos];
    
    if (c == '"' || c == '\'') {
        std::string str;
        if (parseString(content, pos, str)) {
            value = str;
            return true;
        }
        return false;
    } else if (c == '[') {
        return parseArray(content, pos, value);
    } else if (c == '{') {
        return parseObject(content, pos, value);
    } else if (c == 't' || c == 'f') {
        return parseBoolean(content, pos, value);
    } else if (c == '-' || (c >= '0' && c <= '9')) {
        return parseNumber(content, pos, value);
    }
    
    return false;
}

bool JSONParser::parseString(const std::string& content, size_t& pos, std::string& str) {
    char quote = content[pos++];
    size_t start = pos;
    
    while (pos < content.size()) {
        if (content[pos] == '\\') {
            pos++;
            if (pos >= content.size()) return false;
        } else if (content[pos] == quote) {
            str = content.substr(start, pos - start);
            pos++;
            return true;
        }
        pos++;
    }
    
    return false;
}

bool JSONParser::parseNumber(const std::string& content, size_t& pos, ConfigValue& value) {
    size_t start = pos;
    bool isFloat = false;
    
    if (content[pos] == '-') {
        pos++;
    }
    
    while (pos < content.size()) {
        char c = content[pos];
        if (c >= '0' && c <= '9') {
            pos++;
        } else if (c == '.' && !isFloat) {
            isFloat = true;
            pos++;
        } else {
            break;
        }
    }
    
    std::string numStr = content.substr(start, pos - start);
    
    try {
        if (isFloat) {
            float floatVal = std::stof(numStr);
            value = floatVal;
        } else {
            int intVal = std::stoi(numStr);
            value = intVal;
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool JSONParser::parseBoolean(const std::string& content, size_t& pos, ConfigValue& value) {
    if (content.substr(pos, 4) == "true") {
        value = true;
        pos += 4;
        return true;
    } else if (content.substr(pos, 5) == "false") {
        value = false;
        pos += 5;
        return true;
    }
    return false;
}

bool JSONParser::parseArray(const std::string& content, size_t& pos, ConfigValue& value) {
    if (content[pos++] != '[') return false;
    
    std::vector<double> array;
    
    skipWhitespace(content, pos);
    
    if (content[pos] == ']') {
        value = array;
        pos++;
        return true;
    }
    
    while (true) {
        ConfigValue element;
        if (!parseValue(content, pos, element)) {
            return false;
        }
        
        if constexpr (std::is_same_v<decltype(element), std::vector<double>>) {
            array.push_back(std::get<double>(element));
        }
        
        skipWhitespace(content, pos);
        
        if (pos >= content.size()) return false;
        
        if (content[pos] == ']') {
            pos++;
            break;
        } else if (content[pos] == ',') {
            pos++;
            skipWhitespace(content, pos);
        } else {
            return false;
        }
    }
    
    value = array;
    return true;
}

bool JSONParser::parseObject(const std::string& content, size_t& pos, std::vector<ConfigEntry>& entries) {
    if (content[pos++] != '{') return false;
    
    skipWhitespace(content, pos);
    
    while (pos < content.size()) {
        skipWhitespace(content, pos);
        
        if (content[pos] == '}') {
            pos++;
            break;
        }
        
        std::string key;
        if (!parseString(content, pos, key)) {
            return false;
        }
        
        skipWhitespace(content, pos);
        
        if (pos >= content.size() || content[pos] != ':') {
            return false;
        }
        
        pos++;
        skipWhitespace(content, pos);
        
        ConfigValue value;
        if (!parseValue(content, pos, value)) {
            return false;
        }
        
        entries.emplace_back(key, value, ConfigSource::File, "");
        
        skipWhitespace(content, pos);
        
        if (pos >= content.size()) return false;
        
        if (content[pos] == '}') {
            pos++;
            break;
        } else if (content[pos] == ',') {
            pos++;
        } else {
            return false;
        }
    }
    
    return true;
}

void JSONParser::skipWhitespace(const std::string& content, size_t& pos) {
    while (pos < content.size() && isWhitespace(content[pos])) {
        pos++;
    }
}

bool JSONParser::expect(const std::string& content, size_t& pos, char expected) {
    skipWhitespace(content, pos);
    if (pos < content.size() && content[pos] == expected) {
        pos++;
        return true;
    }
    return false;
}

std::string JSONParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    
    if (start == std::string::npos) {
        return "";
    }
    
    return str.substr(start, end - start + 1);
}

bool JSONParser::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool JSONParser::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool JSONParser::isHexDigit(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool YAMLParser::parse(const std::string& content, std::vector<ConfigEntry>& entries) {
    try {
        YAML::Node doc = YAML::Load(content);
        
        if (!doc.IsMap()) {
            return false;
        }
        
        for (const auto& pair : doc) {
            std::string key = pair.first.Scalar();
            ConfigValue value;
            
            if (pair.second.IsScalar()) {
                std::string str = pair.second.Scalar();
                
                // Try to parse as boolean
                if (str == "true" || str == "false") {
                    value = (str == "true");
                } else if (str.find('.') != std::string::npos ||
                          str.find('e') != std::string::npos ||
                          str.find('E') != std::string::npos) {
                    // Try to parse as double
                    try {
                        double doubleVal = std::stod(str);
                        value = doubleVal;
                    } catch (...) {
                        value = str;
                    }
                } else {
                    // Try to parse as integer
                    try {
                        int intVal = std::stoi(str);
                        value = intVal;
                    } catch (...) {
                        value = str;
                    }
                }
            } else if (pair.second.IsSequence()) {
                std::vector<double> array;
                for (const auto& element : pair.second) {
                    if (element.IsScalar()) {
                        std::string str = element.Scalar();
                        try {
                            double doubleVal = std::stod(str);
                            array.push_back(doubleVal);
                        } catch (...) {
                            // Convert string to number if possible, otherwise skip
                            try {
                                int intVal = std::stoi(str);
                                array.push_back(intVal);
                            } catch (...) {
                                // Skip non-numeric strings
                            }
                        }
                    }
                }
                value = array;
            } else if (pair.second.IsMap()) {
                // Nested YAML object - convert to string for now
                value = "YAML_OBJECT:" + pair.second.ToString();
            }
            
            entries.emplace_back(key, value, ConfigSource::File, "");
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string YAMLParser::serialize(const std::vector<ConfigEntry>& entries) {
    YAML::Emitter emitter;
    
    emitter << YAML::BeginMap;
    for (const auto& entry : entries) {
        emitter << YAML::Key << entry.key.c_str();
        
        std::visit([&emitter](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                emitter << static_cast<int>(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                emitter << YAML::DoubleQuoted << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                emitter << YAML::BeginSeq;
                for (const auto& item : arg) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                emitter << YAML::BeginSeq;
                for (const auto& item : arg) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                emitter << YAML::BeginSeq;
                for (const auto& item : arg) {
                    emitter << YAML::DoubleQuoted << item;
                }
                emitter << YAML::EndSeq;
            }
        }, entry.value);
    }
    emitter << YAML::EndMap;
    
    return emitter.c_str();
}

bool YAMLParser::parseYAML(const std::string& content, size_t& pos, std::vector<ConfigEntry>& entries, int indent) {
    skipWhitespace(content, pos);
    
    while (pos < content.size()) {
        // Skip comments
        if (content[pos] == '#') {
            while (pos < content.size() && content[pos] != '\n') {
                pos++;
            }
            continue;
        }
        
        // Check for end of document
        if (content[pos] == '
') {
            pos++;
            continue;
        }
        
        size_t lineStart = pos;
        size_t lineEnd = content.find('\n', pos);
        if (lineEnd == std::string::npos) {
            lineEnd = content.size();
        }
        
        std::string line = content.substr(lineStart, lineEnd - lineStart);
        
        // Calculate indentation
        size_t lineIndent = countIndent(line);
        if (lineIndent < indent) {
            return true; // Back to parent level
        }
        
        if (lineIndent > indent) {
            return false; // Indentation too deep
        }
        
        // Remove leading whitespace and content
        size_t contentPos = line.find_first_not_of(" \t");
        if (contentPos == std::string::npos) {
            continue;
        }
        
        std::string lineContent = line.substr(contentPos);
        
        // Check if it's a key-value pair
        size_t colonPos = lineContent.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(lineContent.substr(0, colonPos));
            std::string valueStr = trim(lineContent.substr(colonPos + 1));
            
            ConfigValue value;
            
            if (valueStr.empty()) {
                // Handle array or nested structure
                continue;
            }
            
            // Try to parse as boolean
            if (valueStr == "true" || valueStr == "false") {
                value = (valueStr == "true");
            } else if (valueStr.find('.') != std::string::npos ||
                      valueStr.find('e') != std::string::npos ||
                      valueStr.find('E') != std::string::npos) {
                try {
                    double doubleVal = std::stod(valueStr);
                    value = doubleVal;
                } catch (...) {
                    value = valueStr;
                }
            } else {
                try {
                    int intVal = std::stoi(valueStr);
                    value = intVal;
                } catch (...) {
                    value = valueStr;
                }
            }
            
            entries.emplace_back(key, value, ConfigSource::File, "");
            pos = lineEnd + 1;
        } else {
            // Handle array or nested structure (simplified)
            pos = lineEnd + 1;
        }
    }
    
    return true;
}

bool YAMLParser::parseYAMLKeyValue(const std::string& content, size_t& pos, std::vector<ConfigEntry>& entries) {
    // Simplified implementation
    return false;
}

bool YAMLParser::parseYAMLArray(const std::string& content, size_t& pos, ConfigValue& value) {
    // Simplified implementation
    return false;
}

bool YAMLParser::parseYAMLString(const std::string& content, size_t& pos, std::string& str) {
    // Simplified implementation
    return false;
}

bool YAMLParser::parseYAMLScalar(const std::string& content, size_t& pos, ConfigValue& value) {
    // Simplified implementation
    return false;
}

void YAMLParser::skipWhitespace(const std::string& content, size_t& pos) {
    while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n')) {
        pos++;
    }
}

void YAMLParser::skipComment(const std::string& content, size_t& pos) {
    while (pos < content.size() && content[pos] != '\n') {
        pos++;
    }
}

size_t YAMLParser::countIndent(const std::string& line) {
    size_t indent = 0;
    while (indent < line.size() && (line[indent] == ' ' || line[indent] == '\t')) {
        indent++;
    }
    return indent;
}

ConfigSchema::ConfigSchema() : trackChanges_(false), hasChanges_(false) {}

void ConfigSchema::addValidationRule(const std::string& key,
                                   std::function<bool(const ConfigValue&)> validator,
                                   const std::string& errorMessage,
                                   bool required) {
    ValidationRule rule;
    rule.key = key;
    rule.validator = validator;
    rule.errorMessage = errorMessage;
    rule.required = required;
    rules_.push_back(rule);
}

bool ConfigSchema::validate(const std::vector<ConfigEntry>& entries, std::vector<std::string>& errors) const {
    bool valid = true;
    
    for (const auto& rule : rules_) {
        bool found = false;
        for (const auto& entry : entries) {
            if (entry.key == rule.key) {
                found = true;
                if (!rule.validator(entry.value)) {
                    std::string error = rule.errorMessage.empty() ? 
                        "Validation failed for key: " + rule.key : rule.errorMessage;
                    errors.push_back(error);
                    valid = false;
                }
                break;
            }
        }
        
        if (rule.required && !found) {
            std::string error = "Missing required key: " + rule.key;
            errors.push_back(error);
            valid = false;
        }
    }
    
    return valid;
}

void ConfigSchema::setDefaultValidation() {
    // Add default validation rules for common configuration keys
    
    // Validate neuron count is positive
    addValidationRule("neuron_count", 
                     [](const ConfigValue& val) {
                         if (std::holds_alternative<int64_t>(val)) {
                             return std::get<int64_t>(val) > 0;
                         }
                         return false;
                     },
                     "Neuron count must be positive", true);
    
    // Validate region count is positive
    addValidationRule("region_count",
                     [](const ConfigValue& val) {
                         if (std::holds_alternative<int64_t>(val)) {
                             return std::get<int64_t>(val) > 0;
                         }
                         return false;
                     },
                     "Region count must be positive", true);
    
    // Validate connection probability is between 0 and 1
    addValidationRule("connection_probability",
                     [](const ConfigValue& val) {
                         if (std::holds_alternative<double>(val)) {
                             double val = std::get<double>(val);
                             return val >= 0.0 && val <= 1.0;
                         }
                         return false;
                     },
                     "Connection probability must be between 0 and 1", true);
    
    // Validate simulation timestep is positive
    addValidationRule("simulation_timestep",
                     [](const ConfigValue& val) {
                         if (std::holds_alternative<double>(val)) {
                             return std::get<double>(val) > 0.0;
                         }
                         return false;
                     },
                     "Simulation timestep must be positive", true);
}

AdvancedConfig::AdvancedConfig() : trackChanges_(false), hasChanges_(false) {}

AdvancedConfig::~AdvancedConfig() = default;

bool AdvancedConfig::loadFromFile(const std::string& filepath, const std::string& format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    std::vector<ConfigEntry> entries;
    bool success = false;
    
    if (format == "auto") {
        // Try JSON first, then YAML
        if (JSONParser::parse(content, entries)) {
            success = true;
        } else if (YAMLParser::parse(content, entries)) {
            success = true;
        }
    } else if (format == "json") {
        success = JSONParser::parse(content, entries);
    } else if (format == "yaml") {
        success = YAMLParser::parse(content, entries);
    }
    
    if (success) {
        config_->clear();
        for (const auto& entry : entries) {
            config_->set(entry.key, entry.value, entry.source);
        }
        markChanged();
    }
    
    return success;
}

bool AdvancedConfig::saveToFile(const std::string& filepath, const std::string& format) const {
    if (!config_) {
        return false;
    }
    
    // Convert config entries to a vector for serialization
    std::vector<ConfigEntry> entries;
    for (const auto& key : config_->getKeys()) {
        auto val = config_->get(key);
        if (val.has_value()) {
            // Need to find the source - this is a simplification
            ConfigSource source = ConfigSource::Runtime;
            entries.emplace_back(key, val.value(), source);
        }
    }
    
    std::string content;
    if (format == "json") {
        content = JSONParser::serialize(entries);
    } else {
        content = YAMLParser::serialize(entries);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

bool AdvancedConfig::setValue(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!config_) {
        return false;
    }
    
    config_->set(key, value, source);
    markChanged();
    return true;
}

bool AdvancedConfig::setString(const std::string& key, const std::string& value, ConfigSource source) {
    return setValue(key, ConfigValue(value), source);
}

bool AdvancedConfig::validate(std::vector<std::string>& errors) const {
    if (!schema_) {
        return true; // No schema means validation passes
    }
    
    std::vector<ConfigEntry> entries;
    for (const auto& key : config_->getKeys()) {
        auto val = config_->get(key);
        if (val.has_value()) {
            entries.emplace_back(key, val.value(), ConfigSource::Runtime);
        }
    }
    
    return schema_->validate(entries, errors);
}

} // namespace nlm
