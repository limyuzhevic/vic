// ErrorHandlingUtilities.hpp - Helper utilities for error handling
#pragma once

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <unordered_map>
#include "ErrorHandling.hpp"

namespace nlm {

// Error context for enhanced error reporting
class ErrorContext {
public:
    ErrorContext(const std::string& operation = "") : operation_(operation) {
        startTime_ = std::chrono::system_clock::now();
    }
    
    void setOperation(const std::string& operation) {
        operation_ = operation;
    }
    
    void addParameter(const std::string& name, const std::string& value) {
        parameters_[name] = value;
    }
    
    void addParameter(const std::string& name, int64_t value) {
        parameters_[name] = std::to_string(value);
    }
    
    void addParameter(const std::string& name, float value) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(6) << value;
        parameters_[name] = ss.str();
    }
    
    void addParameter(const std::string& name, double value) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(10) << value;
        parameters_[name] = ss.str();
    }
    
    template<typename T>
    void addParameter(const std::string& name, const std::vector<T>& value) {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < value.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << value[i];
        }
        ss << "]";
        parameters_[name] = ss.str();
    }
    
    std::string getFormattedContext() const {
        std::ostringstream oss;
        if (!operation_.empty()) {
            oss << "Operation: " << operation_;
        }
        
        for (const auto& pair : parameters_) {
            if (!operation_.empty()) {
                oss << ", ";
            }
            oss << pair.first << "=" << pair.second;
        }
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - startTime_);
        oss << ", Duration: " << duration.count() << "ms";
        
        return oss.str();
    }
    
private:
    std::string operation_;
    std::unordered_map<std::string, std::string> parameters_;
    std::chrono::system_clock::time_point startTime_;
};

// Enhanced error formatter
class ErrorFormatter {
public:
    static std::string formatError(const NLMException& error) {
        std::ostringstream oss;
        
        oss << "=== NLM Error ===\n";
        oss << "Code: " << static_cast<int>(error.getCode()) << "\n";
        oss << "Message: " << error.what() << "\n";
        
        if (!error.getFile().empty()) {
            oss << "File: " << error.getFile();
            if (error.getLine() > 0) oss << ":" << error.getLine();
            oss << "\n";
        }
        
        if (!error.getFunction().empty()) {
            oss << "Function: " << error.getFunction() << "\n";
        }
        
        oss << "Timestamp: " << getCurrentTimestamp() << "\n";
        
        return oss.str();
    }
    
    static std::string formatResultError(const Result<void>& result) {
        std::ostringstream oss;
        oss << "Operation failed with code: " << static_cast<int>(result.errorCode())
            << ". " << result.errorMessage();
        if (!result.getFile().empty()) {
            oss << " (at " << result.getFile();
            if (result.getLine() > 0) oss << ":" << result.getLine();
            if (!result.getFunction().empty()) oss << " in " << result.getFunction();
            oss << ")";
        }
        return oss.str();
    }
    
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
};

// Error recovery utilities
class ErrorRecovery {
public:
    enum class RecoveryAction {
        None,
        Retry,
        Fallback,
        Cleanup,
        Abort
    };
    
    static RecoveryAction getRecoveryAction(ErrorCode code) {
        // Determine recovery action based on error code
        switch (code) {
            case ErrorCode::MemoryAllocationFailed:
            case ErrorCode::OutOfMemory:
                return RecoveryAction::Cleanup;
                
            case ErrorCode::ConfigInvalidParameter:
            case ErrorCode::ConfigMissingRequiredField:
                return RecoveryAction::Fallback;
                
            case ErrorCode::BrainInitializationFailed:
            case ErrorCode::SimulationNotInitialized:
                return RecoveryAction::Abort;
                
            case ErrorCode::CheckpointSaveFailed:
            case ErrorCode::CheckpointLoadFailed:
                return RecoveryAction::Retry;
                
            case ErrorCode::ExternalSystemError:
            case ErrorCode::FileSystemError:
                return RecoveryAction::Fallback;
                
            default:
                return RecoveryAction::None;
        }
    }
    
    static std::string getRecoveryStrategy(ErrorCode code) {
        auto action = getRecoveryAction(code);
        switch (action) {
            case RecoveryAction::Retry:
                return "Operation will be retried with different parameters";
            case RecoveryAction::Fallback:
                return "Falling back to default configuration";
            case RecoveryAction::Cleanup:
                return "Performing cleanup operations";
            case RecoveryAction::Abort:
                return "Operation aborted - critical failure";
            default:
                return "No recovery action specified";
        }
    }
};

// Exception translator for C-style error handling
class ExceptionTranslator {
public:
    static std::runtime_error translateToRuntimeError(ErrorCode code, const std::string& message) {
        return NLMException(code, message);
    }
    
    static std::string translateErrorCode(ErrorCode code) {
        // Convert error codes to human-readable messages
        std::string category;
        std::string description;
        
        // Extract category from error code
        int codeValue = static_cast<int>(code);
        if (codeValue >= 1000 && codeValue < 2000) {
            category = "Configuration";
        } else if (codeValue >= 2000 && codeValue < 3000) {
            category = "Memory";
        } else if (codeValue >= 3000 && codeValue < 4000) {
            category = "Brain/Neural";
        } else if (codeValue >= 4000 && codeValue < 5000) {
            category = "Simulation";
        } else if (codeValue >= 5000 && codeValue < 6000) {
            category = "Memory Systems";
        } else if (codeValue >= 6000 && codeValue < 7000) {
            category = "Neuromodulation";
        } else if (codeValue >= 7000 && codeValue < 8000) {
            category = "Prediction";
        } else if (codeValue >= 8000 && codeValue < 9000) {
            category = "Cognition";
        } else if (codeValue >= 9000 && codeValue < 10000) {
            category = "Development";
        } else if (codeValue >= 10000 && codeValue < 11000) {
            category = "Checkpoint";
        } else if (codeValue >= 11000 && codeValue < 12000) {
            category = "Replay";
        } else if (codeValue >= 12000 && codeValue < 13000) {
            category = "Resources";
        } else if (codeValue >= 13000 && codeValue < 14000) {
            category = "Generic";
        } else if (codeValue >= 14000 && codeValue < 15000) {
            category = "External Systems";
        } else if (codeValue >= 15000 && codeValue < 16000) {
            category = "Security";
        } else if (codeValue >= 16000 && codeValue < 17000) {
            category = "System";
        } else {
            category = "Unknown";
        }
        
        // Get base message from error message map
        auto it = NLMErrorCategory::errorMessages.find(code);
        if (it != NLMErrorCategory::errorMessages.end()) {
            description = it->second;
        } else {
            description = "Unknown error";
        }
        
        return "[" + category + "] " + description;
    }
};

// Macro for enhanced error handling with recovery
define ENHANCED_ERROR_HANDLER(code, message, context, action) \
    do { \
        nlm::ErrorContext errorContext(context); \
        errorContext.addParameter("action", action); \
        \
        auto recoveryAction = nlm::ErrorRecovery::getRecoveryAction(code); \
        if (recoveryAction != nlm::ErrorRecovery::RecoveryAction::None) { \
            NLM_LOG_INFO("Attempting recovery: " + nlm::ErrorRecovery::getRecoveryStrategy(code)); \
        } \
        \
        NLM_THROW_ERROR_CTX(code, message, context); \
    } while (0)

} // namespace nlm
