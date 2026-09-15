#include "ErrorSystem.hpp"
#include <format>
#include <stdexcept>
#include <cstring>

namespace nlm {

// Base exception class for all NLM errors
class NLMException : public std::exception {
protected:
    ErrorCode code;
    ErrorSeverity severity;
    std::string message;
    ErrorContext context;
    std::vector<std::string> recoverySteps;
    
public:
    NLMException(ErrorCode code, ErrorSeverity severity,
                 const std::string& message,
                 const ErrorContext& ctx = ErrorContext())
        : code(code), severity(severity), message(message), context(ctx) {}
    
    virtual ~NLMException() = default;
    
    // Getters
    ErrorCode getCode() const { return code; }
    ErrorSeverity getSeverity() const { return severity; }
    const char* what() const noexcept override { return message.c_str(); }
    const std::string& getMessage() const { return message; }
    const ErrorContext& getContext() const { return context; }
    
    // Add recovery steps
    void addRecoveryStep(const std::string& step) {
        recoverySteps.push_back(step);
    }
    
    const std::vector<std::string>& getRecoverySteps() const {
        return recoverySteps;
    }
    
    // Format error as string
    std::string format() const {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(severity) << "] " << static_cast<int>(code)
            << ": " << message;
        if (!context.component.empty()) {
            oss << " (Component: " << context.component << ")";
        }
        if (!context.operation.empty()) {
            oss << " (Operation: " << context.operation << ")";
        }
        if (!context.file.empty()) {
            oss << " (File: " << context.file;
            if (context.line > 0) {
                oss << ":" << context.line;
            }
            oss << ")";
        }
        return oss.str();
    }
};

// Configuration errors
class ConfigError : public NLMException {
public:
    ConfigError(ErrorCode code, const std::string& message,
                const ErrorContext& ctx = ErrorContext())
        : NLMException(code, ErrorSeverity::Error, message, ctx) {}
};

// Neural computation errors
class NeuronError : public NLMException {
public:
    NeuronError(ErrorCode code, const std::string& message,
                const ErrorContext& ctx = ErrorContext())
        : NLMException(code, ErrorSeverity::Error, message, ctx) {}
};

// Memory management errors
class MemoryError : public NLMException {
public:
    MemoryError(ErrorCode code, const std::string& message,
                const ErrorContext& ctx = ErrorContext())
        : NLMException(code, ErrorSeverity::Critical, message, ctx) {}
};

// File I/O errors
class FileError : public NLMException {
public:
    FileError(ErrorCode code, const std::string& message,
              const ErrorContext& ctx = ErrorContext())
        : NLMException(code, ErrorSeverity::Error, message, ctx) {}
};

// Network/spike propagation errors
class NetworkError : public NLMException {
public:
    NetworkError(ErrorCode code, const std::string& message,
                 const ErrorContext& ctx = ErrorContext())
        : NLMException(code, ErrorSeverity::Warning, message, ctx) {}
};

// Development process errors
class DevelopmentError : public NLMException {
public:
    DevelopmentError(ErrorCode code, const std::string& message,
                     const ErrorContext& ctx = ErrorContext())
        : NLMException(code, ErrorSeverity::Warning, message, ctx) {}
};

// Visualization errors
class VisualizationError : public NLMException {
public:
    VisualizationError(ErrorCode code, const std::string& message,
                       const ErrorContext& ctx = ErrorContext())
        : NLMException(code, ErrorSeverity::Error, message, ctx) {}
};

// Integration errors
class IntegrationError : public NLMException {
public:
    IntegrationError(ErrorCode code, const std::string& message,
                     const ErrorContext& ctx = ErrorContext())
        : NLMException(code, ErrorSeverity::Critical, message, ctx) {}
};

// Error formatter for different output formats
class ErrorFormatter {
public:
    virtual ~ErrorFormatter() = default;
    
