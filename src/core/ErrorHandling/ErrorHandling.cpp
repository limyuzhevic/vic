// Error handling implementation
#include "ErrorHandling.hpp"
#include <iostream>

namespace nlm {

// Global error handler implementation
void logErrorToConsole(const std::string& message, ErrorSeverity severity) {
    std::string severityStr;
    switch (severity) {
        case ErrorSeverity::Debug: severityStr = "DEBUG"; break;
        case ErrorSeverity::Info: severityStr = "INFO"; break;
        case ErrorSeverity::Warning: severityStr = "WARNING"; break;
        case ErrorSeverity::Error: severityStr = "ERROR"; break;
        case ErrorSeverity::Fatal: severityStr = "FATAL"; break;
        default: severityStr = "UNKNOWN"; break;
    }
    
    std::cerr << "[" << severityStr << "] " << message << std::endl;
}

// Initialize global error handler
void initializeErrorHandling() {
    ErrorHandler::getInstance()->setGlobalErrorHandler(logErrorToConsole);
}

} // namespace nlm
