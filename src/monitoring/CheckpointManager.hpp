#pragma once

/**
 * CheckpointManager - Production-ready checkpoint management
 * 
 * Provides robust checkpointing for large-scale neural simulations with
 * compressed files, version-aware format, incremental saves, and validation.
 * 
 * Features:
 * - Brain state serialization to disk (save/load)
 * - Compressed checkpoint files
 * - Version-aware checkpoint format
 * - Incremental checkpoint support
 * - Checkpoint validation
 * - Automatic checkpoint cleanup
 * - Progress reporting
 * - Integration with visualization system
 */

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <limits>
#include <unordered_map>
#include <ctime>

namespace nlm {

// Incremental checkpoint data for incremental saves
struct IncrementalCheckpoint {
    std::string baseName;           // Base checkpoint name
    uint64_t baseStep;              // Step of base checkpoint
    uint64_t nextStep;              // Next step to save
    std::string incrementalPath;    // Path for incremental checkpoint
    bool isCompressed;              // Whether incremental is compressed
    uint64_t deltaSize;             // Size of delta data
    time_t timestamp;               // Creation timestamp
    
    IncrementalCheckpoint() : baseStep(0), nextStep(0), deltaSize(0), 
                             isCompressed(false), timestamp(0) {}
};

// Checkpoint version for compatibility
struct CheckpointVersion {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
    
    CheckpointVersion() : major(1), minor(0), patch(0) {}
    CheckpointVersion(uint32_t maj, uint32_t min, uint32_t patch) 
        : major(maj), minor(min), patch(patch) {}
    
    bool operator==(const CheckpointVersion& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
    
    bool isCompatible(const CheckpointVersion& other) const {
        return major == other.major && minor <= other.minor;
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << major << "." << minor << "." << patch;
        return ss.str();
    }
};

// Checkpoint statistics for monitoring
struct CheckpointStats {
    uint64_t totalSaves;           // Total successful checkpoints saved
    uint64_t totalLoads;           // Total successful checkpoints loaded
    uint64_t totalSizeSaved;        // Total bytes saved
    uint64_t totalSizeLoaded;       // Total bytes loaded
    double averageSaveTime;        // Average save time in seconds
    double averageLoadTime;        // Average load time in seconds
    uint64_t oldestCheckpointAge;   // Age of oldest checkpoint in seconds
    uint64_t newestCheckpointAge;   // Age of newest checkpoint in seconds
    
    CheckpointStats() : totalSaves(0), totalLoads(0), totalSizeSaved(0), 
                       totalSizeLoaded(0), averageSaveTime(0.0), 
                       averageLoadTime(0.0), oldestCheckpointAge(0),
                       newestCheckpointAge(0) {}
};

// Progress callback for checkpoint operations
struct CheckpointProgress {
    std::string operation;         // "save", "load", "validate"
    double progress;               // 0.0 to 1.0
    std::string message;           // Human-readable message
    uint64_t currentStep;          // Current step
    uint64_t totalSteps;           // Total steps (for save operations)
    double estimatedTimeRemaining; // Estimated seconds remaining
    uint64_t bytesProcessed;       // Bytes processed (for large checkpoints)
    uint64_t totalBytes;           // Total bytes (for large checkpoints)
    
    CheckpointProgress() : progress(0.0), currentStep(0), totalSteps(0),
                          estimatedTimeRemaining(0.0), bytesProcessed(0),
                          totalBytes(0) {}
};

class CheckpointManager {
public:
    CheckpointManager();
    ~CheckpointManager();
    
    // Configuration
    bool configure(const std::string& checkpointDir, 
                   uint64_t saveIntervalSteps = 10000,
                   size_t maxCheckpoints = 10,
                   bool compress = true,
                   CompressionLevel compressionLevel = CompressionLevel::Balanced,
                   bool enableIncremental = true);
    
    // Main interface
    bool save(const std::string& name, uint64_t step, double simulationTime,
              const Brain& brain, bool isIncremental = false);
    
    bool load(const std::string& name, Brain& brain, bool incremental = false);
    
    // Automatic checkpoint management
    bool update(uint64_t currentStep, double currentTime);
    bool saveImmediately(const std::string& name = "");
    
    // Data providers (set by brain)
    using NeuronDataProvider = std::function<bool(NeuronCheckpointData&)>
    using SynapseDataProvider = std::function<bool(SynapseCheckpointData&)>
    
    void setNeuronProvider(NeuronDataProvider provider) {
        neuronProvider_ = provider;
    }
    
    void setSynapseProvider(SynapseDataProvider provider) {
        synapseProvider_ = provider;
    }
    
    // Checkpoint management
    bool validate(const std::string& name) const;
    bool deleteCheckpoint(const std::string& name);
    std::vector<std::string> listCheckpoints() const;
    
    // Progress reporting
    CheckpointProgress getCurrentProgress() const;
    bool setProgressCallback(std::function<void(const CheckpointProgress&)> callback);
    
    // Statistics and monitoring
    CheckpointStats getStatistics() const;
    bool resetStatistics();
    
    // Configuration access
    const std::string& getCheckpointDir() const { return checkpointDir_; }
    uint64_t getSaveIntervalSteps() const { return saveIntervalSteps_; }
    size_t getMaxCheckpoints() const { return maxCheckpoints_; }
    bool isCompressed() const { return compress_; }
    bool isIncrementalEnabled() const { return enableIncremental_; }
    
