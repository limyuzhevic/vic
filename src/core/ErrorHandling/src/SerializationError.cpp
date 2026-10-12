// SerializationError implementation
namespace nlm {

SerializationError::SerializationError(const std::string& message,
                                     SerializationErrorType type,
                                     const std::string& component,
                                     const std::string& suggestion)
    : NLMError("Serialization Error: " + message, ErrorSeverity::Error, ErrorCategory::Serialization),
      serializationErrorType_(type),
      component_(component),
      suggestion_(suggestion) {
    setRecoveryActions();
}

std::string SerializationError::getRecoveryHint() const {
    switch (serializationErrorType_) {
        case SerializationErrorType::VersionMismatch:
            return "Use compatible serialization format or update schema version.";
        case SerializationErrorType::CorruptedData:
            return "Check data integrity. Use data validation before serialization.";
        case SerializationErrorType::MissingRequiredData:
            return "Ensure all required data is present before serialization.";
        case SerializationErrorType::InvalidFormat:
            return "Use correct serialization format. Check format specification.";
        case SerializationErrorType::CompressionError:
            return "Try alternative compression method or disable compression.";
        case SerializationErrorType::EncryptionError:
            return "Check encryption key and settings. Use alternative encryption method.";
        case SerializationErrorType::Timeout:
            return "Increase timeout value or reduce data size. Check network connectivity.";
        case SerializationErrorType::MemoryUsageExceeded:
            return "Reduce data size or increase memory allocation. Use streaming serialization.";
        case SerializationErrorType::SerializationFailed:
            return "Check serialization parameters. Use alternative serialization format.";
        case SerializationErrorType::DeserializationFailed:
            return "Check data integrity and format. Use data validation after deserialization.";
        case SerializationErrorType::SchemaMismatch:
            return "Update schema to match serialized data. Check schema version and compatibility.";
        case SerializationErrorType::IntegrityCheckFailed:
            return "Check data integrity. Use integrity checks before and after serialization.";
        default:
            return "Check serialization parameters and data. Use alternative serialization method.";
    }
}

} // namespace nlm