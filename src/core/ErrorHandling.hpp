#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include <string>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>

namespace nlm {

// Error categories for better error classification
enum class ErrorCategory {
    Configuration,
    Initialization,
    Simulation,
    Memory,
    IO,
    Network,
    Validation,
    Development,
    Unknown
};

// Detailed error information
struct ErrorInfo {
    ErrorCategory category;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::unordered_map<std::string, std::string> context;
    std::shared_ptr<std::exception> originalException;
    
    ErrorInfo(ErrorCategory cat, const std::string& msg) 
        : category(cat), message(msg), line(0) {}
};

// Custom exception class for NLM errors
class NLMException : public std::runtime_error {
public:
    ErrorInfo info;
    
    NLMException(ErrorCategory category, const std::string& message)
        : std::runtime_error(message), info(category, message) {}
    
    NLMException(ErrorCategory category, const std::string& message,
                 const std::string& file, int line, const std::string& function)
        : std::runtime_error(message), info(category, message, file, line, function) {}
};

// Error handler class for managing and reporting errors
class ErrorHandler {
private:
    std::vector<ErrorInfo> errorHistory;
    bool throwOnError;
    bool logToConsole;
    bool logToFile;
    std::string logFilePath;
    std::function<void(const ErrorInfo&)> customLogger;
    
public:
    ErrorHandler() : throwOnError(true), logToConsole(true), logToFile(false) {}
    
    void setThrowOnError(bool throwErrors) { throwOnError = throwErrors; }
    void setLogToConsole(bool log) { logToConsole = log; }
    void setLogToFile(bool log, const std::string& filepath) { 
        logToFile = log; 
        logFilePath = filepath; 
    }
    
    void setCustomLogger(std::function<void(const ErrorInfo&)> logger) {
        customLogger = logger;
    }
    
    void addError(ErrorCategory category, const std::string& message,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "") {
        ErrorInfo error(category, message);
        error.file = file;
        error.line = line;
        error.function = function;
        
        errorHistory.push_back(error);
        
        // Log the error
        logError(error);
        
        // Throw if configured
        if (throwOnError) {
            throw NLMException(category, message);
        }
    }
    
    void logError(const ErrorInfo& error) {
        std::string errorMsg = formatError(error);
        
        if (logToConsole) {
            std::cerr << "NLM ERROR: " << errorMsg << std::endl;
        }
        
        if (logToFile) {
            std::ofstream file(logFilePath, std::ios::app);
            if (file.is_open()) {
                file << "NLM ERROR: " << errorMsg << std::endl;
            }
        }
        
        if (customLogger) {
            customLogger(error);
        }
    }
    
    std::string formatError(const ErrorInfo& error) {
        std::stringstream ss;
        ss << "[" << errorToString(error.category) << "] " << error.message;
        
        if (!error.file.empty()) {
            ss << " (at " << error.file << ":" << error.line << ")";
        }
        
        if (!error.context.empty()) {
            ss << " Context: ";
            bool first = true;
            for (const auto& pair : error.context) {
                if (!first) ss << ", ";
                ss << pair.first << "=" << pair.second;
                first = false;
            }
        }
        
        return ss.str();
    }
    
    static std::string errorToString(ErrorCategory category) {
        switch (category) {
            case ErrorCategory::Configuration: return "CONFIG";
            case ErrorCategory::Initialization: return "INIT";
            case ErrorCategory::Simulation: return "SIM";
            case ErrorCategory::Memory: return "MEMORY";
            case ErrorCategory::IO: return "IO";
            case ErrorCategory::Network: return "NETWORK";
            case ErrorCategory::Validation: return "VALIDATION";
            case ErrorCategory::Development: return "DEV";
            default: return "UNKNOWN";
        }
    }
    
    void clearErrors() { errorHistory.clear(); }
    
    size_t getErrorCount() const { return errorHistory.size(); }
    
    const std::vector<ErrorInfo>& getErrorHistory() const { return errorHistory; }
    
    bool hasErrors() const { return !errorHistory.empty(); }
}; 

// Global error handler instance
extern ErrorHandler globalErrorHandler;

// Convenience error reporting functions
#define NLM_THROW_ERROR(category, message) \
    globalErrorHandler.addError(category, message, __FILE__, __LINE__, __func__)

#define NLM_CHECK(condition, category, message) \
    if (!(condition)) { globalErrorHandler.addError(category, message, __FILE__, __LINE__, __func__); }

#define NLM_TRY Catch and handle errors with try-catch blocks around NLM code

// Configuration validation helper functions
namespace Validation {
    
    bool validateConfig(const std::shared_ptr<Config>& config);
    bool validateSimulationParameters(const std::shared_ptr<Config>& config);
    bool validateMemoryLimits(size_t neuronCount, size_t regionCount);
    bool validateFilePath(const std::string& filepath);
    
    // Configuration validation helpers
    bool checkPositive(const std::string& name, float value);
    bool checkRange(const std::string& name, float value, float min, float max);
    bool checkNotNull(const std::string& name, void* ptr);
    
} // namespace Validation

} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP