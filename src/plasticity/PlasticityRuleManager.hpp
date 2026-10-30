#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <chrono>
#include <sstream>
#include <iostream>

#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"

namespace nlm {

// Plasticity rule statistics
struct RuleStats {
    size_t updateCount{0};
    size_t successfulUpdates{0};
    size_t errorCount{0};
    long long totalUpdateTime{0};  // microseconds
    float averageWeightChange{0.0f};
    std::map<std::string, float> parameters;
};

// Learning phases for neural network plasticity
enum class LearningPhase {
    EXPLORATION,
    EXPLOITATION,
    CONSOLIDATION
};

// Brain activity data for plasticity adjustment
struct BrainActivity {
    float averageFiringRate{0.0f};
    float excitationInhibitionRatio{0.0f};
    float noveltyLevel{0.0f};
    float curiosityLevel{0.0f};
    float predictionError{0.0f};

    float getAverageFiringRate() const { return averageFiringRate; }
    void setAverageFiringRate(float rate) { averageFiringRate = rate; }

    float getExcitationInhibitionRatio() const { return excitationInhibitionRatio; }
    void setExcitationInhibitionRatio(float ratio) { excitationInhibitionRatio = ratio; }

    float getNoveltyLevel() const { return noveltyLevel; }
    void setNoveltyLevel(float level) { noveltyLevel = level; }

    float getCuriosityLevel() const { return curiosityLevel; }
    void setCuriosityLevel(float level) { curiosityLevel = level; }

    float getPredictionError() const { return predictionError; }
    void setPredictionError(float error) { predictionError = error; }
};

// Synapse sample for plasticity training
struct SynapseSample {
    std::shared_ptr<Synapse> synapse;
    std::vector<Timestamp> preSpikes;
    std::vector<Timestamp> postSpikes;
    TimestepDuration dt;
};

// Neural network topology for plasticity configuration
struct NeuralNetworkTopology {
    std::string type;  // "layered", "modular", "spiking", etc.
    std::vector<std::string> layers;
    std::vector<std::string> modules;
    float connectionProbability{0.1f};
    size_t maxNeurons{1000};
};

// Training data for plasticity network
struct BrainTrainingData {
    std::vector<SynapseSample> samples;

    size_t getTotalSteps() const { return samples.size(); }

    SynapseSample getSample(size_t index) {
        if (index < samples.size()) {
            return samples[index];
        }
        return SynapseSample{};  // Return empty sample if index out of range
    }
};

// Advanced plasticity rule manager
class PlasticityRuleManager {
private:
    std::map<std::string, std::shared_ptr<PlasticityRule>> rules_;
    std::map<std::string, int> priority_;
    std::map<std::string, bool> enabled_;
    std::map<std::string, RuleStats> ruleStats_;
    float globalLearningRate_;

public:
    PlasticityRuleManager();

    // Rule management
    void addRule(const std::string& name, std::shared_ptr<PlasticityRule> rule, int priority = 0);
    void removeRule(const std::string& name);
    void enableRule(const std::string& name, bool enabled);
    bool isRuleEnabled(const std::string& name) const;

    std::vector<std::string> getEnabledRules() const;
    void clearAllRules();
    void setLearningRate(float rate);
    float getLearningRate() const;

    // Configuration and statistics
    void configureRule(const std::string& name, const std::map<std::string, float>& params);
    std::map<std::string, float> getRuleStats() const;

    // Plasticity application
    void updateAllRules(Synapse* synapse,
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt);
    void applyLearningSignal(float learningSignal);

    // Performance analysis
    std::vector<std::pair<std::string, float>> getTopPerformingRules(size_t count) const;
    std::string getRuleSummary() const;

    // Statistics management
    void resetStatistics();
};

// Neural network plasticity manager
class NeuralNetworkPlasticityManager {
private:
    std::shared_ptr<Brain> brain_;
    std::map<std::string, std::shared_ptr<PlasticityRule>> plasticityRules_;
    std::map<std::string, bool> enabledRules_;

    LearningPhase currentLearningPhase_;
    float learningRate_;
    float explorationRate_;
    float curiosityThreshold_;
    float noveltyThreshold_;

    std::string networkName_;
    NeuralNetworkTopology networkTopology_;

public:
    NeuralNetworkPlasticityManager(std::shared_ptr<Brain> brain);

    // Learning phase management
    void createLearningPhase(const std::string& phaseName);
    LearningPhase getCurrentLearningPhase() const { return currentLearningPhase_; }

    // Plasticity adjustment
    void updatePlasticityBasedOnExperience(const BrainActivity& activity);
    void setLearningRate(float rate) { learningRate_ = std::max(0.0f, std::min(rate, 1.0f)); }
    float getLearningRate() const { return learningRate_; }

    // Network topology management
    void createPlasticityNetwork(const std::string& networkName,
                                const NeuralNetworkTopology& topology);

    // Synapse plasticity application
    void applyPlasticityToSynapse(SynapseId synapseId,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt);

    // Training and learning
    void trainPlasticityNetwork(const BrainTrainingData& trainingData);

    // Configuration and export
    std::map<std::string, float> getPlasticityStatistics() const;
    void exportPlasticityConfiguration(const std::string& filename);
};

} // namespace nlm
