#include "EnhancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <unordered_set>
#include <numeric>

namespace nlm {

struct EnhancedConfigManager::Impl {
    std::shared_ptr<Config> config;
    ConfigSchema schema;
    bool runtimeUpdatesEnabled = false;
    std::function<bool(const std::string&, const ConfigValue&)> customValidator;
    std::chrono::system_clock::time_point lastModified;
    std::string lastSource;
};

EnhancedConfigManager::EnhancedConfigManager() : pImpl(std::make_unique<Impl>()) {
    initializeDefaultSchema();
    pImpl->config = std::make_shared<Config>();
}

EnhancedConfigManager::~EnhancedConfigManager() = default;

void EnhancedConfigManager::initializeDefaultSchema() {
    pImpl->schema = ConfigSchemas::getStandardSchema();
}

bool EnhancedConfigManager::loadConfig(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_ERROR("Configuration file not found: " + filepath);
        return false;
    }
    
    // Clear existing config
    pImpl->config->clear();
    
    // Load using existing config system
    if (!pImpl->config->loadFromFile(filepath)) {
        NLM_LOG_ERROR("Failed to load configuration from file: " + filepath);
        return false;
    }
    
    pImpl->lastSource = filepath;
    pImpl->lastModified = std::chrono::system_clock::now();
    
    // Validate the loaded configuration
    auto errors = validate();
    if (!errors.empty()) {
        NLM_LOG_WARN("Configuration loaded with validation errors:");
        for (const auto& error : errors) {
            NLM_LOG_WARN("  " + error);
        }
        return false;
    }
    
    NLM_LOG_INFO("Configuration loaded successfully from: " + filepath);
    return true;
}

bool EnhancedConfigManager::applyPreset(const std::string& presetName) {
    for (const auto& preset : pImpl->schema.presets) {
        if (preset.name == presetName) {
            return applyPreset(preset);
        }
    }
    
    NLM_LOG_ERROR("Preset not found: " + presetName);
    return false;
}

bool EnhancedConfigManager::applyPreset(const PresetConfig& preset) {
    for (const auto& [key, value] : preset.values) {
        pImpl->config->set(key, value, ConfigSource::Runtime);
    }
    
    NLM_LOG_INFO("Applied preset: " + preset.name);
    return true;
}

std::vector<std::string> EnhancedConfigManager::getAvailablePresets() const {
    std::vector<std::string> presetNames;
    for (const auto& preset : pImpl->schema.presets) {
        presetNames.push_back(preset.name);
    }
    return presetNames;
}

std::vector<PresetConfig> EnhancedConfigManager::getPresetsByTag(const std::string& tag) const {
    std::vector<PresetConfig> matchingPresets;
    for (const auto& preset : pImpl->schema.presets) {
        if (std::find(preset.tags.begin(), preset.tags.end(), tag) != preset.tags.end()) {
            matchingPresets.push_back(preset);
        }
    }
    return matchingPresets;
}

std::vector<std::string> EnhancedConfigManager::validate() const {
    std::vector<std::string> errors;
    
    // Apply schema validation
    applyValidationRules(pImpl->schema);
    
    // Get basic validation errors from config
    for (const auto& rule : pImpl->schema.validationRules) {
        auto value = pImpl->config->get<std::string>(rule.key);
        if (!value && rule.required) {
            errors.push_back("Required configuration key missing: " + rule.key);
            continue;
        }
        
        if (value) {
            // Convert to ConfigValue for validation
            ConfigValue configValue = *value;
            if (!validateValue(rule, configValue)) {
                errors.push_back(getValidationError(rule, configValue));
            }
        }
    }
    
    // Apply custom validator if set
    if (pImpl->customValidator) {
        for (const auto& entry : pImpl->config->getKeys()) {
            auto value = pImpl->config->get<std::string>(entry);
            if (value && !pImpl->customValidator(entry, *value)) {
                errors.push_back("Custom validation failed for key: " + entry);
            }
        }
    }
    
    return errors;
}

bool EnhancedConfigManager::isValid() const {
    return validate().empty();
}

