// Error Context and Diagnostics Framework
// This file provides comprehensive error context, stack traces, and diagnostic information

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <functional>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <atomic>

namespace nlm {

// Error Context Structure
struct ErrorContext {
    // Error identification
    std::string id;
    std::string correlationId;
    std::string operationId;
    std::string component;
    std::string subsystem;
    
    // Error details
    std::string message;
    std::string description;
    std::string cause;
    std::string resolution;
    
    // Error classification
    std::string errorType;
    std::string errorCategory;
    std::string errorSeverity; // DEBUG, INFO, WARNING, ERROR, CRITICAL, FATAL
    int errorCode;
    
    // Location information
    std::string filePath;
    std::string functionName;
    int lineNumber;
    int columnNumber;
    
    // Time information
    std::chrono::system_clock::time_point timestamp;
    std::chrono::milliseconds duration;
    
    // Stack trace
    std::vector<std::string> stackTrace;
    std::string stackTraceString;
    
    // System state
    std::unordered_map<std::string, std::string> systemState;
    std::unordered_map<std::string, double> metrics;
    
    // Exception information
    std::string exceptionType;
    std::string exceptionMessage;
    bool isExceptionThrown;
    
    // Recovery information
    std::string recoveryAction;
    std::string recoveryStrategy;
    bool recoveryAttempted;
    bool recoverySuccessful;
    std::string recoveryDetails;
    
    // Impact analysis
    std::vector<std::string> affectedComponents;
    std::vector<std::string> affectedUsers;
    std::string impactLevel;
    
    // Additional metadata
    std::unordered_map<std::string, std::string> metadata;
    std::unordered_map<std::string, std::vector<std::string>> metadataList;
    
    // Thread safety information
    std::thread::id threadId;
    bool threadSafe;
    std::string threadSafetyDetails;
    
    // Custom properties
    std::unordered_map<std::string, std::string> customProperties;
    
    // Internal flags
    bool isResolved;
    bool isAcknowledged;
    bool isEscalated;
    bool isSilenced;
    
    ErrorContext() = default;
    
    // Constructor with essential information
    ErrorContext(const std::string& msg, const std::string& type = "Error", 
                 const std::string& severity = "ERROR", int code = 0)
        : message(msg), errorType(type), errorSeverity(severity), 
          errorCode(code), isExceptionThrown(false), 
          recoveryAttempted(false), recoverySuccessful(false),
          isResolved(false), isAcknowledged(false), isEscalated(false), 
          isSilenced(false), threadSafe(true) {
        timestamp = std::chrono::system_clock::now();
        lineNumber = 0;
        columnNumber = 0;
        duration = std::chrono::milliseconds(0);
    }
    
    // Enhanced constructor
    ErrorContext(const std::string& msg, const std::string& file, 
                 const std::string& function, int line,
                 const std::string& type = "Error", 
                 const std::string& severity = "ERROR", int code = 0)
        : message(msg), filePath(file), functionName(function), 
          lineNumber(line), columnNumber(0), errorType(type), 
          errorSeverity(severity), errorCode(code), 
          isExceptionThrown(false), recoveryAttempted(false), 
          recoverySuccessful(false), isResolved(false), 
          isAcknowledged(false), isEscalated(false), isSilenced(false),
          threadSafe(true) {
        timestamp = std::chrono::system_clock::now();
        duration = std::chrono::milliseconds(0);
    }
    
    // Utility functions
    std::string getLocation() const;
    std::string getFormattedMessage() const;
    std::string getFormattedTimestamp() const;
    std::string getFormattedDuration() const;
    std::string getStackTraceString() const;
    std::string getSystemStateString() const;
    std::string getMetadataString() const;
    
    // Validation functions
    bool isValid() const;
    bool isCritical() const;
    bool isRecoverable() const;
    bool isSelfDescribing() const;
    
    // Comparison functions
    bool equals(const ErrorContext& other) const;
    bool lessThan(const ErrorContext& other) const;
    
    // Serialization functions
    std::string toJson() const;
    bool fromJson(const std::string& jsonStr);
    std::string toString() const;
    
