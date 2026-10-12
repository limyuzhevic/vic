#include "NLMException.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iomanip>

namespace nlm {

// NLMException implementation
NLMException::NLMException(const std::string& message,
                           ErrorCategory category,
                           ErrorSeverity severity,
                           const std::string& component,
                           const std::string& file,
                           int line,
                           const std::string& function)
    : std::runtime_error(message), message_(message), category_(category),
      severity_(severity), component_(component), file_(file), line_(line),
      function_(function), timestamp_(std::chrono::system_clock::now()) {}

std::string NLMException::getFormattedError() const {
    std::ostringstream oss;
    oss << "[" << static_cast<int>(severity_) << "] ";
    oss << "[" << static_cast<int>(category_) << "] ";
    if (!component_.empty()) oss << component_ << ": ";
    oss << message_;
    oss << " (" << file_ << ":" << line_ << " in " << function_ << ")";
    oss << " [" << std::chrono::system_clock::to_time_t(timestamp_) << "]";
    return oss.str();
}

// ConfigurationException implementation
ConfigurationException::ConfigurationException(const std::string& message,
                                               const std::string& key,
                                               const std::string& component,
                                               const std::string& file,
                                               int line,
                                               const std::string& function)
    : NLMException(message, ErrorCategory::Configuration, ErrorSeverity::Error,
                   component, file, line, function), key_(key) {}

// MemoryException implementation
MemoryException::MemoryException(const std::string& message,
                                 size_t sizeRequested,
                                 const std::string& component,
                                 const std::string& file,
                                 int line,
                                 const std::string& function)
    : NLMException(message, ErrorCategory::Memory, ErrorSeverity::Error,
                   component, file, line, function), sizeRequested_(sizeRequested) {}

// NeuralException implementation
NeuralException::NeuralException(const std::string& message,
                                 NeuronId neuronId,
                                 const std::string& component,
                                 const std::string& file,
                                 int line,
                                 const std::string& function)
    : NLMException(message, ErrorCategory::Neural, ErrorSeverity::Error,
                   component, file, line, function), neuronId_(neuronId) {}

// PlasticityException implementation
PlasticityException::PlasticityException(const std::string& message,
                                         const std::string& rule,
                                         const std::string& component,
                                         const std::string& file,
                                         int line,
                                         const std::string& function)
    : NLMException(message, ErrorCategory::Plasticity, ErrorSeverity::Error,
                   component, file, line, function), rule_(rule) {}

// IOError implementation
IOError::IOError(const std::string& message,
                const std::string& filepath,
                IOError::Operation operation,
                const std::string& component,
                const std::string& file,
                int line,
                const std::string& function)
    : NLMException(message, ErrorCategory::I/O, ErrorSeverity::Error,
                   component, file, line, function), filepath_(filepath),
      operation_(operation) {}

// IntegrationException implementation
IntegrationException::IntegrationException(const std::string& message,
                                           const std::string& component,
                                           const std::string& file,
                                           int line,
                                           const std::string& function)
    : NLMException(message, ErrorCategory::Integration, ErrorSeverity::Critical,
                   component, file, line, function) {}

// DefaultErrorHandler implementation
void DefaultErrorHandler::handleError(const NLMException& error, const ErrorContext& context) {
    // Convert error severity to log level
    LogLevel logLevel;
    switch (error.getSeverity()) {
        case ErrorSeverity::Warning: logLevel = LogLevel::Warning; break;
        case ErrorSeverity::Error: logLevel = LogLevel::Error; break;
        case ErrorSeverity::Critical: logLevel = LogLevel::Critical; break;
        case ErrorSeverity::Fatal: logLevel = LogLevel::Critical; break;
        default: logLevel = LogLevel::Error; break;
    }

    std::string logMessage = error.what();
    if (!context.operation.empty()) {
        logMessage = "Operation '" + context.operation + "': " + logMessage;
    }
    
    if (logLevel >= LogLevel::Error) {
        NLM_LOG_ERROR(logMessage);
    } else {
        NLM_LOG_WARNING(logMessage);
    }
    
    // Log recovery suggestions
    for (const auto& suggestion : context.suggestions) {
        NLM_LOG_INFO("Suggestion: " + suggestion);
    }
}

bool DefaultErrorHandler::shouldHandle(const NLMException& error, const ErrorContext& context) {
    // Always handle errors by default
    return true;
}

RecoveryStrategy DefaultErrorHandler::getRecoveryStrategy(const NLMException& error, const ErrorContext& context) {
    // Return the suggested recovery strategy from context, or default to Retry
    if (context.suggestedRecovery != RecoveryStrategy::Skip) {
        return context.suggestedRecovery;
    }
    
    switch (error.getCategory()) {
        case ErrorCategory::Configuration: return RecoveryStrategy::Shutdown;
        case ErrorCategory::Memory: return RecoveryStrategy::Fallback;
        case ErrorCategory::I/O: return RecoveryStrategy::Retry;
        case ErrorCategory::Integration: return RecoveryStrategy::Shutdown;
        default: return RecoveryStrategy::Retry;
    }
}

// ErrorReporter implementation
ErrorReporter::ErrorReporter() : pImpl(std::make_unique<Impl>()) {}

ErrorReporter::~ErrorReporter() = default;

ErrorReporter& ErrorReporter::getInstance() {
    static ErrorReporter instance;
    return instance;
}

void ErrorReporter::addErrorHandler(std::shared_ptr<IErrorHandler> handler) {
    pImpl->errorHandlers.push_back(handler);
}

void ErrorReporter::removeErrorHandler(std::shared_ptr<IErrorHandler> handler) {
    auto it = std::find(pImpl->errorHandlers.begin(), pImpl->errorHandlers.end(), handler);
    if (it != pImpl->errorHandlers.end()) {
        pImpl->errorHandlers.erase(it);
    }
}

void ErrorReporter::reportError(const NLMException& error, const ErrorContext& context) {
    pImpl->errorCount++;
    pImpl->errorsByCategory[error.getCategory()]++;
    pImpl->errorsBySeverity[error.getSeverity()]++;
    
    ErrorContext fullContext = context;
    fullContext.timestamp = std::chrono::system_clock::now();
    
    // Log the error
    for (const auto& handler : pImpl->errorHandlers) {
        if (handler->shouldHandle(error, fullContext)) {
            handler->handleError(error, fullContext);
        }
    }
    
    // If log file is set, also write to file
    if (!pImpl->logFile.empty() && pImpl->logLevel <= LogLevel::Error) {
        std::ofstream logFile(pImpl->logFile, std::ios::app);
        if (logFile.is_open()) {
            logFile << error.what() << std::endl;
            logFile.close();
        }
    }
    
    // Check if we've exceeded max errors
    if (pImpl->maxErrors > 0 && pImpl->errorCount > pImpl->maxErrors) {
        NLM_LOG_CRITICAL("Maximum error count exceeded. Shutting down.");
        std::exit(1);
    }
}

void ErrorReporter::setGlobalErrorHandler(std::shared_ptr<IErrorHandler> handler) {
    pImpl->globalErrorHandler = handler;
}

std::shared_ptr<IErrorHandler> ErrorReporter::getGlobalErrorHandler() const {
    return pImpl->globalErrorHandler;
}

void ErrorReporter::setLogLevel(LogLevel level) {
    pImpl->logLevel = level;
}

void ErrorReporter::setLogFile(const std::string& filepath) {
    pImpl->logFile = filepath;
}

void ErrorReporter::setMaxErrors(size_t maxErrors) {
    pImpl->maxErrors = maxErrors;
}

size_t ErrorReporter::getTotalErrors() const {
    return pImpl->errorCount;
}

size_t ErrorReporter::getErrorsByCategory(ErrorCategory category) const {
    auto it = pImpl->errorsByCategory.find(category);
    return it != pImpl->errorsByCategory.end() ? it->second : 0;
}

size_t ErrorReporter::getErrorsBySeverity(ErrorSeverity severity) const {
    auto it = pImpl->errorsBySeverity.find(severity);
    return it != pImpl->errorsBySeverity.end() ? it->second : 0;
}

void ErrorReporter::clearStatistics() {
    pImpl->errorCount = 0;
    pImpl->errorsByCategory.clear();
    pImpl->errorsBySeverity.clear();
}
