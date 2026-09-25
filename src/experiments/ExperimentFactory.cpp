#include "ExperimentFactory.hpp"
#include "Experiment.hpp"
#include <spdlog/spdlog.h>

namespace nlm {

std::unique_ptr<Experiment> ExperimentFactory::createExperiment(const ObjectParams& params) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!initialized_) {
        spdlog::warn("ExperimentFactory not initialized, creating basic experiment");
        return std::make_unique<Experiment>();
    }
    
    // Get configuration from params or use defaults
    std::string type = params.getParam<std::string>("type", "basic");
    int phase = params.getParam<int>("phase", 0);
    std::string experimentName = params.getParam<std::string>("name", "");
    bool ablation = params.getParam<bool>("ablation", false);
    bool benchmark = params.getParam<bool>("benchmark", false);
    
    // Handle different experiment creation scenarios based on type
    if (type == "phase" && phase > 0) {
        return createPhaseExperiment(phase);
    } else if (ablation) {
        return createAblationExperiment();
    } else if (benchmark) {
        return createScalingBenchmark();
    } else if (!experimentName.empty()) {
        return createCustomExperiment(experimentName);
    } else {
        // Default basic experiment
        return createBasicExperiment();
    }
}

std::unique_ptr<Experiment> ExperimentFactory::createBasicExperiment() {
    spdlog::debug("Creating basic experiment");
    return std::make_unique<Experiment>();
}

std::unique_ptr<Experiment> ExperimentFactory::createPhaseExperiment(int phase) {
    spdlog::debug("Creating phase experiment with phase: {}", phase);
    auto experiment = std::make_unique<Experiment>();
    // In a real implementation, would add phase-specific properties to the experiment
    return experiment;
}

std::unique_ptr<Experiment> ExperimentFactory::createAblationExperiment() {
    spdlog::debug("Creating ablation experiment");
    auto experiment = std::make_unique<Experiment>();
    // In a real implementation, would add ablation-specific properties to the experiment
    return experiment;
}

std::unique_ptr<Experiment> ExperimentFactory::createScalingBenchmark() {
    spdlog::debug("Creating scaling benchmark experiment");
    auto experiment = std::make_unique<Experiment>();
    // In a real implementation, would add benchmark-specific properties to the experiment
    return experiment;
}

std::unique_ptr<Experiment> ExperimentFactory::createCustomExperiment(const std::string& name) {
    spdlog::debug("Creating custom experiment with name: {}", name);
    auto experiment = std::make_unique<Experiment>();
    // In a real implementation, would add custom properties to the experiment
    return experiment;
}

std::unique_ptr<Experiment> ExperimentFactory::acquireExperimentFromPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!experimentPool_.empty()) {
        auto experiment = std::move(experimentPool_.back());
        experimentPool_.pop_back();
        spdlog::debug("Acquired experiment from pool");
        return experiment;
    }
    return nullptr;
}

void ExperimentFactory::releaseExperimentToPool(std::unique_ptr<Experiment> experiment) {
    if (experiment) {
        std::lock_guard<std::mutex> lock(mutex_);
        experimentPool_.push_back(std::move(experiment));
        spdlog::debug("Released experiment to pool");
    }
}

} // namespace nlm
