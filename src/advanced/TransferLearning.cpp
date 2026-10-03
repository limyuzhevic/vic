// ====================================================================
// TRANSFER LEARNING IMPLEMENTATION
// ====================================================================

#include "advanced/TransferLearning.h"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

void TransferLearning::adaptDomain(std::shared_ptr<Brain> targetBrain,
                                   const std::map<std::string, double>& sourceDomainData,
                                   const std::map<std::string, double>& adaptationParams) {
    auto config = std::make_shared<Config>();
    
    // Adapt target brain configuration based on source domain data
    if (sourceDomainData.count("source_neuron_count")) {
        config->set("neuron_count", static_cast<int64_t>(sourceDomainData.at("source_neuron_count")), ConfigSource::Runtime);
    }
    if (sourceDomainData.count("source_connection_probability")) {
        config->set("connection_probability", sourceDomainData.at("source_connection_probability"), ConfigSource::Runtime);
    }
    if (sourceDomainData.count("source_learning_rate")) {
        config->set("learning_rate", sourceDomainData.at("source_learning_rate"), ConfigSource::Runtime);
    }
    
    // Apply adaptation parameters
    for (const auto& param : adaptationParams) {
        config->set(param.first, param.second, ConfigSource::Runtime);
    }
    
    targetBrain->configure(config);
}

