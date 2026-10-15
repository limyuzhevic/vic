#include "ErrorHandler.hpp"
#include <iostream>
#include <algorithm>
#include <ctime>

namespace nlm {

void DefaultErrorHandler::handleError(const ErrorInfo& error) {
    // Format error message
    std::stringstream ss;
    ss << "[" << error.timestamp << "] ";
    
    // Add error type
    switch (error.type) {
        case ErrorType::TestFailure:
            ss << "[TEST_FAILURE]";
            break;
        case ErrorType::ConfigurationError:
            ss << "[CONFIGURATION_ERROR]";
            break;
        case ErrorType::PluginError:
            ss << "[PLUGIN_ERROR]";
            break;
        case ErrorType::ResourceError:
            ss << "[RESOURCE_ERROR]";
            break;
        case ErrorType::NetworkError:
            ss << "[NETWORK_ERROR]";
            break;
        case ErrorType::MemoryError:
            ss << "[MEMORY_ERROR]";
            break;
        case ErrorType::TimeoutError:
            ss << "[TIMEOUT_ERROR]";
            break;
        case ErrorType::DependencyError:
            ss << "[DEPENDENCY_ERROR]";
            break;
        case ErrorType::ValidationError:
            ss << "[VALIDATION_ERROR]";
            break;
        case ErrorType::SystemError:
            ss << "[SYSTEM_ERROR]";
            break;
        case ErrorType::CustomError:
            ss << "[CUSTOM_ERROR]";
            break;
    }
    
    ss << " " << error.message;
    
    if (!error.testName.empty()) {
        ss << " (Test: " << error.testName << ")";
    }
    
    // Output to console
    std::cerr << ss.str() << std::endl;
    
    // Store error
    errors_.push_back(error);
}

bool DefaultErrorHandler::canRecover(const ErrorInfo& error) const {
    // Check if error is recoverable based on type
    switch (error.type) {
        case ErrorType::TestFailure:
            return error.recoverable && error.recoveryAction == "retry";
        case ErrorType::ConfigurationError:
            return false; // Configuration errors are not recoverable automatically
        case ErrorType::PluginError:
            return error.recoverable && error.recoveryAction == "skip";
        case ErrorType::ResourceError:
            return error.recoverable && error.recoveryAction == "delay";
        case ErrorType::NetworkError:
            return error.recoverable && error.recoveryAction == "retry";
        case ErrorType::MemoryError:
            return error.recoverable && error.recoveryAction == "cleanup";
        case ErrorType::TimeoutError:
            return error.recoverable && error.recoveryAction == "skip";
        case ErrorType::DependencyError:
            return false; // Dependency errors are not recoverable
        case ErrorType::ValidationError:
            return false; // Validation errors require manual fix
        case ErrorType::SystemError:
            return false; // System errors are not recoverable
        case ErrorType::CustomError:
            return error.recoverable && error.recoveryAction == "custom";
        default:
            return false;
    }
}

std::string DefaultErrorHandler::getRecoveryAction(const ErrorInfo& error) const {
    return getDefaultRecoveryAction(error.type);
}

std::chrono::milliseconds DefaultErrorHandler::getRecoveryDelay(const ErrorInfo& error) const {
    return getDefaultRecoveryDelay(error.type);
}

void DefaultErrorHandler::logError(const ErrorInfo& error) {
    handleError(error);
}

void DefaultErrorHandler::formatErrorReport(std::ostream& output) const {
    output << "Error Report" << std::endl;
    output << "=========== " << errors_.size() << " errors" << std::endl;
    output << std::endl;
    
    for (const auto& error : errors_) {
        output << "Error: " << error.message << std::endl;
        output << "Type: " << static_cast<int>(error.type) << std::endl;
        output << "Test: " << error.testName << std::endl;
        output << "Timestamp: " << error.timestamp << std::endl;
        output << "Recoverable: " << (error.recoverable ? "Yes" : "No") << std::endl;
        output << "Recovery Action: " << error.recoveryAction << std::endl;
        output << "---" << std::endl;
    }
}

void DefaultErrorHandler::categorizeError(ErrorInfo& error) {
    // Enhanced error categorization based on content
    std::string message = error.message;
    std::transform(message.begin(), message.end(), message.begin(), ::tolower);
    
    if (message.find("timeout") != std::string::npos) {
        error.type = ErrorType::TimeoutError;
    } else if (message.find("memory") != std::string::npos) {
        error.type = ErrorType::MemoryError;
    } else if (message.find("network") != std::string::npos) {
        error.type = ErrorType::NetworkError;
    } else if (message.find("resource") != std::string::npos) {
        error.type = ErrorType::ResourceError;
    } else if (message.find("plugin") != std::string::npos) {
        error.type = ErrorType::PluginError;
    } else if (message.find("config") != std::string::npos) {
        error.type = ErrorType::ConfigurationError;
    }
    
    // Set default recovery action based on type
    error.recoveryAction = getDefaultRecoveryAction(error.type);
    error.suggestedDelay = getDefaultRecoveryDelay(error.type);
}

std::string DefaultErrorHandler::getDefaultRecoveryAction(ErrorType type) const {
    switch (type) {
        case ErrorType::TestFailure:
            return "retry";
        case ErrorType::ConfigurationError:
            return "manual";
        case ErrorType::PluginError:
            return "skip";
        case ErrorType::ResourceError:
            return "delay";
        case ErrorType::NetworkError:
            return "retry";
        case ErrorType::MemoryError:
            return "cleanup";
        case ErrorType::TimeoutError:
            return "skip";
        case ErrorType::DependencyError:
            return "manual";
        case ErrorType::ValidationError:
            return "manual";
        case ErrorType::SystemError:
            return "abort";
        case ErrorType::CustomError:
            return "custom";
        default:
            return "ignore";
    }
}

std::chrono::milliseconds DefaultErrorHandler::getDefaultRecoveryDelay(ErrorType type) const {
    switch (type) {
        case ErrorType::TestFailure:
            return std::chrono::milliseconds(1000);
        case ErrorType::ConfigurationError:
            return std::chrono::milliseconds(0);
        case ErrorType::PluginError:
            return std::chrono::milliseconds(0);
        case ErrorType::ResourceError:
            return std::chrono::milliseconds(5000);
        case ErrorType::NetworkError:
            return std::chrono::milliseconds(2000);
        case ErrorType::MemoryError:
            return std::chrono::milliseconds(3000);
        case ErrorType::TimeoutError:
            return std::chrono::milliseconds(0);
        case ErrorType::DependencyError:
            return std::chrono::milliseconds(0);
        case ErrorType::ValidationError:
            return std::chrono::milliseconds(0);
        case ErrorType::SystemError:
            return std::chrono::milliseconds(0);
        case ErrorType::CustomError:
            return std::chrono::milliseconds(1000);
        default:
            return std::chrono::milliseconds(0);
    }
}

} // namespace nlm
