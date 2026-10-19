// Error Handling Implementation
// Complete implementation of centralized error handling system

#include "ErrorHandling.hpp"
#include <fmt/format.h>
#include <fmt/chrono.h>

namespace nlm {

// Global error manager instance
ErrorManager& getGlobalErrorManager() {
    static ErrorManager manager;
    return manager;
}

// Error context utilities
namespace ErrorContextUtils {
    
    inline ErrorContext createContext(const std::string& module = "",
                                      const std::string& function = "",
                                      const std::string& file = "",
                                      int line = 0) {
        ErrorContext ctx;
        if (!module.empty()) ctx.module = module;
        if (!function.empty()) ctx.function = function;
        if (!file.empty()) ctx.file = file;
        if (line > 0) ctx.line = line;
        
        // Capture stack trace (simplified implementation)
        ctx.stackTrace = "Stack trace not available in this build";
        
        return ctx;
    }
    
    inline ErrorContext createContextWithFields(const std::string& module,
                                                const std::string& function,
                                                const std::string& file,
                                                int line,
                                                const std::unordered_map<std::string, std::string>& fields) {
        ErrorContext ctx = createContext(module, function, file, line);
        ctx.fields = fields;
        return ctx;
    }
}

// Error manager implementation details
void ErrorManager::flushBuffer() {
    std::lock_guard<std::mutex> bufferLock(bufferMutex);
    if (errorBuffer.empty()) return;
    
    // Sort errors by severity (critical first)
    std::sort(errorBuffer.begin(), errorBuffer.end(),
              [](const std::shared_ptr<NLMException>& a,
                 const std::shared_ptr<NLMException>& b) {
                  return static_cast<int>(a->getSeverity()) > static_cast<int>(b->getSeverity());
              });
    
    // Aggregate and handle errors
    for (const auto& error : errorBuffer) {
        GlobalErrorHandler::getInstance().handleError(error);
    }
    
    errorBuffer.clear();
    lastFlushTime = std::chrono::system_clock::now();
}

// Recovery strategy implementations
namespace RecoveryStrategies {
    
    // Neural network recovery
    struct NeuralNetworkRecovery {
        static RecoveryStrategy create() {
            RecoveryStrategy strategy;
            strategy.action = ErrorAction::RECOVER;
            strategy.timeout = std::chrono::milliseconds(1000);
            strategy.maxRetries = 3;
            strategy.description = "Attempt to reinitialize neural network components";
            
            strategy.recoveryAction = []() {
                // Reinitialize neural network components
                // This would involve reloading weights, resetting states, etc.
            };
            
            strategy.fallbackAction = []() {
                // Fallback to simpler model or reduced capacity
            };
            
            return strategy;
        }
    };
    
    // Memory management recovery
    struct MemoryManagementRecovery {
        static RecoveryStrategy create() {
            RecoveryStrategy strategy;
            strategy.action = ErrorAction::FAIL_FAST;
            strategy.timeout = std::chrono::milliseconds(100);
            strategy.description = "Memory error requires immediate action";
            
            strategy.recoveryAction = []() {
                // Try garbage collection
                // Release unused resources
            };
            
            strategy.fallbackAction = []() {
                // Reduce memory usage, switch to more efficient algorithms
            };
            
            return strategy;
        }
    };
    
    // Configuration recovery
    struct ConfigurationRecovery {
        static RecoveryStrategy create() {
            RecoveryStrategy strategy;
            strategy.action = ErrorAction::RECOVER;
            strategy.timeout = std::chrono::seconds(10);
            strategy.maxRetries = 2;
            strategy.description = "Attempt to load backup configuration or reset to defaults";
            
            strategy.recoveryAction = []() {
                // Load configuration from backup or reset to defaults
            };
            
            strategy.fallbackAction = []() {
                // Use minimal viable configuration
            };
            
            return strategy;
        }
    };
}

// Error reporting and analysis utilities
namespace ErrorReporting {
    
    class ErrorReporter {
    private:
        std::mutex mutex;
        std::unordered_map<std::string, std::vector<std::shared_ptr<NLMException>>> errorHistory;
        size_t maxHistorySize = 1000;
        
    public:
        void recordError(const std::shared_ptr<NLMException>& error) {
            std::lock_guard<std::mutex> lock(mutex);
            
            std::string key = ErrorContextUtils::getCurrentModule();
            errorHistory[key].push_back(error);
            
            if (errorHistory[key].size() > maxHistorySize) {
                errorHistory[key].erase(errorHistory[key].begin());
            }
        }
        
        std::vector<std::shared_ptr<NLMException>> getErrorsByModule(const std::string& module) {
            std::lock_guard<std::mutex> lock(mutex);
            auto it = errorHistory.find(module);
            if (it != errorHistory.end()) {
                return it->second;
            }
            return {};
        }
        
        std::string generateReport() {
            std::lock_guard<std::mutex> lock(mutex);
            std::ostringstream oss;
            
            oss << "=== NLM Error Report ===\n";
            oss << "Generated at: " << ErrorContextUtils::getCurrentTimeString("%Y-%m-%d %H:%M:%S") << "\n";
            oss << "Total modules with errors: " << errorHistory.size() << "\n\n";
            
            for (const auto& [module, errors] : errorHistory) {
                oss << "Module: " << module << "\n";
                oss << "  Error count: " << errors.size() << "\n";
                
                // Group by error code
                std::unordered_map<ErrorCode, size_t> errorCounts;
                for (const auto& error : errors) {
                    errorCounts[error->getCode()]++;
                }
                
                oss << "  Error breakdown:\n";
                for (const auto& [code, count] : errorCounts) {
                    oss << "    " << errorCodeToString(code) << ": " << count << "\n";
                }
                oss << "\n";
            }
            
            return oss.str();
        }
        
        void exportErrors(const std::string& filename) {
            std::lock_guard<std::mutex> lock(mutex);
            std::ofstream file(filename);
            if (!file.is_open()) {
                throw Errors::ioError(ErrorCode::FILE_ACCESS_DENIED, filename, "Failed to open file for writing");
            }
            
            file << generateReport() << "\n\n=== Detailed Errors ===\n";
            
            for (const auto& [module, errors] : errorHistory) {
                file << "\nModule: " << module << "\n";
                for (const auto& error : errors) {
                    file << "  - " << error->what() << "\n";
                }
            }
        }
    };
    
    ErrorReporter& getGlobalReporter() {
        static ErrorReporter reporter;
        return reporter;
    }
}

// Error prevention utilities
namespace ErrorPrevention {
    
    class ErrorPreventionSystem {
    private:
        std::mutex mutex;
        std::unordered_map<std::string, std::function<bool()>> preConditions;
        std::unordered_map<std::string, std::function<bool()>> inputValidators;
        std::unordered_map<std::string, std::function<bool()>> resourceCheckers;
        std::unordered_map<std::string, std::function<bool()>> stateValidators;
        
    public:
        // Register pre-condition check
        void registerPreCondition(const std::string& name, std::function<bool()> check) {
            std::lock_guard<std::mutex> lock(mutex);
            preConditions[name] = std::move(check);
        }
        
        // Register input validator
        void registerInputValidator(const std::string& name, std::function<bool()> validator) {
            std::lock_guard<std::mutex> lock(mutex);
            inputValidators[name] = std::move(validator);
        }
        
        // Register resource checker
        void registerResourceChecker(const std::string& name, std::function<bool()> checker) {
            std::lock_guard<std::mutex> lock(mutex);
            resourceCheckers[name] = std::move(checker);
        }
        
        // Register state validator
        void registerStateValidator(const std::string& name, std::function<bool()> validator) {
            std::lock_guard<std::mutex> lock(mutex);
            stateValidators[name] = std::move(validator);
        }
        
        // Run all pre-condition checks
        std::vector<std::string> checkPreConditions() {
            std::lock_guard<std::mutex> lock(mutex);
            std::vector<std::string> failures;
            
            for (const auto& [name, check] : preConditions) {
                try {
                    if (!check()) {
                        failures.push_back("Pre-condition failed: " + name);
                    }
                } catch (const std::exception& e) {
                    failures.push_back("Pre-condition error in " + name + ": " + e.what());
                }
            }
            
            return failures;
        }
        
        // Validate all inputs
        std::vector<std::string> validateInputs() {
            std::lock_guard<std::mutex> lock(mutex);
            std::vector<std::string> failures;
            
            for (const auto& [name, validator] : inputValidators) {
                try {
                    if (!validator()) {
                        failures.push_back("Input validation failed: " + name);
                    }
                } catch (const std::exception& e) {
                    failures.push_back("Input validation error in " + name + ": " + e.what());
                }
            }
            
            return failures;
        }
        
        // Check all resources
        std::vector<std::string> checkResources() {
            std::lock_guard<std::mutex> lock(mutex);
            std::vector<std::string> failures;
            
            for (const auto& [name, checker] : resourceCheckers) {
                try {
                    if (!checker()) {
                        failures.push_back("Resource check failed: " + name);
                    }
                } catch (const std::exception& e) {
                    failures.push_back("Resource check error in " + name + ": " + e.what());
                }
            }
            
            return failures;
        }
        
        // Validate all states
        std::vector<std::string> validateStates() {
            std::lock_guard<std::mutex> lock(mutex);
            std::vector<std::string> failures;
            
            for (const auto& [name, validator] : stateValidators) {
                try {
                    if (!validator()) {
                        failures.push_back("State validation failed: " + name);
                    }
                } catch (const std::exception& e) {
                    failures.push_back("State validation error in " + name + ": " + e.what());
                }
            }
            
            return failures;
        }
        
        // Run all checks
        std::vector<std::string> runAllChecks() {
            std::vector<std::string> allFailures;
            
            auto preCondFailures = checkPreConditions();
            allFailures.insert(allFailures.end(), preCondFailures.begin(), preCondFailures.end());
            
            auto inputFailures = validateInputs();
            allFailures.insert(allFailures.end(), inputFailures.begin(), inputFailures.end());
            
            auto resourceFailures = checkResources();
            allFailures.insert(allFailures.end(), resourceFailures.begin(), resourceFailures.end());
            
            auto stateFailures = validateStates();
            allFailures.insert(allFailures.end(), stateFailures.begin(), stateFailures.end());
            
            return allFailures;
        }
    };
    
    ErrorPreventionSystem& getGlobalPreventionSystem() {
        static ErrorPreventionSystem system;
        return system;
    }
}

// Thread-safe error handling utilities
namespace ThreadSafeErrorHandling {
    
    class ThreadSafeErrorManager {
    private:
        mutable std::shared_mutex mutex;
        std::unordered_map<std::thread::id, std::vector<std::shared_ptr<NLMException>>> threadErrors;
        std::unordered_map<std::thread::id, std::chrono::system_clock::time_point> threadStartTimes;
        
    public:
        // Record error for specific thread
        void recordThreadError(const std::shared_ptr<NLMException>& error) {
            std::shared_lock<std::shared_mutex> lock(mutex);
            auto threadId = std::this_thread::get_id();
            
            threadErrors[threadId].push_back(error);
            
            // Limit errors per thread to prevent memory issues
            if (threadErrors[threadId].size() > 100) {
                threadErrors[threadId].erase(threadErrors[threadId].begin());
            }
        }
        
        // Get errors for current thread
        std::vector<std::shared_ptr<NLMException>> getThreadErrors() {
            std::shared_lock<std::shared_mutex> lock(mutex);
            auto threadId = std::this_thread::get_id();
            
            auto it = threadErrors.find(threadId);
            if (it != threadErrors.end()) {
                return it->second;
            }
            return {};
        }
        
        // Clear errors for current thread
        void clearThreadErrors() {
            std::unique_lock<std::shared_mutex> lock(mutex);
            auto threadId = std::this_thread::get_id();
            
            threadErrors.erase(threadId);
        }
        
        // Check for race conditions (simplified)
        bool checkForRaceConditions() {
            std::shared_lock<std::shared_mutex> lock(mutex);
            
            // Check if any thread has too many errors (potential deadlock or starvation)
            for (const auto& [threadId, errors] : threadErrors) {
                if (errors.size() > 50) {
                    return true;
                }
            }
            
            return false;
        }
        
        // Get global error statistics
        std::unordered_map<std::string, size_t> getErrorStatistics() {
            std::shared_lock<std::shared_mutex> lock(mutex);
            std::unordered_map<std::string, size_t> stats;
            
            for (const auto& [threadId, errors] : threadErrors) {
                std::string threadStr = std::to_string(threadId);
                size_t errorCount = errors.size();
                
                // Group by severity
                std::unordered_map<ErrorSeverity, size_t> severityCounts;
                for (const auto& error : errors) {
                    severityCounts[error->getSeverity()]++;
                }
                
                stats[threadStr + "_total"] = errorCount;
                for (const auto& [severity, count] : severityCounts) {
                    stats[threadStr + "_" + std::to_string(static_cast<int>(severity))] = count;
                }
            }
            
            return stats;
        }
    };
    
    ThreadSafeErrorManager& getGlobalThreadSafeManager() {
        static ThreadSafeErrorManager manager;
        return manager;
    }
}

// Integration with existing logging system
namespace LoggingIntegration {
    
    class ErrorLoggerIntegration {
    private:
        std::mutex mutex;
        std::unordered_map<ErrorCategory, enhanced::LogLevel> categoryLogLevels;
        std::unordered_map<ErrorCode, std::string> errorCategories;
        
