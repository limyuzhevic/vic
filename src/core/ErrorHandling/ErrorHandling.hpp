#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include <string>
#include <stdexcept>
#include <system_error>
#include <spdlog/spdlog.h>
#include <sstream>

namespace nlm {

// NLM Error Categories
enum class NLMErrorCategory {
    ConfigError,
    MemoryError,
    NetworkError,
    PlasticityError,
    SimulationError,
    IntegrationError,
    ValidationError,
    UnknownError
};

// NLM Error Code
enum class NLMErrorCode {
    // Configuration errors
    ConfigFileNotFound = 1000,
    InvalidConfigValue = 1001,
    MissingRequiredConfig = 1002,
    InvalidConfigFormat = 1003,
    
    // Memory system errors
    MemoryAllocationFailed = 2000,
    MemoryAccessViolation = 2001,
    MemoryCorruptionDetected = 2002,
    MemoryLimitExceeded = 2003,
    
    // Network/synaptic errors
    InvalidConnection = 3000,
    SynapsisFailure = 3001,
    NetworkTimeout = 3002,
    
    // Plasticity errors
    PlasticityUpdateFailed = 4000,
    STDPError =4001,
    HebbianError =4002,
    StructuralPlasticityError =4003,
    
    // Simulation errors
    SimulationStepFailed = 5000,
    NeuronError =5001,
    SpikeEventError =5002,
    TimeStepError =5003,
    
    // Integration errors
    IntegrationFailed = 6000,
    SystemNotInitialized =6001,
    CrossSystemError =6002,
    
    // Validation errors
    ParameterValidationFailed = 7000,
    RangeValidationFailed =7001,
    TypeValidationFailed =7002,
    
    // Generic errors
    UnknownError = 9999,
    NotImplemented = 10000
};

class NLMError : public std::runtime_error {
public:
    NLMError(NLMErrorCode code, const std::string& message, 
             NLMErrorCategory category = NLMErrorCategory::UnknownError,
             const std::string& context = "")
        : std::runtime_error(formatMessage(code, message, category, context)),
          code_(code),
          category_(category),
          context_(context),
          timestamp_(getCurrentTimestamp()) {}

    NLMErrorCode getCode() const { return code_; }
    NLMErrorCategory getCategory() const { return category_; }
    const std::string& getContext() const { return context_; }
    const std::string& getTimestamp() const { return timestamp_; }
    
    // Convert error to user-friendly message
    std::string toString() const {
        std::ostringstream oss;
        oss << "[NLM Error " << static_cast<int>(code_) << "] ";
        oss << std::runtime_error::what();
        oss << " (Context: " << context_ << ")";
        return oss.str();
    }
    
    // Check if error is recoverable
    bool isRecoverable() const {
        switch (code_) {
            case NLMErrorCode::ConfigFileNotFound:
            case NLMErrorCode::MemoryAllocationFailed:
            case NLMErrorCode::NetworkTimeout:
                return false;
            case NLMErrorCode::InvalidConfigValue:
            case NLMErrorCode::MemoryAccessViolation:
            case NLMErrorCode::SimulationStepFailed:
                return true; // Can recover with defaults
            default:
                return true;
        }
    }

private:
    std::string formatMessage(NLMErrorCode code, const std::string& message,
                             NLMErrorCategory category, const std::string& context) {
        std::ostringstream oss;
        oss << "NLM " << errorCategoryToString(category) << " (Code: " << static_cast<int>(code_) << "): ";
        oss << message;
        if (!context.empty()) {
            oss << " | Context: " << context;
        }
        return oss.str();
    }
    
    std::string errorCategoryToString(NLMErrorCategory category) {
        switch (category) {
            case NLMErrorCategory::ConfigError: return "Config";
            case NLMErrorCategory::MemoryError: return "Memory";
            case NLMErrorCategory::NetworkError: return "Network";
            case NLMErrorCategory::PlasticityError: return "Plasticity";
            case NLMErrorCategory::SimulationError: return "Simulation";
            case NLMErrorCategory::IntegrationError: return "Integration";
            case NLMErrorCategory::ValidationError: return "Validation";
            case NLMErrorCategory::UnknownError: return "Unknown";
            default: return "Unknown";
        }
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    NLMErrorCode code_;
    NLMErrorCategory category_;
    std::string context_;
    std::string timestamp_;
};

class ErrorHandler {
public:
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    // Handle error with different levels of severity
    void handleError(const NLMError& error, bool throwException = true) {
        logError(error);
        
        if (throwException) {
            throw error;
        }
    }
    
