#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../plasticity/PlasticityRule.hpp"

namespace nlm {

// Meta-learning task
struct MetaTask {
    std::string id;
    std::string description;
    std::string sourceDomain;
    std::string targetDomain;
    std::vector<std::string> relatedTasks;
    double transferValue;
    double adaptationRate;
    std::map<std::string, double> parameters;
    bool learned;
    double performance;
    double adaptationSpeed;
    
    MetaTask(const std::string& taskId, const std::string& desc,
             const std::string& source, const std::string& target)
        : id(taskId), description(desc), sourceDomain(source), targetDomain(target),
          transferValue(0.0), adaptationRate(0.1), learned(false),
          performance(0.0), adaptationSpeed(0.0) {}
};

// Transfer learning method
class TransferLearningMethod {
public:
    virtual ~TransferLearningMethod() = default;
    virtual double computeTransferValue(const MetaTask& task,
                                       const std::vector<double>& sourcePerformance,
                                       const std::vector<double>& targetPerformance) const = 0;
    virtual std::vector<double> adaptParameters(const std::vector<double>& sourceParams,
                                               const std::vector<double>& adaptationFactors) const = 0;
};

// Feature-based transfer
class FeatureBasedTransfer : public TransferLearningMethod {
public:
    double computeTransferValue(const MetaTask& task,
                               const std::vector<double>& sourcePerformance,
                               const std::vector<double>& targetPerformance) const override {
        if (sourcePerformance.empty() || targetPerformance.empty()) {
            return 0.0;
        }
        
        // Compute similarity between source and target performance patterns
        double correlation = 0.0;
        double sourceMean = std::accumulate(sourcePerformance.begin(), sourcePerformance.end(), 0.0) / sourcePerformance.size();
        double targetMean = std::accumulate(targetPerformance.begin(), targetPerformance.end(), 0.0) / targetPerformance.size();
        
        double numerator = 0.0, denomSource = 0.0, denomTarget = 0.0;
        for (size_t i = 0; i < sourcePerformance.size(); ++i) {
            double sourceDev = sourcePerformance[i] - sourceMean;
            double targetDev = (i < targetPerformance.size()) ? targetPerformance[i] - targetMean : 0.0;
            numerator += sourceDev * targetDev;
            denomSource += sourceDev * sourceDev;
            if (i < targetPerformance.size()) {
                denomTarget += targetDev * targetDev;
            }
        }
        
        if (denomSource > 0 && denomTarget > 0) {
            correlation = numerator / std::sqrt(denomSource * denomTarget);
        }
        
        return (correlation + 1.0) / 2.0; // Normalize to [0, 1]
    }
    
    std::vector<double> adaptParameters(const std::vector<double>& sourceParams,
                                        const std::vector<double>& adaptationFactors) const override {
        std::vector<double> adaptedParams = sourceParams;
        for (size_t i = 0; i < std::min(sourceParams.size(), adaptationFactors.size()); ++i) {
            adaptedParams[i] = sourceParams[i] * (1.0 + adaptationFactors[i]);
        }
        return adaptedParams;
    }
};

// Meta-learning adapter
class MetaLearningAdapter {
private:
    std::vector<MetaTask> metaTasks;
    std::unique_ptr<TransferLearningMethod> transferMethod;
    std::unordered_map<std::string, std::vector<double>> metaKnowledge;
    std::unordered_map<std::string, double> adaptationHistory;
    std::vector<double> performanceHistory;
    double learningRate;
    double transferThreshold;
    
public:
    MetaLearningAdapter(
        std::unique_ptr<TransferLearningMethod> transfer,
        double learningRate = 0.01,
        double transferThreshold = 0.5)
        : transferMethod(std::move(transfer)), learningRate(learningRate),
          transferThreshold(transferThreshold) {}
    
    void addMetaTask(const std::string& id, const std::string& description,
                    const std::string& sourceDomain, const std::string& targetDomain) {
        metaTasks.emplace_back(id, description, sourceDomain, targetDomain);
    }
    
    void updateMetaPerformance(const std::string& taskId, const std::vector<double>& performanceData) {
        if (metaKnowledge.find(taskId) == metaKnowledge.end()) {
            metaKnowledge[taskId] = performanceData;
        } else {
            // Update meta-knowledge with new performance
            std::vector<double>& knowledge = metaKnowledge[taskId];
            for (size_t i = 0; i < std::min(performanceData.size(), knowledge.size()); ++i) {
                knowledge[i] = knowledge[i] * (1.0 - learningRate) + performanceData[i] * learningRate;
            }
        }
    
        // Compute transfer value to related tasks
        for (const auto& task : metaTasks) {
            if (std::find(task.relatedTasks.begin(), task.relatedTasks.end(), taskId) != task.relatedTasks.end()) {
                if (metaKnowledge.find(taskId) != metaKnowledge.end()) {
                    auto& sourcePerf = metaKnowledge[taskId];
                    auto& targetPerf = metaKnowledge[task.id];
                    task.transferValue = transferMethod->computeTransferValue(task, sourcePerf, targetPerf);
                }
            }
        }
    }
    
    std::vector<double> adaptToNewTask(const std::string& taskId,
                                       const std::vector<double>& sourceParams,
                                       const Config& config) {
        std::vector<double> adaptationFactors;
        
        // Find related meta-tasks
        for (const auto& task : metaTasks) {
            if (task.id == taskId || 
                std::find(task.relatedTasks.begin(), task.relatedTasks.end(), taskId) != task.relatedTasks.end()) {
                
                if (metaKnowledge.find(task.id) != metaKnowledge.end()) {
                    double transferValue = task.transferValue;
                    if (transferValue >= transferThreshold) {
                        // Use transfer learning to adapt parameters
                        double adaptationFactor = transferValue * task.adaptationRate;
                        adaptationFactors.push_back(adaptationFactor);
                    }
                }
            }
        }
        
        if (adaptationFactors.empty()) {
            return sourceParams; // No adaptation possible
        }
        
        double avgAdaptation = std::accumulate(adaptationFactors.begin(), adaptationFactors.end(), 0.0) / adaptationFactors.size();
        return transferMethod->adaptParameters(sourceParams, std::vector<double>(sourceParams.size(), avgAdaptation));
    }
    
    void recordAdaptation(const std::string& taskId, double adaptationSpeed) {
        adaptationHistory[taskId] = adaptationSpeed;
        performanceHistory.push_back(adaptationSpeed);
        if (performanceHistory.size() > 1000) {
            performanceHistory.erase(performanceHistory.begin());
        }
    }
    
    double getAverageTransferValue() const {
        double totalValue = 0.0;
        int count = 0;
        for (const auto& task : metaTasks) {
            totalValue += task.transferValue;
            count++;
        }
        return count > 0 ? totalValue / count : 0.0;
    }
    
    double getMetaLearningConvergence() const {
        if (performanceHistory.size() < 10) return 0.0;
        
        double recentAvg = std::accumulate(performanceHistory.end() - 10, performanceHistory.end(), 0.0) / 10.0;
        double overallAvg = std::accumulate(performanceHistory.begin(), performanceHistory.end(), 0.0) / performanceHistory.size();
        
        return 1.0 - std::abs(recentAvg - overallAvg);
    }
    
    std::string getMetaLearningSummary() const {
        std::stringstream ss;
        ss << "Meta-Learning Summary:\n";
        ss << "  Average Transfer Value: " << getAverageTransferValue() << "\n";
        ss << "  Convergence Rate: " << getMetaLearningConvergence() << "\n";
        ss << "  Number of Meta-Tasks: " << metaTasks.size() << "\n";
        ss << "  Adaptation History Size: " << adaptationHistory.size() << "\n";
        return ss.str();
    }
    
    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for saving meta-learning data");
        }
        
        file << "Meta-Learning Data:\n";
        file << "Learning Rate: " << learningRate << "\n";
        file << "Transfer Threshold: " << transferThreshold << "\n";
        file << "Number of Tasks: " << metaTasks.size() << "\n";
        
        for (const auto& task : metaTasks) {
            file << "Task: " << task.id << "\n";
            file << "  Description: " << task.description << "\n";
            file << "  Source Domain: " << task.sourceDomain << "\n";
            file << "  Target Domain: " << task.targetDomain << "\n";
            file << "  Transfer Value: " << task.transferValue << "\n";
            file << "  Adaptation Rate: " << task.adaptationRate << "\n";
            file << "  Learned: " << (task.learned ? "true" : "false") << "\n";
            file << "  Performance: " << task.performance << "\n";
            file << "\n";
        }
        
        file.close();
    }
    
    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for loading meta-learning data");
        }
        
        // Implementation for loading meta-learning data
        file.close();
    }
};

class AdvancedMetaLearning {
private:
    std::unique_ptr<MetaLearningAdapter> adapter;
    std::unordered_map<std::string, std::vector<double>> sourceDomainPerformance;
    std::unordered_map<std::string, std::vector<double>> targetDomainPerformance;
    std::vector<double> transferHistory;
    double overallTransferEffectiveness;
    
public:
    AdvancedMetaLearning(const Config& config) {
        auto transferMethod = std::make_unique<FeatureBasedTransfer>();
        
        double learningRate = config.getOr("meta_learning.learning_rate", 0.01);
        double transferThreshold = config.getOr("meta_learning.transfer_threshold", 0.5);
        
        adapter = std::make_unique<MetaLearningAdapter>(
            std::move(transferMethod), learningRate, transferThreshold);
        
        overallTransferEffectiveness = config.getOr("meta_learning.overall_effectiveness", 0.5);
    }
    
    void addMetaTask(const std::string& id, const std::string& description,
                    const std::string& sourceDomain, const std::string& targetDomain) {
        adapter->addMetaTask(id, description, sourceDomain, targetDomain);
    }
    
    void recordSourcePerformance(const std::string& domain, const std::vector<double>& performance) {
        sourceDomainPerformance[domain] = performance;
        
        // Update related meta-tasks
        for (const auto& task : adapter->getMetaTasks()) {
            if (task.sourceDomain == domain) {
                adapter->updateMetaPerformance(task.id, performance);
            }
        }
    }
    
    void recordTargetPerformance(const std::string& domain, const std::vector<double>& performance) {
        targetDomainPerformance[domain] = performance;
    }
    
    std::vector<double> adaptToNewDomain(const std::string& targetDomain,
                                         const std::vector<double>& sourceParams,
                                         const Config& config) {
        return adapter->adaptToNewTask(targetDomain, sourceParams, config);
    }
    
    void recordAdaptation(const std::string& taskId, double speed) {
        adapter->recordAdaptation(taskId, speed);
        transferHistory.push_back(speed);
        if (transferHistory.size() > 1000) {
            transferHistory.erase(transferHistory.begin());
        }
        
        // Update overall effectiveness
        if (!transferHistory.empty()) {
            double avgTransfer = std::accumulate(transferHistory.begin(), transferHistory.end(), 0.0) / transferHistory.size();
            overallTransferEffectiveness = overallTransferEffectiveness * 0.9 + avgTransfer * 0.1;
        }
    }
    
    double getOverallTransferEffectiveness() const {
        return overallTransferEffectiveness;
    }
    
    std::string getMetaLearningStatus() const {
        std::stringstream ss;
        ss << "Overall Transfer Effectiveness: " << overallTransferEffectiveness << "\n";
        ss << adapter->getMetaLearningSummary();
        return ss.str();
    }
    
    void saveState(const Config& config) const {
        std::string filename = config.getOr("meta_learning.state_file", "meta_learning_state.json");
        adapter->saveToFile(filename);
    }
    
    void loadState(const Config& config) {
        std::string filename = config.getOr("meta_learning.state_file", "meta_learning_state.json");
        adapter->loadFromFile(filename);
    }
};

} // namespace nlm
