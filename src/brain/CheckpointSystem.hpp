#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Config;

// CheckpointSystem: State persistence and recovery
// Manages save/load operations and binary serialization
class CheckpointSystem {
public:
    CheckpointSystem(std::shared_ptr<Config> config);
    ~CheckpointSystem();
    
    // Initialize checkpoint system
    bool initialize();
    
    // Checkpoint management
    bool save(const std::string& filepath, const class Brain* brain) const;
    bool load(const std::string& filepath, class Brain* brain);
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
    // Checkpoint parameters
    void configure(const std::string& directory, size_t maxCheckpoints, 
                   size_t maxFileSize, bool compressionEnabled);
    
    // Statistics
    size_t getMaxCheckpoints() const { return maxCheckpoints_; }
    size_t getMaxFileSize() const { return maxFileSize_; }
    bool isCompressionEnabled() const { return compressionEnabled_; }
    size_t getCurrentCheckpointCount() const { return currentCheckpointCount_; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Checkpoint configuration
    std::string checkpointDirectory_;
    size_t maxCheckpoints_;
    size_t maxFileSize_;
    bool compressionEnabled_;
    size_t currentCheckpointCount_;
};

} // namespace nlm
