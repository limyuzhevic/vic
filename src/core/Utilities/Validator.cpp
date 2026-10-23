// Error handling and validation improvements for NLM
// This module provides comprehensive error handling and validation utilities
// to improve robustness, debugging, and user experience

#include "core/Utilities/ErrorHandling.hpp"
#include <iostream>
#include <sstream>

using namespace nlm;

// Validation utility implementations
template<typename T>
bool Validator::isValid(const T& value, const T& min, const T& max) {
    return value >= min && value <= max;
}

bool Validator::isPositive(int64_t value) {
    return value > 0;
}

bool Validator::isNonNegative(int64_t value) {
    return value >= 0;
}

bool Validator::isPositive(float value) {
    return value > 0.0f;
}

bool Validator::isNonNegative(float value) {
    return value >= 0.0f;
}

bool Validator::isInRange(float value, float min, float max) {
    return value >= min && value <= max;
}

bool Validator::isNonEmpty(const std::string& str) {
    return !str.empty();
}

bool Validator::isNonEmpty(const std::vector<float>& vec) {
    return !vec.empty();
}

std::vector<std::string> Validator::validateConfig(const std::vector<std::string>& requiredKeys, 
                                                   const std::vector<std::string>& actualKeys) {
    std::vector<std::string> missingKeys;
    for (const auto& key : requiredKeys) {
        if (std::find(actualKeys.begin(), actualKeys.end(), key) == actualKeys.end()) {
            missingKeys.push_back(key);
        }
    }
    return missingKeys;
}

// Configuration validation methods for Config class
void Config::validate() const {
    // Validate simulation parameters
    if (!has("simulation_timestep") || getOr<double>("simulation_timestep", 0.001) <= 0.0) {
        ErrorReporter::logWarning("Invalid or missing simulation_timestep, using default value of 0.001");
    }
    
    if (!has("neuron_count") || getOr<int64_t>("neuron_count", 1000) <= 0) {
        ErrorReporter::logWarning("Invalid or missing neuron_count, using default value");
    }
    
    if (!has("region_count") || getOr<int64_t>("region_count", 1) <= 0) {
        ErrorReporter::logWarning("Invalid or missing region_count, using default value");
    }
    
    // Validate plasticity parameters
    if (has("plasticity.stdp.learning_rate") && getOr<float>("plasticity.stdp.learning_rate", 0.0f) < 0.0f) {
        ErrorReporter::logWarning("Invalid STDP learning_rate, must be non-negative");
    }
    
    // Validate neuromodulation parameters
    if (has("neuromod.dopamine.scale") && getOr<float>("neuromod.dopamine.scale", 1.0f) < 0.0f) {
        ErrorReporter::logWarning("Invalid dopamine.scale, must be non-negative");
    }
}

// Brain validation methods
bool Brain::validateConfiguration() const {
    std::vector<std::string> requiredKeys = {
        "simulation_timestep",
        "neuron_count", 
        "region_count"
    };
    
    // Get all configuration keys
    std::vector<std::string> configKeys = getConfig()->getKeys();
    
    // Validate required keys are present
    std::vector<std::string> missingKeys = Validator::validateConfig(requiredKeys, configKeys);
    
    if (!missingKeys.empty()) {
        std::stringstream errorMsg;
        errorMsg << "Missing required configuration keys: ";
        for (size_t i = 0; i < missingKeys.size(); ++i) {
            if (i > 0) errorMsg << ", ";
            errorMsg << missingKeys[i];
        }
        ErrorReporter::logError(errorMsg.str());
        return false;
    }
    
    // Validate parameter ranges
    double timestep = getConfig()->getOr<double>("simulation_timestep", 0.001);
    if (timestep <= 0.0 || timestep > 1.0) {
        ErrorReporter::logError("Invalid simulation_timestep: " + std::to_string(timestep) + ". Must be > 0.0 and <= 1.0");
        return false;
    }
    
    int64_t neuronCount = getConfig()->getOr<int64_t>("neuron_count", 1000);
    if (neuronCount <= 0 || neuronCount > 100000) {
        ErrorReporter::logError("Invalid neuron_count: " + std::to_string(neuronCount) + ". Must be > 0 and <= 100000");
        return false;
    }
    
    int64_t regionCount = getConfig()->getOr<int64_t>("region_count", 1);
    if (regionCount <= 0 || regionCount > 100) {
        ErrorReporter::logError("Invalid region_count: " + std::to_string(regionCount) + ". Must be > 0 and <= 100");
        return false;
    }
    
    return true;
}

// Safe configuration access with validation
float Config::getSafeFloat(const std::string& key, float defaultValue, float minValue, float maxValue) const {
    if (!has(key)) {
        ErrorReporter::logWarning("Configuration key \"" + key + "\" not found, using default value: " + std::to_string(defaultValue));
        return defaultValue;
    }
    
    float value = getOr<float>(key, defaultValue);
    
    if (!Validator::isInRange(value, minValue, maxValue)) {
        ErrorReporter::logWarning("Configuration key \"" + key + "\" value " + std::to_string(value) + " is out of range [" + std::to_string(minValue) + ", " + std::to_string(maxValue) + "], using default value: " + std::to_string(defaultValue));
        return defaultValue;
    }
    
    return value;
}

int64_t Config::getSafeInt(const std::string& key, int64_t defaultValue, int64_t minValue, int64_t maxValue) const {
    if (!has(key)) {
        ErrorReporter::logWarning("Configuration key \"" + key + "\" not found, using default value: " + std::to_string(defaultValue));
        return defaultValue;
    }
    
    int64_t value = getOr<int64_t>(key, defaultValue);
    
    if (!Validator::isInRange(value, minValue, maxValue)) {
        ErrorReporter::logWarning("Configuration key \"" + key + "\" value " + std::to_string(value) + " is out of range [" + std::to_string(minValue) + ", " + std::to_string(maxValue) + "], using default value: " + std::to_string(defaultValue));
        return defaultValue;
    }
    
    return value;
}

// Enhanced error reporting
void ErrorHandler::logErrorWithContext(const std::string& error, const std::string& context) {
    ErrorReporter::logError("Context: " + context + " - " + error);
}

void ErrorHandler::logMemoryError(const std::string& operation, size_t size) {
    ErrorReporter::logError("Memory allocation failed for " + operation + " of size " + std::to_string(size) + " bytes");
}

void ErrorHandler::logFileError(const std::string& operation, const std::string& filepath) {
    ErrorReporter::logError("File operation \"" + operation + "\" failed for file: " + filepath);
}

void ErrorHandler::logConfigurationError(const std::string& key, const std::string& expectedType) {
    ErrorReporter::logError("Configuration error: key \"" + key + "\" has invalid type or is missing. Expected type: " + expectedType);
}