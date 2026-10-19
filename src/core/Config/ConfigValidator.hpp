#pragma once

namespace nlm {

struct ConfigValidationError {
    enum class ErrorCode {
        InvalidRange,
        InvalidType,
        MissingParameter,
        InvalidFileFormat,
        FileNotFound,
        InvalidValue,
        ParameterDependency
    };
    
    ErrorCode code;
    std::string parameter;
    std::string value;
    std::string message;
    std::string suggestion;
};

class ConfigValidator {
public:
    static std::vector<ConfigValidationError> validate(const std::vector<ConfigEntry>& entries);
    static std::vector<ConfigValidationError> validateParameter(const std::string& key, const ConfigValue& value);
    static std::vector<ConfigValidationError> validateFile(const std::string& filepath);
    static std::string errorCodeToString(ConfigValidationError::ErrorCode code);
    
private:
    static std::vector<ConfigValidationError> validateBasicRanges(const std::string& key, const ConfigValue& value);
    static std::vector<ConfigValidationError> validateParameterDependencies(const std::vector<ConfigEntry>& entries);
    static std::vector<ConfigValidationError> validateFileFormat(const std::string& filepath);
};

} // namespace nlm