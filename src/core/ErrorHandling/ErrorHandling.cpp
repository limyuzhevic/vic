#include "ErrorHandling.hpp"
#include <iostream>
#include <stdexcept>

namespace nlm {

// ErrorManager implementation
ErrorManager::ErrorManager() {
    errorHandler = std::make_unique<DefaultErrorHandler>();
}

void ErrorManager::reportError(const Error& error) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Try custom handlers first
    for (auto& handler : customHandlers) {
        if (handler->shouldHandle(error.getCategory())) {
            ErrorRecovery recovery = handler->handle(error);
            if (recovery != ErrorRecovery::None) {
                return;  // Handled by custom handler
            }
        }
    }
    
    // Fall back to default handler
    ErrorRecovery recovery = errorHandler->handle(error);
    
    switch (recovery) {
        case ErrorRecovery::Abort:
            throw;  // Re-throw the error
        case ErrorRecovery::Graceful:
            // Continue with safe defaults
            break;
        case ErrorRecovery::Immediate:
            // Attempt recovery
            break;
        case ErrorRecovery::Retry:
            // Retry logic would go here
            break;
        case ErrorRecovery::Shutdown:
            // Shutdown system
            std::exit(1);
        case ErrorRecovery::None:
        default:
            break;
    }
}

void ErrorManager::setContinueOnError(bool continueOnError) {
    std::lock_guard<std::mutex> lock(mutex);
    errorHandler->setContinueOnError(continueOnError);
}

void ErrorManager::addErrorHandler(std::unique_ptr<ErrorHandler> handler) {
    std::lock_guard<std::mutex> lock(mutex);
    customHandlers.push_back(std::move(handler));
}

void ErrorManager::addReporter(std::unique_ptr<ErrorReporter> reporter) {
    std::lock_guard<std::mutex> lock(mutex);
    errorHandler->addReporter(std::move(reporter));
}

// Template function implementation for safe access
// Note: These are specialized implementations for common types

// Template function for safe access
template<typename T>
T safeAccess(const T* ptr, const std::string& errorMsg, ErrorRecovery recovery) {
    if (!ptr) {
        NullPointerError error(errorMsg, ErrorContext(__FILE__, __func__, __LINE__));
        ErrorManager::instance().reportError(error);
        
        switch (recovery) {
            case ErrorRecovery::Abort: throw; break;
            case ErrorRecovery::Graceful: 
                return T(); break;
            case ErrorRecovery::Immediate: 
                // Attempt recovery
                return T(); break;
            case ErrorRecovery::Retry:
                // Retry logic
                return safeAccess(ptr, errorMsg, recovery); break;
            default: return T(); break;
        }
    }
    return *ptr;
}

// Explicit instantiations for commonly used types
// These will be defined in the implementation file
template float safeAccess<float>(const float*, const std::string&, ErrorRecovery);
template int safeAccess<int>(const int*, const std::string&, ErrorRecovery);
template double safeAccess<double>(const double*, const std::string&, ErrorRecovery);
template size_t safeAccess<size_t>(const size_t*, const std::string&, ErrorRecovery);
template bool safeAccess<bool>(const bool*, const std::string&, ErrorRecovery);

} // namespace nlm
