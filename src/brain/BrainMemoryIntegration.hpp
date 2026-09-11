#pragma once

// BrainMemoryIntegration.hpp - Working memory and episodic memory integration
// Contains memory system integration functionality

#include "BrainCore.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include <memory>

namespace nlm {

// Forward declarations
class BrainCore;

// Brain Memory Integration Implementation
class BrainMemoryIntegration {
public:
    struct Impl;
    Impl* pImpl;
    
    // Create brain memory integration with reference to core brain
    explicit BrainMemoryIntegration(BrainCore* core);
    
    ~BrainMemoryIntegration();
    
    // Disable copying, enable moving
    BrainMemoryIntegration(const BrainMemoryIntegration&) = delete;
    BrainMemoryIntegration& operator=(const BrainMemoryIntegration&) = delete;
    BrainMemoryIntegration(BrainMemoryIntegration&&) noexcept;
    BrainMemoryIntegration& operator=(BrainMemoryIntegration&&) noexcept;
    
    // Initialize memory systems
    bool initialize();
    
    // Update memory systems
    void update(TimestepDuration dt, SimulationStep currentStep, Timestamp currentTime);
    
    // Store neuron activity in working memory
    void storeToNeuron(NeuronId neuronId, float activationLevel);
    
    // Capture current brain state as an episodic memory
    void captureCurrentStateAsEpisode(SimulationStep currentStep, Timestamp currentTime);
    
    // Replay episodes for memory consolidation
    void replayEpisode(const class EpisodicMemoryItem* episode);
    
    // Consolidate episodic memory
    void consolidateMemory(float threshold);
    
    // Get episodes for replay based on importance
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count);
    
    // Clear all memory systems
    void clear();
    
    // Get statistics
    size_t getActiveWorkingMemoryTraces() const;
    size_t getEpisodeCount() const;
    
private:
    // Internal helper methods
    void updateWorkingMemory(TimestepDuration dt);
    void updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime);
    void updateAssociativeMemory();
    void processMemoryReplay(SimulationStep currentStep);
    void processMemoryConsolidation(SimulationStep currentStep);
    
    // Utility methods
    void calculateEpisodicStats() const;
    
    struct Impl {
        BrainCore* brainCore;
        
        // Memory system references
        NeuralWorkingMemory* workingMemory;
        NeuralEpisodicMemory* episodicMemory;
        NeuralAssociativeMemory* associativeMemory;
        
        // Memory state
        std::vector<NeuronId> recentActiveNeurons;
        std::vector<float> recentActivationLevels;
        
        Impl(BrainCore* core);
    };
};

} // namespace nlm

