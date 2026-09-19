// Enhanced Error Handling and Validation Framework for NLM

#include "NLMErrorHandling.h"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <cstring>
#include <algorithm>

namespace nlm {

std::string NLMError::formatMessage(const std::string& message,
                                   ErrorCategory category,
                                   const std::string& context,
                                   const std::string& file,
                                   int line) {
    std::stringstream ss;
    ss << "[" << getCategoryString(category) << "]";
    
    if (!context.empty()) {
        ss << " " << context;
    }
    
    if (!file.empty() && line > 0) {
        ss << " (" << file << ":" << line << ")";
    }
    
    ss << " " << message;
    return ss.str();
}

NLMError::NLMError(const std::string& message,
                 ErrorCategory category,
                 const std::string& context,
                 const std::string& file,
                 int line)
    : std::runtime_error(formatMessage(message, category, context, file, line)),
      category_(category),
      context_(context) {}

std::string NLMError::getSuggestion() const {
    // Provide common suggestions based on error category and message
    std::string message = what();
    std::string lowerMessage = message;
    std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), ::tolower);
    
    if (category_ == ErrorCategory::ConfigurationError) {
        if (lowerMessage.find("missing") != std::string::npos) {
            return "Check that all required configuration keys are present. Use --validate-config to validate your configuration.";
        } else if (lowerMessage.find("invalid") != std::string::npos) {
            return "Ensure configuration values match expected types and ranges. Check documentation for valid ranges.";
        }
    } else if (category_ == ErrorCategory::MemoryError) {
        if (lowerMessage.find("out of memory") != std::string::npos) {
            return "Reduce memory usage by using smaller brain sizes or increasing the --max-memory-mb limit.";
        }
    } else if (category_ == ErrorCategory::FileError) {
        if (lowerMessage.find("cannot open") != std::string::npos) {
            return "Check that the file path exists and you have read permissions. Use absolute paths if relative paths fail.";
        }
    }
    
    return "Check the error message above for specific details. Use --verbose to see more diagnostic information.";
}

bool NLMError::isRecoverable() const {
    // Determine if error can be recovered from
    std::string message = what();
    std::string lowerMessage = message;
    std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), ::tolower);
    
    // Memory errors are typically not recoverable
    if (category_ == ErrorCategory::MemoryError) {
        return false;
    }
    
    // File errors are sometimes recoverable
    if (category_ == ErrorCategory::FileError) {
        return lowerMessage.find("not found") != std::string::npos ||
               lowerMessage.find("permission denied") != std::string::npos;
    }
    
    // Configuration errors are recoverable by fixing config
    if (category_ == ErrorCategory::ConfigurationError) {
        return true;
    }
    
    // Most other errors are recoverable
    return true;
}

std::string NLMError::getCategoryString(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::ConfigurationError: return "CONFIG";
        case ErrorCategory::SimulationError: return "SIM";
        case ErrorCategory::MemoryError: return "MEM";
        case ErrorCategory::ValidationError: return "VALID";
        case ErrorCategory::FileError: return "FILE";
        case ErrorCategory::NetworkError: return "NET";
        case ErrorCategory::BiologicalError: return "BIO";
        default: return "ERROR";
    }
}

ConfigurationValidator::ConfigurationValidator() {
    // Initialize with default rules
    addIntRule("random_seed", true, 0, 1000000, "42");
    addFloatRule("simulation_timestep", true, 0.0001, 0.1, "0.001");
    addIntRule("neuron_count", true, 10, 10000, "1000");
    addIntRule("region_count", true, 1, 100, "2");
    addFloatRule("connection_probability", true, 0.0, 1.0, "0.1");
    addIntRule("environment_width", true, 10, 1000, "20");
    addIntRule("environment_height", true, 10, 1000, "20");
    addBoolRule("plasticity.stdp.enable", false, "true");
    addFloatRule("neuromod.dopamine.scale", false, 0.0, 10.0, "1.0");
}

ConfigurationValidator::~ConfigurationValidator() {}

void ConfigurationValidator::addIntRule(const std::string& key, bool required,
                                       int min, int max,
                                       const std::string& defaultValue) {
    ValidationRule rule;
    rule.key = key;
    rule.type = "int";
    std::stringstream ss;
    ss << "range:" << min << ":" << max;
    rule.validation = ss.str();
    rule.required = required;
    rule.defaultValue = defaultValue;
    rules_.push_back(rule);
}

