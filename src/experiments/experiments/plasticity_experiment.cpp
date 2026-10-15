#include "enhanced_experiments.hpp"
#include "../core/Logger/Logger.hpp"
#include <random>
#include <algorithm>
#include <cmath>

namespace nlm {

// PlasticityExperiment implementation
PlasticityExperiment::PlasticityExperiment(const std::string& name, std::shared_ptr<Config> config)
    : BaseExperiment(name, config), learningRate_(0.01f), stdpEnabled_(true), hebbianEnabled_(true) {
}

bool PlasticityExperiment::initialize() {
    Logger::getInstance().log("Initializing PlasticityExperiment: " + getName(), LogLevel::Info);

    // Validate resources
    if (!validateResources()) {
        setState(State::Failed);
        return false;
    }

    // Initialize plasticity system
    initialSynapseCount_ = 0; // Would be obtained from actual brain
    plasticityMetrics_.clear();

    setState(State::Scheduled);
    return true;
}

bool PlasticityExperiment::run() {
    if (state_ != State::Scheduled && state_ != State::Paused) {
        Logger::getInstance().log("Experiment not in scheduled state", LogLevel::Error);
        return false;
    }

    setState(State::Running);
    updateProgress(0.0, "Starting plasticity experiment");

    // Simulate plasticity dynamics
    for (size_t step = 0; step < totalSteps_; ++step) {
        currentStep_ = step;
        double progress = static_cast<double>(step) / totalSteps_;
        updateProgress(progress, "Plasticity step " + std::to_string(step));

        // Apply plasticity rules
        if (stdpEnabled_) {
            // STDP simulation
            float stdpChange = 0.001f * (static_cast<float>(step) / totalSteps_);
            // Would update actual synapse weights here
            plasticityMetrics_.emplace_back("stdp_weight_change", stdpChange);
        }

        if (hebbianEnabled_) {
            // Hebbian learning simulation
            float hebbianChange = learningRate_ * 0.1f;
            plasticityMetrics_.emplace_back("hebbian_weight_change", hebbianChange);
        }

        // Simulate time
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    setState(State::Completed);
    updateProgress(1.0, "Plasticity experiment completed");
    return true;
}

bool PlasticityExperiment::checkpoint() {
    std::string checkpointFile = "checkpoint_plasticity_" + getName() + ".json";
    if (saveState(checkpointFile)) {
        setState(State::Checkpointed);
        Logger::getInstance().log("Plasticity experiment checkpoint saved", LogLevel::Info);
        return true;
    }
    return false;
}

bool PlasticityExperiment::resume() {
    if (state_ != State::Checkpointed) {
        Logger::getInstance().log("Cannot resume experiment: not checkpointed", LogLevel::Error);
        return false;
    }

    std::string checkpointFile = "checkpoint_plasticity_" + getName() + ".json";
    if (loadState(checkpointFile)) {
        setState(State::Scheduled);
        Logger::getInstance().log("Plasticity experiment resumed", LogLevel::Info);
        return true;
    }
    return false;
}

bool PlasticityExperiment::finalize() {
    setState(State::Completed);

    // Record final plasticity metrics
    double avgStdpChange = 0.0, avgHebbianChange = 0.0;
    size_t stdpCount = 0, hebbianCount = 0;

    for (const auto& metric : plasticityMetrics_) {
        if (metric.first == "stdp_weight_change") {
            avgStdpChange += metric.second;
            stdpCount++;
        } else if (metric.first == "hebbian_weight_change") {
            avgHebbianChange += metric.second;
            hebbianCount++;
        }
    }

    if (stdpCount > 0) avgStdpChange /= stdpCount;
    if (hebbianCount > 0) avgHebbianChange /= hebbianCount;

    recordMetric("avg_stdp_change", avgStdpChange);
    recordMetric("avg_hebbian_change", avgHebbianChange);
    recordMetric("final_plasticity_score", (avgStdpChange + avgHebbianChange) / 2.0);

    Logger::getInstance().log("Plasticity experiment finalized", LogLevel::Info);
    return true;
}

double PlasticityExperiment::calculatePerformanceScore() const {
    double score = 0.0;

    // Weight different plasticity measures
    auto metrics = getMetrics();

    // STDP contribution
    if (hasMetric("avg_stdp_change")) {
        score += getMetric("avg_stdp_change") * 0.4;
    }

    // Hebbian contribution
    if (hasMetric("avg_hebbian_change")) {
        score += getMetric("avg_hebbian_change") * 0.4;
    }

    // Final plasticity score
    if (hasMetric("final_plasticity_score")) {
        score += getMetric("final_plasticity_score") * 0.2;
    }

    return std::min(1.0, score);
}

std::string PlasticityExperiment::getResultsSummary() const {
    std::stringstream ss;
    ss << "Plasticity Experiment Summary:" << std::endl;
    ss << "  Experiment: " << getName() << std::endl;
    ss << "  Type: " << getExperimentType() << std::endl;
    ss << "  STDP Enabled: " << (stdpEnabled_ ? "Yes" : "No") << std::endl;
    ss << "  Hebbian Enabled: " << (hebbianEnabled_ ? "Yes" : "No") << std::endl;
    ss << "  Learning Rate: " << learningRate_ << std::endl;

    auto metrics = getMetrics();
    for (const auto& metric : metrics) {
        ss << "  " << metric.first << ": " << metric.second << std::endl;
    }

    ss << "  Final Performance Score: " << calculatePerformanceScore() << std::endl;

    return ss.str();
}

std::vector<std::string> PlasticityExperiment::getRequiredParameters() const {
    return {"learning_rate", "max_steps"};
}

bool PlasticityExperiment::validateParameters() const {
    if (parameters_.find("learning_rate") == parameters_.end()) {
        return false;
    }

    if (parameters_.find("max_steps") == parameters_.end()) {
        return false;
    }

    try {
        float rate = std::stof(parameters_.at("learning_rate"));
        if (rate < 0.0f || rate > 1.0f) {
            return false;
        }

        size_t steps = std::stoul(parameters_.at("max_steps"));
        if (steps == 0) {
            return false;
        }
    } catch (...) {
        return false;
    }

    return true;
}

// Additional experiment implementations would follow similar patterns...
// For brevity, I'll continue with the other core implementations

} // namespace nlm