    // Compression and version
    bool compressFile(const std::string& inputPath, const std::string& outputPath,
                     CompressionLevel level = CompressionLevel::Balanced);
    bool decompressFile(const std::string& inputPath, const std::string& outputPath);
    
    // Version management
    static const CheckpointVersion& getCurrentVersion() { return currentVersion_; }
    static bool setCurrentVersion(uint32_t major, uint32_t minor, uint32_t patch);
    
    // Validation
    bool validateCheckpoint(const std::string& filepath) const;
    std::string getValidationError() const { return validationError_; }
    
    // File system operations
    bool ensureDirectory(const std::string& path);
    std::string generateCheckpointName(uint64_t step, double simulationTime,
                                        const std::string& prefix = "checkpoint") const;
    
    // Performance optimization
    bool enableIncrementalSaving(bool enable) {
        enableIncremental_ = enable;
        return true;
    }
    
    // Visualization integration
    void setVisualizationInterface(VisualizationInterface* visualization) {
        visualization_ = visualization;
    }

private:
    // Internal helper classes
    class CheckpointFile {
    public:
        CheckpointFile() : handle_(nullptr), compressionLevel_(CompressionLevel::None) {}
        ~CheckpointFile() { if (handle_) fclose(handle_); }
        
        bool open(const std::string& filepath, bool writeMode,
                  CompressionLevel compression = CompressionLevel::None);
        void close() { if (handle_) fclose(handle_); handle_ = nullptr; }
        
        bool write(const void* data, size_t size);
        bool read(void* data, size_t size);
        bool flush() { return fflush(handle_) == 0; }
        size_t getPosition() const { return ftell(handle_); }
        bool seek(size_t position) { return fseek(handle_, position, SEEK_SET) == 0; }
        bool isOpen() const { return handle_ != nullptr; }
        
        CompressionLevel getCompressionLevel() const { return compressionLevel_; }
        
    private:
        FILE* handle_;
        CompressionLevel compressionLevel_;
    };
    
    // Helper functions
    bool writeCheckpointHeader(CheckpointFile& file, uint64_t step,
                               double simulationTime, uint64_t neuronCount,
                               uint64_t synapseCount, uint64_t regionCount);
    
    bool writeCheckpointData(CheckpointFile& file, CheckpointSection sectionType,
                             const void* data, size_t size);
    
    bool writeSectionHeader(CheckpointFile& file, const SectionHeader& header);
    
    bool readCheckpointHeader(CheckpointFile& file, CheckpointHeader& header);
    
    bool readCheckpointData(CheckpointFile& file, CheckpointSection sectionType,
                            void* data, size_t size);
    
    bool computeChecksum(const void* data, size_t size, uint64_t& checksum) const;
    
    bool validateDataIntegrity(const std::string& filepath) const;
    
    bool createIncrementalCheckpoint(const std::string& basePath, uint64_t baseStep,
                                     uint64_t currentStep, const Brain& brain);
    
    bool applyIncrementalCheckpoint(const std::string& basePath, uint64_t baseStep,
                                    Brain& brain);
    
    bool updateProgress(const std::string& operation, double progress,
                        const std::string& message = "",
                        uint64_t step = 0, uint64_t totalSteps = 0,
                        uint64_t bytesProcessed = 0, uint64_t totalBytes = 0);
    
    bool updateStatistics(bool saveSuccess, uint64_t bytesWritten,
                          double saveTime, bool loadSuccess,
                          uint64_t bytesRead, double loadTime);
    
    bool cleanupOldCheckpoints();
    
    bool getCheckpointInfo(const std::string& name, CheckpointInfo& info) const;
    
    bool getIncrementalCheckpointInfo(const std::string& basePath,
                                      IncrementalCheckpoint& incInfo) const;
    
    std::string getIncrementalCheckpointPath(const std::string& basePath,
                                              uint64_t step) const;
    
    std::string getBaseCheckpointPath(const std::string& path) const;
    
    bool notifyVisualization(const std::string& event, double progress = 0.0);
    
    // Member variables
    std::string checkpointDir_;
    uint64_t saveIntervalSteps_;
    size_t maxCheckpoints_;
    bool compress_;
    CompressionLevel compressionLevel_;
    bool enableIncremental_;
    uint64_t lastSaveStep_;
    std::string lastCheckpointPath_;
    
    // Data providers
    NeuronDataProvider neuronProvider_;
    SynapseDataProvider synapseProvider_;
    
    // Progress and statistics
    CheckpointProgress currentProgress_;
    std::function<void(const CheckpointProgress&)> progressCallback_;
    CheckpointStats stats_;
    
    // Version management
    static CheckpointVersion currentVersion_;
    
    // Incremental checkpoint tracking
    std::unordered_map<std::string, IncrementalCheckpoint> incrementalCheckpoints_;
    
    // Validation and error handling
    std::string validationError_;
    
    // Visualization integration
    VisualizationInterface* visualization_;
    
    // File system
    bool ensureDirectory(const std::string& path) {
        return std::filesystem::create_directories(path);
    }
    
    std::string generateTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
        return ss.str();
    }
};