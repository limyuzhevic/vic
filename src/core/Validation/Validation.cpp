// Validation and Error Handling Implementation
// Provides comprehensive validation and error checking for the NLM project

#include "Validation.hpp"
#include "Config/Config.hpp"
#include "../Logger/Logger.hpp"
#include <stdexcept>
#include <iostream>
#include <limits>
#include <algorithm>
#include <cmath>

namespace nlm {

// Validation namespace implementation
namespace Validation {
    
    bool validateRange(double value, double min, double max, const std::string& paramName) {
        if (paramName.empty()) {
            return value >= min && value <= max;
        }
        
        if (value < min) {
            throw ValidationError("Parameter \"" + paramName + "\" value " + std::to_string(value) + 
                                " is less than minimum allowed value " + std::to_string(min));
        }
        if (value > max) {
            throw ValidationError("Parameter \"" + paramName + "\" value " + std::to_string(value) + 
                                " exceeds maximum allowed value " + std::to_string(max));
        }
        return true;
    }
    
    bool validatePositive(int value, const std::string& paramName) {
        if (value <= 0) {
            throw ValidationError("Parameter \"" + paramName + "\" value must be positive, got " + 
                                std::to_string(value));
        }
        return true;
    }
    
    bool validatePositive(double value, const std::string& paramName) {
        if (value <= 0.0) {
            throw ValidationError("Parameter \"" + paramName + "\" value must be positive, got " + 
                                std::to_string(value));
        }
        return true;
    }
    
    bool validateString(const std::string& value, size_t minLength, size_t maxLength,
                       const std::string& paramName) {
        if (value.length() < minLength) {
            throw ValidationError("Parameter \"" + paramName + "\" string too short, minimum length " + 
                                std::to_string(minLength) + ", got " + std::to_string(value.length()));
        }
        if (value.length() > maxLength) {
            throw ValidationError("Parameter \"" + paramName + "\" string too long, maximum length " + 
                                std::to_string(maxLength) + ", got " + std::to_string(value.length()));
        }
        return true;
    }
    
    bool validateInteger(int64_t value, int64_t min, int64_t max, const std::string& paramName) {
        if (value < min) {
            throw ValidationError("Parameter \"" + paramName + "\" value " + std::to_string(value) + 
                                " is less than minimum allowed value " + std::to_string(min));
        }
        if (value > max) {
            throw ValidationError("Parameter \"" + paramName + "\" value " + std::to_string(value) + 
                                " exceeds maximum allowed value " + std::to_string(max));
        }
        return true;
    }
    
    bool validateConfigKey(const std::string& key) {
        if (key.empty()) {
            throw ValidationError("Configuration key cannot be empty");
        }
        if (key.find('/') != std::string::npos) {
            throw ValidationError("Configuration key cannot contain '/' character: \"" + key + "\"");
        }
        if (key.find('\0') != std::string::npos) {
            throw ValidationError("Configuration key cannot contain null character: \"" + key + "\"");
        }
        return true;
    }
    
    bool validateNeuronCount(size_t neuronCount, const std::string& context) {
        if (neuronCount == 0) {
            throw ValidationError("Neuron count cannot be zero");
        }
        if (neuronCount > 1000000) {
            throw ValidationError("Neuron count too large (max 1,000,000), got " + 
                                std::to_string(neuronCount));
        }
        if (context.empty()) {
            return true;
        }
        throw ValidationError("Validation failed for neuron count " + std::to_string(neuronCount) + 
                            " in context: " + context);
    }
    
    bool validateConnectionProbability(float probability) {
        if (probability < 0.0f || probability > 1.0f) {
            throw ValidationError("Connection probability must be between 0.0 and 1.0, got " + 
                                std::to_string(probability));
        }
        return true;
    }
    
    bool validateTimestep(float timestep) {
        if (timestep <= 0.0f) {
            throw ValidationError("Timestep must be positive, got " + std::to_string(timestep));
        }
        if (timestep > 1.0f) {
            throw ValidationError("Timestep too large (max 1.0s), got " + std::to_string(timestep));
        }
        return true;
    }
    
