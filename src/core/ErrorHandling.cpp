// NLM (熙然) - Neural Learning Machine
// Phase 8: Enhanced Error Handling System Implementation
//
// This file implements the error handling system with:
// - Comprehensive exception hierarchy
// - Better error reporting and logging
// - Error classification and handling
// - User-friendly error messages
// - Integration with NLM components

#include "ErrorHandling.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <mutex>
#include <stacktrace>

namespace nlm {

// NLMException implementation
NLMException::NLMException(const std::string& message, ErrorCode code,
                         const std::string& component, int line,
                         const std::string& file, const std::string& function)
    : std::runtime_error(message), errorCode(code), component(component),
      line(line), file(file), function(function) {}

std::string NLMException::getFullError() const {
    std::ostringstream oss;
    oss << "=== NLM Error Report ===\n";
    oss << "Generated: " << getTimestamp(std::chrono::system_clock::now()) << "\n";
    oss << "\n";
    oss << "Error: " << what() << "\n";
    oss << "Error Code: " << static_cast<int>(errorCode) << "\n";
    
    if (!component.empty()) {
        oss << "Component: " << component << "\n";
    }
    
    if (!file.empty()) {
        oss << "File: " << file;
        if (line > 0) {
            oss << ":" << line;
        }
        oss << "\n";
    }
    
    if (!function.empty()) {
        oss << "Function: " << function << "\n";
    }
    
    if (!causes.empty()) {
        oss << "\nCauses:";
        for (size_t i = 0; i < causes.size(); ++i) {
            oss << "\n  " << i + 1 << ". " << causes[i].getFullError();
        }
    }
    
    return oss.str();
}

std::string NLMException::formatUserMessage() const {
    std::ostringstream oss;
    oss << "Error: " << what() << "\n";
    oss << "Component: " << component << "\n";
    return oss.str();
}

std::string NLMException::formatDebugMessage() const {
    std::ostringstream oss;
    oss << "=== DEBUG ERROR INFO ===\n";
    oss << "Error: " << what() << "\n";
    oss << "Error Code: " << static_cast<int>(errorCode) << "\n";
    oss << "Component: " << component << "\n";
    
    if (!file.empty()) {
        oss << "Location: " << file;
        if (line > 0) {
            oss << ":" << line;
        }
        oss << " (function: " << function << ")\n";
    }
    
    oss << "\nStack trace:\n";
    
    if (!causes.empty()) {
        oss << "\nChain of causes:";
        for (size_t i = 0; i < causes.size(); ++i) {
            oss << "\n  " << i + 1 << ". " << causes[i].what();
        }
    }
    
    return oss.str();
}

// ErrorReporter implementation
ErrorReporter::ErrorHandler ErrorReporter::errorHandler = nullptr;
std::vector<NLMException> ErrorReporter::collectedErrors;
std::mutex ErrorReporter::errorMutex;

void ErrorReporter::registerErrorHandler(ErrorHandler handler) {
    std::lock_guard<std::mutex> lock(errorMutex);
    errorHandler = handler;
}

void ErrorReporter::unregisterErrorHandler() {
    std::lock_guard<std::mutex> lock(errorMutex);
    errorHandler = nullptr;
}

void ErrorReporter::reportError(const NLMException& error) {
    std::lock_guard<std::mutex> lock(errorMutex);
    
    // Add to collected errors
    collectedErrors.push_back(error);
    
    // Call custom handler if registered
    if (errorHandler) {
        try {
            errorHandler(error);
        } catch (const std::exception& e) {
            std::cerr << "Error in error handler: " << e.what() << std::endl;
        }
    }
}

void ErrorReporter::reportError(const std::exception& error) {
    std::lock_guard<std::mutex> lock(errorMutex);
    
    // Convert to NLMException if possible
    try {
        auto nlmError = dynamic_cast<const NLMException&>(error);
        reportError(nlmError);
    } catch (...) {
        // Create a generic NLMException
        NLMException nlmError(error.what(), NLMException::ErrorCode::Unknown);
        reportError(nlmError);
    }
}

void ErrorReporter::reportError(const std::string& message) {
    reportError(NLMException(message));
}

void ErrorReporter::collectErrors(const std::vector<NLMException>& errors) {
    std::lock_guard<std::mutex> lock(errorMutex);
    collectedErrors.insert(collectedErrors.end(), errors.begin(), errors.end());
}

std::vector<NLMException> ErrorReporter::getCollectedErrors() {
    std::lock_guard<std::mutex> lock(errorMutex);
    return collectedErrors;
}

void ErrorReporter::clearCollectedErrors() {
    std::lock_guard<std::mutex> lock(errorMutex);
    collectedErrors.clear();
}

size_t ErrorReporter::getErrorCount(ErrorCode code) const {
    std::lock_guard<std::mutex> lock(errorMutex);
    if (code == ErrorCode::Unknown) {
        return collectedErrors.size();
    }
    
    size_t count = 0;
    for (const auto& error : collectedErrors) {
        if (error.getErrorCode() == code) {
            count++;
        }
    }
    return count;
}

double ErrorReporter::getErrorRate() const {
    std::lock_guard<std::mutex> lock(errorMutex);
    if (collectedErrors.empty()) {
        return 0.0;
    }
    
    // This is a simplified error rate calculation
    // In a real implementation, you might want to track successful operations
    return static_cast<double>(collectedErrors.size()) / 1000.0; // Placeholder
}

std::string ErrorReporter::formatErrorReport(const std::vector<NLMException>& errors) const {
    std::ostringstream oss;
    oss << "=== NLM Error Report ===\n";
    oss << "Total errors: " << errors.size() << "\n\n";
    
    for (size_t i = 0; i < errors.size(); ++i) {
        oss << "--- Error " << i + 1 << " ---\n";
        oss << errors[i].getFullError() << "\n";
        if (i < errors.size() - 1) {
            oss << "\n";
        }
    }
    
    return oss.str();
}

std::string ErrorReporter::formatErrorSummary(const std::vector<NLMException>& errors) const {
    std::ostringstream oss;
    oss << "Error Summary:\n";
    oss << "Total errors: " << errors.size() << "\n";
    
    std::map<NLMException::ErrorCode, size_t> errorCounts;
    for (const auto& error : errors) {
        errorCounts[error.getErrorCode()]++;
    }
    
    for (const auto& [code, count] : errorCounts) {
        oss << "  " << static_cast<int>(code) << ": " << count << " errors\n";
    }
    
    return oss.str();
}

std::vector<NLMException> ErrorReporter::filterErrors(ErrorCode code) const {
    std::lock_guard<std::mutex> lock(errorMutex);
    std::vector<NLMException> filtered;
    
    for (const auto& error : collectedErrors) {
        if (error.getErrorCode() == code) {
            filtered.push_back(error);
        }
    }
    
    return filtered;
}

std::vector<NLMException> ErrorReporter::filterErrors(const std::string& component) const {
    std::lock_guard<std::mutex> lock(errorMutex);
    std::vector<NLMException> filtered;
    
    for (const auto& error : collectedErrors) {
        if (error.getComponent() == component) {
            filtered.push_back(error);
        }
    }
    
    return filtered;
}

// ErrorContext implementation
std::map<std::string, std::string> ErrorContext::contexts;
std::mutex ErrorContext::contextMutex;
SimulationStep ErrorContext::currentStep = 0;
size_t ErrorContext::networkNeurons = 0;
size_t ErrorContext::networkSynapses = 0;

void ErrorContext::setContext(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(contextMutex);
    contexts[key] = value;
}

std::string ErrorContext::getContext(const std::string& key) {
    std::lock_guard<std::mutex> lock(contextMutex);
    auto it = contexts.find(key);
    if (it != contexts.end()) {
        return it->second;
    }
    return "";
}

void ErrorContext::clearContext(const std::string& key) {
    std::lock_guard<std::mutex> lock(contextMutex);
    contexts.erase(key);
}

void ErrorContext::clearAllContexts() {
    std::lock_guard<std::mutex> lock(contextMutex);
    contexts.clear();
}

std::map<std::string, std::string> ErrorContext::getAllContexts() {
    std::lock_guard<std::mutex> lock(contextMutex);
    return contexts;
}

void ErrorContext::reportErrorWithContext(const NLMException& error) {
    std::ostringstream oss;
    oss << "Error: " << error.what();
    
    if (currentStep > 0) {
        oss << " (Step: " << currentStep << ")";
    }
    
    if (networkNeurons > 0) {
        oss << " (Network: " << networkNeurons << " neurons)";
    }
    
    std::string errorMessage = oss.str();
    reportError(error);
}

// Global operator new and delete implementations
void* operator new(size_t size) throw(std::bad_alloc) {
    void* ptr = std::malloc(size);
    if (ptr == nullptr) {
        throw std::bad_alloc();
    }
    
    // Track the allocation
    getErrorReporter().reportError(nlm::NLMException("Memory allocation failed", 
                                                    nlm::NLMException::ErrorCode::MemoryAllocation));
    
    return ptr;
}

void* operator new[](size_t size) throw(std::bad_alloc) {
    void* ptr = std::malloc(size);
    if (ptr == nullptr) {
        throw std::bad_alloc();
    }
    
    // Track the allocation
    getErrorReporter().reportError(nlm::NLMException("Memory allocation failed", 
                                                    nlm::NLMException::ErrorCode::MemoryAllocation));
    
    return ptr;
}

void operator delete(void* ptr) throw() {
    if (ptr == nullptr) return;
    std::free(ptr);
}

void operator delete[](void* ptr) throw() {
    if (ptr == nullptr) return;
    std::free(ptr);
}

} // namespace nlm

