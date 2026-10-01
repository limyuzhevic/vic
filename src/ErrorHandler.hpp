// ErrorHandler.hpp
// Error handling and exception management for NLM

#ifndef NLM_ERRORHANDLER_HPP
#define NLM_ERRORHANDLER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <chrono>
#include <sstream>
#include <functional>

namespace nlm {

// Error level enumeration
enum class ErrorLevel {
    Debug,      ///< Debug level messages (verbose)
    Info,       ///< Informational messages
    Warning,    ///< Warning messages (potential issues)
    Error,      ///< Error messages (recoverable errors)
    Critical    ///< Critical errors (unrecoverable, program should exit)
};

// Error information structure
struct ErrorInfo {
    ErrorLevel level;      ///< Error level
    std::string message;   ///< Error message
    std::string context;   ///< Context where error occurred
    std::string timestamp; ///< When error occurred
};

// NLM Exception class
class NLMException : public std::exception {
private:
    std::string message;
    ErrorLevel level;
    std::string context;
    std::string timestamp;
    
public:
    NLMException(const std::string& msg, ErrorLevel lvl, const std::string& ctx);
    
    const char* what() const noexcept override;
    
    ErrorLevel getLevel() const;
    const std::string& getContext() const;
    const std::string& getTimestamp() const;
    
private:
    static std::string getCurrentTimestamp();
};

// Error handler class
class ErrorHandler {
private:
    ErrorLevel currentLevel = ErrorLevel::Info;
    std::vector<ErrorInfo> errorHistory;
    std::ostream* outputStream = &std::cout;
    std::ofstream logFile;
    
public:
    void setLevel(ErrorLevel level);
    ErrorLevel getLevel() const;
    
    void logError(const std::string& message, const std::string& context = "");
    void logWarning(const std::string& message, const std::string& context = "");
    void logInfo(const std::string& message, const std::string& context = "");
    
    void logMessage(ErrorLevel level, const std::string& message, const std::string& context = "");
    void throwException(ErrorLevel level, const std::string& message, const std::string& context = "");
    
    const std::vector<ErrorInfo>& getErrorHistory() const;
    void clearErrorHistory();
    
    void setOutputStream(std::ostream& stream);
    void setFileOutput(const std::string& filepath);
    
private:
    void logMessageImpl(ErrorLevel level, const std::string& message, const std::string& context);
    static std::string getCurrentTimestamp();
    static std::string errorLevelToString(ErrorLevel level);
};

// Global error handler functions
define_global_error_handler(ErrorHandler& handler);
ErrorHandler& get_global_error_handler();
void log_error(const std::string& message, const std::string& context = "");
void log_warning(const std::string& message, const std::string& context = "");
void log_info(const std::string& message, const std::string& context = "");
void throw_error(const std::string& message, ErrorLevel level = ErrorLevel::Error, const std::string& context = "");

} // namespace nlm

#endif // NLM_ERRORHANDLER_HPP