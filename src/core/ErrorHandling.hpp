// Error Handling Macros for NLM
// Comprehensive macros for consistent error handling throughout the codebase

#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include "Errors.hpp"
#include "Logging.hpp"
#include <cassert>
#include <stdexcept>
#include <string>
#include <sstream>

// Source location macro for capturing where errors occur
#ifndef NLM_SOURCE_LOCATION
#define NLM_SOURCE_LOCATION __FILE__, __LINE__, __func__
#endif

// Create SourceLocation from macro arguments
inline SourceLocation makeSourceLocation(const std::string& file, int line, const std::string& function) {
    return SourceLocation(file, line, function);
}

// Macro to capture current source location
#define NLM_CURRENT_LOCATION makeSourceLocation(NLM_SOURCE_LOCATION)

// Error Handling Macros

// NLM_VALIDATE() - Input validation with custom error messages
#define NLM_VALIDATE(condition, error_type, message, operation, ...) \
    do { \
        if (!(condition)) { \
            Logger::getInstance().error(LogCategory::VALIDATION, \
                "Validation failed: " #condition " - " message, \
                ##__VA_ARGS__); \
            throw error_type(message, NLM_CURRENT_LOCATION, operation); \
        } \
    } while(0)

// NLM_THROW() - Exception throwing with context
#define NLM_THROW(error_type, message, ...) \
    do { \
        Logger::getInstance().error(error_type::getCategory(), \
            "Exception thrown: " message, ##__VA_ARGS__); \
        throw error_type(message, NLM_CURRENT_LOCATION); \
    } while(0)

// NLM_LOG_ERROR() - Standardized error logging
#define NLM_LOG_ERROR(category, message, ...) \
    Logger::getInstance().error(category, message, ##__VA_ARGS__)

// NLM_LOG_ERROR_WITH_CONTEXT() - Error logging with additional context
#define NLM_LOG_ERROR_WITH_CONTEXT(category, message, context_key, context_value, ...) \
    do { \
        Logger::getInstance().error(category, message, ##__VA_ARGS__); \
        /* Additional context handling could be added here */ \
    } while(0)

// NLM_HANDLE_ERROR() - Error handling with recovery
#define NLM_HANDLE_ERROR(error, handler, ...) \
    do { \
        try { \
            handler; \
        } catch (const NLMException& e) { \
            Logger::getInstance().logError(e); \
            throw; \
        } catch (...) { \
            Logger::getInstance().error(LogCategory::CORE, \
                "Unhandled exception in error handler", ##__VA_ARGS__); \
            throw; \
        } \
    } while(0)

// NLM_CHECK_POINTER() - Null pointer checking
#define NLM_CHECK_POINTER(ptr, error_type, message, operation) \
    do { \
        if ((ptr) == nullptr) { \
            Logger::getInstance().error(LogCategory::VALIDATION, \
                "Null pointer check failed: " #ptr " is null - " message); \
            throw error_type(message, NLM_CURRENT_LOCATION, operation); \
        } \
    } while(0)

// NLM_CHECK_BOUNDS() - Array bounds checking
#define NLM_CHECK_BOUNDS(index, size, error_type, message, operation) \
    do { \
        if ((index) < 0 || (index) >= (size)) { \
            std::stringstream ss; \
            ss << message << ": index " << (index) << " out of bounds [0, " << (size) << ")"; \
            Logger::getInstance().error(LogCategory::VALIDATION, ss.str()); \
            throw error_type(ss.str(), NLM_CURRENT_LOCATION, operation); \
        } \
    } while(0)

// NLM_CHECK_MEMORY() - Memory allocation checking
#define NLM_CHECK_MEMORY(ptr, error_type, message, operation) \
    do { \
        if ((ptr) == nullptr) { \
            Logger::getInstance().error(LogCategory::MEMORY, \
                "Memory allocation failed: " #ptr " is null - " message); \
            throw error_type(message, NLM_CURRENT_LOCATION, operation); \
        } \
    } while(0)

// NLM_CHECK_FILE() - File operation checking
#define NLM_CHECK_FILE(file, error_type, message, operation) \
    do { \
        if (!(file).is_open() || (file).fail()) { \
            Logger::getInstance().error(LogCategory::FILESYSTEM, \
                "File operation failed: " message); \
            throw error_type(message, NLM_CURRENT_LOCATION, operation); \
        } \
    } while(0)

// NLM_TRY_CATCH() - Standardized try-catch block
#define NLM_TRY_CATCH(try_block, catch_type, catch_handler) \
    do { \
        try { \
            try_block; \
        } catch (const catch_type& e) { \
            catch_handler; \
        } catch (const NLMException& e) { \
            Logger::getInstance().logError(e); \
            throw; \
        } catch (...) { \
            Logger::getInstance().error(LogCategory::CORE, \
                "Unhandled exception caught"); \
            throw; \
        } \
    } while(0)

// NLM_ERROR_GUARD() - Error guard for resource cleanup
#define NLM_ERROR_GUARD(operation, cleanup_action) \
    class ErrorGuard { \
    public: \
        ErrorGuard(const std::string& op, std::function<void()> cleanup) \
            : operation_(op), cleanup_(cleanup), success_(true) {} \
        ~ErrorGuard() { \
            if (success_ && cleanup_) { \
                cleanup_(); \
            } \
        } \
        void success() { success_ = true; } \
        void error() { success_ = false; } \
    private: \
        std::string operation_; \
        std::function<void()> cleanup_; \
        bool success_; \
    }; \
    ErrorGuard errorGuard_(operation, cleanup_action); \
    try { \
        \\
    } catch (...) { \
        errorGuard_.error(); \
        throw; \
    }

// NLM_PERFORMANCE_CHECK() - Performance measurement macro
#define NLM_PERFORMANCE_CHECK(operation, max_time_ms) \
    do { \
        auto start = std::chrono::high_resolution_clock::now(); \
        operation; \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start); \
        if (duration.count() > max_time_ms) { \
            Logger::getInstance().warning(LogCategory::PERFORMANCE, \
                "Performance warning: " #operation " took " + \
                std::to_string(duration.count()) + "ms (limit: " + \
                std::to_string(max_time_ms) + "ms)"); \
        } \
    } while(0)

// NLM_DEFER() - Defer execution until scope exit
#define NLM_DEFER(code) \
    class defer_##__LINE__ { \
    public: \
        ~defer_##__LINE__() { code; } \
    } defer_instance_##__LINE__

// NLM_STACK_TRACE() - Get stack trace for debugging (platform-specific)
#ifdef __linux__
#include <execinfo.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <sstream>

inline std::string getStackTrace(int max_depth = 50) {
    std::vector<void*> call_stack;
    call_stack.resize(max_depth);
    
    int frames = backtrace(call_stack.data(), static_cast<int>(call_stack.size()));
    char** symbols = backtrace_symbols(call_stack.data(), frames);
    
    std::stringstream ss;
    for (int i = 1; i < frames; ++i) { // Skip the first frame (this function)
        ss << "  #" << i << " " << symbols[i] << "\n";
    }
    
    free(symbols);
    return ss.str();
}
#elif defined(_WIN32)
// Windows stack trace implementation
#include <windows.h>
#include <dbghelp.h>
#include <sstream>
#include <vector>

inline std::string getStackTrace(int max_depth = 50) {
    std::vector<void*> call_stack;
    call_stack.resize(max_depth);
    
    USHORT frames = CaptureStackBackTrace(1, static_cast<USHORT>(call_stack.size()),
                                         call_stack.data(), nullptr);
    
    std::stringstream ss;
    for (int i = 0; i < frames; ++i) {
        char buffer[256];
        SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
        symbol->MaxNameLen = 255;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        
        if (SymFromAddr(GetCurrentProcess(), (DWORD64)(call_stack[i]), 0, symbol)) {
            ss << "  #" << i << " " << symbol->Name << "\n";
        }
        
        free(symbol);
    }
    
    return ss.str();
}
#else
// Generic stack trace (platform-specific implementation needed)
inline std::string getStackTrace(int max_depth = 50) {
    return "Stack trace not available on this platform";
}
#endif

// NLM_STACK_TRACE_ERROR() - Create error with stack trace
#define NLM_STACK_TRACE_ERROR(error_type, message, ...) \
    do { \
        Logger::getInstance().error(LogCategory::CORE, message, ##__VA_ARGS__); \
        error_type error_with_trace(message, NLM_CURRENT_LOCATION); \
        error_with_trace.getContext().stackTrace = getStackTrace(); \
        throw error_with_trace; \
    } while(0)

// Error recovery macros

// NLM_GRAACEFUL_DEGRADATION() - Graceful degradation on error
#define NLM_GRACEFUL_DEGRADATION(error_type, message, fallback_action, ...) \
    do { \
        try { \
            error_type e(message, NLM_CURRENT_LOCATION); \
            Logger::getInstance().logError(e); \
            fallback_action; \
        } catch (...) { \
            Logger::getInstance().error(LogCategory::CORE, \
                "Graceful degradation failed, using ultimate fallback", ##__VA_ARGS__); \
            /* Ultimate fallback could be placed here */ \
        } \
    } while(0)

// NLM_RETRY_ON_FAILURE() - Retry mechanism for transient errors
#define NLM_RETRY_ON_FAILURE(operation, max_retries, delay_ms, error_type, ...) \
    do { \
        int retry_count = 0; \
        bool success = false; \
        while (retry_count <= max_retries && !success) { \
            try { \
                operation; \
                success = true; \
            } catch (const error_type& e) { \
                Logger::getInstance().warning(LogCategory::CORE, \
                    "Operation failed (attempt " + std::to_string(retry_count + 1) + \
                    "/" + std::to_string(max_retries + 1) + "): " + e.getMessage()); \
                if (retry_count < max_retries) { \
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms)); \
                    retry_count++; \
                } else { \
                    Logger::getInstance().error(LogCategory::CORE, \
                        "Operation failed after " + std::to_string(max_retries + 1) + " attempts"); \
                    throw; \
                } \
            } \
        } \
    } while(0)

// NLM_ABORT_ON_ERROR() - Abort and cleanup on critical error
#define NLM_ABORT_ON_ERROR(error_type, message, cleanup_action, ...) \
    do { \
        Logger::getInstance().critical(LogCategory::CORE, \
            "Critical error, aborting: " message, ##__VA_ARGS__); \
        try { \
            cleanup_action; \
        } catch (...) { \
            Logger::getInstance().error(LogCategory::CORE, \
                "Error during cleanup"); \
        } \
        throw error_type(message, NLM_CURRENT_LOCATION); \
    } while(0)

// Memory management macros
#define NLM_NEW(type, ...) new(std::nothrow) type(__VA_ARGS__); \
    if (!(result)) { \
        Logger::getInstance().error(LogCategory::MEMORY, \
            "Memory allocation failed for type " #type); \
        throw MemoryError("Memory allocation failed", NLM_CURRENT_LOCATION); \
    }

#define NLM_DELETE(ptr) \
    do { \
        if ((ptr) != nullptr) { \
            delete (ptr); \
            (ptr) = nullptr; \
        } \
    } while(0)

#define NLM_DELETE_ARRAY(ptr) \
    do { \
        if ((ptr) != nullptr) { \
            delete[] (ptr); \
            (ptr) = nullptr; \
        } \
    } while(0)

#endif // NLM_ERROR_HANDLING_HPP