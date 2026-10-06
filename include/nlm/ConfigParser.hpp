// Enhanced Configuration Parser for NLM
// Supports JSON, YAML, and simple key=value formats
// Provides validation, type safety, and comprehensive error handling

#ifndef NLM_CONFIG_PARSER_HPP
#define NLM_CONFIG_PARSER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <optional>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

// Type definitions for configuration values
using ConfigValue = std::variant<int, int64_t, double, float, bool, std::string>;
using ConfigMap = std::unordered_map<std::string, ConfigValue>;
using ConfigArray = std::vector<ConfigValue>;
using ConfigTree = std::unordered_map<std::string, ConfigValue>;

enum class ConfigFormat {
    UNDEFINED,
    SIMPLE,
    JSON,
    YAML,
    UNKNOWN
};

enum class ParseErrorType {
    SUCCESS,
    FILE_NOT_FOUND,
    INVALID_FORMAT,
    SYNTAX_ERROR,
    TYPE_ERROR,
    VALIDATION_ERROR,
    MEMORY_ERROR
};

struct ParseResult {
    ParseErrorType error;
    std::string errorMessage;
    std::string filePath;
    ConfigMap parsedConfig;
    std::vector<std::string> warnings;
};

class ConfigParser {
public:
    ConfigParser();
    ~ConfigParser() = default;
    
    // Parse configuration files
    ParseResult parseFile(const std::string& filePath);
    ParseResult parseString(const std::string& content, ConfigFormat format = ConfigFormat::UNDEFINED);
    
    // Format detection and conversion
    ConfigFormat detectFormat(const std::string& content) const;
    ParseResult convertSimpleToMap(const std::string& content);
    
    // JSON parsing
    ParseResult parseJSON(const std::string& content);
    bool parseJSONValue(const std::string& content, size_t& pos, ConfigValue& value);
    bool parseJSONString(const std::string& content, size_t& pos, std::string& value);
    bool parseJSONNumber(const std::string& content, size_t& pos, ConfigValue& value);
    bool parseJSONBoolean(const std::string& content, size_t& pos, ConfigValue& value);
    bool parseJSONNull(const std::string& content, size_t& pos);
    size_t skipWhitespace(const std::string& content, size_t pos) const;
    size_t skipString(const std::string& content, size_t pos) const;
    
    // YAML parsing
    ParseResult parseYAML(const std::string& content);
    std::vector<std::string> splitYAMLLines(const std::string& content) const;
    ParseResult parseYAMLDocument(const std::vector<std::string>& lines);
    bool parseYAMLKeyValue(const std::string& line, std::string& key, ConfigValue& value);
    
    // Validation and conversion
    ParseResult validateConfig(const ConfigMap& config) const;
    std::optional<ConfigValue> getConfigValue(const ConfigMap& config, const std::string& key) const;
    template<typename T>
    std::optional<T> getTypedValue(const ConfigMap& config, const std::string& key) const;
    
    // Utility functions
    std::string trim(const std::string& str) const;
    bool startsWith(const std::string& str, const std::string& prefix) const;
    bool endsWith(const std::string& str, const std::string& suffix) const;
    size_t findUnquoted(const std::string& str, char ch, size_t start = 0) const;
    std::string escapeString(const std::string& str) const;
    std::string unescapeString(const std::string& str) const;
    
    // Configuration schema validation
    struct ParameterInfo {
        std::string name;
        std::string type;
        std::string defaultValue;
        std::string description;
        std::vector<std::string> validValues;
        double minValue;
        double maxValue;
        bool required;
        std::string dependency;
    };
    
    std::vector<ParameterInfo> getConfigurationSchema() const;
    ParseResult validateAgainstSchema(const ConfigMap& config) const;
    
private:
    static constexpr size_t MAX_JSON_DEPTH = 10;
    static constexpr size_t MAX_STRING_LENGTH = 10000;
    static constexpr size_t MAX_ARRAY_SIZE = 10000;
    static constexpr double DEFAULT_NUMBER_MIN = -1e10;
    static constexpr double DEFAULT_NUMBER_MAX = 1e10;
    
    ConfigMap parameterSchema;
    std::vector<std::string> supportedFileExtensions;
    bool allowUnknownParameters;
    bool strictTypeChecking;
};

} // namespace nlm

#endif // NLM_CONFIG_PARSER_HPP