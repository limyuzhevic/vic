// ErrorHandler implementation
#include "ErrorHandler.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace nlm {

struct ErrorHandler::Impl {
    std::vector<ErrorInfo> errorHistory;
    ErrorStats stats;
    ErrorLevel threshold;
    size_t maxHistorySize;
};

ErrorHandler::ErrorHandler() : pImpl(std::make_unique<Impl>()) {
    pImpl->threshold = ErrorLevel::WARNING;
    pImpl->maxHistorySize = 1000;
    pImpl->stats = {0, 0, 0, 0, 0, 0};
}

ErrorHandler::~ErrorHandler() = default;

ErrorHandler::ErrorHandler(ErrorHandler&& other) noexcept : pImpl(std::move(other.pImpl)) {}

ErrorHandler& ErrorHandler::operator=(ErrorHandler&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void ErrorHandler::report(ErrorLevel level, const std::string& category, const std::string& message,
                          const std::string& file, int line, const std::string& context) {
    if (static_cast<int>(level) < static_cast<int>(pImpl->threshold)) {
        return;  // Below threshold, ignore
    }
    
    ErrorInfo info(level, category, message, file, line, context);
    
    // Update statistics
    switch (level) {
        case ErrorLevel::DEBUG: pImpl->stats.debugCount++; break;
        case ErrorLevel::INFO: pImpl->stats.infoCount++; break;
        case ErrorLevel::WARNING: pImpl->stats.warningCount++; break;
        case ErrorLevel::ERROR: pImpl->stats.errorCount++; break;
        case ErrorLevel::CRITICAL: pImpl->stats.criticalCount++; break;
    }
    pImpl->stats.totalCount++;
    
    // Add to history
    pImpl->errorHistory.push_back(info);
    
    // Trim history if needed
    if (pImpl->errorHistory.size() > pImpl->maxHistorySize) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
}

void ErrorHandler::debug(const std::string& category, const std::string& message,
                         const std::string& file, int line, const std::string& context) {
    report(ErrorLevel::DEBUG, category, message, file, line, context);
}

void ErrorHandler::info(const std::string& category, const std::string& message,
                        const std::string& file, int line, const std::string& context) {
    report(ErrorLevel::INFO, category, message, file, line, context);
}

void ErrorHandler::warning(const std::string& category, const std::string& message,
                           const std::string& file, int line, const std::string& context) {
    report(ErrorLevel::WARNING, category, message, file, line, context);
}

void ErrorHandler::error(const std::string& category, const std::string& message,
                         const std::string& file, int line, const std::string& context) {
    report(ErrorLevel::ERROR, category, message, file, line, context);
}

void ErrorHandler::critical(const std::string& category, const std::string& message,
                            const std::string& file, int line, const std::string& context) {
    report(ErrorLevel::CRITICAL, category, message, file, line, context);
}

void ErrorHandler::throwException(ErrorLevel level, const std::string& category,
                                   const std::string& message, const std::string& file,
                                   int line, const std::string& context) {
    report(level, category, message, file, line, context);
    
    // Convert to specific exception type
    std::string fullMessage = message;
    if (!file.empty()) {
        fullMessage += " (" + file + ":" + std::to_string(line) + ")";
    }
    if (!context.empty()) {
        fullMessage += " Context: " + context;
    }
    
    switch (level) {
        case ErrorLevel::DEBUG:
        case ErrorLevel::INFO:
        case ErrorLevel::WARNING:
            // Not exceptional, just log
            break;
        case ErrorLevel::ERROR:
            throw Error(fullMessage);
        case ErrorLevel::CRITICAL:
            throw CriticalError(fullMessage);
    }
}

bool ErrorHandler::validate(bool condition, const std::string& category,
                            const std::string& message, const std::string& file,
                            int line, const std::string& context) {
    if (!condition) {
        report(ErrorLevel::ERROR, category, message, file, line, context);
        return false;
    }
    return true;
}

void ErrorHandler::validate(const std::string& category, const std::string& message,
                            const std::string& file, int line, const std::string& context) {
    // Alias for error reporting
    report(ErrorLevel::ERROR, category, message, file, line, context);
}

template<typename T>
void ErrorHandler::checkBounds(const T& value, const T& min, const T& max,
                               const std::string& category, const std::string& parameter,
                               const std::string& file, int line) {
    if (value < min || value > max) {
        std::string msg = "Parameter '" + parameter + "' out of bounds: " +
                         std::to_string(value) + " (expected " + std::to_string(min) +
                         " to " + std::to_string(max) + ")";
        report(ErrorLevel::ERROR, category, msg, file, line);
    }
}

template<typename T>
void ErrorHandler::checkNull(const T* ptr, const std::string& category,
                             const std::string& parameter, const std::string& file, int line) {
    if (ptr == nullptr) {
        std::string msg = "Null pointer for parameter '" + parameter + "'";
        report(ErrorLevel::ERROR, category, msg, file, line);
    }
}

void ErrorHandler::validateConfig(const Config& config, const std::vector<ConfigSchema>& schema) {
    // This is a stub - would need actual implementation
}

ErrorHandler::ErrorStats ErrorHandler::getErrorStats() const {
    return pImpl->stats;
}

std::vector<ErrorHandler::ErrorInfo> ErrorHandler::getRecentErrors(ErrorLevel minLevel,
                                                                  const std::string& category) const {
    std::vector<ErrorInfo> recentErrors;
    for (const auto& error : pImpl->errorHistory) {
        if (static_cast<int>(error.level) >= static_cast<int>(minLevel) &&
            (category.empty() || error.category == category)) {
            recentErrors.push_back(error);
        }
    }
    return recentErrors;
}

void ErrorHandler::clearErrorHistory() {
    pImpl->errorHistory.clear();
    // Reset stats
    pImpl->stats = {0, 0, 0, 0, 0, 0};
}

void ErrorHandler::setErrorLevelThreshold(ErrorLevel level) {
    pImpl->threshold = level;
}

ErrorHandler::ErrorLevel ErrorHandler::getErrorLevelThreshold() const {
    return pImpl->threshold;
}

// Global error handler instance implementation
namespace {
    std::unique_ptr<ErrorHandler> globalErrorHandlerInstance;
}

ErrorHandler& errorHandler() {
    if (!globalErrorHandlerInstance) {
        globalErrorHandlerInstance = std::make_unique<ErrorHandler>();
    }
    return *globalErrorHandlerInstance;
}

void reportNeuronError(const Neuron& neuron, const std::string& operation,
                       const std::string& context) {
    errorHandler().warning("NEURON", "Neuron error during " + operation + ": " + context);
}

void reportSynapseError(const Synapse& synapse, const std::string& operation,
                        const std::string& context) {
    errorHandler().warning("SYNAPSE", "Synapse error during " + operation + ": " + context);
}

void reportConfigurationError(const std::string& key, const std::string& operation,
                              const std::string& context) {
    errorHandler().warning("CONFIG", "Configuration error for key '" + key + "' during " + operation + ": " + context);
}

} // namespace nlm
