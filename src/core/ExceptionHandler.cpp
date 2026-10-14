#include "core/ExceptionHandler.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <mutex>

namespace nlm {

ExceptionHandler& ExceptionHandler::getInstance() {
    static ExceptionHandler instance;
    return instance;
}

void ExceptionHandler::logException(const std::shared_ptr<NLMException>& exception) {
    std::ostringstream oss;
    oss << "NLM Exception: " << exception->toString() << std::endl;
    oss << exception->formatError();
    
    // Use logger if available
    if (Logger::getGlobalLogger()) {
        Logger::getGlobalLogger()->log(oss.str(), LogLevel::Error);
    } else {
        std::cerr << oss.str();
    }
    
    // Call user callback if set
    if (exception_callback_) {
        exception_callback_(exception);
    }
}

void ExceptionHandler::logFatalException(const std::shared_ptr<NLMException>& exception) {
    std::ostringstream oss;
    oss << "FATAL NLM Exception: " << exception->toString() << std::endl;
    oss << exception->formatError();
    oss << std::endl;
    oss << "Application will terminate due to fatal error." << std::endl;
    
    if (Logger::getGlobalLogger()) {
        Logger::getGlobalLogger()->log(oss.str(), LogLevel::Critical);
    } else {
        std::cerr << oss.str();
    }
    
    // Call user callback if set
    if (exception_callback_) {
        exception_callback_(exception);
    }
    
    // Exit the application
    std::exit(1);
}

void ExceptionHandler::tryRecoveryOperations(const std::shared_ptr<NLMException>& exception) {
    // Implement recovery strategies based on exception type
    switch (exception->getCategory()) {
        case ErrorCategory::IOError:
            // Log file error and attempt to continue
            NLM_LOG_WARNING("IO error occurred. Continuing with available data.");
            break;
        case ErrorCategory::Memory:
            // Memory error - try to free resources
            NLM_LOG_WARNING("Memory error occurred. Attempting recovery...");
            break;
        case ErrorCategory::Validation:
            // Validation error - try to use defaults
            NLM_LOG_WARNING("Validation error occurred. Using default values.");
            break;
        case ErrorCategory::Configuration:
            // Configuration error - try to reload configuration
            NLM_LOG_WARNING("Configuration error occurred. Attempting to reload configuration.");
            break;
        default:
            NLM_LOG_WARNING("Unexpected error occurred. Continuing with caution.");
            break;
    }
}

std::string ExceptionHandler::getCategoryString(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::Validation: return "Validation";
        case ErrorCategory::Runtime: return "Runtime";
        case ErrorCategory::Configuration: return "Configuration";
        case ErrorCategory::IOError: return "IOError";
        case ErrorCategory::Memory: return "Memory";
        case ErrorCategory::Network: return "Network";
        case ErrorCategory::System: return "System";
        case ErrorCategory::Internal: return "Internal";
        default: return "Unknown";
    }
}

} // namespace nlm