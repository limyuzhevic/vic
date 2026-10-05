#include "EnhancedConfig.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <random>
#include <numeric>

namespace nlm {

struct EnhancedConfig::Impl {
    std::vector<ParameterMetadata> parameterMetadata;
    std::unordered_map<std::string, EnhancedConfigValue> enhancedParameters;
    std::vector<ParameterDependency> dependencies;
    std::vector<ConfigChangeCallback> changeCallbacks;
    std::stack<ConfigurationHistoryEntry> historyStack;
    std::unordered_map<std::string, ConfigurationHistoryEntry> versionHistory;
    std::unordered_map<std::string, std::string> backups;
    std::unordered_map<std::string, std::string> macros;
    std::unordered_map<std::string, bool> inheritanceEnabled;
    std::unordered_map<std::string, std::string> parentPaths;
    
    bool monitoringEnabled = false;
    bool changeNotificationEnabled = true;
    bool advancedValidationEnabled = false;
    bool sensitivityAnalysisEnabled = false;
    bool autoGenerationEnabled = false;
    bool migrationWizardEnabled = false;
    bool performanceTuningEnabled = false;
    bool encryptionEnabled = false;
    bool compressionEnabled = false;
    std::string encryptionKey;
    std::string performanceTarget = "balanced";
    
    std::unordered_map<std::string, std::vector<ParameterChangeNotification>> parameterChangeHistory;
    std::chrono::system_clock::time_point lastMonitoringCheck;
    
    // Expression evaluator state
    std::unordered_map<std::string, std::function<EnhancedConfigValue(const std::unordered_map<std::string, EnhancedConfigValue>&)>> expressionEvaluators;
    
    Impl() {
        lastMonitoringCheck = std::chrono::system_clock::now();
        
        // Register built-in expression functions
        registerBuiltInFunctions();
    }
    
