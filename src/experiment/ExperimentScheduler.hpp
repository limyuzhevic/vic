#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include "ExperimentProtocol.hpp"
#include "ExperimentStateManager.hpp"

namespace nlm {

// Experiment scheduling configuration
struct ExperimentSchedulerConfig {
    std::string name;
    std::string type; // "sequential", "parallel", "pipeline", "batch", "stream"
    std::chrono::system_clock::time_point scheduleTime;
    std::chrono::system_clock::time_point nextRunTime;
    std::chrono::system_clock::time_point lastRunTime;
    bool enabled;
    bool autoSchedule;
    int priority;
    int maxConcurrentExperiments;
    std::chrono::seconds timeout;
    std::vector<std::string> experimentDependencies;
    std::map<std::string, std::string> scheduleParameters;
    std::string scheduleExpression; // cron-like expression
    std::vector<std::chrono::seconds> runIntervals;
    
    ExperimentSchedulerConfig() 
        : enabled(true), autoSchedule(true), priority(0), maxConcurrentExperiments(1),
          timeout(std::chrono::seconds(3600)), scheduleTime(std::chrono::system_clock::now()) {}
};

// Experiment schedule entry
struct ExperimentSchedule {
    std::string experimentName;
    std::chrono::system_clock::time_point scheduledTime;
    std::chrono::system_clock::time_point actualStartTime;
    std::chrono::system_clock::time_point actualEndTime;
    bool isCompleted;
    bool isRunning;
    ExperimentState state;
    std::string scheduleId;
    std::map<std::string, std::string> scheduleData;
    
    ExperimentSchedule() : isCompleted(false), isRunning(false), state(ExperimentState::Pending) {}
    ExperimentSchedule(const std::string& expName, const std::chrono::system_clock::time_point& time)
        : experimentName(expName), scheduledTime(time), isCompleted(false), isRunning(false), 
          state(ExperimentState::Pending) {}
};

// Schedule exception
struct ScheduleException {
    std::string scheduleId;
    std::chrono::system_clock::time_point timestamp;
    std::string errorType;
    std::string errorMessage;
    std::map<std::string, std::string> errorData;
    
    ScheduleException() : timestamp(std::chrono::system_clock::now()) {}
    ScheduleException(const std::string& id, const std::string& type, const std::string& message)
        : scheduleId(id), errorType(type), errorMessage(message), timestamp(std::chrono::system_clock::now()) {}
};

class ExperimentScheduler {
public:
    ExperimentScheduler();
    ~ExperimentScheduler();
    
    // Disable copying, enable moving
    ExperimentScheduler(const ExperimentScheduler&) = delete;
    ExperimentScheduler& operator=(const ExperimentScheduler&) = delete;
    ExperimentScheduler(ExperimentScheduler&&) noexcept;
    ExperimentScheduler& operator=(ExperimentScheduler&&) noexcept;
    
    // Scheduling management
    bool createSchedule(const std::string& scheduleName, const ExperimentSchedulerConfig& config);
    bool removeSchedule(const std::string& scheduleName);
    bool updateSchedule(const std::string& scheduleName, const ExperimentSchedulerConfig& config);
    bool enableSchedule(const std::string& scheduleName, bool enable);
    bool pauseSchedule(const std::string& scheduleName);
    bool resumeSchedule(const std::string& scheduleName);
    
    // Schedule execution
    bool runSchedule(const std::string& scheduleName);
    bool runScheduleNow(const std::string& scheduleName);
    bool triggerSchedule(const std::string& scheduleName);
    bool stopSchedule(const std::string& scheduleName);
    
    // Schedule management
    ExperimentSchedulerConfig getScheduleConfig(const std::string& scheduleName) const;
    std::vector<std::string> getAllScheduleNames() const;
    std::vector<ExperimentSchedule> getScheduleHistory(const std::string& scheduleName, 
                                                      size_t limit = 100) const;
    std::vector<ScheduleException> getScheduleExceptions(const std::string& scheduleName) const;
    
    // Schedule analysis
    bool validateSchedule(const std::string& scheduleName) const;
    std::chrono::system_clock::time_point calculateNextRunTime(const std::string& scheduleName) const;
    double calculateScheduleEfficiency(const std::string& scheduleName) const;
    std::string analyzeSchedulePatterns(const std::string& scheduleName) const;
    
    // Schedule monitoring
    bool addScheduleCallback(const std::string& scheduleName, 
                           std::function<void(const ExperimentSchedule&)> callback);
    bool removeScheduleCallback(const std::string& scheduleName, 
                              std::function<void(const ExperimentSchedule&)> callback);
    std::vector<std::function<void(const ExperimentSchedule&)>> getScheduleCallbacks(const std::string& scheduleName) const;
    
    // Schedule optimization
    bool optimizeSchedule(const std::string& scheduleName);
    bool autoTuneScheduleParameters(const std::string& scheduleName);
    bool redistributeScheduleLoad(const std::string& scheduleName);
    
    // Resource management
    bool checkResourceAvailability(const std::string& scheduleName, 
                                  const std::map<std::string, int>& requiredResources) const;
    bool reserveResources(const std::string& scheduleName, 
                        const std::map<std::string, int>& resources);
    bool releaseResources(const std::string& scheduleName, 
                        const std::map<std::string, int>& resources);
    
    // Cleanup and maintenance
    bool cleanupCompletedSchedules(const std::chrono::system_clock::time_point& cutoffTime);
    bool cleanupFailedSchedules(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetSchedule(const std::string& scheduleName);
    bool clearScheduleHistory(const std::string& scheduleName);
    
    // Schedule statistics and analytics
    size_t getScheduleRunCount(const std::string& scheduleName) const;
    double getScheduleSuccessRate(const std::string& scheduleName) const;
    std::chrono::system_clock::time_point getScheduleLastRunTime(const std::string& scheduleName) const;
    std::chrono::system_clock::time_point getScheduleNextRunTime(const std::string& scheduleName) const;
    std::map<std::string, double> getResourceUtilization(const std::string& scheduleName) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm