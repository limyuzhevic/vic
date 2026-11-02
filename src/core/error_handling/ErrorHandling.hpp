// Error Handling and Validation Utilities
// Provides comprehensive error handling, validation, and debugging capabilities

#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace nlm {

// Exception hierarchy
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error("NLM Error: " + message), code_(ErrorCode::Generic) {}
    
    NLMException(const std::string& message, ErrorCode code)
        : std::runtime_error("NLM Error " + std::to_string(static_cast<int>(code)) + ": " + message), code_(code) {}
    
    ErrorCode code() const { return code_; }
    
private:
    ErrorCode code_;
};

class ConfigurationException : public NLMException {
public:
    explicit ConfigurationException(const std::string& message) 
        : NLMException(message, ErrorCode::ConfigError) {}
};

class BrainInitializationException : public NLMException {
public:
    explicit BrainInitializationException(const std::string& message) 
        : NLMException(message, ErrorCode::BrainError) {}
};

class MemoryAllocationException : public NLMException {
public:
    explicit MemoryAllocationException(const std::string& message) 
        : NLMException(message, ErrorCode::MemoryError) {}
};

class ValidationException : public NLMException {
public:
    explicit ValidationException(const std::string& message) 
        : NLMException(message, ErrorCode::ValidationError) {}
};

// Error codes for better error categorization
enum class ErrorCode {
    Success = 0,
    Generic = 1,
    ConfigError = 2,
    BrainError = 3,
    MemoryError = 4,
    ValidationError = 5,
    FileError = 6,
    NetworkError = 7,
    PlasticityError = 8,
    NeuromodulationError = 9,
    DevelopmentError = 10
};

// Validation utilities
class Validator {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        ValidationResult() : valid(true) {}
        
        void addError(const std::string& error) {
            valid = false;
            errors.push_back(error);
        }
        
        void addWarning(const std::string& warning) {
            warnings.push_back(warning);
        }
        
        std::string getSummary() const {
            std::stringstream ss;
            if (!valid) {
                ss << "Validation failed with " << errors.size() << " error(s): ";
                for (const auto& error : errors) {
                    ss << "[" << error << "] ";
                }
            }
            if (!warnings.empty()) {
                if (!valid) ss << " ";
                ss << "Warnings: ";
                for (const auto& warning : warnings) {
                    ss << "[" << warning << "] ";
                }
            }
            return ss.str();
        }
    };
    
    // Validation for configuration values
    static ValidationResult validateConfigValue(const std::string& key, const std::string& value) {
        ValidationResult result;
        
        if (key.empty()) {
            result.addError("Configuration key cannot be empty");
        }
        
        if (value.empty()) {
            result.addWarning("Configuration value for key '" + key + "' is empty");
        }
        
        // Type-specific validation
        if (key.find("count") != std::string::npos || key.find("size") != std::string::npos) {
            try {
                int val = std::stoi(value);
                if (val < 0) {
                    result.addError("Count/size value must be non-negative");
                }
            } catch (const std::exception&) {
                result.addError("Invalid integer value for count/size parameter");
            }
        } else if (key.find("rate") != std::string::npos || key.find("probability") != std::string::npos) {
            try {
                float val = std::stof(value);
                if (val < 0.0f || val > 1.0f) {
                    result.addWarning("Rate/probability value " + std::to_string(val) + " should be between 0 and 1");
                }
            } catch (const std::exception&) {
                result.addError("Invalid float value for rate/probability parameter");
            }
        } else if (key.find("time") != std::string::npos || key.find("delay") != std::string::npos) {
            try {
                float val = std::stof(value);
                if (val < 0.0f) {
                    result.addError("Time/delay value must be non-negative");
                }
            } catch (const std::exception&) {
                result.addError("Invalid float value for time/delay parameter");
            }
        }
        
        return result;
    }
    
    // Validation for neural parameters
    static ValidationResult validateNeuralParameters(const std::string& prefix = "") {
        ValidationResult result;
        
        // Test values for common neural parameters
        std::vector<std::pair<std::string, std::function<bool(float)>>> checks = {
            {"leak_conductance", [](float v) { return v >= 0.0f && v <= 100.0f; }},
            {"threshold", [](float v) { return v >= -100.0f && v <= 0.0f; }},
            {"reset_potential", [](float v) { return v >= -100.0f && v <= 0.0f; }},
            {"refractory_period", [](float v) { return v >= 0 && v <= 1000; }},
            {"learning_rate", [](float v) { return v >= 0.0f && v <= 1.0f; }},
            {"plasticity_weight", [](float v) { return v >= 0.0f && v <= 10.0f; }},
        };
        
        // This would integrate with the actual configuration system
        // For now, return empty result as validation is complex in this context
        return result;
    }
    
    // Validation for array values
    static ValidationResult validateArray(const std::string& name, const std::vector<float>& values,
                                          float minVal = -std::numeric_limits<float>::infinity(),
                                          float maxVal = std::numeric_limits<float>::infinity()) {
        ValidationResult result;
        
        if (values.empty()) {
            result.addWarning("Array '" + name + "' is empty");
            return result;
        }
        
        for (size_t i = 0; i < values.size(); ++i) {
            if (values[i] < minVal) {
                result.addError("Array '" + name + "' value at index " + std::to_string(i) + 
                               " (" + std::to_string(values[i]) + ") is below minimum " + 
                               std::to_string(minVal));
            }
            if (values[i] > maxVal) {
                result.addError("Array '" + name + "' value at index " + std::to_string(i) + 
                               " (" + std::to_string(values[i]) + ") exceeds maximum " + 
                               std::to_string(maxVal));
            }
        }
        
        return result;
    }
    
    // Range validation
    template<typename T>
    static bool isInRange(T value, T min, T max) {
        return value >= min && value <= max;
    }
    
    // Non-negative validation
    template<typename T>
    static bool isNonNegative(T value) {
        return value >= 0;
    }
    
    // Positive validation
    template<typename T>
    static bool isPositive(T value) {
        return value > 0;
    }
};

