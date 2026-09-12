#include "HierarchicalTaskDecomposition.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <queue>

namespace nlm {

struct HierarchicalTaskDecomposition::Impl {
    // All tasks organized by type
    std::vector<Task> tasks;
    
    // Task execution
    std::deque<size_t> executionQueue;
    std::vector<float> currentState;
    
    // Social learning
    std::unordered_map<size_t, std::vector<size_t>> socialObservations;
    
    // Meta-cognition
    std::vector<float> confidenceScores;
    std::vector<size_t> failureCounts;
    
    // Skill transfer
    std::vector<std::vector<size_t>> skillMappings;
    
    // Integration with brain
    Brain* brain;
    
    // Statistics
    size_t completedTasks;
    float totalReward;
    
    Impl() : brain(nullptr), completedTasks(0), totalReward(0.0f) {
        std::random_device rd;
        rng.seed(rd());
        
        // Initialize confidence and failure tracking
        confidenceScores.reserve(100);
        failureCounts.reserve(100);
    }
    
    std::mt19937 rng;
    
    float computeStateSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
        if (a.size() != b.size() || a.empty()) return 0.0f;
        
        float dot = 0.0f, normA = 0.0f, normB = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            dot += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        
        if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
        
        return dot / (std::sqrt(normA) * std::sqrt(normB));
    }
};

HierarchicalTaskDecomposition::HierarchicalTaskDecomposition() 
    : currentGoalTask_(0) {
    pImpl = std::make_unique<Impl>();
}

HierarchicalTaskDecomposition::~HierarchicalTaskDecomposition() = default;

void HierarchicalTaskDecomposition::setBrain(Brain* brain) {
    pImpl->brain = brain;
}

size_t HierarchicalTaskDecomposition::addTask(const Task& task, size_t parentIndex) {
    size_t taskIndex = pImpl->tasks.size();
    pImpl->tasks.push_back(task);
    
    if (parentIndex != -1) {
        pImpl->tasks[parentIndex].childTaskIndices.push_back(taskIndex);
        pImpl->tasks[taskIndex].parentTaskIndex = parentIndex;
    }
    
    // Initialize social learning data
    pImpl->confidenceScores.push_back(0.5f);
    pImpl->failureCounts.push_back(0);
    
    // Initialize skill transfer
    pImpl->skillMappings.push_back(std::vector<size_t>());
    
    NLM_LOG_INFO("Added task: " + task.name + " with index " + std::to_string(taskIndex) +
                 " as child of " + std::to_string(parentIndex));
    
    return taskIndex;
}

void HierarchicalTaskDecomposition::setCurrentGoal(size_t taskIndex) {
    if (taskIndex < pImpl->tasks.size()) {
        currentGoalTask_ = taskIndex;
        
        // Mark all ancestor tasks as inactive
        size_t current = taskIndex;
        while (current != -1 && current < pImpl->tasks.size()) {
            pImpl->tasks[current].isActive = false;
            current = pImpl->tasks[current].parentTaskIndex;
        }
        
        // Reactivate task and mark descendants as potential
        pImpl->tasks[taskIndex].isActive = true;
        NLM_LOG_INFO("Set current goal: " + pImpl->tasks[taskIndex].name);
    }
}

void HierarchicalTaskDecomposition::executeActiveTasks(const std::vector<float>& currentState) {
    pImpl->currentState = currentState;
    
    // Execute tasks in hierarchical order
    for (size_t taskIndex : pImpl->executionQueue) {
        if (taskIndex >= pImpl->tasks.size()) continue;
        
        Task& task = pImpl->tasks[taskIndex];
        
        if (!task.isActive || task.isCompleted) continue;
        
        // Check if task can be executed
        float completion = 0.0f;
        if (task.type == TaskType::Primitive) {
            // Primitive task: simple action execution
            completion = 0.8f * computeStateSimilarity(currentState, task.goalState);
            
            // Update task metrics
            if (completion > 0.7f) {
                task.isCompleted = true;
                task.accumulatedReward += task.rewardValue;
                pImpl->completedTasks++;
                pImpl->totalReward += task.rewardValue;
                
                NLM_LOG_INFO("Completed primitive task: " + task.name + 
                             " with reward: " + std::to_string(task.rewardValue));
            } else {
                pImpl->failureCounts[taskIndex]++;
                if (pImpl->failureCounts[taskIndex] > 3) {
                    // Give up on failed task
                    task.isActive = false;
                    NLM_LOG_INFO("Abandoning failed task: " + task.name);
                }
            }
        } else if (task.type == TaskType::Composite) {
            // Composite task: execute all child tasks
            bool allChildrenCompleted = true;
            for (size_t childIndex : task.childTaskIndices) {
                if (childIndex < pImpl->tasks.size() && !pImpl->tasks[childIndex].isCompleted) {
                    allChildrenCompleted = false;
                    break;
                }
            }
            
            if (allChildrenCompleted) {
                task.isCompleted = true;
                task.accumulatedReward += task.rewardValue;
                pImpl->completedTasks++;
                pImpl->totalReward += task.rewardValue;
                NLM_LOG_INFO("Completed composite task: " + task.name);
            }
        }
        
        // Update task progress
        task.progress = std::min(1.0f, task.progress + 0.1f);
    }
}

void HierarchicalTaskDecomposition::decomposeTask(size_t taskIndex, size_t decompositionLevel) {
    if (taskIndex >= pImpl->tasks.size()) return;
    
    // Decompose the task recursively
    if (decompositionLevel > 0 && pImpl->tasks[taskIndex].childTaskIndices.empty()) {
        // Create child tasks
        for (size_t i = 0; i < decompositionLevel && i < 3; ++i) {  // Limit decomposition depth
            Task childTask;
            childTask.name = pImpl->tasks[taskIndex].name + "_child" + std::to_string(i);
            childTask.type = (i == 0) ? TaskType::Primitive : TaskType::Composite;
            childTask.rewardValue = pImpl->tasks[taskIndex].rewardValue * 0.5f;
            childTask.complexity = pImpl->tasks[taskIndex].complexity * 0.8f;
            childTask.estimatedDuration = pImpl->tasks[taskIndex].estimatedDuration * 0.9f;
            
            size_t childIndex = addTask(childTask, taskIndex);
            
            // Decompose further if needed
            if (i < decompositionLevel - 1) {
                decomposeTask(childIndex, decompositionLevel - 1);
            }
        }
        
        NLM_LOG_INFO("Decomposed task " + std::to_string(taskIndex) + 
                     " into " + std::to_string(pImpl->tasks[taskIndex].childTaskIndices.size()) + 
                     " child tasks");
    }
}

std::vector<size_t> HierarchicalTaskDecomposition::getTaskHierarchy(size_t taskIndex) const {
    std::vector<size_t> hierarchy;
    size_t current = taskIndex;
    
    while (current != -1 && current < pImpl->tasks.size()) {
        hierarchy.push_back(current);
        current = pImpl->tasks[current].parentTaskIndex;
    }
    
    std::reverse(hierarchy.begin(), hierarchy.end());
    return hierarchy;
}

std::vector<size_t> HierarchicalTaskDecomposition::getExecutableTasks(size_t taskIndex) const {
    std::vector<size_t> executable;
    
    if (taskIndex >= pImpl->tasks.size()) return executable;
    
    const Task& task = pImpl->tasks[taskIndex];
    
    if (task.type == TaskType::Primitive) {
        // Primitive tasks are executable if not completed
        if (!task.isCompleted && task.isActive) {
            executable.push_back(taskIndex);
        }
    } else if (task.type == TaskType::Composite) {
        // Composite tasks are executable if all children are completed
        bool allChildrenCompleted = true;
        for (size_t childIndex : task.childTaskIndices) {
            if (childIndex < pImpl->tasks.size() && !pImpl->tasks[childIndex].isCompleted) {
                allChildrenCompleted = false;
                break;
            }
        }
        
        if (allChildrenCompleted && task.isActive && !task.isCompleted) {
            executable.push_back(taskIndex);
        }
    }
    
    return executable;
}

std::vector<size_t> HierarchicalTaskDecomposition::planTaskSequence(size_t taskIndex,
                                                                    const std::vector<float>& startState) {
    std::vector<size_t> sequence;
    
    if (taskIndex >= pImpl->tasks.size()) return sequence;
    
    // Get all executable tasks in hierarchy
    std::vector<size_t> hierarchy = getTaskHierarchy(taskIndex);
    std::vector<size_t> executable;
    
    for (size_t taskIdx : hierarchy) {
        std::vector<size_t> execTasks = getExecutableTasks(taskIdx);
        executable.insert(executable.end(), execTasks.begin(), execTasks.end());
    }
    
    // Sort by complexity (simpler tasks first)
    std::sort(executable.begin(), executable.end(),
              [this](size_t a, size_t b) {
                  if (a >= pImpl->tasks.size() || b >= pImpl->tasks.size()) return false;
                  return pImpl->tasks[a].complexity < pImpl->tasks[b].complexity;
              });
    
    sequence = executable;
    
    // Add to execution queue
    pImpl->executionQueue.insert(pImpl->executionQueue.end(), sequence.begin(), sequence.end());
    
    NLM_LOG_INFO("Planned task sequence with " + std::to_string(sequence.size()) + " tasks");
    
    return sequence;
}

