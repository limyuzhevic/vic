#include "AdvancedConfigManager.hpp"
#include "core/Logger/Logger.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

// Implementation of AdvancedConfigManager methods

void AdvancedConfigManager::applyDevelopmentProfile(std::shared_ptr<Config> config) {
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.05f);
    config->set("stdp_ltp_weight", 0.005f);
    config->set("stdp_ltd_weight", 0.004f);
    config->set("synaptogenesis_rate", 0.00005f);
    config->set("pruning_rate", 0.000005f);
    config->set("enable_structural_plasticity", true);
    config->set("enable_development", true);
    config->set("enable_curiosity", true);
}

void AdvancedConfigManager::applySmallScaleProfile(std::shared_ptr<Config> config) {
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", static_cast<int64_t>(500));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.1f);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    config->set("enable_structural_plasticity", true);
    config->set("enable_development", true);
    config->set("enable_curiosity", true);
}

void AdvancedConfigManager::applyMediumScaleProfile(std::shared_ptr<Config> config) {
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", static_cast<int64_t>(1000));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("connection_probability", 0.15f);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("synaptogenesis_rate", 0.0002f);
    config->set("pruning_rate", 0.00002f);
    config->set("enable_structural_plasticity", true);
    config->set("enable_development", true);
    config->set("enable_curiosity", true);
    config->set("enable_reward_modulation", true);
    config->set("enable_prediction", true);
}

void AdvancedConfigManager::applyLargeScaleProfile(std::shared_ptr<Config> config) {
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", static_cast<int64_t>(5000));
    config->set("region_count", static_cast<int64_t>(3));
    config->set("connection_probability", 0.2f);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("synaptogenesis_rate", 0.0003f);
    config->set("pruning_rate", 0.00003f);
    config->set("enable_structural_plasticity", true);
    config->set("enable_development", true);
    config->set("enable_curiosity", true);
    config->set("enable_reward_modulation", true);
    config->set("enable_prediction", true);
    config->set("enable_working_memory", true);
    config->set("enable_episodic_memory", true);
    config->set("enable_semantic_memory", true);
}

void AdvancedConfigManager::applyCognitiveDevelopmentProfile(std::shared_ptr<Config> config) {
    applyMediumScaleProfile(config);
    config->set("enable_concept_formation", true);
    config->set("enable_planning", true);
    config->set("enable_attention", true);
    config->set("enable_social_learning", false);
    config->set("enable_continual_learning", true);
    config->set("critical_period_length", 100.0);
    config->set("plasticity_decay_rate", 0.001);
}

void AdvancedConfigManager::applyNeuromodulationStudyProfile(std::shared_ptr<Config> config) {
    applySmallScaleProfile(config);
    config->set("enable_reward_modulation", true);
    config->set("enable_curiosity", true);
    config->set("enable_novelty_detection", true);
    config->set("dopamine_scale", 1.0f);
    config->set("curiosity_threshold", 0.3f);
    config->set("novelty_decay_rate", 0.95f);
    config->set("prediction_error_weight", 0.5f);
}

void AdvancedConfigManager::applyMemoryIntegrationProfile(std::shared_ptr<Config> config) {
    applyMediumScaleProfile(config);
    config->set("enable_working_memory", true);
    config->set("enable_episodic_memory", true);
    config->set("enable_semantic_memory", true);
    config->set("enable_procedural_memory", false);
    config->set("working_memory_capacity", 100);
    config->set("episodic_memory_capacity", 1000);
    config->set("memory_replay_rate", 0.1f);
    config->set("memory_consolidation_threshold", 0.7f);
}

void AdvancedConfigManager::applyLearningDynamicsProfile(std::shared_ptr<Config> config) {
    applyLargeScaleProfile(config);
    config->set("enable_structural_plasticity", true);
    config->set("enable_development", true);
    config->set("enable_reward_modulation", true);
    config->set("stdp_ltp_weight", 0.02f);
    config->set("stdp_ltd_weight", 0.018f);
    config->set("learning_rate", 0.001);
    config->set("weight_decay", 0.0001f);
    config->set("eligibility_trace_decay", 0.9f);
}

void AdvancedConfigManager::applyBehavioralModelingProfile(std::shared_ptr<Config> config) {
    applyLargeScaleProfile(config);
    config->set("enable_structural_plasticity", true);
    config->set("enable_development", true);
    config->set("enable_curiosity", true);
    config->set("enable_reward_modulation", true);
    config->set("enable_prediction", true);
    config->set("action_selection_temperature", 1.0f);
    config->set("exploration_rate", 0.2f);
    config->set("exploitation_rate", 0.8f);
    config->set("behavioral_constraints_enabled", true);
}

std::optional<double> AdvancedConfigManager::getNumericValue(std::shared_ptr<Config> config, const std::string& key) const {
    auto int_value = config->get<int64_t>(key);
    if (int_value.has_value()) {
        return static_cast<double>(int_value.value());
    }
    
    auto float_value = config->get<double>(key);
    if (float_value.has_value()) {
        return float_value.value();
    }
    
    return std::nullopt;
}

std::string AdvancedConfigManager::getCurrentDate() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

AdvancedConfigManager::ParameterMetadata AdvancedConfigManager::getParameterMetadata(const std::string& key) const {
    auto it = parameterMetadata_.find(key);
    if (it != parameterMetadata_.end()) {
        return it->second;
    }
    return ParameterMetadata{}; // Empty metadata
}

bool AdvancedConfigManager::saveResearchSceneToFile(const ResearchScene& scene, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== Research Scene ===" << std::endl;
    file << "Name: " << scene.name << std::endl;
    file << "Description: " << scene.description << std::endl;
    file << "Profile: " << static_cast<int>(scene.profile) << std::endl;
    file << "Author: " << scene.author << std::endl;
    file << "Date: " << scene.date << std::endl;
    file << "Version: " << scene.version << std::endl;
    file << "Parameters:" << std::endl;
    
    for (const auto& param : scene.parameters) {
        file << "  " << param.first << " = " << param.second << std::endl;
    }
    
    file.close();
    return true;
}

bool AdvancedConfigManager::loadResearchSceneFromFile(ResearchScene& scene, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool in_parameters = false;
    
    while (std::getline(file, line)) {
        if (line.find("Name: ") == 0) {
            scene.name = line.substr(6);
        } else if (line.find("Description: ") == 0) {
            scene.description = line.substr(12);
        } else if (line.find("Profile: ") == 0) {
            try {
                scene.profile = static_cast<ResearchProfile>(std::stoi(line.substr(9)));
            } catch (...) {
                scene.profile = ResearchProfile::Custom;
            }
        } else if (line.find("Author: ") == 0) {
            scene.author = line.substr(7);
        } else if (line.find("Date: ") == 0) {
            scene.date = line.substr(6);
        } else if (line.find("Version: ") == 0) {
            scene.version = line.substr(8);
        } else if (line == "Parameters:") {
            in_parameters = true;
        } else if (in_parameters && !line.empty() && line.find("  ") == 0) {
            size_t pos = line.find(" = ");
            if (pos != std::string::npos) {
                std::string key = line.substr(2, pos - 2);
                std::string value = line.substr(pos + 3);
                scene.parameters[key] = value;
            }
        }
    }
    
    file.close();
    return true;
}

