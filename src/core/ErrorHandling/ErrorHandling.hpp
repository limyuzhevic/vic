/*
 * NLM Error Handling and Validation Framework
 * Comprehensive error reporting, exception safety, and validation system
 */

#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <system_error>
#include <chrono>
#include <optional>
#include <unordered_map>
#include <functional>
#include <type_traits>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <atomic>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

// Error categories for better organization
enum class ErrorCategory {
    None = 0,
    Memory = 1,
    Configuration = 2,
    Validation = 3,
    Network = 4,
    FileSystem = 5,
    NeuralNetwork = 6,
    Plasticity = 7,
    Development = 8,
    MemoryManagement = 9,
    ResourceManagement = 10,
    Integration = 11,
    System = 12
};

// Standard error codes
enum class ErrorCode {
    // Success
    Success = 0,
    
    // Memory errors
    MemoryAllocationFailed = 1000,
    MemoryCorruption = 1001,
    BufferOverflow =1002,
    NullPointer = 1003,
    
    // Configuration errors
    ConfigFileNotFound = 2000,
    InvalidConfigFormat = 2001,
    MissingRequiredConfig = 2002,
    InvalidConfigValue = 2003,
    
    // Validation errors
    ValidationFailed = 3000,
    InvalidInput = 3001,
    OutOfRange = 3002,
    DomainError = 3003,
    
    // Neural network errors
    NeuronNotFound = 4000,
    SynapseNotFound = 4001,
    InvalidConnection = 4002,
    NetworkNotInitialized = 4003,
    
    // Plasticity errors
    PlasticityNotEnabled = 5000,
    STDPParametersInvalid = 5001,
    HebbianParametersInvalid = 5002,
    
    // Development errors
    DevelopmentNotInitialized = 6000,
    InvalidDevelopmentalStage = 6001,
    
    // I/O errors
    FileReadError = 7000,
    FileWriteError = 7001,
    FilePermissionError = 7002,
    
    // System errors
    SystemResourceError = 8000,
    TimeoutError = 8001,
    OperationNotSupported = 8002,
    
    // Generic errors
    UnknownError = 9000,
    OperationFailed = 9001,
    NotImplemented = 9002,
    InvalidParameter = 9003
};

// Detailed error information structure
struct ErrorInfo {
    ErrorCode code;
    ErrorCategory category;
    ErrorSeverity severity;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> context;
    std::shared_ptr<std::exception> exception_ptr;
    
    ErrorInfo(ErrorCode c, ErrorCategory cat, ErrorSeverity sev, 
              const std::string& msg, const std::string& file = "",
              int lineNum = 0, const std::string& func = "")
        : code(c), category(cat), severity(sev), message(msg), 
          file(file), line(lineNum), function(func),
          timestamp(std::chrono::system_clock::now()) {}
};

// Exception hierarchy for different error types
class NLMException : public std::runtime_error {
public:
    ErrorCode code;
    ErrorCategory category;
    ErrorSeverity severity;
    std::string file;
    int line;
    std::string function;
    
    NLMException(ErrorCode c, ErrorCategory cat, ErrorSeverity sev,
                 const std::string& message, 
                 const std::string& file = "", 
                 int lineNum = 0,
                 const std::string& function = "")
        : std::runtime_error(message), code(c), category(cat), 
          severity(sev), file(file), line(lineNum), function(function) {}
};

class ValidationException : public NLMException {
public:
    std::vector<std::string> validation_errors;
    
    ValidationException(const std::vector<std::string>& errors,
                        const std::string& message = "Validation failed")
        : NLMException(ErrorCode::ValidationFailed, ErrorCategory::Validation,
                       ErrorSeverity::Error, message),
          validation_errors(errors) {}
};

class MemoryException : public NLMException {
public:
    size_t requested_size;
    size_t available_size;
    
    MemoryException(size_t req, size_t avail,
                    const std::string& message = "Memory allocation failed")
        : NLMException(ErrorCode::MemoryAllocationFailed, 
                       ErrorCategory::Memory, ErrorSeverity::Critical,
                       message),
          requested_size(req), available_size(avail) {}
};

class ConfigurationException : public NLMException {
public:
    std::string key;
    std::string expected_type;
    
    ConfigurationException(const std::string& k, const std::string& type,
                           const std::string& message = "Configuration error")
        : NLMException(ErrorCode::InvalidConfigValue, 
                       ErrorCategory::Configuration, ErrorSeverity::Error,
                       message), key(k), expected_type(type) {}
};

// Error reporting interface
class IErrorReporter {
public:
    virtual ~IErrorReporter() = default;
    
    virtual void report(const ErrorInfo& error) = 0;
    virtual void report(const std::exception& ex) = 0;
    virtual bool isEnabled(ErrorSeverity minSeverity) const = 0;
    virtual void setLevel(ErrorSeverity level) = 0;
    virtual ErrorSeverity getLevel() const = 0;
};

