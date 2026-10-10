// NLM Advanced Features - Progress Tracker
// Advanced progress tracking system with ETA, checkpoints, and callbacks

#ifndef NLM_PROGRESS_TRACKER_HPP
#define NLM_PROGRESS_TRACKER_HPP

#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <memory>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <condition_variable>

namespace nlm {
namespace advanced {

struct ProgressInfo {
    int currentStep;
    int totalSteps;
    double percentage;
    double elapsedTime;
    double estimatedRemainingTime;
    std::string currentStatus;
    std::string stageName;
    std::map<std::string, double> metrics;
};

struct ProgressCallback {
    std::function<void(const ProgressInfo&)> callback;
    std::string name;
    bool enabled;
};

struct Checkpoint {
    std::string name;
    std::chrono::system_clock::time_point timestamp;
    std::string description;
    std::map<std::string, std::string> metadata;
    size_t checkpointSize;
};

class ProgressTracker {
public:
    ProgressTracker();
    ~ProgressTracker();
    
    // Progress tracking
    void startProgress(int totalSteps, const std::string& stageName = "Processing");
    void updateProgress(int currentStep, const std::string& status = "");
    void completeProgress();
    void pauseProgress();
    void resumeProgress();
    void cancelProgress();
    
    // Progress query
    bool isActive() const;
    bool isPaused() const;
    int getCurrentStep() const;
    int getTotalSteps() const;
    double getPercentage() const;
    double getElapsedTime() const;
    double getEstimatedRemainingTime() const;
    const ProgressInfo& getProgressInfo() const;
    std::string getStageName() const;
    std::map<std::string, double> getMetrics() const;
    
    // Callbacks
    void addCallback(const ProgressCallback& callback);
    void removeCallback(const std::string& name);
    void clearCallbacks();
    void setProgressCallback(const std::function<void(const ProgressInfo&)>& callback, const std::string& name = "default");
    
    // ETA calculations
    void updateETA();
    double calculateETA(int current, int total, double elapsedTime) const;
    std::string formatTime(double seconds) const;
    
    // Checkpoint system
    bool createCheckpoint(const std::string& name, const std::string& description = "");
    bool restoreCheckpoint(const std::string& name);
    std::vector<Checkpoint> getAvailableCheckpoints() const;
    bool deleteCheckpoint(const std::string& name);
    bool cleanupOldCheckpoints(int keepCount = 10);
    
    // Progress visualization
    void displayProgressBar(const ProgressInfo& info, bool showPercentage = true, bool showETA = true);
    void displayProgressText(const ProgressInfo& info);
    void displayProgressJSON(const ProgressInfo& info, std::ostream& output = std::cout);
    void displayProgressCSV(const ProgressInfo& info, std::ostream& output = std::cout);
    void displayProgressXML(const ProgressInfo& info, std::ostream& output = std::cout);
    
    // Configuration
    void setUpdateInterval(double interval);
    void setAutoSave(bool enabled);
    void setAutoSaveInterval(double interval);
    void setMaxCheckpoints(int maxCheckpoints);
    void setEnableVisualizations(bool enabled);
    void setVisualizationUpdateInterval(double interval);
    
    // Statistics
    int getUpdateCount() const;
    double getTotalTime() const;
    double getAverageUpdateInterval() const;
    
    // Output
    void exportProgressHistory(const std::string& filename);
    void importProgressHistory(const std::string& filename);
    std::vector<ProgressInfo> getProgressHistory() const;
    
    // Thread safety
    void lock();
    void unlock();
    bool tryLock();
    
    // Threading support
    void waitForCompletion();
    bool waitForCompletion(double timeout);
    void setCompletionEvent(const std::string& eventName);
    
    // Callback functions
    using ProgressCallbackFunc = std::function<void(const ProgressInfo&)>
    using CheckpointCallbackFunc = std::function<void(const Checkpoint&)>
    using CompletionCallbackFunc = std::function<void()>
    
    void setProgressCallback(ProgressCallbackFunc callback)
    void setCheckpointCallback(CheckpointCallbackFunc callback)
    void setCompletionCallback(CompletionCallbackFunc callback)
    
    // Special progress types
    void startNeuralSimulationProgress(int neuronCount, int steps);
    void startBenchmarkProgress(int benchmarkCount, const std::vector<std::string>& benchmarkNames);
    void startBatchProcessingProgress(int fileCount, const std::string& operation);
    void startCheckpointingProgress(int checkpointCount);
    
    // Progress aggregation
    void addProgressTracker(const std::shared_ptr<ProgressTracker>& tracker);
    void removeProgressTracker(const std::shared_ptr<ProgressTracker>& tracker);
    void aggregateProgress();
    
    // Utility functions
    static std::string formatPercentage(double percentage);
    static std::string formatTime(double seconds, bool includeMilliseconds = true);
    static std::string formatSpeed(double value, const std::string& unit = "items/sec");
    static std::string generateProgressID();
    static std::string getCurrentTimestamp();
    
    // Singleton pattern
    static ProgressTracker& getInstance();
    static void resetInstance();

private:
    ProgressInfo currentProgress;
    std::vector<ProgressCallback> callbacks;
    std::vector<Checkpoint> checkpoints;
    std::vector<ProgressInfo> progressHistory;
    
    // Configuration
    double updateInterval;
    bool autoSave;
    double autoSaveInterval;
    int maxCheckpoints;
    bool enableVisualizations;
    double visualizationUpdateInterval;
    
    // State
    bool active;
    bool paused;
    bool cancelled;
    bool completed;
    std::chrono::steady_clock::time_point startTime;
    
    // Thread safety
    mutable std::mutex mutex;
    std::condition_variable completionEvent;
    std::string completionEventName;
    
    // Statistics
    int updateCount;
    double totalTime;
    double totalElapsed;
    std::chrono::steady_clock::time_point lastUpdateTime;
    
    // Callbacks
    ProgressCallbackFunc progressCallback;
    CheckpointCallbackFunc checkpointCallback;
    CompletionCallbackFunc completionCallback;
    
    // Nested progress trackers
    std::vector<std::shared_ptr<ProgressTracker>> childTrackers;
    
    // Helper methods
    void notifyCallbacks();
    void updateProgressHistory();
    bool saveProgressToFile(const std::string& filename) const;
    bool loadProgressFromFile(const std::string& filename);
    void cleanupOldCheckpointsInternal();
    std::string generateCheckpointPath(const std::string& name) const;
    bool validateCheckpoint(const Checkpoint& checkpoint) const;
};

} // namespace advanced
} // namespace nlm

#endif // NLM_PROGRESS_TRACKER_HPP
