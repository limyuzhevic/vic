#include "AgentBrainAdvanced.hpp"
#include "../../core/Config/Config.hpp"
#include "../../core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace nlm {

AgentBrainAdvanced::AgentBrainAdvanced(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
    : AgentBrain(brain), lastConfigurationUpdate_(0.0) {
    // Initialize behavioral features
    behavioralFeatures_ = {
        {"curiosity", true},
        {"novelty_detection", true},
        {"reward_modulation", true},
        {"structural_plasticity", true},
        {"development", true},
        {"exploration", true},
        {"social_learning", false},
        {"prediction", true},
        {"planning", true},
        {"attention", true}
    };
    
    // Load configuration if provided
    if (config) {
        loadFromConfig(config);
    } else {
        // Load default configuration
        agentConfig_ = getDefaultConfiguration();
    }
    
    NLM_LOG_INFO("Advanced AgentBrain initialized with configuration");
    NLM_LOG_INFO("Curiosity enabled: " + std::string(behavioralFeatures_["curiosity"] ? "yes" : "no"));
    NLM_LOG_INFO("Social learning enabled: " + std::string(behavioralFeatures_["social_learning"] ? "yes" : "no"));
}

AgentBrainAdvanced::~AgentBrainAdvanced() = default;

void AgentBrainAdvanced::loadConfiguration(const std::string& configFile) {
    NLM_LOG_INFO("Loading agent configuration from: " + configFile);
    
    try {
        std::ifstream file(configFile);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Could not open configuration file: " + configFile);
            return;
        }
        
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        // Parse JSON configuration
        if (jsonConfig.contains("behavioral_parameters")) {
            const auto& bp = jsonConfig["behavioral_parameters"];
            if (bp.contains("curiosity_threshold")) {
                agentConfig_.curiosityThreshold = bp["curiosity_threshold"].get<float>();
            }
            if (bp.contains("novelty_decay")) {
                agentConfig_.noveltyDecay = bp["novelty_decay"].get<float>();
            }
            if (bp.contains("reward_weight")) {
                agentConfig_.rewardWeight = bp["reward_weight"].get<float>();
            }
            if (bp.contains("exploration_probability")) {
                agentConfig_.explorationProbability = bp["exploration_probability"].get<float>();
            }
        }
        
        if (jsonConfig.contains("neuromodulation_parameters")) {
            const auto& np = jsonConfig["neuromodulation_parameters"];
            if (np.contains("dopamine_sensitivity")) {
                agentConfig_.dopamineSensitivity = np["dopamine_sensitivity"].get<float>();
            }
            if (np.contains("curiosity_baseline")) {
                agentConfig_.curiosityBaseline = np["curiosity_baseline"].get<float>();
            }
            if (np.contains("novelty_threshold")) {
                agentConfig_.noveltyThreshold = np["novelty_threshold"].get<float>();
            }
        }
        
        if (jsonConfig.contains("development_parameters")) {
            const auto& dp = jsonConfig["development_parameters"];
            if (dp.contains("developmental_age")) {
                agentConfig_.developmentalAge = dp["developmental_age"].get<float>();
            }
            if (dp.contains("plasticity_modifier")) {
                agentConfig_.plasticityModifier = dp["plasticity_modifier"].get<float>();
            }
            if (dp.contains("structural_plasticity_rate")) {
                agentConfig_.structuralPlasticityRate = dp["structural_plasticity_rate"].get<float>();
            }
        }
        
        if (jsonConfig.contains("memory_parameters")) {
            const auto& mp = jsonConfig["memory_parameters"];
            if (mp.contains("working_memory_capacity")) {
                agentConfig_.workingMemoryCapacity = mp["working_memory_capacity"].get<size_t>();
            }
            if (mp.contains("episodic_memory_limit")) {
                agentConfig_.episodicMemoryLimit = mp["episodic_memory_limit"].get<size_t>();
            }
        }
        
        if (jsonConfig.contains("behavioral_features")) {
            const auto& bf = jsonConfig["behavioral_features"];
            for (const auto& [feature, enabled] : bf.items()) {
                behavioralFeatures_[feature] = enabled.get<bool>();
            }
        }
        
        lastConfigurationUpdate_ = std::chrono::system_clock::now().time_since_epoch().count();
        
        NLM_LOG_INFO("Agent configuration loaded successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration file: " + std::string(e.what()));
    }
}

void AgentBrainAdvanced::loadFromConfig(std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Config is null, using default configuration");
        agentConfig_ = getDefaultConfiguration();
        return;
    }
    
    // Load configuration from the config system
    agentConfig_.curiosityThreshold = config->getOr<float>("curiosity_threshold", 0.3f);
    agentConfig_.noveltyDecay = config->getOr<float>("novelty_decay", 0.99f);
    agentConfig_.rewardWeight = config->getOr<float>("reward_weight", 1.0f);
    agentConfig_.explorationProbability = config->getOr<float>("exploration_probability", 0.1f);
    
    agentConfig_.dopamineSensitivity = config->getOr<float>("dopamine_sensitivity", 1.0f);
    agentConfig_.curiosityBaseline = config->getOr<float>("curiosity_baseline", 0.1f);
    agentConfig_.noveltyThreshold = config->getOr<float>("novelty_threshold", 0.2f);
    
    agentConfig_.developmentalAge = config->getOr<float>("developmental_age", 0.0f);
    agentConfig_.plasticityModifier = config->getOr<float>("plasticity_modifier", 1.0f);
    agentConfig_.structuralPlasticityRate = config->getOr<float>("structural_plasticity_rate", 0.0001f);
    
    agentConfig_.workingMemoryCapacity = config->getOr<size_t>("working_memory_capacity", 100);
    agentConfig_.episodicMemoryLimit = config->getOr<size_t>("episodic_memory_limit", 1000);
    
    // Load behavioral features
    behavioralFeatures_["curiosity"] = config->getOr<bool>("enable_curiosity", true);
    behavioralFeatures_["novelty_detection"] = config->getOr<bool>("enable_novelty", true);
    behavioralFeatures_["reward_modulation"] = config->getOr<bool>("enable_reward_modulation", true);
    behavioralFeatures_["structural_plasticity"] = config->getOr<bool>("enable_structural_plasticity", true);
    behavioralFeatures_["development"] = config->getOr<bool>("enable_development", true);
    behavioralFeatures_["exploration"] = config->getOr<bool>("enable_exploration", true);
    
    lastConfigurationUpdate_ = std::chrono::system_clock::now().time_since_epoch().count();
    
    NLM_LOG_INFO("Agent configuration loaded from config system");
}

void AgentBrainAdvanced::applyConfiguration(const AgentConfig& agentConfig) {
    agentConfig_ = agentConfig;
    lastConfigurationUpdate_ = std::chrono::system_clock::now().time_since_epoch().count();
    
    NLM_LOG_INFO("Agent configuration applied");
}

void AgentBrainAdvanced::enableBehavioralFeature(const std::string& feature, bool enable) {
    if (behavioralFeatures_.find(feature) != behavioralFeatures_.end()) {
        behavioralFeatures_[feature] = enable;
        NLM_LOG_INFO("Behavioral feature " + feature + " " + std::string(enable ? "enabled" : "disabled"));
    } else {
        NLM_LOG_ERROR("Unknown behavioral feature: " + feature);
    }
}

bool AgentBrainAdvanced::isBehavioralFeatureEnabled(const std::string& feature) const {
    auto it = behavioralFeatures_.find(feature);
    return it != behavioralFeatures_.end() ? it->second : false;
}

std::vector<std::string> AgentBrainAdvanced::validateConfiguration() const {
    std::vector<std::string> errors;
    
    // Validate behavioral parameters
    if (agentConfig_.curiosityThreshold < 0.0f || agentConfig_.curiosityThreshold > 1.0f) {
        errors.push_back("Curiosity threshold must be between 0.0 and 1.0");
    }
    
    if (agentConfig_.noveltyDecay < 0.0f || agentConfig_.noveltyDecay > 1.0f) {
        errors.push_back("Novelty decay must be between 0.0 and 1.0");
    }
    
    if (agentConfig_.rewardWeight < 0.0f) {
        errors.push_back("Reward weight must be positive");
    }
    
    if (agentConfig_.explorationProbability < 0.0f || agentConfig_.explorationProbability > 1.0f) {
        errors.push_back("Exploration probability must be between 0.0 and 1.0");
    }
    
    // Validate neuromodulation parameters
    if (agentConfig_.dopamineSensitivity < 0.0f) {
        errors.push_back("Dopamine sensitivity must be positive");
    }
    
    if (agentConfig_.curiosityBaseline < 0.0f || agentConfig_.curiosityBaseline > 1.0f) {
        errors.push_back("Curiosity baseline must be between 0.0 and 1.0");
    }
    
    if (agentConfig_.noveltyThreshold < 0.0f || agentConfig_.noveltyThreshold > 1.0f) {
        errors.push_back("Novelty threshold must be between 0.0 and 1.0");
    }
    
    // Validate developmental parameters
    if (agentConfig_.plasticityModifier < 0.0f) {
        errors.push_back("Plasticity modifier must be positive");
    }
    
    if (agentConfig_.structuralPlasticityRate < 0.0f) {
        errors.push_back("Structural plasticity rate must be positive");
    }
    
    // Validate memory parameters
    if (agentConfig_.workingMemoryCapacity == 0) {
        errors.push_back("Working memory capacity must be positive");
    }
    
    if (agentConfig_.episodicMemoryLimit == 0) {
        errors.push_back("Episodic memory limit must be positive");
    }
    
    if (agentConfig_.memoryRetentionRate < 0.0f || agentConfig_.memoryRetentionRate > 1.0f) {
        errors.push_back("Memory retention rate must be between 0.0 and 1.0");
    }
    
    return errors;
}

std::string AgentBrainAdvanced::exportConfigurationToJSON() const {
    nlohmann::json jsonConfig;
    
    // Behavioral parameters
    jsonConfig["behavioral_parameters"]["curiosity_threshold"] = agentConfig_.curiosityThreshold;
    jsonConfig["behavioral_parameters"]["novelty_decay"] = agentConfig_.noveltyDecay;
    jsonConfig["behavioral_parameters"]["reward_weight"] = agentConfig_.rewardWeight;
    jsonConfig["behavioral_parameters"]["exploration_probability"] = agentConfig_.explorationProbability;
    
    // Neuromodulation parameters
    jsonConfig["neuromodulation_parameters"]["dopamine_sensitivity"] = agentConfig_.dopamineSensitivity;
    jsonConfig["neuromodulation_parameters"]["curiosity_baseline"] = agentConfig_.curiosityBaseline;
    jsonConfig["neuromodulation_parameters"]["novelty_threshold"] = agentConfig_.noveltyThreshold;
    
    // Development parameters
    jsonConfig["development_parameters"]["developmental_age"] = agentConfig_.developmentalAge;
    jsonConfig["development_parameters"]["plasticity_modifier"] = agentConfig_.plasticityModifier;
    jsonConfig["development_parameters"]["structural_plasticity_rate"] = agentConfig_.structuralPlasticityRate;
    
    // Memory parameters
    jsonConfig["memory_parameters"]["working_memory_capacity"] = agentConfig_.workingMemoryCapacity;
    jsonConfig["memory_parameters"]["episodic_memory_limit"] = agentConfig_.episodicMemoryLimit;
    jsonConfig["memory_parameters"]["memory_retention_rate"] = agentConfig_.memoryRetentionRate;
    
    // Sensory parameters
    jsonConfig["sensory_parameters"]["vision_sensitivity"] = agentConfig_.visionSensitivity;
    jsonConfig["sensory_parameters"]["touch_sensitivity"] = agentConfig_.touchSensitivity;
    jsonConfig["sensory_parameters"]["internal_sensitivity"] = agentConfig_.internalSensitivity;
    jsonConfig["sensory_parameters"]["proprioception_sensitivity"] = agentConfig_.proprioceptionSensitivity;
    
    // Motor parameters
    jsonConfig["motor_parameters"]["motor_noise"] = agentConfig_.motorNoise;
    jsonConfig["motor_parameters"]["action_selection_temperature"] = agentConfig_.actionSelectionTemperature;
    jsonConfig["motor_parameters"]["reward_history_length"] = agentConfig_.rewardHistoryLength;
    
    // Behavioral features
    jsonConfig["behavioral_features"]["curiosity"] = behavioralFeatures_["curiosity"];
    jsonConfig["behavioral_features"]["novelty_detection"] = behavioralFeatures_["novelty_detection"];
    jsonConfig["behavioral_features"]["reward_modulation"] = behavioralFeatures_["reward_modulation"];
    jsonConfig["behavioral_features"]["structural_plasticity"] = behavioralFeatures_["structural_plasticity"];
    jsonConfig["behavioral_features"]["development"] = behavioralFeatures_["development"];
    jsonConfig["behavioral_features"]["exploration"] = behavioralFeatures_["exploration"];
    
    std::ostringstream oss;
    oss << std::setw(2) << jsonConfig;
    
    return oss.str();
}

bool AgentBrainAdvanced::importConfigurationFromJSON(const std::string& json) {
    try {
        nlohmann::json jsonConfig = nlohmann::json::parse(json);
        
        // Parse behavioral parameters
        if (jsonConfig.contains("behavioral_parameters")) {
            const auto& bp = jsonConfig["behavioral_parameters"];
            if (bp.contains("curiosity_threshold")) {
                agentConfig_.curiosityThreshold = bp["curiosity_threshold"].get<float>();
            }
            if (bp.contains("novelty_decay")) {
                agentConfig_.noveltyDecay = bp["novelty_decay"].get<float>();
            }
            if (bp.contains("reward_weight")) {
                agentConfig_.rewardWeight = bp["reward_weight"].get<float>();
            }
            if (bp.contains("exploration_probability")) {
                agentConfig_.explorationProbability = bp["exploration_probability"].get<float>();
            }
        }
        
        // Parse neuromodulation parameters
        if (jsonConfig.contains("neuromodulation_parameters")) {
            const auto& np = jsonConfig["neuromodulation_parameters"];
            if (np.contains("dopamine_sensitivity")) {
                agentConfig_.dopamineSensitivity = np["dopamine_sensitivity"].get<float>();
            }
            if (np.contains("curiosity_baseline")) {
                agentConfig_.curiosityBaseline = np["curiosity_baseline"].get<float>();
            }
            if (np.contains("novelty_threshold")) {
                agentConfig_.noveltyThreshold = np["novelty_threshold"].get<float>();
            }
        }
        
        // Parse behavioral features
        if (jsonConfig.contains("behavioral_features")) {
            const auto& bf = jsonConfig["behavioral_features"];
            if (bf.contains("curiosity")) {
                behavioralFeatures_["curiosity"] = bf["curiosity"].get<bool>();
            }
            if (bf.contains("novelty_detection")) {
                behavioralFeatures_["novelty_detection"] = bf["novelty_detection"].get<bool>();
            }
            if (bf.contains("reward_modulation")) {
                behavioralFeatures_["reward_modulation"] = bf["reward_modulation"].get<bool>();
            }
        }
        
        lastConfigurationUpdate_ = std::chrono::system_clock::now().time_since_epoch().count();
        
        NLM_LOG_INFO("Agent configuration imported successfully from JSON");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error importing configuration from JSON: " + std::string(e.what()));
        return false;
    }
}

std::string AgentBrainAdvanced::getConfigurationStatistics() const {
    std::ostringstream oss;
    oss << "=== Agent Configuration Statistics ===\n";
    oss << "Last update: " << lastConfigurationUpdate_ << " seconds ago\n";
    oss << "\nBehavioral Parameters:\n";
    oss << "  Curiosity threshold: " << agentConfig_.curiosityThreshold << "\n";
    oss << "  Novelty decay: " << agentConfig_.noveltyDecay << "\n";
    oss << "  Reward weight: " << agentConfig_.rewardWeight << "\n";
    oss << "  Exploration probability: " << agentConfig_.explorationProbability << "\n";
    oss << "\nNeuromodulation Parameters:\n";
    oss << "  Dopamine sensitivity: " << agentConfig_.dopamineSensitivity << "\n";
    oss << "  Curiosity baseline: " << agentConfig_.curiosityBaseline << "\n";
    oss << "  Novelty threshold: " << agentConfig_.noveltyThreshold << "\n";
    oss << "\nDevelopment Parameters:\n";
    oss << "  Developmental age: " << agentConfig_.developmentalAge << "\n";
    oss << "  Plasticity modifier: " << agentConfig_.plasticityModifier << "\n";
    oss << "  Structural plasticity rate: " << agentConfig_.structuralPlasticityRate << "\n";
    oss << "\nMemory Parameters:\n";
    oss << "  Working memory capacity: " << agentConfig_.workingMemoryCapacity << "\n";
    oss << "  Episodic memory limit: " << agentConfig_.episodicMemoryLimit << "\n";
    oss << "  Memory retention rate: " << agentConfig_.memoryRetentionRate << "\n";
    oss << "\nEnabled Behavioral Features:\n";
    for (const auto& [feature, enabled] : behavioralFeatures_) {
        if (enabled) {
            oss << "  " << feature << "\n";
        }
    }
    
    return oss.str();
}

void AgentBrainAdvanced::applyAdvancedCuriosity(const SensoryPercept& percept) {
    if (!behavioralFeatures_["curiosity"]) return;
    
    // Enhanced curiosity calculation based on configuration
    float curiosityLevel = agentConfig_.curiosityBaseline;
    
    // Add novelty detection
    if (behavioralFeatures_["novelty_detection"]) {
        // Calculate novelty based on sensory input
        curiosityLevel += computeNovelty(percept) * agentConfig_.noveltyThreshold;
    }
    
    // Apply curiosity-based exploration
    if (curiosityLevel > agentConfig_.curiosityThreshold) {
        curiosityLevel *= (1.0f + agentConfig_.explorationProbability);
        // Apply exploration behavior
    }
    
    // Update curiosity level in the agent
    // This would modify the agent's state
}

void AgentBrainAdvanced::applyAdvancedRewardModulation(float reward, float predictedReward, const std::string& context) {
    if (!behavioralFeatures_["reward_modulation"]) return;
    
    // Advanced reward modulation based on configuration
    float predictionError = reward - predictedReward;
    
    // Apply sensitivity modifier
    predictionError *= agentConfig_.dopamineSensitivity;
    
    // Apply context-specific weighting
    if (!context.empty()) {
        // Context-specific reward adjustment
        predictionError *= getContextWeight(context);
    }
    
    // Apply reward modulation
    // This would modify the agent's synaptic weights
}

void AgentBrainAdvanced::applyAdvancedDevelopment(double timestep, const std::string& environmentalFactors) {
    if (!behavioralFeatures_["development"]) return;
    
    // Advanced developmental progression based on configuration
    agentConfig_.developmentalAge += timestep * agentConfig_.developmentalStageSpeed;
    
    // Apply environmental influences
    if (!environmentalFactors.empty()) {
        // Parse environmental factors and apply development effects
        applyEnvironmentalDevelopment(environmentalFactors);
    }
    
    // Update plasticity based on developmental stage
    if (agentConfig_.developmentalAge < agentConfig_.criticalPeriodDuration) {
        agentConfig_.plasticityModifier = 0.8f * agentConfig_.developmentalAge / agentConfig_.criticalPeriodDuration;
    } else {
        agentConfig_.plasticityModifier = 0.2f;
    }
    
    // Apply structural changes
    if (behavioralFeatures_["structural_plasticity"]) {
        agentConfig_.structuralPlasticityRate *= agentConfig_.plasticityModifier;
    }
}

void AgentBrainAdvanced::setCuriosityParameters(float baseline, float threshold, float decay) {
    if (baseline >= 0.0f && baseline <= 1.0f) {
        agentConfig_.curiosityBaseline = baseline;
    }
    if (threshold >= 0.0f && threshold <= 1.0f) {
        agentConfig_.curiosityThreshold = threshold;
    }
    if (decay >= 0.0f && decay <= 1.0f) {
        agentConfig_.noveltyDecay = decay;
    }
}

void AgentBrainAdvanced::setRewardParameters(float weight, float sensitivity) {
    if (weight >= 0.0f) {
        agentConfig_.rewardWeight = weight;
    }
    if (sensitivity >= 0.0f) {
        agentConfig_.dopamineSensitivity = sensitivity;
    }
}

void AgentBrainAdvanced::setDevelopmentParameters(float stageSpeed, float criticalPeriod, float maturation) {
    if (stageSpeed >= 0.0f) {
        agentConfig_.developmentalStageSpeed = stageSpeed;
    }
    if (criticalPeriod >= 0.0f) {
        agentConfig_.criticalPeriodDuration = criticalPeriod;
    }
    if (maturation >= 0.0f) {
        agentConfig_.maturationThreshold = maturation;
    }
}

void AgentBrainAdvanced::setMemoryParameters(size_t workingCapacity, size_t episodicLimit, float retention) {
    if (workingCapacity > 0) {
        agentConfig_.workingMemoryCapacity = workingCapacity;
    }
    if (episodicLimit > 0) {
        agentConfig_.episodicMemoryLimit = episodicLimit;
    }
    if (retention >= 0.0f && retention <= 1.0f) {
        agentConfig_.memoryRetentionRate = retention;
    }
}

void AgentBrainAdvanced::setSensoryParameters(float vision, float touch, float internal, float proprioception) {
    if (vision >= 0.0f) {
        agentConfig_.visionSensitivity = vision;
    }
    if (touch >= 0.0f) {
        agentConfig_.touchSensitivity = touch;
    }
    if (internal >= 0.0f) {
        agentConfig_.internalSensitivity = internal;
    }
    if (proprioception >= 0.0f) {
        agentConfig_.proprioceptionSensitivity = proprioception;
    }
}

void AgentBrainAdvanced::setMotorParameters(float noise, float temperature, float historyLength) {
    if (noise >= 0.0f) {
        agentConfig_.motorNoise = noise;
    }
    if (temperature >= 0.0f) {
        agentConfig_.actionSelectionTemperature = temperature;
    }
    if (historyLength >= 0.0f) {
        agentConfig_.rewardHistoryLength = historyLength;
    }
}

void AgentBrainAdvanced::setSocialParameters(float imitation, float observation, float learningRate) {
    if (imitation >= 0.0f && imitation <= 1.0f) {
        agentConfig_.imitationProbability = imitation;
    }
    if (observation >= 0.0f && observation <= 1.0f) {
        agentConfig_.observationWeight = observation;
    }
    if (learningRate >= 0.0f) {
        agentConfig_.socialLearningRate = learningRate;
    }
}

void AgentBrainAdvanced::loadPreset(const std::string& presetName) {
    if (presetName == "default") {
        agentConfig_ = getDefaultConfiguration();
    } else if (presetName == "optimized_for_learning") {
        agentConfig_ = getOptimizedForLearning();
    } else if (presetName == "optimized_for_speed") {
        agentConfig_ = getOptimizedForSpeed();
    } else if (presetName == "optimized_for_memory") {
        agentConfig_ = getOptimizedForMemory();
    } else if (presetName == "optimized_for_social_learning") {
        agentConfig_ = getOptimizedForSocialLearning();
    } else {
        NLM_LOG_ERROR("Unknown preset: " + presetName);
    }
}

std::vector<std::string> AgentBrainAdvanced::getAvailablePresets() {
    return {"default", "optimized_for_learning", "optimized_for_speed", 
            "optimized_for_memory", "optimized_for_social_learning"};
}

void AgentBrainAdvanced::updateConfigurationHistory(const std::string& parameter, float value) {
    // Update configuration history for analysis
    parameterHistory_[parameter] = value;
}

float AgentBrainAdvanced::getHistoricalAverage(const std::string& parameter, size_t windowSize) const {
    // Calculate historical average
    // This would be implemented based on parameterHistory_
    return 0.0f;
}

bool AgentBrainAdvanced::isParameterInRange(float value, float min, float max) const {
    return value >= min && value <= max;
}

std::string AgentBrainAdvanced::parameterTypeToString(float value) const {
    // Convert parameter value to appropriate string representation
    if (std::abs(value - std::round(value)) < 0.001f) {
        return std::to_string(static_cast<int>(std::round(value)));
    } else {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3) << value;
        return oss.str();
    }
}

float AgentBrainAdvanced::computeNovelty(const SensoryPercept& percept) const {
    // Compute novelty from sensory input
    // This is a simplified implementation
    float totalVariation = 0.0f;
    
    // Vision novelty
    if (!percept.getVision().empty()) {
        for (float value : percept.getVision()) {
            totalVariation += std::abs(value - 0.5f);
        }
    }
    
    return totalVariation / std::max<size_t>(percept.getVision().size(), 1);
}

float AgentBrainAdvanced::getContextWeight(const std::string& context) const {
    // Get context-specific weight
    // This would be implemented based on context mapping
    return 1.0f;
}

void AgentBrainAdvanced::applyEnvironmentalDevelopment(const std::string& environmentalFactors) {
    // Apply environmental development effects
    // This would parse environmentalFactors and apply changes
}

} // namespace nlm
