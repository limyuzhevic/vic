// NLM (熙然) - Neural Learning Machine Error Handling Framework Implementation
// 
// This file implements the error handling framework defined in ErrorHandling.hpp.
// It provides the actual implementation for error management, exception throwing,
// and all the utility functions used throughout the NLM codebase.

#include "core/ErrorHandling/ErrorHandling.hpp"
#include <iostream>
#include <mutex>
#include <iomanip>

namespace nlm {

// Static member definitions
std::vector<NLMErrors::Error> NLMErrors::errors;

// Static mutex for thread-safe error access
static std::mutex errorMutex;

void NLMErrors::addError(NLMErrors type, const std::string& message, const std::string& context) {
    std::lock_guard<std::mutex> lock(errorMutex);
    errors.emplace_back(type, message, context);
}

std::vector<NLMErrors::Error> NLMErrors::getErrors() {
    std::lock_guard<std::mutex> lock(errorMutex);
    return errors;
}

void NLMErrors::clearErrors() {
    std::lock_guard<std::mutex> lock(errorMutex);
    errors.clear();
}

size_t NLMErrors::errorCount() {
    std::lock_guard<std::mutex> lock(errorMutex);
    return errors.size();
}

void NLMErrors::throwException(NLMErrors type, const std::string& message, const std::string& context) {
    throw NLMErrors::Exception(type, message, context);
}

} // namespace nlm
