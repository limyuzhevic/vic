// NLM Advanced Features - Batch Mode
// Batch processing mode for handling large volumes of data and tasks

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <iomanip>
#include <sstream>
#include <deque>
#include <filesystem>

namespace nlm {
namespace advanced {

// Batch job types
enum class BatchJobType {
    Processing,
    Transformation,
    Analysis,
    Validation,
    Export,
    Import,
    Cleanup,
    Custom
};

// Batch job status
enum class BatchJobStatus {
    Pending,
    Running,
    Completed,
    Failed,
    Cancelled,
    Skipped
};

// Batch job priority
enum class BatchJobPriority {
    Low,
    Normal,
    High,
    Critical
};

// Batch item for file-based operations
struct BatchItem {
    std::string id;
    std::string sourcePath;
    std::string destinationPath;
    std::map<std::string, std::string> metadata;
    BatchJobStatus status;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::string errorMessage;
    uint64_t processedBytes;
    double processingTime;
    
    BatchItem() : status(BatchJobStatus::Pending), processedBytes(0), processingTime(0.0) {
        startTime = std::chrono::system_clock::now();
    }
};

// Batch job definition
struct BatchJob {
    std::string id;
    BatchJobType type;
    std::string description;
    std::vector<std::string> inputFiles;
    std::vector<std::string> outputFiles;
    BatchJobPriority priority;
    BatchJobStatus status;
    std::chrono::system_clock::time_point scheduledTime;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::string errorMessage;
    uint64_t totalItems;
    uint64_t processedItems;
    double estimatedTime;
    double actualTime;
    std::map<std::string, std::string> parameters;
    std::function<bool(const BatchItem&)> itemProcessor;
    std::function<bool(const BatchJob&)> jobValidator;
    std::function<void(const BatchItem&)> itemCallback;
    std::function<void(const BatchJob&)> jobCallback;
    
    BatchJob() : type(BatchJobType::Processing), priority(BatchJobPriority::Normal),
                 status(BatchJobStatus::Pending), totalItems(0), processedItems(0),
                 estimatedTime(0.0), actualTime(0.0) {
        scheduledTime = std::chrono::system_clock::now();
    }
};

// Batch statistics
struct BatchStatistics {
    uint64_t totalJobs;
    uint64_t jobsByType[8]; // Index by BatchJobType
    uint64_t jobsByStatus[6]; // Index by BatchJobStatus
    uint64_t jobsByPriority[4]; // Index by BatchJobPriority
    uint64_t totalItems;
    uint64_t processedItems;
    uint64_t failedItems;
    uint64_t skippedItems;
    uint64_t totalBytesProcessed;
    double totalProcessingTime;
    double successRate;
    double averageProcessingTime;
    std::chrono::system_clock::time_point firstJobTime;
    std::chrono::system_clock::time_point lastJobTime;
    double jobRatePerMinute;
    std::string mostFrequentJob;
    uint64_t mostFrequentJobCount;
    std::deque<std::string> recentJobs;
    
    BatchStatistics() : totalJobs(0), totalItems(0), processedItems(0),
                        failedItems(0), skippedItems(0), totalBytesProcessed(0),
                        totalProcessingTime(0.0), successRate(0.0),
                        averageProcessingTime(0.0), jobRatePerMinute(0.0),
                        mostFrequentJobCount(0) {
        for (int i = 0; i < 8; ++i) jobsByType[i] = 0;
        for (int i = 0; i < 6; ++i) jobsByStatus[i] = 0;
        for (int i = 0; i < 4; ++i) jobsByPriority[i] = 0;
    }
};

// Batch configuration
struct BatchConfig {
    std::string batchFile; // File containing job definitions
    std::string outputDirectory;
    uint32_t maxConcurrentJobs;
    uint32_t maxMemoryUsage;
    double timeoutPerJob;
    bool continueOnFailure;
    bool dryRun;
    bool skipExisting;
    std::string filePattern;
    std::string outputPattern;
    bool enableProgressReporting;
    bool enableLogging;
    std::string logFile;
    std::string reportFormat;
    bool exportToDatabase;
    std::string databaseConnection;
    std::function<bool(const BatchJob&)>) jobFilter;
    std::function<void(const BatchJob&)>) jobPreProcessor;
    std::function<void(const BatchJob&)>) jobPostProcessor;
    
    BatchConfig() : maxConcurrentJobs(4), maxMemoryUsage(1024), // MB
                    timeoutPerJob(3600.0), continueOnFailure(true), // 1 hour per job
                    dryRun(false), skipExisting(false), enableProgressReporting(true),
                    enableLogging(true), reportFormat("text"), exportToDatabase(false) {
    }
};

// Batch mode class
class BatchMode {
public:
    BatchMode();
    ~BatchMode();
    
    // Configuration
    void configure(const BatchConfig& config);
    BatchConfig getConfig() const;
    
    // Batch control
    bool loadJobs(const std::string& filename);
    bool loadJobs(const std::vector<std::string>& filenames);
    bool saveJobs(const std::string& filename) const;
    bool start();
    void stop();
    bool isActive() const;
    bool isRunning() const;
    