void TransferLearning::enableTransferLearning(std::shared_ptr<Brain> targetBrain,
                                               const std::vector<SourceDomain>& sourceDomains,
                                               const std::map<std::string, double>& transferParams) {
    // Calculate ensemble weights for source domains
    std::vector<double> weights;
    double totalSimilarity = 0.0;
    
    for (const auto& domain : sourceDomains) {
        double similarity = 0.0;
        if (domain.domainFeatures.count("similarity_to_target")) {
            similarity = domain.domainFeatures.at("similarity_to_target");
        }
        totalSimilarity += similarity;
        weights.push_back(similarity);
    }
    
    if (totalSimilarity > 0) {
        for (auto& weight : weights) {
            weight /= totalSimilarity;
        }
    } else {
        // Equal weights if no similarity data
        double equalWeight = 1.0 / sourceDomains.size();
        weights.assign(sourceDomains.size(), equalWeight);
    }
    
    // Combine source domain knowledge
    auto config = std::make_shared<Config>();
    
    // Aggregate neuron counts
    size_t totalNeurons = 0;
    for (size_t i = 0; i < sourceDomains.size(); ++i) {
        if (sourceDomains[i].sourceBrain) {
            totalNeurons += sourceDomains[i].sourceBrain->getTotalNeuronCount() * weights[i];
        }
    }
    config->set("neuron_count", static_cast<int64_t>(totalNeurons), ConfigSource::Runtime);
    
    // Aggregate connection probabilities
    double avgConnectionProb = 0.0;
    for (size_t i = 0; i < sourceDomains.size(); ++i) {
        if (sourceDomains[i].sourceBrain) {
            avgConnectionProb += sourceDomains[i].sourceBrain->getConnectionProbability() * weights[i];
        }
    }
    config->set("connection_probability", avgConnectionProb, ConfigSource::Runtime);
    
    // Apply transfer parameters
    for (const auto& param : transferParams) {
        config->set(param.first, param.second, ConfigSource::Runtime);
    }
    
    targetBrain->configure(config);
}

 double TransferLearning::transferKnowledge(std::shared_ptr<Brain> sourceBrain,
                                           std::shared_ptr<Brain> targetBrain,
                                           const std::map<std::string, double>& transferConfig) {
    double transferGain = 0.0;
    
    // Transfer synaptic weights
    auto sourceRegion = sourceBrain->getRegion(RegionId(1));
    auto targetRegion = targetBrain->getRegion(RegionId(1));
    
    if (sourceRegion && targetRegion) {
        auto sourceSynapses = sourceRegion->getSynapses();
        auto targetSynapses = targetRegion->getSynapses();
        
        // Transfer based on configuration
        double transferStrength = transferConfig.count("transfer_strength") ? 
            transferConfig.at("transfer_strength") : 0.5;
        
        // Find matching neurons and transfer weights
        for (size_t i = 0; i < std::min(sourceSynapses.size(), targetSynapses.size()); ++i) {
            double sourceWeight = sourceSynapses[i]->getWeight();
            double targetWeight = targetSynapses[i]->getWeight();
            
            // Apply transfer
            double transferredWeight = sourceWeight * transferStrength;
            targetSynapses[i]->setWeight(transferredWeight);
            
            transferGain += std::abs(transferredWeight - targetWeight);
        }
    }
    
    return transferGain;
}

 double TransferLearning::measureDomainSimilarity(const std::map<std::string, double>& sourceFeatures,
                                                  const std::map<std::string, double>& targetFeatures) {
    double similarity = 0.0;
    
    // Compare feature vectors
    std::vector<std::string> allFeatures;
    for (const auto& pair : sourceFeatures) allFeatures.push_back(pair.first);
    for (const auto& pair : targetFeatures) allFeatures.push_back(pair.first);
    
    // Remove duplicates
    std::sort(allFeatures.begin(), allFeatures.end());
    allFeatures.erase(std::unique(allFeatures.begin(), allFeatures.end()), allFeatures.end());
    
    // Calculate cosine similarity
    double dotProduct = 0.0;
    double sourceNorm = 0.0;
    double targetNorm = 0.0;
    
    for (const auto& feature : allFeatures) {
        double sourceVal = sourceFeatures.count(feature) ? sourceFeatures.at(feature) : 0.0;
        double targetVal = targetFeatures.count(feature) ? targetFeatures.at(feature) : 0.0;
        
        dotProduct += sourceVal * targetVal;
        sourceNorm += sourceVal * sourceVal;
        targetNorm += targetVal * targetVal;
    }
    
    if (sourceNorm > 0 && targetNorm > 0) {
        similarity = dotProduct / (std::sqrt(sourceNorm) * std::sqrt(targetNorm));
    }
    
    return similarity;
}

 std::map<std::string, double> TransferLearning::evaluateTaskTransfer(
    const std::vector<TransferTask>& transferTasks,
    const std::map<std::string, double>& evaluationCriteria) {
    std::map<std::string, double> results;
    
    double totalTransferScore = 0.0;
    double totalExpectedOutcome = 0.0;
    double totalActualOutcome = 0.0;
    
    for (const auto& task : transferTasks) {
        double taskScore = task.expectedOutcome > 0 ? 
            task.actualOutcome / task.expectedOutcome : 0.0;
        
        totalTransferScore += taskScore;
        totalExpectedOutcome += task.expectedOutcome;
        totalActualOutcome += task.actualOutcome;
    }
    
    if (!transferTasks.empty()) {
        results["average_transfer_score"] = totalTransferScore / transferTasks.size();
        results["overall_transfer_efficiency"] = totalActualOutcome / totalExpectedOutcome;
        results["transfer_success_rate"] = (totalActualOutcome > 0) ? 1.0 : 0.0;
    }
    
    return results;
}

 std::shared_ptr<Brain> TransferLearning::createGeneralizableBrain(
    const std::vector<SourceDomain>& sourceDomains,
    const std::map<std::string, double>& generalizableParams) {
    auto config = std::make_shared<Config>();
    
    // Create a brain with generalized capabilities
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.2f, ConfigSource::Default);
    
    // Apply generalizable parameters
    for (const auto& param : generalizableParams) {
        config->set(param.first, param.second, ConfigSource::Runtime);
    }
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    return brain;
}

 std::vector<std::string> TransferLearning::detectNegativeTransfer(
    const std::vector<TransferTask>& transferTasks,
    double negativeTransferThreshold) {
    std::vector<std::string> negativeTasks;
    
    for (const auto& task : transferTasks) {
        if (task.actualOutcome < task.expectedOutcome * negativeTransferThreshold) {
            negativeTasks.push_back(task.taskId);
        }
    }
    
    return negativeTasks;
}

 void TransferLearning::enableMultiSourceTransferLearning(
    std::shared_ptr<Brain> targetBrain,
    const std::vector<SourceDomain>& sourceDomains,
    const std::map<std::string, double>& ensembleParams) {
    // Weighted ensemble of source domains
    std::vector<double> weights;
    double totalWeight = 0.0;
    
    for (const auto& domain : sourceDomains) {
        double weight = domain.domainFeatures.count("ensemble_weight") ? 
            domain.domainFeatures.at("ensemble_weight") : 1.0;
        weights.push_back(weight);
        totalWeight += weight;
    }
    
    if (totalWeight > 0) {
        for (auto& weight : weights) {
            weight /= totalWeight;
        }
    } else {
        double equalWeight = 1.0 / sourceDomains.size();
        weights.assign(sourceDomains.size(), equalWeight);
    }
    
    // Create combined configuration
    auto config = std::make_shared<Config>();
    
    // Aggregate source domain parameters
    double avgNeuronCount = 0.0;
    double avgConnectionProb = 0.0;
    double avgLearningRate = 0.0;
    
    for (size_t i = 0; i < sourceDomains.size(); ++i) {
        if (sourceDomains[i].sourceBrain) {
            avgNeuronCount += sourceDomains[i].sourceBrain->getTotalNeuronCount() * weights[i];
            avgConnectionProb += sourceDomains[i].sourceBrain->getConnectionProbability() * weights[i];
            avgLearningRate += sourceDomains[i].sourceBrain->getLearningRate() * weights[i];
        }
    }
    
    config->set("neuron_count", static_cast<int64_t>(avgNeuronCount), ConfigSource::Runtime);
    config->set("connection_probability", avgConnectionProb, ConfigSource::Runtime);
    config->set("learning_rate", avgLearningRate, ConfigSource::Runtime);
    
    // Apply ensemble parameters
    for (const auto& param : ensembleParams) {
        config->set(param.first, param.second, ConfigSource::Runtime);
    }
    
    targetBrain->configure(config);
}

 std::map<std::string, double> TransferLearning::learnDomainInvariantFeatures(
    const std::vector<SourceDomain>& sourceDomains,
    const std::string& featureType) {
    std::map<std::string, double> invariantFeatures;
    
    // Learn features that are consistent across domains
    std::map<std::string, std::vector<double>> featureValues;
    
    for (const auto& domain : sourceDomains) {
        for (const auto& feature : domain.domainFeatures) {
            featureValues[feature.first].push_back(feature.second);
        }
    }
    
    // Calculate mean and variance for each feature
    for (auto& pair : featureValues) {
        const auto& values = pair.second;
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        double mean = sum / values.size();
        
        double sqSum = 0.0;
        for (double val : values) {
            sqSum += (val - mean) * (val - mean);
        }
        double variance = sqSum / values.size();
        
        invariantFeatures[pair.first + "_mean"] = mean;
        invariantFeatures[pair.first + "_stddev"] = std::sqrt(variance);
        
        // If variance is low, feature is more likely to be invariant
        if (variance < 0.01) {
            invariantFeatures[pair.first + "_invariant"] = 1.0;
        }
    }
    
    return invariantFeatures;
}

 std::map<std::string, double> TransferLearning::optimizeTransferParameters(
    const std::vector<TransferTask>& transferTasks,
    const std::map<std::string, double>& currentParams) {
    std::map<std::string, double> optimizedParams = currentParams;
    
    // Simple gradient-based optimization (simplified)
    double learningRate = optimizedParams.count("learning_rate") ? optimizedParams.at("learning_rate") : 0.01;
    double regularizationStrength = optimizedParams.count("regularization_strength") ? 
        optimizedParams.at("regularization_strength") : 0.01;
    
    // Calculate gradients based on transfer tasks
    std::map<std::string, double> gradients;
    for (const auto& task : transferTasks) {
        double gradient = (task.actualOutcome - task.expectedOutcome) / std::max(task.expectedOutcome, 1.0);
        gradients["transfer_strength"] += gradient;
        gradients["adaptation_rate"] += gradient * 0.5;
    }
    
    // Update parameters
    if (gradients.count("transfer_strength")) {
        optimizedParams["transfer_strength"] = std::max(0.0, optimizedParams["transfer_strength"] - 
                                                     learningRate * gradients["transfer_strength"] - 
                                                     regularizationStrength * optimizedParams["transfer_strength"]);
    }
    
    if (gradients.count("adaptation_rate")) {
        optimizedParams["adaptation_rate"] = std::max(0.0, optimizedParams["adaptation_rate"] - 
                                                   learningRate * gradients["adaptation_rate"] - 
                                                   regularizationStrength * optimizedParams["adaptation_rate"]);
    }
    
    return optimizedParams;
}

 std::shared_ptr<Brain> TransferLearning::createTaskAgnosticLearner(
    const std::vector<SourceDomain>& sourceDomains,
    const std::map<std::string, double>& taskAgnosticParams) {
    auto config = std::make_shared<Config>();
    
    // Create a task-agnostic learner with balanced capabilities
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("learning_rate", 0.01, ConfigSource::Default);
    
    // Apply task-agnostic parameters
    for (const auto& param : taskAgnosticParams) {
        config->set(param.first, param.second, ConfigSource::Runtime);
    }
    
    // Add meta-learning capabilities
    config->set("enable_meta_learning", true, ConfigSource::Runtime);
    config->set("meta_learning_rate", 0.001, ConfigSource::Runtime);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    return brain;
}

 double TransferLearning::zeroShotTransfer(
    std::shared_ptr<Brain> sourceBrain,
    const std::string& targetTaskDescription,
    const std::map<std::string, double>& zeroShotParams) {
    double transferScore = 0.0;
    
    // Zero-shot transfer based on task description analysis
    // Simplified: analyze task description keywords and map to source brain capabilities
    
    std::map<std::string, double> taskKeywords = {
        {"classification", 0.8},
        {"recognition", 0.7},
        {"prediction", 0.6},
        {"detection", 0.5},
        {"generation", 0.4},
        {"analysis", 0.3}
    };
    
    // Simple keyword matching (simplified)
    for (const auto& pair : taskKeywords) {
        if (targetTaskDescription.find(pair.first) != std::string::npos) {
            transferScore += pair.second * 0.1;
        }
    }
    
    // Apply transfer parameters
    double transferStrength = zeroShotParams.count("transfer_strength") ? 
        zeroShotParams.at("transfer_strength") : 0.5;
    transferScore *= transferStrength;
    
    return std::min(transferScore, 1.0);
}

 std::map<std::string, double> TransferLearning::fewShotTransfer(
    std::shared_ptr<Brain> sourceBrain,
    const std::vector<TransferTask>& fewShotTasks,
    int supportSetSize,
    const std::map<std::string, double>& fewShotParams) {
    std::map<std::string, double> results;
    
    if (fewShotTasks.empty()) {
        return results;
    }
    
    // Few-shot transfer: use small examples to adapt to new task
    double totalTransferScore = 0.0;
    int validTasks = 0;
    
    for (const auto& task : fewShotTasks) {
        if (validTasks < supportSetSize) {
            // Use as support example
            double supportTransfer = transferKnowledge(sourceBrain, sourceBrain, std::map<std::string, double>{
                {"transfer_strength", 0.3}
            });
            totalTransferScore += supportTransfer;
            validTasks++;
        }
    }
    
    if (validTasks > 0) {
        results["average_support_transfer"] = totalTransferScore / validTasks;
        results["few_shot_efficiency"] = results["average_support_transfer"] * 0.8;
    }
    
    return results;
}

 std::map<std::string, double> TransferLearning::metaTransferLearning(
    const std::vector<TransferTask>& transferTasks,
    const std::map<std::string, double>& metaParams) {
    std::map<std::string, double> metaResults;
    
    // Meta-transfer learning: learn how to transfer across tasks
    double metaLearningRate = metaParams.count("meta_learning_rate") ? 
        metaParams.at("meta_learning_rate") : 0.01;
    
    // Calculate meta-learning metrics
    double taskDiversity = 0.0;
    double transferConsistency = 0.0;
    
    if (!transferTasks.empty()) {
        // Calculate diversity of transfer tasks
        std::set<std::string> taskTypes;
        for (const auto& task : transferTasks) {
            taskTypes.insert(task.taskType);
        }
        taskDiversity = static_cast<double>(taskTypes.size()) / transferTasks.size();
        
        // Calculate consistency of transfer outcomes
        double meanOutcome = 0.0;
        for (const auto& task : transferTasks) {
            meanOutcome += task.actualOutcome;
        }
        meanOutcome /= transferTasks.size();
        
        double variance = 0.0;
        for (const auto& task : transferTasks) {
            variance += (task.actualOutcome - meanOutcome) * (task.actualOutcome - meanOutcome);
        }
        variance /= transferTasks.size();
        transferConsistency = 1.0 - std::sqrt(variance);
    }
    
    metaResults["task_diversity"] = taskDiversity;
    metaResults["transfer_consistency"] = transferConsistency;
    metaResults["meta_learning_progress"] = metaLearningRate * transferConsistency;
    
    return metaResults;
}

 void TransferLearning::enableContinualTransferLearning(
    std::shared_ptr<Brain> brain,
    const std::vector<SourceDomain>& sourceDomains,
    const std::map<std::string, double>& continualParams,
    double forgettingPreventionStrength) {
    
    // Continual transfer learning with forgetting prevention
    enableTransferLearning(brain, sourceDomains, continualParams);
    
    // Apply forgetting prevention
    auto config = std::make_shared<Config>();
    config->set("enable_forgetting_prevention", true, ConfigSource::Runtime);
    config->set("forgetting_prevention_strength", forgettingPreventionStrength, ConfigSource::Runtime);
    
    brain->configure(config);
}

 std::map<std::string, double> TransferLearning::monitorTransferLearningPerformance(
    const std::vector<TransferTask>& transferTasks,
    const std::map<std::string, double>& performanceMetrics) {
    std::map<std::string, double> monitoringResults;
    
    // Monitor various aspects of transfer learning performance
    double overallPerformance = 0.0;
    int taskCount = 0;
    
    for (const auto& task : transferTasks) {
        double taskPerformance = task.actualOutcome / std::max(task.expectedOutcome, 1.0);
        
        // Incorporate performance metrics
        if (performanceMetrics.count("adaptation_rate")) {
            taskPerformance *= performanceMetrics.at("adaptation_rate");
        }
        if (performanceMetrics.count("transfer_efficiency")) {
            taskPerformance *= performanceMetrics.at("transfer_efficiency");
        }
        
        overallPerformance += taskPerformance;
        taskCount++;
    }
    
    if (taskCount > 0) {
        monitoringResults["average_performance"] = overallPerformance / taskCount;
        monitoringResults["transfer_efficiency"] = monitoringResults["average_performance"] * 0.9;
        monitoringResults["adaptation_effectiveness"] = monitoringResults["average_performance"] * 0.8;
        monitoringResults["generalization_ability"] = monitoringResults["average_performance"] * 0.7;
    }
    
    return monitoringResults;
}

 std::map<std::string, double> TransferLearning::alignDomains(
    const std::vector<SourceDomain>& sourceDomains,
    const std::string& alignmentMethod) {
    std::map<std::string, double> alignmentResults;
    
    if (sourceDomains.empty()) {
        return alignmentResults;
    }
    
    if (alignmentMethod == "feature_matching") {
        // Feature matching alignment (simplified)
        double totalSimilarity = 0.0;
        int comparisonCount = 0;
        
        for (size_t i = 0; i < sourceDomains.size(); ++i) {
            for (size_t j = i + 1; j < sourceDomains.size(); ++j) {
                double similarity = measureDomainSimilarity(
                    sourceDomains[i].domainFeatures,
                    sourceDomains[j].domainFeatures);
                totalSimilarity += similarity;
                comparisonCount++;
            }
        }
        
        if (comparisonCount > 0) {
            alignmentResults["average_domain_similarity"] = totalSimilarity / comparisonCount;
            alignmentResults["alignment_quality"] = alignmentResults["average_domain_similarity"];
        }
    } else if (alignmentMethod == "distribution_matching") {
        // Distribution matching (simplified)
        alignmentResults["distribution_alignment"] = 0.85; // Placeholder value
    }
    
    return alignmentResults;
}

 std::map<std::string, double> TransferLearning::predictTransferSuccess(
    const std::vector<TransferTask>& transferTasks,
    const std::map<std::string, double>& predictionFeatures) {
    std::map<std::string, double> predictionResults;
    
    // Predict transfer learning success based on task characteristics
    double successProbability = 0.0;
    
    for (const auto& task : transferTasks) {
        // Simple prediction based on task difficulty and source-target similarity
        double taskDifficulty = 1.0 - (task.actualOutcome / std::max(task.expectedOutcome, 1.0));
        double similarityScore = task.taskFeatures.count("similarity_to_source") ? 
            task.taskFeatures.at("similarity_to_source") : 0.5;
        
        double taskSuccessProbability = similarityScore * (1.0 - taskDifficulty);
        successProbability += taskSuccessProbability;
    }
    
    if (!transferTasks.empty()) {
        successProbability /= transferTasks.size();
        predictionResults["success_probability"] = successProbability;
        predictionResults["expected_transfer_gain"] = successProbability * 10.0; // Scaling factor
        predictionResults["reliable_transfer"] = successProbability > 0.7 ? 1.0 : 0.0;
    }
    
    return predictionResults;
}

} // namespace nlm
