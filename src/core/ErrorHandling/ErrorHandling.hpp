// Enhanced error handling and validation framework for NLM
// Provides comprehensive error reporting, validation, and recovery mechanisms

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <optional>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Fatal
};

// Error category
enum class ErrorCategory {
    None,
    Memory,
    Configuration,
    Simulation,
    Network,
    Plasticity,
    Development,
    Neuromodulation,
    MemorySystem,
    Prediction,
    Cognition,
    Sensory,
    Motor,
    Environment,
    FileSystem,
    Threading,
    Unknown
};

// Base exception class
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message,
                 ErrorSeverity severity = ErrorSeverity::Error,
                 ErrorCategory category = ErrorCategory::Unknown,
                 const std::string& component = "",
                 int line = 0,
                 const std::string& file = "")
        : std::runtime_error(formatMessage(message, severity, category, component, line, file)),
          severity_(severity),
          category_(category),
          component_(component),
          line_(line),
          file_(file),
          timestamp_(std::chrono::system_clock::now()),
          stackTrace_() {
        // Capture stack trace (simplified)
        captureStackTrace();
    }
    
    virtual ~NLMException() noexcept = default;
    
    ErrorSeverity getSeverity() const noexcept { return severity_; }
    ErrorCategory getCategory() const noexcept { return category_; }
    const std::string& getComponent() const noexcept { return component_; }
    int getLine() const noexcept { return line_; }
    const std::string& getFile() const noexcept { return file_; }
    const std::chrono::system_clock::time_point& getTimestamp() const noexcept { return timestamp_; }
    const std::string& getStackTrace() const noexcept { return stackTrace_; }
    
    std::string getFormattedMessage() const {
        std::ostringstream oss;
        oss << "[NLM " << static_cast<int>(category_);
        oss << "] " << std::string(getSeverity() == ErrorSeverity::Fatal || getSeverity() == ErrorSeverity::Critical ? "CRITICAL" :
                                   getSeverity() == ErrorSeverity::Error ? "ERROR" :
                                   getSeverity() == ErrorSeverity::Warning ? "WARNING" :
                                   getSeverity() == ErrorSeverity::Info ? "INFO" : "DEBUG") << " : ";
        oss << std::string(getComponent());
        oss << ":" << getLine() << ": ";
        oss << what();
        return oss.str();
    }
    
protected:
    static std::string formatMessage(const std::string& message,
                                     ErrorSeverity severity,
                                     ErrorCategory category,
                                     const std::string& component,
                                     int line,
                                     const std::string& file) {
        std::ostringstream oss;
        oss << "NLM ";
        
        // Add category
        switch (category) {
            case ErrorCategory::Memory: oss << "MEMORY "; break;
            case ErrorCategory::Configuration: oss << "CONFIG "; break;
            case ErrorCategory::Simulation: oss << "SIMULATION "; break;
            case ErrorCategory::Network: oss << "NETWORK "; break;
            case ErrorCategory::Plasticity: oss << "PLASTICITY "; break;
            case ErrorCategory::Development: oss << "DEVELOPMENT "; break;
            case ErrorCategory::Neuromodulation: oss << "NEUROMOD "; break;
            case ErrorCategory::MemorySystem: oss << "MEMORY "; break;
            case ErrorCategory::Prediction: oss << "PREDICTION "; break;
            case ErrorCategory::Cognition: oss << "COGNITION "; break;
            case ErrorCategory::Sensory: oss << "SENSORY "; break;
            case ErrorCategory::Motor: oss << "MOTOR "; break;
            case ErrorCategory::Environment: oss << "ENVIRONMENT "; break;
            case ErrorCategory::FileSystem: oss << "FILESYSTEM "; break;
            case ErrorCategory::Threading: oss << "THREADING "; break;
            default: oss << "UNKNOWN "; break;
        }
        
        // Add severity
        oss << "[" << static_cast<int>(severity) << "] ";
        
        // Add component
        if (!component.empty()) {
            oss << component << ":";
        }
        
        // Add location
        if (!file.empty()) {
            oss << file;
            if (line > 0) {
                oss << ":" << line;
            }
            oss << ": ";
        }
        
        oss << message;
        return oss.str();
    }
    
