#include "experiment_factory.hpp"
#include "../core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// ExperimentFactory implementation
std::shared_ptr<BaseExperiment> ExperimentFactory::createExperiment(const std::string& type, const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = experimentCreators_.find(type);
    if (it == experimentCreators_.end()) {
        Logger::getInstance().log("Unknown experiment type: " + type, LogLevel::Error);
        return nullptr;
    }
    
    return it->second(name, config_);
}

std::shared_ptr<BaseExperiment> ExperimentFactory::createExperimentFromTemplate(
    const std::string& templateName,
    const std::map<std::string, std::string>& parameters) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Convert parameters to JSON string for experiment initialization
    json paramsJson = parameters;
    
    // Create experiment using default name if not specified
    std::string experimentName = "template_" + templateName + "_" + std::to_string(nextExperimentId_++);
    
    auto experiment = createExperiment(templateName, experimentName);
    if (!experiment) {
        return nullptr;
    }
    
    // Set parameters
    for (const auto& param : parameters) {
        experiment->setParameter(param.first, param.second);
    }
    
    return experiment;
}

void ExperimentFactory::registerDefaultExperiments() {
    // Register plasticity experiment
    experimentCreators_["plasticity"] = [this](const std::string& name, std::shared_ptr<Config> config) {
        return createPlasticityExperiment(name);
    };

    // Register learning curve experiment
    experimentCreators_["learning_curve"] = [this](const std::string& name, std::shared_ptr<Config> config) {
        return createLearningCurveExperiment(name);
    };

    // Register network connectivity experiment
    experimentCreators_["network_connectivity"] = [this](const std::string& name, std::shared_ptr<Config> config) {
        return createNetworkConnectivityExperiment(name);
    };

    // Register developmental experiment
    experimentCreators_["developmental"] = [this](const std::string& name, std::shared_ptr<Config> config) {
        return createDevelopmentalExperiment(name);
    };

    // Register neuromodulation experiment
    experimentCreators_["neuromodulation"] = [this](const std::string& name, std::shared_ptr<Config> config) {
        return createNeuromodulationExperiment(name);
    };
}

std::shared_ptr<BaseExperiment> ExperimentFactory::createPlasticityExperiment(const std::string& name) {
    auto exp = std::make_shared<PlasticityExperiment>(name, config_);
    
    // Set default parameters for plasticity experiment
    exp->setParameter("learning_rate", "0.01");
    exp->setParameter("max_steps", "1000");
    exp->setParameter("stdp_enabled", "true");
    exp->setParameter("hebbian_enabled", "true");
    
    return exp;
}

std::shared_ptr<BaseExperiment> ExperimentFactory::createLearningCurveExperiment(const std::string& name) {
    auto exp = std::make_shared<LearningCurveExperiment>(name, config_);
    
    // Set default parameters for learning curve experiment
    exp->setParameter("evaluation_interval", "100");
    exp->setParameter("task_complexity", "1.0");
    exp->setParameter("max_steps", "2000");
    
    return exp;
}

std::shared_ptr<BaseExperiment> ExperimentFactory::createNetworkConnectivityExperiment(const std::string& name) {
    auto exp = std::make_shared<NetworkConnectivityExperiment>(name, config_);
    
    // Set default parameters for network connectivity experiment
    exp->setParameter("injected_current", "50.0");
    exp->setParameter("propagation_delay", "10.0");
    exp->setParameter("max_steps", "500");
    
    return exp;
}

std::shared_ptr<BaseExperiment> ExperimentFactory::createDevelopmentalExperiment(const std::string& name) {
    auto exp = std::make_shared<DevelopmentalExperiment>(name, config_);
    
    // Set default parameters for developmental experiment
    exp->setParameter("developmental_age", "0.5");
    exp->setParameter("maturation_rate", "0.01");
    exp->setParameter("max_steps", "1000");
    exp->setParameter("pruning_enabled", "true");
    
    return exp;
}

std::shared_ptr<BaseExperiment> ExperimentFactory::createNeuromodulationExperiment(const std::string& name) {
    auto exp = std::make_shared<NeuromodulationExperiment>(name, config_);
    
    // Set default parameters for neuromodulation experiment
    exp->setParameter("max_steps", "800");
    
    return exp;
}

} // namespace nlm