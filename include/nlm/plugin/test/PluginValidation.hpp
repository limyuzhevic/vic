#ifndef NLM_PLUGIN_TEST_PLUGINVALIDATION_HPP
#define NLM_PLUGIN_TEST_PLUGINVALIDATION_HPP

#include "PluginBase.hpp"
#include "PluginTestSuite.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <chrono>
#include <sstream>
#include <fstream>

namespace nlm {
namespace plugin {

struct ValidationRule {
    std::string name;
    std::string description;
    std::function<bool(const PluginMetadata&)> validationFunction;
    std::string category;
    bool isRequired;
    std::string errorCode;
};

struct ValidationResult {
    std::string pluginName;
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::vector<std::string> infoMessages;
    std::chrono::duration<double> validationTime;
    std::chrono::system_clock::time_point timestamp;
    std::string validationLevel;
    
    std::string toString() const {
        std::stringstream ss;
        ss << "ValidationResult(" << pluginName << "):" << std::endl;
        ss << "  Valid: " << (isValid ? "true" : "false") << std::endl;
        ss << "  Errors: " << errors.size() << std::endl;
        for (const auto& error : errors) {
            ss << "    - " << error << std::endl;
        }
        ss << "  Warnings: " << warnings.size() << std::endl;
        for (const auto& warning : warnings) {
            ss << "    - " << warning << std::endl;
        }
        ss << "  Time: " << validationTime.count() << "s" << std::endl;
        return ss.str();
    }
};

class PluginValidation {
public:
    PluginValidation();
    ~PluginValidation();
    
    // Validation rule management
    bool addValidationRule(const ValidationRule& rule);
    bool removeValidationRule(const std::string& name);
    ValidationRule getValidationRule(const std::string& name) const;
    std::vector<ValidationRule> getAllValidationRules() const;
    std::vector<ValidationRule> getValidationRulesByCategory(const std::string& category) const;
    
    // Validation execution
    ValidationResult validatePlugin(const std::string& pluginName, const PluginMetadata& metadata) const;
    std::vector<ValidationResult> validateAllPlugins(const std::unordered_map<std::string, PluginMetadata>& plugins) const;
    
    // Validation configuration
    bool setValidationConfig(const std::string& key, const std::string& value);
    std::string getValidationConfig(const std::string& key) const;
    void clearValidationConfig();
    
    // Validation testing
    bool runValidationTests(const std::string& pluginName, const PluginMetadata& metadata);
    std::vector<std::string> getValidationTestResults(const std::string& pluginName) const;
    bool clearValidationTestResults(const std::string& pluginName);
    
    // Validation reporting
    std::string generateValidationReport(const std::vector<ValidationResult>& results) const;
    std::string generateDetailedValidationReport(const std::vector<ValidationResult>& results) const;
    bool saveValidationReport(const std::string& filename, const std::vector<ValidationResult>& results) const;
    bool loadValidationReport(const std::string& filename, std::vector<ValidationResult>& results);
    
    // Validation statistics
    struct ValidationStats {
        std::string pluginName;
        uint64_t totalValidations = 0;
        uint64_t successfulValidations = 0;
        uint64_t failedValidations = 0;
        double successRate = 0.0;
        std::chrono::duration<double> averageValidationTime;
        std::string lastValidationError;
        std::chrono::system_clock::time_point lastValidationTime;
    };
    
    std::vector<ValidationStats> getAllValidationStats() const;
    ValidationStats getValidationStats(const std::string& pluginName) const;
    
    // Validation performance
    bool enablePerformanceMonitoring(bool enabled);
    bool isPerformanceMonitoringEnabled() const;
    bool resetPerformanceStats();
    
    // Validation integration
    bool integrateWithTestSuite(PluginTestSuite* testSuite);
    bool deintegrateFromTestSuite(PluginTestSuite* testSuite);
    bool isIntegratedWithTestSuite(PluginTestSuite* testSuite) const;
    
    // Validation security
    bool setValidationSecurityPolicy(const std::string& policy);
    std::string getValidationSecurityPolicy() const;
    bool enableValidationEncryption(bool enabled);
    bool isValidationEncrypted() const;
    
    // Validation debugging
    std::string getValidationDebugInfo() const;
    void dumpValidationState(const std::string& filename) const;
    
    // Validation categories
    bool addValidationCategory(const std::string& category);
    bool removeValidationCategory(const std::string& category);
    std::vector<std::string> getValidationCategories() const;
    std::vector<ValidationRule> getValidationRulesByCategory(const std::string& category) const;
    
    // Validation priorities
    bool setValidationPriority(const std::string& ruleName, int priority);
    int getValidationPriority(const std::string& ruleName) const;
    std::vector<ValidationRule> getValidationRulesByPriority() const;
    
    // Validation error codes
    bool registerErrorCode(const std::string& code, const std::string& description);
    std::string getErrorCodeDescription(const std::string& code) const;
    std::vector<std::string> getAllErrorCodes() const;
    
    // Validation logging
    using ValidationLogCallback = std::function<void(const ValidationResult& result)>;
    void addValidationLogCallback(const ValidationLogCallback& callback);
    void removeValidationLogCallback(const ValidationLogCallback& callback);
    
    // Validation progress reporting
    using ValidationProgressCallback = std::function<void(double progress, const std::string& message)>;
    void addProgressCallback(const ValidationProgressCallback& callback);
    void removeProgressCallback(const ValidationProgressCallback& callback);
    
private:
    struct ValidationEntry {
        ValidationRule rule;
        int priority;
        std::string category;
    };
    
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, ValidationEntry> validationRules_;
    std::vector<std::string> validationCategories_;
    std::unordered_map<std::string, int> validationPriorities_;
    std::unordered_map<std::string, std::string> errorCodeDescriptions_;
    
    bool executionEnabled_;
    bool performanceMonitoringEnabled_;
    
    std::unordered_map<std::string, std::string> validationConfig_;
    
    std::vector<ValidationLogCallback> validationLogCallbacks_;
    std::vector<ValidationProgressCallback> progressCallbacks_;
    
    PluginTestSuite* integratedTestSuite_;
    
    std::unordered_map<std::string, ValidationStats> validationStats_;
    
    bool notifyProgressCallbacks(double progress, const std::string& message);
    bool notifyValidationLogCallbacks(const ValidationResult& result);
    ValidationResult validatePluginInternal(const std::string& pluginName, const PluginMetadata& metadata) const;
    bool applyValidationRule(const ValidationEntry& entry, const PluginMetadata& metadata, ValidationResult& result) const;
    std::string getValidationErrorCode(const std::string& ruleName) const;
    bool validatePluginDependencies(const PluginMetadata& metadata, ValidationResult& result) const;
    bool validatePluginCompatibility(const PluginMetadata& metadata, ValidationResult& result) const;
    bool validatePluginSecurity(const PluginMetadata& metadata, ValidationResult& result) const;
    bool validatePluginPerformance(const PluginMetadata& metadata, ValidationResult& result) const;
    bool validatePluginConfiguration(const PluginMetadata& metadata, ValidationResult& result) const;
    bool validatePluginLifecycle(const PluginMetadata& metadata, ValidationResult& result) const;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_TEST_PLUGINVALIDATION_HPP