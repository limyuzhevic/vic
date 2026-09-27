#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <queue>
#include <unordered_map>
#include <set>

namespace nlm {

// Forward declarations
class Error;
class IErrorReporter;

// Error output format
enum class ErrorOutputFormat {
    Simple,
    Detailed,
    JSON,
    XML,
    CSV
};

// Error statistics
struct ErrorStatistics {
    std::atomic<size_t> totalErrors{0};
    std::atomic<size_t> errorsBySeverity[5]{0};  // Debug, Info, Warning, Error, Critical
    std::atomic<size_t> errorsByType[15]{0};   // Different error types
    std::atomic<size_t> errorsByCategory[8]{0}; // Error categories
    std::chrono::system_clock::time_point firstErrorTime;
    std::chrono::system_clock::time_point lastErrorTime;
    std::map<std::string, std::atomic<size_t>> errorsByContext;
    std::map<std::string, std::atomic<size_t>> errorsBySource;
    double getErrorsPerSecond() const;
    double getErrorRate(ErrorSeverity severity) const;
    std::string getTopErrors(size_t count = 10) const;
    std::string getErrorDistributionReport() const;
    void reset();
};

// Error context for enhanced reporting
struct ErrorContext {
    std::string operation;
    std::string component;
    std::string subsystem;
    std::string module;
    std::map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point timestamp;
    std::string userId;
    std::string sessionId;
    std::map<std::string, std::string> tags;
    
    ErrorContext() : timestamp(std::chrono::system_clock::now()) {}
    ErrorContext(const std::string& op, const std::string& comp, const std::string& sub)
        : operation(op), component(comp), subsystem(sub), timestamp(std::chrono::system_clock::now()) {}
    
    std::string toString() const;
    bool hasTag(const std::string& tag) const;
    std::string getTag(const std::string& tag, const std::string& defaultValue = "") const;
};

// Enhanced error with context and metadata
class EnhancedError : public Error {
public:
    EnhancedError(const std::string& message, ErrorType type = ErrorType::Unknown,
                  ErrorSeverity severity = ErrorSeverity::Error,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "",
                  const std::string& category = "",
                  const ErrorContext& context = ErrorContext())
        : Error(message, type, severity, file, line, function, category)
        , context_(context)
    {}

    const ErrorContext& getContext() const { return context_; }
    void setContext(const ErrorContext& ctx) { context_ = ctx; }
    
    std::string formatWithContext() const;
    std::string formatForJSON() const;
    std::string formatForXML() const;
    std::string formatForCSV() const;
    
    bool hasContext(const std::string& key) const;
    std::string getContextValue(const std::string& key) const;
    
