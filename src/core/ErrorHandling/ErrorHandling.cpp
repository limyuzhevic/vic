#include "ErrorHandling.hpp"
#include <chrono>
#include <sstream>

namespace nlm {

namespace error {

// ErrorHandler implementation
ErrorHandler::ErrorCallback ErrorHandler::errorCallback_ = nullptr;
ErrorSeverity ErrorHandler::errorReportingLevel_ = ErrorSeverity::Error;

// Stack trace collection (simplified implementation)
static std::vector<std::string> captureStackTrace() {
    std::vector<std::string> stackTrace;
    // In a real implementation, this would capture actual stack trace
    // For now, return empty vector
    return stackTrace;
}

// Platform-specific stack trace capture
static std::string getCurrentStackTrace() {
    return "Stack trace not available in this implementation";
}

// Error handling integration with existing code
void logError(const NLMError& error) {
    ErrorHandler::logError(error);
}

void handleError(const NLMError& error) {
    ErrorHandler::handleError(error);
}

// Error handler registration
void registerErrorHandler(ErrorHandler::ErrorCallback callback) {
    ErrorHandler::registerErrorHandler(callback);
}

void unregisterErrorHandler() {
    ErrorHandler::unregisterErrorHandler();
}

// Error recovery utilities
bool canRecover(const NLMError& error) {
    return ErrorHandler::canRecover(error);
}

bool attemptRecovery(const NLMError& error) {
    return ErrorHandler::attemptRecovery(error);
}

// Error reporting
void setErrorReportingLevel(ErrorSeverity level) {
    ErrorHandler::setErrorReportingLevel(level);
}

ErrorSeverity getErrorReportingLevel() {
    return ErrorHandler::getErrorReportingLevel();
}

std::string getErrorSummary(const std::vector<NLMError>& errors) {
    return ErrorHandler::getErrorSummary(errors);
}

} // namespace error

} // namespace nlm
