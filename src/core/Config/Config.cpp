#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <optional>
#include <vector>
#include <variant>
#include <set>
#include <numeric>
#include <type_traits>
#include <source_location>
#include <sstream>

// JSON parser (nlohmann::json)
#include <nlohmann/json.hpp>

namespace nlm {

// Type information for all supported config types
struct TypeInfo {
    template<typename T>
    static std::string name() {
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "array<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "array<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "array<string>";
        else return "unknown";
    }
    
    template<typename T>
    static bool isValidType() {
        return std::is_same_v<T, int> || std::is_same_v<T, int64_t> ||
               std::is_same_v<T, double> || std::is_same_v<T, bool> ||
               std::is_same_v<T, std::string> ||
               std::is_same_v<T, std::vector<int>> ||
               std::is_same_v<T, std::vector<double>> ||
               std::is_same_v<T, std::vector<std::string>>;
    }
};

// Exception types for type safety
class ConfigTypeError : public std::runtime_error {
public:
    explicit ConfigTypeError(const std::string& message, 
                            const std::string& key = "",
                            const std::string& expected = "",
                            const std::string& actual = "")
        : std::runtime_error(message) {
        key_ = key;
        expected_ = expected;
        actual_ = actual;
    }
    
    const std::string& getKey() const { return key_; }
    const std::string& getExpectedType() const { return expected_; }
    const std::string& getActualType() const { return actual_; }
    
    std::string whatDetailed() const {
        std::stringstream ss;
        ss << std::runtime_error::what();
        if (!key_.empty()) ss << " (key: '" << key_ << "')";
        if (!expected_.empty()) ss << ", expected: '" << expected_ << "'";
        if (!actual_.empty()) ss << ", actual: '" << actual_ << "'";
        return ss.str();
    }
private:
    std::string key_;
    std::string expected_;
    std::string actual_;
};

class ConfigCastError : public std::runtime_error {
public:
    explicit ConfigCastError(const std::string& message,
                            const std::string& key = "",
                            const std::string& fromType = "",
                            const std::string& toType = "")
        : std::runtime_error(message) {
        key_ = key;
        fromType_ = fromType;
        toType_ = toType;
    }
    
    const std::string& getKey() const { return key_; }
    const std::string& getFromType() const { return fromType_; }
    const std::string& getToType() const { return toType_; }
    
    std::string whatDetailed() const {
        std::stringstream ss;
        ss << std::runtime_error::what();
        if (!key_.empty()) ss << " (key: '" << key_ << "')";
        if (!fromType_.empty()) ss << ", from: '" << fromType_ << "'";
        if (!toType_.empty()) ss << ", to: '" << toType_ << "'";
        return ss.str();
    }
private:
    std::string key_;
    std::string fromType_;
    std::string toType_;
};

// Type checking utilities
namespace TypeUtils {
    // Get the actual type name from ConfigValue
    template<typename T>
    std::string getActualTypeName(const ConfigValue& value) {
        if (std::holds_alternative<T>(value)) {
            return TypeInfo::name<T>();
        }
        return "unknown";
    }
    
    // Check if a requested type matches the actual stored type
    template<typename T>
    bool isTypeMatch(const ConfigValue& value, const std::string& key = "") {
        if (!std::holds_alternative<T>(value)) {
            return false;
        }
        return true;
    }
    
    // Get actual type name from ConfigValue using visitor
    std::string getActualTypeName(const ConfigValue& value) {
        std::string result;
        std::visit([&result](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            result = TypeInfo::name<T>();
        }, value);
        return result;
    }
    
    // Check type compatibility for variant access
    bool isTypeCompatible(const std::string& expected, const std::string& actual) {
        // Map common aliases
        auto normalize = [](const std::string& type) {
            std::string result = type;
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            return result;
        };
        
        std::string expectedNorm = normalize(expected);
        std::string actualNorm = normalize(actual);
        
        // Direct match
        if (expectedNorm == actualNorm) return true;
        
        // Numeric type compatibility (can be stored as string)
        if (expectedNorm == "string" && (actualNorm == "int" || actualNorm == "int64_t" || 
                                         actualNorm == "double" || actualNorm == "bool"))
            return true;
        
        // Array type compatibility - check element types
        if (expectedNorm.find("array<") == 0 && actualNorm.find("array<") == 0) {
            // For now, require exact match of element types
            return expectedNorm == actualNorm;
        }
        
        return false;
    }
}

// Safe type casting utilities
namespace SafeCast {
    // Cast between numeric types
    template<typename From, typename To>
    To numericCast(const From& value, const std::string& key = "") {
        static_assert(std::is_arithmetic_v<From> && std::is_arithmetic_v<To>, "Types must be numeric");
        
        To result = static_cast<To>(value);
        
        // Check for overflow/underflow for integer types
        if constexpr (std::is_integral_v<From> && std::is_integral_v<To>) {
            if (value > static_cast<From>(std::numeric_limits<To>::max()) ||
                value < static_cast<From>(std::numeric_limits<To>::min())) {
                throw ConfigCastError("Numeric cast overflow/underflow", key,
                                    TypeInfo::name<From>(), TypeInfo::name<To>());
            }
        } else if constexpr (std::is_floating_point_v<From> && std::is_integral_v<To>) {
            if (value > static_cast<From>(std::numeric_limits<To>::max()) ||
                value < static_cast<From>(std::numeric_limits<To>::min())) {
                throw ConfigCastError("Floating point cast overflow/underflow", key,
                                    TypeInfo::name<From>(), TypeInfo::name<To>());
            }
        } else if constexpr (std::is_integral_v<From> && std::is_floating_point_v<To>) {
            // No overflow for float from int
        }
        
        return result;
    }
    