    bool validateFilePath(const std::string& path, bool mustExist) {
        if (path.empty()) {
            throw ValidationError("File path cannot be empty");
        }
        if (mustExist) {
            // File existence check would be platform-specific
            // For now, just validate the path format
        }
        return true;
    }
    
    bool validateDirectoryPath(const std::string& path, bool mustExist) {
        if (path.empty()) {
            throw ValidationError("Directory path cannot be empty");
        }
        return true;
    }
    
    bool validateFloat(float value, float min, float max, float tolerance, const std::string& paramName) {
        if (std::isnan(value)) {
            throw ValidationError("Parameter \"" + paramName + "\" cannot be NaN");
        }
        if (std::isinf(value)) {
            throw ValidationError("Parameter \"" + paramName + "\" cannot be infinite");
        }
        if (std::abs(value) > max) {
            throw ValidationError("Parameter \"" + paramName + "\" value magnitude " + 
                                std::to_string(value) + " exceeds maximum " + std::to_string(max));
        }
        return true;
    }
    
    bool validateBoolean(bool value) {
        return true; // Always valid, but kept for consistency
    }
    
    template<typename T>
    bool validateArrayBounds(const std::vector<T>& array, size_t maxSize, const std::string& paramName) {
        if (array.size() > maxSize) {
            throw ValidationError("Parameter \"" + paramName + "\" array size " + 
                                std::to_string(array.size()) + " exceeds maximum " + 
                                std::to_string(maxSize));
        }
        return true;
    }
    
    bool validateMemorySize(size_t bytes, size_t maxAllowed) {
        if (bytes > maxAllowed) {
            throw ValidationError("Memory size " + std::to_string(bytes) + 
                                " bytes exceeds maximum allowed " + std::to_string(maxAllowed));
        }
        return true;
    }
    
