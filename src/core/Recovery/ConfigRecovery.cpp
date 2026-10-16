// Configuration validation and recovery implementation
#include "ConfigRecovery.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <sstream>

namespace nlm {

// ConfigValidator implementation
void ConfigValidator::addRule(const std::string& field,
                             std::function<bool(const ConfigValue&)> validator,
                             ValidationSeverity severity,
                             const std::string& errorMessage,
                             const std::string& errorCode,
                             const std::string& suggestedAction) {
    ValidationRule rule;
    rule.field = field;
    rule.validator = validator;
    rule.severity = severity;
    rule.errorMessage = errorMessage;
    rule.errorCode = errorCode;
    rule.suggestedAction = suggestedAction;
    rules_.push_back(rule);
}

void ConfigValidator::addRangeRule(const std::string& field, double min, double max,
                                   ValidationSeverity severity,
                                   const std::string& errorMessage,
                                   const std::string& errorCode,
                                   const std::string& suggestedAction) {
    addRule(field, [min, max](const ConfigValue& val) -> bool {
        if (std::holds_alternative<int>(val)) return std::get<int>(val) >= min && std::get<int>(val) <= max;
        if (std::holds_alternative<int64_t>(val)) return std::get<int64_t>(val) >= min && std::get<int64_t>(val) <= max;
        if (std::holds_alternative<double>(val)) return std::get<double>(val) >= min && std::get<double>(val) <= max;
        return false;
    }, severity, errorMessage, errorCode, suggestedAction);
}

void ConfigValidator::addEnumRule(const std::string& field,
                                   const std::vector<std::string>& validValues,
                                   ValidationSeverity severity,
                                   const std::string& errorMessage,
                                   const std::string& errorCode,
                                   const std::string& suggestedAction) {
    addRule(field, [validValues](const ConfigValue& val) -> bool {
        if (!std::holds_alternative<std::string>(val)) return false;
        return std::find(validValues.begin(), validValues.end(), std::get<std::string>(val)) != validValues.end();
    }, severity, errorMessage, errorCode, suggestedAction);
}

void ConfigValidator::addCustomRule(const std::string& field,
                                   std::function<bool(const ConfigValue&, const std::string&, const Config&)> validator,
                                   ValidationSeverity severity,
                                   const std::string& errorMessage,
                                   const std::string& errorCode,
                                   const std::string& suggestedAction) {
    ValidationRule rule;
    rule.field = field;
    rule.validator = [validator, field](const ConfigValue& val) -> bool {
        Config dummyConfig;
        return validator(val, field, dummyConfig);
    };
    rule.severity = severity;
    rule.errorMessage = errorMessage;
    rule.errorCode = errorCode;
    rule.suggestedAction = suggestedAction;
    rules_.push_back(rule);
}

std::vector<ValidationError> ConfigValidator::validate(const Config& config) const {
    std::vector<ValidationError> errors;
    
    for (const auto& rule : rules_) {
        auto valueOpt = config.get<std::string>(rule.field);
        if (!valueOpt.has_value()) {
            // Check if field is required (by trying to get different types)
            bool hasValue = false;
            hasValue |= config.get<int>(rule.field).has_value();
            hasValue |= config.get<int64_t>(rule.field).has_value();
            hasValue |= config.get<double>(rule.field).has_value();
            hasValue |= config.get<bool>(rule.field).has_value();
            hasValue |= config.get<std::string>(rule.field).has_value();
            
            if (!hasValue) {
                ValidationError error;
                error.field = rule.field;
                error.message = rule.errorMessage.empty() ? "Required field is missing" : rule.errorMessage;
                error.severity = ValidationSeverity::Error;
                error.errorCode = rule.errorCode.empty() ? "CFG_MISSING_FIELD" : rule.errorCode;
                error.suggestedAction = rule.suggestedAction.empty() ? "Add required field" : rule.suggestedAction;
                errors.push_back(error);
            }
            continue;
        }
        
        try {
            ConfigValue value;
            if (std::holds_alternative<std::string>(valueOpt.value())) {
                value = ConfigValue(std::get<std::string>(valueOpt.value()));
            } else {
                value = ConfigValue(*valueOpt);
            }
            
            if (!rule.validator(value)) {
                ValidationError error;
                error.field = rule.field;
                error.message = rule.errorMessage.empty() ? "Invalid value" : rule.errorMessage;
                error.severity = rule.severity;
                error.errorCode = rule.errorCode.empty() ? "CFG_INVALID_VALUE" : rule.errorCode;
                error.suggestedAction = rule.suggestedAction.empty() ? "Fix value" : rule.suggestedAction;
                errors.push_back(error);
            }
        } catch (const std::bad_variant_access&) {
            ValidationError error;
            error.field = rule.field;
            error.message = "Invalid value type for field";
            error.severity = ValidationSeverity::Error;
            error.errorCode = "CFG_TYPE_MISMATCH";
            error.suggestedAction = "Use correct data type for field";
            errors.push_back(error);
        }
    }
    
    return errors;
}

std::vector<ValidationError> ConfigValidator::validateField(const Config& config, const std::string& field) const {
    std::vector<ValidationError> errors;
    
    for (const auto& rule : rules_) {
        if (rule.field == field) {
            auto valueOpt = config.get<std::string>(rule.field);
            if (!valueOpt.has_value()) {
                // Field has no value
                continue;
            }
            
            try {
                ConfigValue value;
                if (std::holds_alternative<std::string>(valueOpt.value())) {
                    value = ConfigValue(std::get<std::string>(valueOpt.value()));
                } else {
                    value = ConfigValue(*valueOpt);
                }
                
                if (!rule.validator(value)) {
                    ValidationError error;
                    error.field = rule.field;
                    error.message = rule.errorMessage.empty() ? "Invalid value" : rule.errorMessage;
                    error.severity = rule.severity;
                    error.errorCode = rule.errorCode.empty() ? "CFG_INVALID_VALUE" : rule.errorCode;
                    error.suggestedAction = rule.suggestedAction.empty() ? "Fix value" : rule.suggestedAction;
                    errors.push_back(error);
                }
            } catch (const std::bad_variant_access&) {
                ValidationError error;
                error.field = rule.field;
                error.message = "Invalid value type for field";
                error.severity = ValidationSeverity::Error;
                error.errorCode = "CFG_TYPE_MISMATCH";
                error.suggestedAction = "Use correct data type for field";
                errors.push_back(error);
            }
            break;
        }
    }
    
    return errors;
}

bool ConfigValidator::isValid(const Config& config) const {
    return validate(config).empty();
}

std::vector<ValidationError> ConfigValidator::getErrors(const Config& config) const {
    std::vector<ValidationError> allErrors = validate(config);
    std::vector<ValidationError> errors;
    for (const auto& error : allErrors) {
        if (error.severity == ValidationSeverity::Error || error.severity == ValidationSeverity::Critical) {
            errors.push_back(error);
        }
    }
    return errors;
}

std::vector<ValidationError> ConfigValidator::getCriticalErrors(const Config& config) const {
    std::vector<ValidationError> allErrors = validate(config);
    std::vector<ValidationError> criticalErrors;
    for (const auto& error : allErrors) {
        if (error.severity == ValidationSeverity::Critical) {
            criticalErrors.push_back(error);
        }
    }
    return criticalErrors;
}

std::vector<ValidationError> ConfigValidator::getWarnings(const Config& config) const {
    std::vector<ValidationError> allErrors = validate(config);
    std::vector<ValidationError> warnings;
    for (const auto& error : allErrors) {
        if (error.severity == ValidationSeverity::Warning) {
            warnings.push_back(error);
        }
    }
    return warnings;
}

// ConfigRecovery implementation
void ConfigRecovery::addRecoveryStrategy(std::function<bool(Config&)> strategy) {
    recoveryStrategies_.push_back(strategy);
}

void ConfigRecovery::setAutoRecoveryEnabled(bool enabled) {
    enableAutoRecovery_ = enabled;
}

void ConfigRecovery::setGracefulDegradationEnabled(bool enabled) {
    enableGracefulDegradation_ = enabled;
}

bool ConfigRecovery::validateAndRecover(Config& config) const {
    // First validate the configuration
    auto errors = validator_->getErrors(config);
    
    if (errors.empty()) {
        return true; // Already valid
    }
    
    if (!enableAutoRecovery_) {
        return false; // Auto-recovery disabled
    }
    
    // Apply recovery strategies
    return attemptRecovery(config);
}

void ConfigRecovery::sanitize(Config& config) const {
    // Remove invalid fields or apply safe defaults
    auto errors = validator_->getErrors(config);
    
    for (const auto& error : errors) {
        if (error.severity == ValidationSeverity::Error || error.severity == ValidationSeverity::Critical) {
            // Try to fix the error
            fixInvalidValue(config, error.field);
        }
    }
}

void ConfigRecovery::applySafeDefaults(Config& config) const {
    // Apply safe defaults for all known configuration parameters
    config.set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
    config.set("region_count", static_cast<int64_t>(4), ConfigSource::Runtime);
    config.set("connection_probability", 0.1f, ConfigSource::Runtime);
    config.set("stdp_learning_rate", 0.01f, ConfigSource::Runtime);
    config.set("hebbian_learning_rate", 0.01f, ConfigSource::Runtime);
    config.set("structural_plasticity_rate", 0.001f, ConfigSource::Runtime);
    config.set("working_memory_capacity", static_cast<int64_t>(1000), ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001f, ConfigSource::Runtime);
    config.set("error_threshold", static_cast<int64_t>(ErrorSeverity::Warning), ConfigSource::Runtime);
    config.set("enable_auto_recovery", true, ConfigSource::Runtime);
}

void ConfigRecovery::fixInvalidValues(Config& config) const {
    auto errors = validator_->getErrors(config);
    
    for (const auto& error : errors) {
        fixInvalidValue(config, error.field);
    }
}

void ConfigRecovery::fixInvalidValue(Config& config, const std::string& field) const {
    // Try to fix common invalid values
    if (field == "neuron_count") {
        auto valueOpt = config.get<int64_t>(field);
        if (valueOpt.has_value()) {
            int64_t value = *valueOpt;
            if (value < 10) {
                config.set(field, static_cast<int64_t>(10), ConfigSource::Runtime);
            } else if (value > 10000) {
                config.set(field, static_cast<int64_t>(10000), ConfigSource::Runtime);
            }
        }
    } else if (field == "region_count") {
        auto valueOpt = config.get<int64_t>(field);
        if (valueOpt.has_value()) {
            int64_t value = *valueOpt;
            if (value < 1) {
                config.set(field, static_cast<int64_t>(1), ConfigSource::Runtime);
            } else if (value > 100) {
                config.set(field, static_cast<int64_t>(100), ConfigSource::Runtime);
            }
        }
    } else if (field == "connection_probability") {
        auto valueOpt = config.get<double>(field);
        if (valueOpt.has_value()) {
            double value = *valueOpt;
            if (value < 0.0f) {
                config.set(field, 0.0f, ConfigSource::Runtime);
            } else if (value > 1.0f) {
                config.set(field, 1.0f, ConfigSource::Runtime);
            }
        }
    } else if (field == "stdp_learning_rate") {
        auto valueOpt = config.get<double>(field);
        if (valueOpt.has_value()) {
            double value = *valueOpt;
            if (value < 0.0f) {
                config.set(field, 0.0f, ConfigSource::Runtime);
            } else if (value > 1.0f) {
                config.set(field, 1.0f, ConfigSource::Runtime);
            }
        }
    }
}

std::vector<ValidationError> ConfigRecovery::getValidationErrors(const Config& config) const {
    return validator_->getErrors(config);
}

void ConfigRecovery::initDefaultRules() {
    // Add default validation rules
    validator_->addRangeRule("neuron_count", 10, 10000, ValidationSeverity::Error,
                           "Neuron count must be between 10 and 10000",
                           "CFG_NEURON_COUNT_OUT_OF_RANGE",
                           "Adjust neuron count to within valid range");
    
    validator_->addRangeRule("region_count", 1, 100, ValidationSeverity::Error,
                           "Region count must be between 1 and 100",
                           "CFG_REGION_COUNT_OUT_OF_RANGE",
                           "Adjust region count to within valid range");
    
    validator_->addRangeRule("connection_probability", 0.0f, 1.0f, ValidationSeverity::Error,
                           "Connection probability must be between 0.0 and 1.0",
                           "CFG_CONNECTION_PROBABILITY_OUT_OF_RANGE",
                           "Adjust connection probability to within valid range");
    
    validator_->addRangeRule("stdp_learning_rate", 0.0f, 1.0f, ValidationSeverity::Warning,
                           "STDP learning rate should be between 0.0 and 1.0",
                           "CFG_STDP_LEARNING_RATE_WARNING",
                           "Consider adjusting STDP learning rate");
    
    validator_->addRangeRule("hebbian_learning_rate", 0.0f, 1.0f, ValidationSeverity::Warning,
                           "Hebbian learning rate should be between 0.0 and 1.0",
                           "CFG_HEBBIAN_LEARNING_RATE_WARNING",
                           "Consider adjusting Hebbian learning rate");
    
    validator_->addRangeRule("working_memory_capacity", 100, 100000, ValidationSeverity::Error,
                           "Working memory capacity must be between 100 and 100000",
                           "CFG_MEMORY_CAPACITY_OUT_OF_RANGE",
                           "Adjust memory capacity to within valid range");
    
    validator_->addRangeRule("simulation_timestep", 0.0001f, 1.0f, ValidationSeverity::Error,
                           "Simulation timestep must be between 0.0001 and 1.0",
                           "CFG_TIMESTEP_OUT_OF_RANGE",
                           "Adjust simulation timestep to within valid range");
}

bool ConfigRecovery::attemptRecovery(Config& config) const {
    for (const auto& strategy : recoveryStrategies_) {
        if (strategy(config)) {
            return true;
        }
    }
    
    // Last resort: apply safe defaults
    applySafeDefaults(config);
    return true;
}

// SystemCheckpoint implementation
void SystemCheckpoint::setCheckpointPath(const std::string& path) {
    checkpointPath_ = path;
    std::filesystem::create_directories(path);
}

void SystemCheckpoint::setEnableCheckpointing(bool enabled) {
    enableCheckpointing_ = enabled;
}

void SystemCheckpoint::setMaxCheckpointHistory(size_t history) {
    maxCheckpointHistory_ = history;
}

bool SystemCheckpoint::checkpoint(const std::string& filepath, const Config& config) const {
    if (!enableCheckpointing_) {
        return false;
    }
    
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Write configuration
        file << config.summary();
        
        // Write additional metadata
        file << "checkpoint_time=" << std::chrono::system_clock::to_time_t(lastCheckpoint_) << std::endl;
        file << "version=1" << std::endl;
        
        file.close();
        lastCheckpoint_ = std::chrono::system_clock::now();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool SystemCheckpoint::restore(const std::string& filepath, Config& config) const {
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Parse checkpoint data
            if (line.find("neuron_count=") != std::string::npos) {
                size_t pos = line.find("=") + 1;
                std::string value = line.substr(pos);
                config.set("neuron_count", static_cast<int64_t>(std::stoll(value)), ConfigSource::Runtime);
            } else if (line.find("region_count=") != std::string::npos) {
                size_t pos = line.find("=") + 1;
                std::string value = line.substr(pos);
                config.set("region_count", static_cast<int64_t>(std::stoll(value)), ConfigSource::Runtime);
            }
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::string> SystemCheckpoint::listCheckpoints(const std::string& directory) const {
    std::vector<std::string> checkpoints;
    
    if (!std::filesystem::exists(directory)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".chk") {
            checkpoints.push_back(entry.path().string());
        }
    }
    
    return checkpoints;
}

bool SystemCheckpoint::cleanupOldCheckpoints(const std::string& directory) const {
    if (!std::filesystem::exists(directory)) {
        return false;
    }
    
    auto checkpoints = listCheckpoints(directory);
    if (checkpoints.size() <= maxCheckpointHistory_) {
        return true; // No cleanup needed
    }
    
    // Sort by modification time
    std::vector<std::pair<std::string, std::chrono::system_clock::time_point>> datedCheckpoints;
    for (const auto& checkpoint : checkpoints) {
        auto fileTime = std::filesystem::last_write_time(std::filesystem::path(checkpoint));
        datedCheckpoints.push_back({checkpoint, fileTime});
    }
    
    // Remove oldest checkpoints
    std::sort(datedCheckpoints.begin(), datedCheckpoints.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    for (size_t i = 0; i < checkpoints.size() - maxCheckpointHistory_; ++i) {
        std::filesystem::remove(datedCheckpoints[i].first);
    }
    
    return true;
}

std::chrono::system_clock::time_point SystemCheckpoint::getLastCheckpointTime() const {
    return lastCheckpoint_;
}

void SystemCheckpoint::setLastCheckpointTime(const std::chrono::system_clock::time_point& time) {
    lastCheckpoint_ = time;
}

// SystemRecoveryManager implementation
void SystemRecoveryManager::setAutomaticRecoveryEnabled(bool enabled) {
    enableAutomaticRecovery_ = enabled;
}

void SystemRecoveryManager::setGracefulDegradationEnabled(bool enabled) {
    enableGracefulDegradation_ = enabled;
}

void SystemRecoveryManager::setCheckpointingEnabled(bool enabled) {
    enableCheckpointing_ = enabled;
}

void SystemRecoveryManager::setMemoryMonitoringEnabled(bool enabled) {
    enableMemoryMonitoring_ = enabled;
}

void SystemRecoveryManager::setResourceMonitoringEnabled(bool enabled) {
    enableResourceMonitoring_ = enabled;
}

bool SystemRecoveryManager::recoverFromError(const std::exception& error, Config& config) {
    if (!enableAutomaticRecovery_) {
        return false;
    }
    
    try {
        // Handle the error
        getGlobalErrorHandler().handle(error);
        
        // Attempt to recover
        bool recoverySuccess = configRecovery_->validateAndRecover(config);
        
        if (recoverySuccess) {
            // Attempt memory recovery
            if (enableMemoryMonitoring_) {
                memoryRecovery_->cleanupMemory();
            }
            
            // Attempt resource recovery
            if (enableResourceMonitoring_) {
                resourceRecovery_->cleanupResources();
            }
            
            return true;
        }
        
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

bool SystemRecoveryManager::recoverSystem(Config& config) {
    bool allSuccess = true;
    
    // Recover configuration
    allSuccess &= recoverConfiguration(config);
    
    // Recover state
    allSuccess &= recoverState(config);
    
    // Recover resources
    allSuccess &= recoverResources(config);
    
    // Recover memory
    allSuccess &= recoverMemory(config);
    
    return allSuccess;
}

bool SystemRecoveryManager::recoverConfiguration(Config& config) {
    return configRecovery_->validateAndRecover(config);
}

bool SystemRecoveryManager::recoverState(Config& config) {
    // Restore from checkpoint if available
    auto checkpoints = checkpoint_->listCheckpoints(checkpoint_->getCheckpointPath());
    if (!checkpoints.empty()) {
        // Use the most recent checkpoint
        return checkpoint_->restore(checkpoints.back(), config);
    }
    return true; // Nothing to recover
}

bool SystemRecoveryManager::recoverResources(Config& config) {
    if (enableResourceMonitoring_) {
        resourceRecovery_->cleanupResources();
        return true;
    }
    return true;
}

bool SystemRecoveryManager::recoverMemory(Config& config) {
    if (enableMemoryMonitoring_) {
        memoryRecovery_->cleanupMemory();
        return true;
    }
    return true;
}

bool SystemRecoveryManager::mitigateError(const std::exception& error, Config& config) {
    if (errorMitigation_) {
        return errorMitigation_->mitigateError(error);
    }
    return false;
}

bool SystemRecoveryManager::isSystemHealthy(Config& config) const {
    // Check configuration validity
    if (!configRecovery_->getValidationErrors(config).empty()) {
        return false;
    }
    
    // Check memory health
    if (enableMemoryMonitoring_ && memoryRecovery_->isMemoryUsageExceeded()) {
        return false;
    }
    
    // Check resource health
    if (enableResourceMonitoring_ && resourceRecovery_->isResourceUsageExceeded()) {
        return false;
    }
    
    return true;
}

SystemRecoveryManager::RecoveryStats SystemRecoveryManager::getRecoveryStats() const {
    RecoveryStats stats;
    // Implementation would collect statistics from components
    return stats;
}

std::string SystemRecoveryManager::generateRecoveryReport() const {
    std::ostringstream oss;
    oss << "=== System Recovery Report ===" << std::endl;
    oss << "Configuration Recovery: " << (configRecovery_->getValidationErrors(Config()).empty() ? "OK" : "ISSUES") << std::endl;
    oss << "Memory Recovery: " << (enableMemoryMonitoring_ ? "ENABLED" : "DISABLED") << std::endl;
    oss << "Resource Recovery: " << (enableResourceMonitoring_ ? "ENABLED" : "DISABLED") << std::endl;
    oss << "Checkpointing: " << (enableCheckpointing_ ? "ENABLED" : "DISABLED") << std::endl;
    return oss.str();
}

} // namespace nlm
