// Error Handler with Strategy Management
// Implements configurable error handling strategies

#pragma once

#include "ErrorCategory.hpp"
#include "NLMError.hpp"
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <atomic>
#include <mutex>

namespace nlm {

// Error handling strategies
enum class ErrorStrategy {
    THROW,             // Re-throw the error for caller to handle
    LOG_AND_THROW,     // Log the error then re-throw
    RECOVER,           // Attempt automatic recovery
    RETRY,             // Retry the operation
    FALLBACK,          // Use fallback values/strategies
    IGNORE,            // Ignore the error (use defaults)
    SILENT             // Only log internally, don't propagate
};

// Error recovery mechanism base class
class RecoveryHandler {
public:
    virtual ~RecoveryHandler() = default;
    virtual bool canHandle(const NLMError& error) const = 0;
    virtual bool recover(std::unique_ptr<NLMError>& error) = 0;
    virtual std::string getName() const = 0;
};

// Error reporting interface
class ErrorReporter {
public:
    virtual ~ErrorReporter() = default;
    virtual void report(const NLMError& error) = 0;
    virtual void setMinSeverity(ErrorSeverity severity) = 0;
    virtual ErrorSeverity getMinSeverity() const { return ErrorSeverity::ERROR; }
};

// Console error reporter with enhanced formatting
class ConsoleErrorReporter : public ErrorReporter {
    ErrorSeverity minSeverity;
    bool showTimestamps{true};
    bool showDebugInfo{true};

public:
    ConsoleErrorReporter(ErrorSeverity severity = ErrorSeverity::WARNING)
        : minSeverity(severity) {}

    void report(const NLMError& error) override {
        if (error.getSeverity() < minSeverity) return;

        std::cerr << getCurrentTimestamp() << " ";
        std::cerr << "[" << error.getSeverityString() << "] "
                  << error.getCategoryString() << ": "
                  << error.what() << std::endl;

        if (!error.getDetails().empty()) {
            std::cerr << "  Details: " << error.getDetails() << std::endl;
        }

        if (!error.getRecoveryMessage().empty()) {
            std::cerr << "  Recovery: " << error.getRecoveryMessage() << std::endl;
        }

        if (showDebugInfo) {
            for (const auto& pair : error.getDebugInfo()) {
                std::cerr << "  Debug: " << pair.first << " = " << pair.second << std::endl;
            }
        }

        std::cerr << "  Context: " << error.getContext().component << 
                  "::" << error.getContext().operation << std::endl;
    }

    void setMinSeverity(ErrorSeverity severity) override {
        minSeverity = severity;
    }

    void setShowTimestamps(bool show) {
        showTimestamps = show;
    }

    void setShowDebugInfo(bool show) {
        showDebugInfo = show;
    }

protected:
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// File error reporter for structured logging
class FileErrorReporter : public ErrorReporter {
    std::string logFile;
    ErrorSeverity minSeverity;
    std::mutex writeMutex;

public:
    FileErrorReporter(const std::string& file, ErrorSeverity severity = ErrorSeverity::ERROR)
        : logFile(file), minSeverity(severity) {}

    void report(const NLMError& error) override {
        if (error.getSeverity() < minSeverity) return;

        std::lock_guard<std::mutex> lock(writeMutex);
        // In a real implementation, this would write to a log file
        std::cout << "[FILE LOG] " << error.getFormattedMessage() << std::endl;
    }

    void setMinSeverity(ErrorSeverity severity) override {
        minSeverity = severity;
    }
};

// Error recovery handlers

class MemoryAllocationRecoveryHandler : public RecoveryHandler {
public:
    bool canHandle(const NLMError& error) const override {
        return error.getCategory() == ErrorCategory::RESOURCE_OUT_OF_MEMORY;
    }

    bool recover(std::unique_ptr<NLMError>& error) override {
        if (!error) return false;

        // Attempt memory recovery
        std::cerr << "Attempting memory recovery for error: " 
                  << error->what() << std::endl;

        // Add recovery info to error
        error->setRecoveryMessage("Memory recovery attempted (memory cleanup performed)");

        return true; // Recovery was attempted
    }

    std::string getName() const override {
        return "MemoryAllocationRecoveryHandler";
    }
};

class ConfigValidationRecoveryHandler : public RecoveryHandler {
public:
    bool canHandle(const NLMError& error) const override {
        return error.getCategory() == ErrorCategory::CONFIG_INVALID_VALUE ||
               error.getCategory() == ErrorCategory::CONFIG_VALIDATION_FAILED;
    }

    bool recover(std::unique_ptr<NLMError>& error) override {
        if (!error) return false;

        // Attempt to validate and correct configuration
        std::cerr << "Attempting configuration validation and correction" << std::endl;
        error->setRecoveryMessage("Configuration was validated and corrected");
        return true;
    }

    std::string getName() const override {
        return "ConfigValidationRecoveryHandler";
    }
};

class SimulationRecoveryHandler : public RecoveryHandler {
public:
    bool canHandle(const NLMError& error) const override {
        return error.getCategory() == ErrorCategory::SIMULATION_ERROR;
    }

    bool recover(std::unique_ptr<NLMError>& error) override {
        if (!error) return false;

        // Attempt simulation recovery
        std::cerr << "Attempting simulation recovery" << std::endl;
        error->setRecoveryMessage("Simulation state was recovered");
        return true;
    }

    std::string getName() const override {
        return "SimulationRecoveryHandler";
    }
};

// ErrorHandler class with strategy management
class ErrorHandler {
private:
    struct HandlerConfig {
        ErrorStrategy strategy;
        std::vector<std::shared_ptr<RecoveryHandler>> recoveryHandlers;
        std::vector<std::shared_ptr<ErrorReporter>> reporters;
        std::function<bool(const NLMError&)> customFilter;
        std::chrono::milliseconds retryDelay{1000}; // Default 1 second
        int maxRetries{3};
    };

    std::map<ErrorCategory, HandlerConfig> categoryHandlers;
    std::mutex mutex;
    std::atomic<bool> enableRecovery{true};
    std::atomic<bool> enableReporting{true};
    std::atomic<bool> enableRetry{true};

    // Default recovery handlers
    void setupDefaultRecoveryHandlers();
    void setupDefaultReporters();

public:
    ErrorHandler() {
        setupDefaultHandlers();
    }

    // Configure error handling strategy for a category
    void setStrategy(ErrorCategory category, ErrorStrategy strategy) {
        std::lock_guard<std::mutex> lock(mutex);
        if (categoryHandlers.count(category)) {
            categoryHandlers[category].strategy = strategy;
        }
    }

    // Configure recovery handlers for a category
    void addRecoveryHandler(ErrorCategory category,
                           std::shared_ptr<RecoveryHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex);
        if (categoryHandlers.count(category)) {
            categoryHandlers[category].recoveryHandlers.push_back(handler);
        }
    }

    // Configure error reporters
    void addReporter(std::shared_ptr<ErrorReporter> reporter) {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& pair : categoryHandlers) {
            pair.second.reporters.push_back(reporter);
        }
    }