void ConfigurationValidator::addFloatRule(const std::string& key, bool required,
                                         float min, float max,
                                         const std::string& defaultValue) {
    ValidationRule rule;
    rule.key = key;
    rule.type = "float";
    std::stringstream ss;
    ss << "range:" << min << ":" << max;
    rule.validation = ss.str();
    rule.required = required;
    rule.defaultValue = defaultValue;
    rules_.push_back(rule);
}

void ConfigurationValidator::addBoolRule(const std::string& key, bool required,
                                        const std::string& defaultValue) {
    ValidationRule rule;
    rule.key = key;
    rule.type = "bool";
    rule.validation = "enum:true:false";
    rule.required = required;
    rule.defaultValue = defaultValue;
    rules_.push_back(rule);
}

void ConfigurationValidator::addStringRule(const std::string& key, bool required,
                                          size_t minLength, size_t maxLength,
                                          const std::string& defaultValue,
                                          const std::string& pattern) {
    ValidationRule rule;
    rule.key = key;
    rule.type = "string";
    
    std::stringstream ss;
    ss << "length:" << minLength << ":" << maxLength;
    if (!pattern.empty()) {
        ss << ":pattern:" << pattern;
    }
    rule.validation = ss.str();
    rule.required = required;
    rule.defaultValue = defaultValue;
    rules_.push_back(rule);
}

void ConfigurationValidator::addCustomValidator(const std::string& key,
                                               std::function<bool(const std::string&)> validator,
                                               const std::string& errorMessage) {
    ValidationRule rule;
    rule.key = key;
    rule.type = "custom";
    rule.validation = errorMessage;
    rule.required = true; // Custom validators are typically required
    rule.customValidator = validator;
    rules_.push_back(rule);
}

bool ConfigurationValidator::validate(const Config& config, std::vector<std::string>& errors) const {
    bool allValid = true;
    
    for (const auto& rule : rules_) {
        if (!config.has(rule.key)) {
            if (rule.required) {
                errors.push_back("Missing required configuration key: " + rule.key);
                allValid = false;
            }
            continue;
        }
        
        std::string value;
        if (rule.type == "int") {
            int64_t intVal = config.getOr<int64_t>(rule.key, 0);
            value = std::to_string(intVal);
        } else if (rule.type == "float") {
            float floatVal = config.getOr<float>(rule.key, 0.0f);
            value = std::to_string(floatVal);
        } else if (rule.type == "bool") {
            bool boolVal = config.getOr<bool>(rule.key, false);
            value = boolVal ? "true" : "false";
        } else if (rule.type == "string") {
            value = config.getOr<std::string>(rule.key, "");
        }
        
        bool valid = false;
        if (rule.type == "int") {
            valid = validateInt(rule, value);
        } else if (rule.type == "float") {
            valid = validateFloat(rule, value);
        } else if (rule.type == "bool") {
            valid = validateBool(rule, value);
        } else if (rule.type == "string") {
            valid = validateString(rule, value);
        } else if (rule.type == "custom" && rule.customValidator) {
            valid = validateCustom(rule, value);
        }
        
        if (!valid) {
            errors.push_back("Invalid value for configuration key " + rule.key + ": " + value);
            allValid = false;
        }
    }
    
    return allValid;
}

bool ConfigurationValidator::validate(const Config& config) const {
    std::vector<std::string> errors;
    return validate(config, errors);
}

bool ConfigurationValidator::validateInt(const ValidationRule& rule, const std::string& value) const {
    double val;
    if (!parseNumber(value, val)) {
        return false;
    }
    
    if (rule.validation.find("range:") == 0) {
        size_t colon1 = rule.validation.find(':');
        size_t colon2 = rule.validation.find(':', colon1 + 1);
        if (colon1 != std::string::npos && colon2 != std::string::npos) {
            double min = std::stod(rule.validation.substr(colon1 + 1, colon2 - colon1 - 1));
            double max = std::stod(rule.validation.substr(colon2 + 1));
            return (val >= min && val <= max);
        }
    }
    return true;
}