std::vector<std::string> EnhancedConfigManager::getValidationSuggestions() const {
    std::vector<std::string> suggestions;
    auto errors = validate();
    
    for (const auto& error : errors) {
        // Generate suggestions based on error patterns
        if (error.find("Required configuration key missing") != std::string::npos) {
            // Suggest adding missing keys with defaults
            std::string key = error.substr(34); // Remove "Required configuration key missing: "
            auto schemaIt = pImpl->schema.defaults.find(key);
            if (schemaIt != pImpl->schema.defaults.end()) {
                suggestions.push_back("Consider adding default value for '" + key + "': " + 
                                    getConfigValueString(schemaIt->second));
            }
        } else if (error.find("Range") != std::string::npos) {
            suggestions.push_back("Check that value is within acceptable range");
        } else if (error.find("pattern") != std::string::npos) {
            suggestions.push_back("Validate string format matches expected pattern");
        }
    }
    
    return suggestions;
}

void EnhancedConfigManager::enableRuntimeUpdates(bool enable) {
    pImpl->runtimeUpdatesEnabled = enable;
    NLM_LOG_INFO("Runtime updates " + std::string(enable ? "enabled" : "disabled"));
}

bool EnhancedConfigManager::isRuntimeUpdatesEnabled() const {
    return pImpl->runtimeUpdatesEnabled;
}

