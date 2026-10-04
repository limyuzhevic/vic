// Validation implementation for NLM neural simulation project
// Implements validation functions and error handling utilities

#include "Validation.hpp"

namespace nlm {

// Global instances
std::unique_ptr<ValidationUtils> g_validator;
std::unique_ptr<ErrorHandlingUtils> g_errorHandler;

// ValidationUtils implementation

// NeuralValueValidator implementation

// NeuralSystemValidator implementation

// ErrorHandlingUtils implementation

// Validator implementation

// Initialize error handling utilities
inline void initializeErrorHandlingUtils() {
    g_validator = std::make_unique<ValidationUtils>();
    g_errorHandler = std::make_unique<ErrorHandlingUtils>();
}

// Shutdown error handling utilities
inline void shutdownErrorHandlingUtils() {
    g_validator.reset();
    g_errorHandler.reset();
}

} // namespace nlm
