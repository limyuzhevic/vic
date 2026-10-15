#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <map>
#include <functional>

namespace nlm {

class ErrorHandler {
public:
    enum class ErrorType {
        TestFailure,
        ConfigurationError,
        PluginError,
        ResourceError,
        NetworkError,
        MemoryError,
        TimeoutError,
        DependencyError,
        ValidationError,
        SystemError,
        CustomError
    };
    
    struct ErrorInfo {
        ErrorType type;
        std::string message;
        std::string testName;
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, std::string> context;
        bool recoverable;
        std::string recoveryAction;
        std::chrono::milliseconds suggestedDelay;
        
        // Constructor
        ErrorInfo(ErrorType t, const std::string& msg, const std::string& test = "")
            : type(t), message(msg), testName(test), file(""), line(0), function(""),
              timestamp(std::chrono::system_clock::now()), recoverable(false),
              recoveryAction(""), suggestedDelay(std::chrono::milliseconds(0)) {}
    };
    
    virtual ~ErrorHandler() = default;
    
    // Error handling interface
    virtual void handleError(const ErrorInfo& error) = 0;
    virtual bool canRecover(const ErrorInfo& error) const = 0;
    virtual std::string getRecoveryAction(const ErrorInfo& error) const = 0;
    virtual std::chrono::milliseconds getRecoveryDelay(const ErrorInfo& error) const = 0;
    
    // Error classification
    virtual ErrorType classifyError(const std::string& errorMessage, const std::string& context = "") const;
    
    // Error recovery
    virtual bool attemptRecovery(const ErrorInfo& error);
    virtual void resetErrorState();
    
    // Error statistics
    virtual size_t getErrorCount(ErrorType type = ErrorType::CustomError) const;
    virtual std::vector<ErrorInfo> getErrors(ErrorType type = ErrorType::CustomError) const;
    virtual void clearErrors();
    
    // Configuration
    virtual void setMaxRetries(size_t maxRetries) { maxRetries_ = maxRetries; }
    virtual void setEnableAutoRecovery(bool enable) { enableAutoRecovery_ = enable; }
    virtual void setRecoveryDelay(const std::chrono::milliseconds& delay) { recoveryDelay_ = delay; }
    
protected:
    std::vector<ErrorInfo> errors_;
    size_t maxRetries_;
    bool enableAutoRecovery_;
    std::chrono::milliseconds recoveryDelay_;
};

// Default error handler implementation
class DefaultErrorHandler : public ErrorHandler {
public:
    void handleError(const ErrorInfo& error) override;
    bool canRecover(const ErrorInfo& error) const override;
    std::string getRecoveryAction(const ErrorInfo& error) const override;
    std::chrono::milliseconds getRecoveryDelay(const ErrorInfo& error) const override;
    
    // Enhanced error handling
    void logError(const ErrorInfo& error);
    void formatErrorReport(std::ostream& output) const;
    
private:
    void categorizeError(ErrorInfo& error);
    std::string getDefaultRecoveryAction(ErrorType type) const;
    std::chrono::milliseconds getDefaultRecoveryDelay(ErrorType type) const;
};

// Silent error handler (suppresses errors)
class SilentErrorHandler : public ErrorHandler {
public:
    void handleError(const ErrorInfo& error) override {}
    bool canRecover(const ErrorInfo& error) const override { return false; }
    std::string getRecoveryAction(const ErrorInfo& error) const override { return ""; }
    std::chrono::milliseconds getRecoveryDelay(const ErrorInfo& error) const override { return std::chrono::milliseconds(0); }
};

// Exception-based error handler
class ExceptionErrorHandler : public ErrorHandler {
public:
    void handleError(const ErrorInfo& error) override;
    bool canRecover(const ErrorInfo& error) const override;
    std::string getRecoveryAction(const ErrorInfo& error) const override;
    std::chrono::milliseconds getRecoveryDelay(const ErrorInfo& error) const override;
    
    // Exception management
    virtual void throwAsException(const ErrorInfo& error);
    virtual bool catchException(const std::exception& e);
};

// Error handler factory
class ErrorHandlerFactory {
public:
    static std::unique_ptr<ErrorHandler> createErrorHandler(const std::string& type, 
                                                           const std::map<std::string, std::string>& options = {});
    static std::vector<std::string> getAvailableErrorHandlers();
};

} // namespace nlm
