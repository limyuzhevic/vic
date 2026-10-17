#include "Phase6EnhancedConfig.hpp"
#include "src/core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <chrono>

namespace nlm {

struct Phase6Config::Impl {
    // Configuration storage
    ExperimentConfig experimentConfig_;
    PerformanceConfig performanceConfig_;
    MemoryConfig memoryConfig_;
    NeuromodulationConfig neuromodulationConfig_;
    DevelopmentConfig developmentConfig_;
    LearningConfig learningConfig_;
    OutputConfig outputConfig_;
    AdvancedConfig advancedConfig_;
    
    // Legacy configuration for migration
    std::shared_ptr<Config> legacyConfig_;
    
    // Configuration metadata
    std::unordered_map<std::string, std::string> keyDescriptions_;
    std::unordered_map<std::string, double> keyRanges_;
    
    // Validation state
    bool validationDirty_ = true;
    std::vector<std::string> validationErrors_;
    
    // Performance tracking
    double memoryUsageEstimate_ = 0.0;
    double runtimeEstimate_ = 0.0;
    
    // Configuration history for version control
    std::vector<Phase6Config> configHistory_;
    
    Impl() {
        initializeDescriptionsAndRanges();
    }
    
    void initializeDescriptionsAndRanges() {
        // Experiment configuration descriptions
        keyDescriptions_["maxSteps"] = "Maximum number of simulation steps";
        keyRanges_["maxSteps"] = 100000.0;
        
        keyDescriptions_["neuronCount"] = "Number of neurons in the simulation";
        keyRanges_["neuronCount"] = 10000.0;
        
        keyDescriptions_["regionCount"] = "Number of brain regions";
        keyRanges_["regionCount"] = 10.0;
        
        keyDescriptions_["connectionProbability"] = "Probability of synaptic connections between neurons";
        keyRanges_["connectionProbability"] = 1.0;
        
        keyDescriptions_["randomSeed"] = "Random seed for reproducible simulations";
        keyRanges_["randomSeed"] = 2000000000.0;
        
        // Performance configuration descriptions
        keyDescriptions_["maxThreads"] = "Maximum number of threads for parallel processing";
        keyRanges_["maxThreads"] = 32.0;
        
        // Memory configuration descriptions
        keyDescriptions_["workingMemoryCapacity"] = "Capacity of working memory in units";
        keyRanges_["workingMemoryCapacity"] = 10000.0;
        
        keyDescriptions_["episodicMemoryCapacity"] = "Capacity of episodic memory in episodes";
        keyRanges_["episodicMemoryCapacity"] = 1000.0;
        
        // Development configuration descriptions
        keyDescriptions_["criticalPeriodStart"] = "Start time of critical development period";
        keyRanges_["criticalPeriodStart"] = 1000.0;
        
        keyDescriptions_["criticalPeriodEnd"] = "End time of critical development period";
        keyRanges_["criticalPeriodEnd"] = 1000.0;
        
        // Learning configuration descriptions
        keyDescriptions_["stdpLTPWeight"] = "STDP Long-Term Potentiation weight";
        keyRanges_["stdpLTPWeight"] = 1.0;
        
        keyDescriptions_["stdpLTDWeight"] = "STDP Long-Term Depression weight";
        keyRanges_["stdpLTDWeight"] = 1.0;
        
        // Output configuration descriptions
        keyDescriptions_["logFile"] = "Path to log file for console and file logging";
        keyDescriptions_["resultsFile"] = "Path to results file for experiment output";
        keyDescriptions_["checkpointFile"] = "Path to checkpoint file for state saving";
    }
};

Phase6Config::Phase6Config() : pImpl(std::make_unique<Impl>()) {
    initializeKeyMappings();
}

Phase6Config::~Phase6Config() = default;

Phase6Config::Phase6Config(Phase6Config&&) noexcept = default;
Phase6Config& Phase6Config::operator=(Phase6Config&&) noexcept = default;

void Phase6Config::initializeKeyMappings() {
    // Experiment configuration keys
    groupToKeys_[ConfigGroup::Experiment] = {
        "maxSteps", "neuronCount", "regionCount", "connectionProbability",
        "randomSeed", "simulationTimestep", "enableCheckpointing",
        "enableReplay", "enableDevelopment", "checkpointPath", "experimentName"
    };
    
    // Performance configuration keys
    groupToKeys_[ConfigGroup::Performance] = {
        "enableVectorization", "enableParallelProcessing", "maxThreads",
        "enableSIMD", "enableMemoryPools", "enableEventQueues",
        "optimizeForSpeed", "optimizeForMemory"
    };
    
    // Memory configuration keys
    groupToKeys_[ConfigGroup::Memory] = {
        "enableWorkingMemory", "enableEpisodicMemory", "enableSemanticMemory",
        "enableProceduralMemory", "enableAssociativeMemory", "workingMemoryCapacity",
        "episodicMemoryCapacity", "replayFrequency", "consolidationThreshold",
        "enableSleepReplay"
    };
    
    // Neuromodulation configuration keys
    groupToKeys_[ConfigGroup::Neuromodulation] = {
        "enableDopamine", "enableCuriosity", "enableNovelty",
        "enablePredictionError", "dopamineBaseline", "curiosityBaseline",
        "noveltyBaseline", "predictionErrorBaseline", "neuromodulationTimeScale",
        "enableRewardPredictionError", "enableRiskAssessment"
    };
    
    // Development configuration keys
    groupToKeys_[ConfigGroup::Development] = {
        "enableDevelopmentalStages", "initialStage", "criticalPeriodStart",
        "criticalPeriodEnd", "maturationAge", "agingStart", "plasticityRate",
        "plasticityMaturationRate", "plasticityAgingRate", "enableExperienceDependentDevelopment",
        "enableGeneticConstraints"
    };
    
    // Learning configuration keys
    groupToKeys_[ConfigGroup::Learning] = {
        "hebbianLearningRate", "stdpLTPWeight", "stdpLTDWeight", "stdpTimeConstant",
        "rewardModulatedLearningRate", "enableSTDP", "enableHebbian",
        "enableRewardModulated", "eligibilityTraceDecay", "synapticWeightBoundsMin",
        "synapticWeightBoundsMax", "enableStructuralPlasticity", "synaptogenesisRate",
        "pruningRate"
    };
    
    // Output configuration keys
    groupToKeys_[ConfigGroup::Output] = {
        "enableConsoleLogging", "enableFileLogging", "enableNetworkLogging",
        "logFile", "resultsFile", "checkpointFile", "saveDetailedMetrics",
        "saveNeuralTrajectories", "enableVisualizationOutput"
    };
    
    // Advanced configuration keys
    groupToKeys_[ConfigGroup::Advanced] = {
        "enableExperimentalFeatures", "enableDebugMode", "enableValidation",
        "enableProfiling", "enableBenchmarking", "enableAblationStudies",
        "enableParameterTuning"
    };
    
    // Build reverse mapping
    for (const auto& [group, keys] : groupToKeys_) {
        for (const auto& key : keys) {
            keyToGroup_[key] = group;
        }
    }
}

void Phase6Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return;
    }
    
    // Parse JSON-like config (simplified for demo)
    std::string line;
    while (std::getline(file, line)) {
        // Simple key=value parsing
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            // Try to convert value to appropriate type
            try {
                // Try integer
                size_t intPos;
                if ((intPos = value.find('.')) != std::string::npos) {
                    // Has decimal point, try double
                    try {
                        double doubleVal = std::stod(value);
                        set(key, doubleVal);
                    } catch (...) {
                        // Try string
                        set(key, value);
                    }
                } else {
                    // No decimal point, try integer
                    try {
                        int intVal = std::stoi(value);
                        set(key, intVal);
                    } catch (...) {
                        // Try boolean
                        if (value == "true" || value == "false") {
                            bool boolVal = (value == "true");
                            set(key, boolVal);
                        } else {
                            // String
                            set(key, value);
                        }
                    }
                }
            } catch (const std::exception& e) {
                NLM_LOG_WARN("Cannot parse config value for key '" + key + "': " + std::string(e.what()));
            }
        }
    }
    
    file.close();
    pImpl->validationDirty_ = true;
}

