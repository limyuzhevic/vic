#pragma once

#include "../core/Logger/Logger.hpp"
#include <stdexcept>
#include <string>
#include <sstream>
#include <cassert>
#include <memory>
#include <vector>

namespace nlm {

// Enhanced error handling class for comprehensive validation and error management
class ErrorHandler {
public:
    enum class ErrorType {
        VALIDATION_ERROR,
        INITIALIZATION_ERROR,
        CONFIGURATION_ERROR,
        MEMORY_ERROR,
        NEURAL_ERROR,
        PLASTICITY_ERROR,
        SYSTEM_ERROR,
        FILE_ERROR,
        INTEGRATION_ERROR,
        PERFORMANCE_ERROR
    };

    struct ErrorInfo {
        ErrorType type;
        std::string message;
        std::string file;
        int line;
        std::string function;
        bool recoverable;
        std::vector<std::string> suggestions;
        
        ErrorInfo(ErrorType t, const std::string& msg, 
                  const std::string& file = "", int line = 0,
                  const std::string& function = "", bool recoverable = true)
            : type(t), message(msg), file(file), line(line), 
              function(function), recoverable(recoverable) {}
    };

private:
    static std::vector<ErrorInfo> errorLog;
    static bool validationEnabled;
    static ErrorType lastErrorType;
    static std::string lastErrorMessage;
    
    static void logError(const ErrorInfo& error) {
        errorLog.push_back(error);
        lastErrorType = error.type;
        lastErrorMessage = error.message;
        
        // Convert error type to log level
        std::string logLevel;
        switch (error.type) {
            case ErrorType::VALIDATION_ERROR:
            case ErrorType::CONFIGURATION_ERROR:
                logLevel = "ERROR";
                break;
            case ErrorType::INITIALIZATION_ERROR:
            case ErrorType::MEMORY_ERROR:
                logLevel = "WARNING";
                break;
            case ErrorType::NEURAL_ERROR:
            case ErrorType::PLASTICITY_ERROR:
                logLevel = "INFO";
                break;
            default:
                logLevel = "ERROR";
        }
        
        // Log the error with full context
        std::stringstream ss;
        ss << "[" << error.function << "] " << error.message;
        if (!error.file.empty()) {
            ss << " (" << error.file << ":" << error.line << ")";
        }
        
        if (error.recoverable) {
            ss << " [recoverable]";
            NLM_LOG_WARNING(ss.str().c_str());
        } else {
            ss << " [fatal]";
            NLM_LOG_ERROR(ss.str().c_str());
        }
    }

public:
    ErrorHandler() = default;
    ~ErrorHandler() = default;
    
    static void initialize() {
        errorLog.clear();
        validationEnabled = true;
        lastErrorType = ErrorType::VALIDATION_ERROR;
        lastErrorMessage.clear();
        NLM_LOG_INFO("Error handler initialized");
    }
    
    static void enableValidation(bool enable) {
        validationEnabled = enable;
        if (enable) {
            NLM_LOG_INFO("Validation enabled");
        } else {
            NLM_LOG_INFO("Validation disabled");
        }
    }
    
    static bool isValidationEnabled() {
        return validationEnabled;
    }
    
    // Validation macros and functions
    template<typename T>
    static bool validateNotNull(const T* ptr, const std::string& message, 
                               const std::string& file = "", int line = 0,
                               const std::string& function = "") {
        if (!validationEnabled) return true;
        
        if (ptr == nullptr) {
            ErrorInfo error(ErrorType::VALIDATION_ERROR, message, file, line, function, false);
            error.suggestions.push_back("Check initialization or allocation");
            logError(error);
            return false;
        }
        return true;
    }
    
    static bool validateCondition(bool condition, const std::string& message,
                                  const std::string& file = "", int line = 0,
                                  const std::string& function = "", 
                                  bool recoverable = true) {
        if (!validationEnabled) return condition;
        
        if (!condition) {
            ErrorInfo error(ErrorType::VALIDATION_ERROR, message, file, line, function, recoverable);
            error.suggestions.push_back("Review the condition logic");
            logError(error);
            return false;
        }
        return true;
    }
    
    static bool validateRange(float value, float min, float max, 
                              const std::string& message,
                              const std::string& file = "", int line = 0,
                              const std::string& function = "") {
        if (!validationEnabled) return true;
        
        if (value < min || value > max) {
            std::stringstream ss;
            ss << message << ": " << value << " is not in range [" << min << ", " << max << "]";
            ErrorInfo error(ErrorType::VALIDATION_ERROR, ss.str(), file, line, function, false);
            error.suggestions.push_back("Ensure value is within specified bounds");
            error.suggestions.push_back("Check unit conversions or scaling factors");
            logError(error);
            return false;
        }
        return true;
    }
    
    static bool validateFileExists(const std::string& path, const std::string& message,
                                   const std::string& file = "", int line = 0,
                                   const std::string& function = "") {
        if (!validationEnabled) return true;
        
        std::ifstream f(path.c_str());
        if (!f.good()) {
            ErrorInfo error(ErrorType::FILE_ERROR, message + ": " + path, 
                           file, line, function, false);
            error.suggestions.push_back("Check file path and permissions");
            error.suggestions.push_back("Ensure file is not corrupted");
            logError(error);
            return false;
        }
        return true;
    }
    
    static bool validateMemoryAllocation(size_t bytes, const std::string& message,
                                         const std::string& file = "", int line = 0,
                                         const std::string& function = "") {
        if (!validationEnabled) return true;
        
        if (bytes == 0) {
            ErrorInfo error(ErrorType::MEMORY_ERROR, message + ": zero allocation", 
                           file, line, function, false);
            error.suggestions.push_back("Check allocation logic");
            error.suggestions.push_back("Ensure buffer sizes are correctly calculated");
            logError(error);
            return false;
        }
        
        // Estimate if this allocation is reasonable (less than 1GB)
        if (bytes > 1024 * 1024 * 1024) {
            print_warning("Large memory allocation: " + std::to_string(bytes / (1024*1024)) + "MB");
        }
        
        return true;
    }
    
    static bool validateNeuronId(const NeuronId& id, const std::string& message,
                                const std::string& file = "", int line = 0,
                                const std::string& function = "") {
        if (!validationEnabled) return true;
        
        if (!id.isValid()) {
            ErrorInfo error(ErrorType::NEURAL_ERROR, message + ": invalid neuron ID", 
                           file, line, function, false);
            error.suggestions.push_back("Check neuron creation logic");
            error.suggestions.push_back("Ensure region ID is correctly used");
            logError(error);
            return false;
        }
        return true;
    }
    
    static bool validateSynapticConnection(const NeuronId& source, const NeuronId& target,
                                           float weight, const std::string& message,
                                           const std::string& file = "", int line = 0,
                                           const std::string& function = "") {
        if (!validationEnabled) return true;
        
        bool valid = true;
        
        if (!validateNeuronId(source, "Invalid synaptic source neuron", file, line, function)) {
            valid = false;
        }
        
        if (!validateNeuronId(target, "Invalid synaptic target neuron", file, line, function)) {
            valid = false;
        }
        
        if (!validateRange(weight, -1000.0f, 1000.0f, 
                          "Synaptic weight out of range", file, line, function)) {
            valid = false;
        }
        
        if (!valid) {
            std::stringstream ss;
            ss << message << ": invalid connection between neurons " << source.index() 
               << " and " << target.index() << " with weight " << weight;
            ErrorInfo error(ErrorType::NEURAL_ERROR, ss.str(), file, line, function, false);
            error.suggestions.push_back("Check neural connectivity logic");
            error.suggestions.push_back("Verify region assignments");
            logError(error);
        }
        
        return valid;
    }
    
    static bool validatePlasticityParameters(float ltpWeight, float ltdWeight, float tau,
                                              const std::string& message,
                                              const std::string& file = "", int line = 0,
                                              const std::string& function = "") {
        if (!validationEnabled) return true;
        
        bool valid = true;
        
        if (!validateRange(ltpWeight, 0.0f, 1.0f, "LTP weight out of range", file, line, function)) {
            valid = false;
        }
        
        if (!validateRange(ltdWeight, 0.0f, 1.0f, "LTD weight out of range", file, line, function)) {
            valid = false;
        }
        
        if (!validateRange(tau, 0.1f, 100.0f, "STDP time constant out of range", file, line, function)) {
            valid = false;
        }
        
        if (ltpWeight <= ltdWeight) {
            ErrorInfo error(ErrorType::PLASTICITY_ERROR, 
                           "LTP weight must be greater than LTD weight", file, line, function, false);
            error.suggestions.push_back("Review plasticity rule parameters");
            logError(error);
            valid = false;
        }
        
        return valid;
    }
    
    static void validateBrainState() {
        if (!validationEnabled) return;
        
        NLM_LOG_INFO("Validating brain state...");
        
        // Check core systems
        if (errorLog.empty()) {
            NLM_LOG_INFO("Brain state validation: PASSED (no errors)");
        } else {
            NLM_LOG_INFO("Brain state validation: WARNINGS found");
            
            int validationWarnings = 0;
            for (const auto& error : errorLog) {
                if (error.type == ErrorType::VALIDATION_ERROR || 
                    error.type == ErrorType::CONFIGURATION_ERROR) {
                    validationWarnings++;
                }
            }
            
            NLM_LOG_INFO(std::to_string(validationWarnings) << " validation issues found");
        }
    }
    
