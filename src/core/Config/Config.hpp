#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <any>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <unordered_map>
#include <limits>
#include <regex>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration error codes
enum class ConfigErrorCode {
    None = 0,
    FileNotFound,
    FileReadError,
    FileWriteError,
    ParseError,
    TypeError,
    ValueError,
    ValidationError,
    SchemaError,
    UnsupportedFormat,
    UnknownError
};

// Configuration exception
class ConfigException : public std::runtime_error {
public:
    ConfigException(ConfigErrorCode code, const std::string& message)
        : std::runtime_error(message), code_(code) {}
    
    ConfigErrorCode code() const { return code_; }
    
private:
    ConfigErrorCode code_;
};

// Configuration validation error
class ConfigValidationError : public ConfigException {
public:
    ConfigValidationError(const std::string& key, const std::string& message)
        : ConfigException(ConfigErrorCode::ValidationError,
                         "Validation error for key '" + key + "': " + message) {}
};

// Configuration type definition
struct ConfigType {
    using Validator = std::function<bool(const ConfigValue&)>;
    using DefaultFactory = std::function<ConfigValue()>;
    
    std::string name;
    std::string description;
    bool required;
    Validator validator;
    DefaultFactory defaultFactory;
    
    // Range checking for numeric types
    std::optional<double> minValue;
    std::optional<double> maxValue;
    
    // String validation
    std::optional<std::string> pattern; // Regex pattern
    std::vector<std::string> allowedValues;
    
    ConfigType()
        : name(), description(), required(false), validator(nullptr),
          defaultFactory(nullptr), minValue(std::nullopt), maxValue(std::nullopt) {}
    
    ConfigType(const std::string& n, const std::string& desc = "", bool req = false)
        : name(n), description(desc), required(req), validator(nullptr),
          defaultFactory(nullptr), minValue(std::nullopt), maxValue(std::nullopt) {}
    
    // Type-specific factory methods
    static ConfigType Int(const std::string& name, const std::string& desc = "",
                         bool req = false, int min = std::numeric_limits<int>::min(),
                         int max = std::numeric_limits<int>::max());
    
    static ConfigType Double(const std::string& name, const std::string& desc = "",
                            bool req = false, double min = std::numeric_limits<double>::min(),
                            double max = std::numeric_limits<double>::max());
    
    static ConfigType String(const std::string& name, const std::string& desc = "",
                           bool req = false, const std::string& pattern = "");
    
    static ConfigType Bool(const std::string& name, const std::string& desc = "",
                         bool req = false);
    
    static ConfigType StringList(const std::string& name, const std::string& desc = "",
                               bool req = false);
};

// Configuration schema
class ConfigSchema {
public:
    void addParameter(const ConfigType& param) {
        parameters_[param.name] = param;
    }
    
    bool hasParameter(const std::string& name) const {
        return parameters_.find(name) != parameters_.end();
    }
    
    std::optional<ConfigType> getParameter(const std::string& name) const {
        auto it = parameters_.find(name);
        if (it != parameters_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    const std::map<std::string, ConfigType>& getAllParameters() const {
        return parameters_;
    }
    
    bool validateAll() const {
        for (const auto& pair : parameters_) {
            const auto& param = pair.second;
            if (param.required) {
                return false;
            }
        }
        return true;
    }
    
    std::vector<std::string> getMissingRequired() const {
        std::vector<std::string> missing;
        for (const auto& pair : parameters_) {
            if (pair.second.required) {
                missing.push_back(pair.first);
            }
        }
        return missing;
    }
    
    std::string getSchemaSummary() const {
        std::ostringstream oss;
        oss << "Configuration Schema (" << parameters_.size() << " parameters):\n";
        for (const auto& pair : parameters_) {
            const auto& param = pair.second;
            oss << "  " << param.name << " (" << param.description << ")";
            if (param.required) oss << " [REQUIRED]";
            oss << "\n";
        }
        return oss.str();
    }
    
private:
    std::map<std::string, ConfigType> parameters_;
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource src,
                const std::string& desc = "")
        : key(k), value(v), source(src), description(desc) {}
};

// Configuration change event
struct ConfigChangeEvent {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource source;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigChangeEvent(const std::string& k, const ConfigValue& oldVal,
                     const ConfigValue& newVal, ConfigSource src)
        : key(k), oldValue(oldVal), newValue(newVal), source(src),
          timestamp(std::chrono::system_clock::now()) {}
};

// Main configuration class
class Config {
public:
    using ChangeCallback = std::function<void(const ConfigChangeEvent&)>;
    
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Set global schema
    void setSchema(std::unique_ptr<ConfigSchema> schema) {
        schema_ = std::move(schema);
    }
    
