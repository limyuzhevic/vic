// NLM Experiment Chainer
// Implementation of experiment sequencing and dependency management

#include "ExperimentChainer.hpp"
#include "experiments/Phase6Experiment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <queue>
#include <stack>

namespace nlm {

struct ExperimentChainer::Impl {
    std::string lastError;
    bool shuttingDown = false;
};

ExperimentChainer::ExperimentChainer() : pImpl(std::make_unique<Impl>()) {}
ExperimentChainer::~ExperimentChainer() = default;

void ExperimentChainer::addDependency(const ExperimentDependency& dependency) {
    dependencies.push_back(dependency);
    NLM_LOG_INFO("Added dependency: " + dependency.dependentExperiment + " depends on " +
                dependency.prerequisiteExperiment);
}

void ExperimentChainer::addCondition(const ExperimentCondition& condition) {
    conditions.push_back(condition);
    NLM_LOG_INFO("Added condition for experiment " + condition.experimentName);
}

ExperimentExecutionPlan ExperimentChainer::buildExecutionPlan(
    const std::map<std::string, std::shared_ptr<Experiment>>& experimentMap) {
    
    ExperimentExecutionPlan plan;
    
    // Build dependency graph
    std::map<std::string, std::vector<std::string>> graph;
    
    // Add all experiment names to graph
    for (const auto& pair : experimentMap) {
        graph[pair.first];  // Ensure node exists
    }
    
    // Add dependencies
    for (const auto& dep : dependencies) {
        if (experimentMap.find(dep.prerequisiteExperiment) != experimentMap.end()) {
            graph[dep.prerequisiteExperiment].push_back(dep.dependentExperiment);
        }
    }
    
    // Check for cycles
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursionStack;
    
    for (const auto& pair : graph) {
        if (detectCycle(graph, pair.first, visited, recursionStack)) {
            pImpl->lastError = "Cycle detected in dependency graph";
            NLM_LOG_ERROR(pImpl->lastError);
            return plan;
        }
    }
    
    // Topological sort to get execution order
    plan.executionOrder = topologicalSort(graph);
    
    // Build dependencies map
    for (const auto& dep : dependencies) {
        plan.dependencies[dep.dependentExperiment].push_back(dep.prerequisiteExperiment);
    }
    
    // Build conditions map
    for (const auto& cond : conditions) {
        plan.conditions[cond.experimentName].push_back(cond);
    }
    
    // Estimate durations and calculate timing
    for (const auto& experimentName : plan.executionOrder) {
        auto it = experimentMap.find(experimentName);
        if (it != experimentMap.end()) {
            double duration = estimateExperimentDuration(it->second);
            plan.totalEstimatedDuration += duration;
            
            // Simple critical path calculation
            plan.earliestStartTimes[experimentName] = std::chrono::steady_clock::now();
            
            // Wait for prerequisites
            if (plan.dependencies.find(experimentName) != plan.dependencies.end()) {
                std::chrono::steady_clock::time_point latestPrereqEnd = plan.earliestStartTimes[experimentName];
                for (const auto& prereq : plan.dependencies[experimentName]) {
                    if (plan.earliestStartTimes.find(prereq) != plan.earliestStartTimes.end()) {
                        // In real implementation, calculate based on prereq duration
                        auto prereqEnd = plan.earliestStartTimes[prereq] + std::chrono::seconds((long)duration);
                        if (prereqEnd > latestPrereqEnd) {
                            latestPrereqEnd = prereqEnd;
                        }
                    }
                }
                plan.earliestStartTimes[experimentName] = latestPrereqEnd;
            }
        }
    }
    
    NLM_LOG_INFO("Built execution plan: " + std::to_string(plan.executionOrder.size()) +
                " experiments in order");
    
    return plan;
}

std::vector<ExperimentExecutionResult> ExperimentChainer::executeChain(
    const ExperimentChain& chain,
    const std::map<std::string, std::shared_ptr<Experiment>>& experimentMap,
    const BatchExecutionConfig& config) {
    
    std::vector<ExperimentExecutionResult> results;
    
    // Build execution plan
    ExperimentExecutionPlan plan = buildExecutionPlan(experimentMap);
    
    // Execute experiments in order
    for (const auto& experimentName : plan.executionOrder) {
        auto it = experimentMap.find(experimentName);
        if (it == experimentMap.end()) {
            NLM_LOG_WARNING("Experiment not found: " + experimentName);
            continue;
        }
        
        const auto& experiment = it->second;
        
        // Check if experiment can be executed
        if (!canExecute(experimentName, {}, experimentMap)) {
            NLM_LOG_INFO("Skipping experiment (conditions not met): " + experimentName);
            continue;
        }
        
        NLM_LOG_INFO("Executing chained experiment: " + experimentName +
                    " (" + std::to_string(results.size() + 1) + "/" +
                    std::to_string(plan.executionOrder.size()) + ")");
        
        // Execute experiment
        ExperimentExecutionResult result = executeExperiment(experiment, config);
        results.push_back(result);
        
        // Check for early termination
        if (!config.continueOnError && !result.success) {
            NLM_LOG_WARNING("Execution stopped due to error in experiment: " + experimentName);
            break;
        }
    }
    
    return results;
}

bool ExperimentChainer::canExecute(const std::string& experimentName,
                                  const std::map<std::string, ExperimentExecutionResult>& previousResults,
                                  const std::map<std::string, std::shared_ptr<Experiment>>& experimentMap) {
    
    // Check dependencies
    if (!checkDependencies(experimentName, previousResults)) {
        return false;
    }
    
    // Check conditions
    if (!checkConditions(experimentName, previousResults, experimentMap)) {
        return false;
    }
    
    return true;
}

bool ExperimentChainer::checkDependencies(const std::string& experimentName,
                                         const std::map<std::string, ExperimentExecutionResult>& previousResults) const {
    
    auto it = dependencies.begin();
    for (; it != dependencies.end(); ++it) {
        if (it->dependentExperiment == experimentName) {
            // Check if prerequisite was successful
            auto resultIt = previousResults.find(it->prerequisiteExperiment);
            if (resultIt == previousResults.end()) {
                return false;  // Prerequisite not executed yet
            }
            if (!resultIt->second.success) {
                return false;  // Prerequisite failed
            }
        }
    }
    
    return true;
}

bool ExperimentChainer::checkConditions(const std::string& experimentName,
                                       const std::map<std::string, ExperimentExecutionResult>& previousResults,
                                       const std::map<std::string, std::shared_ptr<Experiment>>& experimentMap) const {
    
    auto condIt = conditions.begin();
    for (; condIt != conditions.end(); ++condIt) {
        if (condIt->experimentName == experimentName) {
            // In real implementation, evaluate condition based on conditionType
            // For now, assume conditions are met
            NLM_LOG_INFO("Condition checked for experiment: " + experimentName);
        }
    }
    
    return true;
}

double ExperimentChainer::estimateExperimentDuration(const std::shared_ptr<Experiment>& experiment) const {
    // Simple estimation based on experiment name
    // In real implementation, this would query experiment metadata
    return 10.0;  // Default 10 seconds
}

std::vector<std::string> ExperimentChainer::topologicalSort(
    const std::map<std::string, std::vector<std::string>>& graph) const {
    
    std::vector<std::string> result;
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> inStack;
    
    // Helper function for DFS
    std::function<void(const std::string&)> dfs = [&](const std::string& node) {
        if (inStack.find(node) != inStack.end()) {
            // Cycle detected
            return;
        }
        if (visited.find(node) != visited.end()) {
            return;
        }
        
        visited.insert(node);
        inStack.insert(node);
        
        for (const auto& neighbor : graph.at(node)) {
            dfs(neighbor);
        }
        
        inStack.erase(node);
        result.push_back(node);
    };
    
    // Perform DFS on all nodes
    for (const auto& pair : graph) {
        if (visited.find(pair.first) == visited.end()) {
            dfs(pair.first);
        }
    }
    
    // Reverse result to get correct topological order
    std::reverse(result.begin(), result.end());
    return result;
}

bool ExperimentChainer::detectCycle(const std::map<std::string, std::vector<std::string>>& graph,
                                   const std::string& node,
                                   std::unordered_set<std::string>& visited,
                                   std::unordered_set<std::string>& recursionStack) const {
    
    if (recursionStack.find(node) != recursionStack.end()) {
        return true;  // Cycle detected
    }
    
    if (visited.find(node) != visited.end()) {
        return false;  // Already processed
    }
    
    visited.insert(node);
    recursionStack.insert(node);
    
    for (const auto& neighbor : graph.at(node)) {
        if (detectCycle(graph, neighbor, visited, recursionStack)) {
            return true;
        }
    }
    
    recursionStack.erase(node);
    return false;
}

} // namespace nlm