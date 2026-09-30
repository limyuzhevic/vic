// This file includes all exception classes and enhanced logging macros for NLM

#ifndef NLM_EXCEPTION_INTEGRATION_H
#define NLM_EXCEPTION_INTEGRATION_H

#include "src/core/Exceptions.hpp"
#include "src/core/Logger/Logger.hpp"
#include <exception>
#include <functional>
#include <vector>

namespace nlm {

// Enhanced exception logging for NLM_LOG macros
void logNLMException(const std::exception& e, const std::string& context = "");

// Global exception handler for logging macros
void setExceptionLoggingEnabled(bool enabled);
bool isExceptionLoggingEnabled();

// Exception recovery mechanisms
class ExceptionRecoveryManager {
public:
    ExceptionRecoveryManager() = default;
    ~ExceptionRecoveryManager() = default;
    
    // Register exception handler for specific exception type
    template<typename ExceptionType>
    void registerHandler(std::function<void(const ExceptionType&)> handler) {
        handlers_[typeid(ExceptionType)] = [handler](const std::exception& ex) {
            if (auto specificEx = dynamic_cast<const ExceptionType*>(&ex)) {
                handler(*specificEx);
            }
        };
    }
    
    // Process exception with registered handlers
    void processException(const std::exception& e, const std::string& context = "") {
        auto it = handlers_.find(typeid(e));
        if (it != handlers_.end()) {
            it->second(e);
        } else {
            // Default handler
            logNLMException(e, context);
        }
    }
    
    // Graceful error handling for critical operations
    template<typename Operation>
    auto executeWithRecovery(Operation&& operation, 
                             const std::string& context = "",
                             const std::string& fallbackMessage = "Operation failed, using fallback") -> decltype(operation()) {
        try {
            return operation();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in " + context + ": " + e.what());
            
            // Try to provide a safe fallback
            try {
                NLM_LOG_WARNING("Attempting to use fallback for " + context);
                // Implement fallback logic here if needed
                return decltype(operation())();
            } catch (...) {
                NLM_LOG_CRITICAL("Fallback also failed for " + context);
                throw;
            }
        }
    }
    
    // Validate input before operations with exception handling
    template<typename... Validators>
    void validateInputs(Validators&&... validators) {
        (validators(), ...);
    }
    
private:
    std::unordered_map<std::type_index, std::function<void(const std::exception&)>> handlers_;
};

// Global exception recovery manager instance
inline ExceptionRecoveryManager& getRecoveryManager() {
    static ExceptionRecoveryManager manager;
    return manager;
}

// Enhanced logging macros with exception handling
#define NLM_LOG_INFO_WITH_EXCEPTION(msg, context) \
    do { \
        try { \
            NLM_LOG_INFO(msg); \
        } catch (const std::exception& e) { \
            nlm::logNLMException(e, "NLM_LOG_INFO " + context); \
        } \
    } while(0)

#define NLM_LOG_ERROR_WITH_EXCEPTION(msg, context) \
    do { \
        try { \
            NLM_LOG_ERROR(msg); \
        } catch (const std::exception& e) { \
            nlm::logNLMException(e, "NLM_LOG_ERROR " + context); \
        } \
    } while(0)

#define NLM_LOG_CRITICAL_WITH_EXCEPTION(msg, context) \
    do { \
        try { \
            NLM_LOG_CRITICAL(msg); \
        } catch (const std::exception& e) { \
            nlm::logNLMException(e, "NLM_LOG_CRITICAL " + context); \
        } \
    } while(0)

// Exception-safe operation macros
#define NLM_SAFE_OPERATION(operation, exception_class, error_code, context) \
    do { \
        try { \
            operation; \
        } catch (const std::exception& e) { \
            NLM_THROW_AND_LOG(exception_class, \
                            std::string("Operation failed in " + context + ": ") + e.what(), \
                            NLMException::Severity::ERROR, \
                            error_code, \
                            context); \
        } \
    } while(0)

// Input validation before operations
#define NLM_VALIDATE_BEFORE(operation, ...) \
    do { \
        nlm::getRecoveryManager().validateInputs(__VA_ARGS__); \
        try { \
            operation; \
        } catch (const std::exception& e) { \
            nlm::getRecoveryManager().processException(e, #operation); \
        } \
    } while(0)

// Error recovery for critical operations
#define NLM_RECOVERABLE_OPERATION(operation, fallback, context) \
    do { \
        auto result = nlm::getRecoveryManager().executeWithRecovery( \
            [&]() { return operation; }, \
            context \
        ); \
        if (!result) { \
            fallback; \
        } \
    } while(0)

} // namespace nlm

#endif // NLM_EXCEPTION_INTEGRATION_H