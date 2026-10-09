#include "AdvancedConfig.hpp"
#include "../core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct AdvancedConfig::Impl {
    // Store configuration values in a flat map for easy access
    std::map<std::string, ConfigValue> values;
    
    // Section prefixes for organization
    static constexpr const char* NEURAL_NETWORK_PREFIX = "neural_network.";
    static constexpr const char* PLASTICITY_PREFIX = "plasticity.";
    static constexpr const char* NEUROMODULATION_PREFIX = "neuromodulation.";
    static constexpr const char* MEMORY_PREFIX = "memory.";
    static constexpr const char* DEVELOPMENT_PREFIX = "development.";
    static constexpr const char* PERFORMANCE_PREFIX = "performance.";
    static constexpr const char* AGENT_PREFIX = "agent.";
    static constexpr const char* ENVIRONMENT_PREFIX = "environment.";
};

AdvancedConfig::AdvancedConfig() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values
    resetToDefaults();
}

AdvancedConfig::~AdvancedConfig() = default;

AdvancedConfig::AdvancedConfig(AdvancedConfig&&) noexcept = default;

AdvancedConfig& AdvancedConfig::operator=(AdvancedConfig&&) noexcept = default;

void AdvancedConfig::resetToDefaults() {
    // Neural network defaults
    neuralNetworkConfig_.neuronThreshold = -50.0;
    neuralNetworkConfig_.neuronRestPotential = -70.0;
    neuralNetworkConfig_.neuronResetPotential = -75.0;
    neuralNetworkConfig_.membraneTimeConstant = 20.0;
    neuralNetworkConfig_.refractoryPeriod = 2.0;
    neuralNetworkConfig_.initialWeightMean = 0.5;
    neuralNetworkConfig_.initialWeightStd = 0.1;
    neuralNetworkConfig_.maxWeight = 1.0;
    neuralNetworkConfig_.minWeight = 0.0;
    neuralNetworkConfig_.weightDecayRate = 0.001;
    neuralNetworkConfig_.connectionProbability = 0.15;
    neuralNetworkConfig_.connectionDistance = 100.0;
    neuralNetworkConfig_.maxConnectionsPerNeuron = 100;
    neuralNetworkConfig_.firingThreshold = 0.5;
    neuralNetworkConfig_.maxFiringRate = 100.0;
    
    // Plasticity defaults
    plasticityConfig_.stdpLtpWeight = 0.02;
    plasticityConfig_.stdpLtdWeight = 0.015;
    plasticityConfig_.stdpTau = 20.0;
    plasticityConfig_.stdpMaxTime = 100.0;
    plasticityConfig_.hebbianLearningRate = 0.001;
    plasticityConfig_.hebbianTimeConstant = 100.0;
    plasticityConfig_.synaptogenesisRate = 0.0001;
    plasticityConfig_.pruningRate = 0.00001;
    plasticityConfig_.minSynapseWeight = 0.01;
    plasticityConfig_.structuralPlasticityTimeConstant = 1000.0;
    plasticityConfig_.rewardLtpModulator = 1.0;
    plasticityConfig_.rewardLtdModulator = 0.9;
    
    // Neuromodulation defaults
    neuromodulationConfig_.dopamineBaseline = 0.0;
    neuromodulationConfig_.dopamineLearningRate = 0.01;
    neuromodulationConfig_.dopamineTimeConstant = 100.0;
    neuromodulationConfig_.dopamineSensitivity = 1.0;
    neuromodulationConfig_.noveltyThreshold = 0.5;
    neuromodulationConfig_.noveltyDecayRate = 0.99;
    neuromodulationConfig_.noveltyLearningRate = 0.01;
    neuromodulationConfig_.curiosityBaseline = 0.1;
    neuromodulationConfig_.curiosityLearningRate = 0.01;
    neuromodulationConfig_.curiosityTimeConstant = 100.0;
    neuromodulationConfig_.curiosityActionSelectionWeight = 0.5;
    neuromodulationConfig_.predictionErrorDecayRate = 0.95;
    neuromodulationConfig_.predictionErrorLearningRate = 0.01;
    
    // Memory defaults
    memoryConfig_.workingMemoryCapacity = 10;
    memoryConfig_.workingMemoryDecayRate = 0.1;
    memoryConfig_.workingMemoryCompetitionStrength = 1.0;
    memoryConfig_.episodicMemoryCapacity = 100;
    memoryConfig_.episodicMemoryReplayRate = 0.1;
    memoryConfig_.episodicMemoryConsolidationThreshold = 0.5;
    memoryConfig_.associativeMemoryCapacity = 1000;
    memoryConfig_.associativeMemoryDecayRate = 0.01;
    memoryConfig_.associativeMemoryStrength = 1.0;
    memoryConfig_.semanticMemoryCapacity = 1000;
    memoryConfig_.semanticMemoryLearningRate = 0.01;
    
    // Development defaults
    developmentConfig_.initialStageAge = 60.0;
    developmentConfig_.criticalPeriodAge = 300.0;
    developmentConfig_.maturationAge = 900.0;
    developmentConfig_.adultAge = 900.0;
    developmentConfig_.initialPlasticityLevel = 1.0;
    developmentConfig_.criticalPeriodPlasticityLevel = 0.8;
    developmentConfig_.maturationPlasticityLevel = 0.5;
    developmentConfig_.adultPlasticityLevel = 0.2;
    developmentConfig_.synaptogenesisBias = 1.0;
    developmentConfig_.pruningBias = 0.5;
    developmentConfig_.maturationBias = 0.8;
    
    // Performance defaults
    performanceConfig_.timeStep = 0.001;
    performanceConfig_.maxSimTime = 60000.0; // 60 seconds
    performanceConfig_.maxNeuronCount = 10000;
    performanceConfig_.maxSynapseCount = 100000;
    performanceConfig_.memoryPoolSize = 1024 * 1024; // 1MB
    performanceConfig_.eventQueueSize = 10000;
    performanceConfig_.enableSIMD = true;
    performanceConfig_.enableVectorization = true;
    performanceConfig_.maxThreads = std::thread::hardware_concurrency();
    performanceConfig_.loadBalancingThreshold = 0.7;
    performanceConfig_.checkpointInterval = 10000.0; // 10 seconds
    performanceConfig_.checkpointFormat = "binary";
    performanceConfig_.enableCompression = false;
    
    // Agent defaults
    agentConfig_.visionWidth = 16;
    agentConfig_.visionHeight = 16;
    agentConfig_.touchSensorCount = 8;
    agentConfig_.internalSignalCount = 4;
    agentConfig_.proprioceptionCount = 6;
    agentConfig_.motorActionCount = 6;
    agentConfig_.actionSelectionThreshold = 0.5;
    agentConfig_.actionSelectionNoise = 0.1;
    agentConfig_.curiosityThreshold = 0.3;
    agentConfig_.noveltyThreshold = 0.5;
    agentConfig_.rewardThreshold = 0.1;
    agentConfig_.learningRate = 0.01;
    agentConfig_.explorationRate = 0.2;
    agentConfig_.exploitationRate = 0.8;
    agentConfig_.balanceRatio = 0.5;
    
    // Environment defaults
    environmentConfig_.worldWidth = 100.0;
    environmentConfig_.worldHeight = 100.0;
    environmentConfig_.worldDepth = 10.0;
    environmentConfig_.gravity = 9.8;
    environmentConfig_.friction = 0.5;
    environmentConfig_.restitution = 0.7;
    environmentConfig_.resourceValue = 1.0;
    environmentConfig_.hazardDamage = 10.0;
    environmentConfig_.visibilityRange = 50.0;
    environmentConfig_.dynamicsTimeStep = 0.01;
    environmentConfig_.maxVelocity = 10.0;
    environmentConfig_.maxAngularVelocity = 5.0;
    
    // Update internal values from configs
    updateInternalValues();
}

