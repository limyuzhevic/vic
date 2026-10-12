// ExperimentFactory.cpp implementation
#include "ExperimentFactory.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Static members definition
std::map<std::string, ExperimentFactory::ExperimentConfig> ExperimentFactory::defaultConfigs;

std::unique_ptr<IExperiment> ExperimentFactory::createExperiment(
    const std::string& experimentName,
    const ExperimentConfig& config) {
    
    // Create experiment based on name
    if (experimentName == "BasicConnectivityExperiment") {
        return std::make_unique<BasicConnectivityExperiment>(config);
    } else if (experimentName == "PlasticityExperiment") {
        return std::make_unique<PlasticityExperiment>(config);
    } else if (experimentName == "STDPVerificationExperiment") {
        return std::make_unique<STDPVerificationExperiment>(config);
    } else if (experimentName == "IntegratedBrainExperiment") {
        return std::make_unique<IntegratedBrainExperiment>(config);
    } else if (experimentName == "MemorySystemExperiment") {
        return std::make_unique<MemorySystemExperiment>(config);
    } else if (experimentName == "PredictionSystemExperiment") {
        return std::make_unique<PredictionSystemExperiment>(config);
    } else if (experimentName == "CognitiveEngineExperiment") {
        return std::make_unique<CognitiveEngineExperiment>(config);
    } else if (experimentName == "NeuromodulationExperiment") {
        return std::make_unique<NeuromodulationExperiment>(config);
    } else if (experimentName == "DevelopmentExperiment") {
        return std::make_unique<DevelopmentExperiment>(config);
    } else if (experimentName == "StructuralPlasticityExperiment") {
        return std::make_unique<StructuralPlasticityExperiment>(config);
    } else {
        return nullptr; // Experiment not found
    }
}

std::vector<std::unique_ptr<IExperiment>> ExperimentFactory::createExperimentBatch(
    const std::vector<std::string>& experimentNames,
    const ExperimentConfig& config) {
    
    std::vector<std::unique_ptr<IExperiment>> experiments;
    
    for (const auto& name : experimentNames) {
        auto exp = createExperiment(name, config);
        if (exp) {
            experiments.push_back(std::move(exp));
        }
    }
    
    return experiments;
}

bool ExperimentFactory::loadConfigFromFile(const std::string& filepath, ExperimentConfig& config) {
    // TODO: Implement file loading
    return false;
}

bool ExperimentFactory::saveConfigToFile(const std::string& filepath, const ExperimentConfig& config) {
    // TODO: Implement file saving
    return false;
}

ExperimentFactory::ExperimentConfig ExperimentFactory::getDefaultConfig(const std::string& experimentName) {
    if (defaultConfigs.find(experimentName) != defaultConfigs.end()) {
        return defaultConfigs[experimentName];
    }
    
    // Return empty config if not found
    return ExperimentConfig(experimentName);
}

ExperimentFactory::ExperimentConfig ExperimentFactory::getPhase2Config(const std::string& experimentName) {
    ExperimentConfig config(experimentName);
    
    // Phase 2 specific settings
    if (experimentName == "BasicConnectivityExperiment") {
        config.doubleParameters["injection_current"] = 50.0;
        config.doubleParameters["simulation_steps"] = 100;
        config.doubleParameters["target_neurons"] = 10;
    } else if (experimentName == "PlasticityExperiment") {
        config.doubleParameters["stdp_enabled"] = true;
        config.doubleParameters["hebbian_enabled"] = true;
        config.doubleParameters["learning_rate"] = 0.01f;
        config.doubleParameters["plasticity_steps"] = 1000;
        config.doubleParameters["input_strength"] = 30.0;
    } else if (experimentName == "STDPVerificationExperiment") {
        config.doubleParameters["num_trials"] = 50;
        config.doubleParameters["pre_post_delay"] = 2;
        config.doubleParameters["neuron_current"] = 60.0;
    }
    
    return config;
}

ExperimentFactory::ExperimentConfig ExperimentFactory::getPhase6Config(const std::string& experimentName) {
    ExperimentConfig config(experimentName);
    
    // Phase 6 specific settings
    if (experimentName == "IntegratedBrainExperiment") {
        config.doubleParameters["max_steps"] = 2000;
        config.doubleParameters["neuron_count"] = 500;
        config.doubleParameters["integration_threshold"] = 0.6;
    } else if (experimentName == "MemorySystemExperiment") {
        config.doubleParameters["num_working_memory_tests"] = 20;
        config.doubleParameters["num_episodic_tests"] = 15;
        config.doubleParameters["num_associative_tests"] = 25;
    } else if (experimentName == "PredictionSystemExperiment") {
        config.doubleParameters["num_trials"] = 100;
        config.doubleParameters["prediction_window"] = 5;
        config.doubleParameters["error_threshold"] = 0.1;
    } else if (experimentName == "CognitiveEngineExperiment") {
        config.doubleParameters["num_planning_tests"] = 30;
        config.doubleParameters["num_concept_tests"] = 50;
        config.doubleParameters["num_attention_tests"] = 40;
        config.doubleParameters["planning_confidence_threshold"] = 0.7;
    } else if (experimentName == "NeuromodulationExperiment") {
        config.doubleParameters["num_curiosity_trials"] = 30;
        config.doubleParameters["num_novelty_trials"] = 25;
        config.doubleParameters["simulation_steps"] = 500;
    } else if (experimentName == "DevelopmentExperiment") {
        config.doubleParameters["total_development_steps"] = 10000;
        config.doubleParameters["synaptogenesis_phase"] = 2000;
        config.doubleParameters["sensory_phase"] = 3000;
    } else if (experimentName == "StructuralPlasticityExperiment") {
        config.doubleParameters["num_synaptogenesis_tests"] = 20;
        config.doubleParameters["num_pruning_tests"] = 15;
        config.doubleParameters["simulation_steps"] = 1000;
    }
    
    return config;
}

} // namespace nlm
