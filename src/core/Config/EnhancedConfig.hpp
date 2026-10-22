#ifndef NLM_CONFIG_ENHANCED_H
#define NLM_CONFIG_ENHANCED_H

#include "Config.hpp"
#include "JsonParser.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

class EnhancedConfig : public Config {
public:
    // JSON/YAML loading
    bool loadFromJson(const std::string& jsonStr, ConfigSource source = ConfigSource::File);
    bool loadFromJsonFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    bool loadFromYaml(const std::string& yamlStr, ConfigSource source = ConfigSource::File);
    bool loadFromYamlFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    
    // Conversion utilities
    static std::string toJson(const Config& config);
    static std::string toYaml(const Config& config);
    
    // Validation
    bool validate(const std::string& schemaJson) const;
    std::vector<std::string> getValidationErrors() const;
    
    // Config merging
    void merge(const Config& other, ConfigSource source = ConfigSource::Default);
    bool hasKey(const std::string& key) const;
    
    // Advanced query
    std::vector<std::string> getKeysStartingWith(const std::string& prefix) const;
    bool matchesPattern(const std::string& pattern) const;
    
private:
    std::vector<std::string> validationErrors;
    
    // Helper methods for JSON/YAML conversion
    static JsonValue configEntryToJson(const ConfigEntry& entry);
    static std::optional<ConfigEntry> jsonToConfigEntry(const std::string& key, const JsonValue& value, ConfigSource source);
    
    // Pattern matching
    bool matchesPatternRecursive(const std::string& pattern, const std::string& text, 
                                size_t patternPos, size_t textPos) const;
};

} // namespace nlm

#endif // NLM_CONFIG_ENHANCED_H