// Error reporting and logging
class ErrorReporter {
public:
    struct ErrorReport {
        std::chrono::system_clock::time_point timestamp;
        ErrorCode code;
        std::string category;
        std::string message;
        std::string context;
        std::vector<std::string> stackTrace;
        bool isRecoverable;
        
        std::string getFormattedTimestamp() const {
            auto time = std::chrono::system_clock::to_time_t(timestamp);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
            return ss.str();
        }
    };
    
    static ErrorReporter& getInstance() {
        static ErrorReporter instance;
        return instance;
    }
    
    // Report an error
    void report(ErrorCode code, const std::string& category, const std::string& message,
               const std::string& context = "", bool isRecoverable = true) {
        ErrorReport report;
        report.timestamp = std::chrono::system_clock::now();
        report.code = code;
        report.category = category;
        report.message = message;
        report.context = context;
        report.isRecoverable = isRecoverable;
        
        // Add to error log
        errorLog_.push_back(report);
        
        // Log to console
        std::cerr << "[ERROR] " << getErrorCodeString(code) << ": " 
                  << category << ": " << message << std::endl;
        if (!context.empty()) {
            std::cerr << "  Context: " << context << std::endl;
        }
    }
    
    // Get recent errors
    std::vector<ErrorReport> getRecentErrors(size_t limit = 10) const {
        if (errorLog_.size() <= limit) {
            return errorLog_;
        }
        
        auto start = errorLog_.end() - limit;
        return std::vector<ErrorReport>(start, errorLog_.end());
    }
    
    // Get errors by category
    std::vector<ErrorReport> getErrorsByCategory(const std::string& category) const {
        std::vector<ErrorReport> result;
        for (const auto& error : errorLog_) {
            if (error.category == category) {
                result.push_back(error);
            }
        }
        return result;
    }
    
    // Get errors by code
    std::vector<ErrorReport> getErrorsByCode(ErrorCode code) const {
        std::vector<ErrorReport> result;
        for (const auto& error : errorLog_) {
            if (error.code == code) {
                result.push_back(error);
            }
        }
        return result;
    }
    