    void registerBuiltInFunctions() {
        // Mathematical functions
        expressionEvaluators["sin"] = [](const std::unordered_map<std::string, EnhancedConfigValue>& vars) {
            if (vars.find("x") != vars.end()) {
                double x = std::get<double>(vars.at("x"));
                return std::sin(x);
            }
            return EnhancedConfigValue(0.0);
        };
        
        expressionEvaluators["cos"] = [](const std::unordered_map<std::string, EnhancedConfigValue>& vars) {
            if (vars.find("x") != vars.end()) {
                double x = std::get<double>(vars.at("x"));
                return std::cos(x);
            }
            return EnhancedConfigValue(0.0);
        };
        
        expressionEvaluators["exp"] = [](const std::unordered_map<std::string, EnhancedConfigValue>& vars) {
            if (vars.find("x") != vars.end()) {
                double x = std::get<double>(vars.at("x"));
                return std::exp(x);
            }
            return EnhancedConfigValue(0.0);
        };
        
        expressionEvaluators["log"] = [](const std::unordered_map<std::string, EnhancedConfigValue>& vars) {
            if (vars.find("x") != vars.end()) {
                double x = std::get<double>(vars.at("x"));
                return std::log(std::max(x, 1e-10));
            }
            return EnhancedConfigValue(0.0);
        };
        
        expressionEvaluators["sqrt"] = [](const std::unordered_map<std::string, EnhancedConfigValue>& vars) {
            if (vars.find("x") != vars.end()) {
                double x = std::get<double>(vars.at("x"));
                return std::sqrt(std::max(x, 0.0));
            }
            return EnhancedConfigValue(0.0);
        };
        
        // Statistical functions
        expressionEvaluators["mean"] = [](const std::unordered_map<std::string, EnhancedConfigValue>& vars) {
            if (vars.find("values") != vars.end()) {
                const auto& values = std::get<std::vector<double>>(vars.at("values"));
                if (!values.empty()) {
                    double sum = std::accumulate(values.begin(), values.end(), 0.0);
                    return sum / values.size();
                }
            }
            return EnhancedConfigValue(0.0);
        };
        
        expressionEvaluators["clamp"] = [](const std::unordered_map<std::string, EnhancedConfigValue>& vars) {
            if (vars.find("value") != vars.end() && vars.find("min") != vars.end() && vars.find("max") != vars.end()) {
                double value = std::get<double>(vars.at("value"));
                double min = std::get<double>(vars.at("min"));
                double max = std::get<double>(vars.at("max"));
                return std::clamp(value, min, max);
            }
            return EnhancedConfigValue(0.0);
        };
    }
};

EnhancedConfig::EnhancedConfig() : pImpl(std::make_unique<Impl>()) {}

EnhancedConfig::~EnhancedConfig() = default;

bool EnhancedConfig::loadFromFile(const std::string& filepath) {
    try {
        // Load base configuration first
        if (!Config::loadFromFile(filepath)) {
            NLM_LOG_ERROR("Failed to load base configuration from: " + filepath);
            return false;
        }
        
        // Load enhanced configuration from JSON file if available
        std::string enhancedFilepath = filepath.substr(0, filepath.find_last_of('.')) + "_enhanced.json";
        if (std::filesystem::exists(enhancedFilepath)) {
            // Load enhanced configuration
            // For now, we'll just log that enhanced config exists
            NLM_LOG_INFO("Enhanced configuration file found: " + enhancedFilepath);
        }
        
        // Load parameter metadata
        std::string metadataFilepath = filepath.substr(0, filepath.find_last_of('.')) + "_metadata.json";
        if (std::filesystem::exists(metadataFilepath)) {
            // Load parameter metadata
            NLM_LOG_INFO("Parameter metadata file found: " + metadataFilepath);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading enhanced configuration file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool EnhancedConfig::loadFromArgs(int argc, char** argv) {
    if (!Config::loadFromArgs(argc, argv)) {
        return false;
    }
    
    // Process enhanced command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --enhanced-* arguments
        if (arg.substr(0, 11) == "--enhanced-") {
            std::string key = arg.substr(11);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string value = argv[++i];
                // Parse and set enhanced parameter
                // For now, just log
                NLM_LOG_INFO("Enhanced config: " + key + " = " + value);
            }
        }
    }
    
    return true;
}

bool EnhancedConfig::saveToFile(const std::string& filepath) const {
    try {
        // Save base configuration first
        if (!Config::saveToFile(filepath)) {
            return false;
        }
        
        // Save enhanced configuration to separate file
        std::string enhancedFilepath = filepath.substr(0, filepath.find_last_of('.')) + "_enhanced.json";
        
        std::ofstream enhancedFile(enhancedFilepath);
        if (!enhancedFile.is_open()) {
            NLM_LOG_ERROR("Failed to open enhanced config file for writing: " + enhancedFilepath);
            return false;
        }
        
        enhancedFile << "{\n";
        
        // Write enhanced parameters
        enhancedFile << "  \"enhanced_parameters\": {\n";
        bool firstParam = true;
        for (const auto& entry : pImpl->enhancedParameters) {
            if (!firstParam) enhancedFile << ",\n";
            firstParam = false;
            enhancedFile << "    \"" << entry.first << "\": " << ConfigTypeUtils::enhancedValueToString(entry.second);
        }
        enhancedFile << "\n  },\n";
        
        // Write parameter metadata
        enhancedFile << "  \"parameter_metadata\": [\n";
        firstParam = true;
        for (const auto& metadata : pImpl->parameterMetadata) {
            if (!firstParam) enhancedFile << ",\n";
            firstParam = false;
            enhancedFile << "    {\n";
            enhancedFile << "      \"key\": \"" << metadata.key << "\",\n";
            enhancedFile << "      \"description\": \"" << metadata.description << "\",\n";
            enhancedFile << "      \"category\": \"" << metadata.category << "\",\n";
            enhancedFile << "      \"type\": \"" << metadata.type << "\",\n";
            enhancedFile << "      \"required\": " << (metadata.required ? "true" : "false") << ",\n";
            enhancedFile << "      \"readOnly\": " << (metadata.readOnly ? "true" : "false") << "\n";
            enhancedFile << "    }";
        }
        enhancedFile << "\n  ]\n";
        
        enhancedFile << "}\n";
        
        enhancedFile.close();
        NLM_LOG_INFO("Enhanced configuration saved to: " + enhancedFilepath);
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception saving enhanced configuration file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

template<typename T>
std::optional<T> EnhancedConfig::get(const std::string& key) const {
    // First try base implementation
    auto baseValue = Config::get<T>(key);
    if (baseValue.has_value()) {
        return baseValue;
    }
    
    // Try enhanced parameter storage
    auto it = pImpl->enhancedParameters.find(key);
    if (it != pImpl->enhancedParameters.end()) {
        try {
            return std::get<T>(it->second);
        } catch (const std::bad_variant_access&) {
            NLM_LOG_ERROR("Type mismatch for enhanced parameter: " + key);
        }
    }
    
    return std::nullopt;
}

template<typename T>
T EnhancedConfig::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value.has_value()) {
        return value.value();
    }
    
    // Fallback to base implementation
    return Config::getOr<T>(key, defaultValue);
}

void EnhancedConfig::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Check if parameter is read-only
    for (const auto& metadata : pImpl->parameterMetadata) {
        if (metadata.key == key && metadata.readOnly) {
            NLM_LOG_WARNING("Cannot modify read-only parameter: " + key);
            return;
        }
    }
    
    // Check if parameter exists in enhanced parameters
    auto it = pImpl->enhancedParameters.find(key);
    EnhancedConfigValue enhancedValue;
    
    if (it != pImpl->enhancedParameters.end()) {
        enhancedValue = it->second;
    }
    
    // Convert ConfigValue to EnhancedConfigValue
    enhancedValue = convertConfigValueToEnhanced(value);
    
    // Validate parameter
    ParameterValidationResult validation = validateParameter(key, enhancedValue);
    if (!validation.isValid && pImpl->advancedValidationEnabled) {
        NLM_LOG_ERROR("Parameter validation failed for key " + key + ": " + validation.errorMessage);
        return;
    }
    
    // Update parameter
    pImpl->enhancedParameters[key] = enhancedValue;
    
    // Update base configuration
    Config::set(key, value, source);
    
    // Notify change
    ParameterChangeNotification notification(key, enhancedValue, enhancedValue, source, "manual");
    notifyChange(notification);
}

bool EnhancedConfig::has(const std::string& key) const {
    return Config::has(key) || pImpl->enhancedParameters.find(key) != pImpl->enhancedParameters.end();
}

void EnhancedConfig::remove(const std::string& key) {
    // Check if parameter is read-only
    for (const auto& metadata : pImpl->parameterMetadata) {
        if (metadata.key == key && metadata.readOnly) {
            NLM_LOG_WARNING("Cannot remove read-only parameter: " + key);
            return;
        }
    }
    
    // Remove from enhanced parameters
    pImpl->enhancedParameters.erase(key);
    
    // Remove from base configuration
    Config::remove(key);
    
    // Notify change
    ParameterChangeNotification notification(key, EnhancedConfigValue(), EnhancedConfigValue(), ConfigSource::Runtime, "removal");
    notifyChange(notification);
}

std::vector<std::string> EnhancedConfig::getKeys() const {
    std::vector<std::string> keys = Config::getKeys();
    for (const auto& entry : pImpl->enhancedParameters) {
        if (std::find(keys.begin(), keys.end(), entry.first) == keys.end()) {
            keys.push_back(entry.first);
        }
    }
    return keys;
}

void EnhancedConfig::clear() {
    Config::clear();
    pImpl->enhancedParameters.clear();
    
    // Notify change
    ParameterChangeNotification notification("all", EnhancedConfigValue(), EnhancedConfigValue(), ConfigSource::Runtime, "clear");
    notifyChange(notification);
}

std::string EnhancedConfig::summary() const {
    std::string baseSummary = Config::summary();
    
    std::ostringstream oss;
    oss << "Enhanced " << baseSummary;
    oss << "\nEnhanced Configuration Statistics:";
    oss << "\n  Total Parameters: " << getParameterCount();
    oss << "\n  Parameter Changes: " << getChangeCount();
    oss << "\n  Current Version: " << getCurrentVersion();
    oss << "\n  Monitoring Enabled: " << (isMonitoringEnabled() ? "Yes" : "No");
    oss << "\n  Change Notifications: " << (isChangeNotificationEnabled() ? "Enabled" : "Disabled");
    
    return oss.str();
}

// Enhanced configuration implementation continues...

EnhancedConfigValue EnhancedConfig::convertConfigValueToEnhanced(const ConfigValue& value) const {
    EnhancedConfigValue enhancedValue;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            enhancedValue = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            enhancedValue = static_cast<double>(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            enhancedValue = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            enhancedValue = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            enhancedValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::vector<double> converted;
            converted.reserve(arg.size());
            for (const auto& item : arg) {
                converted.push_back(static_cast<double>(item));
            }
            enhancedValue = converted;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            enhancedValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            enhancedValue = arg;
        }
    }, value);
    
    return enhancedValue;
}

