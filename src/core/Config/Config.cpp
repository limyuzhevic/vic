// Config implementation with validation support
#include "core/Config/Config.hpp"
#include "core/Config/ConfigurationSchema.hpp"
#include "core/Config/ConfigurationValidator.hpp"
#include "core/Config/NLMConfigSchema.hpp"
#include "core/Config/ValidationError.hpp"
#include <fstream>
#include <sstream>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::unordered_map<std::string, ConfigEntry> configEntries;
    std::shared_ptr<ConfigurationSchema> schema;
    std::shared_ptr<ConfigurationValidator> validator;
    std::vector<ValidationError> validationErrors;
    bool schemaEnabled;
    
    Impl() : schemaEnabled(true) {
        schema = NLMConfigSchema::createDefaultSchema();
        validator = std::make_shared<ConfigurationValidator>();
        validator->setSchema(schema);
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept = default;
Config& Config::operator=(Config&& other) noexcept = default;

// Load from file
bool Config::loadFromFile(const std::string& filepath) {
    return loadFromSimpleFormat(filepath);
}

// Load from configuration file format
bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasEntries = false;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            // Collect validation error for malformed line
            pImpl->validationErrors.emplace_back(
                ValidationErrorType::InvalidFormat,
                "Invalid config line " + std::to_string(lineNumber) + ": " + line,
                lineNumber,
                filepath
            );
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Apply environment variable substitution
        value = substituteEnvironmentVariables(value);
        
        // Validate against schema if enabled
        if (pImpl->schemaEnabled && pImpl->validator) {
            auto errors = pImpl->validator->validateParameter(key, ConfigValue(value));
            pImpl->validationErrors.insert(pImpl->validationErrors.end(), errors.begin(), errors.end());
        }
        
        // Set the value
        set(key, value, ConfigSource::File);
        hasEntries = true;
    }
    
    if (!hasEntries) {
        pImpl->validationErrors.emplace_back(
            ValidationErrorType::InvalidFormat,
            "Config file has no valid entries",
            lineNumber,
            filepath
        );
    }
    
    return !pImpl->validationErrors.empty();
}

bool Config::loadFromJson(const std::string& filepath) {
    // TODO: Implement full JSON parser
    // For now, fall back to simple format for backward compatibility
    NLM_LOG_WARNING("JSON parsing not yet fully implemented - using simple format for: " + filepath);
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromYaml(const std::string& filepath) {
    // TODO: Implement full YAML parser
    // For now, fall back to simple format for backward compatibility
    NLM_LOG_WARNING("YAML parsing not yet fully implemented - using simple format for: " + filepath);
    return loadFromSimpleFormat(filepath);
}

// Load from command line arguments
bool Config::loadFromArgs(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    return validateCLIArguments(args);
}

// Save to file
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& [key, entry] : pImpl->configEntries) {
        file << key << " = " << ConfigurationValidator::configValueToString(entry.value) << "\n";
    }
    
    return true;
}

// Get values (defined here so templates instantiate in every TU)
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->configEntries.find(key);
    if (it == pImpl->configEntries.end()) {
        return std::nullopt;
    }
    
    const ConfigValue& value = it->second.value;
    
    if constexpr (std::is_same_v<T, int>) {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<int>(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return static_cast<int>(std::get<double>(value));
        }
    } else if constexpr (std::is_same_v<T, int64_t>) {
        if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value);
        } else if (std::holds_alternative<int>(value)) {
            return static_cast<int64_t>(std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            return static_cast<int64_t>(std::get<double>(value));
        }
    } else if constexpr (std::is_same_v<T, double>) {
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        } else if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<double>(std::get<int64_t>(value));
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        }
    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
        if (std::holds_alternative<std::vector<int>>(value)) {
            return std::get<std::vector<int>>(value);
        }
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        if (std::holds_alternative<std::vector<double>>(value)) {
            return std::get<std::vector<double>>(value);
        }
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        if (std::holds_alternative<std::vector<std::string>>(value)) {
            return std::get<std::vector<std::string>>(value);
        }
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto valueOpt = get<T>(key);
    if (valueOpt.has_value()) {
        return *valueOpt;
    }
    return defaultValue;
}

// Set values
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate against schema if enabled
    if (pImpl->schemaEnabled && pImpl->validator) {
        auto errors = pImpl->validator->validateParameter(key, value);
        pImpl->validationErrors.insert(pImpl->validationErrors.end(), errors.begin(), errors.end());
    }
    
    pImpl->configEntries[key] = ConfigEntry(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Validate and set string value
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Check existence
bool Config::has(const std::string& key) const {
    return pImpl->configEntries.find(key) != pImpl->configEntries.end();
}

// Remove key
void Config::remove(const std::string& key) {
    pImpl->configEntries.erase(key);
}

// Get all keys
std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->configEntries.size());
    for (const auto& pair : pImpl->configEntries) {
        keys.push_back(pair.first);
    }
    return keys;
}

// Clear all
void Config::clear() {
    pImpl->configEntries.clear();
    pImpl->validationErrors.clear();
}

// Get configuration summary
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:\n";
    oss << "Total parameters: " << pImpl->configEntries.size() << "\n";
    oss << "Validation errors: " << pImpl->validationErrors.size() << "\n";
    
    if (!pImpl->validationErrors.empty()) {
        oss << "\nValidation Errors:\n";
        for (const auto& error : pImpl->validationErrors) {
            oss << "  " << error.toString() << "\n";
        }
    }
    
    return oss.str();
}

