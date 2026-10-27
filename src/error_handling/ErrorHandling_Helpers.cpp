// Error Handling System Helper Functions
// Additional helper functions for error handling

#include "error_handling/ErrorHandling.hpp"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

using namespace nlm;

namespace ErrorHandling {

// Forward declarations for internal implementation
NLMError::NLMError(const std::string& message);

// Initialize global instances
UserFeedbackSystem gUserFeedbackSystem;
ErrorRecoverySystem gErrorRecoverySystem;
ErrorReportingSystem gErrorReportingSystem;

// Helper functions for error handling

// Generate timestamp
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// NLMError constructor implementation
NLMError::NLMError(const std::string& message) {
    ErrorType type;
    type.category = ErrorCategory::UNKNOWN;
    type.code = "GENERIC";
    type.description = message;
    type.suggestedAction = "Check system logs for details";
    
    ErrorContext context;
    context.operation = "unknown";
    context.location = "unknown";
    context.timestamp = getCurrentTimestamp();
    
    details_ = message;
    timestamp_ = getCurrentTimestamp();
    severity_ = ErrorSeverity::INFO;
    
    generateFormattedMessage();
}

} // namespace ErrorHandling