void AdvancedConfigManager::initializeParameterMetadata() {
    // Initialize parameter metadata (same as in header)
    // Neuron parameters
    parameterMetadata_["neuron_count"] = ParameterMetadata{
        "neuron_count", "Total number of neurons", "neuron", "int",
        std::nullopt, std::nullopt, "1000"
    };
    
    parameterMetadata_["region_count"] = ParameterMetadata{
        "region_count", "Number of neural regions", "neuron", "int",
        std::nullopt, std::nullopt, "1"
    };
    
    parameterMetadata_["connection_probability"] = ParameterMetadata{
        "connection_probability", "Probability of connection between neurons", "connectivity", "float",
        0.0, 1.0, "0.1"
    };
    
    // Plasticity parameters
    parameterMetadata_["stdp_ltp_weight"] = ParameterMetadata{
        "stdp_ltp_weight", "STDP long-term potentiation weight", "plasticity", "float",
        0.0, std::nullopt, "0.01"
    };
    
    parameterMetadata_["stdp_ltd_weight"] = ParameterMetadata{
        "stdp_ltd_weight", "STDP long-term depression weight", "plasticity", "float",
        0.0, std::nullopt, "0.012"
    };
    
    // Development parameters
    parameterMetadata_["synaptogenesis_rate"] = ParameterMetadata{
        "synaptogenesis_rate", "Rate of new synapse formation", "development", "float",
        0.0, std::nullopt, "0.0001"
    };
    
    parameterMetadata_["pruning_rate"] = ParameterMetadata{
        "pruning_rate", "Rate of synapse elimination", "development", "float",
        0.0, std::nullopt, "0.00001"
    };
    
    // Neuromodulation parameters
    parameterMetadata_["dopamine_scale"] = ParameterMetadata{
        "dopamine_scale", "Dopamine modulation scale", "neuromodulation", "float",
        0.0, std::nullopt, "1.0"
    };
    
    parameterMetadata_["curiosity_threshold"] = ParameterMetadata{
        "curiosity_threshold", "Curiosity-driven exploration threshold", "neuromodulation", "float",
        0.0, 1.0, "0.3"
    };
    
    // Memory parameters
    parameterMetadata_["working_memory_capacity"] = ParameterMetadata{
        "working_memory_capacity", "Working memory capacity in traces", "memory", "int",
        0, std::nullopt, "100"
    };
    
    parameterMetadata_["episodic_memory_capacity"] = ParameterMetadata{
        "episodic_memory_capacity", "Episodic memory capacity in episodes", "memory", "int",
        0, std::nullopt, "1000"
    };
    
    // Development stage parameters
    parameterMetadata_["critical_period_length"] = ParameterMetadata{
        "critical_period_length", "Length of critical period (time units)", "development", "float",
        0.0, std::nullopt, "100.0"
    };
    
    // System flags
    parameterMetadata_["enable_structural_plasticity"] = ParameterMetadata{
        "enable_structural_plasticity", "Enable structural plasticity (synaptogenesis/pruning)", "system", "bool",
        std::nullopt, std::nullopt, "true"
    };
    
    parameterMetadata_["enable_development"] = ParameterMetadata{
        "enable_development", "Enable developmental processes", "system", "bool",
        std::nullopt, std::nullopt, "true"
    };
    
    parameterMetadata_["enable_curiosity"] = ParameterMetadata{
        "enable_curiosity", "Enable curiosity-driven exploration", "system", "bool",
        std::nullopt, std::nullopt, "true"
    };
    
    parameterMetadata_["enable_reward_modulation"] = ParameterMetadata{
        "enable_reward_modulation", "Enable reward-based learning", "system", "bool",
        std::nullopt, std::nullopt, "true"
    };
    
    parameterMetadata_["enable_prediction"] = ParameterMetadata{
        "enable_prediction", "Enable prediction system", "system", "bool",
        std::nullopt, std::nullopt, "true"
    };
    
    // Advanced features
    parameterMetadata_["enable_working_memory"] = ParameterMetadata{
        "enable_working_memory", "Enable working memory system", "memory", "bool",
        std::nullopt, std::nullopt, "true"
    };
    
    parameterMetadata_["enable_episodic_memory"] = ParameterMetadata{
        "enable_episodic_memory", "Enable episodic memory system", "memory", "bool",
        std::nullopt, std::nullopt, "true"
    };
    
    parameterMetadata_["enable_semantic_memory"] = ParameterMetadata{
        "enable_semantic_memory", "Enable semantic memory system", "memory", "bool",
        std::nullopt, std::nullopt, "false"
    };
    
    parameterMetadata_["enable_planning"] = ParameterMetadata{
        "enable_planning", "Enable neural planning system", "cognition", "bool",
        std::nullopt, std::nullopt, "true"
    };
    
    parameterMetadata_["enable_attention"] = ParameterMetadata{
        "enable_attention", "Enable attention system", "cognition", "bool",
        std::nullopt, std::nullopt, "true"
    };
    
    parameterMetadata_["enable_concept_formation"] = ParameterMetadata{
        "enable_concept_formation", "Enable concept formation system", "cognition", "bool",
        std::nullopt, std::nullopt, "true"
    };
    
    // Performance parameters
    parameterMetadata_["simulation_timestep"] = ParameterMetadata{
        "simulation_timestep", "Simulation time step size", "performance", "float",
        0.0001, std::nullopt, "0.001"
    };
    
    parameterMetadata_["random_seed"] = ParameterMetadata{
        "random_seed", "Random seed for reproducibility", "performance", "int",
        std::nullopt, std::nullopt, "42"
    };
}

} // namespace nlm
