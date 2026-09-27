#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include "../performance/CheckpointSystem.hpp"

namespace nlm {

/**
 * Persistent brain state metadata
 */
struct PersistentBrainStateInfo {
    uint32_t versionMajor;
    uint32_t versionMinor;
    uint32_t versionPatch;
    uint64_t timestamp;
    uint64_t neuronCount;
    uint64_t synapseCount;
    uint64_t memorySize;
    uint64_t simulationStep;
    double simulationTime;
    std::string checksum;
    std::string author;
    std::string notes;
    
    PersistentBrainStateInfo() 
        : versionMajor(1), versionMinor(0), versionPatch(0)
        , timestamp(0), neuronCount(0), synapseCount(0), memorySize(0)
        , simulationStep(0), simulationTime(0.0) {}
};

/**
 * Persistent brain state metadata for serialization
 */
struct PersistentBrainState {
    // Brain state
    uint64_t simulationStep;
    double simulationTime;
    double timestep;
    uint64_t randomSeed;
    
    // Developmental state
    DevelopmentalStage developmentalStage;
    float developmentalAge;
    
    // Neuromodulation state
    float dopamineLevel;
    float noveltyLevel;
    float curiosityLevel;
    float predictionErrorLevel;
    
    // Memory state
    uint32_t workingMemorySize;
    uint32_t episodicMemorySize;
    uint32_t associativeMemorySize;
    
    // Prediction system state
    float predictionConfidence;
    bool predictionActive;
    
    // Cognitive system state
    float attentionInhibition;
    float attentionExcitation;
    size_t planningDepth;
    size_t conceptCount;
    
    // Enhanced neuromodulation state
    float norepinephrineLevel;
    float acetylcholineLevel;
    float serotoninLevel;
    
    // Simulation parameters
    uint64_t replayInterval;
    uint64_t consolidationInterval;
    
    PersistentBrainState()
        : simulationStep(0), simulationTime(0.0), timestep(0.001)
        , randomSeed(42), developmentalStage(DevelopmentalStage::Initial)
        , developmentalAge(0.0)
        , dopamineLevel(0.0f), noveltyLevel(0.0f), curiosityLevel(0.0f)
        , predictionErrorLevel(0.0f)
        , workingMemorySize(0), episodicMemorySize(0), associativeMemorySize(0)
        , predictionConfidence(0.0f), predictionActive(false)
        , attentionInhibition(0.5f), attentionExcitation(1.5f)
        , planningDepth(5), conceptCount(0)
        , norepinephrineLevel(0.0f), acetylcholineLevel(0.0f), serotoninLevel(0.0f)
        , replayInterval(100), consolidationInterval(1000) {}
};

/**
 * Brain persistence manager
 * Handles serialization and deserialization of complete brain state
 */
class PersistenceManager {
public:
    PersistenceManager();
    ~PersistenceManager();
    
    // Disable copying
    PersistenceManager(const PersistenceManager&) = delete;
    PersistenceManager& operator=(const PersistenceManager&) = delete;
    
    /**
     * Initialize persistence manager
     */
    bool initialize(const std::string& baseDirectory = "./brains");
    
    /**
     * Save brain state to file
     * @param filename Base filename (can include path)
     * @param useBinary Whether to use binary format (true) or text format (false)
     * @param compression Compression level for binary format
     * @param incremental Whether to create incremental checkpoint
     * @return true if successful
     */
    bool saveBrainState(const std::string& filename,
                       bool useBinary = true,
                       CheckpointSystem::CompressionLevel compression = CheckpointSystem::CompressionLevel::Balanced,
                       bool incremental = true);
    
    /**
     * Load brain state from file
     * @param filename Input file path
     * @param useBinary Whether the file is in binary format
     * @return true if successful
     */
    bool loadBrainState(const std::string& filename, bool useBinary = true);
    
    /**
     * Check if persistent state is available
     * @param filename Path to check
     * @return true if file exists and is valid
     */
    bool isPersistentStateAvailable(const std::string& filename) const;
    
    /**
     * Get persistent state metadata
     * @param filename Path to analyze
     * @param useBinary Whether the file is in binary format
     * @return State information
     */
    PersistentBrainStateInfo getPersistentStateInfo(const std::string& filename,
                                                  bool useBinary = true) const;
    
    /**
     * Get current persistence directory
     */
    const std::string& getBaseDirectory() const { return baseDirectory_; }
    
    /**
     * Set persistence directory
     */
    void setBaseDirectory(const std::string& directory);
    
    /**
     * Create automatic backup of current brain state
     * @param prefix Optional prefix for backup
     * @return true if successful
     */
    bool createBackup(const std::string& prefix = "auto_backup");
    
    /**
     * Restore from automatic backup
     * @param backupName Name of backup to restore
     * @return true if successful
     */
    bool restoreFromBackup(const std::string& backupName);
    
    /**
     * List all brain state files in directory
     * @return Vector of file paths
     */
    std::vector<std::string> listBrainStates() const;
    
    /**
     * Delete brain state file
     * @param filename File to delete
     * @return true if successful
     */
    bool deleteBrainState(const std::string& filename);
    
    /**
     * Check if checkpoint manager is available
     * @return true if checkpoint manager is initialized
     */
    bool isCheckpointManagerAvailable() const { return checkpointManager_ != nullptr; }
    
    /**
     * Get checkpoint manager reference
     */
    CheckpointManager* getCheckpointManager() { return checkpointManager_.get(); }
    
    /**
     * Set checkpoint manager
     */
    void setCheckpointManager(std::unique_ptr<CheckpointManager> manager);
    
    /**
     * Extract brain state from CheckpointManager
     * @param manager CheckpointManager to extract from
     * @return Extracted state
     */
    PersistentBrainState extractBrainStateFromCheckpoint(CheckpointManager& manager);
    
    /**
     * Apply brain state to CheckpointManager
     * @param manager CheckpointManager to apply to
     * @param state State to apply
     * @return true if successful
     */
    bool applyBrainStateToCheckpoint(CheckpointManager& manager, const PersistentBrainState& state);
    
private:
    /**
     * Save brain state in binary format
     * @param filename Output file
     * @param state Brain state to save
     * @param compression Compression level
     * @return true if successful
     */
    bool saveBinary(const std::string& filename, const PersistentBrainState& state,
                   CheckpointSystem::CompressionLevel compression);
    
    /**
     * Load brain state from binary format
     * @param filename Input file
     * @param state Output state
     * @return true if successful
     */
    bool loadBinary(const std::string& filename, PersistentBrainState& state);
    
    /**
     * Save brain state in text format
     * @param filename Output file
     * @param state Brain state to save
     * @return true if successful
     */
    bool saveText(const std::string& filename, const PersistentBrainState& state);
    
    /**
     * Load brain state from text format
     * @param filename Input file
     * @param state Output state
     * @return true if successful
     */
    bool loadText(const std::string& filename, PersistentBrainState& state);
    
    /**
     * Validate persistent state
     * @param filename File to validate
     * @param useBinary Format
     * @return true if valid
     */
    bool validatePersistentState(const std::string& filename, bool useBinary) const;
    
    /**
     * Get file extension for format
     * @param useBinary Binary format flag
     * @return File extension
     */
    std::string getFileExtension(bool useBinary) const;
    
    std::string baseDirectory_;
    std::unique_ptr<CheckpointManager> checkpointManager_;
    std::chrono::system_clock::time_point lastOperationTime_;
    std::string lastError_;
};

} // namespace nlm