// Core includes for error handling
#pragma once

#include "ErrorHandling.hpp"
#include <memory>
#include <string>

namespace nlm {

// Base error context macros
#define ERROR_CONTEXT(func, component, op) \
    ErrorContext(__FILE__, func, __LINE__, component, op)

// Error handling macros
#define NLM_THROW_ERROR(severity, category, msg, component, op) \
    do { \
        ErrorManager::instance().reportError(Error(severity, category, msg, \
            ErrorContext(__FILE__, __func__, __LINE__, component, op))); \
    } while(0)

// Null pointer check macro
#define NLM_CHECK_NULL_PTR(ptr, msg, component, op) \
    if (!(ptr)) { \
        ErrorManager::instance().reportError(NullPointerError(msg, \
            ErrorContext(__FILE__, __func__, __LINE__, component, op))); \
    }

// Bounds checking macro
#define NLM_CHECK_BOUNDS(index, size, msg, component, op) \
    if ((index) >= (size)) { \
        ErrorManager::instance().reportError(MemoryError(msg, \
            ErrorContext(__FILE__, __func__, __LINE__, component, op))); \
    }

// Configuration validation macros
#define NLM_VALIDATE_PARAM(value, msg, component, op) \
    do { \
        if (!(value)) { \
            ErrorManager::instance().reportError(ConfigurationError(msg, \
                ErrorContext(__FILE__, __func__, __LINE__, component, op))); \
        } \
    } while(0)

// File I/O error macro
#define NLM_THROW_FILEIO_ERROR(msg, component, op) \
    do { \
        ErrorManager::instance().reportError(FileIOError(msg, \
            ErrorContext(__FILE__, __func__, __LINE__, component, op))); \
    } while(0)

// Exception-safe resource management macros
#define NLM_SAFE_RESOURCE_CLEANUP(resource) \
    do { \
        if ((resource) && (resource)->isValid()) { \
            (resource)->cleanup(); \
        } \
    } while(0)

// Template function for safe access
template<typename T>
T safeAccess(const T* ptr, const std::string& errorMsg, ErrorRecovery recovery = ErrorRecovery::Graceful) {
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

// Safe vector access
template<typename T>
T* safeVectorAccess(const std::vector<T*>& vec, size_t index, 
                   const std::string& errorMsg, ErrorRecovery recovery = ErrorRecovery::Graceful) {
    if (index >= vec.size()) {
        MemoryError error(errorMsg, ErrorContext(__FILE__, __func__, __LINE__));
        ErrorManager::instance().reportError(error);
        
        switch (recovery) {
            case ErrorRecovery::Abort: throw; break;
            case ErrorRecovery::Graceful: return nullptr; break;
            case ErrorRecovery::Immediate: 
                // Attempt recovery
                return nullptr; break;
            case ErrorRecovery::Retry:
                // Retry logic
                return safeVectorAccess(vec, index, errorMsg, recovery); break;
            default: return nullptr; break;
        }
    }
    
    T* ptr = vec[index];
    if (!ptr) {
        NullPointerError error(errorMsg, ErrorContext(__FILE__, __func__, __LINE__));
        ErrorManager::instance().reportError(error);
        
        switch (recovery) {
            case ErrorRecovery::Abort: throw; break;
            case ErrorRecovery::Graceful: return nullptr; break;
            case ErrorRecovery::Immediate: 
                // Attempt recovery
                return nullptr; break;
            case ErrorRecovery::Retry:
                // Retry logic
                return safeVectorAccess(vec, index, errorMsg, recovery); break;
            default: return nullptr; break;
        }
    }
    
    return ptr;
}

// Safe map access
template<typename K, typename V>
const V* safeMapAccess(const std::unordered_map<K, V*>& map, const K& key,
                      const std::string& errorMsg, ErrorRecovery recovery = ErrorRecovery::Graceful) {
    auto it = map.find(key);
    if (it == map.end()) {
        MemoryError error(errorMsg, ErrorContext(__FILE__, __func__, __LINE__));
        ErrorManager::instance().reportError(error);
        
        switch (recovery) {
            case ErrorRecovery::Abort: throw; break;
            case ErrorRecovery::Graceful: return nullptr; break;
            case ErrorRecovery::Immediate: 
                // Attempt recovery
                return nullptr; break;
            case ErrorRecovery::Retry:
                // Retry logic
                return safeMapAccess(map, key, errorMsg, recovery); break;
            default: return nullptr; break;
        }
    }
    
    V* ptr = it->second;
    if (!ptr) {
        NullPointerError error(errorMsg, ErrorContext(__FILE__, __func__, __LINE__));
        ErrorManager::instance().reportError(error);
        
        switch (recovery) {
            case ErrorRecovery::Abort: throw; break;
            case ErrorRecovery::Graceful: return nullptr; break;
            case ErrorRecovery::Immediate: 
                // Attempt recovery
                return nullptr; break;
            case ErrorRecovery::Retry:
                // Retry logic
                return safeMapAccess(map, key, errorMsg, recovery); break;
            default: return nullptr; break;
        }
    }
    
    return ptr;
}

// Configuration validation macros
#define NLM_VALIDATE_RANGE(value, min, max, errorMsg) \
    do { \
        if ((value) < (min) || (value) > (max)) { \
            ErrorManager::instance().reportError(ConfigurationError(errorMsg, \
                ErrorContext(__FILE__, __func__, __LINE__))); \
        } \
    } while(0)

#define NLM_VALIDATE_NON_NEGATIVE(value, errorMsg) \
    do { \
        if ((value) < 0) { \
            ErrorManager::instance().reportError(ConfigurationError(errorMsg, \
                ErrorContext(__FILE__, __func__, __LINE__))); \
        } \
    } while(0)

#define NLM_VALIDATE_POSITIVE(value, errorMsg) \
    do { \
        if ((value) <= 0) { \
            ErrorManager::instance().reportError(ConfigurationError(errorMsg, \
                ErrorContext(__FILE__, __func__, __LINE__))); \
        } \
    } while(0)

// File I/O validation macros
#define NLM_VALIDATE_FILEPATH(filepath) \
    do { \
        if ((filepath).empty()) { \
            ErrorManager::instance().reportError(FileIOError("Empty file path", \
                ErrorContext(__FILE__, __func__, __LINE__))); \
        } \
    } while(0)

// Exception safety macros
#define NLM_TRY_SAFE_BLOCK \
    try { \

#define NLM_CATCH_AND_HANDLE_ERROR(retryOnError) \
    } catch (const std::exception& e) { \
        ErrorManager::instance().reportError(CriticalError(e.what(), \
            ErrorContext(__FILE__, __func__, __LINE__))); \
        if (retryOnError) { \
            // Retry logic \
        } \
    }

} // namespace nlm
