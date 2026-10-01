// Implementation of advanced configuration options and presets

#include "AdvancedConfig.hpp"
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <numeric>

namespace nlm {

// AdvancedConfigManager implementation
struct AdvancedConfigManager::Impl {
    std::string configDirectory;
    std::map<std::string, ConfigPreset> presets;
    std::map<std::string, std::string> presetFilenames;
    std::string backupDirectory;
    bool autoSave;
    size_t maxBackups;
};

AdvancedConfigManager::AdvancedConfigManager() : pImpl(std::make_unique<Impl>()) {
    pImpl->configDirectory = "configs/presets";
    pImpl->backupDirectory = "configs/backups";
    pImpl->autoSave = true;
    pImpl->maxBackups = 10;
    
    // Create directories if they don't exist
    std::filesystem::create_directories(pImpl->configDirectory);
    std::filesystem::create_directories(pImpl->backupDirectory);
}

AdvancedConfigManager::~AdvancedConfigManager() = default;

bool AdvancedConfigManager::loadPresets(const std::string& filepath) {
    if (filepath.empty()) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple INI-like parser for presets
    std::string line;
    ConfigPreset currentPreset;
    bool inPreset = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        if (line.find("[") == 0 && line.find("]") != std::string::npos) {
            // New preset section
            if (inPreset) {
                // Save previous preset
                addPreset(currentPreset);
            }
            
            // Extract preset name
            size_t start = line.find("[") + 1;
            size_t end = line.find("]");
            std::string name = line.substr(start, end - start);
            
            currentPreset = ConfigPreset(name, "");
            inPreset = true;
        } else if (inPreset) {
            if (line.find("description =") == 0) {
                size_t pos = line.find("=");
                currentPreset.description = line.substr(pos + 1);
                trim(currentPreset.description);
            } else if (line.find("category =") == 0) {
                size_t pos = line.find("=");
                std::string categoryStr = line.substr(pos + 1);
                trim(categoryStr);
                try {
                    currentPreset.category = static_cast<ConfigPresetCategory>(std::stoi(categoryStr));
                } catch (...) {
                    currentPreset.category = ConfigPresetCategory::Standard;
                }
            } else if (line.find("version =") == 0) {
                size_t pos = line.find("=");
                currentPreset.version = std::stod(line.substr(pos + 1));
            } else if (line.find("setting =") == 0) {
                size_t pos = line.find("=");
                size_t secondPos = line.find(",", pos);
                if (secondPos != std::string::npos) {
                    std::string key = line.substr(pos + 1, secondPos - pos - 1);
                    std::string value = line.substr(secondPos + 1);
                    trim(key);
                    trim(value);
                    
                    std::string note;
                    size_t notePos = value.find(" #");
                    if (notePos != std::string::npos) {
                        note = value.substr(notePos + 2);
                        value = value.substr(0, notePos);
                    }
                    
                    currentPreset.addSetting(key, value, note);
                }
            }
        }
    }
    
    if (inPreset) {
        addPreset(currentPreset);
    }
    
    return true;
}

bool AdvancedConfigManager::savePresets(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& pair : pImpl->presets) {
        const auto& preset = pair.second;
        file << "[" << preset.name << "]\n";
        file << "description = " << preset.description << "\n";
        file << "category = " << static_cast<int>(preset.category) << "\n";
        file << "version = " << preset.version << "\n";
        
        for (const auto& settingPair : preset.settings) {
            file << "setting = " << settingPair.first << ", " << settingPair.second;
            if (preset.notes.find(settingPair.first) != preset.notes.end()) {
                file << " # " << preset.notes.at(settingPair.first);
            }
            file << "\n";
        }
        file << "\n";
    }
    
    return true;
}

void AdvancedConfigManager::addPreset(const ConfigPreset& preset) {
    pImpl->presets[preset.name] = preset;
    pImpl->presetFilenames[preset.name] = pImpl->configDirectory + "/" + preset.name + ".cfg";
    
    // Auto-save if enabled
    if (pImpl->autoSave) {
        savePresetToFile(preset);
    }
}

ConfigPreset AdvancedConfigManager::getPreset(const std::string& name) const {
    auto it = pImpl->presets.find(name);
    if (it != pImpl->presets.end()) {
        return it->second;
    }
    
    throw std::runtime_error("Preset not found: " + name);
}

bool AdvancedConfigManager::hasPreset(const std::string& name) const {
    return pImpl->presets.find(name) != pImpl->presets.end();
}

