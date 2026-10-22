#include "EnhancedConfig.hpp"

namespace nlm {

bool EnhancedConfig::loadFromJson(const std::string& jsonStr, ConfigSource source) {
    auto jsonValue = JsonParser::parse(jsonStr);
    if (!jsonValue) {
        NLM_LOG_ERROR("Failed to parse JSON: " + jsonStr);
        return false;
    }
    
    if (!std::holds_alternative<std::unordered_map<std::string, JsonValue>>(*jsonValue)) {
        NLM_LOG_ERROR("JSON root must be an object");
        return false;
    }
    
    const auto& obj = std::get<std::unordered_map<std::string, JsonValue>>(*jsonValue);
    
    clear(); // Clear existing entries
    
    for (const auto& pair : obj) {
        auto entryOpt = jsonToConfigEntry(pair.first, pair.second, source);
        if (entryOpt) {
            pImpl->entries.push_back(*entryOpt);
        }
    }
    
    return true;
}

bool EnhancedConfig::loadFromJsonFile(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return loadFromJson(buffer.str(), source);
}

bool EnhancedConfig::loadFromYaml(const std::string& yamlStr, ConfigSource source) {
    // Simple YAML parsing - YAML is similar to JSON for basic cases
    // For true YAML support, a proper YAML parser would be needed
    return loadFromJson(yamlStr, source);
}

bool EnhancedConfig::loadFromYamlFile(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open YAML file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return loadFromYaml(buffer.str(), source);
}

std::string EnhancedConfig::toJson(const Config& config) {
    std::unordered_map<std::string, JsonValue> jsonObj;
    
    for (const auto& entry : config.pImpl->entries) {
        jsonObj[entry.key] = configEntryToJson(entry);
    }
    
    return JsonParser::serialize(jsonObj);
}

std::string EnhancedConfig::toYaml(const Config& config) {
    // For YAML, we could add indentation, comments, etc.
    // For now, use the same format as JSON
    return toJson(config);
}

bool EnhancedConfig::validate(const std::string& schemaJson) const {
    validationErrors.clear();
    
    auto schema = JsonParser::parse(schemaJson);
    if (!schema) {
        validationErrors.push_back("Invalid schema JSON");
        return false;
    }
    
    if (!std::holds_alternative<std::unordered_map<std::string, JsonValue>>(*schema)) {
        validationErrors.push_back("Schema root must be an object");
        return false;
    }
    
    const auto& schemaObj = std::get<std::unordered_map<std::string, JsonValue>>(*schema);
    
    for (const auto& pair : schemaObj) {
        const std::string& key = pair.first;
        const JsonValue& schemaValue = pair.second;
        
        // Check if key exists in config
        if (!hasKey(key)) {
            validationErrors.push_back("Missing required key: " + key);
            continue;
        }
        
        // Validate type (simplified - just check if key exists for now)
        // In a full implementation, we would validate types based on schema
    }
    
    return validationErrors.empty();
}

std::vector<std::string> EnhancedConfig::getValidationErrors() const {
    return validationErrors;
}

void EnhancedConfig::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            // Add new entry
            pImpl->entries.push_back(entry);
        } else {
            // Update existing entry with higher priority source
            if (source > it->source) {
                *it = entry;
            }
        }
    }
}

bool EnhancedConfig::hasKey(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& entry) { return entry.key == key; });
}

std::vector<std::string> EnhancedConfig::getKeysStartingWith(const std::string& prefix) const {
    std::vector<std::string> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.size() >= prefix.size() && 
            entry.key.compare(0, prefix.size(), prefix) == 0) {
            result.push_back(entry.key);
        }
    }
    return result;
}

bool EnhancedConfig::matchesPattern(const std::string& pattern) const {
    // Simple wildcard pattern matching
    for (const auto& entry : pImpl->entries) {
        if (matchesPatternRecursive(pattern, entry.key, 0, 0)) {
            return true;
        }
    }
    return false;
}

JsonValue EnhancedConfig::configEntryToJson(const ConfigEntry& entry) {
    if (std::holds_alternative<std::string>(entry.value)) {
        return std::get<std::string>(entry.value);
    } else if (std::holds_alternative<int>(entry.value)) {
        return std::get<int>(entry.value);
    } else if (std::holds_alternative<int64_t>(entry.value)) {
        return std::get<int64_t>(entry.value);
    } else if (std::holds_alternative<double>(entry.value)) {
        return std::get<double>(entry.value);
    } else if (std::holds_alternative<bool>(entry.value)) {
        return std::get<bool>(entry.value);
    }
    
    // Default to string
    return "";
}

std::optional<ConfigEntry> EnhancedConfig::jsonToConfigEntry(const std::string& key, const JsonValue& value, ConfigSource source) {
    std::string description;
    
    if (std::holds_alternative<std::string>(value)) {
        description = "String value";
        return ConfigEntry(key, std::get<std::string>(value), source, description);
    } else if (std::holds_alternative<int>(value)) {
        description = "Integer value";
        return ConfigEntry(key, std::get<int>(value), source, description);
    } else if (std::holds_alternative<double>(value)) {
        description = "Floating point value";
        return ConfigEntry(key, std::get<double>(value), source, description);
    } else if (std::holds_alternative<bool>(value)) {
        description = "Boolean value";
        return ConfigEntry(key, std::get<bool>(value), source, description);
    } else if (std::holds_alternative<std::vector<JsonValue>>(value)) {
        description = "Array value (not directly supported in Config)";
        // For arrays, we could serialize to string
        return ConfigEntry(key, JsonParser::serialize(value), source, description);
    }
    
    return std::nullopt;
}

bool EnhancedConfig::matchesPatternRecursive(const std::string& pattern, const std::string& text,
                                            size_t patternPos, size_t textPos) const {
    if (patternPos == pattern.size() && textPos == text.size()) {
        return true; // Both patterns and text fully consumed
    }
    
    if (patternPos >= pattern.size()) {
        return false; // Pattern exhausted but text still has characters
    }
    
    char patternChar = pattern[patternPos];
    
    if (patternChar == '*') {
        // Match zero or more characters
        for (size_t i = textPos; i <= text.size(); ++i) {
            if (matchesPatternRecursive(pattern, text, patternPos + 1, i)) {
                return true;
            }
        }
        return false;
    } else if (patternChar == '?') {
        // Match exactly one character
        if (textPos < text.size() && matchesPatternRecursive(pattern, text, patternPos + 1, textPos + 1)) {
            return true;
        }
        return false;
    } else {
        // Match exact character
        if (textPos < text.size() && patternChar == text[textPos] && 
            matchesPatternRecursive(pattern, text, patternPos + 1, textPos + 1)) {
            return true;
        }
        return false;
    }
}

} // namespace nlm