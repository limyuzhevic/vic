#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

namespace nlm {

// Error handling and validation utilities
class ErrorHandler {
public:
    // Global error reporting callback
    using ErrorCallback = std::function<void(const std::exception&)>;
    
    static void setErrorCallback(ErrorCallback callback) {
        errorCallback_ = callback;
    }
    
    static ErrorCallback getErrorCallback() { return errorCallback_; }
    
    static void reportError(const std::exception& e) {
        if (errorCallback_) {
            errorCallback_(e);
        }
        // Also log the error
        std::string msg = "ERROR: ";
        try {
            msg += e.what();
        } catch (...) {
            msg += "Unknown error occurred";
        }
        getGlobal().error(msg);
    }
    
    // Input validation helpers
    template<typename T>
    static T validateNonNull(const T& value, const std::string& errorMsg) {
        if (!value) {
            throw NullPointerException(errorMsg);
        }
        return value;
    }
    
    template<typename T>
    static void validateRange(const T& value, const T& min, const T& max, const std::string& errorMsg) {
        if (value < min || value > max) {
            throw ValidationException(errorMsg + " (value: " + std::to_string(value) + ", expected range: [" + 
                                     std::to_string(min) + ", " + std::to_string(max) + "])");
        }
    }
    
    static void validateFileAccess(const std::string& filepath, std::ios_base::openmode mode) {
        std::ifstream file(filepath, mode);
        if (!file.is_open()) {
            throw FileIOError("Cannot open file for access: " + filepath);
        }
    }
    
    static void validateNotEmpty(const std::string& str, const std::string& errorMsg) {
        if (str.empty()) {
            throw ValidationException(errorMsg);
        }
    }
    
private:
    static ErrorCallback errorCallback_;
};

class ValidationHelper {
public:
    template<typename T>
    static T validatePositive(const T& value, const std::string& context) {
        if (value <= 0) {
            throw ValidationException("Value must be positive (" + context + "): " + std::to_string(value));
        }
        return value;
    }
    
    template<typename T>
    static T validateNonNegative(const T& value, const std::string& context) {
        if (value < 0) {
            throw ValidationException("Value must be non-negative (" + context + "): " + std::to_string(value));
        }
        return value;
    }
    
    static void validateNeuronId(const NeuronId& id, const std::string& context) {
        if (id == INVALID_NEURON_ID) {
            throw ValidationException("Invalid neuron ID provided (" + context + ")");
        }
    }
    
    static void validatePopulationId(const PopulationId& id, const std::string& context) {
        if (id == INVALID_POPULATION_ID) {
            throw ValidationException("Invalid population ID provided (" + context + ")");
        }
    }
    
    static void validateSynapseId(const SynapseId& id, const std::string& context) {
        if (id == INVALID_SYNAPSE_ID) {
            throw ValidationException("Invalid synapse ID provided (" + context + ")");
        }
    }
    
    static void validateRegionId(const RegionId& id, const std::string& context) {
        if (id == INVALID_REGION_ID) {
            throw ValidationException("Invalid region ID provided (" + context + ")");
        }
    }
    
    static void validateTime(const Timestamp& time, const std::string& context) {
        if (time < 0.0) {
            throw ValidationException("Invalid time value (" + context + "): " + std::to_string(time));
        }
    }
    
    static void validateWeight(float weight) {
        if (std::isnan(weight) || std::isinf(weight)) {
            throw ValidationException("Invalid synaptic weight: " + std::to_string(weight));
        }
    }
    
    static void validateStep(SimulationStep step) {
        if (step == 0) {
            throw ValidationException("Invalid simulation step: " + std::to_string(step));
        }
    }
    