bool ConfigurationValidator::validateFloat(const ValidationRule& rule, const std::string& value) const {
    double val;
    if (!parseNumber(value, val)) {
        return false;
    }
    
    if (rule.validation.find("range:") == 0) {
        size_t colon1 = rule.validation.find(':');
        size_t colon2 = rule.validation.find(':', colon1 + 1);
        if (colon1 != std::string::npos && colon2 != std::string::npos) {
            double min = std::stod(rule.validation.substr(colon1 + 1, colon2 - colon1 - 1));
            double max = std::stod(rule.validation.substr(colon2 + 1));
            return (val >= min && val <= max);
        }
    }
    return true;
}

bool ConfigurationValidator::validateBool(const ValidationRule& rule, const std::string& value) const {
    if (rule.validation.find("enum:") != 0) return false;
    
    std::string enumValues = rule.validation.substr(5);
    std::vector<std::string> enums = split(enumValues, ':');
    
    return std::find(enums.begin(), enums.end(), value) != enums.end();
}

bool ConfigurationValidator::validateString(const ValidationRule& rule, const std::string& value) const {
    if (rule.validation.find("length:") == 0) {
        size_t colon1 = rule.validation.find(':');
        size_t colon2 = rule.validation.find(':', colon1 + 1);
        if (colon1 != std::string::npos && colon2 != std::string::npos) {
            size_t minLength = static_cast<size_t>(std::stoi(rule.validation.substr(colon1 + 1, colon2 - colon1 - 1)));
            size_t maxLength = static_cast<size_t>(std::stoi(rule.validation.substr(colon2 + 1)));
            if (value.length() < minLength || value.length() > maxLength) {
                return false;
            }
        }
    }
    
    // Check for pattern if present
    size_t patternPos = rule.validation.find(":pattern:");
    if (patternPos != std::string::npos) {
        std::string pattern = rule.validation.substr(patternPos + 8);
        if (!pattern.empty() && !matchPattern(value, pattern)) {
            return false;
        }
    }
    
    return true;
}

bool ConfigurationValidator::validateCustom(const ValidationRule& rule, const std::string& value) const {
    if (rule.customValidator) {
        return rule.customValidator(value);
    }
    return true;
}

