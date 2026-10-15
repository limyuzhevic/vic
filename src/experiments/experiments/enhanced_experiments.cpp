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

// LearningCurveExperiment implementation
LearningCurveExperiment::LearningCurveExperiment(const std::string& name, std::shared_ptr<Config> config)
    : BaseExperiment(name, config), evaluationInterval_(100), taskComplexity_(1.0f) {
}

bool LearningCurveExperiment::initialize() {
    Logger::getInstance().log("Initializing LearningCurveExperiment: " + getName(), LogLevel::Info);

    if (!validateResources()) {
        setState(State::Failed);
        return false;
    }

    performanceHistory_.clear();
    testScores_.clear();
    setState(State::Scheduled);
    return true;
}

bool LearningCurveExperiment::run() {
    if (state_ != State::Scheduled && state_ != State::Paused) {
        Logger::getInstance().log("Experiment not in scheduled state", LogLevel::Error);
        return false;
    }

    setState(State::Running);
    updateProgress(0.0, "Starting learning curve experiment");

    // Record performance at each evaluation interval
    for (size_t step = 0; step < totalSteps_; ++step) {
        currentStep_ = step;

        // Simulate learning progress
        double basePerformance = 0.5 + (taskComplexity_ * 0.3);
        double learningFactor = std::exp(-step / (totalSteps_ / 3.0));
        double performance = basePerformance * (1.0 - learningFactor) + 0.2;

        performanceHistory_.emplace_back(step, performance);

        // Evaluate at intervals
        if (step % evaluationInterval_ == 0) {
            double testScore = performance * (0.8 + 0.2 * (step / static_cast<double>(totalSteps_)));
            testScores_.push_back(testScore);
        }

        updateProgress(static_cast<double>(step) / totalSteps_, "Learning step " + std::to_string(step));

        // Simulate time
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    setState(State::Completed);
    updateProgress(1.0, "Learning curve experiment completed");
    return true;
}

bool LearningCurveExperiment::checkpoint() {
    std::string checkpointFile = "checkpoint_learning_curve_" + getName() + ".json";
    if (saveState(checkpointFile)) {
        setState(State::Checkpointed);
        Logger::getInstance().log("Learning curve experiment checkpoint saved", LogLevel::Info);
        return true;
    }
    return false;
}

bool LearningCurveExperiment::resume() {
    if (state_ != State::Checkpointed) {
        Logger::getInstance().log("Cannot resume experiment: not checkpointed", LogLevel::Error);
        return false;
    }

    std::string checkpointFile = "checkpoint_learning_curve_" + getName() + ".json";
    if (loadState(checkpointFile)) {
        setState(State::Scheduled);
        Logger::getInstance().log("Learning curve experiment resumed", LogLevel::Info);
        return true;
    }
    return false;
}

bool LearningCurveExperiment::finalize() {
    setState(State::Completed);

    // Calculate final learning metrics
    if (!performanceHistory_.empty()) {
        double finalPerformance = performanceHistory_.back().second;
        recordMetric("final_performance", finalPerformance);

        // Calculate learning rate
        double initialPerf = performanceHistory_.front().second;
        double learningRate = (finalPerformance - initialPerf) / totalSteps_;
        recordMetric("learning_rate", learningRate);

        // Calculate convergence score
        double convergence = 0.0;
        size_t window = std::min<size_t>(10, performanceHistory_.size());
        for (size_t i = performanceHistory_.size() - window; i < performanceHistory_.size(); ++i) {
            convergence += std::abs(performanceHistory_[i].second - performanceHistory_.back().second);
        }
        convergence /= window;
        recordMetric("convergence_score", 1.0 - convergence);
    }

    Logger::getInstance().log("Learning curve experiment finalized", LogLevel::Info);
    return true;
}

double LearningCurveExperiment::calculatePerformanceScore() const {
    double score = 0.0;

    auto metrics = getMetrics();

    // Learning rate contribution
    if (hasMetric("learning_rate")) {
        score += getMetric("learning_rate") * 0.5;
    }

    // Performance score contribution
    if (hasMetric("final_performance")) {
        score += getMetric("final_performance") * 0.5;
    }

    // Convergence score contribution
    if (hasMetric("convergence_score")) {
        score += getMetric("convergence_score") * 0.3;
    }

    return std::min(1.0, score);
}

std::string LearningCurveExperiment::getResultsSummary() const {
    std::stringstream ss;
    ss << "Learning Curve Experiment Summary:" << std::endl;
    ss << "  Experiment: " << getName() << std::endl;
    ss << "  Type: " << getExperimentType() << std::endl;
    ss << "  Evaluation Interval: " << evaluationInterval_ << std::endl;
    ss << "  Task Complexity: " << taskComplexity_ << std::endl;
    ss << "  Total Steps: " << totalSteps_ << std::endl;

    if (!testScores_.empty()) {
        double avgTestScore = std::accumulate(testScores_.begin(), testScores_.end(), 0.0) / testScores_.size();
        ss << "  Average Test Score: " << avgTestScore << std::endl;
    }

    auto metrics = getMetrics();
    for (const auto& metric : metrics) {
        ss << "  " << metric.first << ": " << metric.second << std::endl;
    }

    ss << "  Final Performance Score: " << calculatePerformanceScore() << std::endl;

    return ss.str();
}

std::vector<std::string> LearningCurveExperiment::getRequiredParameters() const {
    return {"evaluation_interval", "task_complexity", "max_steps"};
}

bool LearningCurveExperiment::validateParameters() const {
    if (parameters_.find("evaluation_interval") == parameters_.end()) {
        return false;
    }

    if (parameters_.find("task_complexity") == parameters_.end()) {
        return false;
    }

    if (parameters_.find("max_steps") == parameters_.end()) {
        return false;
    }

    try {
        size_t interval = std::stoul(parameters_.at("evaluation_interval"));
        if (interval == 0) {
            return false;
        }

        float complexity = std::stof(parameters_.at("task_complexity"));
        if (complexity < 0.1f || complexity > 10.0f) {
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

// Additional implementations for remaining experiments

// NetworkConnectivityExperiment implementation
NetworkConnectivityExperiment::NetworkConnectivityExperiment(const std::string& name, std::shared_ptr<Config> config)
    : BaseExperiment(name, config), injectCurrent_(50.0f), propagationDelay_(10.0f) {
}

bool NetworkConnectivityExperiment::initialize() {
    Logger::getInstance().log("Initializing NetworkConnectivityExperiment: " + getName(), LogLevel::Info);

    spikeCounts_.clear();
    propagationDelays_.clear();
    initialNeuronCount_ = 0; // Would be obtained from actual brain

    setState(State::Scheduled);
    return true;
}

bool NetworkConnectivityExperiment::run() {
    if (state_ != State::Scheduled && state_ != State::Paused) {
        Logger::getInstance().log("Experiment not in scheduled state", LogLevel::Error);
        return false;
    }

    setState(State::Running);
    updateProgress(0.0, "Starting network connectivity experiment");

    for (size_t step = 0; step < totalSteps_; ++step) {
        currentStep_ = step;

        // Simulate connectivity propagation
        size_t spikeCount = 0;
        float totalDelay = 0.0f;

        // Simulate 20 neurons firing
        for (size_t i = 0; i < 20 && i < initialNeuronCount_; ++i) {
            // Simulate spike
            spikeCount++;
            totalDelay += propagationDelay_ * (1.0f + i * 0.1f);
        }

        spikeCounts_.push_back(spikeCount);
        propagationDelays_.push_back(totalDelay);

        updateProgress(static_cast<double>(step) / totalSteps_, "Connectivity step " + std::to_string(step));

        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }

    setState(State::Completed);
    updateProgress(1.0, "Network connectivity experiment completed");
    return true;
}

bool NetworkConnectivityExperiment::checkpoint() {
    std::string checkpointFile = "checkpoint_network_connectivity_" + getName() + ".json";
    if (saveState(checkpointFile)) {
        setState(State::Checkpointed);
        Logger::getInstance().log("Network connectivity experiment checkpoint saved", LogLevel::Info);
        return true;
    }
    return false;
}

bool NetworkConnectivityExperiment::resume() {
    if (state_ != State::Checkpointed) {
        Logger::getInstance().log("Cannot resume experiment: not checkpointed", LogLevel::Error);
        return false;
    }

    std::string checkpointFile = "checkpoint_network_connectivity_" + getName() + ".json";
    if (loadState(checkpointFile)) {
        setState(State::Scheduled);
        Logger::getInstance().log("Network connectivity experiment resumed", LogLevel::Info);
        return true;
    }
    return false;
}

bool NetworkConnectivityExperiment::finalize() {
    setState(State::Completed);

    // Calculate connectivity metrics
    if (!spikeCounts_.empty()) {
        double totalSpikes = std::accumulate(spikeCounts_.begin(), spikeCounts_.end(), 0.0);
        recordMetric("total_spikes", totalSpikes);

        double avgSpikeCount = totalSpikes / spikeCounts_.size();
        recordMetric("avg_spike_count", avgSpikeCount);

        double totalDelay = std::accumulate(propagationDelays_.begin(), propagationDelays_.end(), 0.0);
        double avgPropagationDelay = totalDelay / propagationDelays_.size();
        recordMetric("avg_propagation_delay", avgPropagationDelay);

        double connectivityScore = (totalSpikes / (initialNeuronCount_ * totalSteps_)) * (1.0 / (1.0 + avgPropagationDelay / 100.0));
        recordMetric("connectivity_score", connectivityScore);
    }

    Logger::getInstance().log("Network connectivity experiment finalized", LogLevel::Info);
    return true;
}

double NetworkConnectivityExperiment::calculatePerformanceScore() const {
    double score = 0.0;

    auto metrics = getMetrics();

    if (hasMetric("avg_spike_count")) {
        score += getMetric("avg_spike_count") * 0.3;
    }

    if (hasMetric("connectivity_score")) {
        score += getMetric("connectivity_score") * 0.7;
    }

    return std::min(1.0, score);
}

std::string NetworkConnectivityExperiment::getResultsSummary() const {
    std::stringstream ss;
    ss << "Network Connectivity Experiment Summary:" << std::endl;
    ss << "  Experiment: " << getName() << std::endl;
    ss << "  Type: " << getExperimentType() << std::endl;
    ss << "  Injected Current: " << injectCurrent_ << std::endl;
    ss << "  Propagation Delay: " << propagationDelay_ << std::endl;

    auto metrics = getMetrics();
    for (const auto& metric : metrics) {
        ss << "  " << metric.first << ": " << metric.second << std::endl;
    }

    ss << "  Final Performance Score: " << calculatePerformanceScore() << std::endl;

    return ss.str();
}

std::vector<std::string> NetworkConnectivityExperiment::getRequiredParameters() const {
    return {"injected_current", "propagation_delay", "max_steps"};
}

bool NetworkConnectivityExperiment::validateParameters() const {
    if (parameters_.find("injected_current") == parameters_.end()) {
        return false;
    }

    if (parameters_.find("propagation_delay") == parameters_.end()) {
        return false;
    }

    if (parameters_.find("max_steps") == parameters_.end()) {
        return false;
    }

    try {
        float current = std::stof(parameters_.at("injected_current"));
        if (current <= 0.0f) {
            return false;
        }

        float delay = std::stof(parameters_.at("propagation_delay"));
        if (delay <= 0.0f) {
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

// DevelopmentalExperiment implementation
DevelopmentalExperiment::DevelopmentalExperiment(const std::string& name, std::shared_ptr<Config> config)
    : BaseExperiment(name, config), developmentalAge_(0.0f), maturationRate_(0.01f), pruningEnabled_(true) {
}

bool DevelopmentalExperiment::initialize() {
    Logger::getInstance().log("Initializing DevelopmentalExperiment: " + getName(), LogLevel::Info);

    developmentalStages_.clear();
    neuronCounts_.clear();
    synapseCounts_.clear();

    setState(State::Scheduled);
    return true;
}

bool DevelopmentalExperiment::run() {
    if (state_ != State::Scheduled && state_ != State::Paused) {
        Logger::getInstance().log("Experiment not in scheduled state", LogLevel::Error);
        return false;
    }

    setState(State::Running);
    updateProgress(0.0, "Starting developmental experiment");

    for (size_t step = 0; step < totalSteps_; ++step) {
        currentStep_ = step;

        // Simulate developmental processes
        float currentAge = developmentalAge_ + step * 0.001f;

        // Maturation phase
        float maturation = maturationRate_ * step;
        size_t neuronCount = static_cast<size_t>(1000 * (1.0 + maturation * 2.0)); // Exponential growth
        neuronCounts_.push_back(neuronCount);

        size_t synapseCount = neuronCount * static_cast<size_t>(std::min(10.0f, maturation * 20.0f));
        synapseCounts_.push_back(synapseCount);

        std::string stage;
        if (currentAge < 0.2f) {
            stage = "Early Development";
        } else if (currentAge < 0.6f) {
            stage = "Rapid Growth";
        } else if (currentAge < 0.9f) {
            stage = "Maturation";
        } else {
            stage = "Stabilization";

            if (pruningEnabled_) {
                // Apply pruning (reduce synapses)
                synapseCount = static_cast<size_t>(synapseCount * 0.7f);
                synapseCounts_.back() = synapseCount;
            }
        }

        developmentalStages_.emplace_back(currentAge, stage);
        updateProgress(static_cast<double>(step) / totalSteps_, "Development step " + std::to_string(step));

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    setState(State::Completed);
    updateProgress(1.0, "Developmental experiment completed");
    return true;
}

bool DevelopmentalExperiment::checkpoint() {
    std::string checkpointFile = "checkpoint_developmental_" + getName() + ".json";
    if (saveState(checkpointFile)) {
        setState(State::Checkpointed);
        Logger::getInstance().log("Developmental experiment checkpoint saved", LogLevel::Info);
        return true;
    }
    return false;
}

bool DevelopmentalExperiment::resume() {
    if (state_ != State::Checkpointed) {
        Logger::getInstance().log("Cannot resume experiment: not checkpointed", LogLevel::Error);
        return false;
    }

    std::string checkpointFile = "checkpoint_developmental_" + getName() + ".json";
    if (loadState(checkpointFile)) {
        setState(State::Scheduled);
        Logger::getInstance().log("Developmental experiment resumed", LogLevel::Info);
        return true;
    }
    return false;
}

bool DevelopmentalExperiment::finalize() {
    setState(State::Completed);

    // Calculate developmental metrics
    if (!neuronCounts_.empty()) {
        double finalNeuronCount = neuronCounts_.back();
        recordMetric("final_neuron_count", static_cast<double>(finalNeuronCount));

        double initialNeuronCount = neuronCounts_.front();
        double growthRatio = finalNeuronCount / initialNeuronCount;
        recordMetric("neuron_growth_ratio", growthRatio);

        double finalSynapseCount = synapseCounts_.back();
        recordMetric("final_synapse_count", static_cast<double>(finalSynapseCount));

        double synapseDensity = finalSynapseCount / finalNeuronCount;
        recordMetric("synapse_density", synapseDensity);

        // Developmental score
        double developmentalScore = (growthRatio * 0.7f) + (1.0f / (1.0f + synapseDensity)) * 0.3f;
        recordMetric("developmental_score", developmentalScore);
    }

    Logger::getInstance().log("Developmental experiment finalized", LogLevel::Info);
    return true;
}

double DevelopmentalExperiment::calculatePerformanceScore() const {
    double score = 0.0;

    auto metrics = getMetrics();

    if (hasMetric("neuron_growth_ratio")) {
        score += getMetric("neuron_growth_ratio") * 0.5;
    }

    if (hasMetric("developmental_score")) {
        score += getMetric("developmental_score") * 0.5;
    }

    return std::min(1.0, score);
}

std::string DevelopmentalExperiment::getResultsSummary() const {
    std::stringstream ss;
    ss << "Developmental Experiment Summary:" << std::endl;
    ss << "  Experiment: " << getName() << std::endl;
    ss << "  Type: " << getExperimentType() << std::endl;
    ss << "  Developmental Age: " << developmentalAge_ << std::endl;
    ss << "  Maturation Rate: " << maturationRate_ << std::endl;
    ss << "  Pruning Enabled: " << (pruningEnabled_ ? "Yes" : "No") << std::endl;

    auto metrics = getMetrics();
    for (const auto& metric : metrics) {
        ss << "  " << metric.first << ": " << metric.second << std::endl;
    }

    ss << "  Final Performance Score: " << calculatePerformanceScore() << std::endl;

    return ss.str();
}

std::vector<std::string> DevelopmentalExperiment::getRequiredParameters() const {
    return {"developmental_age", "maturation_rate", "max_steps"};
}

bool DevelopmentalExperiment::validateParameters() const {
    if (parameters_.find("developmental_age") == parameters_.end()) {
        return false;
    }

    if (parameters_.find("maturation_rate") == parameters_.end()) {
        return false;
    }

    if (parameters_.find("max_steps") == parameters_.end()) {
        return false;
    }

    try {
        float age = std::stof(parameters_.at("developmental_age"));
        if (age < 0.0f || age > 5.0f) {
            return false;
        }

        float rate = std::stof(parameters_.at("maturation_rate"));
        if (rate <= 0.0f || rate > 1.0f) {
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

// NeuromodulationExperiment implementation
NeuromodulationExperiment::NeuromodulationExperiment(const std::string& name, std::shared_ptr<Config> config)
    : BaseExperiment(name, config) {
}

bool NeuromodulationExperiment::initialize() {
    Logger::getInstance().log("Initializing NeuromodulationExperiment: " + getName(), LogLevel::Info);

    rewardValues_.clear();
    noveltyValues_.clear();
    curiosityValues_.clear();

    setState(State::Scheduled);
    return true;
}

bool NeuromodulationExperiment::run() {
    if (state_ != State::Scheduled && state_ != State::Paused) {
        Logger::getInstance().log("Experiment not in scheduled state", LogLevel::Error);
        return false;
    }

    setState(State::Running);
    updateProgress(0.0, "Starting neuromodulation experiment");

    for (size_t step = 0; step < totalSteps_; ++step) {
        currentStep_ = step;

        // Simulate neuromodulation dynamics
        double reward = 0.5 + (rand() % 100) / 100.0 * 0.5; // Random reward 0.5-1.0
        double novelty = (rand() % 100) / 100.0 * 0.8; // Random novelty 0.0-0.8
        double curiosity = (rand() % 100) / 100.0 * 0.6; // Random curiosity 0.0-0.6

        rewardValues_.push_back(reward);
        noveltyValues_.push_back(novelty);
        curiosityValues_.push_back(curiosity);

        // Simulate neuromodulator effects
        double totalNeuromodulation = reward * 0.5 + novelty * 0.3 + curiosity * 0.2;
        recordMetric("total_neuromodulation", totalNeuromodulation);

        updateProgress(static_cast<double>(step) / totalSteps_, "Neuromodulation step " + std::to_string(step));

        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }

    setState(State::Completed);
    updateProgress(1.0, "Neuromodulation experiment completed");
    return true;
}

bool NeuromodulationExperiment::checkpoint() {
    std::string checkpointFile = "checkpoint_neuromodulation_" + getName() + ".json";
    if (saveState(checkpointFile)) {
        setState(State::Checkpointed);
        Logger::getInstance().log("Neuromodulation experiment checkpoint saved", LogLevel::Info);
        return true;
    }
    return false;
}

bool NeuromodulationExperiment::resume() {
    if (state_ != State::Checkpointed) {
        Logger::getInstance().log("Cannot resume experiment: not checkpointed", LogLevel::Error);
        return false;
    }

    std::string checkpointFile = "checkpoint_neuromodulation_" + getName() + ".json";
    if (loadState(checkpointFile)) {
        setState(State::Scheduled);
        Logger::getInstance().log("Neuromodulation experiment resumed", LogLevel::Info);
        return true;
    }
    return false;
}

bool NeuromodulationExperiment::finalize() {
    setState(State::Completed);

    // Calculate neuromodulation metrics
    if (!rewardValues_.empty()) {
        double avgReward = std::accumulate(rewardValues_.begin(), rewardValues_.end(), 0.0) / rewardValues_.size();
        double avgNovelty = std::accumulate(noveltyValues_.begin(), noveltyValues_.end(), 0.0) / noveltyValues_.size();
        double avgCuriosity = std::accumulate(curiosityValues_.begin(), curiosityValues_.end(), 0.0) / curiosityValues_.size();

        recordMetric("average_reward", avgReward);
        recordMetric("average_novelty", avgNovelty);
        recordMetric("average_curiosity", avgCuriosity);

        double neuromodulationBalance = avgReward * 0.5 + avgNovelty * 0.3 + avgCuriosity * 0.2;
        recordMetric("neuromodulation_balance", neuromodulationBalance);
    }

    Logger::getInstance().log("Neuromodulation experiment finalized", LogLevel::Info);
    return true;
}

double NeuromodulationExperiment::calculatePerformanceScore() const {
    double score = 0.0;

    auto metrics = getMetrics();

    if (hasMetric("average_reward")) {
        score += getMetric("average_reward") * 0.4;
    }

    if (hasMetric("neuromodulation_balance")) {
        score += getMetric("neuromodulation_balance") * 0.6;
    }

    return std::min(1.0, score);
}

std::string NeuromodulationExperiment::getResultsSummary() const {
    std::stringstream ss;
    ss << "Neuromodulation Experiment Summary:" << std::endl;
    ss << "  Experiment: " << getName() << std::endl;
    ss << "  Type: " << getExperimentType() << std::endl;

    auto metrics = getMetrics();
    for (const auto& metric : metrics) {
        ss << "  " << metric.first << ": " << metric.second << std::endl;
    }

    ss << "  Final Performance Score: " << calculatePerformanceScore() << std::endl;

    return ss.str();
}

std::vector<std::string> NeuromodulationExperiment::getRequiredParameters() const {
    return {"max_steps"};
}

bool NeuromodulationExperiment::validateParameters() const {
    if (parameters_.find("max_steps") == parameters_.end()) {
        return false;
    }

    try {
        size_t steps = std::stoul(parameters_.at("max_steps"));
        if (steps == 0) {
            return false;
        }
    } catch (...) {
        return false;
    }

    return true;
}

} // namespace nlm