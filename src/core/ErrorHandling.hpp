// Comprehensive error handling and logging for NLM
// Provides robust error checking and consistent error reporting

#ifndef NLM_ERROR_HANDLING_H
#define NLM_ERROR_HANDLING_H

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <sstream>
#include <iomanip>
#include <memory>
#include <thread>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Error category
enum class ErrorCategory {
    None = 0,
    Memory = 1,
    File = 2,
    Configuration = 3,
    Neural = 4,
    Plasticity = 5,
    Neuromodulation = 6,
    World = 7,
    Agent = 8,
    System = 9
};

// Structured error information
struct ErrorInfo {
    ErrorSeverity severity;
    ErrorCategory category;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::map<std::string, std::string> context;
    std::chrono::system_clock::time_point timestamp;
    
    ErrorInfo(ErrorSeverity sev, ErrorCategory cat, const std::string& msg,
              const std::string& file = "", int line = 0, 
              const std::string& func = "") 
        : severity(sev), category(cat), message(msg), file(file), line(line),
          function(func), timestamp(std::chrono::system_clock::now()) {}
};

// Exception class for NLM errors
class NLMError : public std::runtime_error {
public:
    NLMError(const ErrorInfo& info) 
        : std::runtime_error(formatError(info)), info_(info) {}
    
    const ErrorInfo& getInfo() const { return info_; }
    ErrorSeverity getSeverity() const { return info_.severity; }
    ErrorCategory getCategory() const { return info_.category; }
    const std::string& getMessage() const { return info_.message; }
    
    // Add context to existing error
    NLMError& addContext(const std::string& key, const std::string& value) {
        info_.context[key] = value;
        return *this;
    }
    
private:
    static std::string formatError(const ErrorInfo& info) {
        std::stringstream ss;
        ss << "[NLM " << static_cast<int>(info.severity) << "] ";
        ss << "[" << static_cast<int>(info.category) << "] ";
        ss << info.message;
        if (!info.file.empty()) {
            ss << " (" << info.file << ":" << info.line << "; " << info.function << ";";
        }
        return ss.str();
    }
    
    ErrorInfo info_;
};

// Error reporting interface
class ErrorReporter {
public:
    virtual ~ErrorReporter() = default;
    
    virtual void report(const ErrorInfo& error) = 0;
    virtual void report(const NLMError& error) {
        report(error.getInfo());
    }
    
    virtual void setErrorLevel(ErrorSeverity level) = 0;
    virtual ErrorSeverity getErrorLevel() const = 0;
    
    virtual void setFormat(const std::string& format) = 0;
    virtual std::string getFormat() const = 0;
};

// Thread-safe error reporter base
class ThreadSafeErrorReporter : public ErrorReporter {
protected:
    mutable std::mutex mutex_;
    
    ThreadSafeErrorReporter() = default;
    
    std::lock_guard<std::mutex> lock() const {
        return std::lock_guard<std::mutex>(mutex_);
    }
};

// Console error reporter (default)
class ConsoleErrorReporter : public ThreadSafeErrorReporter {
public:
    ConsoleErrorReporter() : errorLevel_(ErrorSeverity::Warning), 
                             includeTimestamps_(true),
                             includeCategory_(true),
                             includeFileLocation_(true) {}
    
    void report(const ErrorInfo& error) override {
        auto lock = this->lock();
        
        if (error.severity >= errorLevel_) {
            std::string timestamp = includeTimestamps_ ? getTimestamp(error.timestamp) : "";
            
            std::stringstream ss;
            if (!timestamp.empty()) ss << "[" << timestamp << "] ";
            
            std::string severityStr;
            switch (error.severity) {
                case ErrorSeverity::Debug: severityStr = "DEBUG"; break;
                case ErrorSeverity::Info: severityStr = "INFO"; break;
                case ErrorSeverity::Warning: severityStr = "WARNING"; break;
                case ErrorSeverity::Error: severityStr = "ERROR"; break;
                case ErrorSeverity::Critical: severityStr = "CRITICAL"; break;
            }
            
            ss << "[" << severityStr << "] ";
            
            if (includeCategory_) {
                ss << "[" << static_cast<int>(error.category) << "] ";
            }
            
            ss << error.message;
            
            if (includeFileLocation_ && !error.file.empty()) {
                ss << " (" << error.file << ":" << error.line << "; "
                   << error.function << ")";
            }
            
            std::cerr << ss.str() << std::endl;
            
            // Print context if available and not empty
            if (!error.context.empty() && error.severity >= ErrorSeverity::Warning) {
                std::cerr << "  Context:" << std::endl;
                for (const auto& pair : error.context) {
                    std::cerr << "    " << pair.first << ": " << pair.second << std::endl;
                }
            }
            
            // For critical errors, also log to a file or system log
            if (error.severity == ErrorSeverity::Critical) {
                logCriticalError(error);
            }
        }
    }
    
