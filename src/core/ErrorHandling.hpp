#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include <string>
#include <stdexcept>
#include <map>
#include <chrono>
#include <functional>

namespace nlm {

enum class ErrorCode {
    Success = 0,
    NullPointer = 1,
    InvalidParameter = 2,
    FileNotFound = 3,
    ConfigurationError = 4,
    MemoryError = 5,
    InitializationError = 6,
    ValidationError = 7,
    NetworkError = 8,
    Timeout = 9,
    NotImplemented = 10,
    PermissionError = 11,
    FormatError = 12,
    Overflow = 13,
    DivisionByZero = 14,
    OutOfRange = 15,
    ConcurrencyError = 16,
    ResourceExhausted = 17,
    Unknown = 18
};

class NLMError : public std::runtime_error {
public:
    NLMError(ErrorCode code, const std::string& message, 
             const std::string& context = "", 
             const std::string& file = "", 
             int line = 0)
        : std::runtime_error(message)
        , code(code)
        , context(context)
        , file(file)
        , line(line)
        , timestamp(std::chrono::system_clock::now()) {
    }
    
    ErrorCode getCode() const { return code; }
    const std::string& getContext() const { return context; }
    const std::string& getFile() const { return file; }
    int getLine() const { return line; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp; }
    
    std::string getFullMessage() const {
        std::ostringstream oss;
        oss << "Error Code: " << static_cast<int>(code) << "\n";
        oss << "Message: " << what() << "\n";
        if (!context.empty()) {
            oss << "Context: " << context << "\n";
        }
        if (!file.empty()) {
            oss << "Location: " << file;
            if (line > 0) {
                oss << ":" << line;
            }
            oss << "\n";
        }
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        oss << "Time: " << std::ctime(&time);
        return oss.str();
    }
    
    static std::string errorCodeToString(ErrorCode code) {
        static const std::map<ErrorCode, std::string> errorStrings = {
            {ErrorCode::Success, "Success"},
            {ErrorCode::NullPointer, "Null pointer error"},
            {ErrorCode::InvalidParameter, "Invalid parameter"},
            {ErrorCode::FileNotFound, "File not found"},
            {ErrorCode::ConfigurationError, "Configuration error"},
            {ErrorCode::MemoryError, "Memory allocation error"},
            {ErrorCode::InitializationError, "Initialization error"},
            {ErrorCode::ValidationError, "Validation error"},
            {ErrorCode::NetworkError, "Network error"},
            {ErrorCode::Timeout, "Operation timeout"},
            {ErrorCode::NotImplemented, "Not implemented"},
            {ErrorCode::PermissionError, "Permission denied"},
            {ErrorCode::FormatError, "Format error"},
            {ErrorCode::Overflow, "Numeric overflow"},
            {ErrorCode::DivisionByZero, "Division by zero"},
            {ErrorCode::OutOfRange, "Value out of range"},
            {ErrorCode::ConcurrencyError, "Concurrency error"},
            {ErrorCode::ResourceExhausted, "Resource exhausted"},
            {ErrorCode::Unknown, "Unknown error"}
        };
        
        auto it = errorStrings.find(code);
        return (it != errorStrings.end()) ? it->second : "Unknown error";
    }

private:
    ErrorCode code;
    std::string context;
    std::string file;
    int line;
    std::chrono::system_clock::time_point timestamp;
};

class ErrorHandler {
public:
    using ErrorCallback = std::function<void(const NLMError&)>;
    
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    void setErrorCallback(ErrorCallback callback) {
        errorCallback = callback;
    }
    
    void handleError(const NLMError& error) {
        if (errorCallback) {
            errorCallback(error);
        } else {
            // Default error handling
            std::cerr << "NLM Error: " << error.getFullMessage() << std::endl;
        }
    }
    
    void handleError(ErrorCode code, const std::string& message,
                     const std::string& context = "", 
                     const std::string& file = "", 
                     int line = 0) {
        handleError(NLMError(code, message, context, file, line));
    }
    
    template<typename T>
    T validateOrDefault(const T& value, const T& defaultValue,
                        const std::string& validationMsg = "Validation failed",
                        ErrorCode errorCode = ErrorCode::ValidationError) {
        if (!isValid(value)) {
            handleError(errorCode, validationMsg);
            return defaultValue;
        }
        return value;
    }
    
    template<typename T>
    bool validate(const T& value, const std::string& validationMsg = "Validation failed",
                  ErrorCode errorCode = ErrorCode::ValidationError) {
        if (!isValid(value)) {
            handleError(errorCode, validationMsg);
            return false;
        }
        return true;
    }
    
    template<typename T>
    bool isValid(const T& value) {
        return true; // Default implementation, can be specialized
    }
    
    // Specializations for common types
    bool isValid(const std::string& value) {
        return !value.empty();
    }
    
    bool isValid(float value) {
        return !std::isnan(value) && !std::isinf(value);
    }
    
    bool isValid(int value) {
        return true; // Basic int validation
    }
    
    bool isValid(const std::vector<float>& values) {
        return std::none_of(values.begin(), values.end(),
                           [](float v) { return std::isnan(v) || std::isinf(v); });
    }
    
    std::string lastErrorMessage() const {
        return lastError.empty() ? "No error" : lastError;
    }
    
    ErrorCode lastErrorCode() const {
        return lastErrorCode_;
    }
    
    bool hasError() const {
        return lastErrorCode_ != ErrorCode::Success;
    }
    
    void clearError() {
        lastError.clear();
        lastErrorCode_ = ErrorCode::Success;
    }
    
private:
    ErrorHandler() : lastErrorCode_(ErrorCode::Success) {}
    
    ErrorCallback errorCallback;
    std::string lastError;
    ErrorCode lastErrorCode_;
};

// Helper macros for error handling
#define NLM_VALIDATE(expr, msg, code) \
    if (!(expr)) { \
        nlm::ErrorHandler::getInstance().handleError(code, msg, __func__, __FILE__, __LINE__); \
        return false; \
    }

#define NLM_VALIDATE_THROW(expr, msg, code) \
    if (!(expr)) { \
        throw nlm::NLMError(code, msg, __func__, __FILE__, __LINE__); \
    }

#define NLM_CHECK_NULL(ptr, msg) \
    if ((ptr) == nullptr) { \
        nlm::ErrorHandler::getInstance().handleError(nlm::ErrorCode::NullPointer, msg, __func__, __FILE__, __LINE__); \
        return false; \
    }

#define NLM_CHECK_RANGE(value, min, max, msg) \
    if ((value) < (min) || (value) > (max)) { \
        nlm::ErrorHandler::getInstance().handleError(nlm::ErrorCode::OutOfRange, msg, __func__, __FILE__, __LINE__); \
        return false; \
    }

// Exception throwing macros
#define NLM_THROW(expr, msg, code) \
    if (!(expr)) { \
        throw nlm::NLMError(code, msg, __func__, __FILE__, __LINE__); \
    }

// Memory allocation error checking
void* safe_malloc(size_t size, const std::string& context = "");
void safe_free(void* ptr, const std::string& context = "");

// File operations with error handling
bool safe_file_exists(const std::string& path);
bool safe_write_file(const std::string& path, const std::string& content);
std::string safe_read_file(const std::string& path);

} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP