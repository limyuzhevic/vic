// ConfigurationError implementation
namespace nlm {

ConfigurationError::ConfigurationError(const std::string& message, 
                                       ConfigErrorType type,
                                       const std::string& field,
                                       const std::string& suggestion)
    : NLMError("Configuration Error: " + message, ErrorSeverity::Error, ErrorCategory::Configuration),
      configErrorType_(type),
      field_(field),
      suggestion_(suggestion) {
    setRecoveryActions();
}

std::string ConfigurationError::getRecoveryHint() const {
    switch (configErrorType_) {
        case ConfigErrorType::InvalidFormat:
            return "Check configuration format. Use default configuration or fix the format.";
        case ConfigErrorType::MissingKey:
            return "Ensure all required configuration keys are present. Provide default values if needed.";
        case ConfigErrorType::TypeMismatch:
            return "Fix type mismatch. Convert value to correct type or adjust type validation rules.";
        case ConfigErrorType::ValidationFailed:
            return "Fix validation failures. Check validation rules and adjust configuration values.";
        case ConfigErrorType::FileIOError:
            return "Check file permissions and availability. Ensure file path is correct.";
        case ConfigErrorType::EnvironmentVariableNotFound:
            return "Set environment variable or provide alternative configuration value.";
        case ConfigErrorType::OutOfRange:
            return "Adjust value to be within valid range. Check range validation rules.";
        case ConfigErrorType::CircularReference:
            return "Remove circular references in configuration. Break dependency loop.";
        case ConfigErrorType::SchemaMismatch:
            return "Update configuration to match schema. Check schema version and compatibility.";
        case ConfigErrorType::DeprecatedKey:
            return "Use updated configuration key. Check documentation for replacements.";
        case ConfigErrorType::ReservedKey:
            return "Avoid using reserved configuration keys. Choose a different key name.";
        default:
            return "Review configuration and fix the error.";
    }
}

} // namespace nlm