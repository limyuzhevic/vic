// NLM Error System Implementation
// Comprehensive exception hierarchy for all error conditions

#include "Error.hpp"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

namespace nlm {

// Initialize error reporter singleton
ErrorReporter& ErrorReporter::getInstance() {
    static ErrorReporter instance;
    return instance;
}

// Global exception handler for uncaught exceptions
void setupGlobalExceptionHandler() {
    std::set_terminate([]() {
        std::cerr << "NLM: Uncaught exception!" << std::endl;
        std::terminate();
    });
}

// Safe exception catching and handling
NLMException catchAndConvert(const std::exception& e, const std::string& context = "") {
    std::string message = "Exception: " + std::string(e.what());
    if (!context.empty()) {
        message = "Context: " + context + ". " + message;
    }
    return NLMException(message, ErrorSeverity::Error, "CONVERTED_EXCEPTION", "", 0, "");
}

} // namespace nlm