#pragma once

#include "TaskInference.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <random>
#include <numeric>
#include <limits>
#include <unordered_map>

namespace nlm {

// TaskInference implementation
struct TaskInference::Impl {
    // Task representation and learning
    std::map<std::string, std::vector<float>> taskParameters;
    std::map<std::string, std::vector<std::vector<float>>> taskExperienceHistory;
    std::map<std::string, std::vector<float>> taskSuccessHistory;
    std::map<std::string, std::vector<float>> taskFailureHistory;
    
    // Task statistics
    std::map<std::string, float> taskSuccessRates;
    std::map<std::string, float> taskAverageRewards;
    std::map<std::string, float> taskVariances;
    std::map<std::string, int> taskExperienceCounts;
    
    // Task similarity and transfer
    std::map<std::string, std::vector<std::string>> taskSimilarities;
    std::map<std::string, std::vector<float>> taskTransferabilityScores;
    
    // Machine learning models for task inference
    std::map<std::string, std::vector<float>> neuralNetworkWeights;
    std::map<std::string, std::vector<float>> taskEmbeddings;
    
    // Random number generation
    std::mt19937 rng{std::random_device{}()};
    std::normal_distribution<float> normalDist{0.0f, 1.0f};
    std::uniform_real_distribution<float> uniformDist{0.0f, 1.0f};
};

TaskInference::TaskInference() {
    pImpl = std::make_unique<Impl>();
    
    // Initialize neural network weights for task inference
    std::normal_distribution<float> dist(0.0f, 0.1f);
    
    // Initialize a default task for the neural network
    std::vector<float> defaultWeights(64, 0.0f);
    for (auto& weight : defaultWeights) {
        weight = dist(pImpl->rng);
    }
    pImpl->neuralNetworkWeights["task_inference_nn"] = defaultWeights;
}

TaskInference::~TaskInference() {}

std::string TaskInference::inferTask(const std::vector<float>& experience) const {
    if (experience.empty()) {
        return "unknown_task";
    }
    
    // Find the most similar task based on experience
    std::string bestTask = "unknown_task";
    float bestSimilarity = -std::numeric_limits<float>::max();
    
    for (const auto& pair : pImpl->taskHistory) {
        const auto& taskHistory = pair.second;
        if (taskHistory.empty()) {
            continue;
        }
        
        // Compute similarity between current experience and task history
        float similarity = computeExperienceSimilarity(experience, taskHistory);
        
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestTask = pair.first;
        }
    }
    
    // If no similar task found, create a new task
    if (bestSimilarity < 0.5f) {
        bestTask = "new_task_" + std::to_string(pImpl->taskHistory.size());
    }
    
    return bestTask;
}

void TaskInference::learnTask(const std::string& taskId, const std::vector<float>& experience, float reward) {
    // Add experience to task history
    pImpl->taskHistory[taskId].push_back(experience);
    
    // Trim history if too large
    if (pImpl->taskHistory[taskId].size() > 1000) {
        pImpl->taskHistory[taskId].erase(pImpl->taskHistory[taskId].begin());
    }
    
    // Update task parameters
    updateTaskParameters(taskId, experience, reward);
    
    // Update task statistics
    updateTaskStatistics(taskId, reward);
    
    // Update task similarity and transferability
    updateTaskSimilarity(taskId);
}

float TaskInference::predictPerformance(const std::string& taskId, const std::vector<float>& experience) const {
    auto taskParamsIt = pImpl->taskParameters.find(taskId);
    auto successRateIt = pImpl->taskSuccessRates.find(taskId);
    
    if (taskParamsIt == pImpl->taskParameters.end() || successRateIt == pImpl->taskSuccessRates.end()) {
        return 0.0f; // Task not known
    }
    
    const auto& taskParams = taskParamsIt->second;
    float successRate = successRateIt->second;
    
    // Compute similarity between current experience and learned task parameters
    float similarity = computeParameterSimilarity(experience, taskParams);
    
    // Predict performance based on similarity and historical success
    float predictedPerformance = similarity * successRate;
    
    // Add noise to prediction based on task variance
    auto varianceIt = pImpl->taskVariances.find(taskId);
    if (varianceIt != pImpl->taskVariances.end()) {
        float noise = normalDist(pImpl->rng) * std::sqrt(varianceIt->second);
        predictedPerformance += noise;
    }
    
    // Ensure performance is within reasonable bounds
    predictedPerformance = std::max(0.0f, std::min(1.0f, predictedPerformance));
    
    return predictedPerformance;
}

float TaskInference::getTaskSuccessRate(const std::string& taskId) const {
    auto it = pImpl->taskSuccessRates.find(taskId);
    if (it != pImpl->taskSuccessRates.end()) {
        return it->second;
    }
    return 0.0f;
}