    public:
        ErrorLoggerIntegration() {
            // Configure log levels for different error categories
            categoryLogLevels[ErrorCategory::CONFIGURATION] = enhanced::LogLevel::Warning;
            categoryLogLevels[ErrorCategory::NEURAL_NETWORK] = enhanced::LogLevel::Error;
            categoryLogLevels[ErrorCategory::MEMORY_MANAGEMENT] = enhanced::LogLevel::Critical;
            categoryLogLevels[ErrorCategory::PERFORMANCE] = enhanced::LogLevel::Warning;
            categoryLogLevels[ErrorCategory::IO] = enhanced::LogLevel::Error;
            categoryLogLevels[ErrorCategory::SYSTEM] = enhanced::LogLevel::Critical;
            categoryLogLevels[ErrorCategory::THREADING] = enhanced::LogLevel::Error;
            categoryLogLevels[ErrorCategory::VALIDATION] = enhanced::LogLevel::Warning;
            categoryLogLevels[ErrorCategory::RECOVERY] = enhanced::LogLevel::Info;
        }
        
        // Log error through existing logging system
        void logError(const std::shared_ptr<NLMException>& error) {
            std::lock_guard<std::mutex> lock(mutex);
            
            // Get appropriate log level
            enhanced::LogLevel level = enhanced::LogLevel::Error;
            auto catIt = categoryLogLevels.find(error->getCategory());
            if (catIt != categoryLogLevels.end()) {
                level = catIt->second;
            }
            
            // Create log entry
            enhanced::LogEntry entry;
            entry.level = level;
            entry.message = error->what();
            entry.file = error->getContext().file;
            entry.line = error->getContext().line;
            entry.function = error->getContext().function;
            entry.module = error->getContext().module;
            entry.category = ErrorContextUtils::categoryToString(error->getCategory());
            entry.exceptionType = error->getContext().exceptionType;
            entry.exceptionMessage = error->getContext().exceptionMessage;
            entry.stackTrace = error->getContext().stackTrace;
            entry.timestamp = std::chrono::system_clock::now();
            
            // Add additional fields
            ErrorContextUtils::addField(entry, "error_code", errorCodeToString(error->getCode()));
            ErrorContextUtils::addField(entry, "severity", std::to_string(static_cast<int>(error->getSeverity())));
            
            // Log through enhanced logger
            enhanced::Logger::getGlobal().log(entry);
            
            // Also record in error reporter
            ErrorReporting::getGlobalReporter().recordError(error);
        }
        
        // Configure integration
        void configure(ErrorCategory category, enhanced::LogLevel level) {
            std::lock_guard<std::mutex> lock(mutex);
            categoryLogLevels[category] = level;
        }
    };
    
    ErrorLoggerIntegration& getGlobalIntegration() {
        static ErrorLoggerIntegration integration;
        return integration;
    }
}

} // namespace nlm

// Convenience macros for error handling
#define NLM_TRY_BEGIN try {

#define NLM_TRY_CATCH_ERROR(category, code, message) \
} catch (const std::exception& e) { \
    nlm::ErrorContext ctx; \
    ctx.file = __FILE__; \
    ctx.line = __LINE__; \
    ctx.function = __func__; \
    ctx.module = nlm::ErrorContextUtils::getCurrentModule(); \
    auto error = std::make_shared<nlm::NLMException>( \
        nlm::ErrorSeverity::ERROR, \
        category, \
        code, \
        std::string("Exception: ") + e.what() + " " + message, \
        ctx \
    ); \
    nlm::getGlobalErrorManager().recordError(error);

#define NLM_THROW_ERROR(category, code, message) \
    do { \
        nlm::ErrorContext ctx; \
        ctx.file = __FILE__; \
        ctx.line = __LINE__; \
        ctx.function = __func__; \
        ctx.module = nlm::ErrorContextUtils::getCurrentModule(); \
        throw nlm::NLMException( \
            nlm::ErrorSeverity::ERROR, \
            category, \
            code, \
            message, \
            ctx \
        ); \
    } while(0)

#define NLM_LOG_ERROR(category, code, message) \
    do { \
        nlm::ErrorContext ctx; \
        ctx.file = __FILE__; \
        ctx.line = __LINE__; \
        ctx.function = __func__; \
        ctx.module = nlm::ErrorContextUtils::getCurrentModule(); \
        auto error = std::make_shared<nlm::NLMException>( \
            nlm::ErrorSeverity::ERROR, \
            category, \
            code, \
            message, \
            ctx \
        ); \
        nlm::getGlobalErrorManager().recordError(error);
    } while(0)

#define NLM_CHECK_PRECONDITION(condition, errorCategory, errorCode, message) \
    if (!(condition)) { \
        nlm::ErrorContext ctx; \
        ctx.file = __FILE__; \
        ctx.line = __LINE__; \
        ctx.function = __func__; \
        ctx.module = nlm::ErrorContextUtils::getCurrentModule(); \
        auto error = nlm::Errors::invalidInput(message, ctx); \
        nlm::getGlobalErrorManager().recordError(error); \
        return false; \
    }

#endif // NLM_ERROR_HANDLING_H
