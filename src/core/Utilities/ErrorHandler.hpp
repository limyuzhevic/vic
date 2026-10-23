// NLM Error Handler
// Enhanced error handling with context and recovery information

#ifndef NLM_ERROR_HANDLER_HPP
#define NLM_ERROR_HANDLER_HPP

#include <string>
#include <vector>
#include <chrono>

namespace nlm {

class ErrorHandler {
public:
    struct ErrorContext {
        std::string error;
        std::string context;
        std::chrono::system_clock::time_point timestamp;
        std::vector<std::string> suggestions;
    };
    
    static void logError(const std::string& error);
    static void logErrorWithContext(const std::string& error, const std::string& context);
    static void logWarning(const std::string& warning);
    static void logInfo(const std::string& info);
    static void logMemoryError(const std::string& operation, size_t size);
    static void logFileError(const std::string& operation, const std::string& filepath);
    static void logConfigurationError(const std::string& key, const std::string& expectedType);
    
    static void clearErrorHistory();
    static std::vector<ErrorContext> getRecentErrors(size_t maxCount = 10);
    static bool hasRecentErrors(double timeWindowSeconds = 300.0);
    
    static std::string formatError(const ErrorContext& context);
    static std::vector<std::string> generateSuggestions(const std::string& error);
    
    static void setFailOnError(bool fail) { failOnError = fail; }
    static bool shouldFailOnError() { return failOnError; }
    
    static void enableStructuredLogging(bool enable) { structuredLoggingEnabled = enable; }
    static bool isStructuredLoggingEnabled() { return structuredLoggingEnabled; }
    
    static void setLogLevel(LogLevel level) { logLevel = level; }
    static LogLevel getLogLevel() { return logLevel; }
    
    static void exportErrorReport(const std::string& filepath);
    
private:
    static std::vector<ErrorContext> errorHistory;
    static bool failOnError;
    static bool structuredLoggingEnabled;
    static LogLevel logLevel;
    
    static std::string getTimestampString();
    static void addError(const std::string& error, const std::string& context = "");
};

} // namespace nlm

#endif // NLM_ERROR_HANDLER_HPP