void AdvancedConfigManager::removePreset(const std::string& name) {
    auto it = pImpl->presets.find(name);
    if (it != pImpl->presets.end()) {
        pImpl->presets.erase(it);
        
        auto fileIt = pImpl->presetFilenames.find(name);
        if (fileIt != pImpl->presetFilenames.end()) {
            std::filesystem::remove(fileIt->second);
            pImpl->presetFilenames.erase(fileIt);
        }
        
        // Auto-save if enabled
        if (pImpl->autoSave) {
            saveAllPresets();
        }
    }
}

std::vector<std::string> AdvancedConfigManager::getPresetNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->presets.size());
    for (const auto& pair : pImpl->presets) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<ConfigPreset> AdvancedConfigManager::getPresetsByCategory(ConfigPresetCategory category) const {
    std::vector<ConfigPreset> presets;
    for (const auto& pair : pImpl->presets) {
        if (pair.second.category == category) {
            presets.push_back(pair.second);
        }
    }
    return presets;
}

std::shared_ptr<Config> AdvancedConfigManager::createConfigFromPreset(const std::string& presetName) const {
    ConfigPreset preset = getPreset(presetName);
    auto config = std::make_shared<Config>();
    
    // Apply preset settings
    for (const auto& pair : preset.settings) {
        config->set(pair.first, pair.second);
    }
    
    return config;
}

void AdvancedConfigManager::mergeWithPreset(std::shared_ptr<Config> config, const std::string& presetName) const {
    ConfigPreset preset = getPreset(presetName);
    
    for (const auto& pair : preset.settings) {
        config->set(pair.first, pair.second);
    }
}

ConfigComparison::ComparisonResult AdvancedConfigManager::comparePresets(
    const std::string& preset1, const std::string& preset2) const {
    ConfigComparison::ComparisonResult result;
    
    if (!hasPreset(preset1) || !hasPreset(preset2)) {
        return result;
    }
    
    const auto& preset1Data = pImpl->presets.at(preset1);
    const auto& preset2Data = pImpl->presets.at(preset2);
    
    // Compare settings
    for (const auto& pair : preset1Data.settings) {
        const std::string& key = pair.first;
        const std::string& value1 = pair.second;
        
        auto it = preset2Data.settings.find(key);
        if (it != preset2Data.settings.end()) {
            const std::string& value2 = it->second;
            if (value1 == value2) {
                result.similar[key] = std::make_pair(value1, value2);
            } else {
                result.different[key] = std::make_pair(value1, value2);
            }
        } else {
            result.onlyInFirst[key] = value1;
        }
    }
    
    for (const auto& pair : preset2Data.settings) {
        const std::string& key = pair.first;
        if (preset1Data.settings.find(key) == preset1Data.settings.end()) {
            result.onlyInSecond[key] = pair.second;
        }
    }
    
    // Calculate similarity score
    size_t totalSettings = preset1Data.settings.size() + preset2Data.settings.size();
    size_t matchingSettings = result.similar.size();
    
    if (totalSettings > 0) {
        result.similarityScore = static_cast<double>(matchingSettings) / totalSettings;
    }
    
    return result;
}

std::vector<std::string> AdvancedConfigManager::getPresetDifferences(
    const std::string& preset1, const std::string& preset2) const {
    std::vector<std::string> differences;
    
    if (!hasPreset(preset1) || !hasPreset(preset2)) {
        return differences;
    }
    
    const auto& preset1Data = pImpl->presets.at(preset1);
    const auto& preset2Data = pImpl->presets.at(preset2);
    
    // Add differences from preset1
    for (const auto& pair : preset1Data.settings) {
        const std::string& key = pair.first;
        auto it = preset2Data.settings.find(key);
        if (it == preset2Data.settings.end()) {
            differences.push_back("Only in " + preset1 + ": " + key + " = " + pair.second);
        } else if (pair.second != it->second) {
            differences.push_back("Different: " + key + " (" + preset1 + ": " + pair.second + 
                                ", " + preset2 + ": " + it->second + ")");
        }
    }
    
    // Add differences from preset2
    for (const auto& pair : preset2Data.settings) {
        const std::string& key = pair.first;
        if (preset1Data.settings.find(key) == preset1Data.settings.end()) {
            differences.push_back("Only in " + preset2 + ": " + key + " = " + pair.second);
        }
    }
    
    return differences;
}