    std::vector<std::string> getTagList() const;

private:
    ErrorContext context_;
};

// Error formatter for different output formats
class ErrorFormatter {
public:
    virtual ~ErrorFormatter() = default;
    virtual std::string format(const std::shared_ptr<Error>& error) const = 0;
    virtual std::string format(const std::vector<std::shared_ptr<Error>>& errors) const = 0;
    virtual std::string getFormatName() const = 0;
};

// Simple error formatter
class SimpleErrorFormatter : public ErrorFormatter {
public:
    std::string format(const std::shared_ptr<Error>& error) const override;
    std::string format(const std::vector<std::shared_ptr<Error>>& errors) const override;
    std::string getFormatName() const override { return "Simple"; }
};

// Detailed error formatter
class DetailedErrorFormatter : public ErrorFormatter {
public:
    std::string format(const std::shared_ptr<Error>& error) const override;
    std::string format(const std::vector<std::shared_ptr<Error>>& errors) const override;
    std::string getFormatName() const override { return "Detailed"; }
};

// JSON error formatter
class JSONErrorFormatter : public ErrorFormatter {
public:
    std::string format(const std::shared_ptr<Error>& error) const override;
    std::string format(const std::vector<std::shared_ptr<Error>>& errors) const override;
    std::string getFormatName() const override { return "JSON"; }
};

// XML error formatter
class XMLErrorFormatter : public ErrorFormatter {
public:
    std::string format(const std::shared_ptr<Error>& error) const override;
    std::string format(const std::vector<std::shared_ptr<Error>>& errors) const override;
    std::string getFormatName() const override { return "XML"; }
};

// CSV error formatter
class CSVErrorFormatter : public ErrorFormatter {
public:
    std::string format(const std::shared_ptr<Error>& error) const override;
    std::string format(const std::vector<std::shared_ptr<Error>>& errors) const override;
    std::string getFormatName() const override { return "CSV"; }
};

// Error filter for selective error reporting
class ErrorFilter {
public:
    virtual ~ErrorFilter() = default;
    virtual bool shouldReport(const std::shared_ptr<Error>& error) const = 0;
    virtual bool shouldReport(ErrorSeverity severity) const = 0;
    virtual bool shouldReport(ErrorType type) const = 0;
    virtual bool shouldReport(const std::string& category) const = 0;
    virtual bool shouldReport(const std::string& context) const = 0;
};

// Severity-based error filter
class SeverityFilter : public ErrorFilter {
public:
    SeverityFilter(ErrorSeverity minSeverity) : minSeverity(minSeverity) {}
    bool shouldReport(const std::shared_ptr<Error>& error) const override;
    bool shouldReport(ErrorSeverity severity) const override;
    bool shouldReport(ErrorType type) const override { return true; }
    bool shouldReport(const std::string& category) const override { return true; }
    bool shouldReport(const std::string& context) const override { return true; }
    
private:
    ErrorSeverity minSeverity;
};

// Error-type-based filter
class ErrorTypeFilter : public ErrorFilter {
public:
    ErrorTypeFilter(const std::set<ErrorType>& errorTypes) : errorTypes(errorTypes) {}
    bool shouldReport(const std::shared_ptr<Error>& error) const override;
    bool shouldReport(ErrorSeverity severity) const override { return true; }
    bool shouldReport(ErrorType type) const override;
    bool shouldReport(const std::string& category) const override { return true; }
    bool shouldReport(const std::string& context) const override { return true; }
    
private:
    std::set<ErrorType> errorTypes;
};

// Context-based filter
class ContextFilter : public ErrorFilter {
public:
    ContextFilter(const std::set<std::string>& contexts) : contexts(contexts) {}
    bool shouldReport(const std::shared_ptr<Error>& error) const override;
    bool shouldReport(ErrorSeverity severity) const override { return true; }
    bool shouldReport(ErrorType type) const override { return true; }
    bool shouldReport(const std::string& category) const override { return true; }
    bool shouldReport(const std::string& context) const override;
    
private:
    std::set<std::string> contexts;
};

// Error sink for asynchronous error processing
class ErrorSink {
public:
    virtual ~ErrorSink() = default;
    virtual void consume(std::shared_ptr<Error> error) = 0;
    virtual void flush() = 0;
    virtual size_t getProcessedCount() const = 0;
    virtual bool isHealthy() const = 0;
};

// Asynchronous error queue for error sinks
class AsynchronousErrorQueue {
public:
    AsynchronousErrorQueue(size_t maxQueueSize = 1000, size_t workerThreads = 4);
    ~AsynchronousErrorQueue();
    
    void push(std::shared_ptr<Error> error);
    bool tryPush(std::shared_ptr<Error> error);
    std::shared_ptr<Error> pop();
    bool tryPop(std::shared_ptr<Error>& error);
    size_t size() const;
    bool empty() const;
    void setSink(std::shared_ptr<ErrorSink> sink);
    void start();
    void stop();
    bool isRunning() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Error reporter with advanced features
class ErrorReporter {
public:
    ErrorReporter();
    explicit ErrorReporter(const std::string& name);
    ~ErrorReporter();

    // Disable copying
    ErrorReporter(const ErrorReporter&) = delete;
    ErrorReporter& operator=(const ErrorReporter&) = delete;

    // Configuration
    void setName(const std::string& name) { name_ = name; }
    const std::string& getName() const { return name_; }
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    void setErrorLevel(ErrorSeverity level) { errorLevel_ = level; }
    ErrorSeverity getErrorLevel() const { return errorLevel_; }
    void setOutputFormat(ErrorOutputFormat format) { outputFormat_ = format; }
    ErrorOutputFormat getOutputFormat() const { return outputFormat_; }
    void setMaxErrors(size_t maxErrors) { maxErrors_ = maxErrors; }
    size_t getMaxErrors() const { return maxErrors_; }
    void setErrorBufferSize(size_t bufferSize) { errorBufferSize_ = bufferSize; }
    size_t getErrorBufferSize() const { return errorBufferSize_; }

