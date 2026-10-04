// ErrorHandlingUtils implementation for NLM neural simulation project
// Provides utility functions for error handling and validation

#include "Validation.hpp"

namespace nlm {

// ErrorHandlingUtils implementation

// Validator implementation

// Initialize validation and error handling systems
void initializeValidationAndErrorHandling() {
    g_validator = std::make_unique<ValidationUtils>();
    g_errorHandler = std::make_unique<ErrorHandlingUtils>();
}

// Shutdown validation and error handling systems
void shutdownValidationAndErrorHandling() {
    g_validator.reset();
    g_errorHandler.reset();
}

} // namespace nlm
