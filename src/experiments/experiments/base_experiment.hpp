#pragma once

#include "Experiment.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace nlm {

// Abstract base class for all experiments
class BaseExperiment : public Experiment {
public:
    // Experiment states
    enum class State {
        Created,
        Scheduled,
        Running,
        Paused,
        Completed,
        Failed,
        Checkpointed
    };

    BaseExperiment(const std::string& name,
                   std::shared_ptr<Config> config = nullptr);
    virtual ~BaseExperiment() = default;

    // Experiment lifecycle
    virtual bool initialize() = 0;
    virtual bool run() = 0;
    virtual bool checkpoint() = 0;
    virtual bool resume() = 0;
    virtual bool finalize() = 0;

    // State management
    State getState() const { return state_; }
    virtual void setState(State state) { 
        std::lock_guard<std::mutex> lock(stateMutex_);
        state_ = state; 
    }

    // Parameter management
    virtual void setParameter(const std::string& key, const std::string& value);
    std::string getParameter(const std::string& key) const;
    const std::map<std::string, std::string>& getParameters() const { return parameters_; }

    // Progress tracking
    virtual double getProgress() const { return progress_; }
    virtual size_t getCurrentStep() const { return currentStep_; }
    virtual size_t getTotalSteps() const { return totalSteps_; }
    virtual void setTotalSteps(size_t steps) { totalSteps_ = steps; }

    // Result analysis
    virtual double calculatePerformanceScore() const = 0;
    virtual std::string getResultsSummary() const = 0;

    // Experiment metadata
    virtual std::string getExperimentType() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::vector<std::string> getRequiredParameters() const = 0;

    // Validation
    virtual bool validateParameters() const = 0;
    virtual bool validateResources() const { return true; }

    // Timing and execution
    virtual std::chrono::duration<double> getExecutionTime() const;
    virtual bool isRunning() const { return state_ == State::Running; }
    virtual bool isCompleted() const { return state_ == State::Completed; }

    // Progress callback
    using ProgressCallback = std::function<void(double, const std::string&)>;
    void setProgressCallback(ProgressCallback callback) { progressCallback_ = callback; }
    virtual void updateProgress(double progress, const std::string& message = "")
    {
        progress_ = progress;
        if (progressCallback_) {
            progressCallback_(progress, message);
        }
    }

    // Serialization
    virtual bool saveState(const std::string& filepath) const;
    virtual bool loadState(const std::string& filepath);

    // Thread safety
    void lock() { std::lock_guard<std::mutex> lock(stateMutex_); }
    bool tryLock() { return stateMutex_.try_lock(); }
    void unlock() { stateMutex_.unlock(); }

protected:
    mutable std::mutex stateMutex_;
    State state_;
    std::map<std::string, std::string> parameters_;
    double progress_;
    size_t currentStep_;
    size_t totalSteps_;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime_;
    ProgressCallback progressCallback_;
    std::shared_ptr<Config> config_;
};

// Parameter sweep result
struct SweepResult {
    std::string parameterName;
    std::string parameterValue;
    double metricValue;
    std::chrono::duration<double> executionTime;
    bool success;
    std::string errorMessage;
    std::shared_ptr<BaseExperiment> experiment;
};

// Multi-objective optimization result
struct OptimizationResult {
    std::vector<std::pair<std::string, double>> paretoFront;
    std::vector<std::shared_ptr<BaseExperiment>> experiments;
    std::string optimizationGoal;
    double convergenceMetric;
};

} // namespace nlm