    virtual std::string format(const NLMException& error) const = 0;
    virtual std::string formatUserFriendly(const NLMException& error) const = 0;
    virtual std::string formatForLogging(const NLMException& error) const = 0;
};

// Plain text formatter
class PlainTextFormatter : public ErrorFormatter {
public:
    std::string format(const NLMException& error) const override {
        return error.format();
    }
    
    std::string formatUserFriendly(const NLMException& error) const override {
        std::ostringstream oss;
        oss << "Error: " << error.getMessage();
        oss << "\n";
        oss << "(Component: " << error.getContext().component << ")";
        oss << "\n";
        oss << "(Operation: " << error.getContext().operation << ")";
        oss << "\n";
        oss << "\nRecovery Steps:";
        for (size_t i = 0; i < error.getRecoverySteps().size(); ++i) {
            oss << "\n  " << (i + 1) << ". " << error.getRecoverySteps()[i];
        }
        return oss.str();
    }
    
    std::string formatForLogging(const NLMException& error) const override {
        std::ostringstream oss;
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        oss << " [" << static_cast<int>(error.getSeverity()) << "] "
            << static_cast<int>(error.getCode()) << ": "
            << error.getMessage() << "\n";
        if (!error.getContext().file.empty()) {
            oss << "  File: " << error.getContext().file;
            if (error.getContext().line > 0) {
                oss << ":" << error.getContext().line;
            }
            oss << "\n";
        }
        if (!error.getContext().function.empty()) {
            oss << "  Function: " << error.getContext().function << "\n";
        }
        return oss.str();
    }
};

// JSON formatter
class JsonFormatter : public ErrorFormatter {
public:
    std::string format(const NLMException& error) const override {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"code\": " << static_cast<int>(error.getCode()) << ",\n";
        oss << "  \"severity\": " << static_cast<int>(error.getSeverity()) << ",\n";
        oss << "  \"message\": \"" << escapeString(error.getMessage()) << "\",\n";
        oss << "  \"context\": {\n";
        oss << "    \"component\": \"" << error.getContext().component << "\",\n";
        oss << "    \"operation\": \"" << error.getContext().operation << "\"\n";
        oss << "  },\n";
        oss << "  \"recoverySteps\": [";
        for (size_t i = 0; i < error.getRecoverySteps().size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << escapeString(error.getRecoverySteps()[i]) << "\"";
        }
        oss << "]\n";
        oss << "}";
        return oss.str();
    }
    
    std::string formatUserFriendly(const NLMException& error) const override {
        // For user-friendly output, use plain text
        PlainTextFormatter plain;
        return plain.formatUserFriendly(error);
    }
    
    std::string formatForLogging(const NLMException& error) const override {
        std::ostringstream oss;
        oss << "{\n";
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        oss << "  \"timestamp\": \"" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
            << "\",\n";
        oss << "  \"severity\": " << static_cast<int>(error.getSeverity()) << ",\n";
        oss << "  \"code\": " << static_cast<int>(error.getCode()) << ",\n";
        oss << "  \"message\": \"" << escapeString(error.getMessage()) << "\",\n";
        oss << "  \"context\": {\n";
        oss << "    \"component\": \"" << error.getContext().component << "\",\n";
        oss << "    \"operation\": \"" << error.getContext().operation << "\",\n";
        oss << "    \"file\": \"" << error.getContext().file << "\",\n";
        oss << "    \"line\": " << error.getContext().line << ",\n";
        oss << "    \"function\": \"" << error.getContext().function << "\"\n";
        oss << "  }\n";
        oss << "}";
        return oss.str();
    }
    
private:
    std::string escapeString(const std::string& str) const {
        std::ostringstream oss;
        for (char c : str) {
            switch (c) {
                case '\"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default: oss << c; break;
            }
        }
        return oss.str();
    }
};

// Error observer interface for error tracking
class IErrorObserver {
public:
    virtual ~IErrorObserver() = default;
    
