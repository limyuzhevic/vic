// ====================================================================
// TRANSFER LEARNING MODULE
// ====================================================================

#ifndef SRC_ADVANCED_TRANSFERLEARNING_H
#define SRC_ADVANCED_TRANSFERLEARNING_H

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
#include <set>

namespace nlm {

class TransferLearning {
public:
    struct SourceDomain {
        std::string domainId;
        std::string domainName;
        std::map<std::string, double> domainFeatures;
        std::shared_ptr<Brain> sourceBrain;
        double domainPerformance;
        std::chrono::system_clock::time_point collectedTime;
        
        SourceDomain() : domainPerformance(0.0) {}
    };
    
    struct TargetDomain {
        std::string domainId;
        std::string domainName;
        std::map<std::string, double> domainParameters;
        std::shared_ptr<Brain> targetBrain;
        std::map<std::string, double> adaptationParameters;
        double similarityScore;
        double transferGain;
        
        TargetDomain() : similarityScore(0.0), transferGain(0.0) {}
    };
    
    struct TransferTask {
        std::string taskId;
        std::string taskType;
        std::shared_ptr<Brain> sourceBrain;
        std::map<std::string, double> taskFeatures;
        std::map<std::string, double> expectedTransfer;
        double actualTransfer;
        std::chrono::system_clock::time_point taskTime;
        
        TransferTask() : actualTransfer(0.0) {}
    };
    
    // Domain adaptation
    static void adaptDomain(std::shared_ptr<Brain> targetBrain,
                           const std::map<std::string, double>& sourceDomainData,
                           const std::map<std::string, double>& adaptationParams);
    
    // Transfer learning implementation
    static void enableTransferLearning(std::shared_ptr<Brain> targetBrain,
                                       const std::vector<SourceDomain>& sourceDomains,
                                       const std::map<std::string, double>& transferParams);
    
    // Cross-domain knowledge transfer
    static double transferKnowledge(std::shared_ptr<Brain> sourceBrain,
                                    std::shared_ptr<Brain> targetBrain,
                                    const std::map<std::string, double>& transferConfig);
    
    // Domain similarity measurement
    static double measureDomainSimilarity(const std::map<std::string, double>& sourceFeatures,
                                          const std::map<std::string, double>& targetFeatures);
    
    // Task transfer evaluation
    static std::map<std::string, double> evaluateTaskTransfer(
        const std::vector<TransferTask>& transferTasks,
        const std::map<std::string, double>& evaluationCriteria);
    
    // Domain generalization
    static std::shared_ptr<Brain> createGeneralizableBrain(
        const std::vector<SourceDomain>& sourceDomains,
        const std::map<std::string, double>& generalizableParams);
    
    // Negative transfer detection
    static std::vector<std::string> detectNegativeTransfer(
        const std::vector<TransferTask>& transferTasks,
        double negativeTransferThreshold = 0.3);
    
    // Multi-source transfer learning
    static void enableMultiSourceTransferLearning(
        std::shared_ptr<Brain> targetBrain,
        const std::vector<SourceDomain>& sourceDomains,
        const std::map<std::string, double>& ensembleParams);
    
    // Domain-invariant feature learning
    static std::map<std::string, double> learnDomainInvariantFeatures(
        const std::vector<SourceDomain>& sourceDomains,
        const std::string& featureType = "neural");
    
    // Transfer learning optimization
    static std::map<std::string, double> optimizeTransferParameters(
        const std::vector<TransferTask>& transferTasks,
        const std::map<std::string, double>& currentParams);
    
    // Task-agnostic learning
    static std::shared_ptr<Brain> createTaskAgnosticLearner(
        const std::vector<SourceDomain>& sourceDomains,
        const std::map<std::string, double>& taskAgnosticParams);
    
    // Zero-shot transfer learning
    static double zeroShotTransfer(
        std::shared_ptr<Brain> sourceBrain,
        const std::string& targetTaskDescription,
        const std::map<std::string, double>& zeroShotParams);
    
    // Few-shot transfer learning
    static std::map<std::string, double> fewShotTransfer(
        std::shared_ptr<Brain> sourceBrain,
        const std::vector<TransferTask>& fewShotTasks,
        int supportSetSize = 5,
        const std::map<std::string, double>& fewShotParams = {});
    
    // Meta-transfer learning
    static std::map<std::string, double> metaTransferLearning(
        const std::vector<TransferTask>& transferTasks,
        const std::map<std::string, double>& metaParams);
    
    // Continual transfer learning
    static void enableContinualTransferLearning(
        std::shared_ptr<Brain> brain,
        const std::vector<SourceDomain>& sourceDomains,
        const std::map<std::string, double>& continualParams,
        double forgettingPreventionStrength = 0.1);
    
    // Transfer learning performance monitoring
    static std::map<std::string, double> monitorTransferLearningPerformance(
        const std::vector<TransferTask>& transferTasks,
        const std::map<std::string, double>& performanceMetrics);
    
    // Domain alignment
    static std::map<std::string, double> alignDomains(
        const std::vector<SourceDomain>& sourceDomains,
        const std::string& alignmentMethod = "feature_matching");
    
    // Transfer learning success prediction
    static std::map<std::string, double> predictTransferSuccess(
        const std::vector<TransferTask>& transferTasks,
        const std::map<std::string, double>& predictionFeatures);
};

} // namespace nlm

#endif // SRC_ADVANCED_TRANSFERLEARNING_H