// Configuration validation and schema
std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    errors.reserve(pImpl->validationErrors.size());
    for (const auto& error : pImpl->validationErrors) {
        errors.push_back(error.toString());
    }
    return errors;
}

std::string Config::getValidationErrorReport() const {
    std::ostringstream oss;
    if (pImpl->validationErrors.empty()) {
        return "No validation errors.";
    }
    
    oss << "Validation Errors (" << pImpl->validationErrors.size() << "):\n";
    for (const auto& error : pImpl->validationErrors) {
        oss << "  - " << error.toString() << "\n";
    }
    
    return oss.str();
}

bool Config::validate(const ConfigurationSchema& schema, bool allowUnknownParameters) const {
    if (!pImpl->validator) {
        return false;
    }
    
    // Update validator schema
    pImpl->validator->setSchema(std::const_pointer_cast<ConfigurationSchema>(std::shared_ptr<ConfigurationSchema>(&schema, [](const ConfigurationSchema*) {})));
    
    // Convert internal config to validator format
    std::unordered_map<std::string, ConfigEntry> configMap;
    for (const auto& [key, entry] : pImpl->configEntries) {
        configMap[key] = entry;
    }
    
    // Validate
    auto errors = pImpl->validator->validateComplete(configMap, allowUnknownParameters);
    pImpl->validationErrors = errors;
    
    return errors.empty();
}

bool Config::validateWithDefaultSchema() const {
    return validate(*pImpl->schema);
}

// Schema evolution and backward compatibility
void Config::setSchema(std::shared_ptr<ConfigurationSchema> schema) {
    pImpl->schema = schema;
    if (pImpl->validator) {
        pImpl->validator->setSchema(schema);
    }
}

std::shared_ptr<ConfigurationSchema> Config::getSchema() const {
    return pImpl->schema;
}

std::vector<std::string> Config::getDeprecatedParameters() const {
    std::vector<std::string> deprecated;
    
    if (!pImpl->schema) {
        return deprecated;
    }
    
    for (const auto& [key, param] : pImpl->schema->getParameters()) {
        if (param.deprecated) {
            deprecated.push_back(key);
        }
    }
    
    return deprecated;
}

// Environment variable substitution and CLI validation
std::string Config::substituteEnvironmentVariables(const std::string& value) const {
    std::string result = value;
    size_t pos = 0;
    
    while ((pos = result.find("${", pos)) != std::string::npos) {
        size_t endPos = result.find('}', pos);
        if (endPos == std::string::npos) {
            break; // Invalid syntax, leave as is
        }
        
        std::string varName = result.substr(pos + 2, endPos - pos - 2);
        const char* varValue = std::getenv(varName.c_str());
        
        if (varValue != nullptr) {
            result.replace(pos, endPos - pos + 1, varValue);
        } else {
            // Environment variable not found, replace with empty string
            result.replace(pos, endPos - pos + 1, "");
        }
        
        // Don't increment pos, as we replaced a variable
    }
    
    return result;
}

bool Config::validateCLIArguments(const std::vector<std::string>& args) {
    // Parse command line arguments
    // Simple implementation: --key=value format
    bool hasErrors = false;
    
    for (const auto& arg : args) {
        if (arg.find("=") == std::string::npos && arg.find("--") == 0) {
            // Flag without value
            continue;
        }
        
        if (arg.find("--") == 0) {
            size_t pos = arg.find("=");
            std::string key = arg.substr(2, pos - 2);
            std::string value = (pos != std::string::npos) ? arg.substr(pos + 1) : "";
            
            // Validate and set
            if (pImpl->schemaEnabled && pImpl->validator) {
                auto errors = pImpl->validator->validateParameter(key, ConfigValue(value));
                if (!errors.empty()) {
                    hasErrors = true;
                    for (const auto& error : errors) {
                        pImpl->validationErrors.push_back(error);
                    }
                }
            }
            
            set(key, value, ConfigSource::CommandLine);
        }
    }
    
    return !hasErrors;
}

// Configuration templates
std::unordered_map<std::string, ConfigValue> Config::applyTemplate(const std::string& templateName) {
    std::unordered_map<std::string, ConfigValue> result;
    
    if (templateName == "default") {
        auto defaultSchema = NLMConfigSchema::createDefaultSchema();
        for (const auto& [key, param] : defaultSchema->getParameters()) {
            if (param.defaultValue.has_value()) {
                result[key] = param.defaultValue.value();
            }
        }
    } else if (templateName == "minimal") {
        auto minimalSchema = NLMConfigSchema::createMinimalSchema();
        for (const auto& [key, param] : minimalSchema->getParameters()) {
            if (param.defaultValue.has_value()) {
                result[key] = param.defaultValue.value();
            }
        }
    } else if (templateName == "development") {
        auto devSchema = NLMConfigSchema::createDevelopmentSchema();
        for (const auto& [key, param] : devSchema->getParameters()) {
            if (param.defaultValue.has_value()) {
                result[key] = param.defaultValue.value();
            }
        }
    } else if (templateName == "optimized") {
        auto optimizedSchema = NLMConfigSchema::createOptimizedSchema();
        for (const auto& [key, param] : optimizedSchema->getParameters()) {
            if (param.defaultValue.has_value()) {
                result[key] = param.defaultValue.value();
            }
        }
    }
    
    return result;
}

// Internal helpers
std::string Config::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && std::isspace(str[start])) {
        ++start;
    }
    
    while (end > start && std::isspace(str[end - 1])) {
        --end;
    }
    
    return str.substr(start, end - start);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

} // namespace nlm