    // Safe string conversion
    template<typename T>
    std::string toString(const T& value) {
        if constexpr (std::is_same_v<T, std::string>) {
            return value;
        } else if constexpr (std::is_same_v<T, bool>) {
            return value ? "true" : "false";
        } else {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }
    }
}

// Configuration validation with enhanced type checking
namespace ConfigValidation {
    // Comprehensive validation with detailed error reporting
    std::vector<std::string> validateConfigWithTypeChecking(const std::vector<ConfigEntry>& entries) {
        std::vector<std::string> errors;
        
        for (const auto& entry : entries) {
            // Check if entry has a type hint
            if (!entry.typeHint.empty()) {
                // Determine expected type from type hint
                std::string expectedType = entry.typeHint;
                std::string actualType = TypeUtils::getActualTypeName(entry.value);
                
                // Validate type match
                if (!TypeUtils::isTypeCompatible(expectedType, actualType)) {
                    errors.push_back(std::string("Type mismatch for key '" + entry.key + 
                                               "': expected '" + expectedType + 
                                               "', found '" + actualType + "'."));
                }
                
                // If type hint is generic (like "int" but we have int64_t), convert if possible
                if (expectedType == "int" && actualType == "int64_t") {
                    int64_t val = std::get<int64_t>(entry.value);
                    if (val > std::numeric_limits<int>::max() || val < std::numeric_limits<int>::min()) {
                        errors.push_back(std::string("Value for key '" + entry.key + 
                                                   "' (int64_t: " + std::to_string(val) + 
                                                   ") cannot be safely cast to int."));
                    }
                }
            }
            
            // Check for null/empty values where required
            if (entry.value.index() == 0) { // std::monostate could be empty, but ConfigValue doesn't use this
                // Check if value is actually empty (was parsed as empty string)
                if (std::holds_alternative<std::string>(entry.value) && 
                    std::get<std::string>(entry.value).empty() &&
                    entry.typeHint != "string") {
                    errors.push_back(std::string("Empty value provided for key '" + entry.key + 
                                               "' which expects a non-empty value."));
                }
            }
        }
        
        return errors;
    }
}

// ConfigParser implementation

// Parse string to appropriate type
std::optional<ConfigValue> ConfigParser::parseValue(const std::string& str, const std::string& typeHint) {
    if (str.empty()) {
        return std::nullopt;
    }
    
    // Try bool parsing
    if (typeHint == "bool" || typeHint.empty()) {
        if (str == "true" || str == "1" || str == "yes" || str == "on") {
            return true;
        }
        if (str == "false" || str == "0" || str == "no" || str == "off") {
            return false;
        }
    }
    
    // Try int64_t parsing
    if (typeHint == "int64_t" || typeHint == "int" || typeHint.empty()) {
        try {
            size_t pos;
            int64_t val = std::stoll(str, &pos);
            if (pos == str.size()) {
                return val;
            }
        } catch (const std::exception&) {}
    }
    
    // Try double parsing
    if (typeHint == "double" || typeHint.empty()) {
        try {
            size_t pos;
            double val = std::stod(str, &pos);
            if (pos == str.size()) {
                return val;
            }
        } catch (const std::exception&) {}
    }
    
    // Try string (default)
    if (typeHint == "string" || typeHint.empty()) {
        return str;
    }
    
    return std::nullopt;
}

// Parse key=value format
bool ConfigParser::parseKeyValueLine(const std::string& line, std::string& key, std::string& value) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return false;
    }
    
    key = trim(line.substr(0, pos));
    value = trim(line.substr(pos + 1));
    
    // Remove quotes if present
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    return !key.empty();
}