private:
    void captureStackTrace() {
        // Simplified stack trace capture
        stackTrace_ = "Stack trace capture not implemented in this build";
    }
    
    ErrorSeverity severity_;
    ErrorCategory category_;
    std::string component_;
    int line_;
    std::string file_;
    std::chrono::system_clock::time_point timestamp_;
    std::string stackTrace_;
};

// Validation framework
class ValidationResult {
public:
    ValidationResult() : isValid_(true) {}
    ValidationResult(const std::string& error) : isValid_(false), errorMessage_(error) {}
    
    bool isValid() const { return isValid_; }
    bool isInvalid() const { return !isValid_; }
    
    ValidationResult& addError(const std::string& error) {
        isValid_ = false;
        if (!errorMessage_.empty()) errorMessage_ += "; ";
        errorMessage_ += error;
        return *this;
    }
    
    ValidationResult& addWarning(const std::string& warning) {
        if (!warningMessage_.empty()) warningMessage_ += "; ";
        warningMessage_ += warning;
        return *this;
    }
    
    const std::string& getErrorMessage() const { return errorMessage_; }
    const std::string& getWarningMessage() const { return warningMessage_; }
    
    std::string getFullMessage() const {
        std::ostringstream oss;
        if (!warningMessage_.empty()) {
            oss << "WARNINGS: " << warningMessage_ << "\n";
        }
        if (!errorMessage_.empty()) {
            oss << "ERRORS: " << errorMessage_;
        }
        return oss.str();
    }
    
private:
    bool isValid_;
    std::string errorMessage_;
    std::string warningMessage_;
};

// Error reporting system
class ErrorReporter {
public:
    static ErrorReporter& getInstance() {
        static ErrorReporter instance;
        return instance;
    }
    
    void logError(const std::shared_ptr<NLMException>& error) {
        std::lock_guard<std::mutex> lock(mutex_);
        errors_.push_back(error);
        
        // Output based on severity
        switch (error->getSeverity()) {
            case ErrorSeverity::Debug:
                if (debugEnabled_) {
                    std::cerr << error->getFormattedMessage() << std::endl;
                }
                break;
            case ErrorSeverity::Info:
                if (infoEnabled_) {
                    std::cerr << error->getFormattedMessage() << std::endl;
                }
                break;
            case ErrorSeverity::Warning:
                std::cerr << "WARNING: " << error->getFormattedMessage() << std::endl;
                break;
            case ErrorSeverity::Error:
                std::cerr << "ERROR: " << error->getFormattedMessage() << std::endl;
                break;
            case ErrorSeverity::Critical:
                std::cerr << "CRITICAL: " << error->getFormattedMessage() << std::endl;
                break;
            case ErrorSeverity::Fatal:
                std::cerr << "FATAL: " << error->getFormattedMessage() << std::endl;
                break;
        }
    }
    
    std::vector<std::shared_ptr<NLMException>> getErrors(ErrorSeverity minSeverity = ErrorSeverity::Error) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::shared_ptr<NLMException>> filteredErrors;
        
        for (const auto& error : errors_) {
            if (error->getSeverity() >= minSeverity) {
                filteredErrors.push_back(error);
            }
        }
        
        return filteredErrors;
    }
    
    void clearErrors() {
        std::lock_guard<std::mutex> lock(mutex_);
        errors_.clear();
    }
    
    size_t getErrorCount(ErrorSeverity minSeverity = ErrorSeverity::Error) {
        return getErrors(minSeverity).size();
    }
    
    void setDebugEnabled(bool enabled) { debugEnabled_ = enabled; }
    void setInfoEnabled(bool enabled) { infoEnabled_ = enabled; }
    
private:
    ErrorReporter() : debugEnabled_(false), infoEnabled_(false) {}
    ~ErrorReporter() = default;
    
    std::mutex mutex_;
    std::vector<std::shared_ptr<NLMException>> errors_;
    bool debugEnabled_;
    bool infoEnabled_;
};

// Validation utilities
class ValidationUtils {
public:
    // Configuration validation
    static ValidationResult validateConfig(const std::unordered_map<std::string, std::string>& values);
    