// Helper functions
inline std::string getTimestamp(const std::chrono::system_clock::time_point& tp) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S UTC");
    return oss.str();
}

// Global error reporting instance
inline nlm::ErrorReporter& getErrorReporter() {
    static nlm::ErrorReporter reporter;
    return reporter;
}

// Error handling integration macros
#ifdef NLM_ERROR_HANDLING_ENABLED
    #define ERROR_CHECK(expr, errorCode) \
        do { \
            if (!(expr)) { \
                throw nlm::NLMException("Condition failed: " #expr, errorCode, "ErrorCheck", __LINE__, __FILE__, __func__); \
            } \
        } while(0)
    
    #define VALIDATE(condition, message, errorType) \
        if (!(condition)) { \
            throw nlm::ConfigurationError(message, nlm::ConfigurationError::Type::ValidationFailed, "", #condition); \
        }
    
    #define CHECK_RESULT(result, errorCode) \
        if (!(result)) { \
            throw nlm::NLMException("Operation failed: " #result, errorCode, "ErrorCheck", __LINE__, __FILE__, __func__); \
        }
#else
    #define ERROR_CHECK(expr, errorCode) \
        if (!(expr)) {}
    
    #define VALIDATE(condition, message, errorType) \
        if (!(condition)) {}
    
    #define CHECK_RESULT(result, errorCode) \
        if (!(result)) {}
#endif

// Error reporting convenience functions
inline void reportNLMError(const std::string& message, nlm::NLMException::ErrorCode code,
                          const std::string& component = "", int line = 0,
                          const std::string& file = "", const std::string& function = "") {
    nlm::NLMException error(message, code, component, line, file, function);
    getErrorReporter().reportError(error);
}

inline void reportMemoryError(size_t size, const std::string& hint) {
    nlm::MemoryAllocationError error("Memory allocation failed", size, hint);
    getErrorReporter().reportError(error);
}

inline void reportConfigError(const std::string& message, nlm::ConfigurationError::Type type,
                             const std::string& key = "", const std::string& expected = "") {
    nlm::ConfigurationError error(message, type, key, expected);
    getErrorReporter().reportError(error);
}

// Exception safety helper
class NLMExceptionSafety {
public:
    template<typename T, typename... Args>
    static T safeExecute(T (*func)(Args...), Args&&... args) {
        try {
            return func(std::forward<Args>(args)...);
        } catch (const nlm::NLMException& e) {
            std::cerr << "NLM Exception: " << e.what() << std::endl;
            throw;
        } catch (const std::exception& e) {
            std::cerr << "Standard Exception: " << e.what() << std::endl;
            throw nlm::NLMException(std::string("Wrapped exception: ") + e.what(),
                                    nlm::NLMException::ErrorCode::Unknown);
        }
    }
};