// Parse JSON from string
nlohmann::json ConfigParser::parseJSON(const std::string& jsonStr) {
    try {
        return nlohmann::json::parse(jsonStr);
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("JSON parse error: " + std::string(e.what()));
    }
}

// Parse YAML from string (simple implementation)
bool ConfigParser::parseYAML(const std::string& yamlStr, std::map<std::string, std::string>& result) {
    std::istringstream stream(yamlStr);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Simple key: value format
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos && colonPos > 0) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Remove quotes from value
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            result[key] = value;
        }
    }
    
    return !result.empty();
}

// Merge multiple config sources
std::vector<ConfigEntry> ConfigParser::mergeConfigs(const std::vector<std::vector<ConfigEntry>>& configSources) {
    std::map<std::string, ConfigEntry> merged;
    
    // Process sources in reverse order (later sources override earlier ones)
    for (const auto& source : configSources) {
        for (const auto& entry : source) {
            ConfigSource finalSource = entry.source;
            
            if (merged.find(entry.key) != merged.end()) {
                finalSource = std::max(finalSource, merged[entry.key].source);
            }
            
            merged[entry.key] = ConfigEntry(
                entry.key,
                entry.value,
                finalSource,
                entry.description,
                entry.typeHint,
                entry.fileSource,
                entry.lineNumber
            );
        }
    }
    
    std::vector<ConfigEntry> result;
    result.reserve(merged.size());
    for (auto& pair : merged) {
        result.push_back(std::move(pair.second));
    }
    
    return result;
}

// Validate config against schema
    static std::vector<std::string> validateConfig(const std::vector<ConfigEntry>& entries) {
        std::vector<std::string> errors;
        
        // Use enhanced validation with type checking
        errors = ConfigValidation::validateConfigWithTypeChecking(entries);
        
        // Keep original schema-based validation for backward compatibility
        std::vector<std::string> schemaErrors;
        std::vector<ConfigSchema> knownSchemas;
        for (const auto& schema : ConfigSchema::KNOWN_CONFIG_SCHEMA) {
            knownSchemas.push_back(schema);
        }
        
        for (const auto& entry : entries) {
            auto schemaIt = std::find_if(
                knownSchemas.begin(),
                knownSchemas.end(),
                [&entry](const ConfigSchema& schema) { return schema.key == entry.key; }
            );
            
            if (schemaIt != knownSchemas.end()) {
                const auto& schema = *schemaIt;
                
                if (!entry.typeHint.empty()) {
                    bool typeMatch = false;
                    if (schema.type == "int" && std::holds_alternative<int>(entry.value)) typeMatch = true;
                    else if (schema.type == "int64_t" && std::holds_alternative<int64_t>(entry.value)) typeMatch = true;
                    else if (schema.type == "double" && std::holds_alternative<double>(entry.value)) typeMatch = true;
                    else if (schema.type == "bool" && std::holds_alternative<bool>(entry.value)) typeMatch = true;
                    else if (schema.type == "string" && std::holds_alternative<std::string>(entry.value)) typeMatch = true;
                    else if (schema.type == "array" && (std::holds_alternative<std::vector<int>>(entry.value) ||
                                                        std::holds_alternative<std::vector<double>>(entry.value) ||
                                                        std::holds_alternative<std::vector<std::string>>(entry.value))) typeMatch = true;
                    
                    if (!typeMatch) {
                        std::string error = "Key \"" + entry.key + "\" has wrong type. Expected " + schema.type + ", got " + entry.typeHint;
                        if (std::find(errors.begin(), errors.end(), error) == errors.end()) {
                            errors.push_back(error);
                        }
                    }
                }
                
                if (!schema.allowedValues.empty()) {
                    std::string strValue;
                    if (std::holds_alternative<std::string>(entry.value)) {
                        strValue = std::get<std::string>(entry.value);
                    } else {
                        strValue = configValueToString(entry.value);
                    }
                    
                    bool valueAllowed = std::find(schema.allowedValues.begin(), schema.allowedValues.end(), strValue) != schema.allowedValues.end();
                    if (!valueAllowed) {
                        std::string error = "Key \"" + entry.key + "\" has invalid value \"" + strValue + "\". Allowed values: " + 
                                           (schema.allowedValues.size() > 3 ? "multiple values" : "");
                        if (std::find(errors.begin(), errors.end(), error) == errors.end()) {
                            errors.push_back(error);
                        }
                    }
                }
                
                for (const auto& constraint : schema.constraints) {
                    if (constraint.first == "min") {
                        bool belowMin = false;
                        if (schema.type == "int" && std::holds_alternative<int>(entry.value)) {
                            belowMin = std::get<int>(entry.value) < std::stoll(constraint.second);
                        } else if (schema.type == "int64_t" && std::holds_alternative<int64_t>(entry.value)) {
                            belowMin = std::get<int64_t>(entry.value) < std::stoll(constraint.second);
                        } else if (schema.type == "double" && std::holds_alternative<double>(entry.value)) {
                            belowMin = std::get<double>(entry.value) < std::stod(constraint.second);
                        }
                        if (belowMin) {
                            std::string error = "Key \"" + entry.key + "\" value below minimum " + constraint.second;
                            if (std::find(errors.begin(), errors.end(), error) == errors.end()) {
                                errors.push_back(error);
                            }
                        }
                    } else if (constraint.first == "max") {
                        bool aboveMax = false;
                        if (schema.type == "int" && std::holds_alternative<int>(entry.value)) {
                            aboveMax = std::get<int>(entry.value) > std::stoll(constraint.second);
                        } else if (schema.type == "int64_t" && std::holds_alternative<int64_t>(entry.value)) {
                            aboveMax = std::get<int64_t>(entry.value) > std::stoll(constraint.second);
                        } else if (schema.type == "double" && std::holds_alternative<double>(entry.value)) {
                            aboveMax = std::get<double>(entry.value) > std::stod(constraint.second);
                        }
                        if (aboveMax) {
                            std::string error = "Key \"" + entry.key + "\" value above maximum " + constraint.second;
                            if (std::find(errors.begin(), errors.end(), error) == errors.end()) {
                                errors.push_back(error);
                            }
                        }
                    }
                }
            }
        }
        
        return errors;
    }