void EnhancedConfig::notifyChange(const ParameterChangeNotification& notification) {
    if (pImpl->changeNotificationEnabled) {
        // Store change history
        pImpl->parameterChangeHistory[notification.key].push_back(notification);
        
        // Call registered callbacks
        for (const auto& callback : pImpl->changeCallbacks) {
            try {
                callback(notification);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Exception in config change callback: " + std::string(e.what()));
            }
        }
        
        // Log change
        NLM_LOG_INFO("Configuration change: " + notification.key + " = " + 
                    ConfigTypeUtils::enhancedValueToString(notification.newValue) +
                    " (reason: " + notification.reason + ")");
    }
}

void EnhancedConfig::validateDependencyChain() const {
    for (const auto& dependency : pImpl->dependencies) {
        if (!validateParameterDependencies(dependency.dependentKey)) {
            NLM_LOG_WARNING("Parameter dependency validation failed for: " + dependency.dependentKey);
        }
    }
}

EnhancedConfigValue EnhancedConfig::resolveInheritance(const std::string& path) const {
    auto it = pImpl->parentPaths.find(path);
    if (it != pImpl->parentPaths.end()) {
        std::string parentPath = it->second;
        auto parentIt = pImpl->enhancedParameters.find(parentPath);
        if (parentIt != pImpl->enhancedParameters.end()) {
            return parentIt->second;
        }
    }
    return EnhancedConfigValue();
}