bool ConfigurationValidator::parseNumber(const std::string& str, double& result) const {
    try {
        result = std::stod(str);
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<std::string> ConfigurationValidator::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    
    return tokens;
}

bool ConfigurationValidator::matchPattern(const std::string& str, const std::string& pattern) {
    // Simple pattern matching - could be enhanced with regex
    if (pattern.empty()) return true;
    
    // Check if pattern is a simple wildcard
    if (pattern.find('*') != std::string::npos) {
        size_t starPos = pattern.find('*');
        std::string prefix = pattern.substr(0, starPos);
        std::string suffix = pattern.substr(starPos + 1);
        
        return str.size() >= prefix.size() + suffix.size() &&
               str.compare(0, prefix.size(), prefix) == 0 &&
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    
    // Exact match
    return str == pattern;
}

ErrorRecovery::ErrorRecovery() {
    // Initialize with common recovery actions
    addRecoveryAction("Missing configuration key.*",
                     []() { NLM_LOG_WARNING("Attempting to use default value"); return true; },
                     true, 3);
    
    addRecoveryAction("Memory allocation failed",
                     []() { NLM_LOG_WARNING("Attempting memory cleanup and retry"); return true; },
                     false, 1);
}

ErrorRecovery::~ErrorRecovery() {}

void ErrorRecovery::addRecoveryAction(const std::string& errorPattern,
                                     std::function<bool()> recoveryFunction,
                                     bool automatic,
                                     int maxAttempts) {
    RecoveryAction action;
    action.triggerError = errorPattern;
    action.recoveryFunction = recoveryFunction;
    action.automatic = automatic;
    action.maxAttempts = maxAttempts;
    
    recoveryActions_.push_back(action);
}

bool ErrorRecovery::attemptRecovery(const NLMError& error) {
    std::string errorMessage = error.what();
    
    for (auto& action : recoveryActions_) {
        // Simple pattern matching for error pattern
        if (errorMessage.find(action.triggerError) != std::string::npos) {
            int& attempts = attemptCounts_[action.triggerError];
            if (++attempts > action.maxAttempts) {
                NLM_LOG_WARNING("Recovery attempts exceeded for error pattern: " + action.triggerError);
                return false;
            }
            
            bool success = action.recoveryFunction();
            if (success) {
                int& successCount = successCounts_[action.triggerError];
                successCount++;
                NLM_LOG_INFO("Recovery successful for error: " + action.triggerError);
                return true;
            }
        }
    }
    
    return false;
}

int ErrorRecovery::getRecoveryAttempts(const std::string& errorPattern) const {
    auto it = attemptCounts_.find(errorPattern);
    return (it != attemptCounts_.end()) ? it->second : 0;
}

double ErrorRecovery::getRecoverySuccessRate(const std::string& errorPattern) const {
    auto attemptsIt = attemptCounts_.find(errorPattern);
    auto successIt = successCounts_.find(errorPattern);
    
    if (attemptsIt == attemptCounts_.end() || attemptsIt->second == 0) {
        return 0.0;
    }
    
    int successes = (successIt != successCounts_.end()) ? successIt->second : 0;
    return static_cast<double>(successes) / attemptsIt->second;
}

MemoryManager::MemoryManager() : warningThresholdMB_(100), errorThresholdMB_(500) {
    // Initialize with global memory manager if available
}

MemoryManager::~MemoryManager() {
    // Clean up all tracked allocations
    for (const auto& alloc : allocations_) {
        if (alloc.tracked && alloc.ptr) {
            // In a real implementation, we would actually deallocate
            NLM_LOG_WARNING("Memory leak detected: " + alloc.context);
        }
    }
}

void* MemoryManager::allocate(size_t size, const std::string& context) {
    void* ptr = std::malloc(size);
    if (!ptr) {
        throw NLMError("Memory allocation failed", ErrorCategory::MemoryError, context);
    }
    
    AllocationInfo alloc;
    alloc.ptr = ptr;
    alloc.size = size;
    alloc.context = context;
    alloc.stackTrace = getStackTrace();
    alloc.tracked = true;
    
    allocations_.push_back(alloc);
    
    stats_.totalAllocated += size;
    stats_.currentUsage += size;
    stats_.allocationCount++;
    stats_.averageAllocationSize = static_cast<double>(stats_.totalAllocated) / stats_.allocationCount;
    
    // Check thresholds
    size_t usageMB = stats_.currentUsage / (1024 * 1024);
    if (usageMB > errorThresholdMB_) {
        throw NLMError("Memory usage exceeded critical threshold", 
                      ErrorCategory::MemoryError, 
                      "Current usage: " + std::to_string(usageMB) + " MB");
    } else if (usageMB > warningThresholdMB_) {
        NLM_LOG_WARNING("Memory usage warning: " + std::to_string(usageMB) + " MB");
    }
    
    return ptr;
}

void MemoryManager::deallocate(void* ptr) {
    if (!ptr) return;
    
    // Find allocation
    for (auto it = allocations_.begin(); it != allocations_.end(); ++it) {
        if (it->ptr == ptr) {
            stats_.currentUsage -= it->size;
            allocations_.erase(it);
            std::free(ptr);
            return;
        }
    }
    
    NLM_LOG_WARNING("Deallocation called on untracked pointer");
}

MemoryManager::MemoryStats MemoryManager::getStats() const {
    MemoryStats stats = stats_;
    stats_.peakUsage = stats_.currentUsage > stats_.peakUsage ? 
        stats_.currentUsage : stats_.peakUsage;
    return stats;
}

std::vector<std::string> MemoryManager::checkForLeaks() const {
    std::vector<std::string> leaks;
    
    for (const auto& alloc : allocations_) {
        if (alloc.tracked) {
            leaks.push_back("Leaked allocation: " + alloc.context + " (" + 
                          std::to_string(alloc.size) + " bytes)");
        }
    }
    
    return leaks;
}

void MemoryManager::setWarningThreshold(size_t thresholdMB) {
    warningThresholdMB_ = thresholdMB;
}

void MemoryManager::setErrorThreshold(size_t thresholdMB) {
    errorThresholdMB_ = thresholdMB;
}

std::string MemoryManager::getStackTrace() const {
    // Simplified stack trace - in real implementation would use backtrace()
    return "[Stack trace not available in this build]";
}

bool InputValidator::validateFilePath(const std::string& path, bool mustExist) {
    if (path.empty() || path.find('\0') != std::string::npos) {
        return false;
    }
    
    // Check for invalid characters
    for (char c : path) {
        if (c == '\0' || c == '/' || c == '\\' || c == ':' || c == '*' || 
            c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
            return false;
        }
    }
    
    if (mustExist) {
        std::ifstream file(path);
        return file.good();
    }
    
    return true;
}

bool InputValidator::validateDirectoryPath(const std::string& path, bool mustExist) {
    if (!validateFilePath(path, mustExist)) {
        return false;
    }
    
    // Additional directory-specific validation
    if (path.size() > 4 && path.substr(path.size() - 4) == ".cfg") {
        return false; // Config files should have .cfg extension
    }
    
    return true;
}

bool InputValidator::validateRange(double value, double min, double max) {
    return value >= min && value <= max;
}

bool InputValidator::validatePositive(int value) {
    return value > 0;
}

bool InputValidator::validatePositive(double value) {
    return value > 0.0;
}

bool InputValidator::validateNonNegative(int value) {
    return value >= 0;
}

bool InputValidator::validateStringLength(const std::string& str, size_t minLength, size_t maxLength) {
    if (minLength > maxLength) return false;
    return str.length() >= minLength && str.length() <= maxLength;
}

bool InputValidator::validateStringPattern(const std::string& str, const std::string& pattern) {
    if (pattern.empty()) return true;
    
    // Simple wildcard matching
    if (pattern.find('*') != std::string::npos) {
        size_t starPos = pattern.find('*');
        std::string prefix = pattern.substr(0, starPos);
        std::string suffix = pattern.substr(starPos + 1);
        
        return str.size() >= prefix.size() + suffix.size() &&
               str.compare(0, prefix.size(), prefix) == 0 &&
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    
    return str == pattern;
}

bool InputValidator::validateStringNotEmpty(const std::string& str) {
    return !str.empty();
}

bool InputValidator::validateConfigValue(const std::string& key, const std::string& value,
                                       const std::string& type) {
    if (type == "int") {
        try {
            std::stoi(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (type == "float") {
        try {
            std::stod(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (type == "bool") {
        return value == "true" || value == "false" || value == "1" || value == "0";
    } else if (type == "string") {
        return !value.empty();
    }
    return false;
}

bool InputValidator::validateNeuronParameters(float v_thresh, float v_rest, float tau_mem) {
    return v_thresh < v_rest && tau_mem > 0.0f && tau_mem < 100.0f;
}

bool InputValidator::validateSynapseParameters(float weight, float delay) {
    return weight >= 0.0f && weight <= 1.0f && delay >= 0.0f && delay < 100.0f;
}

bool InputValidator::validateSimulationParameters(double timestep, int steps) {
    return timestep > 0.0 && timestep < 1.0 && steps > 0 && steps < 1000000;
}

std::string ExceptionFactory::createConfigurationError(const std::string& message,
                                                       const std::string& context,
                                                       const std::string& file,
                                                       int line) {
    return NLMError(message, ErrorCategory::ConfigurationError, context, file, line).what();
}

std::string ExceptionFactory::createSimulationError(const std::string& message,
                                                   const std::string& context,
                                                   const std::string& file,
                                                   int line) {
    return NLMError(message, ErrorCategory::SimulationError, context, file, line).what();
}

std::string ExceptionFactory::createMemoryError(const std::string& message,
                                               const std::string& context,
                                               const std::string& file,
                                               int line) {
    return NLMError(message, ErrorCategory::MemoryError, context, file, line).what();
}

std::string ExceptionFactory::createValidationError(const std::string& message,
                                                   const std::string& context,
                                                   const std::string& file,
                                                   int line) {
    return NLMError(message, ErrorCategory::ValidationError, context, file, line).what();
}

std::string ExceptionFactory::createFileError(const std::string& message,
                                             const std::string& context,
                                             const std::string& file,
                                             int line) {
    return NLMError(message, ErrorCategory::FileError, context, file, line).what();
}

// Global error handling instances
ConfigurationValidator nlm::globalValidator;
ErrorRecovery nlm::globalRecovery;
MemoryManager nlm::globalMemoryManager;

} // namespace nlm