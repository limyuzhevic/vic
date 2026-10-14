#include "ValidationUtils.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <stdexcept>
#include <string>
#include <sstream>
#include <algorithm>

namespace nlm {

// Validation implementation for Config class
template<typename T>
T Config::validateAndGet(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (!value.has_value()) {
        NLM_THROW_ERROR(ConfigValidationError, "Configuration key not found: " + key);
    }
    return value.value();
}

// Enhanced set method with validation
void Config::setValidated(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (!StringValidator::isValidKey(key)) {
        NLM_THROW_ERROR(ConfigValidationError, "Invalid configuration key: '" + key + "'");
    }
    
    // Validate value
    if (!StringValidator::isValidConfigValue(value)) {
        NLM_THROW_ERROR(ConfigValidationError, "Invalid configuration value for key: '" + key + "'");
    }
    
    set(key, value, source);
}

// Enhanced loadFromFile with better error handling
bool Config::loadFromFile(const std::string& filepath) {
    // Validate file path
    if (!StringValidator::isValidFilePath(filepath)) {
        NLM_THROW_ERROR(ConfigFileError, "Invalid file path: '" + filepath + "'");
    }
    
    try {
        return Config::loadFromFile(filepath);
    } catch (const std::bad_alloc&) {
        NLM_THROW_ERROR(MemoryAllocationError, "Memory allocation failed while loading config from file: '" + filepath + "'");
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ConfigFileError, "Failed to load config from file '" + filepath + "': " + e.what());
    } catch (...) {
        NLM_THROW_ERROR(ConfigFileError, "Unknown error loading config from file: '" + filepath + "'");
    }
}

// Enhanced saveToFile with better error handling
bool Config::saveToFile(const std::string& filepath) const {
    // Validate file path
    if (!StringValidator::isValidFilePath(filepath)) {
        NLM_THROW_ERROR(ConfigFileError, "Invalid file path: '" + filepath + "'");
    }
    
    try {
        return Config::saveToFile(filepath);
    } catch (const std::bad_alloc&) {
        NLM_THROW_ERROR(MemoryAllocationError, "Memory allocation failed while saving config to file: '" + filepath + "'");
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ConfigFileError, "Failed to save config to file '" + filepath + "': " + e.what());
    } catch (...) {
        NLM_THROW_ERROR(ConfigFileError, "Unknown error saving config to file: '" + filepath + "'");
    }
}

// Validation method implementation
bool Config::validate() const {
    bool isValid = true;
    
    for (const auto& entry : pImpl->entries) {
        try {
            std::visit([this, &isValid, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                
                // Type-specific validation
                if constexpr (std::is_same_v<T, int>) {
                    if (!ValidationUtils::validateRange(arg, INT_MIN, INT_MAX)) {
                        NLM_THROW_ERROR(ConfigValidationError, 
                            "Invalid integer value for key '" + entry.key + "': " + std::to_string(arg));
                        isValid = false;
                    }
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    if (!ValidationUtils::validateRange(arg, LLONG_MIN, LLONG_MAX)) {
                        NLM_THROW_ERROR(ConfigValidationError, 
                            "Invalid int64 value for key '" + entry.key + "': " + std::to_string(arg));
                        isValid = false;
                    }
                } else if constexpr (std::is_same_v<T, double>) {
                    if (!ValidationUtils::validateRange(arg, -std::numeric_limits<double>::max(), 
                                                        std::numeric_limits<double>::max())) {
                        NLM_THROW_ERROR(ConfigValidationError, 
                            "Invalid double value for key '" + entry.key + "': " + std::to_string(arg));
                        isValid = false;
                    }
                } else if constexpr (std::is_same_v<T, bool>) {
                    // Boolean values are always valid
                    return;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    // String validation - check for null bytes
                    if (arg.find('\0') != std::string::npos) {
                        NLM_THROW_ERROR(ConfigValidationError, 
                            "String value for key '" + entry.key + "' contains null bytes");
                        isValid = false;
                    }
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (!ValidationUtils::validateRange(arg[i], INT_MIN, INT_MAX)) {
                            NLM_THROW_ERROR(ConfigValidationError, 
                                "Invalid integer in vector for key '" + entry.key + "' at index " + 
                                std::to_string(i) + ": " + std::to_string(arg[i]));
                            isValid = false;
                        }
                    }
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (!ValidationUtils::validateRange(arg[i], -std::numeric_limits<double>::max(), 
                                                            std::numeric_limits<double>::max())) {
                            NLM_THROW_ERROR(ConfigValidationError, 
                                "Invalid double in vector for key '" + entry.key + "' at index " + 
                                std::to_string(i) + ": " + std::to_string(arg[i]));
                            isValid = false;
                        }
                    }
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (arg[i].find('\0') != std::string::npos) {
                            NLM_THROW_ERROR(ConfigValidationError, 
                                "String in vector for key '" + entry.key + "' at index " + 
                                std::to_string(i) + " contains null bytes");
                            isValid = false;
                        }
                    }
                }
            }, entry.value);
        } catch (const NLMError&) {
            // Re-throw NLM errors
            throw;
        } catch (const std::exception& e) {
            NLM_THROW_ERROR(ConfigValidationError, 
                "Validation error for key '" + entry.key + "': " + e.what());
            isValid = false;
        } catch (...) {
            NLM_THROW_ERROR(ConfigValidationError, 
                "Unknown validation error for key '" + entry.key + "'");
            isValid = false;
        }
    }
    
    return isValid;
}

// Brain validation implementations
bool Brain::validateInitialization() const {
    if (!pImpl || !pImpl->config) {
        NLM_THROW_ERROR(BrainInitializationError, "Brain not properly initialized - missing configuration");
        return false;
    }
    
    if (pImpl->regions.empty()) {
        NLM_THROW_ERROR(BrainValidationError, "Brain initialization failed - no regions created");
        return false;
    }
    
    return true;
}

void Brain::validateSystemConnections() const {
    // Check if memory systems are accessible
    if (pImpl->memorySystem && !pImpl->memorySystem->isInitialized()) {
        NLM_THROW_ERROR(MemorySystemError, "Memory system not properly initialized");
    }
    
    // Check prediction system
    if (pImpl->predictionSystem && !pImpl->predictionSystem->isReady()) {
        NLM_THROW_ERROR(PredictionSystemError, "Prediction system not properly initialized");
    }
    
    // Check cognition systems
    if (pImpl->neuralPlanner && !pImpl->neuralPlanner->isInitialized()) {
        NLM_THROW_ERROR(CognitionSystemError, "Neural planner not properly initialized");
    }
    
    if (pImpl->conceptFormation && !pImpl->conceptFormation->isInitialized()) {
        NLM_THROW_ERROR(CognitionSystemError, "Concept formation system not properly initialized");
    }
    
    // Check neuromodulation systems
    if (pImpl->dopamine && !pImpl->dopamine->isInitialized()) {
        NLM_THROW_ERROR(NeuromodulationSystemError, "Dopamine system not properly initialized");
    }
    
    if (pImpl->curiosity && !pImpl->curiosity->isInitialized()) {
        NLM_THROW_ERROR(NeuromodulationSystemError, "Curiosity system not properly initialized");
    }
}

// Helper functions for validation
template<typename T>
T validateParameter(const T& value, const std::string& paramName,
                   const std::function<bool(const T&)>& validator,
                   const std::string& errorMessage) {
    if (!validator(value)) {
        NLM_THROW_ERROR(NLMError, errorMessage + " for parameter: " + paramName);
    }
    return value;
}

} // namespace nlm