bool EnhancedConfig::validateNestedPath(const std::string& path) const {
    // Simple path validation
    size_t pos = 0;
    size_t nextPos;
    while ((nextPos = path.find('.', pos)) != std::string::npos) {
        std::string segment = path.substr(pos, nextPos - pos);
        if (segment.empty() || segment.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") == std::string::npos) {
            return false;
        }
        pos = nextPos + 1;
    }
    std::string segment = path.substr(pos);
    if (segment.empty() || segment.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") == std::string::npos) {
        return false;
    }
    return true;
}

std::string EnhancedConfig::normalizePath(const std::string& path) const {
    std::string normalized = path;
    // Remove leading and trailing dots
    while (!normalized.empty() && normalized.front() == '.') {
        normalized.erase(0, 1);
    }
    while (!normalized.empty() && normalized.back() == '.') {
        normalized.pop_back();
    }
    return normalized;
}

std::string EnhancedConfig::formatValue(const EnhancedConfigValue& value) const {
    return ConfigTypeUtils::enhancedValueToString(value);
}

// Parameter management implementation
bool EnhancedConfig::addParameter(const std::string& key, const EnhancedConfigValue& value, 
                                 const ParameterMetadata& metadata) {
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot add parameter with empty key");
        return false;
    }
    
    // Check if parameter already exists
    if (has(key)) {
        NLM_LOG_WARNING("Parameter already exists: " + key);
        return false;
    }
    
    // Set the parameter
    pImpl->enhancedParameters[key] = value;
    
    // Add metadata if provided
    if (!metadata.key.empty()) {
        ParameterMetadata meta = metadata;
        if (meta.key.empty()) {
            meta.key = key;
        }
        pImpl->parameterMetadata.push_back(meta);
    }
    
    return true;
}

