#ifndef NLM_ADVANCED_LEARNING_H
#define NLM_ADVANCED_LEARNING_H

#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <algorithm>
#include <random>
#include <cmath>
#include <chrono>
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/core/Config/Config.hpp"

namespace nlm {
namespace advanced {

/**
 * @brief Advanced learning algorithms for neural systems
 * 
 * This module provides sophisticated learning algorithms that extend
 * the basic STDP and Hebbian learning mechanisms with more complex
 * and biologically plausible learning rules.
 */
class AdvancedLearningSystem {
public:
    /**
     * @brief Configuration for advanced learning
     */
    struct LearningConfig {
        // Learning algorithm selection
        enum Algorithm {
            BASIC_STDP,
            ELIGIBILITY_TRACE,
            POLICY_GRADIENT,
            NEUROMODULATION_BASED,
            PREDICTIVE_CODING,
            META_LEARNING,
            CONSOLIDATION,
            FORGETTING
        };
        
        Algorithm primaryAlgorithm = Algorithm::ELIGIBILITY_TRACE;
        Algorithm secondaryAlgorithm = Algorithm::NEUROMODULATION_BASED;
        
        // Parameters for different algorithms
        float learningRate = 0.01f;
        float eligibilityDecayRate = 0.95f;
        float rewardScaling = 1.0f;
        float predictionErrorWeight = 1.0f;
        float metaLearningRate = 0.001f;
        float consolidationThreshold = 0.8f;
        float forgettingRate = 0.001f;
        
        // Network architecture parameters
        size_t hiddenLayerSize = 500;
        size_t recurrentConnections = 0.1f;
        float connection sparsity = 0.1f;
        
        // Learning dynamics
        float noveltySensitivity = 1.0f;
        float uncertaintyWeight = 0.5f;
        float explorationRate = 0.1f;
        
        // Implementation options
        bool enableReplay = true;
        bool enableConsolidation = true;
        bool enableMetaLearning = false;
        bool enableNeuralPruning = true;
    };
    
    /**
     * @brief Eligibility trace implementation for credit assignment
     */
    class EligibilityTrace {
    public:
        struct TraceEntry {
            SynapseId synapseId;
            NeuronId preNeuronId;
            NeuronId postNeuronId;
            float traceValue;
            Timestamp lastUpdate;
            float decayFactor;
            
            TraceEntry(SynapseId synId, NeuronId preId, NeuronId postId, float decay)
                : synapseId(synId), preNeuronId(preId), postNeuronId(postId),
                  traceValue(0.0f), lastUpdate(0.0), decayFactor(decay) {}
        };
        
        void addTrace(SynapseId synapseId, NeuronId preNeuronId, NeuronId postNeuronId, float decayRate);
        void updateTraces(float decay, float reward);
        void resetTraces();
        std::vector<TraceEntry> getActiveTraces() const;
        float getTraceStrength(SynapseId synapseId) const;
        
    private:
        std::vector<TraceEntry> traces_;
        float globalDecay_ = 0.95f;
    };
    
    /**
     * @brief Policy gradient learning for action selection
     */
    class PolicyGradientLearner {
    public:
        struct PolicyState {
            std::vector<float> actionValues;
            std::vector<float> actionProbabilities;
            std::vector<float> stateFeatures;
            Timestamp timestamp;
            float entropy;
            
            PolicyState(size_t numActions) 
                : actionValues(numActions, 0.0f), actionProbabilities(numActions, 0.0f),
                  entropy(0.0f), timestamp(0.0) {}
        };
        
        PolicyGradientLearner(size_t numActions, float learningRate = 0.01f);
        void observe(const PolicyState& state, int action, float reward, float discount = 0.99f);
        void updatePolicy();
        int selectAction(const PolicyState& state, bool explore = true);
        float calculateAdvantage(const PolicyState& state, int action, float reward);
        void reset();
        
    private:
        size_t numActions_;
        float learningRate_;
        std::vector<float> policyGradient_;
        std::vector<float> valueFunction_;
        std::vector<PolicyState> trajectory_;
        std::mt19937 rng_;
    };
    
    /**
     * @brief Neuromodulation-based learning system
     */
    class NeuromodulationLearner {
    public:
        struct NeuromodulationSignal {
            enum Type { DOPAMINE, NOREPINEFRIN, ACETYLCHOLINE, SEROTONIN, ADRENALINE };
            Type type;
            float magnitude;
            float uncertainty;
            float predictionError;
            Timestamp timestamp;
            
            NeuromodulationSignal(Type t, float mag, float unc, float pe, Timestamp ts)
                : type(t), magnitude(mag), uncertainty(unc), predictionError(pe), timestamp(ts) {}
        };
        
        NeuromodulationLearner();
        void updateSignals(const std::vector<NeuromodulationSignal>& signals);
        float computeModulatedLearningRate(const NeuromodulationSignal& signal, float baseRate);
        void applyModulatedPlasticity(std::shared_ptr<Brain> brain, const std::vector<NeuromodulationSignal>& signals);
        float predictBehavioralResponse(const std::vector<NeuromodulationSignal>& signals);
        
    private:
        std::map<NeuromodulationSignal::Type, float> neuromodulationWeights_;
        std::vector<float> signalHistory_;
    };
    
    /**
     * @brief Predictive coding learning algorithm
     */
    class PredictiveCodingLearner {
    public:
        struct PredictionError {
            Timestamp timestamp;
            float predictionError;
            std::vector<float> topDownPredictions;
            std::vector<float> bottomUpInputs;
            std::vector<size_t> affectedSynapses;
            
            PredictionError(Timestamp ts, float error, const std::vector<float>& topDown,
                          const std::vector<float>& bottomUp,
                          const std::vector<size_t>& synapses)
                : timestamp(ts), predictionError(error),
                  topDownPredictions(topDown), bottomUpInputs(bottomUp),
                  affectedSynapses(synapses) {}
        };
        
        PredictiveCodingLearner(size_t hierarchyLevels = 3);
        void updatePredictions(std::shared_ptr<Brain> brain);
        void computePredictionErrors(std::vector<PredictionError>& errors);
        void applyPredictionErrors(const std::vector<PredictionError>& errors);
        void updateHierarchicalWeights();
        
    private:
        size_t hierarchyLevels_;
        std::vector<std::vector<float>> hierarchicalWeights_;
        std::vector<float> predictionErrors_;
    };
    
    /**
     * @brief Meta-learning system for learning to learn
     */
    class MetaLearningSystem {
    public:
        struct MetaLearningState {
            float learningSpeed;
            float adaptationRate;
            float explorationRate;
            float uncertaintyEstimate;
            std::vector<float> hyperparameterHistory;
            std::vector<float> performanceHistory;
            Timestamp lastUpdate;
        };
        
        MetaLearningSystem();
        void updateMetaParameters(const MetaLearningState& currentState, float performance);
        float optimizeHyperparameters(const std::vector<float>& hyperparameters, float performance);
        float estimateUncertainty(float recentPerformance, size_t historySize = 100);
        float adaptLearningRate(float error, float baseRate);
        MetaLearningState getCurrentMetaState() const;
        
    private:
        MetaLearningState currentMetaState_;
        std::vector<float> hyperparameterMemory_;
        std::vector<float> performanceMemory_;
        std::mt19937 rng_;
    };
    
    /**
     * @brief Memory consolidation system for long-term learning
     */
    class MemoryConsolidationSystem {
    public:
        struct MemoryRecord {
            std::vector<float> synapticWeights;
            std::vector<Timestamp> activationTimestamps;
            float consolidationStrength;
            float replayFrequency;
            float memoryStability;
            
            MemoryRecord(const std::vector<float>& weights, const std::vector<Timestamp>& timestamps)
                : synapticWeights(weights), activationTimestamps(timestamps),
                  consolidationStrength(0.0f), replayFrequency(0.0f),
                  memoryStability(0.0f) {}
        };
        
        MemoryConsolidationSystem();
        void consolidateMemories(std::shared_ptr<Brain> brain);
        void scheduleReplay(const std::vector<MemoryRecord>& memories);
        void replayMemory(const MemoryRecord& memory, float strength);
        void updateConsolidationStrength(std::shared_ptr<Brain> brain);
        std::vector<MemoryRecord> extractImportantMemories(std::shared_ptr<Brain> brain);
        
    private:
        std::vector<MemoryRecord> memoryBank_;
        float consolidationThreshold_ = 0.8f;
        float replayProbability_ = 0.1f;
        Timestamp lastReplayTime_ = 0;
    };
    
    // Main interface
    explicit AdvancedLearningSystem(const LearningConfig& config);
    
    /**
     * @brief Run advanced learning simulation
     */
    void runAdvancedLearning(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                            std::shared_ptr<SimpleWorld> world, size_t numSteps);
    
    /**
     * @brief Apply all learning algorithms to the brain
     */
    void applyAllLearningAlgorithms(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                                   const std::vector<float>& rewards);
    
    /**
     * @brief Update eligibility traces
     */
    void updateEligibilityTraces(std::shared_ptr<Brain> brain, const std::vector<float>& rewards);
    
    /**
     * @brief Apply policy gradient learning
     */
    void applyPolicyGradientLearning(std::shared_ptr<AgentBrain> agent,
                                    const std::vector<float>& rewards);
    
    /**
     * @brief Apply neuromodulation learning
     */
    void applyNeuromodulationLearning(std::shared_ptr<AgentBrain> agent,
                                      const std::vector<float>& neuromodulationSignals);
    
    /**
     * @brief Apply predictive coding learning
     */
    void applyPredictiveCodingLearning(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Apply meta-learning
     */
    void applyMetaLearning(std::shared_ptr<AgentBrain> agent, float performance);
    
    /**
     * @brief Apply memory consolidation
     */
    void applyMemoryConsolidation(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Generate advanced training curriculum
     */
    std::vector<size_t> generateCurriculum(size_t totalSteps, const std::string& taskType);
    
    /**
     * @brief Evaluate learning performance
     */
    py::dict evaluateLearningPerformance(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent);
    
    /**
     * @brief Get learning statistics
     */
    py::dict getLearningStatistics() const;
    
    /**
     * @brief Reset learning system
     */
    void reset();
    
    /**
     * @brief Get current configuration
     */
    const LearningConfig& getConfig() const { return config_; }
    
    /**
     * @brief Update configuration
     */
    void updateConfig(const LearningConfig& newConfig) { config_ = newConfig; }
    
private:
    LearningConfig config_;
    EligibilityTrace eligibilityTrace_;
    PolicyGradientLearner policyLearner_;
    NeuromodulationLearner neuromodulationLearner_;
    PredictiveCodingLearner predictiveCodingLearner_;
    MetaLearningSystem metaLearningSystem_;
    MemoryConsolidationSystem consolidationSystem_;
    
    std::vector<float> recentRewards_;
    std::vector<float> neuromodulationSignals_;
    std::vector<size_t> curriculumSteps_;
    size_t currentCurriculumStep_;
    
    float calculatePredictionError(const std::vector<float>& predictions, const std::vector<float>& actuals);
    float calculateNoveltyScore(std::shared_ptr<Brain> brain);
    void applySynapticModulation(std::shared_ptr<Brain> brain, const std::vector<float>& modulations);
};

/**
 * @brief Factory functions for creating advanced learning systems
 */
inline std::shared_ptr<AdvancedLearningSystem> createAdvancedLearningSystem(
    const AdvancedLearningSystem::LearningConfig& config) {
    return std::make_shared<AdvancedLearningSystem>(config);
}

/**
 * @brief Convenience function for running advanced learning
 */
inline py::dict runAdvancedLearningSimulation(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<AgentBrain> agent,
    std::shared_ptr<SimpleWorld> world,
    size_t numSteps,
    const AdvancedLearningSystem::LearningConfig& learningConfig) {
    
    auto learningSystem = createAdvancedLearningSystem(learningConfig);
    learningSystem->runAdvancedLearning(brain, agent, world, numSteps);
    
    return learningSystem->evaluateLearningPerformance(brain, agent);
}

/**
 * @brief Create predefined learning configurations
 */
inline AdvancedLearningSystem::LearningConfig createStandardLearningConfig() {
    AdvancedLearningSystem::LearningConfig config;
    config.primaryAlgorithm = AdvancedLearningSystem::LearningConfig::ELIGIBILITY_TRACE;
    config.secondaryAlgorithm = AdvancedLearningSystem::LearningConfig::NEUROMODULATION_BASED;
    config.learningRate = 0.01f;
    config.eligibilityDecayRate = 0.95f;
    config.rewardScaling = 1.0f;
    config.enableReplay = true;
    config.enableConsolidation = true;
    config.enableMetaLearning = false;
    config.enableNeuralPruning = true;
    return config;
}

inline AdvancedLearningSystem::LearningConfig createMetaLearningConfig() {
    AdvancedLearningSystem::LearningConfig config;
    config.primaryAlgorithm = AdvancedLearningSystem::LearningConfig::META_LEARNING;
    config.secondaryAlgorithm = AdvancedLearningSystem::LearningConfig::ELIGIBILITY_TRACE;
    config.learningRate = 0.001f;
    config.eligibilityDecayRate = 0.99f;
    config.metaLearningRate = 0.01f;
    config.enableMetaLearning = true;
    config.enableReplay = true;
    config.enableConsolidation = true;
    return config;
}

inline AdvancedLearningSystem::LearningConfig createFastLearningConfig() {
    AdvancedLearningSystem::LearningConfig config;
    config.primaryAlgorithm = AdvancedLearningSystem::LearningConfig::BASIC_STDP;
    config.secondaryAlgorithm = AdvancedLearningSystem::LearningConfig::NEUROMODULATION_BASED;
    config.learningRate = 0.05f;
    config.eligibilityDecayRate = 0.9f;
    config.rewardScaling = 2.0f;
    config.explorationRate = 0.2f;
    config.enableReplay = false;
    config.enableConsolidation = false;
    config.enableMetaLearning = false;
    return config;
}

} // namespace advanced
} // namespace nlm

#endif // NLM_ADVANCED_LEARNING_H
