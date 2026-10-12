// ExperimentScheduler - Manage experiment execution order and timing
#pragma once

#include "IExperiment.hpp"
#include "ExperimentResult.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <vector>
#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

namespace nlm {

struct ExperimentSchedule {
    std::string name;
    int priority;  // Higher priority = earlier execution
    std::chrono::system_clock::time_point scheduledTime;
    std::chrono::system_clock::time_point deadline;
    std::map<std::string, double> parameters;
    bool parallelAllowed;
    size_t maxConcurrentInstances;
    
    ExperimentSchedule(const std::string& n) 
        : name(n), priority(0), parallelAllowed(false), maxConcurrentInstances(1) {}
};

class ExperimentScheduler {
public:
    enum class ScheduleMode { Sequential, Parallel, Priority, Deadline, RoundRobin };
    
    // Constructor
    ExperimentScheduler(ScheduleMode mode = ScheduleMode::Sequential);
    ~ExperimentScheduler();
    
    // Schedule management
    void addExperiment(const std::string& name, const ExperimentSchedule& schedule);
    void removeExperiment(const std::string& name);
    void clearSchedule();
    
    // Execution
    std::vector<ExperimentResult> executeSchedule(std::shared_ptr<Brain> brain, 
                                                 std::shared_ptr<Environment> environment);
    ExperimentResult executeSingle(const std::string& name, 
                                   std::shared_ptr<Brain> brain,
                                   std::shared_ptr<Environment> environment,
                                   const ExperimentSchedule& schedule);
    
    // Batch operations
    std::vector<ExperimentResult> executeBatch(const std::vector<std::string>& names,
                                               std::shared_ptr<Brain> brain,
                                               std::shared_ptr<Environment> environment,
                                               bool parallel = false);
    
    // Scheduling algorithms
    std::vector<std::string> getExecutionOrder() const;
    std::map<std::string, std::chrono::system_clock::time_point> calculateTimeline() const;
    
    // Monitoring
    bool isRunning() const { return running; }
    double getProgress() const { return progress; }
    size_t getCompletedCount() const { return completedCount; }
    size_t getTotalCount() const { return totalCount; }
    std::vector<ExperimentResult> getResults() const { return results; }
    
    // Control
    void pause();
    void resume();
    void stop();
    
    // Callbacks
    void setProgressCallback(const std::function<void(float)>& callback) {
        progressCallback = callback;
    }
    
    void setCompletionCallback(const std::function<void(const ExperimentResult&)>& callback) {
        completionCallback = callback;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    ScheduleMode mode;
    bool running;
    double progress;
    size_t completedCount;
    size_t totalCount;
    std::vector<ExperimentResult> results;
    std::function<void(float)> progressCallback;
    std::function<void(const ExperimentResult&)> completionCallback;
    
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<bool> paused;
};

} // namespace nlm
