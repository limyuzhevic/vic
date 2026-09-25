// Enhanced Error Handling Framework Implementation for NLM
// Comprehensive error management with recovery and diagnostics

#include "core/EnhancedErrorHandling.hpp"
#include <iostream>
#include <mutex>
#include <iomanip>

namespace nlm {

// Static member definitions
std::vector<NLMErrorManager::Error> NLMErrorManager::errors;

// Static mutex for thread-safe error access
static std::mutex errorMutex;

void NLMErrorManager::addError(NLMErrors type, const std::string& message, const std::string& context) {
    std::lock_guard<std::mutex> lock(errorMutex);
    errors.emplace_back(type, message, context);
}

std::vector<NLMErrorManager::Error> NLMErrorManager::getErrors() {
    std::lock_guard<std::mutex> lock(errorMutex);
    return errors;
}

void NLMErrorManager::clearErrors() {
    std::lock_guard<std::mutex> lock(errorMutex);
    errors.clear();
}

size_t NLMErrorManager::errorCount() {
    std::lock_guard<std::mutex> lock(errorMutex);
    return errors.size();
}

void NLMErrorManager::throwException(NLMErrors type, const std::string& message, const std::string& context) {
    throw NLMErrorManager::Exception(type, message, context);
}

} // namespace nlm
