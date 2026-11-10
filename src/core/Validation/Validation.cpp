#include "Validation.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Debug/DebugTools.hpp"
#include <fstream>
#include <sstream>
#include <regex>

namespace nlm {

ValidationResult ValidationEngine::validate(const Config& config) const {
    ValidationResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (const auto& [key, rule] : rules) {
        if (config.has(key)) {
            auto valueOpt = config.get<ConfigValue>(key);
            if (valueOpt) {
                if (!validateConfigValue(key, DebugTools::toString(*valueOpt), rule)) {
                    result.isValid = false;
                    result.errors.push_back("Invalid value for key '" + key + "'");
                }
            } else {
                if (rule.isRequired) {
                    result.isValid = false;
                    result.errors.push_back("Required key missing: '" + key + "'");
                }
            }
        } else if (rule.isRequired) {
            result.isValid = false;
            result.errors.push_back("Required key missing: '" + key + "'");
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.validationTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Update statistics
    getValidationStats().recordValidation("overall", result.isValid);
    
    return result;
}

ValidationResult ValidationEngine::validateKey(const Config& config, const std::string& key) const {
    ValidationResult result;
    
    auto ruleIt = rules.find(key);
    if (ruleIt === rules.end()) {
        result.isValid = false;
        result.errors.push_back("Unknown key: '" + key + "'");
        return result;
    }
    
    const auto& rule = ruleIt->second;
    
    if (config.has(key)) {
        auto valueOpt = config.get<ConfigValue>(key);
        if (valueOpt) {
            if (!validateConfigValue(key, DebugTools::toString(*valueOpt), rule)) {
                result.isValid = false;
                result.errors.push_back("Invalid value for key '" + key + "'");
            }
        } else {
            if (rule.isRequired) {
                result.isValid = false;
                result.errors.push_back("Required key missing: '" + key + "'");
            }
        }
    } else if (rule.isRequired) {
        result.isValid = false;
        result.errors.push_back("Required key missing: '" + key + "'");
    }
    
    return result;
}

std::string ValidationEngine::getHelpForKey(const std::string& key) const {
    auto ruleIt = rules.find(key);
    if (ruleIt === rules.end()) {
        return "No validation rule found for key: '" + key + "'";
    }
    
    const auto& rule = ruleIt->second;
    std::ostringstream help;
    help << "Key: '" << key << "'\n";
    help << "  Description: " << rule.description << "\n";
    help << "  Type: " << rule.expectedType << "\n";
    help << "  Required: " << (rule.isRequired ? "Yes" : "No") << "\n";
    
    if (!rule.validValues.empty()) {
        help << "  Valid Values: ";
        for (size_t i = 0; i < rule.validValues.size(); ++i) {
            if (i > 0) help << ", ";
            help << rule.validValues[i];
        }
        help << "\n";
    }
    
    if (!rule.validationRegex.empty()) {
        help << "  Validation: " << rule.validationRegex << "\n";
    }
    
    return help.str();
}

std::vector<std::string> ValidationEngine::listKeys() const {
    std::vector<std::string> keys;
    keys.reserve(rules.size());
    
    for (const auto& [key, rule] : rules) {
        keys.push_back(key);
    }
    
    return keys;
}

bool ValidationEngine::validateConfigFile(const std::string& filepath) {
    Config config;
    if (!config.loadFromFile(filepath)) {
        NLM_LOG_ERROR("Cannot open configuration file for validation: " + filepath);
        return false;
    }
    
    ValidationResult result = validate(config);
    
    if (!result.isValid) {
        NLM_LOG_ERROR("Configuration validation failed for: " + filepath);
        for (const auto& error : result.errors) {
            NLM_LOG_ERROR("  Error: " + error);
        }
        return false;
    }
    
    NLM_LOG_INFO("Configuration validation passed for: " + filepath);
    return true;
}

Config ValidationEngine::mergeConfigs(const std::string& baseFile, const std::string& overrideFile) {
    Config baseConfig;
    Config overrideConfig;
    
    if (!baseConfig.loadFromFile(baseFile)) {
        NLM_LOG_ERROR("Cannot load base configuration file: " + baseFile);
        return Config();
    }
    
    if (!overrideConfig.loadFromFile(overrideFile)) {
        NLM_LOG_ERROR("Cannot load override configuration file: " + overrideFile);
        return Config();
    }
    
    // Merge configurations
    for (const auto& key : overrideConfig.getKeys()) {
        auto valueOpt = overrideConfig.get<ConfigValue>(key);
        if (valueOpt) {
            baseConfig.set(key, *valueOpt);
        }
    }
    
    return baseConfig;
}

ValidationResult ValidationEngine::validateConfigValues(const std::map<std::string, std::string>& config) {
    ValidationResult result;
    ValidationEngine& engine = ValidationEngine::getInstance();
    
    for (const auto& [key, value] : config) {
        auto ruleIt = engine.rules.find(key);
        if (ruleIt !== engine.rules.end()) {
            if (!validateConfigValue(key, value, ruleIt->second)) {
                result.isValid = false;
                result.errors.push_back("Invalid value for key '" + key + "': '" + value + "'");
            }
        }
    }
    
    return result;
}

std::map<std::string, std::string> ValidationEngine::getDefaultConfig() {
    std::map<std::string, std::string> defaultConfig;
    
    // Common default configuration values
    defaultConfig["neuron_count"] = "1000";
    defaultConfig["region_count"] = "1";
    defaultConfig["connection_probability"] = "0.1";
    defaultConfig["stdp_ltp_weight"] = "0.01";
    defaultConfig["stdp_ltd_weight"] = "0.012";
    defaultConfig["synaptogenesis_rate"] = "0.0001";
    defaultConfig["pruning_rate"] = "0.00001";
    defaultConfig["working_memory_capacity"] = "100";
    defaultConfig["episodic_memory_max_episodes"] = "1000";
    defaultConfig["enable_curiosity"] = "true";
    defaultConfig["enable_novelty"] = "true";
    defaultConfig["enable_reward_modulation"] = "true";
    defaultConfig["developmental_stage"] = "adult";
    defaultConfig["checkpoint_interval"] = "100";
    defaultConfig["replay_interval"] = "100";
    defaultConfig["consolidation_interval"] = "1000";
    
    return defaultConfig;
}

ValidationResult NetworkValidation::validateNetworkConfig(const Config& config) {
    ValidationResult result;
    
    // Validate neuron count
    auto neuronCountOpt = config.get<int64_t>("neuron_count");
    if (neuronCountOpt) {
        auto neuronValidation = validateNeuronCount(static_cast<size_t>(*neuronCountOpt));
        if (!neuronValidation.isValid) {
            result.isValid = false;
            result.errors.insert(result.errors.end(), neuronValidation.errors.begin(), neuronValidation.errors.end());
        }
    }
    
    // Validate region count
    auto regionCountOpt = config.get<int64_t>("region_count");
    if (regionCountOpt) {
        auto regionValidation = validateRegionConfig(
            neuronCountOpt ? static_cast<size_t>(*neuronCountOpt) : 1000,
            static_cast<size_t>(*regionCountOpt)
        );
        if (!regionValidation.isValid) {
            result.isValid = false;
            result.errors.insert(result.errors.end(), regionValidation.errors.begin(), regionValidation.errors.end());
        }
    }
    
    // Validate connection probability
    auto connProbOpt = config.get<double>("connection_probability");
    if (connProbOpt) {
        auto connValidation = validateConnectionProbability(static_cast<float>(*connProbOpt));
        if (!connValidation.isValid) {
            result.isValid = false;
            result.errors.insert(result.errors.end(), connValidation.errors.begin(), connValidation.errors.end());
        }
    }
    
    // Validate STDP parameters
    auto stdpLtpOpt = config.get<double>("stdp_ltp_weight");
    auto stdpLtdOpt = config.get<double>("stdp_ltd_weight");
    if (stdpLtpOpt && stdpLtdOpt) {
        auto stdpValidation = validateSTDPParameters(config);
        if (!stdpValidation.isValid) {
            result.isValid = false;
            result.errors.insert(result.errors.end(), stdpValidation.errors.begin(), stdpValidation.errors.end());
        }
    }
    
    return result;
}

ValidationResult NetworkValidation::validateNeuronCount(size_t count) {
    ValidationResult result;
    
    if (count < 1) {
        result.isValid = false;
        result.errors.push_back("Neuron count must be at least 1, got " + std::to_string(count));
    } else if (count > 10000) {
        result.isValid = false;
        result.errors.push_back("Neuron count cannot exceed 10000, got " + std::to_string(count));
    }
    
    return result;
}

ValidationResult NetworkValidation::validateRegionConfig(size_t neuronCount, size_t regionCount) {
    ValidationResult result;
    
    if (regionCount < 1) {
        result.isValid = false;
        result.errors.push_back("Region count must be at least 1, got " + std::to_string(regionCount));
    } else if (regionCount > 100) {
        result.isValid = false;
        result.errors.push_back("Region count cannot exceed 100, got " + std::to_string(regionCount));
    }
    
    if (neuronCount < regionCount) {
        result.isValid = false;
        result.errors.push_back("Neuron count must be at least region count, got " + std::to_string(neuronCount) + " neurons and " + std::to_string(regionCount) + " regions");
    }
    
    return result;
}

ValidationResult NetworkValidation::validateConnectionProbability(float prob) {
    ValidationResult result;
    
    if (prob < 0.0f || prob > 1.0f) {
        result.isValid = false;
        result.errors.push_back("Connection probability must be between 0.0 and 1.0, got " + std::to_string(prob));
    }
    
    return result;
}

ValidationResult NetworkValidation::validateSTDPParameters(const Config& config) {
    ValidationResult result;
    
    auto ltpOpt = config.get<double>("stdp_ltp_weight");
    auto ltdOpt = config.get<double>("stdp_ltd_weight");
    
    if (ltpOpt && ltdOpt) {
        float ltp = static_cast<float>(*ltpOpt);
        float ltd = static_cast<float>(*ltdOpt);
        
        if (ltp < 0.0f || ltd < 0.0f) {
            result.isValid = false;
            result.errors.push_back("STDP weights must be positive, got LTP=" + std::to_string(ltp) + ", LTD=" + std::to_string(ltd));
        }
        
        if (ltp > ltd) {
            result.warnings.push_back("STDP LTP weight should typically be less than LTD weight for balanced learning");
        }
    }
    
    return result;
}

ValidationResult NetworkValidation::validatePlasticitySettings(const Config& config) {
    ValidationResult result;
    
    auto synaptogenesisOpt = config.get<double>("synaptogenesis_rate");
    auto pruningOpt = config.get<double>("pruning_rate");
    
    if (synaptogenesisOpt && pruningOpt) {
        float synaptogenesis = static_cast<float>(*synaptogenesisOpt);
        float pruning = static_cast<float>(*pruningOpt);
        
        if (synaptogenesis < 0.0f || pruning < 0.0f) {
            result.isValid = false;
            result.errors.push_back("Plasticity rates must be positive, got synaptogenesis=" + std::to_string(synaptogenesis) + ", pruning=" + std::to_string(pruning));
        }
        
        if (synaptogenesis > 0.1f) {
            result.warnings.push_back("Synaptogenesis rate is high, may cause rapid network growth");
        }
        
        if (pruning > 0.01f) {
            result.warnings.push_back("Pruning rate is high, may cause excessive synapse loss");
        }
    }
    
    return result;
}

ValidationResult NetworkValidation::validateMemorySettings(const Config& config) {
    ValidationResult result;
    
    auto workingMemOpt = config.get<int64_t>("working_memory_capacity");
    auto episodicOpt = config.get<int64_t>("episodic_memory_max_episodes");
    
    if (workingMemOpt) {
        int64_t capacity = *workingMemOpt;
        if (capacity < 0) {
            result.isValid = false;
            result.errors.push_back("Working memory capacity cannot be negative, got " + std::to_string(capacity));
        }
    }
    
    if (episodicOpt) {
        int64_t episodes = *episodicOpt;
        if (episodes < 0) {
            result.isValid = false;
            result.errors.push_back("Episodic memory max episodes cannot be negative, got " + std::to_string(episodes));
        }
    }
    
    return result;
}

ValidationResult NetworkValidation::validateDevelopmentSettings(const Config& config) {
    ValidationResult result;
    
    auto stageOpt = config.get<std::string>("developmental_stage");
    
    if (stageOpt) {
        std::string stage = *stageOpt;
        std::vector<std::string> validStages = {"initial", "critical", "maturation", "adult"};
        
        if (std::find(validStages.begin(), validStages.end(), stage) === validStages.end()) {
            result.isValid = false;
            result.errors.push_back("Invalid developmental stage '" + stage + "'. Valid stages: initial, critical, maturation, adult");
        }
    }
    
    return result;
}

ValidationResult NetworkValidation::validateNeuromodulationSettings(const Config& config) {
    ValidationResult result;
    
    auto curiosityOpt = config.get<bool>("enable_curiosity");
    auto noveltyOpt = config.get<bool>("enable_novelty");
    auto rewardOpt = config.get<bool>("enable_reward_modulation");
    
    return result;
}

ValidationResult NetworkValidation::validatePredictionSettings(const Config& config) {
    ValidationResult result;
    
    return result;
}

ValidationResult NetworkValidation::validateCognitionSettings(const Config& config) {
    ValidationResult result;
    
    return result;
}

ValidationResult BrainStateValidation::validateBrainState(const Brain* brain) {
    ValidationResult result;
    
    if (!brain) {
        result.isValid = false;
        result.errors.push_back("Brain pointer is null");
        return result;
    }
    
    auto regions = brain->getRegions();
    if (regions.empty()) {
        result.isValid = false;
        result.errors.push_back("Brain has no regions");
    }
    
    // Validate each region
    for (size_t i = 0; i < regions.size(); ++i) {
        auto regionResult = validateRegionState(regions[i].get());
        if (!regionResult.isValid) {
            result.isValid = false;
            result.errors.push_back("Region " + std::to_string(i) + " validation failed: " + regionResult.errors[0]);
        }
    }
    
    return result;
}

ValidationResult BrainStateValidation::validateNeuronStates(const std::vector<Neuron*>& neurons) {
    ValidationResult result;
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        if (!neurons[i]) {
            result.isValid = false;
            result.errors.push_back("Neuron " + std::to_string(i) + " is null");
            continue;
        }
        
        auto* neuron = neurons[i];
        
        // Validate neuron state
        auto& state = neuron->getState();
        
        if (std::isnan(state.membranePotential)) {
            result.isValid = false;
            result.errors.push_back("Neuron " + std::to_string(i) + " has NaN membrane potential");
        }
        
        if (state.refractoryRemaining > state.refractoryPeriod) {
            result.isValid = false;
            result.errors.push_back("Neuron " + std::to_string(i) + " refractory remaining exceeds refractory period");
        }
    }
    
    return result;
}

ValidationResult BrainStateValidation::validateSynapseStates(const std::vector<Synapse*>& synapses) {
    ValidationResult result;
    
    for (size_t i = 0; i < synapses.size(); ++i) {
        if (!synapses[i]) {
            result.isValid = false;
            result.errors.push_back("Synapse " + std::to_string(i) + " is null");
            continue;
        }
        
        auto* synapse = synapses[i];
        
        // Validate synapse state
        float weight = synapse->getWeight();
        if (std::isnan(weight) || std::isinf(weight)) {
            result.isValid = false;
            result.errors.push_back("Synapse " + std::to_string(i) + " has invalid weight: " + std::to_string(weight));
        }
        
        float eligibility = synapse->getEligibilityTrace();
        if (std::isnan(eligibility) || std::isinf(eligibility)) {
            result.isValid = false;
            result.errors.push_back("Synapse " + std::to_string(i) + " has invalid eligibility trace: " + std::to_string(eligibility));
        }
    }
    
    return result;
}

ValidationResult BrainStateValidation::validateMemoryStates(const Brain* brain) {
    ValidationResult result;
    
    if (!brain) {
        return result;
    }
    
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        // Validate working memory state
        auto neurons = workingMemory->getMemoryNeurons();
        auto activations = workingMemory->getMemoryActivations();
        
        if (neurons.size() !== activations.size()) {
            result.isValid = false;
            result.errors.push_back("Working memory neuron count mismatch with activation count");
        }
        
        for (size_t i = 0; i < activations.size(); ++i) {
            float activation = activations[i];
            if (std::isnan(activation) || std::isinf(activation)) {
                result.isValid = false;
                result.errors.push_back("Working memory activation " + std::to_string(i) + " is invalid: " + std::to_string(activation));
            }
            
            if (activation < 0.0f || activation > 1.0f) {
                result.warnings.push_back("Working memory activation " + std::to_string(i) + " is outside expected range [0,1]: " + std::to_string(activation));
            }
        }
    }
    
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        // Validate episodic memory state
        auto episodes = episodicMemory->getEpisodes();
        
        for (size_t i = 0; i < episodes.size(); ++i) {
            const auto& episode = episodes[i];
            
            if (std::isnan(episode.reward) || std::isinf(episode.reward)) {
                result.isValid = false;
                result.errors.push_back("Episodic memory episode " + std::to_string(i) + " has invalid reward: " + std::to_string(episode.reward));
            }
        }
    }
    
    return result;
}

ValidationResult BrainStateValidation::validatePlasticityStates(const Brain* brain) {
    ValidationResult result;
    
    if (!brain) {
        return result;
    }
    
    auto* stdp = brain->getSTDP();
    if (stdp) {
        // Validate STDP state
        auto ltpWeight = stdp->getLTPWeight();
        auto ltdWeight = stdp->getLTDWeight();
        
        if (std::isnan(ltpWeight) || std::isinf(ltpWeight)) {
            result.isValid = false;
            result.errors.push_back("STDP LTP weight is invalid: " + std::to_string(ltpWeight));
        }
        
        if (std::isnan(ltdWeight) || std::isinf(ltdWeight)) {
            result.isValid = false;
            result.errors.push_back("STDP LTD weight is invalid: " + std::to_string(ltdWeight));
        }
    }
    
    return result;
}

ValidationResult BrainStateValidation::validateDevelopmentStates(const Brain* brain) {
    ValidationResult result;
    
    if (!brain) {
        return result;
    }
    
    auto stage = brain->getDevelopmentalStage();
    
    if (stage === DevelopmentalStage::Initial) {
        result.warnings.push_back("Brain is in initial developmental stage - high plasticity expected");
    } else if (stage === DevelopmentalStage::Adult) {
        result.warnings.push_back("Brain is in adult developmental stage - plasticity reduced");
    }
    
    return result;
}

ValidationResult BrainStateValidation::validateNeuromodulationStates(const Brain* brain) {
    ValidationResult result;
    
    if (!brain) {
        return result;
    }
    
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        float dopamineLevel = dopamine->getLevel();
        
        if (std::isnan(dopamineLevel) || std::isinf(dopamineLevel)) {
            result.isValid = false;
            result.errors.push_back("Dopamine level is invalid: " + std::to_string(dopamineLevel));
        }
        
        if (dopamineLevel < -1.0f || dopamineLevel > 1.0f) {
            result.warnings.push_back("Dopamine level is outside expected range [-1,1]: " + std::to_string(dopamineLevel));
        }
    }
    
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        float curiosityLevel = curiosity->getLevel();
        
        if (std::isnan(curiosityLevel) || std::isinf(curiosityLevel)) {
            result.isValid = false;
            result.errors.push_back("Curiosity level is invalid: " + std::to_string(curiosityLevel));
        }
    }
    
    return result;
}

ValidationResult PerformanceValidation::validatePerformanceMetrics(const PerformanceMetrics& metrics) {
    ValidationResult result;
    
    if (metrics.stepCount < 0) {
        result.isValid = false;
        result.errors.push_back("Step count cannot be negative: " + std::to_string(metrics.stepCount));
    }
    
    if (metrics.totalSpikeCount < 0) {
        result.isValid = false;
        result.errors.push_back("Total spike count cannot be negative: " + std::to_string(metrics.totalSpikeCount));
    }
    
    if (metrics.totalWallTime < 0.0) {
        result.isValid = false;
        result.errors.push_back("Total wall time cannot be negative: " + std::to_string(metrics.totalWallTime));
    }
    
    if (!metrics.stepTimes.empty()) {
        double sum = 0.0;
        for (const auto& time : metrics.stepTimes) {
            sum += time.second;
        }
        double avgTime = sum / metrics.stepTimes.size();
        
        if (avgTime < 0.0) {
            result.isValid = false;
            result.errors.push_back("Average step time cannot be negative: " + std::to_string(avgTime));
        }
    }
    
    if (!metrics.spikeRates.empty()) {
        double sum = std::accumulate(metrics.spikeRates.begin(), metrics.spikeRates.end(), 0.0);
        double avgRate = sum / metrics.spikeRates.size();
        
        if (avgRate < 0.0) {
            result.isValid = false;
            result.errors.push_back("Average firing rate cannot be negative: " + std::to_string(avgRate));
        }
    }
    
    return result;
}

ValidationResult PerformanceValidation::validateStepPerformance(const std::vector<double>& stepTimes) {
    ValidationResult result;
    
    for (size_t i = 0; i < stepTimes.size(); ++i) {
        if (stepTimes[i] < 0.0) {
            result.isValid = false;
            result.errors.push_back("Step time " + std::to_string(i) + " cannot be negative: " + std::to_string(stepTimes[i]));
        }
    }
    
    return result;
}

ValidationResult PerformanceValidation::validateSpikePerformance(const std::vector<double>& spikeRates) {
    ValidationResult result;
    
    for (size_t i = 0; i < spikeRates.size(); ++i) {
        if (spikeRates[i] < 0.0) {
            result.isValid = false;
            result.errors.push_back("Spike rate " + std::to_string(i) + " cannot be negative: " + std::to_string(spikeRates[i]));
        }
    }
    
    return result;
}

ValidationResult PerformanceValidation::validateMemoryUsage(double memoryUsage) {
    ValidationResult result;
    
    if (memoryUsage < 0.0) {
        result.isValid = false;
        result.errors.push_back("Memory usage cannot be negative: " + std::to_string(memoryUsage));
    }
    
    if (memoryUsage > 10000.0) {  // 10GB limit
        result.warnings.push_back("Memory usage is very high: " + std::to_string(memoryUsage) + " MB");
    }
    
    return result;
}

ValidationResult PerformanceValidation::validateProcessingSpeed(double speed) {
    ValidationResult result;
    
    if (speed < 0.0) {
        result.isValid = false;
        result.errors.push_back("Processing speed cannot be negative: " + std::to_string(speed));
    }
    
    return result;
}

} // namespace nlm
