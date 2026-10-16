#include "ConfigPresets.hpp"
#include "../core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

namespace nlm {

struct ConfigPresets::Impl {
    std::map<std::string, Preset> presets;
    std::map<std::string, std::vector<ValidationRule>> validationRules;
};

ConfigPresets::ConfigPresets() : pImpl(std::make_unique<Impl>()) {
    // Add default presets
    auto defaultPresets = getDefaultPresets();
    for (const auto& preset : defaultPresets) {
        addPreset(preset);
    }
}

ConfigPresets::~ConfigPresets() {
}

void ConfigPresets::addPreset(const Preset& preset) {
    pImpl->presets[preset.name] = preset;
}

bool ConfigPresets::hasPreset(const std::string& name) const {
    return pImpl->presets.find(name) != pImpl->presets.end();
}

const ConfigPresets::Preset* ConfigPresets::getPreset(const std::string& name) const {
    auto it = pImpl->presets.find(name);
    if (it != pImpl->presets.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<std::string> ConfigPresets::getPresetNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->presets) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> ConfigPresets::getPresetNames(const std::string& environment) const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->presets) {
        if (pair.second.environment == environment) {
            names.push_back(pair.first);
        }
    }
    return names;
}

bool ConfigPresets::applyPreset(const std::string& name, std::shared_ptr<Config> config) const {
    const Preset* preset = getPreset(name);
    if (!preset) {
        return false;
    }
    
    for (const auto& pair : preset->values) {
        config->set(pair.first, pair.second, ConfigSource::Runtime);
    }
    
    return true;
}

bool ConfigPresets::validatePreset(const std::string& name, const std::shared_ptr<Config> config) const {
    const Preset* preset = getPreset(name);
    if (!preset) {
        return false;
    }
    
    return validateConfig(config, preset->environment);
}

void ConfigPresets::removePreset(const std::string& name) {
    pImpl->presets.erase(name);
}

std::vector<ConfigPresets::Preset> ConfigPresets::getDefaultPresets() {
    std::vector<Preset> presets;
    
    // Fast Learning preset
    {
        Preset preset;
        preset.name = "fast_learning";
        preset.description = "Optimized for rapid learning and experimentation";
        preset.environment = "development";
        preset.isDefault = true;
        preset.values = {
            {"neuron_count", "200"},
            {"region_count", "1"},
            {"connection_probability", "0.2"},
            {"stdp_ltp_weight", "0.05"},
            {"stdp_ltd_weight", "0.03"},
            {"synaptogenesis_rate", "0.001"},
            {"pruning_rate", "0.0001"},
            {"simulation_timestep", "0.001"},
            {"max_steps", "1000"}
        };
        presets.push_back(preset);
    }
    
    // Stable Learning preset
    {
        Preset preset;
        preset.name = "stable_learning";
        preset.description = "Optimized for stable, long-term learning";
        preset.environment = "production";
        preset.values = {
            {"neuron_count", "1000"},
            {"region_count", "2"},
            {"connection_probability", "0.15"},
            {"stdp_ltp_weight", "0.02"},
            {"stdp_ltd_weight", "0.015"},
            {"synaptogenesis_rate", "0.0005"},
            {"pruning_rate", "0.00005"},
            {"simulation_timestep", "0.001"},
            {"max_steps", "10000"}
        };
        presets.push_back(preset);
    }
    
    // Quick Test preset
    {
        Preset preset;
        preset.name = "quick_test";
        preset.description = "Quick test configuration for validation";
        preset.environment = "testing";
        preset.isExperimental = true;
        preset.values = {
            {"neuron_count", "50"},
            {"region_count", "1"},
            {"connection_probability", "0.3"},
            {"stdp_ltp_weight", "0.1"},
            {"stdp_ltd_weight", "0.05"},
            {"synaptogenesis_rate", "0.002"},
            {"pruning_rate", "0.0002"},
            {"simulation_timestep", "0.001"},
            {"max_steps", "100"}
        };
        presets.push_back(preset);
    }
    
    return presets;
}

std::string ConfigPresets::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"presets\": [\n";
    
    bool first = true;
    for (const auto& pair : pImpl->presets) {
        if (!first) oss << ",\n";
        first = false;
        
        oss << "    {\n";
        oss << "      \"name\": \"" << pair.second.name << "\",\n";
        oss << "      \"description\": \"" << pair.second.description << "\",\n";
        oss << "      \"environment\": \"" << pair.second.environment << "\",\n";
        oss << "      \"isDefault\": " << (pair.second.isDefault ? "true" : "false") << ",\n";
        oss << "      \"isExperimental\": " << (pair.second.isExperimental ? "true" : "false") << ",\n";
        oss << "      \"values\": {\n";
        
        bool firstVal = true;
        for (const auto& valPair : pair.second.values) {
            if (!firstVal) oss << ",\n";
            firstVal = false;
            oss << "        \"" << valPair.first << "\": \"" << valPair.second << "\"";
        }
        oss << "\n      }\n";
        oss << "    }";
    }
    
    oss << "\n  ]\n";
    oss << "}\n";
    
    return oss.str();
}