    // Copy and assignment
    ErrorContext(const ErrorContext& other) = default;
    ErrorContext& operator=(const ErrorContext& other) = default;
    
    // Move operations
    ErrorContext(ErrorContext&& other) noexcept = default;
    ErrorContext& operator=(ErrorContext&& other) noexcept = default;
    
    // Destructor
    ~ErrorContext() = default;
};

// Error Context Factory
class ErrorContextFactory {
public:
    static ErrorContext createSystemError(const std::string& message, 
                                         const std::string& component = "System",
                                         const std::string& file = "",
                                         const std::string& function = "",
                                         int line = 0);
    
    static ErrorContext createConfigurationError(const std::string& message,
                                                 const std::string& key = "",
                                                 const std::string& expected = "",
                                                 const std::string& actual = "",
                                                 const std::string& file = "",
                                                 const std::string& function = "",
                                                 int line = 0);
    
    static ErrorContext createResourceError(const std::string& message,
                                            const std::string& resource = "",
                                            const std::string& operation = "",
                                            const std::string& file = "",
                                            const std::string& function = "",
                                            int line = 0);
    
    static ErrorContext createMemoryError(const std::string& message,
                                          size_t allocationSize = 0,
                                          const std::string& file = "",
                                          const std::string& function = "",
                                          int line = 0);
    
    static ErrorContext createNetworkError(const std::string& message,
                                           const std::string& host = "",
                                           int port = 0,
                                           const std::string& file = "",
                                           const std::string& function = "",
                                           int line = 0);
    
    static ErrorContext createValidationError(const std::string& message,
                                              const std::string& field = "",
                                              const std::string& value = "",
                                              const std::string& constraint = "",
                                              const std::string& file = "",
                                              const std::string& function = "",
                                              int line = 0);
    
    static ErrorContext createRecoveryError(const std::string& message,
                                            const std::string& operation = "",
                                            const std::string& state = "",
                                            const std::string& file = "",
                                            const std::string& function = "",
                                            int line = 0);
    
    static ErrorContext createPerformanceError(const std::string& message,
                                               const std::string& metric = "",
                                               double value = 0.0,
                                               double threshold = 0.0,
                                               const std::string& file = "",
                                               const std::string& function = "",
                                               int line = 0);
    
    static ErrorContext createExceptionError(const std::exception& e,
                                             const std::string& file = "",
                                             const std::string& function = "",
                                             int line = 0);
    
    static ErrorContext createCustomError(const std::string& message,
                                          const std::unordered_map<std::string, std::string>& metadata = {});
    
    static std::string generateCorrelationId();
    static std::string generateErrorId();
    static std::chrono::system_clock::time_point getCurrentTime();
    
    static void captureStackTrace(std::vector<std::string>& stackTrace, int maxDepth = 20);
    static std::string captureStackTraceString(int maxDepth = 20);
    
    static void populateSystemState(std::unordered_map<std::string, std::string>& state);
    static std::unordered_map<std::string, std::string> captureSystemState();
    
    static void captureMetrics(std::unordered_map<std::string, double>& metrics);
    static std::unordered_map<std::string, double> capturePerformanceMetrics();
}; 

// Error Context Utilities
namespace ErrorContextUtils {
    // Error Context Comparison
    bool compareBySeverity(const ErrorContext& a, const ErrorContext& b);
    bool compareByTimestamp(const ErrorContext& a, const ErrorContext& b);
    bool compareByErrorCode(const ErrorContext& a, const ErrorContext& b);
    
    // Error Context Filtering
    std::vector<ErrorContext> filterBySeverity(const std::vector<ErrorContext>& errors,
                                               const std::string& severity);
    std::vector<ErrorContext> filterByType(const std::vector<ErrorContext>& errors,
                                           const std::string& type);
    std::vector<ErrorContext> filterByComponent(const std::vector<ErrorContext>& errors,
                                                const std::string& component);
    std::vector<ErrorContext> filterByTime(const std::vector<ErrorContext>& errors,
                                           const std::chrono::system_clock::time_point& start,
                                           const std::chrono::system_clock::time_point& end);
    