    // Set validation callback
    void setValidationCallback(ChangeCallback callback) {
        validationCallback_ = callback;
    }
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath, const std::string& format = "auto") const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value,
             ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    void set(const std::string& key, const std::string& value,
             ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    void set(const std::string& key, int value,
             ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    void set(const std::string& key, double value,
             ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    void set(const std::string& key, bool value,
             ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get schema
    std::shared_ptr<const ConfigSchema> getSchema() const { return schema_; }
    
    // Get schema summary
    std::string getSchemaSummary() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const { return validationErrors_; }
    
    // Config diff
    std::string diff(const Config& other) const;
    
    // Format specific operations
    static bool saveJSON(const std::string& filepath,
                        const std::vector<ConfigEntry>& entries) noexcept;
    static bool saveYAML(const std::string& filepath,
                        const std::vector<ConfigEntry>& entries) noexcept;
    static bool saveSimple(const std::string& filepath,
                          const std::vector<ConfigEntry>& entries) noexcept;
    
    static std::vector<ConfigEntry> loadJSON(const std::string& filepath);
    static std::vector<ConfigEntry> loadYAML(const std::string& filepath);
    static std::vector<ConfigEntry> loadSimple(const std::string& filepath);
    
    static std::string detectFormat(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Schema and validation
    std::unique_ptr<ConfigSchema> schema_;
    ChangeCallback validationCallback_;
    std::vector<std::string> validationErrors_;
    
    // Format detection helpers
    bool isJSONFile(const std::string& filepath) const;
    bool isYAMLFile(const std::string& filepath) const;
    bool isSimpleFile(const std::string& filepath) const;
    
    // Validation helpers
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    void reportValidationError(const std::string& error);
    
    // Format helpers
    static std::string valueToString(const ConfigValue& value) const;
    static ConfigValue stringToValue(const std::string& str) const;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static std::string escapeJSONString(const std::string& str) const;
    static std::string escapeYAMLString(const std::string& str) const;
    
    // File operations
    static bool fileExists(const std::string& filepath) noexcept;
    static std::string getFileExtension(const std::string& filepath);
};

// Implementation details
namespace nlm {
    // ConfigType implementation
    inline ConfigType ConfigType::Int(const std::string& name, const std::string& desc,
                                   bool req, int min, int max) {
        ConfigType type(name, desc, req);
        type.minValue = static_cast<double>(min);
        type.maxValue = static_cast<double>(max);
        type.validator = [min, max](const ConfigValue& val) {
            try {
                if constexpr (std::is_same_v<int, std::decay_t<decltype(std::get<int>(val))>>) {
                    int v = std::get<int>(val);
                    return v >= min && v <= max;
                }
                return false;
            } catch (...) { return false; }
        };
        return type;
    }
    
    inline ConfigType ConfigType::Double(const std::string& name, const std::string& desc,
                                       bool req, double min, double max) {
        ConfigType type(name, desc, req);
        type.minValue = min;
        type.maxValue = max;
        type.validator = [min, max](const ConfigValue& val) {
            try {
                if constexpr (std::is_same_v<double, std::decay_t<decltype(std::get<double>(val))>>) {
                    double v = std::get<double>(val);
                    return v >= min && v <= max;
                }
                return false;
            } catch (...) { return false; }
        };
        return type;
    }
    
    inline ConfigType ConfigType::String(const std::string& name, const std::string& desc,
                                       bool req, const std::string& pattern) {
        ConfigType type(name, desc, req);
        type.pattern = pattern;
        type.validator = [pattern](const ConfigValue& val) {
            try {
                if constexpr (std::is_same_v<std::string, std::decay_t<decltype(std::get<std::string>(val))>>) {
                    const std::string& str = std::get<std::string>(val);
                    if (!pattern.empty()) {
                        std::regex regex(pattern);
                        return std::regex_match(str, regex);
                    }
                    return true;
                }
                return false;
            } catch (...) { return false; }
        };
        return type;
    }
    
    inline ConfigType ConfigType::Bool(const std::string& name, const std::string& desc,
                                     bool req) {
        ConfigType type(name, desc, req);
        type.validator = [](const ConfigValue& val) {
            try {
                return std::holds_alternative<bool>(val);
            } catch (...) { return false; }
        };
        return type;
    }
    
    inline ConfigType ConfigType::StringList(const std::string& name, const std::string& desc,
                                           bool req) {
        ConfigType type(name, desc, req);
        type.validator = [](const ConfigValue& val) {
            try {
                return std::holds_alternative<std::vector<std::string>>(val);
            } catch (...) { return false; }
        };
        return type;
    }

    // Config implementation helper functions
    inline bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }
    
    inline std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    inline std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    inline bool Config::isJSONFile(const std::string& filepath) const {
        return startsWith(filepath, "{") || startsWith(filepath, "[") ||
               getFileExtension(filepath) == "json" ||
               getFileExtension(filepath) == "json5" ||
               getFileExtension(filepath) == "jsonc";
    }
    
    inline bool Config::isYAMLFile(const std::string& filepath) const {
        return startsWith(filepath, "---") || startsWith(filepath, "key:") ||
               getFileExtension(filepath) == "yaml" ||
               getFileExtension(filepath) == "yml";
    }
    
    inline bool Config::isSimpleFile(const std::string& filepath) const {
        return startsWith(filepath, "#") || startsWith(filepath, "//") ||
               startsWith(filepath, "--") || getFileExtension(filepath).empty();
    }
    
    inline bool Config::fileExists(const std::string& filepath) noexcept {
        return std::filesystem::exists(filepath);
    }
    
    inline std::string Config::getFileExtension(const std::string& filepath) {
        size_t dotPos = filepath.find_last_of('.');
        if (dotPos != std::string::npos) {
            return filepath.substr(dotPos + 1);
        }
        return "";
    }
    
    inline std::string Config::escapeJSONString(const std::string& str) const {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
    
    inline std::string Config::escapeYAMLString(const std::string& str) const {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                case ':': result += "\\:"; break;
                case '{': result += "\\{"; break;
                case '}': result += "\\}"; break;
                case '[': result += "\\["; break;
                case ']': result += "\\]"; break;
                case ',': result += "\\,"; break;
                case '#': result += "\\#"; break;
                case '&': result += "\\&"; break;
                case '*': result += "\\*"; break;
                case '!': result += "\\!"; break;
                case '|': result += "\\|"; break;
                case '>': result += "\\>"; break;
                case "'": result += "\\'"; break;
                default:
                    if (c < 32 || c == 127) {
                        result += "\\x" + std::to_string(static_cast<int>(c));
                    } else {
                        result += c;
                    }
                    break;
            }
        }
        return result;
    }
    
    inline std::string Config::valueToString(const ConfigValue& value) const {
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
    
    inline ConfigValue Config::stringToValue(const std::string& str) const {
        if (str.empty()) {
            return "";
        }
        
        std::string content = trim(str);
        
        if (content == "true") {
            return true;
        } else if (content == "false") {
            return false;
        } else if (content.front() == '"' && content.back() == '"') {
            return content.substr(1, content.size() - 2);
        } else if (content.front() == '\'' && content.back() == '\'') {
            return content.substr(1, content.size() - 2);
        } else if (content.front() == '[' && content.back() == ']') {
            std::vector<std::string> tokens;
            size_t start = 1, end;
            while (start < content.size() - 1) {
                end = content.find(',', start);
                if (end == std::string::npos) {
                    end = content.size() - 1;
                }
                std::string token = trim(content.substr(start, end - start));
                if (!token.empty()) {
                    tokens.push_back(token);
                }
                start = end + 1;
                while (start < content.size() && content[start] == ' ') start++;
            }
            
            if (!tokens.empty()) {
                if (tokens[0].find('.') != std::string::npos) {
                    std::vector<double> result;
                    result.reserve(tokens.size());
                    for (const auto& token : tokens) {
                        try {
                            result.push_back(std::stod(token));
                        } catch (...) {}
                    }
                    return result;
                } else {
                    std::vector<int> result;
                    result.reserve(tokens.size());
                    for (const auto& token : tokens) {
                        try {
                            result.push_back(std::stoi(token));
                        } catch (...) {}
                    }
                    return result;
                }
            }
            return std::vector<std::string>();
        } else {
            try {
                if (content.find('.') != std::string::npos) {
                    return std::stod(content);
                } else {
                    return std::stoi(content);
                }
            } catch (...) {
                return content;
            }
        }
    }
    
    inline void Config::reportValidationError(const std::string& error) {
        validationErrors_.push_back(error);
    }
    
    inline bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
        if (schema_ && schema_->hasParameter(key)) {
            auto param = schema_->getParameter(key);
            if (param && param->validator && !param->validator(value)) {
                reportValidationError("Validation failed for key '" + key + "': value '" + 
                                    valueToString(value) + "' does not meet type constraints");
                return false;
            }
            
            if (param && param->minValue.has_value()) {
                try {
                    double val;
                    if constexpr (std::is_same_v<int, std::decay_t<decltype(std::get<int>(value))>>) {
                        val = std::get<int>(value);
                    } else if constexpr (std::is_same_v<double, std::decay_t<decltype(std::get<double>(value))>>) {
                        val = std::get<double>(value);
                    } else {
                        reportValidationError("Type check failed for key '" + key + "': " +
                                            "numeric min/max constraint invalid");
                        return false;
                    }
                    if (val < param->minValue.value()) {
                        reportValidationError("Value " + std::to_string(val) + " < " + 
                                            std::to_string(param->minValue.value()) +
                                            " for key '" + key + "'");
                        return false;
                    }
                } catch (...) {
                    reportValidationError("Type check failed for key '" + key + "': " +
                                        "min/max validation error");
                    return false;
                }
            }
            
            if (param && param->maxValue.has_value()) {
                try {
                    double val;
                    if constexpr (std::is_same_v<int, std::decay_t<decltype(std::get<int>(value))>>) {
                        val = std::get<int>(value);
                    } else if constexpr (std::is_same_v<double, std::decay_t<decltype(std::get<double>(value))>>) {
                        val = std::get<double>(value);
                    } else {
                        reportValidationError("Type check failed for key '" + key + "': " +
                                            "numeric max/min constraint invalid");
                        return false;
                    }
                    if (val > param->maxValue.value()) {
                        reportValidationError("Value " + std::to_string(val) + " > " + 
                                            std::to_string(param->maxValue.value()) +
                                            " for key '" + key + "'");
                        return false;
                    }
                } catch (...) {
                    reportValidationError("Type check failed for key '" + key + "': " +
                                        "max/min validation error");
                    return false;
                }
            }
        }
        return true;
    }
}
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
