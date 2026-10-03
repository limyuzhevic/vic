// ErrorHandling.cpp - Implementation of comprehensive error handling and validation system
//
// This file provides the implementation for the ErrorHandler class and related
// error handling functionality for the NLM brain simulator.

#include "ErrorHandling.hpp"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

// Initialize static member variables
std::vector<ErrorHandler::ErrorInfo> ErrorHandler::errorLog;
bool ErrorHandler::validationEnabled = true;
ErrorHandler::ErrorType ErrorHandler::lastErrorType = ErrorHandler::ErrorType::VALIDATION_ERROR;
std::string ErrorHandler::lastErrorMessage = "";

// Global error handler instance
inline ErrorHandler& getErrorHandler() {
    static ErrorHandler handler;
    return handler;
}

// Convenience functions - already declared in header, included here for completeness
void initializeErrorHandling() {
    getErrorHandler().initialize();
}

bool validateCondition(bool condition, const std::string& message) {
    return getErrorHandler().validateCondition(condition, message);
}

bool validateNotNull(const void* ptr, const std::string& message) {
    return getErrorHandler().validateNotNull(ptr, message);
}

bool validateRange(float value, float min, float max, const std::string& message) {
    return getErrorHandler().validateRange(value, min, max, message);
}

void reportBuildErrors() {
    getErrorHandler().reportErrors();
}

bool hasBuildErrors() {
    return getErrorHandler().hasErrors();
}

bool hasBuildWarnings() {
    return getErrorHandler().hasWarnings();
}

void clearBuildErrors() {
    getErrorHandler().clearErrors();
}

bool isValidationEnabled() {
    return getErrorHandler().isValidationEnabled();
}

void enableValidation(bool enable) {
    getErrorHandler().enableValidation(enable);
}

} // namespace nlm
