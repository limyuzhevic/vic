// NLM Advanced Features - Error Handler
// Enhanced error handling with categorization, recovery, and context

#ifndef NLM_ERROR_HANDLER_HPP
#define NLM_ERROR_HANDLER_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <chrono>
#include <functional>
#include <sstream>
#include <iomanip>

namespace nlm {
namespace advanced {

enum class ErrorCategory {
    None = 0,
    System = 1,
    Configuration = 2,
    Network = 3,
    Memory = 4,
    IO = 5,
    Neural = 6,
    Performance = 7,
    Security = 8,
    Development = 9,
    User = 10,
    External = 11,
    All = 0xFFFFFFFF
};

enum class ErrorSeverity {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Recoverable = 3,
    Error = 4,
    Critical = 5,
    Fatal = 6
};

enum class RecoveryAction {
    None = 0,
    Retry = 1,
    Fallback = 2,
    Skip = 3,
    Abort = 4,
    Continue = 5,
    Reset = 6,
    Restart = 7,
    ConfigUpdate = 8,
    ResourceReallocation = 9
};

struct ErrorContext {
    std::string errorId;
    ErrorCategory category;
    ErrorSeverity severity;
    std::string message;
    std::string details;
    std::string suggestion;
    std::string stackTrace;
    std::chrono::system_clock::time_point timestamp;
    std::string sourceFile;
    int sourceLine;
    std::string functionName;
    std::map<std::string, std::string> metadata;
    std::vector<std::string> recoverySteps;
    RecoveryAction recoveryAction;
    bool isHandled;
    bool isResolved;
    std::string resolvedMessage;
    std::chrono::system_clock::time_point resolvedTime;
    std::string resolvedBy;
};

struct RecoveryStrategy {
    ErrorCategory category;
    ErrorSeverity severity;
    RecoveryAction action;
    std::string condition;
    std::function<bool(const ErrorContext&)> predicate;
    std::function<std::string(ErrorContext&)> recoveryFunc;
    int maxRetries;
    double retryInterval;
    bool autoRecover;
};

struct ErrorStatistics {
    int totalErrors;
    int errorsByCategory[12]; // Index by ErrorCategory
    int errorsBySeverity[7]; // Index by ErrorSeverity
    int errorsByRecoveryAction[10]; // Index by RecoveryAction
    std::chrono::system_clock::time_point firstErrorTime;
    std::chrono::system_clock::time_point lastErrorTime;
    double errorRatePerMinute;
    std::string mostFrequentError;
    int mostFrequentErrorCount;
    std::vector<std::string> recurringErrors;
    std::map<std::string, int> errorFrequency;
};

class ErrorHandler {
public:
    ErrorHandler();
    ~ErrorHandler();
    
    // Error reporting
    static ErrorContext reportError(ErrorCategory category, ErrorSeverity severity, const std::string& message);
    static ErrorContext reportError(ErrorCategory category, ErrorSeverity severity, const std::string& message, const std::string& details);
    static ErrorContext reportError(ErrorCategory category, ErrorSeverity severity, const std::string& message, const std::string& details, const std::string& suggestion);
    static ErrorContext reportError(const std::string& categoryStr, const std::string& severityStr, const std::string& message);
    
    // Error recovery
    static bool recover(ErrorContext& error);
    static bool recover(const std::string& errorId);
    static bool autoRecover(ErrorContext& error);
    static bool manualRecover(ErrorContext& error);
    
    // Error querying
    static ErrorContext getError(const std::string& errorId);
    static std::vector<ErrorContext> getErrors(ErrorCategory category = ErrorCategory::None, ErrorSeverity severity = ErrorSeverity::Info);
    static std::vector<ErrorContext> getRecentErrors(int minutes = 60);
    static std::vector<ErrorContext> getUnhandledErrors();
    static std::vector<ErrorContext> getUnresolvedErrors();
    static std::vector<ErrorContext> getErrorsBySource(const std::string& sourceFile, int line = -1);
    static std::vector<ErrorContext> getErrorsByRecoveryAction(RecoveryAction action);
    
    // Error history and statistics
    static std::vector<ErrorContext> getErrorHistory();
    static void clearErrorHistory();
    static bool saveErrorHistory(const std::string& filename);
    static bool loadErrorHistory(const std::string& filename);
    static ErrorStatistics getErrorStatistics() const;
    static double getErrorRate(const std::string& timeWindow = "1h") const;
    static std::string getErrorTrend(const std::string& timeWindow = "1h") const;
    
    // Error management
    static bool markErrorHandled(const std::string& errorId);
    static bool markErrorResolved(const std::string& errorId, const std::string& resolvedMessage = "");
    static bool deleteError(const std::string& errorId);
    static bool archiveError(const std::string& errorId, const std::string& archiveFile = "");
    static std::vector<ErrorContext> getArchivedErrors();
    