    // Error Context Aggregation
    ErrorContext aggregateErrors(const std::vector<ErrorContext>& errors);
    std::unordered_map<std::string, std::vector<ErrorContext>> groupByComponent(
        const std::vector<ErrorContext>& errors);
    std::unordered_map<std::string, std::vector<ErrorContext>> groupByType(
        const std::vector<ErrorContext>& errors);
    
    // Error Context Analysis
    std::string analyzeErrorPatterns(const std::vector<ErrorContext>& errors);
    std::vector<ErrorContext> getUniqueErrors(const std::vector<ErrorContext>& errors);
    std::vector<ErrorContext> getRecurringErrors(const std::vector<ErrorContext>& errors);
    
    // Error Context Serialization
    std::string serializeErrorContext(const ErrorContext& error);
    ErrorContext deserializeErrorContext(const std::string& serialized);
    
    // Error Context Persistence
    bool saveErrorContext(const ErrorContext& error, const std::string& filepath);
    ErrorContext loadErrorContext(const std::string& filepath);
    
    // Error Context Statistics
    struct ErrorStatistics {
        int totalErrors;
        int criticalErrors;
        int recoverableErrors;
        int resolvedErrors;
        std::chrono::system_clock::time_point earliestError;
        std::chrono::system_clock::time_point latestError;
        std::unordered_map<std::string, int> errorTypeCounts;
        std::unordered_map<std::string, int> componentCounts;
    };
    
    ErrorStatistics calculateStatistics(const std::vector<ErrorContext>& errors);
    std::string generateStatisticsReport(const ErrorStatistics& stats);
    
    // Error Context Validation
    bool validateErrorContext(const ErrorContext& error);
    std::vector<std::string> getValidationErrors(const ErrorContext& error);
    
    // Error Context Enrichment
    void enrichErrorContext(ErrorContext& error);
    ErrorContext enrichErrorContext(const ErrorContext& error);
    
    // Error Context Reporting
    std::string formatErrorReport(const std::vector<ErrorContext>& errors);
    void sendErrorReport(const std::vector<ErrorContext>& errors,
                        const std::string& destination);
    
    // Error Context Management
    void cleanupOldErrors(std::vector<ErrorContext>& errors,
                         const std::chrono::system_clock::time_point& cutoffTime);
    std::vector<ErrorContext> getRecentErrors(const std::vector<ErrorContext>& errors,
                                             int maxAgeSeconds);
    
    // Error Context Search
    std::vector<ErrorContext> searchErrors(const std::vector<ErrorContext>& errors,
                                           const std::string& pattern);
    std::vector<ErrorContext> searchErrorsByMessage(const std::vector<ErrorContext>& errors,
                                                    const std::string& message);
    std::vector<ErrorContext> searchErrorsByMetadata(const std::vector<ErrorContext>& errors,
                                                     const std::string& key,
                                                     const std::string& value);
}

// Error Context Comparison Operators
inline bool operator==(const ErrorContext& a, const ErrorContext& b) {
    return a.equals(b);
}

inline bool operator!=(const ErrorContext& a, const ErrorContext& b) {
    return !a.equals(b);
}

inline bool operator<(const ErrorContext& a, const ErrorContext& b) {
    return a.lessThan(b);
}

inline bool operator>(const ErrorContext& a, const ErrorContext& b) {
    return b.lessThan(a);
}

inline bool operator<=(const ErrorContext& a, const ErrorContext& b) {
    return !b.lessThan(a);
}

inline bool operator>=(const ErrorContext& a, const ErrorContext& b) {
    return !a.lessThan(b);
}

// Error Context Stream Operators
inline std::ostream& operator<<(std::ostream& os, const ErrorContext& error) {
    os << error.getFormattedMessage();
    return os;
}

inline std::istream& operator>>(std::istream& is, ErrorContext& error) {
    std::string line;
    std::getline(is, line);
    error.message = line;
    return is;
}

} // namespace nlm