bool EnhancedConfig::updateParameter(const std::string& key, const EnhancedConfigValue& value,
                                    ConfigSource source, const std::string& reason) {
    if (!has(key)) {
        NLM_LOG_ERROR("Parameter not found: " + key);
        return false;
    }
    
    // Check if parameter is read-only
    for (const auto& metadata : pImpl->parameterMetadata) {
        if (metadata.key == key && metadata.readOnly) {
            NLM_LOG_WARNING("Cannot modify read-only parameter: " + key);
            return false;
        }
    }
    
    EnhancedConfigValue oldValue;
    auto it = pImpl->enhancedParameters.find(key);
    if (it != pImpl->enhancedParameters.end()) {
        oldValue = it->second;
    }
    
    // Validate parameter
    ParameterValidationResult validation = validateParameter(key, value);
    if (!validation.isValid) {
        NLM_LOG_ERROR("Parameter validation failed for key " + key + ": " + validation.errorMessage);
        return false;
    }
    
    // Update parameter
    pImpl->enhancedParameters[key] = value;
    
    // Notify change
    ParameterChangeNotification notification(key, oldValue, value, source, reason);
    notifyChange(notification);
    
    return true;
}

ParameterMetadata EnhancedConfig::getParameterMetadata(const std::string& key) const {
    for (const auto& metadata : pImpl->parameterMetadata) {
        if (metadata.key == key) {
            return metadata;
        }
    }
    
    ParameterMetadata emptyMetadata;
    emptyMetadata.key = key;
    return emptyMetadata;
}

std::vector<ParameterMetadata> EnhancedConfig::getAllParameterMetadata() const {
    return pImpl->parameterMetadata;
}

// Parameter validation implementation
ParameterValidationResult EnhancedConfig::validateParameter(const std::string& key, const EnhancedConfigValue& value) const {
    ParameterValidationResult result;
    
    // Get parameter metadata
    ParameterMetadata metadata;
    for (const auto& meta : pImpl->parameterMetadata) {
        if (meta.key == key) {
            metadata = meta;
            break;
        }
    }
    
    // Validate against metadata
    if (!metadata.key.empty()) {
        if (metadata.required && !has(key)) {
            result.isValid = false;
            result.errorMessage = "Required parameter missing: " + key;
            return result;
        }
        
        if (metadata.readOnly) {
            result.isValid = false;
            result.errorMessage = "Read-only parameter cannot be modified: " + key;
            return result;
        }
    }
    
    // Validate type
    if (metadata.defaultValue.index() != value.index()) {
        result.isValid = false;
        result.errorMessage = "Type mismatch for parameter " + key + ": expected " + 
                            ConfigTypeUtils::getTypeName(metadata.defaultValue) + ", got " + 
                            ConfigTypeUtils::getTypeName(value);
        return result;
    }
    
    // Validate range if metadata has range information
    result = ParameterValidation::validateRange(value, metadata.minValue, metadata.maxValue);
    if (!result.isValid) {
        result.errorMessage = "Range validation failed for parameter " + key + ": " + result.errorMessage;
        return result;
    }
    
    // Validate enum if metadata has allowed values
    if (!metadata.allowedValues.empty()) {
        result = ParameterValidation::validateEnum(value, metadata.allowedValues);
        if (!result.isValid) {
            result.errorMessage = "Enum validation failed for parameter " + key + ": " + result.errorMessage;
            return result;
        }
    }
    
    // Validate dependencies
    if (!validateParameterDependencies(key)) {
        result.isValid = false;
        result.errorMessage = "Parameter dependency validation failed for: " + key;
        return result;
    }
    
    return result;
}