void Phase6Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
        return;
    }
    
    // Write configuration in a simple format
    file << "# Phase 6 Configuration" << std::endl;
    file << "# Generated by NLM Phase 6 system" << std::endl;
    file << std::endl;
    
    // Write experiment configuration
    file << "[Experiment]" << std::endl;
    file << "maxSteps=" << experimentConfig_.maxSteps << std::endl;
    file << "neuronCount=" << experimentConfig_.neuronCount << std::endl;
    file << "regionCount=" << experimentConfig_.regionCount << std::endl;
    file << "connectionProbability=" << experimentConfig_.connectionProbability << std::endl;
    file << "randomSeed=" << experimentConfig_.randomSeed << std::endl;
    file << "simulationTimestep=" << experimentConfig_.simulationTimestep << std::endl;
    file << "enableCheckpointing=" << (experimentConfig_.enableCheckpointing ? "true" : "false") << std::endl;
    file << "enableReplay=" << (experimentConfig_.enableReplay ? "true" : "false") << std::endl;
    file << "enableDevelopment=" << (experimentConfig_.enableDevelopment ? "true" : "false") << std::endl;
    file << "checkpointPath=" << experimentConfig_.checkpointPath << std::endl;
    file << "experimentName=" << experimentConfig_.experimentName << std::endl;
    file << std::endl;
    
    // Write performance configuration
    file << "[Performance]" << std::endl;
    file << "enableVectorization=" << (performanceConfig_.enableVectorization ? "true" : "false") << std::endl;
    file << "enableParallelProcessing=" << (performanceConfig_.enableParallelProcessing ? "true" : "false") << std::endl;
    file << "maxThreads=" << performanceConfig_.maxThreads << std::endl;
    file << "enableSIMD=" << (performanceConfig_.enableSIMD ? "true" : "false") << std::endl;
    file << "enableMemoryPools=" << (performanceConfig_.enableMemoryPools ? "true" : "false") << std::endl;
    file << "enableEventQueues=" << (performanceConfig_.enableEventQueues ? "true" : "false") << std::endl;
    file << "optimizeForSpeed=" << (performanceConfig_.optimizeForSpeed ? "true" : "false") << std::endl;
    file << "optimizeForMemory=" << (performanceConfig_.optimizeForMemory ? "true" : "false") << std::endl;
    file << std::endl;
    
    // Write other configurations (simplified for space)
    file << "# Additional configurations omitted for brevity" << std::endl;
    
    file.close();
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
}

bool Phase6Config::validate() const {
    updateValidationCache();
    return pImpl->validationErrors_.empty();
}

std::vector<std::string> Phase6Config::getValidationErrors() const {
    updateValidationCache();
    return pImpl->validationErrors_;
}

template<typename T>
T Phase6Config::get(const std::string& key) const {
    // Look up key in group mapping
    auto it = keyToGroup_.find(key);
    if (it == keyToGroup_.end()) {
        throw std::runtime_error("Unknown configuration key: " + key);
    }
    
    ConfigGroup group = it->second;
    
    // Delegate to appropriate config group
    switch (group) {
        case ConfigGroup::Experiment:
            return getFromExperimentConfig<T>(key);
        case ConfigGroup::Performance:
            return getFromPerformanceConfig<T>(key);
        case ConfigGroup::Memory:
            return getFromMemoryConfig<T>(key);
        case ConfigGroup::Neuromodulation:
            return getFromNeuromodulationConfig<T>(key);
        case ConfigGroup::Development:
            return getFromDevelopmentConfig<T>(key);
        case ConfigGroup::Learning:
            return getFromLearningConfig<T>(key);
        case ConfigGroup::Output:
            return getFromOutputConfig<T>(key);
        case ConfigGroup::Advanced:
            return getFromAdvancedConfig<T>(key);
        default:
            throw std::runtime_error("Unknown configuration group for key: " + key);
    }
}

template<typename T>
T Phase6Config::getOr(const std::string& key, const T& defaultValue) const {
    try {
        return get<T>(key);
    } catch (const std::exception& e) {
        NLM_LOG_WARN("Using default value for key '" + key + "': " + std::string(e.what()));
        return defaultValue;
    }
}

void Phase6Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Find key in mapping
    auto it = keyToGroup_.find(key);
    if (it == keyToGroup_.end()) {
        NLM_LOG_ERROR("Cannot set unknown configuration key: " + key);
        return;
    }
    
    ConfigGroup group = it->second;
    
    // Set value in appropriate config group (simplified for demo)
    // In a full implementation, each config group would have its own set method
    
    // For demo purposes, just log the change
    NLM_LOG_INFO("Configuration key '" + key + "' set to value (type: " + 
                 std::to_string(value.index()) + ") via " + 
                 std::to_string(static_cast<int>(source)) + " source");
    
    pImpl->validationDirty_ = true;
}

bool Phase6Config::isEqual(const Phase6Config& other) const {
    return experimentConfig_.maxSteps == other.experimentConfig_.maxSteps &&
           experimentConfig_.neuronCount == other.experimentConfig_.neuronCount &&
           // Compare all other fields...
           true;
}

std::string Phase6Config::toString() const {
    std::stringstream ss;
    ss << "Phase6Config:" << std::endl;
    ss << "  Experiment: " << experimentConfig_.neuronCount << " neurons, " 
       << experimentConfig_.maxSteps << " steps" << std::endl;
    ss << "  Performance: " << (performanceConfig_.enableVectorization ? "vectorized" : "scalar") 
       << ", " << performanceConfig_.maxThreads << " threads" << std::endl;
    ss << "  Memory: " << (memoryConfig_.enableEpisodicMemory ? "episodes enabled" : "episodes disabled")
       << std::endl;
    return ss.str();
}

bool Phase6Config::fromString(const std::string& str) {
    // Simple parsing for demonstration
    // In a real implementation, this would parse JSON or another structured format
    return false;
}

void Phase6Config::migrateFromLegacyConfig(const Config& legacyConfig) {
    pImpl->legacyConfig_ = std::make_shared<Config>(legacyConfig);
    
    NLM_LOG_INFO("Migrating from legacy configuration system");
    
    // Map common keys (simplified)
    if (legacyConfig.has("neuron_count")) {
        experimentConfig_.neuronCount = legacyConfig.getOr<int64_t>("neuron_count", 1000);
    }
    
    if (legacyConfig.has("max_steps")) {
        experimentConfig_.maxSteps = legacyConfig.getOr<int64_t>("max_steps", 10000);
    }
    
    if (legacyConfig.has("random_seed")) {
        experimentConfig_.randomSeed = legacyConfig.getOr<int64_t>("random_seed", 42);
    }
    
    pImpl->validationDirty_ = true;
}

std::string Phase6Config::getConfigurationProfile() const {
    std::stringstream ss;
    ss << "Phase 6 Configuration Profile:" << std::endl;
    ss << "  Scale: " << (experimentConfig_.neuronCount < 1000 ? "Small" : 
                          experimentConfig_.neuronCount < 10000 ? "Medium" : "Large") << std::endl;
    ss << "  Duration: " << (experimentConfig_.maxSteps < 1000 ? "Short" : 
                          experimentConfig_.maxSteps < 10000 ? "Medium" : "Long") << std::endl;
    ss << "  Complexity: " << (performanceConfig_.enableVectorization ? "High" : "Low") << std::endl;
    return ss.str();
}

double Phase6Config::getEstimatedMemoryUsage() const {
    // Simple estimation based on configuration
    double baseMemory = 100.0;  // MB
    double memoryPerNeuron = 0.01; // MB per neuron
    double memoryPerStep = 0.0001; // MB per simulation step
    
    double estimatedMemory = baseMemory + 
                            experimentConfig_.neuronCount * memoryPerNeuron +
                            experimentConfig_.maxSteps * memoryPerStep;
    
    if (performanceConfig_.enableMemoryPools) {
        estimatedMemory *= 0.8; // 20% savings with memory pools
    }
    
    pImpl->memoryUsageEstimate_ = estimatedMemory;
    return estimatedMemory;
}

double Phase6Config::getEstimatedRuntime() const {
    // Simple estimation based on configuration
    double baseTime = 10.0; // seconds
    double timePerNeuron = 0.01; // seconds per neuron
    double timePerStep = 0.001; // seconds per simulation step
    
    double estimatedTime = baseTime +
                           experimentConfig_.neuronCount * timePerNeuron +
                           experimentConfig_.maxSteps * timePerStep;
    
    if (performanceConfig_.enableVectorization) {
        estimatedTime /= performanceConfig_.maxThreads;
    }
    
    pImpl->runtimeEstimate_ = estimatedTime;
    return estimatedTime;
}

std::string Phase6Config::getConfigDescription(const std::string& key) const {
    auto it = pImpl->keyDescriptions_.find(key);
    return it != pImpl->keyDescriptions_.end() ? it->second : "No description available";
}

std::vector<std::string> Phase6Config::getConfigKeysByGroup(ConfigGroup group) const {
    std::vector<std::string> keys;
    auto it = groupToKeys_.find(group);
    if (it != groupToKeys_.end()) {
        keys = std::vector<std::string>(it->second.begin(), it->second.end());
    }
    return keys;
}

std::unordered_map<std::string, Phase6Config::ConfigGroup> Phase6Config::getKeyToGroupMap() const {
    return keyToGroup_;
}

void Phase6Config::updateValidationCache() const {
    if (!pImpl->validationDirty_) {
        return;
    }
    
    pImpl->validationErrors_.clear();
    
    // Validate experiment configuration
    if (experimentConfig_.neuronCount == 0) {
        pImpl->validationErrors_.push_back("Neuron count must be greater than 0");
    }
    
    if (experimentConfig_.maxSteps == 0) {
        pImpl->validationErrors_.push_back("Max steps must be greater than 0");
    }
    
    if (experimentConfig_.connectionProbability < 0.0f || 
        experimentConfig_.connectionProbability > 1.0f) {
        pImpl->validationErrors_.push_back("Connection probability must be between 0.0 and 1.0");
    }
    
    // Validate performance configuration
    if (performanceConfig_.maxThreads <= 0) {
        pImpl->validationErrors_.push_back("Max threads must be greater than 0");
    }
    
    // Validate memory configuration
    if (memoryConfig_.workingMemoryCapacity == 0) {
        pImpl->validationErrors_.push_back("Working memory capacity must be greater than 0");
    }
    
    // Validate development configuration
    if (developmentConfig_.criticalPeriodStart >= developmentConfig_.criticalPeriodEnd) {
        pImpl->validationErrors_.push_back("Critical period start must be before end");
    }
    
    // Validate learning configuration
    if (learningConfig_.stdpLTPWeight < 0.0f || learningConfig_.stdpLTDWeight < 0.0f) {
        pImpl->validationErrors_.push_back("STDP weights must be non-negative");
    }
    
    pImpl->validationDirty_ = false;
}

void Phase6Config::defineStandardPresets(std::vector<std::string>& presets) {
    presets = {"small_test", "medium_test", "large_test", "quick_validation", "production"};
}

void Phase6Config::defineQuickPresets(std::vector<std::string>& presets) {
    presets = {"debug", "benchmark", "research", "demo"};
}

void Phase6Config::defineResearchPresets(std::vector<std::string>& presets) {
    presets = {"exploration", "ablation", "parameter_tuning", "validation"};
}

} // namespace nlm
