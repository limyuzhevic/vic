#pragma once

#include "base_experiment.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Plasticity experiment - studies synaptic changes and learning
class PlasticityExperiment : public BaseExperiment {
public:
    PlasticityExperiment(const std::string& name, std::shared_ptr<Config> config = nullptr);
    ~PlasticityExperiment() override = default;

    // Base class overrides
    bool initialize() override;
    bool run() override;
    bool checkpoint() override;
    bool resume() override;
    bool finalize() override;

    // Experiment-specific functionality
    void setPlasticityRule(std::shared_ptr<PlasticityRule> rule) { plasticityRule_ = rule; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setStdpEnabled(bool enabled) { stdpEnabled_ = enabled; }
    void setHebbianEnabled(bool enabled) { hebbianEnabled_ = enabled; }

    double calculatePerformanceScore() const override;
    std::string getResultsSummary() const override;

    std::string getExperimentType() const override { return "plasticity"; }
    std::string getDescription() const override { return "Studies synaptic plasticity and learning mechanisms"; }
    std::vector<std::string> getRequiredParameters() const override;

    bool validateParameters() const override;

private:
    std::shared_ptr<PlasticityRule> plasticityRule_;
    float learningRate_;
    bool stdpEnabled_;
    bool hebbianEnabled_;
    std::vector<std::pair<std::string, double>> plasticityMetrics_;
    size_t initialSynapseCount_;
};

// Learning curve experiment - measures performance across training steps
class LearningCurveExperiment : public BaseExperiment {
public:
    LearningCurveExperiment(const std::string& name, std::shared_ptr<Config> config = nullptr);
    ~LearningCurveExperiment() override = default;

    // Base class overrides
    bool initialize() override;
    bool run() override;
    bool checkpoint() override;
    bool resume() override;
    bool finalize() override;

    // Experiment-specific functionality
    void setTrainingSchedule(const std::vector<size_t>& schedule) { trainingSchedule_ = schedule; }
    void setEvaluationInterval(size_t interval) { evaluationInterval_ = interval; }
    void setTaskComplexity(float complexity) { taskComplexity_ = complexity; }

    double calculatePerformanceScore() const override;
    std::string getResultsSummary() const override;

    std::string getExperimentType() const override { return "learning_curve"; }
    std::string getDescription() const override { return "Measures learning performance across training curves"; }
    std::vector<std::string> getRequiredParameters() const override;

    bool validateParameters() const override;

private:
    std::vector<size_t> trainingSchedule_;
    size_t evaluationInterval_;
    float taskComplexity_;
    std::vector<std::pair<size_t, double>> performanceHistory_;
    std::vector<double> testScores_;
};

// Network connectivity experiment - tests information propagation
class NetworkConnectivityExperiment : public BaseExperiment {
public:
    NetworkConnectivityExperiment(const std::string& name, std::shared_ptr<Config> config = nullptr);
    ~NetworkConnectivityExperiment() override = default;

    // Base class overrides
    bool initialize() override;
    bool run() override;
    bool checkpoint() override;
    bool resume() override;
    bool finalize() override;

    // Experiment-specific functionality
    void setInputPattern(const std::vector<float>& pattern) { inputPattern_ = pattern; }
    void setInjectCurrent(float current) { injectCurrent_ = current; }
    void setPropagationDelay(float delay) { propagationDelay_ = delay; }

    double calculatePerformanceScore() const override;
    std::string getResultsSummary() const override;

    std::string getExperimentType() const override { return "network_connectivity"; }
    std::string getDescription() const override { return "Tests information propagation through neural networks"; }
    std::vector<std::string> getRequiredParameters() const override;

    bool validateParameters() const override;

private:
    std::vector<float> inputPattern_;
    float injectCurrent_;
    float propagationDelay_;
    std::vector<size_t> spikeCounts_;
    std::vector<double> propagationDelays_;
    size_t initialNeuronCount_;
};

// Developmental experiment - studies brain development over time
class DevelopmentalExperiment : public BaseExperiment {
public:
    DevelopmentalExperiment(const std::string& name, std::shared_ptr<Config> config = nullptr);
    ~DevelopmentalExperiment() override = default;

    // Base class overrides
    bool initialize() override;
    bool run() override;
    bool checkpoint() override;
    bool resume() override;
    bool finalize() override;

    // Experiment-specific functionality
    void setDevelopmentalStage(float age) { developmentalAge_ = age; }
    void setMaturationRate(float rate) { maturationRate_ = rate; }
    void setPruningEnabled(bool enabled) { pruningEnabled_ = enabled; }

    double calculatePerformanceScore() const override;
    std::string getResultsSummary() const override;

    std::string getExperimentType() const override { return "developmental"; }
    std::string getDescription() const override { return "Studies brain development and maturation processes"; }
    std::vector<std::string> getRequiredParameters() const override;

    bool validateParameters() const override;

private:
    float developmentalAge_;
    float maturationRate_;
    bool pruningEnabled_;
    std::vector<std::pair<float, std::string>> developmentalStages_;
    std::vector<size_t> neuronCounts_;
    std::vector<size_t> synapseCounts_;
};

// Neuromodulation experiment - studies reward and motivation systems
class NeuromodulationExperiment : public BaseExperiment {
public:
    NeuromodulationExperiment(const std::string& name, std::shared_ptr<Config> config = nullptr);
    ~NeuromodulationExperiment() override = default;

    // Base class overrides
    bool initialize() override;
    bool run() override;
    bool checkpoint() override;
    bool resume() override;
    bool finalize() override;

    // Experiment-specific functionality
    void setRewardSystem(std::shared_ptr<Reward> reward) { rewardSystem_ = reward; }
    void setCuriositySystem(std::shared_ptr<Curiosity> curiosity) { curiositySystem_ = curiosity; }
    void setNoveltySystem(std::shared_ptr<Novelty> novelty) { noveltySystem_ = novelty; }

    double calculatePerformanceScore() const override;
    std::string getResultsSummary() const override;

    std::string getExperimentType() const override { return "neuromodulation"; }
    std::string getDescription() const override { return "Studies reward, motivation, and neuromodulation systems"; }
    std::vector<std::string> getRequiredParameters() const override;

    bool validateParameters() const override;

private:
    std::shared_ptr<Reward> rewardSystem_;
    std::shared_ptr<Curiosity> curiositySystem_;
    std::shared_ptr<Novelty> noveltySystem_;
    std::vector<double> rewardValues_;
    std::vector<double> noveltyValues_;
    std::vector<double> curiosityValues_;
};

} // namespace nlm