#include "core/ErrorHandling/ErrorHandling.hpp"
#include <limits>

namespace nlm {

void ErrorHandler::validateParameter(const std::string& name, const T& value,
                                    const std::string& context,
                                    T min, T max) {
    if (value < min || value > max) {
        throw NLMError(NLMErrorCode::ParameterValidationFailed,
                     "Parameter '" + name + "' value " + std::to_string(value) +
                     " is outside valid range [" + std::to_string(min) + ", " +
                     std::to_string(max) + "]",
                     NLMErrorCategory::ValidationError,
                     context);
    }
}

void ErrorHandler::validateStringParameter(const std::string& name, const std::string& value,
                                          const std::string& context,
                                          size_t minLength, size_t maxLength) {
    if (value.length() < minLength || value.length() > maxLength) {
        throw NLMError(NLMErrorCode::ParameterValidationFailed,
                     "Parameter '" + name + "' length " + std::to_string(value.length()) +
                     " is outside valid range [" + std::to_string(minLength) + ", " +
                     std::to_string(maxLength) + "]",
                     NLMErrorCategory::ValidationError,
                     context);
    }
}

void ErrorHandler::validateConfigType(const std::string& name, const U& value,
                                     const std::string& context) {
    try {
        (void)std::get<T>(value);
    } catch (const std::bad_variant_access&) {
        throw NLMError(NLMErrorCode::TypeValidationFailed,
                     "Configuration parameter '" + name + "' has wrong type. "
                     "Expected " + typeName<T>() + ", got " + typeName<U>() + ".",
                     NLMErrorCategory::ValidationError,
                     context);
    }
}

T ErrorHandler::getConfig(const std::string& key, const std::shared_ptr<Config>& config,
                         const T& defaultValue, const std::string& context) {
    if (!config) {
        throw NLMError(NLMErrorCode::ConfigError,
                     "Configuration is null while accessing parameter '" + key + "'",
                     NLMErrorCategory::ConfigError,
                     context);
    }
    
    auto value = config->getOr<T>(key, defaultValue);
    
    // Validate the retrieved value
    validateParameter(key, value, context);
    
    return value;
}

std::string ErrorHandler::createRecoveryAction(NLMErrorCode code) {
    switch (code) {
        case NLMErrorCode::ConfigFileNotFound:
            return "Using default configuration values";
        case NLMErrorCode::InvalidConfigValue:
            return "Resetting to valid default value";
        case NLMErrorCode::MemoryAllocationFailed:
            return "Trying alternative memory allocation strategy";
        case NLMErrorCode::MemoryAccessViolation:
            return "Restoring from checkpoint if available";
        case NLMErrorCode::NetworkTimeout:
            return "Retry with increased timeout";
        case NLMErrorCode::SimulationStepFailed:
            return "Skipping problematic step and continuing";
        case NLMErrorCode::TypeValidationFailed:
            return "Using type conversion if possible";
        default:
            return "Using system defaults";
    }
}

} // namespace nlm

// Global error handler implementation
namespace nlm {
    ErrorHandler& ErrorHandler::getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    void ErrorHandler::handleError(const NLMError& error, bool throwException) {
        logError(error);
        
        if (throwException) {
            throw error;
        }
    }
    
    void ErrorHandler::handleWarning(const std::string& message, const std::string& context) {
        logWarning(message, context);
    }
    
    void ErrorHandler::handleInfo(const std::string& message, const std::string& context) {
        logInfo(message, context);
    }
    
    // Template method implementations
    template<typename T>
    void ErrorHandler::validateParameter(const std::string& name, const T& value,
                                        const std::string& context,
                                        T min, T max) {
        if (value < min || value > max) {
            throw NLMError(NLMErrorCode::ParameterValidationFailed,
                         "Parameter '" + name + "' value " + std::to_string(value) +
                         " is outside valid range [" + std::to_string(min) + ", " +
                         std::to_string(max) + "]",
                         NLMErrorCategory::ValidationError,
                         context);
        }
    }
    
