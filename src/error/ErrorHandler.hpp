// Error Handler Header
#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <memory>
#include "NLMErrors.hpp"

namespace nlm {

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
    std::unique_ptr<ExceptionTranslator> _translator;
};

} // namespace nlm