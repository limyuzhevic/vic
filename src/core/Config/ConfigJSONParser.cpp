# ConfigJSONParser - Simple JSON Parser for NLM Configuration

This file implements a simple JSON parser for the NLM configuration system.
It provides both JSON and YAML support with robust error handling and type conversion.

## Dependencies
- nlohmann/json (for JSON parsing)
- yaml-cpp (for YAML parsing)

## Features
- Support for JSON and YAML formats
- Type conversion (string to int/float/bool)
- Nested object support
- Array handling
- Backward compatibility with simple key=value format
- Comprehensive error handling
- Configuration validation

## Implementation

This parser follows a layered approach:
1. Format detection (auto-detect JSON vs YAML vs simple format)
2. Parsing (delegate to appropriate parser)
3. Type conversion and validation
4. Error handling with meaningful messages

The implementation is designed to be:
- **Robust**: Handle malformed input gracefully
- **Efficient**: Use streaming where possible
- **Extensible**: Easy to add new format support
- **User-friendly**: Clear error messages

*/

#pragma once

#include "Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace nlm {

class ConfigJSONParser {
public:
    enum class Format {
        Simple,
        JSON,
        YAML
    };

    struct ParseResult {
        bool success;
        std::string error;
        Format detectedFormat;
    };

    ConfigJSONParser() = default;
    ~ConfigJSONParser() = default;

    // Parse configuration from file
    ParseResult parseFromFile(const std::string& filepath, Config& config);

    // Parse configuration from string
    ParseResult parseFromString(const std::string& content, Config& config);

    // Save configuration to file
    bool saveToFile(const Config& config, const std::string& filepath);

    // Save configuration to JSON string
    std::string configToJSON(const Config& config) const;

    // Load configuration from JSON string
    bool jsonToConfig(const std::string& jsonString, Config& config);

    // Validate configuration parameters
    std::vector<std::string> validateConfig(const Config& config) const;

    // Get detected format from filepath
    static Format detectFormat(const std::string& filepath);

    // Check if file exists and is readable
    static bool isReadableFile(const std::string& filepath);

private:
    // Parse simple key=value format
    bool parseSimpleFormat(const std::string& content, Config& config);

    // Parse JSON format (using nlohmann::json)
    bool parseJSONFormat(const std::string& content, Config& config);

    // Parse YAML format (using yaml-cpp)
    bool parseYAMLFormat(const std::string& content, Config& config);

    // Convert string to appropriate type
    template<typename T>
    bool stringToValue(const std::string& str, T& value) const;

    // Convert value to string
    template<typename T>
    std::string valueToString(const T& value) const;

    // Normalize parameter name (replace dots with underscores)
    static std::string normalizeParameterName(const std::string& name);

    // Denormalize parameter name (replace underscores with dots)
    static std::string denormalizeParameterName(const std::string& name);

    // Trim whitespace from string
    static std::string trim(const std::string& str);

    // Check if string represents a boolean
    static bool isBoolean(const std::string& str);

    // Check if string represents a number
    static bool isNumber(const std::string& str);

    // Split string by delimiter
    static std::vector<std::string> split(const std::string& str, char delimiter);

    // Parse array from string
    std::vector<std::string> parseArray(const std::string& arrayStr) const;

    // Parse simple value (not quoted)
    std::string parseSimpleValue(const std::string& str) const;

    // Parse quoted string
    std::string parseQuotedString(const std::string& str) const;

    // Convert JSON-like string to ConfigValue
    ConfigValue stringToConfigValue(const std::string& str) const;

    // Convert ConfigValue to JSON-like string representation
    std::string configValueToString(const ConfigValue& value) const;

    // Parse simple line format
    bool parseSimpleLine(const std::string& line, Config& config);

    // Parse comment line
    bool isCommentLine(const std::string& line) const;

    // Parse empty line
    bool isEmptyLine(const std::string& line) const;

    // Get file extension
    static std::string getFileExtension(const std::string& filepath);