    virtual void onError(const NLMException& error) = 0;
    virtual void onWarning(const NLMException& warning) = 0;
    virtual void onCritical(const NLMException& critical) = 0;
};

// Error statistics collector
struct ErrorStats {
    ErrorCode code;
    ErrorSeverity severity;
    std::chrono::system_clock::time_point firstOccurrence;
    std::chrono::system_clock::time_point lastOccurrence;
    uint64_t count;
    std::map<std::string, uint64_t> occurrencesByComponent;
    double averageImpact;  // Estimated performance impact
    
    ErrorStats(ErrorCode code, ErrorSeverity severity)
        : code(code), severity(severity), count(0), averageImpact(0.0) {
        firstOccurrence = std::chrono::system_clock::now();
        lastOccurrence = firstOccurrence;
    }
};

// Error reporting and recovery system
class ErrorReporter {
private:
    static ErrorReporter* instance;
    std::unique_ptr<ErrorFormatter> formatter;
    std::vector<std::shared_ptr<IErrorObserver>> observers;
    std::map<ErrorCode, ErrorStats> errorStats;
    std::map<std::string, std::function<bool(const NLMException&)>> recoveryHandlers;
    
public:
    static ErrorReporter& getInstance() {
        if (!instance) {
            instance = new ErrorReporter();
        }
        return *instance;
    }
    
    static void destroyInstance() {
        delete instance;
        instance = nullptr;
    }
    
    ErrorReporter() {
        formatter = std::make_unique<PlainTextFormatter>();
    }
    
    void setFormatter(std::unique_ptr<ErrorFormatter> fmt) {
        formatter = std::move(fmt);
    }
    
    void addObserver(std::shared_ptr<IErrorObserver> observer) {
        observers.push_back(observer);
    }
    
    void report(const NLMException& error) {
        // Update statistics
        auto now = std::chrono::system_clock::now();
        if (errorStats.find(error.getCode()) == errorStats.end()) {
            errorStats[error.getCode()] = ErrorStats(error.getCode(), error.getSeverity());
        }
        
        ErrorStats& stats = errorStats[error.getCode()];
        stats.lastOccurrence = now;
        stats.count++;
        stats.occurrencesByComponent[error.getContext().component]++;
        
        // Notify observers
        for (const auto& observer : observers) {
            switch (error.getSeverity()) {
                case ErrorSeverity::Info:
                    observer->onError(error);
                    break;
                case ErrorSeverity::Warning:
                    observer->onWarning(error);
                    break;
                case ErrorSeverity::Error:
                case ErrorSeverity::Critical:
                case ErrorSeverity::Fatal:
                    observer->onCritical(error);
                    break;
            }
        }
        
        // Try recovery if available
        if (recoveryHandlers.count(error.getCode()) > 0) {
            recoveryHandlers[error.getCode()](error);
        }
    }
    
    std::string formatError(const NLMException& error) const {
        return formatter->format(error);
    }
    
    std::string formatUserFriendly(const NLMException& error) const {
        return formatter->formatUserFriendly(error);
    }
    
    std::string formatForLogging(const NLMException& error) const {
        return formatter->formatForLogging(error);
    }
    
    const std::map<ErrorCode, ErrorStats>& getErrorStats() const {
        return errorStats;
    }
    
    void addRecoveryHandler(ErrorCode code, std::function<bool(const NLMException&)> handler) {
        recoveryHandlers[code] = handler;
    }
    
    bool tryRecover(ErrorCode code, const NLMException& error) {
        if (recoveryHandlers.find(code) != recoveryHandlers.end()) {
            return recoveryHandlers[code](error);
        }
        return false;
    }
};

// Initialize static instance
ErrorReporter* ErrorReporter::instance = nullptr;

// Convenience functions for error reporting
inline void reportError(const NLMException& error) {
    ErrorReporter::getInstance().report(error);
}

inline void reportError(ErrorCode code, const std::string& message,
                        const std::string& component = "Unknown",
                        const std::string& operation = "Unknown",
                        const std::string& file = "",
                        int line = 0,
                        const std::string& function = "") {
    ErrorContext ctx(component, operation, file, line, function);
    NLMException error(code, ErrorSeverity::Error, message, ctx);
    reportError(error);
}

} // namespace nlm