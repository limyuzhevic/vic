#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <map>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <condition_variable>

namespace nlm {

class CheckpointManager {
public:
    CheckpointManager();
    ~CheckpointManager();
    
    // Disable copying, enable moving
    CheckpointManager(const CheckpointManager&) = delete;
    CheckpointManager& operator=(const CheckpointManager&) = delete;
    CheckpointManager(CheckpointManager&&) noexcept;
    CheckpointManager& operator=(CheckpointManager&&) noexcept;
    
    // Checkpoint states
    enum class CheckpointState {
        Ready,
        Saving,
        Loading,
        Comparing,
        Cleaning,
        Error
    };
    
    // Checkpoint information
    struct CheckpointInfo {
        std::string filename;
        std::string name;
        std::string description;
        std::chrono::steady_clock::time_point timestamp;
        std::chrono::steady_clock::time_point creationTime;
        std::chrono::steady_clock::time_point lastAccessTime;
        size_t size; // in bytes
        size_t neuronCount;
        size_t synapseCount;
        size_t totalSpikeCount;
        double totalLearning;
        std::string version;
        std::string brainStateHash;
        std::string configHash;
        std::string compressionAlgorithm;
        double compressionRatio;
        bool compressed;
        bool valid;
        std::string errorMessage;
    } currentCheckpoint;
    
    // Checkpoint configuration
    struct CheckpointConfig {
        std::string directory;
        size_t maxCheckpoints;
        size_t maxHistorySize;
        std::string compressionAlgorithm;
        bool autoBackup;
        bool validateOnLoad;
        bool compress;
        double compressionThreshold;
        std::vector<std::string> excludePatterns;
        std::vector<std::string> includePatterns;
        std::chrono::steady_clock::time_point backupInterval;
        std::string namingPattern;
        bool incremental;
        bool differential;
        std::string exportFormat;
        bool enableEncryption;
        std::string encryptionKey;
    } config;
    
    // Checkpoint operation result
    struct CheckpointResult {
        bool success;
        std::string message;
        std::chrono::steady_clock::time_point timestamp;
        std::string checkpointFile;
        std::string operation;
        double duration;
        size_t bytesWritten;
        size_t bytesRead;
        std::string errorCode;
    } lastResult;
    
    // Initialization
    bool initialize();
    
    // Checkpoint operations
    bool saveCheckpoint(const std::string& name = "", const std::string& description = "", const std::string& filename = "");
    bool loadCheckpoint(const std::string& name = "", const std::string& filename = "");
    bool compareCheckpoints(const std::string& checkpoint1, const std::string& checkpoint2);
    bool deleteCheckpoint(const std::string& name = "", const std::string& filename = "");
    bool listCheckpoints();
    bool infoCheckpoint(const std::string& name = "", const std::string& filename = "");
    
    // Checkpoint management
    bool backupCurrentState(const std::string& name = "auto");
    bool restoreFromBackup(const std::string& name = "auto");
    bool createSnapshot(const std::string& name = "snapshot", const std::string& description = "");
    bool restoreSnapshot(const std::string& name = "snapshot");
    bool cleanOldCheckpoints(size_t keepCount = 10);
    
    // Checkpoint configuration
    bool configure(const CheckpointConfig& config);
    bool updateConfig(const std::string& key, const std::string& value);
    CheckpointConfig getConfig() const { return config; }
    
    // Checkpoint listing and browsing
    std::vector<CheckpointInfo> listCheckpoints(const std::string& pattern = "");
    bool browseCheckpoints(const std::string& pattern = "");
    CheckpointInfo getCheckpointInfo(const std::string& name = "", const std::string& filename = "");
    
    // Checkpoint comparison
    std::string compareCheckpoints(const std::string& checkpoint1, const std::string& checkpoint2, bool detailed = false);
    std::string diffCheckpoints(const std::string& checkpoint1, const std::string& checkpoint2);
    
    // Checkpoint analysis
    bool analyzeCheckpoint(const std::string& name = "", const std::string& filename = "");
    std::string getCheckpointAnalysis(const std::string& name = "", const std::string& filename = "");
    double calculateSimilarity(const CheckpointInfo& checkpoint1, const CheckpointInfo& checkpoint2) const;
    
    // Checkpoint validation
    bool validateCheckpoint(const std::string& name = "", const std::string& filename = "");
    bool verifyCheckpointIntegrity(const std::string& name = "", const std::string& filename = "");
    std::vector<std::string> getCheckpointErrors(const std::string& name = "", const std::string& filename = "");
    