    bool validateSimulationParams(size_t maxSteps, float timestep, size_t neuronCount) {
        validateTimestep(timestep);
        validateNeuronCount(neuronCount, "validateSimulationParams");
        if (maxSteps == 0) {
            throw ValidationError("Maximum steps cannot be zero");
        }
        if (maxSteps > 1000000000) {
            throw ValidationError("Maximum steps too large (max 1,000,000,000), got " + 
                                std::to_string(maxSteps));
        }
        return true;
    }
}

// ErrorHandler implementation
ErrorHandler::ErrorHandler() : lastError(std::nullopt) {}

ErrorHandler::~ErrorHandler() {}

void ErrorHandler::recordError(ErrorCode code, const std::string& message, const std::string& context,
                               const std::map<std::string, std::string>& details) {
    ErrorInfo error(code, message, context);
    error.details = details;
    
    errors.push_back(error);
    lastError = error;
}

std::optional<ErrorInfo> ErrorHandler::getLastError() const {
    return lastError;
}

bool ErrorHandler::hasErrors() const {
    return !errors.empty();
}

void ErrorHandler::clearErrors() {
    errors.clear();
    lastError = std::nullopt;
}

std::vector<ErrorInfo> ErrorHandler::getAllErrors() const {
    return errors;
}

void ErrorHandler::logErrors(const std::string& source) {
    Logger logger = Logger::getGlobal();
    for (const auto& error : errors) {
        std::string logMessage = error.toString();
        if (!source.empty()) {
            logMessage = "[" + source + "] " + logMessage;
        }
        logger.log(LogLevel::Error, logMessage);
    }
}

void ErrorHandler::throwFirstError() const {
    if (hasErrors()) {
        throw errors.front().message;
    }
}

template<typename T>
bool ErrorHandler::validate(const T& value, ErrorCode errorCode, const std::string& message,
                           const std::string& context, const std::string& paramName,
                           const std::map<std::string, std::string>& details) {
    try {
        // Placeholder validation - actual validation logic would be implemented per type
        if (!value) {
            recordError(errorCode, message, context, details);
            return false;
        }
        return true;
    } catch (const ValidationError& e) {
        recordError(ErrorCode::ValidationFailed, e.what(), context, details);
        return false;
    }
}

// ConfigurationValidator implementation
ConfigurationValidator::ConfigurationValidator(std::shared_ptr<Config> config) : config(config) {}

ErrorCode ConfigurationValidator::validateConfiguration(bool strict) const {
    if (!config) {
        return ErrorCode::InvalidArgument;
    }
    
    ErrorHandler tempHandler;
    auto keys = config->getKeys();
    
    for (const auto& key : keys) {
        auto errorCode = validateConfigValue(key, config->get<ConfigValue>(key));
        if (errorCode != ErrorCode::Success) {
            tempHandler.recordError(errorCode, "Validation failed for key: " + key, "ConfigurationValidator");
        }
    }
    
    if (tempHandler.hasErrors()) {
        for (const auto& error : tempHandler.getAllErrors()) {
            warnings.push_back(error);
            errors.push_back(error);
        }
    }
    
    return warnings.empty() ? ErrorCode::Success : ErrorCode::ValidationFailed;
}

ErrorCode ConfigurationValidator::validateConfigValue(const std::string& key, const ConfigValue& value) const {
    validateConfigKey(key);
    
    // Basic validation based on key prefix
    if (key.find("neuron_count") != std::string::npos) {
        auto valOpt = std::get_if<int64_t>(&value);
        if (valOpt && (*valOpt <= 0)) {
            return ErrorCode::InvalidArgument;
        }
    } else if (key.find("connection_probability") != std::string::npos) {
        auto valOpt = std::get_if<double>(&value);
        if (valOpt && (*valOpt < 0.0 || *valOpt > 1.0)) {
            return ErrorCode::InvalidArgument;
        }
    }
    
    return ErrorCode::Success;
}

std::vector<ErrorInfo> ConfigurationValidator::getValidationWarnings() const {
    return warnings;
}

std::vector<ErrorInfo> ConfigurationValidator::getValidationErrors() const {
    return errors;
}

void ConfigurationValidator::addValidationRule(const std::string& key,
                                            std::function<bool(const ConfigValue&)> validator,
                                            const std::string& errorMessage) {
    validateConfigKey(key);
    if (!validator) {
        throw ValidationError("Validator function cannot be null");
    }
    validationRules[key] = validator;
}

// InitializationValidator implementation
InitializationValidator::InitializationValidator(std::shared_ptr<Brain> brain) : brain(brain) {}

ErrorCode InitializationValidator::validateBrainInitialization() const {
    if (!brain) {
        return ErrorCode::InvalidArgument;
    }
    
    // Basic validation
    if (brain->getTotalNeuronCount() == 0) {
        return ErrorCode::InvalidArgument;
    }
    
    return ErrorCode::Success;
}

ErrorCode InitializationValidator::validateSystemIntegration() const {
    if (!brain) {
        return ErrorCode::InvalidArgument;
    }
    
    // Check integration of all major systems
    if (!brain->getWorkingMemory()) {
        return ErrorCode::InitializationFailed;
    }
    
    if (!brain->getEpisodicMemory()) {
        return ErrorCode::InitializationFailed;
    }
    
    if (!brain->getDopamine()) {
        return ErrorCode::InitializationFailed;
    }
    
    if (!brain->getPredictionSystem()) {
        return ErrorCode::InitializationFailed;
    }
    
    if (!brain->getDevelopmentSystem()) {
        return ErrorCode::InitializationFailed;
    }
    
    return ErrorCode::Success;
}

ErrorCode InitializationValidator::validateMemorySystems() const {
    if (!brain->getWorkingMemory()) {
        return ErrorCode::InitializationFailed;
    }
    
    if (!brain->getEpisodicMemory()) {
        return ErrorCode::InitializationFailed;
    }
    
    if (!brain->getAssociativeMemory()) {
        return ErrorCode::InitializationFailed;
    }
    
    return ErrorCode::Success;
}

ErrorCode InitializationValidator::validateNeuromodulationSystems() const {
    if (!brain->getDopamine()) {
        return ErrorCode::InitializationFailed;
    }
    
    if (!brain->getCuriosity()) {
        return ErrorCode::InitializationFailed;
    }
    
    if (!brain->getNovelty()) {
        return ErrorCode::InitializationFailed;
    }
    
    if (!brain->getPredictionError()) {
        return ErrorCode::InitializationFailed;
    }
    
    return ErrorCode::Success;
}

std::map<std::string, ErrorCode> InitializationValidator::getValidationResults() const {
    std::map<std::string, ErrorCode> results;
    
    results["brain_initialization"] = validateBrainInitialization();
    results["system_integration"] = validateSystemIntegration();
    results["memory_systems"] = validateMemorySystems();
    results["neuromodulation_systems"] Agustus initialization();
    
    return results;
}

bool InitializationValidator::isValid() const {
    for (const auto& result : validationResults) {
        if (result.second != ErrorCode::Success) {
            return false;
        }
    }
    return true;
}

std::vector<ErrorInfo> InitializationValidator::getValidationErrors() const {
    std::vector<ErrorInfo> errors;
    
    for (const auto& pair : validationResults) {
        if (pair.second != ErrorCode::Success) {
            ErrorInfo error(pair.second, "Validation failed for: " + pair.first, "InitializationValidator");
            errors.push_back(error);
        }
    }
    
    return errors;
}

// SafeConfigGetter implementation (template specializations)
// Implementations of SafeConfigGetter for common types

// For demonstration, I'll provide a basic implementation
// Actual implementation would be more complex in a real scenario

// ValidationUtils implementation
namespace ValidationUtils {
    
