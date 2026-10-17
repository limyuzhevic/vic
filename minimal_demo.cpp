// Minimal, working example of the NLM error handling system

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <chrono>

// Simple error handling system namespace nlm {

// Error code categories
enum class ErrorCategory : uint8_t {
    None = 0,
    Configuration = 1,
    NeuralComputation = 2,
    Memory = 3,
    IO = 4,
    Validation = 5,
    Resource = 6,
    System = 7,
    Integration = 8,
    Recovery = 9
};

// Detailed error codes
enum class ErrorCode : uint32_t {
    Unknown = 0xFFFF,
    InvalidArgument = 0xFFFE,
    RuntimeError = 0xFFFD,
    NotImplemented = 0xFFFC,
    
    // Common error codes
    ConfigValidationFailed = 0x1000,
    NeuronInvalidState = 0x2000,
    MemoryAllocationFailed = 0x3000,
    FileNotFound = 0x4000,
    ParameterInvalid = 0x5000,
    ResourceExhausted = 0x6000,
    IntegrationFailed = 0x8000,
    RecoveryFailed = 0x9000
};

// Error context structure
struct ErrorContext {
    std::string message;
    ErrorCode code;
    ErrorCategory category;
    std::string sourceFile;
    int sourceLine;
    std::string sourceFunction;
    std::chrono::system_clock::time_point timestamp;
    std::string recoveryAction;
    std::map<std::string, std::string> metadata;
    
    ErrorContext() : code(ErrorCode::Unknown), category(ErrorCategory::None), sourceLine(0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    ErrorContext(const std::string& msg, ErrorCode ec, ErrorCategory cat)
        : message(msg), code(ec), category(cat), sourceLine(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Base exception class
class NLMException : public std::exception {
public:
    NLMException(const ErrorContext& context)
        : context_(context), fullMessage_(getFullMessage(context)) {}
    
    virtual ~NLMException() noexcept {}
    
    const char* what() const noexcept override {
        return fullMessage_.c_str();
    }
    
    // Get error context
    const ErrorContext& getContext() const {
        return context_;
    }
    
    // Get error code
    ErrorCode getCode() const {
        return context_.code;
    }
    
    // Get error category
    ErrorCategory getCategory() const {
        return context_.category;
    }
    
protected:
    ErrorContext context_;
    std::string fullMessage_;
    
    static std::string getFullMessage(const ErrorContext& ctx) {
        return "[" + std::to_string(static_cast<int>(ctx.category)) + "] "
               + "0x" + std::to_string(static_cast<int>(ctx.code)) + ": "
               + ctx.message;
    }
};

// Specific exception types
class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& message, ErrorCode code = ErrorCode::ConfigValidationFailed)
        : NLMException(ErrorContext(message, code, ErrorCategory::Configuration)) {}
};

class ValidationException : public NLMException {
public:
    ValidationException(const std::string& message, ErrorCode code = ErrorCode::ParameterInvalid)
        : NLMException(ErrorContext(message, code, ErrorCategory::Validation)) {}
};

class IOError : public NLMException {
public:
    IOError(const std::string& message, ErrorCode code = ErrorCode::FileNotFound)
        : NLMException(ErrorContext(message, code, ErrorCategory::IO)) {}
};

// Macros for easy exception throwing
#define NLM_THROW_CONFIG(msg, code) throw nlm::ConfigurationException(msg, code)
#define NLM_THROW_VALIDATION(msg, code) throw nlm::ValidationException(msg, code)
#define NLM_THROW_IO(msg, code) throw nlm::IOError(msg, code)

// Global error reporting
class ErrorReporter {
public:
    static void reportError(const std::string& message, ErrorCode code = ErrorCode::Unknown,
                           ErrorCategory category = ErrorCategory::None) {
        ErrorContext context(message, code, category);
        std::cout << "[ERROR] " << context.message << std::endl;
    }
    
    static void reportException(const std::exception& e) {
        std::cout << "[EXCEPTION] " << e.what() << std::endl;
    }
};

// Simple validation utilities
class ValidationUtils {
public:
    static bool isInRange(double value, double min, double max) {
        return value >= min && value <= max;
    }
    
    static bool isNotEmpty(const std::string& value) {
        return !value.empty();
    }
    
    static bool isNotEmpty(const std::vector<std::string>& value) {
        return !value.empty();
    }
};

} // namespace nlm

int main() {
    std::cout << "=== NLM Error Handling System Demo ===" << std::endl << std::endl;
    
    // Demonstrate error handling
    std::cout << "1. Testing Configuration Exception..." << std::endl;
    try {
        NLM_THROW_CONFIG("Invalid configuration key", nlm::ErrorCode::ConfigValidationFailed);
    } catch (const nlm::NLMException& e) {
        std::cout << "   Caught: " << e.what() << std::endl;
    }
    
    std::cout << "\n2. Testing Validation Exception..." << std::endl;
    try {
        NLM_THROW_VALIDATION("Invalid parameter value", nlm::ErrorCode::ParameterInvalid);
    } catch (const nlm::NLMException& e) {
        std::cout << "   Caught: " << e.what() << std::endl;
    }
    
    std::cout << "\n3. Testing I/O Exception..." << std::endl;
    try {
        NLM_THROW_IO("File not found", nlm::ErrorCode::FileNotFound);
    } catch (const nlm::NLMException& e) {
        std::cout << "   Caught: " << e.what() << std::endl;
    }
    
    std::cout << "\n4. Testing Error Reporting..." << std::endl;
    nlm::ErrorReporter::reportError("Configuration file not found", 
                                   nlm::ErrorCode::ConfigValidationFailed,
                                   nlm::ErrorCategory::Configuration);
    
    std::cout << "\n5. Testing Validation Utilities..." << std::endl;
    bool inRange = nlm::ValidationUtils::isInRange(50.0, 0.0, 100.0);
    std::cout << "   50 in [0, 100]: " << (inRange ? "true" : "false") << std::endl;
    
    bool notEmpty = nlm::ValidationUtils::isNotEmpty(std::string("test"));
    std::cout << "   String 'test' is not empty: " << (notEmpty ? "true" : "false") << std::endl;
    
    std::cout << "\n=== All demonstrations completed successfully! ===" << std::endl;
    
    return 0;
}
