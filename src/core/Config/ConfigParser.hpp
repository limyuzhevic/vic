// ConfigParser.hpp - Configuration file parser with JSON and key=value support
#pragma once

#include "Config.hpp"
#include <string>
#include <map>
#include <optional>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace nlm {

// Configuration parser that supports JSON and key=value format
class ConfigParser {
public:
    // Constructor
    ConfigParser(Config& config);
    
    // Load configuration from file
    // Supports both JSON format and simple key=value format
    bool loadFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    
    // Load configuration from JSON string
    bool loadJsonString(const std::string& jsonString, ConfigSource source = ConfigSource::File);
    
    // Load configuration from key=value string
    bool loadKeyValueString(const std::string& kvString, ConfigSource source = ConfigSource::File);
    
    // Get file extension for format detection
    static std::string getFileExtension(const std::string& filename);
    
    // Check if file is JSON format based on extension and content
    static bool isJsonFile(const std::string& filename);
    
    // Parse JSON schema
    struct Schema {
        std::string type; // "object", "string", "number", "integer", "boolean", "array"
        std::string description;
        std::optional<std::string> pattern;
        std::optional<double> minimum;
        std::optional<double> maximum;
        std::optional<int> minLength;
        std::optional<int> maxLength;
        std::optional<size_t> minItems;
        std::optional<size_t> maxItems;
        // New: Pattern matching for enhanced validation
        std::optional<std::string> matchPattern;
        // New: Custom validation function callback
        std::function<bool(const ConfigValue&)> customValidator;
    };
    
    // Validate configuration against schema
    bool validateWithSchema(const Schema& schema, const std::string& key, const ConfigValue& value) const;
    
    // Generate documentation from configuration
    std::string generateDocumentation(const std::map<std::string, Schema>& schema) const;
    
    // Generate comprehensive documentation with advanced features
    std::string generateAdvancedDocumentation(const std::map<std::string, Schema>& schema, 
                                              const std::map<std::string, std::string>& examples) const;
    
    // Get parser statistics
    struct Stats {
        size_t totalKeysLoaded = 0;
        size_t jsonKeysLoaded = 0;
        size_t kvKeysLoaded = 0;
        size_t validationErrors = 0;
        double parsingTimeMs = 0.0;
    };
    
    const Stats& getStats() const { return stats; }
    
    // Phase 6 enhanced configuration
    bool isPhase6Config(const std::string& filepath) const;
    void applyPhase6Enhancements(Config& config);
    
    // Enhanced command-line argument parsing with validation
    bool loadFromArgsWithValidation(int argc, char** argv, const std::map<std::string, Schema>& schema);
    
    // Configuration transformation pipeline
    struct Transformation {
        std::string operation; // "scale", "clamp", "normalize", "apply_formula"
        std::string inputKey;
        std::string outputKey;
        std::map<std::string, std::string> parameters;
    };
    
    bool applyTransformations(const std::vector<Transformation>& transformations);
    
private:
    Config& config;
    Stats stats;
    
    // Internal parsing methods
    bool parseJsonFile(const std::string& filepath, ConfigSource source);
    bool parseSimpleFile(const std::string& filepath, ConfigSource source);
    bool parseJsonObject(const nlohmann::json& json, ConfigSource source);
    bool parseSimpleString(const std::string& line, ConfigSource source);
    
    // Helper methods
    void logError(const std::string& message);
    ConfigValue jsonToConfigValue(const nlohmann::json& value);
    nlohmann::json configValueToJson(const ConfigValue& value);
    
    // Format conversion helpers
    static std::string trim(const std::string& str);
    static bool isJsonContent(const std::string& content);
    static bool validateStringPattern(const std::string& value, const std::optional<std::string>& pattern);
    static double applyFormula(const std::string& formula, const std::map<std::string, double>& variables);
};

} // namespace nlm
