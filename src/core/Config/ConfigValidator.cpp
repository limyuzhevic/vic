#include "ConfigValidator.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <limits>

namespace nlm {

void ConfigSchema::addParameter(const ParameterRule& rule) {
    parameters_.push_back(rule);
}

bool ConfigSchema::validate(const Config& config, std::vector<std::string>& errors) const {
    bool isValid = true;
    
    for (const auto& rule : parameters_) {
        if (rule.required && !config.has(rule.name)) {
            errors.push_back("Required parameter \"" + rule.name + "\" is missing");
            isValid = false;
            continue;
        }
        
        if (config.has(rule.name)) {
            // Parameter exists, validate it
            if (!rule.validator(config, rule.name)) {
                errors.push_back("Parameter \"" + rule.name + "\" validation failed");
                isValid = false;
            }
        }
    }
    
    return isValid;
}

ConfigSchema ConfigSchema::createDefaultSchema() {
    ConfigSchema schema;
    
    // Basic brain parameters
    schema.addParameter({
        "brain.neuron_count",
        "int",
        "Total number of neurons in the brain",
        true,
        {{10, 10000}},
        {},
        [](const Config& config, const std::string& name) {
            if (!config.has(name)) return false;
            int64_t value = config.getOr<int64_t>(name, 1000);
            return value >= 10 && value <= 10000;
        }
    });
    
    schema.addParameter({
        "brain.synapse_density",
        "float",
        "Synapse connectivity density (0.0-1.0)",
        false,
        {{0.0, 1.0}},
        {},
        [](const Config& config, const std::string& name) {
            if (!config.has(name)) return true; // Optional parameter
            float value = config.getOr<float>(name, 0.1f);
            return value >= 0.0f && value <= 1.0f;
        }
    });
    
    schema.addParameter({
        "plasticity.stdp.enable",
        "bool",
        "Enable Spike-Timing-Dependent Plasticity",
        false,
        {},
        {},
        [](const Config& config, const std::string& name) {
            if (!config.has(name)) return true; // Optional parameter
            bool value = config.getOr<bool>(name, true);
            return true; // Boolean validation passes
        }
    });
    
    schema.addParameter({
        "plasticity.hebbian.enable",
        "bool",
        "Enable Hebbian plasticity",
        false,
        {},
        {},
        [](const Config& config, const std::string& name) {
            if (!config.has(name)) return true;
            return true;
        }
    });
    
    schema.addParameter({
        "plasticity.structural.enable",
        "bool",
        "Enable structural plasticity (synaptogenesis/pruning)",
        false,
        {},
        {},
        [](const Config& config, const std::string& name) {
            if (!config.has(name)) return true;
            return true;
        }
    });
    
    return schema;
}

ConfigSchema ConfigSchema::createAdvancedSchema() {
    ConfigSchema schema = createDefaultSchema();
    
    // Add advanced parameters
    schema.addParameter({
        "plasticity.stdp.learning_rate",
        "float",
        "STDP learning rate",
        false,
        {{0.0, 1.0}},
        {},
        [](const Config& config, const std::string& name) {
            if (!config.has(name)) return true;
            float value = config.getOr<float>(name, 0.01f);
            return value >= 0.0f && value <= 1.0f;
        }
    });
    
    schema.addParameter({
        "plasticity.stdp.time_constant",
        "float",
        "STDP time constant (ms)",
        false,
        {{1.0, 100.0}},
        {},
        [](const Config& config, const std::string& name) {
            if (!config.has(name)) return true;
            float value = config.getOr<float>(name, 20.0f);
            return value >= 1.0f && value <= 100.0f;
        }
    });
    
    schema.addParameter({
        "neuromod.dopamine.scale",
        "float",
        "Dopamine modulation scale",
        false,
        {{0.0, 5.0}},
        {},
        [](const Config& config, const std::string& name) {
            if (!config.has(name)) return true;
            float value = config.getOr<float>(name, 1.0f);
            return value >= 0.0f && value <= 5.0f;
        }
    });
    
    schema.addParameter({
        "neuromod.curiosity.exploration_rate",
        "float",
        "Curiosity-driven exploration rate (0.0-1.0)",
        false,
        {{0.0, 1.0}},
        {},
        [](const Config& config, const std::string& name) {
            if (!config.has(name)) return true;
            float value = config.getOr<float>(name, 0.5f);
            return value >= 0.0f && value <= 1.0f;
        }
    });
    
    return schema;
}

// Initialize static members
std::map<std::string, std::shared_ptr<Config>> ConfigPresets::presets_ = {};
const std::string ConfigPresets::PRESETS_DIR = "presets/";

std::shared_ptr<Config> ConfigPresets::createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    config->set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
    config->set("plasticity.stdp.enable", true, ConfigSource::Default);
    config->set("plasticity.stdp.learning_rate", 0.01f, ConfigSource::Default);
    config->set("plasticity.stdp.time_constant", 20.0f, ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
    config->set("plasticity.hebbian.learning_rate", 0.01f, ConfigSource::Default);
    config->set("plasticity.structural.enable", true, ConfigSource::Default);
    config->set("plasticity.structural.synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("plasticity.structural.pruning_rate", 0.00001f, ConfigSource::Default);
    config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
    config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
    config->set("neuromod.curiosity.exploration_rate", 0.5f, ConfigSource::Default);
    config->set("neuromod.novelty.enable", true, ConfigSource::Default);
    config->set("neuromod.novelty.sensitivity", 1.0f, ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> ConfigPresets::createDevelopmentConfig() {
    auto config = createDefaultConfig();
    
    // Development-focused parameters
    config->set("brain.neuron_count", static_cast<int64_t>(2000), ConfigSource::Preset);
    config->set("brain.synapse_density", 0.15f, ConfigSource::Preset);
    config->set("plasticity.structural.synaptogenesis_rate", 0.001f, ConfigSource::Preset);
    config->set("plasticity.structural.pruning_rate", 0.0001f, ConfigSource::Preset);
    config->set("neuromod.curiosity.exploration_rate", 0.8f, ConfigSource::Preset);
    config->set("neuromod.novelty.sensitivity", 1.5f, ConfigSource::Preset);
    config->set("development.initial_stage", "CriticalPeriod", ConfigSource::Preset);
    
    return config;
}

std::shared_ptr<Config> ConfigPresets::createLearningConfig() {
    auto config = createDefaultConfig();
    
    // Learning-focused parameters
    config->set("brain.neuron_count", static_cast<int64_t>(1500), ConfigSource::Preset);
    config->set("brain.synapse_density", 0.12f, ConfigSource::Preset);
    config->set("plasticity.stdp.learning_rate", 0.005f, ConfigSource::Preset);
    config->set("plasticity.hebbian.learning_rate", 0.02f, ConfigSource::Preset);
    config->set("neuromod.reward_modulation.enable", true, ConfigSource::Preset);
    config->set("neuromod.reward_modulation.strength", 1.5f, ConfigSource::Preset);
    
    return config;
}

std::shared_ptr<Config> ConfigPresets::createExplorationConfig() {
    auto config = createDefaultConfig();
    
    // Exploration-focused parameters
    config->set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Preset);
    config->set("brain.synapse_density", 0.08f, ConfigSource::Preset);
    config->set("plasticity.structural.synaptogenesis_rate", 0.002f, ConfigSource::Preset);
    config->set("neuromod.curiosity.exploration_rate", 0.9f, ConfigSource::Preset);
    config->set("neuromod.novelty.sensitivity", 1.2f, ConfigSource::Preset);
    
    return config;
}

std::shared_ptr<Config> ConfigPresets::createPerformanceConfig() {
    auto config = createDefaultConfig();
    
    // Performance-focused parameters
    config->set("brain.neuron_count", static_cast<int64_t>(500), ConfigSource::Preset);
    config->set("brain.synapse_density", 0.05f, ConfigSource::Preset);
    config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Preset);
    config->set("plasticity.structural.enable", false, ConfigSource::Preset); // Disable for performance
    config->set("neuromod.curiosity.enable", false, ConfigSource::Preset); // Disable for performance
    
    return config;
}

std::shared_ptr<Config> ConfigPresets::loadPreset(const std::string& presetName) {
    if (presets_.find(presetName) == presets_.end()) {
        // Create preset on first access
        std::string lowerName = presetName;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        
        if (lowerName == "default") {
            presets_[presetName] = createDefaultConfig();
        } else if (lowerName == "development") {
            presets_[presetName] = createDevelopmentConfig();
        } else if (lowerName == "learning") {
            presets_[presetName] = createLearningConfig();
        } else if (lowerName == "exploration") {
            presets_[presetName] = createExplorationConfig();
        } else if (lowerName == "performance") {
            presets_[presetName] = createPerformanceConfig();
        } else {
            throw std::runtime_error("Unknown preset: " + presetName);
        }
    }
    
    return presets_[presetName];
}

bool ConfigPresets::savePreset(const std::string& presetName, const Config& config) {
    try {
        // Ensure presets directory exists
        if (!std::filesystem::exists(PRESETS_DIR)) {
            std::filesystem::create_directories(PRESETS_DIR);
        }
        
        std::string filename = PRESETS_DIR + presetName + ".cfg";
        bool success = config.saveToFile(filename);
        
        if (success) {
            presets_[presetName] = std::make_shared<Config>(config);
        }
        
        return success;
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::string> ConfigPresets::listPresets() {
    std::vector<std::string> presetNames;
    
    // Add built-in presets
    presetNames.push_back("default");
    presetNames.push_back("development");
    presetNames.push_back("learning");
    presetNames.push_back("exploration");
    presetNames.push_back("performance");
    
    // Add file-based presets
    if (std::filesystem::exists(PRESETS_DIR)) {
        for (const auto& entry : std::filesystem::directory_iterator(PRESETS_DIR)) {
            if (entry.is_regular_file() && entry.path().extension() == ".cfg") {
                std::string filename = entry.path().filename().string();
                // Remove .cfg extension
                if (filename.size() > 4) {
                    filename = filename.substr(0, filename.size() - 4);
                    if (std::find(presetNames.begin(), presetNames.end(), filename) == presetNames.end()) {
                        presetNames.push_back(filename);
                    }
                }
            }
        }
    }
    
    return presetNames;
}

ConfigValidator::ConfigValidator() {
    initializeSchema();
}

bool ConfigValidator::validate(const Config& config, std::vector<std::string>& errors) const {
    bool isValid = schema_.validate(config, errors);
    
    // Additional validation checks
    if (isValid) {
        std::vector<std::string> additionalErrors;
        if (!validateRanges(config, additionalErrors)) {
            isValid = false;
            errors.insert(errors.end(), additionalErrors.begin(), additionalErrors.end());
        }
        
        if (!checkConflicts(config, additionalErrors)) {
            isValid = false;
            errors.insert(errors.end(), additionalErrors.begin(), additionalErrors.end());
        }
    }
    
    return isValid;
}

bool ConfigValidator::validateWithWarnings(const Config& config, std::vector<std::string>& warnings) const {
    bool isValid = true;
    
    // Check for deprecated parameters
    std::set<std::string> deprecatedParams = {
        "brain.max_neurons", "plasticity.hebbian.threshold", "neuromod.old_style"
    };
    
    for (const auto& param : deprecatedParams) {
        if (config.has(param)) {
            warnings.push_back("Parameter \"" + param + "\" is deprecated and may be removed in future versions");
        }
    }
    
    // Check for parameter combinations that may cause issues
    if (config.has("brain.neuron_count") && config.has("brain.synapse_density")) {
        int64_t neuronCount = config.getOr<int64_t>("brain.neuron_count", 1000);
        float density = config.getOr<float>("brain.synapse_density", 0.1f);
        
        // Warn about potentially unrealistic configurations
        if (neuronCount > 5000 && density > 0.2f) {
            warnings.push_back("Large neuron count with high synapse density may cause performance issues");
        }
    }
    
    return isValid;
}

void ConfigValidator::applySmartDefaults(Config& config) const {
    if (!config.has("brain.neuron_count")) {
        config.set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    }
    
    if (!config.has("brain.synapse_density")) {
        config.set("brain.synapse_density", 0.1f, ConfigSource::Default);
    }
    
    if (!config.has("plasticity.stdp.enable")) {
        config.set("plasticity.stdp.enable", true, ConfigSource::Default);
    }
    
    if (!config.has("plasticity.stdp.learning_rate")) {
        config.set("plasticity.stdp.learning_rate", 0.01f, ConfigSource::Default);
    }
    
    if (!config.has("plasticity.stdp.time_constant")) {
        config.set("plasticity.stdp.time_constant", 20.0f, ConfigSource::Default);
    }
    
    if (!config.has("neuromod.curiosity.exploration_rate")) {
        config.set("neuromod.curiosity.exploration_rate", 0.5f, ConfigSource::Default);
    }
}

bool ConfigValidator::checkConflicts(const Config& config, std::vector<std::string>& conflicts) const {
    bool hasConflicts = false;
    
    // Check for incompatible parameter combinations
    if (config.has("plasticity.stdp.enable") && 
        config.has("plasticity.structural.enable") &&
        config.getOr<bool>("plasticity.stdp.enable", true) &&
        config.getOr<bool>("plasticity.structural.enable", true)) {
        // This combination is fine, but warn about potential performance impact
        conflicts.push_back("Both STDP and structural plasticity enabled - may impact performance");
        hasConflicts = true;
    }
    
    if (config.has("neuromod.curiosity.enable") && 
        config.has("neuromod.reward_modulation.enable") &&
        config.getOr<bool>("neuromod.curiosity.enable", true) &&
        config.getOr<bool>("neuromod.reward_modulation.enable", true)) {
        // This combination is fine, but warn about potential conflicts
        conflicts.push_back("Both curiosity and reward modulation enabled - may compete for resources");
        hasConflicts = true;
    }
    
    return !hasConflicts;
}

bool ConfigValidator::validateRanges(const Config& config, std::vector<std::string>& errors) const {
    bool isValid = true;
    
    // Validate neuron count range
    if (config.has("brain.neuron_count")) {
        int64_t neuronCount = config.getOr<int64_t>("brain.neuron_count", 1000);
        if (neuronCount < 10 || neuronCount > 10000) {
            errors.push_back("brain.neuron_count must be between 10 and 10000");
            isValid = false;
        }
    }
    
    // Validate synapse density range
    if (config.has("brain.synapse_density")) {
        float density = config.getOr<float>("brain.synapse_density", 0.1f);
        if (density < 0.0f || density > 1.0f) {
            errors.push_back("brain.synapse_density must be between 0.0 and 1.0");
            isValid = false;
        }
    }
    
    // Validate learning rates
    if (config.has("plasticity.stdp.learning_rate")) {
        float learningRate = config.getOr<float>("plasticity.stdp.learning_rate", 0.01f);
        if (learningRate < 0.0f || learningRate > 1.0f) {
            errors.push_back("plasticity.stdp.learning_rate must be between 0.0 and 1.0");
            isValid = false;
        }
    }
    
    if (config.has("plasticity.hebbian.learning_rate")) {
        float learningRate = config.getOr<float>("plasticity.hebbian.learning_rate", 0.01f);
        if (learningRate < 0.0f || learningRate > 1.0f) {
            errors.push_back("plasticity.hebbian.learning_rate must be between 0.0 and 1.0");
            isValid = false;
        }
    }
    
    // Validate time constants
    if (config.has("plasticity.stdp.time_constant")) {
        float timeConstant = config.getOr<float>("plasticity.stdp.time_constant", 20.0f);
        if (timeConstant < 1.0f || timeConstant > 100.0f) {
            errors.push_back("plasticity.stdp.time_constant must be between 1.0 and 100.0");
            isValid = false;
        }
    }
    
    return isValid;
}

ConfigValidator::ValidationStats ConfigValidator::getValidationStats(const Config& config) const {
    ValidationStats stats;
    stats.totalParameters = static_cast<size_t>(config.getKeys().size());
    stats.validParameters = 0;
    stats.invalidParameters = 0;
    stats.warningsGenerated = 0;
    
    // Simple validation for stats calculation
    std::vector<std::string> errors;
    if (validate(config, errors)) {
        stats.validParameters = stats.totalParameters;
    } else {
        stats.invalidParameters = static_cast<size_t>(errors.size());
    }
    
    std::vector<std::string> warnings;
    validateWithWarnings(config, warnings);
    stats.warningsGenerated = static_cast<size_t>(warnings.size());
    
    return stats;
}

void ConfigValidator::initializeSchema() {
    schema_ = ConfigSchema::createDefaultSchema();
}

} // namespace nlm
