#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/BrainSystem.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

class Brain;

// MemorySystem: Manages all memory subsystems
// Integrates working, episodic, and associative memory
//
// Key mechanisms:
// - Provides unified interface to memory systems
// - Manages memory consolidation and replay
// - Coordinates memory between systems
// - Handles memory-based learning and recall

class MemorySystem {
public:
    MemorySystem();
    ~MemorySystem();
    
    // Initialize memory system
    void initialize(BrainSystem* brainSystem);
    
    // Store information in working memory
    void storeToWorkingMemory(NeuronId neuron, float activation);
    
    // Retrieve working memory content
    std::vector<float> retrieveWorkingMemory() const;
    std::vector<NeuronId> getMemoryNeurons() const { return memoryNeurons_; }
    
    // Episodic memory management
    void storeEpisode(const EpisodicMemoryItem& episode);
    void replayEpisodes(size_t count);
    const std::vector<EpisodicMemoryItem>& getEpisodes() const { return episodes_; }
    
    // Associative memory management
    void learnAssociation(const std::vector<float>& pattern1,
                         const std::vector<float>& pattern2,
                         float strength);
    std::vector<float> recallAssociation(const std::vector<float>& pattern) const;
    
    // Memory update and maintenance
    void updateWorkingMemory(TimestepDuration dt);
    void updateEpisodicMemory();
    void consolidateMemory(float threshold);
    void clearWorkingMemory();
    void clearEpisodicMemory();
    
    // Memory statistics
    size_t getActiveWorkingMemoryTraces() const;
    size_t getEpisodeCount() const { return episodes_.size(); }
    size_t getWorkingMemoryCapacity() const { return workingMemoryCapacity_; }
    void setWorkingMemoryCapacity(size_t capacity) { workingMemoryCapacity_ = capacity; }
    
    // Get memory content
    const std::vector<NeuronId>& getMemoryNeurons() { return memoryNeurons_; }
    const std::vector<float>& getMemoryActivations() const { return memoryActivations_; }
    const std::vector<SimulationStep>& getMemoryTimestamps() const { return memoryTimestamps_; }
    
    // Set brain reference for dependency injection
    void setBrain(BrainSystem* brainSystem) { brainSystem_ = brainSystem; }
    
private:
    BrainSystem* brainSystem_;
    
    // Working memory
    size_t workingMemoryCapacity_;
    float workingMemoryDecayRate_;
    
    // Working memory content
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    // Active memory traces
    std::vector<size_t> activeTraces_;
    
    // Recurrent connections for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    
    // Winner neurons (for competition)
    std::vector<NeuronId> winners_;
    
    // Episodic memory
    std::vector<EpisodicMemoryItem> episodes_;
    SimulationStep stepsSinceLastEpisode_;
    
    // Associative memory
    std::unordered_map<std::string, float> associations_;
    float associationDecayRate_;
    
    // For episodic memory replay
    void createEpisodeFromCurrentState();
    
    // Update working memory - maintain traces and decay
    void decayWeakTraces();
    
    // Create recurrent connections for maintenance
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);
};

} // namespace nlm