float TaskInference::getTaskGeneralizationScore(const std::string& sourceTask, const std::string& targetTask) const {
    auto sourceParamsIt = pImpl->taskParameters.find(sourceTask);
    auto targetParamsIt = pImpl->taskParameters.find(targetTask);
    
    if (sourceParamsIt == pImpl->taskParameters.end() || targetParamsIt == pImpl->taskParameters.end()) {
        return 0.0f;
    }
    
    const auto& sourceParams = sourceParamsIt->second;
    const auto& targetParams = targetParamsIt->second;
    
    // Compute parameter similarity for transfer learning
    float similarity = computeParameterSimilarity(targetParams, sourceParams);
    
    // Get transferability score
    auto transferIt = pImpl->taskTransferabilityScores.find(sourceTask);
    if (transferIt != pImpl->taskTransferabilityScores.end()) {
        // Find corresponding target transferability score
        for (size_t i = 0; i < transferIt->second.size(); ++i) {
            if (transferIt->second[i] == similarity) {
                return transferIt->second[i]; // Return the transferability score
            }
        }
    }
    
    return similarity;
}

void TaskInference::updateTaskParameters(const std::string& taskId, const std::vector<float>& experience, float reward) {
    // Initialize task parameters if not exists
    if (pImpl->taskParameters.find(taskId) == pImpl->taskParameters.end()) {
        pImpl->taskParameters[taskId] = experience;
        return;
    }
    
    auto& taskParams = pImpl->taskParameters[taskId];
    
    // Update task parameters using a Hebbian-like learning rule
    for (size_t i = 0; i < std::min(experience.size(), taskParams.size()); ++i) {
        // Hebbian learning: strengthen connections that fire together
        float learningRate = 0.01f * reward;
        float delta = learningRate * (experience[i] - taskParams[i]);
        
        // Add noise for exploration
        float noise = normalDist(pImpl->rng) * 0.001f;
        delta += noise;
        
        taskParams[i] += delta;
    }
}

void TaskInference::updateTaskStatistics(const std::string& taskId, float reward) {
    // Update success rate
    auto& successHistory = pImpl->taskSuccessHistory[taskId];
    successHistory.push_back(reward > 0.0f ? 1.0f : 0.0f);
    
    if (successHistory.size() > 100) {
        successHistory.erase(successHistory.begin());
    }
    
    float successRate = std::accumulate(successHistory.begin(), successHistory.end(), 0.0f) / successHistory.size();
    pImpl->taskSuccessRates[taskId] = successRate;
    
    // Update average reward
    auto& rewardHistory = pImpl->taskHistory[taskId];
    float avgReward = std::accumulate(rewardHistory.begin(), rewardHistory.end(), 0.0f) / rewardHistory.size();
    pImpl->taskAverageRewards[taskId] = avgReward;
    
    // Update variance
    float variance = 0.0f;
    for (float r : rewardHistory) {
        variance += (r - avgReward) * (r - avgReward);
    }
    variance /= rewardHistory.size();
    pImpl->taskVariances[taskId] = variance;
    
    // Update experience count
    pImpl->taskExperienceCounts[taskId]++;
}

void TaskInference::updateTaskSimilarity(const std::string& taskId) {
    // Compute similarity with all other tasks
    pImpl->taskSimilarities[taskId].clear();
    
    for (const auto& otherTask : pImpl->taskHistory) {
        if (otherTask.first == taskId) {
            continue;
        }
        
        // Compute similarity based on experience
        float similarity = computeExperienceSimilarity(
            otherTask.second.back(),
            pImpl->taskHistory.at(taskId).back()
        );
        
        pImpl->taskSimilarities[taskId].push_back(otherTask.first);
        pImpl->taskTransferabilityScores[otherTask.first].push_back(similarity);
    }
}

float TaskInference::computeExperienceSimilarity(const std::vector<float>& exp1, const std::vector<float>& exp2) const {
    if (exp1.empty() || exp2.empty()) {
        return 0.0f;
    }
    
    size_t minSize = std::min(exp1.size(), exp2.size());
    if (minSize == 0) {
        return 0.0f;
    }
    
    // Cosine similarity
    float dotProduct = 0.0f;
    float norm1 = 0.0f, norm2 = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        dotProduct += exp1[i] * exp2[i];
        norm1 += exp1[i] * exp1[i];
        norm2 += exp2[i] * exp2[i];
    }
    
    if (norm1 == 0.0f || norm2 == 0.0f) {
        return 0.0f;
    }
    
    return dotProduct / std::sqrt(norm1 * norm2);
}

float TaskInference::computeParameterSimilarity(const std::vector<float>& params1, const std::vector<float>& params2) const {
    if (params1.empty() || params2.empty()) {
        return 0.0f;
    }
    
    size_t minSize = std::min(params1.size(), params2.size());
    if (minSize == 0) {
        return 0.0f;
    }
    
    // Pearson correlation coefficient
    float mean1 = std::accumulate(params1.begin(), params1.begin() + minSize, 0.0f) / minSize;
    float mean2 = std::accumulate(params2.begin(), params2.begin() + minSize, 0.0f) / minSize;
    
    float numerator = 0.0f, denominator1 = 0.0f, denominator2 = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        float diff1 = params1[i] - mean1;
        float diff2 = params2[i] - mean2;
        numerator += diff1 * diff2;
        denominator1 += diff1 * diff1;
        denominator2 += diff2 * diff2;
    }
    
    if (denominator1 == 0.0f || denominator2 == 0.0f) {
        return 0.0f;
    }
    
    return numerator / std::sqrt(denominator1 * denominator2);
}

} // namespace nlm