// Simplified Error Handling Implementation
#pragma once

#include "Error.hpp"
#include "core/Logger/Logger.hpp"
#include <system_error>
#include <cstring>

namespace nlm {

// Implementation of Error class methods
const std::string NLMError::emptyString_ = "";

std::string NLMError::formatMessage(const std::string& message, const std::string& context,
                                     int line, const std::string& file, const std::string& function) {
    std::ostringstream oss;
    oss << message;
    
    if (!context.empty()) {
        oss << " (Context: " << context << ")";
    }
    
    if (!file.empty() && line > 0) {
        oss << " at " << file << ":" << line;
    }
    
    if (!function.empty()) {
        oss << " in " << function;
    }
    
    return oss.str();
}

// Error message catalog implementation
std::map<std::string, std::string> ErrorCatalog::messages_ = {
    {"config_file_not_found", "Configuration file not found: '{file}'"},
    {"config_parse_error", "Failed to parse configuration file '{file}': {error}"},
    {"config_key_missing", "Required configuration key '{key}' is missing"},
    {"config_value_out_of_range", "Configuration value for key '{key}' is out of valid range [{min}, {max}]"},
    {"memory_allocation_failed", "Memory allocation failed: requested {size} bytes, available {available}"},
    {"neural_network_not_built", "Neural network has not been built. Call buildNetwork() first"},
    {"file_not_found", "File not found: '{path}'"},
    {"file_read_failed", "Failed to read from file '{path}': {error}"},
    {"file_write_failed", "Failed to write to file '{path}': {error}"},
    {"system_resource_exhausted", "System resource exhausted: {resource}. Available: {available}, Requested: {requested}"},
    {"validation_null_pointer", "Null pointer check failed for parameter '{param}'"},
    {"validation_range_exceeded", "Parameter '{param}' value {value} exceeds valid range [{min}, {max}]"},
    {"unknown_error", "An unexpected error occurred: {error}"}
};

const std::string& ErrorCatalog::getMessage(const std::string& key) {
    auto it = messages_.find(key);
    static const std::string empty = "";
    return (it != messages_.end()) ? it->second : empty;
}

void ErrorCatalog::addMessage(const std::string& key, const std::string& message) {
    messages_[key] = message;
}

bool ErrorCatalog::hasMessage(const std::string& key) {
    return messages_.find(key) != messages_.end();
}

// Error utility functions implementation
std::string ErrorUtils::emptyString_ = "";

bool ErrorUtils::validateNotNull(const void* ptr, const std::string& name,
                                const std::string& context,
                                ErrorCategory category,
                                int line, const std::string& file,
                                const std::string& function) {
    if (!ptr) {
        // Log the error
        std::string message = ErrorCatalog::getMessage("validation_null_pointer");
        size_t pos = message.find("{param}");
        if (pos != std::string::npos) {
            message.replace(pos, 7, name);
        }
        
        NLMError error(message, category, context, line, file, function);
        logError(error);
        return false;
    }
    return true;
}

void ErrorUtils::logError(const NLMError& error) {
    std::string formatted = formatError(error);
    Logger::getGlobal().error(formatted, error.getFile(), error.getLine(), error.getFunction());
}

std::string ErrorUtils::formatError(const NLMError& error) {
    std::ostringstream oss;
    
    oss << "[" << static_cast<int>(error.getCategory()) << "] " << error.what();
    return oss.str();
}

bool ErrorUtils::shouldRetry(const NLMError& error) {
    switch (error.getCategory()) {
        case ErrorCategory::Timeout:
        case ErrorCategory::Network:
        case ErrorCategory::System:
            return true;
        case ErrorCategory::Validation:
        case ErrorCategory::Configuration:
        case ErrorCategory::File:
            return false;
        default:
            return false;
    }
}

std::string ErrorUtils::suggestRecovery(const NLMError& error) {
    switch (error.getCategory()) {
        case ErrorCategory::Memory:
            return "Memory recovery: Attempted to free cached resources and retry operation";
        case ErrorCategory::File:
            return "File recovery: Attempted to read from backup file or retry with increased timeout";
        case ErrorCategory::Configuration:
            return "Configuration recovery: Used default values and logged warning";
        default:
            return "Manual intervention required";
    }
}

NLMError ErrorUtils::translate(const std::exception& e, ErrorCategory category) {
    return NLMError(e.what(), category, "Translated from standard exception");
}

NLMError ErrorUtils::translateSystemError(int errorCode, const std::string& context) {
    return NLMError(std::strerror(errorCode), ErrorCategory::System, context);
}

NLMError ErrorUtils::translateFileError(const std::string& filepath, std::error_code ec) {
    return NLMError(ec.message(), ErrorCategory::File, filepath);
}

// Exception translation namespace implementation
NLMError exception::translate(const std::exception& e, ErrorCategory category) {
    return ErrorUtils::translate(e, category);
}

NLMError exception::translateSystemError(int errorCode, const std::string& context) {
    return ErrorUtils::translateSystemError(errorCode, context);
}

NLMError exception::translateFileError(const std::string& filepath, std::error_code ec) {
    return ErrorUtils::translateFileError(filepath, ec);
}

} // namespace nlm