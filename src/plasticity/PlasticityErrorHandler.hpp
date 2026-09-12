// Plasticity system error handling
#pragma once

#include "PlasticityRule.hpp"
#include "../../core/Error/Errors.hpp"
#include <string>

namespace nlm {

class PlasticityErrorHandler {
public:
    // Handle plasticity rule configuration errors
    static void validatePlasticityConfig(const PlasticityRule& rule, const std::string& operation) {
        if (!rule.isEnabled()) {
            throw NLMPlasticityError("Plasticity rule not enabled in " + operation);
        }
    }
    
    // Validate plasticity parameters
    static void validatePlasticityParameters(float learningRate, float weight, 
                                             const std::string& operation) {
        ValidationUtils::validateRange(learningRate, 0.0f, 1.0f, 
                                      "PlasticityErrorHandler::validatePlasticityParameters");
        
        ValidationUtils::validateRange(weight, -1.0f, 1.0f,
                                      "PlasticityErrorHandler::validatePlasticityParameters");
    }
    
    // Handle Hebbian rule specific errors
    static void handleHebbianError(const std::string& operation, const std::string& details = "") {
        throw NLMPlasticityError("Hebbian plasticity error in " + operation + 
                               (details.empty() ? "" : ": " + details));
    }
    
    // Handle STDP specific errors
    static void handleSTDPError(const std::string& operation, const std::string& details = "") {
        throw NLMPlasticityError("STDP error in " + operation +
                               (details.empty() ? "" : ": " + details));
    }
    
    // Handle structural plasticity errors
    static void handleStructuralPlasticityError(const std::string& operation, 
                                               const std::string& details = "") {
        throw NLMPlasticityError("Structural plasticity error in " + operation +
                               (details.empty() ? "" : ": " + details));
    }
};

} // namespace nlm
