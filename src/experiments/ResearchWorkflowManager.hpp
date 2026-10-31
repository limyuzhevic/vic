// Research Workflow Manager
// Advanced workflow management for NLM research experiments

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <map>

namespace nlm {

// Research experiment types
enum class ResearchExperimentType {
    Developmental,
    Learning,
    Neuromodulation,
    Comparative,
    Custom
};

// Research data structure
struct ResearchData {
    std::string experimentName;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    uint64_t totalSteps;
    float totalReward;
    float maxFiringRate;
    float avgFiringRate;
    float eToIRatio;
    std::vector<std::pair<uint64_t, float>> stepRewards;
    std::vector<std::pair<uint64_t, float>> firingRates;
    std::vector<std::pair<uint64_t, float>> neuromodulationLevels;
    std::vector<std::string> learnedBehaviors;
    std::vector<std::string> hypothesesTested;
    std::map<std::string, float> experimentalResults;
    std::vector<std::string> researchNotes;
};

// Research workflow manager
class ResearchWorkflowManager {
public:
    struct WorkflowStep {
        std::string name;
        std::string description;
        std::function<void()> executeFunction;
        bool isCompleted;
        std::chrono::milliseconds duration;
    };
    
    ResearchWorkflowManager();
    
    void addStep(const std::string& name, const std::string& description, std::function<void()> function);
    
    void executeWorkflow();
    void executeStep(size_t stepIndex);
    void skipStep(size_t stepIndex);
    void retryStep(size_t stepIndex);
    
    void saveWorkflowState(const std::string& filepath);
    void loadWorkflowState(const std::string& filepath);
    
    void generateExperimentReport(const std::string& filepath);
    void generateWorkflowReport(const std::string& filepath);
    
    size_t getCurrentStep() const { return currentStep_; }
    size_t getTotalSteps() const { return workflowSteps_.size(); }
    
private:
    std::vector<WorkflowStep> workflowSteps_;
    size_t currentStep_;
};

// Forward declarations for helper functions
void printWorkflowHelp();
void executeResearchExperiment(ResearchExperimentType type);
void executeDevelopmentalExperiment();
void executeLearningExperiment();
void executeNeuromodulationExperiment();
void executeComparativeExperiment();

bool askUserForRetry();

} // namespace nlm