    // Checkpoint export and import
    bool exportCheckpoint(const std::string& name = "", const std::string& filename = "", const std::string& format = "binary");
    bool importCheckpoint(const std::string& filename, const std::string& format = "binary");
    bool exportMultipleCheckpoints(const std::vector<std::string>& names, const std::string& archiveFile);
    bool importMultipleCheckpoints(const std::string& archiveFile);
    
    // Checkpoint compression
    bool compressCheckpoint(const std::string& name = "", const std::string& filename = "", const std::string& algorithm = "gzip");
    bool decompressCheckpoint(const std::string& name = "", const std::string& filename = "", const std::string& algorithm = "gzip");
    double calculateCompressionRatio(const std::string& filename) const;
    
    // Checkpoint search and filtering
    std::vector<CheckpointInfo> searchCheckpoints(const std::string& keyword, const std::string& pattern = "");
    std::vector<CheckpointInfo> filterCheckpoints(const std::string& fromTime = "", const std::string& toTime = "", const std::string& pattern = "");
    std::vector<CheckpointInfo> sortCheckpoints(const std::string& sortBy = "timestamp", const std::string& order = "desc");
    
    // Checkpoint monitoring
    bool monitorCheckpoints(const std::string& pattern = "");
    bool stopMonitoring();
    
    // Results and statistics
    const CheckpointResult& getLastResult() const { return lastResult; }
    std::vector<CheckpointInfo> getRecentCheckpoints(size_t count = 10);
    size_t getCheckpointCount() const;
    size_t getDiskUsage() const;
    
    // State
    bool isSaving() const { return state == CheckpointState::Saving; }
    bool isLoading() const { return state == CheckpointState::Loading; }
    bool isComparing() const { return state == CheckpointState::Comparing; }
    CheckpointState getState() const { return state; }
    
    // Integration
    void setConfig(std::shared_ptr<Config> config) { this->config = config; }
    void setBrain(std::shared_ptr<Brain> brain) { this->brain = brain; }
    void setCLI(CLI* cli) { this->cli = cli; }
    
    // Help and documentation
    void showHelp() const;
    void showCheckpointCommands() const;
    void showComparisonCommands() const;
    void showManagementCommands() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool initialized;
    CheckpointState state;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    CLI* cli;
    
    // Thread management
    std::thread checkpointThread;
    std::mutex checkpointMutex;
    std::condition_variable checkpointCondition;
    std::atomic<bool> shouldStop;
    
    // Checkpoint file operations
    bool saveCheckpointToFile(const CheckpointInfo& checkpoint, const std::string& filepath);
    bool loadCheckpointFromFile(CheckpointInfo& checkpoint, const std::string& filepath);
    bool createCheckpointDirectory() const;
    std::string generateCheckpointFilename(const std::string& name = "", const std::string& extension = ".chk");
    bool validateCheckpointPath(const std::string& path) const;
    bool ensureCheckpointPath(const std::string& path) const;
    
    // Checkpoint compression/decompression
    bool compressFile(const std::string& inputFile, const std::string& outputFile, const std::string& algorithm);
    bool decompressFile(const std::string& inputFile, const std::string& outputFile, const std::string& algorithm);
    std::string calculateFileHash(const std::string& filename) const;
    std::string calculateDirectoryHash(const std::string& directory) const;
    
    // Checkpoint comparison and diff
    std::string compareCheckpointFiles(const std::string& file1, const std::string& file2, bool detailed = false);
    std::string diffCheckpointFiles(const std::string& file1, const std::string& file2);
    
    // Checkpoint analysis and validation
    CheckpointInfo analyzeCheckpointFile(const std::string& filename);
    bool validateCheckpointFile(const std::string& filename);
    std::vector<std::string> getCheckpointFileErrors(const std::string& filename);
    
    // Checkpoint monitoring thread
    void startCheckpointMonitoringThread();
    void stopCheckpointMonitoringThread();
    void monitoringWorker();
    
    // Helper methods
    std::string formatTimestamp(const std::chrono::steady_clock::time_point& timestamp) const;
    std::string formatFileSize(size_t bytes) const;
    std::string getStateString(CheckpointState state) const;
    std::string getOperationString(const std::string& operation) const;
    bool isFileAccessible(const std::string& path) const;
    bool isDirectoryEmpty(const std::string& path) const;
    bool cleanupEmptyDirectories(const std::string& path) const;
    std::string getFileExtension(const std::string& filename) const;
    std::string removeFileExtension(const std::string& filename) const;
    std::string getBackupFilename(const std::string& originalFile) const;
    bool saveCheckpointWithBackup(const std::string& filename, const std::string& backupName = "");
    bool restoreCheckpointWithBackup(const std::string& filename, const std::string& backupName = "");
    void cleanup();
};