ParameterValidationResult EnhancedConfig::validateAllParameters() const {
    ParameterValidationResult result;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    for (const auto& key : getKeys()) {
        auto it = pImpl->enhancedParameters.find(key);
        if (it != pImpl->enhancedParameters.end()) {
            ParameterValidationResult paramResult = validateParameter(key, it->second);
            if (!paramResult.isValid) {
                errors.push_back("Parameter " + key + ": " + paramResult.errorMessage);
                for (const auto& warning : paramResult.warnings) {
                    warnings.push_back("Parameter " + key + ": " + warning);
                }
            }
        }
    }
    
    if (!errors.empty()) {
        result.isValid = false;
        for (const auto& error : errors) {
            result.errorMessage += error + "; ";
        }
        for (const auto& warning : warnings) {
            result.warnings.push_back(warning);
        }
    }
    
    return result;
}

bool EnhancedConfig::validateParameterDependencies(const std::string& key) const {
    for (const auto& dependency : pImpl->dependencies) {
        if (dependency.dependentKey == key) {
            auto it = pImpl->enhancedParameters.find(dependency.requiredKey);
            if (it == pImpl->enhancedParameters.end()) {
                NLM_LOG_WARNING("Required parameter not found for dependency: " + dependency.requiredKey + " (required by " + dependency.dependentKey + ")");
                return false;
            }
            
            // Evaluate dependency rule
            const EnhancedConfigValue& dependentValue = it->second;
            const EnhancedConfigValue& requiredValue = pImpl->enhancedParameters.at(dependency.requiredKey);
            
            if (!dependency.dependencyRule(dependentValue, requiredValue)) {
                NLM_LOG_WARNING("Parameter dependency rule failed for: " + dependency.dependentKey + " -> " + dependency.requiredKey);
                return false;
            }
        }
    }
    return true;
}

std::vector<std::string> EnhancedConfig::getParameterValidationErrors() const {
    std::vector<std::string> errors;
    ParameterValidationResult result = validateAllParameters();
    if (!result.isValid) {
        errors.push_back(result.errorMessage);
    }
    return errors;
}

// Dependency management implementation
bool EnhancedConfig::addParameterDependency(const ParameterDependency& dependency) {
    // Validate dependency
    if (dependency.dependentKey.empty() || dependency.requiredKey.empty()) {
        NLM_LOG_ERROR("Invalid dependency: missing key names");
        return false;
    }
    
    // Check if keys exist
    if (!has(dependency.dependentKey) || !has(dependency.requiredKey)) {
        NLM_LOG_WARNING("Dependency keys not found: " + dependency.dependentKey + " -> " + dependency.requiredKey);
        return false;
    }
    
    // Check for circular dependencies
    if (checkForCircularDependency(dependency.dependentKey, dependency.requiredKey)) {
        NLM_LOG_ERROR("Circular dependency detected");
        return false;
    }
    
    pImpl->dependencies.push_back(dependency);
    return true;
}

bool EnhancedConfig::removeParameterDependency(const std::string& dependentKey, const std::string& requiredKey) {
    auto it = std::remove_if(pImpl->dependencies.begin(), pImpl->dependencies.end(),
                           [&](const ParameterDependency& dep) {
                               return dep.dependentKey == dependentKey && dep.requiredKey == requiredKey;
                           });
    
    if (it != pImpl->dependencies.end()) {
        pImpl->dependencies.erase(it, pImpl->dependencies.end());
        return true;
    }
    return false;
}

std::vector<ParameterDependency> EnhancedConfig::getParameterDependencies(const std::string& key) const {
    std::vector<ParameterDependency> dependencies;
    for (const auto& dep : pImpl->dependencies) {
        if (dep.dependentKey == key) {
            dependencies.push_back(dep);
        }
    }
    return dependencies;
}

std::vector<ParameterDependency> EnhancedConfig::getAllDependencies() const {
    return pImpl->dependencies;
}

// Helper method to check for circular dependencies
bool EnhancedConfig::Impl::checkForCircularDependency(const std::string& dependentKey, const std::string& requiredKey) {
    // Simple check for immediate circular dependency
    if (dependentKey == requiredKey) {
        return true;
    }
    
    // Check for longer circular dependencies
    for (const auto& dep : dependencies) {
        if (dep.dependentKey == dependentKey) {
            if (dep.requiredKey == requiredKey) {
                return true;
            }
        }
    }
    
    return false;
}

// Continue with the rest of the implementation...

} // namespace nlm