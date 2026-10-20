#include "ErrorHandler.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// InputValidator::Impl nested class implementation
struct InputValidator::Impl {
    std::shared_ptr<ErrorHandler> errorHandler;
};

InputValidator::InputValidator(std::shared_ptr<ErrorHandler> errorHandler) 
    : pImpl(std::make_unique<Impl>()) {
    pImpl->errorHandler = errorHandler;
}

InputValidator::~InputValidator() = default;

ValidationResult InputValidator::validateIntRange(const std::string& key, const std::string& value,
                                                 int min, int max, const std::string& testName) const {
    ValidationResult result;
    
    try {
        int intValue = std::stoi(value);
        if (intValue < min) {
            result.addError("Value " + value + " for " + key + " is less than minimum " + std::to_string(min));
        }
        if (intValue > max) {
            result.addError("Value " + value + " for " + key + " is greater than maximum " + std::to_string(max));
        }
    } catch (const std::exception& e) {
        result.addError("Value " + value + " for " + key + " is not a valid integer: " + e.what());
    }
    
    return result;
}

ValidationResult InputValidator::validateDoubleRange(const std::string& key, const std::string& value,
                                                     double min, double max, const std::string& testName) const {
    ValidationResult result;
    
    try {
        double doubleValue = std::stod(value);
        if (doubleValue < min) {
            result.addError("Value " + value + " for " + key + " is less than minimum " + std::to_string(min));
        }
        if (doubleValue > max) {
            result.addError("Value " + value + " for " + key + " is greater than maximum " + std::to_string(max));
        }
    } catch (const std::exception& e) {
        result.addError("Value " + value + " for " + key + " is not a valid number: " + e.what());
    }
    
    return result;
}

ValidationResult InputValidator::validateStringLength(const std::string& key, const std::string& value,
                                                      size_t minLength, size_t maxLength,
                                                      const std::string& testName) const {
    ValidationResult result;
    
    if (value.length() < minLength) {
        result.addError("Value " + value + " for " + key + " is too short (minimum " + 
                       std::to_string(minLength) + " characters)");
    }
    
    if (value.length() > maxLength) {
        result.addError("Value " + value + " for " + key + " is too long (maximum " + 
                       std::to_string(maxLength) + " characters)");
    }
    
    return result;
}

ValidationResult InputValidator::validateFilePath(const std::string& key, const std::string& value,
                                                 const std::string& testName) const {
    ValidationResult result;
    
    // Check if file exists and is readable
    if (!value.empty()) {
        std::ifstream file(value);
        if (!file.good()) {
            result.addWarning("File " + value + " for " + key + " may not be accessible or readable");
        }
        file.close();
    }
    
    return result;
}

ValidationResult InputValidator::validateURL(const std::string& key, const std::string& value,
                                            const std::string& testName) const {
    ValidationResult result;
    
    // Basic URL validation
    if (!value.empty()) {
        // Check if it starts with http:// or https://
        if (value.find("http://") != 0 && value.find("https://") != 0) {
            result.addError("Value " + value + " for " + key + " is not a valid URL (must start with http:// or https://)");
        }
    }
    
    return result;
}

ValidationResult InputValidator::validateTestName(const std::string& key, const std::string& value,
                                                 const std::string& testName) const {
    ValidationResult result;
    
    // Valid test names
    std::vector<std::string> validTestNames = {
        "basic", "plasticity", "stdp", "structural_plasticity",
        "neuromodulation", "development", "memory"
    };
    
    if (std::find(validTestNames.begin(), validTestNames.end(), value) == validTestNames.end()) {
        result.addError("Value " + value + " for " + key + " is not a valid test name");
    }
    
    return result;
}

ValidationResult InputValidator::validateTestParameters(const std::string& testName,
                                                       const std::map<std::string, std::string>& params) const {
    ValidationResult result;
    
    // Test-specific parameter validation
    if (testName == "plasticity") {
        // Validate plasticity-specific parameters
        if (params.find("plasticity_steps") != params.end()) {
            ValidationResult stepResult = validateIntRange("plasticity_steps", params.at("plasticity_steps"), 1, 100000, testName);
            if (!stepResult.valid) {
                for (const auto& error : stepResult.errors) {
                    result.addError(error);
                }
                for (const auto& warning : stepResult.warnings) {
                    result.addWarning(warning);
                }
            }
        }
        
        if (params.find("learning_rate") != params.end()) {
            ValidationResult lrResult = validateDoubleRange("learning_rate", params.at("learning_rate"), 0.0, 1.0, testName);
            if (!lrResult.valid) {
                for (const auto& error : lrResult.errors) {
                    result.addError(error);
                }
                for (const auto& warning : lrResult.warnings) {
                    result.addWarning(warning);
                }
            }
        }
    } else if (testName == "stdp") {
        // Validate STDP-specific parameters
        if (params.find("stdp_trials") != params.end()) {
            ValidationResult trialResult = validateIntRange("stdp_trials", params.at("stdp_trials"), 1, 1000, testName);
            if (!trialResult.valid) {
                for (const auto& error : trialResult.errors) {
                    result.addError(error);
                }
                for (const auto& warning : trialResult.warnings) {
                    result.addWarning(warning);
                }
            }
        }
    }
    
    return result;
}