    void handleWarning(const std::string& message, const std::string& context = "") {
        logWarning(message, context);
    }
    
    void handleInfo(const std::string& message, const std::string& context = "") {
        logInfo(message, context);
    }
    
    // Validate parameters with detailed error messages
    template<typename T>
    void validateParameter(const std::string& name, const T& value,
                          const std::string& context = "",
                          T min = std::numeric_limits<T>::lowest(),
                          T max = std::numeric_limits<T>::max()) {
        if (value < min || value > max) {
            throw NLMError(NLMErrorCode::ParameterValidationFailed,
                         "Parameter '" + name + "' value " + std::to_string(value) +
                         " is outside valid range [" + std::to_string(min) + ", " +
                         std::to_string(max) + "]",
                         NLMErrorCategory::ValidationError,
                         context);
        }
    }
    
    // Validate string parameter
    void validateStringParameter(const std::string& name, const std::string& value,
                                const std::string& context = "",
                                size_t minLength = 0,
                                size_t maxLength = std::numeric_limits<size_t>::max()) {
        if (value.length() < minLength || value.length() > maxLength) {
            throw NLMError(NLMErrorCode::ParameterValidationFailed,
                         "Parameter '" + name + "' length " + std::to_string(value.length()) +
                         " is outside valid range [" + std::to_string(minLength) + ", " +
                         std::to_string(maxLength) + "]",
                         NLMErrorCategory::ValidationError,
                         context);
        }
    }
    
    // Validate configuration value type
    template<typename T, typename U>
    void validateConfigType(const std::string& name, const U& value,
                           const std::string& context = "") {
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
    
    // Check if configuration value exists
    bool hasConfig(const std::string& key, const std::shared_ptr<Config>& config) {
        return config && config->has(key);
    }
    
    // Get configuration value with validation
    template<typename T>
    T getConfig(const std::string& key, const std::shared_ptr<Config>& config,
              const T& defaultValue, const std::string& context = "") {
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
    
    // Create recovery action for different error types
    std::string createRecoveryAction(NLMErrorCode code) {
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
    
private:
    ErrorHandler() {
        logger_ = spdlog::get("nlm");
        if (!logger_) {
            logger_ = spdlog::stdout_color_mt("nlm");
            logger_->set_level(spdlog::level::info);
        }
    }
    
    void logError(const NLMError& error) {
        if (logger_) {
            logger_->error(error.toString());
        }
    }
    
    void logWarning(const std::string& message, const std::string& context = "") {
        if (logger_) {
            std::string fullMessage = message;
            if (!context.empty()) {
                fullMessage += " (Context: " + context + ")";
            }
            logger_->warn(fullMessage);
        }
    }
    
    void logInfo(const std::string& message, const std::string& context = "") {
        if (logger_) {
            std::string fullMessage = message;
            if (!context.empty()) {
                fullMessage += " (Context: " + context + ")";
            }
            logger_->info(fullMessage);
        }
    }
    
    template<typename T>
    std::string typeName() {
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "std::string";
        else if constexpr (std::is_same_v<T, float>) return "float";
        else return "unknown";
    }
    
    std::shared_ptr<spdlog::logger> logger_;
};

// Global error handler instance
inline ErrorHandler& getErrorHandler() {
    return ErrorHandler::getInstance();
}

// Convenience macros for error handling
#define NLM_THROW_ERROR(code, message, category) \
    throw nlm::NLMError(code, message, category, __func__)

#define NLM_VALIDATE_PARAM(name, value, min, max, context) \
    nlm::getErrorHandler().validateParameter(name, value, context, min, max)

#define NLM_VALIDATE_STRING_PARAM(name, value, min_len, max_len, context) \
    nlm::getErrorHandler().validateStringParameter(name, value, context, min_len, max_len)

#define NLM_HANDLE_ERROR(error) \
    nlm::getErrorHandler().handleError(error)

#define NLM_HANDLE_WARNING(message, context) \
    nlm::getErrorHandler().handleWarning(message, context)

#define NLM_HANDLE_INFO(message, context) \
    nlm::getErrorHandler().handleInfo(message, context)

} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP