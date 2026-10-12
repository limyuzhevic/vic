// ErrorHandling.cpp - Implementation of error handling infrastructure for NLM
#include "ErrorHandling.hpp"
#include "ErrorHandlingUtilities.hpp"
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

// Helper function for error result conversion
void setupErrorHandling() {
    // Initialize any error handling resources if needed
    // Currently no specific initialization required
}

// Initialize error messages map
std::map<ErrorCode, std::string> NLMErrorCategory::errorMessages;

// Error handler implementation
void ErrorHandler::log(const LogEntry& entry) {
    // Default implementation - just forward to ostream
    // This can be overridden by specific implementations
}

} // namespace nlm
