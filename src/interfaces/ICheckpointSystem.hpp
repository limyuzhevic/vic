// ICheckpointSystem.hpp
// Abstract interface for checkpoint systems
// Manages state checkpointing, saving, and restoration
// Provides versioning and backup capabilities

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <map>
#include <chrono>
#include <filesystem>

namespace nlm {

// Forward declarations
class Brain;

// Checkpoint metadata
struct CheckpointMetadata {
    SimulationStep step;
    Timestamp timestamp;
    std::string description;
    std::string version;
    std::map<std::string, std::string> metadata;
    uint64_t checkpointId;
    float compressionRatio;
    bool isValid;
    
    CheckpointMetadata() : step(0), timestamp(0), version("1.0.0"), checkpointId(0),
                           compressionRatio(1.0f), isValid(true) {}
};

// Checkpoint information for quick lookup
struct CheckpointInfo {
    uint64_t id;
    SimulationStep step;
    Timestamp timestamp;
    std::string description;
    std::string version;
    float size;  // In MB
    std::chrono::system_clock::time_point creationTime;
    
    CheckpointInfo() : id(0), step(0), timestamp(0), size(0) {}
};

class ICheckpointSystem {
public:
    virtual ~ICheckpointSystem() = default;

    // Initialize checkpoint system with brain reference
    virtual bool initialize(Brain* brain) = 0;

    // Create a checkpoint
    virtual uint64_t createCheckpoint(const std::string& description = "Auto checkpoint") = 0;

    // Create checkpoint with metadata
    virtual uint64_t createCheckpoint(const std::string& description,
                                       const std::map<std::string, std::string>& metadata) = 0;

    // Restore checkpoint by ID
    virtual bool restoreCheckpoint(uint64_t checkpointId) = 0;

    // Restore checkpoint by step
    virtual bool restoreCheckpoint(SimulationStep step) = 0;

    // Get checkpoint metadata
    virtual CheckpointMetadata getCheckpointMetadata(uint64_t checkpointId) const = 0;

    // Get checkpoint info
    virtual CheckpointInfo getCheckpointInfo(uint64_t checkpointId) const = 0;

    // Get all checkpoints
    virtual std::vector<CheckpointInfo> getAllCheckpoints() const = 0;

    // Delete checkpoint
    virtual bool deleteCheckpoint(uint64_t checkpointId) = 0;

    // Clear all checkpoints
    virtual void clear() = 0;

    // Update checkpoint system
    virtual void update(TimestepDuration dt) = 0;

    // Get checkpoint statistics
    virtual std::string getCheckpointStats() const = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "ICheckpointSystem"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Callback pattern for checkpoint events
    using CheckpointCreatedCallback = std::function<void(uint64_t checkpointId)>;
    virtual void setCheckpointCreatedCallback(const CheckpointCreatedCallback& callback) = 0;

    using CheckpointRestoredCallback = std::function<void(uint64_t checkpointId)>;
    virtual void setCheckpointRestoredCallback(const CheckpointRestoredCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

    // Checkpoint system parameters
    virtual void setCheckpointDirectory(const std::string& directory) = 0;
    virtual std::string getCheckpointDirectory() const = 0;
    virtual void setCheckpointInterval(SimulationStep interval) = 0;
    virtual SimulationStep getCheckpointInterval() const = 0;
    virtual void setMaxCheckpoints(size_t max) = 0;
    virtual size_t getMaxCheckpoints() const = 0;
    virtual void setAutoCheckpoint(bool enabled) = 0;
    virtual bool isAutoCheckpointEnabled() const = 0;
    virtual void setCompressionLevel(int level) = 0;
    virtual int getCompressionLevel() const = 0;

    // Performance metrics
    virtual float getCheckpointCreationRate() const = 0;  // Checkpoints per second
    virtual float getCheckpointRestoreRate() const = 0;
    virtual uint64_t getTotalCheckpointsCreated() const = 0;
    virtual uint64_t getTotalCheckpointsRestored() const = 0;
    virtual uint64_t getTotalCheckpointStorage() const = 0;  // In MB

protected:
    ICheckpointSystem() = default;
};

} // namespace nlm
