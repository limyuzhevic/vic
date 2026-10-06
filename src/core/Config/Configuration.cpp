#include "Configuration.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>
#include <filesystem>

using json = nlohmann::json;

namespace nlm {

class Configuration::Impl {
public:
    std::shared_ptr<Config> config;
    std::string schemaFile;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&, const ConfigValue&)>> validationRules;
    
    // Advanced configuration sections
    ResearchMode researchMode;
    MemoryManagementConfig memoryManagementConfig;
    PerformanceMonitoringConfig performanceMonitoringConfig;
    AdvancedSimulationConfig advancedSimulationConfig;
    
    // Research presets
    std::unordered_map<std::string, std::shared_ptr<Config>> researchPresets;
    
    Impl() {
        config = std::make_shared<Config>();
        researchMode = ResearchMode::Basic;
        initializeDefaultMemoryManagementConfig();
        initializeDefaultPerformanceMonitoringConfig();
        initializeDefaultAdvancedSimulationConfig();
        initializeResearchPresets();
        initializeDefaultValidationRules();
    }
    
    void initializeDefaultMemoryManagementConfig() {
        memoryManagementConfig.maxWorkingMemoryCapacity = 1000;
        memoryManagementConfig.maxEpisodicMemoryCapacity = 10000;
        memoryManagementConfig.consolidationInterval = 1000;
        memoryManagementConfig.consolidationThreshold = 0.8f;
        memoryManagementConfig.enableReplay = true;
        memoryManagementConfig.enableCheckpointing = true;
        memoryManagementConfig.checkpointPath = "./checkpoints";
        memoryManagementConfig.autoSaveEnabled = true;
        memoryManagementConfig.memoryDecayRate = 0.001f;
    }
    
    void initializeDefaultPerformanceMonitoringConfig() {
        performanceMonitoringConfig.enableBenchmarking = true;
        performanceMonitoringConfig.enableProfiling = true;
        performanceMonitoringConfig.enableRealTimeMonitoring = false;
        performanceMonitoringConfig.benchmarkIntervalMs = 1000;
        performanceMonitoringConfig.profileIntervalMs = 5000;
        performanceMonitoringConfig.metricsOutputPath = "./metrics.json";
        performanceMonitoringConfig.exportMetricsToFile = true;
        performanceMonitoringConfig.enableVisualization = true;
        performanceMonitoringConfig.visualizationUpdateIntervalMs = 100;
    }
    
    void initializeDefaultAdvancedSimulationConfig() {
        advancedSimulationConfig.researchMode = ResearchMode::Basic;
        advancedSimulationConfig.enableNeuromodulation = true;
        advancedSimulationConfig.enableDevelopment = true;
        advancedSimulationConfig.enableStructuralPlasticity = true;
        advancedSimulationConfig.enableRewardModulation = true;
        advancedSimulationConfig.enableCuriosity = true;
        advancedSimulationConfig.enableNoveltyDetection = true;
        advancedSimulationConfig.neuromodulationStrength = 1.0f;
        advancedSimulationConfig.developmentRate = 0.01f;
        advancedSimulationConfig.plasticityRate = 0.1f;
        advancedSimulationConfig.rewardSensitivity = 1.0f;
        advancedSimulationConfig.explorationBonus = 0.5f;
        advancedSimulationConfig.noveltySensitivity = 1.0f;
    }
    
    void initializeResearchPresets() {
        // Create research preset for intensive testing
        auto intensivePreset = std::make_shared<Config>();
        intensivePreset->set("neuron_count", 2000);
        intensivePreset->set("region_count", 3);
        intensivePreset->set("connection_probability", 0.15);
        intensivePreset->set("simulation_timestep", 0.0005);
        intensivePreset->set("plasticity_stdp_enable", true);
        intensivePreset->set("plasticity_hebbian_enable", true);
        intensivePreset->set("plasticity_structural_enable", true);
        intensivePreset->set("neuromod_curiosity_enable", true);
        intensivePreset->set("neuromod_novelty_enable", true);
        intensivePreset->set("research_mode", static_cast<int>(ResearchMode::Intensive));
        researchPresets["intensive_testing"] = intensivePreset;
        
        // Create research preset for long-term studies
        auto longTermPreset = std::make_shared<Config>();
        longTermPreset->set("neuron_count", 500);
        longTermPreset->set("region_count", 1);
        longTermPreset->set("connection_probability", 0.05);
        longTermPreset->set("simulation_timestep", 0.001);
        longTermPreset->set("plasticity_stdp_enable", true);
        longTermPreset->set("plasticity_hebbian_enable", false);
        longTermPreset->set("plasticity_structural_enable", true);
        longTermPreset->set("neuromod_curiosity_enable", true);
        longTermPreset->set("neuromod_novelty_enable", true);
        longTermPreset->set("research_mode", static_cast<int>(ResearchMode::LongTerm));
        researchPresets["long_term_study"] = longTermPreset;
        
        // Create research preset for stress testing
        auto stressTestPreset = std::make_shared<Config>();
        stressTestPreset->set("neuron_count", 100);
        stressTestPreset->set("region_count", 1);
        stressTestPreset->set("connection_probability", 0.2);
        stressTestPreset->set("simulation_timestep", 0.002);
        stressTestPreset->set("plasticity_stdp_enable", true);
        stressTestPreset->set("plasticity_hebbian_enable", true);
        stressTestPreset->set("plasticity_structural_enable", true);
        stressTestPreset->set("neuromod_curiosity_enable", false);
        stressTestPreset->set("neuromod_novelty_enable", true);
        stressTestPreset->set("research_mode", static_cast<int>(ResearchMode::StressTest));
        researchPresets["stress_testing"] = stressTestPreset;
    }
    
    void initializeDefaultValidationRules() {
        // Type validation rules
        validationRules["range"] = [](const ConfigValue& value, const ConfigValue& range) -> bool {
            if (std::holds_alternative<int>(value)) {
                int val = std::get<int>(value);
                auto rangeIt = std::get<std::unordered_map<std::string, ConfigValue>>(range).find("min");
                if (rangeIt != std::get<std::unordered_map<std::string, ConfigValue>>(range).end()) {
                    int minVal = std::get<int>(rangeIt->second);
                    rangeIt = std::get<std::unordered_map<std::string, ConfigValue>>(range).find("max");
                    if (rangeIt != std::get<std::unordered_map<std::string, ConfigValue>>(range).end()) {
                        int maxVal = std::get<int>(rangeIt->second);
                        return val >= minVal && val <= maxVal;
                    }
                }
            } else if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                auto rangeIt = std::get<std::unordered_map<std::string, ConfigValue>>(range).find("min");
                if (rangeIt != std::get<std::unordered_map<std::string, ConfigValue>>(range).end()) {
                    double minVal = std::get<double>(rangeIt->second);
                    rangeIt = std::get<std::unordered_map<std::string, ConfigValue>>(range).find("max");
                    if (rangeIt != std::get<std::unordered_map<std::string, ConfigValue>>(range).end()) {
                        double maxVal = std::get<double>(rangeIt->second);
                        return val >= minVal && val <= maxVal;
                    }
                }
            } else if (std::holds_alternative<float>(value)) {
                float val = std::get<float>(value);
                auto rangeIt = std::get<std::unordered_map<std::string, ConfigValue>>(range).find("min");
                if (rangeIt != std::get<std::unordered_map<std::string, ConfigValue>>(range).end()) {
                    float minVal = std::get<float>(rangeIt->second);
                    rangeIt = std::get<std::unordered_map<std::string, ConfigValue>>(range).find("max");
                    if (rangeIt != std::get<std::unordered_map<std::string, ConfigValue>>(range).end()) {
                        float maxVal = std::get<float>(rangeIt->second);
                        return val >= minVal && val <= maxVal;
                    }
                }
            }
            return true; // If not numeric, pass validation
        };
        
        validationRules["positive"] = [](const ConfigValue& value, const ConfigValue&) -> bool {
            if (std::holds_alternative<int>(value)) {
                return std::get<int>(value) >= 0;
            } else if (std::holds_alternative<double>(value)) {
                return std::get<double>(value) >= 0.0;
            } else if (std::holds_alternative<float>(value)) {
                return std::get<float>(value) >= 0.0f;
            } else if (std::holds_alternative<bool>(value)) {
                return true;
            }
            return false;
        };
        
        validationRules["non-negative"] = [](const ConfigValue& value, const ConfigValue&) -> bool {
            if (std::holds_alternative<int>(value)) {
                return std::get<int>(value) >= 0;
            } else if (std::holds_alternative<double>(value)) {
                return std::get<double>(value) >= 0.0;
            } else if (std::holds_alternative<float>(value)) {
                return std::get<float>(value) >= 0.0f;
            }
            return false;
        };
        
        validationRules["enabled_features"] = [](const ConfigValue& value, const ConfigValue&) -> bool {
            if (std::holds_alternative<bool>(value)) {
                return true;
            }
            return false;
        };
    }
};

Configuration::Configuration() : pImpl(std::make_unique<Impl>()) {}

Configuration::~Configuration() = default;

bool Configuration::loadFromFile(const std::string& filepath) {
    try {
        if (!pImpl->config->loadFromFile(filepath)) {
            NLM_LOG_ERROR("Failed to load configuration from file: " + filepath);
            return false;
        }
        NLM_LOG_INFO("Configuration loaded from file: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading configuration: " + std::string(e.what()));
        return false;
    }
}

bool Configuration::saveToFile(const std::string& filepath) const {
    try {
        if (!pImpl->config->saveToFile(filepath)) {
            NLM_LOG_ERROR("Failed to save configuration to file: " + filepath);
            return false;
        }
        NLM_LOG_INFO("Configuration saved to file: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception saving configuration: " + std::string(e.what()));
        return false;
    }
}

bool Configuration::loadFromArgs(int argc, char** argv) {
    return pImpl->config->loadFromArgs(argc, argv);
}

bool Configuration::has(const std::string& key) const {
    return pImpl->config->has(key);
}

std::vector<std::string> Configuration::getKeys() const {
    return pImpl->config->getKeys();
}

void Configuration::clear() {
    pImpl->config->clear();
}

std::string Configuration::summary() const {
    return pImpl->config->summary();
}

void Configuration::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    pImpl->config->set(key, value, source);
}

void Configuration::set(const std::string& key, const std::string& value, ConfigSource source) {
    pImpl->config->set(key, value, source);
}

void Configuration::set(const std::string& key, int value, ConfigSource source) {
    pImpl->config->set(key, value, source);
}

void Configuration::set(const std::string& key, float value, ConfigSource source) {
    pImpl->config->set(key, value, source);
}

void Configuration::set(const std::string& key, double value, ConfigSource source) {
    pImpl->config->set(key, value, source);
}

void Configuration::set(const std::string& key, bool value, ConfigSource source) {
    pImpl->config->set(key, value, source);
}

ConfigValue Configuration::getOr(const std::string& key, const ConfigValue& defaultValue) const {
    return pImpl->config->getOr(key, defaultValue);
}

std::string Configuration::getOr(const std::string& key, const std::string& defaultValue) const {
    return pImpl->config->getOr(key, defaultValue);
}

int Configuration::getOr(const std::string& key, int defaultValue) const {
    return pImpl->config->getOr(key, defaultValue);
}

float Configuration::getOr(const std::string& key, float defaultValue) const {
    return pImpl->config->getOr(key, defaultValue);
}

double Configuration::getOr(const std::string& key, double defaultValue) const {
    return pImpl->config->getOr(key, defaultValue);
}

bool Configuration::getOr(const std::string& key, bool defaultValue) const {
    return pImpl->config->getOr(key, defaultValue);
}

ResearchMode Configuration::getResearchMode() const {
    int mode = pImpl->config->getOr<int>("research_mode", static_cast<int>(pImpl->researchMode));
    return static_cast<ResearchMode>(mode);
}

void Configuration::setResearchMode(ResearchMode mode) {
    pImpl->researchMode = mode;
    pImpl->config->set("research_mode", static_cast<int>(mode));
}

const MemoryManagementConfig& Configuration::getMemoryManagementConfig() const {
    return pImpl->memoryManagementConfig;
}

void Configuration::setMemoryManagementConfig(const MemoryManagementConfig& config) {
    pImpl->memoryManagementConfig = config;
    
    // Update underlying config
    pImpl->config->set("memory.max_working_memory_capacity", config.maxWorkingMemoryCapacity);
    pImpl->config->set("memory.max_episodic_memory_capacity", config.maxEpisodicMemoryCapacity);
    pImpl->config->set("memory.consolidation_interval", config.consolidationInterval);
    pImpl->config->set("memory.consolidation_threshold", config.consolidationThreshold);
    pImpl->config->set("memory.enable_replay", config.enableReplay);
    pImpl->config->set("memory.enable_checkpointing", config.enableCheckpointing);
    pImpl->config->set("memory.checkpoint_path", config.checkpointPath);
    pImpl->config->set("memory.auto_save_enabled", config.autoSaveEnabled);
    pImpl->config->set("memory.memory_decay_rate", config.memoryDecayRate);
}

const PerformanceMonitoringConfig& Configuration::getPerformanceMonitoringConfig() const {
    return pImpl->performanceMonitoringConfig;
}

void Configuration::setPerformanceMonitoringConfig(const PerformanceMonitoringConfig& config) {
    pImpl->performanceMonitoringConfig = config;
    
    // Update underlying config
    pImpl->config->set("performance.enable_benchmarking", config.enableBenchmarking);
    pImpl->config->set("performance.enable_profiling", config.enableProfiling);
    pImpl->config->set("performance.enable_real_time_monitoring", config.enableRealTimeMonitoring);
    pImpl->config->set("performance.benchmark_interval_ms", config.benchmarkIntervalMs);
    pImpl->config->set("performance.profile_interval_ms", config.profileIntervalMs);
    pImpl->config->set("performance.metrics_output_path", config.metricsOutputPath);
    pImpl->config->set("performance.export_metrics_to_file", config.exportMetricsToFile);
    pImpl->config->set("performance.enable_visualization", config.enableVisualization);
    pImpl->config->set("performance.visualization_update_interval_ms", config.visualizationUpdateIntervalMs);
}

const AdvancedSimulationConfig& Configuration::getAdvancedSimulationConfig() const {
    return pImpl->advancedSimulationConfig;
}

void Configuration::setAdvancedSimulationConfig(const AdvancedSimulationConfig& config) {
    pImpl->advancedSimulationConfig = config;
    
    // Update underlying config
    pImpl->config->set("simulation.research_mode", static_cast<int>(config.researchMode));
    pImpl->config->set("simulation.enable_neuromodulation", config.enableNeuromodulation);
    pImpl->config->set("simulation.enable_development", config.enableDevelopment);
    pImpl->config->set("simulation.enable_structural_plasticity", config.enableStructuralPlasticity);
    pImpl->config->set("simulation.enable_reward_modulation", config.enableRewardModulation);
    pImpl->config->set("simulation.enable_curiosity", config.enableCuriosity);
    pImpl->config->set("simulation.enable_novelty_detection", config.enableNoveltyDetection);
    pImpl->config->set("simulation.neuromodulation_strength", config.neuromodulationStrength);
    pImpl->config->set("simulation.development_rate", config.developmentRate);
    pImpl->config->set("simulation.plasticity_rate", config.plasticityRate);
    pImpl->config->set("simulation.reward_sensitivity", config.rewardSensitivity);
    pImpl->config->set("simulation.exploration_bonus", config.explorationBonus);
    pImpl->config->set("simulation.novelty_sensitivity", config.noveltySensitivity);
}

void Configuration::loadResearchPreset(const std::string& presetName) {
    auto it = pImpl->researchPresets.find(presetName);
    if (it != pImpl->researchPresets.end()) {
        // Merge preset configuration into current config
        auto& preset = it->second;
        auto keys = preset->getKeys();
        for (const auto& key : keys) {
            pImpl->config->set(key, preset->get(key));
        }
        NLM_LOG_INFO("Research preset loaded: " + presetName);
    } else {
        NLM_LOG_ERROR("Research preset not found: " + presetName);
    }
}

bool Configuration::hasResearchPreset(const std::string& presetName) const {
    return pImpl->researchPresets.find(presetName) != pImpl->researchPresets.end();
}

std::vector<std::string> Configuration::getAvailableResearchPresets() const {
    std::vector<std::string> presetNames;
    for (const auto& pair : pImpl->researchPresets) {
        presetNames.push_back(pair.first);
    }
    return presetNames;
}

void Configuration::optimizeForPerformance() {
    pImpl->config->set("neuron_count", 500);
    pImpl->config->set("region_count", 1);
    pImpl->config->set("connection_probability", 0.05);
    pImpl->config->set("plasticity_stdp_enable", true);
    pImpl->config->set("plasticity_hebbian_enable", false);
    pImpl->config->set("plasticity_structural_enable", false);
    pImpl->config->set("simulation_timestep", 0.001);
    pImpl->config->set("performance.enable_benchmarking", true);
    pImpl->config->set("performance.enable_profiling", false);
    NLM_LOG_INFO("Configuration optimized for performance");
}

void Configuration::optimizeForAccuracy() {
    pImpl->config->set("neuron_count", 2000);
    pImpl->config->set("region_count", 3);
    pImpl->config->set("connection_probability", 0.15);
    pImpl->config->set("plasticity_stdp_enable", true);
    pImpl->config->set("plasticity_hebbian_enable", true);
    pImpl->config->set("plasticity_structural_enable", true);
    pImpl->config->set("simulation_timestep", 0.0005);
    pImpl->config->set("performance.enable_benchmarking", false);
    pImpl->config->set("performance.enable_profiling", true);
    NLM_LOG_INFO("Configuration optimized for accuracy");
}

void Configuration::optimizeForMemory() {
    pImpl->config->set("neuron_count", 100);
    pImpl->config->set("region_count", 1);
    pImpl->config->set("connection_probability", 0.02);
    pImpl->config->set("plasticity_stdp_enable", true);
    pImpl->config->set("plasticity_hebbian_enable", false);
    pImpl->config->set("plasticity_structural_enable", false);
    pImpl->config->set("simulation_timestep", 0.002);
    pImpl->config->set("performance.enable_benchmarking", false);
    pImpl->config->set("performance.enable_profiling", false);
    NLM_LOG_INFO("Configuration optimized for memory efficiency");
}

bool Configuration::validate() const {
    auto keys = pImpl->config->getKeys();
    std::vector<std::string> errors;
    
    for (const auto& key : keys) {
        auto value = pImpl->config->get(key);
        
        // Apply validation rules
        for (const auto& rulePair : pImpl->validationRules) {
            const std::string& ruleName = rulePair.first;
            auto ruleFunc = rulePair.second;
            
            try {
                if (!ruleFunc(value, {})) {
                    errors.push_back("Validation failed for key '" + key + "' with rule '" + ruleName + "'");
                }
            } catch (const std::exception& e) {
                errors.push_back("Validation error for key '" + key + "' with rule '" + ruleName + "': " + std::string(e.what()));
            }
        }
    }
    
    if (!errors.empty()) {
        NLM_LOG_ERROR("Configuration validation failed:");
        for (const auto& error : errors) {
            NLM_LOG_ERROR("  - " + error);
        }
        return false;
    }
    
    return true;
}

std::vector<std::string> Configuration::getValidationErrors() const {
    return std::vector<std::string>();
}

void Configuration::addValidationRule(const std::string& key, std::function<bool(const ConfigValue&)> rule) {
    pImpl->validationRules[key] = rule;
    NLM_LOG_INFO("Validation rule added for key: " + key);
}

std::string Configuration::toString() const {
    return pImpl->config->toString();
}

std::string Configuration::toPrettyString() const {
    return pImpl->config->toPrettyString();
}

} // namespace nlm