// Check if file is JSON
bool ConfigParser::isJSONFile(const std::string& filepath) {
    if (filepath.size() < 5) return false;
    
    std::string ext = getFileExtension(filepath);
    return ext == "json" || ext == "jsonc";
}

// Check if file is YAML
bool ConfigParser::isYAMLFile(const std::string& filepath) {
    if (filepath.size() < 5) return false;
    
    std::string ext = getFileExtension(filepath);
    return ext == "yaml" || ext == "yml";
}

// Get file extension
std::string ConfigParser::getFileExtension(const std::string& filepath) {
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos == std::string::npos) return "";
    
    std::string ext = filepath.substr(dotPos + 1);
    for (char& c : ext) {
        c = std::tolower(c);
    }
    return ext;
}

// Convert value to string for JSON/YAML output
std::string ConfigParser::configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
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
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        }
    }, value);
    
    return oss.str();
}

// ConfigSchema implementation
namespace ConfigSchema {
    const std::vector<ConfigSchema> KNOWN_CONFIG_SCHEMA = {
        {"random_seed", "int64_t", "Random seed for reproducible experiments", "42", {}, {"min": "0"}},
        {"simulation_timestep", "double", "Simulation timestep in seconds", "0.001", {}, {"min": "0.0"}},
        {"neuron_count", "int64_t", "Number of neurons in the brain", "1000", {}, {"min": "0"}},
        {"region_count", "int64_t", "Number of brain regions", "2", {}, {"min": "1"}},
        {"connection_probability", "double", "Connection probability for random connectivity", "0.1", {}, {"min": "0.0", "max": "1.0"}},
        {"plasticity_learning_rate", "double", "Plasticity learning rate", "0.01", {}, {"min": "0.0"}},
        {"stdp_ltp_weight", "double", "STDP LTP weight", "0.01", {}, {"min": "0.0"}},
        {"stdp_ltd_weight", "double", "STDP LTD weight", "0.012", {}, {"min": "0.0"}},
        {"stdp_time_constant", "double", "STDP time constant", "20.0", {}, {"min": "0.0"}},
        {"development_synaptogenesis_rate", "double", "Development synaptogenesis rate", "0.001", {}, {"min": "0.0"}},
        {"development_pruning_rate", "double", "Development pruning rate", "0.0001", {}, {"min": "0.0"}},
        {"dopamine_baseline", "double", "Dopamine baseline", "0.0", {}, {"min": "0.0"}},
        {"reward_discount_factor", "double", "Reward discount factor", "0.99", {}, {"min": "0.0", "max": "1.0"}},
        {"environment_name", "string", "Environment name", "GridWorld", {"GridWorld", "Cave", "Forest"}},
        {"environment_width", "int64_t", "Environment width", "10", {}, {"min": "1"}},
        {"environment_height", "int64_t", "Environment height", "10", {}, {"min": "1"}},
        {"log_level", "string", "Log level", "INFO", {"DEBUG", "INFO", "WARN", "ERROR"}},
        {"log_to_file", "bool", "Log to file", "false"},
        {"log_filename", "string", "Log filename", "nlm.log"},
        {"max_simulation_steps", "int64_t", "Maximum simulation steps", "10000", {}, {"min": "0"}},
        {"simulation_time_limit", "double", "Simulation time limit", "0.0", {}, {"min": "0.0"}},
        {"visualization_enabled", "bool", "Visualization enabled", "false"},
        {"visualization_update_rate", "double", "Visualization update rate", "30.0", {}, {"min": "1.0"}},
        {"synaptogenesis_rate", "double", "Synaptogenesis rate", "0.0001", {}, {"min": "0.0"}},
        {"pruning_rate", "double", "Pruning rate", "0.00001", {}, {"min": "0.0"}},
        {"stdp_tau", "double", "STDP time constant", "20.0", {}, {"min": "0.0"}}
    };
}

