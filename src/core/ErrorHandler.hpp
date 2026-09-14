#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Error handling for NLM neural network operations
enum class ErrorCode {
    Success = 0,
    ComputationOverflow = 1,
    ComputationUnderflow = 2,
    InvalidParameter = 3,
    MemoryAllocationFailed = 4,
    FileIOError = 5,
    ParseError = 6,
    ValidationError = 7,
    SerializationError = 8,
    NetworkError = 9,
    ConcurrencyError = 10,
    ResourceExhausted = 11,
    NotImplemented = 12,
    Unknown = 13
};

struct ErrorInfo {
    ErrorCode code;
    std::string message;
    std::string file;
    int line;
    std::string function;
    LogLevel severity;
    int neuronId;
    int synapseId;
    int regionId;
    SimulationStep step;
    Timestamp time;
    
    ErrorInfo() 
        : code(ErrorCode::Unknown), message("Unknown error"), file("")
        , line(0), function(""), severity(LogLevel::Error), neuronId(-1)
        , synapseId(-1), regionId(-1), step(0), time(0.0) {}
        
    ErrorInfo(ErrorCode c, const std::string& msg)
        : code(c), message(msg), file("")
        , line(0), function(""), severity(LogLevel::Error), neuronId(-1)
        , synapseId(-1), regionId(-1), step(0), time(0.0) {}
        
    ErrorInfo(ErrorCode c, const std::string& msg, LogLevel sev)
        : code(c), message(msg), file("")
        , line(0), function(""), severity(sev), neuronId(-1)
        , synapseId(-1), regionId(-1), step(0), time(0.0) {}
};

// Exception-safe error handling class for neural network operations
class ErrorHandler {
private:
    std::vector<ErrorInfo> errorHistory;
    std::mutex errorMutex;
    static ErrorHandler* instance;
    
public:
    static ErrorHandler& getInstance() {
        if (!instance) {
            instance = new ErrorHandler();
        }
        return *instance;
    }
    
    // Log error with context
    void logError(ErrorCode code, const std::string& message,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "",
                  LogLevel severity = LogLevel::Error,
                  int neuronId = -1, int synapseId = -1,
                  int regionId = -1,
                  SimulationStep step = 0, Timestamp time = 0.0) {
        std::lock_guard<std::mutex> lock(errorMutex);
        
        ErrorInfo error;
        error.code = code;
        error.message = message;
        error.file = file;
        error.line = line;
        error.function = function;
        error.severity = severity;
        error.neuronId = neuronId;
        error.synapseId = synapseId;
        error.regionId = regionId;
        error.step = step;
        error.time = time;
        
        errorHistory.push_back(error);
        
        // Also log to logger for immediate output
        std::ostringstream oss;
        oss << "[NEURON:" << neuronId << "][SYN:" << synapseId << "] " << message;
        Logger::getGlobal().log(severity, oss.str(), file, line, function);
    }
    
    // Log warning
    void logWarning(ErrorCode code, const std::string& message,
                    const std::string& file = "", int line = 0,
                    const std::string& function = "") {
        logError(code, message, file, line, function, LogLevel::Warning);
    }
    
    // Log info
    void logInfo(ErrorCode code, const std::string& message,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "") {
        logError(code, message, file, line, function, LogLevel::Info);
    }
    
    // Check if error is recoverable
    bool isRecoverable(ErrorCode code) const {
        switch (code) {
            case ErrorCode::ComputationOverflow:
            case ErrorCode::ComputationUnderflow:
            case ErrorCode::InvalidParameter:
            case ErrorCode::ValidationError:
                return true; // Can recover by reducing load or using defaults
            case ErrorCode::MemoryAllocationFailed:
            case ErrorCode::ResourceExhausted:
                return false; // Cannot recover without external resources
            case ErrorCode::FileIOError:
                return true; // Can recover with fallback or retry
            case ErrorCode::ParseError:
                return true; // Can recover with defaults
            case ErrorCode::SerializationError:
                return false; // Cannot recover corrupted data
            case ErrorCode::NetworkError:
                return true; // Can recover with retry
            case ErrorCode::ConcurrencyError:
                return true; // Can recover with backoff
            case ErrorCode::Success:
            case ErrorCode::NotImplemented:
            default:
                return false;
        }
    }
    
    // Get error history
    const std::vector<ErrorInfo>& getErrorHistory() const { return errorHistory; }
    
    // Clear error history
    void clearHistory() {
        std::lock_guard<std::mutex> lock(errorMutex);
        errorHistory.clear();
    }
    
    // Check if any critical errors occurred
    bool hasCriticalErrors() const {
        for (const auto& error : errorHistory) {
            if (error.severity == LogLevel::Critical || error.severity == LogLevel::Error) {
                return true;
            }
        }
        return false;
    }
    
    // Helper functions
    static std::string getCurrentTimestamp();
    static std::string levelToString(LogLevel level);
    static void recoverFromError(ErrorCode errorCode);
    static void saveErrorHistory(const std::string& filepath);
    static std::string generateErrorReport();
    
    // Private constructor for singleton pattern
    ErrorHandler() {}
    ~ErrorHandler() {}
    
    // Disable copying and assignment
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;
};

} // namespace nlm