    static void reportErrors() {
        if (errorLog.empty()) {
            NLM_LOG_INFO("No errors to report");
            return;
        }
        
        NLM_LOG_INFO("=== Error Report ===");
        
        int fatalCount = 0;
        int warningCount = 0;
        
        for (const auto& error : errorLog) {
            if (!error.recoverable) {
                fatalCount++;
                NLM_LOG_ERROR(std::string("FATAL: ") + error.message.c_str());
            } else {
                warningCount++;
                NLM_LOG_WARNING(std::string("WARNING: ") + error.message.c_str());
            }
        }
        
        NLM_LOG_INFO("=== Summary ===");
        NLM_LOG_INFO("Fatal errors: " + std::to_string(fatalCount));
        NLM_LOG_INFO("Warnings: " + std::to_string(warningCount));
        NLM_LOG_INFO("Total errors: " + std::to_string(errorLog.size()));
        
        if (fatalCount > 0) {
            NLM_LOG_ERROR("Build cannot continue due to fatal errors");
        }
    }
    
    static bool hasErrors() {
        for (const auto& error : errorLog) {
            if (!error.recoverable) {
                return true;
            }
        }
        return false;
    }
    
    static bool hasWarnings() {
        return !errorLog.empty();
    }
    
    static void clearErrors() {
        errorLog.clear();
        NLM_LOG_INFO("Error log cleared");
    }
    
    static ErrorType getLastErrorType() {
        return lastErrorType;
    }
    
    static const std::string& getLastErrorMessage() {
        return lastErrorMessage;
    }
    
    static size_t getErrorCount() {
        return errorLog.size();
    }
    
    static const std::vector<ErrorInfo>& getAllErrors() {
        return errorLog;
    }
    
    static void exportErrors(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot export errors to file: " + filename);
            return;
        }
        
        file << "NLM Brain Simulator Error Report" << std::endl;
        file << "Generated: " << getCurrentTime() << std::endl;
        file << "Total errors: " << errorLog.size() << std::endl;
        file << "================================" << std::endl << std::endl;
        
        for (size_t i = 0; i < errorLog.size(); ++i) {
            const auto& error = errorLog[i];
            file << "Error " << i + 1 << ": " << error.message << std::endl;
            file << "  Type: " << errorTypeToString(error.type) << std::endl;
            file << "  Location: " << error.function << "()";
            if (!error.file.empty()) {
                file << " in " << error.file << ":" << error.line;
            }
            file << std::endl;
            file << "  Recoverable: " << (error.recoverable ? "Yes" : "No") << std::endl;
            if (!error.suggestions.empty()) {
                file << "  Suggestions:" << std::endl;
                for (const auto& suggestion : error.suggestions) {
                    file << "    - " << suggestion << std::endl;
                }
            }
            file << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("Errors exported to: " + filename);
    }
    
private:
    static std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    static std::string errorTypeToString(ErrorType type) {
        switch (type) {
            case ErrorType::VALIDATION_ERROR: return "Validation Error";
            case ErrorType::INITIALIZATION_ERROR: return "Initialization Error";
            case ErrorType::CONFIGURATION_ERROR: return "Configuration Error";
            case ErrorType::MEMORY_ERROR: return "Memory Error";
            case ErrorType::NEURAL_ERROR: return "Neural Error";
            case ErrorType::PLASTICITY_ERROR: return "Plasticity Error";
            case ErrorType::SYSTEM_ERROR: return "System Error";
            case ErrorType::FILE_ERROR: return "File Error";
            case ErrorType::INTEGRATION_ERROR: return "Integration Error";
            case ErrorType::PERFORMANCE_ERROR: return "Performance Error";
            default: return "Unknown Error";
        }
    }
};

// Global error handler instance
inline ErrorHandler& getErrorHandler() {
    static ErrorHandler handler;
    return handler;
}

// Convenience functions for error handling
inline void initializeErrorHandling() {
    getErrorHandler().initialize();
}

inline bool validateCondition(bool condition, const std::string& message) {
    return getErrorHandler().validateCondition(condition, message);
}

inline bool validateNotNull(const void* ptr, const std::string& message) {
    return getErrorHandler().validateNotNull(ptr, message);
}

inline bool validateRange(float value, float min, float max, const std::string& message) {
    return getErrorHandler().validateRange(value, min, max, message);
}

inline void reportBuildErrors() {
    getErrorHandler().reportErrors();
}

inline bool hasBuildErrors() {
    return getErrorHandler().hasErrors();
}

inline bool hasBuildWarnings() {
    return getErrorHandler().hasWarnings();
}

inline void clearBuildErrors() {
    getErrorHandler().clearErrors();
}

inline bool isValidationEnabled() {
    return getErrorHandler().isValidationEnabled();
}

inline void enableValidation(bool enable) {
    getErrorHandler().enableValidation(enable);
}

} // namespace nlm