    // Configuration
    static void setErrorRecoveryEnabled(bool enabled);
    static void setMaxErrorHistorySize(int size);
    static void setAutoRecoveryEnabled(bool enabled);
    static void setRecoveryStrategy(const RecoveryStrategy& strategy);
    static void addRecoveryStrategy(const RecoveryStrategy& strategy);
    static void setErrorFilter(ErrorCategory category, ErrorSeverity severity);
    static void setOutputDestination(const std::string& destination);
    static void setOutputFormat(const std::string& format);
    
    // Error context management
    static std::string generateErrorId();
    static std::string getCurrentStackTrace();
    static std::string formatErrorContext(const ErrorContext& error, const std::string& format = "detailed");
    static std::string getErrorSummary(const ErrorContext& error);
    static std::string getErrorDetails(const ErrorContext& error);
    static std::string getErrorSuggestion(const ErrorContext& error);
    static std::chrono::system_clock::time_point getCurrentTime();
    
    // Callback system
    static void setErrorCallback(const std::function<void(const ErrorContext&)>& callback);
    static void setRecoveryCallback(const std::function<bool(ErrorContext&)>& callback);
    static void setErrorStatisticsCallback(const std::function<void(const ErrorStatistics&)>& callback);
    
    // Error categorization
    static ErrorCategory stringToErrorCategory(const std::string& categoryStr);
    static std::string errorCategoryToString(ErrorCategory category);
    static ErrorSeverity stringToErrorSeverity(const std::string& severityStr);
    static std::string errorSeverityToString(ErrorSeverity severity);
    static RecoveryAction stringToRecoveryAction(const std::string& actionStr);
    static std::string recoveryActionToString(RecoveryAction action);
    
    // Error checking
    static bool isErrorCategoryEnabled(ErrorCategory category) const;
    static bool isErrorSeverityEnabled(ErrorSeverity severity) const;
    static bool shouldRecover(ErrorContext& error) const;
    static bool isRecoverable(ErrorContext& error) const;
    
    // Statistics utilities
    static std::string formatErrorCount(int count) const;
    static std::string formatErrorRate(double rate) const;
    static std::string getTimeAgo(const std::chrono::system_clock::time_point& time) const;
    
    // Error types and exceptions
    class NLMError : public std::runtime_error {
    public:
        NLMError(const std::string& message, ErrorCategory category = ErrorCategory::None, ErrorSeverity severity = ErrorSeverity::Error);
        
        ErrorCategory getCategory() const { return category; }
        ErrorSeverity getSeverity() const { return severity; }
        const std::string& getErrorId() const { return errorId; }
        
    private:
        std::string errorId;
        ErrorCategory category;
        ErrorSeverity severity;
    };
    
    class NLMFatalError : public NLMError {
    public:
        NLMFatalError(const std::string& message, ErrorCategory category = ErrorCategory::System);
    };
    
    class NLMRecoverableError : public NLMError {
    public:
        NLMRecoverableError(const std::string& message, ErrorCategory category = ErrorCategory::System);
    };
    
    // Singleton pattern
    static ErrorHandler& getInstance();
    static void resetInstance();

private:
    std::vector<ErrorContext> errorHistory;
    std::map<std::string, ErrorContext> errorLookup;
    std::vector<ErrorContext> archivedErrors;
    
    // Configuration
    bool errorRecoveryEnabled;
    int maxErrorHistorySize;
    bool autoRecoveryEnabled;
    std::vector<RecoveryStrategy> recoveryStrategies;
    ErrorCategory errorFilterCategory;
    ErrorSeverity errorFilterSeverity;
    std::string outputDestination;
    std::string outputFormat;
    
    // Statistics
    mutable ErrorStatistics stats;
    
    // Callbacks
    std::function<void(const ErrorContext&)> errorCallback;
    std::function<bool(ErrorContext&)> recoveryCallback;
    std::function<void(const ErrorStatistics&)> errorStatisticsCallback;
    
    // Error ID generation
    std::mt19937_64 rng;
    std::uniform_int_distribution<uint64_t> idDistribution;
    
    // Helper methods
    ErrorContext createErrorContext(ErrorCategory category, ErrorSeverity severity, const std::string& message, const std::string& details = "", const std::string& suggestion = "");
    void addErrorToHistory(ErrorContext& error);
    bool findRecoveryStrategy(ErrorContext& error, RecoveryStrategy& strategy);
    bool applyRecoveryStrategy(ErrorContext& error, const RecoveryStrategy& strategy);
    void updateStatistics(const ErrorContext& error);
    void outputError(const ErrorContext& error);
    std::string formatErrorMessage(const ErrorContext& error) const;
    bool isErrorInFilter(const ErrorContext& error) const;
    void cleanupOldErrors();
    std::string getStackTrace() const;
    void populateStackTrace(std::string& stackTrace) const;
};

} // namespace advanced
} // namespace nlm

#endif // NLM_ERROR_HANDLER_HPP