    // Error management
    void addError(std::shared_ptr<Error> error);
    void addError(const std::string& message, ErrorType type = ErrorType::Unknown,
                  ErrorSeverity severity = ErrorSeverity::Error,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "",
                  const std::string& category = "",
                  const ErrorContext& context = ErrorContext());
    void addEnhancedError(std::shared_ptr<EnhancedError> error);
    
    void removeError(std::shared_ptr<Error> error);
    void clear();
    
    size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const;
    std::vector<std::shared_ptr<Error>> getErrors(ErrorSeverity severity = ErrorSeverity::Error) const;
    std::vector<std::shared_ptr<EnhancedError>> getEnhancedErrors(ErrorSeverity severity = ErrorSeverity::Error) const;
    
    std::shared_ptr<Error> getLastError() const;
    std::shared_ptr<EnhancedError> getLastEnhancedError() const;

    // Error filtering
    void addFilter(std::shared_ptr<ErrorFilter> filter);
    void removeFilter(std::shared_ptr<ErrorFilter> filter);
    void clearFilters();
    bool shouldReport(const std::shared_ptr<Error>& error) const;
    
    // Error formatting
    void addFormatter(std::shared_ptr<ErrorFormatter> formatter);
    void removeFormatter(const std::string& formatName);
    void clearFormatters();
    std::string formatErrors(const std::vector<std::shared_ptr<Error>>& errors) const;
    std::string formatErrors(ErrorSeverity severity) const;
    std::string formatAllErrors() const;

    // Error reporting
    void report(std::shared_ptr<Error> error);
    void report(const std::string& message, ErrorType type = ErrorType::Unknown,
                ErrorSeverity severity = ErrorSeverity::Error,
                const std::string& file = "", int line = 0,
                const std::string& function = "",
                const std::string& category = "",
                const ErrorContext& context = ErrorContext());
    
    void logDebug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
                  const std::string& context = "");
    void logInfo(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
                 const std::string& context = "");
    void logWarning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
                    const std::string& context = "");
    void logError(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
                  const std::string& context = "");
    void logCritical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
                     const std::string& context = "");

    // Error statistics
    const ErrorStatistics& getStatistics() const { return statistics_; }
    std::string getStatisticsReport() const;
    std::string getErrorsByTypeReport() const;
    std::string getErrorsBySeverityReport() const;
    std::string getErrorsByContextReport() const;

    // Error recovery
    bool attemptRecovery(std::shared_ptr<Error> error);
    bool autoRecover();
    bool enableAutoRecovery(bool enabled) { return autoRecoveryEnabled_ = enabled; }
    bool isAutoRecoveryEnabled() const { return autoRecoveryEnabled_; }

    // Asynchronous processing
    void enableAsynchronousProcessing(bool enabled);
    bool isAsynchronousProcessingEnabled() const;
    void setErrorQueue(std::shared_ptr<AsynchronousErrorQueue> queue);
    void setErrorSink(std::shared_ptr<ErrorSink> sink);

    // Context management
    void setDefaultContext(const ErrorContext& context) { defaultContext_ = context; }
    const ErrorContext& getDefaultContext() const { return defaultContext_; }
    ErrorContext getEnrichedContext(const std::string& operation, const std::string& component,
                                    const std::map<std::string, std::string>& metadata = std::map<std::string, std::string>()) const;

    // Export/import
    bool exportErrors(const std::string& filepath) const;
    bool importErrors(const std::string& filepath);
    bool exportStatistics(const std::string& filepath) const;

    // Global error reporter management
    static ErrorReporter& getGlobal();
    static void setGlobal(ErrorReporter& reporter);
    static void resetGlobal();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    std::string name_;
    bool enabled_;
    ErrorSeverity errorLevel_;
    ErrorOutputFormat outputFormat_;
    size_t maxErrors_;
    size_t errorBufferSize_;
    ErrorContext defaultContext_;
    ErrorStatistics statistics_;
    bool autoRecoveryEnabled_;
    std::shared_ptr<AsynchronousErrorQueue> errorQueue_;
    std::shared_ptr<ErrorSink> errorSink_;

    void updateStatistics(const std::shared_ptr<Error>& error);
    std::string formatErrorWithFormatter(const std::shared_ptr<Error>& error) const;
};

