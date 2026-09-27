#pragma once

/**
 * NLM Exception Handling Utilities
 * 
 * Exception handling utilities for robust error handling with proper exception safety,
 * exception conversion, and recovery utilities for the NLM framework.
 */

#include <exception>
#include <stdexcept>
#include <functional>
#include <memory>
#include <type_traits>
#include <new>

namespace nlm {

/**
 * Exception safety guarantees for different operation types
 */
enum class ExceptionSafety {
    NO_THROW = 0,      /**< No throw guarantee */
    BASIC_THROW = 1,   /**< Basic exception safety (no resource leaks) */
    NO_PROPAGATE_THROW = 2,  /**< No throw during copy/assignment */
    STRONG_THROW = 3,  /**< Strong exception safety (state unchanged on exception) */
    NOEXCEPT = 4       /**< No exception guarantee (may crash on exception) */
};

/**
 * Exception wrapper for NLM-specific exceptions
 * 
 * Provides a wrapper around standard exceptions with NLM-specific error handling,
 * conversion utilities, and exception safety guarantees.
 */
class NLMExceptionWrapper {
public:
    /**
     * Construct from standard exception
     * 
     * @param e Standard exception to wrap
     */
    explicit NLMExceptionWrapper(const std::exception& e) noexcept {
        try {
            originalException_ = e.what();
        } catch (...) {
            originalException_ = "Unknown exception (failed to get message)";
        }
    }
    
    /**
     * Construct from exception pointer
     * 
     * @param e Exception pointer to wrap
     */
    explicit NLMExceptionWrapper(const std::exception* e) noexcept {
        if (e) {
            try {
                originalException_ = e->what();
            } catch (...) {
                originalException_ = "Unknown exception (failed to get message)";
            }
        } else {
            originalException_ = "Null exception pointer";
        }
    }
    
    /**
     * Get original exception message
     * 
     * @return const std::string& Original exception message
     */
    const std::string& getOriginalException() const noexcept { return originalException_; }
    
    /**
     * Convert to NLM error
     * 
     * @param errorCode Error code to use
     * @return NLMError Converted NLM error
     */
    NLMError toNLMError(ErrorCode errorCode = ErrorCode::INTERNAL_ERROR) const {
        return NLMError(errorCode, originalException_, ErrorSeverity::ERROR, "Converted from std::exception");
    }
    
    /**
     * Convert to NLM error with custom context
     * 
     * @param errorCode Error code to use
     * @param context Additional context
     * @return NLMError Converted NLM error
     */
    NLMError toNLMError(ErrorCode errorCode, const std::string& context) const {
        return NLMError(errorCode, originalException_, ErrorSeverity::ERROR, context);
    }

private:
    std::string originalException_;
};

/**
 * Exception safety guarantee checker
 */
class ExceptionSafetyChecker {
public:
    /**
     * Check if operation has strong exception safety guarantee
     * 
     * @tparam T Operation type/function
     * @param func Function to check
     * @return ExceptionSafety Exception safety guarantee
     */
    template<typename Func>
    static ExceptionSafety checkStrongGuarantee(Func&& func) {
        try {
            func();
            return ExceptionSafety::STRONG_THROW;
        } catch (...) {
            return ExceptionSafety::BASIC_THROW;
        }
    }
    
    /**
     * Check if operation is noexcept
     * 
     * @tparam T Operation type/function
     * @param func Function to check
     * @return ExceptionSafety Exception safety guarantee
     */
    template<typename Func>
    static ExceptionSafety checkNoexceptGuarantee(Func&& func) {
        try {
            func();
            return ExceptionSafety::NO_THROW;
        } catch (...) {
            return ExceptionSafety::NOEXCEPT;
        }
    }
    
    /**
     * Perform operation with exception safety
     * 
     * @tparam T Operation type/function
     * @tparam RecoveryFunc Recovery function type
     * @param func Function to perform
     * @param recovery Recovery function to call on exception
     * @return bool true if operation succeeded
     */
    template<typename Func, typename RecoveryFunc>
    static bool performWithSafety(Func&& func, RecoveryFunc&& recovery) {
        try {
            func();
            return true;
        } catch (...) {
            try {
                recovery();
                return false; // Operation failed but recovered
            } catch (...) {
                return false; // Both operation and recovery failed
            }
        }
    }
};

/**
 * Exception translation utilities
 */
class ExceptionTranslator {
public:
    /**
     * Translate standard exception to NLM error
     * 
     * @param e Standard exception to translate
     * @return NLMError Translated error
     */
    static NLMError translateToNLM(const std::exception& e) {
        NLMExceptionWrapper wrapper(e);
        return wrapper.toNLMError();
    }
    
    /**
     * Translate exception pointer to NLM error
     * 
     * @param e Exception pointer to translate
     * @return NLMError Translated error
     */
    static NLMError translateToNLM(const std::exception* e) {
        NLMExceptionWrapper wrapper(e);
        return wrapper.toNLMError();
    }
    
    /**
     * Translate any exception to NLM error
     * 
     * @param e Exception to translate (any type)
     * @return NLMError Translated error
     */
    template<typename T>
    static NLMError translateToNLM(const T& e) {
        static_assert(std::is_base_of<std::exception, T>::value, 
                     "T must be derived from std::exception");
        return translateToNLM(static_cast<const std::exception&>(e));
    }
    
    /**
     * Translate exception with context
     * 
     * @param e Standard exception to translate
     * @param context Additional context
     * @return NLMError Translated error
     */
    static NLMError translateToNLMWithContext(const std::exception& e, const std::string& context) {
        NLMExceptionWrapper wrapper(e);
        return wrapper.toNLMError(ErrorCode::INTERNAL_ERROR, context);
    }
    
    /**
     * Try-catch block with automatic translation
     * 
     * @tparam Func Function type
     * @tparam T Exception type
     * @param func Function to execute
     * @return Result of function execution
     * @throws NLMError If exception is thrown and translated
     */
    template<typename Func, typename T = std::exception>
    static auto executeSafely(Func&& func) -> decltype(func()) {
        try {
            return func();
        } catch (const T& e) {
            throw translateToNLM(e);
        } catch (const std::exception& e) {
            throw translateToNLM(e);
        }
    }
    
    /**
     * Try-catch block with exception translation and recovery
     * 
     * @tparam Func Function type
     * @tparam RecoveryFunc Recovery function type
     * @tparam T Exception type
     * @param func Function to execute
     * @param recovery Recovery function to call on exception
     * @return std::pair<bool, decltype(func())> Pair of success flag and result
     */
    template<typename Func, typename RecoveryFunc, typename T = std::exception>
    static std::pair<bool, decltype(func())> executeWithRecovery(Func&& func, RecoveryFunc&& recovery) {
        try {
            auto result = func();
            return std::make_pair(true, result);
        } catch (const T& e) {
            auto error = translateToNLM(e);
            ErrorReporter::logError(error);
            try {
                recovery();
                return std::make_pair(false, decltype(func())());
            } catch (...) {
                return std::make_pair(false, decltype(func())());
            }
        } catch (const std::exception& e) {
            auto error = translateToNLM(e);
            ErrorReporter::logError(error);
            try {
                recovery();
                return std::make_pair(false, decltype(func())());
            } catch (...) {
                return std::make_pair(false, decltype(func())());
            }
        }
    }
    
    /**
     * Exception chaining helper
     * 
     * @tparam OriginalFunc Original function type
     * @tparam FollowingFunc Following function type
     * @param original Original function
     * @param following Following function
     * @return Function that chains both
     */
    template<typename OriginalFunc, typename FollowingFunc>
    static auto chainExceptions(OriginalFunc&& original, FollowingFunc&& following) {
        return [original = std::forward<OriginalFunc>(original), 
                following = std::forward<FollowingFunc>(following)](auto&&... args) {
            try {
                auto result = original(std::forward<decltype(args)>(args)...);
                return following(result);
            } catch (const NLMError&) {
                throw;
            } catch (const std::exception& e) {
                throw translateToNLM(e);
            }
        };
    }
};

/**
 * Exception safety guard
 */
template<typename T>
class ExceptionSafetyGuard {
public:
    /**
     * Construct guard
     * 
     * @tparam Resource Resource type
     * @param resource Resource to protect
     * @param cleanup Cleanup function
     */
    template<typename Resource, typename CleanupFunc>
    ExceptionSafetyGuard(Resource& resource, CleanupFunc&& cleanup) 
        : resource_(&resource), cleanup_(std::forward<CleanupFunc>(cleanup)), active_(true) {}
    
    /**
     * Destructor - ensures cleanup if exception occurred
     */
    ~ExceptionSafetyGuard() {
        if (active_ && cleanup_) {
            cleanup_();
        }
    }
    
    /**
     * Release guard - called manually when operation completes successfully
     */
    void release() {
        active_ = false;
    }
    
    /**
     * Check if guard is active
     * 
     * @return true If guard is active
     */
    bool isActive() const { return active_; }

private:
    void* resource_;
    std::function<void()> cleanup_;
    bool active_;
};

/**
 * Smart exception handling with RAII
 */
class SmartExceptionHandler {
public:
    /**
     * Execute function with automatic exception handling
     * 
     * @tparam Func Function type
     * @param func Function to execute
     * @param errorHandler Error handler function
     * @return Result of function execution
     */
    template<typename Func, typename ErrorHandler>
    static auto executeWithHandler(Func&& func, ErrorHandler&& errorHandler) {
        try {
            return func();
        } catch (const NLMError& e) {
            errorHandler(e);
            throw; // Re-throw after handling
        } catch (const std::exception& e) {
            auto nlmError = ExceptionTranslator::translateToNLM(e);
            errorHandler(nlmError);
            throw nlmError;
        }
    }
    
    /**
     * Execute function and convert to NLM error
     * 
     * @tparam Func Function type
     * @param func Function to execute
     * @return Result or NLM error on failure
     */
    template<typename Func>
    static auto executeAndConvert(Func&& func) {
        try {
            return func();
        } catch (const NLMError& e) {
            throw;
        } catch (const std::exception& e) {
            throw ExceptionTranslator::translateToNLM(e);
        }
    }
    
    /**
     * Execute function with fallback on exception
     * 
     * @tparam Func Function type
     * @tparam FallbackFunc Fallback function type
     * @param func Function to execute
     * @param fallback Fallback function
     * @return Result of function or fallback
     */
    template<typename Func, typename FallbackFunc>
    static auto executeWithFallback(Func&& func, FallbackFunc&& fallback) {
        try {
            return func();
        } catch (...) {
            return fallback();
        }
    }
    
    /**
     * Execute function and capture exception
     * 
     * @tparam Func Function type
     * @tparam ExceptionFunc Exception capture function type
     * @param func Function to execute
     * @param exceptionFunc Function to call with captured exception
     * @return Result of function
     */
    template<typename Func, typename ExceptionFunc>
    static auto executeAndCaptureException(Func&& func, ExceptionFunc&& exceptionFunc) {
        try {
            return func();
        } catch (...) {
            exceptionFunc(std::current_exception());
            throw;
        }
    }
    
    /**
     * Execute function with timeout
     * 
     * @tparam Func Function type
     * @param func Function to execute
     * @param timeout Timeout in milliseconds
     * @return Result of function
     * @throws NLMError if timeout occurs
     */
    template<typename Func>
    static auto executeWithTimeout(Func&& func, std::chrono::milliseconds timeout) {
        std::future<decltype(func())> future = std::async(std::launch::async, func);
        auto status = future.wait_for(timeout);
        
        if (status == std::future_status::ready) {
            return future.get();
        } else {
            future.wait(); // Wait for thread to complete to avoid resource leak
            throw NLMError(ErrorCode::PERFORMANCE_TIMEOUT, 
                          "Operation timed out", 
                          ErrorSeverity::ERROR,
                          "Function execution exceeded timeout limit");
        }
    }
    
    /**
     * Execute function with retry logic
     * 
     * @tparam Func Function type
     * @param func Function to execute
     * @param maxRetries Maximum number of retries
     * @param delayBetweenRetries Delay between retries in milliseconds
     * @return Result of function
     * @throws NLMError if all retries fail
     */
    template<typename Func>
    static auto executeWithRetry(Func&& func, int maxRetries = 3, 
                                 std::chrono::milliseconds delayBetweenRetries = std::chrono::milliseconds(100)) {
        int attempt = 0;
        while (attempt <= maxRetries) {
            try {
                return func();
            } catch (...) {
                attempt++;
                if (attempt > maxRetries) {
                    throw;
                }
                std::this_thread::sleep_for(delayBetweenRetries);
            }
        }
        // Should never reach here
        throw NLMError(ErrorCode::INTERNAL_ERROR, 
                      "Retry logic failed unexpectedly", 
                      ErrorSeverity::ERROR);
    }
    
    /**
     * Execute function and record exceptions
     * 
     * @tparam Func Function type
     * @param func Function to execute
     * @param exceptionRecorder Exception recorder function
     * @return Result of function
     */
    template<typename Func, typename ExceptionRecorder>
    static auto executeAndRecordException(Func&& func, ExceptionRecorder&& exceptionRecorder) {
        try {
            return func();
        } catch (...) {
            exceptionRecorder(std::current_exception());
            throw;
        }
    }
};

/**
 * Exception utilities namespace
 */
namespace exception_utils {
    /**
     * Get current exception type name
     * 
     * @return std::string Current exception type name
     */
    std::string getCurrentExceptionType();
    
    /**
     * Is current exception of specific type
     * 
     * @tparam T Exception type to check
     * @return bool true if current exception is of type T
     */
    template<typename T>
    bool isCurrentException() {
        try {
            throw;
        } catch (const T&) {
            return true;
        } catch (...) {
            return false;
        }
    }
    
    /**
     * Re-throw current exception as different type
     * 
     * @tparam T Target exception type
     * @return T Re-thrown exception
     */
    template<typename T>
    T rethrowAs() {
        try {
            throw;
        } catch (const T& e) {
            return e;
        }
    }
    
    /**
     * Check if exception is critical
     * 
     * @param e Exception to check
     * @return bool true if exception is critical
     */
    bool isCriticalException(const std::exception& e);
    
    /**
     * Get exception severity
     * 
     * @param e Exception to check
     * @return ErrorSeverity Exception severity
     */
    ErrorSeverity getExceptionSeverity(const std::exception& e);
}

/**
 * Exception listener for cross-cutting exception handling
 */
class ExceptionListener {
public:
    /**
     * Virtual destructor
     */
    virtual ~ExceptionListener() = default;
    
    /**
     * Handle exception
     * 
     * @param e Exception to handle
     * @return bool true if exception was handled
     */
    virtual bool handleException(const std::exception& e) = 0;
    
    /**
     * Get listener priority
     * 
     * @return int Priority (higher = earlier processing)
     */
    virtual int getPriority() const { return 0; }
    
    /**
     * Check if listener is enabled
     * 
     * @return bool true if listener is enabled
     */
    virtual bool isEnabled() const { return true; }
};

/**
 * Exception listener registry
 */
class ExceptionListenerRegistry {
public:
    /**
     * Register exception listener
     * 
     * @param listener Exception listener to register
     */
    static void registerListener(std::shared_ptr<ExceptionListener> listener) {
        std::lock_guard<std::mutex> lock(listenersMutex_);
        listeners_.push_back(listener);
        std::sort(listeners_.begin(), listeners_.end(),
                 [](const auto& a, const auto& b) {
                     return a->getPriority() > b->getPriority();
                 });
    }
    
    /**
     * Unregister exception listener
     */
    static void unregisterListener(const std::shared_ptr<ExceptionListener>& listener) {
        std::lock_guard<std::mutex> lock(listenersMutex_);
        auto it = std::find(listeners_.begin(), listeners_.end(), listener);
        if (it != listeners_.end()) {
            listeners_.erase(it);
        }
    }
    
    /**
     * Process exception through listeners
     * 
     * @param e Exception to process
     * @return bool true if exception was handled by any listener
     */
    static bool processException(const std::exception& e) {
        std::lock_guard<std::mutex> lock(listenersMutex_);
        for (const auto& listener : listeners_) {
            if (listener->isEnabled()) {
                try {
                    if (listener->handleException(e)) {
                        return true;
                    }
                } catch (...) {
                    // Listener failed, continue to next
                    continue;
                }
            }
        }
        return false;
    }
    
    /**
     * Clear all listeners
     */
    static void clearListeners() {
        std::lock_guard<std::mutex> lock(listenersMutex_);
        listeners_.clear();
    }

private:
    static std::vector<std::shared_ptr<ExceptionListener>> listeners_;
    static std::mutex listenersMutex_;
};

} // namespace nlm