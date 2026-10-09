#pragma once

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <unordered_map>
#include <thread>
#include <mutex>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Enhanced checkpoint manager with advanced features
class CheckpointManager {
public:
    struct CheckpointInfo {
        std::string filepath;
        SimulationStep step;
        Timestamp timestamp;
        size_t neuronCount;
        size_t synapseCount;
        float simulationTime;
        std::string description;
    };
    
    CheckpointManager();
    ~CheckpointManager();
    
    // Configure checkpoint system
    void configure(const std::string& directory, 
                   SimulationStep saveInterval,
                   size_t maxCheckpoints,
                   bool enableAutoSave,
                   bool enableCompression = false,
                   const std::string& format = "binary");
    
    // Create a checkpoint
    bool save(SimulationStep step, Timestamp timestamp,
              const std::vector<std::unique_ptr<NeuralRegion>>& regions,
              const std::vector<std::unique_ptr<Synapse>>& synapses,
              const std::string& description = "");
    
    // Load a checkpoint
    bool load(SimulationStep targetStep,
              std::vector<std::unique_ptr<NeuralRegion>>& regions,
              std::vector<std::unique_ptr<Synapse>>& synapses,
              SimulationStep& outLoadedStep,
              Timestamp& outLoadedTime);
    
    // Delete old checkpoints to maintain size limit
    void cleanupOldCheckpoints();
    
    // Get list of available checkpoints
    std::vector<CheckpointInfo> getAvailableCheckpoints() const;
    
    // Get current checkpoint directory
    std::string getCheckpointDirectory() const { return checkpointDir_; }
    
    // Enable/disable checkpointing
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    // Enable/disable auto-save
    void setAutoSave(bool autoSave) { autoSave_ = autoSave; }
    bool isAutoSaveEnabled() const { return autoSave_; }
    
    // Get checkpoint statistics
    struct Statistics {
        size_t totalCheckpointsCreated;
        size_t totalBytesSaved;
        size_t totalBytesLoaded;
        SimulationStep nextCheckpointStep;
        double averageSaveTime;
        size_t failedSaves;
    };
    
    Statistics getStatistics() const;
    
    // Set description for next checkpoint
    void setNextCheckpointDescription(const std::string& description) {
        nextCheckpointDescription_ = description;
    }
    
    // Export checkpoint metadata to file
    bool exportMetadata(const std::string& metadataFile) const;
    
    // Import checkpoint metadata from file
    bool importMetadata(const std::string& metadataFile);
    
    // Backup checkpoint directory
    bool backupDirectory(const std::string& backupPath);
    
    // Restore from backup
    bool restoreFromBackup(const std::string& backupPath);
    
private:
    bool enabled_;
    bool autoSave_;
    bool compressionEnabled_;
    std::string checkpointDir_;
    SimulationStep saveInterval_;
    size_t maxCheckpoints_;
    std::string checkpointFormat_;
    
    mutable std::mutex mutex_;
    
    // Statistics
    mutable size_t totalCheckpointsCreated_;
    mutable size_t totalBytesSaved_;
    mutable size_t totalBytesLoaded_;
    mutable SimulationStep nextCheckpointStep_;
    mutable double totalSaveTime_;
    mutable size_t failedSaves_;
    
    // Checkpoint info cache
    mutable std::vector<CheckpointInfo> checkpointInfoCache_;
    mutable bool checkpointInfoCacheDirty_;
    
    std::string nextCheckpointDescription_;
    
    // Internal helper methods
    bool ensureDirectoryExists(const std::string& path) const;
    std::string generateCheckpointFilename(SimulationStep step, Timestamp timestamp) const;
    bool writeBinaryCheckpoint(const std::string& filename,
                               const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                               const std::vector<std::unique_ptr<Synapse>>& synapses);
    bool readBinaryCheckpoint(const std::string& filename,
                              std::vector<std::unique_ptr<NeuralRegion>>& regions,
                              std::vector<std::unique_ptr<Synapse>>& synapses);
    bool writeTextCheckpoint(const std::string& filename,
                             const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                             const std::vector<std::unique_ptr<Synapse>>& synapses);
    bool readTextCheckpoint(const std::string& filename,
                            std::vector<std::unique_ptr<NeuralRegion>>& regions,
                            std::vector<std::unique_ptr<Synapse>>& synapses);
    bool compressFile(const std::string& inputFile, const std::string& outputFile) const;
    bool decompressFile(const std::string& inputFile, const std::string& outputFile) const;
    CheckpointInfo createCheckpointInfo(SimulationStep step, Timestamp timestamp,
                                        size_t neuronCount, size_t synapseCount,
                                        float simulationTime) const;
    void updateCheckpointInfoCache() const;
    void updateStatistics(SimulationStep step, Timestamp timestamp,
                         size_t neuronCount, size_t synapseCount, float simulationTime,
                         bool success, size_t bytesTransferred) const;
    
    // Serialization helper methods
    void saveRegion(std::ofstream& file, const NeuralRegion* region) const;
    std::unique_ptr<NeuralRegion> loadRegion(std::ifstream& file) const;
    void saveSynapse(std::ofstream& file, const Synapse* synapse) const;
    std::unique_ptr<Synapse> loadSynapse(std::ifstream& file) const;
};

} // namespace nlm