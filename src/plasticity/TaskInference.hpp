#pragma once

#include "MetaLearning.hpp"
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

// Task inference and generalization
class TaskInference {
public:
    TaskInference();
    ~TaskInference();
    
    // Infer task from experience
    std::string inferTask(const std::vector<float>& experience) const;
    
    // Learn task parameters from experience
    void learnTask(const std::string& taskId, const std::vector<float>& experience, float reward);
    
    // Predict performance on new task
    float predictPerformance(const std::string& taskId, const std::vector<float>& experience) const;
    
    // Get task statistics
    float getTaskSuccessRate(const std::string& taskId) const;
    float getTaskGeneralizationScore(const std::string& sourceTask, const std::string& targetTask) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    float computeExperienceSimilarity(const std::vector<float>& exp1, const std::vector<float>& exp2) const;
    float computeParameterSimilarity(const std::vector<float>& params1, const std::vector<float>& params2) const;
    void updateTaskParameters(const std::string& taskId, const std::vector<float>& experience, float reward);
    void updateTaskStatistics(const std::string& taskId, float reward);
    void updateTaskSimilarity(const std::string& taskId);
};

} // namespace nlm