    // Neuron validation
    static ValidationResult validateNeuronParameters(float vrest, float vreset, float threshold, 
                                                     float tau, float cm, float refracPeriod);
    
    // Synapse validation
    static ValidationResult validateSynapseParameters(float weight, float delay, bool isExcitatory);
    
    // Network validation
    static ValidationResult validateNetworkTopology(size_t neuronCount, size_t regionCount, 
                                                    float connectionProbability);
    
    // Plasticity validation
    static ValidationResult validatePlasticityParameters(float learningRate, float stdpLTP, float stdpLTD);
    
    // Memory validation
    static ValidationResult validateMemoryParameters(size_t capacity, size_t maxEpisodes);
    
    // Simulation validation
    static ValidationResult validateSimulationParameters(double timestep, size_t maxSteps);
    
    // Type conversion and validation
    static std::optional<int> toInt(const std::string& str, int min = INT_MIN, int max = INT_MAX);
    static std::optional<double> toDouble(const std::string& str, double min = -std::numeric_limits<double>::infinity(),
                                          double max = std::numeric_limits<double>::infinity());
    static std::optional<float> toFloat(const std::string& str, float min = -std::numeric_limits<float>::infinity(),
                                       float max = std::numeric_limits<float>::infinity());
    static std::optional<bool> toBool(const std::string& str);
    
    // String validation
    static bool isValidKey(const std::string& key);
    static bool isValidFilePath(const std::string& path);
    static bool isValidLogLevel(const std::string& level);
    
    // Collection validation
    static bool isNotEmpty(const std::string& str);
    static bool isNotEmpty(const std::vector<std::string>& vec);
    static size_t size(const std::vector<std::string>& vec);
    
    // Range validation
    template<typename T>
    static bool isInRange(const T& value, const T& min, const T& max) {
        return value >= min && value <= max;
    }
    
    // Normalize value to range
    template<typename T>
    static T clamp(const T& value, const T& min, const T& max) {
        return std::max(min, std::min(value, max));
    }
    
    // Round to nearest value
    template<typename T>
    static T round(const T& value, const T& precision = 1) {
        return std::round(value / precision) * precision;
    }
    
private:
    static bool isWhitespace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
    static bool isValidKeyChar(char c) { return std::isalnum(c) || c == '_' || c == '-' || c == '.'; }
};

// Safe resource management
class SafeResourceManager {
public:
    template<typename T, typename... Args>
    static std::shared_ptr<T> createSafe(Args&&... args) {
        try {
            auto resource = std::make_shared<T>(std::forward<Args>(args)...);
            return resource;
        } catch (const std::bad_alloc&) {
            throw NLMException("Memory allocation failed while creating " + std::string(typeid(T).name(),
                              ErrorSeverity::Critical, ErrorCategory::Memory, "SafeResourceManager");
        } catch (const std::exception& e) {
            throw NLMException(std::string("Failed to create resource: ") + e.what(),
                              ErrorSeverity::Error, ErrorCategory::Unknown, "SafeResourceManager");
        }
    }
    
    template<typename T>
    static std::shared_ptr<T> createSafeWithFallback(const std::string& fallbackKey,
                                                     std::function<std::shared_ptr<T>()> creator) {
        try {
            auto resource = creator();
            if (resource) {
                return resource;
            }
        } catch (...) {
            // Try fallback configuration
        }
        
        // Try to create from fallback configuration
        // This is a simplified implementation
        throw NLMException("All resource creation attempts failed for " + fallbackKey,
                          ErrorSeverity::Error, ErrorCategory::Unknown, "SafeResourceManager");
    }
};

// Retry mechanism for operations
class RetryManager {
public:
    struct RetryPolicy {
        int maxAttempts;
        double initialDelayMs;
        double backoffMultiplier;
        double maxDelayMs;
        std::vector<ErrorCategory> retryableCategories;
    };
    