// Console error reporter
class ConsoleErrorReporter : public IErrorReporter {
public:
    struct Impl {
        ErrorSeverity minSeverity;
        bool useColors;
        bool useTimestamps;
        std::mutex mutex;
    };
    
    std::unique_ptr<Impl> pImpl;
    
    ConsoleErrorReporter(ErrorSeverity severity = ErrorSeverity::Warning,
                         bool colors = true, bool timestamps = true)
        : pImpl(std::make_unique<Impl>()) {
        pImpl->minSeverity = severity;
        pImpl->useColors = colors;
        pImpl->useTimestamps = timestamps;
    }
    
    void report(const ErrorInfo& error) override;
    void report(const std::exception& ex) override;
    bool isEnabled(ErrorSeverity severity) const override {
        return severity >= pImpl->minSeverity;
    }
    void setLevel(ErrorSeverity level) override {
        pImpl->minSeverity = level;
    }
    ErrorSeverity getLevel() const override {
        return pImpl->minSeverity;
    }
};

// File error reporter
class FileErrorReporter : public IErrorReporter {
public:
    std::string filepath;
    std::ofstream fileStream;
    bool append;
    mutable std::mutex mutex;
    
    FileErrorReporter(const std::string& path, bool appendMode = true,
                     ErrorSeverity severity = ErrorSeverity::Error)
        : filepath(path), append(appendMode) {
        std::ios::openmode mode = std::ios::app;
        if (!appendMode) {
            mode = std::ios::trunc | std::ios::out;
        }
        fileStream.open(path, mode);
    }
    
    ~FileErrorReporter() override {
        if (fileStream.is_open()) {
            fileStream.close();
        }
    }
    
    void report(const ErrorInfo& error) override {
        std::lock_guard<std::mutex> lock(mutex);
        
        if (!isEnabled(error.severity)) {
            return;
        }
        
        std::ostringstream oss;
        oss << "[" << levelToString(error.severity) << "]";
        
        auto time = std::chrono::system_clock::to_time_t(error.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        oss << " " << error.message;
        
        fileStream << oss.str() << std::endl;
        if (fileStream.fail()) {
            // Failed to write to file - can't report this error
        }
    }
    
    void report(const std::exception& ex) override {
        ErrorInfo error(ErrorCode::UnknownError, ErrorCategory::System,
                       ErrorSeverity::Error, ex.what());
        report(error);
    }
    
    void setOutputFile(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(mutex);
        if (fileStream.is_open()) {
            fileStream.close();
        }
        fileStream.open(filepath, std::ios::app);
        this->filepath = filepath;
    }
    
    void closeOutputFile() {
        std::lock_guard<std::mutex> lock(mutex);
        if (fileStream.is_open()) {
            fileStream.close();
        }
    }
    
    bool isEnabled(ErrorSeverity severity) const override {
        return severity >= ErrorSeverity::Error;
    }
    
    void setLevel(ErrorSeverity level) override {}
    
    ErrorSeverity getLevel() const override {
        return ErrorSeverity::Error;
    }
};

// Error reporting manager
class ErrorManager {
public:
    static ErrorManager& getInstance() {
        static ErrorManager instance;
        return instance;
    }
    
    void addReporter(std::shared_ptr<IErrorReporter> reporter);
    void removeReporter(const IErrorReporter* reporter);
    void clearReporters();
    
    void report(const ErrorInfo& error);
    void report(const std::exception& ex);
    void report(ErrorCode code, ErrorCategory category, ErrorSeverity severity,
                const std::string& message, const std::string& file = "",
                int line = 0, const std::string& function = "",
                const std::unordered_map<std::string, std::string>& context = {});
    
    void setGlobalLevel(ErrorSeverity level);
    ErrorSeverity getGlobalLevel() const { return globalLevel; }
    
    std::string getLastErrorMessage() const { return lastErrorMessage; }
    
    // Validation helpers
    template<typename T>
    void validateNotNull(const T& value, ErrorCode code = ErrorCode::InvalidParameter,
                        const std::string& message = "Value cannot be null") {
        if (!value) {
            report(code, ErrorCategory::Validation, ErrorSeverity::Error, message);
        }
    }
    
    template<typename T>
    void validateRange(const T& value, const T& min, const T& max,
                      ErrorCode code = ErrorCode::OutOfRange,
                      const std::string& message = "Value out of range") {
        if (value < min || value > max) {
            std::stringstream ss;
            ss << message << ": " << value << " not in [" << min << ", " << max << "]";
            report(code, ErrorCategory::Validation, ErrorSeverity::Warning, ss.str());
        }
    }
    
    template<typename T>
    void validatePositive(const T& value, ErrorCode code = ErrorCode::InvalidParameter,
                         const std::string& message = "Value must be positive") {
        if (value <= 0) {
            report(code, ErrorCategory::Validation, ErrorSeverity::Error, message);
        }
    }
    
