#pragma once

#include "Brain.hpp"
#include "BrainCore.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <string>

namespace nlm {

class BrainCheckpoint {
public:
    BrainCheckpoint();
    ~BrainCheckpoint();
    
    // Disable copying
    BrainCheckpoint(const BrainCheckpoint&) = delete;
    BrainCheckpoint& operator=(const BrainCheckpoint&) = delete;
    
    // Initialize with core brain
    bool initialize(BrainCore* core);
    
    // Save brain state to file
    bool save(const std::string& filepath, const std::string& format = "auto") const;
    
    // Load brain state from file
    bool load(const std::string& filepath, const std::string& format = "auto");
    
    // Checkpoint management
    void update(SimulationStep currentStep, Timestamp currentTime);
    
    // Get checkpoint statistics
    size_t getBytesWritten() const { return bytesWritten; }
    size_t getCheckpointCount() const { return checkpointCount; }
    
    // Configuration
    void setCheckpointDirectory(const std::string& dir) { checkpointDir = dir; }
    void setMaxCheckpoints(size_t max) { maxCheckpoints = max; }
    void setCompressionLevel(float level) { compressionLevel = level; }
    
private:
    // Core brain reference
    BrainCore* core = nullptr;
    
    // Checkpoint manager
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Statistics
    size_t bytesWritten;
    size_t checkpointCount;
    
    // Configuration
    std::string checkpointDir;
    size_t maxCheckpoints;
    float compressionLevel;
    
    // Helper methods
    bool saveNeurons(const std::string& filepath) const;
    bool saveSynapses(const std::string& filepath) const;
    bool saveDelayedSpikes(const std::string& filepath) const;
    bool savePhase5Data(const std::string& filepath) const;
    bool saveMemoryData(const std::string& filepath) const;
    
    bool loadNeurons(const std::string& filepath);
    bool loadSynapses(const std::string& filepath);
    bool loadDelayedSpikes(const std::string& filepath);
    bool loadPhase5Data(const std::string& filepath);
    bool loadMemoryData(const std::string& filepath);
    
    // Phase 5 helper functions (from original Brain.cpp)
    static void p5AppendU64(std::vector<uint8_t>& out, uint64_t v);
    static void p5AppendFloat(std::vector<uint8_t>& out, float v);
    static void p5AppendDouble(std::vector<uint8_t>& out, double v);
    template <typename T>
    static void p5AppendVec(std::vector<uint8_t>& out, const std::vector<T>& v);
    
    struct P5Reader {
        const uint8_t* p;
        const uint8_t* end;
        bool ok = true;
        bool u64(uint64_t& v);
        bool f32(float& v);
        bool f64(double& v);
        template <typename T>
        bool vec(std::vector<T>& v);
    };
    
    // Friend class
    friend class Brain;
};

} // namespace nlm