bool EnhancedConfigManager::backupConfig(const std::string& backupPath) {
    try {
        return pImpl->config->saveToFile(backupPath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to backup configuration: " + std::string(e.what()));
        return false;
    }
}

bool EnhancedConfigManager::restoreConfig(const std::string& backupPath) {
    if (!std::filesystem::exists(backupPath)) {
        NLM_LOG_ERROR("Backup file not found: " + backupPath);
        return false;
    }
    
    return loadConfig(backupPath);
}

std::string EnhancedConfigManager::getDetailedSummary() const {
    std::ostringstream oss;
    
    oss << "=== Enhanced Configuration Summary ===\n";
    oss << "Source: " << (pImpl->lastSource.empty() ? "Default" : pImpl->lastSource) << "\n";
    oss << "Last modified: " << 
        std::chrono::format(std::chrono::current_zone()->to_local(pImpl->lastModified), "%Y-%m-%d %H:%M:%S") << "\n";
    oss << "Runtime updates: " << (pImpl->runtimeUpdatesEnabled ? "Enabled" : "Disabled") << "\n";
    oss << "Valid: " << (isValid() ? "Yes" : "No") << "\n\n";
    
    oss << "=== Configuration Entries ===\n";
    oss << pImpl->config->summary() << "\n";
    
    auto errors = validate();
    if (!errors.empty()) {
        oss << "=== Validation Errors ===\n";
        for (const auto& error : errors) {
            oss << "  ERROR: " << error << "\n";
        }
        oss << "\n=== Validation Suggestions ===\n";
        auto suggestions = getValidationSuggestions();
        for (const auto& suggestion : suggestions) {
            oss << "  SUGGESTION: " << suggestion << "\n";
        }
    }
    
    return oss.str();
}

std::shared_ptr<Config> EnhancedConfigManager::getConfig() const {
    return pImpl->config;
}

void EnhancedConfigManager::setValidationCallback(std::function<bool(const std::string&, const ConfigValue&)> callback) {
    pImpl->customValidator = std::move(callback);
    NLM_LOG_INFO("Custom validation callback set");
}

std::string EnhancedConfigManager::diffWith(const EnhancedConfigManager& other) const {
    std::ostringstream oss;
    oss << "=== Configuration Difference ===\n";
    
    auto thisKeys = pImpl->config->getKeys();
    auto otherKeys = other.pImpl->config->getKeys();
    
    std::unordered_set<std::string> thisSet(thisKeys.begin(), thisKeys.end());
    std::unordered_set<std::string> otherSet(otherKeys.begin(), otherKeys.end());
    
    // Keys only in this config
    oss << "Keys only in current config:\n";
    for (const auto& key : thisSet) {
        if (otherSet.find(key) == otherSet.end()) {
            oss << "  + " << key << "\n";
        }
    }
    
    // Keys only in other config
    oss << "\nKeys only in other config:\n";
    for (const auto& key : otherSet) {
        if (thisSet.find(key) == thisSet.end()) {
            oss << "  - " << key << "\n";
        }
    }
    
    // Common keys with different values
    oss << "\nCommon keys with different values:\n";
    for (const auto& key : thisSet) {
        if (otherSet.find(key) != otherSet.end()) {
            auto thisVal = pImpl->config->get<std::string>(key);
            auto otherVal = other.pImpl->config->get<std::string>(key);
            
            if (thisVal != otherVal) {
                oss << "  " << key << ":\n";
                oss << "    current: " << (thisVal ? *thisVal : "(null)") << "\n";
                oss << "    other:   " << (otherVal ? *otherVal : "(null)") << "\n";
            }
        }
    }
    
    return oss.str();
}

bool EnhancedConfigManager::exportConfig(const std::string& filepath, const std::string& format) const {
    if (format == "text") {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << getDetailedSummary();
        file.close();
        return true;
    } else if (format == "json") {
        // TODO: Implement JSON export
        NLM_LOG_WARN("JSON export not yet implemented");
        return false;
    }
    
    NLM_LOG_ERROR("Unsupported export format: " + format);
    return false;
}

bool EnhancedConfigManager::importConfig(const std::string& filepath, const std::string& format) {
    if (format == "text") {
        return loadConfig(filepath);
    } else if (format == "json") {
        // TODO: Implement JSON import
        NLM_LOG_WARN("JSON import not yet implemented");
        return false;
    }
    
    NLM_LOG_ERROR("Unsupported import format: " + format);
    return false;
}

void EnhancedConfigManager::applyValidationRules(const ConfigSchema& schema) const {
    // This is a placeholder for the validation logic
    // In a real implementation, this would apply the rules and potentially
    // modify the configuration to make it valid
}

bool EnhancedConfigManager::validateValue(const ValidationRule& rule, const ConfigValue& value) const {
    return rule.validator(value);
}

std::string EnhancedConfigManager::getValidationError(const ValidationRule& rule, const ConfigValue& value) const {
    return rule.errorMessage;
}

std::string EnhancedConfigManager::getConfigValueString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

// ConfigSchemas implementation
namespace ConfigSchemas {
    
    ConfigSchema getStandardSchema() {
        ConfigSchema schema;
        schema.name = "Standard";
        schema.description = "Standard NLM configuration with all basic settings";
        
        // Add validation rules
        schema.validationRules = {
            {"neuron_count", [](const ConfigValue& v) { return std::get<int64_t>(v) > 0; },
             "neuron_count must be positive", true, "Basic"},
            
            {"region_count", [](const ConfigValue& v) { return std::get<int64_t>(v) > 0 && std::get<int64_t>(v) <= 10; },
             "region_count must be between 1 and 10", true, "Basic"},
            
            {"connection_probability", [](const ConfigValue& v) { return std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; },
             "connection_probability must be between 0.0 and 1.0", true, "Basic"},
            
            {"random_seed", [](const ConfigValue& v) { return std::get<int64_t>(v) >= 0; },
             "random_seed must be non-negative", false, "Basic"},
            
            {"simulation_timestep", [](const ConfigValue& v) { return std::get<double>(v) > 0.0 && std::get<double>(v) <= 0.1; },
             "simulation_timestep must be between 0.001 and 0.1 seconds", true, "Basic"}
        };
        
        // Add default values
        schema.defaults = {
            {"neuron_count", int64_t(100)},
            {"region_count", int64_t(1)},
            {"connection_probability", 0.15f},
            {"random_seed", int64_t(42)},
            {"simulation_timestep", 0.001},
            {"enable_plasticity", true},
            {"enable_development", false},
            {"enable_checkpoint", false}
        };
        
        // Add categories
        schema.categories = {"Basic", "Advanced", "Development", "Performance", "Debug"};
        
        // Create presets
        schema.presets = {
            ConfigPresets::getQuickStartPreset(),
            ConfigPresets::getMinimalPreset(),
            ConfigPresets::getFullPreset(),
            ConfigPresets::getResearchPreset()
        };
        
        return schema;
    }
    
    ConfigSchema getDevelopmentSchema() {
        ConfigSchema schema = getStandardSchema();
        schema.name = "Development";
        schema.description = "Configuration optimized for development and debugging";
        
        // Add development-specific presets
        schema.presets.push_back(ConfigPresets::getDebugPreset());
        schema.presets.push_back(ConfigPresets::getValidationPreset());
        
        // Add more detailed validation rules
        schema.validationRules.push_back({"log_level", 
            [](const ConfigValue& v) { 
                std::string level = std::get<std::string>(v);
                return level == "DEBUG" || level == "INFO" || level == "WARN" || level == "ERROR";
            },
            "log_level must be DEBUG, INFO, WARN, or ERROR", true, "Development"});
        
        return schema;
    }
    
    ConfigSchema getProductionSchema() {
        ConfigSchema schema = getStandardSchema();
        schema.name = "Production";
        schema.description = "Configuration optimized for production use";
        
        // Add production-specific presets
        schema.presets.push_back(ConfigPresets::getPerformancePreset());
        schema.presets.push_back(ConfigPresets::getMemoryEfficientPreset());
        
        return schema;
    }
    
    ConfigSchema getPerformanceSchema() {
        ConfigSchema schema = getStandardSchema();
        schema.name = "Performance";
        schema.description = "Configuration optimized for performance testing";
        
        // Add performance-specific presets
        schema.presets.push_back(ConfigPresets::getSpeedOptimizedPreset());
        
        // Add performance validation rules
        schema.validationRules.push_back({"experiment_steps", 
            [](const ConfigValue& v) { return std::get<int64_t>(v) > 0; },
            "experiment_steps must be positive", true, "Performance"});
        
        return schema;
    }
    
    ConfigSchema getResearchSchema() {
        ConfigSchema schema = getStandardSchema();
        schema.name = "Research";
        schema.description = "Configuration optimized for research experiments";
        
        // Add research-specific presets
        schema.presets.push_back(ConfigPresets::getLongRunningPreset());
        schema.presets.push_back(ConfigPresets::getHighPrecisionPreset());
        schema.presets.push_back(ConfigPresets::getLargeScalePreset());
        
        return schema;
    }
}

// ConfigPresets implementation
namespace ConfigPresets {
    
    PresetConfig getQuickStartPreset() {
        PresetConfig preset;
        preset.name = "quick-start";
        preset.description = "Quick start configuration with minimal settings";
        preset.values = {
            {"neuron_count", int64_t(50)},
            {"region_count", int64_t(1)},
            {"connection_probability", 0.1f},
            {"enable_plasticity", true}
        };
        preset.tags = {"quick", "minimal", "beginner"};
        return preset;
    }
    
    PresetConfig getMinimalPreset() {
        PresetConfig preset;
        preset.name = "minimal";
        preset.description = "Minimal configuration with only essential settings";
        preset.values = {
            {"neuron_count", int64_t(10)},
            {"region_count", int64_t(1)},
            {"connection_probability", 0.05f},
            {"enable_plasticity", false},
            {"enable_development", false},
            {"enable_checkpoint", false}
        };
        preset.tags = {"minimal", "debug", "testing"};
        return preset;
    }
    
    PresetConfig getFullPreset() {
        PresetConfig preset;
        preset.name = "full";
        preset.description = "Full configuration with all advanced features enabled";
        preset.values = {
            {"neuron_count", int64_t(500)},
            {"region_count", int64_t(4)},
            {"connection_probability", 0.2f},
            {"stdp_ltp_weight", 0.02f},
            {"stdp_ltd_weight", 0.015f},
            {"synaptogenesis_rate", 0.0001f},
            {"pruning_rate", 0.00001f},
            {"enable_plasticity", true},
            {"enable_development", true},
            {"enable_checkpoint", true},
            {"enable_reward_modulation", true},
            {"enable_curiosity", true},
            {"log_level", std::string("INFO")}
        };
        preset.tags = {"full", "advanced", "production"};
        return preset;
    }
    
    PresetConfig getResearchPreset() {
        PresetConfig preset;
        preset.name = "research";
        preset.description = "Configuration optimized for research experiments";
        preset.values = {
            {"neuron_count", int64_t(1000)},
            {"region_count", int64_t(8)},
            {"connection_probability", 0.15f},
            {"random_seed", int64_t(12345)},
            {"enable_plasticity", true},
            {"enable_development", true},
            {"enable_checkpoint", true},
            {"enable_reward_modulation", true},
            {"enable_curiosity", true},
            {"log_level", std::string("DEBUG")},
            {"experiment_steps", int64_t(10000)}
        };
        preset.tags = {"research", "long-running", "detailed"};
        return preset;
    }
    
    PresetConfig getPerformancePreset() {
        PresetConfig preset;
        preset.name = "performance";
        preset.description = "Configuration optimized for performance testing";
        preset.values = {
            {"neuron_count", int64_t(2000)},
            {"region_count", int64_t(16)},
            {"connection_probability", 0.25f},
            {"stdp_ltp_weight", 0.01f},
            {"stdp_ltd_weight", 0.008f},
            {"enable_plasticity", false},  // Disable for speed
            {"enable_development", false},  // Disable for speed
            {"enable_checkpoint", false},
            {"enable_reward_modulation", false},
            {"enable_curiosity", false},
            {"log_level", std::string("ERROR")}  // Minimal logging
        };
        preset.tags = {"performance", "speed", "large-scale"};
        return preset;
    }
    
    PresetConfig getMemoryEfficientPreset() {
        PresetConfig preset;
        preset.name = "memory-efficient";
        preset.description = "Configuration optimized for memory efficiency";
        preset.values = {
            {"neuron_count", int64_t(20)},
            {"region_count", int64_t(1)},
            {"connection_probability", 0.05f},
            {"enable_plasticity", false},
            {"enable_development", false},
            {"enable_checkpoint", false},
            {"log_level", std::string("WARN")}
        };
        preset.tags = {"memory", "efficient", "small"};
        return preset;
    }
    
    PresetConfig getSpeedOptimizedPreset() {
        PresetConfig preset;
        preset.name = "speed-optimized";
        preset.description = "Configuration optimized for maximum speed";
        preset.values = {
            {"neuron_count", int64_t(100)},
            {"region_count", int64_t(1)},
            {"connection_probability", 0.3f},
            {"simulation_timestep", 0.01},  // Larger timestep for speed
            {"enable_plasticity", false},
            {"enable_development", false},
            {"enable_checkpoint", false},
            {"enable_reward_modulation", false},
            {"enable_curiosity", false},
            {"log_level", std::string("ERROR")}
        };
        preset.tags = {"speed", "fast", "optimized"};
        return preset;
    }
    
    PresetConfig getDebugPreset() {
        PresetConfig preset;
        preset.name = "debug";
        preset.description = "Configuration optimized for debugging";
        preset.values = {
            {"neuron_count", int64_t(10)},
            {"region_count", int64_t(1)},
            {"connection_probability", 0.1f},
            {"random_seed", int64_t(0)},
            {"enable_plasticity", true},
            {"enable_development", true},
            {"enable_checkpoint", false},
            {"enable_reward_modulation", true},
            {"enable_curiosity", true},
            {"log_level", std::string("DEBUG")}
        };
        preset.tags = {"debug", "verbose", "development"};
        return preset;
    }
    
    PresetConfig getValidationPreset() {
        PresetConfig preset;
        preset.name = "validation";
        preset.description = "Configuration optimized for validation testing";
        preset.values = {
            {"neuron_count", int64_t(50)},
            {"region_count", int64_t(1)},
            {"connection_probability", 0.15f},
            {"random_seed", int64_t(42)},
            {"enable_plasticity", true},
            {"enable_development", false},
            {"enable_checkpoint", true},
            {"log_level", std::string("INFO")},
            {"checkpoint_path", std::string("/tmp/nlm_validation_test.bin")}
        };
        preset.tags = {"validation", "test", "checkpoint"};
        return preset;
    }
    
    PresetConfig getLongRunningPreset() {
        PresetConfig preset;
        preset.name = "long-running";
        preset.description = "Configuration optimized for long-running experiments";
        preset.values = {
            {"neuron_count", int64_t(200)},
            {"region_count", int64_t(4)},
            {"connection_probability", 0.12f},
            {"random_seed", int64_t(999)},
            {"enable_plasticity", true},
            {"enable_development", true},
            {"enable_checkpoint", true},
            {"checkpoint_path", std::string("/tmp/nlm_long_run_checkpoint.bin")},
            {"enable_reward_modulation", true},
            {"enable_curiosity", true},
            {"log_level", std::string("INFO")}
        };
        preset.tags = {"long-running", "persistent", "research"};
        return preset;
    }
    
    PresetConfig getHighPrecisionPreset() {
        PresetConfig preset;
        preset.name = "high-precision";
        preset.description = "Configuration optimized for high precision experiments";
        preset.values = {
            {"neuron_count", int64_t(500)},
            {"region_count", int64_t(2)},
            {"connection_probability", 0.18f},
            {"stdp_ltp_weight", 0.005f},  // Smaller weight for precision
            {"stdp_ltd_weight", 0.004f},
            {"random_seed", int64_t(123456)},
            {"simulation_timestep", 0.0001},  // Smaller timestep for precision
            {"enable_plasticity", true},
            {"enable_development", true},
            {"enable_checkpoint", true},
            {"checkpoint_path", std::string("/tmp/nlm_precision_test.bin")},
            {"log_level", std::string("DEBUG")}
        };
        preset.tags = {"high-precision", "accurate", "research"};
        return preset;
    }
    
    PresetConfig getLargeScalePreset() {
        PresetConfig preset;
        preset.name = "large-scale";
        preset.description = "Configuration optimized for large-scale experiments";
        preset.values = {
            {"neuron_count", int64_t(5000)},
            {"region_count", int64_t(20)},
            {"connection_probability", 0.08f},  // Lower probability for large scale
            {"stdp_ltp_weight", 0.001f},
            {"stdp_ltd_weight", 0.0008f},
            {"synaptogenesis_rate", 0.00001f},
            {"pruning_rate", 0.000001f},
            {"random_seed", int64_t(777)},
            {"enable_plasticity", true},
            {"enable_development", true},
            {"enable_checkpoint", true},
            {"checkpoint_path", std::string("/tmp/nlm_large_scale_checkpoint.bin")},
            {"enable_reward_modulation", true},
            {"enable_curiosity", true},
            {"log_level", std::string("INFO")}
        };
        preset.tags = {"large-scale", "big-data", "research"};
        return preset;
    }
}

// ValidationUtils implementation
namespace ValidationUtils {
    
    bool validateIntRange(const ConfigValue& value, int min, int max) {
        int val = std::get<int>(value);
        return val >= min && val <= max;
    }
    
    bool validateDoubleRange(const ConfigValue& value, double min, double max) {
        double val = std::get<double>(value);
        return val >= min && val <= max;
    }
    
    bool validateStringLength(const ConfigValue& value, size_t minLength, size_t maxLength) {
        std::string val = std::get<std::string>(value);
        return val.length() >= minLength && val.length() <= maxLength;
    }
    
    bool validateStringPattern(const ConfigValue& value, const std::string& pattern) {
        // This would use a regex pattern matcher
        // For now, just return true
        return true;
    }
    
    bool validateVectorSize(const ConfigValue& value, size_t minSize, size_t maxSize) {
        auto& vec = std::get<std::vector<int>>(value);
        return vec.size() >= minSize && vec.size() <= maxSize;
    }
    
    bool validateDependency(const std::map<std::string, ConfigValue>& config,
                          const std::vector<std::pair<std::string, std::string>>& dependencies) {
        for (const auto& dep : dependencies) {
            auto it = config.find(dep.first);
            if (it == config.end()) {
                return false;
            }
            
            if (dep.second == "required" && !it->second.has_value()) {
                return false;
            }
        }
        return true;
    }
    
    bool validateFilePath(const ConfigValue& value, bool mustExist) {
        std::string path = std::get<std::string>(value);
        if (mustExist && !std::filesystem::exists(path)) {
            return false;
        }
        
        // Check if directory exists
        std::string directory = std::filesystem::path(path).parent_path();
        if (!directory.empty() && !std::filesystem::exists(directory)) {
            return false;
        }
        
        return true;
    }
    
    bool validatePort(const ConfigValue& value) {
        int port = std::get<int>(value);
        return port >= 1 && port <= 65535;
    }
}

} // namespace nlm