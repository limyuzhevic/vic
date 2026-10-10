// Resume manager - resume and recovery management
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <mutex>
#include "CheckpointSystem.hpp"

namespace nlm {

// Resume status
enum class ResumeStatus {
    Inactive,
    Checking,
    Validating,
    Restoring,
    Complete,
    Error,
    Failed
};

// Resume result
struct ResumeResult {
    bool success;
    std::string message;
    uint64_t checkpointStep;
    double checkpointTime;
    uint64_t restoredSize;
    double elapsedTime;
    time_t timestamp;
    
    ResumeResult() : success(false), checkpointStep(0), checkpointTime(0.0),
                   restoredSize(0), elapsedTime(0.0), timestamp(0) {}
};

// Resume options
struct ResumeOptions {
    std::string checkpointPath;
    bool validateBeforeResume;
    bool restoreIncremental;
    bool createBackupBeforeResume;
    bool monitorProgress;
    std::chrono::seconds timeout;
    std::function<void(double)> progressCallback; // 0.0 to 1.0
    
    ResumeOptions() : validateBeforeResume(true), restoreIncremental(true),
                     createBackupBeforeResume(true), monitorProgress(false),
                     timeout(std::chrono::seconds(300)) {} // 5 minutes default
};

// Resume statistics
struct ResumeStats {
    uint64_t totalResumes;
    uint64_t successfulResumes;
    uint64_t failedResumes;
    uint64_t totalRestoredSize;
    double averageResumeTime;
    uint32_t errorCount;
    
    ResumeStats() : totalResumes(0), successfulResumes(0), failedResumes(0),
                   totalRestoredSize(0), averageResumeTime(0.0), errorCount(0) {}
};

// Main resume manager
class ResumeManager {
public:
    ResumeManager();
    ~ResumeManager();
    
    // Configuration
    bool configure(const std::string& resumeDir, const std::shared_ptr<Config>& config);
    void shutdown();
    
    // Resume operations
    bool resumeFromCheckpoint(const std::string& checkpointPath, const ResumeOptions& options = ResumeOptions());
    bool resumeFromIncremental(const std::string& checkpointName);
    bool createIncrementalCheckpoint();
    bool getLastCheckpoint(std::string& path, uint64_t& step, double& time) const;
    
    // Checkpoint management
    bool restoreCheckpoint(const std::string& checkpointPath);
    bool restoreIncremental(const std::string& checkpointName);
    
    // Management operations
    bool deleteCheckpoint(const std::string& checkpointPath);
    std::vector<std::string> listAvailableCheckpoints() const;
    bool validateCheckpoint(const std::string& checkpointPath);
    
    // Statistics and monitoring
    ResumeStats getStats() const { return stats_; }
    
    bool isHealthy() const { return errorCount_ == 0; }
    
    // Event handling
    using ResumeEventCallback = std::function<void(const std::string&, const ResumeResult&)>;
    void setResumeEventCallback(ResumeEventCallback callback) { 
        resumeEventCallback_ = callback; 
    }
    
    // Thread safety
    void lock() const { mutex_.lock(); }
    void unlock() const { mutex_.unlock(); }
    bool tryLock() const { return mutex_.try_lock(); }

private:
    bool initialize();
    void cleanup();
    
    ResumeResult executeResumeOperation(const std::string& operation,
                                       const std::string& checkpointPath,
                                       const ResumeOptions& options);
    
    bool validateCheckpointIntegrity(const std::string& checkpointPath);
    bool performPreResumeBackup(const std::string& checkpointPath);
    bool monitorResumeProgress(double progress, const ResumeOptions& options);
    
    void updateStats(const ResumeResult& result);
    void logEvent(const std::string& operation, const std::string& checkpointPath,
                  const ResumeResult& result);
    
    std::string getCheckpointPath(const std::string& checkpointName) const;
    bool findLatestCheckpoint(std::string& path, uint64_t& step, double& time) const;
    
    // Components
    std::unique_ptr<CheckpointValidator> checkpointValidator_;
    std::unique_ptr<CheckpointReader> checkpointReader_;
    std::unique_ptr<CheckpointWriter> checkpointWriter_;
    
    // Configuration
    std::string resumeDir_;
    std::shared_ptr<Config> config_;
    
    // State
    ResumeStatus status_;
    mutable std::mutex mutex_;
    uint32_t errorCount_;
    
    // Statistics
    ResumeStats stats_;
    
    // Events
    ResumeEventCallback resumeEventCallback_;
};

} // namespace nlm