// Config::Impl structure
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigSource> keySources; // Track individual key sources
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::string ext = ConfigParser::getFileExtension(filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    file.close();
    
    bool success = false;
    
    if (ext == "json" || ext == "jsonc") {
        success = parseFromJSON(content, filepath);
    } else if (ext == "yaml" || ext == "yml") {
        success = parseFromYAML(content, filepath);
    } else {
        success = parseFromKeyValue(content, filepath);
    }
    
    return success;
}

bool Config::parseFromJSON(const std::string& content, const std::string& filepath) {
    try {
        auto json = ConfigParser::parseJSON(content);
        
        if (!json.is_object()) {
            throw std::runtime_error("JSON root must be an object");
        }
        
        bool success = false;
        for (const auto& [key, value] : json.items()) {
            std::string typeHint;
            std::string description;
            
            auto schemaIt = std::find_if(
                ConfigSchema::KNOWN_CONFIG_SCHEMA.begin(),
                ConfigSchema::KNOWN_CONFIG_SCHEMA.end(),
                [&key](const ConfigSchema& schema) { return schema.key == key; }
            );
            
            if (schemaIt != ConfigSchema::KNOWN_CONFIG_SCHEMA.end()) {
                typeHint = schemaIt->type;
                description = schemaIt->description;
            }
            
            ConfigValue configValue;
            if (jsonToConfigValue(value, configValue)) {
                ConfigEntry entry(
                    key,
                    configValue,
                    ConfigSource::File,
                    description,
                    typeHint,
                    filepath,
                    0
                );
                
                pImpl->entries.push_back(entry);
                pImpl->keySources[key] = ConfigSource::File;
                success = true;
            } else {
                ConfigEntry entry(
                    key,
                    ConfigValue(value.get<std::string>()),
                    ConfigSource::File,
                    description,
                    "string",
                    filepath,
                    0
                );
                pImpl->entries.push_back(entry);
                pImpl->keySources[key] = ConfigSource::File;
                success = true;
            }
        }
        
        return success;
    } catch (const std::exception& e) {
        return parseFromKeyValue(content, filepath);
    }
}

bool Config::parseFromYAML(const std::string& content, const std::string& filepath) {
    std::map<std::string, std::string> yamlMap;
    if (!ConfigParser::parseYAML(content, yamlMap)) {
        return parseFromKeyValue(content, filepath);
    }
    
    bool success = false;
    
    for (const auto& [key, value] : yamlMap) {
        std::string typeHint;
        std::string description;
        
        auto schemaIt = std::find_if(
            ConfigSchema::KNOWN_CONFIG_SCHEMA.begin(),
            ConfigSchema::KNOWN_CONFIG_SCHEMA.end(),
            [&key](const ConfigSchema& schema) { return schema.key == key; }
        );
        
        if (schemaIt != ConfigSchema::KNOWN_CONFIG_SCHEMA.end()) {
            typeHint = schemaIt->type;
            description = schemaIt->description;
        }
        
        auto parsedValue = ConfigParser::parseValue(value, typeHint);
        if (parsedValue) {
            ConfigEntry entry(
                key,
                *parsedValue,
                ConfigSource::File,
                description,
                typeHint,
                filepath,
                0
            );
            pImpl->entries.push_back(entry);
            pImpl->keySources[key] = ConfigSource::File;
            success = true;
        } else {
            ConfigEntry entry(
                key,
                ConfigValue(value),
                ConfigSource::File,
                description,
                "string",
                filepath,
                0
            );
            pImpl->entries.push_back(entry);
            pImpl->keySources[key] = ConfigSource::File;
            success = true;
        }
    }
    
    return success;
}

