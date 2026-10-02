#include "ConfigPresets.hpp"
#include "../Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>

namespace nlm {
namespace ConfigPresets {

static std::map<std::string, std::shared_ptr<Config>> customPresets_;

static void initializeBuiltInPresets() {
    // Default preset
    auto defaultPreset = std::make_shared<Config>();
    defaultPreset->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    defaultPreset->set("simulation_timestep", 0.001, ConfigSource::Default);
    defaultPreset->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    defaultPreset->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    defaultPreset->set("connection_probability", 0.1f, ConfigSource::Default);
    defaultPreset->set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
    defaultPreset->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    defaultPreset->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    defaultPreset->set("stdp_time_constant", 20.0f, ConfigSource::Default);
    defaultPreset->set("development_synaptogenesis_rate", 0.001f, ConfigSource::Default);
    defaultPreset->set("development_pruning_rate", 0.0001f, ConfigSource::Default);
    defaultPreset->set("dopamine_baseline", 0.1f, ConfigSource::Default);
    defaultPreset->set("reward_discount_factor", 0.99f, ConfigSource::Default);
    defaultPreset->set("environment_name", std::string("GridWorld"), ConfigSource::Default);
    defaultPreset->set("environment_width", static_cast<int64_t>(10), ConfigSource::Default);
    defaultPreset->set("environment_height", static_cast<int64_t>(10), ConfigSource::Default);
    defaultPreset->set("log_level", std::string("INFO"), ConfigSource::Default);
    defaultPreset->set("log_to_file", false, ConfigSource::Default);
    defaultPreset->set("log_filename", std::string("nlm.log"), ConfigSource::Default);
    defaultPreset->set("max_simulation_steps", static_cast<int64_t>(10000), ConfigSource::Default);
    defaultPreset->set("simulation_time_limit", 0.0, ConfigSource::Default);
    defaultPreset->set("visualization_enabled", false, ConfigSource::Default);
    defaultPreset->set("visualization_update_rate", 30.0f, ConfigSource::Default);
    
    customPresets_["Default"] = defaultPreset;
    
    // Fast test preset (optimized for speed)
    auto testPreset = std::make_shared<Config>();
    testPreset->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    testPreset->set("simulation_timestep", 0.001, ConfigSource::Default);
    testPreset->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    testPreset->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    testPreset->set("connection_probability", 0.15f, ConfigSource::Default);
    testPreset->set("plasticity_learning_rate", 0.05f, ConfigSource::Default);
    testPreset->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    testPreset->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    testPreset->set("stdp_time_constant", 20.0f, ConfigSource::Default);
    testPreset->set("development_synaptogenesis_rate", 0.0005f, ConfigSource::Default);
    testPreset->set("development_pruning_rate", 0.00005f, ConfigSource::Default);
    testPreset->set("dopamine_baseline", 0.2f, ConfigSource::Default);
    testPreset->set("reward_discount_factor", 0.95f, ConfigSource::Default);
    testPreset->set("environment_name", std::string("GridWorld"), ConfigSource::Default);
    testPreset->set("environment_width", static_cast<int64_t>(20), ConfigSource::Default);
    testPreset->set("environment_height", static_cast<int64_t>(20), ConfigSource::Default);
    testPreset->set("log_level", std::string("INFO"), ConfigSource::Default);
    testPreset->set("log_to_file", true, ConfigSource::Default);
    testPreset->set("log_filename", std::string("nlm_test.log"), ConfigSource::Default);
    testPreset->set("max_simulation_steps", static_cast<int64_t>(1000), ConfigSource::Default);
    testPreset->set("simulation_time_limit", 0.0, ConfigSource::Default);
    testPreset->set("visualization_enabled", false, ConfigSource::Default);
    testPreset->set("visualization_update_rate", 30.0f, ConfigSource::Default);
    
    customPresets_["FastTest"] = testPreset;
    
    // High accuracy preset (optimized for precision)
    auto highAccuracyPreset = std::make_shared<Config>();
    highAccuracyPreset->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    highAccuracyPreset->set("simulation_timestep", 0.0001, ConfigSource::Default);
    highAccuracyPreset->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
    highAccuracyPreset->set("region_count", static_cast<int64_t>(5), ConfigSource::Default);
    highAccuracyPreset->set("connection_probability", 0.05f, ConfigSource::Default);
    highAccuracyPreset->set("plasticity_learning_rate", 0.005f, ConfigSource::Default);
    highAccuracyPreset->set("stdp_ltp_weight", 0.03f, ConfigSource::Default);
    highAccuracyPreset->set("stdp_ltd_weight", 0.025f, ConfigSource::Default);
    highAccuracyPreset->set("stdp_time_constant", 30.0f, ConfigSource::Default);
    highAccuracyPreset->set("development_synaptogenesis_rate", 0.00005f, ConfigSource::Default);
    highAccuracyPreset->set("development_pruning_rate", 0.000005f, ConfigSource::Default);
    highAccuracyPreset->set("dopamine_baseline", 0.05f, ConfigSource::Default);
    highAccuracyPreset->set("reward_discount_factor", 0.999f, ConfigSource::Default);
    highAccuracyPreset->set("environment_name", std::string("ComplexWorld"), ConfigSource::Default);
    highAccuracyPreset->set("environment_width", static_cast<int64_t>(50), ConfigSource::Default);
    highAccuracyPreset->set("environment_height", static_cast<int64_t>(50), ConfigSource::Default);
    highAccuracyPreset->set("log_level", std::string("DEBUG"), ConfigSource::Default);
    highAccuracyPreset->set("log_to_file", true, ConfigSource::Default);
    highAccuracyPreset->set("log_filename", std::string("nlm_high_accuracy.log"), ConfigSource::Default);
    highAccuracyPreset->set("max_simulation_steps", static_cast<int64_t>(50000), ConfigSource::Default);
    highAccuracyPreset->set("simulation_time_limit", 0.0, ConfigSource::Default);
    highAccuracyPreset->set("visualization_enabled", true, ConfigSource::Default);
    highAccuracyPreset->set("visualization_update_rate", 10.0f, ConfigSource::Default);
    
    customPresets_["HighAccuracy"] = highAccuracyPreset;
    
    // Low power preset (optimized for energy efficiency)
    auto lowPowerPreset = std::make_shared<Config>();
    lowPowerPreset->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    lowPowerPreset->set("simulation_timestep", 0.01, ConfigSource::Default);
    lowPowerPreset->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    lowPowerPreset->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    lowPowerPreset->set("connection_probability", 0.05f, ConfigSource::Default);
    lowPowerPreset->set("plasticity_learning_rate", 0.001f, ConfigSource::Default);
    lowPowerPreset->set("stdp_ltp_weight", 0.005f, ConfigSource::Default);
    lowPowerPreset->set("stdp_ltd_weight", 0.004f, ConfigSource::Default);
    lowPowerPreset->set("stdp_time_constant", 40.0f, ConfigSource::Default);
    lowPowerPreset->set("development_synaptogenesis_rate", 0.00001f, ConfigSource::Default);
    lowPowerPreset->set("development_pruning_rate", 0.000001f, ConfigSource::Default);
    lowPowerPreset->set("dopamine_baseline", 0.0f, ConfigSource::Default);
    lowPowerPreset->set("reward_discount_factor", 0.9f, ConfigSource::Default);
    lowPowerPreset->set("environment_name", std::string("GridWorld"), ConfigSource::Default);
    lowPowerPreset->set("environment_width", static_cast<int64_t>(5), ConfigSource::Default);
    lowPowerPreset->set("environment_height", static_cast<int64_t>(5), ConfigSource::Default);
    lowPowerPreset->set("log_level", std::string("WARNING"), ConfigSource::Default);
    lowPowerPreset->set("log_to_file", false, ConfigSource::Default);
    lowPowerPreset->set("log_filename", std::string("nlm_power.log"), ConfigSource::Default);
    lowPowerPreset->set("max_simulation_steps", static_cast<int64_t>(5000), ConfigSource::Default);
    lowPowerPreset->set("simulation_time_limit", 0.0, ConfigSource::Default);
    lowPowerPreset->set("visualization_enabled", false, ConfigSource::Default);
    lowPowerPreset->set("visualization_update_rate", 1.0f, ConfigSource::Default);
    
    customPresets_["LowPower"] = lowPowerPreset;
}

std::shared_ptr<Config> getPreset(const std::string& presetName) {
    // Initialize built-in presets on first call
    static bool initialized = false;
    if (!initialized) {
        initializeBuiltInPresets();
        initialized = true;
    }
    
    std::string name = presetName;
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    
    // Look for custom preset
    auto it = customPresets_.find(name);
    if (it != customPresets_.end()) {
        return it->second;
    }
    
    // Look for built-in preset
    it = customPresets_.find(presetName);
    if (it != customPresets_.end()) {
        return it->second;
    }
    
    NLM_LOG_WARNING("Preset not found: " + presetName + ". Using default.");
    auto defaultPreset = std::make_shared<Config>();
    defaultPreset->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    defaultPreset->set("simulation_timestep", 0.001, ConfigSource::Default);
    return defaultPreset;
}

std::shared_ptr<Config> getPreset(PresetName presetName) {
    std::string presetNameStr;
    switch (presetName) {
        case PresetName::Default:
            presetNameStr = "Default";
            break;
        case PresetName::FastTest:
            presetNameStr = "FastTest";
            break;
        case PresetName::HighAccuracy:
            presetNameStr = "HighAccuracy";
            break;
        case PresetName::LowPower:
            presetNameStr = "LowPower";
            break;
        default:
            presetNameStr = "Default";
            break;
    }
    return getPreset(presetNameStr);
}

std::vector<std::string> listPresets() {
    static bool initialized = false;
    if (!initialized) {
        initializeBuiltInPresets();
        initialized = true;
    }
    
    std::vector<std::string> presetNames;
    for (const auto& pair : customPresets_) {
        presetNames.push_back(pair.first);
    }
    
    std::sort(presetNames.begin(), presetNames.end());
    return presetNames;
}

void applyPreset(Config& config, const std::string& presetName) {
    std::shared_ptr<Config> preset = getPreset(presetName);
    
    // Copy all entries from preset to config
    config.clear();
    for (const auto& entry : preset->pImpl->entries) {
        config.set(entry.key, entry.value, entry.source);
    }
    
    NLM_LOG_INFO("Applied preset: " + presetName);
}

void applyPreset(Config& config, PresetName presetName) {
    std::string presetNameStr;
    switch (presetName) {
        case PresetName::Default:
            presetNameStr = "Default";
            break;
        case PresetName::FastTest:
            presetNameStr = "FastTest";
            break;
        case PresetName::HighAccuracy:
            presetNameStr = "HighAccuracy";
            break;
        case PresetName::LowPower:
            presetNameStr = "LowPower";
            break;
        default:
            presetNameStr = "Default";
            break;
    }
    applyPreset(config, presetNameStr);
}

std::shared_ptr<Config> createCustomPreset(const std::string& presetName) {
    auto preset = std::make_shared<Config>();
    customPresets_[presetName] = preset;
    NLM_LOG_INFO("Created custom preset: " + presetName);
    return preset;
}

void updatePreset(const std::string& presetName, const Config& config) {
    auto it = customPresets_.find(presetName);
    if (it != customPresets_.end()) {
        // Clear existing preset and copy config entries
        it->second->clear();
        for (const auto& entry : config.pImpl->entries) {
            it->second->set(entry.key, entry.value, entry.source);
        }
        NLM_LOG_INFO("Updated preset: " + presetName);
    } else {
        NLM_LOG_WARNING("Preset not found for update: " + presetName);
    }
}

void removePreset(const std::string& presetName) {
    auto it = customPresets_.find(presetName);
    if (it != customPresets_.end()) {
        customPresets_.erase(it);
        NLM_LOG_INFO("Removed preset: " + presetName);
    } else {
        NLM_LOG_WARNING("Preset not found for removal: " + presetName);
    }
}

bool isBuiltInPreset(const std::string& presetName) {
    static bool initialized = false;
    if (!initialized) {
        initializeBuiltInPresets();
        initialized = true;
    }
    
    std::string name = presetName;
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    
    // Check if preset exists in custom presets
    auto it = customPresets_.find(name);
    if (it != customPresets_.end()) {
        return false;  // It's a custom preset
    }
    
    // Check if it's a built-in preset name
    std::vector<std::string> builtInNames = {"default", "fasttest", "highaccuracy", "lowpower"};
    for (const auto& builtInName : builtInNames) {
        if (name == builtInName) {
            return true;
        }
    }
    
    return false;
}

std::shared_ptr<Config> getDefaultConfig() {
    return getPreset("Default");
}

std::shared_ptr<Config> getTestConfig() {
    return getPreset("FastTest");
}

std::shared_ptr<Config> getHighAccuracyConfig() {
    return getPreset("HighAccuracy");
}

std::shared_ptr<Config> getLowPowerConfig() {
    return getPreset("LowPower");
}

} // namespace ConfigPresets
} // namespace nlm