void AdvancedConfig::updateInternalValues() {
    // Neural network
    setValue("neural_network", "neuron_threshold", neuralNetworkConfig_.neuronThreshold);
    setValue("neural_network", "neuron_rest_potential", neuralNetworkConfig_.neuronRestPotential);
    setValue("neural_network", "neuron_reset_potential", neuralNetworkConfig_.neuronResetPotential);
    setValue("neural_network", "membrane_time_constant", neuralNetworkConfig_.membraneTimeConstant);
    setValue("neural_network", "refractory_period", neuralNetworkConfig_.refractoryPeriod);
    setValue("neural_network", "initial_weight_mean", neuralNetworkConfig_.initialWeightMean);
    setValue("neural_network", "initial_weight_std", neuralNetworkConfig_.initialWeightStd);
    setValue("neural_network", "max_weight", neuralNetworkConfig_.maxWeight);
    setValue("neural_network", "min_weight", neuralNetworkConfig_.minWeight);
    setValue("neural_network", "weight_decay_rate", neuralNetworkConfig_.weightDecayRate);
    setValue("neural_network", "connection_probability", neuralNetworkConfig_.connectionProbability);
    setValue("neural_network", "connection_distance", neuralNetworkConfig_.connectionDistance);
    setValue("neural_network", "max_connections_per_neuron", neuralNetworkConfig_.maxConnectionsPerNeuron);
    setValue("neural_network", "firing_threshold", neuralNetworkConfig_.firingThreshold);
    setValue("neural_network", "max_firing_rate", neuralNetworkConfig_.maxFiringRate);
    
    // Plasticity
    setValue("plasticity", "stdp_ltp_weight", plasticityConfig_.stdpLtpWeight);
    setValue("plasticity", "stdp_ltd_weight", plasticityConfig_.stdpLtdWeight);
    setValue("plasticity", "stdp_tau", plasticityConfig_.stdpTau);
    setValue("plasticity", "stdp_max_time", plasticityConfig_.stdpMaxTime);
    setValue("plasticity", "hebbian_learning_rate", plasticityConfig_.hebbianLearningRate);
    setValue("plasticity", "hebbian_time_constant", plasticityConfig_.hebbianTimeConstant);
    setValue("plasticity", "synaptogenesis_rate", plasticityConfig_.synaptogenesisRate);
    setValue("plasticity", "pruning_rate", plasticityConfig_.pruningRate);
    setValue("plasticity", "min_synapse_weight", plasticityConfig_.minSynapseWeight);
    setValue("plasticity", "structural_plasticity_time_constant", plasticityConfig_.structuralPlasticityTimeConstant);
    setValue("plasticity", "reward_ltp_modulator", plasticityConfig_.rewardLtpModulator);
    setValue("plasticity", "reward_ltd_modulator", plasticityConfig_.rewardLtdModulator);
    
    // Neuromodulation
    setValue("neuromodulation", "dopamine_baseline", neuromodulationConfig_.dopamineBaseline);
    setValue("neuromodulation", "dopamine_learning_rate", neuromodulationConfig_.dopamineLearningRate);
    setValue("neuromodulation", "dopamine_time_constant", neuromodulationConfig_.dopamineTimeConstant);
    setValue("neuromodulation", "dopamine_sensitivity", neuromodulationConfig_.dopamineSensitivity);
    setValue("neuromodulation", "novelty_threshold", neuromodulationConfig_.noveltyThreshold);
    setValue("neuromodulation", "novelty_decay_rate", neuromodulationConfig_.noveltyDecayRate);
    setValue("neuromodulation", "novelty_learning_rate", neuromodulationConfig_.noveltyLearningRate);
    setValue("neuromodulation", "curiosity_baseline", neuromodulationConfig_.curiosityBaseline);
    setValue("neuromodulation", "curiosity_learning_rate", neuromodulationConfig_.curiosityLearningRate);
    setValue("neuromodulation", "curiosity_time_constant", neuromodulationConfig_.curiosityTimeConstant);
    setValue("neuromodulation", "curiosity_action_selection_weight", neuromodulationConfig_.curiosityActionSelectionWeight);
    setValue("neuromodulation", "prediction_error_decay_rate", neuromodulationConfig_.predictionErrorDecayRate);
    setValue("neuromodulation", "prediction_error_learning_rate", neuromodulationConfig_.predictionErrorLearningRate);
    
    // Memory
    setValue("memory", "working_memory_capacity", memoryConfig_.workingMemoryCapacity);
    setValue("memory", "working_memory_decay_rate", memoryConfig_.workingMemoryDecayRate);
    setValue("memory", "working_memory_competition_strength", memoryConfig_.workingMemoryCompetitionStrength);
    setValue("memory", "episodic_memory_capacity", memoryConfig_.episodicMemoryCapacity);
    setValue("memory", "episodic_memory_replay_rate", memoryConfig_.episodicMemoryReplayRate);
    setValue("memory", "episodic_memory_consolidation_threshold", memoryConfig_.episodicMemoryConsolidationThreshold);
    setValue("memory", "associative_memory_capacity", memoryConfig_.associativeMemoryCapacity);
    setValue("memory", "associative_memory_decay_rate", memoryConfig_.associativeMemoryDecayRate);
    setValue("memory", "associative_memory_strength", memoryConfig_.associativeMemoryStrength);
    setValue("memory", "semantic_memory_capacity", memoryConfig_.semanticMemoryCapacity);
    setValue("memory", "semantic_memory_learning_rate", memoryConfig_.semanticMemoryLearningRate);
    
    // Development
    setValue("development", "initial_stage_age", developmentConfig_.initialStageAge);
    setValue("development", "critical_period_age", developmentConfig_.criticalPeriodAge);
    setValue("development", "maturation_age", developmentConfig_.maturationAge);
    setValue("development", "adult_age", developmentConfig_.adultAge);
    setValue("development", "initial_plasticity_level", developmentConfig_.initialPlasticityLevel);
    setValue("development", "critical_period_plasticity_level", developmentConfig_.criticalPeriodPlasticityLevel);
    setValue("development", "maturation_plasticity_level", developmentConfig_.maturationPlasticityLevel);
    setValue("development", "adult_plasticity_level", developmentConfig_.adultPlasticityLevel);
    setValue("development", "synaptogenesis_bias", developmentConfig_.synaptogenesisBias);
    setValue("development", "pruning_bias", developmentConfig_.pruningBias);
    setValue("development", "maturation_bias", developmentConfig_.maturationBias);
    
    // Performance
    setValue("performance", "time_step", performanceConfig_.timeStep);
    setValue("performance", "max_sim_time", performanceConfig_.maxSimTime);
    setValue("performance", "max_neuron_count", performanceConfig_.maxNeuronCount);
    setValue("performance", "max_synapse_count", performanceConfig_.maxSynapseCount);
    setValue("performance", "memory_pool_size", performanceConfig_.memoryPoolSize);
    setValue("performance", "event_queue_size", performanceConfig_.eventQueueSize);
    setValue("performance", "enable_simd", performanceConfig_.enableSIMD);
    setValue("performance", "enable_vectorization", performanceConfig_.enableVectorization);
    setValue("performance", "max_threads", performanceConfig_.maxThreads);
    setValue("performance", "load_balancing_threshold", performanceConfig_.loadBalancingThreshold);
    setValue("performance", "checkpoint_interval", performanceConfig_.checkpointInterval);
    setValue("performance", "checkpoint_format", performanceConfig_.checkpointFormat);
    setValue("performance", "enable_compression", performanceConfig_.enableCompression);
    
    // Agent
    setValue("agent", "vision_width", agentConfig_.visionWidth);
    setValue("agent", "vision_height", agentConfig_.visionHeight);
    setValue("agent", "touch_sensor_count", agentConfig_.touchSensorCount);
    setValue("agent", "internal_signal_count", agentConfig_.internalSignalCount);
    setValue("agent", "proprioception_count", agentConfig_.proprioceptionCount);
    setValue("agent", "motor_action_count", agentConfig_.motorActionCount);
    setValue("agent", "action_selection_threshold", agentConfig_.actionSelectionThreshold);
    setValue("agent", "action_selection_noise", agentConfig_.actionSelectionNoise);
    setValue("agent", "curiosity_threshold", agentConfig_.curiosityThreshold);
    setValue("agent", "novelty_threshold", agentConfig_.noveltyThreshold);
    setValue("agent", "reward_threshold", agentConfig_.rewardThreshold);
    setValue("agent", "learning_rate", agentConfig_.learningRate);
    setValue("agent", "exploration_rate", agentConfig_.explorationRate);
    setValue("agent", "exploitation_rate", agentConfig_.exploitationRate);
    setValue("agent", "balance_ratio", agentConfig_.balanceRatio);
    
    // Environment
    setValue("environment", "world_width", environmentConfig_.worldWidth);
    setValue("environment", "world_height", environmentConfig_.worldHeight);
    setValue("environment", "world_depth", environmentConfig_.worldDepth);
    setValue("environment", "gravity", environmentConfig_.gravity);
    setValue("environment", "friction", environmentConfig_.friction);
    setValue("environment", "restitution", environmentConfig_.restitution);
    setValue("environment", "resource_value", environmentConfig_.resourceValue);
    setValue("environment", "hazard_damage", environmentConfig_.hazardDamage);
    setValue("environment", "visibility_range", environmentConfig_.visibilityRange);
    setValue("environment", "dynamics_time_step", environmentConfig_.dynamicsTimeStep);
    setValue("environment", "max_velocity", environmentConfig_.maxVelocity);
    setValue("environment", "max_angular_velocity", environmentConfig_.maxAngularVelocity);
}

