// ====================================================================
// META LEARNING MODULE
// ====================================================================

#ifndef SRC_ADVANCED_METALEARNING_H
#define SRC_ADVANCED_METALEARNING_H

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <numeric>
#include <random>
#include <set>

namespace nlm {

class MetaLearning {
public:
    struct MetaLearningState {
        double metaLearningRate;
        double transferLearningStrength;
        double continualLearningFactor;
        double federatedLearningWeight;
        double activeLearningScore;
        std::vector<double> learningParameters;
        std::map<std::string, double> adaptationMetrics;
        
        MetaLearningState() : metaLearningRate(0.001), transferLearningStrength(0.5),
                             continualLearningFactor(0.8), federatedLearningWeight(0.3),
                             activeLearningScore(0.0) {}
    };
    
    struct Task {
        std::string taskId;
        std::string taskType;
        std::map<std::string, double> taskParameters;
        double expectedOutcome;
        double actualOutcome;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, double> taskFeatures;
        
        Task() : expectedOutcome(0.0), actualOutcome(0.0) {}
    };
    
    struct LearningExperience {
        std::string experienceId;
        std::vector<Task> tasks;
        double metaLearningGain;
        std::chrono::system_clock::time_point experienceTime;
        std::map<std::string, double> learnedParameters;
        
        LearningExperience() : metaLearningGain(0.0) {}
    };
    
    // Meta-learning control
    static void enableMetaLearning(std::shared_ptr<Brain> brain, const MetaLearningState& state);
    
    // Transfer learning
    static void enableTransferLearning(std::shared_ptr<Brain> brain, 
                                        const std::map<std::string, double>& sourceDomainData,
                                        const std::map<std::string, double>& targetDomainParams);
    
    // Continual learning
    static void enableContinualLearning(std::shared_ptr<Brain> brain,
                                        const std::vector<LearningExperience>& experiences,
                                        double plasticityDecayRate = 0.99);
    
    // Federated learning
    static void enableFederatedLearning(std::shared_ptr<Brain> brain,
                                        const std::vector<std::shared_ptr<Brain>>& agentBrains,
                                        double aggregationStrength = 0.5);
    
    // Active learning
    static Task selectActiveLearningTask(std::shared_ptr<Brain> brain,
                                         const std::vector<Task>& availableTasks,
                                         double explorationRate = 0.1);
    
    static double evaluateTaskOutcome(std::shared_ptr<Brain> brain, const Task& task);
    
    // Meta-learning optimization
    static MetaLearningState optimizeMetaLearningParameters(std::shared_ptr<Brain> brain,
                                                            const std::vector<LearningExperience>& experiences);
    
    // Cross-task knowledge transfer
    static double transferKnowledge(std::shared_ptr<Brain> brain,
                                    const std::string& sourceTaskId,
                                    const std::string& targetTaskId,
                                    const std::map<std::string, double>& transferParams);
    
    // Continual learning without catastrophic forgetting
    static void preventCatastrophicForgetting(std::shared_ptr<Brain> brain,
                                             const std::vector<LearningExperience>& experiences,
                                             double regularizationStrength = 0.01);
    
    // Federated learning aggregation
    static std::shared_ptr<Brain> aggregateFederatedModels(
        const std::vector<std::shared_ptr<Brain>>& agentBrains,
        const std::map<std::string, double>& aggregationWeights);
    
    // Active learning exploration-exploitation
    static std::pair<Task, bool> activeLearningDecision(
        std::shared_ptr<Brain> brain,
        const std::vector<Task>& candidateTasks,
        const std::map<std::string, double>& explorationExploitationParams);
    
    // Meta-learning progress tracking
    static std::map<std::string, double> trackMetaLearningProgress(
        const std::vector<LearningExperience>& experiences,
        const std::string& metric = "meta_learning_rate");
    
    // Transfer learning effectiveness
    static double measureTransferLearningEffectiveness(
        const std::map<std::string, double>& sourceDomainResults,
        const std::map<std::string, double>& targetDomainResults);
    
    // Continual learning stability
    static std::map<std::string, double> assessContinualLearningStability(
        const std::vector<LearningExperience>& experiences,
        double stabilityThreshold = 0.95);
    
    // Federated learning convergence
    static std::map<std::string, double> checkFederatedLearningConvergence(
        const std::vector<std::shared_ptr<Brain>>& agentBrains,
        const std::map<std::string, double>& convergenceCriteria);
    
    // Active learning efficiency
    static std::map<std::string, double> evaluateActiveLearningEfficiency(
        const std::vector<Task>& selectedTasks,
        const std::vector<double>& taskValues,
        const std::string& efficiencyMetric = "bits_per_instance");
};

} // namespace nlm

#endif // SRC_ADVANCED_METALEARNING_H