bool Config::parseFromKeyValue(const std::string& content, const std::string& filepath) {
    std::istringstream stream(content);
    std::string line;
    int lineNum = 0;
    bool success = false;
    
    while (std::getline(stream, line)) {
        lineNum++;
        
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == '/') {
            continue;
        }
        
        std::string key, value;
        if (ConfigParser::parseKeyValueLine(trimmed, key, value)) {
            std::string typeHint;
            std::string description;
            
            auto schemaIt = std::find_if(
                ConfigSchema::KNOWN_CONFIG_SCHEMA.begin(),
                ConfigSchema::KNOWN_CONFIG_SCHEMA.end(),
                [&key](const ConfigSchema& schema) { return schema.key == key; }
            );
            
            if (schemaIt != ConfigSchema::KNOWN_CONFIG_SCHEMA.end()) {
                typeHint = schemaIt->type;
                description = schemaIt->description;
            }
            
            auto parsedValue = ConfigParser::parseValue(value, typeHint);
            if (parsedValue) {
                ConfigEntry entry(
                    key,
                    *parsedValue,
                    ConfigSource::File,
                    description,
                    typeHint,
                    filepath,
                    lineNum
                );
                pImpl->entries.push_back(entry);
                pImpl->keySources[key] = ConfigSource::File;
                success = true;
            } else {
                ConfigEntry entry(
                    key,
                    ConfigValue(value),
                    ConfigSource::File,
                    description,
                    "string",
                    filepath,
                    lineNum
                );
                pImpl->entries.push_back(entry);
                pImpl->keySources[key] = ConfigSource::File;
                success = true;
            }
        }
    }
    
    return success;
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string ext = ConfigParser::getFileExtension(filepath);
    
    if (ext == "json" || ext == "jsonc") {
        nlohmann::json json;
        for (const auto& entry : pImpl->entries) {
            json[entry.key] = configValueToJSON(entry.value);
        }
        
        file << json.dump(4);
    } else if (ext == "yaml" || ext == "yml") {
        file << "# Configuration\n";
        file << "# Generated by NLM Config parser\n\n";
        for (const auto& entry : pImpl->entries) {
            file << entry.key << ": " << ConfigParser::configValueToString(entry.value) << "\n";
        }
    } else {
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << ConfigParser::configValueToString(entry.value) << "\n\n";
        }
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->keySources.find(key);
    if (it == pImpl->keySources.end()) {
        return std::nullopt;
    }
    
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entryIt == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    // Enhanced runtime type validation
    const auto& entry = *entryIt;
    
    // Check if the value matches the requested type using type safety utilities
    if (!TypeUtils::isTypeMatch<T>(entry.value, key)) {
        std::string actualType = TypeUtils::getActualTypeName(entry.value);
        std::string expectedType = TypeInfo::name<T>();
        
        // For better debugging, include source information
        std::string sourceInfo = "";
        if (!entry.fileSource.empty()) {
            sourceInfo = " (from " + entry.fileSource;
            if (entry.lineNumber > 0) sourceInfo += " line " + std::to_string(entry.lineNumber);
            sourceInfo += ")";
        }
        
        throw ConfigTypeError(
            "Type mismatch when accessing configuration key",
            key,
            expectedType,
            actualType
        ) << sourceInfo;
    }
    
    // Safe type access with detailed error reporting
    try {
        T value = std::get<T>(entry.value);
        
        // Additional validation for specific types
        if constexpr (std::is_same_v<T, int>) {
            if (entry.typeHint == "int64_t" || entry.typeHint == "double") {
                // Could be a type conversion scenario
                if (std::holds_alternative<int64_t>(entry.value)) {
                    int64_t val = std::get<int64_t>(entry.value);
                    if (val < std::numeric_limits<int>::min() || val > std::numeric_limits<int>::max()) {
                        throw ConfigTypeError(
                            "Value cannot be safely cast to int - out of range",
                            key,
                            "int",
                            "int64_t"
                        );
                    }
                } else if (std::holds_alternative<double>(entry.value)) {
                    double val = std::get<double>(entry.value);
                    if (val < std::numeric_limits<int>::min() || val > std::numeric_limits<int>::max() ||
                        std::floor(val) != val) {
                        throw ConfigTypeError(
                            "Value cannot be safely cast to int - out of range or not integer",
                            key,
                            "int",
                            "double"
                        );
                    }
                }
            }
        }
        
        // Validate constraints for numeric types
        if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
            auto schemaIt = std::find_if(
                ConfigSchema::KNOWN_CONFIG_SCHEMA.begin(),
                ConfigSchema::KNOWN_CONFIG_SCHEMA.end(),
                [&key](const ConfigSchema& schema) { return schema.key == key; }
            );
            
            if (schemaIt != ConfigSchema::KNOWN_CONFIG_SCHEMA.end()) {
                const auto& schema = *schemaIt;
                for (const auto& constraint : schema.constraints) {
                    if (constraint.first == "min") {
                        if (value < std::stod(constraint.second)) {
                            throw ConfigTypeError(
                                "Value below minimum constraint",
                                key,
                                constraint.second,
                                TypeInfo::name<T>()
                            );
                        }
                    } else if (constraint.first == "max") {
                        if (value > std::stod(constraint.second)) {
                            throw ConfigTypeError(
                                "Value above maximum constraint",
                                key,
                                constraint.second,
                                TypeInfo::name<T>()
                            );
                        }
                    }
                }
            }
        }
        
        return value;
    } catch (const std::bad_variant_access&) {
        // This should theoretically never happen due to the type check above,
        // but we keep it for safety
        throw ConfigTypeError(
            "Internal error: type mismatch in variant access",
            key,
            TypeInfo::name<T>(),
            TypeUtils::getActualTypeName(entry.value)
        );
    }
}