    void setErrorLevel(ErrorSeverity level) override {
        auto lock = this->lock();
        errorLevel_ = level;
    }
    
    ErrorSeverity getErrorLevel() const override {
        auto lock = this->lock();
        return errorLevel_;
    }
    
    void setFormat(const std::string& format) override {
        auto lock = this->lock();
        format_ = format;
    }
    
    std::string getFormat() const override {
        auto lock = this->lock();
        return format_;
    }
    
    void setIncludeTimestamps(bool include) {
        auto lock = this->lock();
        includeTimestamps_ = include;
    }
    
    bool getIncludeTimestamps() const {
        auto lock = this->lock();
        return includeTimestamps_;
    }
    
    void setIncludeCategory(bool include) {
        auto lock = this->lock();
        includeCategory_ = include;
    }
    
    bool getIncludeCategory() const {
        auto lock = this->lock();
        return includeCategory_;
    }
    
    void setIncludeFileLocation(bool include) {
        auto lock = this->lock();
        includeFileLocation_ = include;
    }
    
    bool getIncludeFileLocation() const {
        auto lock = this->lock();
        return includeFileLocation_;
    }
    
    void setLogFile(const std::string& filepath) {
        auto lock = this->lock();
        logFilepath_ = filepath;
        if (!logFilepath_.empty()) {
            openLogFile();
        }
    }
    
    std::string getLogFile() const {
        auto lock = this->lock();
        return logFilepath_;
    }
    
private:
    void openLogFile() {
        if (!logFile_.is_open()) {
            logFile_.open(logFilepath_, std::ios::app);
            if (logFile_.is_open()) {
                logFile_ << "NLM Error Log - Started at " << getCurrentTimestamp() << std::endl;
            }
        }
    }
    
    void logCriticalError(const ErrorInfo& error) {
        if (!logFilepath_.empty()) {
            openLogFile();
            if (logFile_.is_open()) {
                logFile_ << "CRITICAL ERROR: " << error.message << std::endl;
                logFile_ << "  Time: " << getTimestamp(error.timestamp) << std::endl;
                logFile_ << "  File: " << error.file << ":" << error.line << std::endl;
                logFile_ << "  Function: " << error.function << std::endl;
                logFile_ << std::endl;
            }
        }
        // Here you could also log to system logs, crash reporting, etc.
    }
    
    std::string getTimestamp(const std::chrono::system_clock::time_point& tp) const {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string getCurrentTimestamp() const {
        return getTimestamp(std::chrono::system_clock::now());
    }
    
    std::string format_;
    bool includeTimestamps_;
    bool includeCategory_;
    bool includeFileLocation_;
    std::string logFilepath_;
    mutable std::ofstream logFile_;
};

// Memory error reporter
class MemoryErrorReporter : public ThreadSafeErrorReporter {
public:
    MemoryErrorReporter() {
        setErrorLevel(ErrorSeverity::Error);
        setIncludeTimestamps(true);
        setIncludeCategory(true);
        setIncludeFileLocation(true);
    }
    
    void report(const ErrorInfo& error) override {
        auto lock = this->lock();
        
        // Add memory-specific context
        ErrorInfo enhancedError = error;
        enhancedError.context["memory_address"] = std::to_string(reinterpret_cast<uintptr_t>(&error.message[0]));
        
        // Report memory errors with enhanced information
        if (error.category == ErrorCategory::Memory) {
            std::cerr << "MEMORY ERROR: " << error.message << std::endl;
            if (!error.file.empty()) {
                std::cerr << "  File: " << error.file << ":" << error.line << std::endl;
            }
        }
        
        ThreadSafeErrorReporter::report(enhancedError);
    }
};

// Global error reporter instance
class GlobalErrorReporter {
public:
    static GlobalErrorReporter& instance() {
        static GlobalErrorReporter instance;
        return instance;
    }
    
    ErrorReporter* getReporter() const { 
        auto lock = reporterMutex_.lock();
        return reporter_.get(); 
    }
    
    void setReporter(std::unique_ptr<ErrorReporter> reporter) {
        auto lock = reporterMutex_.lock();
        reporter_ = std::move(reporter);
    }
    
    template<typename... Args>
    void report(ErrorSeverity severity, ErrorCategory category, 
                const std::string& message, Args&&... args) {
        ErrorInfo info(severity, category, message);
        formatContext(info, std::forward<Args>(args)...);
        if (reporter_) {
            reporter_->report(info);
        }
    }
    
    template<typename... Args>
    void report(const NLMError& error, Args&&... args) {
        ErrorInfo info = error.getInfo();
        formatContext(info, std::forward<Args>(args)...);
        if (reporter_) {
            reporter_->report(info);
        }
    }
    