bool AdvancedConfigManager::validatePreset(const ConfigPreset& preset) const {
    if (preset.name.empty()) {
        return false;
    }
    
    if (preset.description.empty()) {
        return false;
    }
    
    // Check for duplicate keys with different values
    std::map<std::string, std::string> seen;
    for (const auto& pair : preset.settings) {
        if (seen.find(pair.first) != seen.end() && seen[pair.first] != pair.second) {
            return false; // Duplicate key with different value
        }
        seen[pair.first] = pair.second;
    }
    
    return true;
}

std::string AdvancedConfigManager::generatePresetName(const std::string& baseName) const {
    std::string name = baseName;
    size_t counter = 1;
    
    while (hasPreset(name)) {
        name = baseName + "_" + std::to_string(counter);
        counter++;
    }
    
    return name;
}

void AdvancedConfigManager::savePresetToFile(const ConfigPreset& preset) const {
    std::string filepath = pImpl->configDirectory + "/" + preset.name + ".cfg";
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    file << "[" << preset.name << "]\n";
    file << "description = " << preset.description << "\n";
    file << "category = " << static_cast<int>(preset.category) << "\n";
    file << "version = " << preset.version << "\n";
    
    for (const auto& pair : preset.settings) {
        file << "setting = " << pair.first << ", " << pair.second;
        if (preset.notes.find(pair.first) != preset.notes.end()) {
            file << " # " << preset.notes.at(pair.first);
        }
        file << "\n";
    }
}

void AdvancedConfigManager::saveAllPresets() const {
    for (const auto& pair : pImpl->presets) {
        savePresetToFile(pair.second);
    }
}

// ConfigTemplate implementation
struct ConfigTemplate::Impl {
    std::string name;
    std::string description;
    std::map<std::string, ParameterDefinition> parameters;
};

ConfigTemplate::ConfigTemplate() : pImpl(std::make_unique<Impl>()) {}
ConfigTemplate::~ConfigTemplate() = default;

void ConfigTemplate::setName(const std::string& name) {
    pImpl->name = name;
}

void ConfigTemplate::setDescription(const std::string& description) {
    pImpl->description = description;
}

void ConfigTemplate::addParameter(const std::string& name, const std::string& type,
                                 const std::string& defaultValue, const std::string& description,
                                 bool required, const std::string& validation) {
    ParameterDefinition param;
    param.name = name;
    param.type = type;
    param.defaultValue = defaultValue;
    param.description = description;
    param.required = required;
    param.validation = validation;
    
    pImpl->parameters[name] = param;
}

ConfigTemplate::ParameterDefinition ConfigTemplate::getParameter(const std::string& name) const {
    auto it = pImpl->parameters.find(name);
    if (it != pImpl->parameters.end()) {
        return it->second;
    }
    
    ParameterDefinition param;
    param.name = name;
    param.required = false;
    return param;
}

bool ConfigTemplate::hasParameter(const std::string& name) const {
    return pImpl->parameters.find(name) != pImpl->parameters.end();
}

std::vector<ConfigTemplate::ParameterDefinition> ConfigTemplate::getAllParameters() const {
    std::vector<ParameterDefinition> params;
    params.reserve(pImpl->parameters.size());
    for (const auto& pair : pImpl->parameters) {
        params.push_back(pair.second);
    }
    return params;
}

bool ConfigTemplate::validateParameter(const std::string& name, const std::string& value) const {
    auto it = pImpl->parameters.find(name);
    if (it == pImpl->parameters.end()) {
        return false; // Parameter doesn't exist
    }
    
    const auto& param = it->second;
    
    // Type validation
    if (param.type == "int") {
        try {
            std::stoi(value);
        } catch (...) {
            return false;
        }
    } else if (param.type == "double") {
        try {
            std::stod(value);
        } catch (...) {
            return false;
        }
    } else if (param.type == "bool") {
        if (value != "true" && value != "false") {
            return false;
        }
    } else if (param.type == "string") {
        // Any string is valid
    }
    
    // Validation rules
    if (!param.validation.empty()) {
        // Simple validation checks
        if (param.validation == "positive") {
            double val = std::stod(value);
            if (val <= 0) return false;
        } else if (param.validation == "non-negative") {
            double val = std::stod(value);
            if (val < 0) return false;
        } else if (param.validation == "range(0,100)") {
            double val = std::stod(value);
            if (val <= 0 || val >= 100) return false;
        }
    }
    
    return true;
}

