// Enhanced Error Handling and Validation Framework for NLM

#ifndef NLM_ERROR_HANDLING_H
#define NLM_ERROR_HANDLING_H

#include <string>
#include <vector>
#include <stdexcept>
#include <map>
#include <functional>
#include "core/Config/Config.hpp"

namespace nlm {

// Error categories for better error classification
enum class ErrorCategory {
    ConfigurationError,
    SimulationError,
    MemoryError,
    ValidationError,
    FileError,
    NetworkError,
    BiologicalError,
    GeneralError
};

// Exception class for NLM with enhanced error reporting
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, 
             ErrorCategory category = ErrorCategory::GeneralError,
             const std::string& context = "")
        : std::runtime_error(formatMessage(message, category, context)),
          category_(category),
          context_(context) {}
    
    NLMError(const std::string& message, 
             ErrorCategory category,
             const std::string& context,
             const std::string& file,
             int line)
        : std::runtime_error(formatMessage(message, category, context, file, line)),
          category_(category),
          context_(context) {}
    
    ErrorCategory getCategory() const { return category_; }
    const std::string& getContext() const { return context_; }
    
    // Suggest fixes for common errors
    std::string getSuggestion() const;
    
    // Check if error is recoverable
    bool isRecoverable() const;
    
private:
    static std::string formatMessage(const std::string& message,
                                     ErrorCategory category,
                                     const std::string& context,
                                     const std::string& file = "",
                                     int line = 0);
    
    ErrorCategory category_;
    std::string context_;
};

// Enhanced configuration validator
class ConfigurationValidator {
public:
    struct ValidationRule {
        std::string key;
        std::string type;
        std::string validation; // Range, enum, pattern, etc.
        bool required = false;
        std::string defaultValue;
        std::function<bool(const std::string&)> customValidator;
    };
    
    ConfigurationValidator();
    ~ConfigurationValidator();
    
    // Add validation rules
    void addIntRule(const std::string& key, bool required, 
                   int min = INT_MIN, int max = INT_MAX,
                   const std::string& defaultValue = "");
    void addFloatRule(const std::string& key, bool required,
                     float min = -FLT_MAX, float max = FLT_MAX,
                     const std::string& defaultValue = "");
    void addBoolRule(const std::string& key, bool required,
                    const std::string& defaultValue = "");
    void addStringRule(const std::string& key, bool required,
                      size_t minLength = 0, size_t maxLength = 0,
                      const std::string& defaultValue = "",
                      const std::string& pattern = "");
    
    void addCustomValidator(const std::string& key,
                           std::function<bool(const std::string&)> validator,
                           const std::string& errorMessage);
    
    // Validate a configuration
    bool validate(const Config& config, std::vector<std::string>& errors) const;
    bool validate(const Config& config) const;
    
    // Get validation rules
    const std::vector<ValidationRule>& getRules() const { return rules_; }
    
private:
    std::vector<ValidationRule> rules_;
    
    bool validateInt(const ValidationRule& rule, const std::string& value) const;
    bool validateFloat(const ValidationRule& rule, const std::string& value) const;
    bool validateBool(const ValidationRule& rule, const std::string& value) const;
    bool validateString(const ValidationRule& rule, const std::string& value) const;
    bool validateCustom(const ValidationRule& rule, const std::string& value) const;
    
    bool parseNumber(const std::string& str, double& result) const;
};

// Enhanced error recovery system
class ErrorRecovery {
public:
    struct RecoveryAction {
        std::string triggerError;
        std::function<bool()> recoveryFunction;
        bool automatic = false;
        int maxAttempts = 1;
    };
    
    ErrorRecovery();
    ~ErrorRecovery();
    
    // Add recovery actions
    void addRecoveryAction(const std::string& errorPattern,
                          std::function<bool()> recoveryFunction,
                          bool automatic = false,
                          int maxAttempts = 1);
    
    // Attempt recovery
    bool attemptRecovery(const NLMError& error);
    
    // Get recovery statistics
    int getRecoveryAttempts(const std::string& errorPattern) const;
    double getRecoverySuccessRate(const std::string& errorPattern) const;
    
private:
    std::vector<RecoveryAction> recoveryActions_;
    std::map<std::string, int> attemptCounts_;
    std::map<std::string, int> successCounts_;
};

// Memory management and leak detection
class MemoryManager {
public:
    struct MemoryStats {
        size_t totalAllocated = 0;
        size_t peakUsage = 0;
        size_t currentUsage = 0;
        size_t allocationCount = 0;
        double averageAllocationSize = 0.0;
    };
    
    MemoryManager();
    ~MemoryManager();
    
    // Track allocations
    void* allocate(size_t size, const std::string& context = "");
    void deallocate(void* ptr);
    
    // Get memory statistics
    MemoryStats getStats() const;
    
    // Check for potential leaks
    std::vector<std::string> checkForLeaks() const;
    
    // Memory thresholds for warnings
    void setWarningThreshold(size_t thresholdMB);
    void setErrorThreshold(size_t thresholdMB);
    
private:
    struct AllocationInfo {
        void* ptr;
        size_t size;
        std::string context;
        std::string stackTrace;
        bool tracked = false;
    };
    
    std::vector<AllocationInfo> allocations_;
    MemoryStats stats_;
    size_t warningThresholdMB_;
    size_t errorThresholdMB_;
    
    std::string getStackTrace() const;
};

// Input validation utilities
class InputValidator {
public:
    // File validation
    static bool validateFilePath(const std::string& path, bool mustExist = false);
    static bool validateDirectoryPath(const std::string& path, bool mustExist = false);
    
    // Numerical validation
    static bool validateRange(double value, double min, double max);
    static bool validatePositive(int value);
    static bool validatePositive(double value);
    static bool validateNonNegative(int value);
    
    // String validation
    static bool validateStringLength(const std::string& str, size_t minLength, size_t maxLength);
    static bool validateStringPattern(const std::string& str, const std::string& pattern);
    static bool validateStringNotEmpty(const std::string& str);
    
    // Configuration validation
    static bool validateConfigValue(const std::string& key, const std::string& value,
                                   const std::string& type);
    
    // Neural validation
    static bool validateNeuronParameters(float v_thresh, float v_rest, float tau_mem);
    static bool validateSynapseParameters(float weight, float delay);
    
    // Simulation validation
    static bool validateSimulationParameters(double timestep, int steps);
    
private:
    static bool isValidPath(const std::string& path);
    static bool matchPattern(const std::string& str, const std::string& pattern);
};

// Exception factory for creating specific NLM exceptions
class ExceptionFactory {
public:
    static NLMError createConfigurationError(const std::string& message,
                                            const std::string& context = "",
                                            const std::string& file = "",
                                            int line = 0);
    
    static NLMError createSimulationError(const std::string& message,
                                         const std::string& context = "",
                                         const std::string& file = "",
                                         int line = 0);
    
    static NLMError createMemoryError(const std::string& message,
                                     const std::string& context = "",
                                     const std::string& file = "",
                                     int line = 0);
    
    static NLMError createValidationError(const std::string& message,
                                         const std::string& context = "",
                                         const std::string& file = "",
                                         int line = 0);
    
    static NLMError createFileError(const std::string& message,
                                   const std::string& context = "",
                                   const std::string& file = "",
                                   int line = 0);
};

// Global error handling instances
extern ConfigurationValidator globalValidator;
extern ErrorRecovery globalRecovery;
extern MemoryManager globalMemoryManager;

} // namespace nlm

#endif // NLM_ERROR_HANDLING_H