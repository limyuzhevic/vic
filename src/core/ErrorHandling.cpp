#include "ErrorHandling.hpp"
#include <stdexcept>

namespace nlm {

struct ValidationError::Impl {
    // Additional context for validation errors
    std::string fieldName;
    std::string expectedValue;
    std::string actualValue;
};

std::string ValidationError::formatDetails() const {
    std::ostringstream oss;
    oss << "Validation Error: " << what();
    
    if (!field_.empty()) {
        oss << " (field: " << field_ << ")";
    }
    
    if (!expected_.empty() || !actual_.empty()) {
        oss << "\nExpected: " << (expected_.empty() ? "not specified" : expected_);
        oss << "\nActual: " << (actual_.empty() ? "not specified" : actual_);
    }
    
    return oss.str();
}

void ValidationError::print(std::ostream& os) const {
    os << formatDetails();
}

std::string RuntimeError::formatDetails() const {
    std::ostringstream oss;
    oss << "Runtime Error: " << what();
    
    if (!operation_.empty()) {
        oss << " (operation: " << operation_ << ")";
    }
    
    if (errorCode_ != 0) {
        oss << "\nError code: " << errorCode_;
    }
    
    if (systemError_.value() != 0) {
        oss << "\nSystem error: " << systemError_.message();
    }
    
    return oss.str();
}

void RuntimeError::print(std::ostream& os) const {
    os << formatDetails();
}

std::string SystemError::formatDetails() const {
    std::ostringstream oss;
    oss << "System Error: " << what();
    
    oss << "\nType: " << getTypeName();
    
    if (errorCode_ != 0) {
        oss << "\nError code: " << errorCode_;
    }
    
    if (!operation_.empty()) {
        oss << "\nOperation: " << operation_;
    }
    
    return oss.str();
}

void SystemError::print(std::ostream& os) const {
    os << formatDetails();
}

std::string SystemError::getTypeName() const {
    switch (type_) {
        case SystemErrorType::MemoryAllocation: return "Memory Allocation";
        case SystemErrorType::FileSystem: return "File System";
        case SystemErrorType::Network: return "Network";
        case SystemErrorType::Threading: return "Threading";
        case SystemErrorType::OperatingSystem: return "Operating System";
        case SystemErrorType::LibraryFunction: return "Library Function";
        case SystemErrorType::PlatformSpecific: return "Platform Specific";
        default: return "Unknown";
    }
}

std::string ConfigError::formatDetails() const {
    std::ostringstream oss;
    oss << "Configuration Error: " << what();
    
    oss << "\nType: " << getTypeName();
    
    if (!key_.empty()) {
        oss << "\nKey: " << key_;
    }
    
    if (!filepath_.empty()) {
        oss << "\nFile: " << filepath_;
    }
    
    return oss.str();
}

void ConfigError::print(std::ostream& os) const {
    os << formatDetails();
}

std::string ConfigError::getTypeName() const {
    switch (type_) {
        case ErrorType::InvalidFormat: return "Invalid Format";
        case ErrorType::MissingRequired: return "Missing Required";
        case ErrorType::InvalidValue: return "Invalid Value";
        case ErrorType::CircularReference: return "Circular Reference";
        case ErrorType::SchemaValidation: return "Schema Validation";
        case ErrorType::FileNotFound: return "File Not Found";
        case ErrorType::PermissionDenied: return "Permission Denied";
        default: return "Unknown";
    }
}

void NLMError::logError() const {
    NLM_LOG_ERROR(formatDetails());
}

struct ErrorContextManager::Impl {
    ErrorContext currentContext_ = ErrorContext("", "", 0, "");
};

ErrorContext ErrorContextManager::currentContext_;

} // namespace nlm