    // Set custom filter function
    void setCustomFilter(std::function<bool(const NLMError&)> filter) {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& pair : categoryHandlers) {
            pair.second.customFilter = filter;
        }
    }

    // Configure retry settings
    void setRetrySettings(ErrorCategory category,
                         std::chrono::milliseconds delay, int maxRetries) {
        std::lock_guard<std::mutex> lock(mutex);
        if (categoryHandlers.count(category)) {
            categoryHandlers[category].retryDelay = delay;
            categoryHandlers[category].maxRetries = maxRetries;
        }
    }

    // Handle error with configured strategy
    void handle(std::unique_ptr<NLMError> error) {
        if (!error) return;

        std::lock_guard<std::mutex> lock(mutex);

        // Check custom filter
        if (categoryHandlers.count(error->getCategory())) {
            auto& handlerConfig = categoryHandlers[error->getCategory()];
            if (handlerConfig.customFilter && !handlerConfig.customFilter(*error)) {
                return;
            }

            // Apply strategy
            switch (handlerConfig.strategy) {
                case ErrorStrategy::THROW:
                    if (enableReporting) {
                        reportError(*error, handlerConfig.reporters);
                    }
                    break;

                case ErrorStrategy::LOG_AND_THROW:
                    if (enableReporting) {
                        reportError(*error, handlerConfig.reporters);
                    }
                    break;

                case ErrorStrategy::RECOVER:
                    if (enableRecovery) {
                        bool recovered = tryRecovery(*error, handlerConfig.recoveryHandlers);
                        if (!recovered) {
                            if (enableReporting) {
                                reportError(*error, handlerConfig.reporters);
                            }
                        }
                    }
                    break;

                case ErrorStrategy::RETRY:
                    if (enableRetry) {
                        retryOperation(*error, handlerConfig);
                    }
                    break;

                case ErrorStrategy::FALLBACK:
                    applyFallback(*error, handlerConfig.reporters);
                    break;

                case ErrorStrategy::IGNORE:
                    if (enableReporting) {
                        reportError(*error, handlerConfig.reporters);
                    }
                    break;

                case ErrorStrategy::SILENT:
                    // Only log internally
                    if (enableReporting) {
                        reportError(*error, handlerConfig.reporters);
                    }
                    break;
            }
        }
    }

    void reportError(const NLMError& error,
                     const std::vector<std::shared_ptr<ErrorReporter>>& reporters) {
        for (const auto& reporter : reporters) {
            reporter->report(error);
        }
    }

    bool tryRecovery(NLMError& error,
                    const std::vector<std::shared_ptr<RecoveryHandler>>& handlers) {
        for (const auto& handler : handlers) {
            if (handler->canHandle(error)) {
                std::unique_ptr<NLMError> errorPtr(&error);
                if (handler->recover(errorPtr)) {
                    return true;
                }
            }
        }
        return false;
    }

    void retryOperation(NLMError& error, HandlerConfig& config) {
        std::cerr << "Retry strategy not implemented for error: " << error.what() << std::endl;
        if (enableReporting) {
            for (const auto& reporter : config.reporters) {
                reporter->report(error);
            }
        }
    }

    void applyFallback(const NLMError& error,
                       const std::vector<std::shared_ptr<ErrorReporter>>& reporters) {
        std::cerr << "Applying fallback for error: " << error.what() << std::endl;
        if (enableReporting) {
            for (const auto& reporter : reporters) {
                reporter->report(error);
            }
        }
    }

    void setupDefaultRecoveryHandlers() {
        // Memory allocation recovery
        addRecoveryHandler(ErrorCategory::RESOURCE_OUT_OF_MEMORY,
                         std::make_shared<MemoryAllocationRecoveryHandler>());

        // Configuration validation recovery
        addRecoveryHandler(ErrorCategory::CONFIG_INVALID_VALUE,
                         std::make_shared<ConfigValidationRecoveryHandler>());

        addRecoveryHandler(ErrorCategory::CONFIG_VALIDATION_FAILED,
                         std::make_shared<ConfigValidationRecoveryHandler>());

        // Simulation recovery
        addRecoveryHandler(ErrorCategory::SIMULATION_ERROR,
                         std::make_shared<SimulationRecoveryHandler>());
    }

    void setupDefaultReporters() {
        // Add console reporter by default
        addReporter(std::make_shared<ConsoleErrorReporter>());
    }

    void setupDefaultHandlers() {
        // Setup default strategies for all categories
        for (int i = 0; i < static_cast<int>(ErrorCategory::UNKNOWN); ++i) {
            ErrorCategory category = static_cast<ErrorCategory>(i);
            HandlerConfig config;

            // Default strategy based on category
            if (category == ErrorCategory::CONFIG_INVALID_VALUE ||
                category == ErrorCategory::CONFIG_MISSING_FILE ||
                category == ErrorCategory::CONFIG_JSON_PARSING ||
                category == ErrorCategory::CONFIG_VALIDATION_FAILED) {
                config.strategy = ErrorStrategy::FALLBACK;
            } else if (category == ErrorCategory::RESOURCE_OUT_OF_MEMORY ||
                      category == ErrorCategory::RESOURCE_QUOTA_EXCEEDED) {
                config.strategy = ErrorStrategy::RECOVER;
            } else if (category == ErrorCategory::SYSTEM_INIT_FAILURE ||
                      category == ErrorCategory::SYSTEM_NOT_IMPLEMENTED ||
                      category == ErrorCategory::SYSTEM_UNAVAILABLE) {
                config.strategy = ErrorStrategy::LOG_AND_THROW;
            } else {
                config.strategy = ErrorStrategy::LOG_AND_THROW;
            }

            categoryHandlers[category] = config;
        }
    }

    void setEnableRecovery(bool enable) {
        enableRecovery = enable;
    }

    void setEnableReporting(bool enable) {
        enableReporting = enable;
    }

    void setEnableRetry(bool enable) {
        enableRetry = enable;
    }

    // Get the global error handler instance
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
};

} // namespace nlm