ConfigValue ConfigTemplate::parameterToConfigValue(const std::string& name, const std::string& value) const {
    auto it = pImpl->parameters.find(name);
    if (it == pImpl->parameters.end()) {
        return ConfigValue(); // Empty variant
    }
    
    const auto& param = it->second;
    
    // Convert based on type
    if (param.type == "int") {
        return std::stoi(value);
    } else if (param.type == "double") {
        return std::stod(value);
    } else if (param.type == "bool") {
        return (value == "true");
    } else {
        return value; // string
    }
}

std::shared_ptr<Config> ConfigTemplate::generateConfig(
    const std::map<std::string, std::string>& parameters) const {
    auto config = std::make_shared<Config>();
    
    for (const auto& pair : parameters) {
        if (hasParameter(pair.first)) {
            ConfigValue value = parameterToConfigValue(pair.first, pair.second);
            config->set(pair.first, value);
        }
    }
    
    return config;
}

std::string ConfigTemplate::exportTemplate() const {
    std::ostringstream oss;
    oss << "Template: " << pImpl->name << "\n";
    oss << "Description: " << pImpl->description << "\n";
    oss << "\nParameters:\n";
    
    for (const auto& pair : pImpl->parameters) {
        const auto& param = pair.second;
        oss << "  " << param.name << " (" << param.type << ")";
        oss << " = " << param.defaultValue << " - " << param.description;
        if (param.required) {
            oss << " [REQUIRED]";
        }
        if (!param.validation.empty()) {
            oss << " [VALIDATION: " << param.validation << "]";
        }
        oss << "\n";
    }
    
    return oss.str();
}

bool ConfigTemplate::importTemplate(const std::string& templateString) {
    // Simple parser for template format
    std::istringstream stream(templateString);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        if (line.find("Template: ") == 0) {
            pImpl->name = line.substr(10);
        } else if (line.find("Description: ") == 0) {
            pImpl->description = line.substr(12);
        } else if (line.find("Parameters:") == 0) {
            continue; // Already processed
        } else if (line.find("  ") == 0) {
            // Parameter line
            size_t nameEnd = line.find(" (");
            if (nameEnd != std::string::npos) {
                ParameterDefinition param;
                param.name = line.substr(2, nameEnd - 2);
                
                size_t typeEnd = line.find(")");
                if (typeEnd != std::string::npos) {
                    param.type = line.substr(nameEnd + 2, typeEnd - nameEnd - 2);
                }
                
                size_t equalsPos = line.find(" = ");
                if (equalsPos != std::string::npos) {
                    size_t dashPos = line.find(" - ", equalsPos);
                    if (dashPos != std::string::npos) {
                        param.defaultValue = line.substr(equalsPos + 3, dashPos - equalsPos - 3);
                        param.description = line.substr(dashPos + 3);
                        
                        // Check for required tag
                        if (param.description.find(" [REQUIRED]") != std::string::npos) {
                            param.required = true;
                            param.description.erase(param.description.find(" [REQUIRED]"), 12);
                        }
                        
                        // Check for validation tag
                        size_t validationStart = param.description.find(" [VALIDATION: ");
                        if (validationStart != std::string::npos) {
                            size_t validationEnd = param.description.find("]", validationStart);
                            if (validationEnd != std::string::npos) {
                                param.validation = param.description.substr(validationStart + 14, validationEnd - validationStart - 14);
                                param.description.erase(validationStart, validationEnd - validationStart + 1);
                            }
                        }
                    }
                }
                
                pImpl->parameters[param.name] = param;
            }
        }
    }
    
    return !pImpl->name.empty();
}

std::string ConfigTemplate::getName() const {
    return pImpl->name;
}

std::string ConfigTemplate::getDescription() const {
    return pImpl->description;
}

// ConfigPresetFactory implementation
struct ConfigPresetFactory::Impl {};

ConfigPresetFactory::ConfigPresetFactory() : pImpl(std::make_unique<Impl>()) {}
ConfigPresetFactory::~ConfigPresetFactory() = default;

ConfigPreset ConfigPresetFactory::createDevelopmentPreset() {
    ConfigPreset preset("development", "Development configuration for neural simulation");
    preset.addSetting("neuron_count", "100", "Number of neurons for development testing");
    preset.addSetting("simulation_timestep", "0.01", "Simulation timestep");
    preset.addSetting("stdp_ltp_weight", "0.01", "STDP LTP weight");
    preset.addSetting("stdp_ltd_weight", "0.012", "STDP LTD weight");
    preset.addSetting("enable_curiosity", "true", "Enable curiosity module");
    preset.addSetting("enable_reward_modulation", "true", "Enable reward modulation");
    preset.addSetting("enable_development", "true", "Enable developmental processes");
    preset.addSetting("checkpoint_interval", "1000", "Checkpoint save interval");
    preset.addSetting("max_steps", "5000", "Maximum simulation steps");
    return preset;
}