// Error context builder for convenient error context creation
class ErrorContextBuilder {
public:
    ErrorContextBuilder& setOperation(const std::string& operation);
    ErrorContextBuilder& setComponent(const std::string& component);
    ErrorContextBuilder& setSubsystem(const std::string& subsystem);
    ErrorContextBuilder& setModule(const std::string& module);
    ErrorContextBuilder& addMetadata(const std::string& key, const std::string& value);
    ErrorContextBuilder& setUserId(const std::string& userId);
    ErrorContextBuilder& setSessionId(const std::string& sessionId);
    ErrorContextBuilder& addTag(const std::string& tag, const std::string& value);
    ErrorContext build() const;
    
    static ErrorContextBuilder create() { return ErrorContextBuilder(); }

private:
    ErrorContext context_;
};

// Error reporting macros with enhanced context support
#define NLM_REPORT_ERROR(type, severity, message, context) \
    do { \
        nlm::ErrorContextBuilder builder; \
        builder.setOperation(context.operation).setComponent(context.component).setSubsystem(context.subsystem); \
        ErrorReporter::getGlobal().addError( \
            std::make_shared<type>(message, severity, __FILE__, __LINE__, __func__, "", builder.build()) \
        ); \
    } while(0)

#define NLM_REPORT_ENHANCED_ERROR(type, severity, message, context) \
    do { \
        nlm::ErrorContextBuilder builder; \
        builder.setOperation(context.operation).setComponent(context.component).setSubsystem(context.subsystem); \
        ErrorReporter::getGlobal().addEnhancedError( \
            std::make_shared<type>(message, severity, __FILE__, __LINE__, __func__, "", builder.build()) \
        ); \
    } while(0)

#define NLM_VALIDATE_AND_REPORT(condition, message, errorType, severity, context) \
    do { \
        if (!(condition)) { \
            NLM_REPORT_ERROR(errorType, severity, message, context); \
        } \
    } while(0)

#define NLM_THROW_AND_REPORT(type, severity, message, context) \
    do { \
        nlm::ErrorContextBuilder builder; \
        builder.setOperation(context.operation).setComponent(context.component).setSubsystem(context.subsystem); \
        auto error = std::make_shared<type>(message, severity, __FILE__, __LINE__, __func__, "", builder.build()); \
        ErrorReporter::getGlobal().addError(error); \
        throw error; \
    } while(0)

#define NLM_LOG_ERROR_SEVERITY_WITH_CONTEXT(severity, message, context) \
    do { \
        nlm::ErrorContextBuilder builder; \
        builder.setOperation(context.operation).setComponent(context.component).setSubsystem(context.subsystem); \
        ErrorReporter::getGlobal().addError( \
            std::make_shared<Error>(message, ErrorType::Unknown, severity, __FILE__, __LINE__, __func__, "", builder.build()) \
        ); \
    } while(0)

#define NLM_LOG_ERROR_WITH_CONTEXT(message, context) NLM_LOG_ERROR_SEVERITY_WITH_CONTEXT(ErrorSeverity::Error, message, context)
#define NLM_LOG_WARNING_WITH_CONTEXT(message, context) NLM_LOG_ERROR_SEVERITY_WITH_CONTEXT(ErrorSeverity::Warning, message, context)
#define NLM_LOG_INFO_WITH_CONTEXT(message, context) NLM_LOG_ERROR_SEVERITY_WITH_CONTEXT(ErrorSeverity::Info, message, context)
#define NLM_LOG_DEBUG_WITH_CONTEXT(message, context) NLM_LOG_ERROR_SEVERITY_WITH_CONTEXT(ErrorSeverity::Debug, message, context)
#define NLM_LOG_CRITICAL_WITH_CONTEXT(message, context) NLM_LOG_ERROR_SEVERITY_WITH_CONTEXT(ErrorSeverity::Critical, message, context)

// Convenience macros for common error reporting
#define NLM_ERROR(msg) ErrorReporter::getGlobal().logError(msg, __FILE__, __LINE__, __func__)
#define NLM_WARNING(msg) ErrorReporter::getGlobal().logWarning(msg, __FILE__, __LINE__, __func__)
#define NLM_INFO(msg) ErrorReporter::getGlobal().logInfo(msg, __FILE__, __LINE__, __func__)
#define NLM_DEBUG(msg) ErrorReporter::getGlobal().logDebug(msg, __FILE__, __LINE__, __func__)
#define NLM_CRITICAL(msg) ErrorReporter::getGlobal().logCritical(msg, __FILE__, __LINE__, __func__)

} // namespace nlm
