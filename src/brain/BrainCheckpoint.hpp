#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

class Brain;

class BrainCheckpoint {
public:
    // Forward declaration for friendship
    friend class Brain;
    
    BrainCheckpoint() = default;
    ~BrainCheckpoint() = default;
    
    // Checkpoint management
    bool configure(class Brain* brain, const std::string& directory, 
                   size_t maxCheckpoints, size_t checkpointInterval,
                   bool autoSave);
    bool update(SimulationStep currentStep, Timestamp currentTime);
    bool save(class Brain* brain, const std::string& filepath) const;
    bool load(class Brain* brain, const std::string& filepath);
    
    // Checkpoint statistics
    size_t getMaxCheckpoints() const;
    size_t getCheckpointInterval() const;
    size_t getActiveCheckpointCount() const;
    std::string getCheckpointDirectory() const;
    
    // Status and management
    void listCheckpoints(std::vector<std::string>& checkpoints) const;
    bool removeCheckpoint(const std::string& filepath);
    void clearOldCheckpoints();
    
    // Status
    bool isAutoSaveEnabled() const;
    std::string getStatus() const;
    
private:
    // Checkpoint manager
    class CheckpointManager* checkpointManager;
    
    // Configuration
    std::string checkpointDirectory;
    size_t maxCheckpoints;
    size_t checkpointInterval;
    bool autoSaveEnabled;
    
    // Internal state
    class Brain* brainRef;
    size_t checkpointsCreated;
    size_t lastCheckpointStep;
    std::vector<std::string> recentCheckpoints;
};

} // namespace nlm