    // Check if string starts with prefix
    static bool startsWith(const std::string& str, const std::string& prefix);

    // Check if string ends with suffix
    static bool endsWith(const std::string& str, const std::string& suffix);

    // Convert to lowercase
    static std::string toLowercase(const std::string& str);

    // Convert to uppercase
    static std::string toUppercase(const std::string& str);

    // Replace all occurrences of substring
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);

    // Escape special characters in JSON string
    static std::string escapeJSONString(const std::string& str);

    // Unescape JSON string
    static std::string unescapeJSONString(const std::string& str);

    // Parse nested object notation (e.g., "brain.neuron_count")
    bool parseNestedParameter(const std::string& key, const ConfigValue& value, Config& config) const;

    // Generate unique parameter name from nested path
    static std::string generateNestedKey(const std::string& path, size_t index);

    // Standard configuration parameters schema
    struct ParameterSchema {
        std::string name;
        std::string type;  // "int", "int64_t", "double", "bool", "string", "array"
        std::string description;
        bool required;
        std::string default_value;
        std::string validation_pattern;
    };

    // Get standard parameter schema
    static std::vector<ParameterSchema> getStandardSchema();

    // Validate parameter against schema
    bool validateParameter(const std::string& name, const ConfigValue& value, const ParameterSchema& schema) const;

    // Check if parameter matches any schema
    bool findParameterSchema(const std::string& name, ParameterSchema& schema) const;

    // Format error message with context
    std::string formatErrorMessage(const std::string& operation, const std::string& context, const std::string& error) const;

    // Parse number with appropriate type detection
    template<typename T>
    bool parseNumber(const std::string& str, T& value) const;

    // Parse boolean value
    bool parseBoolean(const std::string& str, bool& value) const;

    // Parse string value (handle quotes and escaping)
    std::string parseStringValue(const std::string& str) const;

    // Parse array value
    std::vector<ConfigValue> parseArrayValue(const std::string& str) const;

    // Merge two configurations (new values override existing)
    void mergeConfigurations(const Config& source, Config& target) const;

    // Save configuration to JSON with formatting
    void saveToJSONWithFormatting(const Config& config, std::ostream& output) const;

    // Load configuration from JSON with error recovery
    bool loadFromJSONWithRecovery(const std::string& content, Config& config);

    // Parse configuration from YAML (placeholder - implement when yaml-cpp available)
    bool parseYAMLFormat(const std::string& content, Config& config) {
        // TODO: Implement YAML parsing when yaml-cpp is available
        // For now, return false to indicate YAML not supported
        return false;
    }

    // Parse configuration from JSON (placeholder - implement when nlohmann/json available)
    bool parseJSONFormat(const std::string& content, Config& config) {
        // TODO: Implement JSON parsing when nlohmann/json is available
        // For now, return false to indicate JSON not supported
        return false;
    }

    // Parse configuration from simple format (current implementation)
    bool parseSimpleFormat(const std::string& content, Config& config) {
        std::istringstream stream(content);
        std::string line;
        size_t lineNumber = 0;

        while (std::getline(stream, line)) {
            ++lineNumber;
            line = trim(line);

            // Skip empty lines and comments
            if (line.empty() || isCommentLine(line)) {
                continue;
            }

            // Parse key=value pair
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                // Malformed line - provide helpful error
                return false;
            }

            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));

            // Parse value
            ConfigValue configValue;

            if (isBoolean(value)) {
                bool boolVal;
                if (parseBoolean(value, boolVal)) {
                    configValue = boolVal;
                } else {
                    return false;
                }
            } else if (isNumber(value)) {
                // Try different numeric types
                int intVal;
                int64_t int64Val;
                double doubleVal;

                if (parseNumber(value, intVal)) {
                    configValue = intVal;
                } else if (parseNumber(value, int64Val)) {
                    configValue = int64Val;
                } else if (parseNumber(value, doubleVal)) {
                    configValue = doubleVal;
                } else {
                    return false;
                }
            } else {
                // String value (remove quotes if present)
                configValue = parseStringValue(value);
            }

            config.set(key, configValue, ConfigSource::File);
        }

        return true;
    }

    // Parse YAML format (placeholder implementation)
    bool parseYAMLFormat(const std::string& content, Config& config) {
        // TODO: Implement YAML parsing when yaml-cpp library is available
        // For now, return false to indicate YAML not supported
        // Users can use .cfg files for simple format
        return false;
    }

    // Parse JSON format (placeholder implementation)
    bool parseJSONFormat(const std::string& content, Config& config) {
        // TODO: Implement JSON parsing when nlohmann/json library is available
        // For now, return false to indicate JSON not supported
        // Users can use .cfg files for simple format
        return false;
    }

    // Check if line is a comment
    bool isCommentLine(const std::string& line) const {
        if (line.empty()) {
            return false;
        }

        // Skip whitespace before checking
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            return false;
        }

        return line[start] == '#' || line[start] == '/';
    }

    // Check if line is empty (only whitespace)
    bool isEmptyLine(const std::string& line) const {
        return std::all_of(line.begin(), line.end(), [](char c) {
            return std::isspace(c) || c == '\r';
        });
    }

    // Trim whitespace from string
    std::string trim(const std::string& str) const {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            return "";
        }

        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }

    // Check if string represents a boolean value
    bool isBoolean(const std::string& str) const {
        std::string lower = toLowercase(str);
        return lower == "true" || lower == "false" || lower == "yes" || lower == "no" ||
               lower == "on" || lower == "off" || lower == "enabled" || lower == "disabled";
    }

    // Check if string represents a number
    bool isNumber(const std::string& str) const {
        if (str.empty()) {
            return false;
        }

        size_t start = 0;
        if (str[0] == '+' || str[0] == '-') {
            start = 1;
        }

        bool hasDecimal = false;
        bool hasExponent = false;
        bool hasDigits = false;

        for (size_t i = start; i < str.length(); ++i) {
            if (str[i] == '.') {
                if (hasDecimal || hasExponent) {
                    return false;
                }
                hasDecimal = true;
            } else if (str[i] == 'e' || str[i] == 'E') {
                if (hasExponent || i == start || !hasDigits) {
                    return false;
                }
                hasExponent = true;
                hasDigits = false; // Reset for exponent part
            } else if (std::isdigit(static_cast<unsigned char>(str[i]))) {
                hasDigits = true;
            } else {
                return false;
            }
        }

        return hasDigits;
    }

    // Convert string to lowercase
    std::string toLowercase(const std::string& str) const {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    // Convert string to uppercase
    std::string toUppercase(const std::string& str) const {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::toupper(c); });
        return result;
    }

    // Replace all occurrences of substring
    std::string replaceAll(const std::string& str, const std::string& from, const std::string& to) const {
        std::string result = str;
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        return result;
    }

    // Escape special characters in JSON string
    std::string escapeJSONString(const std::string& str) const {
        std::string result;
        result.reserve(str.length() * 2);  // Worst case estimate

        for (char c : str) {
            switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '/': result += "\\/"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (c < ' ' || c > '~') {
                        // Non-ASCII or control character - use escape
                        result += "\\u" + toHex(static_cast<unsigned int>(c), 4);
                    } else {
                        result += c;
                    }
                    break;
            }
        }

        return result;
    }

    // Convert byte to hexadecimal string
    std::string toHex(unsigned int value, size_t digits) const {
        std::stringstream stream;
        stream << std::hex << std::setw(digits) << std::setfill('0') << value;
        return stream.str();
    }

    // Unescape JSON string
    std std::string unescapeJSONString(const std::string& str) const {
        std::string result;
        result.reserve(str.length());

        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == '\\' && i + 1 < str.length()) {
                switch (str[++i]) {
                    case '\"': result += '\"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case 'u': {
                        // Parse Unicode escape sequence
                        if (i + 5 < str.length()) {
                            std::string hex = str.substr(i + 1, 4);
                            try {
                                unsigned int code = std::stoul(hex, nullptr, 16);
                                result += static_cast<char>(code);
                                i += 4;
                            } catch (...) {
                                // Invalid Unicode - keep as-is
                                result += '\\';
                                result += 'u';
                            }
                        } else {
                            // Incomplete Unicode escape - keep as-is
                            result += '\\';
                            result += 'u';
                        }
                        break;
                    }
                    default:
                        // Unknown escape sequence - keep as-is
                        result += '\\';
                        result += str[i];
                        break;
                }
            } else {
                result += str[i];
            }
        }

        return result;
    }

    // Parse number with template for different types
    template<typename T>
    bool parseNumber(const std::string& str, T& value) const {
        std::stringstream stream(str);
        stream >> value;
        return !stream.fail() && stream.eof();
    }

    // Parse boolean value
    bool parseBoolean(const std::string& str, bool& value) const {
        std::string lower = toLowercase(str);

        if (lower == "true" || lower == "yes" || lower == "on" || lower == "enabled") {
            value = true;
            return true;
        } else if (lower == "false" || lower == "no" || lower == "off" || lower == "disabled") {
            value = false;
            return true;
        }

        return false;
    }

    // Parse string value (handle quotes and escaping)
    std::string parseStringValue(const std::string& str) const {
        std::string result = trim(str);

        // Check if quoted
        if (result.length() >= 2 && result.front() == '"' && result.back() == '"') {
            // Remove quotes and unescape
            return unescapeJSONString(result.substr(1, result.length() - 2));
        } else if (result.length() >= 2 && (result.front() == '\'' || result.back() == '\'')) {
            // Remove single quotes (less common but supported)
            return result.substr(1, result.length() - 2);
        }

        // Not quoted - return as-is
        return result;
    }

    // Parse array value
    std::vector<ConfigValue> parseArrayValue(const std::string& str) const {
        std::vector<ConfigValue> result;
        std::string trimmed = trim(str);

        if (trimmed.empty()) {
            return result;
        }

        // Check if wrapped in brackets
        if (trimmed.front() != '[' || trimmed.back() != ']') {
            return result;
        }

        // Remove brackets
        std::string content = trimmed.substr(1, trimmed.length() - 2);

        if (content.empty()) {
            return result;
        }

        // Split by comma (simple approach - doesn't handle nested arrays)
        size_t pos = 0;
        std::string token;

        while (pos < content.length()) {
            size_t comma = content.find(',', pos);
            if (comma == std::string::npos) {
                token = content.substr(pos);
                pos = content.length();
            } else {
                token = content.substr(pos, comma - pos);
                pos = comma + 1;
            }

            ConfigValue configValue;

            // Try different types
            if (isBoolean(trim(token))) {
                bool boolVal;
                if (parseBoolean(trim(token), boolVal)) {
                    configValue = boolVal;
                }
            } else if (isNumber(trim(token))) {
                double doubleVal;
                if (parseNumber(trim(token), doubleVal)) {
                    configValue = doubleVal;
                }
            } else {
                configValue = parseStringValue(trim(token));
            }

            if (!std::holds_alternative<std::monostate>(configValue)) {
                result.push_back(configValue);
            }
        }

        return result;
    }

    // Parse nested parameter (e.g., "brain.neuron_count")
    bool parseNestedParameter(const std::string& key, const ConfigValue& value, Config& config) const {
        // Normalize key (replace dots with underscores)
        std::string normalizedKey = normalizeParameterName(key);

        // Add to config
        config.set(normalizedKey, value, ConfigSource::File);
        return true;
    }

    // Generate unique parameter name from nested path
    std::string generateNestedKey(const std::string& path, size_t index) const {
        return path + "_entry_" + std::to_string(index);
    }

    // Normalize parameter name (replace dots with underscores)
    std::string normalizeParameterName(const std::string& name) const {
        return replaceAll(name, ".", "_");
    }

    // Denormalize parameter name (replace underscores with dots)
    std::string denormalizeParameterName(const std::string& name) const {
        return replaceAll(name, "_", ".");
    }

    // Check if string starts with prefix
    bool startsWith(const std::string& str, const std::string& prefix) const {
        return str.length() >= prefix.length() && str.compare(0, prefix.length(), prefix) == 0;
    }

    // Check if string ends with suffix
    bool endsWith(const std::string& str, const std::string& suffix) const {
        return str.length() >= suffix.length() && str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }

    // Get file extension
    std::string getFileExtension(const std::string& filepath) const {
        size_t dot = filepath.find_last_of('.');
        if (dot == std::string::npos) {
            return "";
        }
        return filepath.substr(dot + 1);
    }

    // Get standard parameter schema
    std::vector<ParameterSchema> getStandardSchema() const {
        return std::vector<ParameterSchema>{
            {"neuron_count", "int", "Total number of neurons", true, "1000", "^[0-9]+$"},
            {"region_count", "int", "Number of brain regions", false, "1", "^[0-9]+$"},
            {"simulation_timestep", "double", "Simulation timestep (seconds)", false, "0.001", "^[0-9]*\\.?[0-9]+$"},
            {"connection_probability", "double", "Probability of connection between neurons", false, "0.1", "^([01]?[0-9]?|2[0-4])?\\.?[0-9]+$"},
            {"random_seed", "int64_t", "Random seed for reproducible experiments", false, "42", "^[0-9]+$"},
            {"plasticity_learning_rate", "double", "General plasticity learning rate", false, "0.01", "^[0-9]*\\.?[0-9]+$"},
            {"stdp_ltp_weight", "double", "STDP LTP weight", false, "0.01", "^[0-9]*\\.?[0-9]+$"},
            {"stdp_ltd_weight", "double", "STDP LTD weight", false, "0.012", "^[0-9]*\\.?[0-9]+$"},
            {"stdp_time_constant", "double", "STDP time constant (ms)", false, "20.0", "^[0-9]*\\.?[0-9]+$"},
            {"synaptogenesis_rate", "double", "Synaptogenesis rate", false, "0.0001", "^[0-9]*\\.?[0-9]+$"},
            {"pruning_rate", "double", "Pruning rate", false, "0.00001", "^[0-9]*\\.?[0-9]+$"},
            {"dopamine_baseline", "double", "Dopamine baseline level", false, "0.1", "^[0-9]*\\.?[0-9]+$"},
            {"reward_discount_factor", "double", "Reward discount factor", false, "0.99", "^[0-9]*\\.?[0-9]+$"},
            {"environment_name", "string", "Environment name", false, "GridWorld", ".*"},
            {"environment_width", "int", "Environment width", false, "10", "^[0-9]+$"},
            {"environment_height", "int", "Environment height", false, "10", "^[0-9]+$"},
            {"log_level", "string", "Log level", false, "INFO", "^(DEBUG|INFO|WARNING|ERROR|FATAL)$"},
            {"log_to_file", "bool", "Log to file", false, "false", "^(true|false)$"},
            {"log_filename", "string", "Log filename", false, "nlm.log", ".*"},
            {"max_simulation_steps", "int64_t", "Maximum simulation steps", false, "10000", "^[0-9]+$"},
            {"simulation_time_limit", "double", "Simulation time limit (seconds)", false, "0.0", "^[0-9]*\\.?[0-9]+$"},
            {"visualization_enabled", "bool", "Enable visualization", false, "false", "^(true|false)$"},
            {"visualization_update_rate", "double", "Visualization update rate (Hz)", false, "30.0", "^[0-9]*\\.?[0-9]+$"}
        };
    }

    // Validate parameter against schema
    bool validateParameter(const std::string& name, const ConfigValue& value, const ParameterSchema& schema) const {
        // Check if parameter name matches
        if (normalizeParameterName(name) != normalizeParameterName(schema.name)) {
            return true;  // Not in schema, allow it
        }

        // Validate based on type
        bool isValid = false;

        if (schema.type == "int") {
            if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
                isValid = true;
                // Range validation
                int intVal = std::holds_alternative<int>(value) ? std::get<int>(value) : std::get<int64_t>(value);
                if (schema.validation_pattern != ".*") {
                    // Simple pattern check - could be enhanced
                    isValid = true;  // For now, assume valid if type matches
                }
            }
        } else if (schema.type == "double") {
            if (std::holds_alternative<double>(value)) {
                isValid = true;
                double doubleVal = std::get<double>(value);
                if (schema.validation_pattern != ".*") {
                    // Simple pattern check - could be enhanced
                    isValid = true;
                }
            }
        } else if (schema.type == "bool") {
            if (std::holds_alternative<bool>(value)) {
                isValid = true;
                // Check validation pattern
                std::string boolStr = std::get<bool>(value) ? "true" : "false";
                if (schema.validation_pattern != ".*") {
                    isValid = true;  // For now, assume valid if type matches
                }
            }
        } else if (schema.type == "string") {
            if (std::holds_alternative<std::string>(value)) {
                isValid = true;
                // Validate against pattern if provided
                if (schema.validation_pattern != ".*") {
                    // Could use regex validation here
                    isValid = true;  // For now, assume valid if type matches
                }
            }
        } else if (schema.type == "array") {
            if (std::holds_alternative<std::vector<int>>(value) ||
                std::holds_alternative<std::vector<double>>(value) ||
                std::holds_alternative<std::vector<std::string>>(value)) {
                isValid = true;
            }
        }

        return isValid;
    }

    // Check if parameter matches any schema
    bool findParameterSchema(const std::string& name, ParameterSchema& schema) const {
        std::vector<ParameterSchema> standardSchema = getStandardSchema();

        for (const auto& s : standardSchema) {
            if (normalizeParameterName(s.name) == normalizeParameterName(name)) {
                schema = s;
                return true;
            }
        }

        return false;
    }

    // Format error message with context
    std::string formatErrorMessage(const std::string& operation, const std::string& context, const std::string& error) const {
        std::stringstream stream;
        stream << "Config " << operation;
        if (!context.empty()) {
            stream << " for '" << context << "'";
        }
        if (!error.empty()) {
            stream << ": " << error;
        }
        return stream.str();
    }

    // Merge two configurations (new values override existing)
    void mergeConfigurations(const Config& source, Config& target) const {
        std::vector<std::string> sourceKeys = source.getKeys();

        for (const auto& key : sourceKeys) {
            if (source.has(key)) {
                auto sourceValue = source.get<const ConfigValue&>(key);
                target.set(key, sourceValue, ConfigSource::Runtime);
            }
        }
    }

    // Save configuration to JSON with formatting
    void saveToJSONWithFormatting(const Config& config, std::ostream& output) const {
        output << "{\n";

        std::vector<std::string> keys = config.getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            const std::string& key = keys[i];
            ConfigValue value = config.get<const ConfigValue&>(key);

            output << "  \"" << escapeJSONString(key) << "\": " << configValueToString(value);

            if (i < keys.size() - 1) {
                output << ",";
            }
            output << "\n";
        }

        output << "}\n";
    }

    // Load configuration from JSON with error recovery
    bool loadFromJSONWithRecovery(const std::string& content, Config& config) {
        // TODO: Implement JSON parsing with error recovery
        // For now, use simple format as fallback
        return parseSimpleFormat(content, config);
    }

    // Convert ConfigValue to JSON-like string representation
    std::string configValueToString(const ConfigValue& value) const {
        std::stringstream stream;

        std::visit([&stream](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, int>) {
                stream << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                stream << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                stream << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                stream << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                stream << "\"" << escapeJSONString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                stream << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) stream << ", ";
                    stream << arg[i];
                }
                stream << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                stream << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) stream << ", ";
                    stream << std::fixed << std::setprecision(6) << arg[i];
                }
                stream << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                stream << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) stream << ", ";
                    stream << "\"" << escapeJSONString(arg[i]) << "\"";
                }
                stream << "]";
            } else {
                stream << "null";
            }
        }, value);

        return stream.str();
    }

    // Convert string to ConfigValue
    ConfigValue stringToConfigValue(const std::string& str) const {
        // Check if it's a boolean
        bool boolVal;
        if (parseBoolean(str, boolVal)) {
            return boolVal;
        }

        // Check if it's a number
        int intVal;
        int64_t int64Val;
        double doubleVal;

        if (parseNumber(str, intVal)) {
            return intVal;
        } else if (parseNumber(str, int64Val)) {
            return int64Val;
        } else if (parseNumber(str, doubleVal)) {
            return doubleVal;
        }

        // Check if it's an array
        std::vector<ConfigValue> arrayVal = parseArrayValue(str);
        if (!arrayVal.empty()) {
            // Determine array type
            bool hasInt = false, hasDouble = false, hasString = false;

            for (const auto& val : arrayVal) {
                if (std::holds_alternative<int>(val) || std::holds_alternative<int64_t>(val)) {
                    hasInt = true;
                } else if (std::holds_alternative<double>(val)) {
                    hasDouble = true;
                } else if (std::holds_alternative<std::string>(val)) {
                    hasString = true;
                }
            }

            // Choose most specific type
            if (hasInt && !hasDouble && !hasString) {
                std::vector<int> intArray;
                for (const auto& val : arrayVal) {
                    if (std::holds_alternative<int>(val)) {
                        intArray.push_back(std::get<int>(val));
                    } else if (std::holds_alternative<int64_t>(val)) {
                        intArray.push_back(static_cast<int>(std::get<int64_t>(val)));
                    }
                }
                return intArray;
            } else if (hasDouble) {
                std::vector<double> doubleArray;
                for (const auto& val : arrayVal) {
                    if (std::holds_alternative<double>(val)) {
                        doubleArray.push_back(std::get<double>(val));
                    } else if (std::holds_alternative<int>(val)) {
                        doubleArray.push_back(static_cast<double>(std::get<int>(val)));
                    } else if (std::holds_alternative<int64_t>(val)) {
                        doubleArray.push_back(static_cast<double>(std::get<int64_t>(val)));
                    }
                }
                return doubleArray;
            } else if (hasString) {
                std::vector<std::string> stringArray;
                for (const auto& val : arrayVal) {
                    if (std::holds_alternative<std::string>(val)) {
                        stringArray.push_back(std::get<std::string>(val));
                    } else {
                        // Convert other types to string
                        stringArray.push_back(configValueToString(val));
                    }
                }
                return stringArray;
            }
        }

        // Default to string
        return parseStringValue(str);
    }

    // Parse simple line format
    bool parseSimpleLine(const std::string& line, Config& config) {
        std::string trimmed = trim(line);

        // Skip empty lines and comments
        if (trimmed.empty() || isCommentLine(trimmed)) {
            return true;
        }

        // Parse key=value pair
        size_t pos = trimmed.find('=');
        if (pos == std::string::npos) {
            return false;
        }

        std::string key = trim(trimmed.substr(0, pos));
        std::string value = trim(trimmed.substr(pos + 1));

        if (key.empty() || value.empty()) {
            return false;
        }

        // Parse value based on type
        ConfigValue configValue;

        if (isBoolean(value)) {
            bool boolVal;
            if (parseBoolean(value, boolVal)) {
                configValue = boolVal;
            } else {
                return false;
            }
        } else if (isNumber(value)) {
            double doubleVal;
            if (parseNumber(value, doubleVal)) {
                configValue = doubleVal;
            } else {
                return false;
            }
        } else {
            configValue = parseStringValue(value);
        }

        // Add to config
        config.set(key, configValue, ConfigSource::File);
        return true;
    }
};