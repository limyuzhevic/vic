// Checkpoint System - Brain state persistence and recovery
// Extracted from Brain::save() and Brain::load() methods
#pragma once

#include "brain/Brain.hpp"
#include "core/BrainExceptions.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace nlm {

class CheckpointManager {
public:
    enum class CompressionLevel {
        None,
        Fast,
        Balanced,
        Maximum
    };
    
    enum class CheckpointFormat {
        Binary,
        Text,
        JSON,
        Compressed
    };
    
    struct CheckpointInfo {
        std::string path;
        std::chrono::system_clock::time_point timestamp;
        SimulationStep step;
        Timestamp time;
        size_t neuronCount;
        size_t synapseCount;
        size_t regionCount;
        size_t version;
        bool valid;
        double compressionRatio;
        
        CheckpointInfo(const std::string& p, SimulationStep s, Timestamp t)
            : path(p), timestamp(std::chrono::system_clock::now()), step(s),
              time(t), neuronCount(0), synapseCount(0), regionCount(0),
              version(1), valid(true), compressionRatio(1.0) {}
    };
    
    // Core checkpoint functionality
    CheckpointManager(std::shared_ptr<Brain> brain);
    ~CheckpointManager();
    
    // Configuration
    void configure(const std::string& directory, size_t maxCheckpoints,
                   size_t keepTimeMinutes, bool autoSave);
    
    // Checkpoint operations
    bool createCheckpoint(SimulationStep step, Timestamp time);
    bool save(const std::string& filepath);
    bool load(const std::string& filepath);
    
    // Checkpoint management
    bool cleanupOldCheckpoints();
    bool validateCheckpoint(const std::string& filepath);
    CheckpointInfo getCheckpointInfo(const std::string& filepath);
    
    // State restoration and recovery
    bool restoreFromCheckpoint(SimulationStep targetStep);
    bool restoreFromClosestCheckpoint();
    
    // Checkpoint listing and info
    std::vector<CheckpointInfo> listCheckpoints(const std::string& directory = "");
    bool removeCheckpoint(const std::string& filepath);
    
    // Recovery operations
    bool rollbackToCheckpoint(SimulationStep step);
    bool checkpointForRecovery();
    
    // Configuration getters
    std::string getCheckpointDirectory() const { return checkpointDirectory_; }
    size_t getMaxCheckpoints() const { return maxCheckpoints_; }
    size_t getKeepTimeMinutes() const { return keepTimeMinutes_; }
    bool isAutoSaveEnabled() const { return autoSaveEnabled_; }
    
    // Statistics
    size_t getTotalCheckpointsCreated() const { return totalCheckpointsCreated_; }
    size_t getSuccessfulLoads() const { return successfulLoads_; }
    size_t getSuccessfulSaves() const { return successfulSaves_; }
    double getAverageCheckpointSize() const { return avgCheckpointSize_; }
    
private:
    std::shared_ptr<Brain> brain_;
    
    // Configuration
    std::string checkpointDirectory_;
    size_t maxCheckpoints_;
    size_t keepTimeMinutes_;
    bool autoSaveEnabled_;
    
    // State
    CheckpointFormat defaultFormat_;
    CompressionLevel defaultCompression_;
    
    // Statistics
    size_t totalCheckpointsCreated_;
    size_t successfulLoads_;
    size_t successfulSaves_;
    double avgCheckpointSize_;
    
    // Internal state
    std::vector<CheckpointInfo> recentCheckpoints_;
    
    // Helper methods
    bool ensureDirectoryExists(const std::string& path);
    std::string generateCheckpointFilename(SimulationStep step, Timestamp time);
    bool writeBinaryCheckpoint(const std::string& filepath);
    bool readBinaryCheckpoint(const std::string& filepath);
    bool writeTextCheckpoint(const std::string& filepath);
    bool writeJSONCheckpoint(const std::string& filepath);
    bool compressFile(const std::string& inputPath, const std::string& outputPath);
    size_t calculateFileSize(const std::string& filepath);
    void updateStatistics();
    bool validateCheckpointData() const;
    void cleanupCorruptedCheckpoints();
};

// Checkpoint writer class
class CheckpointWriter {
public:
    CheckpointWriter(const std::string& filepath, CompressionLevel level);
    ~CheckpointWriter();
    
    bool create(const std::string& filepath, CompressionLevel level);
    bool writeMetadata(size_t neuronCount, size_t synapseCount, size_t regionCount,
                      SimulationStep step, Timestamp time);
    bool writeNeurons(const NeuronCheckpointData& neuronData);
    bool writeSynapses(const SynapseCheckpointData& synapseData);
    bool finalize();
    
    std::string getPath() const { return filepath_; }
    size_t getBytesWritten() const { return bytesWritten_; }
    double getCompressionRatio() const { return compressionRatio_; }
    
private:
    std::string filepath_;
    CompressionLevel compressionLevel_;
    std::ofstream outputStream_;
    size_t bytesWritten_;
    double compressionRatio_;
    bool finalized_;
    
    bool openStream();
    bool compressStream();
};

// Checkpoint reader class
class CheckpointReader {
public:
    CheckpointReader(const std::string& filepath);
    ~CheckpointReader();
    
    bool open(const std::string& filepath);
    bool validate() const;
    std::string getError() const { return errorMessage_; }
    
    bool readMetadata(size_t& neuronCount, size_t& synapseCount, size_t& regionCount,
                     SimulationStep& step, Timestamp& time, size_t& version);
    bool readNeurons(NeuronCheckpointData& neuronData);
    bool readSynapses(SynapseCheckpointData& synapseData);
    bool close();
    
    std::string getPath() const { return filepath_; }
    bool isOpen() const { return isOpen_; }
    
private:
    std::string filepath_;
    std::ifstream inputStream_;
    std::string errorMessage_;
    bool isOpen_;
    CheckpointFormat format_;
    
    bool detectFormat() const;
    bool readHeader();
    bool validateFormat() const;
};

} // namespace nlm
