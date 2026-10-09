#include "Errors/Errors.hpp"
#include "Logger/Logger.hpp"
#include <map>
#include <sstream>

namespace nlm {

// Initialize static maps
std::map<ErrorCode, std::string> ErrorReport::errorCodeDescriptions_ = {
    {ErrorCode::ConfigKeyNotFound, "Configuration key not found"},
    {ErrorCode::ConfigInvalidType, "Invalid configuration value type"},
    {ErrorCode::ConfigFileNotFound, "Configuration file not found"},
    {ErrorCode::ConfigParseError, "Error parsing configuration file"},
    
    {ErrorCode::NeuronIdInvalid, "Invalid neuron identifier"},
    {ErrorCode::SynapseIdInvalid, "Invalid synapse identifier"},
    {ErrorCode::NeuralRegionInvalid, "Invalid neural region"},
    {ErrorCode::InvalidMembranePotential, "Membrane potential outside valid range"},
    {ErrorCode::InvalidFiringRate, "Firing rate outside valid range"},
    {ErrorCode::RefractoryPeriodInvalid, "Refractory period outside valid range"},
    
    {ErrorCode::MemoryAllocationFailed, "Memory allocation failed"},
    {ErrorCode::InvalidMemoryReference, "Invalid memory reference"},
    
    {ErrorCode::InvalidTimeStep, "Invalid time step"},
    {ErrorCode::TimestampOutOfRange, "Timestamp out of valid range"},
    
    {ErrorCode::InvalidPhysicalParameters, "Invalid physical parameters"},
    
    {ErrorCode::ParameterOutOfRange, "Parameter value outside valid range"},
    {ErrorCode::NullPointerDetected, "Null pointer detected"},
    {ErrorCode::InvalidInputData, "Invalid input data"},
    
    {ErrorCode::SystemInitializationFailed, "System initialization failed"},
    {ErrorCode::ResourceUnavailable, "Required resource unavailable"},
    
    {ErrorCode::SimulationNotRunning, "Simulation not running"},
    {ErrorCode::InvalidSimulationState, "Invalid simulation state"},
    
    {ErrorCode::UnknownError, "Unknown error"}
};

std::map<ErrorCategory, std::string> ErrorReport::errorCategoryDescriptions_ = {
    {ErrorCategory::Configuration, "Configuration"},
    {ErrorCategory::Neural, "Neural Network"},
    {ErrorCategory::Memory, "Memory"},
    {ErrorCategory::Temporal, "Temporal"},
    {ErrorCategory::Physical, "Physical"},
    {ErrorCategory::Biological, "Biological"},
    {ErrorCategory::Simulation, "Simulation"},
    {ErrorCategory::FileIO, "File I/O"},
    {ErrorCategory::Validation, "Validation"},
    {ErrorCategory::System, "System"},
    {ErrorCategory::Development, "Development"}
};

void ErrorReport::reportError(const std::string& message, ErrorCode code, ErrorCategory category,
                             const std::string& file, int line,
                             const std::string& function) {
    std::ostringstream oss;
    oss << "Error " << static_cast<int>(code) << " [" << static_cast<int>(category) << "]:";
    if (!file.empty()) oss << " " << file;
    if (line > 0) oss << ":" << line;
    oss << " " << message;
    
    // Log using the existing logger
    Logger::getGlobal().error(oss.str());
}

void ErrorReport::logError(const std::string& message, ErrorCode code, ErrorCategory category,
                          const std::string& file, int line,
                          const std::string& function) {
    reportError(message, code, category, file, line, function);
}

std::string ErrorReport::getErrorCodeDescription(ErrorCode code) {
    auto it = errorCodeDescriptions_.find(code);
    return it != errorCodeDescriptions_.end() ? it->second : "Unknown error code";
}

std::string ErrorReport::getErrorCategoryDescription(ErrorCategory category) {
    auto it = errorCategoryDescriptions_.find(category);
    return it != errorCategoryDescriptions_.end() ? it->second : "Unknown category";
}

void Validator::validateNotNull(const void* ptr, const std::string& paramName, const std::string& file, int line) {
    if (ptr == nullptr) {
        throw NullPointerException(paramName, file, line);
    }
}

void Validator::validateRange(double value, double min, double max, const std::string& paramName, const std::string& file, int line) {
    if (value < min || value > max) {
        throw RangeValidationException(paramName, value, min, max, file, line);
    }
}

void Validator::validateRange(int64_t value, int64_t min, int64_t max, const std::string& paramName, const std::string& file, int line) {
    if (value < min || value > max) {
        // Cast to double for exception
        throw RangeValidationException(paramName, static_cast<double>(value), static_cast<double>(min), static_cast<double>(max), file, line);
    }
}

void Validator::validateRange(size_t value, size_t min, size_t max, const std::string& paramName, const std::string& file, int line) {
    if (value < min || value > max) {
        throw RangeValidationException(paramName, static_cast<double>(value), static_cast<double>(min), static_cast<double>(max), file, line);
    }
}

void Validator::validatePositive(const std::string& paramName, double value, const std::string& file, int line) {
    if (value <= 0.0) {
        throw RangeValidationException(paramName, value, 0.0, std::numeric_limits<double>::max(), file, line);
    }
}

void Validator::validateNonNegative(const std::string& paramName, int64_t value, const std::string& file, int line) {
    if (value < 0) {
        throw RangeValidationException(paramName, static_cast<double>(value), 0.0, static_cast<double>(std::numeric_limits<int64_t>::max()), file, line);
    }
}

void Validator::validateMembranePotential(const std::string& paramName, double potential, const std::string& file, int line) {
    const double MIN_POTENTIAL = -200.0; // Absolute refractory limit
    const double MAX_POTENTIAL = 100.0;  // High excitation limit
    validateRange(paramName, potential, MIN_POTENTIAL, MAX_POTENTIAL, file, line);
}

void Validator::validateFiringRate(const std::string& paramName, double rate, const std::string& file, int line) {
    const double MIN_RATE = 0.0;
    const double MAX_RATE = 10000.0; // 10 kHz maximum
    validateRange(paramName, rate, MIN_RATE, MAX_RATE, file, line);
}

void Validator::validateTimeStep(const std::string& paramName, double dt, const std::string& file, int line) {
    const double MIN_DT = 1e-9; // 1 nanosecond minimum
    const double MAX_DT = 1.0;  // 1 second maximum
    validateRange(paramName, dt, MIN_DT, MAX_DT, file, line);
}

void Validator::validateConfigKey(const std::string& key, const std::string& file, int line) {
    if (key.empty()) {
        throw ConfigValidationException("Configuration key cannot be empty", file, line);
    }
}

void Validator::validateConfigValue(const std::string& key, const std::string& value, const std::string& file, int line) {
    if (value.empty()) {
        throw ConfigValidationException("Configuration value cannot be empty for key: " + key, file, line);
    }
}

void Validator::validateArrayBounds(const void* ptr, size_t size, size_t maxSize, const std::string& paramName, const std::string& file, int line) {
    if (ptr == nullptr && size > 0) {
        throw NullPointerException(paramName, file, line);
    }
    if (size > maxSize) {
        throw RangeValidationException(paramName, static_cast<double>(size), 0.0, static_cast<double>(maxSize), file, line);
    }
}

void Validator::validateState(bool condition, const std::string& errorMessage, const std::string& file, int line) {
    if (!condition) {
        throw ValidationException(errorMessage, file, line);
    }
}

void Validator::validatePrecondition(bool condition, const std::string& errorMessage, const std::string& file, int line) {
    if (!condition) {
        throw ValidationException("Precondition violation: " + errorMessage, file, line);
    }
}

void Validator::validatePostcondition(bool condition, const std::string& errorMessage, const std::string& file, int line) {
    if (!condition) {
        throw ValidationException("Postcondition violation: " + errorMessage, file, line);
    }
}

} // namespace nlm