ConfigPreset ConfigPresetFactory::createPerformancePreset() {
    ConfigPreset preset("performance", "Performance-optimized configuration");
    preset.addSetting("neuron_count", "1000", "Number of neurons for performance testing");
    preset.addSetting("simulation_timestep", "0.001", "High-resolution simulation timestep");
    preset.addSetting("stdp_ltp_weight", "0.005", "Conservative STDP LTP weight");
    preset.addSetting("stdp_ltd_weight", "0.006", "Conservative STDP LTD weight");
    preset.addSetting("enable_curiosity", "false", "Disable curiosity for speed");
    preset.addSetting("enable_reward_modulation", "true", "Enable reward modulation");
    preset.addSetting("enable_development", "false", "Disable development for speed");
    preset.addSetting("checkpoint_interval", "10000", "Infrequent checkpointing");
    preset.addSetting("max_steps", "20000", "Long simulation for performance");
    return preset;
}

ConfigPreset ConfigPresetFactory::createResearchPreset() {
    ConfigPreset preset("research", "Research configuration for scientific experiments");
    preset.addSetting("neuron_count", "5000", "Large scale research simulation");
    preset.addSetting("simulation_timestep", "0.0005", "Very high-resolution timestep");
    preset.addSetting("stdp_ltp_weight", "0.001", "Precise STDP LTP weight");
    preset.addSetting("stdp_ltd_weight", "0.0012", "Precise STDP LTD weight");
    preset.addSetting("enable_curiosity", "true", "Enable curiosity for exploration");
    preset.addSetting("enable_reward_modulation", "true", "Enable reward modulation");
    preset.addSetting("enable_development", "true", "Enable developmental processes");
    preset.addSetting("enable_structural_plasticity", "true", "Enable structural plasticity");
    preset.addSetting("checkpoint_interval", "5000", "Frequent checkpointing for research");
    preset.addSetting("max_steps", "100000", "Very long simulation for research");
    return preset;
}

ConfigPreset ConfigPresetFactory::createEducationalPreset() {
    ConfigPreset preset("educational", "Educational configuration for learning");
    preset.addSetting("neuron_count", "50", "Small simulation for educational purposes");
    preset.addSetting("simulation_timestep", "0.01", "Simple timestep for educational use");
    preset.addSetting("stdp_ltp_weight", "0.02", "Simple STDP LTP weight");
    preset.addSetting("stdp_ltd_weight", "0.03", "Simple STDP LTD weight");
    preset.addSetting("enable_curiosity", "true", "Enable curiosity for engagement");
    preset.addSetting("enable_reward_modulation", "true", "Enable reward modulation");
    preset.addSetting("enable_development", "true", "Enable developmental processes");
    preset.addSetting("checkpoint_interval", "500", "Frequent checkpointing for safety");
    preset.addSetting("max_steps", "2000", "Short simulation for educational use");
    return preset;
}

ConfigPreset ConfigPresetFactory::createStandardPreset() {
    ConfigPreset preset("standard", "Standard configuration for general use");
    preset.addSetting("neuron_count", "1000", "Standard number of neurons");
    preset.addSetting("simulation_timestep", "0.001", "Standard simulation timestep");
    preset.addSetting("stdp_ltp_weight", "0.01", "Standard STDP LTP weight");
    preset.addSetting("stdp_ltd_weight", "0.012", "Standard STDP LTD weight");
    preset.addSetting("enable_curiosity", "true", "Enable curiosity");
    preset.addSetting("enable_reward_modulation", "true", "Enable reward modulation");
    preset.addSetting("enable_development", "true", "Enable developmental processes");
    preset.addSetting("checkpoint_interval", "10000", "Standard checkpoint interval");
    preset.addSetting("max_steps", "10000", "Standard maximum steps");
    return preset;
}

ConfigPreset ConfigPresetFactory::createCustomPreset(const std::string& name, const std::string& description) {
    return ConfigPreset(name, description);
}

ConfigPreset ConfigPresetFactory::createHighPerformancePreset() {
    ConfigPreset preset("high_performance", "High-performance configuration");
    preset.addSetting("neuron_count", "2000", "Moderate number of neurons");
    preset.addSetting("simulation_timestep", "0.001", "Standard timestep");
    preset.addSetting("stdp_ltp_weight", "0.002", "Low STDP weight for stability");
    preset.addSetting("stdp_ltd_weight", "0.003", "Low STDP weight for stability");
    preset.addSetting("enable_curiosity", "false", "Disable curiosity for speed");
    preset.addSetting("enable_reward_modulation", "false", "Disable reward modulation for speed");
    preset.addSetting("enable_development", "false", "Disable development for speed");
    preset.addSetting("checkpoint_interval", "50000", "Infrequent checkpointing");
    preset.addSetting("max_steps", "50000", "Long simulation");
    return preset;
}

ConfigPreset ConfigPresetFactory::createMemoryEfficientPreset() {
    ConfigPreset preset("memory_efficient", "Memory-efficient configuration");
    preset.addSetting("neuron_count", "200", "Small number of neurons");
    preset.addSetting("simulation_timestep", "0.01", "Coarse timestep");
    preset.addSetting("stdp_ltp_weight", "0.015", "Moderate STDP weight");
    preset.addSetting("stdp_ltd_weight", "0.018", "Moderate STDP weight");
    preset.addSetting("enable_curiosity", "true", "Enable curiosity for behavior");
    preset.addSetting("enable_reward_modulation", "true", "Enable reward modulation");
    preset.addSetting("enable_development", "true", "Enable development for adaptation");
    preset.addSetting("checkpoint_interval", "2000", "Moderate checkpointing");
    preset.addSetting("max_steps", "10000", "Medium simulation");
    return preset;
}

ConfigPreset ConfigPresetFactory::createAccuratePreset() {
    ConfigPreset preset("accurate", "High-accuracy configuration");
    preset.addSetting("neuron_count", "3000", "Large number of neurons for accuracy");
    preset.addSetting("simulation_timestep", "0.0005", "Very fine timestep for accuracy");
    preset.addSetting("stdp_ltp_weight", "0.005", "Moderate STDP weight");
    preset.addSetting("stdp_ltd_weight", "0.008", "Moderate STDP weight");
    preset.addSetting("enable_curiosity", "true", "Enable curiosity");
    preset.addSetting("enable_reward_modulation", "true", "Enable reward modulation");
    preset.addSetting("enable_development", "true", "Enable development");
    preset.addSetting("checkpoint_interval", "1000", "Frequent checkpointing for accuracy");
    preset.addSetting("max_steps", "50000", "Very long simulation for accuracy");
    return preset;
}

ConfigPreset ConfigPresetFactory::createFastPreset() {
    ConfigPreset preset("fast", "Fast simulation configuration");
    preset.addSetting("neuron_count", "100", "Small number of neurons");
    preset.addSetting("simulation_timestep", "0.01", "Coarse timestep");
    preset.addSetting("stdp_ltp_weight", "0.03", "High STDP weight");
    preset.addSetting("stdp_ltd_weight", "0.04", "High STDP weight");
    preset.addSetting("enable_curiosity", "false", "Disable curiosity for speed");
    preset.addSetting("enable_reward_modulation", "false", "Disable reward modulation for speed");
    preset.addSetting("enable_development", "false", "Disable development for speed");
    preset.addSetting("checkpoint_interval", "10000", "Infrequent checkpointing");
    preset.addSetting("max_steps", "1000", "Very short simulation");
    return preset;
}

std::vector<std::string> ConfigPresetFactory::getBuiltInPresetNames() {
    return {"development", "performance", "research", "educational", "standard", 
            "high_performance", "memory_efficient", "accurate", "fast"};
}

bool ConfigPresetFactory::validatePresetCompatibility(const ConfigPreset& preset, const std::string& targetVersion) {
    // Simple version compatibility check
    double presetVersion = std::stod(preset.version);
    double targetVersionNum = std::stod(targetVersion);
    
    // Allow for version compatibility (allow slight differences)
    return std::abs(presetVersion - targetVersionNum) <= 0.1;
}

std::vector<std::string> ConfigPresetFactory::getPresetRecommendations(const std::string& useCase) {
    std::vector<std::string> recommendations;
    
    if (useCase == "development" || useCase == "testing") {
        recommendations = {"development", "educational"};
    } else if (useCase == "production" || useCase == "performance") {
        recommendations = {"performance", "high_performance", "memory_efficient"};
    } else if (useCase == "research") {
        recommendations = {"research", "accurate"};
    } else if (useCase == "education" || useCase == "learning") {
        recommendations = {"educational", "standard"};
    } else {
        recommendations = {"standard"};
    }
    
    return recommendations;
}

} // namespace nlm
