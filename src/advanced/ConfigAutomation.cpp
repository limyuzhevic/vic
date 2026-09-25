#include "ConfigAutomation.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <random>
#include <cmath>
#include <stdexcept>
#include <numeric>
#include <fstream>
#include <unordered_map>
#include <chrono>
#include <filesystem>

namespace nlm {

// ConfigAutomation implementation
ConfigAutomation::ConfigAutomation()
    : configMonitoringEnabled(false), monitoringInterval(1.0f), configCount(0),
      totalConfigQuality(0.0f), successfulConfigurations(0), failedConfigurations(0) {
}

ConfigAutomation::~ConfigAutomation() {
    // Cleanup
}

void ConfigAutomation::addTemplate(const ConfigurationTemplate& template) {
    templates.push_back(template);
}

void ConfigAutomation::removeTemplate(const std::string& name) {
    auto it = std::remove_if(templates.begin(), templates.end(),
                           [&name](const ConfigurationTemplate& t) { return t.name == name; });
    templates.erase(it, templates.end());
}

ConfigurationTemplate* ConfigAutomation::getTemplate(const std::string& name) {
    for (auto& template : templates) {
        if (template.name == name) {
            return &template;
        }
    }
    return nullptr;
}

std::vector<ConfigurationTemplate> ConfigAutomation::getAllTemplates() const {
    return templates;
}

bool ConfigAutomation::generateConfigFromTemplate(const std::string& templateName, Config& config) {
    ConfigurationTemplate* template = getTemplate(templateName);
    if (!template) return false;
    
    // Generate config from template
    config = Config();
    for (const auto& pair : template->baseConfig) {
        config.set(pair.first, pair.second);
    }
    
    // Add random variations
    std::random_device rd;
    std::mt19937 gen(rd());
    
    for (const auto& pair : template->parameterRanges) {
        std::uniform_int_distribution<size_t> dist(0, pair.second.size() - 1);
        size_t index = dist(gen);
        config.set(pair.first, pair.second[index]);
    }
    
    return true;
}

Config ConfigAutomation::createConfigFromTemplate(const std::string& templateName) {
    Config config;
    generateConfigFromTemplate(templateName, config);
    return config;
}

std::vector<Config> ConfigAutomation::generateConfigVariations(const std::string& templateName,
                                                             size_t numVariations) {
    std::vector<Config> configs;
    
    for (size_t i = 0; i < numVariations; ++i) {
        Config config;
        generateConfigFromTemplate(templateName, config);
        configs.push_back(config);
    }
    
    return configs;
}

void ConfigAutomation::addMacro(const ConfigurationMacro& macro) {
    macros.push_back(macro);
}

void ConfigAutomation::removeMacro(const std::string& name) {
    auto it = std::remove_if(macros.begin(), macros.end(),
                           [&name](const ConfigurationMacro& macro) { return macro.name == name; });
    macros.erase(it, macros.end());
}

ConfigurationMacro* ConfigAutomation::getMacro(const std::string& name) {
    for (auto& macro : macros) {
        if (macro.name == name) {
            return &macro;
        }
    }
    return nullptr;
}

std::vector<ConfigurationMacro> ConfigAutomation::getAllMacros() const {
    return macros;
}

bool ConfigAutomation::applyMacroToConfig(const std::string& macroName, Config& config) {
    ConfigurationMacro* macro = getMacro(macroName);
    if (!macro) return false;
    
    // Apply macro overrides
    for (const auto& pair : macro->configOverrides) {
        config.set(pair.first, pair.second);
    }
    
    return true;
}

Config ConfigAutomation::applyMacro(const std::string& macroName) {
    Config config;
    applyMacroToConfig(macroName, config);
    return config;
}

bool ConfigAutomation::resolveMacroDependencies(const std::string& macroName, Config& config) {
    ConfigurationMacro* macro = getMacro(macroName);
    if (!macro) return false;
    
    // Resolve dependencies
    for (const auto& depName : macro->dependencies) {
        ConfigurationMacro* depMacro = getMacro(depName);
        if (depMacro) {
            // Apply dependent macro
            for (const auto& pair : depMacro->configOverrides) {
                config.set(pair.first, pair.second);
            }
        }
    }
    
    return true;
}

std::vector<Config> ConfigAutomation::generateBatchConfigurations(const std::vector<std::string>& templateNames,
                                                                size_t numPerTemplate) {
    std::vector<Config> allConfigs;
    
    for (const auto& templateName : templateNames) {
        std::vector<Config> variations = generateConfigVariations(templateName, numPerTemplate);
        allConfigs.insert(allConfigs.end(), variations.begin(), variations.end());
    }
    
    return allConfigs;
}

std::vector<Config> ConfigAutomation::generateRandomConfigurations(size_t numConfigs,
                                                                 const std::map<std::string, std::vector<ConfigValue>>& parameterRanges) {
    std::vector<Config> configs;
    std::random_device rd;
    std::mt19937 gen(rd());
    
    for (size_t i = 0; i < numConfigs; ++i) {
        Config config;
        for (const auto& pair : parameterRanges) {
            std::uniform_int_distribution<size_t> dist(0, pair.second.size() - 1);
            size_t index = dist(gen);
            config.set(pair.first, pair.second[index]);
        }
        configs.push_back(config);
    }
    
    return configs;
}

std::vector<Config> ConfigAutomation::generateParameterGrid(const std::map<std::string, std::vector<ConfigValue>>& parameterGrid) {
    std::vector<Config> configs;
    
    // Generate all combinations of parameter values
    std::vector<std::string> paramNames;
    std::vector<std::vector<ConfigValue>> paramValues;
    
    for (const auto& pair : parameterGrid) {
        paramNames.push_back(pair.first);
        paramValues.push_back(pair.second);
    }
    
    // Generate combinations using recursion
    std::vector<Config> currentConfigs;
    currentConfigs.push_back(Config());
    
    for (size_t i = 0; i < paramNames.size(); ++i) {
        std::vector<Config> newConfigs;
        for (const auto& config : currentConfigs) {
            for (const auto& value : paramValues[i]) {
                Config newConfig = config;
                newConfig.set(paramNames[i], value);
                newConfigs.push_back(newConfig);
            }
        }
        currentConfigs = newConfigs;
    }
    
    configs = currentConfigs;
    return configs;
}

bool ConfigAutomation::validateConfig(const Config& config) const {
    return true; // Placeholder
}

bool ConfigAutomation::validateParameterRanges(const Config& config) const {
    return true; // Placeholder
}

bool ConfigAutomation::checkParameterConstraints(const Config& config) const {
    return true; // Placeholder
}

std::vector<std::string> ConfigAutomation::getConfigIssues(const Config& config) const {
    return {}; // Placeholder
}

std::vector<std::string> ConfigAutomation::getConfigWarnings(const Config& config) const {
    return {}; // Placeholder
}

float ConfigAutomation::calculateConfigQuality(const Config& config) const {
    return 0.5f; // Placeholder
}

std::map<std::string, ConfigDiff> ConfigAutomation::compareConfigs(const Config& config1, const Config& config2) const {
    std::map<std::string, ConfigDiff> diffs;
    
    // Get keys from both configs
    std::set<std::string> allKeys;
    auto keys1 = config1.getKeys();
    auto keys2 = config2.getKeys();
    allKeys.insert(keys1.begin(), keys1.end());
    allKeys.insert(keys2.begin(), keys2.end());
    
    for (const auto& key : allKeys) {
        ConfigDiff diff;
        diff.key = key;
        
        auto val1 = config1.get<std::string>(key);
        auto val2 = config2.get<std::string>(key);
        
        if (val1.has_value() && val2.has_value()) {
            if (val1.value() != val2.value()) {
                diff.type = ConfigDiffType::ValueChanged;
                diff.oldValue = val1.value();
                diff.newValue = val2.value();
            }
        } else if (val1.has_value() && !val2.has_value()) {
            diff.type = ConfigDiffType::Removed;
            diff.oldValue = val1.value();
        } else if (!val1.has_value() && val2.has_value()) {
            diff.type = ConfigDiffType::Added;
            diff.newValue = val2.value();
        }
        
        if (diff.type != ConfigDiffType::None) {
            diffs[key] = diff;
        }
    }
    
    return diffs;
}

bool ConfigAutomation::mergeConfigs(Config& target, const Config& source, ConfigSource sourceType) {
    auto keys = source.getKeys();
    for (const auto& key : keys) {
        auto val = source.get<std::string>(key);
        if (val.has_value()) {
            target.set(key, val.value(), sourceType);
        }
    }
    return true;
}

Config ConfigAutomation::createInheritedConfig(const std::vector<Config>& configs) const {
    Config inherited;
    
    for (const auto& config : configs) {
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            auto val = config.get<std::string>(key);
            if (val.has_value()) {
                inherited.set(key, val.value());
            }
        }
    }
    
