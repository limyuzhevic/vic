# ParameterValidator Implementation
#include "ParameterValidator.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <random>

namespace nlm {

struct ParameterValidator::Impl {
    std::unordered_map<std::string, ParameterMetadata> parameterMetadata;
    std::vector<std::string> validationRules;
    std::vector<ParameterDependency> dependencies;
    std::unordered_map<std::string, std::string> inheritanceMap;
    std::unordered_map<std::string, std::string> macroDefinitions;
    std::vector<ConfigurationHistoryEntry> versionHistory;
    std::unordered_map<std::string, std::string> backups;
    
    bool performanceOptimizationEnabled = false;
    bool realTimeMonitoringEnabled = false;
    bool encryptionEnabled = false;
    bool compressionEnabled = false;
    std::string encryptionKey;
    std::string performanceTarget = "balanced";
    int monitoringInterval = 1000;
    
    std::unordered_map<std::string, double> parameterStatistics;
    size_t parameterChangeCount = 0;
    std::chrono::system_clock::time_point lastMonitoringCheck;
    
    Impl() {
        lastMonitoringCheck = std::chrono::system_clock::now();
    }
};

ParameterValidator::ParameterValidator() : pImpl(std::make_unique<Impl>()) {}

ParameterValidator::~ParameterValidator() = default;

bool ParameterValidator::validateParameter(const std::string& key, const std::variant<int, double, bool, std::string>& value,
                                          const std::string& type, const std::string& category) const {
    // Basic validation
    if (key.empty()) {
        return false;
    }
    
    // Check type
    std::string actualType = getValueTypeName(value);
    if (!type.empty() && actualType != type) {
        return false;
    }
    
    // Check metadata if available
    auto it = pImpl->parameterMetadata.find(key);
    if (it != pImpl->parameterMetadata.end()) {
        const ParameterMetadata& metadata = it->second;
        
        // Check type match
        if (!metadata.type.empty() && getValueTypeName(metadata.defaultValue) != metadata.type) {
            return false;
        }
        
        // Check range
        if (!isEmptyVariant(metadata.minValue) && !isEmptyVariant(metadata.maxValue)) {
            if (getValueTypeName(metadata.minValue) != getValueTypeName(value) ||
                getValueTypeName(metadata.maxValue) != getValueTypeName(value)) {
                return false;
            }
            
            if (std::get<double>(value) < std::get<double>(metadata.minValue) ||
                std::get<double>(value) > std::get<double>(metadata.maxValue)) {
                return false;
            }
        }
        
        // Check enum values
        if (!metadata.allowedValues.empty()) {
            std::string strValue = variantToString(value);
            if (std::find(metadata.allowedValues.begin(), metadata.allowedValues.end(), strValue) == metadata.allowedValues.end()) {
                return false;
            }
        }
    }
    
    return true;
}

bool ParameterValidator::validateParameterRange(const std::variant<int, double, bool, std::string>& value,
                                               const std::variant<int, double, bool, std::string>& minValue,
                                               const std::variant<int, double, bool, std::string>& maxValue) const {
    if (isEmptyVariant(minValue) || isEmptyVariant(maxValue)) {
        return true;
    }
    
    if (getValueTypeName(minValue) != getValueTypeName(value) ||
        getValueTypeName(maxValue) != getValueTypeName(value)) {
        return false;
    }
    
    double val = std::get<double>(value);
    double min = std::get<double>(minValue);
    double max = std::get<double>(maxValue);
    
    return val >= min && val <= max;
}

bool ParameterValidator::validateParameterEnum(const std::variant<int, double, bool, std::string>& value,
                                              const std::vector<std::string>& allowedValues) const {
    std::string strValue = variantToString(value);
    return std::find(allowedValues.begin(), allowedValues.end(), strValue) != allowedValues.end();
}

bool ParameterValidator::validateParameterDependencies(const std::string& key,
                                                      const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& currentParameters) const {
    // Check dependencies for this key
    for (const auto& dependency : pImpl->dependencies) {
        if (dependency.dependentKey == key) {
            auto it = currentParameters.find(dependency.requiredKey);
            if (it == currentParameters.end()) {
                return false;
            }
            
            if (!dependency.dependencyRule(it->second, currentParameters.at(key))) {
                return false;
            }
        }
    }
    
    // Also check if any dependencies require this key
    for (const auto& dependency : pImpl->dependencies) {
        if (dependency.requiredKey == key) {
            auto it = currentParameters.find(dependency.dependentKey);
            if (it != currentParameters.end()) {
                if (!dependency.dependencyRule(currentParameters.at(dependency.dependentKey), it->second)) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

bool ParameterValidator::validateConfiguration(const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& parameters) const {
    // Validate all parameters
    for (const auto& param : parameters) {
        if (!validateParameter(param.first, param.second, "", "")) {
            return false;
        }
    }
    
    // Validate dependencies
    for (const auto& param : parameters) {
        if (!validateParameterDependencies(param.first, parameters)) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ParameterValidator::getValidationErrors() const {
    std::vector<std::string> errors;
    // Implementation would need actual error tracking
    return errors;
}

std::vector<std::string> ParameterValidator::getValidationWarnings() const {
    std::vector<std::string> warnings;
    // Implementation would need actual warning tracking
    return warnings;
}

bool ParameterValidator::loadPresets(const std::string& presetsFile) {
    try {
        std::ifstream file(presetsFile);
        if (!file.is_open()) {
            return false;
        }
        
        // Parse JSON file
        // For now, just return true
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ParameterValidator::savePresets(const std::string& presetsFile) const {
    try {
        std::ofstream file(presetsFile);
        if (!file.is_open()) {
            return false;
        }
        
        // Write JSON
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::string> ParameterValidator::getPresetNames() const {
    return std::vector<std::string>();
}

std::unordered_map<std::string, std::variant<int, double, bool, std::string>> 
ParameterValidator::getPresetParameters(const std::string& name) const {
    return std::unordered_map<std::string, std::variant<int, double, bool, std::string>>();
}

std::unordered_map<std::string, std::variant<int, double, bool, std::string>> 
ParameterValidator::optimizeParameters(const std::string& algorithm, const std::vector<std::string>& parameterKeys) const {
    return std::unordered_map<std::string, std::variant<int, double, bool, std::string>>();
}

std::unordered_map<std::string, double> 
ParameterValidator::performSensitivityAnalysis(
    const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& baselineParameters,
    const std::vector<std::string>& parameterKeys) const {
    return std::unordered_map<std::string, double>();
}

void ParameterValidator::addValidationRule(const std::string& ruleName,
                                          const std::string& ruleExpression,
                                          const std::vector<std::string>& affectedParameters) {
    pImpl->validationRules.push_back(ruleName);
}

void ParameterValidator::removeValidationRule(const std::string& ruleName) {
    auto it = std::remove(pImpl->validationRules.begin(), pImpl->validationRules.end(), ruleName);
    pImpl->validationRules.erase(it, pImpl->validationRules.end());
}

std::vector<std::string> ParameterValidator::getValidationRuleNames() const {
    return pImpl->validationRules;
}

void ParameterValidator::addParameterMetadata(const std::string& key,
                                             const std::string& description,
                                             const std::string& type,
                                             const std::string& category,
                                             const std::variant<int, double, bool, std::string>& defaultValue,
                                             bool required,
                                             const std::variant<int, double, bool, std::string>& minValue,
                                             const std::variant<int, double, bool, std::string>& maxValue) {
    ParameterMetadata metadata;
    metadata.key = key;
    metadata.description = description;
    metadata.type = type;
    metadata.category = category;
    metadata.defaultValue = defaultValue;
    metadata.required = required;
    metadata.minValue = minValue;
    metadata.maxValue = maxValue;
    
    pImpl->parameterMetadata[key] = metadata;
}

ParameterMetadata ParameterValidator::getParameterMetadata(const std::string& key) const {
    auto it = pImpl->parameterMetadata.find(key);
    if (it != pImpl->parameterMetadata.end()) {
        return it->second;
    }
    
    ParameterMetadata emptyMetadata;
    emptyMetadata.key = key;
    return emptyMetadata;
}

std::vector<ParameterMetadata> ParameterValidator::getAllParameterMetadata() const {
    std::vector<ParameterMetadata> metadata;
    for (const auto& pair : pImpl->parameterMetadata) {
        metadata.push_back(pair.second);
    }
    return metadata;
}

void ParameterValidator::setInheritance(const std::string& childKey, const std::string& parentKey) {
    pImpl->inheritanceMap[childKey] = parentKey;
}

void ParameterValidator::removeInheritance(const std::string& childKey) {
    pImpl->inheritanceMap.erase(childKey);
}

bool ParameterValidator::isInherited(const std::string& key) const {
    return pImpl->inheritanceMap.find(key) != pImpl->inheritanceMap.end();
}

std::string ParameterValidator::getParentKey(const std::string& key) const {
    auto it = pImpl->inheritanceMap.find(key);
    if (it != pImpl->inheritanceMap.end()) {
        return it->second;
    }
    return "";
}

void ParameterValidator::defineMacro(const std::string& name, const std::string& definition) {
    pImpl->macroDefinitions[name] = definition;
}

std::variant<int, double, bool, std::string> ParameterValidator::expandMacro(const std::string& macroName,
                                                                           const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& arguments) const {
    auto it = pImpl->macroDefinitions.find(macroName);
    if (it != pImpl->macroDefinitions.end()) {
        // For now, just return the definition as string
        return it->second;
    }
    return std::variant<int, double, bool, std::string>("");
}

std::vector<std::string> ParameterValidator::getMacroNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->macroDefinitions) {
        names.push_back(pair.first);
    }
    return names;
}

bool ParameterValidator::saveVersion(const std::string& versionName,
                                    const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& parameters,
                                    const std::string& comment) {
    ConfigurationHistoryEntry entry(versionName, comment);
    pImpl->versionHistory.push_back(entry);
    return true;
}

bool ParameterValidator::loadVersion(const std::string& versionName,
                                    std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& outParameters) const {
    // For now, just return false
    return false;
}

std::vector<std::string> ParameterValidator::getAvailableVersions() const {
    std::vector<std::string> versions;
    for (const auto& entry : pImpl->versionHistory) {
        versions.push_back(entry.version);
    }
    return versions;
}

std::vector<ConfigurationHistoryEntry> ParameterValidator::getConfigurationHistory() const {
    return pImpl->versionHistory;
}

bool ParameterValidator::createBackup(const std::string& backupName) {
    // For now, just return false
    return false;
}

bool ParameterValidator::restoreBackup(const std::string& backupName,
                                       std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& outParameters) const {
    // For now, just return false
    return false;
}

std::vector<std::string> ParameterValidator::getAvailableBackups() const {
    return std::vector<std::string>();
}

bool ParameterValidator::deleteBackup(const std::string& backupName) {
    // For now, just return false
    return false;
}

void ParameterValidator::enablePerformanceOptimization(bool enabled) {
    pImpl->performanceOptimizationEnabled = enabled;
}

bool ParameterValidator::isPerformanceOptimizationEnabled() const {
    return pImpl->performanceOptimizationEnabled;
}

double ParameterValidator::getEstimatedPerformance() const {
    return 1.0;
}

void ParameterValidator::setPerformanceTarget(const std::string& target) {
    pImpl->performanceTarget = target;
}

std::string ParameterValidator::getPerformanceTarget() const {
    return pImpl->performanceTarget;
}

void ParameterValidator::enableRealTimeMonitoring(bool enabled) {
    pImpl->realTimeMonitoringEnabled = enabled;
}

bool ParameterValidator::isMonitoringEnabled() const {
    return pImpl->realTimeMonitoringEnabled;
}

void ParameterValidator::setMonitoringInterval(int intervalMs) {
    pImpl->monitoringInterval = intervalMs;
}

int ParameterValidator::getMonitoringInterval() const {
    return pImpl->monitoringInterval;
}

size_t ParameterValidator::getParameterCount() const {
    return pImpl->parameterMetadata.size();
}

size_t ParameterValidator::getChangeCount() const {
    return pImpl->parameterChangeCount;
}

double ParameterValidator::getConfigurationEntropy() const {
    // Calculate entropy based on parameter values
    if (pImpl->parameterMetadata.empty()) {
        return 0.0;
    }
    
    std::vector<double> values;
    for (const auto& param : pImpl->parameterMetadata) {
        values.push_back(std::get<double>(param.second.defaultValue));
    }
    
    double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    double variance = 0.0;
    for (double val : values) {
        variance += (val - mean) * (val - mean);
    }
    variance /= values.size();
    
    return std::sqrt(variance);
}

std::unordered_map<std::string, double> ParameterValidator::getParameterStatistics() const {
    return pImpl->parameterStatistics;
}

void ParameterValidator::enableEncryption(bool enabled, const std::string& encryptionKey) {
    pImpl->encryptionEnabled = enabled;
    pImpl->encryptionKey = encryptionKey;
}

bool ParameterValidator::isEncryptionEnabled() const {
    return pImpl->encryptionEnabled;
}

std::string ParameterValidator::encryptConfiguration(const std::string& data) const {
    // For now, just return the data unchanged
    return data;
}

std::string ParameterValidator::decryptConfiguration(const std::string& data) const {
    // For now, just return the data unchanged
    return data;
}

void ParameterValidator::enableCompression(bool enabled) {
    pImpl->compressionEnabled = enabled;
}

bool ParameterValidator::isCompressionEnabled() const {
    return pImpl->compressionEnabled;
}

std::string ParameterValidator::compressConfiguration(const std::string& data) const {
    // For now, just return the data unchanged
    return data;
}

std::string ParameterValidator::decompressConfiguration(const std::string& data) const {
    // For now, just return the data unchanged
    return data;
}

std::string ParameterValidator::exportConfiguration(const std::string& format) const {
    // For now, return JSON format
    return "{}";
}

bool ParameterValidator::importConfiguration(const std::string& data, const std::string& format) {
    // For now, just return false
    return false;
}

ParameterValidator::DetailedValidationResult ParameterValidator::validateWithDetails(
    const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& parameters) const {
    DetailedValidationResult result;
    
    for (const auto& param : parameters) {
        // Validate each parameter
        if (!validateParameter(param.first, param.second, "", "")) {
            result.criticalErrors.push_back("Invalid parameter: " + param.first);
        }
    }
    
    return result;
}

std::string ParameterValidator::getValidationReport(
    const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& parameters) const {
    std::ostringstream oss;
    oss << "Configuration Validation Report\n";
    oss << "===============================\n";
    
    DetailedValidationResult result = validateWithDetails(parameters);
    
    if (result.isValid) {
        oss << "VALIDATION PASSED\n";
        oss << "No errors or warnings found.\n";
    } else {
        oss << "VALIDATION FAILED\n";
        oss << "Critical Errors:\n";
        for (const auto& error : result.criticalErrors) {
            oss << "  - " << error << "\n";
        }
        
        if (!result.warnings.empty()) {
            oss << "Warnings:\n";
            for (const auto& warning : result.warnings) {
                oss << "  - " << warning << "\n";
            }
        }
    }
    
    return oss.str();
}

std::string ParameterValidator::compareConfigurations(
    const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& config1,
    const std::unordered_map<std::string, std::variant<int, double, bool, std::string>>& config2) const {
    std::ostringstream oss;
    oss << "Configuration Comparison\n";
    oss << "========================\n";
    
    oss << "Config 1: " << config1.size() << " parameters\n";
    oss << "Config 2: " << config2.size() << " parameters\n";
    
    // Find common keys
    std::vector<std::string> commonKeys;
    for (const auto& param : config1) {
        if (config2.find(param.first) != config2.end()) {
            commonKeys.push_back(param.first);
        }
    }
    
    oss << "Common parameters: " << commonKeys.size() << "\n";
    
    // Find differences
    oss << "\nDifferences:\n";
    for (const auto& key : commonKeys) {
        if (config1.at(key) != config2.at(key)) {
            oss << "  " << key << ": " << variantToString(config1.at(key)) << " != " << variantToString(config2.at(key)) << "\n";
        }
    }
    
    // Find only in config1
    for (const auto& param : config1) {
        if (config2.find(param.first) == config2.end()) {
            oss << "Only in Config 1: " << param.first << " = " << variantToString(param.second) << "\n";
        }
    }
    
    // Find only in config2
    for (const auto& param : config2) {
        if (config1.find(param.first) == config1.end()) {
            oss << "Only in Config 2: " << param.first << " = " << variantToString(param.second) << "\n";
        }
    }
    
    return oss.str();
}

// Static utility methods
bool ParameterValidator::isValidKey(const std::string& key) {
    return !key.empty() && key.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.-") == std::string::npos;
}

bool ParameterValidator::isValidValue(const std::variant<int, double, bool, std::string>& value) {
    // Basic validation
    return !value.index();
}

std::string ParameterValidator::getValueTypeName(const std::variant<int, double, bool, std::string>& value) {
    switch (value.index()) {
        case 0: return "int";
        case 1: return "double";
        case 2: return "bool";
        case 3: return "string";
        default: return "unknown";
    }
}

std::string ParameterValidator::variantToString(const std::variant<int, double, bool, std::string>& value) {
    std::ostringstream oss;
    std::visit([&](auto&& arg) {
        oss << arg;
    }, value);
    return oss.str();
}

std::variant<int, double, bool, std::string> ParameterValidator::stringToVariant(const std::string& str, const std::string& typeHint) {
    if (typeHint == "int") {
        try {
            return std::stoi(str);
        } catch (...) {
            return 0;
        }
    } else if (typeHint == "double") {
        try {
            return std::stod(str);
        } catch (...) {
            return 0.0;
        }
    } else if (typeHint == "bool") {
        if (str == "true" || str == "1") return true;
        if (str == "false" || str == "0") return false;
        return false;
    } else {
        return str;
    }
}

bool ParameterValidator::isEmptyVariant(const std::variant<int, double, bool, std::string>& value) {
    // Simple check - if it's the default value
    return std::get_if<int>(&value) != nullptr && std::get<int>(value) == 0;
}

} // namespace nlm