    // Explicit template instantiations for commonly used types
    template void ErrorHandler::validateParameter<int>(const std::string&, const int&, const std::string&, int, int);
    template void ErrorHandler::validateParameter<int64_t>(const std::string&, const int64_t&, const std::string&, int64_t, int64_t);
    template void ErrorHandler::validateParameter<double>(const std::string&, const double&, const std::string&, double, double);
    template void ErrorHandler::validateParameter<float>(const std::string&, const float&, const std::string&, float, float);
    template void ErrorHandler::validateParameter<bool>(const std::string&, const bool&, const std::string&, bool, bool);
    template void ErrorHandler::validateParameter<size_t>(const std::string&, const size_t&, const std::string&, size_t, size_t);
    
    template void ErrorHandler::getConfig<int>(const std::string&, const std::shared_ptr<Config>&, const int&, const std::string&);
    template void ErrorHandler::getConfig<int64_t>(const std::string&, const std::shared_ptr<Config>&, const int64_t&, const std::string&);
    template void ErrorHandler::getConfig<double>(const std::string&, const std::shared_ptr<Config>&, const double&, const std::string&);
    template void ErrorHandler::getConfig<float>(const std::string&, const std::shared_ptr<Config>&, const float&, const std::string&);
    template void ErrorHandler::getConfig<bool>(const std::string&, const std::shared_ptr<Config>&, const bool&, const std::string&);
    template void ErrorHandler::getConfig<size_t>(const std::string&, const std::shared_ptr<Config>&, const size_t&, const std::string&);
    
} // namespace nlm

#include "core/ErrorHandling/ErrorHandling.cpp"

namespace nlm {
    ErrorHandler& getErrorHandler() {
        return ErrorHandler::getInstance();
    }
} // namespace nlm

// Error handling implementation
ErrorHandler::ErrorHandler() {
    logger_ = spdlog::get("nlm");
    if (!logger_) {
        logger_ = spdlog::stdout_color_mt("nlm");
        logger_->set_level(spdlog::level::info);
    }
}

void ErrorHandler::logError(const NLMError& error) {
    if (logger_) {
        logger_->error(error.toString());
    }
}

void ErrorHandler::logWarning(const std::string& message, const std::string& context) {
    if (logger_) {
        std::string fullMessage = message;
        if (!context.empty()) {
            fullMessage += " (Context: " + context + ")";
        }
        logger_->warn(fullMessage);
    }
}

void ErrorHandler::logInfo(const std::string& message, const std::string& context) {
    if (logger_) {
        std::string fullMessage = message;
        if (!context.empty()) {
            fullMessage += " (Context: " + context + ")";
        }
        logger_->info(fullMessage);
    }
}

// Template method implementations
namespace nlm {
    template<typename T>
    void ErrorHandler::validateParameter(const std::string& name, const T& value,
                                        const std::string& context,
                                        T min, T max) {
        if (value < min || value > max) {
            throw NLMError(NLMErrorCode::ParameterValidationFailed,
                         "Parameter '" + name + "' value " + std::to_string(value) +
                         " is outside valid range [" + std::to_string(min) + ", " +
                         std::to_string(max) + "]",
                         NLMErrorCategory::ValidationError,
                         context);
        }
    }
}

// Explicit template instantiations
namespace nlm {
    template void ErrorHandler::validateParameter<int>(const std::string&, const int&, const std::string&, int, int);
    template void ErrorHandler::validateParameter<int64_t>(const std::string&, const int64_t&, const std::string&, int64_t, int64_t);
    template void ErrorHandler::validateParameter<double>(const std::string&, const double&, const std::string&, double, double);
    template void ErrorHandler::validateParameter<float>(const std::string&, const float&, const std::string&, float, float);
    template void ErrorHandler::validateParameter<bool>(const std::string&, const bool&, const std::string&, bool, bool);
    template void ErrorHandler::validateParameter<size_t>(const std::string&, const size_t&, const std::string&, size_t, size_t);
}