    // Convenience functions for common error types
    template<typename... Args>
    void error(ErrorCategory category, const std::string& message, Args&&... args) {
        report(ErrorSeverity::Error, category, message, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void warning(ErrorCategory category, const std::string& message, Args&&... args) {
        report(ErrorSeverity::Warning, category, message, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void info(ErrorCategory category, const std::string& message, Args&&... args) {
        report(ErrorSeverity::Info, category, message, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void debug(ErrorCategory category, const std::string& message, Args&&... args) {
        report(ErrorSeverity::Debug, category, message, std::forward<Args>(args)...);
    }
    
    // Error checking functions
    template<typename... Args>
    void check(bool condition, ErrorSeverity severity, ErrorCategory category, 
               const std::string& message, Args&&... args) {
        if (!condition) {
            report(severity, category, message, std::forward<Args>(args)...);
        }
    }
    
    template<typename... Args>
    void checkNull(const void* ptr, ErrorCategory category, 
                   const std::string& message, Args&&... args) {
        if (ptr == nullptr) {
            report(ErrorSeverity::Error, category, message, std::forward<Args>(args)...);
        }
    }
    
    // Memory allocation error reporting
    void reportMemoryError(const std::string& operation, size_t size, 
                          const std::string& hint = "") {
        std::stringstream ss;
        ss << "Memory allocation failed for " << operation << " (" << size << " bytes)";
        if (!hint.empty()) {
            ss << ": " << hint;
        }
        
        report(ErrorSeverity::Error, ErrorCategory::Memory, ss.str())
            .addContext("operation", operation)
            .addContext("size", std::to_string(size))
            .addContext("hint", hint);
    }
    
private:
    GlobalErrorReporter() {
        setReporter(std::make_unique<ConsoleErrorReporter>());
    }
    
    template<typename Arg>
    void formatContext(ErrorInfo& info, Arg&& arg) {
        if constexpr (std::is_convertible_v<Arg, std::string>) {
            info.context["arg"] = std::forward<Arg>(arg);
        } else if constexpr (std::is_convertible_v<Arg, const char*>) {
            info.context["arg"] = std::forward<Arg>(arg);
        }
    }
    
    template<typename Arg, typename... Args>
    void formatContext(ErrorInfo& info, Arg&& arg, Args&&... rest) {
        formatContext(info, std::forward<Arg>(arg));
        formatContext(info, std::forward<Args>(rest)...);
    }
    
    std::unique_ptr<ErrorReporter> reporter_;
    mutable std::mutex reporterMutex_;
};

// Assertion macros for debugging
#define NLM_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            ::nlm::GlobalErrorReporter::instance().error( \
                ::nlm::ErrorCategory::System, \
                "Assertion failed: " #cond " - " #msg); \
            throw ::nlm::NLMError(::nlm::ErrorInfo( \
                ::nlm::ErrorSeverity::Critical, \
                ::nlm::ErrorCategory::System, \
                "Assertion failed: " #cond " - " #msg, \
                __FILE__, __LINE__, __func__)); \
        } \
    } while(0)

#define NLM_THROW(category, msg) \
    throw ::nlm::NLMError(::nlm::ErrorInfo( \
        ::nlm::ErrorSeverity::Error, \
        category, \
        msg, \
        __FILE__, __LINE__, __func__))

#define NLM_CHECK_NULL(ptr, msg) \
    do { \
        if ((ptr) == nullptr) { \
            ::nlm::GlobalErrorReporter::instance().error( \
                ::nlm::ErrorCategory::System, \
                "Null pointer check failed: " #ptr " - " #msg); \
            throw ::nlm::NLMError(::nlm::ErrorInfo( \
                ::nlm::ErrorSeverity::Error, \
                ::nlm::ErrorCategory::System, \
                "Null pointer check failed: " #ptr " - " #msg, \
                __FILE__, __LINE__, __func__)); \
        } \
    } while(0)

#define NLM_LOG_ERROR(msg) \
    ::nlm::GlobalErrorReporter::instance().error( \
        ::nlm::ErrorCategory::System, msg)

#define NLM_LOG_WARNING(msg) \
    ::nlm::GlobalErrorReporter::instance().warning( \
        ::nlm::ErrorCategory::System, msg)

#define NLM_LOG_INFO(msg) \
    ::nlm::GlobalErrorReporter::instance().info( \
        ::nlm::ErrorCategory::System, msg)

#define NLM_LOG_DEBUG(msg) \
    ::nlm::GlobalErrorReporter::instance().debug( \
        ::nlm::ErrorCategory::System, msg)

// Helper function to get current timestamp
inline std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm

#endif // NLM_ERROR_HANDLING_H