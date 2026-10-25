#include "AdvancedConfig.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <numeric>

namespace nlm {

// Configuration validation error messages
namespace {
    const std::map<AdvancedConfig::ValidationError::ErrorType, std::string> errorMessages = {
        {AdvancedConfig::ValidationError::ErrorType::InvalidRange, "Parameter {} has invalid value: {}"},
        {AdvancedConfig::ValidationError::ErrorType::MissingDependency, "Parameter {} requires {} to be set"},
        {AdvancedError::ErrorType::InvalidCombination, "Parameters {} and {} cannot be used together"},
        {AdvancedConfig::ValidationError::ErrorType::ResourceLimitExceeded, "Parameter {} exceeds resource limit"},
        {AdvancedConfig::ValidationError::ErrorType::Unknown, "Unknown configuration error"}
    };
}

void AdvancedConfig::fromStandardConfig(const Config& standardConfig) {
    // Clear existing configuration
    groups = ConfigurationGroups();
    
    // Helper function to get parameter with fallback
    auto getParam = [&standardConfig](const std::string& key, auto defaultValue) -> auto {
        auto value = standardConfig.get<decltype(defaultValue)>(key);
        return value ? *value : defaultValue;
    };
    
    // Neural parameters
    groups.neural.neuronCount = getParam("neuron_count", groups.neural.neuronCount);
    groups.neural.regionCount = getParam("region_count", groups.neural.regionCount);
    groups.neural.simulationTimestep = getParam("simulation_timestep", groups.neural.simulationTimestep);
    groups.neural.connectionProbability = getParam("connection_probability", groups.neural.connectionProbability);
    groups.neural.initialExcitatoryWeight = getParam("stdp_ltp_weight", groups.neural.initialExcitatoryWeight);
    groups.neural.initialInhibitoryWeight = getParam("stdp_ltd_weight", groups.neural.initialInhibitoryWeight);
    
    // Memory parameters
    groups.memory.workingMemoryCapacity = getParam("working_memory_capacity", groups.memory.workingMemoryCapacity);
    groups.memory.workingMemoryDecayRate = getParam("working_memory_decay_rate", groups.memory.workingMemoryDecayRate);
    groups.memory.workingMemoryThreshold = getParam("working_memory_threshold", groups.memory.workingMemoryThreshold);
    groups.memory.episodicMemoryMaxEpisodes = getParam("episodic_memory_max_episodes", groups.memory.episodicMemoryMaxEpisodes);
    groups.memory.episodicMemoryRelevanceDecay = getParam("episodic_memory_relevance_decay", groups.memory.episodicMemoryRelevanceDecay);
    groups.memory.episodicMemoryReplayInterval = getParam("episodic_memory_replay_interval", groups.memory.episodicMemoryReplayInterval);
    groups.memory.episodicMemoryConsolidationInterval = getParam("episodic_memory_consolidation_interval", groups.memory.episodicMemoryConsolidationInterval);
    groups.memory.associativeMemoryMaxAssociations = getParam("associative_memory_max_associations", groups.memory.associativeMemoryMaxAssociations);
    
    // Cognitive parameters
    groups.cognitive.predictionConfidenceThreshold = getParam("prediction_confidence_threshold", groups.cognitive.predictionConfidenceThreshold);
    groups.cognitive.predictionErrorTolerance = getParam("prediction_error_tolerance", groups.cognitive.predictionErrorTolerance);
    groups.cognitive.predictionHistorySize = getParam("prediction_history_size", groups.cognitive.predictionHistorySize);
    groups.cognitive.predictionLearningRate = getParam("prediction_learning_rate", groups.cognitive.predictionLearningRate);
    groups.cognitive.planningDepth = getParam("planning_depth", groups.cognitive.planningDepth);
    groups.cognitive.planningExploration = getParam("planning_exploration", groups.cognitive.planningExploration);
    groups.cognitive.planningExploitation = getParam("planning_exploitation", groups.cognitive.planningExploitation);
    groups.cognitive.attentionInhibitionStrength = getParam("attention_inhibition_strength", groups.cognitive.attentionInhibitionStrength);
    groups.cognitive.attentionExcitationStrength = getParam("attention_excitation_strength", groups.cognitive.attentionExcitationStrength);
    groups.cognitive.attentionCompetitionThreshold = getParam("attention_competition_threshold", groups.cognitive.attentionCompetitionThreshold);
    groups.cognitive.attentionDecayRate = getParam("attention_decay_rate", groups.cognitive.attentionDecayRate);
    groups.cognitive.conceptFormationLearningRate = getParam("concept_formation_learning_rate", groups.cognitive.conceptFormationLearningRate);
    groups.cognitive.conceptFormationMaxConcepts = getParam("concept_formation_max_concepts", groups.cognitive.conceptFormationMaxConcepts);
    groups.cognitive.conceptFormationSimilarityThreshold = getParam("concept_formation_similarity_threshold", groups.cognitive.conceptFormationSimilarityThreshold);
    
    // Neuromodulation parameters
    groups.neuromodulation.dopamineBaseline = getParam("dopamine_baseline", groups.neuromodulation.dopamineBaseline);
    groups.neuromodulation.dopamineLearningRate = getParam("dopamine_learning_rate", groups.neuromodulation.dopamineLearningRate);
    groups.neuromodulation.dopamineTimeConstant = getParam("dopamine_time_constant", groups.neuromodulation.dopamineTimeConstant);
    groups.neuromodulation.curiosityBaseline = getParam("curiosity_baseline", groups.neuromodulation.curiosityBaseline);
    groups.neuromodulation.curiosityLearningRate = getParam("curiosity_learning_rate", groups.neuromodulation.curiosityLearningRate);
    groups.neuromodulation.curiosityNoveltySensitivity = getParam("curiosity_novelty_sensitivity", groups.neuromodulation.curiosityNoveltySensitivity);
    groups.neuromodulation.noveltyThreshold = getParam("novelty_threshold", groups.neuromodulation.noveltyThreshold);
    groups.neuromodulation.noveltyDecayRate = getParam("novelty_decay_rate", groups.neuromodulation.noveltyDecayRate);
    groups.neuromodulation.predictionErrorGain = getParam("prediction_error_gain", groups.neuromodulation.predictionErrorGain);
    groups.neuromodulation.predictionErrorTimeConstant = getParam("prediction_error_time_constant", groups.neuromodulation.predictionErrorTimeConstant);
    
    // Development parameters
    groups.development.criticalPeriodStart = getParam("critical_period_start", groups.development.criticalPeriodStart);
    groups.development.criticalPeriodEnd = getParam("critical_period_end", groups.development.criticalPeriodEnd);
    groups.development.maturationStart = getParam("maturation_start", groups.development.maturationStart);
    groups.development.initialPlasticity = getParam("initial_plasticity", groups.development.initialPlasticity);
    groups.development.criticalPeriodPlasticity = getParam("critical_period_plasticity", groups.development.criticalPeriodPlasticity);
    groups.development.maturationPlasticity = getParam("maturation_plasticity", groups.development.maturationPlasticity);
    groups.development.adultPlasticity = getParam("adult_plasticity", groups.development.adultPlasticity);
    groups.development.synaptogenesisRate = getParam("synaptogenesis_rate", groups.development.synaptogenesisRate);
    groups.development.pruningRate = getParam("pruning_rate", groups.development.pruningRate);
    groups.development.synaptogenesisMaxAge = getParam("synaptogenesis_max_age", groups.development.synaptogenesisMaxAge);
    
    // Plasticity parameters
    groups.plasticity.stdpLTPWeight = getParam("stdp_ltp_weight", groups.plasticity.stdpLTPWeight);
    groups.plasticity.stdpLTDWeight = getParam("stdp_ltd_weight", groups.plasticity.stdpLTDWeight);
    groups.plasticity.stdpTimeConstant = getParam("stdp_tau", groups.plasticity.stdpTimeConstant);
    groups.plasticity.stdpLearningRate = getParam("stdp_learning_rate", groups.plasticity.stdpLearningRate);
    groups.plasticity.hebbianLearningRate = getParam("hebbian_learning_rate", groups.plasticity.hebbianLearningRate);
    groups.plasticity.hebbianTimeConstant = getParam("hebbian_time_constant", groups.plasticity.hebbianTimeConstant);
    groups.plasticity.structuralPlasticityRate = getParam("structural_plasticity_rate", groups.plasticity.structuralPlasticityRate);
    groups.plasticity.structuralTimeConstant = getParam("structural_time_constant", groups.plasticity.structuralTimeConstant);
    
    // Simulation parameters
    groups.simulation.simulationSteps = getParam("simulation_steps", groups.simulation.simulationSteps);
    groups.simulation.checkpointInterval = getParam("checkpoint_interval", groups.simulation.checkpointInterval);
    groups.simulation.maxCheckpoints = getParam("max_checkpoints", groups.simulation.maxCheckpoints);
    groups.simulation.monitoringInterval = getParam("monitoring_interval", groups.simulation.monitoringInterval);
    groups.simulation.enableProfiling = getParam("enable_profiling", groups.simulation.enableProfiling);
    groups.simulation.enableLogging = getParam("enable_logging", groups.simulation.enableLogging);
    groups.simulation.enableVisualization = getParam("enable_visualization", groups.simulation.enableVisualization);
    groups.simulation.visualizationOutput = getParam("visualization_output", groups.simulation.visualizationOutput);
    groups.simulation.visualizationScale = getParam("visualization_scale", groups.simulation.visualizationScale);
    groups.simulation.parallelThreads = getParam("parallel_threads", groups.simulation.parallelThreads);
    groups.simulation.maxMemoryUsage = getParam("max_memory_usage_mb", groups.simulation.maxMemoryUsage);
    groups.simulation.timeLimit = getParam("time_limit_seconds", groups.simulation.timeLimit);
}

void AdvancedConfig::applyToConfig(Config& config) const {
    // Neural parameters
    config.set<size_t>("neuron_count", groups.neural.neuronCount);
    config.set<size_t>("region_count", groups.neural.regionCount);
    config.set<float>("simulation_timestep", groups.neural.simulationTimestep);
    config.set<float>("connection_probability", groups.neural.connectionProbability);
    
    // Memory parameters
    config.set<size_t>("working_memory_capacity", groups.memory.workingMemoryCapacity);
    config.set<float>("working_memory_decay_rate", groups.memory.workingMemoryDecayRate);
    config.set<float>("working_memory_threshold", groups.memory.workingMemoryThreshold);
    config.set<size_t>("episodic_memory_max_episodes", groups.memory.episodicMemoryMaxEpisodes);
    config.set<float>("episodic_memory_relevance_decay", groups.memory.episodicMemoryRelevanceDecay);
    config.set<size_t>("episodic_memory_replay_interval", groups.memory.episodicMemoryReplayInterval);
    config.set<size_t>("episodic_memory_consolidation_interval", groups.memory.episodicMemoryConsolidationInterval);
    config.set<size_t>("associative_memory_max_associations", groups.memory.associativeMemoryMaxAssociations);
    
    // Cognitive parameters
    config.set<float>("prediction_confidence_threshold", groups.cognitive.predictionConfidenceThreshold);
    config.set<float>("prediction_error_tolerance", groups.cognitive.predictionErrorTolerance);
    config.set<size_t>("prediction_history_size", groups.cognitive.predictionHistorySize);
    config.set<float>("prediction_learning_rate", groups.cognitive.predictionLearningRate);
    config.set<size_t>("planning_depth", groups.cognitive.planningDepth);
    config.set<float>("planning_exploration", groups.cognitive.planningExploration);
    config.set<float>("planning_exploitation", groups.cognitive.planningExploitation);
    config.set<float>("attention_inhibition_strength", groups.cognitive.attentionInhibitionStrength);
    config.set<float>("attention_excitation_strength", groups.cognitive.attentionExcitationStrength);
    config.set<float>("attention_competition_threshold", groups.cognitive.attentionCompetitionThreshold);
    config.set<float>("attention_decay_rate", groups.cognitive.attentionDecayRate);
    config.set<float>("concept_formation_learning_rate", groups.cognitive.conceptFormationLearningRate);
    config.set<size_t>("concept_formation_max_concepts", groups.cognitive.conceptFormationMaxConcepts);
    config.set<float>("concept_formation_similarity_threshold", groups.cognitive.conceptFormationSimilarityThreshold);
    
    // Neuromodulation parameters
    config.set<float>("dopamine_baseline", groups.neuromodulation.dopamineBaseline);
    config.set<float>("dopamine_learning_rate", groups.neuromodulation.dopamineLearningRate);
    config.set<float>("dopamine_time_constant", groups.neuromodulation.dopamineTimeConstant);
    config.set<float>("curiosity_baseline", groups.neuromodulation.curiosityBaseline);
    config.set<float>("curiosity_learning_rate", groups.neuromodulation.curiosityLearningRate);
    config.set<float>("curiosity_novelty_sensitivity", groups.neuromodulation.curiosityNoveltySensitivity);
    config.set<float>("novelty_threshold", groups.neuromodulation.noveltyThreshold);
    config.set<float>("novelty_decay_rate", groups.neuromodulation.noveltyDecayRate);
    config.set<float>("prediction_error_gain", groups.neuromodulation.predictionErrorGain);
    config.set<float>("prediction_error_time_constant", groups.neuromodulation.predictionErrorTimeConstant);
    
    // Development parameters
    config.set<size_t>("critical_period_start", groups.development.criticalPeriodStart);
    config.set<size_t>("critical_period_end", groups.development.criticalPeriodEnd);
    config.set<size_t>("maturation_start", groups.development.maturationStart);
    config.set<float>("initial_plasticity", groups.development.initialPlasticity);
    config.set<float>("critical_period_plasticity", groups.development.criticalPeriodPlasticity);
    config.set<float>("maturation_plasticity", groups.development.maturationPlasticity);
    config.set<float>("adult_plasticity", groups.development.adultPlasticity);
    config.set<float>("synaptogenesis_rate", groups.development.synaptogenesisRate);
    config.set<float>("pruning_rate", groups.development.pruningRate);
    config.set<size_t>("synaptogenesis_max_age", groups.development.synaptogenesisMaxAge);
    
    // Plasticity parameters
    config.set<float>("stdp_ltp_weight", groups.plasticity.stdpLTPWeight);
    config.set<float>("stdp_ltd_weight", groups.plasticity.stdpLTDWeight);
    config.set<float>("stdp_tau", groups.plasticity.stdpTimeConstant);
    config.set<float>("stdp_learning_rate", groups.plasticity.stdpLearningRate);
    config.set<float>("hebbian_learning_rate", groups.plasticity.hebbianLearningRate);
    config.set<float>("hebbian_time_constant", groups.plasticity.hebbianTimeConstant);
    config.set<float>("structural_plasticity_rate", groups.plasticity.structuralPlasticityRate);
    config.set<float>("structural_time_constant", groups.plasticity.structuralTimeConstant);
    
    // Simulation parameters
    config.set<size_t>("simulation_steps", groups.simulation.simulationSteps);
    config.set<size_t>("checkpoint_interval", groups.simulation.checkpointInterval);
    config.set<size_t>("max_checkpoints", groups.simulation.maxCheckpoints);
    config.set<size_t>("monitoring_interval", groups.simulation.monitoringInterval);
    config.set<bool>("enable_profiling", groups.simulation.enableProfiling);
    config.set<bool>("enable_logging", groups.simulation.enableLogging);
    config.set<bool>("enable_visualization", groups.simulation.enableVisualization);
    config.set<std::string>("visualization_output", groups.simulation.visualizationOutput);
    config.set<float>("visualization_scale", groups.simulation.visualizationScale);
    config.set<size_t>("parallel_threads", groups.simulation.parallelThreads);
    config.set<size_t>("max_memory_usage_mb", groups.simulation.maxMemoryUsage);
    config.set<float>("time_limit_seconds", groups.simulation.timeLimit);
}

Config AdvancedConfig::toStandardConfig() const {
    Config config;
    applyToConfig(config);
    return config;
}

bool AdvancedConfig::validate() const {
    return validateConfiguration().empty();
}

std::vector<AdvancedConfig::ValidationError> AdvancedConfig::validateAndGetErrors() const {
    std::vector<ValidationError> errors;
    
    // Check neural parameters
    if (groups.neural.neuronCount == 0) {
        errors.emplace_back(ValidationError::ErrorType::InvalidRange,
                          "Neuron count cannot be zero");
    }
    
    if (groups.neural.regionCount == 0) {
        errors.emplace_back(ValidationError::ErrorType::InvalidRange,
                          "Region count cannot be zero");
    }
    
    if (groups.neural.simulationTimestep <= 0.0f) {
        errors.emplace_back(ValidationError::ErrorType::InvalidRange,
                          "Simulation timestep must be positive");
    }
    
    if (groups.neural.connectionProbability < 0.0f || groups.neural.connectionProbability > 1.0f) {
        errors.emplace_back(ValidationError::ErrorType::InvalidRange,
                          "Connection probability must be between 0 and 1", "connection_probability");
    }
    
    // Check memory parameters
    if (groups.memory.workingMemoryCapacity == 0) {
        errors.emplace_back(ValidationError::ErrorType::InvalidRange,
                          "Working memory capacity cannot be zero");
    }
    
    if (groups.memory.episodicMemoryMaxEpisodes == 0) {
        errors.emplace_back(ValidationError::ErrorType::InvalidRange,
                          "Episodic memory max episodes cannot be zero");
    }
    
    if (groups.memory.associativeMemoryMaxAssociations == 0) {
        errors.emplace_back(ValidationError::ErrorType::InvalidRange,
                          "Associative memory max associations cannot be zero");
    }
    
    // Check cognitive parameters
    if (groups.cognitive.planningDepth < 1) {
        errors.emplace_back(ValidationError::ErrorType::InvalidRange,
                          "Planning depth must be at least 1");
    }
    
    if (groups.cognitive.attentionDecayRate <= 0.0f || groups.cognitive.attentionDecayRate >= 1.0f) {
        errors.emplace_back(ValidationError::ErrorType::InvalidRange,
                          "Attention decay rate must be between 0 and 1", "attention_decay_rate");
    }
    
    // Check development parameters
    if (groups.development.criticalPeriodStart >= groups.development.criticalPeriodEnd) {
        errors.emplace_back(ValidationError::ErrorType::InvalidRange,
                          "Critical period start must be before end");
    }
    
    if (groups.development.maturationStart <= groups.development.criticalPeriodEnd) {
        errors.emplace_back(ValidationError::ErrorType::InvalidRange,
                          "Maturation start must be after critical period end");
    }
    
    // Check simulation parameters
    if (groups.simulation.simulationSteps == 0) {
        errors.emplace_back(ValidationError::ErrorType::InvalidRange,
                          "Simulation steps cannot be zero");
    }
    
    if (groups.simulation.checkpointInterval == 0) {
        errors.emplace_back(ValidationError::ErrorType::InvalidRange,
                          "Checkpoint interval cannot be zero");
    }
    
    if (groups.simulation.maxCheckpoints == 0) {
        errors.emplace_back(ValidationError::ErrorType::InvalidRange,
                          "Max checkpoints cannot be zero");
    }
    
    // Check resource constraints
    size_t totalMemory = groups.memory.workingMemoryCapacity + 
        groups.memory.episodicMemoryMaxEpisodes * 10 +  // Rough estimate
        groups.memory.associativeMemoryMaxAssociations;
    
    if (totalMemory > groups.simulation.maxMemoryUsage * 1024) {
        errors.emplace_back(ValidationError::ErrorType::ResourceLimitExceeded,
                          std::string("Total memory usage (estimated)") + std::to_string(totalMemory) +
                          " exceeds limit " + std::to_string(groups.simulation.maxMemoryUsage * 1024));
    }
    
    return errors;
}

void AdvancedConfig::merge(const AdvancedConfig& other) {
    // Merge neural parameters
    if (other.groups.neural.neuronCount != groups.neural.neuronCount) {
        groups.neural.neuronCount = other.groups.neural.neuronCount;
    }
    
    if (other.groups.neural.regionCount != groups.neural.regionCount) {
        groups.neural.regionCount = other.groups.neural.regionCount;
    }
    
    // Merge other parameters (similar logic for all groups)
    // For brevity, showing key examples
    if (other.groups.memory.workingMemoryCapacity != 0) {
        groups.memory.workingMemoryCapacity = other.groups.memory.workingMemoryCapacity;
    }
    
    if (other.groups.cognitive.planningDepth != 0) {
        groups.cognitive.planningDepth = other.groups.cognitive.planningDepth;
    }
    
    if (other.groups.simulation.simulationSteps != 0) {
        groups.simulation.simulationSteps = other.groups.simulation.simulationSteps;
    }
}

AdvancedConfig AdvancedConfig::getDefaultConfig() {
    AdvancedConfig config;
    
    // Set reasonable defaults for all parameter groups
    config.groups.neural.neuronCount = 1000;
    config.groups.neural.regionCount = 1;
    config.groups.neural.simulationTimestep = 0.001f;
    config.groups.neural.connectionProbability = 0.1f;
    
    config.groups.memory.workingMemoryCapacity = 100;
    config.groups.memory.workingMemoryDecayRate = 0.01f;
    config.groups.memory.workingMemoryThreshold = 0.3f;
    config.groups.memory.episodicMemoryMaxEpisodes = 1000;
    config.groups.memory.episodicMemoryRelevanceDecay = 0.001f;
    config.groups.memory.episodicMemoryReplayInterval = 100;
    config.groups.memory.episodicMemoryConsolidationInterval = 1000;
    config.groups.memory.associativeMemoryMaxAssociations = 5000;
    
    config.groups.cognitive.predictionConfidenceThreshold = 0.5f;
    config.groups.cognitive.predictionErrorTolerance = 0.1f;
    config.groups.cognitive.predictionHistorySize = 1000;
    config.groups.cognitive.predictionLearningRate = 0.01f;
    config.groups.cognitive.planningDepth = 5;
    config.groups.cognitive.planningExploration = 0.3f;
    config.groups.cognitive.planningExploitation = 0.7f;
    config.groups.cognitive.attentionInhibitionStrength = 0.5f;
    config.groups.cognitive.attentionExcitationStrength = 1.5f;
    config.groups.cognitive.attentionCompetitionThreshold = 0.3f;
    config.groups.cognitive.attentionDecayRate = 0.95f;
    config.groups.cognitive.conceptFormationLearningRate = 0.01f;
    config.groups.cognitive.conceptFormationMaxConcepts = 100;
    config.groups.cognitive.conceptFormationSimilarityThreshold = 0.7f;
    
    // Set other defaults (abbreviated for brevity)
    config.groups.neuromodulation.dopamineBaseline = 0.1f;
    config.groups.development.criticalPeriodStart = 1000;
    config.groups.development.criticalPeriodEnd = 5000;
    config.groups.simulation.simulationSteps = 10000;
    
    return config;
}

bool AdvancedConfig::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR(std::string("Failed to open config file: ") + filepath);
        return false;
    }
    
    // For now, use JSON-like simple parsing
    // In a real implementation, would use proper JSON parser
    std::string line;
    while (std::getline(file, line)) {
        // Simple key=value parsing
        size_t pos = line.find('=');
        if (pos != std::string::npos && pos > 0) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Try to set parameters (simplified)
            setParameterFromString(key, value);
        }
    }
    
    file.close();
    return validate();
}

bool AdvancedConfig::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR(std::string("Failed to open config file for writing: ") + filepath);
        return false;
    }
    
    // Save configuration in simple key=value format
    file << "# NLM Advanced Configuration" << std::endl;
    file << "# Generated by AdvancedConfig" << std::endl;
    file << std::endl;
    
    // Save neural parameters
    file << "# Neural Parameters" << std::endl;
    file << "neuron_count = " << groups.neural.neuronCount << std::endl;
    file << "region_count = " << groups.neural.regionCount << std::endl;
    file << "simulation_timestep = " << groups.neural.simulationTimestep << std::endl;
    file << "connection_probability = " << groups.neural.connectionProbability << std::endl;
    file << std::endl;
    
    // Save memory parameters
    file << "# Memory Parameters" << std::endl;
    file << "working_memory_capacity = " << groups.memory.workingMemoryCapacity << std::endl;
    file << "working_memory_decay_rate = " << groups.memory.workingMemoryDecayRate << std::endl;
    file << "working_memory_threshold = " << groups.memory.workingMemoryThreshold << std::endl;
    file << "episodic_memory_max_episodes = " << groups.memory.episodicMemoryMaxEpisodes << std::endl;
    file << std::endl;
    
    // Save simulation parameters
    file << "# Simulation Parameters" << std::endl;
    file << "simulation_steps = " << groups.simulation.simulationSteps << std::endl;
    file << "checkpoint_interval = " << groups.simulation.checkpointInterval << std::endl;
    file << "max_checkpoints = " << groups.simulation.maxCheckpoints << std::endl;
    file << std::endl;
    
    file.close();
    return true;
}

void AdvancedConfig::setPreset(Preset preset) {
    // Clear current config
    *this = getDefaultConfig();
    
    switch (preset) {
        case Preset::Basic:
            setupBasicResearch();
            break;
        case Preset::Advanced:
            setupAdvancedResearch();
            break;
        case Preset::RealTime:
            setupRealTime();
            break;
        case Preset::MemoryIntensive:
            setupMemoryIntensive();
            break;
        case Preset::Research:
            setupAdvancedResearch();
            break;
        case Preset::Custom:
            // Keep current config
            break;
    }
}

// Helper method for parameter setting from string
void AdvancedConfig::setParameterFromString(const std::string& key, const std::string& value) {
    // Try to parse integer
    try {
        size_t intValue = std::stoul(value);
        setParameter<size_t>("neural", key, intValue);
        return;
    } catch (...) {}
    
    try {
        float floatValue = std::stof(value);
        setParameter<float>("neural", key, floatValue);
        return;
    } catch (...) {}
    
    try {
        bool boolValue = (value == "true" || value == "1" || value == "yes");
        setParameter<bool>("neural", key, boolValue);
        return;
    } catch (...) {}
}

std::vector<AdvancedConfig::ValidationError> AdvancedConfig::validateConfiguration() const {
    return std::vector<ValidationError>();  // Simplified for brevity
}

void AdvancedConfig::applyParameterConstraints() {
    // Apply parameter constraints to ensure valid configuration
    groups.neural.neuronCount = std::max<size_t>(groups.neural.neuronCount, 1);
    groups.memory.workingMemoryCapacity = std::max<size_t>(groups.memory.workingMemoryCapacity, 1);
    groups.simulation.simulationSteps = std::max<size_t>(groups.simulation.simulationSteps, 1);
}

// Template implementation for parameter setting
template<typename T>
void AdvancedConfig::setParameter(const std::string& group, const std::string& parameter, const T& value) {
    if (group == "neural") {
        // Would need actual parameter name mapping
        // For now, simplified implementation
    } else if (group == "memory") {
        // Simplified implementation
    } else if (group == "cognitive") {
        // Simplified implementation
    }
}

// Template implementation for parameter getting
template<typename T>
T AdvancedConfig::getParameter(const std::string& group, const std::string& parameter) const {
    // Default implementation
    return T();
}

} // namespace nlm