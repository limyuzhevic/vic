// NLM Error Handling Headers
#pragma once

#include <string>
#include <map>
#include <chrono>
#include <memory>
#include <vector>

namespace nlm {

/**
 * Error codes for NLM
 */
enum class ErrorCode {
    SUCCESS = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4,
    
    // Specific error codes
    INVALID_ARGUMENT = 100,
    OUT_OF_RANGE = 101,
    MEMORY_ERROR = 102,
    FILE_ERROR = 103,
    NETWORK_ERROR = 104,
    CONFIGURATION_ERROR = 105,
    INITIALIZATION_ERROR = 106,
    VALIDATION_ERROR = 107,
    INTEGRATION_ERROR = 108,
    PLUGIN_ERROR = 109,
    INTERNAL_ERROR = 999
};

/**
 * NLM error
 */
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, ErrorCode code, const std::string& file = "", int line = 0);
    NLMError(const std::string& message, ErrorCode code, const std::string& file, int line,
             const std::map<std::string, std::string>& context);
    
    const char* what() const noexcept override;
    
    ErrorCode getCode() const { return code; }
    const std::string& getFile() const { return file; }
    int getLine() const { return line; }
    const std::string& getContext(const std::string& key) const;
    std::string getTimestamp() const;
    std::string toString() const;
    
private:
    ErrorCode code;
    std::string file;
    int line;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> context;
    mutable std::string _whatBuffer;
};

/**
 * Error recovery interface
 */
class IErrorRecovery {
public:
    virtual ~IErrorRecovery() = default;
    virtual void execute(const NLMError& error) = 0;
};

/**
 * Error handler for NLM
 */
class ErrorHandler {
public:
    ErrorHandler();
    ~ErrorHandler();
    
    /**
     * Handle NLM error
     */
    void handleError(const NLMError& error);
    
    /**
     * Handle C++ exception
     */
    void handleException(const std::exception& ex);
    
    /**
     * Set log level
     */
    void setLogLevel(LogLevel level);
    
    /**
     * Set output errors
     */
    void setOutputErrors(bool toConsole, bool toFile, const std::string& filePath = "");
    
    /**
     * Add recovery strategy
     */
    void addRecoveryStrategy(ErrorCode code, std::unique_ptr<IErrorRecovery> strategy);
    
    /**
     * Get error log
     */
    const std::vector<NLMError>& getErrorLog() const;
    
    /**
     * Clear error log
     */
    void clearErrorLog();
    
    /**
     * Export error log
     */
    void exportErrorLog(const std::string& filePath);
    
private:
    bool canRecoverFrom(const NLMError& error) const;
    bool recoverFrom(const NLMError& error);
    void logError(const NLMError& error);
    
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };
    
    bool _outputErrorsToConsole;
    bool _outputErrorsToFile;
    std::string _errorLogFile;
    LogLevel _logLevel;
    
    mutable std::mutex _mutex;
    std::vector<NLMError> _errorLog;
    std::map<ErrorCode, std::unique_ptr<IErrorRecovery>> _recoveryStrategies;
    std::unique_ptr<class ExceptionTranslator> _translator;
};

/**
 * Error recovery base class
 */
class ErrorRecovery : public IErrorRecovery {
public:
    ErrorRecovery(ErrorCode code, const std::string& description);
    virtual ~ErrorRecovery() = default;
    
    ErrorCode getCode() const;
    std::string getDescription() const;
    
    virtual void execute(const NLMError& error) override;
    
private:
    ErrorCode _code;
    std::string _description;
};

/**
 * Error context
 */
class ErrorContext {
public:
    ErrorContext();
    ~ErrorContext();
    
    /**
     * Set context value
     */
    void setContext(const std::string& key, const std::string& value);
    
    /**
     * Get context value
     */
    std::string getContext(const std::string& key) const;
    
    /**
     * Get all context
     */
    const std::map<std::string, std::string>& getAllContext() const;
    
    /**
     * Clear context
     */
    void clearContext();
    
    /**
     * Set context from error
     */
    void setContextFromError(const NLMError& error);
    
private:
    std::map<std::string, std::string> _context;
};

/**
 * Exception translator
 */
class ExceptionTranslator {
public:
    ExceptionTranslator();
    ~ExceptionTranslator();
    
    /**
     * Translate C++ exception to NLM error
     */
    NLMError translate(const std::exception& ex);
    
    /**
     * Register exception translation
     */
    template<typename TException>
    void registerTranslation(ErrorCode code) {
        _translationMap[&typeid(TException)] = code;
    }
    
    /**
     * Register custom exception translation
     */
    template<typename TException>
    void registerTranslation(ErrorCode code, std::function<NLMError(const std::exception&)> translator) {
        _translationMap[&typeid(TException)] = code;
        _customTranslators[&typeid(TException)] = translator;
    }
    
    /**
     * Translate exception with specific type
     */
    template<typename TException>
    NLMError translate(const std::exception& ex) {
        auto it = _customTranslators.find(&typeid(TException));
        if (it != _customTranslators.end()) {
            return it->second(ex);
        }
        
        return translate(ex);
    }
    
private:
    std::map<const std::type_info*, ErrorCode> _translationMap;
    std::map<const std::type_info*, std::function<NLMError(const std::exception&)>> _customTranslators;
};

} // namespace nlm