#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../memory/MemoryReplaySystem.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declarations
class Brain;
class NeuralEpisodicMemory;
struct SpikeEvent;

// Enhanced visualization interface for debugging and analysis
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization
    bool initialize();
    
    // Update visualization data
    void update();
    
    // Render current state
    void render();
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Brain visualization methods
    void visualizeNetwork(const Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const Brain& brain);
    
    // Enhanced visualization methods for memory replay
    void visualizeMemoryReplay(const MemoryReplaySystem& replaySystem);
    void visualizeEpisodicMemory(const NeuralEpisodicMemory& episodicMemory);
    void visualizeReplayStatistics(const ReplayStats& stats);
    void visualizeBrainStatesForReplay(const Brain& brain);
    void visualizeReplayTimeline(const MemoryReplaySystem& replaySystem);
    void visualizeReplayQuality(const ReplayStats& stats);
    
    // Memory-specific visualization
    void visualizeMemoryConsolidation(const NeuralEpisodicMemory& episodicMemory);
    void visualizeMemoryRetrieval(const NeuralEpisodicMemory& episodicMemory);
    void visualizeMemoryAssociations(const class NeuralAssociativeMemory& associativeMemory);
    
    // Integration methods
    void setReplaySystem(MemoryReplaySystem* replaySystem) { replaySystem_ = replaySystem; }
    void setEpisodicMemory(NeuralEpisodicMemory* episodicMemory) { episodicMemory_ = episodicMemory; }
    
    // Configuration
    void enableReplayVisualization(bool enable) { replayVisualizationEnabled_ = enable; }
    bool isReplayVisualizationEnabled() const { return replayVisualizationEnabled_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Integration pointers
    MemoryReplaySystem* replaySystem_;
    NeuralEpisodicMemory* episodicMemory_;
    bool replayVisualizationEnabled_;
};

} // namespace nlm