// Enhanced getOr method with type safety
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return *val;
    }
    
    // Log access attempt for debugging
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!validateSchema(ConfigEntry(key, value, source))) {
        throw std::runtime_error("Invalid configuration key: " + key);
    }
    
    auto it = pImpl->keySources.find(key);
    
    if (it != pImpl->keySources.end()) {
        auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (entryIt != pImpl->entries.end()) {
            entryIt->value = value;
            entryIt->source = source;
        }
    } else {
        pImpl->entries.emplace_back(key, value, source);
        pImpl->keySources[key] = source;
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    auto schemaIt = std::find_if(
        ConfigSchema::KNOWN_CONFIG_SCHEMA.begin(),
        ConfigSchema::KNOWN_CONFIG_SCHEMA.end(),
        [&key](const ConfigSchema& schema) { return schema.key == key; }
    );
    
    std::string typeHint = schemaIt != ConfigSchema::KNOWN_CONFIG_SCHEMA.end() ? schemaIt->type : "";
    
    auto parsedValue = ConfigParser::parseValue(value, typeHint);
    if (parsedValue) {
        set(key, *parsedValue, source);
    } else {
        set(key, ConfigValue(value), source);
    }
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

bool Config::has(const std::string& key) const {
    return pImpl->keySources.find(key) != pImpl->keySources.end();
}

void Config::remove(const std::string& key) {
    pImpl->keySources.erase(key);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->keySources.size());
    for (const auto& pair : pImpl->keySources) {
        keys.push_back(pair.first);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->keySources.clear();
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

std::vector<std::string> Config::validate() const {
    return ConfigParser::validateConfig(pImpl->entries);
}

std::vector<ConfigSource> Config::getSources(const std::string& key) const {
    std::vector<ConfigSource> sources;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            sources.push_back(entry.source);
        }
    }
    
    return sources;
}

// Private helper methods
bool Config::validateSchema(const ConfigEntry& entry) {
    auto schemaIt = std::find_if(
        ConfigSchema::KNOWN_CONFIG_SCHEMA.begin(),
        ConfigSchema::KNOWN_CONFIG_SCHEMA.end(),
        [&entry](const ConfigSchema& schema) { return schema.key == entry.key; }
    );
    
    if (schemaIt == ConfigSchema::KNOWN_CONFIG_SCHEMA.end()) {
        return true;
    }
    
    return validateType(entry) && validateConstraints(entry);
}

// Validate type information
    bool Config::validateType(const ConfigEntry& entry) {
        // Enhanced type validation using type safety utilities
        std::string expectedType = "unknown";
        
        // Try to find schema match
        auto schemaIt = std::find_if(
            ConfigSchema::KNOWN_CONFIG_SCHEMA.begin(),
            ConfigSchema::KNOWN_CONFIG_SCHEMA.end(),
            [&entry](const ConfigSchema& schema) { return schema.key == entry.key; }
        );
        
        if (schemaIt != ConfigSchema::KNOWN_CONFIG_SCHEMA.end()) {
            expectedType = schemaIt->type;
        }
        
        // If entry has type hint, use it as primary reference
        if (!entry.typeHint.empty()) {
            expectedType = entry.typeHint;
        }
        
        // Use enhanced type checking
        std::string actualType = TypeUtils::getActualTypeName(entry.value);
        
        // Check type compatibility
        if (expectedType != "unknown" && actualType != "unknown") {
            return TypeUtils::isTypeCompatible(expectedType, actualType);
        }
        
        return true; // If we can't determine types, assume valid
    }
    
    // Validate constraints
    bool Config::validateConstraints(const ConfigEntry& entry) {
        auto schemaIt = std::find_if(
            ConfigSchema::KNOWN_CONFIG_SCHEMA.begin(),
            ConfigSchema::KNOWN_CONFIG_SCHEMA.end(),
            [&entry](const ConfigSchema& schema) { return schema.key == entry.key; }
        );
        
        if (schemaIt == ConfigSchema::KNOWN_CONFIG_SCHEMA.end()) {
            return true;
        }
        
        const auto& schema = *schemaIt;
        
        for (const auto& constraint : schema.constraints) {
            if (constraint.first == "min") {
                bool belowMin = false;
                if (schema.type == "int" && std::holds_alternative<int>(entry.value)) {
                    belowMin = std::get<int>(entry.value) < std::stoll(constraint.second);
                } else if (schema.type == "int64_t" && std::holds_alternative<int64_t>(entry.value)) {
                    belowMin = std::get<int64_t>(entry.value) < std::stoll(constraint.second);
                } else if (schema.type == "double" && std::holds_alternative<double>(entry.value)) {
                    belowMin = std::get<double>(entry.value) < std::stod(constraint.second);
                }
                if (belowMin) return false;
            } else if (constraint.first == "max") {
                bool aboveMax = false;
                if (schema.type == "int" && std::holds_alternative<int>(entry.value)) {
                    aboveMax = std::get<int>(entry.value) > std::stoll(constraint.second);
                } else if (schema.type == "int64_t" && std::holds_alternative<int64_t>(entry.value)) {
                    aboveMax = std::get<int64_t>(entry.value) > std::stoll(constraint.second);
                } else if (schema.type == "double" && std::holds_alternative<double>(entry.value)) {
                    aboveMax = std::get<double>(entry.value) > std::stod(constraint.second);
                }
                if (aboveMax) return false;
            }
        }
        
        return true;
    }