ValidationResult InputValidator::validateExecutionMode(const std::string& mode) const {
    ValidationResult result;
    
    // Valid execution modes
    std::vector<std::string> validModes = {"sequential", "parallel", "dependent"};
    
    if (std::find(validModes.begin(), validModes.end(), mode) == validModes.end()) {
        result.addError("Invalid execution mode: " + mode + ". Valid modes: sequential, parallel, dependent");
    }
    
    return result;
}

ValidationResult InputValidator::validateDependencies(const std::map<std::string, std::string>& params) const {
    ValidationResult result;
    
    // Check parameter dependencies
    if (params.find("plasticity_steps") != params.end() && 
        params.find("enable_plasticity") == params.end()) {
        result.addWarning("plasticity_steps specified without enable_plasticity - behavior may be unexpected");
    }
    
    if (params.find("stdp_ltp_weight") != params.end() && 
        params.find("stdp_ltd_weight") == params.end()) {
        result.addWarning("stdp_ltp_weight specified without stdp_ltd_weight - unbalanced STDP");
    }
    
    return result;
}

bool InputValidator::isValidParameterType(const std::string& key, const std::string& value) const {
    // Basic type validation based on key patterns
    if (key.find("count") != std::string::npos || 
        key.find("steps") != std::string::npos ||
        key.find("trials") != std::string::npos) {
        try {
            std::stoi(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (key.find("rate") != std::string::npos ||
               key.find("weight") != std::string::npos ||
               key.find("time") != std::string::npos ||
               key == "learning_rate") {
        try {
            std::stod(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (key.find("enable_") != std::string::npos || 
               key.find("flag") != std::string::npos) {
        // Boolean values
        return value == "true" || value == "false" || value == "1" || value == "0" || value == "yes" || value == "no";
    } else if (key.find("path") != std::string::npos || 
               key.find("file") != std::string::npos) {
        // File paths - accept any non-empty string
        return !value.empty();
    } else if (key.find("name") != std::string::npos) {
        // Names - accept any non-empty string
        return !value.empty();
    }
    
    // Default: accept any non-empty string
    return !value.empty();
}

std::string InputValidator::getParameterType(const std::string& key) const {
    // Determine parameter type based on key patterns
    if (key.find("count") != std::string::npos || 
        key.find("steps") != std::string::npos ||
        key.find("trials") != std::string::npos) {
        return "integer";
    } else if (key.find("rate") != std::string::npos ||
               key.find("weight") != std::string::npos ||
               key.find("time") != std::string::npos ||
               key == "learning_rate") {
        return "float";
    } else if (key.find("enable_") != std::string::npos || 
               key.find("flag") != std::string::npos) {
        return "boolean";
    } else if (key.find("path") != std::string::npos || 
               key.find("file") != std::string::npos) {
        return "filepath";
    } else if (key.find("name") != std::string::npos) {
        return "name";
    }
    
    return "string";
}

ValidationResult InputValidator::validateAgainstSchema(const std::string& key, const std::string& value,
                                                      const std::string& testName) const {
    // Delegated to error handler
    return pImpl->errorHandler->validateInput(key, value, testName);
}

ValidationResult InputValidator::validateAgainstNeuralSchema(const std::map<std::string, std::string>& params) const {
    ValidationResult result;
    
    // Neural network schema validation
    if (params.find("neuron_count") != params.end()) {
        ValidationResult neuronResult = validateIntRange("neuron_count", params.at("neuron_count"), 1, 100000, "");
        if (!neuronResult.valid) {
            for (const auto& error : neuronResult.errors) {
                result.addError(error);
            }
            for (const auto& warning : neuronResult.warnings) {
                result.addWarning(warning);
            }
        }
    }
    
    return result;
}

ValidationResult InputValidator::validateAgainstPlasticitySchema(const std::map<std::string, std::string>& params) const {
    ValidationResult result;
    
    // Plasticity schema validation
    if (params.find("plasticity_steps") != params.end()) {
        ValidationResult stepResult = validateIntRange("plasticity_steps", params.at("plasticity_steps"), 1, 100000, "");
        if (!stepResult.valid) {
            for (const auto& error : stepResult.errors) {
                result.addError(error);
            }
            for (const auto& warning : stepResult.warnings) {
                result.addWarning(warning);
            }
        }
    }
    
    return result;
}

ValidationResult InputValidator::validateAgainstMemorySchema(const std::map<std::string, std::string>& params) const {
    ValidationResult result;
    
    // Memory schema validation
    if (params.find("memory_capacity") != params.end()) {
        ValidationResult capResult = validateIntRange("memory_capacity", params.at("memory_capacity"), 1, 1000000, "");
        if (!capResult.valid) {
            for (const auto& error : capResult.errors) {
                result.addError(error);
            }
            for (const auto& warning : capResult.warnings) {
                result.addWarning(warning);
            }
        }
    }
    
    return result;
}

ValidationResult InputValidator::validateAgainstPerformanceSchema(const std::map<std::string, std::string>& params) const {
    ValidationResult result;
    
    // Performance schema validation
    if (params.find("max_execution_time") != params.end()) {
        ValidationResult timeResult = validateIntRange("max_execution_time", params.at("max_execution_time"), 1, 86400, "");
        if (!timeResult.valid) {
            for (const auto& error : timeResult.errors) {
                result.addError(error);
            }
            for (const auto& warning : timeResult.warnings) {
                result.addWarning(warning);
            }
        }
    }
    
    return result;
}

} // namespace nlm
