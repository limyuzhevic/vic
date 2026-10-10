// ConfigError.cpp implementation
#include "ConfigError.hpp"

namespace nlm {

// ConfigError implementation details
namespace {
    // Helper function to get error type string
    const char* errorTypeToString(ConfigErrorType type) {
        switch (type) {
            case ConfigErrorType::None: return "None";
            case ConfigErrorType::InvalidType: return "InvalidType";
            case ConfigErrorType::OutOfRange: return "OutOfRange";
            case ConfigErrorType::MissingDependency: return "MissingDependency";
            case ConfigErrorType::ConflictingValues: return "ConflictingValues";
            case ConfigErrorType::InvalidValue: return "InvalidValue";
            case ConfigErrorType::FileNotFound: return "FileNotFound";
            case ConfigErrorType::ParseError: return "ParseError";
            case ConfigErrorType::SchemaError: return "SchemaError";
            default: return "Unknown";
        }
    }
}

} // namespace nlm