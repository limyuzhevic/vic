#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <sstream>
#include <stdexcept>

namespace nlm {

// Error codes for NLM operations
enum class ErrorCode {
    Success = 0,
    NullPointer = 1,
    FileNotFound = 2,
    FilePermissionDenied = 3,
    FileIOError = 4,
    InvalidFormat = 5,
    MemoryAllocationFailed = 6,
    OutOfRange = 7,
    ValidationFailed = 8,
    OperationNotSupported = 9,
    ConfigurationError = 10,
    SystemError = 11,
    Unknown = 12
};

// Error class for detailed error reporting
class NLMError {
public:
    NLMError(ErrorCode code, const std::string& message, 
             const std::string& file = "", int line = 0,
             const std::string& function = "")
        : code(code), message(message), file(file), 
          line(line), function(function), 
          timestamp(std::chrono::system_clock::now()) {}
    
    ErrorCode getCode() const { return code; }
    const std::string& getMessage() const { return message; }
    const std::string& getFile() const { return file; }
    int getLine() const { return line; }
    const std::string& getFunction() const { return function; }
    auto getTimestamp() const { return timestamp; }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Error " << static_cast<int>(code) << ": " << message;
        if (!file.empty()) oss << " in " << file;
        if (line > 0) oss << ":" << line;
        if (!function.empty()) oss << " (" << function << ")";
        return oss.str();
    }
    
private:
    ErrorCode code;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
};

// Error reporting system
class ErrorReporter {
public:
    static ErrorReporter& getInstance() {
        static ErrorReporter instance;
        return instance;
    }
    
    void report(const NLMError& error) {
        errors.push_back(error);
        std::cerr << "NLM ERROR: " << error.toString() << std::endl;
    }
    
    const std::vector<NLMError>& getErrors() const { return errors; }
    void clear() { errors.clear(); }
    
    static NLMError nullPointerError(const std::string& name, 
                                     const std::string& file = "", 
                                     int line = 0, 
                                     const std::string& function = "") {
        return NLMError(ErrorCode::NullPointer, 
                       "Null pointer access to: " + name, 
                       file, line, function);
    }
    
    static NLMError fileNotFoundError(const std::string& path,
                                      const std::string& file = "",
                                      int line = 0,
                                      const std::string& function = "") {
        return NLMError(ErrorCode::FileNotFound, 
                       "File not found: " + path,
                       file, line, function);
    }
    
    static NLMError fileIOError(const std::string& path,
                                const std::string& operation,
                                const std::string& file = "",
                                int line = 0,
                                const std::string& function = "") {
        return NLMError(ErrorCode::FileIOError,
                       "File I/O error during " + operation + 
                       " on: " + path,
                       file, line, function);
    }
    
    static NLMError memoryAllocationError(const std::string& object,
                                           const std::string& file = "",
                                           int line = 0,
                                           const std::string& function = "") {
        return NLMError(ErrorCode::MemoryAllocationFailed,
                       "Memory allocation failed for: " + object,
                       file, line, function);
    }
    
    static NLMError validationError(const std::string& field,
                                    const std::string& value,
                                    const std::string& file = "",
                                    int line = 0,
                                    const std::string& function = "") {
        return NLMError(ErrorCode::ValidationFailed,
                       "Validation failed for " + field + ": " + value,
                       file, line, function);
    }
    
private:
    ErrorReporter() = default;
    std::vector<NLMError> errors;
};

} // namespace nlm