template<typename T>
T AdvancedConfig::getValue(const std::string& section, const std::string& key) const {
    std::string fullKey = section + "." + key;
    auto it = pImpl->values.find(fullKey);
    if (it == pImpl->values.end()) {
        throw std::runtime_error("Configuration key not found: " + fullKey);
    }
    try {
        return std::get<T>(it->second);
    } catch (const std::bad_variant_access&) {
        throw std::runtime_error("Configuration value type mismatch for key: " + fullKey);
    }
}

template<typename T>
void AdvancedConfig::setValue(const std::string& section, const std::string& key, const T& value) {
    std::string fullKey = section + "." + key;
    pImpl->values[fullKey] = ConfigValue(value);
}

bool AdvancedConfig::hasValue(const std::string& section, const std::string& key) const {
    std::string fullKey = section + "." + key;
    return pImpl->values.find(fullKey) != pImpl->values.end();
}

std::vector<std::string> AdvancedConfig::getKeys(const std::string& section) const {
    std::vector<std::string> keys;
    std::string prefix = section + ".";
    
    for (const auto& pair : pImpl->values) {
        if (pair.first.compare(0, prefix.size(), prefix) == 0) {
            keys.push_back(pair.first.substr(prefix.size()));
        }
    }
    
    return keys;
}

std::vector<std::string> AdvancedConfig::getSections() const {
    std::vector<std::string> sections;
    for (const auto& pair : pImpl->values) {
        size_t dotPos = pair.first.find('.');
        if (dotPos != std::string::npos) {
            std::string section = pair.first.substr(0, dotPos);
            if (std::find(sections.begin(), sections.end(), section) == sections.end()) {
                sections.push_back(section);
            }
        }
    }
    return sections;
}

bool AdvancedConfig::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Advanced Configuration File\n";
    file << "# Generated on " << __TIMESTAMP__ << "\n\n";
    
    for (const auto& pair : pImpl->values) {
        file << pair.first << " = ";
        
        // Simple serialization based on type
        try {
            if (std::holds_alternative<int>(pair.second)) {
                file << std::get<int>(pair.second);
            } else if (std::holds_alternative<double>(pair.second)) {
                file << std::get<double>(pair.second);
            } else if (std::holds_alternative<bool>(pair.second)) {
                file << (std::get<bool>(pair.second) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(pair.second)) {
                file << "\"" << std::get<std::string>(pair.second) << "\"";
            } else if (std::holds_alternative<std::vector<int>>(pair.second)) {
                const auto& vec = std::get<std::vector<int>>(pair.second);
                file << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << vec[i];
                }
                file << "]";
            } else if (std::holds_alternative<std::vector<double>>(pair.second)) {
                const auto& vec = std::get<std::vector<double>>(pair.second);
                file << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << vec[i];
                }
                file << "]";
            }
        } catch (const std::bad_variant_access&) {
            file << "UNKNOWN_TYPE";
        }
        
        file << "\n";
    }
    
    return true;
}

bool AdvancedConfig::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    pImpl->values.clear();
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            continue; // Skip malformed lines
        }
        
        std::string key = line.substr(0, equalsPos);
        std::string valueStr = line.substr(equalsPos + 1);
        
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        valueStr.erase(0, valueStr.find_first_not_of(" \t\r\n"));
        valueStr.erase(valueStr.find_last_not_of(" \t\r\n") + 1);
        
        // Simple parsing for basic types
        if (valueStr.find("[") != std::string::npos && valueStr.find("]") != std::string::npos) {
            // Try to parse as array
            size_t bracketStart = valueStr.find("[");
            size_t bracketEnd = valueStr.find("]");
            if (bracketStart < bracketEnd) {
                std::string arrayContent = valueStr.substr(bracketStart + 1, bracketEnd - bracketStart - 1);
                // Simple array parsing - would be more sophisticated in real implementation
                // For now, just store as string
                pImpl->values[key] = ConfigValue(valueStr);
                continue;
            }
        }
        
        // Parse basic types
        if (valueStr == "true" || valueStr == "false") {
            bool boolValue = (valueStr == "true");
            pImpl->values[key] = ConfigValue(boolValue);
        } else if (valueStr.find('.') != std::string::npos) {
            try {
                double doubleValue = std::stod(valueStr);
                pImpl->values[key] = ConfigValue(doubleValue);
            } catch (...) {
                pImpl->values[key] = ConfigValue(valueStr);
            }
        } else {
            try {
                int intValue = std::stoi(valueStr);
                pImpl->values[key] = ConfigValue(intValue);
            } catch (...) {
                pImpl->values[key] = ConfigValue(valueStr);
            }
        }
    }
    
    // Update config structures from loaded values
    updateConfigsFromInternalValues();
    
    return true;
}

void AdvancedConfig::updateConfigsFromInternalValues() {
    // Update neural network config
    neuralNetworkConfig_.neuronThreshold = getValue<double>("neural_network", "neuron_threshold");
    neuralNetworkConfig_.neuronRestPotential = getValue<double>("neural_network", "neuron_rest_potential");
    neuralNetworkConfig_.neuronResetPotential = getValue<double>("neural_network", "neuron_reset_potential");
    neuralNetworkConfig_.membraneTimeConstant = getValue<double>("neural_network", "membrane_time_constant");
    neuralNetworkConfig_.refractoryPeriod = getValue<double>("neural_network", "refractory_period");
    neuralNetworkConfig_.initialWeightMean = getValue<double>("neural_network", "initial_weight_mean");
    neuralNetworkConfig_.initialWeightStd = getValue<double>("neural_network", "initial_weight_std");
    neuralNetworkConfig_.maxWeight = getValue<double>("neural_network", "max_weight");
    neuralNetworkConfig_.minWeight = getValue<double>("neural_network", "min_weight");
    neuralNetworkConfig_.weightDecayRate = getValue<double>("neural_network", "weight_decay_rate");
    neuralNetworkConfig_.connectionProbability = getValue<double>("neural_network", "connection_probability");
    neuralNetworkConfig_.connectionDistance = getValue<double>("neural_network", "connection_distance");
    neuralNetworkConfig_.maxConnectionsPerNeuron = getValue<size_t>("neural_network", "max_connections_per_neuron");
    neuralNetworkConfig_.firingThreshold = getValue<double>("neural_network", "firing_threshold");
    neuralNetworkConfig_.maxFiringRate = getValue<double>("neural_network", "max_firing_rate");
    
    // Update other configs similarly...
    // (Implement for all other sections)
}

std::string AdvancedConfig::summary() const {
    std::ostringstream oss;
    oss << "Advanced NLM Configuration (" << pImpl->values.size() << " entries):\n";
    
    for (const auto& section : getSections()) {
        oss << "\nSection: " << section << "\n";
        oss << "  ";
        for (const auto& key : getKeys(section)) {
            std::string fullKey = section + "." + key;
            auto it = pImpl->values.find(fullKey);
            if (it != pImpl->values.end()) {
                oss << key << "=";
                try {
                    if (std::holds_alternative<int>(it->second)) {
                        oss << std::get<int>(it->second);
                    } else if (std::holds_alternative<double>(it->second)) {
                        oss << std::get<double>(it->second);
                    } else if (std::holds_alternative<bool>(it->second)) {
                        oss << (std::get<bool>(it->second) ? "true" : "false");
                    } else if (std::holds_alternative<std::string>(it->second)) {
                        oss << "\"" << std::get<std::string>(it->second) << "\"";
                    }
                } catch (const std::bad_variant_access&) {
                    oss << "UNKNOWN";
                }
                oss << " ";
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

void AdvancedConfig::applyTo(Config& config) const {
    // Apply advanced configuration to existing Config object
    // This allows advanced users to easily upgrade their basic configs
    
    // Convert advanced config values to basic config keys
    // This would map advanced configuration parameters to the basic config system
    
    // Example mappings:
    config.set("brain.v_thresh", neuralNetworkConfig_.neuronThreshold);
    config.set("brain.v_rest", neuralNetworkConfig_.neuronRestPotential);
    config.set("brain.tau_mem", neuralNetworkConfig_.membraneTimeConstant);
    config.set("brain.initial_weight_mean", neuralNetworkConfig_.initialWeightMean);
    config.set("brain.initial_weight_std", neuralNetworkConfig_.initialWeightStd);
    
    // Add more mappings as needed
}

AdvancedConfig createDefaultAdvancedConfig() {
    return AdvancedConfig();
}

} // namespace nlm
