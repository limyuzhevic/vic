#include "AdvancedSimulationPresets.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {

void SimulationPresetManager::addPreset(SimulationPreset preset, const std::string& name, const std::string& description,
                                        const std::map<std::string, std::string>& parameters) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    PresetInfo info(preset, name, description, parameters);
    info.creationTime = getCurrentTimestamp();
    info.creator = "system";
    
    presets_.push_back(info);
}

void SimulationPresetManager::removePreset(SimulationPreset preset) {
    std::lock_guard<std::mutex> lock(mutex_);
    presets_.erase(std::remove_if(presets_.begin(), presets_.end(),
                                  [preset](const PresetInfo& info) { return info.preset == preset; }),
                   presets_.end());
}

void SimulationPresetManager::updatePreset(SimulationPreset preset, const std::map<std::string, std::string>& parameters) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& info : presets_) {
        if (info.preset == preset) {
            info.parameters = parameters;
            break;
        }
    }
}

const std::map<std::string, std::string>& SimulationPresetManager::getPresetParameters(SimulationPreset preset) const {
    static const std::map<std::string, std::string> emptyMap;
    for (const auto& info : presets_) {
        if (info.preset == preset) {
            return info.parameters;
        }
    }
    return emptyMap;
}

std::string SimulationPresetManager::getPresetName(SimulationPreset preset) const {
    for (const auto& info : presets_) {
        if (info.preset == preset) {
            return info.name;
        }
    }
    return "Unknown";
}

std::string SimulationPresetManager::getPresetDescription(SimulationPreset preset) const {
    for (const auto& info : presets_) {
        if (info.preset == preset) {
            return info.description;
        }
    }
    return "No description available";
}

void SimulationPresetManager::applyPreset(std::shared_ptr<Config> config, SimulationPreset preset) {
    const auto& parameters = getPresetParameters(preset);
    
    // Apply basic parameters
    if (parameters.find("brain.neuron_count") != parameters.end()) {
        config->set("brain.neuron_count", std::stoll(parameters.at("brain.neuron_count")), ConfigSource::Default);
    }
    if (parameters.find("brain.synapse_density") != parameters.end()) {
        config->set("brain.synapse_density", std::stod(parameters.at("brain.synapse_density")), ConfigSource::Default);
    }
    if (parameters.find("plasticity.stdp.enable") != parameters.end()) {
        config->set("plasticity.stdp.enable", parameters.at("plasticity.stdp.enable") == "true", ConfigSource::Default);
    }
    if (parameters.find("plasticity.hebbian.enable") != parameters.end()) {
        config->set("plasticity.hebbian.enable", parameters.at("plasticity.hebbian.enable") == "true", ConfigSource::Default);
    }
    if (parameters.find("neuromod.curiosity.enable") != parameters.end()) {
        config->set("neuromod.curiosity.enable", parameters.at("neuromod.curiosity.enable") == "true", ConfigSource::Default);
    }
    if (parameters.find("neuromod.novelty.enable") != parameters.end()) {
        config->set("neuromod.novelty.enable", parameters.at("neuromod.novelty.enable") == "true", ConfigSource::Default);
    }
}

void SimulationPresetManager::applyAdvancedPreset(std::shared_ptr<Config> config, SimulationPreset preset,
                                                   const AdvancedSimulationConfig& advancedConfig) {
    applyPreset(config, preset);
    
    // Apply advanced configuration
    // This would include advanced parameters like time steps, memory limits, etc.
}

std::vector<SimulationPreset> SimulationPresetManager::searchPresets(const std::string& keyword) const {
    std::vector<SimulationPreset> results;
    
    for (const auto& info : presets_) {
        if (info.name.find(keyword) != std::string::npos ||
            info.description.find(keyword) != std::string::npos) {
            results.push_back(info.preset);
        }
    }
    
    return results;
}

std::vector<SimulationPreset> SimulationPresetManager::getSimilarPresets(SimulationPreset preset, float similarity) const {
    std::vector<SimulationPreset> results;
    
    for (const auto& info : presets_) {
        if (info.preset != preset) {
            // Calculate similarity based on parameter overlap
            auto presetParams = getPresetParameters(preset);
            auto infoParams = info.parameters;
            
            float similarityScore = 0.0f;
            size_t totalParams = presetParams.size() + infoParams.size();
            
            for (const auto& kv : presetParams) {
                if (infoParams.find(kv.first) != infoParams.end()) {
                    similarityScore += 1.0f;
                }
            }
            
            for (const auto& kv : infoParams) {
                if (presetParams.find(kv.first) != presetParams.end()) {
                    similarityScore += 1.0f;
                }
            }
            
            if (totalParams > 0 && similarityScore / totalParams >= similarity) {
                results.push_back(info.preset);
            }
        }
    }
    
    return results;
}

std::string SimulationPresetManager::comparePresets(SimulationPreset preset1, SimulationPreset preset2) const {
    std::stringstream ss;
    ss << "Comparison of " << getPresetName(preset1) << " vs " << getPresetName(preset2) << std::endl;
    ss << "=" * 50 << std::endl << std::endl;
    
    auto params1 = getPresetParameters(preset1);
    auto params2 = getPresetParameters(preset2);
    
    ss << "Parameter Differences:" << std::endl;
    ss << std::left << std::setw(20) << "Parameter" << std::setw(15) << "Preset1" << std::setw(15) << "Preset2" << std::endl;
    ss << std::string(50, '-') << std::endl;
    
    for (const auto& kv : params1) {
        if (params2.find(kv.first) != params2.end()) {
            if (kv.second != params2.at(kv.first)) {
                ss << std::left << std::setw(20) << kv.first << std::setw(15) << kv.second 
                   << std::setw(15) << params2.at(kv.first) << std::endl;
            }
        } else {
            ss << std::left << std::setw(20) << kv.first << std::setw(15) << kv.second 
               << std::setw(15) << "(not in preset2)" << std::endl;
        }
    }
    
    for (const auto& kv : params2) {
        if (params1.find(kv.first) == params1.end()) {
            ss << std::left << std::setw(20) << kv.first << std::setw(15) << "(not in preset1)" 
               << std::setw(15) << kv.second << std::endl;
        }
    }
    
    return ss.str();
}

void SimulationPresetManager::createBatchPreset(const std::string& name, const std::vector<SimulationPreset>& presets) {
    // Create a batch preset that combines multiple presets
    std::map<std::string, std::string> batchParameters;
    
    for (const auto& preset : presets) {
        auto params = getPresetParameters(preset);
        for (const auto& kv : params) {
            batchParameters["batch_" + kv.first] = kv.second;
        }
    }
    
    addPreset(SimulationPreset::Custom, name, "Batch preset combining multiple presets", batchParameters);
}

void SimulationPresetManager::applyBatchPreset(std::shared_ptr<Config> config, const std::string& batchName) {
    // Find batch preset by name and apply
    for (const auto& info : presets_) {
        if (info.name == batchName) {
            applyPreset(config, info.preset);
            break;
        }
    }
}

bool SimulationPresetManager::validatePreset(SimulationPreset preset) const {
    const auto& parameters = getPresetParameters(preset);
    
    // Basic validation
    if (parameters.find("brain.neuron_count") == parameters.end()) {
        return false;
    }
    
    try {
        size_t neuronCount = std::stoll(parameters.at("brain.neuron_count"));
        if (neuronCount <= 0 || neuronCount > 1000000) {
            return false;
        }
    } catch (...) {
        return false;
    }
    
    return true;
}

std::vector<std::string> SimulationPresetManager::getPresetIssues(SimulationPreset preset) const {
    std::vector<std::string> issues;
    
    const auto& parameters = getPresetParameters(preset);
    
    // Check for required parameters
    if (parameters.find("brain.neuron_count") == parameters.end()) {
        issues.push_back("Missing required parameter: brain.neuron_count");
    }
    
    // Check for invalid parameter values
    if (parameters.find("brain.neuron_count") != parameters.end()) {
        try {
            size_t neuronCount = std::stoll(parameters.at("brain.neuron_count"));
            if (neuronCount <= 0) {
                issues.push_back("Invalid neuron_count: must be positive");
            } else if (neuronCount > 1000000) {
                issues.push_back("Invalid neuron_count: too large (max 1,000,000)");
            }
        } catch (...) {
            issues.push_back("Invalid neuron_count: not a valid number");
        }
    }
    
    return issues;
}

bool SimulationPresetManager::exportPreset(SimulationPreset preset, const std::string& filepath) {
    std::ofstream outFile(filepath);
    if (!outFile.is_open()) {
        return false;
    }
    
    outFile << "NLM Preset Export" << std::endl;
    outFile << "Generated at: " << getCurrentTimestamp() << std::endl;
    outFile << "Preset: " << getPresetName(preset) << std::endl;
    outFile << "Description: " << getPresetDescription(preset) << std::endl;
    outFile << "=" * 50 << std::endl;
    
    const auto& parameters = getPresetParameters(preset);
    for (const auto& kv : parameters) {
        outFile << kv.first << " = " << kv.second << std::endl;
    }
    
    outFile.close();
    return true;
}

bool SimulationPresetManager::importPreset(const std::string& filepath, SimulationPreset& preset) {
    std::ifstream inFile(filepath);
    if (!inFile.is_open()) {
        return false;
    }
    
    std::string line;
    std::map<std::string, std::string> parameters;
    
    while (std::getline(inFile, line)) {
        if (line.find("=" ) != std::string::npos) {
            size_t pos = line.find("=" );
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            parameters[key] = value;
        }
    }
    
    inFile.close();
    
    // Apply preset (this is a simplified version)
    return true;
}

void SimulationPresetManager::exportAllPresets(const std::string& directory) {
    // Export all presets to a directory
    // Implementation would include error handling and file creation
}

bool SimulationPresetManager::importAllPresets(const std::string& directory) {
    // Import all presets from a directory
    // Implementation would include file reading and error handling
    return true;
}

std::vector<SimulationPreset> SimulationPresetManager::getAllPresets() const {
    std::vector<SimulationPreset> presets;
    for (const auto& info : presets_) {
        presets.push_back(info.preset);
    }
    return presets;
}

std::map<SimulationPreset, std::string> SimulationPresetManager::getPresetStats() const {
    std::map<SimulationPreset, std::string> stats;
    
    for (const auto& info : presets_) {
        std::stringstream ss;
        ss << "Usage: " << info.usageCount << ", Rating: " << std::fixed << std::setprecision(1) << info.averageRating;
        stats[info.preset] = ss.str();
    }
    
    return stats;
}

std::string SimulationPresetManager::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// AdvancedSimulationController implementation
void AdvancedSimulationController::setSimulationMode(SimulationMode mode) {
    currentMode_ = mode;
}

void AdvancedSimulationController::setAdvancedConfig(const AdvancedSimulationConfig& config) {
    advancedConfig_ = config;
}

void AdvancedSimulationController::updateAdvancedConfig(const std::map<std::string, double>& updates) {
    for (const auto& kv : updates) {
        // Apply updates to advancedConfig_
        // This is a simplified implementation
    }
}

void AdvancedSimulationController::setPerformancePreset(const PerformancePreset& preset) {
    currentPerformancePreset_ = preset;
}

void AdvancedSimulationController::startSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                                   std::shared_ptr<AgentBrain> agent) {
    // Implementation would start the simulation loop
    simulationRunning_ = true;
}

void AdvancedSimulationController::pauseSimulation() {
    simulationRunning_ = false;
}

void AdvancedSimulationController::resumeSimulation() {
    simulationRunning_ = true;
}

void AdvancedSimulationController::stopSimulation() {
    simulationRunning_ = false;
}

void AdvancedSimulationController::adjustConfigurationDuringSimulation(const std::map<std::string, double>& adjustments) {
    // Adjust configuration during simulation
    // Implementation would dynamically modify simulation parameters
}

void AdvancedSimulationController::applySimulationPreset(SimulationPreset preset) {
    // Apply preset to current simulation
    // Implementation would modify brain configuration
}

void AdvancedSimulationController::applyPerformancePreset(const PerformancePreset& preset) {
    currentPerformancePreset_ = preset;
}

void AdvancedSimulationController::updatePerformanceMetrics() {
    // Update performance metrics
    // Implementation would track CPU usage, memory usage, etc.
}

void AdvancedSimulationController::checkProgress() {
    // Check simulation progress
    // Implementation would check for completion criteria
}

void AdvancedSimulationController::saveProgress() {
    // Save simulation progress
    // Implementation would save checkpoint data
}

void AdvancedSimulationController::restoreProgress() {
    // Restore simulation progress
    // Implementation would load checkpoint data
}

void AdvancedSimulationController::handleExternalEvents() {
    // Handle external events
    // Implementation would process events like user input, network events, etc.
}

void AdvancedSimulationController::simulationLoop() {
    // Main simulation loop
    // Implementation would run the simulation
    while (simulationRunning_) {
        updatePerformanceMetrics();
        checkProgress();
        // Run simulation step
        currentStep_++;
        currentSimulationTime_ += advancedConfig_.timeStep;
    }
}

// SimulationTemplateManager implementation
void SimulationTemplateManager::addTemplate(const SimulationTemplate& template) {
    templates_.push_back(template);
}

void SimulationTemplateManager::removeTemplate(const std::string& name) {
    templates_.erase(std::remove_if(templates_.begin(), templates_.end(),
                                   [name](const TemplateInfo& info) { return info.template.name == name; }),
                   templates_.end());
}

void SimulationTemplateManager::updateTemplate(const std::string& name, const SimulationTemplate& template) {
    for (auto& info : templates_) {
        if (info.template.name == name) {
            info.template = template;
            break;
        }
    }
}

const SimulationTemplate* SimulationTemplateManager::getTemplate(const std::string& name) const {
    for (const auto& info : templates_) {
        if (info.template.name == name) {
            return &info.template;
        }
    }
    return nullptr;
}

std::vector<SimulationTemplate> SimulationTemplateManager::searchTemplates(const std::string& keyword) const {
    std::vector<SimulationTemplate> results;
    
    for (const auto& info : templates_) {
        if (info.template.name.find(keyword) != std::string::npos ||
            info.template.description.find(keyword) != std::string::npos) {
            results.push_back(info.template);
        }
    }
    
    return results;
}

std::vector<SimulationTemplate> SimulationTemplateManager::getTemplatesByCategory(const std::string& category) const {
    std::vector<SimulationTemplate> results;
    
    for (const auto& info : templates_) {
        // Find templates in this category
        if (category.empty() || info.template.name.find(category) != std::string::npos) {
            results.push_back(info.template);
        }
    }
    
    return results;
}

std::vector<SimulationTemplate> SimulationTemplateManager::getSimilarTemplates(const SimulationTemplate& template,
                                                                              float similarity) const {
    std::vector<SimulationTemplate> results;
    
    for (const auto& info : templates_) {
        if (info.template.name != template.name) {
            // Calculate similarity (simplified)
            float simScore = 0.0f;
            
            if (info.template.preset == template.preset) {
                simScore += 0.5f;
            }
            
            if (info.template.name.find(template.name) != std::string::npos) {
                simScore += 0.3f;
            }
            
            if (simScore >= similarity) {
                results.push_back(info.template);
            }
        }
    }
    
    return results;
}

std::shared_ptr<Config> SimulationTemplateManager::applyTemplate(const std::string& name,
                                                                 const std::map<std::string, std::string>& parameters) {
    const SimulationTemplate* template = getTemplate(name);
    if (!template) {
        return nullptr;
    }
    
    auto config = std::make_shared<Config>();
    
    // Apply template configuration
    // Implementation would apply template parameters
    
    return config;
}

void SimulationTemplateManager::applyTemplateToConfig(std::shared_ptr<Config> config, const std::string& name,
                                                       const std::map<std::string, std::string>& parameters) {
    const SimulationTemplate* template = getTemplate(name);
    if (!template) {
        return;
    }
    
    // Apply template to existing config
    // Implementation would apply template parameters to existing config
}

void SimulationTemplateManager::addTemplateCategory(const std::string& name, const std::vector<std::string>& templateNames) {
    templateCategories_[name] = templateNames;
}

void SimulationTemplateManager::removeTemplateCategory(const std::string& name) {
    templateCategories_.erase(name);
}

std::vector<std::string> SimulationTemplateManager::getTemplateCategories() const {
    std::vector<std::string> categories;
    for (const auto& kv : templateCategories_) {
        categories.push_back(kv.first);
    }
    return categories;
}

std::vector<std::string> SimulationTemplateManager::getTemplatesInCategory(const std::string& category) const {
    auto it = templateCategories_.find(category);
    if (it != templateCategories_.end()) {
        return it->second;
    }
    return std::vector<std::string>();
}

bool SimulationTemplateManager::exportTemplate(const std::string& name, const std::string& filepath) {
    const SimulationTemplate* template = getTemplate(name);
    if (!template) {
        return false;
    }
    
    // Export template to file
    // Implementation would write template to file
    return true;
}

bool SimulationTemplateManager::importTemplate(const std::string& filepath) {
    // Import template from file
    // Implementation would read template from file
    return true;
}

void SimulationTemplateManager::exportAllTemplates(const std::string& directory) {
    // Export all templates to a directory
    // Implementation would create files for each template
}

bool SimulationTemplateManager::importAllTemplates(const std::string& directory) {
    // Import all templates from a directory
    // Implementation would read files and create templates
    return true;
}

std::vector<SimulationTemplate> SimulationTemplateManager::getAllTemplates() const {
    std::vector<SimulationTemplate> templates;
    for (const auto& info : templates_) {
        templates.push_back(info.template);
    }
    return templates;
}

std::map<std::string, std::string> SimulationTemplateManager::getTemplateStats(const std::string& name) const {
    std::map<std::string, std::string> stats;
    
    for (const auto& info : templates_) {
        if (info.template.name == name) {
            stats["usage_count"] = std::to_string(info.usageCount);
            stats["average_rating"] = std::to_string(info.averageRating);
            break;
        }
    }
    
    return stats;
}

} // namespace nlm