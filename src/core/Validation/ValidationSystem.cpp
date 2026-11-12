// Implementation of Configuration Validation and Error Handling System
//
// This file provides the implementation for the NLM configuration validation
// and error handling system.

#include "core/Validation/ValidationSystem.hpp"
#include <stdexcept>

namespace nlm {

// ConfigValidator instance
ConfigValidator ConfigValidator::instance;

// Global error handler function
void setGlobalErrorHandler(std::function<void(const NLMError&)> handler) {
    ConfigValidator::instance.setErrorHandler(handler);
}

// Convenience function to validate brain configuration
std::vector<NLMError> validateBrainConfig(const std::shared_ptr<Config>& config) {
    return ConfigValidator::instance.validate(config);
}

// Initialize validation with default rules
void initializeValidation() {
    ConfigValidator::instance.initializeValidationRules();
}

} // namespace nlm