    template<typename T>
    void validateNonZero(const T& value, ErrorCode code = ErrorCode::InvalidParameter,
                        const std::string& message = "Value cannot be zero") {
        if (value == 0) {
            report(code, ErrorCategory::Validation, ErrorSeverity::Error, message);
        }
    }
    
private:
    ErrorManager() : globalLevel(ErrorSeverity::Warning), lastErrorMessage("") {}
    ~ErrorManager() = default;
    ErrorManager(const ErrorManager&) = delete;
    ErrorManager& operator=(const ErrorManager&) = delete;
    
    std::vector<std::shared_ptr<IErrorReporter>> reporters;
    ErrorSeverity globalLevel;
    std::string lastErrorMessage;
    mutable std::mutex mutex;
};

// Exception-safe resource management
class ResourceGuard {
public:
    template<typename F>
    ResourceGuard(F&& cleanupFunc) : cleanup(std::forward<F>(cleanupFunc)), active(true) {}
    
    ~ResourceGuard() {
        if (active && cleanup) {
            try {
                cleanup();
            } catch (...) {
                ErrorManager::getInstance().report(ErrorCode::UnknownError,
                                                 ErrorCategory::System,
                                                 ErrorSeverity::Error,
                                                 "Exception in resource cleanup");
            }
        }
    }
    
    void disarm() { active = false; }
    
    ResourceGuard(const ResourceGuard&) = delete;
    ResourceGuard& operator=(const ResourceGuard&) = delete;
    
private:
    std::function<void()> cleanup;
    bool active;
};

// Transaction-like operation support
class TransactionGuard {
public:
    TransactionGuard(const std::string& description = "transaction")
        : description(description), rolledBack(false) {}
    
    virtual ~TransactionGuard() {
        if (!rolledBack) {
            try {
                rollback();
            } catch (...) {
                ErrorManager::getInstance().report(ErrorCode::UnknownError,
                                                 ErrorCategory::System,
                                                 ErrorSeverity::Error,
                                                 "Exception in transaction rollback");
            }
        }
    }
    
    virtual void commit() {
        rolledBack = false; // Mark as committed
    }
    
    virtual void rollback() {
        rolledBack = true;
    }
    
    bool isRolledBack() const { return rolledBack; }
    
    TransactionGuard(const TransactionGuard&) = delete;
    TransactionGuard& operator=(const TransactionGuard&) = delete;
    
protected:
    std::string description;
    bool rolledBack;
};

// Validation utilities
class ValidationUtils {
public:
    template<typename T>
    static bool isInRange(const T& value, const T& min, const T& max) {
        return value >= min && value <= max;
    }
    
    template<typename T>
    static bool isPositive(const T& value) {
        return value > 0;
    }
    
    template<typename T>
    static bool isNonZero(const T& value) {
        return value != 0;
    }
    
    static bool isNotNull(const void* ptr) {
        return ptr != nullptr;
    }
    
    template<typename Container>
    static bool isNotEmpty(const Container& container) {
        return !container.empty();
    }
    
    template<typename T>
    static bool isFinite(const T& value) {
        return std::isfinite(static_cast<double>(value));
    }
    
    static bool isValidNeuronId(const NeuronId& id);
    static bool isValidSynapseId(const SynapseId& id);
    static bool isValidRegionId(const RegionId& id);
    
    template<typename Iterator>
    static bool isValidIterator(Iterator begin, Iterator end, Iterator pos) {
        return pos >= begin && pos <= end;
    }
    
    static std::vector<std::string> validateVector(const std::vector<float>& values,
                                                  float min = std::numeric_limits<float>::lowest(),
                                                  float max = std::numeric_limits<float>::max());
};

// Convenience macros for error reporting
#define NLM_REPORT_ERROR(code, category, severity, msg) \
    do { \
        nlm::ErrorInfo error(code, category, severity, msg, __FILE__, __LINE__, __func__); \
        nlm::ErrorManager::getInstance().report(error); \
    } while(0)

#define NLM_REPORT_EXCEPTION(ex) \
    do { \
        nlm::ErrorManager::getInstance().report(ex); \
    } while(0)

#define NLM_THROW_ERROR(code, category, severity, msg) \
    throw nlm::NLMException(code, category, severity, msg, __FILE__, __LINE__, __func__)

#define NLM_THROW_VALIDATION(errors, msg) \
    throw nlm::ValidationException(errors, msg)

#define NLM_VALIDATE(cond, error_code, message) \
    if (!(cond)) { \
        NLM_THROW_ERROR(error_code, nlm::ErrorCategory::Validation, nlm::ErrorSeverity::Error, message); \
    }

#define NLM_SAFE_CALL(func, error_msg) \
    do { \
        try { func; } catch (const std::exception& e) { \
            NLM_THROW_ERROR(nlm::ErrorCode::OperationFailed, nlm::ErrorCategory::System, nlm::ErrorSeverity::Error, error_msg + std::string(" - ") + e.what()); \
        } \
    } while(0)

// Global initialization
class ErrorHandlingSystem {
public:
    static void initialize() {
        ErrorManager::getInstance().setGlobalLevel(ErrorSeverity::Warning);
    }
    
    static void shutdown() {
        ErrorManager::getInstance().clearReporters();
    }
};

} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP
