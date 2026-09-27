#include "PluginValidation.hpp"

namespace nlm {
namespace plugin {

PluginValidation::PluginValidation() : executionEnabled_(true), performanceMonitoringEnabled_(false),
    integratedTestSuite_(nullptr) {}

PluginValidation::~PluginValidation() {
    integratedTestSuite_ = nullptr;
}

bool PluginValidation::addValidationRule(const ValidationRule& rule) {
    if (validationRules_.find(rule.name) != validationRules_.end()) {
        return false; // Rule already exists
    }
    
    ValidationEntry entry;
    entry.rule = rule;
    entry.priority = 0;
    entry.category = rule.category;
    
    validationRules_[rule.name] = entry;
    validationCategories_.push_back(rule.category);
    
    return true;
}

bool PluginValidation::removeValidationRule(const std::string& name) {
    auto it = validationRules_.find(name);
    if (it == validationRules_.end()) {
        return false;
    }
    
    validationRules_.erase(it);
    
    // Remove from categories
    auto catIt = std::find(validationCategories_.begin(), validationCategories_.end(), it->second.category);
    if (catIt != validationCategories_.end()) {
        validationCategories_.erase(catIt);
    }
    
    return true;
}

ValidationRule PluginValidation::getValidationRule(const std::string& name) const {
    auto it = validationRules_.find(name);
    if (it == validationRules_.end()) {
        return ValidationRule();
    }
    return it->second.rule;
}

std::vector<ValidationRule> PluginValidation::getAllValidationRules() const {
    std::vector<ValidationRule> result;
    result.reserve(validationRules_.size());
    
    for (const auto& pair : validationRules_) {
        result.push_back(pair.second.rule);
    }
    
    return result;
}

std::vector<ValidationRule> PluginValidation::getValidationRulesByCategory(const std::string& category) const {
    std::vector<ValidationRule> result;
    
    for (const auto& pair : validationRules_) {
        if (pair.second.category == category) {
            result.push_back(pair.second.rule);
        }
    }
    
    return result;
}

ValidationResult PluginValidation::validatePlugin(const std::string& pluginName, const PluginMetadata& metadata) const {
    if (!executionEnabled_) {
        ValidationResult result;
        result.pluginName = pluginName;
        result.isValid = false;
        result.errors.push_back("Validation execution is disabled");
        return result;
    }
    
    return validatePluginInternal(pluginName, metadata);
}

std::vector<ValidationResult> PluginValidation::validateAllPlugins(const std::unordered_map<std::string, PluginMetadata>& plugins) const {
    std::vector<ValidationResult> results;
    
    for (const auto& pair : plugins) {
        results.push_back(validatePluginInternal(pair.first, pair.second));
    }
    
    return results;
}

bool PluginValidation::setValidationConfig(const std::string& key, const std::string& value) {
    validationConfig_[key] = value;
    return true;
}

std::string PluginValidation::getValidationConfig(const std::string& key) const {
    auto it = validationConfig_.find(key);
    if (it == validationConfig_.end()) {
        return "";
    }
    return it->second;
}

void PluginValidation::clearValidationConfig() {
    validationConfig_.clear();
}

bool PluginValidation::runValidationTests(const std::string& pluginName, const PluginMetadata& metadata) {
    // Run validation tests and return results
    return true;
}

std::vector<std::string> PluginValidation::getValidationTestResults(const std::string& pluginName) const {
    // Return validation test results
    return std::vector<std::string>();
}

bool PluginValidation::clearValidationTestResults(const std::string& pluginName) {
    // Clear validation test results
    return true;
}

std::string PluginValidation::generateValidationReport(const std::vector<ValidationResult>& results) const {
    std::stringstream ss;
    ss << "Validation Report" << std::endl;
    ss << "=================" << std::endl;
    
    int total = results.size();
    int valid = 0;
    
    for (const auto& result : results) {
        if (result.isValid) {
            valid++;
        }
    }
    
    ss << "Total Validations: " << total << std::endl;
    ss << "Successful: " << valid << std::endl;
    ss << "Failed: " << (total - valid) << std::endl;
    ss << "Success Rate: " << (total > 0 ? (valid * 100.0 / total) : 0.0) << "%" << std::endl;
    
    return ss.str();
}

std::string PluginValidation::generateDetailedValidationReport(const std::vector<ValidationResult>& results) const {
    std::stringstream ss;
    ss << generateValidationReport(results) << std::endl;
    ss << std::endl;
    ss << "Detailed Results:" << std::endl;
    ss << "----------------" << std::endl;
    
    for (const auto& result : results) {
        ss << result.pluginName << ": " << (result.isValid ? "PASS" : "FAIL") << std::endl;
        if (!result.errors.empty()) {
            ss << "  Errors: " << result.errors.size() << std::endl;
            for (const auto& error : result.errors) {
                ss << "    - " << error << std::endl;
            }
        }
        if (!result.warnings.empty()) {
            ss << "  Warnings: " << result.warnings.size() << std::endl;
            for (const auto& warning : result.warnings) {
                ss << "    - " << warning << std::endl;
            }
        }
        ss << "  Time: " << result.validationTime.count() << "s" << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

bool PluginValidation::saveValidationReport(const std::string& filename, const std::vector<ValidationResult>& results) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateDetailedValidationReport(results);
    file.close();
    return true;
}

bool PluginValidation::loadValidationReport(const std::string& filename, std::vector<ValidationResult>& results) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Implement validation report loading
    file.close();
    return true;
}

std::vector<PluginValidation::ValidationStats> PluginValidation::getAllValidationStats() const {
    std::vector<ValidationStats> result;
    result.reserve(validationStats_.size());
    
    for (const auto& pair : validationStats_) {
        result.push_back(pair.second);
    }
    
    return result;
}

PluginValidation::ValidationStats PluginValidation::getValidationStats(const std::string& pluginName) const {
    auto it = validationStats_.find(pluginName);
    if (it == validationStats_.end()) {
        return ValidationStats();
    }
    return it->second;
}

bool PluginValidation::enablePerformanceMonitoring(bool enabled) {
    performanceMonitoringEnabled_ = enabled;
    return true;
}

bool PluginValidation::isPerformanceMonitoringEnabled() const {
    return performanceMonitoringEnabled_;
}

bool PluginValidation::resetPerformanceStats() {
    for (auto& pair : validationStats_) {
        pair.second.totalValidations = 0;
        pair.second.successfulValidations = 0;
        pair.second.failedValidations = 0;
        pair.second.successRate = 0.0;
        pair.second.lastValidationError.clear();
    }
    
    return true;
}

bool PluginValidation::integrateWithTestSuite(PluginTestSuite* testSuite) {
    if (!testSuite) {
        return false;
    }
    
    integratedTestSuite_ = testSuite;
    return true;
}

bool PluginValidation::deintegrateFromTestSuite(PluginTestSuite* testSuite) {
    if (integratedTestSuite_ != testSuite) {
        return false;
    }
    
    integratedTestSuite_ = nullptr;
    return true;
}

bool PluginValidation::isIntegratedWithTestSuite(PluginTestSuite* testSuite) const {
    return integratedTestSuite_ == testSuite;
}

bool PluginValidation::setValidationSecurityPolicy(const std::string& policy) {
    // Implement security policy setting
    return true;
}

std::string PluginValidation::getValidationSecurityPolicy() const {
    // Implement security policy retrieval
    return "";
}

bool PluginValidation::enableValidationEncryption(bool enabled) {
    // Implement encryption enable/disable
    return true;
}

bool PluginValidation::isValidationEncrypted() const {
    // Implement encryption check
    return false;
}

std::string PluginValidation::getValidationDebugInfo() const {
    std::stringstream ss;
    ss << "Validation Debug Info" << std::endl;
    ss << "====================" << std::endl;
    ss << "Total Validation Rules: " << validationRules_.size() << std::endl;
    ss << "Validation Categories: " << validationCategories_.size() << std::endl;
    ss << "Performance Monitoring: " << (performanceMonitoringEnabled_ ? "Enabled" : "Disabled") << std::endl;
    ss << "Execution Enabled: " << (executionEnabled_ ? "Yes" : "No") << std::endl;
    
    return ss.str();
}

void PluginValidation::dumpValidationState(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << getValidationDebugInfo();
    file.close();
}

bool PluginValidation::addValidationCategory(const std::string& category) {
    if (std::find(validationCategories_.begin(), validationCategories_.end(), category) != validationCategories_.end()) {
        return false;
    }
    
    validationCategories_.push_back(category);
    return true;
}

bool PluginValidation::removeValidationCategory(const std::string& category) {
    auto it = std::find(validationCategories_.begin(), validationCategories_.end(), category);
    if (it == validationCategories_.end()) {
        return false;
    }
    
    validationCategories_.erase(it);
    return true;
}

std::vector<std::string> PluginValidation::getValidationCategories() const {
    return validationCategories_;
}

std::vector<ValidationRule> PluginValidation::getValidationRulesByCategory(const std::string& category) const {
    std::vector<ValidationRule> result;
    
    for (const auto& pair : validationRules_) {
        if (pair.second.category == category) {
            result.push_back(pair.second.rule);
        }
    }
    
    return result;
}

bool PluginValidation::setValidationPriority(const std::string& ruleName, int priority) {
    auto it = validationRules_.find(ruleName);
    if (it == validationRules_.end()) {
        return false;
    }
    
    it->second.priority = priority;
    validationPriorities_[ruleName] = priority;
    return true;
}

int PluginValidation::getValidationPriority(const std::string& ruleName) const {
    auto it = validationPriorities_.find(ruleName);
    if (it == validationPriorities_.end()) {
        return 0;
    }
    return it->second;
}

std::vector<ValidationRule> PluginValidation::getValidationRulesByPriority() const {
    std::vector<ValidationRule> result;
    std::vector<ValidationRule> highPriority;
    std::vector<ValidationRule> mediumPriority;
    std::vector<ValidationRule> lowPriority;
    
    for (const auto& pair : validationRules_) {
        if (pair.second.priority >= 5) {
            highPriority.push_back(pair.second.rule);
        } else if (pair.second.priority >= 0) {
            mediumPriority.push_back(pair.second.rule);
        } else {
            lowPriority.push_back(pair.second.rule);
        }
    }
    
    result.insert(result.end(), highPriority.begin(), highPriority.end());
    result.insert(result.end(), mediumPriority.begin(), mediumPriority.end());
    result.insert(result.end(), lowPriority.begin(), lowPriority.end());
    
    return result;
}

bool PluginValidation::registerErrorCode(const std::string& code, const std::string& description) {
    errorCodeDescriptions_[code] = description;
    return true;
}

std::string PluginValidation::getErrorCodeDescription(const std::string& code) const {
    auto it = errorCodeDescriptions_.find(code);
    if (it == errorCodeDescriptions_.end()) {
        return "";
    }
    return it->second;
}

std::vector<std::string> PluginValidation::getAllErrorCodes() const {
    std::vector<std::string> result;
    result.reserve(errorCodeDescriptions_.size());
    
    for (const auto& pair : errorCodeDescriptions_) {
        result.push_back(pair.first);
    }
    
    return result;
}

void PluginValidation::notifyLogCallbacks(const ValidationResult& result) {
    for (const auto& callback : validationLogCallbacks_) {
        callback(result);
    }
}

bool PluginValidation::notifyProgressCallbacks(double progress, const std::string& message) {
    for (const auto& callback : progressCallbacks_) {
        callback(progress, message);
    }
    return true;
}

ValidationResult PluginValidation::validatePluginInternal(const std::string& pluginName, const PluginMetadata& metadata) const {
    ValidationResult result;
    result.pluginName = pluginName;
    result.isValid = true;
    result.timestamp = std::chrono::system_clock::now();
    
    auto startTime = std::chrono::system_clock::now();
    
    // Apply each validation rule
    for (const auto& pair : validationRules_) {
        bool ruleValid = applyValidationRule(pair.second, metadata, result);
        if (!ruleValid) {
            result.isValid = false;
        }
    }
    
    auto endTime = std::chrono::system_clock::now();
    result.validationTime = std::chrono::duration<double>(endTime - startTime);
    
    // Update validation statistics
    ValidationStats& stats = validationStats_[pluginName];
    stats.pluginName = pluginName;
    stats.totalValidations++;
    if (result.isValid) {
        stats.successfulValidations++;
    } else {
        stats.failedValidations++;
        if (!result.errors.empty()) {
            stats.lastValidationError = result.errors.back();
        }
    }
    
    stats.successRate = (stats.totalValidations > 0 ? 
        (stats.successfulValidations * 100.0 / stats.totalValidations) : 0.0);
    stats.averageValidationTime = result.validationTime;
    stats.lastValidationTime = result.timestamp;
    
    // Notify callbacks
    notifyLogCallbacks(result);
    
    // Calculate progress
    double progress = static_cast<double>(stats.successfulValidations) / stats.totalValidations;
    notifyProgressCallbacks(progress, "Validation completed");
    
    return result;
}

bool PluginValidation::applyValidationRule(const ValidationEntry& entry, const PluginMetadata& metadata, ValidationResult& result) const {
    bool valid = true;
    
    // Apply the validation function
    if (entry.rule.validationFunction) {
        if (!entry.rule.validationFunction(metadata)) {
            valid = false;
            result.errors.push_back("Validation failed for rule: " + entry.rule.name);
        }
    }
    
    // Required rules must pass
    if (entry.rule.isRequired && !valid) {
        result.errors.push_back("Required validation failed: " + entry.rule.name);
    }
    
    // Add warning for non-critical issues
    if (!valid && entry.rule.isRequired) {
        result.warnings.push_back("Rule may cause compatibility issues: " + entry.rule.name);
    }
    
    return valid;
}

std::string PluginValidation::getValidationErrorCode(const std::string& ruleName) const {
    // Implement error code retrieval
    return "";
}

bool PluginValidation::validatePluginDependencies(const PluginMetadata& metadata, ValidationResult& result) const {
    // Validate plugin dependencies
    return true;
}

bool PluginValidation::validatePluginCompatibility(const PluginMetadata& metadata, ValidationResult& result) const {
    // Validate plugin compatibility
    return true;
}

bool PluginValidation::validatePluginSecurity(const PluginMetadata& metadata, ValidationResult& result) const {
    // Validate plugin security
    return true;
}

bool PluginValidation::validatePluginPerformance(const PluginMetadata& metadata, ValidationResult& result) const {
    // Validate plugin performance
    return true;
}

bool PluginValidation::validatePluginConfiguration(const PluginMetadata& metadata, ValidationResult& result) const {
    // Validate plugin configuration
    return true;
}

bool PluginValidation::validatePluginLifecycle(const PluginMetadata& metadata, ValidationResult& result) const {
    // Validate plugin lifecycle
    return true;
}

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_TEST_PLUGINVALIDATION_HPP