float HierarchicalTaskDecomposition::evaluateTaskSequence(const std::vector<size_t>& taskSequence,
                                                         const std::vector<float>& startState) const {
    if (taskSequence.empty()) return 0.0f;
    
    float totalReward = 0.0f;
    float totalConfidence = 0.0f;
    
    for (size_t taskIndex : taskSequence) {
        if (taskIndex >= pImpl->tasks.size()) continue;
        
        const Task& task = pImpl->tasks[taskIndex];
        
        // Base reward
        float reward = task.rewardValue;
        
        // Confidence adjustment based on task history
        float confidence = pImpl->confidenceScores[taskIndex];
        if (confidence < 0.5f) {
            reward *= confidence;
        }
        
        totalReward += reward;
        totalConfidence += confidence;
    }
    
    // Return average reward weighted by confidence
    float avgConfidence = totalConfidence / taskSequence.size();
    return totalReward * avgConfidence;
}

void HierarchicalTaskDecomposition::observeOtherAgentTask(size_t agentId, size_t taskIndex) {
    if (taskIndex < pImpl->tasks.size()) {
        pImpl->socialObservations[agentId].push_back(taskIndex);
        NLM_LOG_INFO("Agent " + std::to_string(agentId) + " observed task " + 
                     std::to_string(taskIndex));
    }
}

void HierarchicalTaskDecomposition::learnFromObservation(size_t observingAgentId, size_t taskIndex,
                                                          const std::vector<float>& successIndicators) {
    if (taskIndex < pImpl->tasks.size() && observingAgentId < pImpl->tasks.size()) {
        const Task& task = pImpl->tasks[taskIndex];
        
        // Update confidence based on observation
        float observationValue = 0.5f;  // Default
        if (!successIndicators.empty()) {
            float avgSuccess = 0.0f;
            for (float success : successIndicators) {
                avgSuccess += success;
            }
            avgSuccess /= successIndicators.size();
            observationValue = avgSuccess;
        }
        
        pImpl->confidenceScores[observingAgentId] = 
            (pImpl->confidenceScores[observingAgentId] * 0.7f) + 
            (observationValue * 0.3f);
        
        NLM_LOG_INFO("Agent " + std::to_string(observingAgentId) + 
                     " learned from observation with value: " + std::to_string(observationValue));
    }
}

std::vector<size_t> HierarchicalTaskDecomposition::getImitationCandidates(const std::vector<float>& currentState) const {
    std::vector<size_t> candidates;
    
    for (size_t agentId = 0; agentId < pImpl->socialObservations.size(); ++agentId) {
        auto it = pImpl->socialObservations.find(agentId);
        if (it == pImpl->socialObservations.end()) continue;
        
        for (size_t taskIndex : it->second) {
            if (taskIndex < pImpl->tasks.size()) {
                const Task& task = pImpl->tasks[taskIndex];
                
                // Check if task is suitable for imitation
                if (!task.isCompleted && task.isActive) {
                    // Rate suitability based on current state
                    float suitability = computeStateSimilarity(currentState, task.goalState);
                    if (suitability > 0.6f) {
                        candidates.push_back(taskIndex);
                    }
                }
            }
        }
    }
    
    return candidates;
}

void HierarchicalTaskDecomposition::monitorTaskExecution(size_t taskIndex, 
                                                         const std::vector<float>& actualOutcome) {
    if (taskIndex < pImpl->tasks.size()) {
        Task& task = pImpl->tasks[taskIndex];
        
        // Update meta-cognitive state
        float expectedProgress = 0.5f;  // Would be based on prediction
        float actualProgress = computeStateSimilarity(actualOutcome, task.goalState);
        
        float metaCognitiveScore = std::abs(expectedProgress - actualProgress);
        
        NLM_LOG_INFO("Meta-cognitive assessment for task " + task.name + 
                     ": discrepancy = " + std::to_string(metaCognitiveScore));
        
        // Adjust task strategy if discrepancy is high
        if (metaCognitiveScore > 0.3f) {
            // Update confidence based on performance
            pImpl->confidenceScores[taskIndex] *= (1.0f - metaCognitiveScore);
            pImpl->confidenceScores[taskIndex] = std::max(0.1f, pImpl->confidenceScores[taskIndex]);
        }
    }
}

float HierarchicalTaskDecomposition::assessMetaCognitiveState() const {
    if (pImpl->confidenceScores.empty()) return 0.5f;
    
    float totalConfidence = 0.0f;
    for (float confidence : pImpl->confidenceScores) {
        totalConfidence += confidence;
    }
    
    return totalConfidence / pImpl->confidenceScores.size();
}

void HierarchicalTaskDecomposition::optimizeTaskExecution(const std::vector<float>& feedback) {
    // Use feedback to improve task execution strategies
    for (size_t i = 0; i < pImpl->tasks.size(); ++i) {
        Task& task = pImpl->tasks[i];
        
        // Adjust task parameters based on feedback
        float adjustment = 0.0f;
        if (!feedback.empty() && i < feedback.size()) {
            adjustment = feedback[i] * 0.1f;  // Small adjustment
        }
        
        // Update task complexity and estimated duration
        task.complexity = std::max(0.1f, task.complexity * (1.0f + adjustment));
        task.estimatedDuration = std::max(0.5f, task.estimatedDuration * (1.0f + adjustment * 0.5f));
        
        // Update confidence
        pImpl->confidenceScores[i] = std::min(1.0f, pImpl->confidenceScores[i] + adjustment);
    }
}

void HierarchicalTaskDecomposition::improveTaskStrategies() {
    // Meta-learning: improve task execution strategies based on experience
    for (size_t i = 0; i < pImpl->tasks.size(); ++i) {
        Task& task = pImpl->tasks[i];
        
        // If task has been successful, remember the strategy
        if (task.progress > 0.8f && task.isCompleted) {
            // Learn from successful execution
            float successRate = task.accumulatedReward / task.rewardValue;
            pImpl->confidenceScores[i] = std::min(1.0f, pImpl->confidenceScores[i] + successRate * 0.1f);
            
            NLM_LOG_INFO("Improved strategy for task " + task.name + 
                         " based on " + std::to_string(successRate * 100) + "% success rate");
        }
    }
}

void HierarchicalTaskDecomposition::transferSkill(size_t sourceTaskIndex, size_t targetTaskIndex) {
    if (sourceTaskIndex < pImpl->tasks.size() && 
        targetTaskIndex < pImpl->tasks.size() &&
        sourceTaskIndex != targetTaskIndex) {
        
        const Task& sourceTask = pImpl->tasks[sourceTaskIndex];
        Task& targetTask = pImpl->tasks[targetTaskIndex];
        
        // Transfer learned parameters
        targetTask.complexity = std::min(targetTask.complexity, sourceTask.complexity * 0.9f);
        targetTask.estimatedDuration = std::min(targetTask.estimatedDuration, 
                                               sourceTask.estimatedDuration * 0.9f);
        
        // Update confidence based on source task performance
        targetTask.accumulatedReward += sourceTask.accumulatedReward * 0.5f;
        
        NLM_LOG_INFO("Transferred skill from task " + std::to_string(sourceTaskIndex) + 
                     " to task " + std::to_string(targetTaskIndex));
    }
}

std::vector<size_t> HierarchicalTaskDecomposition::findTransferableSkills(size_t sourceTaskIndex) const {
    std::vector<size_t> transferable;
    
    if (sourceTaskIndex >= pImpl->tasks.size()) return transferable;
    
    const Task& sourceTask = pImpl->tasks[sourceTaskIndex];
    
    // Find tasks that can benefit from similar skills
    for (size_t i = 0; i < pImpl->tasks.size(); ++i) {
        if (i == sourceTaskIndex) continue;
        
        const Task& targetTask = pImpl->tasks[i];
        
        // Check similarity between tasks
        float similarity = 0.0f;
        if (sourceTask.goalState.size() == targetTask.goalState.size()) {
            similarity = 0.0f;  // Would compute actual similarity
        }
        
        // If similarity is high enough, consider transferable
        if (similarity > 0.7f && targetTask.type == TaskType::Composite) {
            transferable.push_back(i);
        }
    }
    
    return transferable;
}

const HierarchicalTaskDecomposition::Task& HierarchicalTaskDecomposition::getTask(size_t index) const {
    if (index < pImpl->tasks.size()) {
        return pImpl->tasks[index];
    }
    return pImpl->tasks[0]; // Return first task if index out of range
}

std::vector<size_t> HierarchicalTaskDecomposition::getRootTasks() const {
    std::vector<size_t> rootTasks;
    
    for (size_t i = 0; i < pImpl->tasks.size(); ++i) {
        if (pImpl->tasks[i].parentTaskIndex == -1) {
            rootTasks.push_back(i);
        }
    }
    
    return rootTasks;
}

void HierarchicalTaskDecomposition::step(float timestep) {
    // Process task execution queue
    if (!pImpl->executionQueue.empty()) {
        // Process one task per step (simplified)
        size_t taskIndex = pImpl->executionQueue.front();
        
        if (taskIndex < pImpl->tasks.size()) {
            Task& task = pImpl->tasks[taskIndex];
            
            if (!task.isCompleted) {
                // Execute task based on type
                if (task.type == TaskType::Primitive) {
                    // Execute primitive action
                    task.progress += 0.1f * timestep;
                    if (task.progress >= 1.0f) {
                        task.isCompleted = true;
                        task.accumulatedReward += task.rewardValue;
                        pImpl->completedTasks++;
                        pImpl->totalReward += task.rewardValue;
                        NLM_LOG_INFO("Completed primitive task: " + task.name);
                    }
                }
            }
            
            pImpl->executionQueue.pop_front();
        }
    }
}

} // namespace nlm