    static void validateCapacity(size_t capacity) {
        if (capacity == 0) {
            throw ValidationException("Capacity must be greater than zero: " + std::to_string(capacity));
        }
    }
};

// Macros for convenient error handling and validation
#define NLM_VALIDATE_NON_NULL(ptr, msg) \
    do { \
        if (!(ptr)) { \
            throw NullPointerException(msg, __FILE__, __LINE__, __func__); \
        } \
    } while(0)

#define NLM_VALIDATE_RANGE(val, min, max, msg) \
    do { \
        if ((val) < (min) || (val) > (max)) { \
            std::string rangeMsg = msg + " (value: " + std::to_string(val) + \", expected range: [" + \
                                  std::to_string(min) + ", " + std::to_string(max) + \"])"; \
            throw ValidationException(rangeMsg, __FILE__, __LINE__, __func__); \
        } \
    } while(0)

#define NLM_VALIDATE_NOT_EMPTY(str, msg) \
    do { \
        if ((str).empty()) { \
            throw ValidationException(msg, __FILE__, __LINE__, __func__); \
        } \
    } while(0)

#define NLM_THROW_EXCEPT(type, msg) \
    do { \
        throw type(msg, __FILE__, __LINE__, __func__); \
    } while(0)

// Error checking macros for function entry points
#define NLM_CHECK_AND_THROW(condition, type, msg) \
    do { \
        if (!(condition)) { \
            throw type(msg, __FILE__, __LINE__, __func__); \
        } \
    } while(0)

// Success/failure return type with error propagation
class Result {
public:
    template<typename T>
    static Result<T> success(T value) {
        return Result<T>(std::move(value));
    }
    
    template<typename T>
    static Result<T> failure(const std::string& error) {
        return Result<T>(NLMException(error));
    }
    
    template<typename T>
    bool isSuccess() const {
        return !hasError();
    }
    
    template<typename T>
    bool hasError() const {
        return !success_;
    }
    
    template<typename T>
    const T& getValue() const {
        if (!success_) {
            throw NLMException("Cannot get value from failed result");
        }
        return value_;
    }
    
    template<typename T>
    T& getValue() {
        if (!success_) {
            throw NLMException("Cannot get value from failed result");
        }
        return value_;
    }
    
    template<typename T>
    const std::exception& getError() const {
        if (success_) {
            throw NLMException("Cannot get error from successful result");
        }
        return error_;
    }
    
private:
    Result() : success_(false) {}
    Result(const std::exception& error) : success_(false), error_(error) {}
    Result(T value) : success_(true), value_(std::move(value)) {}
    
    bool success_;
    std::exception error_;
    T value_;
    
    template<typename> friend class Result;
};

// Global error handler callback static instance
static ErrorHandler::ErrorCallback ErrorHandler::errorCallback_ = nullptr;

// Convenience functions for error handling in existing code
inline void throwIfError(bool condition, const std::string& message) {
    if (condition) {
        throw ValidationException(message);
    }
}

inline void throwIfNullptr(void* ptr, const std::string& message) {
    if (!ptr) {
        throw NullPointerException(message);
    }
}

inline void logAndThrow(const std::exception& e) {
    getGlobal().error(std::string("Exception: ") + e.what());
    throw;
}

inline bool tryExecute(std::function<void()> func, const std::string& errorContext = "") {
    try {
        func();
        return true;
    } catch (const std::exception& e) {
        std::string context = errorContext.empty() ? "" : (errorContext + ": ");
        getGlobal().error(context + e.what());
        return false;
    }
}

// Log level for error reporting
enum class ErrorLevel {
    Info = 0,
    Warning = 1,
    Error = 2,
    Critical = 3
};

// Enhanced error logging with level
inline void logError(const std::string& message, ErrorLevel level = ErrorLevel::Error) {
    switch (level) {
        case ErrorLevel::Info:
            getGlobal().info(message);
            break;
        case ErrorLevel::Warning:
            getGlobal().warning(message);
            break;
        case ErrorLevel::Error:
            getGlobal().error(message);
            break;
        case ErrorLevel::Critical:
            getGlobal().critical(message);
            break;
    }
}

// Safe file operations with error checking
inline std::ifstream safeOpenInputFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw FileIOError("Failed to open input file: " + filepath);
    }
    return file;
}

inline std::ofstream safeOpenOutputFile(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw FileIOError("Failed to open output file: " + filepath);
    }
    return file;
}

} // namespace nlm