// Type conversion helpers
nlohmann::json Config::configValueToJSON(const ConfigValue& value) {
    nlohmann::json json;
    
    std::visit([&json](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json = arg;
        }
    }, value);
    
    return json;
}

bool Config::jsonToConfigValue(const nlohmann::json& json, ConfigValue& value) {
    if (json.is_boolean()) {
        value = json.get<bool>();
        return true;
    } else if (json.is_number_integer()) {
        if (json.get<int64_t>() <= std::numeric_limits<int>::max() &&
            json.get<int64_t>() >= std::numeric_limits<int>::min()) {
            value = json.get<int>();
        } else {
            value = json.get<int64_t>();
        }
        return true;
    } else if (json.is_number_float()) {
        value = json.get<double>();
        return true;
    } else if (json.is_string()) {
        value = json.get<std::string>();
        return true;
    } else if (json.is_array()) {
        if (json.empty()) {
            value = std::vector<double>();
            return true;
        }
        
        bool allIntegers = true;
        bool allNumbers = true;
        std::vector<double> doubleArray;
        
        for (const auto& item : json) {
            if (item.is_number_integer()) {
                doubleArray.push_back(item.get<int64_t>());
            } else if (item.is_number_float()) {
                doubleArray.push_back(item.get<double>());
                allIntegers = false;
            } else {
                allNumbers = false;
                break;
            }
        }
        
        if (allNumbers && !doubleArray.empty()) {
            if (allIntegers) {
                std::vector<int> intArray;
                for (double d : doubleArray) {
                    intArray.push_back(static_cast<int>(d));
                }
                value = intArray;
            } else {
                value = doubleArray;
            }
            return true;
        } else if (!allNumbers) {
            std::vector<std::string> stringArray;
            for (const auto& item : json) {
                if (item.is_string()) {
                    stringArray.push_back(item.get<std::string>());
                } else {
                    return false;
                }
            }
            value = stringArray;
            return true;
        }
        
        return false;
    }
    
    return false;
}

// Explicit template instantiations
namespace detail {
    template<> const std::string TypeInfo<int>::name = "int";
    template<> const std::string TypeInfo<int64_t>::name = "int64_t";
    template<> const std::string TypeInfo<double>::name = "double";
    template<> const std::string TypeInfo<bool>::name = "bool";
    template<> const std::string TypeInfo<std::string>::name = "string";
    template<> const std::string TypeInfo<std::vector<int>>::name = "array<int>";
    template<> const std::string TypeInfo<std::vector<double>>::name = "array<double>";
    template<> const std::string TypeInfo<std::vector<std::string>>::name = "array<string>";
}

} // namespace nlm

// Explicit instantiations for Config class
template std::optional<int> nlm::Config::get<int>(const std::string&) const;
template std::optional<int64_t> nlm::Config::get<int64_t>(const std::string&) const;
template std::optional<double> nlm::Config::get<double>(const std::string&) const;
template std::optional<bool> nlm::Config::get<bool>(const std::string&) const;
template std::optional<std::string> nlm::Config::get<std::string>(const std::string&) const;

template int nlm::Config::getOr<int>(const std::string&, const int&) const;
template int64_t nlm::Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double nlm::Config::getOr<double>(const std::string&, const double&) const;
template bool nlm::Config::getOr<bool>(const std::string&, const bool&) const;
template std::string nlm::Config::getOr<std::string>(const std::string&, const std::string&) const;