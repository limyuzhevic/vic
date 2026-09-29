// error_handling.cpp - Implementation of comprehensive error handling for NLM

#include "ErrorHandling.hpp"
#include <iostream>
#include <stdexcept>
#include <ctime>
#include <sstream>

namespace nlm {

// NLMError implementation
NLMError::NLMError(const std::string& message, 
                   NLMErrorCode code,
                   NLMErrorCategory category,
                   const std::string& file,
                   int line,
                   const std::string& function)
    : std::runtime_error(formatFullError()),
      code_(code),
      category_(category),
      file_(file),
      line_(line),
      function_(function) {
    // Store the original message for reference
    originalMessage_ = message;
}

const char* NLMError::getCategoryName(NLMErrorCategory category) {
    static const char* names[] = {
        "Configuration",
        "Memory", 
        "Simulation",
        "Input",
        "IO",
        "Network",
        "Plugin",
        "System",
        "User",
        "Unknown"
    };
    
    int idx = static_cast<int>(category);
    return (idx >= 0 && idx < static_cast<int>(NLMErrorCategory::Unknown)) ? 
           names[idx] : "Unknown";
}

const char* NLMError::getCodeName(NLMErrorCode code) {
    static const char* names[] = {
        "UnknownError",
        "ConfigFileNotFound",
        "ConfigParseError",
        "ConfigValidationError",
        "ConfigMissingRequired",
        "ConfigInvalidValue",
        "MemoryAllocationFailed",
        "MemoryCorruption",
        "MemoryLimitExceeded",
        "SimulationNotInitialized",
        "SimulationStepFailed",
        "NeuronInvalid",
        "SynapseInvalid",
        "RegionInvalid",
        "InvalidArgument",
        "ArgumentOutOfRange",
        "InputFormatError",
        "FileOpenFailed",
        "FileReadError",
        "FileWriteError",
        "SystemCallFailed",
        "ThreadError",
        "UserInterrupt",
        "UserExit",
        "NotImplemented",
        "FeatureNotAvailable",
        "ResourceUnavailable"
    };
    
    int idx = static_cast<int>(code);
    return (idx >= 0 && idx < static_cast<int>(NLMErrorCode::ResourceUnavailable)) ? 
           names[idx] : "UnknownError";
}

std::string NLMError::formatFullError() const {
    std::ostringstream oss;
    oss << "[" << getCategoryName(category_) << "] ";
    oss << "[" << getCodeName(code_) << "] ";
    
    if (!file_.empty()) {
        oss << file_;
        if (line_ > 0) {
            oss << ":" << line_;
        }
        if (!function_.empty()) {
            oss << " in " << function_;
        }
        oss << "; ";
    }
    
    oss << originalMessage_;
    return oss.str();
}

bool NLMError::isRecoverable() const {
    switch (code_) {
        case NLMErrorCode::ConfigFileNotFound:
        case NLMErrorCode::ConfigParseError:
        case NLMErrorCode::MemoryAllocationFailed:
        case NLMErrorCode::SimulationNotInitialized:
            return true;
        case NLMErrorCode::MemoryCorruption:
        case NLMErrorCode::SystemCallFailed:
        case NLMErrorCode::ThreadError:
            return false;
        default:
            return true;
    }
}

bool NLMError::isUserError() const {
    switch (category_) {
        case NLMErrorCategory::User:
            return true;
        case NLMErrorCategory::Configuration:
            return code_ == NLMErrorCode::ConfigFileNotFound || 
                   code_ == NLMErrorCode::ConfigValidationError;
        case NLMErrorCategory::Input:
            return code_ == NLMErrorCode::InvalidArgument ||
                   code_ == NLMErrorCode::ArgumentOutOfRange;
        default:
            return false;
    }
}

// NLMExceptionFactory implementation
std::runtime_error NLMExceptionFactory::create(const std::string& message,
                                               NLMErrorCode code,
                                               NLMErrorCategory category,
                                               const std::string& file,
                                               int line,
                                               const std::string& function) {
    return NLMError(message, code, category, file, line, function);
}

std::runtime_error NLMExceptionFactory::configFileError(const std::string& filepath, const std::string& reason) {
    std::string message = "Configuration file error: ";
    message += filepath + " - " + reason;
    return NLMError(message, NLMErrorCode::ConfigFileNotFound, NLMErrorCategory::Configuration);
}

std::runtime_error NLMExceptionFactory::configValidationError(const std::string& key, 
                                                             const std::string& expected, 
                                                             const std::string& actual) {
    std::string message = "Configuration validation error for key '" + key + "': ";
    message += "expected " + expected + ", got '" + actual + "'";
    return NLMError(message, NLMErrorCode::ConfigValidationError, NLMErrorCategory::Configuration);
}

std::runtime_error NLMExceptionFactory::memoryError(const std::string& operation, size_t size) {
    std::string message = "Memory allocation failed for " + operation + ": ";
    message += std::to_string(size) + " bytes";
    return NLMError(message, NLMErrorCode::MemoryAllocationFailed, NLMErrorCategory::Memory);
}

std::runtime_error NLMExceptionFactory::simulationError(const std::string& context, const std::string& reason) {
    std::string message = "Simulation error in " + context + ": " + reason;
    return NLMError(message, NLMErrorCode::SimulationStepFailed, NLMErrorCategory::Simulation);
}

std::runtime_error NLMExceptionFactory::inputError(const std::string& parameter, const std::string& value) {
    std::string message = "Invalid input for parameter '" + parameter + "': '" + value + "'";
    return NLMError(message, NLMErrorCode::InvalidArgument, NLMErrorCategory::Input);
}

std::runtime_error NLMExceptionFactory::ioError(const std::string& operation, const std::string& filepath) {
    std::string message = "IO error during " + operation + ": " + filepath;
    return NLMError(message, NLMErrorCode::FileOpenFailed, NLMErrorCategory::IO);
}

std::runtime_error NLMExceptionFactory::systemError(const std::string& operation) {
    std::string message = "System error during " + operation + ": ";
    // Add system error message if possible
    message += "See system logs for details";
    return NLMError(message, NLMErrorCode::SystemCallFailed, NLMErrorCategory::System);
}

// ErrorRecovery implementation
ErrorRecovery::RecoveryAction ErrorRecovery::determineRecoveryAction(const NLMError& error) {
    if (!error.isRecoverable()) {
        return RecoveryAction::Abort;
    }
    
    if (error.isUserError()) {
        return RecoveryAction::Exit;
    }
    
    // Default recovery actions based on error type
    switch (error.getCode()) {
        case NLMErrorCode::ConfigFileNotFound:
        case NLMErrorCode::ConfigParseError:
            return RecoveryAction::Fallback;
        case NLMErrorCode::MemoryAllocationFailed:
            return RecoveryAction::Retry;
        case NLMErrorCode::SimulationNotInitialized:
            return RecoveryAction::Exit;
        case NLMErrorCode::InvalidArgument:
            return RecoveryAction::Exit;
        default:
            return RecoveryAction::Continue;
    }
}

bool ErrorRecovery::canRecover(const NLMError& error) {
    return error.isRecoverable() && !error.isUserError();
}

void ErrorRecovery::logError(const NLMError& error, bool verbose) {
    std::cerr << "ERROR: " << error.what() << std::endl;
    
    if (verbose) {
        std::cerr << "  Full error details:" << std::endl;
        std::cerr << "    Category: " << NLMError::getCategoryName(error.getCategory()) << std::endl;
        std::cerr << "    Code: " << NLMError::getCodeName(error.getCode()) << std::endl;
        if (!error.getFile().empty()) {
            std::cerr << "    Location: " << error.getFile();
            if (error.getLine() > 0) std::cerr << ":" << error.getLine();
            if (!error.getFunction().empty()) std::cerr << " in " << error.getFunction();
            std::cerr << std::endl;
        }
    }
}

void ErrorRecovery::logWarning(const std::string& message) {
    std::cerr << "WARNING: " << message << std::endl;
}

void ErrorRecovery::logInfo(const std::string& message) {
    std::cout << "INFO: " << message << std::endl;
}

void ErrorRecovery::logDebug(const std::string& message) {
    std::cout << "DEBUG: " << message << std::endl;
}

// DefaultErrorHandler implementation
void DefaultErrorHandler::handleError(const NLMError& error) {
    ErrorRecovery::logError(error, true);
}

bool DefaultErrorHandler::shouldExit(const NLMError& error) const {
    return error.isUserError() || 
           error.getCode() == NLMErrorCode::MemoryCorruption ||
           error.getCode() == NLMErrorCode::SystemCallFailed;
}

ErrorRecovery::RecoveryAction DefaultErrorHandler::getRecoveryAction(const NLMError& error) {
    return ErrorRecovery::determineRecoveryAction(error);
}

// ErrorHandlerManager implementation
ErrorHandlerManager& ErrorHandlerManager::getInstance() {
    static ErrorHandlerManager instance;
    return instance;
}

void ErrorHandlerManager::setErrorHandler(std::unique_ptr<IErrorHandler> handler) {
    errorHandler_ = std::move(handler);
}

IErrorHandler& ErrorHandlerManager::getErrorHandler() {
    if (!errorHandler_) {
        errorHandler_ = std::make_unique<DefaultErrorHandler>();
    }
    return *errorHandler_;
}

void ErrorHandlerManager::handleError(const NLMError& error) {
    getErrorHandler().handleError(error);
    
    // Update statistics
    stats_.totalErrors++;
    stats_.errorCounts[error.getCode()]++;
    stats_.categoryCounts[error.getCategory()]++;
    
    if (!error.isRecoverable()) {
        stats_.fatalErrors++;
    } else if (error.isUserError()) {
        // User errors don't count toward recoverable stats
    } else {
        stats_.recoverableErrors++;
    }
    
    if (verbose_) {
        std::cout << "[ErrorHandlerManager] Error handled: " << error.what() << std::endl;
    }
}

bool ErrorHandlerManager::shouldExit(const NLMError& error) {
    return getErrorHandler().shouldExit(error);
}

ErrorRecovery::RecoveryAction ErrorHandlerManager::getRecoveryAction(const NLMError& error) {
    return getErrorHandler().getRecoveryAction(error);
}

// ErrorContext implementation
thread_local std::vector<ErrorContext*> ErrorContext::contextStack_;

ErrorContext::ErrorContext(const std::string& operation, const std::string& details)
    : operation_(operation), details_(details) {
    contextStack_.push_back(this);
}

ErrorContext::~ErrorContext() {
    auto it = std::find(contextStack_.begin(), contextStack_.end(), this);
    if (it != contextStack_.end()) {
        contextStack_.erase(it);
    }
}

void ErrorContext::addDetail(const std::string& key, const std::string& value) {
    detailsMap_[key] = value;
}

std::string ErrorContext::getContextSummary() const {
    std::ostringstream oss;
    oss << operation_;
    if (!details_.empty()) {
        oss << " (" << details_ << ")";
    }
    if (!detailsMap_.empty()) {
        oss << " [";
        bool first = true;
        for (const auto& pair : detailsMap_) {
            if (!first) oss << ", ";
            oss << pair.first << "=" << pair.second;
            first = false;
        }
        oss << "]";
    }
    return oss.str();
}

std::vector<ErrorContext*> ErrorContext::getContextStack() {
    return contextStack_;
}

// Result implementation
void Result::throwIfError() const {
    if (status_ == ErrorResult::Error || status_ == ErrorResult::Fatal) {
        throw NLMError(message_, NLMErrorCode::UnknownError, NLMErrorCategory::Unknown);
    }
}

void Result::throwIfFatal() const {
    if (status_ == ErrorResult::Fatal) {
        throw NLMError(message_, NLMErrorCode::UnknownError, NLMErrorCategory::Unknown);
    }
}

// ResultUtils implementation
template<typename T>
Result<T> ResultUtils::combineResults(const Result<T>& a, const Result<T>& b, const std::string& operation) {
    if (a.isFatal() || b.isFatal()) {
        return Result<T>(a.value(), ErrorResult::Fatal, "Fatal error in " + operation);
    }
    
    if (a.isError() || b.isError()) {
        return Result<T>(a.value(), ErrorResult::Error, "Error in " + operation);
    }
    
    if (a.isWarning() || b.isWarning()) {
        return Result<T>(a.value(), ErrorResult::Warning, "Warning in " + operation);
    }
    
    return Result<T>(a.value(), ErrorResult::Success, "");
}

template<typename T>
Result<T> ResultUtils::safeOperation(const std::function<Result<T>()>& operation, const std::string& fallbackMessage) {
    try {
        Result<T> result = operation();
        if (result.isFatal()) {
            throw NLMError(result.message(), NLMErrorCode::UnknownError, NLMErrorCategory::Unknown);
        }
        return result;
    } catch (const NLMError& e) {
        if (fallbackMessage.empty()) {
            throw;
        }
        return Result<T>(T(), ErrorResult::Error, fallbackMessage + ": " + e.what());
    } catch (const std::exception& e) {
        if (fallbackMessage.empty()) {
            throw NLMError(std::string(e.what()), NLMErrorCode::UnknownError, NLMErrorCategory::Unknown);
        }
        return Result<T>(T(), ErrorResult::Error, fallbackMessage + ": " + std::string(e.what()));
    }
}

template<typename T, typename U>
Result<U> ResultUtils::transformResult(const Result<T>& result, const std::function<U(const T&)>& transform) {
    if (result.isFatal() || result.isError()) {
        return Result<U>(U(), result.status_, result.message());
    }
    
    try {
        U transformed = transform(result.value());
        return Result<U>(std::move(transformed), result.status_, result.message());
    } catch (const std::exception& e) {
        return Result<U>(U(), ErrorResult::Error, "Transformation failed: " + std::string(e.what()));
    }
}

} // namespace nlm