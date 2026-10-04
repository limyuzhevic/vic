// Experiment configuration manager implementation

#include "experiments/ExperimentConfigManager.hpp"
#include "experiments/Experiment.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <stdexcept>

namespace nlm {

// Constructor initializes default configurations
ExperimentConfigManager::ExperimentConfigManager() {
    initializeDefaultConfigs();
}

void ExperimentConfigManager::initializeDefaultConfigs() {
    // Basic connectivity experiment configuration
    ExperimentConfig basicConfig;
    basicConfig.name = "BasicConnectivityExperiment";
    basicConfig.type = ExperimentType::BasicConnectivity;
    basicConfig.duration = 50.0;
    basicConfig.timestep = 0.001;
    basicConfig.neuronCount = 100;
    basicConfig.connectionProbability = 0.05f; // Lower connectivity for simple test
    basicConfig.enablePlasticity = false;
    basicConfig.enableNeuromodulation = false;
    basicConfig.enableDevelopment = false;
    basicConfig.noveltyThreshold = 0.3f;
    basicConfig.curiosityThreshold = 0.2f;
    basicConfig.rewardScale = 0.1f;
    basicConfig.predictionErrorWeight = 0.5f;
    experimentConfigs_[ExperimentType::BasicConnectivity] = basicConfig;
    
    // Plasticity learning experiment configuration
    ExperimentConfig plasticityConfig;
    plasticityConfig.name = "PlasticityLearningExperiment";
    plasticityConfig.type = ExperimentType::PlasticityLearning;
    plasticityConfig.duration = 1000.0;
    plasticityConfig.timestep = 0.001;
    plasticityConfig.neuronCount = 500;
    plasticityConfig.connectionProbability = 0.15f;
    plasticityConfig.enablePlasticity = true;
    plasticityConfig.enableNeuromodulation = true;
    plasticityConfig.enableDevelopment = true;
    plasticityConfig.noveltyThreshold = 0.3f;
    plasticityConfig.curiosityThreshold = 0.4f;
    plasticityConfig.rewardScale = 0.5f;
    plasticityConfig.predictionErrorWeight = 1.0f;
    experimentConfigs_[ExperimentType::PlasticityLearning] = plasticityConfig;
    
    // STDP verification experiment configuration
    ExperimentConfig stdpConfig;
    stdpConfig.name = "STDPVerificationExperiment";
    stdpConfig.type = ExperimentType::STDPVerification;
    stdpConfig.duration = 200.0;
    stdpConfig.timestep = 0.001;
    stdpConfig.neuronCount = 200;
    stdpConfig.connectionProbability = 0.1f;
    stdpConfig.enablePlasticity = true;
    stdpConfig.enableNeuromodulation = true;
    stdpConfig.enableDevelopment = false;
    stdpConfig.noveltyThreshold = 0.2f;
    stdpConfig.curiosityThreshold = 0.3f;
    stdpConfig.rewardScale = 0.2f;
    stdpConfig.predictionErrorWeight = 2.0f;
    experimentConfigs_[ExperimentType::STDPVerification] = stdpConfig;
    
    // Comprehensive learning experiment configuration
    ExperimentConfig comprehensiveConfig;
    comprehensiveConfig.name = "ComprehensiveLearningExperiment";
    comprehensiveConfig.type = ExperimentType::ComprehensiveLearning;
    comprehensiveConfig.duration = 2000.0;
    comprehensiveConfig.timestep = 0.001;
    comprehensiveConfig.neuronCount = 1000;
    comprehensiveConfig.connectionProbability = 0.15f;
    comprehensiveConfig.enablePlasticity = true;
    comprehensiveConfig.enableNeuromodulation = true;
    comprehensiveConfig.enableDevelopment = true;
    comprehensiveConfig.noveltyThreshold = 0.2f;
    comprehensiveConfig.curiosityThreshold = 0.6f;
    comprehensiveConfig.rewardScale = 1.0f;
    comprehensiveConfig.predictionErrorWeight = 1.0f;
    experimentConfigs_[ExperimentType::ComprehensiveLearning] = comprehensiveConfig;
}

ExperimentConfig ExperimentConfigManager::getConfig(ExperimentType type) const {
    auto it = experimentConfigs_.find(type);
    if (it != experimentConfigs_.end()) {
        return it->second;
    }
    return defaultConfig_;
}

void ExperimentConfigManager::setConfig(ExperimentType type, const ExperimentConfig& config) {
    if (!config.validate()) {
        throw std::invalid_argument("Invalid experiment configuration: validation failed");
    }
    experimentConfigs_[type] = config;
}

void ExperimentConfigManager::applyConfig(std::shared_ptr<Brain> brain, const ExperimentConfig& config) {
    if (!brain) {
        throw std::invalid_argument("Cannot apply configuration to null brain");
    }
    
    // Validate brain configuration can accommodate the experiment
    if (brain->getTotalNeuronCount() < config.neuronCount) {
        throw std::invalid_argument("Brain cannot accommodate experiment neuron count");
    }
    
    // Store configuration parameters in brain (implementation-specific)
    brain->setExperimentConfig(config);
    
    // Enable/disable plasticity systems
    brain->getSTDP()->enablePlasticity(config.enablePlasticity);
    brain->getHebbian()->enablePlasticity(config.enablePlasticity);
    brain->getStructuralPlasticity()->enablePlasticity(config.enablePlasticity);
    
    // Set neuromodulation parameters
    brain->getNeuromodulator()->setNoveltyThreshold(config.noveltyThreshold);
    brain->getNeuromodulator()->setCuriosityThreshold(config.curiosityThreshold);
    
    // Configure development system
    brain->getDevelopmentSystem()->setEnabled(config.enableDevelopment);
    
    // Set reward and prediction parameters
    brain->setRewardScale(config.rewardScale);
    brain->setPredictionErrorWeight(config.predictionErrorWeight);
}

ExperimentConfig ExperimentConfigManager::createExperimentConfig(ExperimentType type, const std::string& customName) {
    ExperimentConfig config = getConfig(type);
    
    if (!customName.empty()) {
        config.name = customName;
    }
    
    return config;
}

void ExperimentConfigManager::updateConfig(ExperimentType type, const std::map<std::string, float>& customParams) {
    ExperimentConfig config = getConfig(type);
    
    for (const auto& param : customParams) {
        config.setParameter(param.first, param.second);
    }
    
    setConfig(type, config);
}

std::vector<ExperimentType> ExperimentConfigManager::getAvailableExperimentTypes() const {
    std::vector<ExperimentType> types;
    for (const auto& pair : experimentConfigs_) {
        types.push_back(pair.first);
    }
    return types;
}

std::string ExperimentConfigManager::getExperimentName(ExperimentType type) const {
    auto it = experimentConfigs_.find(type);
    if (it != experimentConfigs_.end()) {
        return it->second.name;
    }
    return "UnknownExperiment";
}

} // namespace nlm