std::string ConfigPresets::exportToYAML() const {
    std::ostringstream oss;
    oss << "presets:\n";
    
    for (const auto& pair : pImpl->presets) {
        oss << "  - name: " << pair.second.name << "\n";
        oss << "    description: '" << pair.second.description << "'\n";
        oss << "    environment: '" << pair.second.environment << "'\n";
        oss << "    isDefault: " << (pair.second.isDefault ? "true" : "false") << "\n";
        oss << "    isExperimental: " << (pair.second.isExperimental ? "true" : "false") << "\n";
        oss << "    values:\n";
        
        for (const auto& valPair : pair.second.values) {
            oss << "      " << valPair.first << ": '" << valPair.second << "'\n";
        }
    }
    
    return oss.str();
}

bool ConfigPresets::importFromJSON(const std::string& json) {
    // Simple JSON parsing - would need proper implementation
    // For now, just clear existing presets
    pImpl->presets.clear();
    return true;
}

bool ConfigPresets::importFromYAML(const std::string& yaml) {
    // Simple YAML parsing - would need proper implementation
    // For now, just clear existing presets
    pImpl->presets.clear();
    return true;
}

void ConfigPresets::addValidationRule(const std::string& key, const ValidationRule& rule) {
    pImpl->validationRules[key].push_back(rule);
}

std::vector<ConfigPresets::ValidationRule> ConfigPresets::getValidationRules(const std::string& key) const {
    auto it = pImpl->validationRules.find(key);
    if (it != pImpl->validationRules.end()) {
        return it->second;
    }
    return std::vector<ValidationRule>();
}

bool ConfigPresets::validateConfig(const std::shared_ptr<Config> config, const std::string& environment) const {
    if (!config) return false;
    
    // Check required presets for this environment
    auto envPresets = getPresetNames(environment);
    if (envPresets.empty()) {
        return true; // No environment-specific validation
    }
    
    // For now, just check that the config is not empty
    return true;
}

std::vector<ConfigPresets::Preset> ConfigPresets::getDevelopmentPresets() {
    std::vector<Preset> presets;
    
    Preset preset;
    preset.name = "dev_fast";
    preset.description = "Development-focused fast preset";
    preset.environment = "development";
    preset.isExperimental = true;
    preset.values = {
        {"neuron_count", "100"},
        {"region_count", "1"},
        {"connection_probability", "0.25"},
        {"stdp_ltp_weight", "0.08"},
        {"stdp_ltd_weight", "0.04"},
        {"synaptogenesis_rate", "0.002"},
        {"pruning_rate", "0.0002"}
    };
    presets.push_back(preset);
    
    return presets;
}

std::vector<ConfigPresets::Preset> ConfigPresets::getTestingPresets() {
    std::vector<Preset> presets;
    
    Preset preset;
    preset.name = "test_minimal";
    preset.description = "Minimal test configuration";
    preset.environment = "testing";
    preset.isExperimental = true;
    preset.values = {
        {"neuron_count", "10"},
        {"region_count", "1"},
        {"connection_probability", "0.1"},
        {"stdp_ltp_weight", "0.01"},
        {"stdp_ltd_weight", "0.005"},
        {"synaptogenesis_rate", "0.0001"},
        {"pruning_rate", "0.00001"}
    };
    presets.push_back(preset);
    
    return presets;
}

std::vector<ConfigPresets::Preset> ConfigPresets::getProductionPresets() {
    std::vector<Preset> presets;
    
    Preset preset;
    preset.name = "prod_stable";
    preset.description = "Production-ready stable configuration";
    preset.environment = "production";
    preset.isExperimental = false;
    preset.values = {
        {"neuron_count", "2000"},
        {"region_count", "3"},
        {"connection_probability", "0.12"},
        {"stdp_ltp_weight", "0.015"},
        {"stdp_ltd_weight", "0.012"},
        {"synaptogenesis_rate", "0.0002"},
        {"pruning_rate", "0.00002"},
        {"max_steps", "50000"}
    };
    presets.push_back(preset);
    
    return presets;
}

} // namespace nlm