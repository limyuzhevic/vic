#include "Error.hpp"
#include "../core/Logger/Logger.hpp"
#include <cstdlib>
#include <sstream>
#include <unordered_map>
#include <chrono>
#include <thread>

namespace nlm {

// Exception translator implementation
void translateNLMException(const NLMError& e) {
    std::lock_guard<std::mutex> lock(ThreadSafeErrorHandler::getErrorMutex());
    logError(e);
}

// Default error reporter implementation
class DefaultErrorReporter : public ErrorReporter {
public:
    void reportError(const NLMError& error) override {
        logError(error);
    }

    void reportWarning(const std::string& message, const std::string& context) override {
        std::ostringstream oss;
        oss << "[WARNING] " << message;
        if (!context.empty()) oss << " (Context: " << context << ")";
        NLM_LOG_WARNING(oss.str());
    }
};

namespace {
    std::unique_ptr<ErrorReporter> globalErrorReporter;
    std::mutex reporterMutex;
    std::mutex threadSafetyMutex;
    bool threadSafetyEnabled = true;
    std::unordered_map<std::string, bool> criticalOperations;
    double defaultRetryDelay = 0.1;
}

// Global error reporter functions
ErrorReporter& getErrorReporter() {
    std::lock_guard<std::mutex> lock(reporterMutex);
    if (!globalErrorReporter) {
        globalErrorReporter = std::make_unique<DefaultErrorReporter>();
    }
    return *globalErrorReporter;
}

void setErrorReporter(std::unique_ptr<ErrorReporter> reporter) {
    std::lock_guard<std::mutex> lock(reporterMutex);
    globalErrorReporter = std::move(reporter);
}

// Thread-safe error handler implementation
void ThreadSafeErrorHandler::setThreadSafetyEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(threadSafetyMutex);
    threadSafetyEnabled = enabled;
}

bool ThreadSafeErrorHandler::isThreadSafetyEnabled() {
    std::lock_guard<std::mutex> lock(threadSafetyMutex);
    return threadSafetyEnabled;
}

void ThreadSafeErrorHandler::lockForErrorHandling() {
    if (isThreadSafetyEnabled()) {
        threadSafetyMutex.lock();
    }
}

void ThreadSafeErrorHandler::unlockFromErrorHandling() {
    if (isThreadSafetyEnabled()) {
        threadSafetyMutex.unlock();
    }
}

std::mutex& ThreadSafeErrorHandler::getErrorMutex() {
    static std::mutex errorMutex;
    return errorMutex;
}

// Memory allocation wrappers
void* safe_malloc(size_t size, const std::string& context) {
    if (size == 0) {
        throw MemoryError("Invalid memory allocation size", context);
    }
    void* ptr = std::malloc(size);
    if (!ptr) {
        throw MemoryError("Memory allocation failed", context);
    }
    return ptr;
}

void safe_free(void* ptr, const std::string& context) {
    if (ptr) {
        std::free(ptr);
    }
}

// Input validation implementations
bool InputValidator::isPositive(const std::string& key, float value) {
    if (std::isnan(value) || std::isinf(value)) {
        throw ValidationError("Invalid value: must be finite", key + " isPositive");
    }
    if (value < 0.0f) {
        throw ValidationError("Value must be positive", key + " value: " + std::to_string(value));
    }
    return true;
}

bool InputValidator::isWithinRange(const std::string& key, float value, float min, float max) {
    if (std::isnan(value) || std::isinf(value)) {
        throw ValidationError("Invalid value: must be finite", key + " isWithinRange");
    }
    if (value < min || value > max) {
        throw ValidationError("Value out of range", 
                            key + " value: " + std::to_string(value) + 
                            " range: [" + std::to_string(min) + ", " + std::to_string(max) + "]");
    }
    return true;
}

bool InputValidator::isNonEmpty(const std::string& key, const std::string& value) {
    if (value.empty()) {
        throw ValidationError("Value must not be empty", key + " isNonEmpty");
    }
    return true;
}

bool InputValidator::isValidIndex(const std::string& key, size_t index, size_t maxSize) {
    if (index >= maxSize) {
        throw ValidationError("Index out of bounds", 
                            key + " index: " + std::to_string(index) + 
                            " max: " + std::to_string(maxSize));
    }
    return true;
}

bool InputValidator::isFinite(const std::string& key, double value) {
    if (std::isnan(value) || std::isinf(value)) {
        throw ValidationError("Value must be finite", key + " value: " + std::to_string(value));
    }
    return true;
}

bool InputValidator::matchesPattern(const std::string& key, const std::string& value, const std::string& pattern) {
    // Basic pattern matching - can be enhanced with regex
    if (pattern.empty()) {
        return true; // No pattern to match
    }
    
    if (pattern == "positive_number") {
        try {
            double val = std::stod(value);
            return val > 0.0;
        } catch (...) {
            return false;
        }
    } else if (pattern == "non_empty_string") {
        return !value.empty();
    } else if (pattern == "float_range_0_1") {
        try {
            double val = std::stod(value);
            return val >= 0.0 && val <= 1.0;
        } catch (...) {
            return false;
        }
    }
    
    return true; // Unknown pattern - assume valid
}

// Error recovery implementation
bool ErrorRecovery::attemptRecovery(const std::string& operation, float maxRetries) {
    if (!isOperationCritical(operation)) {
        return true; // Non-critical operations can be skipped
    }
    
    size_t attempts = static_cast<size_t>(maxRetries);
    for (size_t i = 0; i < attempts; ++i) {
        try {
            // Try the operation - caller should implement specific recovery logic
            // This is a template - specific implementations should override this
            return true;
        } catch (const std::exception& e) {
            if (i == attempts - 1) {
                throw; // Last attempt failed, rethrow
            }
            std::this_thread::sleep_for(std::chrono::duration<double>(defaultRetryDelay * (i + 1)));
        }
    }
    return false;
}

void ErrorRecovery::setRetryDelay(double seconds) {
    defaultRetryDelay = seconds;
}

bool ErrorRecovery::isOperationCritical(const std::string& operation) {
    std::lock_guard<std::mutex> lock(threadSafetyMutex);
    return criticalOperations.find(operation) != criticalOperations.end();
}

// Error logging implementation
void logError(const NLMError& error) {
    std::ostringstream oss;
    oss << "[ERROR " << error.getCode() << "] " << error.getMessage();
    if (!error.getContext().empty()) {
        oss << " (Context: " << error.getContext() << ")";
    }
    if (!error.getSourceFile().empty()) {
        oss << " at " << error.getSourceFile();
        if (error.getSourceLine() > 0) {
            oss << ":" << error.getSourceLine();
        }
    }
    oss << " [" << error.getFormattedTimestamp() << "]";
    
    NLM_LOG_ERROR(oss.str());
}

void logWarning(const std::string& message, const std::string& context) {
    std::ostringstream oss;
    oss << "[WARNING] " << message;
    if (!context.empty()) {
        oss << " (Context: " << context << ")";
    }
    NLM_LOG_WARNING(oss.str());
}

} // namespace nlm