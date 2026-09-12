#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <deque>

namespace nlm {

// Goal-based Planning with Hierarchical Task Decomposition
class HierarchicalTaskDecomposition {
public:
    // Task types for different levels of abstraction
    enum class TaskType {
        Primitive,      // Atomic action
        Composite,      // Sequence of primitives
        Subgoal,        // High-level goal
        Strategy,       // Multi-step plan
        Meta            // Self-improvement goal
    };
    
    // Task execution status
    struct Task {
        std::string name;
        TaskType type;
        std::vector<float> goalState;      // Desired state
        float rewardValue;                  // Expected reward
        float complexity;                    // Task difficulty
        float estimatedDuration;            // Time to complete
        
        // Hierarchy
        std::vector<size_t> childTaskIndices;
        size_t parentTaskIndex;              // -1 if root task
        
        // Execution state
        bool isCompleted;
        bool isActive;
        float progress;                      // 0.0 to 1.0
        float accumulatedReward;             // Actual reward received
        
        // Social learning metadata
        std::vector<size_t> observedByAgents;  // Which agents have observed this
        std::vector<float> imitationSuccessRate;
        
        Task() : rewardValue(0.5f), complexity(1.0f), estimatedDuration(1.0f),
                parentTaskIndex(-1), isCompleted(false), isActive(false),
                progress(0.0f), accumulatedReward(0.0f) {}
    };
    
    HierarchicalTaskDecomposition();
    ~HierarchicalTaskDecomposition();
    
    // Task management
    size_t addTask(const Task& task, size_t parentIndex = -1);
    void setCurrentGoal(size_t taskIndex);
    void executeActiveTasks(const std::vector<float>& currentState);
    
    // Hierarchical decomposition
    void decomposeTask(size_t taskIndex, size_t decompositionLevel);
    std::vector<size_t> getTaskHierarchy(size_t taskIndex) const;
    std::vector<size_t> getExecutableTasks(size_t taskIndex) const;
    
    // Planning
    std::vector<size_t> planTaskSequence(size_t taskIndex,
                                         const std::vector<float>& startState);
    float evaluateTaskSequence(const std::vector<size_t>& taskSequence,
                              const std::vector<float>& startState) const;
    
    // Social learning and imitation
    void observeOtherAgentTask(size_t agentId, size_t taskIndex);
    void learnFromObservation(size_t observingAgentId, size_t taskIndex,
                             const std::vector<float>& successIndicators);
    std::vector<size_t> getImitationCandidates(const std::vector<float>& currentState) const;
    
    // Meta-cognition
    void monitorTaskExecution(size_t taskIndex, const std::vector<float>& actualOutcome);
    float assessMetaCognitiveState() const;  // Self-monitoring confidence
    void optimizeTaskExecution(const std::vector<float>& feedback);
    void improveTaskStrategies();  // Self-improvement
    
    // Transfer of skills
    void transferSkill(size_t sourceTaskIndex, size_t targetTaskIndex);
    std::vector<size_t> findTransferableSkills(size_t sourceTaskIndex) const;
    
    // Integration with brain
    void setBrain(Brain* brain);
    void step(float timestep);
    
    // API for external agents
    const Task& getTask(size_t index) const;
    std::vector<size_t> getRootTasks() const;
    size_t getCurrentGoalTask() const { return currentGoalTask_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    size_t currentGoalTask_;
};

} // namespace nlm
