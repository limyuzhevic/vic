// ValidationError implementation
namespace nlm {

ValidationError::ValidationError(const std::string& message,
                               ValidationErrorType type,
                               const std::string& field,
                               const std::string& suggestion)
    : NLMError("Validation Error: " + message, ErrorSeverity::Warning, ErrorCategory::Validation),
      validationErrorType_(type),
      field_(field),
      suggestion_(suggestion) {
    setRecoveryActions();
}

std::string ValidationError::getRecoveryHint() const {
    switch (validationErrorType_) {
        case ValidationErrorType::TypeMismatch:
            return "Convert value to correct type or skip validation for this field.";
        case ValidationErrorType::ConstraintViolation:
            return "Adjust value to meet constraints or modify constraint rules.";
        case ValidationErrorType::FormatError:
            return "Fix value format. Check format specification and adjust accordingly.";
        case ValidationErrorType::RangeError:
            return "Adjust value to be within valid range. Check range validation rules.";
        case ValidationErrorType::PatternMismatch:
            return "Fix value pattern. Check pattern specification and adjust accordingly.";
        case ValidationErrorType::SchemaViolation:
            return "Update value to match schema requirements. Check schema version and compatibility.";
        case ValidationErrorType::DependencyError:
            return "Ensure dependent fields have valid values. Check field dependencies.";
        case ValidationErrorType::ConditionalValidationFailed:
            return "Adjust dependent field values to meet conditional validation rules.";
        case ValidationErrorType::CrossFieldValidationFailed:
            return "Fix cross-field relationships. Ensure field values meet cross-field validation rules.";
        case ValidationErrorType::BusinessLogicViolation:
            return "Adjust values to meet business logic rules. Check business validation rules.";
        case ValidationErrorType::SecurityPolicyViolation:
            return "Fix security policy violations. Check security rules and adjust values accordingly.";
        default:
            return "Fix validation error or skip validation for this field.";
    }
}

} // namespace nlm