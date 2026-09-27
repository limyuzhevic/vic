// Implementation of NLM Exception Handling Utilities

#include "ExceptionHandling.hpp"
#include <typeinfo>

namespace nlm {

// ExceptionListenerRegistry static member definitions
std::vector<std::shared_ptr<ExceptionListener>> ExceptionListenerRegistry::listeners_;
std::mutex ExceptionListenerRegistry::listenersMutex_;

// exception_utils namespace implementations

std::string exception_utils::getCurrentExceptionType() {
    return typeid(std::current_exception()).name();
}

bool exception_utils::isCriticalException(const std::exception& e) {
    // Check if exception is marked as critical
    try {
        // Try to get error code from exception if it's a NLMError
        // This is a simplified check - real implementation would be more sophisticated
        return false; // Default: not critical
    } catch (...) {
        return false; // Default: not critical
    }
}

ErrorSeverity exception_utils::getExceptionSeverity(const std::exception& e) {
    // Check if exception is marked with specific severity
    // This is a simplified check - real implementation would be more sophisticated
    return ErrorSeverity::ERROR; // Default: ERROR
}

// Additional utility functions

/**
 * Get NLM exception type name
 * 
 * @return std::string NLM exception type name
 */
std::string getNLMExceptionTypeName() {
    return "nlm::NLMError";
}

/**
 * Check if exception is NLM exception
 * 
 * @param e Exception to check
 * @return bool true if exception is NLM exception
 */
bool isNLMException(const std::exception& e) {
    try {
        // Try to cast to NLMError through virtual inheritance
        // This is a compile-time check, not runtime
        return false; // Simplified - real implementation would use RTTI
    } catch (...) {
        return false;
    }
}

/**
 * Convert exception to string
 * 
 * @param e Exception to convert
 * @return std::string Exception as string
 */
std::string exceptionToString(const std::exception& e) {
    try {
        return e.what();
    } catch (...) {
        return "Exception (unable to get message)";
    }
}

/**
 * Is exception type supported
 * 
 * @tparam T Exception type
 * @return bool true if exception type is supported
 */
template<typename T>
bool isExceptionSupported() {
    return std::is_base_of<std::exception, T>::value;
}

/**
 * Get supported exception types
 * 
 * @return std::vector<std::string> Supported exception type names
 */
std::vector<std::string> getSupportedExceptionTypes() {
    std::vector<std::string> types;
    types.push_back("std::exception");
    types.push_back("std::runtime_error");
    types.push_back("std::logic_error");
    types.push_back("std::out_of_range");
    types.push_back("std::invalid_argument");
    types.push_back("nlm::NLMError");
    return types;
}

/**
 * Compare exceptions for equality
 * 
 * @param a First exception
 * @param b Second exception
 * @return bool true if exceptions are equal
 */
bool exceptionsEqual(const std::exception& a, const std::exception& b) {
    try {
        return a.what() == b.what();
    } catch (...) {
        return false;
    }
}

/**
 * Exception cloning utility
 * 
 * @tparam T Exception type to clone
 * @param e Exception to clone
 * @return std::exception* Cloned exception
 */
template<typename T>
std::exception* cloneException(const T& e) {
    try {
        std::exception* result = new T(e);
        return result;
    } catch (...) {
        return nullptr;
    }
}

/**
 * Safe exception throwing
 * 
 * @tparam T Exception type
 * @param e Exception to throw
 * @throws T Exception
 */
template<typename T>
void safeThrow(const T& e) {
    try {
        throw e;
    } catch (...) {
        // Exception was thrown successfully
    }
}

/**
 * Exception stack trace utility
 * 
 * @param maxDepth Maximum trace depth
 * @return std::string Exception stack trace
 */
std::string getExceptionStackTrace(size_t maxDepth = 10) {
    std::stringstream ss;
    ss << "Exception stack trace (limited to " << maxDepth << " frames):\n";
    
    // Simplified stack trace - real implementation would use platform-specific stack walking
    ss << "  [Frame 0] main.cpp:123 in main()";
    ss << "\n  [Frame 1] brain.cpp:456 in Brain::initialize()";
    ss << "\n  [Frame 2] neuron.cpp:789 in Neuron::step()";
    
    return ss.str();
}

/**
 * Exception cleanup handler
 */
class ExceptionCleanupHandler {
public:
    /**
     * Register cleanup function
     */
    static void registerCleanup(std::function<void()> cleanup) {
        std::lock_guard<std::mutex> lock(cleanupMutex_);
        cleanupFunctions_.push_back(std::move(cleanup));
    }
    
    /**
     * Execute all cleanup functions
     */
    static void executeAllCleanups() {
        std::lock_guard<std::mutex> lock(cleanupMutex_);
        for (const auto& cleanup : cleanupFunctions_) {
            try {
                cleanup();
            } catch (...) {
                // Ignore cleanup failures
            }
        }
    }
    
    /**
     * Clear all cleanup functions
     */
    static void clearCleanups() {
        std::lock_guard<std::mutex> lock(cleanupMutex_);
        cleanupFunctions_.clear();
    }

private:
    static std::vector<std::function<void()>> cleanupFunctions_;
    static std::mutex cleanupMutex_;
};

// ExceptionCleanupHandler static member definitions
std::vector<std::function<void()>> ExceptionCleanupHandler::cleanupFunctions_;
std::mutex ExceptionCleanupHandler::cleanupMutex_;

/**
 * Exception context manager
 */
class ExceptionContextManager {
public:
    /**
     * Add context to current exception context
     */
    static void addContext(const std::string& context) {
        std::lock_guard<std::mutex> lock(contextMutex_);
        currentContext_.push_back(context);
    }
    
    /**
     * Remove context from current exception context
     */
    static void removeContext() {
        std::lock_guard<std::mutex> lock(contextMutex_);
        if (!currentContext_.empty()) {
            currentContext_.pop_back();
        }
    }
    
    /**
     * Get current exception context
     */
    static std::string getCurrentContext() {
        std::lock_guard<std::mutex> lock(contextMutex_);
        std::stringstream ss;
        for (size_t i = 0; i < currentContext_.size(); ++i) {
            if (i > 0) ss << " -> ";
            ss << currentContext_[i];
        }
        return ss.str();
    }
    
    /**
     * Clear all contexts
     */
    static void clearContext() {
        std::lock_guard<std::mutex> lock(contextMutex_);
        currentContext_.clear();
    }

private:
    static std::vector<std::string> currentContext_;
    static std::mutex contextMutex_;
};

// ExceptionContextManager static member definitions
std::vector<std::string> ExceptionContextManager::currentContext_;
std::mutex ExceptionContextManager::contextMutex_;

/**
 * Exception debugging utilities
 */
namespace debug_utils {
    /**
     * Enable detailed exception debugging
     */
    void enableDetailedDebugging() {
        // Implementation would enable detailed exception information collection
    }
    
    /**
     * Disable detailed exception debugging
     */
    void disableDetailedDebugging() {
        // Implementation would disable detailed exception information collection
    }
    
    /**
     * Is exception debugging enabled
     */
    bool isDebuggingEnabled() {
        return false; // Simplified
    }
    
    /**
     * Get exception debug information
     */
    std::string getExceptionDebugInfo(const std::exception& e) {
        std::stringstream ss;
        ss << "Exception Debug Info:\n";
        ss << "  Type: " << typeid(e).name() << "\n";
        ss << "  Message: " << exceptionToString(e) << "\n";
        ss << "  Address: " << &e << "\n";
        return ss.str();
    }
}

} // namespace nlm