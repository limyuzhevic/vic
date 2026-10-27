// Enhanced Learning Experiment System for NLM Brain
// Advanced learning experiments with improved analysis and reporting

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <mutex>
#include "Brain.hpp"

namespace nlm {

// Learning experiment types
enum class LearningExperimentType {
    PlasticityTest,
    STDPVerification,
    HebbianLearning,
    RewardBasedLearning,
    CuriosityDrivenLearning,
    NoveltyBasedLearning,
    MemoryConsolidation,
    DevelopmentEffects,
    NetworkIntegration,
    Custom
};

// Learning experiment results
struct LearningExperimentResults {
    std::string experimentName;
    LearningExperimentType type;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    double durationSeconds;
    
    // Core metrics
    size_t initialSynapseCount;
    size_t finalSynapseCount;
    float initialMeanWeight;
    float finalMeanWeight;
    float weightChange;
    size_t strengthenedSynapses;
    size_t weakenedSynapses;
    size_t unchangedSynapses;
    
    // Performance metrics
    size_t totalSpikes;
    size_t firingNeuronCount;
    float averageFiringRate;
    float excitationInhibitionRatio;
    
    // Learning effectiveness
    bool learningDetected;
    float learningMagnitude;
    float convergenceRate;
    
    // Validation and quality metrics
    bool validationPassed;
    std::string validationMessage;
    size_t sampleSize;
    float confidenceInterval;
    
    LearningExperimentResults()
        : type(LearningExperimentType::Custom), initialSynapseCount(0), finalSynapseCount(0),
          initialMeanWeight(0.0f), finalMeanWeight(0.0f), weightChange(0.0f),
          strengthenedSynapses(0), weakenedSynapses(0), unchangedSynapses(0),
          totalSpikes(0), firingNeuronCount(0), averageFiringRate(0.0f),
          excitationInhibitionRatio(0.0f), learningDetected(false),
          learningMagnitude(0.0f), convergenceRate(0.0f),
          validationPassed(false), sampleSize(0), confidenceInterval(0.0f) {}
};

// Enhanced learning experiment analyzer
class LearningExperimentAnalyzer {
public:
    LearningExperimentAnalyzer();
    ~LearningExperimentAnalyzer();
    
    // Experiment execution
    LearningExperimentResults runExperiment(const std::string& name, LearningExperimentType type,
                                           std::shared_ptr<Brain> brain,
                                           const std::unordered_map<std::string, std::string>& parameters);
    
    // Analysis
    std::string analyzeLearningEffectiveness(const LearningExperimentResults& results) const;
    std::string analyzePlasticityChanges(const LearningExperimentResults& results) const;
    std::string analyzeMemoryEffects(const LearningExperimentResults& results) const;
    
    // Statistical analysis
    double calculateEffectSize(const LearningExperimentResults& results) const;
    double calculateStatisticalSignificance(const LearningExperimentResults& results) const;
    
    // Pattern recognition
    std::string detectLearningPhases(const LearningExperimentResults& results) const;
    
    // Export and reporting
    std::string exportResults(const LearningExperimentResults& results, const std::string& format = "text") const;
    std::string generateDetailedReport(const LearningExperimentResults& results) const;
    
    // Catalog management
    std::vector<std::string> getAvailableExperiments() const;
    std::string getExperimentDescription(const std::string& name) const;
};

// Enhanced learning experiment runner
class EnhancedLearningExperimentRunner {
public:
    EnhancedLearningExperimentRunner(std::shared_ptr<Brain> brain);
    ~EnhancedLearningExperimentRunner();
    
    // Experiment execution
    LearningExperimentResults runPlasticityExperiment(const std::string& name);
    LearningExperimentResults runSTDPExperiment(const std::string& name);
    LearningExperimentResults runHebbianExperiment(const std::string& name);
    LearningExperimentResults runRewardBasedExperiment(const std::string& name);
    LearningExperimentResults runCuriosityDrivenExperiment(const std::string& name);
    LearningExperimentResults runNoveltyBasedExperiment(const std::string& name);
    LearningExperimentResults runMemoryConsolidationExperiment(const std::string& name);
    LearningExperimentResults runDevelopmentExperiment(const std::string& name);
    LearningExperimentResults runNetworkIntegrationExperiment(const std::string& name);
    
    // Custom experiments
    LearningExperimentResults runCustomExperiment(const std::string& name,
                                                  LearningExperimentType type,
                                                  const std::unordered_map<std::string, std::string>& parameters);
    
    // Batch experiments
    std::vector<LearningExperimentResults> runExperimentSeries(const std::string& experimentType, size_t count);
    std::vector<LearningExperimentResults> runComparativeExperiments(const std::vector<std::string>& experimentNames);
    
    // Reporting
    std::string generateLearningReport(const LearningExperimentResults& results) const;
    std::string generateComparativeReport(const std::vector<LearningExperimentResults>& results) const;
    
    // Experiment management
    bool saveExperimentResults(const LearningExperimentResults& results, const std::string& filename) const;
    bool loadExperimentResults(LearningExperimentResults& results, const std::string& filename) const;
    std::vector<LearningExperimentResults> getExperimentHistory() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience functions
inline LearningExperimentResults runPlasticityLearningExperiment(std::shared_ptr<Brain> brain, const std::string& name = "Plasticity Experiment") {
    if (!brain) return LearningExperimentResults{};
    EnhancedLearningExperimentRunner runner(brain);
    return runner.runPlasticityExperiment(name);
}

inline LearningExperimentResults runSTDSimulationExperiment(std::shared_ptr<Brain> brain, const std::string& name = "STDP Experiment") {
    if (!brain) return LearningExperimentResults{};
    EnhancedLearningExperimentRunner runner(brain);
    return runner.runSTDPExperiment(name);
}

inline LearningExperimentResults runHebbianLearningExperiment(std::shared_ptr<Brain> brain, const std::string& name = "Hebbian Learning Experiment") {
    if (!brain) return LearningExperimentResults{};
    EnhancedLearningExperimentRunner runner(brain);
    return runner.runHebbianExperiment(name);
}

inline LearningExperimentResults runRewardBasedLearningExperiment(std::shared_ptr<Brain> brain, const std::string& name = "Reward-Based Learning Experiment") {
    if (!brain) return LearningExperimentResults{};
    EnhancedLearningExperimentRunner runner(brain);
    return runner.runRewardBasedExperiment(name);
}

inline LearningExperimentResults runCuriosityDrivenLearningExperiment(std::shared_ptr<Brain> brain, const std::string& name = "Curiosity-Driven Learning Experiment") {
    if (!brain) return LearningExperimentResults{};
    EnhancedLearningExperimentRunner runner(brain);
    return runner.runCuriosityDrivenExperiment(name);
}

inline LearningExperimentResults runMemoryConsolidationExperiment(std::shared_ptr<Brain> brain, const std::string& name = "Memory Consolidation Experiment") {
    if (!brain) return LearningExperimentResults{};
    EnhancedLearningExperimentRunner runner(brain);
    return runner.runMemoryConsolidationExperiment(name);
}

inline LearningExperimentResults runDevelopmentExperiment(std::shared_ptr<Brain> brain, const std::string& name = "Development Experiment") {
    if (!brain) return LearningExperimentResults{};
    EnhancedLearningExperimentRunner runner(brain);
    return runner.runDevelopmentExperiment(name);
}

inline LearningExperimentResults runNetworkIntegrationExperiment(std::shared_ptr<Brain> brain, const std::string& name = "Network Integration Experiment") {
    if (!brain) return LearningExperimentResults{};
    EnhancedLearningExperimentRunner runner(brain);
    return runner.runNetworkIntegrationExperiment(name);
}

} // namespace nlm