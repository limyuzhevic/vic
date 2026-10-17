// NLM Experiment Chainer
// Manages experiment sequencing and dependencies

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include "experiments/Experiment.hpp"
#include "batch/BatchExecutor.hpp"

namespace nlm {

// Experiment dependency definition
struct ExperimentDependency {
    std::string dependentExperiment;
    std::string prerequisiteExperiment;
    std::string dependencyType;  // "sequential", "conditional", "parallel"
    std::string conditionExpression;
    double timeoutSeconds = 0.0;
    
    ExperimentDependency() {}
    ExperimentDependency(const std::string& dependent, const std::string& prerequisite)
        : dependentExperiment(dependent), prerequisiteExperiment(prerequisite) {}
};

// Experiment condition
struct ExperimentCondition {
    std::string experimentName;
    std::string conditionType;  // "success", "failure", "metric", "time"
    std::string metricName;
    double metricValue;
    std::string operator;  // ">", "<", "==", "!=", ">=", "<="
    bool conditionMet = false;
    
    ExperimentCondition() {}
    ExperimentCondition(const std::string& name, const std::string& type)
        : experimentName(name), conditionType(type) {}
};

// Experiment execution plan
struct ExperimentExecutionPlan {
    std::vector<std::string> executionOrder;
    std::map<std::string, std::vector<std::string>> dependencies;
    std::map<std::string, std::vector<ExperimentCondition>> conditions;
    std::map<std::string, std::chrono::steady_clock::time_point> earliestStartTimes;
    std::map<std::string, std::chrono::steady_clock::time_point> latestStartTimes;
    double totalEstimatedDuration = 0.0;
    
    ExperimentExecutionPlan() {}
    explicit ExperimentExecutionPlan(const std::vector<std::string>& order) : executionOrder(order) {}
};

// Main experiment chainer class
class ExperimentChainer {
public:
    ExperimentChainer();
    ~ExperimentChainer();
    
    // Add experiment dependency
    void addDependency(const ExperimentDependency& dependency);
    
    // Add experiment condition
    void addCondition(const ExperimentCondition& condition);
    
    // Build execution plan
    ExperimentExecutionPlan buildExecutionPlan(
        const std::map<std::string, std::shared_ptr<Experiment>>& experimentMap);
    
    // Execute experiment chain
    std::vector<ExperimentExecutionResult> executeChain(
        const ExperimentChain& chain,
        const std::map<std::string, std::shared_ptr<Experiment>>& experimentMap,
        const BatchExecutionConfig& config);
    
    // Check if experiment can be executed
    bool canExecute(const std::string& experimentName,
                   const std::map<std::string, ExperimentExecutionResult>& previousResults,
                   const std::map<std::string, std::shared_ptr<Experiment>>& experimentMap);
    
    // Get experiment dependencies
    const std::vector<ExperimentDependency>& getDependencies() const { return dependencies; }
    
    // Get experiment conditions
    const std::vector<ExperimentCondition>& getConditions() const { return conditions; }
    
    // Static chainer instance
    static ExperimentChainer& getInstance() {
        static ExperimentChainer instance;
        return instance;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::vector<ExperimentDependency> dependencies;
    std::vector<ExperimentCondition> conditions;
    
    // Topological sort for execution order
    std::vector<std::string> topologicalSort(
        const std::map<std::string, std::vector<std::string>>& graph) const;
    
    // Check dependency satisfaction
    bool checkDependencies(const std::string& experimentName,
                          const std::map<std::string, ExperimentExecutionResult>& previousResults) const;
    
    // Check conditions
    bool checkConditions(const std::string& experimentName,
                        const std::map<std::string, ExperimentExecutionResult>& previousResults,
                        const std::map<std::string, std::shared_ptr<Experiment>>& experimentMap) const;
    
    // Estimate experiment duration
    double estimateExperimentDuration(const std::shared_ptr<Experiment>& experiment) const;
    
    // Detect cycles in dependency graph
    bool detectCycle(const std::map<std::string, std::vector<std::string>>& graph,
                    const std::string& node,
                    std::unordered_set<std::string>& visited,
                    std::unordered_set<std::string>& recursionStack) const;
};

} // namespace nlm