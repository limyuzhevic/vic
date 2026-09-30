// LearningAnalysis.hpp - Learning efficiency analysis for NLM

#pragma once

#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace nlm {

// Learning analysis framework for measuring learning efficiency and trajectory analysis
class LearningAnalysis {
public:
    LearningAnalysis(std::shared_ptr<Logger> logger);
    ~LearningAnalysis();
    
    // Initialize learning analysis system
    bool initialize();
    
    // Learning efficiency metrics
    struct LearningMetrics {
        // Learning rate and speed
        float learningRate; // rate of synaptic weight change over time
        float acquisitionTime; // time to reach learning asymptote
        float sampleEfficiency; // samples needed to achieve target performance
        float timeEfficiency; // time efficiency of learning
        
        // Quality of learning
        float retentionQuality; // ability to retain learned information
        float generalizationAbility; // ability to apply learning to new contexts
        float transferEfficiency; // ability to transfer learning across tasks
        float stability; // resistance to forgetting after learning
        
        // Performance trajectory
        std::vector<float> performanceCurve; // performance over time/steps
        std::vector<float> learningTrajectory; // learning progress over time
        std::vector<float> errorTrajectory; // prediction error over time
        
        // Statistical properties of learning
        float learningVariance; // variability in learning rate
        float learningBias; // systematic deviation from expected learning
        float overfittingIndex; // tendency to overfit training data
        float underfittingIndex; // inability to capture patterns
        
        // Thresholds and benchmarks
        float convergenceThreshold; // criteria for learning convergence
        float performanceThreshold; // criteria for acceptable performance
        float retentionThreshold; // criteria for memory retention
    };
    
    LearningMetrics computeLearningMetrics(const Brain& brain, 
                                           double learningWindow = 60.0);
    
    // Memory consolidation metrics
    struct ConsolidationMetrics {
        // Consolidation process
        float consolidationRate; // rate at which memories are consolidated
        float integrationScore; // how well memories are integrated into existing knowledge
        float interferenceLevel; // level of memory interference during consolidation
        float rehearsalEffect; // effectiveness of memory rehearsal
        
        // Memory quality
        std::vector<float> memoryStrengths; // strength of different memory traces
        std::vector<float> memoryLatencies; // access times for different memories
        std::vector<size_t> memoryRanks; // priority/importance of memories
        
        // Consolidation dynamics
        std::vector<float> consolidationTrajectory; // consolidation over time
        float consolidationPeak; // peak consolidation activity
        float consolidationDuration; // duration of consolidation process
        float consolidationEfficiency; // efficiency of consolidation process
    };
    
    ConsolidationMetrics computeConsolidationMetrics(const Brain& brain);
    
    // Decision making and policy learning analysis
    struct DecisionMetrics {
        // Decision quality
        float choiceConsistency; // consistency of choices across trials
        float reactionTime; // average reaction time to make decisions
        float accuracy; // decision accuracy
        float confidence; // confidence in decisions
        
        // Exploration-exploitation balance
        float explorationRate; // tendency to explore new options
        float exploitationRate; // tendency to exploit known options
        float explorationExploitationBalance; // balance between exploration and exploitation
        float curiosityDrivenLearning; // level of curiosity-driven exploration
        
        // Policy learning
        std::vector<std::vector<float>> valueFunction; // learned values for different states
        std::vector<std::vector<size_t>> policy; // learned actions for different states
        std::vector<float> policyStability; // stability of learned policies
        
        // Reward processing
        std::vector<float> rewardHistory; // history of received rewards
        std::vector<float> predictionError; // difference between expected and actual rewards
        float rewardPredictionError; // overall reward prediction error
        float dopamineSignal; // simulated dopamine response
    };
    
    DecisionMetrics computeDecisionMetrics(const Brain& brain);
    
    // Learning phases identification
    enum class LearningPhase {
        EXPLORATION,      // Initial random exploration
        ACQUISITION,      // Active learning and hypothesis testing
        CONSOLIDATION,    // Memory consolidation and integration
        RETENTION,        // Maintaining learned information
        TRANSFER,        // Applying learning to new contexts
        FORGETTING        // Natural decay and interference
    };
    
    std::vector<std::pair<LearningPhase, float>> identifyLearningPhases(
        const Brain& brain, float threshold = 0.1f);
    
    // Learning efficiency comparison
    struct ComparativeLearning {
        std::string taskType; // type of learning task
        float performanceScore; // overall performance on task
        float learningSpeed; // speed of learning
        float retentionScore; // ability to retain learning
        float generalizationScore; // ability to generalize
        float transferScore; // ability to transfer to new tasks
        float stabilityScore; // stability of learned behavior
        float adaptabilityScore; // ability to adapt to changing environments
    };
    
    std::vector<ComparativeLearning> compareLearningAcrossTasks(const Brain& brain);
    
    // Export learning analysis results
    bool exportLearningMetrics(const std::string& filename, const LearningMetrics& metrics);
    bool exportConsolidationMetrics(const std::string& filename, const ConsolidationMetrics& metrics);
    bool exportDecisionMetrics(const std::string& filename, const DecisionMetrics& metrics);
    bool exportLearningPhases(const std::string& filename, const std::vector<std::pair<LearningPhase, float>>& phases);
    
    // Configuration methods
    void setLearningWindow(double window);
    void setPerformanceThreshold(float threshold);
    void setRetentionThreshold(float threshold);
    void enablePhaseIdentification(bool enable);
    void setLearningModel(const std::string& model);
    void setEvaluationMetrics(const std::vector<std::string>& metrics);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

