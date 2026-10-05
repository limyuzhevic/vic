# Parameter Validator for Enhanced Configuration
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <cmath>
#include <algorithm>

namespace nlm {

class ParameterValidator {
public:
    ParameterValidator();
    ~ParameterValidator();
    
    // Parameter validation
    bool validateParameter(const std::string& key, const std::variant<int, double, bool, std::string>& value,
                          const std::string& type, const std::string& category) const;
    bool validateParameterRange(const std::variant<int, double, bool, std::string>& value,
                               const std::variant<int, double, bool, std::string>& minValue,
                               const std::variant<int, double, bool, std::string>& maxValue) const;
    bool validateParameterEnum(const std::variant<int, double, bool, std::string>& value,
                              const std::vector<std::string>& allowedValues) const;
    bool validateParameterDependencies(const std::string& key,
                                      const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& currentParameters) const;
    
    // Configuration validation
    bool validateConfiguration(const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& parameters) const;
    std::vector<std::string> getValidationErrors() const;
    std::vector<std::string> getValidationWarnings() const;
    
    // Configuration presets
    bool loadPresets(const std::string& presetsFile);
    bool savePresets(const std::string& presetsFile) const;
    std::vector<std::string> getPresetNames() const;
    std::unordered_map<std::string, std::variant<int, double, bool, std::string>> getPresetParameters(const std::string& name) const;
    
    // Parameter tuning
    std::unordered_map<std::string, std::variant<int, double, bool, std::string>> optimizeParameters(
        const std::string& algorithm = "bayesian_optimization",
        const std::vector<std::string>& parameterKeys = {}) const;
    
    // Sensitivity analysis
    std::unordered_map<std::string, double> performSensitivityAnalysis(
        const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& baselineParameters,
        const std::vector<std::string>& parameterKeys = {}) const;
    
    // Configuration validation rules
    void addValidationRule(const std::string& ruleName,
                          const std::string& ruleExpression,
                          const std::vector<std::string>& affectedParameters);
    void removeValidationRule(const std::string& ruleName);
    std::vector<std::string> getValidationRuleNames() const;
    
    // Parameter metadata
    void addParameterMetadata(const std::string& key,
                             const std::string& description,
                             const std::string& type,
                             const std::string& category,
                             const std::variant<int, double, bool, std::string>& defaultValue,
                             bool required = false,
                             const std::variant<int, double, bool, std::string>& minValue = {},
                             const std::variant<int, double, bool, std::string>& maxValue = {});
    ParameterMetadata getParameterMetadata(const std::string& key) const;
    std::vector<ParameterMetadata> getAllParameterMetadata() const;
    
    // Configuration inheritance
    void setInheritance(const std::string& childKey, const std::string& parentKey);
    void removeInheritance(const std::string& childKey);
    bool isInherited(const std::string& key) const;
    std::string getParentKey(const std::string& key) const;
    
    // Configuration expressions
    std::variant<int, double, bool, std::string> evaluateExpression(const std::string& expression,
                                                                   const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& variables) const;
    bool validateExpressionSyntax(const std::string& expression) const;
    
    // Configuration macros
    void defineMacro(const std::string& name, const std::string& definition);
    std::variant<int, double, bool, std::string> expandMacro(const std::string& macroName,
                                                           const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& arguments = {}) const;
    std::vector<std::string> getMacroNames() const;
    
    // Configuration versioning
    bool saveVersion(const std::string& versionName,
                    const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& parameters,
                    const std::string& comment = "");
    bool loadVersion(const std::string& versionName,
                    std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& outParameters) const;
    std::vector<std::string> getAvailableVersions() const;
    std::vector<ConfigurationHistoryEntry> getConfigurationHistory() const;
    
    // Configuration backup and restoration
    bool createBackup(const std::string& backupName);
    bool restoreBackup(const std::string& backupName,
                      std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& outParameters) const;
    std::vector<std::string> getAvailableBackups() const;
    bool deleteBackup(const std::string& backupName);
    
    // Performance optimization
    void enablePerformanceOptimization(bool enabled);
    bool isPerformanceOptimizationEnabled() const;
    double getEstimatedPerformance() const;
    void setPerformanceTarget(const std::string& target);
    std::string getPerformanceTarget() const;
    
    // Real-time monitoring
    void enableRealTimeMonitoring(bool enabled);
    bool isMonitoringEnabled() const;
    void setMonitoringInterval(int intervalMs);
    int getMonitoringInterval() const;
    
    // Configuration statistics
    size_t getParameterCount() const;
    size_t getChangeCount() const;
    double getConfigurationEntropy() const;
    std::unordered_map<std::string, double> getParameterStatistics() const;
    
    // Configuration security
    void enableEncryption(bool enabled, const std::string& encryptionKey = "");
    bool isEncryptionEnabled() const;
    std::string encryptConfiguration(const std::string& data) const;
    std::string decryptConfiguration(const std::string& data) const;
    
    // Configuration compression
    void enableCompression(bool enabled);
    bool isCompressionEnabled() const;
    std::string compressConfiguration(const std::string& data) const;
    std::string decompressConfiguration(const std::string& data) const;
    
    // Configuration export/import
    std::string exportConfiguration(const std::string& format = "json") const;
    bool importConfiguration(const std::string& data, const std::string& format = "json");
    
    // Configuration validation with detailed error messages
    struct DetailedValidationResult {
        bool isValid;
        std::vector<std::string> criticalErrors;
        std::vector<std::string> warnings;
        std::vector<std::string> suggestions;
        std::unordered_map<std::string, std::vector<std::string>> parameterSpecificErrors;
        std::unordered_map<std::string, std::vector<std::string>> parameterSpecificWarnings;
        
        DetailedValidationResult() : isValid(true) {}
    };
    
    DetailedValidationResult validateWithDetails(const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& parameters) const;
    std::string getValidationReport(const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& parameters) const;
    
    // Configuration comparison
    std::string compareConfigurations(const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& config1,
                                     const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& config2) const;
    
    // Configuration validation utility functions
    static bool isValidKey(const std::string& key);
    static bool isValidValue(const std::variant<int, double, bool, std::string>& value);
    static std::string getValueTypeName(const std::variant<int, double, bool, std::string>& value);
    static std::string variantToString(const std::variant<int, double, bool, std::string>& value);
    static std::variant<int, double, bool, std::string> stringToVariant(const std::string& str, const std::string& typeHint = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    std::variant<int, double, bool, std::string> resolveInheritance(const std::string& key,
                                                                   const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& parameters) const;
    bool validateExpressionSyntax(const std::string& expression, std::vector<std::string>& outErrors) const;
    std::string evaluateMathExpression(const std::string& expression) const;
};

} // namespace nlm