    template<typename F>
    static auto executeWithRetry(F&& operation, const RetryPolicy& policy) {
        int attempt = 0;
        double delay = policy.initialDelayMs;
        
        while (attempt < policy.maxAttempts) {
            attempt++;
            
            try {
                return operation();
            } catch (const NLMException& e) {
                // Check if error is retryable
                bool isRetryable = policy.retryableCategories.empty() || 
                                 std::find(policy.retryableCategories.begin(),
                                          policy.retryableCategories.end(), e.getCategory()) != policy.retryableCategories.end();
                
                if (!isRetryable || attempt >= policy.maxAttempts) {
                    throw;
                }
                
                // Wait before retry
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(delay)));
                
                // Exponential backoff
                delay = std::min(delay * policy.backoffMultiplier, policy.maxDelayMs);
            }
        }
        
        throw NLMException("Max retry attempts exceeded", ErrorSeverity::Error, ErrorCategory::Unknown, "RetryManager");
    }
};

// Recovery action interface
class RecoveryAction {
public:
    virtual ~RecoveryAction() = default;
    virtual bool canRecover() const = 0;
    virtual bool performRecovery() = 0;
    virtual std::string getDescription() const = 0;
};

// Recovery manager
class RecoveryManager {
public:
    RecoveryManager() = default;
    ~RecoveryManager() = default;
    
    void addRecoveryAction(const std::shared_ptr<RecoveryAction>& action) {
        recoveryActions_.push_back(action);
    }
    
    bool attemptRecovery(const std::shared_ptr<NLMException>& error) {
        for (const auto& action : recoveryActions_) {
            if (action->canRecover()) {
                try {
                    if (action->performRecovery()) {
                        return true;
                    }
                } catch (...) {
                    continue;
                }
            }
        }
        return false;
    }
    
    std::vector<std::string> getRecoveryActions() const {
        std::vector<std::string> descriptions;
        for (const auto& action : recoveryActions_) {
            descriptions.push_back(action->getDescription());
        }
        return descriptions;
    }
    
private:
    std::vector<std::shared_ptr<RecoveryAction>> recoveryActions_;
};

// Comprehensive error handling wrapper
class ErrorHandlingWrapper {
public:
    template<typename F>
    static auto executeWithErrorHandling(F&& operation,
                                         const std::string& operationName,
                                         ErrorCategory category = ErrorCategory::Unknown,
                                         const std::vector<std::shared_ptr<RecoveryAction>>& recoveryActions = {}) {
        try {
            RecoveryManager recoveryManager;
            for (const auto& action : recoveryActions) {
                recoveryManager.addRecoveryAction(action);
            }
            
            return operation();
            
        } catch (const NLMException& e) {
            // Log the error
            ErrorReporter::getInstance().logError(std::make_shared<NLMException>(e));
            
            // Attempt recovery
            if (ErrorReporter::getInstance().attemptRecovery(std::make_shared<NLMException>(e))) {
                throw NLMException("Recovery failed for " + operationName,
                                  ErrorSeverity::Error, category, operationName);
            }
            
            throw; // Re-throw after recovery attempt
        } catch (const std::exception& e) {
            ErrorReporter::getInstance().logError(
                std::make_shared<NLMException>(std::string("Unexpected error in " + operationName + ": ") + e.what(),
                                             ErrorSeverity::Error, category, operationName));
            
            throw NLMException("Operation failed: " + operationName + " - " + std::string(e.what()),
                              ErrorSeverity::Error, category, operationName);
        } catch (...) {
            ErrorReporter::getInstance().logError(
                std::make_shared<NLMException>("Unknown error in " + operationName,
                                             ErrorSeverity::Fatal, category, operationName));
            
            throw NLMException("Unknown error in " + operationName,
                              ErrorSeverity::Fatal, category, operationName);
        }
    }
};

// Global error handling setup
class GlobalErrorHandler {
public:
    static void setup(bool enableDebug = false, bool enableInfo = false) {
        ErrorReporter::getInstance().setDebugEnabled(enableDebug);
        ErrorReporter::getInstance().setInfoEnabled(enableInfo);
    }
    
    static void cleanup() {
        ErrorReporter::getInstance().clearErrors();
    }
    
    static std::vector<std::string> getRecentErrors(ErrorSeverity minSeverity = ErrorSeverity::Error) {
        auto errors = ErrorReporter::getInstance().getErrors(minSeverity);
        std::vector<std::string> messages;
        for (const auto& error : errors) {
            messages.push_back(error->getFormattedMessage());
        }
        return messages;
    }
};

} // namespace nlm
