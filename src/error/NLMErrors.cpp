// NLM Error Handling and Recovery
// Comprehensive error handling and recovery mechanisms for NLM

#include "error/NLMErrors.hpp"
#include "error/ErrorHandler.hpp"
#include "error/ErrorRecovery.hpp"
#include "error/ErrorContext.hpp"
#include "error/ExceptionTranslator.hpp"
#include <stdexcept>
#include <map>
#include <vector>
#include <stack>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace nlm {

NLMError::NLMError(const std::string& message, ErrorCode code, const std::string& file, int line)
    : std::runtime_error(message), code(code), file(file), line(line), timestamp(std::chrono::system_clock::now()) {
}

NLMError::NLMError(const std::string& message, ErrorCode code, const std::string& file, int line,
                   const std::map<std::string, std::string>& context)
    : std::runtime_error(message), code(code), file(file), line(line), timestamp(std::chrono::system_clock::now()), context(context) {
}

const char* NLMError::what() const noexcept {
    std::stringstream ss;
    ss << "NLM Error [" << static_cast<int>(code) << "]: " << std::runtime_error::what();
    if (!file.empty()) {
        ss << " (" << file << ":" << line << ")";
    }
    _whatBuffer = ss.str();
    return _whatBuffer.c_str();
}

std::string NLMError::getContext(const std::string& key) const {
    auto it = context.find(key);
    return (it != context.end()) ? it->second : "";
}

std::string NLMError::getTimestamp() const {
    auto in_time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string NLMError::toString() const {
    std::stringstream ss;
    ss << "Error Code: " << static_cast<int>(code) << "\n";
    ss << "Message: " << std::runtime_error::what() << "\n";
    ss << "File: " << file << "\n";
    ss << "Line: " << line << "\n";
    ss << "Timestamp: " << getTimestamp() << "\n";
    
    if (!context.empty()) {
        ss << "Context:\n";
        for (const auto& pair : context) {
            ss << "  " << pair.first << ": " << pair.second << "\n";
        }
    }
    
    return ss.str();
}

// ============================================================================
// ErrorHandler
// ============================================================================

ErrorHandler::ErrorHandler() : _logLevel(LogLevel::INFO), _outputErrorsToConsole(true), _outputErrorsToFile(false) {
    NLM_LOG_INFO("Initializing Error Handler");
}

ErrorHandler::~ErrorHandler() {
    NLM_LOG_INFO("Shutting down Error Handler");
}

void ErrorHandler::handleError(const NLMError& error) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    // Log the error
    logError(error);
    
    // Check if error can be recovered from
    if (canRecoverFrom(error)) {
        NLM_LOG_INFO("Attempting to recover from error: " + std::to_string(static_cast<int>(error.code)));
        if (recoverFrom(error)) {
            NLM_LOG_INFO("Successfully recovered from error");
            return;
        }
        NLM_LOG_ERROR("Failed to recover from error");
    }
    
    // If error cannot be recovered from, handle it based on severity
    if (error.code >= ErrorCode::CRITICAL) {
        NLM_LOG_CRITICAL("Critical error: " + std::string(error.what()));
    } else if (error.code >= ErrorCode::ERROR) {
        NLM_LOG_ERROR("Error: " + std::string(error.what()));
    } else if (error.code >= ErrorCode::WARNING) {
        NLM_LOG_WARNING("Warning: " + std::string(error.what()));
    } else {
        NLM_LOG_INFO("Info: " + std::string(error.what()));
    }
}

void ErrorHandler::handleException(const std::exception& ex) {
    NLM_LOG_INFO("Handling exception: " + std::string(ex.what()));
    
    // Try to translate to NLM error
    NLMError error = _translator.translate(ex);
    if (!error.what()) {
        // If translation failed, create a generic error
        error = NLMError("Unhandled exception: " + std::string(ex.what()), ErrorCode::INTERNAL_ERROR);
    }
    
    handleError(error);
}

void ErrorHandler::logError(const NLMError& error) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    // Add to error log
    _errorLog.push_back(error);
    
    // Output to console
    if (_outputErrorsToConsole) {
        std::cerr << "[ERROR] " << error.getTimestamp() << ": " << error.what() << std::endl;
    }
    
    // Output to file
    if (_outputErrorsToFile) {
        std::ofstream file(_errorLogFile);
        if (file.is_open()) {
            file << error.toString() << "\n";
            file.close();
        }
    }
}

bool ErrorHandler::canRecoverFrom(const NLMError& error) const {
    // Check if there's a recovery strategy for this error code
    auto it = _recoveryStrategies.find(error.code);
    return (it != _recoveryStrategies.end() && it->second);
}

bool ErrorHandler::recoverFrom(const NLMError& error) {
    auto it = _recoveryStrategies.find(error.code);
    if (it == _recoveryStrategies.end() || !it->second) {
        return false;
    }
    
    try {
        // Execute recovery strategy
        it->second->execute(error);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Recovery strategy failed: " + std::string(e.what()));
        return false;
    }
}

void ErrorHandler::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(_mutex);
    _logLevel = level;
}

void ErrorHandler::setOutputErrors(bool toConsole, bool toFile, const std::string& filePath) {
    std::lock_guard<std::mutex> lock(_mutex);
    _outputErrorsToConsole = toConsole;
    _outputErrorsToFile = toFile;
    if (toFile) {
        _errorLogFile = filePath;
    }
}

void ErrorHandler::addRecoveryStrategy(ErrorCode code, std::unique_ptr<IErrorRecovery> strategy) {
    std::lock_guard<std::mutex> lock(_mutex);
    _recoveryStrategies[code] = std::move(strategy);
}

const std::vector<NLMError>& ErrorHandler::getErrorLog() const {
    return _errorLog;
}

void ErrorHandler::clearErrorLog() {
    std::lock_guard<std::mutex> lock(_mutex);
    _errorLog.clear();
}

void ErrorHandler::exportErrorLog(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open error log file for writing: " + filePath);
        return;
    }
    
    for (const auto& error : _errorLog) {
        file << error.toString() << "\n";
        file << "---\n";
    }
    
    file.close();
    NLM_LOG_INFO("Error log exported to: " + filePath);
}

// ============================================================================
// ErrorRecovery
// ============================================================================

ErrorRecovery::ErrorRecovery(ErrorCode code, const std::string& description)
    : _code(code), _description(description) {
}

ErrorRecovery::~ErrorRecovery() = default;

ErrorCode ErrorRecovery::getCode() const {
    return _code;
}

std::string ErrorRecovery::getDescription() const {
    return _description;
}

void ErrorRecovery::execute(const NLMError& error) {
    // Default implementation does nothing
    // Subclasses should override this method
    NLM_LOG_INFO("Executing recovery for error: " + std::to_string(static_cast<int>(_code)));
}

// ============================================================================
// ErrorContext
// ============================================================================

ErrorContext::ErrorContext() {
    NLM_LOG_INFO("Initializing Error Context");
}

ErrorContext::~ErrorContext() {
    NLM_LOG_INFO("Shutting down Error Context");
}

void ErrorContext::setContext(const std::string& key, const std::string& value) {
    _context[key] = value;
}

std::string ErrorContext::getContext(const std::string& key) const {
    auto it = _context.find(key);
    return (it != _context.end()) ? it->second : "";
}

const std::map<std::string, std::string>& ErrorContext::getAllContext() const {
    return _context;
}

void ErrorContext::clearContext() {
    _context.clear();
}

void ErrorContext::setContextFromError(const NLMError& error) {
    // Copy error context
    _context = error.context;
    
    // Add system context
    _context["system.timestamp"] = error.getTimestamp();
    _context["system.error_code"] = std::to_string(static_cast<int>(error.code));
    _context["system.file"] = error.file;
    _context["system.line"] = std::to_string(error.line);
}

// ============================================================================
// ExceptionTranslator
// ============================================================================

ExceptionTranslator::ExceptionTranslator() {
    NLM_LOG_INFO("Initializing Exception Translator");
    
    // Register common exception translations
    registerTranslation<std::runtime_error>(ErrorCode::RUNTIME_ERROR);
    registerTranslation<std::invalid_argument>(ErrorCode::INVALID_ARGUMENT);
    registerTranslation<std::out_of_range>(ErrorCode::OUT_OF_RANGE);
    registerTranslation<std::bad_alloc>(ErrorCode::MEMORY_ERROR);
    registerTranslation<std::logic_error>(ErrorCode::LOGIC_ERROR);
    registerTranslation<std::domain_error>(ErrorCode::DOMAIN_ERROR);
    registerTranslation<std::length_error>(ErrorCode::LENGTH_ERROR);
    registerTranslation<std::out_of_range>(ErrorCode::OUT_OF_RANGE);
    registerTranslation<std::range_error>(ErrorCode::RANGE_ERROR);
    registerTranslation<std::overflow_error>(ErrorCode::OVERFLOW_ERROR);
    registerTranslation<std::underflow_error>(ErrorCode::UNDERFLOW_ERROR);
}

ExceptionTranslator::~ExceptionTranslator() {
    NLM_LOG_INFO("Shutting down Exception Translator");
}

NLMError ExceptionTranslator::translate(const std::exception& ex) {
    std::string message = ex.what();
    ErrorCode code = ErrorCode::INTERNAL_ERROR;
    
    // Find the appropriate error code
    for (const auto& pair : _translationMap) {
        if (std::is_same<decltype(pair.first), decltype(&ex)>::value) {
            code = pair.second;
            break;
        }
    }
    
    return NLMError(message, code);
}

void ExceptionTranslator::registerTranslation(const std::type_info& type, ErrorCode code) {
    _translationMap[&type] = code;
}

void ExceptionTranslator::registerTranslation(const std::type_info& type, ErrorCode code,
                                               std::function<NLMError(const std::exception&)> translator) {
    _translationMap[&type] = code;
    _customTranslators[&type] = translator;
}

NLMError ExceptionTranslator::translate(const std::exception& ex, const std::type_info& type) {
    auto it = _customTranslators.find(&type);
    if (it != _customTranslators.end()) {
        return it->second(ex);
    }
    
    return translate(ex);
}

} // namespace nlm