#pragma once

#include "ResearchConfig.hpp"
#include "Experiment.hpp"
#include "ExperimentRunner.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>
#include <future>
#include <chrono>
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Batch processing priorities
enum class BatchPriority {
    Low,
    Normal,
    High,
    Critical
};

// Batch job status
enum class JobStatus {
    Pending,
    Running,
    Completed,
    Failed,
    Cancelled
};

// Batch job structure
struct BatchJob {
    std::string jobId;
    std::string configName;
    ResearchConfig config;
    BatchPriority priority;
    JobStatus status;
    uint64_t seed;
    SimulationStep maxSteps;
    std::shared_ptr<Experiment> experiment;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::string errorMessage;
    double progress;  // 0.0 to 1.0
    
    BatchJob() : priority(BatchPriority::Normal), status(JobStatus::Pending), 
                 seed(0), maxSteps(0), progress(0.0) {}
};

// Worker thread status
struct WorkerStatus {
    std::string workerId;
    bool isActive;
    size_t tasksCompleted;
    size_t tasksFailed;
    std::chrono::system_clock::time_point lastActive;
    std::string currentTask;
};

// Batch processing statistics
struct BatchStats {
    size_t totalJobs;
    size_t completedJobs;
    size_t failedJobs;
    size_t pendingJobs;
    size_t runningJobs;
    double averageProgress;
    std::chrono::duration<double> totalElapsedTime;
    size_t parallelWorkers;
    
    BatchStats() : totalJobs(0), completedJobs(0), failedJobs(0), 
                   pendingJobs(0), runningJobs(0), averageProgress(0.0),
                   totalElapsedTime(0), parallelWorkers(0) {}
};

// Parallel batch processing engine
class ParallelBatchProcessor {
public:
    ParallelBatchProcessor();
    ~ParallelBatchProcessor();
    
    // Configuration
    void setConfig(std::shared_ptr<ResearchConfigManager> configManager);
    void setMaxWorkers(size_t maxWorkers);
    void setQueueSize(size_t maxQueueSize);
    void enableProgressMonitoring(bool enable);
    void setCheckpointFrequency(size_t frequency);
    
    // Job management
    std::string submitJob(const ResearchConfig& config, uint64_t seed, SimulationStep maxSteps);
    std::string submitJob(const std::string& configName, uint64_t seed, SimulationStep maxSteps);
    bool cancelJob(const std::string& jobId);
    bool pauseJob(const std::string& jobId);
    bool resumeJob(const std::string& jobId);
    
    // Job query
    const BatchJob* getJob(const std::string& jobId) const;
    std::vector<const BatchJob*> getJobs(JobStatus status) const;
    BatchStats getStats() const;
    std::vector<WorkerStatus> getWorkerStatus() const;
    
    // Control
    void start();
    void stop();
    void pause();
    void resume();
    bool isRunning() const;
    bool isPaused() const;
    
    // Progress tracking
    double getOverallProgress() const;
    std::string getCurrentJobId() const;
    std::chrono::duration<double> getEstimatedTimeRemaining() const;
    
    // Results management
    std::shared_ptr<Experiment> getExperimentResult(const std::string& jobId) const;
    std::vector<std::shared_ptr<Experiment>> getAllResults() const;
    bool saveResults(const std::string& filepath) const;
    
    // Monitoring and reporting
    std::string generateProgressReport() const;
    std::string generateDetailedReport() const;
    std::string generateWorkerReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Worker thread function
    void workerThread(size_t workerId);
    
    // Job queue management
    void scheduleJob(std::shared_ptr<BatchJob> job);
    std::shared_ptr<BatchJob> getNextJob();
    
    // Job execution
    bool executeJob(std::shared_ptr<BatchJob> job);
    void updateJobProgress(std::shared_ptr<BatchJob> job, double progress);
    void completeJob(std::shared_ptr<BatchJob> job, bool success, const std::string& errorMsg = "");
    
    // Helper functions
    std::string generateJobId() const;
    size_t getActiveWorkerCount() const;
    void cleanupCompletedJobs();
    
    // Thread synchronization
    void notifyWorkers();
    void waitForWorkers(std::chrono::milliseconds timeout = std::chrono::milliseconds(100));
};

// Batch processing utility functions
namespace batch_utils {
    // Generate unique job ID
    std::string generateJobId();
    
    // Priority comparison for job scheduling
    bool compareJobsByPriority(const std::shared_ptr<BatchJob>& a, const std::shared_ptr<BatchJob>& b);
    
    // Job status string conversion
    std::string jobStatusToString(JobStatus status);
    std::string batchPriorityToString(BatchPriority priority);
    
    // Progress calculation
    double calculateJobProgress(const BatchJob& job);
    
    // Time formatting utilities
    std::string formatDuration(std::chrono::duration<double> duration);
    std::string formatTimePoint(std::chrono::system_clock::time_point tp);
    
    // Thread pool management
    class ThreadPool {
    public:
        ThreadPool(size_t numThreads);
        ~ThreadPool();
        
        template<typename F>
        std::future<typename std::result_of<F()>::type> enqueue(F&& f);
        
        size_t getThreadCount() const { return threads.size(); }
        bool isRunning() const { return running; }
        void stop();
        void waitForCompletion();
        
    private:
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks;
        std::mutex queueMutex;
        std::condition_variable condition;
        bool running;
        size_t completedTasks;
    };
};

} // namespace batch_utils

} // namespace nlm