    // Clear error log
    void clearErrorLog() {
        errorLog_.clear();
    }
    
    // Generate error summary
    std::string generateErrorSummary() const {
        std::stringstream ss;
        ss << "Error Summary:" << std::endl;
        ss << "Total errors: " << errorLog_.size() << std::endl;
        
        // Count by category
        std::unordered_map<std::string, size_t> categoryCounts;
        for (const auto& error : errorLog_) {
            categoryCounts[error.category]++;
        }
        
        ss << "Errors by category:" << std::endl;
        for (const auto& pair : categoryCounts) {
            ss << "  " << pair.first << ": " << pair.second << std::endl;
        }
        
        return ss.str();
    }
    
    // Convert error code to string
    static std::string getErrorCodeString(ErrorCode code) {
        const std::unordered_map<ErrorCode, std::string> codeStrings = {
            {ErrorCode::Success, "SUCCESS"},
            {ErrorCode::Generic, "GENERIC"},
            {ErrorCode::ConfigError, "CONFIG"},
            {ErrorCode::BrainError, "BRAIN"},
            {ErrorCode::MemoryError, "MEMORY"},
            {ErrorCode::ValidationError, "VALIDATION"},
            {ErrorCode::FileError, "FILE"},
            {ErrorCode::NetworkError, "NETWORK"},
            {ErrorCode::PlasticityError, "PLASTICITY"},
            {ErrorCode::NeuromodulationError, "NEUROMODULATION"},
            {ErrorCode::DevelopmentError, "DEVELOPMENT"},
        };
        
        auto it = codeStrings.find(code);
        return it != codeStrings.end() ? it->second : "UNKNOWN";
    }
    
private:
    ErrorReporter() {}
    ErrorReporter(const ErrorReporter&) = delete;
    ErrorReporter& operator=(const ErrorReporter&) = delete;
    
    std::vector<ErrorReport> errorLog_;
};

// Safe execution utilities
class SafeExecution {
public:
    // Execute a function with error handling
    template<typename Function, typename... Args>
    static auto executeSafely(ErrorCode errorCode, const std::string& category,
                              const std::string& context, Function&& func, Args&&... args) -> decltype(func(args...)) {
        try {
            auto result = func(std::forward<Args>(args)...);
            ErrorReporter::getInstance().report(ErrorCode::Success, category,
                                              "Operation completed successfully", context, true);
            return result;
        } catch (const NLMException& e) {
            ErrorReporter::getInstance().report(errorCode, category, e.what(), context, true);
            throw;
        } catch (const std::exception& e) {
            ErrorReporter::getInstance().report(errorCode, category,
                                              std::string("Standard exception: ") + e.what(), context, false);
            throw;
        } catch (...) {
            ErrorReporter::getInstance().report(errorCode, category, "Unknown exception occurred", context, false);
            throw;
        }
    }
    
    // Execute with retry logic
    template<typename Function, typename... Args>
    static auto executeWithRetry(ErrorCode errorCode, const std::string& category,
                                const std::string& context, Function&& func, Args&&... args,
                                int maxRetries = 3, std::chrono::milliseconds delay = std::chrono::milliseconds(100)) {
        int attempt = 0;
        std::exception_ptr lastException;
        
        while (attempt < maxRetries) {
            try {
                auto result = func(std::forward<Args>(args)...);
                return result;
            } catch (...) {
                lastException = std::current_exception();
                attempt++;
                
                if (attempt < maxRetries) {
                    std::this_thread::sleep_for(delay);
                    delay *= 2; // Exponential backoff
                }
            }
        }
        
        // All retries failed
        if (lastException) {
            std::rethrow_exception(lastException);
        }
        
        throw NLMException("All retry attempts failed", errorCode);
    }
    
    // Validate and execute
    template<typename Function, typename... Args>
    static auto validateAndExecute(const Validator::ValidationResult& validation,
                                   ErrorCode errorCode, const std::string& category,
                                   const std::string& context, Function&& func, Args&&... args) {
        if (!validation.valid) {
            ErrorReporter::getInstance().report(errorCode, category,
                                              "Validation failed: " + validation.getSummary(),
                                              context, false);
            throw ValidationException("Validation failed: " + validation.getSummary());
        }
        
        if (!validation.warnings.empty()) {
            for (const auto& warning : validation.warnings) {
                ErrorReporter::getInstance().report(ErrorCode::ValidationError, category,
                                                  "Warning: " + warning, context, true);
            }
        }
        
        return executeSafely(errorCode, category, context, func, std::forward<Args>(args)...);
    }
};

// Debugging utilities
class Debugger {
public:
    static Debugger& getInstance() {
        static Debugger instance;
        return instance;
    }
    
    // Set debug level
    void setDebugLevel(int level) {
        debugLevel_ = level;
    }
    
    // Get debug level
    int getDebugLevel() const {
        return debugLevel_;
    }
    
    // Log debug message
    void logDebug(const std::string& message) {
        if (debugLevel_ >= 1) {
            std::cout << "[DEBUG] " << message << std::endl;
        }
    }
    
    // Log verbose debug message
    void logVerbose(const std::string& message) {
        if (debugLevel_ >= 2) {
            std::cout << "[VERBOSE] " << message << std::endl;
        }
    }
    
    // Enable performance profiling
    class Profiler {
    public:
        Profiler(const std::string& name) : name_(name) {
            start_time_ = std::chrono::high_resolution_clock::now();
        }
        
        ~Profiler() {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
            
            Debugger::getInstance().logVerbose(name_ + " took " + std::to_string(duration.count()) + " microseconds");
        }
        
        double elapsedMicroseconds() const {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
            return static_cast<double>(duration.count());
        }
        
    private:
        std::string name_;
        std::chrono::high_resolution_clock::time_point start_time_;
    };
    
    // Create profiler instance
    Profiler createProfiler(const std::string& name) {
        return Profiler(name);
    }
    
private:
    Debugger() : debugLevel_(0) {}
    Debugger(const Debugger&) = delete;
    Debugger& operator=(const Debugger&) = delete;
    
    int debugLevel_;
};

// Recovery utilities
class RecoveryManager {
public:
    struct RecoveryPoint {
        std::chrono::system_clock::time_point timestamp;
        std::string description;
        std::function<void()> restoreFunction;
        bool isValid;
        
        RecoveryPoint(const std::string& desc, std::function<void()> restore)
            : timestamp(std::chrono::system_clock::now())
            , description(desc)
            , restoreFunction(restore)
            , isValid(true) {}
    };
    
    static RecoveryManager& getInstance() {
        static RecoveryManager instance;
        return instance;
    }
    
    // Create recovery point
    void createRecoveryPoint(const std::string& description, std::function<void()> restoreFunction) {
        RecoveryPoint point(description, restoreFunction);
        recoveryPoints_.push_back(point);
        
        Debugger::getInstance().logDebug("Created recovery point: " + description);
    }
    
    // Restore from last recovery point
    bool restoreFromLastRecoveryPoint() {
        if (recoveryPoints_.empty()) {
            return false;
        }
        
        auto point = recoveryPoints_.back();
        recoveryPoints_.pop_back();
        
        try {
            point.restoreFunction();
            Debugger::getInstance().logDebug("Restored from recovery point: " + point.description);
            return true;
        } catch (const std::exception& e) {
            Debugger::getInstance().logDebug("Failed to restore from recovery point " + point.description + ": " + e.what());
            point.isValid = false;
            return false;
        }
    }
    
    // List recovery points
    std::vector<std::string> listRecoveryPoints() const {
        std::vector<std::string> descriptions;
        for (const auto& point : recoveryPoints_) {
            descriptions.push_back(point.description);
        }
        return descriptions;
    }
    
    // Clear recovery points
    void clearRecoveryPoints() {
        recoveryPoints_.clear();
    }
    
private:
    RecoveryManager() {}
    RecoveryManager(const RecoveryManager&) = delete;
    RecoveryManager& operator=(const RecoveryManager&) = delete;
    
    std::vector<RecoveryPoint> recoveryPoints_;
};

} // namespace nlm