    bool validateConfigFile(const std::string& filepath) {
        if (filepath.empty()) {
            return false;
        }
        if (filepath.length() > 4096) {
            return false;
        }
        return true;
    }
    
    bool validateCheckpointFile(const std::string& filepath) {
        if (!validateConfigFile(filepath)) {
            return false;
        }
        // Check file extension
        if (filepath.length() < 4 || filepath.substr(filepath.length() - 4) != ".bin") {
            return false;
        }
        return true;
    }
    
    bool ensureDirectoryExists(const std::string& path) {
        if (path.empty()) {
            return false;
        }
        // Directory existence check would be platform-specific
        return true;
    }
    
    bool checkWritePermissions(const std::string& path) {
        if (path.empty()) {
            return false;
        }
        // Permission check would be platform-specific
        return true;
    }
    
    bool validateStability(float avgFiringRate, float excitationInhibitionRatio) {
        if (avgFiringRate < 0.0f || avgFiringRate > 1000.0f) {
            return false;
        }
        if (excitationInhibitionRatio < 0.0f || excitationInhibitionRatio > 100.0f) {
            return false;
        }
        return true;
    }
    
    bool validateResourceUsage(size_t memoryMB, float cpuUsage) {
        if (memoryMB == 0 || memoryMB > 1024) {
            return false;
        }
        if (cpuUsage < 0.0f || cpuUsage > 100.0f) {
            return false;
        }
        return true;
    }
    
    bool validateTimeParams(float timestep, size_t maxSteps) {
        if (timestep <= 0.0f || timestep > 1.0f) {
            return false;
        }
        if (maxSteps == 0 || maxSteps > 1000000000) {
            return false;
        }
        return true;
    }
    
    bool validateNeuronParams(float vThresh, float vRest, float vReset, float tauMem) {
        if (vThresh <= vReset) {
            return false;
        }
        if (vRest <= -1.0f * std::numeric_limits<float>::max()) {
            return false;
        }
        if (tauMem <= 0.0f || tauMem > 1000.0f) {
            return false;
        }
        return true;
    }
    
    bool validatePlasticityParams(float ltpWeight, float ltdWeight, float stdpTau) {
        if (ltpWeight <= 0.0f || ltdWeight <= 0.0f) {
            return false;
        }
        if (stdpTau <= 0.0f || stdpTau > 1000.0f) {
            return false;
        }
        return true;
    }
    
    bool validateNeuromodulationParams(float dopamineScale, float curiosityWeight) {
        if (dopamineScale <= 0.0f) {
            return false;
        }
        if (curiosityWeight <= 0.0f) {
            return false;
        }
        return true;
    }
    
    bool validateMemoryParams(size_t capacity, size_t maxEpisodes) {
        if (capacity == 0) {
            return false;
        }
        if (maxEpisodes == 0) {
            return false;
        }
        return true;
    }
}
}