// NLM Error Handling Implementation
// Provides robust error handling and validation utilities for NLM

#include "core/Utilities/ErrorHandling.hpp"
#include <iostream>

using namespace nlm;

void ErrorReporter::logError(const std::string& error, bool throwException) {
    // Log error using existing logging system
    // For now, output to stderr
    std::cerr << "NLM ERROR: " << error << std::endl;
    if (throwException) {
        throw NLMError(error);
    }
}

void ErrorReporter::logWarning(const std::string& warning) {
    std::cerr << "NLM WARNING: " << warning << std::endl;
}

void ErrorReporter::logInfo(const std::string& info) {
    std::cout << "NLM INFO: " << info << std::endl;
}