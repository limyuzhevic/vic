#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../plasticity/PlasticityRule.hpp"

namespace nlm {

// Learning task
struct LearningTask {
    std::string id;
    std::string description;
    std::string type;
    double difficulty;
    double complexity;
    double duration;
    double reward;
    std::map<std::string, double> parameters;
    bool completed;
    int attempts;
    double performance;
    
    LearningTask(const std::string& taskId, const std::string& desc,
                 const std::string& taskType, double diff, double comp)
        : id(taskId), description(desc), type(taskType), difficulty(diff),
          complexity(comp), duration(0.0), reward(0.0), completed(false),
          attempts(0), performance(0.0) {}
};

// Adaptive curriculum policy
class AdaptiveCurriculumPolicy {
public:
    virtual ~AdaptiveCurriculumPolicy() = default;
    virtual double calculateDifficulty(const LearningTask& task, const Brain& brain,
                                       const Config& config) const = 0;
    virtual bool shouldAdvance(const std::vector<LearningTask>& tasks,
                              const Brain& brain) const = 0;
};

// Exponential difficulty policy
class ExponentialDifficultyPolicy : public AdaptiveCurriculumPolicy {
public:
    double calculateDifficulty(const LearningTask& task, const Brain& brain,
                               const Config& config) const override {
        double baseDifficulty = task.difficulty;
        double performanceFactor = 1.0 + brain.getGlobalExcitation() / 100.0;
        double complexityFactor = 1.0 + task.complexity / 10.0;
        
        return baseDifficulty * performanceFactor * complexityFactor;
    }
    
    bool shouldAdvance(const std::vector<LearningTask>& tasks,
                      const Brain& brain) const override {
        if (tasks.empty()) return true;
        
        double avgPerformance = 0.0;
        int completedCount = 0;
        
        for (const auto& task : tasks) {
            if (task.completed) {
                avgPerformance += task.performance;
                completedCount++;
            }
        }
        
        if (completedCount == 0) return false;
        
        avgPerformance /= completedCount;
        return avgPerformance >= 0.8; // 80% mastery threshold
    }
};

// Progressive complexity curriculum
class ProgressiveComplexityCurriculum {
private:
    std::vector<LearningTask> tasks;
    std::unique_ptr<AdaptiveCurriculumPolicy> policy;
    std::vector<double> difficultyProgression;
    size_t currentTaskIndex;
    double learningRate;
    double convergenceThreshold;
    
public:
    ProgressiveComplexityCurriculum(
        std::unique_ptr<AdaptiveCurriculumPolicy> curriculumPolicy,
        double learningRate = 0.01,
        double convergenceThreshold = 0.95)
        : policy(std::move(curriculumPolicy)), learningRate(learningRate),
          convergenceThreshold(convergenceThreshold), currentTaskIndex(0) {
        
        // Initialize difficulty progression based on task complexity
        difficultyProgression = {0.1, 0.3, 0.6, 0.8, 1.0};
    }
    
    void addTask(const std::string& id, const std::string& description,
                const std::string& type, double difficulty, double complexity) {
        tasks.emplace_back(id, description, type, difficulty, complexity);
    }
    
    void updateTaskPerformance(const std::string& taskId, double performance) {
        for (auto& task : tasks) {
            if (task.id == taskId) {
                task.performance = performance;
                if (performance >= convergenceThreshold) {
                    task.completed = true;
                }
                break;
            }
        }
    }
    
    std::string getNextTask(const Brain& brain, const Config& config) {
        if (currentTaskIndex >= tasks.size()) {
            return ""; // No more tasks
        }
        
        // Get current task
        auto& currentTask = tasks[currentTaskIndex];
        
        // Calculate adaptive difficulty
        double adaptiveDifficulty = policy->calculateDifficulty(
            currentTask, brain, config);
        
        return currentTask.id;
    }
    
    void completeTask(const std::string& taskId, const Brain& brain,
                     const Config& config) {
        for (auto& task : tasks) {
            if (task.id == taskId) {
                task.completed = true;
                task.attempts++;
                
                // Update brain based on performance
                double performance = task.performance;
                if (performance > 0.7) {
                    double learningStrength = config.getOr("curriculum.learning_strength", 0.1);
                    double weightChange = learningStrength * (performance - 0.5);
                }
                
                currentTaskIndex++;
                break;
            }
        }
    }
    
    bool isCompleted() const {
        return currentTaskIndex >= tasks.size();
    }
    
    double getProgress() const {
        if (tasks.empty()) return 1.0;
        return static_cast<double>(currentTaskIndex) / tasks.size();
    }
    
    std::vector<std::string> getCompletedTasks() const {
        std::vector<std::string> completed;
        for (const auto& task : tasks) {
            if (task.completed) {
                completed.push_back(task.id);
            }
        }
        return completed;
    }
    
    std::vector<std::string> getIncompleteTasks() const {
        std::vector<std::string> incomplete;
        for (size_t i = currentTaskIndex; i < tasks.size(); ++i) {
            incomplete.push_back(tasks[i].id);
        }
        return incomplete;
    }
    
    std::string getStatusSummary() const {
        std::stringstream ss;
        ss << "Progress: " << (getProgress() * 100.0) << "%\n";
        ss << "Tasks Completed: " << getCompletedTasks().size() << "\n";
        ss << "Current Phase: " << currentTaskIndex << "/" << tasks.size() << "\n";
        return ss.str();
    }
    
    void saveProgress(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for saving curriculum progress");
        }
        
        file << "Progress:\n";
        file << "CurrentTaskIndex: " << currentTaskIndex << "\n";
        file << "TotalTasks: " << tasks.size() << "\n";
        
        for (const auto& task : tasks) {
            file << "Task " << task.id << ":\n";
            file << "  Description: " << task.description << "\n";
            file << "  Type: " << task.type << "\n";
            file << "  Difficulty: " << task.difficulty << "\n";
            file << "  Complexity: " << task.complexity << "\n";
            file << "  Performance: " << task.performance << "\n";
            file << "  Completed: " << (task.completed ? "true" : "false") << "\n";
            file << "  Attempts: " << task.attempts << "\n";
            file << "\n";
        }
        
        file.close();
    }
    
    void loadProgress(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for loading curriculum progress");
        }
        
        // Implementation for loading progress
        file.close();
    }
};

class AdvancedCurriculumLearning {
private:
    std::unique_ptr<ProgressiveComplexityCurriculum> curriculum;
    std::vector<double> performanceHistory;
    std::unordered_map<std::string, double> taskMetrics;
    std::string currentTaskId;
    double convergenceRate;
    
public:
    AdvancedCurriculumLearning(const Config& config) {
        auto policy = std::make_unique<ExponentialDifficultyPolicy>();
        
        convergenceRate = config.getOr("curriculum.convergence_rate", 0.01);
        double learningRate = config.getOr("curriculum.learning_rate", 0.01);
        double convergenceThreshold = config.getOr("curriculum.convergence_threshold", 0.95);
        
        curriculum = std::make_unique<ProgressiveComplexityCurriculum>(
            std::move(policy), learningRate, convergenceThreshold);
    }
    
    void addLearningTask(const std::string& id, const std::string& description,
                        const std::string& type, double difficulty, double complexity) {
        curriculum->addTask(id, description, type, difficulty, complexity);
    }
    
    void updatePerformance(const std::string& taskId, double performance) {
        curriculum->updateTaskPerformance(taskId, performance);
        performanceHistory.push_back(performance);
        if (performanceHistory.size() > 1000) {
            performanceHistory.erase(performanceHistory.begin());
        }
        taskMetrics[taskId] = performance;
    }
    
    std::string getCurrentTask(const Brain& brain, const Config& config) {
        currentTaskId = curriculum->getNextTask(brain, config);
        return currentTaskId;
    }
    
    void completeTask(const std::string& taskId, const Brain& brain,
                     const Config& config) {
        curriculum->completeTask(taskId, brain, config);
    }
    
    double getConvergenceRate() const {
        return convergenceRate;
    }
    
    std::string getProgressSummary() const {
        std::stringstream ss;
        ss << "Convergence Rate: " << convergenceRate << "\n";
        ss << "Performance History Size: " << performanceHistory.size() << "\n";
        ss << curriculum->getStatusSummary();
        return ss.str();
    }
};

} // namespace nlm