    // Job management
    bool addJob(const BatchJob& job);
    bool removeJob(const std::string& jobId);
    bool updateJob(const BatchJob& job);
    bool pauseJob(const std::string& jobId);
    bool resumeJob(const std::string& jobId);
    bool cancelJob(const std::string& jobId);
    
    std::vector<BatchJob> getJobs() const;
    std::vector<BatchJob> getJobsByType(BatchJobType type) const;
    std::vector<BatchJob> getJobsByStatus(BatchJobStatus status) const;
    std::vector<BatchJob> getJobsByPriority(BatchJobPriority priority) const;
    BatchJob getJob(const std::string& jobId) const;
    
    // Item management
    bool addItemToJob(const std::string& jobId, const BatchItem& item);
    bool removeItemFromJob(const std::string& jobId, const std::string& itemId);
    bool updateItemInJob(const std::string& jobId, const BatchItem& item);
    std::vector<BatchItem> getItemsInJob(const std::string& jobId) const;
    BatchItem getItem(const std::string& jobId, const std::string& itemId) const;
    
    // Processing control
    bool processJob(const std::string& jobId);
    bool processAllJobs();
    bool waitForJobCompletion(const std::string& jobId, double timeout);
    bool waitForAllJobsCompletion(double timeout);
    
    // Query and analysis
    BatchStatistics getStatistics() const;
    double getTotalProcessingTime() const;
    uint64_t getTotalItemsProcessed() const;
    uint64_t getTotalItemsFailed() const;
    double getSuccessRate() const;
    
    // Reporting
    std::string generateReport() const;
    std::string generateJobReport(const std::string& jobId) const;
    std::string generateProgressReport() const;
    bool exportReport(const std::string& filename) const;
    bool exportDetailedReport(const std::string& filename) const;
    
    // Export
    bool exportToDatabase(const std::string& connection) const;
    bool importFromDatabase(const std::string& connection);
    
    // Filtering
    void setJobFilter(const std::function<bool(const BatchJob&)>& filter);
    void addIncludePattern(const std::string& pattern);
    void addExcludePattern(const std::string& pattern);
    
    // Callbacks
    void setJobCallback(const std::function<void(const BatchJob&)>& callback);
    void setItemCallback(const std::function<void(const BatchItem&)>& callback);
    void setProgressCallback(const std::function<void(double)>& callback);
    
    // Singleton pattern
    static BatchMode& getInstance();
    static void resetInstance();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    void startWorkerThreads();
    void stopWorkerThreads();
    void workerThread();
    bool validateJob(const BatchJob& job) const;
    void updateJobStatus(const std::string& jobId, BatchJobStatus status);
    void updateJobProgress(const std::string& jobId);
    std::string generateJobId() const;
    std::string generateItemId() const;
    std::string formatTime(double seconds) const;
    std::string formatBytes(uint64_t bytes) const;
    std::string getTimeString(const std::chrono::system_clock::time_point& time) const;
};

} // namespace advanced
} // namespace nlm

// Export BatchMode functions for external use
namespace nlm {
    namespace advanced {
        // Global batch mode functions
        inline bool loadBatchFile(const std::string& filename) {
            return BatchMode::getInstance().loadJobs(filename);
        }
        
        inline bool startBatchProcessing() {
            return BatchMode::getInstance().start();
        }
        
        inline void stopBatchProcessing() {
            BatchMode::getInstance().stop();
        }
        
        inline bool isBatchProcessing() {
            return BatchMode::getInstance().isActive();
        }
        
        inline std::vector<BatchJob> getBatchJobs() {
            return BatchMode::getInstance().getJobs();
        }
        
        // Convenience macros
        #define NLM_BATCH_JOB(id, type, description) \
            BatchJob job; \
            job.id = id; \
            job.type = type; \
            job.description = description; \
            BatchMode::getInstance().addJob(job)
        
        #define NLM_BATCH_PROCESS(jobId) \
            BatchMode::getInstance().processJob(jobId)
        
    } // namespace advanced
} // namespace nlm

// Include implementation
#include "BatchMode.cpp"

// Initialize global batch macros
namespace nlm {
    namespace advanced {
        // Global batch processing macros for use throughout the codebase
        #define NLM_BATCH_JOB_DEF(id, type, desc) \
            BatchJob id##_job; \
            id##_job.id = id; \
            id##_job.type = type; \
            id##_job.description = desc
        
        #define NLM_BATCH_JOB_SUBMIT(id) \
            BatchMode::getInstance().addJob(id##_job); \
            BatchMode::getInstance().processJob(id)
        
        #define NLM_BATCH_ITEM_DEF(id, src, dst) \
            BatchItem id##_item; \
            id##_item.id = id; \
            id##_item.sourcePath = src; \
            id##_item.destinationPath = dst
        
        #define NLM_BATCH_ITEM_ADD(jobId, item) \
            BatchMode::getInstance().addItemToJob(jobId, item)
        
    } // namespace advanced
} // namespace nlm