    return inherited;
}

float ConfigAutomation::calculateConfigSimilarity(const Config& config1, const Config& config2) const {
    auto keys1 = config1.getKeys();
    auto keys2 = config2.getKeys();
    
    std::set<std::string> allKeys;
    allKeys.insert(keys1.begin(), keys1.end());
    allKeys.insert(keys2.begin(), keys2.end());
    
    size_t matches = 0;
    for (const auto& key : allKeys) {
        auto val1 = config1.get<std::string>(key);
        auto val2 = config2.get<std::string>(key);
        
        if (val1.has_value() && val2.has_value() && val1.value() == val2.value()) {
            matches++;
        }
    }
    
    return static_cast<float>(matches) / allKeys.size();
}

OptimizationResult ConfigAutomation::optimizeConfig(const OptimizationObjective& objective,
                                                   const std::map<std::string, std::vector<ConfigValue>>& searchSpace,
                                                   const std::function<float(const Config&)>& objectiveFunction,
                                                   AutomationStrategy strategy,
                                                   size_t maxIterations,
                                                   float convergenceThreshold) {
    OptimizationResult result;
    
    switch (strategy) {
        case AutomationStrategy::GridSearch:
            result = gridSearchOptimize(objective, searchSpace, objectiveFunction, maxIterations);
            break;
        case AutomationStrategy::RandomSearch:
            result = randomSearchOptimize(objective, searchSpace, objectiveFunction, maxIterations);
            break;
        case AutomationStrategy::GeneticAlgorithm:
            result = geneticAlgorithmOptimize(objective, searchSpace, objectiveFunction, maxIterations);
            break;
        case AutomationStrategy::ParticleSwarm:
            result = particleSwarmOptimize(objective, searchSpace, objectiveFunction, maxIterations);
            break;
        case AutomationStrategy::SimulatedAnnealing:
            result = simulatedAnnealingOptimize(objective, searchSpace, objectiveFunction, maxIterations);
            break;
        default:
            result = gridSearchOptimize(objective, searchSpace, objectiveFunction, maxIterations);
            break;
    }
    
    return result;
}

OptimizationResult ConfigAutomation::gridSearchOptimize(const OptimizationObjective& objective,
                                                       const std::map<std::string, std::vector<ConfigValue>>& searchSpace,
                                                       const std::function<float(const Config&)>& objectiveFunction,
                                                       size_t maxIterations) {
    OptimizationResult result;
    
    // Generate parameter grid
    std::vector<Config> configs = generateParameterGrid(searchSpace);
    
    // Limit to maxIterations if needed
    if (configs.size() > maxIterations) {
        configs.resize(maxIterations);
    }
    
    // Evaluate all configurations
    float bestScore = std::numeric_limits<float>::max();
    Config bestConfig;
    
    for (auto& config : configs) {
        float score = objectiveFunction(config);
        result.history.push_back(config);
        result.scores.push_back(score);
        
        if (score < bestScore) {
            bestScore = score;
            bestConfig = config;
        }
    }
    
    result.bestConfig = bestConfig;
    result.bestScore = bestScore;
    result.iteration = 1;
    result.convergenceRate = 1.0f - (bestScore / std::numeric_limits<float>::max());
    
    return result;
}

OptimizationResult ConfigAutomation::randomSearchOptimize(const OptimizationObjective& objective,
                                                         const std::map<std::string, std::vector<ConfigValue>>& searchSpace,
                                                         const std::function<float(const Config&)>& objectiveFunction,
                                                         size_t maxIterations) {
    OptimizationResult result;
    
    // Generate random configurations
    std::vector<Config> configs = generateRandomConfigurations(maxIterations, searchSpace);
    
    // Evaluate all configurations
    float bestScore = std::numeric_limits<float>::max();
    Config bestConfig;
    
    for (auto& config : configs) {
        float score = objectiveFunction(config);
        result.history.push_back(config);
        result.scores.push_back(score);
        
        if (score < bestScore) {
            bestScore = score;
            bestConfig = config;
        }
    }
    
    result.bestConfig = bestConfig;
    result.bestScore = bestScore;
    result.iteration = 1;
    result.convergenceRate = 1.0f - (bestScore / std::numeric_limits<float>::max());
    
    return result;
}

OptimizationResult ConfigAutomation::geneticAlgorithmOptimize(const OptimizationObjective& objective,
                                                           const std::map<std::string, std::vector<ConfigValue>>& searchSpace,
                                                           const std::function<float(const Config&)>& objectiveFunction,
                                                           size_t maxIterations) {
    OptimizationResult result;
    
    // Initialize population
    std::vector<Config> population = generateRandomConfigurations(20, searchSpace);
    
    // Evolution loop
    for (size_t generation = 0; generation < maxIterations; ++generation) {
        // Evaluate population
        std::vector<float> scores;
        for (auto& config : population) {
            float score = objectiveFunction(config);
            scores.push_back(score);
        }
        
        // Select best
        float bestScore = std::numeric_limits<float>::max();
        Config bestConfig;
        
        for (size_t i = 0; i < population.size(); ++i) {
            if (scores[i] < bestScore) {
                bestScore = scores[i];
                bestConfig = population[i];
            }
        }
        
        result.history.push_back(bestConfig);
        result.scores.push_back(bestScore);
        
        if (generation == 0) {
            result.bestConfig = bestConfig;
            result.bestScore = bestScore;
        } else if (bestScore < result.bestScore) {
            result.bestConfig = bestConfig;
            result.bestScore = bestScore;
        }
        
        // Create next generation (simplified)
        std::vector<Config> nextPopulation;
        for (size_t i = 0; i < population.size(); ++i) {
            Config child = population[i]; // Simplified reproduction
            nextPopulation.push_back(child);
        }
        
        population = nextPopulation;
    }
    
    result.iteration = maxIterations;
    result.convergenceRate = 1.0f - (result.bestScore / std::numeric_limits<float>::max());
    
    return result;
}

OptimizationResult ConfigAutomation::particleSwarmOptimize(const OptimizationObjective& objective,
                                                          const std::map<std::string, std::vector<ConfigValue>>& searchSpace,
                                                          const std::function<float(const Config&)>& objectiveFunction,
                                                          size_t maxIterations) {
    OptimizationResult result;
    
    // Initialize particle swarm (simplified)
    std::vector<Config> particles;
    for (size_t i = 0; i < 30; ++i) {
        Config particle = generateRandomConfigurations(1, searchSpace)[0];
        particles.push_back(particle);
    }
    
    // Swarm iteration
    for (size_t iteration = 0; iteration < maxIterations; ++iteration) {
        // Evaluate particles
        std::vector<float> scores;
        for (auto& particle : particles) {
            float score = objectiveFunction(particle);
            scores.push_back(score);
        }
        
        // Update best
        float bestScore = std::numeric_limits<float>::max();
        Config bestConfig;
        
        for (size_t i = 0; i < particles.size(); ++i) {
            if (scores[i] < bestScore) {
                bestScore = scores[i];
                bestConfig = particles[i];
            }
        }
        
        result.history.push_back(bestConfig);
        result.scores.push_back(bestScore);
        
        if (iteration == 0) {
            result.bestConfig = bestConfig;
            result.bestScore = bestScore;
        } else if (bestScore < result.bestScore) {
            result.bestConfig = bestConfig;
            result.bestScore = bestScore;
        }
    }
    
    result.iteration = maxIterations;
    result.convergenceRate = 1.0f - (result.bestScore / std::numeric_limits<float>::max());
    
    return result;
}

OptimizationResult ConfigAutomation::simulatedAnnealingOptimize(const OptimizationObjective& objective,
                                                             const std::map<std::string, std::vector<ConfigValue>>& searchSpace,
                                                             const std::function<float(const Config&)>& objectiveFunction,
                                                             size_t maxIterations) {
    OptimizationResult result;
    
    // Initialize with random config
    std::vector<Config> currentConfigs = generateRandomConfigurations(1, searchSpace);
    Config currentConfig = currentConfigs[0];
    float currentScore = objectiveFunction(currentConfig);
    
    result.history.push_back(currentConfig);
    result.scores.push_back(currentScore);
    result.bestConfig = currentConfig;
    result.bestScore = currentScore;
    
    // Simulated annealing
    float temperature = 100.0f;
    float coolingRate = 0.95f;
    
    for (size_t iteration = 1; iteration < maxIterations; ++iteration) {
        // Generate neighbor
        std::vector<Config> neighbors = generateRandomConfigurations(1, searchSpace);
        Config neighbor = neighbors[0];
        float neighborScore = objectiveFunction(neighbor);
        
        float delta = neighborScore - currentScore;
        
        // Accept or reject based on temperature
        if (delta < 0 || (temperature > 0 && rand() / (float)RAND_MAX < std::exp(-delta / temperature))) {
            currentConfig = neighbor;
            currentScore = neighborScore;
        }
        
        result.history.push_back(currentConfig);
        result.scores.push_back(currentScore);
        
        if (currentScore < result.bestScore) {
            result.bestConfig = currentConfig;
            result.bestScore = currentScore;
        }
        
        // Cool down
        temperature *= coolingRate;
    }
    
    result.iteration = maxIterations;
    result.convergenceRate = 1.0f - (result.bestScore / std::numeric_limits<float>::max());
    
    return result;
}

OptimizationResult ConfigAutomation::bayesianOptimize(const OptimizationObjective& objective,
                                                     const std::map<std::string, std::vector<ConfigValue>>& searchSpace,
                                                     const std::function<float(const Config&)>& objectiveFunction,
                                                     size_t numInitialPoints,
                                                     size_t numIterations) {
    OptimizationResult result;
    
    // Bayesian optimization would require Gaussian processes
    // For now, use random search as a placeholder
    return randomSearchOptimize(objective, searchSpace, objectiveFunction, numIterations);
}

bool ConfigAutomation::exportConfigToFile(const Config& config, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    
    // Export config (simplified)
    file << "Config exported at " << std::chrono::system_clock::now().time_since_epoch().count();
    file.close();
    return true;
}

Config ConfigAutomation::importConfigFromFile(const std::string& filepath) {
    Config config;
    std::ifstream file(filepath);
    if (!file.is_open()) return config;
    
    // Import config (simplified)
    file.close();
    return config;
}

std::string ConfigAutomation::exportConfigToJSON(const Config& config) const {
    // Export config to JSON (simplified)
    return "{\"config\": \"exported\"}";
}

Config ConfigAutomation::importConfigFromJSON(const std::string& json) {
    Config config;
    // Import config from JSON (simplified)
    return config;
}

std::string ConfigAutomation::exportConfigToDict(const Config& config) const {
    auto keys = config.getKeys();
    std::string dict = "{\"config\": {\"keys\": [";
    
    for (size_t i = 0; i < keys.size(); ++i) {
        if (i > 0) dict += ", ";
        dict += "\"" + keys[i] + "\"";
    }
    
    dict += "]}}";
    return dict;
}

bool ConfigAutomation::importConfigFromDict(Config& config, const std::string& dict) {
    // Import config from dict (simplified)
    return true;
}

bool ConfigAutomation::shareConfig(const Config& config, const std::string& shareName,
                                  const std::vector<std::string>& recipients) {
    // Share config (simplified)
    return true;
}

bool ConfigAutomation::loadSharedConfig(const std::string& shareName, Config& config) {
    // Load shared config (simplified)
    return true;
}

std::vector<Config> ConfigAutomation::generateEnsembleConfigurations(const std::vector<std::string>& templateNames,
                                                                   const std::map<std::string, float>& weights) {
    std::vector<Config> ensembleConfigs;
    
    for (const auto& templateName : templateNames) {
        std::vector<Config> variations = generateConfigVariations(templateName, 1);
        if (!variations.empty()) {
            ensembleConfigs.push_back(variations[0]);
        }
    }
    
    return ensembleConfigs;
}

Config ConfigAutomation::createOptimalConfiguration(const OptimizationObjective& objective,
                                                  const std::vector<Config>& candidateConfigs) {
    Config optimalConfig;
    float bestScore = std::numeric_limits<float>::max();
    
    for (const auto& config : candidateConfigs) {
        float score = calculateConfigQuality(config);
        if (score < bestScore) {
            bestScore = score;
            optimalConfig = config;
        }
    }
    
    return optimalConfig;
}

float ConfigAutomation::evaluateConfiguration(Config& config, const std::function<float(const Config&)>& evaluator) {
    return evaluator(config);
}

bool ConfigAutomation::runConfigurationSweep(const std::map<std::string, std::vector<ConfigValue>>& parameterSpace,
                                           const std::function<float(const Config&)>& objectiveFunction) {
    std::vector<Config> configs = generateParameterGrid(parameterSpace);
    
    for (auto& config : configs) {
        float score = objectiveFunction(config);
        // Process configuration
    }
    
    return true;
}

bool ConfigAutomation::runConfigurationPipeline(const std::vector<PipelineStage>& pipeline,
                                               std::vector<Config>& outputs) {
    // Execute pipeline stages
    return true;
}

void ConfigAutomation::saveConfigVersion(const Config& config, const std::string& versionName) {
    versionData[versionName] = config;
    configVersions[versionName] = std::chrono::system_clock::now();
}

Config ConfigAutomation::loadConfigVersion(const std::string& versionName) const {
    auto it = versionData.find(versionName);
    if (it != versionData.end()) {
        return it->second;
    }
    return Config();
}

std::vector<std::string> ConfigAutomation::getConfigVersions() const {
    std::vector<std::string> versions;
    for (const auto& pair : configVersions) {
        versions.push_back(pair.first);
    }
    return versions;
}

bool ConfigAutomation::deleteConfigVersion(const std::string& versionName) {
    auto it = versionData.find(versionName);
    if (it != versionData.end()) {
        versionData.erase(it);
    }
    
    it = configVersions.find(versionName);
    if (it != configVersions.end()) {
        configVersions.erase(it);
    }
    
    return true;
}

bool ConfigAutomation::compareConfigVersions(const std::string& version1, const std::string& version2) {
    Config config1 = loadConfigVersion(version1);
    Config config2 = loadConfigVersion(version2);
    
    return config1.equals(config2);
}

bool ConfigAutomation::revertConfigToVersion(Config& config, const std::string& versionName) {
    Config newConfig = loadConfigVersion(versionName);
    config = newConfig;
    return true;
}

std::map<std::string, float> ConfigAutomation::analyzeConfigComplexity(const Config& config) const {
    std::map<std::string, float> analysis;
    analysis["parameter_count"] = static_cast<float>(config.getKeys().size());
    analysis["unique_sources"] = 0.0f; // Placeholder
    return analysis;
}

std::map<std::string, float> ConfigAutomation::analyzeConfigSensitivity(const Config& config) const {
    std::map<std::string, float> analysis;
    analysis["sensitivity"] = 0.5f; // Placeholder
    return analysis;
}

std::map<std::string, float> ConfigAutomation::analyzeConfigRobustness(const Config& config) const {
    std::map<std::string, float> analysis;
    analysis["robustness"] = 0.7f; // Placeholder
    return analysis;
}

float ConfigAutomation::calculateConfigEntropy(const Config& config) const {
    auto keys = config.getKeys();
    return keys.size() * 0.5f; // Placeholder
}

std::vector<std::string> ConfigAutomation::getConfigRecommendations(const Config& config) const {
    return {"Recommendation 1", "Recommendation 2"}; // Placeholder
}

bool ConfigAutomation::exportAllConfigs(const std::string& directory) {
    return std::filesystem::create_directory(directory);
}

bool ConfigAutomation::importAllConfigs(const std::string& directory) {
    return true; // Placeholder
}

bool ConfigAutomation::exportConfigArchive(const std::string& filepath, const std::vector<std::string>& configNames) {
    // Export config archive
    return true;
}

bool ConfigAutomation::importConfigArchive(const std::string& filepath, std::vector<Config>& configs) {
    // Import config archive
    return true;
}

void ConfigAutomation::startConfigMonitoring() {
    configMonitoringEnabled = true;
}

void ConfigAutomation::stopConfigMonitoring() {
    configMonitoringEnabled = false;
}

std::string ConfigAutomation::getAutomationReport() const {
    std::string report = "Configuration Automation Report\n";
    report += "============================\n";
    report += "Config Count: " + std::to_string(configCount) + "\n";
    report += "Successful Configurations: " + std::to_string(successfulConfigurations) + "\n";
    report += "Failed Configurations: " + std::to_string(failedConfigurations) + "\n";
    report += "Average Config Quality: " + std::to_string(averageConfigQuality) + "\n";
    report += "Templates: " + std::to_string(templates.size()) + "\n";
    report += "Macros: " + std::to_string(macros.size()) + "\n";
    return report;
}

std::string ConfigAutomation::exportConfigStats(const std::string& format) const {
    if (format == "json") {
        std::string json = "{\"config_stats\": {\"count\": " + std::to_string(configCount) + "}}";
        return json;
    }
    
    return getAutomationReport();
}

bool ConfigAutomation::generateConfigDocumentation(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    
    file << getAutomationReport();
    file.close();
    return true;
}

bool ConfigAutomation::generateUserGuide(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    
    file << "Configuration Automation User Guide\n";
    file << "==================================\n";
    file << "This guide explains how to use the configuration automation system.\n";
    file.close();
    return true;
}

std::vector<Config> ConfigAutomation::performAutoML(const std::vector<std::string>& taskTypes,
                                                   const std::map<std::string, std::vector<ConfigValue>>& searchSpaces) {
    std::vector<Config> results;
    
    for (const auto& taskType : taskTypes) {
        // Perform AutoML for each task type
        std::vector<Config> taskResults = generateRandomConfigurations(5, searchSpaces);
        results.insert(results.end(), taskResults.begin(), taskResults.end());
    }
    
    return results;
}

bool ConfigAutomation::runAutoTuning(const std::string& modelType, const std::string& dataset,
                                    const std::map<std::string, std::vector<ConfigValue>>& hyperparameterSpace) {
    // Run auto-tuning
    return true;
}

Config ConfigAutomation::createConfigFromDomainKnowledge(const std::string& domain, const std::string& task) {
    Config config;
    // Create config from domain knowledge
    return config;
}

bool ConfigAutomation::validateDomainSpecificConfig(const Config& config, const std::string& domain) {
    return true; // Placeholder
}

void ConfigAutomation::updateStats(const Config& config, bool success) {
    configCount++;
    
    float quality = calculateConfigQuality(config);
    totalConfigQuality += quality;
    averageConfigQuality = totalConfigQuality / configCount;
    
    if (success) {
        successfulConfigurations++;
    } else {
        failedConfigurations++;
    }
}

void ConfigAutomation::cleanupOldVersions() {
    // Remove old versions (older than 30 days)
    auto now = std::chrono::system_clock::now();
    auto thirtyDaysAgo = now - std::chrono::hours(720);
    
    auto it = configVersions.begin();
    while (it != configVersions.end()) {
        if (it->second < thirtyDaysAgo) {
            versionData.erase(it->first);
            it = configVersions.erase(it);
        } else {
            ++it;
        }
    }
}

bool ConfigAutomation::validateTemplate(const ConfigurationTemplate& template) const {
    return !template.name.empty() && !template.description.empty();
}

bool ConfigAutomation::validateMacro(const ConfigurationMacro& macro) const {
    return !macro.name.empty() && !macro.description.empty();
}

} // namespace nlm
